/*
 * Copyright 2021, Cypress Semiconductor Corporation or a subsidiary of 
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

/** @file
 *  CapSense platform API
 */
#pragma once
#include "stdint.h"
#include "platform_constants.h"     /* for platform_result_t */
#include "wwd_constants.h"          /* for wiced_bool_t      */
#include "platform_toolchain.h"     /* for stddefs.h         */

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/**
 * Helper macro to check that _PTR_ is of type _TYPE_
 *
 * @param[in]     _TYPE_           : expected type
 * @param[in]     _PTR_            : pointer to be verfied
 *
 * @return    _PTR_                : on success.
 * @return    compiler diagnostic  : if _PTR_ is not of type _TYPE_
 */
#define PLATFORM_CAPSENSE_CHECK_PTR_OF( _TYPE_, _PTR_ ) ( 1 ? (_PTR_) : (_TYPE_)0 )

/**
 * CapSense button widget initializer
 *
 * @param[in]     _ID_             : CapSense button widget id obtained from the Creator CapSense component (e.g. CapSense_BUTTON0_WDGT_ID)
 * @param[in]     _DATAPTR_        : Non-const pointer to @ref platform_capsense_button_data_t
 *
 * @return @ref platform_capsense_widget_t initializer list for button widget
 *
 */
#define PLATFORM_CAPSENSE_BUTTON_WIDGET_INIT( _ID_, _DATAPTR_ ) \
    { .id           = (_ID_),                                   \
      .driver       = &platform_capsense_button_driver,         \
      .data         = (platform_capsense_widget_data_t*)PLATFORM_CAPSENSE_CHECK_PTR_OF( platform_capsense_button_data_t*, (_DATAPTR_) ), \
      .data_sizeof  = (sizeof *(_DATAPTR_)),                    \
      .config       = NULL,                                     \
    }

/**
 * CapSense linear-slider widget initializer
 *
 * @param[in]     _ID_             : CapSense linear-slider widget id obtained from the Creator CapSense component ( e.g. CapSense_LINEARSLIDER0_WDGT_ID)
 * @param[in]     _DATAPTR_        : Non-const pointer to @ref platform_capsense_linear_slider_data_t
 * @param[in]     _CONFIGPTR_      : Non-const pointer to @ref platform_capsense_linear_slider_config_t
 *
 * @return @ref platform_capsense_widget_t initializer list for linear-slider widget
 */
#define PLATFORM_CAPSENSE_LINEAR_SLIDER_WIDGET_INIT( _ID_, _DATAPTR_, _CONFIGPTR_ ) \
    { .id           = (_ID_),                                                       \
      .driver       = &platform_capsense_linear_slider_driver,                      \
      .data         = (platform_capsense_widget_data_t*)PLATFORM_CAPSENSE_CHECK_PTR_OF(platform_capsense_linear_slider_data_t*, (_DATAPTR_)), \
      .data_sizeof  = (sizeof *(_DATAPTR_)),                                        \
      .config       = PLATFORM_CAPSENSE_CHECK_PTR_OF( platform_capsense_linear_slider_config_t*, (_CONFIGPTR_) ) \
    }

/**
 * Simple CapSense interface driver initializer
 *
 * @param[in]     _DRIVERCONFIGPTR_ : Pointer to @ref platform_capsense_simple_driver_config_t
 *
 * @return @ref platform_capsense_simple_interface_t initializer list
 */
#define PLATFORM_CAPSENSE_SIMPLE_INTERFACE_INIT( _DRIVERCONFIGPTR_ )  \
    { .driver       = &platform_capsense_simple_driver,            \
      .data         = PLATFORM_CAPSENSE_CHECK_PTR_OF(platform_capsense_simple_driver_config_t*, (_DRIVERCONFIGPTR_)) \
    }

/******************************************************
 *                    Constants
 ******************************************************/

#define PLATFORM_CAPSENSE_DEFAULT_ACTIVE_WIDGET_TIMEOUT_MS  (30)
#define PLATFORM_CAPSENSE_DEFAULT_TIMEOUT_MS                (200)

/******************************************************
 *                   Enumerations
 ******************************************************/

/**
 * Platform-level widget
 */
