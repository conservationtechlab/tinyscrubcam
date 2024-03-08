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
 * Declares ISR prototypes for PSoC 6 MCU family
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

extern void NMIException           ( void );  // 2  Non Maskable Interrupt
extern void HardFaultException     ( void );  // 3  Hard Fault interrupt
extern void MemManageException     ( void );  // 4  Memory Management Fault interrupt
extern void BusFaultException      ( void );  // 5  Bus Fault interrupt
extern void UsageFaultException    ( void );  // 6  Usage Fault interrupt
extern void SVC_irq                ( void );  // 11 SVC interrupt
extern void DebugMonitor           ( void );  // 12 Debug Monitor interrupt
extern void PENDSV_irq             ( void );  // 14 PendSV interrupt
extern void SYSTICK_irq            ( void );  // 15 Sys Tick Interrupt

/* External interrupts                                           Power Mode  Description                                           */
extern void ioss_interrupts_gpio_0_IRQn_Handler      ( void ); /* DeepSleep   GPIO Port Interrupt #0                                */
extern void ioss_interrupts_gpio_1_IRQn_Handler      ( void ); /* DeepSleep   GPIO Port Interrupt #1                                */
extern void ioss_interrupts_gpio_2_IRQn_Handler      ( void ); /* DeepSleep   GPIO Port Interrupt #2                                */
extern void ioss_interrupts_gpio_3_IRQn_Handler      ( void ); /* DeepSleep   GPIO Port Interrupt #3                                */
extern void ioss_interrupts_gpio_4_IRQn_Handler      ( void ); /* DeepSleep   GPIO Port Interrupt #4                                */
extern void ioss_interrupts_gpio_5_IRQn_Handler      ( void ); /* DeepSleep   GPIO Port Interrupt #5                                */
extern void ioss_interrupts_gpio_6_IRQn_Handler      ( void ); /* DeepSleep   GPIO Port Interrupt #6                                */
extern void ioss_interrupts_gpio_7_IRQn_Handler      ( void ); /* DeepSleep   GPIO Port Interrupt #7                                */
extern void ioss_interrupts_gpio_8_IRQn_Handler      ( void ); /* DeepSleep   GPIO Port Interrupt #8                                */
extern void ioss_interrupts_gpio_9_IRQn_Handler      ( void ); /* DeepSleep   GPIO Port Interrupt #9                                */
extern void ioss_interrupts_gpio_10_IRQn_Handler     ( void ); /* DeepSleep   GPIO Port Interrupt #10                               */
extern void ioss_interrupts_gpio_11_IRQn_Handler     ( void ); /* DeepSleep   GPIO Port Interrupt #11                               */
extern void ioss_interrupts_gpio_12_IRQn_Handler     ( void ); /* DeepSleep   GPIO Port Interrupt #12                               */
extern void ioss_interrupts_gpio_13_IRQn_Handler     ( void ); /* DeepSleep   GPIO Port Interrupt #13                               */
extern void ioss_interrupts_gpio_14_IRQn_Handler     ( void ); /* DeepSleep   GPIO Port Interrupt #14                               */
extern void ioss_interrupt_gpio_IRQn_Handler         ( void ); /* DeepSleep   GPIO All Ports                                        */
extern void ioss_interrupt_vdd_IRQn_Handler          ( void ); /* DeepSleep   GPIO Supply Detect Interrupt                          */
extern void lpcomp_interrupt_IRQn_Handler            ( void ); /* DeepSleep   Low Power Comparator Interrupt                        */
extern void scb_8_interrupt_IRQn_Handler             ( void ); /* DeepSleep   Serial Communication Block #8 (DeepSleep capable)     */
extern void srss_interrupt_mcwdt_0_IRQn_Handler      ( void ); /* DeepSleep   Multi Counter Watchdog Timer interrupt                */
extern void srss_interrupt_mcwdt_1_IRQn_Handler      ( void ); /* DeepSleep   Multi Counter Watchdog Timer interrupt                */
extern void srss_interrupt_backup_IRQn_Handler       ( void ); /* DeepSleep   Backup domain interrupt                               */
extern void srss_interrupt_IRQn_Handler              ( void ); /* DeepSleep   Other combined Interrupts for SRSS (LVD, WDT, CLKCAL) */
extern void pass_interrupt_ctbs_IRQn_Handler         ( void ); /* DeepSleep   CTBm Interrupt (all CTBms)                            */
extern void bless_interrupt_IRQn_Handler             ( void ); /* DeepSleep   Bluetooth Radio interrupt                             */
extern void cpuss_interrupts_ipc_0_IRQn_Handler      ( void ); /* DeepSleep   CPUSS Inter Process Communication Interrupt #0        */
extern void cpuss_interrupts_ipc_1_IRQn_Handler      ( void ); /* DeepSleep   CPUSS Inter Process Communication Interrupt #1        */
extern void cpuss_interrupts_ipc_2_IRQn_Handler      ( void ); /* DeepSleep   CPUSS Inter Process Communication Interrupt #2        */
extern void cpuss_interrupts_ipc_3_IRQn_Handler      ( void ); /* DeepSleep   CPUSS Inter Process Communication Interrupt #3        */
extern void cpuss_interrupts_ipc_4_IRQn_Handler      ( void ); /* DeepSleep   CPUSS Inter Process Communication Interrupt #4        */
extern void cpuss_interrupts_ipc_5_IRQn_Handler      ( void ); /* DeepSleep   CPUSS Inter Process Communication Interrupt #5        */
extern void cpuss_interrupts_ipc_6_IRQn_Handler      ( void ); /* DeepSleep   CPUSS Inter Process Communication Interrupt #6        */
extern void cpuss_interrupts_ipc_7_IRQn_Handler      ( void ); /* DeepSleep   CPUSS Inter Process Communication Interrupt #7        */
extern void cpuss_interrupts_ipc_8_IRQn_Handler      ( void ); /* DeepSleep   CPUSS Inter Process Communication Interrupt #8        */
extern void cpuss_interrupts_ipc_9_IRQn_Handler      ( void ); /* DeepSleep   CPUSS Inter Process Communication Interrupt #9        */
extern void cpuss_interrupts_ipc_10_IRQn_Handler     ( void ); /* DeepSleep   CPUSS Inter Process Communication Interrupt #10       */
extern void cpuss_interrupts_ipc_11_IRQn_Handler     ( void ); /* DeepSleep   CPUSS Inter Process Communication Interrupt #11       */
extern void cpuss_interrupts_ipc_12_IRQn_Handler     ( void ); /* DeepSleep   CPUSS Inter Process Communication Interrupt #12       */
extern void cpuss_interrupts_ipc_13_IRQn_Handler     ( void ); /* DeepSleep   CPUSS Inter Process Communication Interrupt #13       */
extern void cpuss_interrupts_ipc_14_IRQn_Handler     ( void ); /* DeepSleep   CPUSS Inter Process Communication Interrupt #14       */
extern void cpuss_interrupts_ipc_15_IRQn_Handler     ( void ); /* DeepSleep   CPUSS Inter Process Communication Interrupt #15       */
extern void scb_0_interrupt_IRQn_Handler             ( void ); /* Active      Serial Communication Block #0                         */
extern void scb_1_interrupt_IRQn_Handler             ( void ); /* Active      Serial Communication Block #1                         */
extern void scb_2_interrupt_IRQn_Handler             ( void ); /* Active      Serial Communication Block #2                         */
extern void scb_3_interrupt_IRQn_Handler             ( void ); /* Active      Serial Communication Block #3                         */
extern void scb_4_interrupt_IRQn_Handler             ( void ); /* Active      Serial Communication Block #4                         */
extern void scb_5_interrupt_IRQn_Handler             ( void ); /* Active      Serial Communication Block #5                         */
extern void scb_6_interrupt_IRQn_Handler             ( void ); /* Active      Serial Communication Block #6                         */
extern void scb_7_interrupt_IRQn_Handler             ( void ); /* Active      Serial Communication Block #7                         */
extern void csd_interrupt_IRQn_Handler               ( void ); /* Active      CSD (CapSense) interrupt                              */
extern void cpuss_interrupts_dw0_0_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #0, Channel #0                         */
extern void cpuss_interrupts_dw0_1_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #0, Channel #1                         */
extern void cpuss_interrupts_dw0_2_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #0, Channel #2                         */
extern void cpuss_interrupts_dw0_3_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #0, Channel #3                         */
extern void cpuss_interrupts_dw0_4_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #0, Channel #4                         */
extern void cpuss_interrupts_dw0_5_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #0, Channel #5                         */
extern void cpuss_interrupts_dw0_6_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #0, Channel #6                         */
extern void cpuss_interrupts_dw0_7_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #0, Channel #7                         */
extern void cpuss_interrupts_dw0_8_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #0, Channel #8                         */
extern void cpuss_interrupts_dw0_9_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #0, Channel #9                         */
extern void cpuss_interrupts_dw0_10_IRQn_Handler     ( void ); /* Active      CPUSS DataWire #0, Channel #10                        */
extern void cpuss_interrupts_dw0_11_IRQn_Handler     ( void ); /* Active      CPUSS DataWire #0, Channel #11                        */
extern void cpuss_interrupts_dw0_12_IRQn_Handler     ( void ); /* Active      CPUSS DataWire #0, Channel #12                        */
extern void cpuss_interrupts_dw0_13_IRQn_Handler     ( void ); /* Active      CPUSS DataWire #0, Channel #13                        */
extern void cpuss_interrupts_dw0_14_IRQn_Handler     ( void ); /* Active      CPUSS DataWire #0, Channel #14                        */
extern void cpuss_interrupts_dw0_15_IRQn_Handler     ( void ); /* Active      CPUSS DataWire #0, Channel #15                        */
extern void cpuss_interrupts_dw1_0_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #1, Channel #0                         */
extern void cpuss_interrupts_dw1_1_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #1, Channel #1                         */
extern void cpuss_interrupts_dw1_2_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #1, Channel #2                         */
extern void cpuss_interrupts_dw1_3_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #1, Channel #3                         */
extern void cpuss_interrupts_dw1_4_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #1, Channel #4                         */
extern void cpuss_interrupts_dw1_5_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #1, Channel #5                         */
extern void cpuss_interrupts_dw1_6_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #1, Channel #6                         */
extern void cpuss_interrupts_dw1_7_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #1, Channel #7                         */
extern void cpuss_interrupts_dw1_8_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #1, Channel #8                         */
extern void cpuss_interrupts_dw1_9_IRQn_Handler      ( void ); /* Active      CPUSS DataWire #1, Channel #9                         */
extern void cpuss_interrupts_dw1_10_IRQn_Handler     ( void ); /* Active      CPUSS DataWire #1, Channel #10                        */
extern void cpuss_interrupts_dw1_11_IRQn_Handler     ( void ); /* Active      CPUSS DataWire #1, Channel #11                        */
extern void cpuss_interrupts_dw1_12_IRQn_Handler     ( void ); /* Active      CPUSS DataWire #1, Channel #12                        */
extern void cpuss_interrupts_dw1_13_IRQn_Handler     ( void ); /* Active      CPUSS DataWire #1, Channel #13                        */
extern void cpuss_interrupts_dw1_14_IRQn_Handler     ( void ); /* Active      CPUSS DataWire #1, Channel #14                        */
extern void cpuss_interrupts_dw1_15_IRQn_Handler     ( void ); /* Active      CPUSS DataWire #1, Channel #15                        */
extern void cpuss_interrupts_fault_0_IRQn_Handler    ( void ); /* Active      CPUSS Fault Structure Interrupt #0                    */
extern void cpuss_interrupts_fault_1_IRQn_Handler    ( void ); /* Active      CPUSS Fault Structure Interrupt #1                    */
extern void cpuss_interrupt_crypto_IRQn_Handler      ( void ); /* Active      CRYPTO Accelerator Interrupt                          */
extern void cpuss_interrupt_fm_IRQn_Handler          ( void ); /* Active      FLASH Macro Interrupt                                 */
extern void cpuss_interrupts_cm0_cti_0_IRQn_Handler  ( void ); /* Active      CM0+ CTI #0                                           */
extern void cpuss_interrupts_cm0_cti_1_IRQn_Handler  ( void ); /* Active      CM0+ CTI #1                                           */
extern void cpuss_interrupts_cm4_cti_0_IRQn_Handler  ( void ); /* Active      CM4 CTI #0                                            */
extern void cpuss_interrupts_cm4_cti_1_IRQn_Handler  ( void ); /* Active      CM4 CTI #1                                            */
extern void tcpwm_0_interrupts_0_IRQn_Handler        ( void ); /* Active      TCPWM #0, Counter #0                                  */
extern void tcpwm_0_interrupts_1_IRQn_Handler        ( void ); /* Active      TCPWM #0, Counter #1                                  */
extern void tcpwm_0_interrupts_2_IRQn_Handler        ( void ); /* Active      TCPWM #0, Counter #2                                  */
extern void tcpwm_0_interrupts_3_IRQn_Handler        ( void ); /* Active      TCPWM #0, Counter #3                                  */
extern void tcpwm_0_interrupts_4_IRQn_Handler        ( void ); /* Active      TCPWM #0, Counter #4                                  */
extern void tcpwm_0_interrupts_5_IRQn_Handler        ( void ); /* Active      TCPWM #0, Counter #5                                  */
extern void tcpwm_0_interrupts_6_IRQn_Handler        ( void ); /* Active      TCPWM #0, Counter #6                                  */
extern void tcpwm_0_interrupts_7_IRQn_Handler        ( void ); /* Active      TCPWM #0, Counter #7                                  */
extern void tcpwm_1_interrupts_0_IRQn_Handler        ( void ); /* Active      TCPWM #1, Counter #0                                  */
extern void tcpwm_1_interrupts_1_IRQn_Handler        ( void ); /* Active      TCPWM #1, Counter #1                                  */
extern void tcpwm_1_interrupts_2_IRQn_Handler        ( void ); /* Active      TCPWM #1, Counter #2                                  */
extern void tcpwm_1_interrupts_3_IRQn_Handler        ( void ); /* Active      TCPWM #1, Counter #3                                  */
extern void tcpwm_1_interrupts_4_IRQn_Handler        ( void ); /* Active      TCPWM #1, Counter #4                                  */
extern void tcpwm_1_interrupts_5_IRQn_Handler        ( void ); /* Active      TCPWM #1, Counter #5                                  */
extern void tcpwm_1_interrupts_6_IRQn_Handler        ( void ); /* Active      TCPWM #1, Counter #6                                  */
extern void tcpwm_1_interrupts_7_IRQn_Handler        ( void ); /* Active      TCPWM #1, Counter #7                                  */
extern void tcpwm_1_interrupts_8_IRQn_Handler        ( void ); /* Active      TCPWM #1, Counter #8                                  */
extern void tcpwm_1_interrupts_9_IRQn_Handler        ( void ); /* Active      TCPWM #1, Counter #9                                  */
extern void tcpwm_1_interrupts_10_IRQn_Handler       ( void ); /* Active      TCPWM #1, Counter #10                                 */
extern void tcpwm_1_interrupts_11_IRQn_Handler       ( void ); /* Active      TCPWM #1, Counter #11                                 */
extern void tcpwm_1_interrupts_12_IRQn_Handler       ( void ); /* Active      TCPWM #1, Counter #12                                 */
extern void tcpwm_1_interrupts_13_IRQn_Handler       ( void ); /* Active      TCPWM #1, Counter #13                                 */
extern void tcpwm_1_interrupts_14_IRQn_Handler       ( void ); /* Active      TCPWM #1, Counter #14                                 */
extern void tcpwm_1_interrupts_15_IRQn_Handler       ( void ); /* Active      TCPWM #1, Counter #15                                 */
extern void tcpwm_1_interrupts_16_IRQn_Handler       ( void ); /* Active      TCPWM #1, Counter #16                                 */
extern void tcpwm_1_interrupts_17_IRQn_Handler       ( void ); /* Active      TCPWM #1, Counter #17                                 */
extern void tcpwm_1_interrupts_18_IRQn_Handler       ( void ); /* Active      TCPWM #1, Counter #18                                 */
extern void tcpwm_1_interrupts_19_IRQn_Handler       ( void ); /* Active      TCPWM #1, Counter #19                                 */
extern void tcpwm_1_interrupts_20_IRQn_Handler       ( void ); /* Active      TCPWM #1, Counter #20                                 */
extern void tcpwm_1_interrupts_21_IRQn_Handler       ( void ); /* Active      TCPWM #1, Counter #21                                 */
extern void tcpwm_1_interrupts_22_IRQn_Handler       ( void ); /* Active      TCPWM #1, Counter #22                                 */
extern void tcpwm_1_interrupts_23_IRQn_Handler       ( void ); /* Active      TCPWM #1, Counter #23                                 */
extern void udb_interrupts_0_IRQn_Handler            ( void ); /* Active      UDB Interrupt #0                                      */
extern void udb_interrupts_1_IRQn_Handler            ( void ); /* Active      UDB Interrupt #1                                      */
extern void udb_interrupts_2_IRQn_Handler            ( void ); /* Active      UDB Interrupt #2                                      */
extern void udb_interrupts_3_IRQn_Handler            ( void ); /* Active      UDB Interrupt #3                                      */
extern void udb_interrupts_4_IRQn_Handler            ( void ); /* Active      UDB Interrupt #4                                      */
extern void udb_interrupts_5_IRQn_Handler            ( void ); /* Active      UDB Interrupt #5                                      */
extern void udb_interrupts_6_IRQn_Handler            ( void ); /* Active      UDB Interrupt #6                                      */
extern void udb_interrupts_7_IRQn_Handler            ( void ); /* Active      UDB Interrupt #7                                      */
extern void udb_interrupts_8_IRQn_Handler            ( void ); /* Active      UDB Interrupt #8                                      */
extern void udb_interrupts_9_IRQn_Handler            ( void ); /* Active      UDB Interrupt #9                                      */
extern void udb_interrupts_10_IRQn_Handler           ( void ); /* Active      UDB Interrupt #10                                     */
extern void udb_interrupts_11_IRQn_Handler           ( void ); /* Active      UDB Interrupt #11                                     */
extern void udb_interrupts_12_IRQn_Handler           ( void ); /* Active      UDB Interrupt #12                                     */
extern void udb_interrupts_13_IRQn_Handler           ( void ); /* Active      UDB Interrupt #13                                     */
extern void udb_interrupts_14_IRQn_Handler           ( void ); /* Active      UDB Interrupt #14                                     */
extern void udb_interrupts_15_IRQn_Handler           ( void ); /* Active      UDB Interrupt #15                                     */
extern void pass_interrupt_sar_IRQn_Handler          ( void ); /* Active      SAR ADC interrupt                                     */
extern void audioss_interrupt_i2s_IRQn_Handler       ( void ); /* Active      I2S Audio interrupt                                   */
extern void audioss_interrupt_pdm_IRQn_Handler       ( void ); /* Active      PDM/PCM Audio interrupt                               */
extern void profile_interrupt_IRQn_Handler           ( void ); /* Active      Energy Profiler interrupt                             */
extern void smif_interrupt_IRQn_Handler              ( void ); /* Active      Serial Memory Interface interrupt                     */
extern void usb_interrupt_hi_IRQn_Handler            ( void ); /* Active      USB Interrupt                                         */
extern void usb_interrupt_med_IRQn_Handler           ( void ); /* Active      USB Interrupt                                         */
extern void usb_interrupt_lo_IRQn_Handler            ( void ); /* Active      USB Interrupt                                         */
extern void pass_interrupt_dacs_IRQn_Handler         ( void ); /* Active      Consolidated interrupt for all DACs                   */

#ifdef __cplusplus
} /* extern "C" */
#endif

