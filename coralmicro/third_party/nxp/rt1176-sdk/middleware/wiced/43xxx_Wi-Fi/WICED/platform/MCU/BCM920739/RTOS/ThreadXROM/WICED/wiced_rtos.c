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
 *
 */
#include "wiced_rtos.h"
#include "wiced_time.h"
#include "wiced_defaults.h"
#include "wiced_low_power.h"
#include "RTOS/wwd_rtos_interface.h"
#include "wiced_utilities.h"
#include "wwd_debug.h"
#include "wwd_assert.h"
#include "internal/wiced_internal_api.h"
#include "RTOS/wiced_rtos_common.h"
#include "wiced_rtos_common.h"

#ifdef TX_ENABLE_EVENT_TRACE
#include "platform_config.h"  /* pick up platform specific tracex buffer location */
#endif /* ifdef TX_ENABLE_EVENT_TRACE */

/******************************************************
 *                      Macros
 ******************************************************/

#define WORKER_THREAD_MONITOR_UPDATE(worker, delay)    do { worker->monitor_info.last_update = host_rtos_get_time(); worker->monitor_info.longest_delay = delay; } while(0)

//#define TX_TIMEOUT(timeout_ms)   ((timeout_ms != WICED_NEVER_TIMEOUT) ? ((ULONG)(timeout_ms / ms_to_tick_ratio)) : TX_WAIT_FOREVER )

/******************************************************
 *                    Constants
 ******************************************************/

#ifndef APPLICATION_STACK_SIZE
#define APPLICATION_STACK_SIZE   WICED_DEFAULT_APPLICATION_STACK_SIZE
#endif

#define SYSTEM_MONITOR_THREAD_STACK_SIZE   512

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/
extern uint32_t clock_SystemTimeMicroseconds32( void );
/******************************************************
 *               Variable Definitions
 ******************************************************/

const uint32_t ms_to_tick_ratio = 1000 / SYSTICK_FREQUENCY;
wiced_worker_thread_t wiced_hardware_io_worker_thread;
wiced_worker_thread_t wiced_networking_worker_thread;

static wiced_time_t present_sys_time = 0;
static wiced_time_t last_sys_time = 0;

#ifdef TX_ENABLE_EVENT_TRACE
#ifndef WICED_TRACEX_BUFFER_SIZE
#define WICED_TRACEX_BUFFER_SIZE       (0x10000)
#endif /* ifndef WICED_TRACEX_BUFFER_SIZE */
#ifndef WICED_TRACEX_OBJECT_COUNT
#define WICED_TRACEX_OBJECT_COUNT      (100)
#endif /* ifndef WICED_TRACEX_OBJECT_COUNT */
#ifndef WICED_TRACEX_BUFFER_ADDRESS
static uint8_t tracex_buffer[WICED_TRACEX_BUFFER_SIZE];
#define WICED_TRACEX_BUFFER_ADDRESS          (tracex_buffer)
#endif /* ifndef WICED_TRACEX_BUFFER_ADDRESS */
#endif /* ifdef TX_ENABLE_EVENT_TRACE */

/* These are the stubs for the de-init implementation from the BT FW2
 * Should be removed when we have this implemented in FW2 code.
 * */
wiced_result_t wiced_rtos_delete_thread_rom( wiced_thread_t* arg )
{
    UNUSED_PARAMETER( arg );
    return WICED_SUCCESS;
}
wiced_result_t wiced_rtos_deinit_semaphore_rom( wiced_semaphore_t* arg )
{
    UNUSED_PARAMETER( arg );
    return WICED_SUCCESS;
}
wiced_result_t wiced_rtos_deinit_mutex_rom( wiced_mutex_t* arg )
{
    UNUSED_PARAMETER( arg );
    return WICED_SUCCESS;
}
wiced_result_t wiced_rtos_deinit_queue_rom( wiced_queue_t* arg )
{
    UNUSED_PARAMETER( arg );
    return WICED_SUCCESS;
}
wiced_result_t wiced_rtos_deinit_timer_rom( wiced_timer_t* arg )
{
    UNUSED_PARAMETER( arg );
    return WICED_SUCCESS;
}
wiced_result_t wiced_rtos_deinit_event_flags_rom( wiced_event_flags_t* arg )
{
    UNUSED_PARAMETER( arg );
    return WICED_SUCCESS;
}

