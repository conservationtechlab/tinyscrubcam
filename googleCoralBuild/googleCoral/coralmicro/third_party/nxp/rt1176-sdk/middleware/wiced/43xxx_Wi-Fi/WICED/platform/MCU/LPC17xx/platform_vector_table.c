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
 * LPC43xx vector table
 */
#include <stdint.h>
#include "platform_cmsis.h"
#include "platform_assert.h"
#include "platform_constants.h"
#include "platform_isr.h"
#include "platform_isr_interface.h"
#include "wwd_rtos_isr.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#ifndef SVC_irq
#error SVC_irq not defined - this will probably cause RTOS to fail to run
#endif

#ifndef PENDSV_irq
#error PENDSV_irq not defined - this will probably cause RTOS to fail to run
#endif

#ifndef SYSTICK_irq
#error SYSTICK_irq not defined - this will probably cause RTOS to fail to run
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

extern void UnhandledInterrupt( void );
extern void reset_handler     ( void );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/* Pointer to stack location */
extern void* link_stack_end;

uint32_t  crp_bank = 0xFFFFFFFF;


PLATFORM_DEFINE_INTERRUPT_VECTOR_TABLE_ARRAY( interrupt_vector_table, PLATFORM_INTERRUPT_VECTOR_TABLE_HAS_VARIABLE_SIZE ) =
{
        (uint32_t)&link_stack_end       , // 0  Initial stack location
        (uint32_t)reset_handler         , // 1  Reset vector
        (uint32_t)NMIException          , // 2  Non Maskable Interrupt
        (uint32_t)HardFaultException    , // 3  Hard Fault interrupt
        (uint32_t)MemManageException    , // 4  Memory Management Fault interrupt
        (uint32_t)BusFaultException     , // 5  Bus Fault interrupt
        (uint32_t)UsageFaultException   , // 6  Usage Fault interrupt
        (uint32_t)0                     , // 7  Contains 2's compliment checksum of vector entries 0-6
        (uint32_t)0                     , // 8  Reserved
        (uint32_t)0                     , // 9  Reserved
        (uint32_t)0                     , // 10 Reserved
        (uint32_t)SVC_irq               , // 11 SVC interrupt
        (uint32_t)DebugMonitor          , // 12 Debug Monitor interrupt
        (uint32_t)0                     , // 13 Reserved
        (uint32_t)PENDSV_irq            , // 14 PendSV interrupt
        (uint32_t)SYSTICK_irq           , // 15 Sys Tick Interrupt
        (uint32_t) WDT_irq              , // 16 0x40 WDT Watchdog Interrupt (WDINT)
        (uint32_t) TIMER0_irq           , // 17 0x44 Timer 0
        (uint32_t) TIMER1_irq           , // 18 0x48 Timer 1
        (uint32_t) TIMER2_irq           , // 19 0x4C Timer 2
        (uint32_t) TIMER3_irq           , // 20 0x50 Timer 3
        (uint32_t) UART0_irq            , // 21 0x54 UART0
        (uint32_t) UART1_irq            , // 22 0x58 UART1
        (uint32_t) UART2_irq            , // 23 0x5C UART 2
        (uint32_t) UART3_irq            , // 24 0x60 UART 3
        (uint32_t) PWM1_irq             , // 25 0x64 PWM1
        (uint32_t) I2C0_irq             , // 26 0x68 I2C0
        (uint32_t) I2C1_irq             , // 27 0x6C I2C1
        (uint32_t) I2C2_irq             , // 28 0x70 I2C2
        (uint32_t) SPI_irq              , // 29 0x74 SPI
        (uint32_t) SSP0_irq             , // 30 0x78 SSP0
        (uint32_t) SSP1_irq             , // 31 0x7C SSP 1
        (uint32_t) PLL0_irq             , // 32 0x80 PLL0 (Main PLL)
        (uint32_t) RTC_irq              , // 33 0x84 RTC
        (uint32_t) EINT0_irq            , // 34 0x88 External Interrupt(EINT0)
        (uint32_t) EINT1_irq            , // 35 0x88 External Interrupt(EINT0)
        (uint32_t) EINT2_irq            , // 36 0x88 External Interrupt(EINT0)
        (uint32_t) EINT3_irq            , // 37 0x88 External Interrupt(EINT0)
        (uint32_t) ADC_irq              , // 38 0x98 ADC
        (uint32_t) BOD_irq              , // 39 0x9C BOD
        (uint32_t) USB_irq              , // 40 0xA0 USB
        (uint32_t) CAN_irq              , // 41 0xA4 CAN
        (uint32_t) DMA_irq              , // 42 0xA8 GPDMA
        (uint32_t) I2S_irq              , // 43 0xAC I2S
        (uint32_t) ETH_irq              , // 44 0xB0 Ethernet
        (uint32_t) RIT_irq              , // 45 0xB4 Repetitive Interrupt
        (uint32_t) MCPWM_irq            , // 46 0xB8 Motor Control PWM
        (uint32_t) QEI_irq              , // 47 0xBC Quadrature Encoder
        (uint32_t) PLL1_irq             , // 48 0xC0 PLL1 (USB PLL)
        (uint32_t) USBActivity_irq      , // 49 0xC4 USB Activity Interrupt
        (uint32_t) CANActivity_irq      , // 50 0xC8 CAN Activity Interrupt
};

