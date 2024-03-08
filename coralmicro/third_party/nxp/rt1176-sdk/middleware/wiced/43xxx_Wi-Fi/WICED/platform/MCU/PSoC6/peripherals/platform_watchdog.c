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
 * PSoC 6 platform watchdog driver
 *
 */

#include "platform_cmsis.h"
#include "platform_constants.h"
#include "platform_peripheral.h"
#include "platform_stdio.h"
#include "platform_isr.h"
#include "platform_isr_interface.h"
#include "platform_assert.h"
#include "wwd_assert.h"
#include "wwd_rtos.h"
#include "wwd_rtos_isr.h"
#include "wiced_defaults.h"
#include "platform_config.h" /* For CPU_CLOCK_HZ */


#ifndef USE_MCWDT       /* Use Regular WDT logic */

/******************************************************
 *                      Macros
 ******************************************************/
/* Seems the Largest value that can be set as
 * Counter Ignore above 12 bits is Not allowed */

#define WD_MATCH_COUNT       (0xffff)

/* Number of interrupts to clear to extend the WatchDog
 * Timeout. After clearing interrupt for this count
 * let the WatchDog run without clearing and either kick or WD
 * will occur */

#define NUM_INT_TO_CLEAR     (3)

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

#ifdef        ENABLE_SW_WD_EXT
/* Counter to extend the WatchDog timer to beyond its natural maximum value */

static volatile int32_t        watchdog_int_count_down;
#endif        /* ENABLE_SW_WD_EXT */

/******************************************************
 *               Function Definitions
 ******************************************************/

/* Enables s/w interrupt based extending of WatchDog timeout * not recommended */
// #define        ENABLE_SW_WD_EXT        (1)

platform_result_t platform_watchdog_init( void )
{
#ifndef WICED_DISABLE_WATCHDOG
    Cy_WDT_Init();                              /* Sets up Default Watch Dog Counter Value */

    Cy_WDT_SetMatch( WD_MATCH_COUNT );          /* Set Match Count */

#ifdef        ENABLE_SW_WD_EXT

    Cy_WDT_ClearInterrupt();                    /* Clear Pending interrupt as well for safety */

    Cy_WDT_MaskInterrupt();

    NVIC_EnableIRQ( srss_interrupt_IRQn );      /* Enable interrupt on Interrupt controller */

    watchdog_int_count_down = NUM_INT_TO_CLEAR; /* Extend Watchdog timeout using s/w */

    Cy_WDT_UnmaskInterrupt();                   /* Unmask Watchdog interrupt */

#endif        /* ENABLE_SW_WD_EXT */

    Cy_WDT_Enable();

    Cy_WDT_Lock();                              /* Lock the WD Counter register */


    return PLATFORM_SUCCESS;
#else   /* WICED_DISABLE_WATCHDOG */
    return PLATFORM_FEATURE_DISABLED;
#endif  /* WICED_DISABLE_WATCHDOG */
}

platform_result_t platform_watchdog_kick( void )
{
#ifndef WICED_DISABLE_WATCHDOG

#ifdef        ENABLE_SW_WD_EXT
    watchdog_int_count_down = NUM_INT_TO_CLEAR;
#endif        /* ENABLE_SW_WD_EXT */

    Cy_WDT_ClearWatchdog();                    /* The Stroke function for watch dog */

    return PLATFORM_SUCCESS;
#else   /* WICED_DISABLE_WATCHDOG */
    return PLATFORM_FEATURE_DISABLED;
#endif  /* WICED_DISABLE_WATCHDOG */
}

wiced_bool_t platform_watchdog_check_last_reset( void )
{
#ifndef WICED_DISABLE_WATCHDOG
    uint32_t reset_reason;

    reset_reason = Cy_SysLib_GetResetReason(); /* Return the Reset Cause */

    if ( reset_reason & CY_SYSLIB_RESET_HWWDT )
    {
       return WICED_TRUE;
    }
#endif  /* WICED_DISABLE_WATCHDOG */

    return WICED_FALSE;
}