/******************************************************
 *               Function Definitions
 ******************************************************/

wiced_result_t wiced_rtos_create_thread( wiced_thread_t* thread, uint8_t priority, const char* name, wiced_thread_function_t function, uint32_t stack_size, void* arg )
{
    thread->thread = wiced_rtos_create_thread_rom( );
    if ( thread->thread == NULL )
        return WICED_ERROR;
    /*2039 FW limits priority to 0(lowest) to 7(highest)*/
    if ( priority > 6 )
        priority = 1; /* priority 0 is reserved for IDLE thread in 20739 FW */
    else
        priority = (uint8_t) ( 7 - (uint32_t) priority );
    return wiced_rtos_init_thread( thread->thread, priority, name, function, stack_size, arg );
}

wiced_result_t wiced_rtos_create_thread_with_stack( wiced_thread_t* thread, uint8_t priority, const char* name, wiced_thread_function_t function, void* stack, uint32_t stack_size, void* arg )
{
    UNUSED_PARAMETER( thread );
    UNUSED_PARAMETER( priority );
    UNUSED_PARAMETER( name );
    UNUSED_PARAMETER( function );
    UNUSED_PARAMETER( stack );
    UNUSED_PARAMETER( stack_size );
    UNUSED_PARAMETER( arg );
    return WICED_UNSUPPORTED;
}

wiced_result_t wiced_rtos_delete_thread( wiced_thread_t* thread )
{
    return wiced_rtos_delete_thread_rom( thread->thread );
}

wiced_result_t wiced_rtos_thread_join( wiced_thread_t* thread )
{
    return wiced_rtos_thread_join_rom( thread->thread );
}

wiced_result_t wiced_rtos_thread_force_awake( wiced_thread_t* thread )
{
    return wiced_rtos_thread_force_awake_rom( thread->thread );
}

wiced_result_t wiced_rtos_is_current_thread( wiced_thread_t* thread )
{
    return wiced_rtos_is_current_thread_rom( thread->thread );
}

wiced_result_t wiced_time_get_time( wiced_time_t* time_ptr )
{
    wiced_time_t temp_time;
    temp_time = (wiced_time_t) ( clock_SystemTimeMicroseconds32( ) );
    present_sys_time += ( ( temp_time - last_sys_time ) / 1000 );
    last_sys_time = temp_time;
    *time_ptr = present_sys_time;
    return WICED_SUCCESS;
}

wiced_result_t wiced_time_set_time( const wiced_time_t* time_ptr )
{
    last_sys_time = (wiced_time_t) ( clock_SystemTimeMicroseconds32( ) );
    present_sys_time = *time_ptr;
    return WICED_SUCCESS;
}

/** Delay current thread for a given period of microseconds
 * As per wiced definition this API is supposed to block **/
