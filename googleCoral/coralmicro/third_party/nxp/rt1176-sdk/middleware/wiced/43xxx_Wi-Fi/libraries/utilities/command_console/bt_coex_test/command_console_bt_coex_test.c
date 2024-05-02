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
#include "command_console.h"
#include "string.h"
#include <stdio.h>
#include "wwd_debug.h"
#include "bt_target.h"
#include "command_console.h"
#include "wwd_assert.h"
#include "stdlib.h"
#include "wiced_bt_stack.h"
#include "wiced.h"
#include "internal/wiced_internal_api.h"
#include "wiced_bt_ble.h"
#include "wiced_bt_gatt.h"
#include "wiced_bt_cfg.h"
#include "wiced_bt_stack.h"
#include "gattdefs.h"
#include "sdpdefs.h"
#include "wiced_bt_sdp.h"
#include "wiced_bt_a2dp_sink.h"
#include "wiced_bt_l2c.h"
#include "wiced_bt_dev.h"
#include "wiced_bt_rfcomm.h"
#include "command_console_bt_coex_test_rfcomm.h"
#include "wiced_time.h"


extern wiced_semaphore_t*  wiced_rtos_create_semaphore( void );
extern uint16_t bt_rfcomm_port;
extern void bt_rfcomm_server_init( void );
extern int a2dp_sink_start( void );
extern const wiced_bt_cfg_settings_t wiced_bt_cfg_settings;
extern const wiced_bt_cfg_buf_pool_t wiced_bt_cfg_buf_pools[];


/******************************************************
 *                      Macros
 ******************************************************/
#define SEND_DATA_LE                    1
#define SEND_DATA_RFCOMM                2
#define STOP_DATA_SEND                  0
#define APP_WORKER_THREAD_STACK_SIZE    (2048)
#define APP_QUEUE_MAX_ENTRIES           (5)
#define DISABLE_BLUETOOTH_LPM

BOOLEAN bt_state = WICED_FALSE;

/**
 * Macro to verify that BT is on
 * if not, return with result value;
 */
#define IS_BT_ON()   { if (!bt_state) { WPRINT_APP_INFO( (" Error BT is OFF \r\n")); return WICED_ERROR;}}


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
 *               Variable Definitions
 ******************************************************/
uint8_t app_worker_thread_stack[ APP_WORKER_THREAD_STACK_SIZE ] __attribute__((section (".ccm")));
uint8_t rfcomm_data [BT_RFCOMM_SERVER_APP_MTU] = {[0 ... (BT_RFCOMM_SERVER_APP_MTU-1)] = 0x55};
wiced_semaphore_t send_data_semaphore;
int send_data_type;
uint32_t data_le_tx_counter;
uint32_t data_le_rx_counter;
uint32_t data_acl_tx_counter;
uint32_t data_acl_rx_counter;
BOOLEAN le_coc_data_rx_flag;
wiced_time_t start_time;
wiced_time_t end_time;
wiced_thread_t notify_thread;


/******************************************************
 *               Static Function Declarations
 ******************************************************/
static void send_data_thread( uint32_t args );

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
void ble_app_start( void );
extern wiced_bt_dev_status_t bt_dualmode_management_cback( wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data );
void le_coc_set_advertisement_data( void );
void le_coc_connect_ind_cback( void *context, BD_ADDR bda, UINT16 local_cid, UINT16 psm, UINT8 id, UINT16 mtu_peer );
void le_coc_connect_cfm_cback( void *context, UINT16 local_cid, UINT16 result, UINT16 mtu_peer );
void le_coc_disconnect_ind_cback( void *context, UINT16 local_cid, BOOLEAN ack );
void le_coc_disconnect_cfm_cback( void *context, UINT16 local_cid, UINT16 result );
void le_coc_congestion_cback( void *context, UINT16 local_cid, BOOLEAN congested );
void le_coc_data_cback( void *context, UINT16 local_cid, UINT8 *p_data, UINT16 len );
void le_coc_tx_complete_cback( void *context, uint16_t local_cid, uint16_t bufcount );
void le_coc_scan_result_cback( wiced_bt_ble_scan_results_t *p_scan_result, uint8_t *p_adv_data );
void start_data_thread();

