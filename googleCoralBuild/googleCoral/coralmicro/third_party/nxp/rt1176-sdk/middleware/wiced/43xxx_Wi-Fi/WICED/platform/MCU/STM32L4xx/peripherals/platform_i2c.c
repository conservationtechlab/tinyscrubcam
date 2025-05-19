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
 * STM32L4xx I2C implementation
 */
#include "platform_peripheral.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/* I2C bus frequency in Hz based on speed mode */
#define I2C_LOW_SPEED_MODE_FREQ_HZ      (10000)
#define I2C_STANDARD_SPEED_MODE_FREQ_HZ (100000)
#define I2C_HIGH_SPEED_MODE_FREQ_HZ     (400000)

/*
 * The I2C TIMING register configuration based on the bus speed.
 * The register value is obtained using the STM32CubeMX tool as below:
 * 1. I2C clock source is set to PCLK1
 * 2. PCLK1 frequency is 80MHz
 * 3. Rise time is set to 100ns. Fall time is set to 100ns.
 * 4. The desired speed mode (Standard Mode, Fast Mode).
 * 5. The desired speed frequency (10KHz, 100KHz, 400KHz).
 */
#define I2C_LOW_SPEED_MODE_TIMING      (0xF011F1FF)
#define I2C_STANDARD_SPEED_MODE_TIMING (0x10019CE4)
#define I2C_HIGH_SPEED_MODE_TIMING     (0x00F12981)

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
    PLATFORM_I2C1,
    PLATFORM_I2C2,
    PLATFORM_I2C3,
    PLATFORM_I2C4,
    PLATFORM_I2C_MAX,
} platform_i2c_type_t;

void hal_rcc_i2c1_clock_enable( void )
{
    RCC_PeriphCLKInitTypeDef rcc_i2c1_clock;

    /* Configure the I2C clock source */
    rcc_i2c1_clock.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
    rcc_i2c1_clock.I2c1ClockSelection   = RCC_I2C1CLKSOURCE_PCLK1;
    HAL_RCCEx_PeriphCLKConfig( &rcc_i2c1_clock );

    /* Enable the I2C clock */
    __HAL_RCC_I2C1_CLK_ENABLE( );
}

void hal_rcc_i2c2_clock_enable( void )
{
    RCC_PeriphCLKInitTypeDef rcc_i2c2_clock;

    /* Configure the I2C clock source */
    rcc_i2c2_clock.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
    rcc_i2c2_clock.I2c2ClockSelection   = RCC_I2C2CLKSOURCE_PCLK1;
    HAL_RCCEx_PeriphCLKConfig( &rcc_i2c2_clock );

    /* Enable the I2C clock */
    __HAL_RCC_I2C2_CLK_ENABLE( );
}

void hal_rcc_i2c3_clock_enable( void )
{
    RCC_PeriphCLKInitTypeDef rcc_i2c3_clock;

    /* Configure the I2C clock source */
    rcc_i2c3_clock.PeriphClockSelection = RCC_PERIPHCLK_I2C3;
    rcc_i2c3_clock.I2c3ClockSelection   = RCC_I2C3CLKSOURCE_PCLK1;
    HAL_RCCEx_PeriphCLKConfig( &rcc_i2c3_clock );

    /* Enable the I2C clock */
    __HAL_RCC_I2C3_CLK_ENABLE( );
}

void hal_rcc_i2c4_clock_enable( void )
{
    RCC_PeriphCLKInitTypeDef rcc_i2c4_clock;

    /* Configure the I2C clock source */
    rcc_i2c4_clock.PeriphClockSelection = RCC_PERIPHCLK_I2C4;
    rcc_i2c4_clock.I2c4ClockSelection   = RCC_I2C4CLKSOURCE_PCLK1;
    HAL_RCCEx_PeriphCLKConfig( &rcc_i2c4_clock );

    /* Enable the I2C clock */
    __HAL_RCC_I2C4_CLK_ENABLE( );
}

void hal_rcc_i2c1_clock_disable( void )
{
    /* Disable the I2C clock */
    __HAL_RCC_I2C1_CLK_DISABLE( );
}

void hal_rcc_i2c2_clock_disable( void )
{
    /* Disable the I2C clock */
    __HAL_RCC_I2C2_CLK_DISABLE( );
}

void hal_rcc_i2c3_clock_disable( void )
{
    /* Disable the I2C clock */
    __HAL_RCC_I2C3_CLK_DISABLE( );
}

void hal_rcc_i2c4_clock_disable( void )
{
    /* Disable the I2C clock */
    __HAL_RCC_I2C4_CLK_DISABLE( );
}

const platform_i2c_clock_enable_function_t i2c_clock_enable_function[ NUMBER_OF_I2C_PORTS ] =
{
    [ PLATFORM_I2C1 ] = hal_rcc_i2c1_clock_enable,
    [ PLATFORM_I2C2 ] = hal_rcc_i2c2_clock_enable,
    [ PLATFORM_I2C3 ] = hal_rcc_i2c3_clock_enable,
    [ PLATFORM_I2C4 ] = hal_rcc_i2c4_clock_enable,
};

