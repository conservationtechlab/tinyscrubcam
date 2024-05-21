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
#pragma once

#include <stdint.h>
#include <string.h>
#include "wiced_defaults.h"
#include "wiced_result.h"
#include "wiced_power_logger.h"
#include "wiced_low_power.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef PLATFORM_WPL_HEADER_INCLUDED
#error "Platform WPL header file must not be included directly, Please use wpl_platform_api.h instead."
#endif


/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                      WPL-Macros
 ******************************************************/
#define WPL_DEFAULT_WORKER_PRIORITY 5
#define WPL_UART_STACK_SIZE      ( 6 * 1024 )
#define WPL_TIME_TO_WAIT_FOR_CONSOLE        2000 /* msec */

/* Use this macro to save WPL specific AON variables. Currently we cannot use WICED macro as 20739 has its own MACRO */
#define WPL_DEEP_SLEEP_SAVED_VAR(var)       WICED_DEEP_SLEEP_SAVED_VAR( var )
#define WPL_DEEP_SLEEP_IS_WARMBOOT()        WICED_DEEP_SLEEP_IS_WARMBOOT()
#define WPL_DEEP_SLEEP_IS_WARMBOOT_HANDLE() WICED_DEEP_SLEEP_IS_WARMBOOT_HANDLE()

#define wpl_semaphore_t wiced_semaphore_t
#define wpl_mutex_t wiced_mutex_t
#define wpl_thread_t wiced_thread_t

#define platform_wpl_rtos_init_semaphore wiced_rtos_init_semaphore
#define platform_wpl_rtos_set_semaphore wiced_rtos_set_semaphore
#define platform_wpl_rtos_get_semaphore wiced_rtos_get_semaphore
#define platform_wpl_rtos_deinit_semaphore wiced_rtos_deinit_semaphore
#define platform_wpl_rtos_init_mutex wiced_rtos_init_mutex
#define platform_wpl_rtos_lock_mutex wiced_rtos_lock_mutex
#define platform_wpl_rtos_unlock_mutex wiced_rtos_unlock_mutex
#define platform_wpl_rtos_deinit_mutex wiced_rtos_deinit_mutex
#define platform_wpl_rtos_create_thread wiced_rtos_create_thread
#define platform_wpl_rtos_delete_thread wiced_rtos_delete_thread
#define platform_wpl_delay_milliseconds wiced_rtos_delay_milliseconds
#define platform_wpl_uart_receive_bytes(...) wiced_uart_receive_bytes(STDIO_UART, __VA_ARGS__)
#define platform_wpl_uart_transmit_bytes(...) wiced_uart_transmit_bytes(STDIO_UART, __VA_ARGS__)

// Inform WPL core that mandatory Platform MACROs and data structures are defined
#define PLATFORM_WPL_MACROS_DEFINED
#define PLATFORM_RTOS_API_DEFINED



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


#ifdef __cplusplus
} /* extern "C" */
#endif


