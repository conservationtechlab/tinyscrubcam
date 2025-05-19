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
 *
 */
#include "stdint.h"
#include "string.h"
#include "platform_peripheral.h"
#include "platform_isr.h"
#include "platform_isr_interface.h"
#include "wwd_rtos.h"
#include "wwd_assert.h"
#include "wiced_platform.h"


/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define PLATFORM_PWM_PERI_CLOCK_FREQ            ( 100000000 ) /* Peripheral Clock Frequency */

#ifndef PLATFORM_PWM_CLOCK_FREQ
#define PLATFORM_PWM_CLOCK_FREQ                 ( 4000000 )   /* Desired PWM block input frequency */
#endif  /* PLATFORM_PWM_CLOCK_FREQ */

#define PLATFORM_PWM_MAX_FREQ                   ( PLATFORM_PWM_CLOCK_FREQ / 4 )  /* Max PWM frequency */
#define PLATFORM_PWM_MAX_DUTY_CYCLE             ( 100 )
#define PLATFORM_PWM_DIVIDER_COUNT              ( ( PLATFORM_PWM_PERI_CLOCK_FREQ / PLATFORM_PWM_CLOCK_FREQ ) - 1 )
#define PLATFORM_PWM_DIVIDER_FRAC_COUNT         ( 0 )         /* Not using Fractional Counter */

#define PLATFORM_PSOC_PWM_MAX_BLOCK             ( 1 )
#define PLATFORM_PSOC_PWM_BLOCK_0_MAX_CNTR      ( 7 )
#define PLATFORM_PSOC_PWM_BLOCK_1_MAX_CNTR      ( 23 )
#define PLATFORM_PSOC_PWM_BLOCK0_MAX_COUNT      ( 0xffffffff )
#define PLATFORM_PSOC_PWM_BLOCK1_MAX_COUNT      ( 0xffff )
#define PLATFORM_PSOC_PWM_BLOCK_MAX_DIV_COUNT   ( 128 )

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
static uint32_t counter_div_8_used_mask;
static uint32_t counter_div_16_used_mask;
static uint32_t counter_div_16_5_used_mask;
static uint32_t counter_div_24_5_used_mask;

/******************************************************
 *               Function Definitions
 ******************************************************/

