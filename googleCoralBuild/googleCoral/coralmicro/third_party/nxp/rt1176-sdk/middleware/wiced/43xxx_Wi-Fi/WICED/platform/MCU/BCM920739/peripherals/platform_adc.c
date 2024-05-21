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

/** @file BCM920739B0 platform ADC
 *
 */
#include "stdint.h"
#include "string.h"
#include "platform_peripheral.h"
#include "wwd_assert.h"
#include "brcm_fw_types.h"
#include "wiced_hal_adc.h"
#include "wiced_hal_gpio.h"
#include "wiced_rtos.h"
#include "wiced_bt_trace.h"
/******************************************************
 *                      Macros
 ******************************************************/
#define ADC_CH_ADD (1)
#define ADC_CH_REMOVE (-1)
#define ADC_MAX_STEP_VAL ((float)65535)
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

/******************************************************
 *               Variable Definitions
 ******************************************************/
static wiced_bool_t platform_adcinit = WICED_FALSE;
static wiced_mutex_t adc_mutex;
static uint16_t adc_usecount;
static ADC_INPUT_RANGE_SEL adc_in_range = ADC_RANGE_0_3P6V;
static float adc_step_factor;
/*ADC input vol levels in mvol (indexed with ADC_INPUT_RANGE_SEL)*/
static uint16_t adc_ref_vol[] = {3600,1800};
/******************************************************
 *               Function Definitions
 ******************************************************/
/*ADC raw samples returned from the FW2 HAL code is not calibrated
 * however the voltage values returned are calibrated, hence we read the
 * voltage first and then convert it to ADC step value */
static void adc_calibrate(void)
{
    adc_step_factor = ADC_MAX_STEP_VAL/adc_ref_vol[adc_in_range];
}

static uint16_t adc_channel_count(int16_t count)
{
   wiced_rtos_lock_mutex(&adc_mutex);
   adc_usecount+=count;
   wiced_rtos_unlock_mutex(&adc_mutex);
   return adc_usecount;
}
/*
 * Set the ADC input voltage range,This needs to be called from the
 * platform init code once,before using any ADC CH,
 * Valid values ADC_RANGE_0_1P8V,ADC_RANGE_0_3P6V
 */
void platform_adc_setinput_range(PLATFORM_ADC_VOLRANGE_SEL range)
{
    adc_in_range = range;
}
/*
 * 20739 HAL does not support sample_cycle config.
 *
 * */
platform_result_t platform_adc_init( const platform_adc_t* adc, uint32_t sample_cycle )
{
    UNUSED_PARAMETER( sample_cycle );

    /*if GPIO info is NULL,this ADC ch is not usable
     * pin-muxed for other use*/
    if (adc->pin == NULL )
        return PLATFORM_INIT_FAIL;

    wiced_hal_gpio_configure_pin( adc->pin->pin_number, GPIO_INPUT_ENABLE, GPIO_PIN_OUTPUT_LOW );
    if(platform_adcinit == WICED_FALSE)
    {
        if(WICED_SUCCESS != wiced_rtos_init_mutex(&adc_mutex))
            return PLATFORM_ERROR;
        adc_usecount = 0;
        wiced_hal_adc_init();
        //wiced_hal_adc_set_power(ADC_POWER_UP);
        wiced_hal_adc_set_input_range(adc_in_range);
        adc_calibrate();
        platform_adcinit = WICED_TRUE;
    }
    adc_channel_count(ADC_CH_ADD);

    return PLATFORM_SUCCESS;
}
/* we just power down the ADC here*/
platform_result_t platform_adc_deinit( const platform_adc_t* adc )
{
    UNUSED_PARAMETER( adc );

    if(platform_adcinit == WICED_TRUE)
    {
        if(0 == adc_channel_count(ADC_CH_REMOVE))
        {
            platform_adcinit = WICED_FALSE;
            wiced_rtos_deinit_mutex(&adc_mutex);
        }
        return PLATFORM_SUCCESS;
    }
    return PLATFORM_UNINITLIASED;
}

platform_result_t platform_adc_take_sample( const platform_adc_t* adc, uint16_t* output )
{
    wiced_assert( "output is NULL", output != NULL );
    uint32_t adc_vol;
    /*if GPIO info is NULL,this ADC ch is not usable
     * pin-muxed for other use*/
    if (adc->pin == NULL )
        return PLATFORM_ERROR;

    if(platform_adcinit == WICED_FALSE)
        return PLATFORM_UNINITLIASED;

    adc_vol = wiced_hal_adc_read_voltage((ADC_INPUT_CHANNEL_SEL)adc->channel);
    *output = (uint16_t)(adc_vol*adc_step_factor);

    return PLATFORM_SUCCESS;
}

platform_result_t platform_adc_take_sample_stream( const platform_adc_t* adc, void* buffer, uint16_t buffer_length )
{
    /*buffer_length is in bytes,we read 16 bit samples*/
    uint32_t adc_vol;
    uint16_t read_size = buffer_length>>1;
    uint16_t *read_buffer = (uint16_t*)buffer;

    /*if GPIO info is NULL,this ADC ch is not usable
     * pin-muxed for other use*/
    if (adc->pin == NULL )
        return PLATFORM_ERROR;

    if(platform_adcinit == WICED_FALSE)
        return PLATFORM_UNINITLIASED;

    while(read_size)
    {
        adc_vol = wiced_hal_adc_read_voltage((ADC_INPUT_CHANNEL_SEL)adc->channel);
        *read_buffer = (uint16_t)(adc_vol*adc_step_factor);
        read_size--;
        read_buffer++;
    }

    return PLATFORM_SUCCESS;
}
