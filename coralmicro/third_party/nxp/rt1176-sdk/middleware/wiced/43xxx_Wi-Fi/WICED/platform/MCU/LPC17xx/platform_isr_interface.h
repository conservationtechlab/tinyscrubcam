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

extern void NMIException        ( void ); // 2 Non Maskable Interrupt
extern void HardFaultException  ( void ); // 3 Hard Fault interrupt
extern void MemManageException  ( void ); // 4 Memory Management Fault interrupt
extern void BusFaultException   ( void ); // 5 Bus Fault interrupt
extern void UsageFaultException ( void ); // 6 Usage Fault interrupt
//extern void Sign_Value          ( void ); // 7 Contains 2's compliment checksum of vector entries 0-6
extern void SVC_irq             ( void ); // 11 SVC interrupt
extern void DebugMonitor        ( void ); // 12 Debug Monitor interrupt
extern void PENDSV_irq          ( void ); // 14 PendSV interrupt
extern void SYSTICK_irq         ( void ); // 15 Sys Tick Interrupt
extern void  WDT_irq            ( void );
extern void  TIMER0_irq         ( void );
extern void  TIMER1_irq         ( void );
extern void  TIMER2_irq         ( void );
extern void  TIMER3_irq         ( void );
extern void  UART0_irq          ( void );
extern void  UART1_irq          ( void );
extern void  UART2_irq          ( void );
extern void  UART3_irq          ( void );
extern void  PWM1_irq           ( void );
extern void  I2C0_irq           ( void );
extern void  I2C1_irq           ( void );
extern void  I2C2_irq           ( void );
extern void  SPI_irq            ( void );
extern void  SSP0_irq           ( void );
extern void  SSP1_irq           ( void );
extern void  PLL0_irq           ( void );
extern void  RTC_irq            ( void );
extern void  EINT0_irq          ( void );
extern void  EINT1_irq          ( void );
extern void  EINT2_irq          ( void );
extern void  EINT3_irq          ( void );
extern void  ADC_irq            ( void );
extern void  BOD_irq            ( void );
extern void  USB_irq            ( void );
extern void  CAN_irq            ( void );
extern void  DMA_irq            ( void );
extern void  I2S_irq            ( void );
extern void  ETH_irq            ( void );
extern void  RIT_irq            ( void );
extern void  MCPWM_irq          ( void );
extern void  QEI_irq            ( void );
extern void  PLL1_irq           ( void );
extern void  USBActivity_irq    ( void );
extern void  CANActivity_irq    ( void );
extern void  SDIO_irq           ( void );
extern void  UART4_irq          ( void );
extern void  SSP2_irq           ( void );
extern void  LCD_irq            ( void );
extern void  GPIO_irq           ( void );
extern void  PWM0_irq           ( void );
extern void  EEPROM_irq         ( void );



#ifdef __cplusplus
} /* extern "C" */
#endif

