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
 * Define default SAM4S initialisation functions
 */
#include "platform_init.h"
#include "platform_isr.h"
#include "platform_peripheral.h"
#include "platform_sleep.h"
#include "platform_config.h"
#include "platform_toolchain.h"
#include "platform/wwd_platform_interface.h"
#include "core_cm4.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define AIRCR_VECTKEY_MASK           ((uint32_t)0x05FA0000)
#define NVIC_PriorityGroup_4         ((uint32_t)0x300) /*!< 4 bits for pre-emption priority
                                                            0 bits for subpriority */

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
 *               Function Definitions
 ******************************************************/

void platform_mcu_reset( void )
{
    NVIC_SystemReset( );

    /* Loop forever */
    while ( 1 )
    {
    }
}

WEAK void platform_init_system_clocks( void )
{
    sysclk_init();

    /* Switch Slow Clock source to external 32K crystal */
    pmc_switch_sclk_to_32kxtal( 0 );
    while( pmc_osc_is_ready_32kxtal( ) == 0 )
    {
    }

    pmc_disable_udpck( );
}

WEAK void platform_init_memory( void )
{

}

void platform_init_mcu_infrastructure( void )
{
    uint8_t i;

#ifdef INTERRUPT_VECTORS_IN_RAM
    SCB->VTOR = 0x20000000; /* Change the vector table to point to start of SRAM */
#endif /* ifdef INTERRUPT_VECTORS_IN_RAM */

    /* Initialise watchdog */
    platform_watchdog_init( );

    /* Initialise interrupt priorities */
    for ( i = 0; i < 35; i++ )
    {
        NVIC_SetPriority( i, 0xf );
    }

    /* Enable PIO peripheral clocks */
    ioport_init();

    //NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

    /* set priority grouping inside cpu core */
    SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_PriorityGroup_4;

    platform_init_rtos_irq_priorities();
    platform_init_peripheral_irq_priorities();

#ifndef WICED_DISABLE_MCU_POWERSAVE
    /* Initialise MCU powersave */
    platform_mcu_powersave_init( );
    platform_mcu_powersave_disable( );

    /* Initialise RTC */
//    platform_rtc_init( );
#endif /* ifndef WICED_DISABLE_MCU_POWERSAVE */
}

void platform_init_connectivity_module( void )
{
    /* Ensure 802.11 device is in reset. */
    host_platform_init( );
}

WEAK void platform_init_external_devices( void )
{

}

