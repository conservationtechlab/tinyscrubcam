/* Copyright 2021, Cypress Semiconductor Corporation or a subsidiary of 
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software"),
 * is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

#include "platform_capsense.h"

#include "platform.h"
#include "platform_config.h"
#include "platform_init.h"
#include "platform_isr.h"
#include "wwd_platform_common.h"
#include "wwd_rtos_isr.h"
#include "wiced_defaults.h"
#include "wiced_rtos.h"
#include "wiced_platform.h"
#include "wwd_assert.h"
#include "string.h"     // for memeset

#include "cyfitter_sysint_cfg.h"

#include "platform_assert.h"

#include "CapSense.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#if !defined( PLATFORM_CAPSENSE_THREAD_PRIORITY )
#define PLATFORM_CAPSENSE_THREAD_PRIORITY               (3)
#endif /* !defined( PLATFORM_CAPSENSE_THREAD_PRIORITY) */

#if !defined( PLATFORM_CAPSENSE_EVENT_QUEUE_DEPTH )
#define PLATFORM_CAPSENSE_EVENT_QUEUE_DEPTH             (5)
#endif /* !defined( PLATFORM_CAPSENSE_EVENT_QUEUE_DEPTH ) */

#if !defined( PLATFORM_CAPSENSE_THREAD_STACK_SIZE )
#define PLATFORM_CAPSENSE_THREAD_STACK_SIZE             (1024*4)
#endif /* !defined( PLATFORM_CAPSENSE_THREAD_STACK_SIZE ) */

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum platform_capsense_event_id
{
    PLATFORM_CAPSENSE_QUIT_THREAD_EVENT_ID      = 0,
    PLATFORM_CAPSENSE_WRITE_WIDGET_EVENT_ID,
} platform_capsense_event_id_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct platform_capsense_info
{
    wiced_thread_t                              event_thread;
    wiced_mutex_t                               mutex;
    wiced_queue_t                               event_queue;
    wiced_semaphore_t                           synchronous_push_to_queue_semaphore;
    platform_capsense_widget_t const*           widget[CapSense_TOTAL_WIDGETS];
    platform_capsense_event_callback_t*         callback;
    platform_capsense_simple_driver_config_t    config;
} platform_capsense_info_t;

typedef struct platform_capsense_event_message
{
    platform_capsense_event_id_t                event_id;
    platform_capsense_widget_t const*           widget;
    uint8_t                                     is_synchronous  : 1;
    uint8_t                                     is_enabled      : 1;
} platform_capsense_event_message_t;

typedef platform_result_t (platform_capnsense_query_fn)( const platform_capsense_widget_t* widget, void* arg );

/******************************************************
 *                    Structures
 ******************************************************/

/* Reserved for future proofing. */
struct platform_capsense_driver
{
    uint8_t         reserved;
};

struct platform_capsense_widget_driver
{
    void (*data_init)(const platform_capsense_widget_t*);
    void (*event_handler)(const platform_capsense_widget_t*);
};

/******************************************************
 *               Function Declarations
 ******************************************************/

static wiced_bool_t         platform_capsense_is_any_widget_enabled( void );
static platform_result_t    platform_capsense_doit( platform_capsense_info_t* info, wiced_bool_t* at_least_one_active_widget_out );
static void                 platform_capsense_event_thread_main( wiced_thread_arg_t arg );
static platform_result_t    platform_capsense_synchronous_push_to_queue( platform_capsense_info_t* info, platform_capsense_event_message_t* message );

/* Widget state query functions. */
static platform_result_t    platform_capsense_query_is_active( const platform_capsense_widget_t* widget, void* arg );
static platform_result_t    platform_capsense_query_centroid_position( const platform_capsense_widget_t* widget, void* arg );

/* Widget initialization functions. */
static void                 platform_capsense_widget_data_init( const platform_capsense_widget_t* widget );
static void                 platform_capsense_linear_slider_data_init( const platform_capsense_widget_t* widget );

/* Widget event handlers. */
static void                 platform_capsense_button_event_handler( const platform_capsense_widget_t* widget );
static void                 platform_capsense_linear_slider_event_handler( const platform_capsense_widget_t* widget );

/******************************************************
 *               Variables Definitions
 ******************************************************/

/**
 * Statics.
 */

