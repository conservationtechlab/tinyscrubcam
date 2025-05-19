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
#if defined ( IAR_TOOLCHAIN )
#include "platform_cmis.h"
#endif
#include <stdint.h>
#include <string.h>
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_sleep.h"
#include "platform_assert.h"
#include "wwd_assert.h"
#include "brcm_fw_types.h"

#include "platform_mcu_peripheral.h"
#include "wwd_rtos_interface.h"
#include "20739mapa0.h"
#include "wiced_hal_gpio.h"
#include "spiffy_common.h"
#ifdef DMA_ENABLED
#include "bcs_dma.h"
#endif
#include "wiced_rtos.h"
#include "wiced_power_logger.h"
/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#ifdef DMA_ENABLED
#define SPI_RX_CHANNEL                              2
#define SPI_TX_CHANNEL                              1
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

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/
/* To protect spi init being called by modules to communicate with different slave devices */
static int is_spi_init = 0;

#ifdef DMA_ENABLED
static wiced_semaphore_t dma_rx_semaphore, dma_tx_semaphore;
static DMA_REQ_t spi_tx_dma_req, spi_rx_dma_req;
#endif

/******************************************************
 *               Function Definitions
 ******************************************************/
#ifdef DMA_ENABLED

void spi_tx_dma_done( void* arg )
{
    wiced_rtos_set_semaphore( &dma_tx_semaphore );
}

void spi_rx_dma_done( void* arg )
{
    wiced_rtos_set_semaphore( &dma_rx_semaphore );
}
void spi_dma_init( SpiffyInstance instance )
{
    uint32_t spiffy_tx_fifo = ( instance == SPIFFYD_1 ) ? (UINT32) spiffy_TxFIFO_adr : (UINT32) spiffy2_spiffy_TxFIFO_adr;
    uint32_t spiffy_rx_fifo = ( instance == SPIFFYD_1 ) ? (UINT32) spiffy_RxFIFO_adr : (UINT32) spiffy2_spiffy_RxFIFO_adr;
    uint32_t memory_to_ptu_index = ( instance == SPIFFYD_1 ) ? (UINT32) DMAC_PERIPHERAL_INDEX_MEMORY_TO_PTU : (UINT32) DMAC_PERIPHERAL_INDEX_MEMORY_TO_PTU_AUX;
    uint32_t ptu_to_memory_index = ( instance == SPIFFYD_1 ) ? (UINT32) DMAC_PERIPHERAL_INDEX_PTU_TO_MEMORY : (UINT32) DMAC_PERIPHERAL_INDEX_PTU_AUX_TO_MEMORY;

    //RX init
    spi_rx_dma_req.channel = SPI_RX_CHANNEL;
    spi_rx_dma_req.DMACCx_first_segment_regs.DMACCxSrcAddr = (void *) spiffy_rx_fifo;
    spi_rx_dma_req.DMACCx_first_segment_regs.DMACCxLLI = 0;
    spi_rx_dma_req.DMACCxConfiguration = DMACCXCONFIG_DMA_DONE_INTERRUPT_ENABLE | DMACCXCONFIG_DMA_ERROR_INTERRUPT_ENABLE | DMACCXCONFIG_FLOWCNTRL_PERIPH_TO_MEMORY_SRC_FLOW | ( ptu_to_memory_index << DMACCXCONFIG_SRC_PERIPHERAL_INDEX_SHIFT ) | DMACCXCONFIG_CHANNEL_ENABLED;
    spi_rx_dma_req.fp_DoneInterrupt = spi_rx_dma_done;
    spi_rx_dma_req.fp_DoneInterrupt_arg = NULL;
    spi_rx_dma_req.DMACCx_first_segment_regs.DMACCxControl =
    DMACCXCONTROL_DMA_DONE_INTERRUPT_ENABLE | DMACCXCONTROL_DEST_INCREMENT | DMACCXCONTROL_DEST_WIDTH_8_BITS | DMACCXCONTROL_SRC_WIDTH_8_BITS | DMACCXCONTROL_DEST_BURST_SIZE_16_TRANSFERS | DMACCXCONTROL_SRC_BURST_SIZE_16_TRANSFERS;

    dma_SetPeripheralDMACSync( DMACCXCONFIG_SRC_PERIPHERAL_INDEX_SHIFT, FALSE );

    //TX init
    spi_tx_dma_req.channel = SPI_TX_CHANNEL;
    spi_tx_dma_req.DMACCx_first_segment_regs.DMACCxDestAddr = (void *) spiffy_tx_fifo;
    spi_tx_dma_req.DMACCx_first_segment_regs.DMACCxLLI = 0;
    spi_tx_dma_req.DMACCxConfiguration = DMACCXCONFIG_DMA_DONE_INTERRUPT_ENABLE | DMACCXCONFIG_DMA_ERROR_INTERRUPT_ENABLE | DMACCXCONFIG_FLOWCNTRL_MEMORY_TO_PERIPH_DEST_FLOW | ( memory_to_ptu_index << DMACCXCONFIG_DEST_PERIPHERAL_INDEX_SHIFT ) | DMACCXCONFIG_CHANNEL_ENABLED;
    spi_tx_dma_req.DMACCx_first_segment_regs.DMACCxControl = DMACCXCONTROL_DMA_DONE_INTERRUPT_ENABLE | DMACCXCONTROL_SRC_INCREMENT | DMACCXCONTROL_DEST_WIDTH_8_BITS | DMACCXCONTROL_SRC_WIDTH_8_BITS | DMACCXCONTROL_DEST_BURST_SIZE_16_TRANSFERS | DMACCXCONTROL_SRC_BURST_SIZE_16_TRANSFERS;
    spi_tx_dma_req.fp_DoneInterrupt = spi_tx_dma_done;
    spi_tx_dma_req.fp_DoneInterrupt_arg = NULL;
    dma_SetPeripheralDMACSync( DMACCXCONFIG_DEST_PERIPHERAL_INDEX_SHIFT, FALSE );

    wiced_rtos_init_semaphore( &dma_rx_semaphore );
    wiced_rtos_init_semaphore( &dma_tx_semaphore );
}

