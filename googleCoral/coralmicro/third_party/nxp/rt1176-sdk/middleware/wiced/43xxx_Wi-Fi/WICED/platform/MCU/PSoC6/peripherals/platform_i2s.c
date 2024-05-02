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
 * PSoC 6 platform I2S driver
 */

#include "platform_i2s.h"
#include "platform_audio.h"
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

/* Configuration settings for I2S interface */
#define I2S_CHANNEL_COUNT        (2)
#define I2S_BITS_PER_SAMPLE      (16)
#define I2S_TX_ENABLED           ((bool)(1))
#define I2S_RX_ENABLED           ((bool)(1))
#define I2S_TX_FIFO_SIZE         (256)
#define I2S_RX_FIFO_SIZE         (256)
#define I2S_TX_FIFO_TRG_LVL      (128)
#define I2S_RX_FIFO_TRG_LVL      (127)
#define I2S_TX_DMA_TRIGGER       ((bool)(0))
#define I2S_RX_DMA_TRIGGER       ((bool)(0))
#define I2S_TX_WDG_ENABLE        ((bool)(0))
#define I2S_RX_WDG_ENABLE        ((bool)(0))
#define I2S_TX_WDG_VALUE         (0xFFFFFFFF)
#define I2S_RX_WDG_VALUE         (0xFFFFFFFF)
#define I2S_TX_SDO_LATCHING_TIME ((bool)(0))
#define I2S_RX_SDI_LATCHING_TIME ((bool)(0))
#define I2S_TX_SCKO_INVERSION    ((bool)(0))
#define I2S_RX_SCKO_INVERSION    ((bool)(0))
#define I2S_TX_SCKI_INVERSION    ((bool)(0))
#define I2S_RX_SCKI_INVERSION    ((bool)(0))
#define I2S_TX_CHANNEL_LEN       ((cy_en_i2s_len_t)(CY_I2S_LEN32))
#define I2S_RX_CHANNEL_LEN       ((cy_en_i2s_len_t)(CY_I2S_LEN32))
#define I2S_TX_WORD_LEN          ((cy_en_i2s_len_t)(CY_I2S_LEN16))
#define I2S_RX_WORD_LEN          ((cy_en_i2s_len_t)(CY_I2S_LEN16))
#define I2S_TX_OVH_VAL           ((cy_en_i2s_overhead_t)(CY_I2S_OVHDATA_ZERO))
#define I2S_RX_BIT_EXT           ((bool)(0))

#define I2S_INT_MASK_TX ( \
    (_BOOL2FLD(I2S_INTR_MASK_TX_TRIGGER,   true))  | \
    (_BOOL2FLD(I2S_INTR_MASK_TX_NOT_FULL,  false)) | \
    (_BOOL2FLD(I2S_INTR_MASK_TX_EMPTY,     false)) | \
    (_BOOL2FLD(I2S_INTR_MASK_TX_OVERFLOW,  false)) | \
    (_BOOL2FLD(I2S_INTR_MASK_TX_UNDERFLOW, true)))

#define I2S_INT_MASK_RX ( \
    (_BOOL2FLD(I2S_INTR_MASK_RX_TRIGGER,   true))  | \
    (_BOOL2FLD(I2S_INTR_MASK_RX_NOT_EMPTY, false)) | \
    (_BOOL2FLD(I2S_INTR_MASK_RX_FULL,      false)) | \
    (_BOOL2FLD(I2S_INTR_MASK_RX_OVERFLOW,  true))  | \
    (_BOOL2FLD(I2S_INTR_MASK_RX_UNDERFLOW, false)))

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/* TX/RX stream. */
typedef struct i2s_stream
{
    uint8_t                      is_xrun : 1;
    uint16_t                     period_size;
    uint32_t                     position;
    uint8_t*                     audio_buffer_ptr;
    uint16_t                     audio_buffer_size;
    uint16_t                     data_index;
    uint16_t                     periods_count;
    wiced_audio_session_ref      sh;

    /* Metrics. */
    uint32_t                     periods_transferred;
    uint32_t                     sw_xrun, hw_xrun;
} i2s_stream_t;

