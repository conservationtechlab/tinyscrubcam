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
 * PSoC 6 platform PDM_PCM driver
 */

#include <stdint.h>
#include <string.h>
#include "wiced_platform.h"
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_sleep.h"
#include "platform_assert.h"
#include "wwd_assert.h"
#include "wiced_rtos.h"
#include "wwd_rtos_isr.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define PDM_PCM_GAIN_RIGHT              ((uint8_t)(15))
#define PDM_PCM_GAIN_LEFT               ((uint8_t)(15))
#define PDM_PCM_SOFT_MUTE_ENABLE        ((bool)(0))
#define PDM_PCM_SOFT_MUTE_GAIN          ((bool)(0))
#define PDM_PCM_CHAN_SWAP               ((bool)(0))
#define PDM_PCM_SOFT_MUTE_CYCLES        ((uint8_t)(0))
#define PDM_PCM_CKO_DELAY               ((uint8_t)(0))
#define PDM_PCM_HPF_COEFF               ((uint8_t)(11))
#define PDM_PCM_HPF_DISABLE             ((bool)(0))
#define PDM_PCM_WORD_BIT_EXT            ((bool)(0))
#define PDM_PCM_DATA_STREAM_ENABLE      ((bool)(1))
#define PDM_PCM_RX_FIFO_TRG_LVL         ((uint8_t)(127))
#define PDM_PCM_DMA_TRG_ENABLE          ((bool)(0))
#define PDM_PCM_IRQ_RX_TRIGGER          ((bool)(1))
#define PDM_PCM_IRQ_RX_NOT_EMPTY        ((bool)(0))
#define PDM_PCM_IRQ_RX_OVERFLOW         ((bool)(0))
#define PDM_PCM_IRQ_RX_UNDERFLOW        ((bool)(0))
#define PDM_PCM_INT_MASK                (_BOOL2FLD(PDM_INTR_RX_TRIGGER,     PDM_PCM_IRQ_RX_TRIGGER)   | \
                                         _BOOL2FLD(PDM_INTR_RX_NOT_EMPTY,   PDM_PCM_IRQ_RX_NOT_EMPTY) | \
                                         _BOOL2FLD(PDM_INTR_RX_OVERFLOW,    PDM_PCM_IRQ_RX_OVERFLOW)  | \
                                         _BOOL2FLD(PDM_INTR_RX_UNDERFLOW,   PDM_PCM_IRQ_RX_UNDERFLOW) \
                                        )

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    uint32_t sample_rate;
    cy_en_pdm_pcm_clk_div_t clk_div;
    cy_en_pdm_pcm_clk_div_t mclk_div;
    uint8_t cko_div;
    uint8_t sinc_rate;
} pdm_pcm_sample_rate_config_t;

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

static pdm_pcm_sample_rate_config_t sample_rate_config[] =
{
    {.sample_rate = 44100, .clk_div = CY_PDM_PCM_CLK_DIV_1_4, .mclk_div = CY_PDM_PCM_CLK_DIV_1_4, .cko_div = 1, .sinc_rate = 32},
};

/* Base reference to PDM_PCM block */
static PDM_Type* pdm_pcm_base;

/* Semaphore to read samples from RX FIFO */
static host_semaphore_type_t rx_complete;

/******************************************************
 *               Function Definitions
 ******************************************************/

static pdm_pcm_sample_rate_config_t* pdm_pcm_get_sample_rate_config( uint32_t sample_rate )
{
    uint32_t idx;

    for ( idx = 0 ; idx < (sizeof(sample_rate_config)/sizeof(sample_rate_config[0])) ; idx++ )
    {
        if ( sample_rate_config[idx].sample_rate == sample_rate )
        {
            return &sample_rate_config[idx];
        }
    }

    return NULL;
}

static platform_result_t pdm_pcm_pin_init( const platform_pdm_pcm_t* pdm_pcm )
{
    GPIO_PRT_Type *port_base;

    if ( pdm_pcm->clock_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( pdm_pcm->clock_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, pdm_pcm->clock_pin->pin_num, CY_GPIO_DM_STRONG_IN_OFF, 0x1, pdm_pcm->clock_pin->hsiom );
    }

    if ( pdm_pcm->data_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( pdm_pcm->data_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, pdm_pcm->data_pin->pin_num, CY_GPIO_DM_HIGHZ, 0x0, pdm_pcm->data_pin->hsiom );
    }

    return PLATFORM_SUCCESS;
}

