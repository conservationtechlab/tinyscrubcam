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
#include "wwd_assert.h"

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

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

platform_result_t platform_adc_init( const platform_adc_t* adc, uint32_t sample_cycle )
{
    UNUSED_PARAMETER(adc);
    UNUSED_PARAMETER(sample_cycle);

    platform_mcu_powersave_disable();

    sysclk_enable_peripheral_clock( adc->peripheral_id );
    ioport_set_pin_mode( adc->adc_pin->pin, 0 );
    ioport_set_pin_dir ( adc->adc_pin->pin, IOPORT_DIR_INPUT );

    adc_init( adc->peripheral, CPU_CLOCK_HZ, adc->adc_clock_hz, 8 );

    /* Maximum track time is 16 cycles */
    if ( sample_cycle > 16 )
    {
        sample_cycle = 16;
    }

    /* Tracking time = TRACKTIM + 1 */
    sample_cycle--;
    adc_configure_timing( adc->peripheral, sample_cycle, adc->settling_time, 1 );

    adc_set_resolution( adc->peripheral, adc->resolution );

    adc_enable_channel( adc->peripheral, adc->channel );

    adc_configure_trigger( adc->peripheral, adc->trigger, 0 );

    platform_mcu_powersave_enable();


    return PLATFORM_SUCCESS;
}

platform_result_t platform_adc_deinit( const platform_adc_t* adc )
{
    UNUSED_PARAMETER(adc);

    platform_mcu_powersave_disable();

    adc_disable_channel( adc->peripheral, adc->channel );
    platform_gpio_deinit( adc->adc_pin );

    platform_mcu_powersave_enable();

    return PLATFORM_SUCCESS;
}

platform_result_t platform_adc_take_sample( const platform_adc_t* adc, uint16_t* output )
{

    uint32_t temp;

    adc_start( adc->peripheral );

    while ( ( adc_get_status( adc->peripheral ) & ADC_ISR_DRDY ) == 0 )
    {
    }

    temp = adc_get_latest_value( ADC );

    *output = (uint16_t)( temp & 0xffffffffU ) ;

    return PLATFORM_SUCCESS;
}

platform_result_t platform_adc_take_sample_stream( const platform_adc_t* adc, void* buffer, uint16_t buffer_length )
{
    UNUSED_PARAMETER( adc );
    UNUSED_PARAMETER( buffer );
    UNUSED_PARAMETER( buffer_length );
    wiced_assert( "unimplemented", 0!=0 );
    return PLATFORM_SUCCESS;
}