typedef struct i2s_control
{
    /* Configuration from initialize. */
    uint8_t                sample_bits;
    uint8_t                channels;
    uint32_t               sample_rate;
    wiced_i2s_spdif_mode_t i2s_spdif_mode;

    /* Global interrupt mask. */
    uint32_t               intmask;

    /* Indexed by WICED_I2S_READ/WRITE. */
    i2s_stream_t           streams[2];

    /* Bit-fields where bit positions are PLATFORM_I2S_READ/WRITE. */
    uint8_t                in_use : 2;
} i2s_control_t;

typedef struct
{
    uint32_t sample_rate;
    uint8_t  clk_divider;
} i2s_sample_rate_config_t;

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

static i2s_control_t i2s_control;

static i2s_sample_rate_config_t sample_rate_config[] =
{
    {.sample_rate = 44100, .clk_divider = 4},
};

/* Base reference to I2S block */
static I2S_Type* i2s_base = I2S;

/******************************************************
 *               Function Definitions
 ******************************************************/

static const platform_i2s_t* i2s_get_interface( wiced_i2s_t i2s )
{
    extern const platform_i2s_t i2s_interfaces[WICED_I2S_MAX];
    return &i2s_interfaces[i2s];
}

static i2s_sample_rate_config_t* i2s_get_sample_rate_config( uint32_t sample_rate )
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

static platform_result_t i2s_clock_init( const platform_i2s_t* i2s )
{
    /*
     * The PLL is configured during startup to generate the appropriate clk_hf[1]
     * clock which is used to provide the clk_audio_i2s clock to the I2S block.
     * The sck and ws signals are generated either using clk_audio_i2s
     * (derived from clk_hf[1]) internal clock or clk_i2s_if external clock.
     */

    /* Any clock initialization (MCLK?) can be added here */
    return PLATFORM_SUCCESS;
}

