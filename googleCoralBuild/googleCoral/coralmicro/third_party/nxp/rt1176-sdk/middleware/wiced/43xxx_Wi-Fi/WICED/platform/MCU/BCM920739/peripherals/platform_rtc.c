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
 * BCM4774 RTC implementation
 */
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "wwd_assert.h"
#include "platform_constants.h"
#include "platform_peripheral.h"
#include "wwd_rtos.h"
#include "rtc.h"
/******************************************************
 *                      Macros
 ******************************************************/
#define PLATFORM_EPOCH_RTC_YEAR     (2010)
/* There is no specific comments from the wiced interface on the epoch time,
 * it seems to assume epoch as 2000(from some app example code), BT HAL epoch is at 2010,
 * so we limit the platform epoch at 2010 */
#define PLATFORM_UTC_EPOCH_DIFF     (2010 - 2000)
#define NUM_SECONDS_IN_MINUTE       (60)
#define NUM_MINUTES_IN_HOUR         (60)
#define NUM_HOURS_IN_DAY            (24)
#define NUM_DAYS_IN_MONTH           (31)
#define NUM_MONTHS_IN_YEAR          (12)
#define NUM_YEARS_IN_DCENTURY       (200)

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/
static wiced_bool_t validate_input_time_calendar( const platform_rtc_time_t* time );
/******************************************************
 *               Variable Definitions
 ******************************************************/
static uint16_t prtc_init = WICED_FALSE;
/******************************************************
 *               Function Definitions
 ******************************************************/
static wiced_bool_t validate_input_time_calendar( const platform_rtc_time_t* time )
{
    wiced_bool_t valid_time_calendar = WICED_FALSE;

    if (time->year < NUM_YEARS_IN_DCENTURY && time->year >= PLATFORM_UTC_EPOCH_DIFF)
    {
        if ( time->sec < NUM_SECONDS_IN_MINUTE )
        {
            if ( time->min < NUM_MINUTES_IN_HOUR )
            {
                if ( time->hr < NUM_HOURS_IN_DAY )
                {
                    if ( (time->date > 0) && (time->date <= NUM_DAYS_IN_MONTH) )
                    {
                        if ( (time->month > 0) && (time->month <= NUM_MONTHS_IN_YEAR) )
                        {
                                        valid_time_calendar = WICED_TRUE;
                        }
                    }
                }
            }
        }
    }
    return valid_time_calendar;
}

/*
 * Initialize the RTC
 */
platform_result_t platform_rtc_init(void)
{
    if(prtc_init == WICED_FALSE)
    {
       rtc_init();
       prtc_init = WICED_TRUE;
    }

    return PLATFORM_SUCCESS;
}

/* Sets the RTC time to platform registers
 * platfrom_rtc_time_t limits the year to uint8_t type
 * hence we limit the year to 200.
 * Note : platfrom_rtc_time limits the year to uint8_t, so we represent year in
 * two digit like 16 for 2016(assuming epoch at 2000)
 */
platform_result_t platform_rtc_set_time( const platform_rtc_time_t* time )
{
    RtcTime rtime;

    wiced_assert( "bad argument", ( time != NULL ) );

    platform_rtc_init();

    /* Validate the input time */
    if ( validate_input_time_calendar(time) != WICED_TRUE )
    {
        return PLATFORM_ERROR;
    }
    /*Weekday will be auto populated from mktime()*/
    rtime.year = (uint16_t)time->year + PLATFORM_EPOCH_RTC_YEAR;
    rtime.month = (uint16_t)time->month;    /*0-Jan..11-Dec*/
    rtime.day = (uint16_t)time->date;
    rtime.hour = (uint16_t)time->hr;
    rtime.minute = (uint16_t)time->min;
    rtime.second = (uint16_t)time->sec;

    if(!rtc_setRTCTime(&rtime))
        return PLATFORM_ERROR;

    return PLATFORM_SUCCESS;
}
/*
 * Get platform RTC time.
 * In order to get correct time, RTC must be set at least once
 * before calling get.
 * */
platform_result_t platform_rtc_get_time( platform_rtc_time_t* time )
{
    RtcTime rtime;
    struct tm *cal_time;
    time_t t_of_day;
    uint32_t rtc_time;

    wiced_assert( "bad argument", ( time != NULL ) );

    platform_rtc_init();

    rtc_getRTCTime(&rtime);

    time->year  = (uint8_t)rtime.year - PLATFORM_EPOCH_RTC_YEAR;
    time->month = (uint8_t)rtime.month;    /*0-Jan..11-Dec*/
    time->date  = (uint8_t)rtime.day;
    time->hr    = (uint8_t)rtime.hour;
    time->min   = (uint8_t)rtime.minute;
    time->sec   = (uint8_t)rtime.second;
    /*weekday calculation*/
    /*get rtc time in seconds*/
    rtc_RtcTime2Sec(&rtime, &rtc_time);
    t_of_day = (time_t)rtc_time;
    /*get calendar time with weekday */
    cal_time = localtime(&t_of_day);
    if ( cal_time == NULL )
    {
        return PLATFORM_PARTIAL_RESULTS;
    }
    time->weekday = cal_time->tm_wday;  /*0-Sunday...*/

    return PLATFORM_SUCCESS;
}

uint32_t host_platform_get_cycle_count(void)
{
    /* This function is supposed to return CPU Cycle Count
     * Reading "0xE0001004" DWT_CYCCNT from ARM, value doesn't seem
     * to change and hence few functions such as random number generators
     * which use this API to feed in the seed based on this functions return
     * value doesnt change. In order to generate atleast different numbers
     * apart from 0, using the RTC clock tick value.
     */
    tRTC_REAL_TIME_CLOCK rtc_clock;
    rtc_getRTCRawClock(&rtc_clock);
    return rtc_clock.reg32map.rtc32[0];
}

