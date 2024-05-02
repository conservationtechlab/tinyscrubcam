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
 *  PSoC 6 Platform RTC Driver
 */

#include <stdint.h>
#include <string.h>
#include "wiced_platform.h"
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_sleep.h"
#include "platform_assert.h"
#include "wwd_assert.h"
#include "wiced_rtos.h"

/******************************************************
 *                      Macros
 ******************************************************/

/* Check to identify a leap year */
#define IS_LEAP_YEAR(year) ( (((((year) % 4) == 0) && (((year) % 100) != 0)) || (((year) % 400) == 0)) ? (1) : (0) )

/*
 * NOTE:
 * The PSoC6 RTC block uses a 1 Hz counter clock.
 * This means a time counter accuracy of 1 second.
 */
#define NUM_SECONDS_IN_MINUTE       (60)
#define NUM_MINUTES_IN_HOUR         (60)
#define NUM_HOURS_IN_DAY            (24)
#define NUM_DAYS_IN_WEEK            (7)
#define NUM_MONTHS_IN_YEAR          (12)
#define NUM_YEARS_IN_CENTURY        (100)

#define RTC_CURRENT_CENTURY         (20)

/* Timeout in microseconds for the WCO to be ready */
#define WCO_READY_TIMEOUT_US        (500000)

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

/* Default RTC time. Set to 00:00:00 01/01/2000 Saturday */
static const platform_rtc_time_t default_rtc_time =
{
   .sec     = 00,
   .min     = 00,
   .hr      = 00,
   .weekday = 7,
   .date    = 1,
   .month   = 1,
   .year    = 00,
};

