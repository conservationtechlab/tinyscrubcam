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
#include "wiced.h"
#include "wpl_transport_uart.h"
#include "wpl_packet.h"
#include "wpl_core.h"
#include "wiced_power_logger.h"

#include "wpl_platform_api.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define WPL_UART_THREAD_NAME     "WPL UART"

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
static wiced_result_t wpl_transport_uart_read_handler( wpl_packet_t** packet );
/******************************************************
 *               Variable Definitions
 ******************************************************/

static wiced_bool_t                                 wpl_transport_uart_thread_initialised = WICED_FALSE;
static wpl_transport_received_packet_handler_t      wpl_transport_received_packet_handler = NULL;

static volatile wiced_bool_t                  uart_thread_running = WICED_FALSE;

wpl_semaphore_t          wpl_stdio_uart_rx_semaphore;

static wpl_thread_t                         uart_thread;

/******************************************************
 *               Function Definitions
 ******************************************************/
void wpl_transport_uart_thread_main( uint32_t arg )
{
    wpl_packet_t* packet = NULL;
    WPRINT_LIB_INFO( ( "wpl_transport_uart_thread_main: enter\n" ) );

    /* turn off buffers, so IO occurs immediately */
#ifdef _IONBF
    setvbuf( stdin, NULL, _IONBF, 0 );
    setvbuf( stdout, NULL, _IONBF, 0 );
    setvbuf( stderr, NULL, _IONBF, 0 );
#endif

    while( 1 )
    {
        if ( ( wpl_get_mode( ) != WPL_MODE_NO_CONSOLE ) && WPL_DEEP_SLEEP_IS_WARMBOOT_HANDLE( ) ) {
            WPRINT_LIB_DEBUG( ( "wpl_transport_uart_thread_main: Already in console mode before warmboot..\n" ) );
            break;
        }
        WPRINT_LIB_DEBUG( ( "wpl_transport_uart_thread_main: Getting into delay\n" ) );
        /* Wait for some time to give command console the opportunity to come up and take UART control */
        if ( wpl_get_mode( ) != WPL_MODE_NO_CONSOLE )
            platform_wpl_delay_milliseconds( WPL_TIME_TO_WAIT_FOR_CONSOLE );
        WPRINT_LIB_DEBUG( ( "wpl_transport_uart_thread_main: Out of delay\n" ) );

        /* Try to get the stdio uart control semaphore */
        platform_wpl_rtos_get_semaphore( &wpl_stdio_uart_rx_semaphore, NEVER_TIMEOUT );

        /* Set to No Console mode */
        wpl_set_mode( WPL_MODE_NO_CONSOLE );
        while ( 1 )
        {
            if ( wpl_get_mode( ) != WPL_MODE_NO_CONSOLE )
            {
                /* PAD mode is not enabled, release the semaphore and wait for the mode to be enabled */
                platform_wpl_rtos_set_semaphore( &wpl_stdio_uart_rx_semaphore );
                break;
            }
            WPRINT_LIB_DEBUG( ( "wpl_transport_uart_thread_main: Got semaphore to read\n" ) );

            if ( wpl_transport_uart_read_handler( &packet ) != WICED_SUCCESS )
            {
                continue;
            }
            /* Read successful. Notify upper layer via callback that a new packet is available */
            wpl_transport_received_packet_handler( packet );
        }
    }
    WICED_END_OF_CURRENT_THREAD( );
}

wiced_result_t wpl_uart_transport_init( wpl_transport_received_packet_handler_t handler )
{
    wiced_result_t result = WICED_SUCCESS;

    if ( handler == NULL )
    {
        return WICED_BADARG;
    }

    if ( wpl_transport_uart_thread_initialised == WICED_TRUE )
    {
        return WICED_SUCCESS;
    }

    /* Create UART thread to receive the packets.
     */
    uart_thread_running     = WICED_TRUE;
    wpl_transport_received_packet_handler    = handler;

    /* Set the semaphore for Command Console or WPL */
    platform_wpl_rtos_set_semaphore( &wpl_stdio_uart_rx_semaphore );


    result = platform_wpl_rtos_create_thread( &uart_thread, WPL_DEFAULT_WORKER_PRIORITY, WPL_UART_THREAD_NAME, wpl_transport_uart_thread_main, WPL_UART_STACK_SIZE, NULL );
    if ( result != WICED_SUCCESS )
    {
        WPRINT_LIB_ERROR( ( "Error creating UART thread\n" ) );
        goto error;
    }
    return WICED_SUCCESS;

    error:
    wpl_uart_transport_deinit( );
    return result;
}

