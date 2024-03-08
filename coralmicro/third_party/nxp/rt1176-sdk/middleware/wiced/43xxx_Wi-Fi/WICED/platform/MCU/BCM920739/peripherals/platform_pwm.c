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
#include <stdint.h>

#include "platform_peripheral.h"
#include "platform.h"
#include "wwd_assert.h"
#include "wiced_rtos.h"
#include "brcm_fw_types.h"
#include "wiced_hal_gpio.h"
#include "wiced_hal_aclk.h"
#include "wiced_hal_pwm.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define PWM_INT_COUNTER             ((uint32_t)0x10000)
#define PWM_TOGGLE_COUNTER          ((uint32_t)0xFFFF)

/*This will be the max PWM freq we can generate
 * we have set it to 256KHz now ,can be configured as desired*/
#define PWM_BASE_CLK    ((uint32_t)256000)

#define PWM_OUT_NOINVERT            (0)                 /*0-OFF,1-Invert*/
#define PWM_OUT_INVERT              (1)                 /*0-OFF,1-Invert*/
#define PWM_DISABLE                 ((uint8_t)0)
#define PWM_ENABLE                  ((uint8_t)1)
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
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/
static uint32_t pwm_init_cnt[WICED_PWM_MAX];
static uint32_t pwm_toggle_cnt[WICED_PWM_MAX];
static uint8_t pwm_status[WICED_PWM_MAX];
/******************************************************
 *               Function Definitions
 ******************************************************/

static wiced_mutex_t pwm_aclk_mutex;
static uint32_t pwm_aclk_count;
static wiced_bool_t pwm_aclk_state = WICED_FALSE;

/*This should be called in the platform init stage once*/
platform_result_t platform_pwm_clk_init(void)
{
    if(WICED_SUCCESS != wiced_rtos_init_mutex(&pwm_aclk_mutex))
    {
        WPRINT_PLATFORM_ERROR(("PWM clk mutex init failed\n"));
        return PLATFORM_ERROR;
    }
    return PLATFORM_SUCCESS;
}

/*enable the ACLK, keep use count
 * PWM BASE CLK is the clk freq used by the PWM */
void pwm_clk_enable(void)
{
  wiced_rtos_lock_mutex(&pwm_aclk_mutex);

    if (pwm_aclk_count == 0)
    {
        wiced_hal_aclk_enable(PWM_BASE_CLK, ACLK1, ACLK_FREQ_24_MHZ);
        pwm_aclk_state = WICED_TRUE;
    }
    pwm_aclk_count++;
  wiced_rtos_unlock_mutex(&pwm_aclk_mutex);
}

/*Disable the PWM base clock*/
void pwm_clk_disable(void)
{
    wiced_rtos_lock_mutex(&pwm_aclk_mutex);

    if (pwm_aclk_state == WICED_FALSE)
        return;

    pwm_aclk_count--;
    if (pwm_aclk_count == 0)
    {
        wiced_hal_aclk_disable(ACLK1);
        pwm_aclk_state = WICED_FALSE;
    }
    wiced_rtos_unlock_mutex(&pwm_aclk_mutex);
}

/* we first configure the PWM pin as gpio output, then calculate
 * the toggle and init count to be set based on the requested
 * freq and duty cycle, as described below
 *
 *         pwm#_init_cnt = 0x10000 - (pwm# clock source frequency/pwm# frequency)
 *        pwm#_togg_cnt = 0xFFFF - pwm# duty cycle*(0x10000 - pwm#_init_cnt)
 *
 * Note: we must have done wiced_hal_gpio_init() before using the PWM as we use
 * the GPIO configure to set the pin in output mode.
 */
platform_result_t
platform_pwm_init( const platform_pwm_t* pwm, uint32_t frequency, float duty_cycle )
{
    /*if GPIO info is NULL,this PWM ch is not usable
     * pin-muxed for other use*/
    if (pwm->pin == NULL )
        return PLATFORM_ERROR;
    /*configure the PMW GPIO pin as output*/
    if(pwm_status[pwm->channel] == PWM_DISABLE)
        wiced_hal_gpio_configure_pin( pwm->pin->pin_number, GPIO_OUTPUT_ENABLE, pwm->invert );
    /*Calculate the init and toggle count*/
    if(duty_cycle > 100)
        duty_cycle = 100.0;
    else if(duty_cycle < 0)
        duty_cycle = 0;
    duty_cycle = duty_cycle/100; /*duty cycle in %*/

    pwm_init_cnt[pwm->channel] = PWM_INT_COUNTER - (uint32_t)(PWM_BASE_CLK/frequency);
    pwm_toggle_cnt[pwm->channel] = PWM_TOGGLE_COUNTER - (uint32_t)(duty_cycle*(PWM_INT_COUNTER - pwm_init_cnt[pwm->channel]));

    return PLATFORM_SUCCESS;
}

platform_result_t
platform_pwm_start( const platform_pwm_t* pwm )
{
    wiced_assert( "bad argument", pwm != NULL );
    if(pwm_status[pwm->channel] == PWM_DISABLE)
    {
    pwm_clk_enable();
    wiced_hal_pwm_start(pwm->channel, PMU_CLK, pwm_toggle_cnt[pwm->channel], pwm_init_cnt[pwm->channel], pwm->invert);
        pwm_status[pwm->channel] = PWM_ENABLE;
    }
    else
    {
        wiced_hal_pwm_change_values(pwm->channel, pwm_toggle_cnt[pwm->channel], pwm_init_cnt[pwm->channel]);
    }
    return PLATFORM_SUCCESS;
}

platform_result_t
platform_pwm_stop( const platform_pwm_t* pwm )
{
    wiced_assert( "bad argument", pwm != NULL );
    if(pwm_status[pwm->channel] == PWM_ENABLE)
    {
    wiced_hal_pwm_disable(pwm->channel);
    pwm_clk_disable();
        pwm_status[pwm->channel] = PWM_DISABLE;
    }
    return PLATFORM_SUCCESS;
}
