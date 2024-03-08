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

#include "typedefs.h"
#include "sbchipc.h"
#include "wpl_platform_api.h"

/******************************************************
 *                      Macros
 ******************************************************/
//PMU timer related macros for getting the timestamp
#define CYCLES_PER_TICK( clock_hz )               ( ( ( ( clock_hz ) + ( SYSTICK_FREQUENCY / 2 ) ) / SYSTICK_FREQUENCY ) )
#define CYCLES_PMU_INIT_PER_TICK                CYCLES_PER_TICK( ILP_CLOCK )

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
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

/**
 * Gets time in milliseconds
 *
 * @Note: Platform specific, need to be ported accroding to the platform.
 *
 * @Note: In the current platform time stamp is obtained from PMU timer.
 *
 * @returns Time in milliseconds
 */

uint32_t platform_wpl_get_time_stamp( void )
{
    return platform_tick_get_time( PLATFORM_TICK_GET_SLOW_TIME_STAMP ) / CYCLES_PMU_INIT_PER_TICK;
}

uint8_t platform_wpl_get_mcu_power_state_before_warm_boot( void )
{
    return (uint8_t)EVENT_DESC_POWER_DEEPSLEEP;
}

uint8_t platform_wpl_get_first_mcu_power_state( void )
{
#if PLATFORM_APPS_POWERSAVE
    return (uint8_t)EVENT_DESC_POWER_ACTIVE1;
#else
    return (uint8_t)EVENT_DESC_POWER_ACTIVE2;
#endif
}

WICED_DEEP_SLEEP_EVENT_HANDLER( wpl_deep_sleep_event_handler )
{
    if ( wpl_logging_status( ) == WICED_FALSE )
        return;

    if ( event == WICED_DEEP_SLEEP_EVENT_ENTER )
    {
        wpl_set_deep_sleep_state( WICED_TRUE );
    }
    else if ( event == WICED_DEEP_SLEEP_EVENT_CANCEL )
    {
        wpl_set_deep_sleep_state( WICED_FALSE );
    }
}

void platform_wpl_enable_logging( wiced_bool_t enable )
{
    UNUSED_PARAMETER( enable );
}

wiced_result_t platform_wpl_init(void)
{
    return WICED_SUCCESS;
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
    if(processor_id == EVENT_PROC_ID_WIFI)
    {
        wpl_wifi_update_power_data();
    }
    return;
}