void spiffy_dma_tx( SpiffyInstance instance, uint16_t total_len, const uint8_t * p_tx_data )
{
    tSPIFFY_REG *spiffyRegPtr = ( instance == SPIFFYD_1 ) ? (tSPIFFY_REG *) spiffy_cfg_adr : (tSPIFFY_REG *) spiffy2_spiffy_cfg_adr;

    spi_tx_dma_req.DMACCx_first_segment_regs.DMACCxSrcAddr = (void *) p_tx_data;
    spiffyRegPtr->tx_dma_len = total_len;
    spi_tx_dma_req.fp_DoneInterrupt = spi_tx_dma_done;

    spiffy_disableFifos( spiffyRegPtr );
    dma_RequestTransfer( &spi_tx_dma_req );
    spiffy_masterEnableTxOnly( spiffyRegPtr );

    spiffyRegPtr->int_status = SPIFFY_INT_GENERIC_SPI_MASTER_DONE;
    spiffyRegPtr->transmission_len = total_len;

    wiced_rtos_get_semaphore( &dma_tx_semaphore, NEVER_TIMEOUT );

    //Observed delay between DMA interrupt and actual SPI operation
    //complete. To be on safe side after DMA completion,
    //waiting on SPI interrupt status too.
    while ( !( spiffyRegPtr->int_status & SPIFFY_INT_GENERIC_SPI_MASTER_DONE ) )
        ;
}

void spiffy_dma_rx( SpiffyInstance instance, uint16_t total_len, const uint8_t * p_tx_data, const uint8_t * p_rx_data )
{
    tSPIFFY_REG *spiffyRegPtr = ( instance == SPIFFYD_1 ) ? (tSPIFFY_REG *) spiffy_cfg_adr : (tSPIFFY_REG *) spiffy2_spiffy_cfg_adr;

    spi_tx_dma_req.DMACCx_first_segment_regs.DMACCxSrcAddr = (void *) p_tx_data;
    spi_rx_dma_req.DMACCx_first_segment_regs.DMACCxDestAddr = (void *) p_rx_data;
    spiffyRegPtr->tx_dma_len = total_len;
    spiffyRegPtr->rx_dma_len = total_len;
    spi_tx_dma_req.fp_DoneInterrupt = NULL;

    spiffy_disableFifos( spiffyRegPtr );

    dma_RequestTransfer( &spi_tx_dma_req );
    spiffy_masterEnableFd( spiffyRegPtr );

    spiffyRegPtr->int_status = SPIFFY_INT_GENERIC_SPI_MASTER_DONE;
    spiffyRegPtr->transmission_len = total_len;

    dma_RequestTransfer( &spi_rx_dma_req );
    wiced_rtos_get_semaphore( &dma_rx_semaphore, NEVER_TIMEOUT );
}
#endif