typedef enum platform_widget_event_id
{
    PLATFORM_WIDGET_TRACK_BUTTON_EVENT_ID               = 0,    /* button tracking event */
    PLATFORM_WIDGET_TRACK_LINEAR_SLIDER_EVENT_ID        = 1,    /* linear slider tracking event */
} platform_widget_event_id_t;


/******************************************************
 *                    Structures
 ******************************************************/

/* Forwards. */
struct platform_capsense_driver;
struct platform_capsense_widget_driver;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/**
 * CapSense driver interface.
 */
typedef struct platform_capsense_driver platform_capsense_driver_t;

/**
 * CapSense widget driver interface.
 */
typedef struct platform_capsense_widget_driver platform_capsense_widget_driver_t;

/**
 * Configuration settings for Simple CapSense driver.
 */
typedef struct platform_capsense_simple_driver_config
{
    uint32_t                                    wait_for_activity_scan_period_in_millisecond;   /* number of milliseconds between scan-processing-dispatch when sensors are active */
    uint32_t                                    active_scan_period_in_milliseconds;             /* number of milliseconds between scan-processing-dispatch when sensors are inactive */
} platform_capsense_simple_driver_config_t;

/**
 * Non-cost working data inherited by other widget types
 */
typedef struct platform_capsense_widget_data
{
    wiced_bool_t                                last_is_active_status;      /* last active widget status from previous sensor scan */
} platform_capsense_widget_data_t;

/**
 * Non-const working data for the button widget
 */
typedef struct platform_capsense_button_data
{
    platform_capsense_widget_data_t             base;       /* inherited data */
} platform_capsense_button_data_t;

/**
 * Non-const working data for the linear-slider widget
 */
typedef struct platform_capsense_linear_slider_data
{
    platform_capsense_widget_data_t             base;       /* inherited data */
    uint32_t                                    last_centroid_position;     /* last centroid position from previous sensor scan */
} platform_capsense_linear_slider_data_t;

/**
 * Linear-slider widget configuration.
 */
typedef struct platform_capsense_linear_slider_config
{
    uint32_t                                    position_notification_threshold;    /* position must change by this threshold value to trigger @ref PLATFORM_WIDGET_TRACK_LINEAR_SLIDER_EVENT_ID */
} platform_capsense_linear_slider_config_t;

/**
 * Abstract widget data structure.
 * Initialized at the platform-level with one of the helper initializer: PLATFORM_CAPSENSE_XXX_WIDGET_INIT, where XXX is a specific widget type.
 */
typedef struct platform_capsense_widget
{
    uint32_t                                    id;             /* CapSense widget id obtained from Creator (e.g. CapSense_XXX_WDGT_ID, where XXX is widget type) */
    platform_capsense_widget_driver_t const*    driver;         /* Pointer to platform_capsense_XXX_driver_t, where XXX is widget type */
    platform_capsense_widget_data_t*            data;           /* Pointer to non-const platform_capsense_XXX_data_t, where XXX is widget type */
    size_t                                      data_sizeof;    /* Return value of sizeof platform_capsense_XXX_data_t, where XXX is widget type */
    void*                                       config;         /* Optional pointer to platform_capsense_XXX_config_t, where XXX is widget type */
} platform_capsense_widget_t;

/**
 * Base event data
 */
typedef struct platform_base_event_data
{
    wiced_bool_t                                is_active;      /* WICED_TRUE when sensor activity was detected; WICED_FALSE otherwise */
} platform_base_event_data_t;

/**
 * Event data for @ref PLATFORM_WIDGET_TRACK_BUTTON_EVENT_ID
 */
typedef struct platform_track_button_event_data
{
    platform_base_event_data_t                  base;           /* inherited event data */
} platform_track_button_event_data_t;

/**
 * Event data for @ref PLATFORM_WIDGET_TRACK_LINEAR_SLIDER_EVENT_ID
 */
typedef struct platform_track_linear_slider_event_data
{
    platform_base_event_data_t                  base;           /* inherited event data */
    uint32_t                                    position;       /* centroid position */
} platform_track_linear_slider_event_data_t;

/**
 * CapSense platform interface driver.
 * Select and configure an underlying CapSense platform driver.
 *
 * Use PLATFORM_CAPSENSE_XXX_INTERFACE_INIT where XXX is a platform interface driver.
 */
