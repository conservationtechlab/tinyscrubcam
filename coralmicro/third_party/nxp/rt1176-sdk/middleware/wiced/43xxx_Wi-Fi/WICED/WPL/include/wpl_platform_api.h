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
 * WICED Power Logger (WPL) tool provides an estimate of power consumed by a target
 * device (or platform). The WPL tool consists of two main components: Target-WPL and Host-WPL.
 * The Target-WPL is a Wiced module that captures the power events from different components on the target
 * platform and sends these to the Host-WPL. The Host-WPL runs on the PC to receive power events
 * over UART and displays the real-time power plot. It refers to a platform-specific power database (xml)
 * and estimates current based on the power events and duration.
 *
 * WPL can be ported across Wiced platforms. This header exposes WPL Platform APIs to be implemented by platform while porting.
 * This header also provides WPL APIs that are used by WPL platform implementation to access/request WPL core features.
 */
#pragma once
#include "wiced_power_logger.h"


#ifdef __cplusplus
extern "C"
{
#endif

/* platform_wpl.h is part of WPL platform implementation and expected to define WPL Macros listed below */
#define PLATFORM_WPL_HEADER_INCLUDED
#include "platform_wpl.h"

/******************************************************
 *                      Macros
 ******************************************************/
/* platform_wpl.h must set this MACRO */
#ifndef PLATFORM_WPL_MACROS_DEFINED
#error "platform_wpl.h (WPL Platform implementation) header file must define mandatory WPL MACROS "
#endif

/* Following are the mandatory MACROs that need to be defined in platform WPL header file */
//#define WPL_DEFAULT_WORKER_PRIORITY
//#define WPL_UART_STACK_SIZE
//#define WPL_TIME_TO_WAIT_FOR_CONSOLE
//#define WPL_DEEP_SLEEP_SAVED_VAR(var)
//#define WPL_DEEP_SLEEP_IS_WARMBOOT()
//#define WPL_DEEP_SLEEP_IS_WARMBOOT_HANDLE()

/* Following MACROs are in WICED Wi-Fi and need to be defined by BT Platforms */
//#define WPRINT_APP_DEBUG( args )
//#define WPRINT_APP_INFO( args )
//#define WPRINT_LIB_DEBUG( args )
//#define WPRINT_LIB_INFO( args )
//#define WPRINT_LIB_ERROR( args )
//#define WPRINT_PLATFORM_ERROR( args )
//#define UNUSED_PARAMETER(x)
//#define NEVER_TIMEOUT
//#define malloc
//#define free


/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/
/**
 * @brief WPL Mode: WPL Core receives commands either in Console Mode or Direct Mode (No Console). The Console Mode is enabled
 * when application is using Wiced Command Console and WPL commands are sent to WPL Core through the Command Console
 * framework. In Direct mode, WPL Host directly communicates with WPL core using WPL Protocol
 *
 */
typedef enum {
    WPL_MODE_CONSOLE, /**< WPL Console Mode */
    WPL_MODE_NO_CONSOLE, /**< WPL Direct Mode (No Console) */
} wpl_mode_t;

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

/*****************************************************************************/

/**
 * WPL's platform module implements this API. WPL Core calls it to initialize WPL platform implementation.
 *
 * @param           none
 * @return          WICED_SUCCESS : on success.
 * @return          WICED_ERROR   : if an error occurred with any step
 */
wiced_result_t platform_wpl_init(void);

/**
 * WPL's platform module implements this API. WPL Core calls it to enable/disable WPL logging for all the components
 * supported by platform
 *
 * @param[in]  enable      : If true, enables WPL logging in platform else disables
 * @return     none
 */
void platform_wpl_enable_logging( wiced_bool_t enable );

/**
 * WPL's platform module implements this API. WPL Core calls it to reset power data stored for a WPL processor component
 *
 * @param[in]  processor_id      : WPL processor component like EVENT_PROC_ID_BT
 * @return     none
 */
void platform_wpl_reset_power_data(cpl_procid_t processor_id);

/**
 * WPL's platform module implements this API. WPL Core calls it to update power data for a WPL processor component.
 * WPL platform implementation retrieves the power data from components and logs against relevant power state using
 * WICED_POWER_LOGGER_DATA()
 *
 * @param[in]  processor_id      : WPL processor component like EVENT_PROC_ID_BT
 * @return     none
 */
void platform_wpl_update_power_data(cpl_procid_t processor_id);

/**
 * WPL's platform module implements this API. WPL components call it to get the current timestamp in milliseconds
 *
 * @param  none
 * @return The current timestamp in milliseconds
 */
uint32_t platform_wpl_get_time_stamp( void );

/**
 * WPL's platform module implements this API. WPL core saves the power data on AON before going into deep-sleep.
 * During warmboot the WPL core gets the MCU power state in which time spent in deep sleep is to be logged.
 * Mostly, platform returns the deep sleep state.
 *
 * @param  none
 * @return The MCU power state before warmboot
 */
uint8_t platform_wpl_get_mcu_power_state_before_warm_boot( void );

/**
 * WPL's platform module implements this API. WPL core gets this MCU power state from WPL platform
 * implementation to initialize MCU WPL logging
 *
 * @param  none
 * @return The MCU power state to initialize MCU WPL logging
 */
uint8_t platform_wpl_get_first_mcu_power_state( void );

/**
 * WPL Core implements this API. WPL Platform implementation calls this API to push power data to WPL host before going
 * into sleep.
 *
 * @param           none
 * @return          WICED_SUCCESS : on success.
 * @return          WICED_ERROR   : if an error occurred with any step
 */
wiced_result_t wpl_send_power_data_to_host( void );

/**
 * WPL Core implements this API to return the current WPL logging status
 *
 * @param           none
 * @return          WICED_TRUE : WPL logging is enabled
 * @return          WICED_FALSE: WPL logging is disabled
 */
wiced_bool_t wpl_logging_status( void );

/**
 * WPL core implements this API to initiate deep-sleep state on its data structures. WPL Platform
 * implementation calls this when platform triggers deep sleep
 *
 * @param[in]  state      : If true, enables deep sleep state on WPL.
 * @return     none
 */
void wpl_set_deep_sleep_state( wiced_bool_t state );

/**
 * WPL core's Wi-Fi module implements this API to reset Wi-Fi's power data. If WPL platform implementation supports
 * Wi-Fi power logging, will call this API when platform WPL power reset is called
 *
 * @param      none
 * @return     none
 */
void wpl_wifi_reset_power_data(void);

/**
 * WPL core's Wi-Fi module implements this API to trigger update in Wi-Fi's power data. WPL uses WWD's API to retrieve
 * power data. If WPL platform implementation supports Wi-Fi power logging, will call this API when platform WPL power
 * update is called
 *
 * @param      none
 * @return     none
 */
void wpl_wifi_update_power_data(void);


/* WPL APIs to support WPL console commands */

/**
 * Sets the command mode of WPL to Console Mode or Direct Mode
 *
 * @param           mode: WPL_MODE_CONSOLE or WPL_MODE_NO_CONSOLE
 * @return          NO return value.
 */
void wpl_set_mode(wpl_mode_t mode);

/**
 * Gets the command mode of WPL
 *
 * @param           none
 * @return          WPL_MODE_CONSOLE or WPL_MODE_NO_CONSOLE
 */
wpl_mode_t wpl_get_mode(void);

/**
 * Process the command received on Console mode using command string "wpl_cmd <parameters>"
 *
 * @param           argc: Number of arguments in console command string
 * @param           argv: console command arguments
 * @return          WICED_SUCCESS or WICED_ERROR
 */
wiced_result_t wpl_process_console_command( int argc, char* argv[] );

/**
 * Process the command received on Console mode using command string "wpl_debug <parameters>" to enable/disable
 * WPL prints on console
 *
 * @param           argc: Number of arguments in console command string
 * @param           argv: console command arguments
 * @return          none
 */
void wpl_process_enable_console_prints( int argc, char* argv[] );

#ifndef PLATFORM_RTOS_API_DEFINED

/* The functions and data structures of this section are used to maintain portability across WicedBT and Wiced Wi-Fi.
 *
 */
typedef struct{
    void* semaphore;
} wpl_semaphore_t;

typedef struct{
    void* mutex;
} wpl_mutex_t;

typedef struct{
    void *thread;
} wpl_thread_t;


/* Functions to maintain portability across Wiced-Wi-Fi and Wiced Smart */
wiced_result_t platform_wpl_rtos_init_semaphore( wpl_semaphore_t* semaphore );

wiced_result_t platform_wpl_rtos_set_semaphore( wpl_semaphore_t* semaphore );

wiced_result_t platform_wpl_rtos_get_semaphore( wpl_semaphore_t* semaphore, uint32_t timeout_ms );

wiced_result_t platform_wpl_rtos_deinit_semaphore( wpl_semaphore_t* semaphore );

wiced_result_t platform_wpl_rtos_init_mutex( wpl_mutex_t* mutex );

wiced_result_t platform_wpl_rtos_lock_mutex( wpl_mutex_t* mutex );

wiced_result_t platform_wpl_rtos_unlock_mutex( wpl_mutex_t* mutex );

wiced_result_t platform_wpl_rtos_deinit_mutex( wpl_mutex_t* mutex );


wiced_result_t platform_wpl_rtos_create_thread( wpl_thread_t* thread, uint8_t priority, const char* name, wiced_thread_function_t function, uint32_t stack_size, void* arg );

wiced_result_t platform_wpl_rtos_delete_thread( wpl_thread_t* thread );

wiced_result_t platform_wpl_delay_milliseconds( uint32_t milliseconds );

wiced_result_t platform_wpl_uart_receive_bytes( uint8_t* data_in, uint32_t* expected_data_size, uint32_t timeout_ms );

wiced_result_t platform_wpl_uart_transmit_bytes( const uint8_t* data_out, uint32_t size );

#endif



#ifdef __cplusplus
} /* extern "C" */
#endif