platform_result_t platform_spi_init( const platform_spi_t* spi, const platform_spi_config_t* config )
{
    uint32_t portdiv = 0x0;
    uint32_t input = 0x0;
#ifdef DMA_ENABLED
    spi_dma_init( spi->port );
#endif

    /*spiffy output*/
    REG32(iocfg_fcn_p0_adr+(4*spi->pin_clock->pin_number)) = spi->pin_clock->mux_mode; //clk = 0x25 i.e. P36
    REG32(iocfg_fcn_p0_adr+(4*spi->pin_miso->pin_number)) = spi->pin_miso->mux_mode; //miso = 0x26 i.e. P37
    REG32(iocfg_fcn_p0_adr+(4*spi->pin_mosi->pin_number)) = spi->pin_mosi->mux_mode; //mosi = 0x27 i.e. P38
    REG32(iocfg_fcn_p0_adr+(4*config->chip_select->pin_number)) = config->chip_select->mux_mode; //cs = 0x28 i.e. P39

    /*spiffy input*/
    if ( spi->port == SPI1 )
    {
        REG32(mia_dbg_adr) |= ( 0x1 << 8 );
        input = ( ( ( config->chip_select->pin_number + 1 ) << 24 ) | ( ( spi->pin_clock->pin_number + 1 ) << 16 ) );
        REG32(iocfg_premux_0_adr) = input; // cs, clk, 0, 0 //0x28250000
        input = ( ( ( spi->pin_miso->pin_number + 1 ) << 8 ) | ( spi->pin_mosi->pin_number + 1 ) );
        REG32(iocfg_premux_1_adr) = input; // 0, 0, miso, mosi //0x2627
        WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SPI_1, EVENT_DESC_SPI_IDLE );
    }
    else if ( spi->port == SPI2 )
    {
        REG32(mia_dbg_adr) |= ( 0x1 << 16 );
        input = ( ( ( spi->pin_mosi->pin_number + 1 ) << 24 ) | ( ( config->chip_select->pin_number + 1 ) << 16 ) | ( ( spi->pin_clock->pin_number + 1 ) << 8 ) ); //0x27282500;
        REG32(iocfg_premux_2_adr) = input; // mosi, cs, clk //0x27282500
        REG32(iocfg_premux_3_adr) = ( spi->pin_miso->pin_number + 1 );       // miso = 0x26
    }
    else
    {
        //Error;
        return PLATFORM_ERROR;
    }

    switch ( config->speed )
    {
        case 8000000:   //8MHz
            portdiv = SPIFFY_TPORT_DIV_2;
            break;
        case 12000000:  //12MHz
            portdiv = SPIFFY_TPORT_DIV_1;
            break;
        default:
            portdiv = SPIFFY_TPORT_DIV_0;  ////24MHz
            break;
    }

    wiced_hal_gpio_configure_pin( spi->pin_clock->pin_number, GPIO_PULL_UP, GPIO_PIN_OUTPUT_HIGH );
    wiced_hal_gpio_configure_pin( spi->pin_miso->pin_number, GPIO_PULL_UP, GPIO_PIN_OUTPUT_HIGH );
    wiced_hal_gpio_configure_pin( spi->pin_mosi->pin_number, GPIO_PULL_UP, GPIO_PIN_OUTPUT_HIGH );
    wiced_hal_gpio_configure_pin( config->chip_select->pin_number, GPIO_PULL_UP, GPIO_PIN_OUTPUT_HIGH );

    is_spi_init++;

    /*SPIffy Configuration register settings    */
    spiffy_sw_reset( (uint32_t) spi->port,
    SPIFFY_CFG_SPIMODE_11 | /*SPI Mode :11b -SPI Mode 3,falling clock edge,rising clock edge */
    SPIFFY_CFG_FLOWCTL_TX_RX | /*Flow Control:10b - Full-duplex flow control */
    SPIFFY_CFG_AUTOCS | /*AutoCS:1b */
    SPIFFY_CFG_BIT_BE | /*BitEndianness:1b - Big Endian */
    SPIFFY_CFG_PROTOCOL_GM, /*Protocol:010b - Generic Master */
    portdiv ); /*spiffy_ClockCfg_adr register = 0x02, 12MHz */

    return PLATFORM_SUCCESS;

}