static platform_capsense_info_t         platform_capsense_info;
static cy_israddress                    ss_irq_handler;

/**
 * Externs.
 */

const platform_capsense_driver_t platform_capsense_simple_driver;

const platform_capsense_widget_driver_t platform_capsense_button_driver =
{
    .data_init          = platform_capsense_widget_data_init,
    .event_handler      = platform_capsense_button_event_handler,
};

const platform_capsense_widget_driver_t platform_capsense_linear_slider_driver =
{
    .data_init          = platform_capsense_linear_slider_data_init,
    .event_handler      = platform_capsense_linear_slider_event_handler,
};

/******************************************************
 *             Function definitions
 ******************************************************/

void CapSense_SsIsrInitialize(cy_israddress address)
{
    /* Disable interrupt */
    #if defined(CapSense_ISR__INTC_ASSIGNED)
        NVIC_DisableIRQ(CapSense_ISR_cfg.intrSrc);
    #endif

    /* Configure interrupt with priority and vector */
    #if defined(CapSense_ISR__INTC_ASSIGNED)
        ss_irq_handler = address;
    #endif

    /* Enable interrupt */
    #if defined(CapSense_ISR__INTC_ASSIGNED)
        NVIC_EnableIRQ(CapSense_ISR_cfg.intrSrc);
    #endif
}

static inline wiced_bool_t platform_capsense_is_widget_enabled( uint32_t widget_id )
{
    platform_capsense_info_t*   info    = &platform_capsense_info;
    return info->widget[ widget_id ] != NULL ? WICED_TRUE : WICED_FALSE;
}

static void platform_capsense_widget_data_init( const platform_capsense_widget_t* widget )
{
    memset( widget->data, 0, widget->data_sizeof );
}

static void platform_capsense_linear_slider_data_init( const platform_capsense_widget_t* widget )
{
    platform_capsense_linear_slider_data_t* data = (platform_capsense_linear_slider_data_t*)widget->data;

    platform_capsense_widget_data_init( widget );
    data->last_centroid_position = CapSense_SLIDER_NO_TOUCH;
}

static void platform_capsense_button_event_handler( const platform_capsense_widget_t* widget )
{
    platform_capsense_info_t*               info        = &platform_capsense_info;
    platform_capsense_button_data_t*        data        = (platform_capsense_button_data_t*)widget->data;
    uint32_t                                is_active;
    platform_track_button_event_data_t      event_data;

    is_active = CapSense_IsWidgetActive( widget->id ) != 0uL ? WICED_TRUE : WICED_FALSE;

    if ( data->base.last_is_active_status == is_active )
    {
        /* No change. */
        return;
    }

    memset( &event_data, 0, sizeof event_data );
    event_data.base.is_active = is_active;

    /* Dispatch. */
    (*info->callback)( widget, PLATFORM_WIDGET_TRACK_BUTTON_EVENT_ID, &event_data );

    data->base.last_is_active_status = is_active;
}

static void platform_capsense_linear_slider_event_handler( const platform_capsense_widget_t* widget )
{
    platform_capsense_info_t*                           info                = &platform_capsense_info;
    platform_capsense_linear_slider_data_t*             data                = (platform_capsense_linear_slider_data_t*)widget->data;
    platform_capsense_linear_slider_config_t    const* config               = widget->config != NULL ? (platform_capsense_linear_slider_config_t*)widget->config : NULL;
    uint32_t                                            position;
    uint32_t                                            position_delta;
    uint32_t                                    const   position_threshold  = config != NULL ? config->position_notification_threshold : 0;
    wiced_bool_t                                        is_active;
    platform_track_linear_slider_event_data_t           event_data;

    position        = CapSense_GetCentroidPos( widget->id );
    is_active       = position != CapSense_SLIDER_NO_TOUCH ? WICED_TRUE : WICED_FALSE;
    position_delta  = data->last_centroid_position > position ? (data->last_centroid_position - position) : (position - data->last_centroid_position);

    if ( (data->base.last_is_active_status == is_active) && (position_delta < position_threshold) )
    {
        /* No change. */
        return;
    }

    memset( &event_data, 0, sizeof event_data );
    event_data.base.is_active   = is_active;
    event_data.position         = is_active == WICED_TRUE ? position : data->last_centroid_position;

    /* Dispatch. */
    (*info->callback)( widget, PLATFORM_WIDGET_TRACK_LINEAR_SLIDER_EVENT_ID, &event_data );

    data->base.last_is_active_status    = is_active;
    data->last_centroid_position        = position;
}

