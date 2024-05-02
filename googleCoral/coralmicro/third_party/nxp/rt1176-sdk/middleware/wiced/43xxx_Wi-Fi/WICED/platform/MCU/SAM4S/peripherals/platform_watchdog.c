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

#include "platform_peripheral.h"
#include "wiced_defaults.h"
#include "wdt.h"
#include "sam4s.h"

#ifndef WICED_DISABLE_WATCHDOG

/******************************************************
 *                      Macros
 ******************************************************/

#define WATCHDOG_SEC_TO_USEC(sec)    ( sec * 1000000 )
#define RSTC_RESET_TYPE(SR)          (( SR & RSTC_SR_RSTTYP_Msk ) >> RSTC_SR_RSTTYP_Pos )

/******************************************************
 *                    Constants
 ******************************************************/

#define WDT_SCLK_HZ             ( 32768 )   /* SAM4S Slow Clk rate in Hz */

#ifdef APPLICATION_WATCHDOG_TIMEOUT_SECONDS
#if APPLICATION_WATCHDOG_TIMEOUT_SECONDS > 16   /* SAM4S supports Wdog timeout upto 16 secs */
#undef APPLICATION_WATCHDOG_TIMEOUT_SECONDS
#define APPLICATION_WATCHDOG_TIMEOUT_SECONDS    ( 16 )
#warning APPLICATION_WATCHDOG_TIMEOUT_SECONDS reduced to 16 seconds
#endif
#define WATCHDOG_TIMEOUT                ( APPLICATION_WATCHDOG_TIMEOUT_SECONDS )
#else
#define WATCHDOG_TIMEOUT                ( MAX_WATCHDOG_TIMEOUT_SECONDS )
#endif /* APPLICATION_WATCHDOG_TIMEOUT_SECONDS */

/* RSTC Reset types */
#define RSTC_RESET_TYPE_GEN    (0x0)
#define RSTC_RESET_TYPE_BKP    (0x1)
#define RSTC_RESET_TYPE_WDT    (0x2)
#define RSTC_RESET_TYPE_SOFT   (0x3)
#define RSTC_RESET_TYPE_USR    (0x4)

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

/******************************************************
 *             WICED Function Definitions
 ******************************************************/

/******************************************************
 *             SAM4S Function Definitions
 ******************************************************/

platform_result_t platform_watchdog_init( void )
{
    uint32_t counter_value;

    counter_value = wdt_get_timeout_value( WATCHDOG_SEC_TO_USEC( WATCHDOG_TIMEOUT ), WDT_SCLK_HZ );
    if ( counter_value != WDT_INVALID_ARGUMENT )
    {
        /* Cause watchdog to generate both an interrupt and reset at underflow */
        wdt_init( WDT, WDT_MR_WDFIEN | WDT_MR_WDRSTEN, counter_value, counter_value );
    }
    return PLATFORM_SUCCESS;
}

platform_result_t platform_watchdog_kick( void )
{
    wdt_restart( WDT );
    return PLATFORM_SUCCESS;
}

wiced_bool_t platform_watchdog_check_last_reset( void )
{
    if ( RSTC_RESET_TYPE( RSTC->RSTC_SR ) == RSTC_RESET_TYPE_WDT )
    {
        return WICED_TRUE;
    }

    return WICED_FALSE;
}

#else
platform_result_t platform_watchdog_init( void )
{
    wdt_disable( WDT );
    return WICED_SUCCESS;
}

platform_result_t platform_watchdog_kick( void )
{
    return WICED_SUCCESS;
}

wiced_bool_t platform_watchdog_check_last_reset( void )
{
    return WICED_FALSE;
}
#endif /* ifndef WICED_DISABLE_WATCHDOG */
