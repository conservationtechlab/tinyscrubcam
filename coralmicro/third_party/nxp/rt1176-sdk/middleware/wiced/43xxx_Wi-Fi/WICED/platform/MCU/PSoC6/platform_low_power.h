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
 *  Low power support functions.
 *
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "cy_mcwdt.h"
#include "platform_isr.h"
#include "platform_isr_interface.h"
#include "platform_peripheral.h"


/******************************************************
 *             Macros
 ******************************************************/

/******************************************************
 *             Constants
 ******************************************************/

/******************************************************
 *             Enumerations
 ******************************************************/

/******************************************************
 *             Structures
 ******************************************************/

/******************************************************
 *             Variables
 ******************************************************/

/******************************************************
 *             Function declarations
 ******************************************************/

void platform_stop_timer_tick( unsigned long* remaining_ticks );
platform_result_t platform_set_low_power_wakeup_timer( uint32_t sleep_ms );
platform_result_t platform_pdl_enter_sleep( void );
platform_result_t platform_pdl_enter_deep_sleep( void );
platform_result_t platform_enter_sleep( unsigned long time_until_next_event, unsigned long* time_spent_in_sleep );
platform_result_t platform_enter_deep_sleep( unsigned long time_until_next_event, unsigned long* time_spent_in_deep_sleep );
platform_result_t platform_stop_low_power_wakeup_timer( unsigned long *time_elapsed_ms );
void platform_start_timer_tick( void );

#ifdef __cplusplus
} /* extern "C" */
#endif
