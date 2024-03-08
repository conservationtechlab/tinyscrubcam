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
 * BCM920739B0 PUART implementation
 */
#include <stdint.h>
#include <string.h>
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_sleep.h"
#include "platform_assert.h"
#include "wwd_assert.h"
#include "wiced_rtos.h"
#include "wiced_bt_trace.h"
#include "wiced_hal_puart.h"
#include "wwd_debug.h"
#include "platform_mcu_peripheral.h"
#include "wiced_power_logger.h"
/******************************************************
 *                      Macros
 ******************************************************/
#define PLAT_UART_RX_BUFSIZE ((uint32_t)64)
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
/*no prototype in wiced_hal_puart.h*/
extern void wiced_hal_puart_register_interrupt(void (*puart_rx_cbk)(void*));
extern void wiced_hal_puart_reset_puart_interrupt(void);
extern void wiced_hal_puart_set_watermark_level(uint32_t watermark_level);

/******************************************************
 *               Variable Definitions
 ******************************************************/
//static uint8_t uart_rdata;
static wiced_bool_t puart_init=WICED_FALSE;
static wiced_bool_t user_ring_buf=WICED_FALSE;
static wiced_semaphore_t uart_r;
static wiced_semaphore_t uart_t;
static wiced_ring_buffer_t *rx_ring_buf = NULL;

/******************************************************
 *               Function Definitions
 ******************************************************/
/*RX callback handler
 * No user argument in the callback, it will be NULL */
void platfrom_puart_rx_intr_callback(void *arg)
{
    UNUSED_PARAMETER(arg);
    uint8_t uart_rdata;
    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_RX );
    if(wiced_hal_puart_read(&uart_rdata) && rx_ring_buf)
    {
        ring_buffer_write( rx_ring_buf, &uart_rdata, sizeof(uart_rdata));
        /*if ring buf was empty, set semaphore, reader could be waiting for data*/
        if(ring_buffer_used_space(rx_ring_buf)==1)
          wiced_rtos_set_semaphore(&uart_r);
    }
    wiced_hal_puart_reset_puart_interrupt( );
    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_IDLE );
}

platform_result_t platform_uart_init( platform_uart_driver_t* driver, const platform_uart_t* peripheral, const platform_uart_config_t* config, wiced_ring_buffer_t* optional_ring_buffer )
{
    uint8_t rx_pin,tx_pin,cts_pin,rts_pin;
    platform_result_t    result = PLATFORM_SUCCESS;

    if(puart_init == WICED_TRUE)
        return PLATFORM_SUCCESS;

    wiced_assert( "bad argument", ( driver != NULL ) && ( peripheral != NULL ) && ( config != NULL ) );
    /* set UART pin mux
    Tx and Rx can't be NULL, CTS/RTS should be set to NULL to disable.
    */
    if((peripheral->rx_pin == NULL)||(peripheral->tx_pin == NULL))
        return PLATFORM_ERROR;

    cts_pin=0; rts_pin=0;
    rx_pin = peripheral->rx_pin->pin_number;
    tx_pin = peripheral->tx_pin->pin_number;
    if(peripheral->cts_pin)
        cts_pin = peripheral->cts_pin->pin_number;
    if(peripheral->rts_pin)
        rts_pin = peripheral->rts_pin->pin_number;

    if (!wiced_hal_puart_select_uart_pads(rx_pin, tx_pin, cts_pin, rts_pin))
        return PLATFORM_ERROR;

    if(WICED_SUCCESS != wiced_rtos_init_semaphore(&uart_r))
        return PLATFORM_ERROR;

    if(WICED_SUCCESS != wiced_rtos_init_semaphore(&uart_t))
        return PLATFORM_ERROR;

    /*optional ring buffer from caller*/
    if(optional_ring_buffer)
    {
        if(optional_ring_buffer->buffer)
        {
            /*if caller provides buffer we don't allocate*/
            user_ring_buf = WICED_TRUE;
            driver->rx_buffer = optional_ring_buffer;
        }
        else
            return PLATFORM_BADARG;
    }
    else
    {
        /*we allocate ring buffer*/
        uint8_t *rx_buf;
        driver->rx_buffer = (wiced_ring_buffer_t*)malloc(sizeof(wiced_ring_buffer_t));
        if(driver->rx_buffer == NULL)
            return PLATFORM_INIT_FAIL;
        rx_buf = (uint8_t*)malloc(PLAT_UART_RX_BUFSIZE);
        if(rx_buf == NULL)
        {
            free(driver->rx_buffer);
            return PLATFORM_INIT_FAIL;
        }
        if (WICED_SUCCESS != ring_buffer_init(driver->rx_buffer, rx_buf, PLAT_UART_RX_BUFSIZE))
        {
            free(rx_buf);
            free(driver->rx_buffer);
            return PLATFORM_ERROR;
        }
        user_ring_buf = WICED_FALSE;
    }

    rx_ring_buf = driver->rx_buffer;

    wiced_set_debug_uart( WICED_ROUTE_DEBUG_TO_PUART );
    wiced_hal_puart_init();
    wiced_hal_puart_flow_off();
    /* copy the device info to driver structure */
    driver->peripheral.device_id = peripheral->device_id;
    driver->peripheral.tx_pin = peripheral->tx_pin;
    driver->peripheral.rx_pin = peripheral->rx_pin;
    driver->peripheral.cts_pin = peripheral->cts_pin;
    driver->peripheral.rts_pin = peripheral->rts_pin;

    memcpy(&driver->uart_config,config,sizeof(platform_uart_config_t));
    /*we don't have separate CTS/RTS control, we set both*/
    if(config->flow_control !=FLOW_CONTROL_DISABLED)
        wiced_hal_puart_flow_on();
    if(config->baud_rate)
        wiced_hal_puart_set_baudrate(config->baud_rate);

    wiced_hal_puart_register_interrupt(platfrom_puart_rx_intr_callback);
    /* set water mark level to 1 to receive interrupt up on receiving each byte */
    wiced_hal_puart_set_watermark_level(1);
    wiced_hal_puart_enable_tx();
    wiced_hal_puart_enable_rx();
    puart_init = WICED_TRUE;
    // Set semaphore for Tx
    wiced_rtos_set_semaphore(&uart_t);
    return result;

}

