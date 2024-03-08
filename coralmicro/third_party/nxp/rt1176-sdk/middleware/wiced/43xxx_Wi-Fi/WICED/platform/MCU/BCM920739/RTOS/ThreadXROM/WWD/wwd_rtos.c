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
 *  Implementation of wiced_rtos.c for ThreadX
 *
 *  This is the ThreadX implementation of the Wiced RTOS
 *  abstraction layer.
 *  It provides Wiced with standard ways of using threads,
 *  semaphores and time functions
 *
 */

#include "wwd_rtos.h"
#include "wiced_rtos.h"
#include "rtos.h"
#include <stdint.h>
#include "wwd_constants.h"
#include "RTOS/wwd_rtos_interface.h"
#include "wwd_assert.h"
#include "wiced_rtos_common.h"
#ifdef __GNUC__
#include <reent.h>
#endif /* #ifdef __GNUC__ */
#include "platform/wwd_platform_interface.h"


#define UINT uint32_t

#define TX_SUCCESS                      0x00
#define TX_NO_INSTANCE                  0x0D
#define TX_WAIT_ABORTED                 0x1A

#ifdef __GNUC__
void __malloc_lock(struct _reent *ptr);
void __malloc_unlock(struct _reent *ptr);
#endif /* ifdef __GNUC__ */

/******************************************************
 *             Constants
 ******************************************************/

/******************************************************
 *             Variables
 ******************************************************/
#ifdef __GNUC__
//static uint32_t  malloc_mutex_inited = 0;
//static void* malloc_mutex;
#endif /* ifdef __GNUC__ */


/******************************************************
 *             Function definitions
 ******************************************************/
extern uint64_t clock_SystemTimeMicroseconds64_nolock(void);
extern wiced_result_t wiced_rtos_init_thread( wiced_thread_t* thread, uint8_t priority, const char* name, wiced_thread_function_t function, uint32_t stack_size, void* arg );


/**
 * Creates a new thread
 *
 * @param thread         : pointer to variable which will receive handle of created thread
 * @param entry_function : main thread function
 * @param name           : a string thread name used for a debugger
 *
 * @returns WWD_SUCCESS on success, WICED_ERROR otherwise
 */
wwd_result_t host_rtos_create_thread( /*@out@*/ host_thread_type_t* thread, void(*entry_function)( uint32_t ), const char* name, /*@null@*/ void* stack, uint32_t stack_size, uint32_t priority )
{
    wwd_result_t  status ;
    UNUSED_PARAMETER(stack);
    thread->thread = wiced_rtos_create_thread_rom();

    if (thread->thread == NULL)
        return WWD_THREAD_CREATE_FAILED;
    status =  wiced_rtos_init_thread_nocheck( thread->thread, (uint8_t)priority, name, entry_function, stack_size, NULL );

    if (status != WWD_SUCCESS)
        return WWD_THREAD_CREATE_FAILED;
    else
        return WWD_SUCCESS;
}

wwd_result_t host_rtos_create_thread_with_arg( host_thread_type_t* thread, void(*entry_function)( uint32_t ), const char* name, /*@null@*/ void* stack, uint32_t stack_size, uint32_t priority, uint32_t arg )
{
    wwd_result_t  status ;
    UNUSED_PARAMETER(stack);
    thread->thread = wiced_rtos_create_thread_rom();

    if (thread->thread == NULL)
        return WWD_THREAD_CREATE_FAILED;
    status =  wiced_rtos_init_thread( thread->thread, (uint8_t)priority, name, entry_function, stack_size, (void*)arg );

    if (status != WWD_SUCCESS)
        return WWD_THREAD_CREATE_FAILED;
    else
        return WWD_SUCCESS;
}

/**
 * Terminates the current thread
 *
 * This does nothing since ThreadX threads can exit by just returning
 *
 * @param thread         : handle of the thread to terminate
 *
 * @returns WWD_SUCCESS on success, WICED_ERROR otherwise
 */
wwd_result_t host_rtos_finish_thread( host_thread_type_t* thread )
{
    return    wiced_rtos_delete_thread_rom(thread->thread);
}

/**
 * Deletes a terminated thread
 *
 * ThreadX requires that another thread deletes any terminated threads
 *
 * @param thread         : handle of the terminated thread to delete
 *
 * @returns WWD_SUCCESS on success, WICED_ERROR otherwise
 */
