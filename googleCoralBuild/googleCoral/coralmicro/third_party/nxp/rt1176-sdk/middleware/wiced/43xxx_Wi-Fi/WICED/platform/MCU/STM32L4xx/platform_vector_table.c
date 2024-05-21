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
 * STM32L4xx vector table
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
    (uint32_t)stm32l4xx_systick_irq , // Sys Tick Interrupt
    (uint32_t)WWDG_irq              , // Window WatchDog Interrupt
    (uint32_t)PVD_PVM_irq           , // PVD/PVM1/PVM2/PVM3/PVM4 through EXTI Line detection Interrupts
    (uint32_t)TAMP_STAMP_irq        , // Tamper and TimeStamp interrupts through the EXTI line
    (uint32_t)RTC_WKUP_irq          , // RTC Wakeup interrupt through the EXTI line
    (uint32_t)FLASH_irq             , // FLASH global Interrupt
    (uint32_t)RCC_irq               , // RCC global Interrupt
    (uint32_t)EXTI0_irq             , // EXTI Line0 Interrupt
    (uint32_t)EXTI1_irq             , // EXTI Line1 Interrupt
    (uint32_t)EXTI2_irq             , // EXTI Line2 Interrupt
    (uint32_t)EXTI3_irq             , // EXTI Line3 Interrupt
    (uint32_t)EXTI4_irq             , // EXTI Line4 Interrupt
    (uint32_t)DMA1_Channel1_irq     , // DMA1 Channel 1 global Interrupt
    (uint32_t)DMA1_Channel2_irq     , // DMA1 Channel 2 global Interrupt
    (uint32_t)DMA1_Channel3_irq     , // DMA1 Channel 3 global Interrupt
    (uint32_t)DMA1_Channel4_irq     , // DMA1 Channel 4 global Interrupt
    (uint32_t)DMA1_Channel5_irq     , // DMA1 Channel 5 global Interrupt
    (uint32_t)DMA1_Channel6_irq     , // DMA1 Channel 6 global Interrupt
    (uint32_t)DMA1_Channel7_irq     , // DMA1 Channel 7 global Interrupt
    (uint32_t)ADC1_2_irq            , // ADC1, ADC2 SAR global Interrupts
    (uint32_t)CAN1_TX_irq           , // CAN1 TX Interrupt
    (uint32_t)CAN1_RX0_irq          , // CAN1 RX0 Interrupt
    (uint32_t)CAN1_RX1_irq          , // CAN1 RX1 Interrupt
    (uint32_t)CAN1_SCE_irq          , // CAN1 SCE Interrupt
    (uint32_t)EXTI9_5_irq           , // External Line[9:5] Interrupts
    (uint32_t)TIM1_BRK_TIM15_irq    , // TIM1 Break interrupt and TIM15 global interrupt
    (uint32_t)TIM1_UP_TIM16_irq     , // TIM1 Update Interrupt and TIM16 global interrupt
    (uint32_t)TIM1_TRG_COM_TIM17_irq, // TIM1 Trigger and Commutation Interrupt and TIM17 global interrupt
    (uint32_t)TIM1_CC_irq           , // TIM1 Capture Compare Interrupt
    (uint32_t)TIM2_irq              , // TIM2 global Interrupt
    (uint32_t)TIM3_irq              , // TIM3 global Interrupt
    (uint32_t)TIM4_irq              , // TIM4 global Interrupt
    (uint32_t)I2C1_EV_irq           , // I2C1 Event Interrupt
    (uint32_t)I2C1_ER_irq           , // I2C1 Error Interrupt
    (uint32_t)I2C2_EV_irq           , // I2C2 Event Interrupt
    (uint32_t)I2C2_ER_irq           , // I2C2 Error Interrupt
    (uint32_t)SPI1_irq              , // SPI1 global Interrupt
    (uint32_t)SPI2_irq              , // SPI2 global Interrupt
    (uint32_t)USART1_irq            , // USART1 global Interrupt
    (uint32_t)USART2_irq            , // USART2 global Interrupt
    (uint32_t)USART3_irq            , // USART3 global Interrupt
    (uint32_t)EXTI15_10_irq         , // External Line[15:10] Interrupts
    (uint32_t)RTC_Alarm_irq         , // RTC Alarm (A and B) through EXTI Line Interrupt
    (uint32_t)DFSDM1_FLT3_irq       , // DFSDM1 Filter 3 global Interrupt
    (uint32_t)TIM8_BRK_irq          , // TIM8 Break Interrupt
    (uint32_t)TIM8_UP_irq           , // TIM8 Update Interrupt
    (uint32_t)TIM8_TRG_COM_irq      , // TIM8 Trigger and Commutation Interrupt
    (uint32_t)TIM8_CC_irq           , // TIM8 Capture Compare Interrupt
    (uint32_t)ADC3_irq              , // ADC3 global  Interrupt
    (uint32_t)FMC_irq               , // FMC global Interrupt
    (uint32_t)SDMMC1_irq            , // SDMMC1 global Interrupt
    (uint32_t)TIM5_irq              , // TIM5 global Interrupt
    (uint32_t)SPI3_irq              , // SPI3 global Interrupt
    (uint32_t)UART4_irq             , // UART4 global Interrupt
    (uint32_t)UART5_irq             , // UART5 global Interrupt
    (uint32_t)TIM6_DAC_irq          , // TIM6 global and DAC1&2 underrun error  interrupts
    (uint32_t)TIM7_irq              , // TIM7 global interrupt
    (uint32_t)DMA2_Channel1_irq     , // DMA2 Channel 1 global Interrupt
    (uint32_t)DMA2_Channel2_irq     , // DMA2 Channel 2 global Interrupt
    (uint32_t)DMA2_Channel3_irq     , // DMA2 Channel 3 global Interrupt
    (uint32_t)DMA2_Channel4_irq     , // DMA2 Channel 4 global Interrupt
    (uint32_t)DMA2_Channel5_irq     , // DMA2 Channel 5 global Interrupt
    (uint32_t)DFSDM1_FLT0_irq       , // DFSDM1 Filter 0 global Interrupt
    (uint32_t)DFSDM1_FLT1_irq       , // DFSDM1 Filter 1 global Interrupt
    (uint32_t)DFSDM1_FLT2_irq       , // DFSDM1 Filter 2 global Interrupt
    (uint32_t)COMP_irq              , // COMP1 and COMP2 Interrupts
    (uint32_t)LPTIM1_irq            , // LP TIM1 interrupt
    (uint32_t)LPTIM2_irq            , // LP TIM2 interrupt
    (uint32_t)OTG_FS_irq            , // USB OTG FS global Interrupt
    (uint32_t)DMA2_Channel6_irq     , // DMA2 Channel 6 global interrupt
    (uint32_t)DMA2_Channel7_irq     , // DMA2 Channel 7 global interrupt
    (uint32_t)LPUART1_irq           , // LP UART1 interrupt
    (uint32_t)QUADSPI_irq           , // Quad SPI global interrupt
    (uint32_t)I2C3_EV_irq           , // I2C3 event interrupt
    (uint32_t)I2C3_ER_irq           , // I2C3 error interrupt
    (uint32_t)SAI1_irq              , // Serial Audio Interface 1 global interrupt
    (uint32_t)SAI2_irq              , // Serial Audio Interface 2 global interrupt
    (uint32_t)SWPMI1_irq            , // Serial Wire Interface 1 global interrupt
    (uint32_t)TSC_irq               , // Touch Sense Controller global interrupt
    (uint32_t)LCD_irq               , // LCD global interrupt
    (uint32_t)RNG_irq               , // RNG global interrupt
    (uint32_t)FPU_irq               , // FPU global interrupt
    (uint32_t)CRS_irq               , // CRS global interrupt
    (uint32_t)I2C4_EV_irq           , // I2C4 Event interrupt
    (uint32_t)I2C4_ER_irq           , // I2C4 Error interrupt
    (uint32_t)DCMI_irq              , // DCMI global interrupt
    (uint32_t)CAN2_TX_irq           , // CAN2 TX interrupt
    (uint32_t)CAN2_RX0_irq          , // CAN2 RX0 interrupt
    (uint32_t)CAN2_RX1_irq          , // CAN2 RX1 interrupt
    (uint32_t)CAN2_SCE_irq          , // CAN2 SCE interrupt
    (uint32_t)DMA2D_irq             , // DMA2D global interrupt
};

/******************************************************
 *               Function Definitions
 ******************************************************/

WWD_RTOS_DEFINE_ISR(stm32l4xx_systick_irq)
{
    HAL_IncTick();
    SYSTICK_irq();
}
