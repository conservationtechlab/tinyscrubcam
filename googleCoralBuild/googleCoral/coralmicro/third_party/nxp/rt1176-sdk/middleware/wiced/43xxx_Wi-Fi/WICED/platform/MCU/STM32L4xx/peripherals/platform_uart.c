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
 * STM32L4xx UART implementation
 */
#include "platform_peripheral.h"

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

typedef enum
{
    PLATFORM_USART1,
    PLATFORM_USART2,
    PLATFORM_USART3,
    PLATFORM_UART4,
    PLATFORM_UART5,
    PLATFORM_UART_MAX,
} platform_usart_type_t;

void hal_rcc_usart1_clock_enable( void )
{
    __HAL_RCC_USART1_CLK_ENABLE( );
}

void hal_rcc_usart2_clock_enable( void )
{
    __HAL_RCC_USART2_CLK_ENABLE( );
}
void hal_rcc_usart3_clock_enable( void )
{
    __HAL_RCC_USART3_CLK_ENABLE( );
}

void hal_rcc_uart4_clock_enable( void )
{
    __HAL_RCC_UART4_CLK_ENABLE( );
}

void hal_rcc_uart5_clock_enable( void )
{
    __HAL_RCC_UART5_CLK_ENABLE( );
}

void hal_rcc_usart1_clock_disable( void )
{
    __HAL_RCC_USART1_CLK_DISABLE( );
}

void hal_rcc_usart2_clock_disable( void )
{
    __HAL_RCC_USART2_CLK_DISABLE( );
}

void hal_rcc_usart3_clock_disable( void )
{
    __HAL_RCC_USART3_CLK_DISABLE( );
}

void hal_rcc_uart4_clock_disable( void )
{
    __HAL_RCC_UART4_CLK_DISABLE( );
}

void hal_rcc_uart5_clock_disable( void )
{
    __HAL_RCC_UART5_CLK_DISABLE( );
}

const platform_uart_clock_enable_function_t uart_clock_enable_function[ NUMBER_OF_UART_PORTS ] =
{
    [ PLATFORM_USART1 ] = hal_rcc_usart1_clock_enable,
    [ PLATFORM_USART2 ] = hal_rcc_usart2_clock_enable,
    [ PLATFORM_USART3 ] = hal_rcc_usart3_clock_enable,
    [ PLATFORM_UART4  ] = hal_rcc_uart4_clock_enable,
    [ PLATFORM_UART5  ] = hal_rcc_uart5_clock_enable,
};

const platform_uart_clock_disable_function_t uart_clock_disable_function[ NUMBER_OF_UART_PORTS ] =
{
    [ PLATFORM_USART1 ] = hal_rcc_usart1_clock_disable,
    [ PLATFORM_USART2 ] = hal_rcc_usart2_clock_disable,
    [ PLATFORM_USART3 ] = hal_rcc_usart3_clock_disable,
    [ PLATFORM_UART4  ] = hal_rcc_uart4_clock_disable,
    [ PLATFORM_UART5  ] = hal_rcc_uart5_clock_disable,
};

/* UART interrupt vectors */
const IRQn_Type uart_irq_vectors[ NUMBER_OF_UART_PORTS ] =
{
    [ PLATFORM_USART1 ] = USART1_IRQn,
    [ PLATFORM_USART2 ] = USART2_IRQn,
    [ PLATFORM_USART3 ] = USART3_IRQn,
    [ PLATFORM_UART4  ] = UART4_IRQn,
    [ PLATFORM_UART5  ] = UART5_IRQn,
};

const uint8_t uart_alternate_functions[ NUMBER_OF_UART_PORTS ] =
{
    [ PLATFORM_USART1 ] = GPIO_AF7_USART1,
    [ PLATFORM_USART2 ] = GPIO_AF7_USART2,
    [ PLATFORM_USART3 ] = GPIO_AF7_USART3,
    [ PLATFORM_UART4  ] = GPIO_AF8_UART4,
    [ PLATFORM_UART5  ] = GPIO_AF8_UART5,
};

/******************************************************
 *               Function Definitions
 ******************************************************/

uint8_t platform_uart_get_port_number( USART_TypeDef* uart )
{
    if ( uart == USART1 )
    {
        return PLATFORM_USART1;
    }
    else if ( uart == USART2 )
    {
        return PLATFORM_USART2;
    }
    else if ( uart == USART3 )
    {
        return PLATFORM_USART3;
    }
    else if ( uart == UART4 )
    {
        return PLATFORM_UART4;
    }
    else if ( uart == UART5 )
    {
        return PLATFORM_UART5;
    }
    else
    {
        return INVALID_UART_PORT_NUMBER;
    }
}

void platform_uart_rx_dma_init(DMA_HandleTypeDef *dma_handle_rx, UART_HandleTypeDef * uart_handle, const platform_uart_t* peripheral)
{
    /* Configure the DMA handler for reception process */
   dma_handle_rx->Instance                 = peripheral->rx_dma_config.stream;
   dma_handle_rx->Init.Request             = peripheral->rx_dma_config.channel;
   dma_handle_rx->Init.Direction           = DMA_PERIPH_TO_MEMORY;
   dma_handle_rx->Init.PeriphInc           = DMA_PINC_DISABLE;
   dma_handle_rx->Init.MemInc              = DMA_MINC_ENABLE;
   dma_handle_rx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
   dma_handle_rx->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
   dma_handle_rx->Init.Mode                = DMA_CIRCULAR;
   dma_handle_rx->Init.Priority            = DMA_PRIORITY_VERY_HIGH;

   HAL_DMA_Init( dma_handle_rx );

   /* Associate the initialized DMA handle to the the UART handle */
   __HAL_LINKDMA( uart_handle, hdmarx, *dma_handle_rx );
}

void platform_uart_tx_dma_init(DMA_HandleTypeDef *dma_handle_tx, UART_HandleTypeDef * uart_handle, const platform_uart_t* peripheral)
{
    /* Configure the DMA handler for reception process */
   dma_handle_tx->Instance                 = peripheral->tx_dma_config.stream;
   dma_handle_tx->Init.Request             = peripheral->tx_dma_config.channel;
   dma_handle_tx->Init.Direction           = DMA_MEMORY_TO_PERIPH;
   dma_handle_tx->Init.PeriphInc           = DMA_PINC_DISABLE;
   dma_handle_tx->Init.MemInc              = DMA_MINC_ENABLE;
   dma_handle_tx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
   dma_handle_tx->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
   dma_handle_tx->Init.Mode                = DMA_NORMAL;
   dma_handle_tx->Init.Priority            = DMA_PRIORITY_VERY_HIGH;

   HAL_DMA_Init( dma_handle_tx );

   /* Associate the initialized DMA handle to the the UART handle */
   __HAL_LINKDMA( uart_handle, hdmatx, *dma_handle_tx );
}

void platform_uart_update_rx_ring_buffer_tail(platform_uart_driver_t* driver, DMA_HandleTypeDef* dma_handle_rx)
{
    driver->rx_buffer->tail = driver->rx_buffer->size - ( (DMA_Channel_TypeDef *) ( dma_handle_rx->Instance ) )->CNDTR;
}
