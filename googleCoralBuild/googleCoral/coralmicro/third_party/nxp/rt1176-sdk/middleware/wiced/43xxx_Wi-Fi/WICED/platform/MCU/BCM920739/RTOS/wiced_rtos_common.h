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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

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
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/* WICED <-> RTOS API */
extern wiced_result_t wiced_rtos_init  ( void );
extern wiced_result_t wiced_rtos_deinit( void );

/* Entry point for user Application */
extern void application_start          ( void );

extern wiced_thread_t*  wiced_rtos_create_thread_rom( void );
extern wiced_result_t wiced_rtos_delete_thread_rom( wiced_thread_t* thread );
extern wiced_result_t wiced_rtos_thread_join_rom( wiced_thread_t* thread );
extern wiced_result_t wiced_rtos_thread_join_rom( wiced_thread_t* thread );
extern wiced_result_t wiced_rtos_thread_force_awake_rom( wiced_thread_t* thread );
extern wiced_result_t wiced_rtos_is_current_thread_rom( wiced_thread_t* thread );

extern wiced_result_t wiced_rtos_set_semaphore_rom( wiced_semaphore_t* semaphore );
extern wiced_result_t wiced_rtos_get_semaphore_rom( wiced_semaphore_t* semaphore, uint32_t timeout_ms );
extern wiced_result_t wiced_rtos_deinit_semaphore_rom( wiced_semaphore_t* semaphore );
extern wiced_result_t wiced_rtos_lock_mutex_rom( wiced_mutex_t* mutex );
extern wiced_result_t wiced_rtos_unlock_mutex_rom( wiced_mutex_t* mutex );
extern wiced_result_t wiced_rtos_deinit_mutex_rom( wiced_mutex_t* mutex );
extern wiced_result_t wiced_rtos_push_to_queue_rom( wiced_queue_t* queue, void* message, uint32_t timeout_ms );
extern wiced_result_t wiced_rtos_pop_from_queue_rom( wiced_queue_t* queue, void* message, uint32_t timeout_ms );
extern wiced_result_t wiced_rtos_deinit_queue_rom( wiced_queue_t* queue );
extern wiced_result_t wiced_rtos_is_queue_empty_rom( wiced_queue_t* queue );
extern wiced_result_t wiced_rtos_is_queue_full_rom( wiced_queue_t* queue );
extern wiced_result_t wiced_rtos_get_queue_occupancy_rom( wiced_queue_t* queue, uint32_t *count );
extern wiced_result_t wiced_rtos_start_timer_rom( wiced_timer_t* timer );
extern wiced_result_t wiced_rtos_stop_timer_rom( wiced_timer_t* timer );
extern wiced_result_t wiced_rtos_deinit_timer_rom( wiced_timer_t* timer );
extern wiced_result_t wiced_rtos_is_timer_running_rom( wiced_timer_t* timer );
extern wiced_result_t wiced_rtos_wait_for_event_flags_rom( wiced_event_flags_t* event_flags, uint32_t flags_to_wait_for, uint32_t* flags_set, wiced_bool_t clear_set_flags, wiced_event_flags_wait_option_t wait_option, uint32_t timeout_ms );
extern wiced_result_t wiced_rtos_set_event_flags_rom( wiced_event_flags_t* event_flags, uint32_t flags_to_set );
extern wiced_result_t wiced_rtos_deinit_event_flags_rom( wiced_event_flags_t* event_flags );

extern wiced_worker_thread_t* wiced_rtos_create_worker_thread_rom(void);
extern wiced_result_t wiced_rtos_init_event_flags_rom( wiced_event_flags_t* event_flags );
extern wiced_result_t wiced_rtos_init_semaphore_rom( wiced_semaphore_t* semaphore );
extern wiced_result_t wiced_rtos_init_mutex_rom( wiced_mutex_t* semaphore );
extern wiced_result_t wiced_rtos_init_queue_rom( wiced_queue_t* queue, const char* name, uint32_t message_size, uint32_t number_of_messages );
extern wiced_result_t wiced_rtos_init_timer_rom( wiced_timer_t* timer, uint32_t time_ms, timer_handler_t function, void* arg );
extern wiced_result_t wiced_rtos_delay_milliseconds_rom( uint32_t milliseconds, wiced_delay_type_t delay_type);
extern wiced_event_flags_t*  wiced_rtos_create_event_flags( void );
extern wiced_rtos_timer_t*  wiced_rtos_create_timer( void );
extern wiced_mutex_t*  wiced_rtos_create_mutex( void );
extern wiced_semaphore_t*  wiced_rtos_create_semaphore( void );
extern wiced_queue_t*  wiced_rtos_create_queue( void );
//extern wiced_result_t wiced_rtos_delete_thread( wiced_thread_t* thread );
extern wiced_result_t wiced_rtos_thread_join( wiced_thread_t* thread );
extern wiced_result_t wiced_rtos_init_thread( wiced_thread_t* thread, uint8_t priority, const char* name, wiced_thread_function_t function, uint32_t stack_size, void* arg );
extern wiced_result_t wiced_rtos_init_thread_nocheck( wiced_thread_t* thread, uint8_t priority, const char* name, wiced_thread_function_t function, uint32_t stack_size, void* arg );


#ifdef __cplusplus
} /* extern "C" */
#endif