typedef struct platform_capsense_interface
{
    platform_capsense_driver_t const*           driver;     /* driver interface */
    void const*                                 data;       /* driver configuration data */
} platform_capsense_interface_t;

/**
 * CapSense event notification callback.
 *
 * @param[in]     widget           : CapSense platform widget
 * @param[in]     event_id         : CapSense widget event
 * @param[in]     event_data       : CapSense widget-specific event data (cast to one of platform_XXX_event_data_t types, where XXX is a widget type)
 */
typedef void (platform_capsense_event_callback_t)( const platform_capsense_widget_t*, platform_widget_event_id_t, const void* );

/******************************************************
 *                 Global Variables
 ******************************************************/

/**
 * CapSense button widget driver.
 * Do not use directly, instead use @ref PLATFORM_CAPSENSE_BUTTON_WIDGET_INIT
 */
extern const platform_capsense_widget_driver_t platform_capsense_button_driver;

/**
 * CapSense linear-slider widget driver.
 * Do not use directly, instead use @ref PLATFORM_CAPSENSE_LINEAR_SLIDER_WIDGET_INIT
 */
extern const platform_capsense_widget_driver_t platform_capsense_linear_slider_driver;

/**
 * Simple CapSense platform driver.
 * Do not use directly, instead use @ref PLATFORM_CAPSENSE_SIMPLE_DRIVER_INIT
 */
extern const platform_capsense_driver_t platform_capsense_simple_driver;

/******************************************************
 *               Function Declarations
 ******************************************************/

/**
 * Initialise the CapSense interface
 *
 * Start the CapSense scan-processing-dispatch loop as per the optional
 * configuration provided.  The loop will scan all widgets, process
 * widget data then notify clients by calling the client registered
 * callback function.
 *
 * Sensor "ganging" is unsupported via this interface.
 *
 * This function must be called before using any other CapSense function.
 *
 * @param[in]     driver           : CapSense platform driver (must be @ref platform_capsense_simple_driver)
 * @param[in]     config           : configuration data for CapSense (must be of type @ref platform_capsense_simple_driver_config_t)
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_capsense_init( const platform_capsense_driver_t* driver, const void* config );

/**
 * Deinitialise the CapSense interface
 *
 * Stop the CapSense interface.
 * Do not call any other CapSense functions other than platform_capsense_init after
 * stopping the interface.
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_capsense_deinit( void );

/**
 * Enable or disable a CapSense widget
 *
 * Enabled widgets may be queried and generate events.
 *
 * @param[in]     widget           : CapSense platform widget
 * @param[in]     want_enabled     : WICED_TRUE to enable the widget; otherwise WICED_FALSE to disable
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_capsense_enable( const platform_capsense_widget_t* widget, wiced_bool_t want_enabled );

/**
 * Query the CapSense widget for the "active" state.
 *
 * An "active" widget means that at least one sensor detected activity,
 * (i.e. sensing a user's "touch").
 * The widget must be enabled prior to calling this function.
 *
 * @param[in]     widget           : CapSense platform widget
 * @param[out]    is_active        : WICED_TRUE for active; WICED_FALSE otherwise
 *
 * @return    PLATFORM_SUCCESS : on success.
 * @return    PLATFORM_ERROR   : when disabled.
 * @return @ref platform_result_t
 */
platform_result_t platform_capsense_is_active( const platform_capsense_widget_t* widget, wiced_bool_t* is_active );

/**
 * Query the CapSense widget for its centroid position.
 *
 * The widget must be enabled prior to calling this function.
 *
 * @param[in]     widget           : CapSense linear or radial slider widget
 * @param[out]    position         : centroid position
 *
 * @return    PLATFORM_SUCCESS : on success.
 * @return    PLATFORM_ERROR   : when disabled or inactive.
 * @return @ref platform_result_t
 */
platform_result_t platform_capsense_centroid_position( const platform_capsense_widget_t* widget, uint32_t* position );

/**
 * Register a callback function to for client to receive CapSense events.
 *
 * The event callback is invoked per widget when changes are detected,
 * subject to widget configuration.  The callback frequency is also
 * subject to the configuration provided during initialisation.  Limit
 * workloads within the definition of the callback function.
 *
 * @param[in]     callback         : event callback function
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_capsense_register_event_callback( platform_capsense_event_callback_t* callback );

#ifdef __cplusplus
} /*"C" */
#endif