static const char month_days_not_leap_year[] =
{
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const char month_days_in_leap_year[] =
{
    0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

/******************************************************
 *               Function Definitions
 ******************************************************/

static wiced_bool_t validate_input_time_calendar( const platform_rtc_time_t* time )
{
    wiced_bool_t valid_time_calendar = WICED_FALSE;

    if ( time->sec < NUM_SECONDS_IN_MINUTE )
    {
        if ( time->min < NUM_MINUTES_IN_HOUR )
        {
            if ( time->hr < NUM_HOURS_IN_DAY )
            {
                if ( (time->weekday > 0) && (time->weekday <= NUM_DAYS_IN_WEEK) )
                {
                    if ( (time->month > 0) && (time->month <= NUM_MONTHS_IN_YEAR) )
                    {
                        if ( time->year < NUM_YEARS_IN_CENTURY )
                        {
                            if ( IS_LEAP_YEAR((RTC_CURRENT_CENTURY * 100) + time->year) == 1 )
                            {
                                if ( (time->date > 0) && (time->date <= month_days_in_leap_year[time->month]) )
                                {
                                    valid_time_calendar = WICED_TRUE;
                                }
                            }
                            else
                            {
                                if ( (time->date > 0) && (time->date <= month_days_not_leap_year[time->month]) )
                                {
                                    valid_time_calendar = WICED_TRUE;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return valid_time_calendar;
}

static platform_result_t platform_rtc_read_time_calendar( platform_rtc_time_t* time )
{
    cy_stc_rtc_config_t rtc_time_calendar;

    /* Read the time and calendar from RTC */
    Cy_RTC_GetDateAndTime( &rtc_time_calendar );

    time->sec     = (uint8_t)(rtc_time_calendar.sec);
    time->min     = (uint8_t)(rtc_time_calendar.min);
    time->hr      = (uint8_t)(rtc_time_calendar.hour);
    time->weekday = (uint8_t)(rtc_time_calendar.dayOfWeek);
    time->date    = (uint8_t)(rtc_time_calendar.date);
    time->month   = (uint8_t)(rtc_time_calendar.month);
    time->year    = (uint8_t)(rtc_time_calendar.year);

    return PLATFORM_SUCCESS;
}

static platform_result_t platform_rtc_write_time_calendar( const platform_rtc_time_t* time )
{
    cy_stc_rtc_config_t rtc_time_calendar;

    /* Validate the input time and calendar settings */
    if ( validate_input_time_calendar( time ) != WICED_TRUE )
    {
        return PLATFORM_ERROR;
    }

    rtc_time_calendar.sec       = (uint32_t)(time->sec);
    rtc_time_calendar.min       = (uint32_t)(time->min);
    rtc_time_calendar.hour      = (uint32_t)(time->hr);
    rtc_time_calendar.hrFormat  = CY_RTC_24_HOURS;
    rtc_time_calendar.dayOfWeek = (uint32_t)(time->weekday);
    rtc_time_calendar.date      = (uint32_t)(time->date);
    rtc_time_calendar.month     = (uint32_t)(time->month);
    rtc_time_calendar.year      = (uint32_t)(time->year);

    /* Write the time and calendar to RTC */
    if ( Cy_RTC_SetDateAndTime( &rtc_time_calendar ) != CY_RTC_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}

platform_result_t platform_rtc_get_time( platform_rtc_time_t* time )
{
    if ( time == NULL )
    {
        wiced_assert( "bad argument", 0 );
        return PLATFORM_BADARG;
    }

    return platform_rtc_read_time_calendar( time );
}

platform_result_t platform_rtc_set_time( const platform_rtc_time_t* time )
{
    if ( time == NULL )
    {
        wiced_assert( "bad argument", 0 );
        return PLATFORM_BADARG;
    }

    return platform_rtc_write_time_calendar( time );
}

platform_result_t platform_rtc_init( void )
{
    GPIO_PRT_Type *port_base;
    cy_stc_rtc_config_t default_rtc_config;

    /*
     * The RTC is located in the backup domain and primarily runs from a 32768-Hz clock.
     * The WCO needs to generate the backup domain clock for the RTC clock operation.
     */

    /* The WCO pins (SRSS_WCO_IN_PIN, SRSS_WCO_OUT_PIN) are supplied with an external crystal. */
    port_base = Cy_GPIO_PortToAddr( SRSS_WCO_IN_PORT );
    Cy_GPIO_Pin_FastInit( port_base, SRSS_WCO_IN_PIN, CY_GPIO_DM_ANALOG, 0x0, P0_0_GPIO );
    port_base = Cy_GPIO_PortToAddr( SRSS_WCO_OUT_PORT );
    Cy_GPIO_Pin_FastInit( port_base, SRSS_WCO_OUT_PIN, CY_GPIO_DM_ANALOG, 0x0, P0_1_GPIO );

    /* Clear the WCO bypassed mode - WCO operates with an external 32.768KHz crystal. */
    Cy_SysClk_WcoBypass( CY_SYSCLK_WCO_NOT_BYPASSED );

    /* Configure the RTC clock frequency pre-scaler value for the 32.768KHz WCO. */
    Cy_RTC_SelectFrequencyPrescaler( CY_RTC_FREQ_WCO_32768_HZ );

    /* Enable the WCO with a stabilization timeout of 500 milliseconds. */
    if ( Cy_SysClk_WcoEnable( WCO_READY_TIMEOUT_US ) != CY_SYSCLK_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    /* Set the backup domain clock source to WCO */
    Cy_SysClk_ClkBakSetSource( CY_SYSCLK_BAK_IN_WCO );

    /* Make sure the backup domain clock source is WCO */
    if ( Cy_SysClk_ClkBakGetSource() != CY_SYSCLK_BAK_IN_WCO )
    {
        return PLATFORM_ERROR;
    }

    /* Default RTC configuration */
    default_rtc_config.sec       = (uint32_t)(default_rtc_time.sec);
    default_rtc_config.min       = (uint32_t)(default_rtc_time.min);
    default_rtc_config.hour      = (uint32_t)(default_rtc_time.hr);
    default_rtc_config.hrFormat  = CY_RTC_24_HOURS;
    default_rtc_config.dayOfWeek = (uint32_t)(default_rtc_time.weekday);
    default_rtc_config.date      = (uint32_t)(default_rtc_time.date);
    default_rtc_config.month     = (uint32_t)(default_rtc_time.month);
    default_rtc_config.year      = (uint32_t)(default_rtc_time.year);

    /* Initialize and start RTC */
    if ( Cy_RTC_Init( &default_rtc_config ) != CY_RTC_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}