wwd_result_t host_rtos_delete_terminated_thread( host_thread_type_t* thread )
{
    return    wiced_rtos_delete_thread_rom(thread->thread);
}

/**
 * Blocks the current thread until the indicated thread is complete
 *
 * @param thread         : handle of the thread to terminate
 *
 * @returns WWD_SUCCESS on success, WICED_ERROR otherwise
 */
wwd_result_t host_rtos_join_thread( host_thread_type_t* thread )
{
    return wiced_rtos_thread_join_rom(thread->thread);
}

/**
 * Creates a semaphore
 *
 * @param semaphore         : pointer to variable which will receive handle of created semaphore
 *
 * @returns WWD_SUCCESS on success, WICED_ERROR otherwise
 */
wwd_result_t host_rtos_init_mutex( host_mutex_type_t* mutex )
{
    (void) mutex;
    return WWD_UNSUPPORTED;
}

wwd_result_t host_rtos_lock_mutex( host_mutex_type_t* mutex )
{
    (void) mutex;
    return WWD_UNSUPPORTED;
}

wwd_result_t host_rtos_unlock_mutex( host_mutex_type_t* mutex )
{
    (void) mutex;
    return WWD_UNSUPPORTED;
}

wwd_result_t host_rtos_deinit_mutex( host_mutex_type_t* mutex )
{
    (void) mutex;
    return WWD_UNSUPPORTED;
}
/**
 * Creates a semaphore
 *
 * @param semaphore         : pointer to variable which will receive handle of created semaphore
 *
 * @returns WWD_SUCCESS on success, WICED_ERROR otherwise
 */
wwd_result_t host_rtos_init_semaphore( /*@out@*/ host_semaphore_type_t* semaphore ) /*@modifies *semaphore@*/
{
    wwd_result_t status;

    semaphore->semaphore = wiced_rtos_create_semaphore();
    if (semaphore->semaphore == NULL)
        return WWD_SEMAPHORE_ERROR;
    status = (wwd_result_t)wiced_rtos_init_semaphore_rom(semaphore->semaphore);
    if ( status != WWD_SUCCESS)
        return     WWD_SEMAPHORE_ERROR;

    return WWD_SUCCESS;
}

/**
 * Gets a semaphore
 *
 * If value of semaphore is larger than zero, then the semaphore is decremented and function returns
 * Else If value of semaphore is zero, then current thread is suspended until semaphore is set.
 * Value of semaphore should never be below zero
 *
 * Must not be called from interrupt context, since it could block, and since an interrupt is not a
 * normal thread, so could cause RTOS problems if it tries to suspend it.
 *
 * @param semaphore   : Pointer to variable which will receive handle of created semaphore
 * @param timeout_ms  : Maximum period to block for. Can be passed NEVER_TIMEOUT to request no timeout
 * @param will_set_in_isr : True if the semaphore will be set in an ISR. Currently only used for NoOS/NoNS
 *
 */
wwd_result_t host_rtos_get_semaphore( host_semaphore_type_t* semaphore, uint32_t timeout_ms, wiced_bool_t will_set_in_isr )
{
    wwd_result_t status;
    UNUSED_PARAMETER( will_set_in_isr );

    status = (wwd_result_t) wiced_rtos_get_semaphore_rom( semaphore->semaphore, timeout_ms / OS_TIMER_TICK_PERIOD_IN_MS );

    if ( status == TX_SUCCESS )
    {
        return WWD_SUCCESS;
    }
    else if ( status == TX_NO_INSTANCE )
    {
        return WWD_TIMEOUT;
    }
    else if ( status == TX_WAIT_ABORTED )
    {
        return WWD_WAIT_ABORTED;
    }
    else
    {
        wiced_assert( "semaphore error ", 0 );
        return WWD_SEMAPHORE_ERROR;
    }
}

/**
 * Sets a semaphore
 *
 * If any threads are waiting on the semaphore, the first thread is resumed
 * Else increment semaphore.
 *
 * Can be called from interrupt context, so must be able to handle resuming other
 * threads from interrupt context.
 *
 * @param semaphore       : Pointer to variable which will receive handle of created semaphore
 * @param called_from_ISR : Value of WICED_TRUE indicates calling from interrupt context
 *                          Value of WICED_FALSE indicates calling from normal thread context
 *
 * @return wwd_result_t : WWD_SUCCESS if semaphore was successfully set
 *                        : WICED_ERROR if an error occurred
 *
 */
