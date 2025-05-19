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
 * RFCOMM Server Sample Application
 *
 */

#include "wiced_bt_cfg.h"
#include <string.h>
#include <stdio.h>
#include "wiced.h"
#include "wiced_bt_stack.h"
#include "wiced_bt_dev.h"
#include "wiced_bt_sdp.h"
#include "wiced_bt_rfcomm.h"
#include "wiced_bt_cfg.h"
#include "command_console_bt_coex_test_rfcomm.h"

extern uint32_t data_acl_tx_counter;
extern uint32_t data_acl_rx_counter;
extern wiced_time_t start_time;
extern wiced_time_t end_time;
/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
/* Mask of RFCOMM events handled by app callback */
#define BT_RFCOMM_SERVER_EVT_MASK   ( ( wiced_bt_rfcomm_port_event_t)(PORT_EV_FC | PORT_EV_FCS | PORT_EV_RXCHAR | \
                                            PORT_EV_TXEMPTY | PORT_EV_CTS | PORT_EV_DSR | \
                                            PORT_EV_RING | PORT_EV_CTSS | PORT_EV_DSRS ) )

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
 *               Function Declarations
 ******************************************************/
void bt_rfcomm_server_init( void );
//static void bt_rfcomm_server_log_data( uint8_t *p_data, uint16_t len );

/******************************************************
 *               Variable Definitions
 ******************************************************/
uint16_t bt_rfcomm_server_handle;
uint16_t bt_rfcomm_port;
uint32_t data_rfcomm_rx_counter;
uint32_t data_rfcomm_tx_counter;
BOOLEAN acl_data_rx_flag;
extern int send_data_type;
extern wiced_semaphore_t send_data_semaphore;
#define SEND_DATA_RFCOMM 2

/******************************************************
 *               Function Definitions
 ******************************************************/

/* RFCOMM connection management callback callback */
void bt_rfcomm_server_conn_cback( wiced_bt_rfcomm_result_t code, uint16_t port_handle )
{
    WPRINT_APP_INFO( ( "[%s]\n", __func__ ) );
    if ( code == WICED_BT_RFCOMM_SUCCESS )
    {
        /* RFCOMM connection established. Send test string to remote device */
        WPRINT_APP_INFO( ( "RFCOMM connection established.\n" ) );
    }
    else if ( code == WICED_BT_RFCOMM_CLOSED )
    {
        WPRINT_APP_INFO( ( "RFCOMM connection closed.\n" ) );
        wiced_time_get_time(&end_time);
    }
    else
    {
        WPRINT_APP_INFO( ( "%s unhandled code=0x%x\n", __FUNCTION__, code ) );
    }
}

/* RFCOMM port event callback */
void bt_rfcomm_server_evt_cback( wiced_bt_rfcomm_port_event_t event, uint16_t port_handle )
{
    //WPRINT_APP_INFO( ( "bt_rfcomm_server_evt_cback event mask=0x%04X\n", (int)event ) );
    bt_rfcomm_port = port_handle;
    if (event == 0x04) // stands for transmit queue is empty
    {
        //WPRINT_APP_INFO(("sending RFCOMM data , send_data_type = %d\n" , send_data_type));
        if(send_data_type != 0){
            send_data_type = SEND_DATA_RFCOMM;
            wiced_rtos_set_semaphore( &send_data_semaphore );
            data_acl_tx_counter += BT_RFCOMM_SERVER_APP_MTU;
            //WPRINT_APP_INFO( ( "bt_rfcomm_server_evt_cback data_acl_tx_counter= %lu\n", data_acl_tx_counter ) );
        }
    }
    else if( event == PORT_EV_CONNECT_ERR)
    {
        wiced_time_get_time(&end_time);
    }
    else
    {
        //WPRINT_APP_INFO(("Others \n"));
    }
}

/* RFCOMM Data RX callback */
int bt_rfcomm_server_data_cback( uint16_t port_handle, void *p_data, uint16_t len )
{

    //WPRINT_APP_INFO( ( "RFCOMM RX (len=%i)\n", len ) );
    if ( acl_data_rx_flag == FALSE)
    {
        acl_data_rx_flag = TRUE;
        start_time = 0;
        wiced_time_get_time(&start_time);
    }
    data_acl_rx_counter += len;
    //WPRINT_APP_INFO( ( "RFCOMM RX (len=%lu)\n", data_acl_rx_counter ) );
    return WICED_TRUE;
}

/* Initialize the RFCOMM */
void bt_rfcomm_init( void )
{
    WPRINT_APP_INFO(("bt_rfcomm_init \n"));
    /* Initialize SDP server database for rfcble_app */
    wiced_bt_sdp_db_init( (UINT8 *)wiced_bt_sdp_db, wiced_bt_sdp_db_size );

    /* Enable connectablity (use default connectablity window/interval) */
    wiced_bt_dev_set_connectability ( BTM_CONNECTABLE, 0, 0 );

    /* Enable discoverablity (use default discoverablity window/interval) */
    wiced_bt_dev_set_discoverability ( BTM_GENERAL_DISCOVERABLE, 0, 0 );
}

/* Initialize the RFCOMM server */
void bt_rfcomm_server_init( void )
{
    WPRINT_APP_INFO(("rfcomm_server_init \n"));
    int result = 0;

    wiced_bt_device_address_t bd_addr = { 0 };

    /* Init RFCOMM */
    bt_rfcomm_init();

    /* Create RFCOMM server connection */
    result = wiced_bt_rfcomm_create_connection( UUID_SERVCLASS_SERIAL_PORT,
                BT_RFCOMM_SERVER_APP_SCN,
                TRUE,
                BT_RFCOMM_SERVER_APP_MTU,
                bd_addr,
                &bt_rfcomm_server_handle,
                bt_rfcomm_server_conn_cback );

    WPRINT_APP_INFO(("rfcomm create_connection result:%d\n", result ));

    /* Set data callback RFCOMM */
    wiced_bt_rfcomm_set_data_callback( bt_rfcomm_server_handle, bt_rfcomm_server_data_cback );

    /* Set event callback RFCOMM, and events to be notified of */
    wiced_bt_rfcomm_set_event_mask( bt_rfcomm_server_handle, BT_RFCOMM_SERVER_EVT_MASK );
    wiced_bt_rfcomm_set_event_callback( bt_rfcomm_server_handle, bt_rfcomm_server_evt_cback );

    WPRINT_APP_INFO( ( "Waiting for RFCOMM connection (scn=%i)...\n", BT_RFCOMM_SERVER_APP_SCN ) );

}

/* Initialize the RFCOMM Client */
void bt_rfcomm_client_init( void )
{
    WPRINT_APP_INFO(("rfcomm_client_init \n"));
    //int result = 0;

    //wiced_bt_device_address_t bd_addr = { 0 };

    /* Init RFCOMM */
    bt_rfcomm_init();


}