platform_result_t platform_capsense_init( const platform_capsense_driver_t* driver, const void* void_config )
{
    wiced_result_t                  wiced_result;
    platform_capsense_info_t*       info            = &platform_capsense_info;
    cy_status                       status;

    if ( driver != &platform_capsense_simple_driver )
    {
        return PLATFORM_UNSUPPORTED;
    }

    wiced_result = wiced_rtos_init_mutex( &info->mutex );
    if ( wiced_result != WICED_SUCCESS ) goto ERROR1;

    wiced_result = wiced_rtos_init_semaphore( &info->synchronous_push_to_queue_semaphore );
    if ( wiced_result != WICED_SUCCESS ) goto ERROR2;

    wiced_result = wiced_rtos_init_queue( &info->event_queue, "CapSense", sizeof(platform_capsense_event_message_t), PLATFORM_CAPSENSE_EVENT_QUEUE_DEPTH );
    if ( wiced_result != WICED_SUCCESS ) goto ERROR3;

    wiced_result = wiced_rtos_create_thread( &info->event_thread, PLATFORM_CAPSENSE_THREAD_PRIORITY, "CapSense", platform_capsense_event_thread_main, PLATFORM_CAPSENSE_THREAD_STACK_SIZE, info );
    if ( wiced_result != WICED_SUCCESS ) goto ERROR4;

    /* Start CapSense. */
    status = CapSense_Start();
    if ( status != CY_RET_SUCCESS ) goto ERROR5;

    /* Set configuration. */
    if ( void_config == NULL )
    {
        info->config.wait_for_activity_scan_period_in_millisecond = PLATFORM_CAPSENSE_DEFAULT_TIMEOUT_MS;
        info->config.active_scan_period_in_milliseconds  = PLATFORM_CAPSENSE_DEFAULT_ACTIVE_WIDGET_TIMEOUT_MS;
    }
    else
    {
        const platform_capsense_simple_driver_config_t* config = (const platform_capsense_simple_driver_config_t*)void_config;
        info->config = *config;
    }

    return PLATFORM_SUCCESS;

ERROR5:
    wiced_rtos_delete_thread( &info->event_thread );
ERROR4:
    wiced_rtos_deinit_queue( &info->event_queue );
ERROR3:
    wiced_rtos_deinit_semaphore( &info->synchronous_push_to_queue_semaphore );
ERROR2:
    wiced_rtos_deinit_mutex( &info->mutex );
ERROR1:

    return PLATFORM_ERROR;
}