/******************************************************
 *               Function Definitions
 ******************************************************/

int bt_on( int argc, char* argv[ ] )
{
    int result = 0;
    if (bt_state == WICED_TRUE)
    {
        WPRINT_APP_INFO( ("BT is already ON \n"));
        return result;
    }
#if 0
    {
        /* Configure the Device Name and Class of Device from the DCT */
         platform_dct_bt_config_t* dct_bt_config;
        wiced_dct_read_lock( (void**) &dct_bt_config, WICED_TRUE, DCT_BT_CONFIG_SECTION, 0, sizeof(platform_dct_bt_config_t) );
        WPRINT_APP_INFO( ("WICED DCT BT NAME: %s \r\n", dct_bt_config->bluetooth_device_name) );
        strlcpy((char*)bluetooth_device_name, (char*)dct_bt_config->bluetooth_device_name, sizeof(bluetooth_device_name));
        wiced_bt_cfg_settings.device_name = bluetooth_device_name;
        WPRINT_APP_INFO( ("WICED DCT BT DEVICE CLASS : %02x %02x %02x\r\n", dct_bt_config->bluetooth_device_class[0],
                dct_bt_config->bluetooth_device_class[1],dct_bt_config->bluetooth_device_class[2]) );
        memcpy(wiced_bt_cfg_settings.device_class, dct_bt_config->bluetooth_device_class, sizeof(dct_bt_config->bluetooth_device_class));
        wiced_dct_read_unlock( (void*) dct_bt_config, WICED_TRUE );
    }
#endif
    wiced_bt_stack_init( bt_dualmode_management_cback, &wiced_bt_cfg_settings, wiced_bt_cfg_buf_pools );
    WPRINT_APP_INFO( ("bt_on\n") );
    return result;
}

int bt_off( int argc, char* argv[ ] )
{
    int result = 0;
    WPRINT_APP_INFO( ("bt_off\n") );
    wiced_bt_stack_deinit( );
    bt_state = WICED_FALSE;
    return result;
}

int adv_ble_start( int argc, char* argv[ ] )
{
    int result = 0;
    IS_BT_ON();
    le_coc_set_advertisement_data( );
    result = wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL );
    WPRINT_APP_INFO( ( "wiced_bt_start_advertisements %d\n", result ) );
    return result;
}

int adv_ble_stop( int argc, char* argv[ ] )
{
    int result = 0;
    IS_BT_ON();
    result = wiced_bt_start_advertisements( BTM_BLE_ADVERT_OFF, 0, NULL );
    WPRINT_APP_INFO( ("Stop BLE Advertisements , result = %d\n", result) );
    return result;
}
wiced_bt_gatt_status_t notify_status = WICED_BT_GATT_SUCCESS;

int ble_start_scan( int argc, char* argv[ ] )
{
    int result = 0;
    IS_BT_ON();
    WPRINT_APP_INFO( ("Start BLE Scan\n") );
    result = wiced_bt_ble_scan( BTM_BLE_SCAN_TYPE_HIGH_DUTY, WICED_TRUE, NULL );
    WPRINT_APP_INFO( ("Start Scan result = %d\n", result) );
    return result;
}

int ble_stop_scan( int argc, char* argv[ ] )
{
    int result = 0;
    IS_BT_ON();
    result = wiced_bt_ble_scan( BTM_BLE_SCAN_TYPE_NONE, WICED_TRUE, NULL );
    WPRINT_APP_INFO( ("Stop Scan result = %d\n", result) );
    return result;
}

void ble_app_start( void )
{

}

