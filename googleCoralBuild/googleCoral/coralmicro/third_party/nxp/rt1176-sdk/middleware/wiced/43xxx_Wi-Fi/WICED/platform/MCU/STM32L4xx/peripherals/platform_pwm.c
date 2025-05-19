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
 * STM32L4xx PWM implementation
 */
#include "platform_peripheral.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define TIM_PERIOD_MAX    (65536)
#define TIM_PRESCALER_MAX (65536)

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

/******************************************************
 *               Variable Definitions
 ******************************************************/

typedef enum
{
    PLATFORM_PWM1,
    PLATFORM_PWM2,
    PLATFORM_PWM3,
    PLATFORM_PWM4,
    PLATFORM_PWM5,
    PLATFORM_PWM6,
    PLATFORM_PWM7,
    PLATFORM_PWM8,
    PLATFORM_PWM9,
    PLATFORM_PWM_MAX,
} platform_pwm_type_t;

void hal_rcc_tim1_clock_enable( void )
{
    __HAL_RCC_TIM1_CLK_ENABLE( );
}

void hal_rcc_tim2_clock_enable( void )
{
    __HAL_RCC_TIM2_CLK_ENABLE( );
}

void hal_rcc_tim3_clock_enable( void )
{
    __HAL_RCC_TIM3_CLK_ENABLE( );
}

void hal_rcc_tim4_clock_enable( void )
{
    __HAL_RCC_TIM4_CLK_ENABLE();
}

void hal_rcc_tim5_clock_enable( void )
{
    __HAL_RCC_TIM5_CLK_ENABLE( );
}

void hal_rcc_tim8_clock_enable( void )
{
    __HAL_RCC_TIM8_CLK_ENABLE( );
}

void hal_rcc_tim15_clock_enable( void )
{
    __HAL_RCC_TIM15_CLK_ENABLE( );
}

void hal_rcc_tim16_clock_enable( void )
{
    __HAL_RCC_TIM16_CLK_ENABLE( );
}

void hal_rcc_tim17_clock_enable( void )
{
    __HAL_RCC_TIM17_CLK_ENABLE( );
}

void hal_rcc_tim1_clock_disable( void )
{
    __HAL_RCC_TIM1_CLK_DISABLE( );
}

void hal_rcc_tim2_clock_disable( void )
{
    __HAL_RCC_TIM2_CLK_DISABLE( );
}

void hal_rcc_tim3_clock_disable( void )
{
    __HAL_RCC_TIM3_CLK_DISABLE( );
}

void hal_rcc_tim4_clock_disable( void )
{
    __HAL_RCC_TIM4_CLK_DISABLE( );
}

void hal_rcc_tim5_clock_disable( void )
{
    __HAL_RCC_TIM5_CLK_DISABLE( );
}

void hal_rcc_tim8_clock_disable( void )
{
    __HAL_RCC_TIM8_CLK_DISABLE( );
}

void hal_rcc_tim15_clock_disable( void )
{
    __HAL_RCC_TIM15_CLK_DISABLE( );
}

void hal_rcc_tim16_clock_disable( void )
{
    __HAL_RCC_TIM16_CLK_DISABLE( );
}

void hal_rcc_tim17_clock_disable( void )
{
    __HAL_RCC_TIM17_CLK_DISABLE( );
}

const platform_pwm_clock_enable_function_t pwm_clock_enable_function[ NUMBER_OF_PWM_PORTS ] =
{
    [ PLATFORM_PWM1 ]  = hal_rcc_tim1_clock_enable,
    [ PLATFORM_PWM2 ]  = hal_rcc_tim2_clock_enable,
    [ PLATFORM_PWM3 ]  = hal_rcc_tim3_clock_enable,
    [ PLATFORM_PWM4 ]  = hal_rcc_tim4_clock_enable,
    [ PLATFORM_PWM5 ]  = hal_rcc_tim5_clock_enable,
    [ PLATFORM_PWM6 ]  = hal_rcc_tim8_clock_enable,
    [ PLATFORM_PWM7 ]  = hal_rcc_tim15_clock_enable,
    [ PLATFORM_PWM8 ]  = hal_rcc_tim16_clock_enable,
    [ PLATFORM_PWM9 ]  = hal_rcc_tim17_clock_enable,
};

const platform_pwm_clock_disable_function_t pwm_clock_disable_function[ NUMBER_OF_PWM_PORTS ] =
{
    [ PLATFORM_PWM1 ]  = hal_rcc_tim1_clock_disable,
    [ PLATFORM_PWM2 ]  = hal_rcc_tim2_clock_disable,
    [ PLATFORM_PWM3 ]  = hal_rcc_tim3_clock_disable,
    [ PLATFORM_PWM4 ]  = hal_rcc_tim4_clock_disable,
    [ PLATFORM_PWM5 ]  = hal_rcc_tim5_clock_disable,
    [ PLATFORM_PWM6 ]  = hal_rcc_tim8_clock_disable,
    [ PLATFORM_PWM7 ]  = hal_rcc_tim15_clock_disable,
    [ PLATFORM_PWM8 ]  = hal_rcc_tim16_clock_disable,
    [ PLATFORM_PWM9 ]  = hal_rcc_tim17_clock_disable,
};

