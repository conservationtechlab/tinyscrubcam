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
 * PSoC 6 MCU powersave implementation
 */
#include <stdint.h>
#include <string.h>
#include "platform_cmsis.h"
#include "platform_init.h"
#include "platform_constants.h"
#include "platform_assert.h"
#include "platform_peripheral.h"
#include "platform_isr_interface.h"
#include "platform_sleep.h"
#include "wwd_rtos_isr.h"
#include "wiced_defaults.h"
#include "wwd_rtos.h"
#include "wiced_low_power.h"
#include "platform_config.h"
#include "wiced_platform.h"
#include "wwd_assert.h"
#include "wiced_time.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define CYCLES_PER_SYSTICK            ( ( CPU_CLOCK_HZ / SYSTICK_FREQUENCY ) - 1 )
#define PSOC_MCWDT_ENABLE_DELAY       (20)
#define C0_MAX                        (0xffff) /* Max = 2sec */
#define C1_MAX                        (0xffff) /* Max = ~35hours when cascaded with C0 */
#define MAX_MCWDT_DURATION            (35*(HOURS))
#define PLATFORM_MAX_DEEP_SLEEP_TIME  (MAX_MCWDT_DURATION)
#define TICKS_IN_MS                   (32.767)

#ifndef PLATFORM_DEEP_SLEEP_THRESHOLD
#define PLATFORM_DEEP_SLEEP_THRESHOLD (500) /* Idle time above this period (in ms) will take system to deep-sleep */
#endif

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

#ifdef WICED_DISABLE_MCU_POWERSAVE
static unsigned long idle_power_down_hook( unsigned long sleep_ms );
#endif /* WICED_DISABLE_MCU_POWERSAVE */
static platform_result_t platform_mcu_powersave_add_disable_counter( int add );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/* MCU powersave disabled by default */
static int powersave_disable_counter = 1;

/******************************************************
 *               Function Definitions
 ******************************************************/

platform_result_t platform_mcu_powersave_init( void )
{
    return PLATFORM_SUCCESS;
}

void platform_mcu_powersave_exit_notify( void )
{
}

/******************************************************
 *         IRQ Handlers Definition & Mapping
 ******************************************************/

WWD_RTOS_DEFINE_ISR( platform_mcwdt_1_irq )
{
    uint32_t cause;
    uint32_t interrupt_mask;

    cause = Cy_MCWDT_GetInterruptStatusMasked(MCWDT_STRUCT1);

    /* Disable further interrupts, till enabled back again by setting up MCWDT */
    Cy_MCWDT_Unlock( MCWDT_STRUCT1 );
    Cy_MCWDT_ClearInterrupt(MCWDT_STRUCT1, cause);
    interrupt_mask = Cy_MCWDT_GetInterruptMask(MCWDT_STRUCT1);
    Cy_MCWDT_SetInterruptMask(MCWDT_STRUCT1, (interrupt_mask & ~(CY_MCWDT_CTR0 | CY_MCWDT_CTR1 )));
    Cy_MCWDT_Lock( MCWDT_STRUCT1 );
}

WWD_RTOS_MAP_ISR( platform_mcwdt_1_irq, srss_interrupt_mcwdt_1_IRQn_Handler )

/******************************************************
 *               RTOS Powersave Hooks
 ******************************************************/

platform_result_t platform_mcu_powersave_disable( void )
{
    return platform_mcu_powersave_add_disable_counter( 1 );
}

platform_result_t platform_mcu_powersave_enable( void )
{
    return platform_mcu_powersave_add_disable_counter( -1 );
}

wiced_bool_t platform_mcu_powersave_is_permitted( void )
{
    return ( powersave_disable_counter == 0 ) ? WICED_TRUE : WICED_FALSE;
}

static platform_result_t platform_mcu_powersave_add_disable_counter( int add )
{
    uint32_t flags;

    WICED_SAVE_INTERRUPTS( flags );

    powersave_disable_counter += add;
    wiced_assert( "unbalanced powersave calls", powersave_disable_counter >= 0 );

    WICED_RESTORE_INTERRUPTS( flags );

    return PLATFORM_SUCCESS;
}

void platform_stop_timer_tick( unsigned long* remaining_ticks )
{

    /* Get remaining SyStick value */
    *remaining_ticks = SysTick->VAL;

    /* Disable SySTick */
    SysTick->CTRL &= (uint32_t) ~( SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk );
}

void platform_start_timer_tick( void )
{
    SysTick->LOAD = ( uint32_t )( CYCLES_PER_SYSTICK );
    SysTick->CTRL = ( uint32_t )( SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_CLKSOURCE_Msk );
}

