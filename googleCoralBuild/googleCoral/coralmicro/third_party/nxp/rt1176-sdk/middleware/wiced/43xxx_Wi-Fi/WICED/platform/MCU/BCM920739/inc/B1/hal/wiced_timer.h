/*
 * Copyright 2014, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */
 /** @file
 *
 *Implements the interfaces for Timer services
 */

#ifndef _WICED_TIMER_H_
#define _WICED_TIMER_H_

#include "wiced_bt_app.h"

/*
* Defines the wiced timer types
*/
enum wiced_timer_type_e
{
    WICED_SECONDS_TIMER = 1,
    WICED_MILLI_SECONDS_TIMER, /* The minimum resolution supported is 1 ms */
    WICED_SECONDS_PERIODIC_TIMER,
    WICED_MILLI_SECONDS_PERIODIC_TIMER /*The minimum resolution supported is 1 ms */
};
typedef UINT8 wiced_timer_type_t;/* (see #wiced_timer_type_e) */

/* Function pointer for the timer call backs.
*/
typedef void (*wiced_timer_callback_fp)(uint32_t cBackparam);

/**
 * Defines the  wiced timer instance size
 */
 #define WICED_TIMER_INSTANCE_SIZE_IN_WORDS      17

/*
 * Defines the wiced timer structure.
 */
typedef struct
{
    uint32_t reserved[WICED_TIMER_INSTANCE_SIZE_IN_WORDS];
}wiced_timer_t;

/* Convert from ms to us*/
#define QUICK_TIMER_MS_TO_US(tout)    tout*1000

/**  Initializes the timer
 *
 *@param[in]    p_timer             :Pointer to the timer structure
 *@param[in]    p_cb                 :Timer callback function to be invoked on timer expiry
 *@param[in]    cb_param        :Parameter to be passed to the timer callback function which
 *                                              gets invoked on timer expiry,if any
 *@param[in]    timer_type         :Type of the timer
 *
 * @return   wiced_result_t
 */
wiced_result_t wiced_init_timer( wiced_timer_t* p_timer, wiced_timer_callback_fp TimerCb,
                                 UINT32 cBackparam, wiced_timer_type_t type);

/** Starts the timer
 *
 * @param[in]    wiced_timer_t           ::Pointer to the timer structure
 *
 * @return       wiced_result_t
 */

wiced_result_t wiced_start_timer( wiced_timer_t* p_timer,uint32_t timeout );

/** Stops the timer
 *
 * @param[in]    wiced_timer_t           :Pointer to the timer structure
 *
 * @return       wiced_result_t
 */

wiced_result_t wiced_stop_timer( wiced_timer_t* p_timer );

/**  Checks if the timer is in use
*
*@param[in]    p_timer             :Pointer to the timer structure
*
* @return   TRUE if the timer is in use and FALSE if the timer is not in use
*/
wiced_bool_t wiced_is_timer_in_use(wiced_timer_t* p_timer);

#endif // _WICED_TIMER_H_