int start_rfcomm_server( int argc, char* argv[ ] )
{
    IS_BT_ON();
    WPRINT_APP_INFO( ("Start rfcomm_server_init \n") );
    bt_rfcomm_server_init( );
    return 0;
}

int start_rfcomm_client( int argc, char* argv[ ] )
{
    IS_BT_ON();
    WPRINT_APP_INFO( ("Start start_rfcomm_client \n") );
    return 0;
}

int rfcomm_scan_connect ( int argc, char* argv[] )
{
    WPRINT_APP_INFO( ("Start rfcomm_scan_connect \n") );
    return 0;
 }

int get_throughput ( int argc, char* argv[ ] )
{
    float data_rate;
    int elapsed_time;
    WPRINT_APP_INFO( ("get_throughput \n") );
    WPRINT_APP_INFO( ("start Time = %lu ...\n",start_time) );
    WPRINT_APP_INFO( ("END Time = %lu ...\n",end_time) );
    WPRINT_APP_INFO( ("elapsed time in seconds = %f \n", (float) ((end_time - start_time)/1000)) );

    elapsed_time = (end_time - start_time)/1000;
    if( data_le_rx_counter != 0 )
    {
        data_rate = (data_le_rx_counter * 8)/elapsed_time;
        WPRINT_APP_INFO( ("total le bytes recieved = %lu \n", data_le_rx_counter));
        WPRINT_APP_INFO( ("data le rx throughput = %f\n",data_rate));
    }
    if( data_le_tx_counter != 0 )
    {
        data_rate = (data_le_tx_counter * 8)/elapsed_time;
        WPRINT_APP_INFO( ("total le bytes transferred  = %lu \n", data_le_tx_counter));
        WPRINT_APP_INFO( ("data le tx throughput = %f\n",data_rate));
    }
    if( data_acl_tx_counter !=0 )
    {
        data_rate = (data_acl_tx_counter * 8)/elapsed_time;
        WPRINT_APP_INFO( ("total ACL bytes transferred = %lu \n", data_acl_tx_counter));
        WPRINT_APP_INFO( ("acl tx throughput = %f\n",data_rate));
    }
    if( data_acl_rx_counter !=0 )
    {
        data_rate = (data_acl_rx_counter * 8)/elapsed_time;
        WPRINT_APP_INFO( ("total ACL bytes recieved = %lu \n", data_acl_rx_counter));
        WPRINT_APP_INFO( ("acl rx throughput = %f\n",data_rate));
    }
    return 0;
}
void start_data_thread()
{
    wiced_rtos_init_semaphore( &send_data_semaphore );

    wiced_rtos_create_thread_with_stack( &notify_thread,
            WICED_DEFAULT_WORKER_PRIORITY,
            "send_data_thread",
            send_data_thread,
            app_worker_thread_stack,
            APP_WORKER_THREAD_STACK_SIZE,
            NULL
    );
}

