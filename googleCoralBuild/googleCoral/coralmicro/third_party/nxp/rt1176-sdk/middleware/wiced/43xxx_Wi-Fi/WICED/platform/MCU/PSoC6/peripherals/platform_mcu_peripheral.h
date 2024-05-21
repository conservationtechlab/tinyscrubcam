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
 * Defines PSoC 6 common peripheral structures, macros, constants and declares PSoC 6 peripheral API
 */
#pragma once
#include "platform_cmsis.h"
#include "platform_constants.h"
#include "platform_isr.h"
#include "wwd_constants.h"
#include "RTOS/wwd_rtos_interface.h"
#include "ring_buffer.h"
#include "cy_syslib.h"
#include "cy_sysclk.h"
#include "cy_gpio.h"
#include "cy_wdt.h"
#include "cy_flash.h"
#include "cy_scb_uart.h"
#include "cy_scb_i2c.h"
#include "cy_scb_spi.h"
#include "cy_pdm_pcm.h"
#include "cy_i2s.h"
#include "cy_tcpwm_pwm.h"
#include "cy_sar.h"
#include "cy_rtc.h"
#include "platform_assert.h"
#include "platform_capsense.h"
#include "SDIO_HOST.h"


#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/* Assign PCLK_XXX_CLOCK to a divider instance. */
#define DEFINE_PLATFORM_CLOCK_DIV_X( _PERIPH_, _DIVNUM_, _DIVTYPE_, _MAX_ )       \
    /* wiced_static_assert(                    */                \
    /*        _DIVNUM__exceeds_max_value,      */                \
    /*        ( (_DIVNUM_) < (_MAX_) )         */                \
    /*);                                       */                \
    platform_peripheral_clock_t const peripheral_clock_ ## _PERIPH_ = {\
            .clock_dest     = (_PERIPH_),                        \
            .divider_type   = (_DIVTYPE_),                       \
            .divider_num    = (_DIVNUM_),                        \
    }
/* Returns platform_peripheral_clock_t const* from a peripheral clock instance. */
#define PLATFORM_CLOCK_DIV_PTR( _PERIPH_ )  (&peripheral_clock_ ## _PERIPH_ )

/* Define an 8-bit clock divider peripheral assignment. */
#define DEFINE_PLATFORM_CLOCK_DIV_8( _PERIPH_, _DIVNUM_ ) \
    DEFINE_PLATFORM_CLOCK_DIV_X( _PERIPH_, _DIVNUM_, CY_SYSCLK_DIV_8_BIT, PLATFORM_SYSCLK_DIV_8_BIT_MAX )

/* Define a 16-bit clock divider peripheral assignment. */
#define DEFINE_PLATFORM_CLOCK_DIV_16( _PERIPH_, _DIVNUM_ ) \
    DEFINE_PLATFORM_CLOCK_DIV_X( _PERIPH_, _DIVNUM_, CY_SYSCLK_DIV_16_BIT, PLATFORM_SYSCLK_DIV_16_BIT_MAX )

/******************************************************
 *                    Constants
 ******************************************************/

/* Maximum number of divider resources per x-bit divider. */
#define PLATFORM_SYSCLK_DIV_8_BIT_MAX       (8)
#define PLATFORM_SYSCLK_DIV_16_BIT_MAX      (16)
#define PLATFORM_SYSCLK_DIV_16_5_BIT_MAX    (4)
#define PLATFORM_SYSCLK_DIV_24_5_BIT_MAX    (1)

/* Default STDIO buffer size */
#ifndef STDIO_BUFFER_SIZE
#define STDIO_BUFFER_SIZE         (64)
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

typedef struct
{
    uint32_t            port_num;       /* Port Number of the GPIO port */
    uint32_t            pin_num;        /* Pin Number within Port */
    en_hsiom_sel_t      hsiom;          /* HSIOM Configuration for GPIO operation for this Pin */
} platform_gpio_t;

typedef struct
{
    uint32_t                              channel;
    uint32_t                              enabled;
    cy_en_sar_chan_config_input_mode_t    input_mode;
    cy_en_sar_chan_config_pos_port_addr_t vplus_port_addr;
    cy_en_sar_chan_config_neg_port_addr_t vminus_port_addr;
    cy_en_sar_chan_config_pos_pin_addr_t  vplus_pin_addr;
    cy_en_sar_chan_config_neg_pin_addr_t  vminus_pin_addr;
    const platform_gpio_t*                vplus_pin;
    const platform_gpio_t*                vminus_pin;
} platform_adc_t;

typedef struct
{
    en_clk_dst_t clock_dest;
    cy_en_divider_types_t divider_type;
    uint32_t divider_num;
} platform_peripheral_clock_t;

