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
 *This implemenration is not complete.
 */
#include <stdint.h>
#include <string.h>
#include "platform_cmsis.h"
#include "platform_constants.h"
#include "platform_peripheral.h"
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

void SPI_Init(LPC_SPI_T *pSPI, platform_spi_peripheral_config_t* spi_config);

/******************************************************
 *                   Variables
 ******************************************************/

/* SPI peripheral clock functions */
static const platform_peripheral_clock_function_t spi_peripheral_clock_functions[NUMBER_OF_SPI_PORTS] =
{
    [0] = Chip_Clock_Enable,
};

/* SPI peripheral clocks */
static const uint32_t spi_peripheral_clocks[NUMBER_OF_SPI_PORTS] =
{
    [0] = CLK_SPI,
};

/******************************************************
 *               Function Definitions
 ******************************************************/

uint8_t platform_spi_get_port_number( platform_spi_port_t* spi )
{
    UNUSED_PARAMETER (spi );
    return 0;
}

platform_result_t platform_spi_init( const platform_spi_t* spi, const platform_spi_config_t* config )
{
    /* TODO: unimplemented */
    UNUSED_PARAMETER( spi );
    UNUSED_PARAMETER( config );

    return WICED_SUCCESS;
}

void SPI_Init(LPC_SPI_T *pSPI, platform_spi_peripheral_config_t* spi_config)
{
    UNUSED_PARAMETER( spi_config );
}

platform_result_t platform_spi_deinit( const platform_spi_t* spi )
{
    UNUSED_PARAMETER( spi );
    /* TODO: unimplemented */
    return PLATFORM_SUCCESS;
}

platform_result_t platform_spi_transfer( const platform_spi_t* spi, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments )
{
    platform_result_t result = PLATFORM_SUCCESS;
    /* TODO: unimplemented */
    UNUSED_PARAMETER( spi );
    UNUSED_PARAMETER( number_of_segments );
    UNUSED_PARAMETER( config );
    UNUSED_PARAMETER( segments );

    return result;
}