const platform_i2c_clock_disable_function_t i2c_clock_disable_function[ NUMBER_OF_I2C_PORTS ] =
{
    [ PLATFORM_I2C1 ] = hal_rcc_i2c1_clock_disable,
    [ PLATFORM_I2C2 ] = hal_rcc_i2c2_clock_disable,
    [ PLATFORM_I2C3 ] = hal_rcc_i2c3_clock_disable,
    [ PLATFORM_I2C4 ] = hal_rcc_i2c4_clock_disable,
};

const uint8_t i2c_alternate_functions[ NUMBER_OF_I2C_PORTS ] =
{
    [ PLATFORM_I2C1 ] = GPIO_AF4_I2C1,
    [ PLATFORM_I2C2 ] = GPIO_AF4_I2C2,
    [ PLATFORM_I2C3 ] = GPIO_AF4_I2C3,
    [ PLATFORM_I2C4 ] = GPIO_AF4_I2C4,
};

const IRQn_Type i2c_event_irq_vectors[ NUMBER_OF_I2C_PORTS ] =
{
    [ PLATFORM_I2C1 ] = I2C1_EV_IRQn,
    [ PLATFORM_I2C2 ] = I2C2_EV_IRQn,
    [ PLATFORM_I2C3 ] = I2C3_EV_IRQn,
    [ PLATFORM_I2C4 ] = I2C4_EV_IRQn,
};

const IRQn_Type i2c_error_irq_vectors[ NUMBER_OF_I2C_PORTS ] =
{
    [ PLATFORM_I2C1 ] = I2C1_ER_IRQn,
    [ PLATFORM_I2C2 ] = I2C2_ER_IRQn,
    [ PLATFORM_I2C3 ] = I2C3_ER_IRQn,
    [ PLATFORM_I2C4 ] = I2C4_ER_IRQn,
};

/******************************************************
 *               Function Definitions
 ******************************************************/

uint8_t platform_i2c_get_port_number( platform_i2c_port_t* i2c_port )
{
    if ( i2c_port == I2C1 )
    {
        return PLATFORM_I2C1;
    }
    else if ( i2c_port == I2C2 )
    {
        return PLATFORM_I2C2;
    }
    else if ( i2c_port == I2C3 )
    {
        return PLATFORM_I2C3;
    }
    else if ( i2c_port == I2C4 )
    {
        return PLATFORM_I2C4;
    }
    else
    {
        return INVALID_I2C_PORT_NUMBER;
    }
}

uint32_t platform_i2c_get_port_timing( platform_i2c_port_t* i2c_port, uint32_t bus_freq )
{
    uint32_t timing;

    if ( bus_freq < I2C_STANDARD_SPEED_MODE_FREQ_HZ )
    {
        /* I2C bus in low speed mode */
        timing = I2C_LOW_SPEED_MODE_TIMING;
    }
    else if ( bus_freq < I2C_HIGH_SPEED_MODE_FREQ_HZ )
    {
        /* I2C bus in standard speed mode */
        timing = I2C_STANDARD_SPEED_MODE_TIMING;
    }
    else
    {
        /* I2C bus in high speed mode */
        timing = I2C_HIGH_SPEED_MODE_TIMING;
    }

    return timing;
}

void platform_i2c_tx_dma_init( DMA_HandleTypeDef* dma_handle_tx, I2C_HandleTypeDef* i2c_handle, const platform_i2c_t* peripheral )
{
    /* Configure the DMA handler for transmit process */
   dma_handle_tx->Instance                 = peripheral->tx_dma_config.stream;
   dma_handle_tx->Init.Request             = peripheral->tx_dma_config.channel;
   dma_handle_tx->Init.Direction           = DMA_MEMORY_TO_PERIPH;
   dma_handle_tx->Init.PeriphInc           = DMA_PINC_DISABLE;
   dma_handle_tx->Init.MemInc              = DMA_MINC_ENABLE;
   dma_handle_tx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
   dma_handle_tx->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
   dma_handle_tx->Init.Mode                = DMA_NORMAL;
   dma_handle_tx->Init.Priority            = DMA_PRIORITY_HIGH;

   HAL_DMA_Init( dma_handle_tx );

   /* Associate the initialized DMA handle to the the I2C handle */
   __HAL_LINKDMA( i2c_handle, hdmatx, *dma_handle_tx );
}

void platform_i2c_rx_dma_init( DMA_HandleTypeDef* dma_handle_rx, I2C_HandleTypeDef* i2c_handle, const platform_i2c_t* peripheral )
{
    /* Configure the DMA handler for reception process */
   dma_handle_rx->Instance                 = peripheral->rx_dma_config.stream;
   dma_handle_rx->Init.Request             = peripheral->rx_dma_config.channel;
   dma_handle_rx->Init.Direction           = DMA_PERIPH_TO_MEMORY;
   dma_handle_rx->Init.PeriphInc           = DMA_PINC_DISABLE;
   dma_handle_rx->Init.MemInc              = DMA_MINC_ENABLE;
   dma_handle_rx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
   dma_handle_rx->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
   dma_handle_rx->Init.Mode                = DMA_NORMAL;
   dma_handle_rx->Init.Priority            = DMA_PRIORITY_HIGH;

   HAL_DMA_Init( dma_handle_rx );

   /* Associate the initialized DMA handle to the the I2C handle */
   __HAL_LINKDMA( i2c_handle, hdmarx, *dma_handle_rx );
}