typedef struct
{
  uint32_t                           block;
  uint32_t                           cntnum;
  const platform_peripheral_clock_t* pclk;
  const platform_gpio_t*             out_pin;
  en_hsiom_sel_t                     hsiom;     /* HSIOM Configuration for PWM operation for this Pin */
  wiced_bool_t                       is_single_shot;
  wiced_bool_t                       invert;
  uint32_t                           alignment;
  uint32_t                           dead_clocks;
} platform_pwm_t;

typedef struct {
    CySCB_Type*                         scb_base;
    const platform_peripheral_clock_t*  pclk;
    IRQn_Type                           irq_num;
} platform_scb_t;

typedef struct
{
    const platform_scb_t* port;
    uint32_t ssel;
    const platform_gpio_t* mosi_pin;
    const platform_gpio_t* miso_pin;
    const platform_gpio_t* clock_pin;
    const platform_gpio_t* cs_pin;
} platform_spi_t;

typedef struct
{
    const platform_spi_t* peripheral;
    const struct platform_spi_slave_config* config;
} platform_spi_slave_driver_t;

typedef struct
{
    uint32_t port;
    const platform_scb_t* scb;
    const platform_gpio_t* scl_pin;
    const platform_gpio_t* sda_pin;
} platform_i2c_t;

typedef struct
{
    const uint32_t             port;
    const platform_scb_t*      scb;
    const platform_gpio_t*     rx_pin;
    const platform_gpio_t*     tx_pin;
    const platform_gpio_t*     cts_pin;
    const platform_gpio_t*     rts_pin;
} platform_uart_t;

typedef struct
{
    platform_uart_t*           peripheral;
    wiced_ring_buffer_t*       tx_buffer;
    wiced_ring_buffer_t*       rx_buffer;
    host_semaphore_type_t      tx_complete;
    host_semaphore_type_t      rx_complete;
    volatile uint32_t          tx_size;
    volatile uint32_t          rx_size;
    uint32_t                   tx_underflow;
    uint32_t                   rx_overflow;
} platform_uart_driver_t;

typedef struct
{
    uint32_t dummy;
} platform_dma_t;

typedef struct
{
    uint32_t                 external_clock;
    uint32_t                 tx_master_mode;
    uint32_t                 rx_master_mode;
    const platform_gpio_t*   clk_i2s_if_pin;
    const platform_gpio_t*   mclk_pin;
    const platform_gpio_t*   tx_sck_pin;
    const platform_gpio_t*   tx_ws_pin;
    const platform_gpio_t*   tx_sdo_pin;
    const platform_gpio_t*   rx_sck_pin;
    const platform_gpio_t*   rx_ws_pin;
    const platform_gpio_t*   rx_sdi_pin;
} platform_i2s_port_info_t;

typedef enum
{
    PLATFORM_I2S_READ,
    PLATFORM_I2S_WRITE,
    PLATFORM_I2S_MAX,
} platform_i2s_direction_t;

typedef struct
{
    const platform_i2s_port_info_t* port_info;
    platform_i2s_direction_t stream_direction;
} platform_i2s_t;

typedef struct
{
    uint32_t               sample_rate;
    cy_en_pdm_pcm_out_t    output_mode;
    const platform_gpio_t* clock_pin;
    const platform_gpio_t* data_pin;
} platform_pdm_pcm_t;

typedef struct
{
    uint32_t hf_clock;
    cy_en_clkhf_in_sources_t clock_path;
} platform_root_clock_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

platform_result_t platform_gpio_irq_manager_init      ( void );

platform_result_t platform_watchdog_init              ( void );

platform_result_t platform_mcu_powersave_init         ( void );

platform_result_t platform_filesystem_init            ( void );

platform_result_t platform_root_clock_init            ( uint32_t hf_clock, cy_en_clkhf_dividers_t hf_div );

platform_result_t platform_peripheral_clock_init      ( platform_peripheral_clock_t const* peripheral_clock, uint32_t div_int, uint32_t div_frac );

void              platform_uart_irq                   ( uint32_t port );

platform_result_t platform_pdm_pcm_init               ( const platform_pdm_pcm_t* pdm_pcm );

platform_result_t platform_pdm_pcm_read_samples       ( const platform_pdm_pcm_t* pdm_pcm, void* buffer, uint32_t buffer_length );

void              platform_csd_irq              ( void );

platform_result_t platform_rtc_init             ( void );

wiced_bool_t      platform_mcu_powersave_is_permitted ( void );

#ifdef __cplusplus
} /* extern "C" */
#endif

