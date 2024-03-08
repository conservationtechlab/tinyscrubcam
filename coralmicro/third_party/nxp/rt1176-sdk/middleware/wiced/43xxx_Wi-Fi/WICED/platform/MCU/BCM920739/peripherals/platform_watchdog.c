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

#include <stdint.h>
#include <string.h>
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_sleep.h"
#include "platform_assert.h"
#include "wwd_assert.h"
#include "platform_mcu_peripheral.h"
#include "wiced_hal_wdog.h"
#include "wwd_debug.h"
#include "wiced_rtos.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define WATCH_DOG_KICK_IN_TIME_MS           1000


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
 *               Static Function Declarations
 ******************************************************/
/******************************************************
 *               Variable Definitions
 ******************************************************/
static wiced_timer_t       watchdog_timer;


/******************************************************
 *               Function Definitions
 ******************************************************/

static void watchdog_kick_exp_cb(void* val)
{
    platform_watchdog_kick();
}

static platform_result_t start_watchdog_kick_timer(void)
{
    wiced_result_t result;
    result =  wiced_rtos_init_timer( &watchdog_timer, WATCH_DOG_KICK_IN_TIME_MS, (timer_handler_t)watchdog_kick_exp_cb, NULL);
    if (result != WICED_SUCCESS )
    {
        //WICED_BT_TRACE("timer Create Failed\n");
        return PLATFORM_ERROR;
    }

    result =  wiced_rtos_start_timer( &watchdog_timer );
    if (result != WICED_SUCCESS )
    {
        //WICED_BT_TRACE("timer start Failed\n");
        return PLATFORM_ERROR;
    }
    return PLATFORM_SUCCESS;
}

/*
* Disable the system watchdog. Useful for debugging when the watchdog would
* interfere and reset the system when not desired (e.g. when the system
* is connected to a debugger, etc).
*
* NOTE: This is for debugging purposes only. Hence, there is no complementary
* "Enable Watchdog" function. Please do *not* use this function in production
* applications.
*
* \param none
*
* \return none
*/
void platform_watchdog_disable(void)
{
    wiced_hal_wdog_disable();

    // Stop Watchdog timer
    wiced_rtos_stop_timer( &watchdog_timer );
}

/*
* Execute a soft reset of the system.
*
* \param none
*
* \return none
*/
void platform_watchdog_reset_system(void)
{
    wiced_hal_wdog_reset_system();
    while(1);
}

/*
 * platform watchdog initialization if any
 */
platform_result_t platform_watchdog_init( void )
{
    return start_watchdog_kick_timer();
}

/*Restart the watchdog (restart the watchdog's internal timer). Used to
 *  manually "pet" the watchdog when certain processes might otherwise cause
 *  the watchdog to trigger and reset the system.
 */
platform_result_t platform_watchdog_kick( void )
{
    wiced_hal_wdog_restart();
    return PLATFORM_SUCCESS;
}

/*Checks if last system reset was due to watchdog*/
wiced_bool_t platform_watchdog_check_last_reset( void )
{
    return WICED_FALSE;
}