platform_result_t platform_pdm_pcm_init( const platform_pdm_pcm_t* pdm_pcm )
{
    cy_stc_pdm_pcm_config_t pdm_pcm_config;
    pdm_pcm_sample_rate_config_t* sample_rate_conf;

    platform_mcu_powersave_disable();

    if ( pdm_pcm == NULL )
    {
        wiced_assert( "Bad argument", 0 );
        return PLATFORM_ERROR;
    }

    /* Initialize PDM_PCM base */
    pdm_pcm_base = PDM;

    /* Initialize RX semaphore */
    host_rtos_init_semaphore( &rx_complete );

    /* Get sample rate configuration */
    sample_rate_conf = pdm_pcm_get_sample_rate_config( pdm_pcm->sample_rate );

    if ( sample_rate_conf == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Setup PDM_PCM configuration */
    pdm_pcm_config.clkDiv = sample_rate_conf->clk_div;
    pdm_pcm_config.mclkDiv = sample_rate_conf->mclk_div;
    pdm_pcm_config.ckoDiv = sample_rate_conf->cko_div;
    pdm_pcm_config.ckoDelay = PDM_PCM_CKO_DELAY;

    pdm_pcm_config.sincDecRate = sample_rate_conf->sinc_rate;

    pdm_pcm_config.chanSelect = pdm_pcm->output_mode;
    pdm_pcm_config.chanSwapEnable = PDM_PCM_CHAN_SWAP;

    pdm_pcm_config.highPassFilterGain = PDM_PCM_HPF_COEFF;
    pdm_pcm_config.highPassDisable = PDM_PCM_HPF_DISABLE;

    pdm_pcm_config.softMuteCycles = PDM_PCM_SOFT_MUTE_CYCLES;
    pdm_pcm_config.softMuteFineGain = PDM_PCM_SOFT_MUTE_GAIN;
    pdm_pcm_config.softMuteEnable = PDM_PCM_SOFT_MUTE_ENABLE;

    pdm_pcm_config.wordLen = CY_PDM_PCM_WLEN_16_BIT;
    pdm_pcm_config.signExtension = PDM_PCM_WORD_BIT_EXT;

    pdm_pcm_config.gainLeft = PDM_PCM_GAIN_LEFT;
    pdm_pcm_config.gainRight = PDM_PCM_GAIN_RIGHT;

    pdm_pcm_config.rxFifoTriggerLevel = PDM_PCM_RX_FIFO_TRG_LVL;

    pdm_pcm_config.dmaTriggerEnable = PDM_PCM_DMA_TRG_ENABLE;
    pdm_pcm_config.interruptMask = PDM_PCM_INT_MASK;

    /* Initialize PDM_PCM pins */
    if ( pdm_pcm_pin_init( pdm_pcm ) != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    /* Initialize PDM_PCM block */
    if ( Cy_PDM_PCM_Init( pdm_pcm_base, &pdm_pcm_config ) != CY_PDM_PCM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    /* Enable PDM_PCM block */
    Cy_PDM_PCM_Enable( pdm_pcm_base );

    /* Enable PDM_PCM IRQ */
    NVIC_EnableIRQ( audioss_interrupt_pdm_IRQn );

    platform_mcu_powersave_enable();

    return PLATFORM_SUCCESS;
}

platform_result_t platform_pdm_pcm_read_samples( const platform_pdm_pcm_t* pdm_pcm, void* buffer, uint32_t buffer_length )
{
    uint32_t length;
    uint16_t* samples;
    uint32_t num_samples;

    if ( (pdm_pcm == NULL) || (buffer == NULL) || (buffer_length == 0) )
    {
        wiced_assert( "Bad argument", 0 );
        return PLATFORM_ERROR;
    }

    samples = (uint16_t*)buffer;
    num_samples = buffer_length / 2;

    while ( num_samples > 0 )
    {
        host_rtos_get_semaphore( &rx_complete, WICED_NEVER_TIMEOUT, WICED_TRUE );

        length = MIN( num_samples, (PDM_PCM_RX_FIFO_TRG_LVL + 1) );

        while ( length > 0 )
        {
            *samples = (uint16_t)Cy_PDM_PCM_ReadFifo( pdm_pcm_base );
            samples++;
            num_samples--;
            length--;
        }

        NVIC_EnableIRQ( audioss_interrupt_pdm_IRQn );
    }

    return PLATFORM_SUCCESS;
}

void platform_pdm_pcm_irq( void )
{
    NVIC_DisableIRQ( audioss_interrupt_pdm_IRQn );

    if ( (Cy_PDM_PCM_GetInterruptStatusMasked(pdm_pcm_base) & PDM_INTR_MASK_RX_TRIGGER_Msk) != 0 )
    {
        Cy_PDM_PCM_ClearInterrupt( pdm_pcm_base, PDM_INTR_MASK_RX_TRIGGER_Msk );

        host_rtos_set_semaphore( &rx_complete, WICED_TRUE );
    }
}

/******************************************************
 *           Interrupt Handlers
 ******************************************************/

WWD_RTOS_DEFINE_ISR( PDM_PCM_IRQ )
{
    platform_pdm_pcm_irq();
}
WWD_RTOS_MAP_ISR( PDM_PCM_IRQ, audioss_interrupt_pdm_IRQn_Handler )