wiced_result_t wiced_rtos_delay_microseconds( uint32_t microseconds )
{
    uint32_t current_time;
    uint32_t target_time;

    current_time = clock_SystemTimeMicroseconds32( );
    target_time = current_time + microseconds;
    while ( current_time < target_time )
    {
        current_time = clock_SystemTimeMicroseconds32( );
    }

    return WICED_SUCCESS;
}
/** Sleep for a given period of milliseconds
 *
 * Causes the current thread to sleep for AT LEAST the
 * specified number of milliseconds. If the processor is heavily loaded
 * with higher priority tasks, the delay may be much longer than requested.
 *
 * @param milliseconds : the time to sleep in milliseconds
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_delay_milliseconds( uint32_t milliseconds )
{
    if ( milliseconds > 0 )
        return wiced_rtos_delay_milliseconds_rom( milliseconds, ALLOW_THREAD_TO_SLEEP );
    else
        return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_init_semaphore( wiced_semaphore_t* semaphore )
{
    semaphore->semaphore = wiced_rtos_create_semaphore( );
    if ( semaphore->semaphore == NULL )
        return WICED_ERROR;
    return wiced_rtos_init_semaphore_rom( semaphore->semaphore );
}

wiced_result_t wiced_rtos_set_semaphore( wiced_semaphore_t* semaphore )
{
    return wiced_rtos_set_semaphore_rom( semaphore->semaphore );
}

wiced_result_t wiced_rtos_get_semaphore( wiced_semaphore_t* semaphore, uint32_t timeout_ms )
{
    wiced_result_t status;

    status = wiced_rtos_get_semaphore_rom( semaphore->semaphore, timeout_ms / OS_TIMER_TICK_PERIOD_IN_MS );

    if ( status == TX_SUCCESS )
    {
        return WICED_SUCCESS;
    }
    else if ( status == TX_NO_INSTANCE )
    {
        return WICED_TIMEOUT;
    }
    else if ( status == TX_WAIT_ABORTED )
    {
        return WICED_ABORTED;
    }
    else
    {
        return WICED_ERROR;
    }
}

wiced_result_t wiced_rtos_deinit_semaphore( wiced_semaphore_t* semaphore )
{
    return wiced_rtos_deinit_semaphore_rom( semaphore->semaphore );
}

wiced_result_t wiced_rtos_init_mutex( wiced_mutex_t* mutex )
{
    mutex->mutex = wiced_rtos_create_mutex( );
    if ( mutex->mutex == NULL )
        return WICED_ERROR;
    return wiced_rtos_init_mutex_rom( mutex->mutex );
}

wiced_result_t wiced_rtos_lock_mutex( wiced_mutex_t* mutex )
{
    return wiced_rtos_lock_mutex_rom( mutex->mutex );
}

wiced_result_t wiced_rtos_unlock_mutex( wiced_mutex_t* mutex )
{
    return wiced_rtos_unlock_mutex_rom( mutex->mutex );
}

wiced_result_t wiced_rtos_deinit_mutex( wiced_mutex_t* mutex )
{
    return wiced_rtos_deinit_mutex_rom( mutex->mutex );
}

wiced_result_t wiced_rtos_init_queue( wiced_queue_t* queue, const char* name, uint32_t message_size, uint32_t number_of_messages )
{
    queue->queue = wiced_rtos_create_queue( );
    if ( queue->queue == NULL )
        return WICED_ERROR;
    return wiced_rtos_init_queue_rom( queue->queue, name, message_size, number_of_messages );
}

wiced_result_t wiced_rtos_push_to_queue( wiced_queue_t* queue, void* message, uint32_t timeout_ms )
{
    return wiced_rtos_push_to_queue_rom( queue->queue, message, timeout_ms );
}

wiced_result_t wiced_rtos_pop_from_queue( wiced_queue_t* queue, void* message, uint32_t timeout_ms )
{
    return wiced_rtos_pop_from_queue_rom( queue->queue, message, timeout_ms );
}

wiced_result_t wiced_rtos_deinit_queue( wiced_queue_t* queue )
{
    return wiced_rtos_deinit_queue_rom( queue->queue );
}

wiced_result_t wiced_rtos_is_queue_empty( wiced_queue_t* queue )
{
    return wiced_rtos_is_queue_empty_rom( queue->queue );
}

wiced_result_t wiced_rtos_is_queue_full( wiced_queue_t* queue )
{
    return wiced_rtos_is_queue_full_rom( queue->queue );
}

wiced_result_t wiced_rtos_get_queue_occupancy( wiced_queue_t* queue, uint32_t *count )
{
    return wiced_rtos_get_queue_occupancy_rom( queue->queue, count );
}

wiced_result_t wiced_rtos_init_timer( wiced_timer_t* timer, uint32_t time_ms, timer_handler_t function, void* arg )
{
    timer->timer = wiced_rtos_create_timer( );
    if ( timer->timer == NULL )
        return WICED_ERROR;
    return wiced_rtos_init_timer_rom( timer->timer, time_ms, function, arg );
}

wiced_result_t wiced_rtos_start_timer( wiced_timer_t* timer )
{
    return wiced_rtos_start_timer_rom( timer->timer );
}

wiced_result_t wiced_rtos_stop_timer( wiced_timer_t* timer )
{
    return wiced_rtos_stop_timer_rom( timer->timer );
}

wiced_result_t wiced_rtos_deinit_timer( wiced_timer_t* timer )
{
    if ( WICED_SUCCESS == wiced_rtos_is_timer_running( timer ) )
        wiced_rtos_stop_timer( timer );
    return wiced_rtos_deinit_timer_rom( timer->timer );
}

wiced_result_t wiced_rtos_is_timer_running( wiced_timer_t* timer )
{
    return wiced_rtos_is_timer_running_rom( timer->timer );
}

#ifndef WTHREAD_FIX
wiced_result_t wiced_rtos_create_worker_thread( wiced_worker_thread_t* worker_thread, uint8_t priority, uint32_t stack_size, uint32_t event_queue_size )
{
    worker_thread->worker_thread = wiced_rtos_create_worker_thread_rom( );
    if ( worker_thread->worker_thread == NULL )
        return WICED_ERROR;
    return wiced_rtos_init_worker_thread( worker_thread->worker_thread, priority, stack_size, event_queue_size );
}
/* Should be removed once BT FW2 implements this function*/
wiced_result_t wiced_rtos_delete_worker_thread_rom( wiced_worker_thread_t* worker_thread )
{
    UNUSED_PARAMETER( worker_thread );
    return WICED_SUCCESS;
}

