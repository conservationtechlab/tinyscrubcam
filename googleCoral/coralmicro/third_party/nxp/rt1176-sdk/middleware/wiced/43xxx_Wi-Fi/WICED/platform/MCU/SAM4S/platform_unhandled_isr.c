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
 * Defines STM32F2xx default unhandled ISR and default mappings to unhandled ISR
 */
#include <stdint.h>
#include "platform_cmsis.h"
#include "platform_assert.h"
#include "platform_constants.h"
#include "platform_isr.h"
#include "platform_isr_interface.h"
#include "wwd_rtos.h"
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
 *               Static Function Declarations
 ******************************************************/

extern void UnhandledInterrupt( void );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

PLATFORM_DEFINE_ISR( UnhandledInterrupt )
{
    uint32_t active_interrupt_vector = (uint32_t) ( SCB->ICSR & 0x3fU );

    /* This variable tells you which interrupt vector is currently active */
    (void)active_interrupt_vector;
    WICED_TRIGGER_BREAKPOINT( );

    /* reset the processor immeditly if not debug */
    platform_mcu_reset( );

    while( 1 )
    {
    }
}

/******************************************************
 *          Default IRQ Handler Declarations
 ******************************************************/

PLATFORM_SET_DEFAULT_ISR( NMIException           , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( HardFaultException     , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( MemManageException     , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( BusFaultException      , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( UsageFaultException    , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( DebugMonitor           , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( SUPPLY_CTRL_irq        , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( RESET_CTRL_irq         , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( RTC_irq                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( RTT_irq                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( WDT_irq                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PMC_irq                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( EEFC_irq               , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( UART0_irq              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( UART1_irq              , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( SMC_irq                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PIO_CTRL_A_irq         , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PIO_CTRL_B_irq         , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PIO_CTRL_C_irq         , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( USART0_irq             , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( USART1_irq             , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( MCI_irq                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( TWI0_irq               , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( TWI1_irq               , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( SPI_irq                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( SSC_irq                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( TC0_irq                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( TC1_irq                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( TC2_irq                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( TC3_irq                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( TC4_irq                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( TC5_irq                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( ADC_irq                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( DAC_irq                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( PWM_irq                , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( CRCCU_ir               , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( AC_irq                 , UnhandledInterrupt )
PLATFORM_SET_DEFAULT_ISR( USB_irq                , UnhandledInterrupt )

