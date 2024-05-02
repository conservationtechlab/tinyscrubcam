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
 *  Deep-sleep support functions.
 *
 */

#pragma once

#include <stdint.h>
#include "platform_config.h"
#include "platform_mcu_peripheral.h"
#include "spar_utils.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *             Macros
 ******************************************************/

#ifndef PLATFORM_LOW_POWER_HEADER_INCLUDED
#error "Platform header file must not be included directly, Please use wiced_deep_sleep.h instead."
#endif

/* Include these MACRO definition only when deep sleep supported.
 * Else default definition would be picked from wiced_low_power.h
*/

#if !defined(WICED_DISABLE_MCU_POWERSAVE) && defined(PLATFORM_SUPPORTS_LOW_POWER_MODES)

#define WICED_DEEP_SLEEP_IS_WARMBOOT( ) \
    platform_mcu_powersave_is_warmboot( )

#define WICED_DEEP_SLEEP_IS_ENABLED( )                           1

/* Disabling WICED_DEEP_SLEEP_SAVED_VAR for 20739MCU. */
#if 0
#define WICED_DEEP_SLEEP_SAVED_VAR( var ) \
        PLACE_DATA_IN_RETENTION_RAM var
#endif

/* This is a 20739MCU specific macro to save variable into AON */
#define WICED_SHUTDOWN_SLEEP_SAVED_VAR( var ) \
        PLACE_DATA_IN_RETENTION_RAM var

#endif

#ifndef WICED_SHUTDOWN_SLEEP_SAVED_VAR
#define WICED_SHUTDOWN_SLEEP_SAVED_VAR( var )                                               var
#endif

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

#ifdef __cplusplus
} /* extern "C" */
#endif