static platform_result_t platform_validate_pwm( const platform_pwm_t* pwm, TCPWM_Type** base )
{
    if ( pwm == NULL )
    {
        return ( PLATFORM_BADARG );
    }

    if ( base == NULL )
    {
        return ( PLATFORM_BADARG );
    }

    if ( pwm -> block > PLATFORM_PSOC_PWM_MAX_BLOCK )
    {
        return ( PLATFORM_BADARG );
    }

    if ( pwm -> block == 1 )
    {
        if ( pwm -> cntnum > PLATFORM_PSOC_PWM_BLOCK_1_MAX_CNTR )
        {
            return ( PLATFORM_BADARG );
        }
        *base = TCPWM1;
    }
    else
    {
        if ( pwm -> cntnum > PLATFORM_PSOC_PWM_BLOCK_0_MAX_CNTR )
        {
            return ( PLATFORM_BADARG );
        }
        *base = TCPWM0;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t platform_pwm_clock_init( const platform_pwm_t* pwm )
{
    uint32_t divider_type, divider_cntr;
    uint32_t* counter_use_mask;
    platform_result_t result;

    if ( pwm == NULL )
    {
        return ( PLATFORM_BADARG );
    }

    divider_type = pwm -> pclk -> divider_type;
    divider_cntr = pwm -> pclk -> divider_num;

    switch ( divider_type )
    {
        case CY_SYSCLK_DIV_8_BIT:
             counter_use_mask = &counter_div_8_used_mask;
             break;
        case CY_SYSCLK_DIV_16_BIT:
             counter_use_mask = &counter_div_16_used_mask;
             break;
        case CY_SYSCLK_DIV_16_5_BIT:
             counter_use_mask = &counter_div_16_5_used_mask;
             break;
        case CY_SYSCLK_DIV_24_5_BIT:
             counter_use_mask = &counter_div_24_5_used_mask;
             break;
        default:
             return ( PLATFORM_BADARG );
    }

    /* Do Not reinitialize a Divider in Use */
    if ( *counter_use_mask & ( 1 << divider_cntr ) )
    {
        /* Do not reinitialize the counter but assign it to destination */
        Cy_SysClk_PeriphAssignDivider( pwm -> pclk ->clock_dest, pwm -> pclk ->divider_type,
                                       pwm -> pclk ->divider_num );
        return PLATFORM_SUCCESS;
    }

    result = platform_peripheral_clock_init( pwm->pclk, PLATFORM_PWM_DIVIDER_COUNT, PLATFORM_PWM_DIVIDER_FRAC_COUNT );

    if ( result != PLATFORM_SUCCESS )
    {
        return ( result );
    }

    *counter_use_mask |= ( 1 << divider_cntr );

    return PLATFORM_SUCCESS;
}

static platform_result_t platform_pwm_calc_prescaler_per_comp( uint32_t block, uint32_t frequency, float duty_cycle,
                                    uint32_t is_deadtime_mode, uint32_t *period, uint32_t *compare, uint32_t *pwm_div_cnt  )
{
    uint32_t calc_period;
    uint32_t calc_compare;
    uint32_t calc_div_cnt;
    uint32_t calc_pwm_div_cnt;

    if ( PLATFORM_PWM_CLOCK_FREQ > PLATFORM_PWM_PERI_CLOCK_FREQ )
    {
        return ( PLATFORM_BADARG );
    }

    if ( frequency > PLATFORM_PWM_MAX_FREQ )
    {
        return ( PLATFORM_BADARG );
    }

    if ( duty_cycle > PLATFORM_PWM_MAX_DUTY_CYCLE )
    {
        return ( PLATFORM_BADARG );
    }

    calc_period = PLATFORM_PWM_CLOCK_FREQ / frequency;

    if ( block == 0 )
    {
        calc_div_cnt = calc_period / PLATFORM_PSOC_PWM_BLOCK0_MAX_COUNT;
        if ( ( calc_period % PLATFORM_PSOC_PWM_BLOCK0_MAX_COUNT ) != 0 )
        {
            calc_div_cnt++;
        }
    }
    else
    {
        calc_div_cnt = calc_period / PLATFORM_PSOC_PWM_BLOCK1_MAX_COUNT;
        if ( ( calc_period % PLATFORM_PSOC_PWM_BLOCK1_MAX_COUNT ) != 0 )
        {
            calc_div_cnt++;
        }
    }

    /* Dead time mode cannot use prescaler */
    if ( is_deadtime_mode && ( calc_div_cnt > 1 ) )
    {
        return ( PLATFORM_BADARG );
    }

    if ( calc_div_cnt > 128 )
    {
        return ( PLATFORM_ERROR );
    } else if ( calc_div_cnt > 64 )
    {
       calc_pwm_div_cnt = CY_TCPWM_PWM_PRESCALER_DIVBY_128;
       calc_div_cnt = 128;
    } else if ( calc_div_cnt > 32 )
    {
       calc_pwm_div_cnt = CY_TCPWM_PWM_PRESCALER_DIVBY_64;
       calc_div_cnt = 64;
    } else if ( calc_div_cnt > 16 )
    {
       calc_pwm_div_cnt = CY_TCPWM_PWM_PRESCALER_DIVBY_32;
       calc_div_cnt = 32;
    } else if ( calc_div_cnt > 8 )
    {
       calc_pwm_div_cnt = CY_TCPWM_PWM_PRESCALER_DIVBY_16;
       calc_div_cnt = 16;
    } else if ( calc_div_cnt > 4 )
    {
       calc_pwm_div_cnt = CY_TCPWM_PWM_PRESCALER_DIVBY_8;
       calc_div_cnt = 8;
    } else if ( calc_div_cnt > 2 )
    {
       calc_pwm_div_cnt = CY_TCPWM_PWM_PRESCALER_DIVBY_4;
       calc_div_cnt = 4;
    } else if ( calc_div_cnt > 1 )
    {
       calc_pwm_div_cnt = CY_TCPWM_PWM_PRESCALER_DIVBY_2;
       calc_div_cnt = 2;
    } else
    {
       calc_pwm_div_cnt = CY_TCPWM_PWM_PRESCALER_DIVBY_1;
    }

    *pwm_div_cnt = calc_pwm_div_cnt;

    calc_period = calc_period / calc_div_cnt;

    calc_compare = (uint32_t) ( ( ( calc_period * duty_cycle ) / 100.0f ) + 0.5f );

    *period = calc_period;
    *compare = calc_compare;

    return PLATFORM_SUCCESS;
}

platform_result_t platform_pwm_init( const platform_pwm_t* pwm, uint32_t frequency, float duty_cycle )
{
    TCPWM_Type* base;
    cy_en_tcpwm_status_t status;
    cy_stc_tcpwm_pwm_config_t config = {'\0'};
    platform_result_t result;

    wiced_assert( "bad argument", pwm != NULL );

    result = platform_validate_pwm( pwm, &base );

    if ( result != PLATFORM_SUCCESS )
    {
        return ( result );
    }

    if ( PLATFORM_PWM_CLOCK_FREQ > PLATFORM_PWM_PERI_CLOCK_FREQ )
    {
        return ( PLATFORM_BADARG );
    }

    result =  platform_pwm_clock_init( pwm );

    if ( result != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    result = platform_pwm_calc_prescaler_per_comp( pwm -> block, frequency, duty_cycle, pwm -> dead_clocks,
                                                   &config.period0, &config.compare0, &config.clockPrescaler );

    if ( result != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    if ( pwm -> dead_clocks )
    {
        config.pwmMode = CY_TCPWM_PWM_MODE_DEADTIME;
        config.deadTimeClocks = pwm -> dead_clocks;
    }
    else
    {
        config.pwmMode = CY_TCPWM_PWM_MODE_PWM;
    }

    if ( pwm -> alignment > CY_TCPWM_PWM_ASYMMETRIC_ALIGN )
    {
        return ( PLATFORM_BADARG );
    }
    config.pwmAlignment = pwm -> alignment;

    if ( pwm -> is_single_shot )
    {
        config.runMode = CY_TCPWM_PWM_ONESHOT;
    }
    else
    {
        config.runMode = CY_TCPWM_PWM_CONTINUOUS;
    }

    config.enablePeriodSwap = 0;
    config.enableCompareSwap = 0;
    config.interruptSources = 0;

    config.invertPWMOut = pwm -> invert;
    config.invertPWMOutN = pwm -> invert;

    /* Values from creator */
    /* setting the CY_TCPWM_INPUT_CREATOR leaves the setting to default power on reset value */
    config.killMode = 2;
    config.swapInputMode = 3UL;
    config.swapInput = CY_TCPWM_INPUT_CREATOR;
    config.reloadInputMode = 3UL;
    config.reloadInput = CY_TCPWM_INPUT_CREATOR;
    config.startInputMode = 3UL;
    config.startInput = CY_TCPWM_INPUT_CREATOR;
    config.killInputMode = 3UL;
    config.killInput = CY_TCPWM_INPUT_CREATOR;
    config.countInputMode = 3UL;
    config.countInput = CY_TCPWM_INPUT_CREATOR;

    status =  Cy_TCPWM_PWM_Init( base, pwm -> cntnum,  &config );

    if ( pwm -> out_pin )
    {
        GPIO_PRT_Type *port_base;
        port_base = Cy_GPIO_PortToAddr( pwm->out_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, pwm->out_pin->pin_num, CY_GPIO_DM_STRONG_IN_OFF,
                              0x1, pwm->hsiom );
    }

    platform_mcu_powersave_enable();

    if ( status != CY_TCPWM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}

platform_result_t platform_pwm_start( const platform_pwm_t* pwm )
{
    TCPWM_Type* base;
    platform_result_t result;

    wiced_assert( "bad argument", pwm != NULL );

    result = platform_validate_pwm( pwm, &base );

    if ( result != PLATFORM_SUCCESS )
    {
        return ( result );
    }

    Cy_TCPWM_PWM_Enable( base, pwm -> cntnum );

    Cy_TCPWM_TriggerStart(base, ( 1 << pwm -> cntnum ) );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_pwm_stop( const platform_pwm_t* pwm )
{
    TCPWM_Type* base;
    platform_result_t result;

    wiced_assert( "bad argument", pwm != NULL );

    result = platform_validate_pwm( pwm, &base );

    if ( result != PLATFORM_SUCCESS )
    {
        return ( result );
    }

    platform_mcu_powersave_disable();

    Cy_TCPWM_PWM_Disable( base, pwm -> cntnum );

    platform_mcu_powersave_enable();

    return PLATFORM_SUCCESS;
}
