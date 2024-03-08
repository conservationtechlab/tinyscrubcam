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
#include "wiced.h"
#include "wiced_low_power.h"
#include "wpl_platform_api.h"
#include "wiced_time.h"
#include "wiced_bt_dev.h"
#include "bt_types.h"
#include <wiced_sleep.h>
#include <wiced_hal_gpio.h>
#include "brcm_fw_types.h"

/******************************************************
 *                      Macros
 ******************************************************/

/* Vendor Specific Command to get BCS profiling information on 20719 */
#define BTHCI_CMD_VS_OCF_GET_CONTROLLER_ACTIVITY_ENERGY_INFO                                0x159

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

/* This is taken from FW2 mcu_profile.h */
typedef struct MCU_PROFILING_DATA
{
    UINT32 sleepTime;
    UINT32 idleTime;
} MCU_PROFILING_DATA;

/******************************************************
 *               Function Declarations
 ******************************************************/
void mcu_profiling_retrieve_data(MCU_PROFILING_DATA* profile);
void mcu_profiling_set(UINT8 set);

/******************************************************
 *               Variables Definitions
 ******************************************************/
static uint32_t prev_bt_profile_timestamp;

uint32_t WPL_DEEP_SLEEP_SAVED_VAR( deep_sleep_enter_timestamp );
uint16_t WPL_DEEP_SLEEP_SAVED_VAR( bt_duration_percentage_before_deep_sleep_poll );
static uint32_t time_spent_in_deepsleep;

/******************************************************
 *               Function Definitions
 ******************************************************/

/**
 * Gets time in milliseconds
 *
 * @Note: Platform specific, need to be ported accroding to the platform.
 *
 *
 * @returns Time in milliseconds
 */

uint32_t platform_wpl_get_time_stamp( void )
{
    return (uint32_t)( clock_SystemTimeMicroseconds64( ) / 1000 );
}

uint8_t platform_wpl_get_mcu_power_state_before_warm_boot( void )
{
    return (uint8_t)EVENT_DESC_POWER_DEEPSLEEP;
}


uint8_t platform_wpl_get_first_mcu_power_state( void )
{
    return (uint8_t)EVENT_DESC_POWER_ACTIVE1;
}


void bcs_profile_data_callback( wiced_bt_dev_vendor_specific_command_complete_params_t *pcm_params )
{
    uint8_t status = 0xFF, *p;
    uint32_t curr_timestamp = 0;
    uint32_t profile_duration = 0;
    uint32_t tx_value, rx_value, bt_idle_value;
    uint32_t fw2_active_time = 0;
    MCU_PROFILING_DATA profileData;
    uint32_t bt_time_during_deepsleep = 0;

    /* Check status of command complete event */
    p = pcm_params->p_param_buf;
    STREAM_TO_UINT8( status, p );

    if ( status != 0 )
    {
        WPRINT_LIB_ERROR( ("bcs_profile_data_callback: Status: %x\n", status) );
        return;
    }

    curr_timestamp = platform_wpl_get_time_stamp( );

    // Retrieve MCU Profile data
    mcu_profiling_retrieve_data( &profileData );

    /* Calculat profiling duration based on the stored timestamps */
    profile_duration = curr_timestamp - prev_bt_profile_timestamp;

    // Reset pre timestamp
    prev_bt_profile_timestamp = curr_timestamp;

    // Update Idle time
    WICED_POWER_LOGGER_DATA( EVENT_PROC_ID_MCU, EVENT_ID_POWERSTATE, EVENT_DESC_POWER_ACTIVE1, profileData.idleTime );

    // Get Active Time
    fw2_active_time = profile_duration - profileData.idleTime - profileData.sleepTime;

    WICED_POWER_LOGGER_DATA( EVENT_PROC_ID_MCU, EVENT_ID_POWERSTATE, EVENT_DESC_POWER_ACTIVE2, fw2_active_time );

    WICED_POWER_LOGGER_DATA( EVENT_PROC_ID_MCU, EVENT_ID_POWERSTATE, EVENT_DESC_POWER_PDS, profileData.sleepTime );

    WPRINT_LIB_DEBUG(("FW2 Info: Duration:%d Sleep:%d Idle:%d , Active:%d CPU:%d  \n", (int)profile_duration, (int)profileData.sleepTime, (int)profileData.idleTime , (int)fw2_active_time, (int)((fw2_active_time*100)/profile_duration)));


    // Update BT profiling

    STREAM_TO_UINT32( tx_value, p ); // Get Tx
    STREAM_TO_UINT32( rx_value, p ); // Get Rx

    if(profileData.sleepTime )
    {
        WICED_POWER_LOGGER_DATA( EVENT_PROC_ID_BT, EVENT_ID_BT_DATA, EVENT_DESC_BT_POWER_TX_PDS, tx_value );
        WICED_POWER_LOGGER_DATA( EVENT_PROC_ID_BT, EVENT_ID_BT_DATA, EVENT_DESC_BT_POWER_RX_PDS, rx_value );
    }
    else
    {
        WICED_POWER_LOGGER_DATA( EVENT_PROC_ID_BT, EVENT_ID_BT_DATA, EVENT_DESC_BT_POWER_TX, tx_value );
        WICED_POWER_LOGGER_DATA( EVENT_PROC_ID_BT, EVENT_ID_BT_DATA, EVENT_DESC_BT_POWER_RX, rx_value );
    }

    if ( time_spent_in_deepsleep )
    {
        /* Estimate the time spent in BT during deep sleep based on the time spent just before deep sleep */
        bt_time_during_deepsleep = ( bt_duration_percentage_before_deep_sleep_poll * time_spent_in_deepsleep ) / (100*1000);
        WICED_POWER_LOGGER_DATA( EVENT_PROC_ID_MCU, EVENT_ID_POWERSTATE, EVENT_DESC_POWER_DEEPSLEEP, time_spent_in_deepsleep );
        WICED_POWER_LOGGER_DATA( EVENT_PROC_ID_BT, EVENT_ID_BT_DATA, EVENT_DESC_BT_POWER_DEEP_SLEEP, bt_time_during_deepsleep );
    }
    else
    {
        WICED_POWER_LOGGER_DATA( EVENT_PROC_ID_MCU, EVENT_ID_POWERSTATE, EVENT_DESC_POWER_DEEPSLEEP, 0 );
        WICED_POWER_LOGGER_DATA( EVENT_PROC_ID_BT, EVENT_ID_BT_DATA, EVENT_DESC_BT_POWER_DEEP_SLEEP, 0 );
    }

    bt_idle_value = (profile_duration + time_spent_in_deepsleep) - (tx_value + rx_value + bt_time_during_deepsleep);

    /* For BT we are interested only in Tx and Rx, take rest as idle */
    WICED_POWER_LOGGER_DATA( EVENT_PROC_ID_BT, EVENT_ID_BT_DATA, EVENT_DESC_BT_POWER_IDLE, bt_idle_value );

    time_spent_in_deepsleep = 0;
    /* Keep track of the time spent in BT during the poll interval just before deep sleep.
     * We are dealing with very small time intervals, keep percentage*1000
     * */
    bt_duration_percentage_before_deep_sleep_poll = ( (tx_value + rx_value) * 100 * 1000) /  profile_duration;
}