wiced_bt_dev_status_t bt_dualmode_management_cback( wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data )
{
    wiced_bt_device_address_t bda;
    wiced_bt_ble_advert_mode_t *p_mode;

    WPRINT_APP_INFO( ( "Bluetooth Management Event: 0x%x\n", event ) );

    wiced_bt_dev_status_t status = WICED_BT_SUCCESS;
    switch ( event )
    {
        case BTM_ENABLED_EVT:
            /* Bluetooth controller and host stack enabled */
            WPRINT_APP_INFO( ( "Bluetooth enabled (%s)\n", ( (p_event_data->enabled.status == WICED_BT_SUCCESS) ? "success":"failure" ) ) );

            if ( p_event_data->enabled.status == WICED_BT_SUCCESS )
            {
                bt_state = WICED_TRUE;
#ifdef DISABLE_BLUETOOTH_LPM
                if ( wiced_bt_dev_get_low_power_mode( ) )
                {
                    wiced_result_t result;
                    result = wiced_bt_dev_set_low_power_mode( WICED_FALSE );
                    WPRINT_APP_INFO( ("bluetooth_management_callback setting low_power_mode: %d\r\n", (int)WICED_FALSE ) );
                    if ( result != WICED_BT_SUCCESS )
                    {
                        WPRINT_APP_INFO( ("bluetooth_management_callback: wiced_bt_dev_set_low_power_mode returns %d\n", (int)result) );
                    }
                }
#endif
                wiced_bt_dev_read_local_addr( bda );
                WPRINT_APP_INFO( ( "Local Bluetooth Address: [%02X:%02X:%02X:%02X:%02X:%02X]\n", bda[0], bda[1], bda[2], bda[3], bda[4], bda[5] ) );
#if 0
#ifdef WICED_DCT_INCLUDE_BT_CONFIG
                {
                    /* Configure the Device Address from the DCT */
                    platform_dct_bt_config_t* dct_bt_config;
                    wiced_dct_read_lock( (void**) &dct_bt_config, WICED_TRUE, DCT_BT_CONFIG_SECTION, 0, sizeof(platform_dct_bt_config_t) );
                    WPRINT_APP_INFO( ("WICED DCT BT ADDR 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x \r\n",
                                    dct_bt_config->bluetooth_device_address[0], dct_bt_config->bluetooth_device_address[1],
                                    dct_bt_config->bluetooth_device_address[2], dct_bt_config->bluetooth_device_address[3],
                                    dct_bt_config->bluetooth_device_address[4], dct_bt_config->bluetooth_device_address[5]) );
                    wiced_bt_set_local_bdaddr ( dct_bt_config->bluetooth_device_address );
                    wiced_dct_read_unlock( (void*) dct_bt_config, WICED_TRUE );
                }
#endif
#endif
            }
            start_data_thread();

            break;

        case BTM_DISABLED_EVT:
            WPRINT_APP_INFO( ( "BT turned off \n" ) );
            break;

        case BTM_PAIRED_DEVICE_LINK_KEYS_REQUEST_EVT:
            /* Request for stored link keys for remote device (if any) */
            /* (sample app does not store link keys to NVRAM) */
            status = WICED_BT_UNKNOWN_ADDR;
            break;

        case BTM_PAIRED_DEVICE_LINK_KEYS_UPDATE_EVT:
            /* Request store newly generated pairing link keys to NVRAM */
            /* (sample app does not store link keys to NVRAM) */
            break;

        case BTM_LOCAL_IDENTITY_KEYS_REQUEST_EVT:
            /* Request to restore local identity keys from NVRAM (requested during Bluetooth start up) */
            /* (sample app does not store keys to NVRAM. New local identity keys will be generated).   */
            status = WICED_BT_NO_RESOURCES;
            break;

        case BTM_LOCAL_IDENTITY_KEYS_UPDATE_EVT:
            /* Request to store newly generated local identity keys to NVRAM */
            /* (sample app does not store keys to NVRAM) */
            break;
#if 0
            case BTM_PAIRING_IO_CAPABILITIES_REQUEST_EVT:
            /* Request for local IO capabilities (sample app does not have i/o capabilities) */
            p_event_data->pairing_io_capabilities_request.local_io_cap = BTM_IO_CAPABILIES_NONE;
            break;
#endif
        case BTM_USER_CONFIRMATION_REQUEST_EVT:
            /* User confirmation request for pairing (sample app always accepts) */
            wiced_bt_dev_confirm_req_reply( WICED_BT_SUCCESS, p_event_data->user_confirmation_request.bd_addr );
            break;

        case BTM_PAIRING_COMPLETE_EVT:
            /* Pairing complete */
            break;

        case BTM_BLE_ADVERT_STATE_CHANGED_EVT:
            /* adv state change callback */
            WPRINT_APP_INFO( ( "---->>> New ADV state: %d\n", p_event_data->ble_advert_state_changed ) );
            p_mode = &p_event_data->ble_advert_state_changed;
            WPRINT_APP_INFO( ( "Advertisement State Change: %d\n", *p_mode) );
            if ( *p_mode == BTM_BLE_ADVERT_OFF )
            {
                //hello_sensor_advertisement_stopped();
                WPRINT_APP_INFO( ( "ADV stop\n") );
            }
            break;

        default:
            WPRINT_APP_INFO( ( "Unhandled Bluetooth Management Event: 0x%x\n", event ) );
            break;
    }

    return ( status );
}

