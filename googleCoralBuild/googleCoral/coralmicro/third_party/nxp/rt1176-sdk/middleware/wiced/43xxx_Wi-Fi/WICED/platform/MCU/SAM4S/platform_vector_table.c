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
 * STM32F2xx vector table
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

PLATFORM_DEFINE_INTERRUPT_VECTOR_TABLE_ARRAY( interrupt_vector_table, PLATFORM_INTERRUPT_VECTOR_TABLE_HAS_VARIABLE_SIZE ) =
{
    (uint32_t)&link_stack_end       , // Initial stack location
    (uint32_t)reset_handler         , // Reset vector
    (uint32_t)NMIException          , // Non Maskable Interrupt
    (uint32_t)HardFaultException    , // Hard Fault interrupt
    (uint32_t)MemManageException    , // Memory Management Fault interrupt
    (uint32_t)BusFaultException     , // Bus Fault interrupt
    (uint32_t)UsageFaultException   , // Usage Fault interrupt
    (uint32_t)0                     , // Reserved
    (uint32_t)0                     , // Reserved
    (uint32_t)0                     , // Reserved
    (uint32_t)0                     , // Reserved
    (uint32_t)SVC_irq               , // SVC interrupt
    (uint32_t)DebugMonitor          , // Debug Monitor interrupt
    (uint32_t)0                     , // Reserved
    (uint32_t)PENDSV_irq            , // PendSV interrupt
    (uint32_t)SYSTICK_irq           , // Sys Tick Interrupt
    (uint32_t)SUPPLY_CTRL_irq       ,
    (uint32_t)RESET_CTRL_irq        ,
    (uint32_t)RTC_irq               ,
    (uint32_t)RTT_irq               ,
    (uint32_t)WDT_irq               ,
    (uint32_t)PMC_irq               ,
    (uint32_t)EEFC_irq              ,
    (uint32_t)0                     , // Reserved
    (uint32_t)UART0_irq             ,
    (uint32_t)UART1_irq             ,
    (uint32_t)SMC_irq               ,
    (uint32_t)PIO_CTRL_A_irq        ,
    (uint32_t)PIO_CTRL_B_irq        ,
    (uint32_t)PIO_CTRL_C_irq        ,
    (uint32_t)USART0_irq            ,
    (uint32_t)USART1_irq            ,
    (uint32_t)0                     , //Reserved
    (uint32_t)0                     , //Reserved
    (uint32_t)MCI_irq               ,
    (uint32_t)TWI0_irq              ,
    (uint32_t)TWI1_irq              ,
    (uint32_t)SPI_irq               ,
    (uint32_t)SSC_irq               ,
    (uint32_t)TC0_irq               ,
    (uint32_t)TC1_irq               ,
    (uint32_t)TC2_irq               ,
    (uint32_t)TC3_irq               ,
    (uint32_t)TC4_irq               ,
    (uint32_t)TC5_irq               ,
    (uint32_t)ADC_irq               ,
    (uint32_t)DAC_irq               ,
    (uint32_t)PWM_irq               ,
    (uint32_t)CRCCU_ir              ,
    (uint32_t)AC_irq                ,
    (uint32_t)USB_irq               ,
    (uint32_t)0
};

/******************************************************
 *               Function Definitions
 ******************************************************/
