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
 * Declares ISR prototypes for STM32F2xx MCU family
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

extern void NMIException       (void);
extern void HardFaultException (void);
extern void MemManageException (void);
extern void BusFaultException  (void);
extern void UsageFaultException(void);
extern void SVC_irq            (void);
extern void DebugMonitor       (void);
extern void PENDSV_irq         (void);
extern void SYSTICK_irq        (void);
extern void SUPPLY_CTRL_irq    (void);
extern void RESET_CTRL_irq     (void);
extern void RTC_irq            (void);
extern void RTT_irq            (void);
extern void WDT_irq            (void);
extern void PMC_irq            (void);
extern void EEFC_irq           (void);
extern void UART0_irq          (void);
extern void UART1_irq          (void);
extern void SMC_irq            (void);
extern void PIO_CTRL_A_irq     (void);
extern void PIO_CTRL_B_irq     (void);
extern void PIO_CTRL_C_irq     (void);
extern void USART0_irq         (void);
extern void USART1_irq         (void);
extern void MCI_irq            (void);
extern void TWI0_irq           (void);
extern void TWI1_irq           (void);
extern void SPI_irq            (void);
extern void SSC_irq            (void);
extern void TC0_irq            (void);
extern void TC1_irq            (void);
extern void TC2_irq            (void);
extern void TC3_irq            (void);
extern void TC4_irq            (void);
extern void TC5_irq            (void);
extern void ADC_irq            (void);
extern void DAC_irq            (void);
extern void PWM_irq            (void);
extern void CRCCU_ir           (void);
extern void AC_irq             (void);
extern void USB_irq            (void);

#ifdef __cplusplus
} /* extern "C" */
#endif