int a2dp_sink_init( int argc, char* argv[ ] )
{
    int result = FALSE;

    WPRINT_APP_INFO( ("a2dp_sink init \n") );

    //result = a2dp_sink_start( );

    return result;

}

/** ########## LE COC Applicaiton ###########
 *
 * LE COC Application (Server/Client)
 *
 * Features demonstrated
 *  - WICED BT LE L2CAP APIs for Connection Oriented Channels
 *
 * Server:
 *  - Start advertisements from client control
 *  - Waits for connection from peer application (on the chosen l2cap psm)
 *  - On connection, waits for data from peer
 *  - Upon receiving data from peer, displays received data in the client control
 *
 * Client:
 *  - Scan from the Client control for the Server
 *  - Connect to server and send data
 *
 */

/******************************************************************************
 *                                Constants
 ******************************************************************************/
#define DEFAULT_LE_COC_MTU 100
#define LE_COC_PSM  19
enum
{
    LE_COC_STATE_INIT, LE_COC_STATE_ERROR_NO_PRIVATE_BUF, LE_COC_STATE_IDLE, LE_COC_STATE_IDLE_CONNECTED, LE_COC_STATE_SENDING, LE_COC_STATE_WAIT_FOR_BUFS
};

/******************************************************
 *                    Structures
 ******************************************************/
/* Application control block */
typedef struct
{
    uint16_t local_cid;
    wiced_bt_device_address_t peer_bda;
    uint8_t congested;
    uint16_t peer_mtu;
} le_coc_cb_t;

wiced_bt_l2cap_le_appl_information_t l2c_appl_info =
{ le_coc_connect_ind_cback, le_coc_connect_cfm_cback, le_coc_disconnect_ind_cback, le_coc_disconnect_cfm_cback, le_coc_data_cback, le_coc_congestion_cback, le_coc_tx_complete_cback };

le_coc_cb_t le_coc_cb;
uint16_t psm;
uint16_t mtu;
uint8_t lec_coc_data[ DEFAULT_LE_COC_MTU ] = { [0 ... (DEFAULT_LE_COC_MTU-1)] = 0x11 };


/* L2CAP Data RX callback */
void le_coc_data_cback( void *context, UINT16 local_cid, UINT8 *p_data, UINT16 len )
{
    //WPRINT_APP_INFO(("[%s] received %d bytes\n", __func__, len));
    if ( le_coc_data_rx_flag == FALSE)
    {
        le_coc_data_rx_flag = TRUE;
        start_time = 0;
        wiced_time_get_time(&start_time);
    }
    data_le_rx_counter += len;
    return;
}

/* L2CAP connection management callback */
void le_coc_connect_ind_cback( void *context, BD_ADDR bda, UINT16 local_cid, UINT16 psm, UINT8 id, UINT16 mtu_peer )
{
    uint8_t *p_data = le_coc_cb.peer_bda;

    WPRINT_APP_INFO( ( "[%s] from  [%02X:%02X:%02X:%02X:%02X:%02X]\n",__func__, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5] ) );
    WPRINT_APP_INFO( ("[%s] CID %d PSM 0x%x MTU %d \n", __func__, local_cid, psm, mtu_peer) );

    /* Accept the connection */
    wiced_bt_l2cap_le_connect_rsp( bda, id, local_cid, L2CAP_CONN_OK, mtu, L2CAP_DEFAULT_BLE_CB_POOL_ID );

    /* Store peer info for reference*/
    le_coc_cb.local_cid = local_cid;
    BDADDR_TO_STREAM( p_data, bda );
    le_coc_cb.peer_mtu = mtu_peer;

    /* Stop advertising */
    wiced_bt_start_advertisements( BTM_BLE_ADVERT_OFF, 0, NULL );

    le_coc_data_rx_flag = FALSE;

}