#ifndef WICED_DISABLE_WATCHDOG

#ifdef        ENABLE_SW_WD_EXT

WWD_RTOS_DEFINE_ISR( platform_wdt_irq )
{
    if ( watchdog_int_count_down-- > 0 )
    {
        Cy_WDT_ClearInterrupt();
    }
}

WWD_RTOS_MAP_ISR( platform_wdt_irq , srss_interrupt_IRQn_Handler )

#endif        /* ENABLE_SW_WD_EXT */

#endif  /* WICED_DISABLE_WATCHDOG */

#else /* USE_MCWDT */

/* Using MCWDT0 for Watchdog behavior */

#include "cy_mcwdt.h"

#define PSOC_MCWDT_ENABLE_DELAY     (20)   /* 20 usec delay in setting enable mode is recommended */
#define PSOC_MCWDT_RESET_CNTR_DELAY (62)   /* 62 usec delay in setting enable mode is recommended */

/* Counters set to get 6 seconds before WDT */
#define C0_MATCH_CNT                (0x7fff)   /* Counter 0 match count - 1 per second */
#define C1_MATCH_CNT                (29)       /* Counter 1 match count - 29 seconds   */

platform_result_t platform_watchdog_init( void )
{
#ifndef WICED_DISABLE_WATCHDOG

    cy_stc_mcwdt_config_t mcwdt_cfg = {'\0'};

    Cy_MCWDT_Unlock( MCWDT_STRUCT0 );              /* Unlock the WD Counter register */

    mcwdt_cfg.c0Match = C0_MATCH_CNT;
    mcwdt_cfg.c1Match = C1_MATCH_CNT;
    mcwdt_cfg.c0Mode  = CY_MCWDT_MODE_NONE;
    mcwdt_cfg.c1Mode  = CY_MCWDT_MODE_RESET;
    mcwdt_cfg.c2Mode  = CY_MCWDT_MODE_NONE;
    mcwdt_cfg.c0ClearOnMatch = 1;
    mcwdt_cfg.c1ClearOnMatch = 1;
    mcwdt_cfg.c0c1Cascade = 1;
    mcwdt_cfg.c1c2Cascade = 0;

    Cy_MCWDT_Init( MCWDT_STRUCT0, &mcwdt_cfg );    /* Sets up Default Watch Dog Counter Value */

    Cy_MCWDT_Enable( MCWDT_STRUCT0, (CY_MCWDT_CTR0 | CY_MCWDT_CTR1), PSOC_MCWDT_ENABLE_DELAY );

    Cy_MCWDT_Lock( MCWDT_STRUCT0 );                /* Lock the WD Counter register */

    return PLATFORM_SUCCESS;
#else
    return PLATFORM_FEATURE_DISABLED;
#endif
}


platform_result_t platform_watchdog_kick( void )
{
#ifndef WICED_DISABLE_WATCHDOG

    Cy_MCWDT_Unlock(MCWDT_STRUCT0);     /* Unlock the WD Counter register */
    Cy_MCWDT_ResetCounters(MCWDT_STRUCT0, (CY_MCWDT_CTR0 | CY_MCWDT_CTR1), PSOC_MCWDT_RESET_CNTR_DELAY);
    Cy_MCWDT_Lock(MCWDT_STRUCT0);       /* Lock the WD Counter register */

    return PLATFORM_SUCCESS;
#else
    return PLATFORM_FEATURE_DISABLED;
#endif
}

wiced_bool_t platform_watchdog_check_last_reset( void )
{
#ifndef WICED_DISABLE_WATCHDOG
    uint32_t reset_reason;

    reset_reason = Cy_SysLib_GetResetReason(); /* Return the Reset Cause */

    if ( reset_reason & CY_SYSLIB_RESET_SWWDT0 ) /* MCWDT0 Reset Cause */
    {
       return WICED_TRUE;
    }
#endif

    return WICED_FALSE;
}

#endif /* USE_MCWDT */
