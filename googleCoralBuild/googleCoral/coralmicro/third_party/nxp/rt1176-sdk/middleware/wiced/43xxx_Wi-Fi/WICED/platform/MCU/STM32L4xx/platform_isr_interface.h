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
 * Declares ISR prototypes for STM32L4xx MCU family
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

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

extern void NMIException           ( void );  // Non Maskable Interrupt
extern void HardFaultException     ( void );  // Hard Fault interrupt
extern void MemManageException     ( void );  // Memory Management Fault interrupt
extern void BusFaultException      ( void );  // Bus Fault interrupt
extern void UsageFaultException    ( void );  // Usage Fault interrupt
extern void SVC_irq                ( void );  // SVC interrupt
extern void DebugMonitor           ( void );  // Debug Monitor interrupt
extern void PENDSV_irq             ( void );  // PendSV interrupt
extern void stm32l4xx_systick_irq  ( void );  // Sys Tick Interrupt
extern void WWDG_irq               ( void );  // Window WatchDog Interrupt
extern void PVD_PVM_irq            ( void );  // PVD/PVM1/PVM2/PVM3/PVM4 through EXTI Line detection Interrupts
extern void TAMP_STAMP_irq         ( void );  // Tamper and TimeStamp interrupts through the EXTI line
extern void RTC_WKUP_irq           ( void );  // RTC Wakeup interrupt through the EXTI line
extern void FLASH_irq              ( void );  // FLASH global Interrupt
extern void RCC_irq                ( void );  // RCC global Interrupt
extern void EXTI0_irq              ( void );  // EXTI Line0 Interrupt
extern void EXTI1_irq              ( void );  // EXTI Line1 Interrupt
extern void EXTI2_irq              ( void );  // EXTI Line2 Interrupt
extern void EXTI3_irq              ( void );  // EXTI Line3 Interrupt
extern void EXTI4_irq              ( void );  // EXTI Line4 Interrupt
extern void DMA1_Channel1_irq      ( void );  // DMA1 Channel 1 global Interrupt
extern void DMA1_Channel2_irq      ( void );  // DMA1 Channel 2 global Interrupt
extern void DMA1_Channel3_irq      ( void );  // DMA1 Channel 3 global Interrupt
extern void DMA1_Channel4_irq      ( void );  // DMA1 Channel 4 global Interrupt
extern void DMA1_Channel5_irq      ( void );  // DMA1 Channel 5 global Interrupt
extern void DMA1_Channel6_irq      ( void );  // DMA1 Channel 6 global Interrupt
extern void DMA1_Channel7_irq      ( void );  // DMA1 Channel 7 global Interrupt
extern void ADC1_2_irq             ( void );  // ADC1, ADC2 SAR global Interrupts
extern void CAN1_TX_irq            ( void );  // CAN1 TX Interrupt
extern void CAN1_RX0_irq           ( void );  // CAN1 RX0 Interrupt
extern void CAN1_RX1_irq           ( void );  // CAN1 RX1 Interrupt
extern void CAN1_SCE_irq           ( void );  // CAN1 SCE Interrupt
extern void EXTI9_5_irq            ( void );  // External Line[9:5] Interrupts
extern void TIM1_BRK_TIM15_irq     ( void );  // TIM1 Break interrupt and TIM15 global interrupt
extern void TIM1_UP_TIM16_irq      ( void );  // TIM1 Update Interrupt and TIM16 global interrupt
extern void TIM1_TRG_COM_TIM17_irq ( void );  // TIM1 Trigger and Commutation Interrupt and TIM17 global interrupt
extern void TIM1_CC_irq            ( void );  // TIM1 Capture Compare Interrupt
extern void TIM2_irq               ( void );  // TIM2 global Interrupt
extern void TIM3_irq               ( void );  // TIM3 global Interrupt
extern void TIM4_irq               ( void );  // TIM4 global Interrupt
extern void I2C1_EV_irq            ( void );  // I2C1 Event Interrupt
extern void I2C1_ER_irq            ( void );  // I2C1 Error Interrupt
extern void I2C2_EV_irq            ( void );  // I2C2 Event Interrupt
extern void I2C2_ER_irq            ( void );  // I2C2 Error Interrupt
extern void SPI1_irq               ( void );  // SPI1 global Interrupt
extern void SPI2_irq               ( void );  // SPI2 global Interrupt
extern void USART1_irq             ( void );  // USART1 global Interrupt
extern void USART2_irq             ( void );  // USART2 global Interrupt
extern void USART3_irq             ( void );  // USART3 global Interrupt
extern void EXTI15_10_irq          ( void );  // External Line[15:10] Interrupts
extern void RTC_Alarm_irq          ( void );  // RTC Alarm (A and B) through EXTI Line Interrupt
extern void DFSDM1_FLT3_irq        ( void );  // DFSDM1 Filter 3 global Interrupt
extern void TIM8_BRK_irq           ( void );  // TIM8 Break Interrupt
extern void TIM8_UP_irq            ( void );  // TIM8 Update Interrupt
extern void TIM8_TRG_COM_irq       ( void );  // TIM8 Trigger and Commutation Interrupt
extern void TIM8_CC_irq            ( void );  // TIM8 Capture Compare Interrupt
extern void ADC3_irq               ( void );  // ADC3 global  Interrupt
extern void FMC_irq                ( void );  // FMC global Interrupt
extern void SDMMC1_irq             ( void );  // SDMMC1 global Interrupt
extern void TIM5_irq               ( void );  // TIM5 global Interrupt
extern void SPI3_irq               ( void );  // SPI3 global Interrupt
extern void UART4_irq              ( void );  // UART4 global Interrupt
extern void UART5_irq              ( void );  // UART5 global Interrupt
extern void TIM6_DAC_irq           ( void );  // TIM6 global and DAC1&2 underrun error  interrupts
extern void TIM7_irq               ( void );  // TIM7 global interrupt
extern void DMA2_Channel1_irq      ( void );  // DMA2 Channel 1 global Interrupt
extern void DMA2_Channel2_irq      ( void );  // DMA2 Channel 2 global Interrupt
extern void DMA2_Channel3_irq      ( void );  // DMA2 Channel 3 global Interrupt
extern void DMA2_Channel4_irq      ( void );  // DMA2 Channel 4 global Interrupt
extern void DMA2_Channel5_irq      ( void );  // DMA2 Channel 5 global Interrupt
extern void DFSDM1_FLT0_irq        ( void );  // DFSDM1 Filter 0 global Interrupt
extern void DFSDM1_FLT1_irq        ( void );  // DFSDM1 Filter 1 global Interrupt
extern void DFSDM1_FLT2_irq        ( void );  // DFSDM1 Filter 2 global Interrupt
extern void COMP_irq               ( void );  // COMP1 and COMP2 Interrupts
extern void LPTIM1_irq             ( void );  // LP TIM1 interrupt
extern void LPTIM2_irq             ( void );  // LP TIM2 interrupt
extern void OTG_FS_irq             ( void );  // USB OTG FS global Interrupt
extern void DMA2_Channel6_irq      ( void );  // DMA2 Channel 6 global interrupt
extern void DMA2_Channel7_irq      ( void );  // DMA2 Channel 7 global interrupt
extern void LPUART1_irq            ( void );  // LP UART1 interrupt
extern void QUADSPI_irq            ( void );  // Quad SPI global interrupt
extern void I2C3_EV_irq            ( void );  // I2C3 event interrupt
extern void I2C3_ER_irq            ( void );  // I2C3 error interrupt
extern void SAI1_irq               ( void );  // Serial Audio Interface 1 global interrupt
extern void SAI2_irq               ( void );  // Serial Audio Interface 2 global interrupt
extern void SWPMI1_irq             ( void );  // Serial Wire Interface 1 global interrupt
extern void TSC_irq                ( void );  // Touch Sense Controller global interrupt
extern void LCD_irq                ( void );  // LCD global interrupt
extern void RNG_irq                ( void );  // RNG global interrupt
extern void FPU_irq                ( void );  // FPU global interrupt
extern void CRS_irq                ( void );  // CRS global interrupt
extern void I2C4_EV_irq            ( void );  // I2C4 Event interrupt
extern void I2C4_ER_irq            ( void );  // I2C4 Error interrupt
extern void DCMI_irq               ( void );  // DCMI global interrupt
extern void CAN2_TX_irq            ( void );  // CAN2 TX interrupt
extern void CAN2_RX0_irq           ( void );  // CAN2 RX0 interrupt
extern void CAN2_RX1_irq           ( void );  // CAN2 RX1 interrupt
extern void CAN2_SCE_irq           ( void );  // CAN2 SCE interrupt
extern void DMA2D_irq              ( void );  // DMA2D global interrupt

#ifdef __cplusplus
} /* extern "C" */
#endif