void le_coc_connect_cfm_cback( void *context, UINT16 local_cid, UINT16 result, UINT16 mtu_peer )
{
    WPRINT_APP_INFO( ("[%s] MTU %d \n", __func__, mtu_peer) );

    if ( result == 0 )
    {
        /* Store peer info for reference*/
        le_coc_cb.local_cid = local_cid;
        le_coc_cb.peer_mtu = mtu_peer;
    }

    le_coc_data_rx_flag = FALSE;

}

void le_coc_disconnect_ind_cback( void *context, UINT16 local_cid, BOOLEAN ack )
{
    WPRINT_APP_INFO( ("[%s] CID %d \n", __func__, local_cid) );

    wiced_time_get_time(&end_time);

    /* Send disconnect response if needed */
    if ( ack )
    {
        wiced_bt_l2cap_le_disconnect_rsp( local_cid );
    }

    if ( le_coc_cb.local_cid == local_cid )
    {
        /* Indicate to client control */
        //le_coc_send_to_client_control(HCI_CONTROL_LE_COC_EVENT_DISCONNECTED, le_coc_cb.peer_bda, BD_ADDR_LEN);
        le_coc_cb.local_cid = 0xFFFF;
        memset( le_coc_cb.peer_bda, 0, BD_ADDR_LEN );
    }
}

void le_coc_disconnect_cfm_cback( void *context, UINT16 local_cid, UINT16 result )
{
    WPRINT_APP_INFO( ("[%s] CID %d \n", __func__, local_cid) );

    if ( le_coc_cb.local_cid == local_cid )
    {
        /* Indicate to client control */
        //le_coc_send_to_client_control(HCI_CONTROL_LE_COC_EVENT_DISCONNECTED, le_coc_cb.peer_bda, BD_ADDR_LEN);
        le_coc_cb.local_cid = 0xFFFF;
        memset( le_coc_cb.peer_bda, 0, BD_ADDR_LEN );
    }
}

void le_coc_congestion_cback( void *context, UINT16 local_cid, BOOLEAN congested )
{
    WPRINT_APP_INFO( ("[%s] CID %d \n", __func__, local_cid) );

    if ( !( congested ) )
    {
        //TODO: send data if pending
    }
}

void le_coc_tx_complete_cback( void *context, uint16_t local_cid, uint16_t bufcount )
{
    //WPRINT_APP_INFO(("[%s] CID %d bufcount %d\n", __func__, local_cid, bufcount));
    if(send_data_type != 0){
        send_data_type = SEND_DATA_LE;
        wiced_rtos_set_semaphore( &send_data_semaphore );
        data_le_tx_counter += mtu;
    }

}

