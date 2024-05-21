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
 * PSoC 6 platform ADC driver
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

#define ADC_CHANNEL_COUNT ( sizeof(platform_adc_peripherals) / sizeof(platform_adc_peripherals[0]) )

/******************************************************
 *                    Constants
 ******************************************************/

/* The ADC clock is an integer divider of the PeriClk.
* The maximum supported clock frequency for the SAR is 18 MHz.
* With a PeriClk of 100 MHz, the minimum clock divider is 6.
* SAR Clock = 16.67 MHz.
*/
#define SAR_CLOCK_DIVIDER       ( 6 )

#define SAR_CTRL                ( CY_SAR_VREF_PWR_100                 /* Set reference voltage buffer to full power (100%). */ \
                                | CY_SAR_VREF_SEL_VDDA_DIV_2          /* Select internal VDDA/2 as the Vref. */ \
                                | CY_SAR_BYPASS_CAP_ENABLE            /* Enable Vref bypass capacitory. */ \
                                | CY_SAR_NEG_SEL_VREF                 /* Use Vref as the negative terminal for all single-ended channels. */ \
                                | CY_SAR_CTRL_NEGVREF_HW              /* Enable hardware control of switch between Vref and negative terminal. */ \
                                | CY_SAR_CTRL_COMP_DLY_2P5            /* Set comparator latch delay to 2.5 ns delay for maximum conversion rates. */ \
                                | CY_SAR_COMP_PWR_100                 /* Set comparator power to full power for maximum conversion rates. */ \
                                | CY_SAR_DEEPSLEEP_SARMUX_OFF         /* Disable SARMUX in Deep Sleep mode. */ \
                                | CY_SAR_SARSEQ_SWITCH_ENABLE )       /* Enable the SARSEQ. */

#define SAR_SAMPLE_CTRL         ( CY_SAR_RIGHT_ALIGN                  /* Right align result data to bits[11:0]. */ \
                                | CY_SAR_SINGLE_ENDED_UNSIGNED        /* Single ended channels are unsigned. */ \
                                | CY_SAR_DIFFERENTIAL_UNSIGNED        /* Differential channels are unsigned. */ \
                                | CY_SAR_AVG_CNT_2                    /* Set number of samples averaged to 2. */ \
                                | CY_SAR_AVG_MODE_SEQUENTIAL_FIXED    /* Averaging mode is sequential 12-bit fixed.*/ \
                                | CY_SAR_TRIGGER_MODE_FW_ONLY )       /* Hardware trigger is disabled. */

#define SAR_SAMPLE_TIME01       ( (302 << CY_SAR_SAMPLE_TIME0_SHIFT)  /* Sample Time 0 set to 302 ADC clock cycles. */ \
                                | (4 << CY_SAR_SAMPLE_TIME1_SHIFT) )  /* Sample Time 1 set to 3 ADC clock cycles. */

#define SAR_SAMPLE_TIME23       ( (4 << CY_SAR_SAMPLE_TIME2_SHIFT)    /* Sample Time 2 set to 3 ADC clock cycles. */ \
                                | (4 << CY_SAR_SAMPLE_TIME3_SHIFT) )  /* Sample Time 3 set to 3 ADC clock cycles. */

#define SAR_RANGE_THRES         ( CY_SAR_DEINIT )                     /* Disable the range threshold. */

#define SAR_RANGE_COND          ( CY_SAR_RANGE_COND_BELOW )           /* Default setting for range condition. */

#define SAR_INTR_MASK           ( CY_SAR_INTR_EOS_MASK )              /* Enable the End of Scan interrupt only. */

#define SAR_SAT_INTR_MASK       ( CY_SAR_DEINIT )                     /* Disable the saturation interrupt. */

#define SAR_RANGE_INTR_MASK     ( CY_SAR_DEINIT )                     /* Disable the range interrupt. */

#define SAR_CONFIG_ROUTING      ( true )                              /* Enable SARMUX/SARSEQ routing config. */

#define SAR_VREF_MV_VALUE       ( 1650 )                              /* Internal VDDA/2 is the Vref. */

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct
{
    cy_en_sar_chan_config_pos_port_addr_t port_addr;
    cy_en_sar_chan_config_pos_pin_addr_t  pin_addr;
    cy_en_sar_mux_switch_fw_ctrl_t        fw_ctrl;
    cy_en_sar_mux_switch_sq_ctrl_t        seq_ctrl;
} adc_mux_switch_vplus_t;