platform_result_t platform_uart_deinit( platform_uart_driver_t* driver )
{

    wiced_assert( "bad argument", ( driver != NULL ) );
    puart_init = WICED_FALSE;
    wiced_hal_puart_disable_tx();
    wiced_rtos_deinit_semaphore(&uart_r);
    wiced_rtos_deinit_semaphore(&uart_t);
    driver->peripheral.device_id = -1;
    rx_ring_buf = NULL;
    if(!user_ring_buf)
    {
        if(driver->rx_buffer)
        {
            if(driver->rx_buffer->buffer)
                free(driver->rx_buffer->buffer);
            free(driver->rx_buffer);
        }
    }
    driver->rx_buffer = NULL;

    return PLATFORM_SUCCESS;
}

platform_result_t platform_uart_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
{
    wiced_result_t result;
    wiced_assert( "bad argument", ( driver != NULL ) && ( data_out != NULL ) && ( size != 0 ) );

    /* Wait for 10 secs to get Tx semaphore. Timed wait would avoid blocking in error scenarios */
    result = wiced_rtos_get_semaphore( &uart_t, 10000 );
    if ( result != WICED_SUCCESS )
    {
        WPRINT_PLATFORM_ERROR( ("platform_uart_transmit_bytes: Not able to transmit on UART, error: %d\n", result) );
        return PLATFORM_ERROR;
    }
    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_TX );
    while(size)
    {
        wiced_hal_puart_write(*data_out);
        data_out++;
        size--;
    }
    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_IDLE );
    wiced_rtos_set_semaphore( &uart_t );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_uart_receive_bytes( platform_uart_driver_t* driver, uint8_t* data_in, uint32_t* expected_data_size, uint32_t timeout_ms )
{
    uint32_t    bytes_left   = 0;
    platform_result_t result = PLATFORM_SUCCESS;

    wiced_assert( "bad argument", ( driver != NULL ) && ( data_in != NULL ) && ( expected_data_size != 0 ) );
    bytes_left = *expected_data_size;

    if( rx_ring_buf == NULL )
    {
        result = PLATFORM_UNINITLIASED;
    }
    else
    {

        while ( 0 != bytes_left )
        {
            uint32_t bytes_available = 0;
            uint8_t* available_data = NULL;
            /* Get available bytes and pointer to available data in the ring buffer */
            ring_buffer_get_data( rx_ring_buf, &available_data, &bytes_available );
            bytes_available = MIN( bytes_available, bytes_left );

            /*if no rx data, wait for data until timeout*/
            if ( bytes_available == 0 )
            {
                result = wiced_rtos_get_semaphore( &uart_r, timeout_ms );
                if ( PLATFORM_SUCCESS != result )
                {
                    rx_ring_buf->head = 0;
                    rx_ring_buf->tail = 0;
                    break;
                }
                continue;
            }
            memcpy( data_in, available_data, bytes_available );
            data_in       += bytes_available;
            bytes_left    -= bytes_available;
            ring_buffer_consume( rx_ring_buf, bytes_available );
        }
    }

    /* Return actual bytes read */
    *expected_data_size -= bytes_left;

    return result;
}

platform_result_t platform_uart_ioctl(platform_uart_driver_t* driver, platform_uart_ioctl_cmd_t cmd, platform_uart_conf_t* conf)
{
    switch(cmd)
    {
    case PLATFORM_UART_IOCTL_SET_SPEED:
        wiced_hal_puart_set_baudrate(conf->baud_rate);
        break;
    case PLATFORM_UART_IOCTL_GET_SPEED:
        conf->baud_rate = driver->uart_config.baud_rate;
        break;
    case PLATFORM_UART_IOCTL_ENABLE_HW_FLOW_CTRL:
        wiced_hal_puart_flow_on();
        break;
    case PLATFORM_UART_IOCTL_DISABLE_HW_FLOW_CTRL:
        wiced_hal_puart_flow_off();
        break;
    default:
        return PLATFORM_ERROR;
    }
    return PLATFORM_SUCCESS;
}
platform_result_t platform_uart_exception_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
{
    /* Called in exception context and must not use interrupts.
     * we call the normal UART Tx for now*/
    if ( ( driver == NULL ) || ( data_out == NULL ) || ( size == 0 ) )
    {
        return PLATFORM_ERROR;
    }
    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_TX );
    while(size)
    {
        wiced_hal_puart_write(*data_out);
        data_out++;
        size--;
    }
    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_IDLE );
    return PLATFORM_SUCCESS;
}

#ifdef WICED_POWER_LOGGER_ENABLE
int wiced_va_printf(char * buffer, int len, va_list va);

int wpl_wiced_printf(char * buffer, int len, ...)
{
    int used = 0;
    va_list va;
    wiced_result_t result;

    result = wiced_rtos_get_semaphore( &uart_t, 10000 );
    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_TX );

    va_start(va,len);
    used = wiced_va_printf(buffer, len, va);
    va_end(va);

    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_IDLE );

    if(result == WICED_SUCCESS)
        wiced_rtos_set_semaphore( &uart_t );

    return used;
}
#endif
