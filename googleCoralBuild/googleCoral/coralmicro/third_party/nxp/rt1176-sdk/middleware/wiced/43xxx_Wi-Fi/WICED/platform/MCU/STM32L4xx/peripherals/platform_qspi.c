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
 * STM32L4xx QSPI implementation
 */
#include "platform_peripheral.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/* Definition for QSPI clock resources */
#define QSPI_CLK_ENABLE()              __HAL_RCC_QSPI_CLK_ENABLE()
#define QSPI_CLK_DISABLE()             __HAL_RCC_QSPI_CLK_DISABLE()
#define QSPI_CS_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOG_CLK_ENABLE()
#define QSPI_CLK_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_BK1_D0_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()
#define QSPI_BK1_D1_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()
#define QSPI_BK1_D2_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()
#define QSPI_BK1_D3_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()

#define QSPI_MDMA_CLK_ENABLE()         __HAL_RCC_DMA1_CLK_ENABLE()

#define QSPI_FORCE_RESET()             __HAL_RCC_QSPI_FORCE_RESET()
#define QSPI_RELEASE_RESET()           __HAL_RCC_QSPI_RELEASE_RESET()

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
DMA_HandleTypeDef hdma ={ 0 };
extern const platform_gpio_t wiced_qspi_flash[ ];

void platform_hal_qspi_mspinit( QSPI_HandleTypeDef* hqspi )
{

    /* Enable the QuadSPI memory interface clock */
    QSPI_CLK_ENABLE( );

    /* Reset the QuadSPI memory interface */
    QSPI_FORCE_RESET( );
    QSPI_RELEASE_RESET( );

    /* Enable DMA clock */
    QSPI_MDMA_CLK_ENABLE( );

    platform_gpio_set_alternate_function( wiced_qspi_flash[ WICED_QSPI_PIN_CS ].port, wiced_qspi_flash[ WICED_QSPI_PIN_CS ].pin_number, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF10_QUADSPI );
    platform_gpio_set_alternate_function( wiced_qspi_flash[ WICED_QSPI_PIN_CLK].port, wiced_qspi_flash[ WICED_QSPI_PIN_CLK].pin_number, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF10_QUADSPI );
    platform_gpio_set_alternate_function( wiced_qspi_flash[ WICED_QSPI_PIN_D0 ].port, wiced_qspi_flash[ WICED_QSPI_PIN_D0 ].pin_number, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF10_QUADSPI );
    platform_gpio_set_alternate_function( wiced_qspi_flash[ WICED_QSPI_PIN_D1 ].port, wiced_qspi_flash[ WICED_QSPI_PIN_D1 ].pin_number, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF10_QUADSPI );
    platform_gpio_set_alternate_function( wiced_qspi_flash[ WICED_QSPI_PIN_D2 ].port, wiced_qspi_flash[ WICED_QSPI_PIN_D2 ].pin_number, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF10_QUADSPI );
    platform_gpio_set_alternate_function( wiced_qspi_flash[ WICED_QSPI_PIN_D3 ].port, wiced_qspi_flash[ WICED_QSPI_PIN_D3 ].pin_number, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF10_QUADSPI );

    /* NVIC configuration for QSPI interrupt */
    HAL_NVIC_SetPriority( QUADSPI_IRQn, 0x0F, 0 );
    HAL_NVIC_EnableIRQ( QUADSPI_IRQn );

    /* Enable MDMA clock */
    /* Input MDMA */
    /* Set the parameters to be configured */

    hdma.Init.Request             = DMA_REQUEST_5;
    hdma.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma.Init.MemInc              = DMA_MINC_ENABLE;
    hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma.Init.Mode                = DMA_NORMAL;
    hdma.Init.Priority            = DMA_PRIORITY_LOW;
    hdma.Instance                 = DMA1_Channel5;

    /* Associate the DMA handle */
    __HAL_LINKDMA( hqspi, hdma, hdma );

    /* DeInitialize the MDMA Stream */
    HAL_DMA_DeInit( &hdma );
    /* Initialize the MDMA stream */
    HAL_DMA_Init( &hdma );

    /* Enable and set QuadSPI interrupt to the lowest priority */
    HAL_NVIC_SetPriority( DMA1_Channel5_IRQn, 0x00, 0 );
    HAL_NVIC_EnableIRQ( DMA1_Channel5_IRQn );
}

void platform_hal_qspi_mspdeinit( QSPI_HandleTypeDef *hqspi )
{
    HAL_NVIC_DisableIRQ( QUADSPI_IRQn );

    /* De-configure DMA channel */
    HAL_DMA_DeInit( &hdma );

    /* De-Configure QSPI pins */
    platform_gpio_deinit( &wiced_qspi_flash[ WICED_QSPI_PIN_CS ] );
    platform_gpio_deinit( &wiced_qspi_flash[ WICED_QSPI_PIN_CLK ] );
    platform_gpio_deinit( &wiced_qspi_flash[ WICED_QSPI_PIN_D0 ] );
    platform_gpio_deinit( &wiced_qspi_flash[ WICED_QSPI_PIN_D1 ] );
    platform_gpio_deinit( &wiced_qspi_flash[ WICED_QSPI_PIN_D2 ] );
    platform_gpio_deinit( &wiced_qspi_flash[ WICED_QSPI_PIN_D3 ] );

    /* Reset the QuadSPI memory interface */
    QSPI_FORCE_RESET( );
    QSPI_RELEASE_RESET( );

    /* Disable the QuadSPI memory interface clock */
    QSPI_CLK_DISABLE( );
}