typedef struct
{
    cy_en_sar_chan_config_neg_port_addr_t port_addr;
    cy_en_sar_chan_config_neg_pin_addr_t  pin_addr;
    cy_en_sar_mux_switch_fw_ctrl_t        fw_ctrl;
    cy_en_sar_mux_switch_sq_ctrl_t        seq_ctrl;
} adc_mux_switch_vminus_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

/* Base reference to ADC block */
static SAR_Type* adc_base = SAR;

/* ADC channel configuration */
static uint32_t adc_chan_conf[CY_SAR_MAX_NUM_CHANNELS];

/* ADC channel scan results */
static uint16_t adc_chan_data[CY_SAR_MAX_NUM_CHANNELS];

/* Semaphore to read samples from RX FIFO */
static host_semaphore_type_t rx_complete;

static adc_mux_switch_vplus_t adc_mux_table_vplus[] =
{
    { .port_addr = CY_SAR_POS_PORT_ADDR_SARMUX, .pin_addr = CY_SAR_CHAN_POS_PIN_ADDR_0, .fw_ctrl = CY_SAR_MUX_FW_P0_VPLUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_P0 },
    { .port_addr = CY_SAR_POS_PORT_ADDR_SARMUX, .pin_addr = CY_SAR_CHAN_POS_PIN_ADDR_1, .fw_ctrl = CY_SAR_MUX_FW_P1_VPLUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_P1 },
    { .port_addr = CY_SAR_POS_PORT_ADDR_SARMUX, .pin_addr = CY_SAR_CHAN_POS_PIN_ADDR_2, .fw_ctrl = CY_SAR_MUX_FW_P2_VPLUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_P2 },
    { .port_addr = CY_SAR_POS_PORT_ADDR_SARMUX, .pin_addr = CY_SAR_CHAN_POS_PIN_ADDR_3, .fw_ctrl = CY_SAR_MUX_FW_P3_VPLUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_P3 },
    { .port_addr = CY_SAR_POS_PORT_ADDR_SARMUX, .pin_addr = CY_SAR_CHAN_POS_PIN_ADDR_4, .fw_ctrl = CY_SAR_MUX_FW_P4_VPLUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_P4 },
    { .port_addr = CY_SAR_POS_PORT_ADDR_SARMUX, .pin_addr = CY_SAR_CHAN_POS_PIN_ADDR_5, .fw_ctrl = CY_SAR_MUX_FW_P5_VPLUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_P5 },
    { .port_addr = CY_SAR_POS_PORT_ADDR_SARMUX, .pin_addr = CY_SAR_CHAN_POS_PIN_ADDR_6, .fw_ctrl = CY_SAR_MUX_FW_P6_VPLUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_P6 },
    { .port_addr = CY_SAR_POS_PORT_ADDR_SARMUX, .pin_addr = CY_SAR_CHAN_POS_PIN_ADDR_7, .fw_ctrl = CY_SAR_MUX_FW_P7_VPLUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_P7 },
    { .port_addr = CY_SAR_POS_PORT_ADDR_SARMUX_VIRT, .pin_addr = CY_SAR_CHAN_POS_PIN_ADDR_0, .fw_ctrl = CY_SAR_MUX_FW_TEMP_VPLUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_TEMP },
    { .port_addr = CY_SAR_POS_PORT_ADDR_SARMUX_VIRT, .pin_addr = CY_SAR_CHAN_POS_PIN_ADDR_2, .fw_ctrl = CY_SAR_MUX_FW_AMUXBUSA_VPLUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_AMUXBUSA },
    { .port_addr = CY_SAR_POS_PORT_ADDR_SARMUX_VIRT, .pin_addr = CY_SAR_CHAN_POS_PIN_ADDR_3, .fw_ctrl = CY_SAR_MUX_FW_AMUXBUSB_VPLUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_AMUXBUSB },
};