void platform_wpl_update_bt_power_data( void )
{
    wiced_bt_dev_status_t status;

    status = wiced_bt_dev_vendor_specific_command( BTHCI_CMD_VS_OCF_GET_CONTROLLER_ACTIVITY_ENERGY_INFO, 0, NULL, bcs_profile_data_callback );
    if ( status != WICED_SUCCESS )
    {
        WPRINT_LIB_ERROR( ("platform_wpl_update_bt_power_data: Not able to retrieve BLE information from VSC\n") );
    }
}

wiced_result_t platform_wpl_init(void)
{
    return WICED_SUCCESS;
}

WICED_DEEP_SLEEP_EVENT_HANDLER( wpl_deep_sleep_event_handler )
{
    if ( wpl_logging_status( ) == WICED_FALSE )
        return;

    if ( event == WICED_DEEP_SLEEP_EVENT_ENTER )
    {
//        wpl_send_pad_cmd( CMD_LOG_REQUEST );
//        wpl_set_deep_sleep_state( WICED_TRUE );
        deep_sleep_enter_timestamp =  platform_wpl_get_time_stamp( );
    }
    else if ( event == WICED_DEEP_SLEEP_EVENT_CANCEL )
    {
        wpl_set_deep_sleep_state( WICED_FALSE );
        time_spent_in_deepsleep = 0;
        deep_sleep_enter_timestamp = 0;
    }
    else if ( event == WICED_DEEP_SLEEP_EVENT_LEAVE )
    {
        time_spent_in_deepsleep = platform_wpl_get_time_stamp() - deep_sleep_enter_timestamp;
        deep_sleep_enter_timestamp = 0;
    }
}

/* Callback to reset the profiling information in BT FW */
void bcs_profile_data_reset_callback( wiced_bt_dev_vendor_specific_command_complete_params_t *pcm_params )
{
    MCU_PROFILING_DATA profileData;

    /* Retrieve the MCU profile to reset in FW2 */
    mcu_profiling_retrieve_data(&profileData);
    prev_bt_profile_timestamp = platform_wpl_get_time_stamp( );
}

void platform_wpl_enable_logging( wiced_bool_t enable )
{
    wiced_bt_dev_status_t status;
    /* Reset BT profiling information and UART logging */
    if ( enable )
    {
        mcu_profiling_set(WICED_TRUE);
        status = wiced_bt_dev_vendor_specific_command( BTHCI_CMD_VS_OCF_GET_CONTROLLER_ACTIVITY_ENERGY_INFO, 0, NULL, bcs_profile_data_reset_callback );
        if ( status != WICED_SUCCESS )
        {
            WPRINT_LIB_ERROR( ("platform_wpl_update_bt_power_data: Not able to retrieve BLE information from VSC\n") );
        }
        // Reset UART event
        WICED_POWER_LOGGER_DATA( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_IDLE, 0 );
        // Start Logging
        WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_IDLE );

    }
    else
    {
        prev_bt_profile_timestamp = 0;
        mcu_profiling_set(WICED_FALSE);
    }
}

void platform_wpl_pre_deep_sleep( void )
{
    /* Give WPL chance to send power logger data to host before going into deep sleep */
    wpl_send_power_data_to_host();
    wpl_set_deep_sleep_state( WICED_TRUE );
}

platform_result_t platform_wpl_pre_sleep( void )
{
    /* During PDS mode, device will miss the log request messages from host. Give WPL a chance to send the
     * power log data to host.
     */
    if ( wpl_send_power_data_to_host() == WICED_SUCCESS )
        return PLATFORM_SUCCESS;
    return PLATFORM_ERROR;
}

void platform_wpl_reset_power_data(cpl_procid_t processor_id)
{
    if(processor_id == EVENT_PROC_ID_WIFI)
    {
        wpl_wifi_reset_power_data();
    }
    return;
}

void platform_wpl_update_power_data(cpl_procid_t processor_id)
{
    if(processor_id == EVENT_PROC_ID_BT)
    {
        platform_wpl_update_bt_power_data();
    }
    else if(processor_id == EVENT_PROC_ID_WIFI)
    {
        wpl_wifi_update_power_data();
    }
    return;
}