void le_coc_disconnect( void )
{
    WPRINT_APP_INFO( ("[%s]" , __func__) );
    le_coc_data_rx_flag = FALSE;
    if ( le_coc_cb.local_cid )
        wiced_bt_l2cap_le_disconnect_req( le_coc_cb.local_cid );
}
void le_coc_set_advertisement_data( void )
{
    wiced_bt_ble_advert_elem_t adv_data[ 3 ];
    uint8_t adv_flags = BTM_BLE_GENERAL_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED;
    uint8_t num_elem = 0;
    wiced_result_t result;

    adv_data[ num_elem ].advert_type = BTM_BLE_ADVERT_TYPE_FLAG;
    adv_data[ num_elem ].len = 1;
    adv_data[ num_elem ].p_data = &adv_flags;
    num_elem++;

    adv_data[ num_elem ].advert_type = BTM_BLE_ADVERT_TYPE_NAME_SHORT;
    adv_data[ num_elem ].len = strlen( (const char *) wiced_bt_cfg_settings.device_name );
    adv_data[ num_elem ].p_data = wiced_bt_cfg_settings.device_name;
    num_elem++;

    if ( ( result = wiced_bt_ble_set_raw_advertisement_data( num_elem, adv_data ) ) != WICED_BT_SUCCESS )
    {
        WPRINT_APP_INFO( ("[%s] Unble to set ADV data... \n", __func__) );
    }
    else
    {
        WPRINT_APP_INFO( ("[%s] advertisement data is set ... \n", __func__) );
    }
}

int le_coc_init( int argc, char* argv[ ] )
{
    int result = 0;
    IS_BT_ON();
    // set the MTU of the COC to 100
    mtu = DEFAULT_LE_COC_MTU;

    /* Clear app control block */
    memset( &le_coc_cb, 0, sizeof(le_coc_cb_t) );
    le_coc_cb.local_cid = 0xFFFF;

    /* Register LE l2cap callbacks */
    result = wiced_bt_l2cap_le_register( LE_COC_PSM, &l2c_appl_info, NULL );

    WPRINT_APP_INFO( (" LE COC PSM of the local device = %d, and LE COC MTU of the local device = %d  \n", LE_COC_PSM, DEFAULT_LE_COC_MTU) );

    return result;
}

int le_coc_adv( int argc, char* argv[ ] )
{
    int result = 0;
    IS_BT_ON();
    le_coc_set_advertisement_data( );
    wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL );
    return result;
}

int le_coc_scan_connect( int argc, char* argv[ ] )
{
    int result = 0;
    IS_BT_ON();
    result = wiced_bt_ble_scan( BTM_BLE_SCAN_TYPE_HIGH_DUTY, 0, le_coc_scan_result_cback );
    return result;
}

/* Initiate connection */
void le_coc_connect( wiced_bt_device_address_t bd_addr, wiced_bt_ble_address_type_t bd_addr_type )
{
    uint8_t req_security = 0;
    uint8_t req_encr_key_size = 0;
    uint8_t *p_data = le_coc_cb.peer_bda;
    /* Initiate the connection L2CAP connection */
    wiced_bt_l2cap_le_connect_req( LE_COC_PSM, (uint8_t*) bd_addr, bd_addr_type, BLE_CONN_MODE_HIGH_DUTY, mtu,
    L2CAP_DEFAULT_BLE_CB_POOL_ID, req_security, req_encr_key_size );
    BDADDR_TO_STREAM( p_data, bd_addr );
}

int le_coc_send_data( int argc, char* argv[ ] )
{
    uint8_t ret_val = 0;
    start_time = 0;
    data_le_tx_counter = 0;
    IS_BT_ON();
    wiced_time_get_time(&start_time);
    WPRINT_APP_INFO( ("[%s] start time : %lu \n", __func__, start_time) );
    send_data_type = SEND_DATA_LE;
    wiced_rtos_set_semaphore( &send_data_semaphore );
    //ret_val = wiced_bt_l2cap_le_data_write( le_coc_cb.local_cid, lec_coc_data, mtu, 0 );
    //WPRINT_APP_INFO( ("[%s] ret_val : %d data_len : %d\n", __func__, ret_val, mtu) );
    return ret_val;
}

int send_rfcomm_data( int argc, char* argv[ ] )
{
    uint8_t ret_val = 0;
    start_time = 0;
    data_acl_tx_counter = 0;
    IS_BT_ON();
    wiced_time_get_time(&start_time);
    WPRINT_APP_INFO( ("[%s] start time : %lu \n", __func__, start_time) );
    send_data_type = SEND_DATA_RFCOMM;
    wiced_rtos_set_semaphore( &send_data_semaphore );
    return ret_val;
}