platform_result_t platform_capsense_deinit( void )
{
    wiced_result_t                      wiced_result;
    platform_capsense_info_t*           info            = &platform_capsense_info;
    platform_capsense_event_message_t   message         = { .event_id = PLATFORM_CAPSENSE_QUIT_THREAD_EVENT_ID, };

    wiced_result = wiced_rtos_push_to_queue( &info->event_queue, (void*)&message, WICED_WAIT_FOREVER );
    if ( wiced_result != WICED_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    wiced_rtos_thread_join( &info->event_thread );

    wiced_rtos_deinit_mutex( &info->mutex );
    wiced_rtos_deinit_queue( &info->event_queue );
    wiced_rtos_delete_thread( &info->event_thread );

    memset( info, 0, sizeof *info );

    return PLATFORM_SUCCESS;
}

static wiced_bool_t platform_capsense_is_any_widget_enabled( void )
{
    uint32_t    idx;

    for (idx = 0u; idx < CapSense_WDGT_STATUS_WORDS; idx++)
    {
        if ( CapSense_dsRam.wdgtEnable[idx] != 0uL )
        {
            return WICED_TRUE;
        }
    }
    return WICED_FALSE;
}

/* This is a very simple CapSense pipeline loop.  In the future we might
 * want to handle widget process, scan more robustly where is we are processing
 * the last scanned widget.
 */
static platform_result_t platform_capsense_doit( platform_capsense_info_t* info, wiced_bool_t* at_least_one_active_widget_out )
{
    platform_result_t   result                      = PLATFORM_SUCCESS;
    wiced_mutex_t*      mutex                       = &info->mutex;
    uint32_t            widget_id;
    wiced_bool_t        at_least_one_active_widget  = WICED_FALSE;

    if ( CapSense_IsBusy() != CapSense_NOT_BUSY )
    {
        return PLATFORM_SUCCESS;
    }

    wiced_rtos_lock_mutex( mutex );

    CapSense_ProcessAllWidgets();

    /* Update internal and report. */
    for (widget_id = 0u; widget_id < CapSense_TOTAL_WIDGETS; widget_id++)
    {
        platform_capsense_widget_t          const*  widget                      = info->widget[widget_id];
        platform_capsense_widget_driver_t   const*  driver                      = widget->driver;
        wiced_bool_t                                curr_is_active_status;

        if ( platform_capsense_is_widget_enabled( widget_id ) != WICED_TRUE )
        {
            /* Widget disabled; try next. */
            continue;
        }

        curr_is_active_status = CapSense_IsWidgetActive( widget_id ) != 0uL ? WICED_TRUE : WICED_FALSE;

        wiced_assert( "widget event handler is NULL", driver->event_handler != NULL );

        if ( info->callback != NULL )
        {
            (*driver->event_handler)( widget );
        }

        if ( curr_is_active_status == WICED_TRUE && at_least_one_active_widget != WICED_TRUE)
        {
            at_least_one_active_widget = WICED_TRUE;
        }
    }

    /* Scan all widgets. */
    CapSense_ScanAllWidgets();

    wiced_rtos_unlock_mutex( mutex );

    /* Copy-out. */
    *at_least_one_active_widget_out = at_least_one_active_widget;

    return result;
}

static void platform_capsense_event_thread_main( wiced_thread_arg_t arg )
{
    wiced_result_t                      result;
    platform_capsense_info_t*           info                        = &platform_capsense_info;
    platform_capsense_event_message_t   current_event;
    wiced_bool_t                        at_least_one_active_widget  = WICED_FALSE;
    uint32_t                            timeout                     = WICED_NEVER_TIMEOUT;
    uint32_t                            new_timeout;

    UNUSED_PARAMETER( arg );

    while(1)
    {
        result = wiced_rtos_pop_from_queue( &info->event_queue, &current_event, timeout );

        if ( result == WICED_SUCCESS )
        {
            platform_capsense_event_id_t event_id = current_event.event_id;

            if ( event_id == PLATFORM_CAPSENSE_QUIT_THREAD_EVENT_ID )
            {
                break;
            }
            if ( event_id == PLATFORM_CAPSENSE_WRITE_WIDGET_EVENT_ID )
            {
                platform_capsense_widget_t const*   widget      = current_event.widget;
                uint8_t                             is_enabled  = current_event.is_enabled;

                if ( is_enabled != 0u )
                {
                    info->widget[ widget->id ] = widget;
                    if ( widget->driver->data_init )
                    {
                        (*widget->driver->data_init)( widget );
                    }
                }
                else
                {
                    info->widget[ widget->id ] = NULL;
                }
            }
            if ( current_event.is_synchronous != 0 )
            {
                wiced_rtos_set_semaphore( &info->synchronous_push_to_queue_semaphore );
            }
        }
        else if ( result == WICED_TIMEOUT )
        {
            platform_capsense_doit( info, &at_least_one_active_widget );
        }

        /* Adjust timeout. */
        if ( platform_capsense_is_any_widget_enabled() == WICED_TRUE )
        {
            /* There is at least one enabled widget. */
            new_timeout = at_least_one_active_widget == WICED_TRUE ? info->config.active_scan_period_in_milliseconds : info->config.wait_for_activity_scan_period_in_millisecond;
        }
        else
        {
            new_timeout = WICED_NEVER_TIMEOUT;
        }

        if ( new_timeout != timeout )
        {
            timeout = new_timeout;
        }
    }
}

static platform_result_t platform_capsense_synchronous_push_to_queue( platform_capsense_info_t* info, platform_capsense_event_message_t* message )
{
    platform_result_t           result;
    wiced_result_t              wiced_result    = WICED_SUCCESS;

    /* Mark this message as synchronous. */
    message->is_synchronous = 1;

    /* Push the message asynchronously. */
    wiced_result = wiced_rtos_push_to_queue( &info->event_queue, (void*)message, WICED_WAIT_FOREVER );
    if ( wiced_result != WICED_SUCCESS )
    {
        result = PLATFORM_ERROR;
    }
    else
    {
        /* Wait for message to be acknowledge. */
        (void)wiced_rtos_get_semaphore( &info->synchronous_push_to_queue_semaphore, WICED_WAIT_FOREVER );
        result = PLATFORM_SUCCESS;
    }

    return result;
}

platform_result_t platform_capsense_enable( const platform_capsense_widget_t* widget, wiced_bool_t want_enabled )
{
    platform_result_t           result                      = PLATFORM_SUCCESS;
    platform_capsense_info_t*   info                        = &platform_capsense_info;
    wiced_bool_t volatile       is_enabled;

    /* Synchronization is provided by push-to-queue, so no need for a mutex. */
    is_enabled = platform_capsense_is_widget_enabled( widget->id );

    /* Is the status being changed? */
    if ( is_enabled != want_enabled )
    {
        platform_capsense_event_message_t message = {
            .event_id               = PLATFORM_CAPSENSE_WRITE_WIDGET_EVENT_ID,
            .widget                 = widget,
            .is_enabled             = want_enabled == WICED_TRUE ? 1 : 0,
        };
        result = platform_capsense_synchronous_push_to_queue( info, &message );
        if ( result == PLATFORM_SUCCESS )
        {
            is_enabled = platform_capsense_is_widget_enabled( widget->id );
            wiced_assert( "status not set", is_enabled == want_enabled );
        }
    }
    else
    {
        /* The new status is the same as the current status.
         * API called more than once?
         */
        wiced_assert( "widget status is unchanged", 0!=0 );
        result = PLATFORM_ERROR;
    }

    return result;
}

static platform_result_t platform_capsense_query_is_active( const platform_capsense_widget_t* widget, void* arg )
{
    wiced_bool_t* is_active = (wiced_bool_t*)arg;
    *is_active = CapSense_IsWidgetActive( widget->id ) != 0uL ? WICED_TRUE : WICED_FALSE;
    return PLATFORM_SUCCESS;
}

static platform_result_t platform_capsense_query_centroid_position( const platform_capsense_widget_t* widget, void* arg )
{
    uint32_t* position = (uint32_t*)arg;
    *position = CapSense_GetCentroidPos( widget->id );
    if ( *position == CapSense_SLIDER_NO_TOUCH )
        return PLATFORM_ERROR;
    return PLATFORM_SUCCESS;
}

static platform_result_t platform_capsense_query( const platform_capsense_widget_t* widget, platform_capnsense_query_fn* query_fn, void* arg )
{
    platform_result_t                   result;
    platform_capsense_info_t*           info        = &platform_capsense_info;
    wiced_mutex_t*                      mutex       = &info->mutex;

    if ( platform_capsense_is_widget_enabled( widget->id ) != WICED_TRUE )
    {
        /* Widget must be enabled. */
        return PLATFORM_ERROR;
    }

    wiced_rtos_lock_mutex( mutex );
    result = (*query_fn)( widget, arg );
    wiced_rtos_unlock_mutex( mutex );

    return result;
}

platform_result_t platform_capsense_is_active( const platform_capsense_widget_t* widget, wiced_bool_t* is_active )
{
    return platform_capsense_query( widget, platform_capsense_query_is_active, is_active );
}

platform_result_t platform_capsense_get_centroid_position( const platform_capsense_widget_t* widget, uint32_t* position )
{
    return platform_capsense_query( widget, platform_capsense_query_centroid_position, position );
}

platform_result_t platform_capsense_register_event_callback( platform_capsense_event_callback_t* callback )
{
    platform_capsense_info_t*           info        = &platform_capsense_info;
    wiced_mutex_t*                      mutex       = &info->mutex;

    wiced_rtos_lock_mutex( mutex );
    info->callback = callback;
    wiced_rtos_unlock_mutex( mutex );

    return PLATFORM_SUCCESS;
}

void platform_csd_irq(void)
{
    if (ss_irq_handler != NULL )
    {
        (*ss_irq_handler)();
    }
}
