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
 * Define default STM32L4xx initialisation functions
 */
#include "platform_init.h"
#include "platform_isr.h"
#include "platform_peripheral.h"
#include "platform_sleep.h"
#include "platform_config.h"
#include "platform_toolchain.h"
#include "platform/wwd_platform_interface.h"

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

#if !defined ( BOOTLOADER )
static void SystemClock_Config( void )
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType      = CONFIG_RCC_OSCILLATOR_SOURCE;
    RCC_OscInitStruct.MSIState            = CONFIG_RCC_MSI_STATE;
    RCC_OscInitStruct.LSIState            = CONFIG_RCC_LSI_STATE;
    RCC_OscInitStruct.HSI48State          = CONFIG_RCC_HSI48_STATE;;
    RCC_OscInitStruct.MSICalibrationValue = CONFIG_RCC_MSI_CALIBRATION_VALUE;
    RCC_OscInitStruct.MSIClockRange       = CONFIG_RCC_MSI_CLOCK_RANGE;
    RCC_OscInitStruct.PLL.PLLState        = CONFIG_RCC_PLL_STATE;
    RCC_OscInitStruct.PLL.PLLSource       = CONFIG_RCC_PLL_SOURCE;
    RCC_OscInitStruct.PLL.PLLM            = CONFIG_RCC_PLL_M;
    RCC_OscInitStruct.PLL.PLLN            = CONFIG_RCC_PLL_N;
    RCC_OscInitStruct.PLL.PLLP            = CONFIG_RCC_PLL_P;
    RCC_OscInitStruct.PLL.PLLQ            = CONFIG_RCC_PLL_Q;
    RCC_OscInitStruct.PLL.PLLR            = CONFIG_RCC_PLL_R;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        while(1);
    }

    RCC_ClkInitStruct.ClockType      = CONFIG_RCC_CLOCKTYPE;
    RCC_ClkInitStruct.SYSCLKSource   = CONFIG_RCC_SYSCLKSOURCE;
    RCC_ClkInitStruct.AHBCLKDivider  = CONFIG_RCC_AHB_CLKDIVIDER;
    RCC_ClkInitStruct.APB1CLKDivider = CONFIG_RCC_APB1_CLKDIVIDER;
    RCC_ClkInitStruct.APB2CLKDivider = CONFIG_RCC_APB2_CLKDIVIDER;

    if ( HAL_RCC_ClockConfig( &RCC_ClkInitStruct, CONFIG_FLASH_LATENCY ) != HAL_OK )
    {
        /* Initialization Error */
        while ( 1 );
    }

     PeriphClkInit.PeriphClockSelection = CONFIG_RCC_PERIPHCLK_SELECTION;
     PeriphClkInit.Usart1ClockSelection = CONFIG_USART1_CLOCK_SELECTION;
     PeriphClkInit.Sdmmc1ClockSelection = CONFIG_SDMMC1_CLOCK_SELECTION;

    if ( HAL_RCCEx_PeriphCLKConfig( &PeriphClkInit ) != HAL_OK )
    {
        /* Initialization Error */
        while ( 1 );
    }

    /* HAL_RCC_ClockConfig() is not called, call below two functions to update
    * STM32 HAL Global variables.
    */

    /* Update SystemCoreClock in STM32 HAL */
    SystemCoreClockUpdate( );

    /* Update SystemD2Clock in STM32 HAL */
    HAL_RCC_GetHCLKFreq( );

    /* Configure the main internal regulator output voltage */
    if ( HAL_PWREx_ControlVoltageScaling( PWR_REGULATOR_VOLTAGE_SCALE1 ) != HAL_OK )
    {
        /* Initialization Error */
        while ( 1 );
    }

    /* Configure the Systick interrupt time */
    HAL_SYSTICK_Config( SystemCoreClock / 1000 );

    /* Configure the Systick */
    HAL_SYSTICK_CLKSourceConfig( SYSTICK_CLKSOURCE_HCLK );
}

#endif /* !defined ( BOOTLOADER ) */

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

void platform_mcu_reset( void )
{
    NVIC_SystemReset( );

    /* Loop forever */
    while ( 1 )
    {
    }
}

/* STM32L4 common clock initialization function
 * This brings up enough clocks to allow the processor to run quickly while initializing memory.
 */
WEAK void platform_init_system_clocks( void )
{

}

WEAK void platform_init_memory( void )
{

}

void platform_init_mcu_infrastructure( void )
{
    uint8_t i;

#if !defined ( BOOTLOADER )

    /* Initialize watchdog */
    platform_watchdog_init( );

    __HAL_RCC_SYSCFG_CLK_ENABLE( );
    __HAL_RCC_PWR_CLK_ENABLE( );

    HAL_Init( );
    SystemClock_Config( );

#endif /* !defined ( BOOTLOADER ) */

#ifdef INTERRUPT_VECTORS_IN_RAM
    SCB->VTOR = 0x20000000; /* Change the vector table to point to start of SRAM */
#endif /* ifdef INTERRUPT_VECTORS_IN_RAM */

    /* Initialise interrupt priorities */
    for ( i = 0; i < 91; i++ )
    {
        NVIC_SetPriority( (IRQn_Type) i, 0xf );
    }
    NVIC_SetPriorityGrouping( NVIC_PRIORITYGROUP_4 );
    platform_init_rtos_irq_priorities( );
    platform_init_peripheral_irq_priorities( );

    /* Initialise GPIO IRQ manager */
    platform_gpio_irq_manager_init( );

#ifndef WICED_DISABLE_MCU_POWERSAVE
    /* Initialise MCU powersave */
    platform_mcu_powersave_init( );
    platform_mcu_powersave_disable( );

    /* Initialize RTC */
    platform_rtc_init( );

#endif /* ifndef WICED_DISABLE_MCU_POWERSAVE */
}

void platform_init_connectivity_module( void )
{
    /* Ensure 802.11 device is in reset. */
    host_platform_init( );
}

WEAK void platform_init_external_devices( void )
{

}