wiced_result_t wiced_rtos_delete_worker_thread( wiced_worker_thread_t* worker_thread )
{
    return wiced_rtos_delete_worker_thread_rom( worker_thread->worker_thread );
}

wiced_result_t wiced_rtos_register_timed_event( wiced_timed_event_t* event_object, wiced_worker_thread_t* worker_thread, event_handler_t function, uint32_t time_ms, void* arg )
{
    return wiced_rtos_register_timed_event_rom( event_object, worker_thread->worker_thread, function, time_ms, arg );
}

wiced_result_t wiced_rtos_send_asynchronous_event( wiced_worker_thread_t* worker_thread, event_handler_t function, void* arg )
{
    return wiced_rtos_send_asynchronous_event_rom( worker_thread->worker_thread, function, arg );
}
#endif /*ifndef WTHREAD_FIX*/

wiced_result_t wiced_rtos_init_event_flags( wiced_event_flags_t* event_flags )
{
    event_flags->event_flags = wiced_rtos_create_event_flags( );
    if ( event_flags->event_flags == NULL )
        return WICED_ERROR;
    return wiced_rtos_init_event_flags_rom( event_flags->event_flags );
}

wiced_result_t wiced_rtos_wait_for_event_flags( wiced_event_flags_t* event_flags, uint32_t flags_to_wait_for, uint32_t* flags_set, wiced_bool_t clear_set_flags, wiced_event_flags_wait_option_t wait_option, uint32_t timeout_ms )
{
    return wiced_rtos_wait_for_event_flags_rom( event_flags->event_flags, flags_to_wait_for, flags_set, clear_set_flags, wait_option, timeout_ms );
}

wiced_result_t wiced_rtos_set_event_flags( wiced_event_flags_t* event_flags, uint32_t flags_to_set )
{
    return wiced_rtos_set_event_flags_rom( event_flags->event_flags, flags_to_set );
}

wiced_result_t wiced_rtos_deinit_event_flags( wiced_event_flags_t* event_flags )
{
    return wiced_rtos_deinit_event_flags_rom( event_flags->event_flags );
}

WICED_SLEEP_EVENT_HANDLER( deep_sleep_rtos_event_handler )
{
    if ( event == WICED_SLEEP_EVENT_ENTER )
    {
        /* Save current time before entering deep-sleep */
        /* before_deep_sleep_time = tx_time_get( );*/
    }
}