static adc_mux_switch_vminus_t adc_mux_table_vminus[] =
{
    { .port_addr = CY_SAR_NEG_PORT_ADDR_SARMUX, .pin_addr = CY_SAR_CHAN_NEG_PIN_ADDR_0, .fw_ctrl = CY_SAR_MUX_FW_P0_VMINUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_P0 },
    { .port_addr = CY_SAR_NEG_PORT_ADDR_SARMUX, .pin_addr = CY_SAR_CHAN_NEG_PIN_ADDR_1, .fw_ctrl = CY_SAR_MUX_FW_P1_VMINUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_P1 },
    { .port_addr = CY_SAR_NEG_PORT_ADDR_SARMUX, .pin_addr = CY_SAR_CHAN_NEG_PIN_ADDR_2, .fw_ctrl = CY_SAR_MUX_FW_P2_VMINUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_P2 },
    { .port_addr = CY_SAR_NEG_PORT_ADDR_SARMUX, .pin_addr = CY_SAR_CHAN_NEG_PIN_ADDR_3, .fw_ctrl = CY_SAR_MUX_FW_P3_VMINUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_P3 },
    { .port_addr = CY_SAR_NEG_PORT_ADDR_SARMUX, .pin_addr = CY_SAR_CHAN_NEG_PIN_ADDR_4, .fw_ctrl = CY_SAR_MUX_FW_P4_VMINUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_P4 },
    { .port_addr = CY_SAR_NEG_PORT_ADDR_SARMUX, .pin_addr = CY_SAR_CHAN_NEG_PIN_ADDR_5, .fw_ctrl = CY_SAR_MUX_FW_P5_VMINUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_P5 },
    { .port_addr = CY_SAR_NEG_PORT_ADDR_SARMUX, .pin_addr = CY_SAR_CHAN_NEG_PIN_ADDR_6, .fw_ctrl = CY_SAR_MUX_FW_P6_VMINUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_P6 },
    { .port_addr = CY_SAR_NEG_PORT_ADDR_SARMUX, .pin_addr = CY_SAR_CHAN_NEG_PIN_ADDR_7, .fw_ctrl = CY_SAR_MUX_FW_P7_VMINUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_P7 },
    { .port_addr = CY_SAR_NEG_PORT_ADDR_SARMUX_VIRT, .pin_addr = CY_SAR_CHAN_NEG_PIN_ADDR_0, .fw_ctrl = CY_SAR_MUX_FW_VSSA_VMINUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_VSSA },
    { .port_addr = CY_SAR_NEG_PORT_ADDR_SARMUX_VIRT, .pin_addr = CY_SAR_CHAN_NEG_PIN_ADDR_2, .fw_ctrl = CY_SAR_MUX_FW_AMUXBUSA_VMINUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_AMUXBUSA },
    { .port_addr = CY_SAR_NEG_PORT_ADDR_SARMUX_VIRT, .pin_addr = CY_SAR_CHAN_NEG_PIN_ADDR_3, .fw_ctrl = CY_SAR_MUX_FW_AMUXBUSB_VMINUS, .seq_ctrl = CY_SAR_MUX_SQ_CTRL_AMUXBUSB },
};

extern const platform_adc_t platform_adc_peripherals[WICED_ADC_MAX];

extern const platform_peripheral_clock_t peripheral_clock_PCLK_PASS_CLOCK_SAR;

/******************************************************
 *               Function Definitions
 ******************************************************/

static platform_result_t adc_scan_results( void )
{
    uint32_t chan_idx;

    /* Get scan results for all channels */
    for ( chan_idx = 0 ; chan_idx < CY_SAR_MAX_NUM_CHANNELS ; chan_idx++ )
    {
        adc_chan_data[chan_idx] = (uint16_t)Cy_SAR_GetResult16( adc_base, chan_idx );
    }

    return PLATFORM_SUCCESS;
}

static uint32_t adc_channel_enable( void )
{
    const platform_adc_t* adc;
    uint32_t chan_enab;
    uint32_t chan_idx;

    chan_enab = 0;

    for ( chan_idx = 0 ; chan_idx < ADC_CHANNEL_COUNT ; chan_idx++ )
    {
        adc = &platform_adc_peripherals[chan_idx];

        if ( adc->enabled != 0 )
        {
            chan_enab |= (1 << chan_idx);
        }
    }

    return chan_enab;
}

