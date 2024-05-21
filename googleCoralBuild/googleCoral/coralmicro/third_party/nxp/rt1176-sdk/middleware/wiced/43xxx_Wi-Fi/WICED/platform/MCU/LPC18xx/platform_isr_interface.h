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
 * Declares interrupt handlers prototype for LPC43xx MCU family
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

extern void NMIException        ( void ); /* 2 Non Maskable Interrupt                             */
extern void HardFaultException  ( void ); /* 3 Hard Fault interrupt                               */
extern void MemManageException  ( void ); /* 4 Memory Management Fault interrupt                  */
extern void BusFaultException   ( void ); /* 5 Bus Fault interrupt                                */
extern void UsageFaultException ( void ); /* 6 Usage Fault interrupt                              */
extern void SVC_irq             ( void ); /* 11 SVC interrupt                                     */
extern void DebugMonitor        ( void ); /* 12 Debug Monitor interrupt                           */
extern void PENDSV_irq          ( void ); /* 14 PendSV interrupt                                  */
extern void SYSTICK_irq         ( void ); /* 15 Sys Tick Interrupt                                */
extern void DAC_irq             ( void ); /* 16 DAC                                               */
extern void MAPP_irq            ( void ); /* 17 Cortex M0 APP                                     */
extern void DMA_irq             ( void ); /* 18 DMA                                               */
extern void FLASHEEPROM_irq     ( void ); /* 20 ORed flash bank A, flash bank B,EEPROM interrupts */
extern void ETH_irq             ( void ); /* 21 Ethernet                                          */
extern void SDIO_irq            ( void ); /* 22 SD/MMC                                            */
extern void LCD_irq             ( void ); /* 23 LCD                                               */
extern void USB0_irq            ( void ); /* 24 USB0                                              */
extern void USB1_irq            ( void ); /* 25 USB1                                              */
extern void SCT_irq             ( void ); /* 26 State Configurable Timer                          */
extern void RIT_irq             ( void ); /* 27 Repetitive Interrupt Timer                        */
extern void TIMER0_irq          ( void ); /* 28 Timer0                                            */
extern void TIMER1_irq          ( void ); /* 29 Timer1                                            */
extern void TIMER2_irq          ( void ); /* 30 Timer2                                            */
extern void TIMER3_irq          ( void ); /* 31 Timer3                                            */
extern void MCPWM_irq           ( void ); /* 32 Motor Control PWM                                 */
extern void ADC0_irq            ( void ); /* 33 A/D Converter 0                                   */
extern void I2C0_irq            ( void ); /* 34 I2C0                                              */
extern void I2C1_irq            ( void ); /* 35 I2C1                                              */
extern void SPI_irq             ( void ); /* 35 SPI                                               */
extern void ADC1_irq            ( void ); /* 37 A/D Converter 1                                   */
extern void SSP0_irq            ( void ); /* 38 SSP0                                              */
extern void SSP1_irq            ( void ); /* 39 SSP1                                              */
extern void USART0_irq          ( void ); /* 40 UART0                                             */
extern void USART1_irq          ( void ); /* 41 UART1                                             */
extern void USART2_irq          ( void ); /* 42 UART2                                             */
extern void USART3_irq          ( void ); /* 43 UART3                                             */
extern void I2S0_irq            ( void ); /* 44 I2S0                                              */
extern void I2S1_irq            ( void ); /* 45 I2S1                                              */
extern void GPIO0_irq           ( void ); /* 48 GPIO0                                             */
extern void GPIO1_irq           ( void ); /* 49 GPIO1                                             */
extern void GPIO2_irq           ( void ); /* 50 GPIO2                                             */
extern void GPIO3_irq           ( void ); /* 51 GPIO3                                             */
extern void GPIO4_irq           ( void ); /* 52 GPIO4                                             */
extern void GPIO5_irq           ( void ); /* 53 GPIO5                                             */
extern void GPIO6_irq           ( void ); /* 54 GPIO6                                             */
extern void GPIO7_irq           ( void ); /* 55 GPIO7                                             */
extern void GINT0_irq           ( void ); /* 56 GINT0                                             */
extern void GINT1_irq           ( void ); /* 57 GINT1                                             */
extern void EVRT_irq            ( void ); /* 58 Event Router                                      */
extern void CAN1_irq            ( void ); /* 59 C_CAN1                                            */
extern void ATIMER_irq          ( void ); /* 62 ATIMER                                            */
extern void RTC_irq             ( void ); /* 63 RTC                                               */
extern void WDT_irq             ( void ); /* 65 WDT                                               */
extern void CAN0_irq            ( void ); /* 67 C_CAN0                                            */
extern void QEI_irq             ( void ); /*                                                      */

#ifdef __cplusplus
} /* extern "C" */
#endif