wiced_result_t wpl_uart_transport_deinit( void )
{
    if ( wpl_transport_uart_thread_initialised == WICED_FALSE )
        return WICED_SUCCESS;

    uart_thread_running = WICED_FALSE;
    platform_wpl_rtos_delete_thread( &uart_thread );
    wpl_transport_received_packet_handler = NULL;
    wpl_transport_uart_thread_initialised      = WICED_FALSE;
    return WICED_SUCCESS;
}

wiced_result_t wpl_uart_transport_send_packet( wpl_packet_t* packet )
{
    wiced_result_t result = WICED_SUCCESS;

    if ( wpl_get_console_prints_status( ) )
    {
        uint32_t i = 0;
        for( i = 0; i < packet->packet_size; i++  )
        {
            WPRINT_APP_INFO( ( "0x%x ", *( packet->packet_start+i ) ) );
        }
        WPRINT_APP_INFO( ( "\n" ) );
    }
    else
    {
        result = platform_wpl_uart_transmit_bytes(packet->packet_start, packet->packet_size);
    }
    if ( result != WICED_SUCCESS )
    {
        wpl_free_packet( packet );
        return result;
    }

    /* Destroy packet */
    return wpl_free_packet( packet );
}


wiced_result_t wpl_transport_uart_read_handler( wpl_packet_t** packet )
{
    uint8_t packet_byte=0;
    wiced_result_t    result = WICED_SUCCESS;
    uint32_t payload_len =  0;
    uint32_t       expected_transfer_size = 1;

    /* Get the SYN byte */
    result = platform_wpl_uart_receive_bytes(&packet_byte, &expected_transfer_size, NEVER_TIMEOUT);
    if ( ( result != WICED_SUCCESS ) || ( packet_byte != WPL_SYN_BYTE1 ) )
    {
        return WICED_PACKET_BUFFER_CORRUPT;
    }
    expected_transfer_size = 1;

    result = platform_wpl_uart_receive_bytes(&packet_byte, &expected_transfer_size, NEVER_TIMEOUT);

    if ( ( result != WICED_SUCCESS ) || ( packet_byte != WPL_SYN_BYTE2 ) )
    {
        return WICED_PACKET_BUFFER_CORRUPT;
    }

    /* Get the packet type */
    expected_transfer_size = 1;
    result = platform_wpl_uart_receive_bytes(&packet_byte, &expected_transfer_size, NEVER_TIMEOUT);
    if ( result != WICED_SUCCESS ) {
        return result;
    }

    /* Read the header and determine the   */
    switch ( packet_byte )
    {
        case CMD_TARGET_DETECTION:
        case CMD_GET_TARGET_STATUS:
        case CMD_GET_TARGET_WPL_VERSION:
        case CMD_GET_PROCESSOR_LIST:
        case CMD_LOG_REQUEST:
        case 'L':
        {
            payload_len = 0;
            break;
        }
        case CMD_GET_EVENTS_LIST:
        {
            payload_len = 1;
            break;
        }
        case CMD_GET_EVENT_DESCRIPTOR_LIST:
        {
            payload_len = 2;
            break;
        }
        case CMD_START_LOGGING:
        case CMD_STOP_LOGGING:
        {
            uint8_t events;
            /* Get number of events */
            expected_transfer_size = 1;
            result = platform_wpl_uart_receive_bytes(&events, &expected_transfer_size, NEVER_TIMEOUT);
            if ( result != WICED_SUCCESS )
                return result;
            payload_len = events * 2;
            break;
        }
        case CMD_LOG_POLL_PERIOD:
            payload_len = 1;
            break;
        default:
            return WICED_UNSUPPORTED;
    }
    result = wpl_dynamic_allocate_packet( packet, WPL_PAD_REQUEST, packet_byte, payload_len );
    if ( result != WICED_SUCCESS )
    {
        WPRINT_LIB_DEBUG(("Failed to allocate request packet\n"));
        return result;
    }

    /* Retrieve the payload data */
    if ( payload_len )
    {
        expected_transfer_size = payload_len;
        result = platform_wpl_uart_receive_bytes(( *packet )->payload_start, &expected_transfer_size, NEVER_TIMEOUT);
        if ( result != WICED_SUCCESS )
        {
            WPRINT_LIB_DEBUG(("Failed to read the request packet\n"));
            wpl_free_packet( *packet );
            return result;
        }
    }
    return WICED_SUCCESS;
}