static platform_result_t adc_channel_config( void )
{
    const platform_adc_t* adc;
    uint32_t chan_cfg;
    uint32_t chan_idx;

    for ( chan_idx = 0 ; chan_idx < CY_SAR_MAX_NUM_CHANNELS ; chan_idx++ )
    {
        adc_chan_conf[chan_idx] = 0;
        adc_chan_data[chan_idx] = 0;
    }

    for ( chan_idx = 0 ; chan_idx < ADC_CHANNEL_COUNT ; chan_idx++ )
    {
        adc = &platform_adc_peripherals[chan_idx];

        chan_cfg = 0;

        if ( adc->enabled != 0 )
        {
            chan_cfg |= adc->input_mode;
            chan_cfg |= CY_SAR_CHAN_SAMPLE_TIME_0;
            chan_cfg |= CY_SAR_CHAN_AVG_DISABLE;
            chan_cfg |= adc->vplus_port_addr;
            chan_cfg |= adc->vplus_pin_addr;

            if ( adc->input_mode != CY_SAR_CHAN_SINGLE_ENDED )
            {
                chan_cfg |= adc->vminus_port_addr;
                chan_cfg |= adc->vminus_pin_addr;
            }
        }

        adc_chan_conf[chan_idx] = chan_cfg;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t adc_channel_reset( void )
{
    uint32_t chan_idx;

    for ( chan_idx = 0 ; chan_idx < CY_SAR_MAX_NUM_CHANNELS ; chan_idx++ )
    {
        adc_chan_conf[chan_idx] = 0;
        adc_chan_data[chan_idx] = 0;
    }

    return PLATFORM_SUCCESS;
}

static void adc_channel_mux( const platform_adc_t* adc, uint32_t* chan_mux_fw, uint32_t* chan_mux_seq)
{
    uint32_t fw_ctrl;
    uint32_t seq_ctrl;
    uint32_t chan_idx;

    fw_ctrl  = 0;
    seq_ctrl = 0;

    if ( adc->enabled != 0 )
    {
        for ( chan_idx = 0 ; chan_idx < (sizeof(adc_mux_table_vplus)/sizeof(adc_mux_table_vplus[0])) ; chan_idx++ )
        {
            if ( (adc_mux_table_vplus[chan_idx].port_addr == adc->vplus_port_addr) && (adc_mux_table_vplus[chan_idx].pin_addr == adc->vplus_pin_addr) )
            {
                fw_ctrl  |= adc_mux_table_vplus[chan_idx].fw_ctrl;
                seq_ctrl |= adc_mux_table_vplus[chan_idx].seq_ctrl;

                break;
            }
        }

        if ( adc->input_mode == CY_SAR_CHAN_SINGLE_ENDED )
        {
            fw_ctrl  |= CY_SAR_MUX_FW_VSSA_VMINUS;
            seq_ctrl |= CY_SAR_MUX_SQ_CTRL_VSSA;
        }
        else
        {
            for ( chan_idx = 0 ; chan_idx < (sizeof(adc_mux_table_vminus)/sizeof(adc_mux_table_vminus[0])) ; chan_idx++ )
            {
                if ( (adc_mux_table_vminus[chan_idx].port_addr == adc->vminus_port_addr) && (adc_mux_table_vminus[chan_idx].pin_addr == adc->vminus_pin_addr) )
                {
                    fw_ctrl  |= adc_mux_table_vminus[chan_idx].fw_ctrl;
                    seq_ctrl |= adc_mux_table_vminus[chan_idx].seq_ctrl;

                    break;
                }
            }
        }
    }

    *chan_mux_fw  = fw_ctrl;
    *chan_mux_seq = seq_ctrl;
}

static platform_result_t adc_mux_init( uint32_t* mux_switch_ctrl_fw, uint32_t* mux_switch_ctrl_seq )
{
    const platform_adc_t* adc;
    uint32_t mux_switch_chan_fw;
    uint32_t mux_switch_chan_seq;
    uint32_t chan_idx;

    *mux_switch_ctrl_fw  = 0;
    *mux_switch_ctrl_seq = 0;

    for ( chan_idx = 0 ; chan_idx < ADC_CHANNEL_COUNT ; chan_idx++ )
    {
        adc = &platform_adc_peripherals[chan_idx];

        mux_switch_chan_fw  = 0;
        mux_switch_chan_seq = 0;

        adc_channel_mux( adc, &mux_switch_chan_fw, &mux_switch_chan_seq );

        *mux_switch_ctrl_fw  |= mux_switch_chan_fw;
        *mux_switch_ctrl_seq |= mux_switch_chan_seq;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t adc_clock_init( void )
{
    const platform_peripheral_clock_t* adc_clk = PLATFORM_CLOCK_DIV_PTR( PCLK_PASS_CLOCK_SAR );

    /* Initialize the SAR ADC clock */
    if ( platform_peripheral_clock_init( adc_clk, (SAR_CLOCK_DIVIDER - 1), 0 ) != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t adc_pin_init( void )
{
    uint32_t chan_idx;
    const platform_adc_t* adc;
    GPIO_PRT_Type *port_base;

    for ( chan_idx = 0 ; chan_idx < ADC_CHANNEL_COUNT ; chan_idx++ )
    {
        adc = &platform_adc_peripherals[chan_idx];

        if ( adc->enabled != 0 )
        {
            if ( adc->vplus_pin != NULL )
            {
                port_base = Cy_GPIO_PortToAddr( adc->vplus_pin->port_num );
                Cy_GPIO_Pin_FastInit( port_base, adc->vplus_pin->pin_num, CY_GPIO_DM_ANALOG, 0x1, adc->vplus_pin->hsiom );
            }

            if ( adc->input_mode != CY_SAR_CHAN_SINGLE_ENDED )
            {
                if ( adc->vminus_pin != NULL )
                {
                    port_base = Cy_GPIO_PortToAddr( adc->vminus_pin->port_num );
                    Cy_GPIO_Pin_FastInit( port_base, adc->vminus_pin->pin_num, CY_GPIO_DM_ANALOG, 0x1, adc->vminus_pin->hsiom );
                }
            }
        }
    }

    return PLATFORM_SUCCESS;
}

platform_result_t platform_adc_init( const platform_adc_t* adc, uint32_t sample_cycle )
{
    uint32_t chan_idx;
    uint32_t adc_chan_enab;
    uint32_t mux_switch_fw_ctrl;
    uint32_t mux_switch_seq_ctrl;
    cy_stc_sar_config_t adc_config;

    platform_mcu_powersave_disable();

    if ( adc == NULL )
    {
        wiced_assert( "Bad argument", 0 );
        return PLATFORM_ERROR;
    }

    /* Initialize RX semaphore */
    host_rtos_init_semaphore( &rx_complete );

    /* Enable channels */
    adc_chan_enab = adc_channel_enable();

    /* Configure channels */
    adc_channel_config();

    /* Setup MUX switch control */
    adc_mux_init( &mux_switch_fw_ctrl, &mux_switch_seq_ctrl );

    /* Setup ADC configuration */
    adc_config.ctrl = SAR_CTRL;
    adc_config.sampleCtrl = SAR_SAMPLE_CTRL;
    adc_config.sampleTime01 = SAR_SAMPLE_TIME01;
    adc_config.sampleTime23 = SAR_SAMPLE_TIME23;
    adc_config.rangeThres = SAR_RANGE_THRES;
    adc_config.rangeCond = SAR_RANGE_COND;
    adc_config.chanEn = adc_chan_enab;

    for ( chan_idx = 0 ; chan_idx < CY_SAR_MAX_NUM_CHANNELS ; chan_idx++ )
    {
        adc_config.chanConfig[chan_idx] = adc_chan_conf[chan_idx];
    }

    adc_config.intrMask = SAR_INTR_MASK;
    adc_config.satIntrMask = SAR_SAT_INTR_MASK;
    adc_config.rangeIntrMask = SAR_RANGE_INTR_MASK;
    adc_config.muxSwitch = mux_switch_fw_ctrl;
    adc_config.muxSwitchSqCtrl = mux_switch_seq_ctrl;
    adc_config.configRouting = SAR_CONFIG_ROUTING;
    adc_config.vrefMvValue = SAR_VREF_MV_VALUE;

    /* Initialize Analog block */
    if ( Cy_SysAnalog_Init( &Cy_SysAnalog_Fast_Local ) != CY_SYSANALOG_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    /* Enable Analog block */
    Cy_SysAnalog_Enable();

    /* Initialize ADC clock */
    if ( adc_clock_init() != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    /* Initialize ADC pins */
    if ( adc_pin_init() != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    /* Initialize ADC block */
    if ( Cy_SAR_Init( adc_base, &adc_config ) != CY_SAR_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    /* Enable ADC block */
    Cy_SAR_Enable( adc_base );

    /* Enable ADC IRQ */
    NVIC_EnableIRQ( pass_interrupt_sar_IRQn );

    platform_mcu_powersave_enable();

    return PLATFORM_SUCCESS;
}

platform_result_t platform_adc_deinit( const platform_adc_t* adc )
{
    if ( adc == NULL )
    {
        wiced_assert( "Bad argument", 0 );
        return PLATFORM_ERROR;
    }

    platform_mcu_powersave_disable();

    /* Disable ADC IRQ */
    NVIC_DisableIRQ( pass_interrupt_sar_IRQn );

    /* De-initialize ADC block */
    Cy_SAR_DeInit( adc_base, true );

    /* Reset channel configuration */
    adc_channel_reset();

    /* De-initialize RX semaphore */
    host_rtos_deinit_semaphore( &rx_complete );

    platform_mcu_powersave_enable();

    return PLATFORM_SUCCESS;
}

platform_result_t platform_adc_take_sample( const platform_adc_t* adc, uint16_t* output )
{
    if ( (adc == NULL) || (output == NULL) )
    {
        wiced_assert( "Bad argument", 0 );
        return PLATFORM_ERROR;
    }

    if ( adc->enabled == 0 )
    {
        *output = 0;

        return PLATFORM_ERROR;
    }

    /* Start single-shot conversion */
    Cy_SAR_StartConvert( adc_base, CY_SAR_START_CONVERT_SINGLE_SHOT );

    host_rtos_get_semaphore( &rx_complete, WICED_NEVER_TIMEOUT, WICED_TRUE );

    *output = adc_chan_data[adc->channel];

    NVIC_EnableIRQ( pass_interrupt_sar_IRQn );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_adc_take_sample_stream( const platform_adc_t* adc, void* buffer, uint16_t buffer_length )
{
    uint16_t* samples;
    uint16_t num_samples;

    if ( (adc == NULL) || (buffer == NULL) || (buffer_length == 0) )
    {
        wiced_assert( "Bad argument", 0 );
        return PLATFORM_ERROR;
    }

    samples = (uint16_t*)buffer;
    num_samples = buffer_length / 2;

    if ( adc->enabled == 0 )
    {
        while ( num_samples > 0 )
        {
            *samples = 0;
            samples++;
            num_samples--;
        }

        return PLATFORM_ERROR;
    }

    /* Start continuous conversion */
    Cy_SAR_StartConvert( adc_base, CY_SAR_START_CONVERT_CONTINUOUS );

    while ( num_samples > 0 )
    {
        host_rtos_get_semaphore( &rx_complete, WICED_NEVER_TIMEOUT, WICED_TRUE );

        *samples = adc_chan_data[adc->channel];
        samples++;
        num_samples--;

        NVIC_EnableIRQ( pass_interrupt_sar_IRQn );
    }

    /* Stop continuous conversion */
    Cy_SAR_StopConvert( adc_base );

    return PLATFORM_SUCCESS;
}

/******************************************************
 *           Interrupt Handlers
 ******************************************************/

void platform_adc_irq( void )
{
    NVIC_DisableIRQ( pass_interrupt_sar_IRQn );

    /* Check for end-of-scan interrupt. */
    if ( (Cy_SAR_GetInterruptStatusMasked( adc_base ) & CY_SAR_INTR_EOS_MASK) != 0 )
    {
        Cy_SAR_ClearInterrupt( adc_base, CY_SAR_INTR_EOS_MASK );

        adc_scan_results();

        host_rtos_set_semaphore( &rx_complete, WICED_TRUE );
    }
}

WWD_RTOS_DEFINE_ISR( ADC_IRQ )
{
    platform_adc_irq();
}
WWD_RTOS_MAP_ISR( ADC_IRQ, pass_interrupt_sar_IRQn_Handler )