platform_result_t platform_stop_low_power_wakeup_timer( unsigned long *time_elapsed_ms )
{
    unsigned long c0_count;
    unsigned long c1_count;

    c0_count = Cy_MCWDT_GetCount( MCWDT_STRUCT1, CY_MCWDT_COUNTER0 );
    c1_count = Cy_MCWDT_GetCount( MCWDT_STRUCT1, CY_MCWDT_COUNTER1 );

    *time_elapsed_ms = ( ( c1_count << 16 ) | c0_count ) / TICKS_IN_MS;

    /* Disable MCWDT Timer */
    Cy_MCWDT_Unlock( MCWDT_STRUCT1 );
    Cy_MCWDT_Disable( MCWDT_STRUCT1, ( CY_MCWDT_CTR0 | CY_MCWDT_CTR1 ), PSOC_MCWDT_ENABLE_DELAY );
    Cy_MCWDT_Lock( MCWDT_STRUCT1 );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_set_low_power_wakeup_timer( uint32_t sleep_ms )
{

    cy_stc_mcwdt_config_t mcwdt_cfg = { '\0' };
    uint32_t counters = ( CY_MCWDT_CTR0 | CY_MCWDT_CTR1 );

    /* if time <= 2.0 seconds Use Counter 0 only and interrupt on C0 */
    if ( sleep_ms <= 2000 )
    {
        uint32_t c0_count;

        c0_count                 = TICKS_IN_MS * sleep_ms;
        mcwdt_cfg.c0Match        = c0_count;
        mcwdt_cfg.c1Match        = 0;
        mcwdt_cfg.c0Mode         = CY_MCWDT_MODE_INT;
        mcwdt_cfg.c1Mode         = CY_MCWDT_MODE_NONE;
        mcwdt_cfg.c2Mode         = CY_MCWDT_MODE_NONE;
        mcwdt_cfg.c0ClearOnMatch = 0;
        mcwdt_cfg.c1ClearOnMatch = 0;
        mcwdt_cfg.c0c1Cascade    = 0;
        mcwdt_cfg.c1c2Cascade    = 0;
        counters                 = CY_MCWDT_CTR0;

    }
    /* else Cascade C0,C1 and interrupt on C1, this provides max sleep time of 35 hours */
    else
    {

        uint32_t c1_count;

        c1_count                 = ( TICKS_IN_MS * sleep_ms ) / C0_MAX;
        mcwdt_cfg.c0Match        = C0_MAX;
        mcwdt_cfg.c1Match        = c1_count;
        mcwdt_cfg.c0Mode         = CY_MCWDT_MODE_NONE;
        mcwdt_cfg.c1Mode         = CY_MCWDT_MODE_INT;
        mcwdt_cfg.c2Mode         = CY_MCWDT_MODE_NONE;
        mcwdt_cfg.c0ClearOnMatch = 1;
        mcwdt_cfg.c1ClearOnMatch = 0;
        mcwdt_cfg.c0c1Cascade    = 1;
        mcwdt_cfg.c1c2Cascade    = 0;
        counters                 = ( CY_MCWDT_CTR0 | CY_MCWDT_CTR1 );
    }

    Cy_MCWDT_Unlock( MCWDT_STRUCT1 );
    Cy_MCWDT_Disable( MCWDT_STRUCT1, ( CY_MCWDT_CTR0 | CY_MCWDT_CTR1 ), PSOC_MCWDT_ENABLE_DELAY );
    Cy_MCWDT_ResetCounters( MCWDT_STRUCT1, ( CY_MCWDT_CTR0 | CY_MCWDT_CTR1 ), PSOC_MCWDT_ENABLE_DELAY );
    Cy_MCWDT_Init( MCWDT_STRUCT1, &mcwdt_cfg );
    Cy_MCWDT_SetInterruptMask( MCWDT_STRUCT1, counters );

    Cy_MCWDT_Enable( MCWDT_STRUCT1, counters, PSOC_MCWDT_ENABLE_DELAY );
    Cy_MCWDT_Lock( MCWDT_STRUCT1 );

    NVIC_EnableIRQ( srss_interrupt_mcwdt_1_IRQn );
    NVIC_SetPriority( srss_interrupt_mcwdt_1_IRQn, 3 );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_pdl_enter_deep_sleep( void )
{
    platform_result_t result;

    SDIO_SaveConfig( );

    if ( CY_SYSPM_SUCCESS != Cy_SysPm_DeepSleep( CY_SYSPM_WAIT_FOR_INTERRUPT ) )
    {
        result = PLATFORM_ERROR;
    }
    else
    {
        result = PLATFORM_SUCCESS;
    }

    SDIO_RestoreConfig( );

    return result;
}

platform_result_t platform_pdl_enter_sleep( void )
{
    if ( CY_SYSPM_SUCCESS != Cy_SysPm_Sleep( CY_SYSPM_WAIT_FOR_INTERRUPT ) )
    {
        return PLATFORM_ERROR;
    }
    else
    {
        return PLATFORM_SUCCESS;
    }
}

void platform_idle_hook( void )
{
    __asm("wfi");
}

#ifdef WICED_DISABLE_MCU_POWERSAVE
/* MCU Powersave is disabled */
static unsigned long idle_power_down_hook( unsigned long sleep_ms )
{
    UNUSED_PARAMETER( sleep_ms );

    WICED_DISABLE_INTERRUPTS( );

    /* Is PENDSVSET?
     * Need this check around WFI because we are executing
     * within a PENDSV handler and PENDSVSET will not preempt
     * it - i.e. wfi will not be treated as a nop.
     */
    if ( (SCB->ICSR & SCB_ICSR_PENDSVSET_Msk) == 0 )
    {
        /* No, PENDSVSET is not set. */
        /* Nothing pending, go to sleep. */
        __asm("wfi");
    }

    WICED_ENABLE_INTERRUPTS( );

    return 0;
}
#endif

unsigned long platform_power_down_hook( unsigned long time_until_next_event )
{
    unsigned long time_spent_in_low_power = 0;

#ifndef WICED_DISABLE_MCU_POWERSAVE
    if ( WICED_TRUE != platform_mcu_powersave_is_permitted( ) )
    {
        return 0;
    }

    if ( time_until_next_event > PLATFORM_DEEP_SLEEP_THRESHOLD )
    {
        if ( time_until_next_event > PLATFORM_MAX_DEEP_SLEEP_TIME )
        {
            time_until_next_event = PLATFORM_MAX_DEEP_SLEEP_TIME;
        }

        platform_enter_deep_sleep( time_until_next_event, &time_spent_in_low_power );
    }
    else
    {
        platform_enter_sleep( time_until_next_event, &time_spent_in_low_power );
    }

#else
    idle_power_down_hook( time_until_next_event );
#endif
    return time_spent_in_low_power;

}

platform_result_t platform_prepare_to_enter_low_power_mode( wiced_low_power_state_type_t state, unsigned long time_until_next_event )
{
    uint32_t remaining_ticks = 0;
    wiced_result_t result = WICED_SUCCESS;

    platform_stop_timer_tick( &remaining_ticks );
    platform_set_low_power_wakeup_timer( time_until_next_event );

#if WICED_ENABLE_LOW_POWER_EVENT_HANDLERS
    WICED_SLEEP_CALL_EVENT_HANDLERS( WICED_TRUE, state, WICED_SLEEP_EVENT_ENTER, &result );
#else
    UNUSED_PARAMETER( state );
#endif /* WICED_ENABLE_LOW_POWER_EVENT_HANDLERS */

    return ( result == WICED_SUCCESS ? PLATFORM_SUCCESS : PLATFORM_ERROR );
}

unsigned long wiced_post_exit_low_power_mode( wiced_low_power_state_type_t state )
{
    unsigned long time_spent_in_sleep = 0;
    wiced_result_t result;

#if WICED_ENABLE_LOW_POWER_EVENT_HANDLERS
    WICED_SLEEP_CALL_EVENT_HANDLERS( WICED_TRUE, state, WICED_SLEEP_EVENT_LEAVE, &result );
#else
    UNUSED_PARAMETER( state );
    UNUSED_PARAMETER( result );
#endif /* WICED_ENABLE_LOW_POWER_EVENT_HANDLERS */

    platform_stop_low_power_wakeup_timer( &time_spent_in_sleep );
    platform_start_timer_tick( );
    return time_spent_in_sleep;
}

platform_result_t platform_enter_deep_sleep( unsigned long time_until_next_event, unsigned long* time_spent_in_deep_sleep )
{
    platform_result_t result;

    if ( PLATFORM_SUCCESS != platform_prepare_to_enter_low_power_mode( WICED_LOW_POWER_DEEP_SLEEP, time_until_next_event ) )
    {
        result = PLATFORM_ERROR;
    }

    else
    {
        platform_watchdog_kick();
        result = platform_pdl_enter_deep_sleep( );
    }

    *time_spent_in_deep_sleep = wiced_post_exit_low_power_mode( WICED_LOW_POWER_DEEP_SLEEP );
    return result;
}

platform_result_t platform_enter_sleep( unsigned long time_until_next_event, unsigned long* time_spent_in_sleep )
{
    *time_spent_in_sleep = 0;
    return platform_pdl_enter_sleep( );
}
