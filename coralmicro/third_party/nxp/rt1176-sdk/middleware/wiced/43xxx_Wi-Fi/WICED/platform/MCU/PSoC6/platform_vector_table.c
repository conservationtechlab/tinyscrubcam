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
 * PSoC 6 vector table
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

/* Assign vector table entry (IRQ) to a handler. */
#define INTERRUPT_VECTOR_TABLE_ENTRY( _IRQn_Type_ ) \
    [_IRQn_Type_ + 16] = (uint32_t)_IRQn_Type_##_Handler

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
    (uint32_t)&link_stack_end       , // 0  Initial stack location
    (uint32_t)reset_handler         , // 1  Reset vector
    (uint32_t)NMIException          , // 2  Non Maskable Interrupt
    (uint32_t)HardFaultException    , // 3  Hard Fault interrupt
    (uint32_t)MemManageException    , // 4  Memory Management Fault interrupt
    (uint32_t)BusFaultException     , // 5  Bus Fault interrupt
    (uint32_t)UsageFaultException   , // 6  Usage Fault interrupt
    (uint32_t)0                     , // 7  Reserved
    (uint32_t)0                     , // 8  Reserved
    (uint32_t)0                     , // 9  Reserved
    (uint32_t)0                     , // 10 Reserved
    (uint32_t)SVC_irq               , // 11 SVC interrupt
    (uint32_t)DebugMonitor          , // 12 Debug Monitor interrupt
    (uint32_t)0                     , // 13 Reserved
    (uint32_t)PENDSV_irq            , // 14 PendSV interrupt
    (uint32_t)SYSTICK_irq           , // 15 Sys Tick Interrupt

    INTERRUPT_VECTOR_TABLE_ENTRY( ioss_interrupts_gpio_0_IRQn )      , /* DeepSleep   GPIO Port Interrupt #0                                */
    INTERRUPT_VECTOR_TABLE_ENTRY( ioss_interrupts_gpio_1_IRQn )      , /* DeepSleep   GPIO Port Interrupt #1                                */
    INTERRUPT_VECTOR_TABLE_ENTRY( ioss_interrupts_gpio_2_IRQn )      , /* DeepSleep   GPIO Port Interrupt #2                                */
    INTERRUPT_VECTOR_TABLE_ENTRY( ioss_interrupts_gpio_3_IRQn )      , /* DeepSleep   GPIO Port Interrupt #3                                */
    INTERRUPT_VECTOR_TABLE_ENTRY( ioss_interrupts_gpio_4_IRQn )      , /* DeepSleep   GPIO Port Interrupt #4                                */
    INTERRUPT_VECTOR_TABLE_ENTRY( ioss_interrupts_gpio_5_IRQn )      , /* DeepSleep   GPIO Port Interrupt #5                                */
    INTERRUPT_VECTOR_TABLE_ENTRY( ioss_interrupts_gpio_6_IRQn )      , /* DeepSleep   GPIO Port Interrupt #6                                */
    INTERRUPT_VECTOR_TABLE_ENTRY( ioss_interrupts_gpio_7_IRQn )      , /* DeepSleep   GPIO Port Interrupt #7                                */
    INTERRUPT_VECTOR_TABLE_ENTRY( ioss_interrupts_gpio_8_IRQn )      , /* DeepSleep   GPIO Port Interrupt #8                                */
    INTERRUPT_VECTOR_TABLE_ENTRY( ioss_interrupts_gpio_9_IRQn )      , /* DeepSleep   GPIO Port Interrupt #9                                */
    INTERRUPT_VECTOR_TABLE_ENTRY( ioss_interrupts_gpio_10_IRQn )     , /* DeepSleep   GPIO Port Interrupt #10                               */
    INTERRUPT_VECTOR_TABLE_ENTRY( ioss_interrupts_gpio_11_IRQn )     , /* DeepSleep   GPIO Port Interrupt #11                               */
    INTERRUPT_VECTOR_TABLE_ENTRY( ioss_interrupts_gpio_12_IRQn )     , /* DeepSleep   GPIO Port Interrupt #12                               */
    INTERRUPT_VECTOR_TABLE_ENTRY( ioss_interrupts_gpio_13_IRQn )     , /* DeepSleep   GPIO Port Interrupt #13                               */
    INTERRUPT_VECTOR_TABLE_ENTRY( ioss_interrupts_gpio_14_IRQn )     , /* DeepSleep   GPIO Port Interrupt #14                               */
    INTERRUPT_VECTOR_TABLE_ENTRY( ioss_interrupt_gpio_IRQn )         , /* DeepSleep   GPIO All Ports                                        */
    INTERRUPT_VECTOR_TABLE_ENTRY( ioss_interrupt_vdd_IRQn )          , /* DeepSleep   GPIO Supply Detect Interrupt                          */
    INTERRUPT_VECTOR_TABLE_ENTRY( lpcomp_interrupt_IRQn )            , /* DeepSleep   Low Power Comparator Interrupt                        */
    INTERRUPT_VECTOR_TABLE_ENTRY( scb_8_interrupt_IRQn )             , /* DeepSleep   Serial Communication Block #8 (DeepSleep capable)     */
    INTERRUPT_VECTOR_TABLE_ENTRY( srss_interrupt_mcwdt_0_IRQn )      , /* DeepSleep   Multi Counter Watchdog Timer interrupt                */
    INTERRUPT_VECTOR_TABLE_ENTRY( srss_interrupt_mcwdt_1_IRQn )      , /* DeepSleep   Multi Counter Watchdog Timer interrupt                */
    INTERRUPT_VECTOR_TABLE_ENTRY( srss_interrupt_backup_IRQn )       , /* DeepSleep   Backup domain interrupt                               */
    INTERRUPT_VECTOR_TABLE_ENTRY( srss_interrupt_IRQn )              , /* DeepSleep   Other combined Interrupts for SRSS (LVD, WDT, CLKCAL) */
    INTERRUPT_VECTOR_TABLE_ENTRY( pass_interrupt_ctbs_IRQn )         , /* DeepSleep   CTBm Interrupt (all CTBms)                            */
    INTERRUPT_VECTOR_TABLE_ENTRY( bless_interrupt_IRQn )             , /* DeepSleep   Bluetooth Radio interrupt                             */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_ipc_0_IRQn )      , /* DeepSleep   CPUSS Inter Process Communication Interrupt #0        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_ipc_1_IRQn )      , /* DeepSleep   CPUSS Inter Process Communication Interrupt #1        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_ipc_2_IRQn )      , /* DeepSleep   CPUSS Inter Process Communication Interrupt #2        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_ipc_3_IRQn )      , /* DeepSleep   CPUSS Inter Process Communication Interrupt #3        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_ipc_4_IRQn )      , /* DeepSleep   CPUSS Inter Process Communication Interrupt #4        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_ipc_5_IRQn )      , /* DeepSleep   CPUSS Inter Process Communication Interrupt #5        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_ipc_6_IRQn )      , /* DeepSleep   CPUSS Inter Process Communication Interrupt #6        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_ipc_7_IRQn )      , /* DeepSleep   CPUSS Inter Process Communication Interrupt #7        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_ipc_8_IRQn )      , /* DeepSleep   CPUSS Inter Process Communication Interrupt #8        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_ipc_9_IRQn )      , /* DeepSleep   CPUSS Inter Process Communication Interrupt #9        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_ipc_10_IRQn )     , /* DeepSleep   CPUSS Inter Process Communication Interrupt #10       */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_ipc_11_IRQn )     , /* DeepSleep   CPUSS Inter Process Communication Interrupt #11       */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_ipc_12_IRQn )     , /* DeepSleep   CPUSS Inter Process Communication Interrupt #12       */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_ipc_13_IRQn )     , /* DeepSleep   CPUSS Inter Process Communication Interrupt #13       */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_ipc_14_IRQn )     , /* DeepSleep   CPUSS Inter Process Communication Interrupt #14       */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_ipc_15_IRQn )     , /* DeepSleep   CPUSS Inter Process Communication Interrupt #15       */
    INTERRUPT_VECTOR_TABLE_ENTRY( scb_0_interrupt_IRQn )             , /* Active      Serial Communication Block #0                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( scb_1_interrupt_IRQn )             , /* Active      Serial Communication Block #1                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( scb_2_interrupt_IRQn )             , /* Active      Serial Communication Block #2                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( scb_3_interrupt_IRQn )             , /* Active      Serial Communication Block #3                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( scb_4_interrupt_IRQn )             , /* Active      Serial Communication Block #4                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( scb_5_interrupt_IRQn )             , /* Active      Serial Communication Block #5                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( scb_6_interrupt_IRQn )             , /* Active      Serial Communication Block #6                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( scb_7_interrupt_IRQn )             , /* Active      Serial Communication Block #7                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( csd_interrupt_IRQn )               , /* Active      CSD (CapSense) interrupt                              */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw0_0_IRQn )      , /* Active      CPUSS DataWire #0, Channel #0                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw0_1_IRQn )      , /* Active      CPUSS DataWire #0, Channel #1                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw0_2_IRQn )      , /* Active      CPUSS DataWire #0, Channel #2                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw0_3_IRQn )      , /* Active      CPUSS DataWire #0, Channel #3                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw0_4_IRQn )      , /* Active      CPUSS DataWire #0, Channel #4                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw0_5_IRQn )      , /* Active      CPUSS DataWire #0, Channel #5                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw0_6_IRQn )      , /* Active      CPUSS DataWire #0, Channel #6                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw0_7_IRQn )      , /* Active      CPUSS DataWire #0, Channel #7                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw0_8_IRQn )      , /* Active      CPUSS DataWire #0, Channel #8                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw0_9_IRQn )      , /* Active      CPUSS DataWire #0, Channel #9                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw0_10_IRQn )     , /* Active      CPUSS DataWire #0, Channel #10                        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw0_11_IRQn )     , /* Active      CPUSS DataWire #0, Channel #11                        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw0_12_IRQn )     , /* Active      CPUSS DataWire #0, Channel #12                        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw0_13_IRQn )     , /* Active      CPUSS DataWire #0, Channel #13                        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw0_14_IRQn )     , /* Active      CPUSS DataWire #0, Channel #14                        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw0_15_IRQn )     , /* Active      CPUSS DataWire #0, Channel #15                        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw1_0_IRQn )      , /* Active      CPUSS DataWire #1, Channel #0                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw1_1_IRQn )      , /* Active      CPUSS DataWire #1, Channel #1                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw1_2_IRQn )      , /* Active      CPUSS DataWire #1, Channel #2                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw1_3_IRQn )      , /* Active      CPUSS DataWire #1, Channel #3                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw1_4_IRQn )      , /* Active      CPUSS DataWire #1, Channel #4                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw1_5_IRQn )      , /* Active      CPUSS DataWire #1, Channel #5                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw1_6_IRQn )      , /* Active      CPUSS DataWire #1, Channel #6                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw1_7_IRQn )      , /* Active      CPUSS DataWire #1, Channel #7                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw1_8_IRQn )      , /* Active      CPUSS DataWire #1, Channel #8                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw1_9_IRQn )      , /* Active      CPUSS DataWire #1, Channel #9                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw1_10_IRQn )     , /* Active      CPUSS DataWire #1, Channel #10                        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw1_11_IRQn )     , /* Active      CPUSS DataWire #1, Channel #11                        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw1_12_IRQn )     , /* Active      CPUSS DataWire #1, Channel #12                        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw1_13_IRQn )     , /* Active      CPUSS DataWire #1, Channel #13                        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw1_14_IRQn )     , /* Active      CPUSS DataWire #1, Channel #14                        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_dw1_15_IRQn )     , /* Active      CPUSS DataWire #1, Channel #15                        */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_fault_0_IRQn )    , /* Active      CPUSS Fault Structure Interrupt #0                    */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_fault_1_IRQn )    , /* Active      CPUSS Fault Structure Interrupt #1                    */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupt_crypto_IRQn )      , /* Active      CRYPTO Accelerator Interrupt                          */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupt_fm_IRQn )          , /* Active      FLASH Macro Interrupt                                 */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_cm0_cti_0_IRQn )  , /* Active      CM0+ CTI #0                                           */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_cm0_cti_1_IRQn )  , /* Active      CM0+ CTI #1                                           */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_cm4_cti_0_IRQn )  , /* Active      CM4 CTI #0                                            */
    INTERRUPT_VECTOR_TABLE_ENTRY( cpuss_interrupts_cm4_cti_1_IRQn )  , /* Active      CM4 CTI #1                                            */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_0_interrupts_0_IRQn )        , /* Active      TCPWM #0, Counter #0                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_0_interrupts_1_IRQn )        , /* Active      TCPWM #0, Counter #1                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_0_interrupts_2_IRQn )        , /* Active      TCPWM #0, Counter #2                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_0_interrupts_3_IRQn )        , /* Active      TCPWM #0, Counter #3                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_0_interrupts_4_IRQn )        , /* Active      TCPWM #0, Counter #4                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_0_interrupts_5_IRQn )        , /* Active      TCPWM #0, Counter #5                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_0_interrupts_6_IRQn )        , /* Active      TCPWM #0, Counter #6                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_0_interrupts_7_IRQn )        , /* Active      TCPWM #0, Counter #7                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_0_IRQn )        , /* Active      TCPWM #1, Counter #0                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_1_IRQn )        , /* Active      TCPWM #1, Counter #1                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_2_IRQn )        , /* Active      TCPWM #1, Counter #2                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_3_IRQn )        , /* Active      TCPWM #1, Counter #3                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_4_IRQn )        , /* Active      TCPWM #1, Counter #4                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_5_IRQn )        , /* Active      TCPWM #1, Counter #5                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_6_IRQn )        , /* Active      TCPWM #1, Counter #6                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_7_IRQn )        , /* Active      TCPWM #1, Counter #7                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_8_IRQn )        , /* Active      TCPWM #1, Counter #8                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_9_IRQn )        , /* Active      TCPWM #1, Counter #9                                  */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_10_IRQn )       , /* Active      TCPWM #1, Counter #10                                 */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_11_IRQn )       , /* Active      TCPWM #1, Counter #11                                 */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_12_IRQn )       , /* Active      TCPWM #1, Counter #12                                 */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_13_IRQn )       , /* Active      TCPWM #1, Counter #13                                 */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_14_IRQn )       , /* Active      TCPWM #1, Counter #14                                 */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_15_IRQn )       , /* Active      TCPWM #1, Counter #15                                 */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_16_IRQn )       , /* Active      TCPWM #1, Counter #16                                 */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_17_IRQn )       , /* Active      TCPWM #1, Counter #17                                 */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_18_IRQn )       , /* Active      TCPWM #1, Counter #18                                 */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_19_IRQn )       , /* Active      TCPWM #1, Counter #19                                 */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_20_IRQn )       , /* Active      TCPWM #1, Counter #20                                 */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_21_IRQn )       , /* Active      TCPWM #1, Counter #21                                 */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_22_IRQn )       , /* Active      TCPWM #1, Counter #22                                 */
    INTERRUPT_VECTOR_TABLE_ENTRY( tcpwm_1_interrupts_23_IRQn )       , /* Active      TCPWM #1, Counter #23                                 */
    INTERRUPT_VECTOR_TABLE_ENTRY( udb_interrupts_0_IRQn )            , /* Active      UDB Interrupt #0                                      */
    INTERRUPT_VECTOR_TABLE_ENTRY( udb_interrupts_1_IRQn )            , /* Active      UDB Interrupt #1                                      */
    INTERRUPT_VECTOR_TABLE_ENTRY( udb_interrupts_2_IRQn )            , /* Active      UDB Interrupt #2                                      */
    INTERRUPT_VECTOR_TABLE_ENTRY( udb_interrupts_3_IRQn )            , /* Active      UDB Interrupt #3                                      */
    INTERRUPT_VECTOR_TABLE_ENTRY( udb_interrupts_4_IRQn )            , /* Active      UDB Interrupt #4                                      */
    INTERRUPT_VECTOR_TABLE_ENTRY( udb_interrupts_5_IRQn )            , /* Active      UDB Interrupt #5                                      */
    INTERRUPT_VECTOR_TABLE_ENTRY( udb_interrupts_6_IRQn )            , /* Active      UDB Interrupt #6                                      */
    INTERRUPT_VECTOR_TABLE_ENTRY( udb_interrupts_7_IRQn )            , /* Active      UDB Interrupt #7                                      */
    INTERRUPT_VECTOR_TABLE_ENTRY( udb_interrupts_8_IRQn )            , /* Active      UDB Interrupt #8                                      */
    INTERRUPT_VECTOR_TABLE_ENTRY( udb_interrupts_9_IRQn )            , /* Active      UDB Interrupt #9                                      */
    INTERRUPT_VECTOR_TABLE_ENTRY( udb_interrupts_10_IRQn )           , /* Active      UDB Interrupt #10                                     */
    INTERRUPT_VECTOR_TABLE_ENTRY( udb_interrupts_11_IRQn )           , /* Active      UDB Interrupt #11                                     */
    INTERRUPT_VECTOR_TABLE_ENTRY( udb_interrupts_12_IRQn )           , /* Active      UDB Interrupt #12                                     */
    INTERRUPT_VECTOR_TABLE_ENTRY( udb_interrupts_13_IRQn )           , /* Active      UDB Interrupt #13                                     */
    INTERRUPT_VECTOR_TABLE_ENTRY( udb_interrupts_14_IRQn )           , /* Active      UDB Interrupt #14                                     */
    INTERRUPT_VECTOR_TABLE_ENTRY( udb_interrupts_15_IRQn )           , /* Active      UDB Interrupt #15                                     */
    INTERRUPT_VECTOR_TABLE_ENTRY( pass_interrupt_sar_IRQn )          , /* Active      SAR ADC interrupt                                     */
    INTERRUPT_VECTOR_TABLE_ENTRY( audioss_interrupt_i2s_IRQn )       , /* Active      I2S Audio interrupt                                   */
    INTERRUPT_VECTOR_TABLE_ENTRY( audioss_interrupt_pdm_IRQn )       , /* Active      PDM/PCM Audio interrupt                               */
    INTERRUPT_VECTOR_TABLE_ENTRY( profile_interrupt_IRQn )           , /* Active      Energy Profiler interrupt                             */
    INTERRUPT_VECTOR_TABLE_ENTRY( smif_interrupt_IRQn )              , /* Active      Serial Memory Interface interrupt                     */
    INTERRUPT_VECTOR_TABLE_ENTRY( usb_interrupt_hi_IRQn )            , /* Active      USB Interrupt                                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( usb_interrupt_med_IRQn )           , /* Active      USB Interrupt                                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( usb_interrupt_lo_IRQn )            , /* Active      USB Interrupt                                         */
    INTERRUPT_VECTOR_TABLE_ENTRY( pass_interrupt_dacs_IRQn )         , /* Active      Consolidated interrupt for all DACs                   */
};

/******************************************************
 *               Function Definitions
 ******************************************************/