int stop_data_send( int argc, char* argv[] )
{
    uint8_t ret_val = 0;
    IS_BT_ON();
    WPRINT_APP_INFO( ("stop_data_send ...\n") );
    send_data_type = STOP_DATA_SEND;
    wiced_rtos_set_semaphore( &send_data_semaphore );
    return ret_val;
}
/*
 * Process advertisement packet received
 */
void le_coc_scan_result_cback( wiced_bt_ble_scan_results_t *p_scan_result, uint8_t *p_adv_data )
{
//    uint32_t i;
//    uint8_t tx_buf[128 + 64], *p = tx_buf, len;
    uint8_t length;
    uint8_t * p_data;
    WPRINT_APP_INFO( ("le_coc_scan_result_cback \n") );
    if ( p_scan_result )
    {
        p_data = wiced_bt_ble_check_advertising_data( p_adv_data, BTM_BLE_ADVERT_TYPE_NAME_SHORT, &length );
        WPRINT_APP_INFO( ("device name = [%s] \n" , p_data) );
        if ( memcmp( p_data, wiced_bt_cfg_settings.device_name, strlen( (const char *) wiced_bt_cfg_settings.device_name ) ) == 0 )
        {
            WPRINT_APP_INFO( ("LE COC device is found , issuing COC Connect") );
            uint8_t req_security = 0;
            uint8_t req_encr_key_size = 0;
            /* Initiate the connection L2CAP connection */
            // already assigned the MTU in the init
            //mtu = DEFAULT_LE_COC_MTU;
            wiced_bt_ble_scan( BTM_BLE_SCAN_TYPE_NONE, 0, NULL );
            wiced_bt_l2cap_le_connect_req( LE_COC_PSM, p_scan_result->remote_bd_addr, p_scan_result->ble_addr_type, BLE_CONN_MODE_HIGH_DUTY, mtu,
            L2CAP_DEFAULT_BLE_CB_POOL_ID, req_security, req_encr_key_size );
            //
        }
        else
        {
            return;
        }
    }
}

static void send_data_thread( uint32_t args )
{
    WPRINT_APP_INFO( ("Started send data Thread...\n") );
    do
    {
        //WPRINT_APP_INFO( ("waiting for semaphore \n") );
        wiced_rtos_get_semaphore( &send_data_semaphore, WICED_NEVER_TIMEOUT );
        //WPRINT_APP_INFO( ("Acquired the semaphore \n") );
        if (send_data_type == SEND_DATA_LE)
        {
            //WPRINT_APP_INFO( ("send_data_type LE...\n") );
            wiced_bt_l2cap_le_data_write( le_coc_cb.local_cid, lec_coc_data, mtu, 0 );

        }
        else if(send_data_type == SEND_DATA_RFCOMM)
        {
            //WPRINT_APP_INFO( ("send_data_type RFCOMM..., %d\n" , len_sent) );
            uint16_t len_sent;
            wiced_bt_rfcomm_write_data( bt_rfcomm_port, (char *)rfcomm_data, BT_RFCOMM_SERVER_APP_MTU, &len_sent );

        }else
        {
            WPRINT_APP_INFO( ("STOP DATA SEND...\n") );
            wiced_time_get_time(&end_time);
            WPRINT_APP_INFO( ("start Time = %lu ...\n",start_time) );
            WPRINT_APP_INFO( ("END Time = %lu ...\n",end_time) );
            WPRINT_APP_INFO( ("elapsed time = %lu \n", (end_time - start_time)) );
            WPRINT_APP_INFO( ("total le bytes transmitted = %lu \n", data_le_tx_counter));
            WPRINT_APP_INFO( ("total acl bytes transmitted = %lu \n", data_acl_tx_counter));


        }
    }while(1);
}