const uint8_t pwm_alternate_functions[ NUMBER_OF_PWM_PORTS ] =
{
    [ PLATFORM_PWM1 ]  = GPIO_AF1_TIM1,
    [ PLATFORM_PWM2 ]  = GPIO_AF1_TIM2,
    [ PLATFORM_PWM3 ]  = GPIO_AF2_TIM3,
    [ PLATFORM_PWM4 ]  = GPIO_AF2_TIM4,
    [ PLATFORM_PWM5 ]  = GPIO_AF2_TIM5,
    [ PLATFORM_PWM6 ]  = GPIO_AF3_TIM8,
    [ PLATFORM_PWM7 ]  = GPIO_AF14_TIM15,
    [ PLATFORM_PWM8 ]  = GPIO_AF14_TIM16,
    [ PLATFORM_PWM9 ]  = GPIO_AF14_TIM17,
};

/******************************************************
 *               Function Definitions
 ******************************************************/

uint8_t platform_pwm_get_port_number( platform_pwm_port_t* pwm_port )
{
    if ( pwm_port == TIM1 )
    {
        return PLATFORM_PWM1;
    }
    else if ( pwm_port == TIM2 )
    {
        return PLATFORM_PWM2;
    }
    else if ( pwm_port == TIM3 )
    {
        return PLATFORM_PWM3;
    }
    else if ( pwm_port == TIM4 )
    {
        return PLATFORM_PWM4;
    }
    else if ( pwm_port == TIM5 )
    {
        return PLATFORM_PWM5;
    }
    else if ( pwm_port == TIM8 )
    {
        return PLATFORM_PWM6;
    }
    else if ( pwm_port == TIM15 )
    {
        return PLATFORM_PWM7;
    }
    else if ( pwm_port == TIM16 )
    {
        return PLATFORM_PWM8;
    }
    else if ( pwm_port == TIM17 )
    {
        return PLATFORM_PWM9;
    }
    else
    {
        return INVALID_PWM_PORT_NUMBER;
    }
}

platform_result_t platform_pwm_get_divider( platform_pwm_port_t* pwm_port, uint32_t frequency, uint32_t* div_period, uint32_t* div_prescaler )
{
    uint32_t hclk_freq;
    uint32_t pclk_freq;
    uint64_t tim_clock;
    uint64_t pwm_freq;
    uint64_t period;
    uint64_t prescaler;
    uint64_t div_temp;

    if ( ( div_period == NULL ) || ( div_prescaler == NULL ) )
    {
        return PLATFORM_BADARG;
    }

    pwm_freq = (uint64_t) frequency;
    hclk_freq = HAL_RCC_GetHCLKFreq( );

    if ( ( pwm_port == TIM2 ) || ( pwm_port == TIM3 ) || ( pwm_port == TIM4 ) || ( pwm_port == TIM5 ) )
    {
        /* TIM2, TIM3, TIM4, TIM5 are on APB1 */
        pclk_freq = HAL_RCC_GetPCLK1Freq( );

    }
    else if ( ( pwm_port == TIM1 ) || ( pwm_port == TIM8 ) || ( pwm_port == TIM15 ) || ( pwm_port == TIM16 ) || ( pwm_port == TIM17 ) )
    {
        /* TIM1, TIM8, TIM15, TIM16, TIM17 are on APB2 */
        pclk_freq = HAL_RCC_GetPCLK2Freq( );
    }
    else
    {
        return INVALID_PWM_PORT_NUMBER;
    }

    if ( hclk_freq == pclk_freq )
    {
        tim_clock = (uint64_t) ( pclk_freq );
    }
    else
    {
        tim_clock = (uint64_t) ( 2 * pclk_freq );
    }

    if ( pwm_freq >= ( tim_clock / 2 ) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* First try with prescaler 0
     * For example for tim_clk = 80Mhz, prescaler = 0
     * max output (pwm_freq) = 40Mhz
     * min output (pwm_freq) = 80M/(TIM_PERIOD_MAX) = 1.2Khz
     */
    period = tim_clock / pwm_freq;
    if ( ( period > 0 ) && ( period <= TIM_PRESCALER_MAX ) )
    {
        *div_period    = (uint32_t) ( period - 1 );
        *div_prescaler = 0;

        return PLATFORM_SUCCESS;
    }

    /* For pwm_freq < ( tim_clk / (TIM_PERIOD_MAX) ) */
    for ( period = 100; period <= TIM_PERIOD_MAX; period += 100 )
    {
        div_temp = pwm_freq * period;

        if ( ( tim_clock % div_temp ) == 0 )
        {
            prescaler = tim_clock / div_temp;

            if ( ( prescaler > 0 ) && ( prescaler <= TIM_PRESCALER_MAX ) )
            {
                *div_period    = (uint32_t) ( period );
                *div_prescaler = (uint32_t) ( prescaler - 1 );

                return PLATFORM_SUCCESS;
            }
        }
    }

    return PLATFORM_UNSUPPORTED;
}