wwd_result_t host_rtos_set_semaphore( host_semaphore_type_t* semaphore, wiced_bool_t called_from_ISR )
{
    UNUSED_PARAMETER( called_from_ISR );
    if ((wwd_result_t)wiced_rtos_set_semaphore_rom(semaphore->semaphore) != WWD_SUCCESS)
        return     WWD_SEMAPHORE_ERROR;

    return WWD_SUCCESS;
}

/**
 * Deletes a semaphore
 *
 * WICED uses this function to delete a semaphore.
 *
 * @param semaphore         : Pointer to the semaphore handle
 *
 * @return wwd_result_t : WWD_SUCCESS if semaphore was successfully deleted
 *                        : WICED_ERROR if an error occurred
 *
 */
wwd_result_t host_rtos_deinit_semaphore( host_semaphore_type_t* semaphore )
{
    if((wwd_result_t)wiced_rtos_deinit_semaphore_rom(semaphore->semaphore) != WWD_SUCCESS)
            return     WWD_SEMAPHORE_ERROR;
    return WWD_SUCCESS;
}

/**
 * Gets time in milliseconds since RTOS start
 *
 * @Note: since this is only 32 bits, it will roll over every 49 days, 17 hours.
 *
 * @returns Time in milliseconds since RTOS started.
 */
wwd_time_t host_rtos_get_time( void )  /*@modifies internalState@*/
{
    return (wwd_time_t)(clock_SystemTimeMicroseconds64_nolock() / 1000);
}

/**
 * Delay for a number of milliseconds
 *
 * Processing of this function depends on the minimum sleep
 * time resolution of the RTOS.
 * The current thread sleeps for the longest period possible which
 * is less than the delay required, then makes up the difference
 * with a tight loop
 *
 * @return wwd_result_t : WWD_SUCCESS if delay was successful
 *                        : WICED_ERROR if an error occurred
 *
 */

wwd_result_t host_rtos_delay_milliseconds( uint32_t num_ms )
{
    if(num_ms > 0)
        return    (wwd_result_t)wiced_rtos_delay_milliseconds_rom(num_ms, ALLOW_THREAD_TO_SLEEP);
    else
        return WWD_SUCCESS;

}

unsigned long host_rtos_get_tickrate( void )
{
    return SYSTICK_FREQUENCY;
}

wwd_result_t host_rtos_init_queue( host_queue_type_t* queue, void* buffer, uint32_t buffer_size, uint32_t message_size )
{
    UNUSED_PARAMETER(buffer);
    queue->queue = wiced_rtos_create_queue();
    if(queue->queue == NULL)
        return WWD_MALLOC_FAILURE;
    if((wwd_result_t)wiced_rtos_init_queue_rom( queue->queue, NULL, message_size, (buffer_size/message_size) ) != WWD_SUCCESS)
        return WWD_QUEUE_ERROR;
    return WWD_SUCCESS;
}

wwd_result_t host_rtos_push_to_queue( host_queue_type_t* queue, void* message, uint32_t timeout_ms )
{
    return (wwd_result_t)wiced_rtos_push_to_queue_rom(queue->queue,message,timeout_ms);
}

wwd_result_t host_rtos_pop_from_queue( host_queue_type_t* queue, void* message, uint32_t timeout_ms )
{
    if ((wiced_rtos_is_queue_empty_rom(queue->queue) == 1) && (timeout_ms == 0))
    {
        return WWD_QUEUE_ERROR;
    }
    else
    {
        return (wwd_result_t)wiced_rtos_pop_from_queue_rom(queue->queue,message,timeout_ms);;
    }
}

wwd_result_t host_rtos_deinit_queue( host_queue_type_t* queue )
{
    return (wwd_result_t)wiced_rtos_deinit_queue_rom(queue->queue);
}



#ifdef __GNUC__
void __malloc_lock(struct _reent *ptr)
{
    UNUSED_PARAMETER( ptr );
}

void __malloc_unlock(struct _reent *ptr)
{
    UNUSED_PARAMETER( ptr );
}
#endif
