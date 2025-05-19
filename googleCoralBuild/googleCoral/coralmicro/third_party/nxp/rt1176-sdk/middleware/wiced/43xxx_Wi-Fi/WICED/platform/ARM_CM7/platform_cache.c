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
 * Defines functions to manipulate caches
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "platform_cache.h"
#include "platform_peripheral.h"

/******************************************************
 *                      Macros
 ******************************************************/

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
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

#ifdef PLATFORM_L1_CACHE_SHIFT

void platform_icache_inv_all( void )
{
    SCB_InvalidateICache( );
}

void platform_dcache_inv_range( volatile void *p, uint32_t length )
{
    SCB_InvalidateDCache_by_Addr((uint32_t *)(PLATFORM_L1_CACHE_ROUND_DOWN( (uint32_t )p)), (int32_t)PLATFORM_L1_CACHE_ROUND_UP( length ));
}

void platform_dcache_clean_range( const volatile void *p, uint32_t length )
{
#ifndef WICED_DCACHE_WTHROUGH

    SCB_CleanDCache_by_Addr((uint32_t *)(PLATFORM_L1_CACHE_ROUND_DOWN( (uint32_t )p)), (int32_t)PLATFORM_L1_CACHE_ROUND_UP( length ));

#else

    UNUSED_PARAMETER(p);
    UNUSED_PARAMETER(length);

    __asm__ __volatile__ ("DSB");

#endif /* WICED_DCACHE_WTHROUGH */
}

void platform_dcache_clean_and_inv_range( const volatile void *p, uint32_t length)
{
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *)(PLATFORM_L1_CACHE_ROUND_DOWN( (uint32_t )p)), (int32_t)PLATFORM_L1_CACHE_ROUND_UP( length ));
}

#endif /* PLATFORM_L1_CACHE_SHIFT */