static platform_result_t i2s_pin_init( const platform_i2s_t* i2s )
{
    GPIO_PRT_Type *port_base;

    /*
     * When operating as a master, the TX/RX sub-block will setup the SCK/WS clock lines as strong drive with input buffer off.
     * When operating as a slave, the TX/RX sub-block will setup the SCK/WS clock lines as high-impedance with input buffer on.
     * The SDO and SDI data lines are always strong drive (input buffer off) and high-impedance (input buffer on) respectively.
     * The MCLK and CLK_I2S_IF lines are always strong drive (input buffer off) and high-impedance (input buffer on) respectively.
     */

    if ( i2s->port_info->clk_i2s_if_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( i2s->port_info->clk_i2s_if_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, i2s->port_info->clk_i2s_if_pin->pin_num, CY_GPIO_DM_HIGHZ, 0x1, i2s->port_info->clk_i2s_if_pin->hsiom );
    }

    if ( i2s->port_info->mclk_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( i2s->port_info->mclk_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, i2s->port_info->mclk_pin->pin_num, CY_GPIO_DM_STRONG_IN_OFF, 0x1, i2s->port_info->mclk_pin->hsiom );
    }

    if ( i2s->port_info->tx_sck_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( i2s->port_info->tx_sck_pin->port_num );

        if ( i2s->port_info->tx_master_mode != 0 )
        {
            Cy_GPIO_Pin_FastInit( port_base, i2s->port_info->tx_sck_pin->pin_num, CY_GPIO_DM_STRONG_IN_OFF, 0x1, i2s->port_info->tx_sck_pin->hsiom );
        }
        else
        {
            Cy_GPIO_Pin_FastInit( port_base, i2s->port_info->tx_sck_pin->pin_num, CY_GPIO_DM_HIGHZ, 0x1, i2s->port_info->tx_sck_pin->hsiom );
        }
    }

    if ( i2s->port_info->tx_ws_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( i2s->port_info->tx_ws_pin->port_num );

        if ( i2s->port_info->tx_master_mode != 0 )
        {
            Cy_GPIO_Pin_FastInit( port_base, i2s->port_info->tx_ws_pin->pin_num, CY_GPIO_DM_STRONG_IN_OFF, 0x1, i2s->port_info->tx_ws_pin->hsiom );
        }
        else
        {
            Cy_GPIO_Pin_FastInit( port_base, i2s->port_info->tx_ws_pin->pin_num, CY_GPIO_DM_HIGHZ, 0x1, i2s->port_info->tx_ws_pin->hsiom );
        }
    }

    if ( i2s->port_info->tx_sdo_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( i2s->port_info->tx_sdo_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, i2s->port_info->tx_sdo_pin->pin_num, CY_GPIO_DM_STRONG_IN_OFF, 0x1, i2s->port_info->tx_sdo_pin->hsiom );
    }

    if ( i2s->port_info->rx_sck_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( i2s->port_info->rx_sck_pin->port_num );

        if ( i2s->port_info->rx_master_mode != 0 )
        {
            Cy_GPIO_Pin_FastInit( port_base, i2s->port_info->rx_sck_pin->pin_num, CY_GPIO_DM_STRONG_IN_OFF, 0x1, i2s->port_info->rx_sck_pin->hsiom );
        }
        else
        {
            Cy_GPIO_Pin_FastInit( port_base, i2s->port_info->rx_sck_pin->pin_num, CY_GPIO_DM_HIGHZ, 0x1, i2s->port_info->rx_sck_pin->hsiom );
        }
    }

    if ( i2s->port_info->rx_ws_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( i2s->port_info->rx_ws_pin->port_num );

        if ( i2s->port_info->rx_master_mode != 0 )
        {
            Cy_GPIO_Pin_FastInit( port_base, i2s->port_info->rx_ws_pin->pin_num, CY_GPIO_DM_STRONG_IN_OFF, 0x1, i2s->port_info->rx_ws_pin->hsiom );
        }
        else
        {
            Cy_GPIO_Pin_FastInit( port_base, i2s->port_info->rx_ws_pin->pin_num, CY_GPIO_DM_HIGHZ, 0x1, i2s->port_info->rx_ws_pin->hsiom );
        }
    }

    if ( i2s->port_info->rx_sdi_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( i2s->port_info->rx_sdi_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, i2s->port_info->rx_sdi_pin->pin_num, CY_GPIO_DM_HIGHZ, 0x1, i2s->port_info->rx_sdi_pin->hsiom );
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t platform_i2s_interface_init( wiced_i2s_t port, wiced_i2s_params_t* params )
{
    const platform_i2s_t* i2s;
    cy_stc_i2s_config_t i2s_config;
    i2s_sample_rate_config_t* sample_rate_conf;

    /* Check parameters */
    if ( (params->channels != I2S_CHANNEL_COUNT) || (params->bits_per_sample != I2S_BITS_PER_SAMPLE) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Get I2S interface */
    i2s = i2s_get_interface( port );

    /* Get sample rate configuration */
    sample_rate_conf = i2s_get_sample_rate_config( params->sample_rate );

    if ( sample_rate_conf == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /*
     * The I2S block consists of two sub-blocks:
     * I2S Transmit (Tx) block: word select (tx_ws), clock (tx_sck) and data (tx_sdo) output signals.
     * I2S Receive (Rx) block: word select (rx_ws), clock (rx_sck) and data (rx_sdi) input signals.
     * When PSoC6 is driving both TX and RX as I2S Master, the TX and RX clock lines can be out of sync.
     * If the TX/RX clock lines are connected together to an audio codec, it may not be able to output audio.
     * In such settings, simultaneous RX and TX (full duplex I2S) audio can be achieved as follows:
     * I2S transmitter is in slave mode.
     * I2S receiver is in master mode.
     * (Also note that AC spec. should be satisfied at the PSoC6 pin in order to function correctly)
     */

    /* Setup I2S configuration */
    i2s_config.txEnabled          = I2S_TX_ENABLED;
    i2s_config.rxEnabled          = I2S_RX_ENABLED;
    i2s_config.txDmaTrigger       = I2S_TX_DMA_TRIGGER;
    i2s_config.rxDmaTrigger       = I2S_RX_DMA_TRIGGER;
    i2s_config.clkDiv             = sample_rate_conf->clk_divider;
    i2s_config.extClk             = (i2s->port_info->external_clock != 0) ? true : false;
    i2s_config.txMasterMode       = (i2s->port_info->tx_master_mode != 0) ? true : false;
    i2s_config.txAlignment        = CY_I2S_I2S_MODE;
    i2s_config.txWsPulseWidth     = CY_I2S_WS_ONE_CHANNEL_LENGTH;
    i2s_config.txWatchdogEnable   = I2S_TX_WDG_ENABLE;
    i2s_config.txWatchdogValue    = I2S_TX_WDG_VALUE;
    i2s_config.txSdoLatchingTime  = I2S_TX_SDO_LATCHING_TIME;
    i2s_config.txSckoInversion    = I2S_TX_SCKO_INVERSION;
    i2s_config.txSckiInversion    = I2S_TX_SCKI_INVERSION;
    i2s_config.txChannels         = I2S_CHANNEL_COUNT;
    i2s_config.txChannelLength    = I2S_TX_CHANNEL_LEN;
    i2s_config.txWordLength       = I2S_TX_WORD_LEN;
    i2s_config.txOverheadValue    = I2S_TX_OVH_VAL;
    i2s_config.txFifoTriggerLevel = (uint8_t)I2S_TX_FIFO_TRG_LVL;
    i2s_config.rxMasterMode       = (i2s->port_info->rx_master_mode != 0) ? true : false;
    i2s_config.rxAlignment        = CY_I2S_I2S_MODE;
    i2s_config.rxWsPulseWidth     = CY_I2S_WS_ONE_CHANNEL_LENGTH;
    i2s_config.rxWatchdogEnable   = I2S_RX_WDG_ENABLE;
    i2s_config.rxWatchdogValue    = I2S_RX_WDG_VALUE;
    i2s_config.rxSdiLatchingTime  = I2S_RX_SDI_LATCHING_TIME;
    i2s_config.rxSckoInversion    = I2S_RX_SCKO_INVERSION;
    i2s_config.rxSckiInversion    = I2S_RX_SCKI_INVERSION;
    i2s_config.rxChannels         = I2S_CHANNEL_COUNT;
    i2s_config.rxChannelLength    = I2S_RX_CHANNEL_LEN;
    i2s_config.rxWordLength       = I2S_RX_WORD_LEN;
    i2s_config.rxSignExtension    = I2S_RX_BIT_EXT;
    i2s_config.rxFifoTriggerLevel = (uint8_t)I2S_RX_FIFO_TRG_LVL;

    /* Initialize I2S clock */
    if ( i2s_clock_init( i2s ) != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    /* Initialize I2S pins */
    if ( i2s_pin_init( i2s ) != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    /* Initialize I2S block */
    if ( Cy_I2S_Init( i2s_base, &i2s_config ) != CY_I2S_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    /* Enable I2S IRQ */
    NVIC_EnableIRQ( audioss_interrupt_i2s_IRQn );

    return PLATFORM_SUCCESS;
}

static platform_result_t platform_i2s_direction_init( wiced_i2s_t port )
{
    platform_result_t result;
    platform_i2s_direction_t dir;
    uint32_t channel_index;

    dir = i2s_get_interface(port)->stream_direction;

    if ( dir == PLATFORM_I2S_WRITE )
    {
        /* Clear I2S TX FIFO */
        Cy_I2S_ClearTxFifo( i2s_base );

        /* Fill left and right channels of first TX data frame with zero data */
        for ( channel_index = 0 ; channel_index < I2S_CHANNEL_COUNT ; channel_index++ )
        {
            Cy_I2S_WriteTxData( i2s_base, 0 );
        }

        result = PLATFORM_SUCCESS;
    }
    else if ( dir == PLATFORM_I2S_READ )
    {
        /* Clear I2S RX FIFO */
        Cy_I2S_ClearRxFifo( i2s_base );

        result = PLATFORM_SUCCESS;
    }
    else
    {
        result = PLATFORM_ERROR;
    }

    return result;
}

static platform_result_t platform_i2s_interface_deinit( wiced_i2s_t port )
{
    /* Disable I2S IRQ */
    NVIC_DisableIRQ( audioss_interrupt_i2s_IRQn );

    /* De-initialize I2S block */
    Cy_I2S_DeInit( i2s_base );

    return PLATFORM_SUCCESS;
}

static platform_result_t platform_i2s_direction_deinit( wiced_i2s_t port )
{
    platform_result_t result;
    platform_i2s_direction_t dir;
    uint32_t i2s_intr_mask;

    dir = i2s_get_interface(port)->stream_direction;

    if ( dir == PLATFORM_I2S_WRITE )
    {
        /* Clear TX interrupt mask */
        i2s_intr_mask = Cy_I2S_GetInterruptMask( i2s_base );
        i2s_intr_mask &= ~I2S_INT_MASK_TX;
        Cy_I2S_SetInterruptMask( i2s_base, i2s_intr_mask );

        /* Stop I2S TX */
        Cy_I2S_DisableTx( i2s_base );

        result = PLATFORM_SUCCESS;
    }
    else if ( dir == PLATFORM_I2S_READ )
    {
        /* Clear RX interrupt mask */
        i2s_intr_mask = Cy_I2S_GetInterruptMask( i2s_base );
        i2s_intr_mask &= ~I2S_INT_MASK_RX;
        Cy_I2S_SetInterruptMask( i2s_base, i2s_intr_mask );

        /* Stop I2S RX */
        Cy_I2S_DisableRx( i2s_base );

        result = PLATFORM_SUCCESS;
    }
    else
    {
        result = PLATFORM_ERROR;
    }

    return result;
}

static platform_result_t i2s_start_stream( platform_i2s_direction_t dir )
{
    platform_result_t result;
    uint32_t i2s_intr_mask;

    if ( dir == PLATFORM_I2S_WRITE )
    {
        /* Start I2S TX */
        Cy_I2S_EnableTx( i2s_base );

        /* Set TX interrupt mask */
        i2s_intr_mask = Cy_I2S_GetInterruptMask( i2s_base );
        i2s_intr_mask |= I2S_INT_MASK_TX;
        Cy_I2S_SetInterruptMask( i2s_base, i2s_intr_mask );

        result = PLATFORM_SUCCESS;
    }
    else if ( dir == PLATFORM_I2S_READ )
    {
        /* Start I2S RX */
        Cy_I2S_EnableRx( i2s_base );

        /* Set RX interrupt mask */
        i2s_intr_mask = Cy_I2S_GetInterruptMask( i2s_base );
        i2s_intr_mask |= I2S_INT_MASK_RX;
        Cy_I2S_SetInterruptMask( i2s_base, i2s_intr_mask );

        result = PLATFORM_SUCCESS;
    }
    else
    {
        result = PLATFORM_ERROR;
    }

    return result;
}

static platform_result_t platform_i2s_start( wiced_i2s_t port )
{
    platform_i2s_direction_t dir = i2s_get_interface(port)->stream_direction;

    return i2s_start_stream( dir );
}

static platform_result_t i2s_stop_stream( platform_i2s_direction_t dir )
{
    platform_result_t result;
    uint32_t i2s_intr_mask;

    if ( dir == PLATFORM_I2S_WRITE )
    {
        /* Clear TX interrupt mask */
        i2s_intr_mask = Cy_I2S_GetInterruptMask( i2s_base );
        i2s_intr_mask &= ~I2S_INT_MASK_TX;
        Cy_I2S_SetInterruptMask( i2s_base, i2s_intr_mask );

        /* Stop I2S TX */
        Cy_I2S_DisableTx( i2s_base );

        result = PLATFORM_SUCCESS;
    }
    else if ( dir == PLATFORM_I2S_READ )
    {
        /* Clear RX interrupt mask */
        i2s_intr_mask = Cy_I2S_GetInterruptMask( i2s_base );
        i2s_intr_mask &= ~I2S_INT_MASK_RX;
        Cy_I2S_SetInterruptMask( i2s_base, i2s_intr_mask );

        /* Stop I2S RX */
        Cy_I2S_DisableRx( i2s_base );

        result = PLATFORM_SUCCESS;
    }
    else
    {
        result = PLATFORM_ERROR;
    }

    return result;
}

static platform_result_t platform_i2s_stop( wiced_i2s_t port )
{
    platform_i2s_direction_t dir = i2s_get_interface(port)->stream_direction;

    return i2s_stop_stream( dir );
}

wiced_result_t wiced_i2s_init( wiced_audio_session_ref sh, wiced_i2s_t i2s, wiced_i2s_params_t* params, uint32_t* mclk )
{
    wiced_result_t           result = WICED_SUCCESS;
    platform_i2s_direction_t dir    = i2s_get_interface(i2s)->stream_direction;
    i2s_stream_t*            stream = &i2s_control.streams[dir];

    /* Check arguments. */
    if ( ((dir == PLATFORM_I2S_WRITE) && (params->period_size > I2S_TX_PERIOD_BYTES_MAX)) ||
         ((dir == PLATFORM_I2S_READ)  && (params->period_size > I2S_RX_PERIOD_BYTES_MAX)) )
    {
        result = WICED_BADARG;
    }

    if ( params->i2s_spdif_mode != WICED_I2S_SPDIF_MODE_OFF )
    {
        result = WICED_BADARG;
    }

    if ( result == WICED_SUCCESS )
    {
        if ( i2s_control.in_use & (1 << dir) )
        {
            /* Already initialized. */
            result = WICED_ERROR;
            goto DONE_STREAM_IN_USE;
        }

        if ( i2s_control.in_use & ~(1 << dir) )
        {
            /* The other stream is in-use. */
            /* Verify that the configuration is compatible with the other stream. */
            result = (i2s_control.channels       == params->channels         &&
                      i2s_control.sample_rate    == params->sample_rate      &&
                      i2s_control.sample_bits    == params->bits_per_sample  &&
                      i2s_control.i2s_spdif_mode == params->i2s_spdif_mode) ? WICED_SUCCESS : WICED_BADARG;

            /* Don't do a full configuration since the settings are shared. */
            goto DONE_OTHER_STREAM_IN_USE;
        }
    }

    if ( result == WICED_SUCCESS )
    {
        /* Initialize I2S interface. */
        result = platform_i2s_interface_init( i2s, params );
    }

    if ( result == WICED_SUCCESS )
    {
        i2s_control.sample_bits    = params->bits_per_sample;
        i2s_control.channels       = params->channels;
        i2s_control.sample_rate    = params->sample_rate;
        i2s_control.i2s_spdif_mode = params->i2s_spdif_mode;
    }

DONE_OTHER_STREAM_IN_USE:
    if ( result == WICED_SUCCESS )
    {
        stream->period_size = params->period_size;
        stream->sh          = sh;

        /* Initialize I2S direction. */
        result = platform_i2s_direction_init( i2s );
    }

    if ( result == WICED_SUCCESS )
    {
        /* Mark this stream as in-use. */
        i2s_control.in_use |= (1 << dir);
    }

DONE_STREAM_IN_USE:
    return result;
}

wiced_result_t wiced_i2s_deinit( wiced_i2s_t i2s )
{
    platform_i2s_direction_t dir    = i2s_get_interface(i2s)->stream_direction;
    i2s_stream_t*            stream = &i2s_control.streams[dir];

    if ( !(i2s_control.in_use & (1 << dir)) )
    {
        /*
         * Not in use so there's nothing to do.
         */
        return WICED_SUCCESS;
    }

    /* Clean-up stream. */
    if ( i2s_control.in_use & (1 << dir) )
    {
        /* De-initialize I2S direction. */
        platform_i2s_direction_deinit( i2s );

        memset(stream, 0, sizeof *stream);

        /* This stream is no longer in-use. */
        i2s_control.in_use &= ~(1 << dir);
    }

    /* Check whether to perform shared clean-up. */
    if ( !(i2s_control.in_use & ~(1 << dir)) )
    {
        /* No other in-use streams; clean-up. */
        platform_i2s_interface_deinit( i2s );

        memset(&i2s_control, 0, sizeof i2s_control);
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_i2s_start( wiced_i2s_t i2s )
{
    wiced_result_t           result = WICED_SUCCESS;
    platform_i2s_direction_t dir    = i2s_get_interface(i2s)->stream_direction;
    i2s_stream_t*            stream = &i2s_control.streams[dir];

    wiced_assert( "Stop not called on xrun?", stream->is_xrun == 0 );
    stream->is_xrun = 0;

    if ( platform_i2s_start( i2s ) != PLATFORM_SUCCESS )
    {
        result = WICED_ERROR;
    }

    return result;
}

wiced_result_t wiced_i2s_stop( wiced_i2s_t i2s )
{
    wiced_result_t           result = WICED_SUCCESS;
    platform_i2s_direction_t dir    = i2s_get_interface(i2s)->stream_direction;
    i2s_stream_t*            stream = &i2s_control.streams[dir];

    if ( platform_i2s_stop( i2s ) != PLATFORM_SUCCESS )
    {
        result = WICED_ERROR;
    }

    if ( result == WICED_SUCCESS )
    {
        stream->is_xrun = 0;
    }

    return result;
}

wiced_result_t wiced_i2s_set_audio_buffer_details( wiced_i2s_t i2s, uint8_t* buffer_ptr, uint16_t size )
{
    if( (buffer_ptr != NULL) && (size != 0) )
    {
        platform_i2s_direction_t dir    = i2s_get_interface(i2s)->stream_direction;
        i2s_stream_t*            stream = &i2s_control.streams[dir];

        stream->audio_buffer_ptr = buffer_ptr;
        stream->audio_buffer_size = size;
        stream->position = 0;
        stream->data_index = 0;

        return WICED_SUCCESS;
    }
    else
    {
        return WICED_ERROR;
    }
}

wiced_result_t wiced_i2s_get_current_hw_pointer( wiced_i2s_t i2s, uint32_t* hw_pointer )
{
    platform_i2s_direction_t dir = i2s_get_interface(i2s)->stream_direction;

    if ( dir == PLATFORM_I2S_WRITE )
    {
        *hw_pointer = (uint32_t)Cy_I2S_GetTxWritePointer( i2s_base );

        return WICED_SUCCESS;
    }
    else if ( dir == PLATFORM_I2S_READ )
    {
        *hw_pointer = (uint32_t)Cy_I2S_GetTxReadPointer( i2s_base );

        return WICED_SUCCESS;
    }
    else
    {
        return WICED_ERROR;
    }
}

wiced_result_t wiced_i2s_get_current_hw_buffer_weight( wiced_i2s_t i2s, uint32_t* weight )
{
    return WICED_UNSUPPORTED;
}


wiced_result_t wiced_i2s_pll_set_fractional_divider( wiced_i2s_t i2s, float value )
{
    return WICED_UNSUPPORTED;
}

wiced_result_t wiced_i2s_get_clocking_details( const wiced_i2s_params_t* config, uint32_t* mclk )
{
    return WICED_UNSUPPORTED;
}

#ifdef WICED_USE_AUDIO
static void service_transfer_complete( platform_i2s_direction_t dir )
{
    i2s_stream_t* stream = &i2s_control.streams[dir];
    uint16_t length;
    uint16_t* samples;
    uint16_t num_samples;

    num_samples = (stream->period_size - stream->data_index) / 2;

    if ( num_samples == 0 )
    {
        /* We have transferred all requested periods; see if we have got more. */
        stream->periods_count = wiced_audio_buffer_platform_get_periods(stream->sh);

        if ( stream->periods_count > 0 )
        {
            stream->position = (stream->position + stream->period_size) % stream->audio_buffer_size;
            stream->data_index = 0;
            num_samples = (stream->period_size - stream->data_index) / 2;
        }
        else
        {
            /* Underflow/Overflow condition, stop stream? */
        }
    }

    if ( num_samples > 0 )
    {
        samples = (uint16_t*)(&(stream->audio_buffer_ptr[stream->position + stream->data_index]));

        if ( dir == PLATFORM_I2S_WRITE )
        {
            length = (num_samples < (I2S_TX_FIFO_SIZE - I2S_TX_FIFO_TRG_LVL)) ? num_samples : (I2S_TX_FIFO_SIZE - I2S_TX_FIFO_TRG_LVL);

            while ( length > 0 )
            {
                Cy_I2S_WriteTxData( i2s_base, (uint32_t)(*samples) );
                samples++;
                num_samples--;
                length--;
                stream->data_index += 2;
            }
        }
        else if ( dir == PLATFORM_I2S_READ )
        {
            length = (num_samples < (I2S_RX_FIFO_TRG_LVL + 1)) ? num_samples : (I2S_RX_FIFO_TRG_LVL + 1);

            while ( length > 0 )
            {
                *samples = (uint16_t)Cy_I2S_ReadRxData( i2s_base );
                samples++;
                num_samples--;
                length--;
                stream->data_index += 2;
            }
        }

        if ( num_samples == 0 )
        {
            stream->periods_transferred++;

            /* Update our position in the audio buffer, so the framework knows how much real estate in the buffer is available for a user */
            wiced_audio_buffer_platform_event( stream->sh, WICED_AUDIO_PERIOD_ELAPSED );

            /* We have transferred all requested periods; see if we have got more. */
            stream->periods_count = wiced_audio_buffer_platform_get_periods(stream->sh);

            if ( stream->periods_count > 0 )
            {
                stream->position = (stream->position + stream->period_size) % stream->audio_buffer_size;
                stream->data_index = 0;
            }
            else
            {
                /* Underflow/Overflow condition, stop stream? */
            }
        }
    }
}

static void service_xrun( platform_i2s_direction_t dir )
{
    i2s_stream_t* stream = &i2s_control.streams[dir];

    stream->hw_xrun++;

    if ( !stream->is_xrun )
    {
        //WICED_TRIGGER_BREAKPOINT();
        stream->is_xrun = 1;
        wiced_audio_buffer_platform_event( stream->sh, WICED_AUDIO_UNDERRUN );
    }
}

void platform_i2s_irq( void )
{
    uint32_t i2s_irq_status_masked;
    uint32_t i2s_irq_mask;

    /* Disable I2S IRQ */
    NVIC_DisableIRQ( audioss_interrupt_i2s_IRQn );

    /* Get interrupt status mask */
    i2s_irq_status_masked = Cy_I2S_GetInterruptStatusMasked( i2s_base );

    /* Disable I2S interrupts */
    i2s_irq_mask = Cy_I2S_GetInterruptMask( i2s_base );
    Cy_I2S_SetInterruptMask( i2s_base, 0 );

    /* TX handling */
    if ( (i2s_irq_status_masked & I2S_INTR_MASK_TX_TRIGGER_Msk) != 0 )
    {
        service_transfer_complete( PLATFORM_I2S_WRITE );
    }

    if ( (i2s_irq_status_masked & I2S_INTR_MASK_TX_UNDERFLOW_Msk) != 0 )
    {
        service_xrun( PLATFORM_I2S_WRITE );
    }

    /* RX handling */
    if ( (i2s_irq_status_masked & I2S_INTR_MASK_RX_TRIGGER_Msk) != 0 )
    {
        service_transfer_complete( PLATFORM_I2S_READ );
    }

    if ( (i2s_irq_status_masked & I2S_INTR_MASK_RX_OVERFLOW_Msk) != 0 )
    {
        service_xrun( PLATFORM_I2S_READ );
    }

    /* Clear I2S interrupts */
    Cy_I2S_ClearInterrupt( i2s_base, i2s_irq_status_masked );

    /* Enable I2S interrupts */
    Cy_I2S_SetInterruptMask( i2s_base, i2s_irq_mask );

    /* Enable I2S IRQ */
    NVIC_EnableIRQ( audioss_interrupt_i2s_IRQn );
}

/******************************************************
 *           Interrupt Handlers
 ******************************************************/

WWD_RTOS_DEFINE_ISR( I2S_IRQ )
{
    platform_i2s_irq();
}
WWD_RTOS_MAP_ISR( I2S_IRQ, audioss_interrupt_i2s_IRQn_Handler )
#endif /* WICED_USE_AUDIO */