platform_result_t platform_spi_deinit( const platform_spi_t* spi )
{
    UNUSED_PARAMETER( spi );
    if ( --is_spi_init == 0 )
    {
        if ( spi->port == SPI1 )
            WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SPI_1, EVENT_DESC_SPI_OFF );
    }

    return PLATFORM_SUCCESS;
}

platform_result_t platform_spi_transfer( const platform_spi_t* spi, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments )
{
    platform_result_t result = PLATFORM_SUCCESS;
    uint32_t count = 0;
    uint16_t i;
    uint8_t defval = 0xFF;

    wiced_assert( "bad argument", ( spi != NULL ) && ( config != NULL ) && ( segments != NULL ) && ( number_of_segments != 0 ) );
    platform_mcu_powersave_disable();

    for ( i = 0; i < number_of_segments; i++ )
    {
        count = segments[ i ].length;

        //handled by spiffy driver
        const uint8_t* send_ptr = (const uint8_t*) segments[ i ].tx_buffer;
        uint8_t* rcv_ptr = (uint8_t*) segments[ i ].rx_buffer;
        uint8_t* data = &defval;

        if ( send_ptr != NULL )
        {
            data = (uint8_t*) send_ptr;
        }

        if ( rcv_ptr == NULL )
        {
            if(spi->port == SPI1)
                WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SPI_1, EVENT_DESC_SPI_WRITE );
#ifdef DMA_ENABLED
            //Avoid DMA for shorter length SPI transfers
            //This may slow down operations especially during Wifi boot up
            if ( count > 100 )
                spiffy_dma_tx( spi->port, count, data );
            else
                spiffyd_txData( spi->port, count, data );
#else
            spiffyd_txData(spi->port, count, data);
#endif
            if(spi->port == SPI1)
                WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SPI_1, EVENT_DESC_SPI_IDLE );
        }
        else
        {
            if(spi->port == SPI1)
                WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SPI_1, EVENT_DESC_SPI_READ );
#ifdef DMA_ENABLED
            spiffy_dma_rx( spi->port, count, data, rcv_ptr );
#else
            spiffyd_exchangeData(spi->port, count, data, rcv_ptr);
#endif
            if(spi->port == SPI1)
                WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SPI_1, EVENT_DESC_SPI_IDLE );
        }
    }

    platform_mcu_powersave_enable();

    return result;
}

platform_result_t platform_spi_slave_init( platform_spi_slave_driver_t* driver, const platform_spi_t* peripheral, const platform_spi_slave_config_t* config )
{
    return PLATFORM_SUCCESS;
}

platform_result_t platform_spi_slave_deinit( platform_spi_slave_driver_t* driver )
{
    return PLATFORM_SUCCESS;
}

platform_result_t platform_spi_slave_send_error_status( platform_spi_slave_driver_t* driver, platform_spi_slave_transfer_status_t error_status )
{
    return PLATFORM_SUCCESS;
}

platform_result_t platform_spi_slave_receive_command( platform_spi_slave_driver_t* driver, platform_spi_slave_command_t* command, uint32_t timeout_ms )
{
    return PLATFORM_SUCCESS;
}

platform_result_t platform_spi_slave_transfer_data_internal( platform_spi_slave_driver_t* driver, platform_spi_slave_transfer_direction_t direction, platform_spi_slave_data_buffer_t* buffer, uint32_t timeout_ms )
{
    return PLATFORM_SUCCESS;
}
platform_result_t platform_spi_slave_transfer_data( platform_spi_slave_driver_t* driver, platform_spi_slave_transfer_direction_t direction, platform_spi_slave_data_buffer_t* buffer, uint32_t timeout_ms )
{
    return PLATFORM_SUCCESS;
}
