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
 * Define default PSoC 6 initialization functions
 */
#include "platform_init.h"
#include "platform_isr.h"
#include "platform_peripheral.h"
#include "platform_sleep.h"
#include "platform_config.h"
#include "platform_toolchain.h"
#include "platform/wwd_platform_interface.h"
#include "cy_device_headers.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define NO_OF_INTERRUPTS_SUPPORTED    ( CPUSS_IRQ_NR )

#define SRAM_START_ADDRESS_LOCATION    ((uint32_t)&sram_start_addr_loc)
extern void* sram_start_addr_loc;

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

}

WEAK void platform_init_memory( void )
{

}

void platform_init_mcu_infrastructure( void )
{
    uint8_t i;

    /* XXX Global variables within this call.
     * Call after BSS/DATA initialization.
     */
    SystemInit();

#ifdef INTERRUPT_VECTORS_IN_RAM
    SCB->VTOR = SRAM_START_ADDRESS_LOCATION; /* Change the vector table to point to start of SRAM */
#endif /* ifdef INTERRUPT_VECTORS_IN_RAM */

#if !defined ( BOOTLOADER )
    /* Initialize reset-cause-result global variable.
     * This API is called once because the underlying
     * register could be read/reset. Since the bootloader
     * and app will go through the same start-up, call
     * the API only for the app.
     */
#endif /* !defined ( BOOTLOADER ) */

    /* Initialize watchdog */
    platform_watchdog_init( );

    /* Initialize interrupt priorities */
    for ( i = 0; i < NO_OF_INTERRUPTS_SUPPORTED; i++ )
    {
        NVIC_SetPriority( (IRQn_Type) i, 0xf );
    }
    NVIC_SetPriorityGrouping( 3 );
    platform_init_rtos_irq_priorities();
    platform_init_peripheral_irq_priorities();

    /* Initialize GPIO IRQ manager */
    platform_gpio_irq_manager_init();

#ifndef WICED_DISABLE_MCU_POWERSAVE
    /* Initialize MCU powersave */
    platform_mcu_powersave_init( );

    /* Initialize RTC */
    platform_rtc_init( );
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

uint8_t platform_get_chip_revision ( void )
{
     uint8_t rev;
     rev = Cy_SysLib_GetDeviceRevision( );

     return ( rev );
}
