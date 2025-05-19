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
#include <ctype.h>
#include "command_console_wifi.h"
#include "wwd_wlioctl.h"
#include "wwd_wifi.h"
#include "string.h"
#include "wwd_debug.h"
#include "command_console.h"
#include "wwd_assert.h"
#include "network/wwd_network_interface.h"
#include "stdlib.h"
#include "wwd_management.h"
#include "internal/wwd_sdpcm.h"
#include "internal/wwd_internal.h"
#include "network/wwd_buffer_interface.h"
#include "wiced_management.h"
#include "wiced_crypto.h"
#include "wiced.h"
#include "wiced_security_internal.h"
#include "internal/wiced_internal_api.h"
#include "besl_host.h"
#include "besl_host_interface.h"
#include "certificate.h"
#include "wiced_tls.h"
#include "wiced_utilities.h"
#include "wiced_supplicant.h"
#include "wiced_tls.h"
#include "wwd_events.h"
#include "wiced_log.h"
#include "wifi_utils.h"

#ifdef COMMAND_CONSOLE_WPS_ENABLED
#include "command_console_wps.h"
#include "wiced_wps.h"
#include "wwd_events.h"
#include "wps_common.h"
#endif

#ifdef COMMAND_CONSOLE_P2P_ENABLED
#include "p2p_structures.h"
#include "wiced_p2p.h"
#endif

#include "wwd_wlioctl.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define CHECK_IOCTL_BUFFER( buff )  if ( buff == NULL ) {  wiced_assert("Allocation failed\n", 0 == 1); return ERR_UNKNOWN; }

#define NAME_TO_STR(name) #name

/******************************************************
 *                    Constants
 ******************************************************/
#define CHANNEL_BW           ( 20 )
#define MAX_PASSPHRASE_LEN   ( 64 )
#define MIN_PASSPHRASE_LEN   (  8 )
#define A_SHA_DIGEST_LEN     ( 20 )
#define DOT11_PMK_LEN        ( 32 )

#define DUMP_TX     1
#define DUMP_RX     2
#define DUMP_RATE   4
#define DUMP_AMPDU  8

#define DS1_STATUS_STRING_LENGTH 100

#ifndef DUMP_COMMAND_BUFFER_SIZE
#define DUMP_COMMAND_BUFFER_SIZE 200
#endif

#define MINIOPT_MAXKEY    128    /* Max options */

/* Used with streaming wlog */
#define EVENT_PROCESSOR_THREAD_STACK_SIZE  (4096)
#define EVENT_PROCESSOR_THREAD_QUEUE_SIZE  (10)
#define CONTINUOUS_WLAN_LOG_SIZE           (2000)

/* DHCP offload */
#define UDP_PORT_DHCPv4_SERVER          67           /* UDP port DHCPv4 server */
#define UDP_PORT_DHCPv4_CLIENT          68           /* UDP port DHCPv4 client */
#define DHCPV4_SERVER_HOST_NAME_LEN     64           /* DHCPv4 server host name length */
#define DHCPV4_BOOT_FILE_NAME_LEN       128          /* Boot file name length */
#define DHCPV4_BOOT_REQ_MSG             1            /* Boot request message */
#define DHCPV4_BOOT_HW_TYPE             1            /* HW type: Ethernet */
#define DHCPV4_BOOTP_BROADCAST_FLAG     (1 << 15)    /* Broadcast bootp */
#define DHCPV4_MAGIC_COOKIE             0x63538263   /* Magic cookie for DHCP */
#define DHCPV4_MSG_TYPE                 53           /* TLV tag for message */
#define DHCPV4_REQUEST_MSG              3            /* TLV value for DHCP IPv4 request message */

#define IPv4_TTL                       128           /* TTL value */
#define IPV4_ADDR_LEN                   4
#define IPV4_MIN_HEADER_LEN             20           /* Minimum size for an IP header (no options) */
#define IPV4_VER_SHIFT                  4            /* IPV4 version shift */

#define WLC_IOCTL_DHCP_PKT_LEN         512           /* DHCP offload IOCTL packet length */

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static wiced_result_t scan_result_handler( wiced_scan_handler_result_t* malloced_scan_result );
static void           ac_params_print( const wiced_edcf_ac_param_t *acp, const int *priority );
#ifndef WICED_CONFIG_DISABLE_ENTERPRISE_SECURITY
static eap_type_t     str_to_enterprise_security_type ( char* arg );
static supplicant_tunnel_auth_type_t str_to_tunnel_authentication_type( char* arg );
#endif /* WICED_CONFIG_DISABLE_ENTERPRISE_SECURITY */
static int            parse_country_spec( char *spec, int8_t *ccode, int32_t *regrev );
static int            test_join_process( int argc, char* argv[], int specific );
static void           wifi_ds1_callback( void* );
static void ds1_free_offload_container( wiced_offloads_container_t *offload );
static wiced_result_t print_log( void *arg );

static int print_nan_config_status ( wwd_nan_state_t* nan_cfg_status );
static int print_nan_mac_address ( wwd_nan_cluster_id_t* ether_addr );
static wwd_result_t wl_nan_iovar(wiced_bool_t set, uint16_t cmd_id, void *buffer, wiced_bool_t start );
static int print_nan_device_state( uint8_t nan_device_state );

static void app_nan_event_handler(const void *event_header, const uint8_t *event_data );
static void print_nan_event( const wwd_event_header_t *event_header );

static wiced_result_t set_roam_delta_per_band( int32_t delta, wiced_802_11_band_t band );
static wiced_result_t get_roam_delta_per_band( int32_t *delta, wiced_802_11_band_t band );

static int dltro_connect_init(wwd_dltro_connect_t* connect, uint8_t index, uint8_t hops,
            uint8_t ip_addr_type, uint8_t offload_type, wiced_mac_t* saddr,
            wiced_mac_t *daddr, void *sipaddr, void *dipaddr,
            uint32_t tid, uint32_t lease_time_remaining, uint32_t renew_seconds_before_expiry );

static int dltro_pkt_v4(uint8_t* buf, wiced_mac_t* daddr, wiced_mac_t* saddr,
        wwd_ipv4_addr_t* dipaddr, wwd_ipv4_addr_t* sipaddr, uint8_t hops, uint32_t tid );

static uint16_t ipv4_hdr_cksum( uint8_t *ip, int ip_len);
static uint32_t ip_cksum_partial( uint32_t sum, uint8_t *val8, uint32_t count);
static uint16_t ip_cksum(uint32_t sum, uint8_t *val8, uint32_t count );

    /******************************************************
     *               Variable Definitions
     ******************************************************/
    /******************************************************
     *                      Macros
     ******************************************************/

    /******************************************************
     *                    Constants
     ******************************************************/

#define MAX_CREDENTIAL_COUNT   5
#define JOIN_ATTEMPT_TIMEOUT   60000

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

char last_started_ssid[SSID_NAME_SIZE+1] = "";       /* 32 characters + terminating null */
static char last_soft_ap_passphrase[MAX_PASSPHRASE_LEN+1] = "";
static int record_count;
static wiced_semaphore_t scan_semaphore;
static wiced_ip_setting_t ap_ip_settings =
{
    INITIALISER_IPV4_ADDRESS( .ip_address, MAKE_IPV4_ADDRESS( 192,168,  0,  1 ) ),
    INITIALISER_IPV4_ADDRESS( .netmask,    MAKE_IPV4_ADDRESS( 255,255,255,  0 ) ),
    INITIALISER_IPV4_ADDRESS( .gateway,    MAKE_IPV4_ADDRESS( 192,168,  0,  1 ) ),
};

/* Used with streaming wlog */
static wiced_worker_thread_t  log_processor;
static wiced_timed_event_t    wlog_app_timer;
static char                   *continous_wlan_buf   = NULL;
static int cnt_verbose = 0;

#ifndef WICED_CONFIG_DISABLE_ENTERPRISE_SECURITY
static wiced_tls_session_t tls_session = { 0 };
static supplicant_workspace_t supplicant_workspace;
static uint8_t eap_identity[] = "wiced";
static wiced_tls_context_t context;
static wiced_tls_identity_t identity;
#endif /* WICED_CONFIG_DISABLE_ENTERPRISE_SECURITY */

/* RM Enable Capabilities */
static radio_resource_management_capability_debug_msg_t rrm_msg[] =
{
    { DOT11_RRM_CAP_LINK,    "Link_Measurement" },                /* bit0 */
    { DOT11_RRM_CAP_NEIGHBOR_REPORT, "Neighbor_Report" },         /* bit1 */
    { DOT11_RRM_CAP_PARALLEL,    "Parallel_Measurement" },        /* bit2 */
    { DOT11_RRM_CAP_REPEATED,    "Repeated_Measurement" },        /* bit3 */
    { DOT11_RRM_CAP_BCN_PASSIVE, "Beacon_Passive" },          /* bit4 */
    { DOT11_RRM_CAP_BCN_ACTIVE,  "Beacon_Active" },           /* bit5 */
    { DOT11_RRM_CAP_BCN_TABLE,   "Beacon_Table" },            /* bit6 */
    { DOT11_RRM_CAP_BCN_REP_COND,    "Beacon_measurement_Reporting_Condition" }, /* bit7 */
    { DOT11_RRM_CAP_FM,  "Frame_Measurement" },               /* bit8 */
    { DOT11_RRM_CAP_CLM, "Channel_load_Measurement" },            /* bit9 */
    { DOT11_RRM_CAP_NHM, "Noise_Histogram_measurement" },         /* bit10 */
    { DOT11_RRM_CAP_SM,  "Statistics_Measurement" },          /* bit11 */
    { DOT11_RRM_CAP_LCIM,    "LCI_Measurement" },             /* bit12 */
    { DOT11_RRM_CAP_LCIA,    "LCI_Azimuth" },                 /* bit13 */
    { DOT11_RRM_CAP_TSCM,    "Tx_Stream_Category_Measurement" },      /* bit14 */
    { DOT11_RRM_CAP_TTSCM,   "Triggered_Tx_stream_Category_Measurement" },    /* bit15 */
    { DOT11_RRM_CAP_AP_CHANREP,  "AP_Channel_Report" },           /* bit16 */
    { DOT11_RRM_CAP_RMMIB,   "RM_MIB" },                  /* bit17 */
    { 18, "unused"},
    { 19, "unused"},
    { 20, "unused"},
    { 21, "unused"},
    { 22, "unused"},
    { 23, "unused"},
    { 24, "unused"},
    { 25, "unused"},
    { 26, "unused"},
    /* bit 18-26, unused */
    { DOT11_RRM_CAP_MPTI,    "Measurement_Pilot_Transmission_Information" },  /* bit27 */
    { DOT11_RRM_CAP_NBRTSFO, "Neighbor_Report_TSF_Offset" },          /* bit28 */
    { DOT11_RRM_CAP_RCPI,    "RCPI_Measurement" },                /* bit29 */
    { DOT11_RRM_CAP_RSNI,    "RSNI_Measurement" },                /* bit30 */
    { DOT11_RRM_CAP_BSSAAD,  "BSS_Average_Access_Delay" },            /* bit31 */
    { DOT11_RRM_CAP_BSSAAC,  "BSS_Available_Admission_Capacity" },        /* bit32 */
    { DOT11_RRM_CAP_AI,  "Antenna_Information" },             /* bit33 */

};


/* WNM debug message struct  */
typedef struct wnm_debug_msg
{
    uint32_t value;
    const char* string;
} wnm_debug_msg_t;


typedef struct
{
    uint8_t  msg_type;                                      /* Message type */
    uint8_t  hw_type;                                       /* Hardware type */
    uint8_t  hwaddr_len;                                    /* Hardware address length */
    uint8_t  hops;                                          /* Hop counter */
    uint32_t tid;                                           /* Transaction ID */
    uint16_t sec_elapsed;                                   /* Seconds elapsed */
    uint16_t bootp_flags;                                   /* Bootp flags */
    uint8_t  ip_addr[IPV4_ADDR_LEN];                        /* Sender IP addr */
    uint8_t  client_ip_addr[IPV4_ADDR_LEN];                 /* Client IP addr */
    uint8_t  next_server_ip_addr[IPV4_ADDR_LEN];            /* Next server IP addr */
    uint8_t  relay_agent_ip_addr[IPV4_ADDR_LEN];            /* Relay agent IP addr */
    uint8_t  client_mac_addr[ETHER_ADDR_LEN];               /* Client mac address */
    uint8_t  client_mac_addr_padding[10];                   /* Mac address padding */
    uint8_t  server_host_name[DHCPV4_SERVER_HOST_NAME_LEN]; /* Server host name */
    uint8_t  boot_file_name[DHCPV4_BOOT_FILE_NAME_LEN];     /* Boot file name */
    uint32_t magic_cookie;                                  /* DHCP magic cookie */
    uint8_t  tlv_field[60];                                 /* TLV fields */
} dhcpv4_pkt_t;

static wnm_debug_msg_t wnm_capabilities_msg [] =
{
        { WL_WNM_BSSTRANS,  "WNM BSS TRANSITION" },
        { WL_WNM_PROXYARP,  "WNM PROXY ARP"      },
        { WL_WNM_MAXIDLE,   "WNM MAX IDLE"       },
        { WL_WNM_TIMBC,     "WNM TIM BROADCAST"  },
        { WL_WNM_TFS,       "WNM TFS"            },
        { WL_WNM_SLEEP,     "WNM SLEEP"          },
        { WL_WNM_DMS,       "WNM DMS"            },
        { WL_WNM_FMS,       "WNM FMS"            },
        { WL_WNM_NOTIF,     "WNM NOTIF"          },
        { WL_WNM_WBTEXT,    "WNM WBTEXT"         },
        { WL_WNM_MAX,       "WNM MAX"            },
};

static wnm_debug_msg_t wnm_bss_trans_resp_msg [] =
{
        { WL_BSSTRANS_POLICY_ROAM_ALWAYS,  "WNM BSS TRANSITION POLICY ROAM ALWAYS"                            },
        { WL_BSSTRANS_POLICY_ROAM_IF_MODE, "WNM BSS TRANSITION POLICY ROAM IF REQUESTED MODE FIELD is SET"    },
        { WL_BSSTRANS_POLICY_ROAM_IF_PREF, "WNM BSS TRANSITION POLICY ROAM IF PREFERRED BSS PROVIDED"         },
        { WL_BSSTRANS_POLICY_WAIT,         "WNM BSS TRANSITION POLICY WAIT FOR DEAUTH and SEND ACCEPT STATUS" },
        { WL_BSSTRANS_POLICY_PRODUCT,      "WMM BSS TRANSITION POLICY FOR REAL PRODUCT USE CASES"             },
};

typedef struct miniopt {

    /* These are persistent after miniopt_init() */
    const char* name;        /* name for prompt in error strings */
    const char* flags;        /* option chars that take no args */
    uint8_t longflags;        /* long options may be flags */
    uint8_t opt_end;        /* at end of options (passed a "--") */

    /* These are per-call to miniopt() */

    int consumed;        /* number of argv entries cosumed in
                 * the most recent call to miniopt()
                 */
    uint8_t positional;
    uint8_t good_int;        /* 'val' member is the result of a sucessful
                 * strtol conversion of the option value
                 */
    char opt;
    char key[MINIOPT_MAXKEY];
    char* valstr;        /* positional param, or value for the option,
                 * or null if the option had
                 * no accompanying value
                 */
    uint uval;        /* strtol translation of valstr */
    int  val;        /* strtol translation of valstr */
} miniopt_t;

static unsigned int n_phyrates_kb[] = {7200, 14400, 21700, 28900, 43300, 57800, 65000, 72200};
static wiced_offloads_container_t *offloads_container = NULL;

/******************************************************
 *               Function Definitions
 ******************************************************/

/*!
 ******************************************************************************
 * Processes a pair of join auth_type strings.
 *
 * @return  0 for success, otherwise error
 */
int process_join_security( int argc, char* ssid, wiced_security_t auth_type, char* key_string, uint8_t *key_length, uint8_t *wep_key_buffer, uint8_t** security_key)
{

    if ( auth_type == WICED_SECURITY_UNKNOWN )
    {
        WPRINT_APP_INFO(( "Error: Invalid security type\n" ));
        return ERR_UNKNOWN;
    }


    if ( auth_type == WICED_SECURITY_WEP_PSK )
    {
#ifdef ENABLE_WEP
        uint8_t temp_key_length = *key_length;
        int a;
        wiced_wep_key_t* temp_wep_key = (wiced_wep_key_t*)wep_key_buffer;
        char temp_string[3];
        temp_string[2] = 0;
        temp_key_length = (uint8_t) strlen(key_string)/2;

        /* Setup WEP key 0 */
        temp_wep_key[0].index = 0;
        temp_wep_key[0].length = temp_key_length;
        for (a = 0; a < temp_wep_key[0].length; ++a)
        {
            uint32_t tmp_val;
            memcpy(temp_string, &key_string[a*2], 2);
            string_to_unsigned( temp_string, 2, &tmp_val, 1 );
            temp_wep_key[0].data[a] = (uint8_t) tmp_val;
        }

        /* Setup WEP keys 1 to 3 */
        memcpy(wep_key_buffer + 1*(2 + temp_key_length), temp_wep_key, (2 + temp_key_length));
        memcpy(wep_key_buffer + 2*(2 + temp_key_length), temp_wep_key, (2 + temp_key_length));
        memcpy(wep_key_buffer + 3*(2 + temp_key_length), temp_wep_key, (2 + temp_key_length));
        wep_key_buffer[1*(2 + temp_key_length)] = 1;
        wep_key_buffer[2*(2 + temp_key_length)] = 2;
        wep_key_buffer[3*(2 + temp_key_length)] = 3;

        temp_key_length = 4*(2 + temp_key_length);
        *key_length = temp_key_length;

        *security_key = wep_key_buffer;
#else
        WPRINT_MACRO(("WEP not supported; Define the flag ENABLE_WEP in the build to enable it\n"));
        return ERR_BAD_ARG;
#endif /* ENABLE_WEP */
    }
    else if ( ( auth_type != WICED_SECURITY_OPEN ) && ( argc < 4 ) )
    {
        *key_length = 0;
        WPRINT_APP_INFO(("Error: Missing security key\n" ));
        return ERR_UNKNOWN;
    }
    else
    {
        *key_length = strlen(key_string);
    }

    return ERR_CMD_OK;
}

/*!
 ******************************************************************************
 * Joins an access point specified by the provided arguments
 *
 * @return  0 for success, otherwise error
 */

int join( int argc, char* argv[] )
{
    int              result;
    char*            ssid = argv[1];
    wiced_security_t auth_type = wifi_utils_str_to_authtype(argv[2]);
    uint8_t*         security_key;
    uint8_t*         security_key_psk;
    uint8_t          key_length;
    uint8_t          psk_length;
    uint8_t          wep_key_buffer[64] = { 0 };

    if (argc > 9)
    {
        return ERR_TOO_MANY_ARGS;
    }

    if( ( ( strcmp( argv[2], "open") == 0 ) && !( argc == 3 || argc == 4 || argc == 7 ) ) ||
        ( ( strcmp( argv[2], "wpa3_wpa2") == 0 ) && !( argc == 5 || argc == 6 || argc == 9 ) ) ||
        ( ( ( strcmp( argv[2], "wpa3_wpa2") != 0 ) && ( strcmp( argv[2], "open") != 0 ) ) && !( argc == 4 || argc == 5 || argc == 8 ) ) )
    {
        return ERR_INSUFFICENT_ARGS;
    }

    /* argv[4] : PSK value, when authentication mode is set to wpa3_wpa2 */
    if( auth_type == WICED_SECURITY_WPA3_WPA2_PSK )
    {
        security_key_psk = (uint8_t*)argv[4];
        psk_length = strlen( argv[4] );

        result = wwd_wifi_enable_sup_set_passphrase( security_key_psk, psk_length, auth_type );
        if( result!= WWD_SUCCESS )
        {
            return result;
        }
    }

    security_key = (uint8_t*)argv[3];
    result = process_join_security(argc, ssid, auth_type, argv[3], &key_length, wep_key_buffer, &security_key);
    if (result == ERR_CMD_OK)
    {
        if ( argc == 7 )
        {
            result = wifi_join( ssid, strlen(ssid), auth_type, security_key, key_length, argv[4], argv[5], argv[6]);
        }
        else if ( argc == 8 )
        {
            result = wifi_join( ssid, strlen(ssid), auth_type, security_key, key_length, argv[5], argv[6], argv[7]);
        }
        else if ( argc == 9 )
        {
            result = wifi_join( ssid, strlen(ssid), auth_type, security_key, key_length, argv[6], argv[7], argv[8]);
        }
        else
        {
            result = wifi_join( ssid, strlen(ssid), auth_type, security_key, key_length, NULL, NULL, NULL );
        }
    }

    return result;
}
#ifndef WICED_CONFIG_DISABLE_ENTERPRISE_SECURITY
/*!
 ******************************************************************************
 * Joins an access point using enterprise security
 *
 * @return  0 for success, otherwise error
 */
int join_ent( int argc, char* argv[] )
{
    char* ssid = argv[1];
    wiced_security_t auth_type;
    eap_type_t       eap_type;
    eap_type_t       inner_eap_type = EAP_TYPE_NONE;
    besl_result_t   res;
    supplicant_connection_info_t conn_info;
    supplicant_tunnel_auth_type_t tunnel_auth_type;
    /* When using PEAP note:
     * There are two identities that are being passed, outer identity (un encrypted) and inner identity (encrypted).
     * Different servers have different requirements for the outer identity.
     * For windows IAS its something like "wiced@wiced.local: ( where wiced.local in your server's domain )
     * FreeRadius doesn't care much about outer name
     * Cisco must have the outer identity in the user list e.g. "wiced" ( i.e. outer and inner identities must match ).
     *
     * When using EAP-TLS note:
     * If the RADIUS server is configured to check the user name against the certificate common name then eap_identity needs to be the same as the certificate common name.
     * For example:
     * char eap_identity[] = "wifi-user@wifilabs.local";
     */

    if ( argc < 4 )
    {
        return ERR_INSUFFICENT_ARGS;
    }

    eap_type = str_to_enterprise_security_type(argv[2]);
    if ( eap_type == EAP_TYPE_NONE )
    {
        WPRINT_APP_INFO(("Unknown security type\n" ));
        return ERR_CMD_OK;
    }

    auth_type = wifi_utils_str_to_enterprise_authtype(argv[argc-1]);
    if ( auth_type == WICED_SECURITY_UNKNOWN )
    {
        WPRINT_APP_INFO(("Unknown security type\n" ));
        return ERR_CMD_OK;
    }
    if ( ( eap_type == EAP_TYPE_PEAP ) && ( argc < 6 ) )
    {
        return ERR_INSUFFICENT_ARGS;
    }

    if ( ( eap_type == EAP_TYPE_TTLS ) && ( argc < 7 ) )
    {
        return ERR_INSUFFICENT_ARGS;
    }

    if(eap_type == EAP_TYPE_TTLS)
    {
        tunnel_auth_type = str_to_tunnel_authentication_type(argv[5]);
        if(tunnel_auth_type == TUNNEL_TYPE_EAP)
        {
            inner_eap_type = str_to_enterprise_security_type(argv[6]);
            if ( inner_eap_type == EAP_TYPE_NONE )
            {
                WPRINT_APP_INFO(("Unknown inner eap type\n" ));
                return ERR_CMD_OK;
            }
        }
        else
        {
            WPRINT_APP_INFO(("Unsupported Tunnel Authentication Type\n" ));
            return ERR_CMD_OK;
        }
    }


    memset(&conn_info, 0x0, sizeof(conn_info));
    conn_info.interface = WWD_STA_INTERFACE;
    conn_info.tls_identity = &identity;
    conn_info.tls_session = &tls_session;
    conn_info.context = &context;
    conn_info.trusted_ca_certificates = (uint8_t*) WIFI_ROOT_CERTIFICATE_STRING;
    conn_info.root_ca_cert_length = (uint32_t)strlen( (char*)WIFI_ROOT_CERTIFICATE_STRING );
    conn_info.eap_type = eap_type;
    conn_info.auth_type = auth_type;
    conn_info.eap_identity = eap_identity;

    if(eap_type == EAP_TYPE_TTLS)
    {
        conn_info.tunnel_auth_type = tunnel_auth_type;
        conn_info.inner_eap_type = inner_eap_type;
        if(argc > 7)
        {
            char* arg;
            if(tunnel_auth_type == TUNNEL_TYPE_EAP)
            {
                arg = argv[7];
            }
            /* This code will be used when support for tunnel types other than TUNNEL_TYPE_EAP is added.
             * Until then this will be dead code, so commenting out to fix the coverity issue : CID 63300
             */
            /*
             else
             {
             arg = argv[6];
             }
             */
            if ( strcmp( arg, "client-cert" ) == 0 )
            {
                conn_info.is_client_cert_required = 1;
            }
            else
            {
                conn_info.is_client_cert_required = 0;
            }
        }
        else
        {
            conn_info.is_client_cert_required = 0;
        }
    }
    if (eap_type == EAP_TYPE_PEAP || eap_type == EAP_TYPE_TTLS)
    {
        conn_info.user_name = (uint8_t*) argv[ 3 ];
        conn_info.password = (uint8_t*) argv[ 4 ];
    }
    if ( eap_type == EAP_TYPE_TLS || ( eap_type == EAP_TYPE_TTLS && conn_info.is_client_cert_required))
    {
        conn_info.user_cert = (uint8_t*) WIFI_USER_CERTIFICATE_STRING;
        conn_info.user_cert_length = (uint32_t) strlen( (char*) WIFI_USER_CERTIFICATE_STRING );
        conn_info.private_key = (uint8_t*) WIFI_USER_PRIVATE_KEY_STRING;
        conn_info.key_length = (uint32_t) strlen( (char*) WIFI_USER_PRIVATE_KEY_STRING );
    }

    res = besl_supplicant_init( &supplicant_workspace, &conn_info);
    if ( res == BESL_SUCCESS )
    {
        if ( besl_supplicant_start( &supplicant_workspace ) == BESL_SUCCESS )
        {
            if ( wifi_join( ssid, strlen( ssid ), auth_type, NULL, 0, NULL, NULL, NULL ) == ERR_CMD_OK )
            {
                int ret;
                if ( ( ret = mbedtls_ssl_get_session( &supplicant_workspace.tls_context->context, &tls_session) ) != 0 )
                {
                    WPRINT_APP_INFO( ( " Failed to retrieve the session information %d \r\n", ret ) );
                }
                else
                {
                    WPRINT_APP_INFO( ( " Successfully retrieved the session information %d \r\n", ret ) );
                }
            }
            else
            {
                res = besl_supplicant_stop( &supplicant_workspace );
                if ( res != BESL_SUCCESS )
                {
                    WPRINT_APP_DEBUG( ("supplicant Stop failed with error = [%d]\n", res) );
                }

                WPRINT_APP_DEBUG( ("De-init supplicant\n" ) );
                res = besl_supplicant_deinit( &supplicant_workspace );
            }
        }
    }
    else
    {
        WPRINT_APP_DEBUG( ("Unable to initialize supplicant. Error = [%d]\n", res ) );
    }

    WPRINT_APP_DEBUG( ("After join_ent\n" ) );


    return ERR_CMD_OK;
}
#endif /* WICED_CONFIG_DISABLE_ENTERPRISE_SECURITY */
int read_wlan_ioctls( int argc, char* argv[ ] )
{
    wwd_result_t result = WICED_SUCCESS;
    result = wwd_ioctl_print( );
    if ( result == WWD_DOES_NOT_EXIST )
    {
        WPRINT_APP_INFO( ( "wwd ioctl log (WWD_IOCTL_LOG_ENABLE) not enabled. \n") );
    }

    return ERR_CMD_OK;
}

/*!
 ******************************************************************************
 * Leaves enterprise security access point
 *
 * @return  0 for success, otherwise error
 */
#ifndef WICED_CONFIG_DISABLE_ENTERPRISE_SECURITY
int leave_ent( int argc, char* argv[] )
{
    wiced_result_t result;
    besl_result_t res;

    WPRINT_APP_DEBUG( ("Stop supplicant\n" ) );
    res = besl_supplicant_stop( &supplicant_workspace );
    if (res != BESL_SUCCESS)
    {
        WPRINT_APP_DEBUG( ("supplicant Stop failed with error = [%d]\n", res) );
    }

    WPRINT_APP_DEBUG( ("De-init supplicant\n" ) );
    res = besl_supplicant_deinit( &supplicant_workspace );
    if (res != BESL_SUCCESS)
    {
        WPRINT_APP_INFO( ("supplicant De-init failed with error = [%d]\n", res) );
    }

    WPRINT_APP_DEBUG( ("Bring down network\n" ) );
    result = wiced_network_down( WICED_STA_INTERFACE );
    if (WICED_SUCCESS != result)
    {
        WPRINT_APP_INFO(("Couldn't leave AP. Error = [%d]\n", result));
    }
    else
    {
        WPRINT_APP_INFO(("leave test: ok\n"));
    }

    WPRINT_APP_DEBUG( ("After leave_ent\n" ) );
    return ERR_CMD_OK;
}
#endif
/*!
 ******************************************************************************
 * Joins an adhoc network using the provided arguments
 *
 * @return  0 for success, otherwise error
 */

int join_adhoc( int argc, char* argv[] )
{
    int              result;
    char*            ssid = argv[1];
    wiced_security_t auth_type = wifi_utils_str_to_authtype(argv[2]);
    uint8_t          key_length = strlen(argv[3]);
    uint8_t          wep_key_buffer[64];
    uint8_t*         security_key = (uint8_t*)argv[3];
    wiced_result_t   Wiced_result;

    if (argc < 8)
    {
        return ERR_INSUFFICENT_ARGS;
    }

    if (argc > 8)
    {
        return ERR_TOO_MANY_ARGS;
    }

    result = process_join_security(argc, ssid, auth_type, argv[3], &key_length, wep_key_buffer, &security_key);
    if (result == ERR_CMD_OK)
    {
        Wiced_result = wiced_wifi_join_adhoc( ssid, strlen(ssid), auth_type, security_key,
                key_length, argv[4], argv[5], argv[6], argv[7]);
        if(Wiced_result != WICED_SUCCESS)
        {
            wifi_utils_analyse_failed_join_result( result );
            return ERR_UNKNOWN;
        }
        return result;
    }
    else
        return result;
}

/*!
 ******************************************************************************
 * Joins a specific access point using the provided arguments
 *
 * @return  0 for success, otherwise error
 */

int join_specific( int argc, char* argv[] )
{
    int              result;
    char*            ssid = argv[1];
    wiced_security_t auth_type = wifi_utils_str_to_authtype(argv[4]);
    uint8_t          key_length;
    uint8_t          wep_key_buffer[64];
    uint8_t*         security_key;
    wiced_result_t   Wiced_result;

    if (argc > 9)
    {
        return ERR_TOO_MANY_ARGS;
    }

    if (argc > 6 && argc != 9)
    {
        return ERR_INSUFFICENT_ARGS;
    }

    if ( auth_type == WICED_SECURITY_UNKNOWN )
    {
        WPRINT_APP_INFO(( "Error: Invalid security type\n" ));
        return ERR_UNKNOWN;
    }

    security_key = (uint8_t*)argv[5];
    result = process_join_security(argc, ssid, auth_type, argv[5], &key_length, wep_key_buffer, &security_key);

    if ( ERR_CMD_OK == result )
    {
        if ( 9 == argc )
            Wiced_result =  wiced_wifi_join_specific( ssid, strlen(ssid), auth_type, security_key, key_length, argv[2], argv[3], argv[6], argv[7], argv[8]);
        else
            Wiced_result =  wiced_wifi_join_specific( ssid, strlen(ssid), auth_type, security_key, key_length, argv[2], argv[3], NULL, NULL, NULL);
        if( Wiced_result != WICED_SUCCESS)
        {
            wifi_utils_analyse_failed_join_result( result );
            return ERR_UNKNOWN;
        }
    }

    return result;
}

int wifi_join(char* ssid, uint8_t ssid_length, wiced_security_t auth_type, uint8_t* key, uint16_t key_length, char* ip, char* netmask, char* gateway )
{
    /* ensure the operation is ok, then jump into utils */
    if ( wwd_wifi_is_ready_to_transceive( WWD_AP_INTERFACE ) == WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("AP will be moved to the STA assoc channel"));

        /* If not ok, then break processing */
        if ( 0 == console_prompt_confirm( ) )
        {
            return ERR_CMD_OK;
        }
    }

    return wifi_utils_join( ssid, ssid_length, auth_type, key, key_length, ip, netmask, gateway );
}

/**
 *  Scan result callback
 *  Called whenever a scan result is available
 *
 *  @param result_ptr : pointer to pointer for location where result is stored. The inner pointer
 *                      can be updated to cause the next result to be put in a new location.
 *  @param user_data : unused
 */
static wiced_result_t scan_result_handler( wiced_scan_handler_result_t* malloced_scan_result )
{
    if ( malloced_scan_result != NULL )
    {
        malloc_transfer_to_curr_thread( malloced_scan_result );

        if ( malloced_scan_result->status == WICED_SCAN_INCOMPLETE )
        {
            wiced_scan_result_t* record = &malloced_scan_result->ap_details;

            WPRINT_APP_INFO( ( "%3d ", record_count ) );
            print_scan_result( record );

            ++record_count;
        }
        else
        {
            wiced_rtos_set_semaphore(&scan_semaphore);
        }

        free( malloced_scan_result );
        malloced_scan_result = NULL;
    }

    return WICED_SUCCESS;
}

static int
parse_country_spec( char *spec, int8_t *ccode, int32_t *regrev )
{
    char *revstr;
    char *endptr = NULL;
    int ccode_len;
    int rev = -1;

    revstr = strchr( spec, '/' );
    if ( revstr )
    {
        rev = strtol( revstr + 1, &endptr, 10 );
        if ( *endptr != '\0' )
        {
            return ERR_UNKNOWN_CMD;
        }
    }
    if ( revstr )
    {
        ccode_len = (int)(strlen( spec ) - strlen( revstr ));
    }
    else
    {
        ccode_len = (int)strlen( spec );
    }
    if ( ccode_len > 3 )
    {
        return ERR_UNKNOWN_CMD;
    }
    memcpy( ccode, spec, ccode_len );
    ccode[ccode_len] = '\0';
    *regrev = rev;

    return ERR_CMD_OK;
}

int
scan_disable( int argc, char* argv[] )
{
    int err = ERR_CMD_OK;
    wiced_result_t result;
    if ( argv[1][0] != '0' )
    {
        result = wiced_wifi_scan_disable( );
    }
    else
    {
        result = wiced_wifi_scan_enable( );
    }

    if ( WICED_SUCCESS != result )
    {
        WPRINT_APP_ERROR( ("Failed scan %sable; result = %d\n", ( argv[1][0] != '0' ) ? "dis" : "en", result ) );
    }

    return err;
}

/*!
 ******************************************************************************
 * Scans for access points and prints out results
 *
 * @return  0 for success, otherwise error
 */

int scan( int argc, char* argv[] )
{
    wiced_result_t result;
    record_count = 0;

    wiced_scan_type_t scan_type = WICED_SCAN_TYPE_ACTIVE;
    wiced_bss_type_t  bss_type = WICED_BSS_TYPE_ANY;
    wiced_ssid_t* optional_ssid = NULL;
    wiced_ssid_t  ssid;
    wiced_mac_t* optional_mac = NULL;
    wiced_mac_t   mac;
    uint16_t *channel_list = NULL;
    uint16_t chan_list[WICED_MAX_CHANNEL_NUM + 1]; // +1 for ending 0
    wiced_scan_extended_params_t* optional_extended_params = NULL;
    wiced_scan_extended_params_t extended_params;
    wiced_interface_t interface = WICED_STA_INTERFACE;
#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
    interface = WICED_AP_INTERFACE;
#endif

    if (argc > 7)
    {
        return ERR_TOO_MANY_ARGS;
    }

    for ( int i = 1; i < argc; i++ )
    {
         if ( strcmp(argv[i], "-") == 0 )
             continue;

         switch (i)
         {
             case 1:
               scan_type = atoi(argv[i]);
               break;
             case 2:
               bss_type = atoi(argv[i]);
               break;
             case 3:
               if ( strlen(argv[i]) >= SSID_NAME_SIZE )
               {
                   WPRINT_APP_INFO( ( "SSID too long!\n" ) );
                   return ERR_UNKNOWN;
               }
               ssid.length = strlen(argv[i]);
               memset( ssid.value, 0, sizeof(ssid.value) );
               memcpy( ssid.value, argv[i], ssid.length );
               optional_ssid = &ssid;
               break;
             case 4:
               wifi_utils_str_to_mac(argv[i], &mac);
               optional_mac = &mac;
               break;
             case 5:
               if ( wifi_utils_str_to_channel_list(argv[i], chan_list ) < 2 )
               {
                  WPRINT_APP_INFO( ( "WARNING: Empty channel_list!\n" ) );
               }
               else
               {
                  channel_list = chan_list;
               }
               break;
             case 6:
               if ( wifi_utils_str_to_extended_params( argv[i], &extended_params ) == WICED_TRUE )
               {
                 optional_extended_params = &extended_params;
               }
               else
               {
                 WPRINT_APP_INFO( ( "Wrong optional_extended_params format!\n" ) );
                 return ERR_UNKNOWN;
               }
               break;
             default:
               return ERR_UNKNOWN;
        }
    }

    WPRINT_APP_INFO( ( "Waiting for scan results...\n" ) );
    WPRINT_APP_INFO( ("  # Type  BSSID              RSSI Rate Chan Security               SSID                            CCode    Flag\n" ) );
    WPRINT_APP_INFO( ("------------------------------------------------------------------------------------------------------------------\n" ) );

    /* Initialise the semaphore that will tell us when the scan is complete */
    wiced_rtos_init_semaphore(&scan_semaphore);

    if ( ( result = wiced_wifi_scan_networks_ex(scan_result_handler, NULL, scan_type, bss_type,
                         optional_ssid, optional_mac, channel_list, optional_extended_params, interface) ) == WICED_SUCCESS )
    {
        /* Wait until scan is complete */
        wiced_rtos_get_semaphore(&scan_semaphore, WICED_WAIT_FOREVER);
    }
    else
    {
        WPRINT_APP_INFO( ( "Error starting scan! Error=%d\n", result ) );
    }
    wiced_rtos_deinit_semaphore(&scan_semaphore);

    /* Done! */
    WPRINT_APP_INFO( ( "\nEnd of scan results, found %d APs\n" , record_count ) );

    return ERR_CMD_OK;
}

/*!
 ******************************************************************************
 * Starts a soft AP as specified by the provided arguments
 *
 * @return  0 for success, otherwise error
 */

int start_ap( int argc, char* argv[] )
{
    char* ssid                 = argv[1];
    wiced_security_t auth_type = wifi_utils_str_to_authtype(argv[2]);
    char* security_key         = argv[3];
    uint8_t  channel           = atoi(argv[4]);
    uint8_t  bandwidth         = CHANNEL_BW;
    uint32_t sta_channel       = 0;
    wiced_result_t result;
    wwd_result_t   wwd_result;
    uint8_t pmk[DOT11_PMK_LEN + 8]; /* PMK storage must be 40 octets in length for use in various functions */
    platform_dct_wifi_config_t* dct_wifi_config;
    uint8_t  key_length = 0;

    if ( wwd_wifi_is_ready_to_transceive( WWD_AP_INTERFACE ) == WWD_SUCCESS )
    {
        WPRINT_APP_INFO(( "Error: AP already started\n" ));
        return ERR_UNKNOWN;
    }

    if ( ( auth_type != WICED_SECURITY_WPA2_AES_PSK ) &&
         ( auth_type != WICED_SECURITY_OPEN ) &&
         ( auth_type != WICED_SECURITY_WPA2_MIXED_PSK ) &&
         ( auth_type != WICED_SECURITY_WEP_PSK ) &&
         ( auth_type != WICED_SECURITY_WEP_SHARED ) )
    {
        WPRINT_APP_INFO(( "Error: Invalid security type\n" ));
        return ERR_UNKNOWN;
    }

    if ( auth_type == WICED_SECURITY_OPEN )
    {
        WPRINT_APP_INFO(( "Open without any encryption" ));
        if ( 0 == console_prompt_confirm( ) )
        {
            return ERR_CMD_OK;
        }
    }

    if ( argc > 5 )
    {
        bandwidth = atoi(argv[5]);
    }

    if ( argc == 7 )
    {
        if ( memcmp( argv[6], "wps", sizeof("wps") ) != 0 )
        {
            return ERR_UNKNOWN;
        }
    }

    wwd_wifi_set_bandwidth( bandwidth );

    key_length = strlen(security_key);

    if ( ( auth_type & WPA2_SECURITY ) && ( key_length < MIN_PASSPHRASE_LEN ) )
    {
        WPRINT_APP_INFO(("Error: WPA key too short\n" ));
        return ERR_UNKNOWN;
    }

    if ( wwd_wifi_is_ready_to_transceive( WWD_STA_INTERFACE ) == WWD_SUCCESS )
    {
        wwd_result = wwd_wifi_get_channel( WWD_STA_INTERFACE, &sta_channel );
        if ( WWD_SUCCESS == wwd_result && sta_channel != channel )
        {
            WPRINT_APP_INFO(("AP will be started on same channel as STA (%lu); NOT (%d)", sta_channel, channel));
            if ( 0 == console_prompt_confirm( ) )
            {
                return ERR_CMD_OK;
            }
        }
    }

    /* Read config */
    wiced_dct_read_lock( (void**) &dct_wifi_config, WICED_TRUE, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t) );

    strlcpy( last_soft_ap_passphrase, security_key, sizeof( last_soft_ap_passphrase ) );

    /* Modify config */
    if ( key_length < MAX_PASSPHRASE_LEN)
    {
        char  temp_security_key[64];
        memset( temp_security_key, 0, MAX_PASSPHRASE_LEN );

        if ( auth_type == WICED_SECURITY_WEP_PSK || auth_type == WICED_SECURITY_WEP_SHARED )
        {
#ifdef WICED_WIFI_SOFT_AP_WEP_SUPPORT_ENABLED
            /* Format WEP security key */
            format_wep_keys( temp_security_key, security_key, &key_length, WEP_KEY_TYPE );
#else
            WPRINT_APP_INFO(( "Error: WEP is disabled\n" ));
            return ERR_UNKNOWN;
#endif
        }
        else
        {
            memset(pmk, 0, sizeof(pmk));
            if ( besl_802_11_generate_pmk( security_key, (unsigned char *) ssid, strlen( ssid ), (unsigned char*) pmk ) != BESL_SUCCESS )
            {
                WPRINT_APP_INFO(( "Error: Failed to generate pmk\n" ));
                return ERR_UNKNOWN;
            }

            key_length = MAX_PASSPHRASE_LEN;
            besl_host_hex_bytes_to_chars( temp_security_key, pmk, DOT11_PMK_LEN );
        }
        dct_wifi_config->soft_ap_settings.security_key_length = key_length;
        memcpy( dct_wifi_config->soft_ap_settings.security_key, temp_security_key, MAX_PASSPHRASE_LEN );
    }
    else
    {
        dct_wifi_config->soft_ap_settings.security_key_length = MAX_PASSPHRASE_LEN;
        /* strlcpy( ) guarantee to NUL-terminate the result
         *  - strlcpy( ) src ( parameter 2 ) must be NUL-terminated
         */
        strlcpy( dct_wifi_config->soft_ap_settings.security_key, security_key, sizeof( dct_wifi_config->soft_ap_settings.security_key ) );
    }

    if (strlen(ssid) > SSID_NAME_SIZE)
    {
        WPRINT_APP_INFO(( "Error: SSID longer than 32 characters\n" ));
        return ERR_UNKNOWN;
    }

    memcpy( (char*)dct_wifi_config->soft_ap_settings.SSID.value, ssid, strlen( ssid ) );
    dct_wifi_config->soft_ap_settings.security = auth_type;
    dct_wifi_config->soft_ap_settings.channel = channel;
    dct_wifi_config->soft_ap_settings.SSID.length = strlen( ssid );

    /* Write config */
    wiced_dct_write( (const void*)dct_wifi_config, DCT_WIFI_CONFIG_SECTION, 0, sizeof( platform_dct_wifi_config_t));
    wiced_dct_read_unlock( (void*) dct_wifi_config, WICED_TRUE );

    /* ip address specified ? */
    if ( argc >= 8 )
    {
        str_to_ip( argv[7], &ap_ip_settings.ip_address );
    }

    /* subnet specified ? */
    if ( argc >= 9 )
    {
        str_to_ip( argv[8], &ap_ip_settings.netmask );
    }

    if ( ( result = wiced_network_up( WICED_AP_INTERFACE, WICED_USE_INTERNAL_DHCP_SERVER, &ap_ip_settings ) ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("Error starting AP %u\n", (unsigned int)result));
        return result;
    }
#ifdef COMMAND_CONSOLE_WPS_ENABLED
    if ( ( argc >= 7 ) && ( memcmp( argv[6], "wps", sizeof("wps") ) == 0 ) )
    {
        result = enable_ap_registrar_events();
        if ( result != WICED_SUCCESS )
        {
            return result;
        }
    }
#endif
    strlcpy( last_started_ssid, ssid, sizeof ( last_started_ssid ) );
    return ERR_CMD_OK;
}

/*!
 ******************************************************************************
 * Stops a running soft AP
 *
 * @return  0 for success, otherwise error
 */

int stop_ap( int argc, char* argv[] )
{
    if (wwd_wifi_is_ready_to_transceive( WWD_AP_INTERFACE ) != WWD_SUCCESS)
    {
        return ERR_CMD_OK;
    }

#ifdef COMMAND_CONSOLE_WPS_ENABLED
    disable_ap_registrar_events();
#endif

    wwd_wifi_deauth_all_associated_client_stas( WWD_DOT11_RC_UNSPECIFIED, WWD_AP_INTERFACE );

    return wiced_network_down( WICED_AP_INTERFACE );
}

int get_associated_sta_list( int argc, char* argv[] )
{
    uint8_t* buffer = NULL;
    wiced_maclist_t * clients = NULL;
    const wiced_mac_t * current;
    wl_bss_info_t ap_info;
    wiced_security_t sec;
    uint32_t max_associations = 0;
    size_t size = 0;
    int32_t rssi = 0;

    if ( wwd_wifi_get_max_associations( &max_associations ) != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Failed to get max number of associations\n"));
        max_associations = 5;
    }
    else
    {
        WPRINT_APP_INFO(("Max number of associations: %u\n", (unsigned int)max_associations));
    }

    size = (sizeof(uint32_t) + (max_associations * sizeof(wiced_mac_t)));
    buffer = calloc(1, size);

    if (buffer == NULL)
    {
        WPRINT_APP_INFO(("Unable to allocate memory for associations list\n"));
        return WICED_ERROR;
    }

    clients = (wiced_maclist_t*)buffer;
    clients->count = max_associations;/* field must be updated for use in the below wwd function */

    wwd_wifi_get_associated_client_list(clients, size);

    memset(&ap_info, 0, sizeof(wl_bss_info_t));
    if (wwd_wifi_is_ready_to_transceive( WWD_STA_INTERFACE ) == WWD_SUCCESS)
    {
        wwd_wifi_get_ap_info( &ap_info, &sec );
        if (clients->count == 0 )
        {
            clients->count = 1;
            memcpy(&clients->mac_list[0], &ap_info.BSSID, sizeof(wl_ether_addr_t));
        }
    }

    WPRINT_APP_INFO(("Current number of associated STAs: %u\n", (unsigned int)clients->count));
    current = &clients->mac_list[0];
    WPRINT_APP_INFO(("\n"));
    while ((clients->count > 0) && (!(NULL_MAC(current->octet))))
    {
        WPRINT_APP_INFO(("%02x:%02x:%02x:%02x:%02x:%02x ",
                            current->octet[0],
                            current->octet[1],
                            current->octet[2],
                            current->octet[3],
                            current->octet[4],
                            current->octet[5]));
        if (memcmp(current->octet, &(ap_info.BSSID), sizeof(wiced_mac_t)) != 0)
        {
            wwd_wifi_get_ap_client_rssi(&rssi, (wiced_mac_t*)&current->octet[0]);
            WPRINT_APP_INFO(("%3lddBm  Client\n", (long int)rssi));
        }
        else
        {
            wwd_wifi_get_rssi( &rssi );
            WPRINT_APP_INFO(("%3lddBm  AP\n", (long int)rssi));
        }
        --clients->count;
        ++current;
    }
    WPRINT_APP_INFO(("\n"));
    besl_host_free( buffer );
    return ERR_CMD_OK;
}

int test_ap( int argc, char* argv[] )
{
    int i;
    int iterations;

    if (  argc < 6 )
    {
        return ERR_UNKNOWN;
    }
    iterations = atoi(argv[argc - 1]);
    WPRINT_APP_INFO(("Iterations: %d\n", iterations));
    for (i = 0; i < iterations; i++ )
    {
        WPRINT_APP_INFO(( "Iteration %d\n", i));
        start_ap( argc-1, argv );
        stop_ap( 0, NULL );
    }
    wiced_mac_t mac;
    if ( wwd_wifi_get_mac_address( &mac, WWD_STA_INTERFACE ) == WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Test Pass (MAC address is: %02X:%02X:%02X:%02X:%02X:%02X)\n", mac.octet[0], mac.octet[1], mac.octet[2], mac.octet[3], mac.octet[4], mac.octet[5]));
    }
    else
    {
        WPRINT_APP_INFO(("Test Fail\n"));
    }
    return ERR_CMD_OK;
}

static int test_join_process( int argc, char* argv[], int specific )
{
    int result = 0;
    int i;
    int iterations;
    uint32_t join_fails = 0, leave_fails = 0, join_successes = 0;
    wiced_time_t t1, t2, average_time = 0;

    if (  argc < 5 )
    {
        return ERR_INSUFFICENT_ARGS;
    }
    iterations = atoi(argv[argc - 1]);

    for ( i = 0 ; i < iterations ; i++ )
    {
        WPRINT_APP_INFO(( "%d ", i));
        wiced_time_get_time( &t1 );

        if ( 0 == specific )
        {
            result = join( argc-1, argv );
        }
        else
        {
            result = join_specific( argc-1, argv );
        }

        if ( ERR_CMD_OK != result )
        {
            ++join_fails;
        }
        else
        {
            wiced_time_get_time( &t2 );
            t2 = t2 - t1;
            WPRINT_APP_INFO( ( "Time for successful join = %u ms\n", (unsigned int)t2 ) );
            average_time += t2;
            join_successes++;
        }

        if ( ERR_CMD_OK != leave( 0, NULL ) )
        {
            ++leave_fails;
        }
    }

    WPRINT_APP_INFO(("Join failures:     %u\n", (unsigned int)join_fails));
    WPRINT_APP_INFO(("Leave failures:    %u\n", (unsigned int)leave_fails));
    if (join_successes > 0)
    {
        WPRINT_APP_INFO(("Average join time: %u ms\n", (unsigned int)(average_time/join_successes)));
    }

    return ERR_CMD_OK;
}

int test_join( int argc, char* argv[] )
{
    return test_join_process( argc, argv, 0 );
}

int test_join_specific( int argc, char* argv[] )
{
    return test_join_process( argc, argv, 1 );
}

int test_credentials( int argc, char* argv[] )
{
    wwd_result_t result;
    wiced_scan_result_t ap;

    memset(&ap, 0, sizeof(ap));

    ap.SSID.length = strlen(argv[1]);
    memcpy(ap.SSID.value, argv[1], ap.SSID.length);
    wifi_utils_str_to_mac(argv[2], &ap.BSSID);
    ap.channel = atoi(argv[3]);
    ap.security = wifi_utils_str_to_authtype(argv[4]);
    result = wwd_wifi_test_credentials(&ap, (uint8_t*)argv[5], strlen(argv[5]));

    if ( result == WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Credentials are good\n"));
    }
    else
    {
        WPRINT_APP_INFO(("Credentials are bad\n"));
    }

    return ERR_CMD_OK;
}

int get_soft_ap_credentials( int argc, char* argv[] )
{
    wiced_security_t sec;
    platform_dct_wifi_config_t* dct_wifi_config;

    if ( wwd_wifi_is_ready_to_transceive( WWD_AP_INTERFACE ) != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Use start_ap command to bring up AP interface first\n"));
        return ERR_CMD_OK;
    }

    /* Read config to get internal AP settings */
    wiced_dct_read_lock( (void**) &dct_wifi_config, WICED_FALSE, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t) );
    WPRINT_APP_INFO(("SSID : %s\n", (char*)dct_wifi_config->soft_ap_settings.SSID.value));
    sec = dct_wifi_config->soft_ap_settings.security;
    WPRINT_APP_INFO( ( "Security : %s\n", ( sec == WICED_SECURITY_OPEN )           ? "Open" :
                                            ( sec == WICED_SECURITY_WEP_PSK )        ? "WEP" :
                                            ( sec == WICED_SECURITY_WPA_TKIP_PSK )   ? "WPA TKIP" :
                                            ( sec == WICED_SECURITY_WPA_AES_PSK )    ? "WPA AES" :
                                            ( sec == WICED_SECURITY_WPA2_AES_PSK )   ? "WPA2 AES" :
                                            ( sec == WICED_SECURITY_WPA2_TKIP_PSK )  ? "WPA2 TKIP" :
                                            ( sec == WICED_SECURITY_WPA2_MIXED_PSK ) ? "WPA2 Mixed" :
                                            "Unknown" ) );
    WPRINT_APP_INFO(("Passphrase : %s\n", last_soft_ap_passphrase));

    wiced_dct_read_unlock( (void*) dct_wifi_config, WICED_FALSE );

    return ERR_CMD_OK;
}

int get_pmk( int argc, char* argv[] )
{
    char pmk[64];

    if ( wwd_wifi_get_pmk( argv[1], strlen( argv[1] ), pmk ) == WWD_SUCCESS )
    {
        WPRINT_APP_INFO( ("%s\n", pmk) );
        return ERR_CMD_OK;
    }
    else
    {
        return ERR_UNKNOWN;
    }
}



/* WLMCD_VER_01 is specific to 43012 WLAN firmware wl commands structure */
#ifdef WLCMD_VER_01


/* For core-revision greater than >= 40 the MAC STAT counters for ucode have
 * been moved to into new struct wl_cnt_ge40mcst_v1_t
 */
#define CNT_DIFF40(type, first, second, field) (type->field = ((second->field - first->field)/diff_secs))
static int
counter_diff40(int diff_secs,
    wl_cnt_wlc_t* first_wl_cnt,
    wl_cnt_ge40mcst_v1_t* first_wl_cnt_ge40,
    wl_cnt_wlc_t* second_wl_cnt,
    wl_cnt_ge40mcst_v1_t* second_wl_cnt_ge40,
    wl_cnt_wlc_t* wl_cnt_results,
    wl_cnt_ge40mcst_v1_t* wl_cnt_ge40_results )
{
    if (diff_secs <= 0)
    {
        return ERR_UNKNOWN;
    }
    if ((first_wl_cnt_ge40 == NULL)|| (second_wl_cnt_ge40 == NULL) ||
            ( first_wl_cnt == NULL) || (second_wl_cnt == NULL))
    {
        return ERR_UNKNOWN;
    }

    /* transmit stat counters */
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txframe                );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txbyte                 );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txretrans              );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txerror                );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txctl                  );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txprshort              );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txserr                 );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txnobuf                );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txnoassoc              );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txrunt                 );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txchit                 );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txcmiss                );

    /* transmit chip error counters */
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt,  txuflo                );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt,  txphyerr              );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt,  txphycrs              );

    /* receive stat counters */
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxframe                );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxbyte                 );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxerror                );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxctl                  );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxnobuf                );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxnondata              );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxbadds                );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxbadcm                );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxfragerr              );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxrunt                 );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxgiant                );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxnoscb                );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxbadproto             );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxbadsrcmac            );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxbadda                );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxfilter               );

    /* receive chip error counters */
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxoflo                 );
    //rxuflo[NFIFO]

    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, d11cnt_txrts_off       );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, d11cnt_rxcrc_off       );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, d11cnt_txnocts_off     );

    /* misc counters */
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, dmade                  );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, dmada                  );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, dmape                  );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, reset                  );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, tbtt                   );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txdmawar               );
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, pkt_callback_reg_fail  );

    /* MAC counters: 32-bit version of d11.h's macstat_t */
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, txallfrm);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, txrtsfrm);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, txctsfrm);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, txackfrm);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, txdnlfrm);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, txbcnfrm);
    //txfunfl[6]
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, txtplunfl);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, txphyerror);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxfrmtoolong);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxfrmtooshrt);
 // CNT_DIFF( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxinvmachdr);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxbadfcs);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxbadplcp);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxcrsglitch);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxstrt);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxdtucastmbss);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxmgucastmbss);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxctlucast);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxrtsucast);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxctsucast);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxackucast);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxdtocast);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxmgocast);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxctlocast);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxrtsocast);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxctsocast);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxdtmcast);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxmgmcast);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxctlmcast);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxbeaconmbss);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxdtucastobss);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxbeaconobss);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxrsptmout);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, bcntxcancl);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxf0ovfl);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxf1ovfl);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxhlovfl);
//  CNT_DIFF(txsfovfl);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, pmqovfl);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxcgprqfrm);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, rxcgprsqovfl);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, txcgprsfail);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, txcgprssuc);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, prs_timeout);
//  CNT_DIFF(rxnack);
//  CNT_DIFF(frmscons);
//  CNT_DIFF(txnack);

    /* 802.11 MIB counters, pp. 614 of 802.11 reaff doc. */
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txfrag);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txmulti);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txfail);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txretry);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txretrie);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxdup);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txrts);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txnocts);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txnoack);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxfrag);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxmulti);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxcrc);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txfrmsnt);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxundec);

    /* WPA2 counters (see rxundec for DecryptFailureCount) */
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, tkipmicfaill);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, tkipcntrmsr);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, tkipreplay);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, ccmpfmterr);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, ccmpreplay);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, ccmpundec);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, fourwayfail);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, wepundec);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, wepicverr);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, decsuccess);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, tkipicverr);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, wepexcluded);

    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txchanrej);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, psmwds);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, phywatchdog);

    /* MBSS counters, AP only */
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, prq_entries_handled);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, prq_undirected_entries);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, prq_bad_entries);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, atim_suppress_count);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, bcn_template_not_ready);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, bcn_template_not_ready_done);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, late_tbtt_dpc);

    /* per-rate receive stat counters */
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx1mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx2mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx5mbps5);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx6mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx9mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx11mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx12mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx18mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx24mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx36mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx48mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx54mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx108mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx162mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx216mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx270mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx324mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx378mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx432mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx486mbps);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rx540mbps);

    /* pkteng rx frame stats */
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, pktengrxducast);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, pktengrxdmcast);

    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rfdisable);
    CNT_DIFF40( wl_cnt_ge40_results, first_wl_cnt_ge40, second_wl_cnt_ge40, bphy_rxcrsglitch);

    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txexptime);

    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txmpdu_sgi);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxmpdu_sgi);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, txmpdu_stbc);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxmpdu_stbc);

    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, rxundec_mcst);

    /* WPA2 counters (see rxundec for DecryptFailureCount) */
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, tkipmicfaill_mcst);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, tkipcntrmsr_mcst);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, tkipreplay_mcst);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, ccmpfmterr_mcst);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, ccmpreplay_mcst);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, ccmpundec_mcst);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, fourwayfail_mcst);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, wepundec_mcst);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, wepicverr_mcst);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, decsuccess_mcst);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, tkipicverr_mcst);
    CNT_DIFF40( wl_cnt_results, first_wl_cnt, second_wl_cnt, wepexcluded_mcst);

    return ERR_CMD_OK;
}
#else
#define CNT_DIFF(field) results->field = (second->field - first->field)/diff_secs

/* results = second - first */
static int
counter_diff(int diff_secs,
    wiced_counters_t* first,
    wiced_counters_t* second,
    wiced_counters_t* results)
{

    if (diff_secs <= 0)
        return ERR_UNKNOWN;
    if (!first || !second || !results)
        return ERR_UNKNOWN;
    if ((first->version != second->version )|| (first->length != second->length))
        return ERR_UNKNOWN;

    /* transmit stat counters */
    CNT_DIFF(txframe);
    CNT_DIFF(txbyte);
    CNT_DIFF(txretrans);
    CNT_DIFF(txerror );
    CNT_DIFF(txctl);
    CNT_DIFF(txprshort);
    CNT_DIFF(txserr);
    CNT_DIFF(txnobuf);
    CNT_DIFF(txnoassoc);
    CNT_DIFF(txrunt);
    CNT_DIFF(txchit);
    CNT_DIFF(txcmiss);

    /* transmit chip error counters */
    CNT_DIFF(txuflo);
    CNT_DIFF(txphyerr);
    CNT_DIFF(txphycrs);

    /* receive stat counters */
    CNT_DIFF(rxframe);
    CNT_DIFF(rxbyte);
    CNT_DIFF(rxerror);
    CNT_DIFF(rxctl);
    CNT_DIFF(rxnobuf);
    CNT_DIFF(rxnondata);
    CNT_DIFF(rxbadds);
    CNT_DIFF(rxbadcm);
    CNT_DIFF(rxfragerr);
    CNT_DIFF(rxrunt);
    CNT_DIFF(rxgiant);
    CNT_DIFF(rxnoscb);
    CNT_DIFF(rxbadproto);
    CNT_DIFF(rxbadsrcmac);
    CNT_DIFF(rxbadda);
    CNT_DIFF(rxfilter);

    /* receive chip error counters */
    CNT_DIFF(rxoflo               );
    //rxuflo[NFIFO]

    CNT_DIFF(d11cnt_txrts_off);
    CNT_DIFF(d11cnt_rxcrc_off);
    CNT_DIFF(d11cnt_txnocts_off);

    /* misc counters */
    CNT_DIFF(dmade);
    CNT_DIFF(dmada);
    CNT_DIFF(dmape);
    CNT_DIFF(reset);
    CNT_DIFF(tbtt);
    CNT_DIFF(txdmawar);
    CNT_DIFF(pkt_callback_reg_fail);

    /* MAC counters: 32-bit version of d11.h's macstat_t */
    CNT_DIFF(txallfrm);
    CNT_DIFF(txrtsfrm);
    CNT_DIFF(txctsfrm);
    CNT_DIFF(txackfrm);
    CNT_DIFF(txdnlfrm);
    CNT_DIFF(txbcnfrm);
    //txfunfl[6]
    CNT_DIFF(txtplunfl);
    CNT_DIFF(txphyerror);
    CNT_DIFF(rxfrmtoolong);
    CNT_DIFF(rxfrmtooshrt);
    CNT_DIFF(rxinvmachdr);
    CNT_DIFF(rxbadfcs);
    CNT_DIFF(rxbadplcp);
    CNT_DIFF(rxcrsglitch);
    CNT_DIFF(rxstrt);
    CNT_DIFF(rxdfrmucastmbss);
    CNT_DIFF(rxmfrmucastmbss);
    CNT_DIFF(rxcfrmucast);
    CNT_DIFF(rxrtsucast);
    CNT_DIFF(rxctsucast);
    CNT_DIFF(rxackucast);
    CNT_DIFF(rxdfrmocast);
    CNT_DIFF(rxmfrmocast);
    CNT_DIFF(rxcfrmocast);
    CNT_DIFF(rxrtsocast);
    CNT_DIFF(rxctsocast);
    CNT_DIFF(rxdfrmmcast);
    CNT_DIFF(rxmfrmmcast);
    CNT_DIFF(rxcfrmmcast);
    CNT_DIFF(rxbeaconmbss);
    CNT_DIFF(rxdfrmucastobss);
    CNT_DIFF(rxbeaconobss);
    CNT_DIFF(rxrsptmout);
    CNT_DIFF(bcntxcancl);
    CNT_DIFF(rxf0ovfl);
    CNT_DIFF(rxf1ovfl);
    CNT_DIFF(rxf2ovfl);
    CNT_DIFF(txsfovfl);
    CNT_DIFF(pmqovfl);
    CNT_DIFF(rxcgprqfrm);
    CNT_DIFF(rxcgprsqovfl);
    CNT_DIFF(txcgprsfail);
    CNT_DIFF(txcgprssuc);
    CNT_DIFF(prs_timeout);
    CNT_DIFF(rxnack);
    CNT_DIFF(frmscons);
    CNT_DIFF(txnack);

    /* 802.11 MIB counters, pp. 614 of 802.11 reaff doc. */
    CNT_DIFF(txfrag);
    CNT_DIFF(txmulti);
    CNT_DIFF(txfail);
    CNT_DIFF(txretry);
    CNT_DIFF(txretrie);
    CNT_DIFF(rxdup);
    CNT_DIFF(txrts);
    CNT_DIFF(txnocts);
    CNT_DIFF(txnoack);
    CNT_DIFF(rxfrag);
    CNT_DIFF(rxmulti);
    CNT_DIFF(rxcrc);
    CNT_DIFF(txfrmsnt);
    CNT_DIFF(rxundec);

    /* WPA2 counters (see rxundec for DecryptFailureCount) */
    CNT_DIFF(tkipmicfaill);
    CNT_DIFF(tkipcntrmsr);
    CNT_DIFF(tkipreplay);
    CNT_DIFF(ccmpfmterr);
    CNT_DIFF(ccmpreplay);
    CNT_DIFF(ccmpundec);
    CNT_DIFF(fourwayfail);
    CNT_DIFF(wepundec);
    CNT_DIFF(wepicverr);
    CNT_DIFF(decsuccess);
    CNT_DIFF(tkipicverr);
    CNT_DIFF(wepexcluded);

    CNT_DIFF(txchanrej);
    CNT_DIFF(psmwds);
    CNT_DIFF(phywatchdog);

    /* MBSS counters, AP only */
    CNT_DIFF(prq_entries_handled);
    CNT_DIFF(prq_undirected_entries);
    CNT_DIFF(prq_bad_entries);
    CNT_DIFF(atim_suppress_count);
    CNT_DIFF(bcn_template_not_ready);
    CNT_DIFF(bcn_template_not_ready_done);
    CNT_DIFF(late_tbtt_dpc);

    /* per-rate receive stat counters */
    CNT_DIFF(rx1mbps);
    CNT_DIFF(rx2mbps);
    CNT_DIFF(rx5mbps5);
    CNT_DIFF(rx6mbps);
    CNT_DIFF(rx9mbps);
    CNT_DIFF(rx11mbps);
    CNT_DIFF(rx12mbps);
    CNT_DIFF(rx18mbps);
    CNT_DIFF(rx24mbps);
    CNT_DIFF(rx36mbps);
    CNT_DIFF(rx48mbps);
    CNT_DIFF(rx54mbps);
    CNT_DIFF(rx108mbps);
    CNT_DIFF(rx162mbps);
    CNT_DIFF(rx216mbps);
    CNT_DIFF(rx270mbps);
    CNT_DIFF(rx324mbps);
    CNT_DIFF(rx378mbps);
    CNT_DIFF(rx432mbps);
    CNT_DIFF(rx486mbps);
    CNT_DIFF(rx540mbps);

    /* pkteng rx frame stats */
    CNT_DIFF(pktengrxducast);
    CNT_DIFF(pktengrxdmcast);

    CNT_DIFF(rfdisable);
    CNT_DIFF(bphy_rxcrsglitch);

    CNT_DIFF(txexptime);

    CNT_DIFF(txmpdu_sgi);
    CNT_DIFF(rxmpdu_sgi);
    CNT_DIFF(txmpdu_stbc);
    CNT_DIFF(rxmpdu_stbc);

    CNT_DIFF(rxundec_mcst);

    /* WPA2 counters (see rxundec for DecryptFailureCount) */
    CNT_DIFF(tkipmicfaill_mcst);
    CNT_DIFF(tkipcntrmsr_mcst);
    CNT_DIFF(tkipreplay_mcst);
    CNT_DIFF(ccmpfmterr_mcst);
    CNT_DIFF(ccmpreplay_mcst);
    CNT_DIFF(ccmpundec_mcst);
    CNT_DIFF(fourwayfail_mcst);
    CNT_DIFF(wepundec_mcst);
    CNT_DIFF(wepicverr_mcst);
    CNT_DIFF(decsuccess_mcst);
    CNT_DIFF(tkipicverr_mcst);
    CNT_DIFF(wepexcluded_mcst);

    return ERR_CMD_OK;
}
#endif
#define CNT_LOG( type,field)  if (type->field || cnt_verbose) WPRINT_APP_INFO(("%-16s%4ld%s  ", #field, type->field, normal?"/s":""))
static int
print_counters_rate(void* result, int normal)
{
#ifdef  WLCMD_VER_01
    wl_cnt_wlc_t* results = (wl_cnt_wlc_t *)result;
#else
    wiced_counters_t* results = (wiced_counters_t *)result;
#endif
    CNT_LOG(results,rx1mbps);
    CNT_LOG(results,rx2mbps);
    CNT_LOG(results,rx5mbps5);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rx6mbps);
    CNT_LOG(results,rx9mbps);
    CNT_LOG(results,rx11mbps);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rx12mbps);
    CNT_LOG(results,rx18mbps);
    CNT_LOG(results,rx24mbps);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rx36mbps);
    CNT_LOG(results,rx48mbps);
    CNT_LOG(results,rx54mbps);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rx108mbps);
    CNT_LOG(results,rx162mbps);
    CNT_LOG(results,rx216mbps);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rx270mbps);
    CNT_LOG(results,rx324mbps);
    CNT_LOG(results,rx378mbps);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rx432mbps);
    CNT_LOG(results,rx486mbps);
    CNT_LOG(results,rx540mbps);
WPRINT_APP_INFO(("\n"));

    return ERR_CMD_OK;
};

static int
print_counters_tx( void* result, void* result1, int normal)
{
#ifdef  WLCMD_VER_01
    wl_cnt_ge40mcst_v1_t* results = (wl_cnt_ge40mcst_v1_t *)result;
    wl_cnt_wlc_t* results1 = (wl_cnt_wlc_t *)result1;

    CNT_LOG(results1,txframe);
    CNT_LOG(results1,txbyte);
    CNT_LOG(results1,txretrans);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results1,txerror );
    CNT_LOG(results1,txctl);
#else
    wiced_counters_t* results = (wiced_counters_t *)result;

    CNT_LOG(results,txframe);
    CNT_LOG(results,txbyte);
    CNT_LOG(results,txretrans);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,txerror );
    CNT_LOG(results,txctl);
#endif
    CNT_LOG(results,txallfrm);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,txrtsfrm);
    CNT_LOG(results,txctsfrm);
    CNT_LOG(results,txackfrm);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,txdnlfrm);
    CNT_LOG(results,txbcnfrm);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,txtplunfl);
    CNT_LOG(results,txphyerror);
    CNT_LOG(results,bcntxcancl);
WPRINT_APP_INFO(("\n"));

#ifdef WLCMD_VER_01
   // CNT_LOG(results1,txsfovfl);
#else
    CNT_LOG(results,txsfovfl);
#endif
    CNT_LOG(results,txcgprsfail);
    CNT_LOG(results,txcgprssuc);

#ifdef WLCMD_VER_01

WPRINT_APP_INFO(("\n"));
    // CNT_LOG(results1,txnack);
    CNT_LOG(results1,txfrag);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results1,txmulti);
    CNT_LOG(results1,txfail);
    CNT_LOG(results1,txretry);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results1,txretrie);
    CNT_LOG(results1,txrts);
    CNT_LOG(results1,txnocts);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results1,txnoack);
    CNT_LOG(results1,txfrmsnt);
    CNT_LOG(results1,txchanrej);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results1,txmpdu_sgi);
    CNT_LOG(results1,txmpdu_stbc);
    CNT_LOG(results1,txexptime);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results1,txprshort);
    CNT_LOG(results1,txprshort);
    CNT_LOG(results1,txserr);
    CNT_LOG(results1,txnobuf);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results1,txnoassoc);
    CNT_LOG(results1,txrunt);
    CNT_LOG(results1,txchit);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results1,txcmiss);
    CNT_LOG(results1,txuflo);
    CNT_LOG(results1,txphyerr);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results1,txphycrs);
    CNT_LOG(results1,d11cnt_txrts_off);
    CNT_LOG(results1,d11cnt_txnocts_off);
    CNT_LOG(results1,txdmawar);
#else
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,txnack);
    CNT_LOG(results,txfrag);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,txmulti);
    CNT_LOG(results,txfail);
    CNT_LOG(results,txretry);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,txretrie);
    CNT_LOG(results,txrts);
    CNT_LOG(results,txnocts);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,txnoack);
    CNT_LOG(results,txfrmsnt);
    CNT_LOG(results,txchanrej);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,txmpdu_sgi);
    CNT_LOG(results,txmpdu_stbc);
    CNT_LOG(results,txexptime);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,txprshort);
    CNT_LOG(results,txprshort);
    CNT_LOG(results,txserr);
    CNT_LOG(results,txnobuf);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,txnoassoc);
    CNT_LOG(results,txrunt);
    CNT_LOG(results,txchit);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,txcmiss);
    CNT_LOG(results,txuflo);
    CNT_LOG(results,txphyerr);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,txphycrs);
    CNT_LOG(results,d11cnt_txrts_off);
    CNT_LOG(results,d11cnt_txnocts_off);
    CNT_LOG(results,txdmawar);
#endif

WPRINT_APP_INFO(("\n"));
    return ERR_CMD_OK;
}


static int
print_counters_rx(void* result, void* result1, int normal)
{
#ifdef WLCMD_VER_01
    wl_cnt_ge40mcst_v1_t* results = (wl_cnt_ge40mcst_v1_t *)result;
    wl_cnt_wlc_t* results1 = (wl_cnt_wlc_t *)result1;

    CNT_LOG(results1,rxframe);
    CNT_LOG(results1,rxbyte);
    CNT_LOG(results1,rxerror);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results1,rxctl);
    CNT_LOG(results1,rxnobuf);
    CNT_LOG(results1,rxnondata);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results1,rxbadds);
    CNT_LOG(results1,rxbadcm);
    CNT_LOG(results1,rxfragerr);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results1,rxrunt);
    CNT_LOG(results1,rxgiant);
    CNT_LOG(results1,rxnoscb);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results1,rxbadproto);
    CNT_LOG(results1,rxbadsrcmac);
    CNT_LOG(results1,rxbadda);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results1,rxfilter);
    CNT_LOG(results1,rxoflo);
    CNT_LOG(results1,d11cnt_rxcrc_off);
WPRINT_APP_INFO(("\n"));

#else
    wiced_counters_t* results = (wiced_counters_t *)result;

    CNT_LOG(results,rxframe);
    CNT_LOG(results,rxbyte);
    CNT_LOG(results,rxerror);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rxctl);
    CNT_LOG(results,rxnobuf);
    CNT_LOG(results,rxnondata);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rxbadds);
    CNT_LOG(results,rxbadcm);
    CNT_LOG(results,rxfragerr);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rxrunt);
    CNT_LOG(results,rxgiant);
    CNT_LOG(results,rxnoscb);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rxbadproto);
    CNT_LOG(results,rxbadsrcmac);
    CNT_LOG(results,rxbadda);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rxfilter);
    CNT_LOG(results,rxoflo);
    CNT_LOG(results,d11cnt_rxcrc_off);
WPRINT_APP_INFO(("\n"));
#endif

    CNT_LOG(results,rxfrmtoolong);
    CNT_LOG(results,rxfrmtooshrt);
WPRINT_APP_INFO(("\n"));

#ifdef WLCMD_VER_01
    // CNT_LOG(results1,rxinvmachdr);
#else
    CNT_LOG(results,rxinvmachdr);
#endif

    CNT_LOG(results,rxbadfcs);
    CNT_LOG(results,rxbadplcp);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rxcrsglitch);
    CNT_LOG(results,rxstrt);
WPRINT_APP_INFO(("\n"));

#ifdef WLCMD_VER_01
    CNT_LOG(results,rxctlucast);
#else
    CNT_LOG(results,rxcfrmucast);
#endif

    CNT_LOG(results,rxrtsucast);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rxctsucast);
    CNT_LOG(results,rxackucast);
    CNT_LOG(results,rxf0ovfl);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rxbeaconobss);

#ifdef WLCMD_VER_01
    CNT_LOG(results,rxdtucastobss);
#else
    CNT_LOG(results,rxdfrmucastobss);
#endif
    CNT_LOG(results,rxrsptmout);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rxf1ovfl);
#ifdef WLCMD_VER_01
    //  CNT_LOG(results,rxf2ovfl);
#else
    CNT_LOG(results,rxf2ovfl);
#endif
    CNT_LOG(results,rxcgprqfrm);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rxcgprsqovfl);

#ifdef WLCMD_VER_01
    // CNT_LOG(results,rxnack);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results1,rxdup);
    CNT_LOG(results1,rxfrag);
    CNT_LOG(results1,rxmulti);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results1,rxcrc);
    CNT_LOG(results1,rxundec);
#else
    CNT_LOG(results,rxnack);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rxdup);
    CNT_LOG(results,rxfrag);
    CNT_LOG(results,rxmulti);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rxcrc);
    CNT_LOG(results,rxundec);
#endif
    CNT_LOG(results,pktengrxducast);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,pktengrxdmcast);
    CNT_LOG(results,bphy_rxcrsglitch);
#ifdef WLCMD_VER_01
    CNT_LOG(results1,rxmpdu_sgi);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results1,rxmpdu_stbc);
#else
    CNT_LOG(results,rxmpdu_sgi);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rxmpdu_stbc);
#endif

#ifdef WLCMD_VER_01
    CNT_LOG(results1,rxundec_mcst);
#else
    CNT_LOG(results,rxundec_mcst);
#endif
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rxbeaconmbss);
#ifdef WLCMD_VER_01
    CNT_LOG(results,rxdtucastmbss);
    CNT_LOG(results,rxmgucastmbss);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rxdtmcast);
    CNT_LOG(results,rxmgmcast);
    CNT_LOG(results,rxctlmcast);
WPRINT_APP_INFO(("\n"));
    CNT_LOG(results,rxdtocast);
    CNT_LOG(results,rxmgocast);
    CNT_LOG(results,rxctlocast);
WPRINT_APP_INFO(("\n"));
#else
   CNT_LOG(results,rxdfrmucastmbss);
   CNT_LOG(results,rxmfrmucastmbss);
WPRINT_APP_INFO(("\n"));
   CNT_LOG(results,rxdfrmmcast);
   CNT_LOG(results,rxmfrmmcast);
   CNT_LOG(results,rxcfrmmcast);
WPRINT_APP_INFO(("\n"));
   CNT_LOG(results,rxdfrmocast);
   CNT_LOG(results,rxmfrmocast);
   CNT_LOG(results,rxcfrmocast);
WPRINT_APP_INFO(("\n"));
#endif
    CNT_LOG(results,rxrtsocast);
    CNT_LOG(results,rxctsocast);
WPRINT_APP_INFO(("\n"));
    return ERR_CMD_OK;
}

/* Example: get_counters: -t 3 -n rx rxrate
 *  -t seconds: collect 'seconds' worth of data
 *  -n          normalize into per/seconds.
 *  tx          tx counters
 *  rx          rx counters
 *  rate        rx rate histogram
 */
int get_counters( int argc, char* argv[] )
{
    UNUSED_PARAMETER( argc );
    UNUSED_PARAMETER( argv );
    wiced_counters_t* data, *data2 = NULL;
    int seconds = 0;    /* Num seconds to measure */
    int normal = 0;     /* Normalize to per second? */
    int flags = 0;
#ifdef WLCMD_VER_01
    reinit_rsns_t reinit_rsns, reinit_rsns2;
    wl_cnt_wlc_t wl_cnt_wlc, wl_cnt_wlc2, wl_cnt_results;
    wl_cnt_ge40mcst_v1_t wl_cnt_ge40, wl_cnt_get40_2, wl_cnt_ge40_results;
    wwd_result_t result = WWD_SUCCESS;
#endif

    while (*++argv) {
        if (!strcmp(*argv, "-t")){
            if (*(argv+1)) {
                seconds = atoi(*(argv+1));
                if (seconds < 0)
                {
                    WPRINT_APP_INFO(("Seconds (%d) must be > 0.\n", seconds));
                    return ERR_UNKNOWN;
                }
                argv++;
            } else {
                WPRINT_APP_INFO(("Usage: get_counters [-t seconds [-n]] [-v] [tx] [rx] [rate]\n"));
            }
        } else
            if (!strcmp(*argv, "-n")) {
                normal++;
            } else
                if (!strcmp(*argv, "tx")) {
                    flags |= DUMP_TX;
                } else
                    if (!strcmp(*argv, "rx")) {
                        flags |= DUMP_RX;
                    } else
                        if (!strcmp(*argv, "rate")) {
                            flags |= DUMP_RATE;
                        } else
                            if (!strcmp(*argv, "-v")) {
                                cnt_verbose = 1;
                            } else {
                                WPRINT_APP_INFO(("%s: Unknown parameter: %s\n", __FUNCTION__, *argv));
                                return ERR_UNKNOWN;
                            }
    }

    if (seconds == 0)   /* Can't normalize 0 seconds */
        normal = 0;
    if (flags == 0)     /* Default to all counters */
        flags = DUMP_TX | DUMP_RX | DUMP_RATE | DUMP_AMPDU;

    WPRINT_APP_DEBUG(("Flags 0x%x, Normalize %d, Seconds %d\n", flags, normal, seconds));

    data = malloc_named( "stats_counters_buffer", sizeof(wiced_counters_t));
    if (!data) {
         WPRINT_APP_ERROR(("Unable to allocate data buffer!\n"));
         return ERR_UNKNOWN;
    }
    if (seconds) {
        data2 = malloc_named( "stats_counters_buffer2", sizeof(wiced_counters_t));
        if (!data2) {
            free(data);
            WPRINT_APP_ERROR(("Unable to allocate second data buffer!\n"));
            return ERR_UNKNOWN;
        }
    }

    if (wwd_get_counters(data) != WWD_SUCCESS) {
        free(data);
        if (seconds)
            free(data2);
        return ERR_UNKNOWN;
    }

#ifdef WLCMD_VER_01
    if ( data->datalen > sizeof(wiced_counters_t))
    {
        WPRINT_APP_INFO(("Data Length greater than sizeof Buffer allocated..\n"));
        return ERR_UNKNOWN;
    }

    result =  wwd_unpack_wl_counters_xtlv(  data, &reinit_rsns, &wl_cnt_wlc, &wl_cnt_ge40 );
    if ( result != WWD_SUCCESS )
    {   WPRINT_APP_INFO(("wwd_unpack_wl_counters_xtlv failed result:%d..\n", result ));
        return ERR_UNKNOWN;
    }
#endif
    if (seconds) {
        wiced_rtos_delay_milliseconds(seconds * 1000);
        if (wwd_get_counters(data2) == WWD_SUCCESS) {
            if (!normal)
                seconds = 1; /* Dividing by 1 effectively does nothing */
#ifdef WLCMD_VER_01
            result =  wwd_unpack_wl_counters_xtlv(  data2, &reinit_rsns2, &wl_cnt_wlc2, &wl_cnt_get40_2 );
            if ( result != WWD_SUCCESS )
            {
                WPRINT_APP_INFO(("wwd_unpack_wl_counters_xtlv failed2 result:%d..\n", result ));
                return ERR_UNKNOWN;
            }

            counter_diff40(seconds, &wl_cnt_wlc, &wl_cnt_ge40, &wl_cnt_wlc2,
                         &wl_cnt_get40_2, &wl_cnt_results, &wl_cnt_ge40_results);
#else
            counter_diff(seconds, data, data2, data);
#endif
        }
    }

    if (flags & DUMP_RATE) {
        WPRINT_APP_INFO(("Rate counters:\n"));
#ifdef WLCMD_VER_01
        print_counters_rate(&wl_cnt_wlc, normal);
#else
        print_counters_rate(data, normal);
#endif
    }

    if (flags & DUMP_TX) {
        WPRINT_APP_INFO(("TX counters:\n"));
#ifdef WLCMD_VER_01
        print_counters_tx(&wl_cnt_ge40, &wl_cnt_wlc, normal);
#else
        print_counters_tx(data, NULL, normal);
#endif
    }

    if (flags & DUMP_RX) {
        WPRINT_APP_INFO(("RX counters:\n"));
#ifdef WLCMD_VER_01
        print_counters_rx( &wl_cnt_ge40, &wl_cnt_wlc, normal);
#else
        print_counters_rx(data, NULL, normal);
#endif
    }

    free(data);
    if (seconds)
        free(data2);

    return ERR_CMD_OK;
}

#define WL_FORMAT_INT       0x00000001
#define WL_FORMAT_UINT      0x00000002
#define WL_FORMAT_STRING    0x00000004
#define WL_FORMAT_HEX       0x00000010
#define WL_FORMAT_BUFFER    0x00000020

#define WL_FORMAT_LENGTH_SPECIFIER "-length="
#define WL_FORMAT_IFC_SPECIFIER    "-ifc="
#define WL_FORCE_IOCTL_SPECIFIER   "ioctl="

typedef struct wl_command_format
{
    uint32_t flag;
    const char* format_string;
} wl_command_format_t;

typedef struct wl_string_to_ioctl
{
    const char *command_name;
    uint16_t   get_command_value;
    uint16_t   set_command_value;
} wl_string_to_ioctl_t;

void dump_bytes(const uint8_t* bptr, uint32_t len)
{
    int i = 0;

    for (i = 0; i < len; )
    {
        if ((i & 0x0f) == 0)
        {
            WPRINT_APP_INFO( ( "\n" ) );
        }
        else if ((i & 0x07) == 0)
        {
            WPRINT_APP_INFO( (" ") );
        }
        WPRINT_APP_INFO( ( "%02x ", bptr[i++] ) );
    }
    WPRINT_APP_INFO( ( "\n" ) );
}

int wl_formatted_command_handler( int argc, char* argv[] )
{
    int err                   = ERR_CMD_OK;
    uint32_t format           = 0;
    wiced_bool_t done_parsing = WICED_FALSE;
    int cur_arg               = 1; /* start after wlf */
    const char *command_name  = NULL;
    const char *parsable_fmt  = NULL;
    char *end_ptr             = NULL;
    uint32_t   get_length     = 0;
    uint32_t   buf_length     = 0;
    int table_index           = 0;
    wwd_result_t wwd_result   = WWD_SUCCESS;
    wwd_interface_t interface = WWD_STA_INTERFACE;
    wl_string_to_ioctl_t ioctl_scratch;
    wl_string_to_ioctl_t *ioctl_entry = NULL;

    const wl_command_format_t wlf_format_specifiers[] =
    {
        {WL_FORMAT_INT,       "-i"},
        {WL_FORMAT_UINT,      "-ui"},
        {WL_FORMAT_STRING,    "-s"},
        {WL_FORMAT_HEX,       "-x"},
        {WL_FORMAT_BUFFER,    "-b"},
    };

    /* map from command string to ioctl get and/or set value */
    wl_string_to_ioctl_t ioctl_map[] =
    {
        { "up",             0,                    WLC_UP },
        { "isup",           WLC_GET_UP,           0 },
        { "down",           0,                    WLC_DOWN },
        { "out",            0,                    WLC_OUT },
        { "PM",             WLC_GET_PM,           WLC_SET_PM },
        { "roam_trigger",   WLC_GET_ROAM_TRIGGER, WLC_SET_ROAM_TRIGGER },
        { "roam_delta",     WLC_GET_ROAM_DELTA,   WLC_SET_ROAM_DELTA },
        { "phytype",        WLC_GET_PHYTYPE,      0 },
        { "scansuppress",   WLC_GET_SCANSUPPRESS, WLC_SET_SCANSUPPRESS },
        { "dtim",           WLC_GET_DTIMPRD,      WLC_SET_DTIMPRD },
        { "rate",           WLC_GET_RATE,         0 },
        { "rateset",        WLC_GET_RATESET,      WLC_SET_RATESET },
    };

    if (argc == 1)
    {
        goto error;
    }

    /* parse any format specifiers */
    while ( cur_arg < argc && done_parsing == WICED_FALSE )
    {
        wiced_bool_t match = WICED_FALSE;
        done_parsing       = WICED_TRUE;
        uint32_t flag      = 0;
        if ( strlen( argv[cur_arg] ) > strlen( WL_FORMAT_LENGTH_SPECIFIER ) &&
            0 == strncmp( argv[cur_arg], WL_FORMAT_LENGTH_SPECIFIER, strlen( WL_FORMAT_LENGTH_SPECIFIER ) ) )
        {
            parsable_fmt = argv[cur_arg];
            parsable_fmt += strlen( WL_FORMAT_LENGTH_SPECIFIER );
            get_length   = strtoul( parsable_fmt, &end_ptr, 10 );

            match        = WICED_TRUE;
        }
        else if ( strlen( argv[cur_arg] ) > strlen( WL_FORMAT_IFC_SPECIFIER ) &&
            0 == strncmp( argv[cur_arg], WL_FORMAT_IFC_SPECIFIER, strlen( WL_FORMAT_IFC_SPECIFIER ) ) )
        {
            parsable_fmt  = argv[cur_arg];
            parsable_fmt += strlen( WL_FORMAT_IFC_SPECIFIER );
            interface     = (wwd_interface_t)strtoul( parsable_fmt, &end_ptr, 10 );

            /* check for invalid */
            if ( WWD_INTERFACE_MAX <= interface )
            {
                WPRINT_APP_ERROR( ("Interface %d exceeds or eq to %d max\n", interface, WWD_INTERFACE_MAX) );
                goto error;
            }

            match = WICED_TRUE;
        }
        else
        {
            for ( table_index = 0 ; table_index < sizeof(wlf_format_specifiers)/sizeof(wlf_format_specifiers[0]) ; table_index++ )
            {
                if ( strlen( argv[cur_arg] ) == strlen( wlf_format_specifiers[table_index].format_string ) &&
                    0 == strncmp( argv[cur_arg], wlf_format_specifiers[table_index].format_string, strlen( wlf_format_specifiers[table_index].format_string ) ) )
                {
                    match = WICED_TRUE;
                    flag = wlf_format_specifiers[table_index].flag;
                    break;
                }
            }
        }

        if ( match == WICED_TRUE )
        {
            format      |= flag;
            done_parsing = WICED_FALSE;
            cur_arg++;
        }
    }

    /* remember what the command is */
    if ( cur_arg == argc )
    {
        /* there's no support for dumping out what we might be able to do in FW */
        WPRINT_APP_ERROR( ("Unsupported to dump out command list. (Compile with wl support and use wl)\n") );
        goto error;
    }
    command_name = argv[cur_arg];
    cur_arg++;

    /* length of buffer will be big enough for one uint32, but could be bigger */
    buf_length = sizeof( uint32_t );
    buf_length = MAX( buf_length, get_length );

    /* force an ioctl to execute */
    if ( 0 == strncmp( WL_FORCE_IOCTL_SPECIFIER, command_name, strlen(WL_FORCE_IOCTL_SPECIFIER) ) && strlen(command_name) > strlen(WL_FORCE_IOCTL_SPECIFIER) )
    {
        ioctl_entry = &ioctl_scratch;
        ioctl_entry->get_command_value = strtoul(command_name + strlen(WL_FORCE_IOCTL_SPECIFIER), NULL, 0);
        ioctl_entry->set_command_value = ioctl_entry->get_command_value;
    }
    else
    {
        /* is ioctl or iovar? */
        for ( table_index = 0 ; table_index < sizeof( ioctl_map )/sizeof( ioctl_map[0] ) ; table_index++ )
        {
            if ( strlen( command_name ) == strlen( ioctl_map[table_index].command_name ) && 0 == strncmp( command_name, ioctl_map[table_index].command_name, strlen( command_name ) ) )
            {
                ioctl_entry = &ioctl_map[table_index];
                break;
            }
        }
    }

    /* SET - send it down to firmware and report based on error code
       * If ioctl and no get available, or have parameters
       */
    if ( ( NULL != ioctl_entry && ioctl_entry->get_command_value == 0 ) || ( cur_arg != argc ) )
    {
        /* if a buffer is being used to do set */
        if ( ( format & WL_FORMAT_BUFFER ) != 0 )
        {
            uint32_t hex_buffer_size = MAX( buf_length, strlen( argv[cur_arg] )/2 + 1 );
            uint8_t *hex_buffer      = malloc( hex_buffer_size );

            if ( hex_buffer == NULL )
            {
                WPRINT_APP_ERROR( ("Out of heap space: size %lu\n", hex_buffer_size ) );
                err = ERR_OUT_OF_HEAP;
                goto exit;
            }

            memset( hex_buffer, 0, hex_buffer_size );

            get_length = wiced_ascii_to_hex( argv[cur_arg], hex_buffer, hex_buffer_size );

            if ( get_length != 0 )
            {
                /* partially specified data */
                if ( hex_buffer_size != get_length)
                {
                    WPRINT_APP_INFO( ("Warn: unspecified data will be passed as 0 (chars specified=%lu total length=%lu)\n", get_length, hex_buffer_size) );
                }

                if ( NULL == ioctl_entry )
                {
                    wwd_result = wwd_wifi_set_iovar_buffer( command_name, hex_buffer, buf_length, interface );

                }
                else
                {
                    wwd_result = wwd_wifi_set_ioctl_buffer( ioctl_entry->set_command_value, hex_buffer, buf_length, interface );
                }

                if ( WWD_SUCCESS != wwd_result )
                {
                    WPRINT_APP_ERROR( ("Error %d\n", wwd_result) );
                    err = ERR_UNKNOWN;
                }

            }
            else
            {
                WPRINT_APP_ERROR( ("Error xlating to hex\n") );
                err = ERR_UNKNOWN;
            }

            free( hex_buffer );
        }
        else if ( cur_arg == argc )
        {
        /* else if no args to command */

            if ( NULL == ioctl_entry )
            {
                /* parameterless set: e.g. up/down */
                wwd_result = wwd_wifi_set_iovar_void( command_name, interface );
            }
            else
            {
                wwd_result = wwd_wifi_set_ioctl_void( ioctl_entry->set_command_value, interface );
            }
        }
        else
        {
            if ( NULL == ioctl_entry )
            {
                wwd_result = wwd_wifi_set_iovar_value( command_name, strtol( argv[argc], &end_ptr, 0 ), interface );
            }
            else
            {
                wwd_result = wwd_wifi_set_ioctl_value( ioctl_entry->set_command_value, strtol( argv[argc], &end_ptr, 0 ), interface );
            }
        }

        if ( wwd_result == WWD_SUCCESS )
        {
            goto exit;
        }
        else
        {
            WPRINT_APP_ERROR( ("Error %d\n", wwd_result) );
        }
    }
    else
    {
        /* GET */
        if ( ( format & WL_FORMAT_BUFFER ) != 0 )
        {
            uint8_t* buffer_input = NULL;
            wiced_buffer_t buffer;
            wiced_buffer_t response;
            uint8_t*      data;
            uint16_t      byte_counter = 0;

            if ( NULL == ioctl_entry )
            {
                buffer_input = (uint8_t*) wwd_sdpcm_get_iovar_buffer( &buffer, buf_length, command_name );
                CHECK_IOCTL_BUFFER( buffer_input );
                wwd_result = wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, interface );
            }
            else
            {
                buffer_input = (uint8_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, buf_length );
                CHECK_IOCTL_BUFFER( buffer_input );
                wwd_result = wwd_sdpcm_send_ioctl( SDPCM_GET, ioctl_entry->get_command_value, buffer, &response, interface );
            }

            if ( WWD_SUCCESS != wwd_result )
            {
                WPRINT_APP_ERROR( ("Error %d from iovar\n", wwd_result) );
                err = ERR_UNKNOWN;
                /* response is freed already */
                goto error;
            }
            data = (uint8_t*) host_buffer_get_current_piece_data_pointer( response );

            for ( ; buf_length > 0 ; )
            {
                if ( buf_length >= 4 )
                {
                    WPRINT_APP_INFO( ("%02x %02x %02x %02x: %02d\n", (unsigned int)data[3],
                        (unsigned int)data[2], (unsigned int)data[1], (unsigned int)data[0], byte_counter) );

                    buf_length -= 4;
                    data += 4;
                }
                else
                {
                    do
                    {
                        WPRINT_APP_INFO( ("%02x ", (unsigned int)data[0]) );
                        data++;
                        buf_length--;
                    } while ( buf_length != 0 );

                    WPRINT_APP_INFO( (": %02d\n", byte_counter) );
                }
                byte_counter += 4;
            }

            host_buffer_release( response, WWD_NETWORK_RX );
        }
        else if ( ( format & WL_FORMAT_STRING ) == 0 )
        {
            /* it's a number */
            uint32_t cmd_return = 0;

            if ( NULL == ioctl_entry )
            {
                /* not expecting a string */
                wwd_result = wwd_wifi_get_iovar_value( command_name, &cmd_return, interface );
            }
            else
            {
                wwd_result = wwd_wifi_get_ioctl_value( ioctl_entry->get_command_value, &cmd_return, interface );
            }

            if ( wwd_result == WWD_SUCCESS )
            {
                /* display response buffer based on format specifier */
                if ( ( format & WL_FORMAT_INT ) != 0 )
                {
                    WPRINT_APP_INFO( ("%ld\n", (int32_t)cmd_return) );
                }
                else if ( ( format & WL_FORMAT_HEX ) != 0 )
                {
                    WPRINT_APP_INFO( ("0x%08x\n", (unsigned int)cmd_return) );
                }
                else
                {
                    WPRINT_APP_INFO( ("%lu\n", cmd_return) );
                }
            }
            else
            {
                WPRINT_APP_ERROR( ("Error %d\n", wwd_result) );
                goto error;
            }

        }
        else
        {
            /* get the string */
            wiced_buffer_t buffer;
            wiced_buffer_t response;

            CHECK_IOCTL_BUFFER( wwd_sdpcm_get_iovar_buffer( &buffer, buf_length, command_name ) );

            if ( NULL == ioctl_entry )
            {
                wwd_result = wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, interface );
            }
            else
            {
                wwd_result =  wwd_sdpcm_send_ioctl( SDPCM_GET, ioctl_entry->get_command_value, buffer, &response, interface );
            }

            if ( wwd_result == WWD_SUCCESS )
            {
                WPRINT_APP_INFO( ( "%s\n", host_buffer_get_current_piece_data_pointer( response ) ) );
                host_buffer_release( response, WWD_NETWORK_RX );
            }
            else
            {
                WPRINT_APP_ERROR( ("Error %d\n", wwd_result) );
            }
        }
    }

exit:
    return err;
error:
    WPRINT_APP_INFO( ("wlf [optional format specifiers] <command> <command parameter>\n") );
    WPRINT_APP_INFO( ("\"help\" for more info\n") );
    return ERR_UNKNOWN;
}

typedef struct wl_nan_iovar_cmds
{
    const char *command_name;
    uint16_t cmd_id;
    void *buffer;
} wl_nan_iovar_cmds_t;


int wl_nan (int argc, char* argv[] )
{

    uint8_t role = WL_NAN_ROLE_NON_MASTER_NON_SYNC;
    uint8_t hop_count = 2;
    uint8_t hop_limit = 5;
    uint8_t nan_band = NAN_BAND_B;
    uint32_t warmup_time = 120;
    wwd_nan_config_rssi_threshold_t rssi_thresh;
    wwd_nan_state_t nan_cfg_status;
    uint16_t bcn_interval = 100;
    uint16_t disc_txtime = 1000;
    uint16_t stop_beacon = NAN_BAND_B;
    uint16_t dw_len = 16;
    wwd_result_t result = WWD_SUCCESS;
    wiced_bool_t set = WICED_FALSE;
    uint8_t host_enable = WICED_TRUE;
    uint8_t merge = WICED_TRUE;
    wwd_nan_timeslot_t nan_timeslot;
    uint16_t abitmap = 128;
    wwd_nan_sd_publish_t nan_sd_publish, nan_sd_subscribe;
    wwd_nan_service_list_t nan_sd_publish_list, nan_sd_subscribe_list;
    uint8_t instance_id = 10;
    wwd_nan_sd_transmit_t nan_sd_transmit;
    wiced_bool_t start = WICED_FALSE;
    uint8_t *params = (uint8_t *) &nan_sd_publish;
    wwd_tlv_t *ptlv = NULL;

    UNUSED_PARAMETER(result);
    memset(&rssi_thresh, 0, sizeof(rssi_thresh));
    memset(&nan_cfg_status, 0, sizeof(nan_cfg_status));
    memset(&nan_timeslot, 0, sizeof(wwd_nan_timeslot_t));
    memset(&nan_sd_publish, 0, sizeof(wwd_nan_sd_publish_t));
    memset(&nan_sd_subscribe, 0, sizeof(wwd_nan_sd_publish_t));
    memset(&nan_sd_publish_list, 0, sizeof(wwd_nan_service_list_t));
    memset(&nan_sd_subscribe_list, 0, sizeof(wwd_nan_service_list_t));

    memset(&nan_sd_transmit, 0, sizeof(wwd_nan_sd_transmit_t));


    nan_sd_publish.flags = WL_NAN_PUB_BOTH | WL_NAN_RANGE_LIMITED ;
    nan_sd_publish.instance_id = instance_id;
    nan_sd_publish.length = sizeof(nan_sd_publish);
    nan_sd_publish.period = NAN_PUBLISH_PERIOD;
    memcpy(nan_sd_publish.svc_hash, WL_NAN_SVC_HASH, sizeof(nan_sd_publish.svc_hash));
    nan_sd_publish.ttl = WL_NAN_TTL_UNTIL_CANCEL;

    printf("sizeof(nan_sd_publish.nan_sd_params):%d\n", sizeof(wwd_nan_sd_publish_t));
    dump_bytes ((const uint8_t *) &nan_sd_publish, (uint32_t)sizeof(wwd_nan_sd_publish_t));

    ptlv = (wwd_tlv_t *)(params + OFFSETOF(wwd_nan_sd_publish_t, optional[0]));
    ptlv->id = 0;
    ptlv->len = 4;
    memset(ptlv->data, 0, sizeof(ptlv->data));

    nan_sd_subscribe.instance_id = instance_id;
    nan_sd_subscribe.flags = WL_NAN_PUB_BOTH | WL_NAN_RANGE_LIMITED;
    nan_sd_subscribe.instance_id = instance_id;
    nan_sd_subscribe.length = sizeof(nan_sd_subscribe);
    nan_sd_subscribe.period = NAN_SUBSCRIBE_PERIOD;
    memcpy(nan_sd_subscribe.svc_hash, WL_NAN_SVC_HASH, sizeof(nan_sd_subscribe.svc_hash));
    nan_sd_subscribe.ttl = WL_NAN_TTL_UNTIL_CANCEL;

    nan_sd_publish_list.id_count = 1;
    nan_sd_publish_list.list[0].instance_id = instance_id;
    memcpy(nan_sd_publish_list.list[0].service_hash, WL_NAN_SVC_HASH, sizeof(nan_sd_publish_list.list[0].service_hash));

    nan_sd_subscribe_list.id_count = 1;
    nan_sd_subscribe_list.list[0].instance_id = instance_id;
    memcpy(nan_sd_subscribe_list.list[0].service_hash, WL_NAN_SVC_HASH, sizeof(nan_sd_subscribe_list.list[0].service_hash));

    int i, j;

    rssi_thresh.nan_band = NAN_BAND_B;
    rssi_thresh.rssi_close = 20;
    rssi_thresh.rssi_mid = 40;

    wwd_nan_config_oui_type_t nan_config_oui = {
            .nan_oui[0] = 0x50,
            .nan_oui[1] = 0x6F,
            .nan_oui[2] = 0x9A,
            .type = 19
    };
    wwd_nan_config_count_t nan_config_count = {
            .cnt_bcn_tx = 0,
            .cnt_bcn_rx = 0,
            .cnt_svc_disc_tx = 0,
            .cnt_svc_disc_rx = 0
    };

    wwd_nan_cluster_id_t ether_addr = {
            .octet[0] = 0x50,
            .octet[1] = 0x6f,
            .octet[2] = 0x9a,
            .octet[3] = 0x01,
            .octet[4] = 0x00,
            .octet[5] = 0x00
    };
    chanspec_t chanspec = 0x1006;

    wwd_nan_sid_beacon_control_t sid_beacon = {
            .sid_enable = 1,
            .sid_count = 1,
            .pad[0] = 0,
            .pad[1] = 0
    };

    wwd_nan_election_metric_config_t elect_metrics = {
            .random_factor = 100,
            .master_pref = 3,
            .pad[0] = 0,
            .pad[1] = 0
    };

    wwd_nan_join_t join = {
            .start_cluster = WICED_FALSE,  /* start a NAN cluster  if TRUE */
            .pad[0] = 0,
            .pad[1] = 0,
            .pad[2] = 0,
            .cluster_id.octet[0] = 0x50,
            .cluster_id.octet[1] = 0x6f,
            .cluster_id.octet[2] = 0x9a,
            .cluster_id.octet[3] = 0x01,
            .cluster_id.octet[4] = 0x00,
            .cluster_id.octet[5] = 0x00
    };



    wl_nan_iovar_cmds_t nan_iovar_cmds[] = {
     {   "enable",                      WL_NAN_CMD_CFG_ENABLE,              NULL                   },
     {   "disable",                     WL_NAN_CMD_CFG_ENABLE,              NULL                   },
     {   "device_state",                WL_NAN_CMD_CFG_STATE,               &role                  },
     {   "hop_count",                   WL_NAN_CMD_CFG_HOP_CNT,             &hop_count             },
     {   "hop_limit",                   WL_NAN_CMD_CFG_HOP_LIMIT,           &hop_limit             },
     {   "warmup_time",                 WL_NAN_CMD_CFG_WARMUP_TIME,         &warmup_time           },
     {   "rssi_threshold",              WL_NAN_CMD_CFG_RSSI_THRESHOLD,      &rssi_thresh           },
     {   "nan_cfg_status",              WL_NAN_CMD_CFG_STATUS,              &nan_cfg_status        },
     {   "nan_oui",                     WL_NAN_CMD_CFG_OUI,                 &nan_config_oui        },
     {   "nan_count",                   WL_NAN_CMD_CFG_COUNT,               &nan_config_count      },
     {   "nan_clear_counters",          WL_NAN_CMD_CFG_CLEARCOUNT,          NULL                   },
     {   "nan_chanspec",                WL_NAN_CMD_CFG_CHANNEL,             &chanspec              },
     {   "nan_band",                    WL_NAN_CMD_CFG_BAND,                &nan_band              },
     {   "nan_cluster_id",              WL_NAN_CMD_CFG_CID,                 &ether_addr            },
     {   "nan_if_addr",                 WL_NAN_CMD_CFG_IF_ADDR,             &ether_addr            },
     {   "nan_bcn_interval",            WL_NAN_CMD_CFG_BCN_INTERVAL,        &bcn_interval          },
     {   "nan_svc_disc_frame_tx_time",  WL_NAN_CMD_CFG_SDF_TXTIME,          &disc_txtime           },
     {   "nan_stop_beacon_transmit",    WL_NAN_CMD_CFG_STOP_BCN_TX,         &stop_beacon           },
     {   "nan_sid_beacon",              WL_NAN_CMD_CFG_SID_BEACON,          &sid_beacon            },
     {   "nan_dwlen",                   WL_NAN_CMD_CFG_DW_LEN,              &dw_len                },
     {   "nan_hostenable",              WL_NAN_CMD_ELECTION_HOST_ENABLE,    &host_enable           },
     {   "nan_elect_metrics_config",    WL_NAN_CMD_ELECTION_METRICS_CONFIG, &elect_metrics         },
     {   "nan_elect_metrics_state",     WL_NAN_CMD_ELECTION_METRICS_STATE,  &elect_metrics         },
     {   "nan_election_join",           WL_NAN_CMD_ELECTION_JOIN,           &join                  },
     {   "nan_election_merge",          WL_NAN_CMD_ELECTION_MERGE,          &merge                 },
     {   "nan_election_stop",           WL_NAN_CMD_ELECTION_STOP,           &ether_addr            },
     {   "nan_sync_timeslot_reserve",   WL_NAN_CMD_SYNC_TSRESERVE,          &nan_timeslot          },
     {   "nan_timeslot_release",        WL_NAN_CMD_SYNC_TSRELEASE,          &abitmap               },
     {   "nan_sd_publish",              WL_NAN_CMD_SD_PUBLISH,              &nan_sd_publish        },
     {   "nan_sd_publish_list",         WL_NAN_CMD_SD_PUBLISH_LIST,         &nan_sd_publish_list   },
     {   "nan_sd_cancel_publish",       WL_NAN_CMD_SD_CANCEL_PUBLISH,       &instance_id           },
     {   "nan_sd_subscribe",            WL_NAN_CMD_SD_SUBSCRIBE,            &nan_sd_subscribe      },
     {   "nan_sd_subscribe_list",       WL_NAN_CMD_SD_SUBSCRIBE_LIST,       &nan_sd_subscribe_list },
     {   "nan_sd_cancel_subscribe",     WL_NAN_CMD_SD_CANCEL_SUBSCRIBE,     &instance_id           },
     {   "nan_sd_transmit",             WL_NAN_CMD_SD_TRANSMIT,             &nan_sd_transmit       },
     {   "scan",                        WL_NAN_CMD_SCAN,                    &join                  },

    };

    for (i = 0 ; i < sizeof(nan_iovar_cmds)/sizeof(wl_nan_iovar_cmds_t); i++ )
    {
        if ( (strcmp(argv[1], nan_iovar_cmds[i].command_name) == 0 ))
        {
            if ( ( strcmp(argv[2], "set") == 0) || ( strcmp(argv[1], "enable") == 0) )
            {
                set = WICED_TRUE;
            }

            if  ( ( strcmp (argv[2], "start") == 0 ) &&  (strcmp (argv[1], "nan_election_join") == 0) )
            {
                       start = WICED_TRUE;
            }
            result =  wl_nan_iovar ( set, nan_iovar_cmds[i].cmd_id, nan_iovar_cmds[i].buffer, start );
            break;
        }

    }

    if ( i == sizeof (nan_iovar_cmds)/sizeof(wl_nan_iovar_cmds_t) )
    {
        WPRINT_APP_INFO (( "++++++++++++++++\n "));
        WPRINT_APP_INFO (( "   NAN commands \n "));
        WPRINT_APP_INFO (( "++++++++++++++++\n "));
        for ( j = 0; j < i; j++ )
        {
            WPRINT_APP_INFO (( "%s\n", nan_iovar_cmds[j].command_name ));
        }
    }

    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO (( "command failed result:%d\n", result ));
    }
    return ERR_CMD_OK;
}

static wwd_result_t wl_nan_iovar(wiced_bool_t set, uint16_t cmd_id, void *buffer, wiced_bool_t start )
{
    wwd_result_t result = WWD_SUCCESS;
    UNUSED_PARAMETER (set);
    UNUSED_PARAMETER (cmd_id);
    UNUSED_PARAMETER (buffer);

    switch (cmd_id) {
        case WL_NAN_CMD_CFG_ENABLE :
            result = (set == WICED_TRUE ?  wiced_nan_config_enable(app_nan_event_handler) : wiced_nan_config_disable());
            break;
        case WL_NAN_CMD_CFG_STATE:
            result = wwd_nan_config_device_state (set, ( uint8_t* ) buffer);
            print_nan_device_state( *( uint8_t* )buffer );
            break;
        case WL_NAN_CMD_CFG_HOP_CNT:
            result = wwd_nan_config_hop_count(set, ( uint8_t* ) buffer);
            break;
        case WL_NAN_CMD_CFG_HOP_LIMIT:
            result = wwd_nan_config_hop_limit(set, ( uint8_t* ) buffer);
            break;
        case WL_NAN_CMD_CFG_WARMUP_TIME:
            result = wwd_nan_config_warmup_time(set, ( uint32_t* ) buffer);
            break;
        case WL_NAN_CMD_CFG_RSSI_THRESHOLD:
            result = wwd_nan_config_rssi_threshold(set, ( wwd_nan_config_rssi_threshold_t* ) buffer);
            break;
        case WL_NAN_CMD_CFG_STATUS:
            result = wwd_nan_config_get_status( ( wwd_nan_state_t* ) buffer );
            print_nan_config_status(( wwd_nan_state_t* ) buffer );
            break;
        case WL_NAN_CMD_CFG_OUI:
            result = wwd_nan_config_oui(set, ( wwd_nan_config_oui_type_t* ) buffer );
            break;
        case WL_NAN_CMD_CFG_COUNT:
            result = wwd_nan_config_get_count( (wwd_nan_config_count_t*) buffer );
            break;
        case WL_NAN_CMD_CFG_CLEARCOUNT:
            result = wwd_nan_config_clear_counters();
            break;
        case WL_NAN_CMD_CFG_CHANNEL:
            result = wwd_nan_config_set_chanspec( ( chanspec_t*) buffer );
            break;
        case WL_NAN_CMD_CFG_BAND:
            result = wwd_nan_config_band ( *( uint8_t* ) buffer );
            break;
        case WL_NAN_CMD_CFG_CID:
            result = wwd_nan_config_cluster_id( set, ( wwd_nan_cluster_id_t* ) buffer );
            print_nan_mac_address( ( wwd_nan_cluster_id_t* ) buffer );
            break;
        case WL_NAN_CMD_CFG_IF_ADDR:
            result =  wwd_nan_config_interface_address ( set, ( wwd_nan_cluster_id_t* ) buffer );
            print_nan_mac_address( ( wwd_nan_cluster_id_t* ) buffer );
            break;
        case WL_NAN_CMD_CFG_BCN_INTERVAL:
            result = wwd_nan_config_discovery_beacon_interval ( set, ( uint16_t* ) buffer );
            break;
        case WL_NAN_CMD_CFG_SDF_TXTIME:
            result = wwd_nan_config_service_discovery_frame_tx_time ( set, ( uint16_t* ) buffer );
            break;
        case WL_NAN_CMD_CFG_STOP_BCN_TX:
            result = wwd_nan_config_stop_beacon_transmit ( set, ( uint16_t *) buffer );
            break;
        case WL_NAN_CMD_CFG_SID_BEACON:
            result = wwd_nan_config_service_id_beacon ( set, ( wwd_nan_sid_beacon_control_t* ) buffer );
            break;
        case WL_NAN_CMD_CFG_DW_LEN:
            result = wwd_nan_config_discover_window_length ( set, ( uint16_t* ) buffer );
            break;
        case WL_NAN_CMD_ELECTION_HOST_ENABLE:
            result = wwd_nan_election_host_enable ( set, ( uint8_t* ) buffer );
            break;
        case WL_NAN_CMD_ELECTION_METRICS_CONFIG:
            result = wwd_nan_election_metric_config ( ( wwd_nan_election_metric_config_t* ) buffer );
            break;
        case WL_NAN_CMD_ELECTION_METRICS_STATE:
            result = wwd_nan_election_metric_state_get ( ( wwd_nan_election_metric_config_t* ) buffer );
            break;
        case WL_NAN_CMD_ELECTION_JOIN:
            {
              wwd_nan_join_t *nan_join = (wwd_nan_join_t *)buffer;
              nan_join->start_cluster = start;
              result = wwd_nan_election_join ( nan_join );
            }
            break;
        case WL_NAN_CMD_SCAN:
            result = wwd_nan_election_join ( ( wwd_nan_join_t*) buffer );
            break;

        case WL_NAN_CMD_ELECTION_MERGE:
            result = wwd_nan_election_merge ( set, ( uint8_t* ) buffer );
            break;
        case WL_NAN_CMD_ELECTION_STOP:
            result = wwd_nan_election_stop ( ( wwd_nan_cluster_id_t* ) buffer );
            break;
        case WL_NAN_CMD_SYNC_TSRESERVE:
            result = wwd_nan_sync_timeslot_reserve ( ( wwd_nan_timeslot_t* ) buffer );
            break;
        case WL_NAN_CMD_SYNC_TSRELEASE:
            result = wwd_nan_sync_timeslot_release ( ( uint32_t* ) buffer );
            break;
        case WL_NAN_CMD_SD_PUBLISH:
            result = wwd_nan_sd_publish ( set,  ( wwd_nan_sd_publish_t *) buffer);
            break;
        case WL_NAN_CMD_SD_PUBLISH_LIST:
            result = wwd_nan_sd_publish_list ( ( wwd_nan_service_list_t* ) buffer );
            break;
        case WL_NAN_CMD_SD_CANCEL_PUBLISH:
            result = wwd_nan_sd_cancel_publish ( *( uint8_t *) buffer );
            break;
        case WL_NAN_CMD_SD_SUBSCRIBE:
            result = wwd_nan_sd_subscribe ( set, ( wwd_nan_sd_subscribe_t* ) buffer );
            break;
        case WL_NAN_CMD_SD_SUBSCRIBE_LIST:
            result = wwd_nan_sd_subscribe_list ( ( wwd_nan_service_list_t* ) buffer );
            break;
        case WL_NAN_CMD_SD_CANCEL_SUBSCRIBE:
            result = wwd_nan_sd_cancel_subscribe ( *( uint8_t *) buffer );
            break;
        case WL_NAN_CMD_SD_TRANSMIT:
            result = wwd_nan_sd_transmit ( ( wwd_nan_sd_transmit_t* ) buffer );
            break;
        default:
            WPRINT_APP_INFO ((" cmd_id not found:%x\n", cmd_id ));
            break;
    }

    return result;

}

int reset_statistics_counters( int argc, char* argv[] )
{
    UNUSED_PARAMETER( argc );
    UNUSED_PARAMETER( argv );

    if (wwd_reset_statistics_counters() == WWD_SUCCESS)
    {
        return ERR_CMD_OK;
    }
    else
    {
        WPRINT_APP_INFO(("Counter reset failed.\n"));
        return ERR_UNKNOWN;
    }
}

int start_tx_phyrate_log( int argc, char* argv[] )
{
    UNUSED_PARAMETER( argc );
    UNUSED_PARAMETER( argv );

    if (wwd_phyrate_log(WICED_WIFI_PHYRATE_LOG_TX) == WWD_SUCCESS)
    {
        return ERR_CMD_OK;
    }
    else
    {
        return ERR_UNKNOWN;
    }
}

int start_rx_phyrate_log( int argc, char* argv[] )
{
    UNUSED_PARAMETER( argc );
    UNUSED_PARAMETER( argv );

    if (wwd_phyrate_log(WICED_WIFI_PHYRATE_LOG_RX) == WWD_SUCCESS)
    {
        return ERR_CMD_OK;
    }
    else
    {
        return ERR_UNKNOWN;
    }
}

int stop_phyrate_log( int argc, char* argv[] )
{
    UNUSED_PARAMETER( argc );
    UNUSED_PARAMETER( argv );

    if (wwd_phyrate_log(WICED_WIFI_PHYRATE_LOG_OFF) == WWD_SUCCESS)
    {
        return ERR_CMD_OK;
    }
    else
    {
        return ERR_UNKNOWN;
    }
}

int get_phyrate_cnts( int argc, char* argv[] )
{
    UNUSED_PARAMETER( argc );
    UNUSED_PARAMETER( argv );
    wiced_phyrate_counters_t phyrate_counters;

    if (wwd_get_phyrate_statistics_counters(&phyrate_counters, sizeof(phyrate_counters)) == WWD_SUCCESS)
    {

        WPRINT_APP_INFO(("Received frames at MCS rates 0-3 = %u, %u, %u, %u.\n",(unsigned int)phyrate_counters.rx1mbps,(unsigned int)phyrate_counters.rx2mbps,
                                                                                (unsigned int)phyrate_counters.rx5mbps5,(unsigned int)phyrate_counters.rx6mbps));
        WPRINT_APP_INFO(("Received frames at MCS rates 4-7 = %u, %u, %u, %u.\n",(unsigned int)phyrate_counters.rx9mbps,(unsigned int)phyrate_counters.rx11mbps,
                                                                                (unsigned int)phyrate_counters.rx12mbps,(unsigned int)phyrate_counters.rx18mbps));
        WPRINT_APP_INFO(("Received frames at MCS rates 8-11 = %u, %u, %u, %u.\n",(unsigned int)phyrate_counters.rx24mbps,(unsigned int)phyrate_counters.rx36mbps,
                                                                                 (unsigned int)phyrate_counters.rx48mbps,(unsigned int)phyrate_counters.rx54mbps));
        WPRINT_APP_INFO(("Received frames at MCS rates 12-15 = %u, %u, %u, %u.\n",(unsigned int)phyrate_counters.rx108mbps,(unsigned int)phyrate_counters.rx162mbps,
                                                                                  (unsigned int)phyrate_counters.rx216mbps,(unsigned int)phyrate_counters.rx270mbps));
    }
    else
    {
        return ERR_UNKNOWN;
    }
    return ERR_CMD_OK;
}



/* Get the phyrate log from a numbered start index to a stop index inclusive. */
int get_phyrate_log( int argc, char* argv[] )
{
    wiced_phyrate_log_t *data = NULL;
    float *binned_data = NULL;
    unsigned int start;
    unsigned int stop = 0;
    unsigned int log_size;
    unsigned int bin_size = 0;
    unsigned int i;
    unsigned int ibin = 0;
    float bin_total = 0.0;
    wwd_result_t result;

    if (argc < 1)
    {
        WPRINT_APP_INFO(("Usage: phyrate_dump <bin_size> .lt. %u>\n", WICED_WIFI_PHYRATE_LOG_SIZE));
        return ERR_INSUFFICENT_ARGS;
    }

    bin_size = atoi(argv[1]);

    start = 0;
    result = wwd_get_phyrate_log_size(&log_size);
    stop = log_size - 1;

    if ((start >= stop) || (stop >= WICED_WIFI_PHYRATE_LOG_SIZE))
    {
        WPRINT_APP_INFO(("phyrate_dump: log limits error:: start %u, stop %u.\n", start, stop));
        return ERR_UNKNOWN;
    }

    data = malloc_named( "phyrate_buffer", sizeof(wiced_phyrate_log_t));
    if (!data)
    {
             WPRINT_APP_INFO(("Unable to allocate data buffer!\n"));
             return ERR_UNKNOWN;
    }

    if ((bin_size == 0) || (bin_size > log_size))
    {
        WPRINT_APP_INFO(("Usage: phyrate_dump <bin_size> .lt. %u.\n", WICED_WIFI_PHYRATE_LOG_SIZE));
        WPRINT_APP_INFO(("     : bin_size = %u, bin averaging off.\n", bin_size));
        free(data);
        return ERR_UNKNOWN;
    }

    WPRINT_APP_INFO(("phyrate_dump: start = %u, stop = %u, bin size = %u.\n", start, stop, bin_size));

    binned_data = malloc_named( "avg_rate_buffer", WICED_WIFI_PHYRATE_LOG_SIZE/bin_size * sizeof(float));
    if (!binned_data)
    {
         WPRINT_APP_INFO(("Unable to allocate binned data buffer!\n"));
         free(data);
         return ERR_UNKNOWN;
    }

    result = wwd_get_phyrate_log(data);

    memset(binned_data, 0, sizeof(*binned_data));

    if (result == WWD_SUCCESS)
    {
        if(bin_size > 0)
        {
            for(i = start;  i <= stop; i++)
            {
                bin_total += data->log[i];
                if ((i % bin_size) == (bin_size - 1))
                {
                    binned_data[ibin++] = (float)bin_total/(float)bin_size;
                    bin_total = 0;
                }
            }
        }
        WPRINT_APP_INFO(("\nIndx MCS Phyrate_kb "));
        if (ibin > 0)
            WPRINT_APP_INFO(( "BIN_AVG Avg Phyrate_Mbps"));
        WPRINT_APP_INFO(("\n"));
        for(i = start;  i <= stop; i++)
        {
           unsigned int whole_avg = binned_data[i];
           unsigned int avg_phyrate_kb;
           if (i < ibin)
           {
               if (binned_data[i] == whole_avg)
                   avg_phyrate_kb = n_phyrates_kb[whole_avg];
               else
               {
                   float fract_avg = binned_data[i] - (float)whole_avg;
                   unsigned int rate_difference = n_phyrates_kb[whole_avg +1] - n_phyrates_kb[whole_avg];
                   unsigned int fract_increase = (unsigned int)(fract_avg * (float)rate_difference);
                   avg_phyrate_kb = n_phyrates_kb[whole_avg] + fract_increase;
               }
           }

           WPRINT_APP_INFO(("%04d  %02u      %5.5u", i, data->log[i], n_phyrates_kb[data->log[i]]));

           if (i < ibin)
                WPRINT_APP_INFO(("     %2.1f         %3.1f", binned_data[i], (float)avg_phyrate_kb/1000));
            WPRINT_APP_INFO(("\n"));
        }
        WPRINT_APP_INFO(("\n"));
    }

    free(data);
    free(binned_data);
    return result;
}

int get_ap_info( int argc, char* argv[] )
{
    wl_bss_info_t ap_info;
    wiced_security_t sec;

    if ( wwd_wifi_get_ap_info( &ap_info, &sec ) == WWD_SUCCESS )
    {
        WPRINT_APP_INFO( ("SSID  : %s\n", (char*)ap_info.SSID ) );
        WPRINT_APP_INFO( ("BSSID : %02X:%02X:%02X:%02X:%02X:%02X\n", ap_info.BSSID.octet[0], ap_info.BSSID.octet[1], ap_info.BSSID.octet[2], ap_info.BSSID.octet[3], ap_info.BSSID.octet[4], ap_info.BSSID.octet[5]) );
        WPRINT_APP_INFO( ("RSSI  : %d\n", ap_info.RSSI) );
        WPRINT_APP_INFO( ("SNR   : %d\n", ap_info.SNR) );
        WPRINT_APP_INFO( ("Noise : %d\n", ap_info.phy_noise) );
        WPRINT_APP_INFO( ("Beacon period : %u\n", ap_info.beacon_period) );
        WPRINT_APP_INFO( ( "Security : %s\n", ( sec == WICED_SECURITY_OPEN )           ? "Open" :
                                                ( sec == WICED_SECURITY_WEP_PSK )        ? "WEP" :
                                                ( sec == WICED_SECURITY_WPA_TKIP_PSK )   ? "WPA TKIP" :
                                                ( sec == WICED_SECURITY_WPA_AES_PSK )    ? "WPA AES" :
                                                ( sec == WICED_SECURITY_WPA2_AES_PSK )   ? "WPA2 AES" :
                                                ( sec == WICED_SECURITY_WPA2_TKIP_PSK )  ? "WPA2 TKIP" :
                                                ( sec == WICED_SECURITY_WPA2_MIXED_PSK ) ? "WPA2 Mixed" :
                                                "Unknown" ) );
    }
    else
    {
        return ERR_UNKNOWN;
    }
    return ERR_CMD_OK;
}

/*!
 ******************************************************************************
 * Leaves an associated access point
 *
 * @return  0 for success, otherwise error
 */

int leave( int argc, char* argv[] )
{
    wiced_result_t result;
    result = wiced_network_down( WICED_STA_INTERFACE );

    return result;
}

/*!
 ******************************************************************************
 * Prints the device MAC address
 *
 * @return  0 for success, otherwise error
 */

int get_mac_addr( int argc, char* argv[] )
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    wiced_mac_t mac;
    wwd_interface_t interface = WWD_STA_INTERFACE;

    memset(&mac, 0, sizeof( wiced_mac_t));

    CHECK_IOCTL_BUFFER( wwd_sdpcm_get_iovar_buffer( &buffer, sizeof(wiced_mac_t), IOVAR_STR_CUR_ETHERADDR ) );

    if (argc == 2 && argv[1][0] == '1')
    {
        interface = WWD_AP_INTERFACE;
    }

    if ( wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, interface ) == WWD_SUCCESS )
    {
        memcpy( mac.octet, host_buffer_get_current_piece_data_pointer( response ), sizeof(wiced_mac_t) );
        host_buffer_release( response, WWD_NETWORK_RX );
    }
    WPRINT_APP_INFO(("MAC address is: %02X:%02X:%02X:%02X:%02X:%02X\n", mac.octet[0], mac.octet[1], mac.octet[2], mac.octet[3], mac.octet[4], mac.octet[5]));
    return ERR_CMD_OK;
}

/*!
 ******************************************************************************
 * Enables or disables power save mode as specified by the arguments
 *
 * @return  0 for success, otherwise error
 */

int wifi_powersave( int argc, char* argv[] )
{
    int a = atoi( argv[1] );

    switch( a )
    {
        case 0:
        {
            if ( wwd_wifi_disable_powersave( ) != WWD_SUCCESS )
            {
                WPRINT_APP_INFO( ("Failed to disable Wi-Fi powersave\n") );
            }
            break;
        }

        case 1:
        {
            if ( wwd_wifi_enable_powersave( ) != WWD_SUCCESS )
            {
                WPRINT_APP_INFO( ("Failed to enable Wi-Fi powersave\n") );
            }
            break;
        }

        case 2:
        {
            uint8_t return_to_sleep_delay_ms = (uint8_t) atoi( argv[ 2 ] );

            if ( wwd_wifi_enable_powersave_with_throughput( return_to_sleep_delay_ms ) != WWD_SUCCESS )
            {
                WPRINT_APP_INFO( ("Failed to enable Wi-Fi powersave with throughput\n") );
            }
            break;
        }

        default:
            return ERR_UNKNOWN_CMD;

    }

    return ERR_CMD_OK;
}

/*!
 ******************************************************************************
 * Resumes networking after deep-sleep
 *
 * @return  0 for success, otherwise error
 */

int wifi_resume( int argc, char* argv[] )
{
    wiced_network_config_t network_config = WICED_USE_EXTERNAL_DHCP_SERVER;
    wiced_ip_setting_t* ip_settings = NULL;
    wiced_ip_setting_t static_ip_settings;

    if ( argc == 4 )
    {
        network_config = WICED_USE_STATIC_IP;
        str_to_ip( argv[1], &static_ip_settings.ip_address );
        str_to_ip( argv[2], &static_ip_settings.netmask );
        str_to_ip( argv[3], &static_ip_settings.gateway );
        ip_settings = &static_ip_settings;
    }

    if ( wiced_network_resume_after_deep_sleep( WICED_STA_INTERFACE, network_config, ip_settings ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("Failed to resume networking\n") );
    }

    return ERR_CMD_OK;
}

/*!
 ******************************************************************************
 * Sets the STA listen interval as specified by the arguments
 *
 * @return  0 for success, otherwise error
 */

int set_listen_interval( int argc, char* argv[] )
{
    int listen_interval;
    int time_unit;

    if ( argc < 3 )
    {
        return ERR_UNKNOWN_CMD;
    }

    /* No bounds checking as console app user cannot know beacon interval or DTIM interval without a sniffer */
    listen_interval = atoi( argv[1] );

    time_unit = atoi( argv[2] );
    if ( ( time_unit != WICED_LISTEN_INTERVAL_TIME_UNIT_BEACON ) && ( time_unit != WICED_LISTEN_INTERVAL_TIME_UNIT_DTIM ) )
    {
        WPRINT_APP_INFO( ("0 for units in Beacon Intervals, 1 for units in DTIM intervals\n") );
        return ERR_UNKNOWN_CMD;
    }

    if ( wiced_wifi_set_listen_interval( (uint8_t)listen_interval, (wiced_listen_interval_time_unit_t)time_unit ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("Failed to set listen interval\n") );
    }

    return ERR_CMD_OK;
}

/*!
 ******************************************************************************
 * Sets the transmit power as specified in arguments in dBm
 *
 * @return  0 for success, otherwise error
 */

int set_tx_power( int argc, char* argv[] )
{
    int dbm = atoi( argv[1] );
    return (wwd_wifi_set_tx_power(dbm) != WWD_SUCCESS );
}

/*!
 ******************************************************************************
 * Gets the current transmit power in dBm
 *
 * @return  0 for success, otherwise error
 */

int get_tx_power( int argc, char* argv[] )
{
    wwd_result_t result;
    uint8_t dbm;

    if ( WWD_SUCCESS != ( result = wwd_wifi_get_tx_power( &dbm ) ) )
    {
        return result;
    }

    WPRINT_APP_INFO(("Transmit Power : %ddBm\n", dbm ));

    return result;
}

/*!
 ******************************************************************************
 * Prints the latest RSSI value
 *
 * @return  0 for success, otherwise error
 */

int get_rssi( int argc, char* argv[] )
{
    int32_t rssi;
    wwd_wifi_get_rssi( &rssi );
    WPRINT_APP_INFO(("RSSI is %d\n", (int)rssi));
    return ERR_CMD_OK;
}

/*!
 ******************************************************************************
 * Prints the latest PHY Noise value
 *
 * @return  0 for success, otherwise error
 */

int get_noise( int argc, char* argv[] )
{
    int32_t noise;
    wwd_wifi_get_noise( &noise );
    WPRINT_APP_INFO(("NOISE is %d\r\n", (int)noise));
    return ERR_CMD_OK;
}

/*!
 ******************************************************************************
 * Returns the status of the Wi-Fi interface
 *
 * @return  0 for success, otherwise error
 */

int status( int argc, char* argv[] )
{
    wiced_status_result();
    return ERR_CMD_OK;
}

/* Select an antenna to use or automatically select */
int antenna( int argc, char* argv[] )
{
    uint32_t value;
    string_to_unsigned( argv[1], strlen(argv[1]), &value, 0);
    if ( ( value == WICED_ANTENNA_1 ) || ( value == WICED_ANTENNA_2 ) || ( value == WICED_ANTENNA_AUTO ) )
    {
        wwd_result_t result = wwd_wifi_select_antenna( (wiced_antenna_t) value );
        if ( WWD_SUCCESS == result )
        {
            return ERR_CMD_OK;
        }
        else
        {
            WPRINT_APP_ERROR( ("ERROR %d: unable to select antenna.\n", result) );
            return ERR_UNKNOWN;
        }
    }
    else
    {
        WPRINT_APP_ERROR( ("ERROR: Unknown antenna\n") );
        return ERR_BAD_ARG;
    }
}

/* Configure antenna(s) */
int ant_sel( int argc, char* argv[] )
{
    wiced_buffer_t buffer;
    uint32_t value;
    string_to_unsigned( argv[1], strlen(argv[1]), &value, 0);
    wlc_antselcfg_t* sel = (wlc_antselcfg_t*)wwd_sdpcm_get_iovar_buffer(&buffer, sizeof(wlc_antselcfg_t), "nphy_antsel");
    CHECK_IOCTL_BUFFER( sel );
    sel->ant_config[0] = value;
    sel->ant_config[1] = value;
    sel->ant_config[2] = value;
    sel->ant_config[3] = value;
    sel->num_antcfg = 0;
    if ( WWD_SUCCESS == wwd_sdpcm_send_iovar(SDPCM_SET, buffer, NULL, WWD_STA_INTERFACE) )
    {
        return ERR_CMD_OK;
    }
    else
    {
        return ERR_UNKNOWN;
    }
}

/*!
 ******************************************************************************
 * Set/Get antdiv
 *
 * @return  0 for success, otherwise error
 */
int antdiv( int argc, char* argv[] )
{
    int32_t err = 0;
    char *cmd;
    uint32_t antdiv_val = 0;
    wiced_interface_t interface;

    wiced_get_default_ready_interface(&interface);

    cmd = *argv;

    /* Skip the command name */
    argv++;

    if ( *argv )
        antdiv_val = atoi( *argv );

    if ( *argv == NULL) {

        if ( ( err = wwd_wifi_get_ioctl_value( WLC_GET_ANTDIV, &antdiv_val, interface ) ) != WWD_SUCCESS)
        {
            WPRINT_APP_INFO( ( "%s: Get IOCTL failed\n", cmd ) );
            err = ERR_INSUFFICENT_ARGS;
        }
        WPRINT_APP_INFO( ( "%d\n", (unsigned int)antdiv_val ) );

    } else {
        if ( ( err = wwd_wifi_set_ioctl_value( WLC_SET_ANTDIV, antdiv_val, interface) ) != WWD_SUCCESS )
        {
            WPRINT_APP_INFO( ( "%s: Set IOCTL failed\n", cmd ) );
            err = ERR_INSUFFICENT_ARGS;
        }
    }
    return err;
}

/*!
 ******************************************************************************
 * Set/Get Channel
 *
 * @return  0 for success, otherwise error
 */
int channel( int argc, char* argv[] )
{
    int32_t err = 0;
    char *cmd;
    uint32_t channel_val = 0;
    wiced_interface_t interface;

    wiced_get_default_ready_interface(&interface);

    cmd = *argv;

    /* Skip the command name */
    argv++;

    if ( *argv )
        channel_val = atoi( *argv );

    if ( *argv == NULL) {

        if ( ( err = wwd_wifi_get_ioctl_value( WLC_GET_CHANNEL, &channel_val, interface ) ) != WWD_SUCCESS)
        {
            WPRINT_APP_INFO( ( "%s: Get IOCTL failed\n", cmd ) );
            err = ERR_INSUFFICENT_ARGS;
        }
        WPRINT_APP_INFO( ( "%d\n", (unsigned int)channel_val ) );

    } else {
        if ( ( err = wwd_wifi_set_ioctl_value( WLC_SET_CHANNEL, channel_val, interface) ) != WWD_SUCCESS )
        {
            WPRINT_APP_INFO( ( "%s: Set IOCTL failed\n", cmd ) );
            err = ERR_INSUFFICENT_ARGS;
        }
    }
    return err;
}


int txant( int argc, char* argv[] )
{
    wiced_buffer_t buffer;
    uint32_t* data = wwd_sdpcm_get_ioctl_buffer(&buffer, sizeof(uint32_t));
    CHECK_IOCTL_BUFFER( data );
    uint32_t tmp_val;
    string_to_unsigned( argv[1], strlen(argv[1]), &tmp_val, 0 );
    *data = tmp_val;
    if (wwd_sdpcm_send_ioctl(SDPCM_SET, WLC_SET_TXANT, buffer, NULL, WWD_STA_INTERFACE) == WWD_SUCCESS)
    {
        return ERR_CMD_OK;
    }
    else
    {
        return ERR_UNKNOWN;
    }
}

int ucantdiv( int argc, char* argv[] )
{
    wiced_buffer_t buffer;
    uint32_t* data = wwd_sdpcm_get_ioctl_buffer(&buffer, sizeof(uint32_t));
    CHECK_IOCTL_BUFFER( data );
    uint32_t tmp_val;
    string_to_unsigned( argv[1], strlen(argv[1]), &tmp_val, 0 );
    *data = tmp_val;
    if (wwd_sdpcm_send_ioctl(SDPCM_SET, WLC_SET_UCANTDIV, buffer, NULL, WWD_STA_INTERFACE) == WWD_SUCCESS)
    {
        return ERR_CMD_OK;
    }
    else
    {
        return ERR_UNKNOWN;
    }
}

/*!
 ******************************************************************************
 * Get/get country from/to fw
 *
 * @return  0 for success, otherwise error
 */
int get_set_country( int argc, char* argv[] )
{
    /* Get country information and print the abbreviation */
    wwd_country_t  country = {0};
    char *ccode = NULL;

    wwd_result_t   result = ERR_TOO_MANY_ARGS;

    if ( argc == 1 )
    {
        result = wwd_wifi_get_ccode ( &country );
        if ( WWD_SUCCESS == result )
        {
            WPRINT_APP_INFO( ( "Country is (%s) \r\n", country ) );
        }
        else
        {
            WPRINT_APP_INFO( ("Cannot get Ccode! %d\n", result) );
        }

    }
    else if ( argc == 2 )
    {
       ccode = argv[1];

       /* copy two bytes country code */
       memcpy (country, ccode,  (WWD_CNTRY_BUF_SZ - 2));
       result = wwd_wifi_set_ccode ( &country );
       if ( WWD_SUCCESS == result )
       {
            WPRINT_APP_INFO( ( " Country is  (%s) \r\n",  country) );
       }
       else
       {
            WPRINT_APP_INFO( ("Cannot set Ccode! %d\n", result) );
       }

    }

    return result;
}

int get_country( int argc, char* argv[] )
{
    /* Get country information and print the abbreviation */
    wl_country_t cspec;
    wiced_buffer_t buffer;
    wiced_buffer_t response;

    wl_country_t* temp = (wl_country_t*)wwd_sdpcm_get_iovar_buffer( &buffer, sizeof( wl_country_t ), "country" );
    CHECK_IOCTL_BUFFER( temp );
    memset( temp, 0, sizeof(wl_country_t) );
    wwd_result_t result = wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, WWD_STA_INTERFACE );

    if (result == WWD_SUCCESS)
    {
        memcpy( (char *)&cspec, (char *)host_buffer_get_current_piece_data_pointer( response ), sizeof(wl_country_t) );
        host_buffer_release(response, WWD_NETWORK_RX);
        WPRINT_APP_INFO(( "Country is %s (%s/%ld)\n", cspec.country_abbrev , cspec.ccode, cspec.rev ));
    }
    else
    {
        WPRINT_APP_INFO(("country iovar not supported, trying ioctl\n"));
        temp = (wl_country_t*) wwd_sdpcm_get_ioctl_buffer( &response, sizeof(wl_country_t) );
        CHECK_IOCTL_BUFFER( temp );
        memset( temp, 0, sizeof( wl_country_t ) );
        result = wwd_sdpcm_send_ioctl( SDPCM_GET, WLC_GET_COUNTRY, buffer, &response, WWD_STA_INTERFACE );
        if ( result == WWD_SUCCESS )
        {
            memcpy( (char *)&cspec, (char *)host_buffer_get_current_piece_data_pointer( response ), sizeof(wl_country_t) );
            host_buffer_release(response, WWD_NETWORK_RX);
        }
    }

    return result;
}

int set_country( int argc, char* argv[] )
{
   wl_country_t*  country_struct;
   wl_country_t cspec = {{0}, 0, {0}};
   wiced_buffer_t buffer;
   int retval;
   /* Check argument list count */
   if ( argc > 2 )
   {
       return ERR_TOO_MANY_ARGS;
   }
   country_struct = (wl_country_t*) wwd_sdpcm_get_iovar_buffer( &buffer,
           (uint16_t) sizeof(wl_country_t), IOVAR_STR_COUNTRY );
   if ( country_struct == NULL )
   {
       wiced_assert( "Could not get buffer for IOCTL", 0 != 0 );
       return WWD_BUFFER_ALLOC_FAIL;
   }
   memset( country_struct, 0, sizeof(wl_country_t) );
   /* Parse a country specification, e.g. "US/1", or a country code.
    * cspec.rev will be -1 if not specified.
    */
   retval = parse_country_spec( argv[1], (int8_t *)cspec.country_abbrev, &cspec.rev );
   if ( retval == ERR_CMD_OK )
   {
       memcpy( country_struct->country_abbrev, cspec.country_abbrev, WLC_CNTRY_BUF_SZ );
       memcpy( country_struct->ccode, cspec.country_abbrev, WLC_CNTRY_BUF_SZ );
       country_struct->rev = cspec.rev;
       retval = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );
   }
   else
   {
       WPRINT_APP_INFO(( "%s Country is not supported by firmware\n", argv[1] ));
   }

   return retval;
}

int get_rate (int argc, char* argv[])
{
    uint32_t rate;
    wwd_wifi_get_rate(WWD_STA_INTERFACE, &rate);

    if (rate == 0)
        WPRINT_APP_INFO(("auto"));
    else if (rate > 1000) /* this indicates that units are kbps */
        WPRINT_APP_INFO(("%u Kbps", (unsigned int)rate));
    else
        WPRINT_APP_INFO(("%u%s Mbps", ((unsigned int)rate / 2), ((unsigned int)rate & 1) ? ".5" : ""));

    WPRINT_APP_INFO(("\n"));

    return ERR_CMD_OK;
}

int set_legacy_rate (int argc, char* argv[])
{
    uint32_t rate;

    rate = (uint32_t)(2 * atof( argv[1] ));

    if (WWD_SUCCESS != wwd_wifi_set_legacy_rate(WWD_STA_INTERFACE, rate))
        WPRINT_APP_INFO(("Invalid legacy rate rate specification\n"));

    return ERR_CMD_OK;
}

int disable_11n (int argc, char* argv[])
{
    if (WWD_SUCCESS != wwd_wifi_set_11n_support(WWD_STA_INTERFACE, WICED_11N_SUPPORT_DISABLED))
        WPRINT_APP_INFO(("Cannot disable 11n mode\n"));

    return ERR_CMD_OK;
}

int enable_11n (int argc, char* argv[])
{
    if (WWD_SUCCESS != wwd_wifi_set_11n_support(WWD_STA_INTERFACE, WICED_11N_SUPPORT_ENABLED))
        WPRINT_APP_INFO(("Cannot enable 11n mode\n"));

    return ERR_CMD_OK;
}

int set_mcs_rate (int argc, char* argv[])
{
    int32_t mcs;
    wiced_bool_t mcsonly = WICED_FALSE;

    mcs = (int32_t)(atoi( argv[1] ));
    if (argc == 3)
        mcsonly = (wiced_bool_t)(atoi( argv[2]));

    if (WWD_SUCCESS != wwd_wifi_set_mcs_rate(WWD_STA_INTERFACE, mcs, mcsonly))
        WPRINT_APP_INFO(("Invalid MCS rate specification\n"));

    return ERR_CMD_OK;
}

/* "dump ampdu" requires FW also to support amdpu dump, which needs a special FW
 * Enable WICED_ENABLE_AMPDU_TINY_DUMP_FW := 1 from App makefile for choosing special FW while building
 * To increase buffer size use below
 * GLOBAL_DEFINES += DUMP_COMMAND_BUFFER_SIZE=XXXX
 */
int dump( int argc, char* argv[] )
{
    wwd_result_t ret;
    char *dump_buf;
    wiced_interface_t interface;
    char dump_cmd[DUMP_COMMAND_BUFFER_SIZE] = "\0";

    if ( WICED_ERROR == wiced_get_default_ready_interface( &interface ))
        interface = WICED_STA_INTERFACE;

    dump_buf = calloc( DUMP_COMMAND_BUFFER_SIZE, sizeof(char) );
    if (dump_buf == NULL) {
        WPRINT_APP_INFO( ( "Failed to allocate dump buffer of %d bytes\n", DUMP_COMMAND_BUFFER_SIZE ) );
        return ERR_OUT_OF_HEAP;
    }

    //Skip the first string i.e. "dump"
    argv++;

    /* create the dump section name list */
    while ( *argv ) {
        /* add space delimiter if this is not the first section name */
        if ( dump_cmd[0] != '\0' )
            strncat( dump_cmd, " ", strlen(" ") );

        strncat( dump_cmd, *argv, strlen(*argv) );

        argv++;
    }

    ret = wwd_get_dump( interface, ( uint8_t * )dump_buf, DUMP_COMMAND_BUFFER_SIZE, dump_cmd, strlen( dump_cmd ) );

    if (!ret) {
        fputs(dump_buf, stdout);
    } else {
        WPRINT_APP_INFO( ( "dump failed with error : %d, Below can be the possible reasons..\n", (int16_t)ret ) );
        WPRINT_APP_INFO( ( "1.Make sure that FW supports this particular dump feature, enable WICED_ENABLE_AMPDU_TINY_DUMP_FW := 1 from App makefile\n" ) );
        WPRINT_APP_INFO( ( "2.Make sure DUMP_COMMAND_BUFFER_SIZE is enough\n" ) );
    }
    free( dump_buf );
    return ret;
}

int ampdu_clear_dump( int argc, char* argv[] )
{
    int ret;
    wiced_interface_t interface;

    wiced_get_default_ready_interface( &interface );

    ret = wwd_ampdu_clear_dump( interface );
    if ( ret ) {
        WPRINT_APP_ERROR( ( "ampdu_clear_dump failed with error : %d \n", ret ) );
        return ERR_UNKNOWN;
    }

    return ERR_CMD_OK;
}

int get_txrx_frag( int argc, char* argv[] )
{
    int ret;
    uint32_t txfrag, rxfrag;

    ret = wwd_get_txrx_frag( &txfrag, &rxfrag);
    if ( ret ) {
        WPRINT_APP_ERROR( ( "get_txrx_frag failed with error : %d \n", ret ) );
        return ERR_UNKNOWN;
    }

    WPRINT_APP_INFO( ( "txfrag:%u , rxfrag:%u\n", ( unsigned int )txfrag, ( unsigned int )rxfrag));

    return ERR_CMD_OK;
}

/*!
 ******************************************************************************
 * Gets the current band
 *
 * @return  0 for success, otherwise error
 */
int get_curr_band( int argc, char* argv[] )
{
    uint32_t band = 0;
    wiced_interface_t interface;
    wiced_get_default_ready_interface( &interface );

    if ( wwd_wifi_get_current_band( interface,  &band ) == WWD_SUCCESS )
    {
        switch ( band )
        {
            case WLC_BAND_5G:
                WPRINT_APP_INFO( ( "Current Band is 5GHz\n" ) );
                break;

            case WLC_BAND_2G:
                WPRINT_APP_INFO( ( "Current Band is 2.4GHz\n" ) );
                break;

            default:
                WPRINT_APP_INFO( ( "Current Band is unknown\n" ) );
        }
    }
    else
    {
        WPRINT_APP_ERROR( ( "Unable to read current band.\n" ) );
    }
    return ERR_CMD_OK;
}

/*!
 ******************************************************************************
 * Gets the bandwidth
 *
 * @return  0 for success, otherwise error
 */
int get_bw( int argc, char* argv[] )
{
    uint32_t bandwidth = 0;

    if ( wwd_wifi_get_bw( &bandwidth ) == WWD_SUCCESS )
    {
        WPRINT_APP_INFO( ( "BandWidth : %u\n",(unsigned int)bandwidth ) );
    }
    else
    {
        WPRINT_APP_ERROR( ( "Unable to read bandwidth.\n" ) );
    }
    return ERR_CMD_OK;
}

/*!
 ******************************************************************************
 * Gets the Wi-Fi chips PM mode
 *
 * @return  0 for success, otherwise error
 */
int get_pm_mode( int argc, char* argv[] )
{
    uint32_t pm_mode = 0;
    wiced_interface_t interface;
    wiced_get_default_ready_interface( &interface );

    if ( wwd_wifi_get_pm_mode( interface, &pm_mode ) == WWD_SUCCESS )
    {
        WPRINT_APP_INFO( ("WLAN PM mode : %u\n",(unsigned int)pm_mode ) );
    }
    else
    {
        WPRINT_APP_ERROR( ( "Unable to read WLAN PM mode.\n" ) );
    }
    return ERR_CMD_OK;
}

/*
*Sets the channel number
*
* @return  0 for success, otherwise error
*/
int set_channel( int argc, char* argv[] )
{
    uint32_t channel;
    wiced_interface_t interface;
    wiced_get_default_ready_interface(&interface);

    channel = atoi(argv[1]);

    if (wwd_wifi_set_channel( interface, channel ) != WWD_SUCCESS)
    {
        WPRINT_APP_ERROR( ("Unable to set channel.\n") );
    }
    return ERR_CMD_OK;
}

void
miniopt_init(miniopt_t *t, const char* name, const char* flags, uint8_t longflags)
{
    static const char *null_flags = "";

    memset( t, 0, sizeof( miniopt_t ) );
    t->name = name;
    if ( flags == NULL )
        t->flags = null_flags;
    else
        t->flags = flags;
    t->longflags = longflags;
}

/* ----------------------------------------------------------------------- */
int
miniopt( miniopt_t *t, char **argv )
{
    int32_t keylen;
    char *p, *eq, *valstr, *endptr = NULL;
    int32_t err = 0;

    t->consumed = 0;
    t->positional = WICED_FALSE;
    memset( t->key, 0, MINIOPT_MAXKEY );
    t->opt = '\0';
    t->valstr = NULL;
    t->good_int = WICED_FALSE;
    valstr = NULL;

    if (*argv == NULL) {
        err = -1;
        goto exit;
    }

    p = *argv++;
    t->consumed++;

    if ( !t->opt_end && !strcmp( p, "--" ) ) {
        t->opt_end = WICED_TRUE;
        if (*argv == NULL) {
            err = -1;
            goto exit;
        }
        p = *argv++;
        t->consumed++;
    }

    if ( t->opt_end ) {
        t->positional = WICED_TRUE;
        valstr = p;
    }
    else if ( !strncmp( p, "--", 2  )) {
        eq = strchr( p, '=' );
        if ( eq == NULL && !t->longflags ) {
            WPRINT_APP_ERROR( ( "%s: missing \" = \" in long param \"%s\"\n", t->name, p ) );
            err = 1;
            goto exit;
        }
        keylen = eq ? (eq - (p + 2)) : (int)strlen(p) - 2;
        if (keylen > 63) keylen = 63;
        memcpy( t->key, p + 2, keylen);

        if ( eq ) {
            valstr = eq + 1;
            if ( *valstr == '\0' ) {
                WPRINT_APP_ERROR( ( "%s: missing value after \" = \" in long param \"%s\"\n",
                        t->name, p ) );
                err = 1;
                goto exit;
            }
        }
    }
    else if ( !strncmp( p, "-", 1 ) ) {
        t->opt = p[1];
        if ( strlen( p ) > 2 ) {
            WPRINT_APP_ERROR( ( "%s: only single char options, error on param \"%s\"\n",
                t->name, p ) );
            err = 1;
            goto exit;
        }
        if ( strchr( t->flags, t->opt ) ) {
            /* this is a flag option, no value expected */
            valstr = NULL;
        } else {
            if ( *argv == NULL ) {
                WPRINT_APP_ERROR( ( "%s: missing value parameter after \"%s\"\n", t->name, p ) );
                err = 1;
                goto exit;
            }
            valstr = *argv;
            argv++;
            t->consumed++;
        }
    } else {
        t->positional = WICED_TRUE;
        valstr = p;
    }

    /* parse valstr as int just in case */
    if ( valstr ) {
        t->uval = ( uint32_t )strtoul( valstr, &endptr, 0 );
        t->val = ( int32_t )t->uval;
        t->good_int = ( *endptr == '\0' );
    }

    t->valstr = valstr;

exit:
    if ( err == 1 )
        t->opt = '?';

    return err;
}

/*
 * Format a ratespec for output of any of the wl_rate() iovars
 */
char*
rate_print( char *rate_buf, uint32_t rspec )
{
    uint32_t encode, rate, txexp, bw_val;
    const char* stbc;
    const char* ldpc;
    const char* sgi;
    const char* bw;

    encode = ( rspec & WL_RSPEC_ENCODING_MASK );
    rate = ( rspec & WL_RSPEC_RATE_MASK );
    txexp = ( rspec & WL_RSPEC_TXEXP_MASK ) >> WL_RSPEC_TXEXP_SHIFT;
    bw_val = ( rspec & WL_RSPEC_BW_MASK );
    stbc  = ( ( rspec & WL_RSPEC_STBC ) != 0 ) ? " stbc" : "";
    ldpc  = ( ( rspec & WL_RSPEC_LDPC ) != 0 ) ? " ldpc" : "";
    sgi   = ( ( rspec & WL_RSPEC_SGI )  != 0 ) ? " sgi"  : "";

    if ( bw_val == WL_RSPEC_BW_UNSPECIFIED ) {
        bw = "auto";
    } else if ( bw_val == WL_RSPEC_BW_20MHZ ) {
        bw = "20";
    } else if ( bw_val == WL_RSPEC_BW_40MHZ ) {
        bw = "40";
    } else if ( bw_val == WL_RSPEC_BW_80MHZ ) {
        bw = "80";
    } else if ( bw_val == WL_RSPEC_BW_160MHZ ) {
        bw = "160";
    } else if ( bw_val == WL_RSPEC_BW_10MHZ ) {
        bw = "10";
    } else if ( bw_val == WL_RSPEC_BW_5MHZ ) {
        bw = "5";
    } else if ( bw_val == WL_RSPEC_BW_2P5MHZ ) {
        bw = "2.5";
    } else {
        bw = "???";
    }

    if ( ( rspec & ~WL_RSPEC_TXEXP_MASK ) == 0 ) { /* Ignore TxExpansion for NULL rspec check */
        sprintf(rate_buf, "auto");
    } else if ( encode == WL_RSPEC_ENCODE_RATE ) {
        sprintf(rate_buf, "rate %d%s Mbps Tx Exp %d",
            ( unsigned int )rate/2, ( unsigned int )( rate % 2 )?".5":"", (unsigned int)txexp);
    } else if ( encode == WL_RSPEC_ENCODE_HT ) {
        sprintf( rate_buf, "ht mcs %d Tx Exp %d BW %s%s%s%s",
                ( unsigned int)rate, (unsigned int)txexp, bw, stbc, ldpc, sgi );
    } else if ( encode == WL_RSPEC_ENCODE_VHT ) {
        uint mcs = ( rspec & WL_RSPEC_VHT_MCS_MASK );
        uint Nss = ( rspec & WL_RSPEC_VHT_NSS_MASK ) >> WL_RSPEC_VHT_NSS_SHIFT;
        sprintf(rate_buf, "vht mcs %d Nss %d Tx Exp %d BW %s%s%s%s",
                ( unsigned int )mcs, ( unsigned int )Nss, ( unsigned int )txexp, bw, stbc, ldpc, sgi );
    } else {
        sprintf( rate_buf, "<unknown encoding for ratespec 0x%08X>", ( unsigned int )rspec );
    }

    return rate_buf;
}

/* parse the -v/--vht or -c argument for the wl_rate() command.
 * return FALSE if the arg does not look like MxS or cMsS, where M and S are single digits
 * return TRUE if the arg does look like MxS or cMsS, setting mcsp to M, and nssp to S
 */
static int
parse_vht_spec( const char* cp, int* mcsp, int* nssp )
{
    char *startp, *endp;
    char c;
    int32_t mcs, nss;
    char sx;

    if ( cp == NULL || cp[0] == '\0' ) {
        return WICED_FALSE;
    }

    if ( cp[0] == 'c' ) {
        startp = ( char* )cp + 1;
        sx = 's';
    }
    else {
        startp = ( char* )cp;
        sx = 'x';
    }

    mcs = ( int )strtol( startp, &endp, 10 );
    /* verify MCS 0-9, and next char is 's' or 'x' */
    if ( mcs < 0 || mcs > 9 || endp[0] != sx ) {
        return WICED_FALSE;
    }

    /* grab the char after the 'x'/'s' and convert to value */
    c = endp[1];
    nss = 0;
    if ( isdigit( ( int )c ) ) {
        nss = c - '0';
    }

    /* consume trailing space after digit */
    cp = &endp[2];
    while ( isspace( ( int )( *cp ) ) ) {
        cp++;
    }

    /* check for trailing garbage after digit */
    if ( cp[0] != '\0' ) {
        return WICED_FALSE;
    }

    if ( nss < 1 || nss > 8 ) {
        return WICED_FALSE;
    }

    *mcsp = mcs;
    *nssp = nss;

    return WICED_TRUE;
}

int
rate( int argc, char* argv[] )
{
    miniopt_t mo;
    char *option_name = NULL;
    char* endp;
    const int option_name_len = 64;
    const char* iov_name;
    const char* fn_name = "wl_rate";
    uint8_t options = WICED_FALSE;
    uint8_t auto_set = WICED_FALSE;
    uint8_t legacy_set = WICED_FALSE, ht_set = WICED_FALSE, vht_set = WICED_FALSE;
    int rate, mcs, Nss, tx_exp, bw;
    uint8_t stbc, ldpc, sgi;
    int err , opt_err;
    uint32_t rspec = 0;
    wiced_interface_t interface;
    wiced_get_default_ready_interface(&interface);

    /* set default values */
    rate = 0;
    mcs = 0;
    Nss = 0;
    tx_exp = 0;
    stbc = WICED_FALSE;
    ldpc = WICED_FALSE;
    sgi = WICED_FALSE;
    bw = 0;

    /* save the command name */
    iov_name = *argv++;

    /* process a GET */
    if (!*argv) {
        uint32_t val = 0;
        char * rate_str;
        const int rate_str_len = 64;

        if ( wwd_wifi_get_iovar_value( iov_name, &val, interface ) != WWD_SUCCESS )
        {
            WPRINT_APP_INFO( ( "%s: Get IOVAR Value failed\n", iov_name ) );
            err = ERR_INSUFFICENT_ARGS;
        }

        rate_str = malloc(rate_str_len);
        if (rate_str == NULL) {
            WPRINT_APP_ERROR( ( "%s: malloc failure for rate string buffer.\n", fn_name ) );
            return ERR_OUT_OF_HEAP;
        }
        memset(rate_str, 0, rate_str_len);

        rate_print(rate_str, val);

        WPRINT_APP_INFO( ( "%s\n", rate_str ) );

        free(rate_str);

        return 0;
    }

    /* process a SET */

    /* initialze to common error for the miniopt processing */
    err = ERR_CMD_OK;

    /* alloc option name for error messages */
    option_name = malloc(option_name_len);
    if (option_name == NULL) {
        WPRINT_APP_ERROR( ( "%s: malloc failure for option_name buffer.\n", fn_name ) );
        return ERR_OUT_OF_HEAP;
    }
    memset(option_name, 0, option_name_len);

    miniopt_init( &mo, fn_name, "lg", WICED_TRUE );
    while ( ( opt_err = miniopt( &mo, argv ) ) != -1 ) {
        if (opt_err == 1) {
            err = ERR_BAD_ARG;
            goto exit;
        }
        argv += mo.consumed;

        /* track whether or not the command line is just a single positional
         * parameter of "auto" or a legacy rate, or is using options
         */
        if ( !mo.positional ) {
            options = WICED_TRUE;    /* command line is using options */
        }
        if ( mo.positional ) {
            /* command line is using a positional parameter,
             * complain if options are also being used
             */
            if ( options ) {
                WPRINT_APP_ERROR( ( "%s: cannot mix positional args and options. "
                        "Got positional arg \"%s\" after options.\n",
                        fn_name, mo.valstr ) );
                goto exit;
            }
            /* complain if there are any more parameters since there should only
             * be one positional param, "auto" or a legacy rate.
             */
            if ( *argv != NULL ) {
                WPRINT_APP_ERROR( ( "%s: unexpected parameter \"%s\" after rate value.\n",
                        fn_name, *argv ) );
                goto exit;
            }
            /* test for "auto" to clear the rate override */
            if ( !strcmp( mo.valstr, "auto" ) ) {
                auto_set = WICED_TRUE;
            } else {
                /* pretend there was a '-r' option */
                mo.opt = 'r';
            }
        }

        /* format the option name for error messages */
        if ( mo.key[0] != '\0' ) {
            /* would like to do the following, but snprintf() is not availble in
             * all target builds. Fails in win_mfgtest_wl build.
             *
             * snprintf(option_name, option_name_len, "--%s", mo.key);
             * option_name[option_name_len - 1] = '\0';
             */
            size_t key_len;

            key_len = strlen( mo.key );
            /* limit key_len to space in str buffer minus the '--' and null */
            key_len = MIN( ( uint)(option_name_len - 3 ), key_len );

            memcpy( option_name, "--", 2 );
            memcpy( option_name + 2, mo.key, key_len );
            option_name[2 + key_len] = '\0';
        } else {
            sprintf( option_name, "-%c", mo.opt );
        }

        /* Option: -r or --rate */
        if (mo.opt == 'r' ||
            !strcmp( mo.key, "rate" ) ) {
            if ( mo.valstr == NULL ) {
                WPRINT_APP_ERROR( ( "%s: expected a value for %s option\n",
                        fn_name, option_name ) );
                goto exit;
            }

            /* special case check for "-r 5.5" */
            if ( !strcmp( mo.valstr, "5.5" ) ) {
                rate = 11;
            } else if ( !mo.good_int ) {
                WPRINT_APP_ERROR( ( "%s: could not parse \"%s\" as a value for %s option\n",
                        fn_name, mo.valstr, option_name ) );
                goto exit;
            } else {
                rate = mo.val*2;
            }

            legacy_set = WICED_TRUE;
        }
        /* Option: -h or --ht */
        if ( mo.opt == 'h' ||
            !strcmp( mo.key, "ht" ) ) {
            if (mo.valstr == NULL) {
                WPRINT_APP_ERROR( ( "%s: expected a value for %s option\n",
                        fn_name, option_name ) );
                goto exit;
            }

            if (!mo.good_int) {
                WPRINT_APP_ERROR( ( "%s: could not parse \"%s\" as a value for %s option\n",
                        fn_name, mo.valstr, option_name ) );
                goto exit;
            }

            mcs = mo.val;
            ht_set = WICED_TRUE;
        }
        /* Option: -v or --vht */
        if ( mo.opt == 'v' ||
            !strcmp(mo.key, "vht" ) ) {
            if ( mo.valstr == NULL || mo.valstr[0] == 'c' ) {
                WPRINT_APP_ERROR( ( "%s: expected a value for %s option\n",
                        fn_name, option_name ) );
                goto exit;
            }

            mcs = ( int )strtol( mo.valstr, &endp, 10 );
            if (*endp == '\0') {
                mcs = mo.val;
                vht_set = WICED_TRUE;
            } else if ( parse_vht_spec( mo.valstr, &mcs, &Nss ) ) {
                vht_set = WICED_TRUE;
            } else {
                WPRINT_APP_ERROR( ( "%s: could not parse \"%s\" as a value for %s option\n",
                        fn_name, mo.valstr, option_name ) );
                goto exit;
            }
        }
        /* Option: -c (system team's c notiation: c<MCS>s<Nss>) */
        if ( mo.opt == 'c' ) {
            if ( mo.valstr == NULL || mo.valstr[0] != 'c' ) {
                WPRINT_APP_ERROR( ( "%s: expected a value start with c for %s option\n",
                        fn_name, option_name ) );
                goto exit;
            }

            mcs = ( int )strtol( mo.valstr + 1, &endp, 10 );
            if ( *endp == '\0' ) {
                vht_set = WICED_TRUE;
            } else if ( parse_vht_spec( mo.valstr, &mcs, &Nss ) ) {
                vht_set = WICED_TRUE;
            } else {
                WPRINT_APP_ERROR( ( "%s: could not parse \"%s\" as a value for %s option\n",
                        fn_name, mo.valstr, option_name ) );
                goto exit;
            }
        }
        /* Option: -s or --ss */
        if ( mo.opt == 's' ||
            !strcmp(mo.key, "ss" ) ) {
            if ( mo.valstr == NULL ) {
                WPRINT_APP_ERROR( ( "%s: expected a value for %s option\n",
                        fn_name, option_name ) );
                goto exit;
            }

            if ( !mo.good_int ) {
                WPRINT_APP_ERROR( ( "%s: could not parse \"%s\" as a value for %s option\n",
                        fn_name, mo.valstr, option_name ) );
                goto exit;
            }
            Nss = mo.val;
        }
        /* Option: -x or --exp */
        if (mo.opt == 'x' ||
            !strcmp(mo.key, "exp")) {
            if (mo.valstr == NULL) {
                WPRINT_APP_ERROR( ( "%s: expected a value for %s option\n",
                        fn_name, option_name ) );
                goto exit;
            }

            if (!mo.good_int) {
                WPRINT_APP_ERROR( ( "%s: could not parse \"%s\" as a value for %s option\n",
                        fn_name, mo.valstr, option_name ) );
                goto exit;
            }
            tx_exp = mo.val;
            if ( tx_exp < 0 || tx_exp > 3 ) {
                WPRINT_APP_ERROR( ( "%s: tx expansion %d out of range [0-3]\n",
                        fn_name, tx_exp ) );
                err = ERR_TOO_LARGE_ARG;
                goto exit;
            }
        }
        /* Option: --stbc */
        if ( !strcmp( mo.key, "stbc" ) ) {
            if ( mo.valstr != NULL ) {
                WPRINT_APP_ERROR( ( "%s: unexpected a value for %s option\n",
                        fn_name, option_name ) );
                goto exit;
            }
            stbc = WICED_TRUE;
        }
        /* Option: -l or --ldpc */
        if ( mo.opt == 'l' ||
            !strcmp( mo.key, "ldpc" ) ) {
            if ( mo.valstr != NULL ) {
                WPRINT_APP_ERROR( ( "%s: unexpected a value for %s option\n",
                        fn_name, option_name ) );
                goto exit;
            }
            ldpc = WICED_TRUE;
        }
        /* Option: -g or --sgi */
        if ( mo.opt == 'g' ||
            !strcmp( mo.key, "sgi" ) ) {
            if (mo.valstr != NULL) {
                WPRINT_APP_ERROR( ( "%s: unexpected a value for %s option\n",
                        fn_name, option_name ) );
                goto exit;
            }
            sgi = WICED_TRUE;
        }
        /* Option: -b or --bandwidth */
        if ( mo.opt == 'b' ||
            !strcmp( mo.key, "bandwidth" ) ) {
            if (mo.valstr == NULL) {
                WPRINT_APP_ERROR( ( "%s: expected a value for %s option\n",
                        fn_name, option_name ) );
                goto exit;
            }

            if ( !mo.good_int ) {
                WPRINT_APP_ERROR( ( "%s: could not parse \"%s\" as a value for %s option\n",
                        fn_name, mo.valstr, option_name ) );
                goto exit;
            }
            if ( mo.val == 20 ) {
                bw = WL_RSPEC_BW_20MHZ;
            } else if ( mo.val == 40 ) {
                bw = WL_RSPEC_BW_40MHZ;
            } else if ( mo.val == 80 ) {
                bw = WL_RSPEC_BW_80MHZ;
            } else if ( mo.val == 160 ) {
                bw = WL_RSPEC_BW_160MHZ;
            } else {
                WPRINT_APP_ERROR( ( "%s: unexpected bandwidth specified \"%s\", "
                        "expected 20, 40, 80, or 160\n",
                        fn_name, mo.valstr ) );
                goto exit;
            }
        }
    }

    /*
     * check for necessary use of one of -r/-h/-v or auto
     */

    if (!auto_set && !legacy_set && !ht_set && !vht_set) {
        WPRINT_APP_ERROR( ( "%s: must specify one of \"auto\", legacy rate -r/--rate, "
                "HT (11n) rate -h/--ht, or VHT (11ac) rate -v/--vht\n",
                fn_name ) );
        goto exit;
    }

    /*
     * check for incompatible use of -r/-h/-v
     */

    if ( legacy_set && ht_set ) {
        WPRINT_APP_ERROR( ( "%s: cannot use legacy rate -r/--rate and "
                "HT rate -h/--ht at the same time\n",
                fn_name ) );
        goto exit;
    }

    if ( legacy_set && vht_set ) {
        WPRINT_APP_ERROR( ( "%s: cannot use legacy rate -r/--rate and "
                "HT rate -v/--vht at the same time\n",
                fn_name ) );
        goto exit;
    }

    if ( ht_set && vht_set ) {
        WPRINT_APP_ERROR( ( "%s: cannot use HT rate -h/--ht and "
                "HT rate -v/--vht at the same time\n",
                fn_name ) );
        goto exit;
    }

    /* Nss can only be used with VHT */
    if ( !vht_set && Nss != 0 ) {
        WPRINT_APP_ERROR( ( "%s: cannot use -s/--ss option with non VHT rate\n",
                fn_name ) );
        goto exit;
    }

    /* STBC, LDPC, SGI can only be used with HT/VHT rates */
    if ( ( stbc || ldpc || sgi) && !(ht_set || vht_set ) ) {
        WPRINT_APP_ERROR( ( "%s: cannot use STBC/LDPC/SGI options with non HT/VHT rates\n",
                fn_name ) );
        goto exit;
    }

    /* set the ratespec encoding type and basic rate value */
    if ( auto_set ) {
        rspec = 0;
    } else if ( legacy_set ) {
        rspec = WL_RSPEC_ENCODE_RATE;    /* 11abg */
        rspec |= rate;
    } else if ( ht_set ) {
        rspec = WL_RSPEC_ENCODE_HT;    /* 11n HT */
        rspec |= mcs;
    } else {
        rspec = WL_RSPEC_ENCODE_VHT;    /* 11ac VHT */
        if ( Nss == 0 ) {
            Nss = 1; /* default Nss = 1 if --ss option not given */
        }
        rspec |= ( Nss << WL_RSPEC_VHT_NSS_SHIFT ) | mcs;
    }

    /* set the other rspec fields */
    rspec |= ( tx_exp << WL_RSPEC_TXEXP_SHIFT );
    rspec |= bw;
    rspec |= ( stbc ? WL_RSPEC_STBC : 0 );
    rspec |= ( ldpc ? WL_RSPEC_LDPC : 0 );
    rspec |= ( sgi  ? WL_RSPEC_SGI  : 0 );

    if ( wwd_wifi_set_iovar_value( iov_name, rspec, interface ) != WWD_SUCCESS )
    {
        WPRINT_APP_INFO( ( "%s: Set IOVAR Value failed\n", iov_name ) );
        err = ERR_INSUFFICENT_ARGS;
    }

exit:
    if (option_name != NULL) {
        free(option_name);
    }

    return err;
}

/*!
 ******************************************************************************
 * Set/Get per-band bandwidth
 *
 * @return  0 for success, otherwise error
 */
int bw_cap( int argc, char* argv[] )
{
    int32_t err = 0;
    char *cmd;
    struct {
        uint32_t band;
        uint32_t bw_cap;
    } param = { 0, 0 };
    char *s = NULL;
    uint8_t buf[255];
    wiced_interface_t interface;
    wiced_get_default_ready_interface(&interface);

    cmd = *argv;

    /* Skip the command name */
    argv++;

    if ( *argv ) {
        if ( !strcmp(*argv, "a" ) || !strcmp( *argv, "5" ) || !strcmp( *argv, "5g" ) ) {
            param.band = WLC_BAND_5G;
        } else if (!strcmp( *argv, "b" ) || !strcmp( *argv, "2" ) || !strcmp( *argv, "2g" ) ) {
            param.band = WLC_BAND_2G;
        } else {
            WPRINT_APP_INFO( ( "%s: invalid band %s\n", cmd, *argv ) );
            err = ERR_BAD_ARG;
            goto exit;
        }

        argv++;

        if ( *argv ) {
            /* Optional 2nd arg is used to set the bandwidth cap */
            s = NULL;

            param.bw_cap = ( uint32_t ) strtoul( *argv, &s, 0 );
            if (s && *s != '\0') {
                WPRINT_APP_INFO( ( "%s: invalid bandwidth '%s'\n",    cmd, *argv ) );
                err = ERR_BAD_ARG;
                goto exit;
            }
        }
    } else {
        WPRINT_APP_INFO( ( "%s: band unspecified\n", cmd ) );
        err = ERR_INSUFFICENT_ARGS;
        goto exit;
    }

    if ( param.bw_cap == 0 ) {

        if ( wwd_wifi_get_iovar_buffer_with_param( "bw_cap", &param, sizeof( param ),
                           (uint8_t *)buf, sizeof( buf ), interface ) != WWD_SUCCESS)
        {
            WPRINT_APP_INFO( ( "%s: Get IOVAR failed\n", cmd ) );
            err = ERR_INSUFFICENT_ARGS;
        }
        WPRINT_APP_INFO( ( "0x%x\n", (unsigned int)buf[0] ) );

    } else {
        if ( wwd_wifi_set_iovar_buffer_with_param( "bw_cap", &param, sizeof( param ),
                           ( uint8_t * )buf, sizeof( buf ), interface ) != WWD_SUCCESS )
        {
            WPRINT_APP_INFO( ( "%s: Set IOVAR failed\n", cmd ) );
            err = ERR_INSUFFICENT_ARGS;
        }
    }

exit:
    return err;
}

/*!
 ******************************************************************************
 * Set/Get mpc
 *
 * @return  0 for success, otherwise error
 */
int mpc( int argc, char* argv[] )
{
    int32_t err = 0;
    uint32_t value = 0;
    wiced_interface_t interface;
    wiced_get_default_ready_interface(&interface);

    if ( argv[1] == NULL)
    {
        if ( wwd_wifi_get_iovar_value ( IOVAR_STR_MPC, &value, interface ) != WWD_SUCCESS )
        {
            WPRINT_APP_INFO( ( "Get IOVAR failed\n" ) );
            err = ERR_UNKNOWN;
        }
        WPRINT_APP_INFO( ( "%d\n", (unsigned int)value ) );
    }
    else
    {
        value = htod32(strtoul( argv[1], NULL, 0) );
        if(value < 3)
        {
            if ( wwd_wifi_set_iovar_value ( IOVAR_STR_MPC, value, interface ) != WWD_SUCCESS )
            {
                WPRINT_APP_INFO( ("Set IOVAR failed\n" ) );
                err = ERR_UNKNOWN;
            }
        }
        else
        {
            WPRINT_APP_INFO( ("Set IOVAR failed\n" ) );
            err = ERR_UNKNOWN;
        }
    }
    return err;
}

int set_data_rate( int argc, char* argv[] )
{
    wiced_buffer_t buffer;
    uint32_t*      data;
    wwd_result_t   result;
    uint32_t       rate;

    data = wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 4, "bg_rate" );
    CHECK_IOCTL_BUFFER( data );

    /* Set data to 2 * <rate> as legacy rate unit is in 0.5Mbps */
    rate  = (uint32_t)(2 * atof( argv[1] ));
    *data = rate;

    if ( ( result = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE ) ) != WWD_SUCCESS )
    {
        if ( result == WWD_WLAN_UNSUPPORTED )
        {
            data = wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 4, "2g_rate" );
            CHECK_IOCTL_BUFFER( data );
            *data = rate;
            result = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );
        }
    }

    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to set rate %u\n", (unsigned int)result));
        return ERR_UNKNOWN;
    }

    return ERR_CMD_OK;
}

int get_data_rate( int argc, char* argv[] )
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    uint32_t*      data;
    wwd_result_t   result;


    data = (uint32_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 4, "bg_rate" );
    CHECK_IOCTL_BUFFER( data );

    if ( ( result = wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, WWD_STA_INTERFACE ) ) != WWD_SUCCESS )
    {
        if ( result == WWD_WLAN_UNSUPPORTED )
        {
            data = (uint32_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 4, "2g_rate" );
            CHECK_IOCTL_BUFFER( data );
            result = wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, WWD_STA_INTERFACE );
        }
    }

    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to get data rate %u\n", (unsigned int)result));
        return ERR_UNKNOWN;
    }

    data = (uint32_t*) host_buffer_get_current_piece_data_pointer( response );

    *data &= 0x000000FF;

    /* 5.5 Mbps */
    if ( *data == 11 )
    {
        WPRINT_APP_INFO(( "data rate: 5.5 Mbps\n\r" ));
    }
    else
    {
        WPRINT_APP_INFO(( "data rate: %d Mbps\n\r", (int)(*data / 2) ));

    }

    host_buffer_release( response, WWD_NETWORK_RX );
    return ERR_CMD_OK;
}

/*!
 ******************************************************************************
 * Interface to the wiced_crypto_get_random() function. Prints result
 *
 * @return  0 for success, otherwise error
 */
int get_random( int argc, char* argv[] )
{
    uint8_t random[64];
    if ( wiced_crypto_get_random( random, 64 ) == WICED_SUCCESS )
    {
        int a;
        WPRINT_APP_INFO(("Random data is 0x"));
        for ( a = 0 ; a < sizeof(random) ; ++a )
        {
            WPRINT_APP_INFO(("%.2x", random[a]));
        }
        WPRINT_APP_INFO(("\n"));
        return ERR_CMD_OK;
    }

    return ERR_UNKNOWN;
}

/*!
 ******************************************************************************
 * Get the access categories being used in STA mode
 *
 * @return  0 for success
 */
int get_access_category_parameters_sta( int argc, char* argv[] )
{
    edcf_acparam_t ac_params[AC_COUNT];
    int ac_priority[AC_COUNT];

    if ( wwd_wifi_get_acparams_sta( ac_params ) != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(( "Error reading EDCF AC Parameters\n"));
    }

    wwd_wifi_prioritize_acparams( ac_params, ac_priority ); // Re-prioritize access categories to match AP configuration
    ac_params_print( ac_params, ac_priority );

    return ERR_CMD_OK;
}

/*!
 ******************************************************************************
 * Read WLAN chip console buffer and output to host console
 *
 * @return  0 for success, otherwise error
 */
int read_wlan_chip_console_log( int argc, char* argv[] )
{
    const unsigned buffer_size = 200;
    int result = ERR_UNKNOWN;

    char* buffer = malloc_named( "console", buffer_size );
    if ( buffer == NULL )
    {
        return ERR_OUT_OF_HEAP;
    }

    if ( wwd_wifi_read_wlan_log( buffer, buffer_size ) == WWD_SUCCESS )
    {
        result = ERR_CMD_OK;
    }

    free( buffer );

    return result;
}

/*!
 ******************************************************************************
 * Print WWD Stats
 *
 * @return  0 for success, otherwise error
 */
int print_wwd_stats( int argc, char* argv[] )
{
    wwd_result_t result;
    int32_t reset_stats = 0;

    if ( argc > 1 )
    {
        reset_stats = atoi( argv[1] );
    }

    result = wwd_print_stats( reset_stats );
    if ( result == WWD_DOES_NOT_EXIST )
    {
        WPRINT_APP_ERROR( ( "wwd stats(WWD_ENABLE_STATS) not enabled. \n") );
    }

    if ( result == WWD_SUCCESS )
    {
        return ERR_CMD_OK;
    }
    else
    {
        return ERR_UNKNOWN;
    }
}

int peek(int argc, char* argv[])
{
    unsigned int *addr;
    volatile unsigned int value;

    addr = (unsigned int *)((strtoul( argv[1], (char **)NULL, 16 )) & 0xFFFFFFFC);

    if (addr != NULL)
    {
        wwd_bus_set_backplane_window( (uint32_t) addr );
        memcpy ((void *)&value, (void *)addr, sizeof(value));
        WPRINT_APP_INFO(("addr 0x%08x = 0x%08x.\n", (unsigned int)addr, value));
    }
    else
    {
        WPRINT_APP_INFO(("Usage: peek <hex address>\n"));
    }

    return ERR_CMD_OK;
}

int poke(int argc, char* argv[])
{
    volatile unsigned int *addr;
    unsigned int value = 0xDEADF00D;

    addr  = (unsigned int *)((strtoul( argv[1], (char **)NULL, 16 )) & 0xFFFFFFFC);
    value = (unsigned int)   (strtoul( argv[2], (char **)NULL, 16 ));

    if (addr != NULL)
    {
        if (value != 0xDEADF00D)
            *addr = value;

        WPRINT_APP_INFO(("addr 0x%08x = 0x%08x (wrote 0x%08x).\n", (unsigned int)addr, *addr, value));
    }
    else
    {
        WPRINT_APP_INFO(("Usage: poke <hex address> <hex value>\n"));
    }

    return ERR_CMD_OK;
}

int peek_wifi(int argc, char* argv[])
{
    uint32_t addr;
    volatile uint32_t value;
    uint32_t iterations = 1;
    uint32_t loop_count = 0;

    if ( argc == 2 || argc == 3 )
    {
        /* number of register to dump */
        if ( argc == 3 )
        {
            iterations = (uint32_t)( strtoul( argv[2], (char **)NULL, 10 ) );
        }

        addr = (uint32_t)( strtoul( argv[1], (char **)NULL, 16 ) );

        for ( loop_count = 0 ; loop_count < iterations ; loop_count++, addr += 4 )
        {
            wwd_wifi_peek( addr, 4, (uint8_t*)&value );
            WPRINT_APP_INFO(("addr 0x%08x = 0x%08x.\n", (unsigned int)addr, (unsigned int)value));
        }
    }
    else
    {
        WPRINT_APP_INFO(("Usage: peek <hex address>\n"));
    }

    return ERR_CMD_OK;
}

int poke_wifi(int argc, char* argv[])
{
    volatile uint32_t addr;
    volatile uint32_t value = 0xDEADF00D;

    if ( argc == 3 )
    {
        addr  = (uint32_t)   (strtoul( argv[1], (char **)NULL, 16 ));
        value = (uint32_t)   (strtoul( argv[2], (char **)NULL, 16 ));

        wwd_wifi_poke( addr, 4, value );
        WPRINT_APP_INFO(("(writing 0x%08x).\n", (unsigned int)value));

        /* Only one access to volatile variable per statement to ensure proper access order */
        wwd_wifi_peek( addr, 4, (uint8_t*)&value );
        WPRINT_APP_INFO(("addr 0x%08x = 0x%08x\n", (unsigned int)addr, (unsigned int)value));
    }
    else
    {
        WPRINT_APP_INFO(("Usage: poke <hex address> <hex value>\n"));
    }

    return ERR_CMD_OK;
}

int get_btc_params(int argc, char* argv[])
{
    uint32_t addr;
    volatile uint32_t value;
    wwd_interface_t interface = WWD_STA_INTERFACE;

    if ( argc == 2 )
    {
        addr = (uint32_t)( strtoul( argv[1], (char **)NULL, 16 ) );
        if (argc == 2 && argv[1][0] == '1')
        {
            interface = WWD_AP_INTERFACE;
        }

        value = wwd_wifi_get_btc_params(addr, interface);
        WPRINT_APP_INFO(("BTC 0x%lx = 0x%lx\n", addr, value));

    }
    else
    {
        WPRINT_APP_INFO(("Usage: get_btc_params <hex address> [<interface 0/1>]\n"));
    }

    return ERR_CMD_OK;
}
/*!
 ******************************************************************************
 * Sets the preferred band for association. This is only useful for devices that support more than one band.
 *
 * @return  0 for success, otherwise error
 */

int set_preferred_association_band( int argc, char* argv[] )
{
    int32_t band = 0;

    band = atoi( argv[1] );

    if ( ( argc == 2 ) && ( band ) >= 0 && ( band <= WLC_BAND_2G ) )
    {
        if ( wwd_wifi_set_preferred_association_band( band ) != WWD_SUCCESS )
        {
            WPRINT_APP_INFO( ("Failed to set preferred band for association.\n") );
        }
    }
    else
    {
        return ERR_UNKNOWN;
    }

    return ERR_CMD_OK;
}

/*!
 ******************************************************************************
 * Gets the preferred band for association.
 *
 * @return  0 for success, otherwise error
 */

int get_preferred_association_band( int argc, char* argv[] )
{
    int32_t                     band = 0;

    if ( wwd_wifi_get_preferred_association_band( &band ) == WWD_SUCCESS )
    {
        switch ( band )
        {
            case WLC_BAND_AUTO:
                WPRINT_APP_INFO( ("Preferred band for association is set to auto\n") );
                break;

            case WLC_BAND_5G:
                WPRINT_APP_INFO( ("Preferred band for association is 5GHz\n") );
                break;

            case WLC_BAND_2G:
                WPRINT_APP_INFO( ("Preferred band for association is 2.4GHz\n") );
                break;

            default:
                WPRINT_APP_INFO( ("Preferred band for association is unknown\n") );
        }
    }
    else
    {
        WPRINT_APP_INFO( ("Unable to read preferred band for association. It is not set by default so try setting it before reading it back.\n") );
    }

    return ERR_CMD_OK;
}



void ac_params_print( const wiced_edcf_ac_param_t *acp, const int *priority )
{
    int aci;
    int acm, aifsn, ecwmin, ecwmax, txop;
    static const char ac_names[AC_COUNT][6] = {"AC_BE", "AC_BK", "AC_VI", "AC_VO"};

    if ( acp != NULL )
    {
        for (aci = 0; aci < AC_COUNT; aci++, acp++)
        {
            if (((acp->ACI & EDCF_ACI_MASK) >> EDCF_ACI_SHIFT) != aci)
            {
                WPRINT_APP_INFO(("Warning: AC params out of order\n"));
            }
            acm = (acp->ACI & EDCF_ACM_MASK) ? 1 : 0;
            aifsn = acp->ACI & EDCF_AIFSN_MASK;
            ecwmin = acp->ECW & EDCF_ECWMIN_MASK;
            ecwmax = (acp->ECW & EDCF_ECWMAX_MASK) >> EDCF_ECWMAX_SHIFT;
            txop = (uint16_t)acp->TXOP;
            WPRINT_APP_INFO(("%s: raw: ACI 0x%x ECW 0x%x TXOP 0x%x\n", ac_names[aci], acp->ACI, acp->ECW, txop));
            WPRINT_APP_INFO(("       dec: aci %d acm %d aifsn %d " "ecwmin %d ecwmax %d txop 0x%x\n", aci, acm, aifsn, ecwmin, ecwmax, txop) );
                /* CWmin = 2^(ECWmin) - 1 */
                /* CWmax = 2^(ECWmax) - 1 */
                /* TXOP = number of 32 us units */
            WPRINT_APP_INFO(("       eff: CWmin %d CWmax %d TXop %dusec\n", EDCF_ECW2CW(ecwmin), EDCF_ECW2CW(ecwmax), EDCF_TXOP2USEC(txop)));
        }
    }

    if ( priority != NULL )
    {
        for (aci = 0; aci < AC_COUNT; aci++, priority++)
        {
            WPRINT_APP_INFO(("%s: ACI %d Priority %d\n", ac_names[aci], aci, *priority));
        }
    }
}
#ifndef WICED_CONFIG_DISABLE_ENTERPRISE_SECURITY
/*!
 ******************************************************************************
 * Convert a security enterprise type string to an eap_type_t type.
 *
 * @param[in] arg  The string containing the value.
 *
 * @return    The value represented by the string.
 */
static eap_type_t str_to_enterprise_security_type( char* arg )
{
    if ( strcmp( arg, "eap_tls" ) == 0 )
    {
        return EAP_TYPE_TLS;
    }
    else if ( strcmp( arg, "peap" ) == 0 )
    {
        return EAP_TYPE_PEAP;
    }
    else if(strcmp( arg, "eap_ttls" ) == 0)
    {
        return EAP_TYPE_TTLS;
    }
    else if(strcmp( arg, "mschapv2" ) == 0)
    {
        return EAP_TYPE_MSCHAPV2;
    }
    else if(strcmp( arg, "cisco_leap" ) == 0)
    {
        return EAP_TYPE_LEAP;
    }
    else
    {
        WPRINT_APP_INFO( ("Bad EAP type: '%s'\r\n", arg) );
        return EAP_TYPE_NONE;
    }
}
/*!
 ******************************************************************************
 * Convert a type string to an supplicant_tunnel_auth_type_t type.
 *
 * @param[in] arg  The string containing the value.
 *
 * @return    The value represented by the string.
 */
static supplicant_tunnel_auth_type_t str_to_tunnel_authentication_type( char* arg )
{
    if ( strcmp( arg, "eap" ) == 0 )
    {
        return TUNNEL_TYPE_EAP;
    }
    else
    {
        WPRINT_APP_INFO( ("Unsupported Tunnel Authentication Type: '%s'\r\n", arg) );
        return EAP_TYPE_NONE;
    }
}
#endif /* WICED_CONFIG_DISABLE_ENTERPRISE_SECURITY */

/*!
 ******************************************************************************
 * Find AP
 *
 * @return  0 for success, otherwise error
 */

int find_ap( int argc, char* argv[] )
{
    wiced_scan_result_t ap_info;
    if ( wiced_wifi_find_ap( argv[ 1 ], &ap_info, NULL ) == WICED_SUCCESS )
    {
        print_scan_result( &ap_info );
    }
    else
    {
        WPRINT_APP_INFO( ("\"%s\" not found\n", argv[1] ) );
    }

    return ERR_CMD_OK;
}

/* Set and Get RRM Capabilities */
int set_get_rrm(int argc, char* argv[])
{
   radio_resource_management_capability_ie_t rrm_cap;
   char *endptr = NULL;
   char *s;
   /* Input number with prefix 0x */
   char c[32] = "0x";
   int ret = ERR_CMD_OK;
   uint32_t hval = 0, val = 0, len;
   uint32_t high = 0, low = 0, bit = 0, hbit = 0;
   uint32_t init_hval = 0, init_low = 0;
   char str[32];
   wiced_bool_t found = WICED_FALSE;
   uint32_t rmcap_add = 0, rmcap2_add = 0;
   uint32_t rmcap_del = 0, rmcap2_del = 0;
   radio_resource_management_capability_debug_msg_t *dbg_msg = rrm_msg;
   int i;

   memset(&rrm_cap, 0, sizeof(radio_resource_management_capability_ie_t));
   argv++;

   if( *argv == NULL )
   {
       WPRINT_APP_INFO( (" Bad Usage\n" ) );
       return ERR_UNKNOWN;
   }
   if ( strcmp( *argv, "set" ) == 0 )
   {
       wwd_wifi_get_radio_resource_management_capabilities(WWD_STA_INTERFACE, &rrm_cap);
       init_hval =  rrm_cap.radio_resource_management[4];
       init_low = (rrm_cap.radio_resource_management[3] << 24) | (rrm_cap.radio_resource_management[2] << 16) \
                       | (rrm_cap.radio_resource_management[1] << 8) | rrm_cap.radio_resource_management[0];
       argv++;
       s = (char *)*argv;

       while (*argv)
       {
           found = WICED_FALSE;
           val = strtoul(s, &endptr, 0);
           if ( *s == '+' || *s == '-' )
           {
                   s++;
           }
           else
           {
               /* used for clearing previous value */
               rmcap_del = 0;
               rmcap2_del = 0;
           }

           val = strtoul(s, &endptr, 0);
           /* Input is decimal number or hex with prefix 0x and > 32 bits */
           if ( val == 0xFFFFFFFF )
           {
               if ( !(*s == '0' && *(s+1) == 'x') )
               {
                      WPRINT_APP_INFO(("bits > 32 take only numerical input in hex\n"));
                      val = 0;
                      return ERR_UNKNOWN;
               }
               else
               {

                    len = strlen(s);
                    hval = strtoul(strncpy(str, s, len-8), &endptr, 0);
                    *endptr = 0;
                     s = s + strlen(str);
                     s = strncat(c, s, sizeof(c) - strlen(c) - 1);
                     val = strtoul(s, &endptr, 0);
                     /* Input number > 64bit */
                     if ( hval == 0xFFFFFFFF )
                     {
                           WPRINT_APP_INFO(( "Invalid entry for Radio Resource Management Capabilities\n" ));
                           return ERR_UNKNOWN;
                     }
               }
         }
         if ( *endptr != '\0' )
         {

                  for ( i = 0; ((bit = dbg_msg[i].value) <= DOT11_RRM_CAP_BSSAAD ); i++)
                  {
                         if ( strcasecmp(dbg_msg[i].string, s) == 0 )
                         {
                              found = WICED_TRUE;
                              break;
                         }
                  }
                  if ( !found )
                  {
                          for ( ; (hbit = dbg_msg[i].value) <= DOT11_RRM_CAP_AI; i++ ) {
                              if ( strcasecmp(dbg_msg[i].string, s ) == 0) {
                                  break;
                              }
                          }
                          if ( hbit )
                              hval = 1 << (hbit - DOT11_RRM_CAP_BSSAAC);
                          else
                              hval = 0;
                   }
                   else
                   {
                           val = 1 << bit;
                   }
                  if ( !val && !hval )
                       return ERR_UNKNOWN;

         }

          if ( **argv == '-' )  {
                    rmcap_del |= val;
                    if ( !found )
                         rmcap2_del |= hval;
          }
          else
          {
                   rmcap_add |= val;
                   if (!found)
                         rmcap2_add |= hval;
         }
          argv++;
       }
       /* if rrm set 0x0 then clear everything */
       if ( ( rmcap_add == 0 ) && ( rmcap2_del == 0 ) && ( rmcap2_add == 0 ) && ( rmcap2_del == 0 ) ) {
           memset ( &rrm_cap, 0, sizeof(radio_resource_management_capability_ie_t ) );
       }
       else
       {
           low = init_low & ~rmcap_del;
           high = init_hval & ~rmcap2_del;
           low =  low | rmcap_add;
           high = high | rmcap2_add;

           rrm_cap.radio_resource_management[4] = high;
           rrm_cap.radio_resource_management[3] = (low & 0xff000000) >> 24;
           rrm_cap.radio_resource_management[2] = (low & 0x00ff0000) >> 16;
           rrm_cap.radio_resource_management[1] = (low & 0x0000ff00) >> 8;
           rrm_cap.radio_resource_management[0] = low & 0x000000ff;
       }

       wwd_wifi_set_radio_resource_management_capabilities( WWD_STA_INTERFACE, &rrm_cap );
       WPRINT_APP_INFO(( "RRM CAPABILITIES: +enabled -disabled\n" ) );
       print_rrm_caps( &rrm_cap );
   }
   else if (strcmp(*argv, "get") == 0 )
   {
       WPRINT_APP_INFO(( "RRM CAPABILITIES: +enabled -disabled\n" ) );
       wwd_wifi_get_radio_resource_management_capabilities ( WWD_STA_INTERFACE, &rrm_cap );
       print_rrm_caps ( &rrm_cap );
       dump_bytes ( (uint8_t *)&rrm_cap, sizeof(radio_resource_management_capability_ie_t) );
   }
   else
   {
        ret = ERR_UNKNOWN;
   }
   return ret;
}

/* Print RRM Capabilities */
int print_rrm_caps(radio_resource_management_capability_ie_t *rrm_cap)
{
    int i, j;
    radio_resource_management_capability_debug_msg_t *dbg_msg = rrm_msg;
    if ( rrm_cap != NULL )
    {
        for ( i = 0 ; i < RRM_CAPABILITIES_LEN; i++ )
        {
            for ( j = 0; j < 8; j++ )
            {
                if ( (i * 8) + j <=  DOT11_RRM_CAP_AI )
                {
                    const char *str = dbg_msg[(i * 8) + j].string;
                    if ( strcmp(str, "unused") != 0 )
                    {
                        if ( rrm_cap->radio_resource_management[i] & (1 << j) )
                        {
                            WPRINT_APP_INFO ( ( "bit %d +%s\n", ((i * 8) + j + 1), str) );
                        }
                        else
                        {
                            WPRINT_APP_INFO ( ("bit %d -%s\n", ((i * 8) + j + 1), str) );
                        }
                    }
                }
            }
        }
    }

    return ERR_CMD_OK;
}

/* Params: [bcn mode] [da] [duration] [random int] [channel] [ssid] [repetitions] */
int rrm_bcn_req(int argc, char* argv[])
{

   radio_resource_management_beacon_req_t rrm_bcn_req;
   uint32_t len;

   memset( &rrm_bcn_req, 0, sizeof(radio_resource_management_beacon_req_t) );

   if ( argv[1] )
   {
       /* beacon mode: ACTIVE/PASSIVE/SCAN_CACHE */
       rrm_bcn_req.bcn_mode = htod32( strtoul(argv[1], NULL, 0) );
       if ( rrm_bcn_req.bcn_mode > 2 )
       {
           WPRINT_APP_INFO(("wl_rrm_bcn_req parsing bcn mode failed\n"));
               return ERR_UNKNOWN;
       }
   }
   /* destination MAC address */
   if ( argv[2] )
   {
       wifi_utils_str_to_mac((char *)argv[2], &rrm_bcn_req.da);
   }

   /* duration */
   if( argv[3] )
   {
      rrm_bcn_req.duration =  htod32(strtoul(argv[3], NULL, 0));
   }

   /* random interval */
   if ( argv[4] )
   {
      rrm_bcn_req.random_int =  htod32(strtoul(argv[4], NULL, 0));
   }

   /* channel */
   if ( argv[5] )
   {
     rrm_bcn_req.channel = htod32(strtoul(argv[5], NULL, 0));
   }

   /* SSID */
   if ( argv[6] )
   {
      len = strlen(argv[6]);
      if (len > SSID_NAME_SIZE)
      {
          WPRINT_APP_INFO ( ("SSID too long\n") );
          return ERR_UNKNOWN;
      }
      memcpy ( rrm_bcn_req.ssid.SSID, argv[6], len );
      rrm_bcn_req.ssid.SSID_len = len;
   }

    /* repetitions */
    if ( argv[7] )
    {
       rrm_bcn_req.repetitions = htod32(strtoul(argv[7], NULL, 0));
    }

    dump_bytes( (uint8_t *)&rrm_bcn_req, sizeof(radio_resource_management_beacon_req_t) );
    if( wwd_wifi_radio_resource_management_beacon_req( WWD_STA_INTERFACE, &rrm_bcn_req) != WWD_SUCCESS ) {
            WPRINT_APP_INFO((" wwd_wifi_rrm_bcn_req.. FAILED\n"));
    }

    return ERR_CMD_OK;
}

/* Params : [regulatory] [da] [duration] [random int] [channel] [repetitions] */
/* rrm_chload_req and rrm_noise_req parameters are same so consolidate to one rrm_req */
int rrm_req(int argc, char* argv[])
{
    radio_resource_management_req_t rrm_req;

    memset( &rrm_req, 0, sizeof(radio_resource_management_req_t) );

    if ( argv[1] )
    {
        /* Regulatory class */
        rrm_req.regulatory = htod32( strtoul(argv[1], NULL, 0) );
    }

    /* destination MAC Address */
    if ( argv[2] )
    {
        wifi_utils_str_to_mac( (char *)argv[2], &rrm_req.da );
    }

    /* duration */
    if ( argv[3] )
    {
        rrm_req.duration = htod32( strtoul(argv[3], NULL, 0) );
    }

    /* random interval */
    if ( argv[4] )
    {
        rrm_req.random_int = htod32( strtoul(argv[4], NULL, 0) );
    }

    /* channel */
    if (argv[5])
    {
        rrm_req.channel = htod32( strtoul(argv[5], NULL, 0) );
    }

    /* repetitions */
    if (argv[6])
    {
        rrm_req.repetitions = htod32(strtoul(argv[6], NULL, 0));
    }

    dump_bytes( (uint8_t *)&rrm_req, sizeof(radio_resource_management_req_t) );
    if( strcmp(argv[0], IOVAR_STR_RRM_CHLOAD_REQ) == 0)
    {
        if( wwd_wifi_radio_resource_management_channel_load_req( WWD_STA_INTERFACE, &rrm_req ) != WWD_SUCCESS ) {
            WPRINT_APP_INFO( (" wwd_wifi_rrm_chload_req.. FAILED\n") );
        }
    }
    else if ( strcmp(argv[0], IOVAR_STR_RRM_NOISE_REQ) == 0 )
    {
        if ( wwd_wifi_radio_resource_management_noise_req( WWD_STA_INTERFACE, &rrm_req ) != WWD_SUCCESS )
        {
                WPRINT_APP_INFO ((" wwd_wifi_rrm_noise_req.. FAILED\n"));
        }
    }
    else
    {
        WPRINT_APP_INFO((" unknown command.. FAILED\n"));
        return ERR_UNKNOWN_CMD;
    }

    return ERR_CMD_OK;
}

/* Params: [regulatory] [da] [duration] [random int] [channel] [ta] [repetitions] */
int rrm_frame_req(int argc, char* argv[])
{
    radio_resource_management_framereq_t rrm_framereq;
    memset( &rrm_framereq, 0, sizeof(radio_resource_management_framereq_t) );

    if (argv[1])
    {
        /* Regulatory class */
        rrm_framereq.regulatory = htod32( strtoul(argv[1], NULL, 0) );
    }

    /* destination address */
    if (argv[2])
    {
        wifi_utils_str_to_mac( (char *)argv[2], &rrm_framereq.da );
    }

    /* duration */
    if (argv[3])
    {
        rrm_framereq.duration = htod32( strtoul(argv[3], NULL, 0) );
    }

    /* random interval */
    if (argv[4])
    {
        rrm_framereq.random_int = htod32( strtoul(argv[4], NULL, 0) );
    }

    /* channel */
    if (argv[5])
    {
        rrm_framereq.channel = htod32( strtoul(argv[5], NULL, 0) );
    }

    /* transmit address */
    if (argv[6])
    {
        wifi_utils_str_to_mac( argv[6], &rrm_framereq.ta );
    }

    /* repetitions */
    if (argv[7])
    {
        rrm_framereq.repetitions = htod32( strtoul(argv[7], NULL, 0) );
    }

    dump_bytes( (uint8_t *)&rrm_framereq, sizeof(radio_resource_management_framereq_t) );

    if( wwd_wifi_radio_resource_management_frame_req( WWD_STA_INTERFACE, &rrm_framereq ) != WWD_SUCCESS )
    {
        WPRINT_APP_INFO((" rrm_frame_req.. FAILED\n"));
    }
    return ERR_CMD_OK;
}

/* Params: [da] [random int] [duration] [peer] [group id] [repetitions] */
int rrm_stat_req(int argc, char* argv[])
{

    radio_resource_management_statreq_t rrm_statreq;

    memset( &rrm_statreq, 0, sizeof(radio_resource_management_statreq_t) );

    if ( argv[1] )
    {
        /* destination MAC Address */
        wifi_utils_str_to_mac( argv[1], &rrm_statreq.da );
    }

    /* random interval */
    if ( argv[2] )
    {
        rrm_statreq.random_int = htod32( strtoul(argv[2], NULL, 0) );
    }

    /* duration */
    if ( argv[3] )
    {
        rrm_statreq.duration = htod32( strtoul(argv[3], NULL, 0) );
    }

    /* peer address */
    if ( argv[4] )
    {
        wifi_utils_str_to_mac( argv[4], &rrm_statreq.peer );
    }

    /* group id */
    if (argv[5])
    {
        rrm_statreq.group_id = htod32( strtoul(argv[5], NULL, 0) );
    }

    /* repetitions */
    if ( argv[6] )
    {
        rrm_statreq.repetitions = htod32( strtoul(argv[6], NULL, 0) );
    }

    dump_bytes((uint8_t *)&rrm_statreq, sizeof(radio_resource_management_statreq_t));

    if( wwd_wifi_radio_resource_management_stat_req( WWD_STA_INTERFACE, &rrm_statreq ) != WWD_SUCCESS )
    {
        WPRINT_APP_INFO((" wwd_wifi_rrm_stat_req.. FAILED\n"));
    }

    return ERR_CMD_OK;
}

/* get 11k neighbor report list  supported only in AP mode */
int rrm_nbr_list(int argc, char* argv[])
{
    uint8_t buffer[WLC_IOCTL_SMLEN];
    uint16_t buflen = 4;
    uint8_t *ptr;
    int i;
    radio_resource_management_nbr_element_t *nbr_elt;

    memset( buffer, 0, WLC_IOCTL_SMLEN );

    if( wwd_wifi_radio_resource_management_neighbor_list( WWD_AP_INTERFACE, buffer, buflen) != WWD_SUCCESS )
    {
        WPRINT_APP_INFO((" wwd_wifi_rrm_nbr_list.. FAILED\n"));
    }
    buflen = *(uint16_t *)( buffer + strlen(IOVAR_STR_RRM_NBR_LIST) + 1 );
    if ( buflen == 0 )
    {
        WPRINT_APP_INFO(("RRM Neighbor Report List: Buffer EMPTY\n"));
        return ERR_UNKNOWN;
    }

    WPRINT_APP_INFO(("RRM Neighbor Report List: buflen:%d data:%02x %02x %02x %02x\n",
            buflen, buffer[0], buffer[1], buffer[2], buffer[3]));

    if( wwd_wifi_radio_resource_management_neighbor_list(WWD_AP_INTERFACE, buffer, buflen) != WWD_SUCCESS )
    {
        WPRINT_APP_INFO((" wwd_wifi_rrm_nbr_list list_cnt.. FAILED\n"));
        return ERR_UNKNOWN;
    }

    ptr = buffer;

    for (i = 0; i < buflen; i++)
    {
        nbr_elt = (radio_resource_management_nbr_element_t *)ptr;
        WPRINT_APP_INFO(("AP %2d: ", i + 1));
        WPRINT_APP_INFO(("bssid %02x:%02x:%02x:%02x:%02x:%02x ", nbr_elt->bssid.octet[0],
               nbr_elt->bssid.octet[1], nbr_elt->bssid.octet[2], nbr_elt->bssid.octet[3],
               nbr_elt->bssid.octet[4], nbr_elt->bssid.octet[5]));

        WPRINT_APP_INFO(("bssid_info %08x ",(uint) &nbr_elt->bssid_info));
        WPRINT_APP_INFO(("reg %2d channel %3d phytype %d\n", nbr_elt->regulatory,
               nbr_elt->channel, nbr_elt->phytype));

        ptr += TLV_HDR_LEN + DOT11_NEIGHBOR_REP_IE_FIXED_LEN;
    }

    return ERR_CMD_OK;
}


/* delete node from 11k neighbor report list supported only in AP mode */
/* Params: [bssid] */
int rrm_nbr_del_nbr(int argc, char* argv[])
{
    wiced_mac_t ea;

    if ( *++argv == NULL )
    {
         WPRINT_APP_INFO(("no BSSID specified\n"));
         return ERR_UNKNOWN;
    }

    /* bssid */
    if ( argv[1] )
    {
        wifi_utils_str_to_mac( argv[1], &ea );
    }

    if ( wwd_wifi_radio_resource_management_neighbor_del_neighbor(WWD_AP_INTERFACE, &ea) != WWD_SUCCESS )
    {
        dump_bytes((uint8_t *)&ea, sizeof(wiced_mac_t));
        WPRINT_APP_INFO((" wwd_wifi_rrm_nbr_del_nbr.. FAILED\n"));
    }

    return ERR_CMD_OK;
}

/* add node to 11k neighbor report list  supported only in AP mode */
/* Params: [bssid] [bssid info] [regulatory] [channel] [phytype] */
int rrm_nbr_add_nbr(int argc, char* argv[])
{
    uint16_t buflen = TLV_HDR_LEN + DOT11_NEIGHBOR_REP_IE_FIXED_LEN;
    radio_resource_management_nbr_element_t nbr_elt;

    memset( &nbr_elt, 0, sizeof(radio_resource_management_nbr_element_t) );

    for (argc = 0; argv[argc]; argc++);

    if ( argc != 6 )
    {
        WPRINT_APP_INFO((" Too many arguments.. FAILED\n"));
        return ERR_UNKNOWN;
    }

    /* BSSID */
    if( argv[1] )
    {
         wifi_utils_str_to_mac(argv[1], &nbr_elt.bssid);
    }

    /* BSSID info */
    nbr_elt.bssid_info = htod32( strtoul(argv[2], NULL, 0) );

    /* Regulatory class */
    nbr_elt.regulatory = htod32( strtoul(argv[3], NULL, 0) );

    /* channel */
    nbr_elt.channel = htod32( strtoul(argv[4], NULL, 0) );

    /* PHY Type */
    nbr_elt.phytype = htod32( strtoul(argv[5], NULL, 0) );

    nbr_elt.id = DOT11_MNG_NEIGHBOR_REP_ID;
    nbr_elt.length = DOT11_NEIGHBOR_REP_IE_FIXED_LEN;

    if( wwd_wifi_radio_resource_management_neighbor_add_neighbor( WWD_AP_INTERFACE, &nbr_elt, buflen ) != WWD_SUCCESS )
    {
        WPRINT_APP_INFO((" wwd_wifi_rrm_nbr_add_nbr.. FAILED\n"));
        return ERR_UNKNOWN;
    }
    return ERR_CMD_OK;
}

/* Send Link Management Request */
int rrm_lm_req(int argc, char* argv[])
{
   wiced_mac_t da;

   memset( &da, 0, sizeof(da) );
   /* BSSID of destination */
   wifi_utils_str_to_mac((char *)argv[1], &da );

   if( wwd_wifi_radio_resource_management_link_management_req(WWD_STA_INTERFACE, &da) != WWD_SUCCESS )
   {
       WPRINT_APP_INFO((" wwd_wifi_rrm_lm_req.. FAILED\n"));
   }
   dump_bytes( (uint8_t *)&da, sizeof(wiced_mac_t) );

   return ERR_CMD_OK;
}
/* Send RRM Neighbor Request */
int rrm_nbr_req(int argc, char* argv[])
{
   wiced_ssid_t ssid;
   memset( &ssid, 0, sizeof(wiced_ssid_t) );
   ssid.length = (uint8_t) (strlen((const char *)&argv[2]) + 1 );

   memcpy( ssid.value, &argv[2], ssid.length );

   if ( wwd_wifi_radio_resource_management_neighbor_req(WWD_STA_INTERFACE, &ssid) != WWD_SUCCESS )
   {
       WPRINT_APP_INFO((" wwd_wifi_rrm_nbr_req.. FAILED\n"));
       return ERR_UNKNOWN;
   }
    return ERR_CMD_OK;
}

/* get/set beacon throttle window (milli-seconds) in which off-channel time is computed */
int rrm_bcn_req_thrtl_win (int argc, char* argv[] )
{
    uint32_t value = 0;

    if(argv[1] != NULL )
    {
        if( strcasecmp (argv[1], "set" ) == 0 )
        {
           value = htod32(strtoul( argv[2], NULL, 0) );

           if ( wwd_wifi_set_iovar_value ( IOVAR_STR_RRM_BCNREQ_THRTL_WIN, value, WWD_STA_INTERFACE ) != WWD_SUCCESS )
           {
               WPRINT_APP_INFO((" set beacon throttle window..Failed:\n") );
           }
        }
        else if( strcasecmp ( argv[1], "get" ) == 0 )
        {
            if ( wwd_wifi_get_iovar_value ( IOVAR_STR_RRM_BCNREQ_THRTL_WIN, &value, WWD_STA_INTERFACE ) != WWD_SUCCESS )
            {
               WPRINT_APP_INFO((" Get beacon throttle window..Failed:\n") );
            }
            else
            {
               WPRINT_APP_INFO((" RRM Beacon Req Throttle Window :%dms \n", (int)value ));

            }
        }
    }
    else
    {
        return ERR_UNKNOWN;
    }

    return ERR_CMD_OK;
}

/* get/set maximum scan time allowed in beacon throttle window (milli-seconds) */
int rrm_bcn_req_max_off_chan_time (int argc, char* argv[] )
{
    uint32_t value = 0;

    if(argv[1] != NULL )
    {
        if( strcasecmp (argv[1], "set" ) == 0 )
        {
           value = htod32(strtoul( argv[2], NULL, 0) );

           if ( wwd_wifi_set_iovar_value ( IOVAR_STR_RRM_BCNREQ_MAXOFF_TIME, value, WWD_STA_INTERFACE ) != WWD_SUCCESS )
           {
               WPRINT_APP_INFO((" set scan time in beacon throttle window..Failed: \n" ) );
           }
        }
        else if( strcasecmp ( argv[1], "get" ) == 0 )
        {
            if ( wwd_wifi_get_iovar_value ( IOVAR_STR_RRM_BCNREQ_MAXOFF_TIME, &value, WWD_STA_INTERFACE ) != WWD_SUCCESS )
            {
               WPRINT_APP_INFO((" get scan time in beacon throttle window..Failed:\n") );
            }
            else
            {
               WPRINT_APP_INFO((" RRM scan time :%dms \n", (int)value ));

            }
        }
    }
    else
    {
        return ERR_UNKNOWN;
    }

    return ERR_CMD_OK;
}

/* get/set milli-second period to check traffic */
int rrm_bcn_req_traff_meas_per (int argc, char* argv[] )
{
    uint32_t value = 0;
    wwd_result_t result = WWD_SUCCESS;

    if(argv[1] != NULL )
    {
        if( strcasecmp (argv[1], "set" ) == 0 )
        {
           value = htod32(strtoul( argv[2], NULL, 0) );

           if  ( ( result = wwd_wifi_set_iovar_value ( IOVAR_STR_RRM_BCNREQ_TRFMS_PRD, value, WWD_STA_INTERFACE )) != WWD_SUCCESS )
           {
               WPRINT_APP_INFO((" set Traffic measurement period..Failed::%d\n", result) );
           }
        }
        else if( strcasecmp ( argv[1], "get" ) == 0 )
        {
            if ( wwd_wifi_get_iovar_value ( IOVAR_STR_RRM_BCNREQ_TRFMS_PRD, &value, WWD_STA_INTERFACE ) != WWD_SUCCESS )
            {
               WPRINT_APP_INFO((" get Traffic measurement period .Failed:\n") );
            }
            else
            {
               WPRINT_APP_INFO((" RRM Traffic Measurement period :%dms \n", (int)value ));

            }
        }
    }
    else
    {
        return ERR_UNKNOWN;
    }

    return ERR_CMD_OK;
}




/* parse hex string into buffer */
static int hex_string_to_uint8(const char *hex_string, uint8_t **output_buffer_return, uint16_t *output_buffer_length_out)
{
    const char *iterator         = hex_string;
    size_t output_buffer_length  = strlen(hex_string)/2 + 1;
    wiced_bool_t odd_size_buffer = ( output_buffer_length%2 != 0 ) ? WICED_TRUE : WICED_FALSE;
    uint8_t *output_buffer       = malloc_named( "console", output_buffer_length );
    int   output_buffer_index    = 0;
    /* for strtoul */
    char *end = NULL;


    if ( output_buffer == NULL )
    {
        *output_buffer_return = NULL;
        return ERR_OUT_OF_HEAP;
    }

    memset( output_buffer, 0, output_buffer_length );

    /* skip prefix */
    if ( (*iterator == '0') && tolower((int)iterator[1]) == 'x' )
    {
        iterator += 2;
    }

    if ( *iterator == '\0' )
    {
        /* value is 0 */
        *output_buffer_return     = output_buffer;
        *output_buffer_length_out = 1;
        return ERR_CMD_OK;
    }

    /* handle odd size here, so the loop below is cleaner (don't force user to prepend 0's) */
    if ( odd_size_buffer == WICED_TRUE )
    {
        char short_buffer[2] = { 0 };
        short_buffer[0] = *iterator;
        iterator++;
        output_buffer[output_buffer_index++] = strtoul(short_buffer, &end, 16);
    }

    while ( *iterator != '\0' )
    {
        char buffer[3] = { 0 };

        buffer[0] = *iterator;
        iterator++;
        buffer[1] = *iterator;
        iterator++;

        output_buffer[output_buffer_index++] = strtoul(buffer, &end, 16);
    }

    *output_buffer_return     = output_buffer;

    if ( NULL != output_buffer_length_out )
    {
        *output_buffer_length_out = output_buffer_index;
    }

    return ERR_CMD_OK;
}

static void wifi_ds1_callback( void *user_parameter )
{
    /* stop the wait loop */
    *((volatile int*)user_parameter) = 1;
}

static int ds1_parse_debug_options(int argc, char* argv[], wiced_bool_t *read_back_mode, wiced_ds1_debug_t  *overrides)
{
    int arg_idx     = 0;
    int found_opts  = 0;
    const char *wowl_os_opt   = "-wowl_os=";
    const char *wowl_opt      = "-wowl=";
    const char *read_back_opt = "-read_back";

    /* Match arguments with debug options */
    for ( ; arg_idx < argc ; arg_idx++ )
    {

        found_opts++;
        if ( 0 == strncmp( argv[arg_idx], wowl_os_opt, strlen( wowl_os_opt ) ) )
        {
            overrides->wowl_os       = strtoul( argv[arg_idx] + strlen( wowl_os_opt ), NULL, 16 );
            overrides->wowl_os_valid = WICED_TRUE;
        }
        else if ( 0 == strncmp( argv[arg_idx], wowl_opt, strlen( wowl_opt ) ) )
        {
            overrides->wowl       = strtoul( argv[arg_idx] + strlen( wowl_opt ), NULL, 16 );
            overrides->wowl_valid = WICED_TRUE;
        }
        else if ( 0 == strncmp( argv[arg_idx], read_back_opt, strlen( read_back_opt ) ) )
        {
            *read_back_mode = WICED_TRUE;
        }
        else
        {
            found_opts--;
        }
    }

    return found_opts;
}

int ds1_enter(int argc, char* argv[])
{
    wiced_bool_t read_back_mode     = WICED_FALSE;
    int err                         = ERR_CMD_OK;
    wiced_result_t result           = WICED_ERROR;
    const char *sub_cmd_name        = argv[1];
    uint8_t *buffer                 = NULL;
    uint16_t data_length            = 0;
    char *end                       = NULL;
    uint16_t ulp_milliseconds       = 0;
    volatile int ds1_entry_complete = 0;
    int wait_loop_delay             = 0;

    wiced_ds1_debug_t overrides     = { 0 };
    wwd_result_t      wwd_result    = WWD_SUCCESS;

    wiced_offload_value_t offload_array[1];
    wiced_offload_value_t *offload = &offload_array[0]; /* to make coverity happy */

    memset( offload_array, 0, sizeof( offload_array ) );

    result = wiced_wifi_ds1_set_complete_callback( (wiced_wifi_ds1_complete_callback_t)wifi_ds1_callback, (void*)&ds1_entry_complete );

    if ( WICED_SUCCESS != result )
    {
        WPRINT_APP_INFO( ("Error %d setting callback for ds enter\n", result) );
        err = ERR_UNKNOWN;
        goto exit;
    }

    /* parse any commands done to change how ds entry is done; reduce unparsed commands by that amount */
    argc -= ds1_parse_debug_options( argc, argv, &read_back_mode, &overrides );

    /* read back and print out values after they are changed in firmware if this is true */
    wwd_result = wwd_wifi_set_fw_cmd_debug_mode( read_back_mode );

    if ( WWD_SUCCESS != wwd_result )
    {
        WPRINT_APP_INFO( ("Error %d setting fw cmd debug mode\n", wwd_result) );
        err = ERR_UNKNOWN;
        goto exit;
    }

    ulp_milliseconds = strtoul( argv[2], &end, 10 );

    if ( ulp_milliseconds < WICED_WIFI_ULP_MIN_MILLISECONDS )
    {
        WPRINT_APP_ERROR( ("ERROR: Wait period must be at least %d\n", WICED_WIFI_ULP_MIN_MILLISECONDS) );
        err = ERR_BAD_ARG;
        goto exit;
    }

    if ( strlen(sub_cmd_name) == strlen("magic") &&
        strncmp(sub_cmd_name, "magic", strlen(sub_cmd_name) ) == 0 )
    {
        result = wiced_wifi_enter_ds1_debug( WICED_STA_INTERFACE, WICED_OFFLOAD_MAGIC, offload_array, ulp_milliseconds, &overrides );
        goto exit;
    }

    if ( argc == 3 )
    {
        /* only magic packet (above) can get by without more args */
        err = ERR_INSUFFICENT_ARGS;
        goto exit;
    }

    if ( strlen(sub_cmd_name) == strlen("keep_alive") &&
        strncmp(sub_cmd_name, "keep_alive", strlen(sub_cmd_name) ) == 0 )
    {
        /* Example: ds1_enter keep_alive <ulp wait: ex. 8> <period msecs: ex. 20> <packet data: ex. 0x3243567abcdef> */
        if ( argc != 5 )
        {
            err = argc < 5 ? ERR_INSUFFICENT_ARGS : ERR_TOO_MANY_ARGS;
            goto exit;
        }

        err = hex_string_to_uint8(argv[4], &buffer, &data_length);

        if (err != ERR_CMD_OK)
        {
            goto exit;
        }

        offload->keep_alive_packet_info.period_msec   = strtoul(argv[3], &end, 10);
        offload->keep_alive_packet_info.packet_length = data_length;
        offload->keep_alive_packet_info.packet        = buffer;

        result = wiced_wifi_enter_ds1_debug( WICED_STA_INTERFACE, WICED_OFFLOAD_KEEP_ALIVE, offload_array, ulp_milliseconds, &overrides );
        free(buffer);
    }
    else if ( strlen(sub_cmd_name) == strlen("arp_hostip") &&
        strncmp(sub_cmd_name, "arp_hostip", strlen(sub_cmd_name) ) == 0 )
    {
        /* Example: ds1_enter arp_hostip <ulp wait: ex. 8> <v4 address: ex. 192.168.1.115> */
        if ( argc != 4 )
        {
            err = argc < 4 ? ERR_INSUFFICENT_ARGS : ERR_TOO_MANY_ARGS;
            goto exit;
        }

        /* convert ascii to 32bit address */
        err = str_to_ip( argv[3], &offload->ipv4_address );

        /* only support an IPv4 address for now; if not, return error */
        if ( err == 0 && offload->ipv4_address.version == WICED_IPV4 )
        {
            result = wiced_wifi_enter_ds1_debug( WICED_STA_INTERFACE, WICED_OFFLOAD_ARP_HOSTIP, offload_array, ulp_milliseconds, &overrides );
        }
        else
        {
            err = ERR_BAD_ARG;
        }
    }
    else if ( strlen(sub_cmd_name) == strlen("pattern") &&
        strncmp(sub_cmd_name, "pattern", strlen(sub_cmd_name) ) == 0 )
    {
        /* Example: ds1_enter pattern <ulp wait: ex. 8> <offset in packet: ex. 20> <mask: ex. 0xffe008> <pattern: ex. 0x34567890> */
        if ( argc != 6 )
        {
            err = argc < 6 ? ERR_INSUFFICENT_ARGS : ERR_TOO_MANY_ARGS;
            goto exit;
        }

        /* mask */
        err = hex_string_to_uint8(argv[4], &buffer, &data_length);

        if ( err != ERR_CMD_OK )
        {
            WPRINT_APP_ERROR( ("err:%d", __LINE__) );
            goto exit;
        }
        offload->pattern.mask_size = data_length;
        offload->pattern.mask      = buffer;

        /* pattern */
        err = hex_string_to_uint8(argv[5], &buffer, &data_length);

        if ( err != ERR_CMD_OK )
        {
            free(offload->pattern.mask);
            WPRINT_APP_ERROR( ("err:%d", __LINE__) );
            goto exit;
        }
        offload->pattern.pattern_size = data_length;
        offload->pattern.pattern      = buffer;

        /* ulp and offset */
        offload->pattern.match_offset = strtoul(argv[3], &end, 10);

        result = wiced_wifi_enter_ds1_debug( WICED_STA_INTERFACE, WICED_OFFLOAD_PATTERN, offload_array, ulp_milliseconds, &overrides );

        free(offload->pattern.mask);
        free(offload->pattern.pattern);
    }
    else
    {
        WPRINT_APP_ERROR( ("Bad ip or invalid addr type\n") );
        err = ERR_BAD_ARG;
    }

exit:
    if ( ERR_CMD_OK == err && WICED_SUCCESS == result )
    {
        WPRINT_APP_INFO( ("Waiting for ds1 entry completion\n") );
        while ( 0 == ds1_entry_complete && wait_loop_delay < 2 * ulp_milliseconds )
        {
            host_rtos_delay_milliseconds(1);
            wait_loop_delay++;
        }
        WPRINT_APP_INFO( ("Ds1 entry %s; Waited about %d ms\n", (0 != ds1_entry_complete ) ? "succeeded" : "failed", wait_loop_delay) );

        if ( ( 0 == ds1_entry_complete ) )
        {
            result = WICED_ERROR;
        }
    }

    /* update err code for return when wiced APIs return an error */
    if ( result != WICED_SUCCESS && err == ERR_CMD_OK )
    {
        if ( result == WICED_OUT_OF_HEAP_SPACE )
        {
            err = ERR_OUT_OF_HEAP;
        }
        else
        {
            WPRINT_APP_ERROR( ( "Failed ds1 enter; result=%d", result ) );
            err = ERR_UNKNOWN;
        }
    }

    /* clear debug mode now */
    wwd_wifi_set_fw_cmd_debug_mode( WICED_FALSE );

    ds1_status( 0, NULL );

    return err;
}

int ds1_enable(int argc, char* argv[])
{
    int err = ERR_UNKNOWN;
    wiced_result_t result = wiced_wifi_ds1_enable( WICED_STA_INTERFACE );

    if ( WICED_SUCCESS == result )
    {
        err = ERR_CMD_OK;
    }
    else
    {
        WPRINT_APP_ERROR(("Error %d when enabing DS1.  Did you configure DS1 first?\n", result));
    }

    return err;
}

static wiced_offloads_container_t *ds1_alloc_offload_container(int argc, char* argv[])
{
    wiced_offloads_container_t *container;
    int offload_count = 0;
    const char* offload_names[] = {"magic", "keep_alive", "arp_hostip", "pattern", "gtk", "deauth", "all"};

    /* count offloads */
    for ( int string_index = 0 ; string_index < argc ; string_index++ )
    {
        for ( int name_index = 0 ; name_index < sizeof( offload_names )/sizeof( offload_names[0] ) ; name_index++ )
        {
            if ( 0 == strncmp( offload_names[name_index], argv[string_index], strlen(argv[string_index] ) ) )
            {
                offload_count++;
            }
        }

        if ( offload_count > sizeof( offload_names )/sizeof( offload_names[0] ) )
        {
            /* only support the named ones in array above */
            WPRINT_APP_ERROR(("Too many offloads specified\n"));
            return NULL;
        }
    }

    /* allocate container */
    container = malloc( sizeof(*container) );

    if ( NULL == container )
    {
        return NULL;
    }

    container->types  = calloc( offload_count, sizeof( container->types[0] ) );
    if ( NULL == container->types )
    {
        ds1_free_offload_container( container );
        return NULL;
    }

    container->values = calloc( offload_count, sizeof( container->values[0] ) );
    if ( NULL == container->values )
    {
        ds1_free_offload_container( container );
        return NULL;
    }

    container->num_offloads = offload_count;

    return container;
}

/* Return success if the current argument is a match for a one word WOWL setting, and if so, modify the offload_type parameter to reflect which offload */
static wiced_bool_t ds1_extract_name_only_wowl( char* argv[], int cur_argv, wiced_offload_t* offload_type )
{
    const char *sub_cmd_name = argv[cur_argv];
    wiced_bool_t success = WICED_FALSE;

    /* mapping command names to WICED offload types */
    command_console_name_value_pair_t named_values[] = { { "magic",  (void*)WICED_OFFLOAD_MAGIC  },
                                                         { "gtk",    (void*)WICED_OFFLOAD_GTK    },
                                                         { "deauth", (void*)WICED_OFFLOAD_DEAUTH },
                                                         { "all",    (void*)WICED_OFFLOAD_ALL    } };

    /* search for matches for sub_cmd_name */
    for ( int name_value_index = 0 ; name_value_index < ARRAY_SIZE( named_values ) ; name_value_index++ )
    {
        /* if there's a match, then use the value associated with the name to return offload_type */
        if ( strlen(sub_cmd_name) == strlen(named_values[name_value_index].name) && strncmp(sub_cmd_name, named_values[name_value_index].name, strlen(sub_cmd_name) ) == 0 )
        {
            *offload_type = (wiced_offload_t)named_values[name_value_index].value;

            /* we're done and we found a match */
            success = WICED_TRUE;
            break;
        }
    }

    return success;
}

static wiced_bool_t ds1_extract_keep_alive( int argc, char* argv[], int cur_argv, wiced_offload_value_t *value )
{
    const char *sub_cmd_name = argv[cur_argv];
    int args_after_cmd       = ( argc - cur_argv - 1 );
    uint8_t *buffer          = NULL;
    int err;
    const int msec_index     = cur_argv + 1;
    const int data_index     = cur_argv + 2;
    uint16_t data_length     = 0;
    char *end                = NULL;

    wiced_bool_t success     = WICED_FALSE;

    if ( strlen(sub_cmd_name) == strlen("keep_alive") &&
        strncmp(sub_cmd_name, "keep_alive", strlen(sub_cmd_name) ) == 0 )
    {
        /* Example: keep_alive <period msecs: ex. 20> <packet data: ex. 0x3243567abcdef> */
        if ( args_after_cmd < 2 )
        {
            WPRINT_APP_ERROR(("Bad formatting of keep_alive (see example usage)\n"));
            goto exit;
        }
        else
        {
            err = hex_string_to_uint8(argv[data_index], &buffer, &data_length);

            if (err != ERR_CMD_OK)
            {
                goto exit;
            }

            value->keep_alive_packet_info.period_msec   = strtoul(argv[msec_index], &end, 10);
            value->keep_alive_packet_info.packet_length = data_length;
            value->keep_alive_packet_info.packet        = buffer;

            success = WICED_TRUE;
        }
    }
exit:
    return success;
}

static wiced_bool_t ds1_extract_pattern( int argc, char* argv[], int cur_argv, wiced_offload_value_t *value )
{
    const char *sub_cmd_name = argv[cur_argv];
    int args_after_cmd       = ( argc - cur_argv - 1 );
    uint8_t *buffer          = NULL;
    int err;
    const int offset_index   = cur_argv + 1;
    const int mask_index     = cur_argv + 2;
    const int pattern_index  = cur_argv + 3;
    char *end                = NULL;

    uint16_t data_length     = 0;

    wiced_bool_t success = WICED_FALSE;

    /* Example: pattern <offset in packet: ex. 20> <mask: ex. 0xffe008> <pattern: ex. 0x34567890> */
    if ( strlen(sub_cmd_name) == strlen("pattern") &&
        strncmp(sub_cmd_name, "pattern", strlen(sub_cmd_name) ) == 0 )
    {
        if ( args_after_cmd < 3 )
        {
            goto exit;
        }

        /* mask */
        err = hex_string_to_uint8(argv[mask_index], &buffer, &data_length);

        if ( err != ERR_CMD_OK )
        {
            WPRINT_APP_ERROR( ("err:%d", __LINE__) );
            goto exit;
        }
        value->pattern.mask_size = data_length;
        value->pattern.mask      = buffer;

        /* pattern */
        err = hex_string_to_uint8(argv[pattern_index], &buffer, &data_length);

        if ( err != ERR_CMD_OK )
        {
            free(value->pattern.mask);
            WPRINT_APP_ERROR( ("err:%d", __LINE__) );
            goto exit;
        }
        value->pattern.pattern_size = data_length;
        value->pattern.pattern      = buffer;

        /* offset */
        value->pattern.match_offset = strtoul(argv[offset_index], &end, 10);

        success = WICED_TRUE;

    }
exit:
    return success;
}

static wiced_bool_t ds1_extract_arp_hostip( int argc, char* argv[], int cur_argv, wiced_offload_value_t *value )
{
    const char *sub_cmd_name = argv[cur_argv];
    int args_after_cmd       = ( argc - cur_argv - 1 );
    const int address_index  = cur_argv + 1;
    int err;

    wiced_bool_t success     = WICED_FALSE;

    /* Example: arp_hostip <v4 address: ex. 192.168.1.115> */

    if ( strlen(sub_cmd_name) == strlen("arp_hostip") &&
        strncmp(sub_cmd_name, "arp_hostip", strlen(sub_cmd_name) ) == 0 )
    {
        if ( args_after_cmd < 1 )
        {
            WPRINT_APP_ERROR(("Bad formatting of arp_hostip (see example usage)\n"));
            goto exit;
        }

        /* convert ascii to 32bit address */
        err = str_to_ip( argv[address_index], &value->ipv4_address );

        /* only support an IPv4 address for now; if not, return error */
        if ( err == 0 && value->ipv4_address.version == WICED_IPV4 )
        {
            success = WICED_TRUE;
        }
    }
exit:
    return success;
}

static wiced_result_t ds1_fill_offload_container( int argc, char* argv[], wiced_offloads_container_t *container )
{
    int cur_argv                 = 2; /* skip over the ds1_config command and the ulp wait period */
    wiced_offload_t offload_type = WICED_OFFLOAD_MAGIC;
    const int arp_hostip_arg_count = 2;
    const int keep_alive_arg_count = 3;
    const int pattern_arg_count    = 4;

    /* init all types to MAGIC, for convenience in processing in case of error: code will handle memory free appropriately*/
    for ( int cur_offload_index = 0 ; cur_offload_index < container->num_offloads ; cur_offload_index++ )
    {
        container->types[cur_offload_index] = WICED_OFFLOAD_MAGIC;
    }

    /* do the extraction */
    for ( int cur_offload_index = 0 ; cur_offload_index < container->num_offloads ; cur_offload_index++ )
    {
        /* extract a wowl type that is a name only like magic, gtk, or deauth or all */
        if ( WICED_TRUE == ds1_extract_name_only_wowl( argv, cur_argv, &offload_type ) )
        {
            cur_argv++;
        }
        else if ( WICED_TRUE == ds1_extract_keep_alive( argc, argv, cur_argv, &container->values[cur_offload_index] ) )
        {
            /* Example: keep_alive <period msecs: ex. 20> <packet data: ex. 0x3243567abcdef> */
            cur_argv += keep_alive_arg_count;
            offload_type = WICED_OFFLOAD_KEEP_ALIVE;
        }
        else if ( WICED_TRUE == ds1_extract_arp_hostip( argc, argv, cur_argv, &container->values[cur_offload_index] ) )
        {
            /* Example: arp_hostip <v4 address: ex. 192.168.1.115> */
            cur_argv += arp_hostip_arg_count;
            offload_type = WICED_OFFLOAD_ARP_HOSTIP;
        }
        else if ( WICED_TRUE == ds1_extract_pattern( argc, argv, cur_argv, &container->values[cur_offload_index] ) )
        {
            /* Example: pattern <offset in packet: ex. 20> <mask: ex. 0xffe008> <pattern: ex. 0x34567890> */
            cur_argv += pattern_arg_count;
            offload_type = WICED_OFFLOAD_PATTERN;
        }
        else
        {
            return WICED_ERROR;
        }
        container->types[cur_offload_index] = offload_type;
    }

    return WICED_SUCCESS;
}

static void ds1_free_offload_container( wiced_offloads_container_t *offload )
{
    if ( NULL != offload->types )
    {
        if ( NULL != offload->values )
        {
            for ( int offload_index = 0 ; offload_index < offload->num_offloads ; offload_index++ )
            {
                switch ( offload->types[offload_index] )
                {
                    case WICED_OFFLOAD_PATTERN:
                        free( offload->values[offload_index].pattern.pattern );
                        free( offload->values[offload_index].pattern.mask );
                        break;
                    case WICED_OFFLOAD_KEEP_ALIVE:
                        free( offload->values[offload_index].keep_alive_packet_info.packet );
                        break;
                    case WICED_OFFLOAD_ARP_HOSTIP:
                    case WICED_OFFLOAD_MAGIC:
                    default:
                        /* NO-OP */
                        break;
                }
            }

            free( offload->values );
        }
        free( offload->types );
    }
    free( offload );
}

int ds1_config(int argc, char* argv[])
{
    char *end                 = NULL;
    uint32_t ulp_milliseconds = strtoul( argv[1], &end, 10 );
    wiced_result_t result     = WICED_ERROR;

    if ( NULL != offloads_container )
    {
        ds1_free_offload_container( offloads_container );
        offloads_container = NULL;
    }

    offloads_container = ds1_alloc_offload_container( argc, argv );

    if ( NULL != offloads_container )
    {
        result = ds1_fill_offload_container( argc, argv, offloads_container );

        if ( WICED_SUCCESS != result )
        {
            ds1_free_offload_container( offloads_container );
            offloads_container = NULL;
        }
        else
        {
            result = wiced_wifi_ds1_config( WICED_STA_INTERFACE, offloads_container, ulp_milliseconds );
        }
    }
    else
    {
        result = WICED_OUT_OF_HEAP_SPACE;
    }

    if ( WICED_SUCCESS != result )
    {
        WPRINT_APP_ERROR(("Error %d: unable to config", result));
        return ERR_UNKNOWN;
    }
    return ERR_CMD_OK;
}

int ds1_status(int argc, char* argv[])
{
    uint8_t status_string[DS1_STATUS_STRING_LENGTH];
    UNUSED_PARAMETER(argc);
    UNUSED_PARAMETER(argv);
    wwd_wifi_ds1_get_status_string( (uint8_t*)status_string, sizeof(status_string) );

    WPRINT_APP_INFO(("DS1 state now: %s\n", status_string ));

    wiced_wifi_deep_sleep_get_status_string( (uint8_t*)status_string, sizeof(status_string) );
    WPRINT_APP_INFO(("%s\n", status_string ));

    return ERR_CMD_OK;
}

int ds1_wake(int argc, char* argv[])
{
    wiced_result_t result = wiced_wifi_wake_ds1( WICED_STA_INTERFACE );

    if ( result != WICED_SUCCESS )
    {
       WPRINT_APP_INFO((" %s.. FAILED. error=%d\n", __FUNCTION__, result));
       return ERR_UNKNOWN;
    }

    ds1_status( 0, NULL );

    return ERR_CMD_OK;
}

int ds1_disable(int argc, char* argv[])
{
    wiced_result_t result = wiced_wifi_ds1_disable( WICED_STA_INTERFACE );

    if ( result != WICED_SUCCESS )
    {
       WPRINT_APP_INFO((" %s.. FAILED. error=%d\n", __FUNCTION__, result));
       return ERR_UNKNOWN;
    }

    ds1_status( 0, NULL );

    return ERR_CMD_OK;
}

/* get fast bss transition over distribution system
 * 1 : FBT(Fast BSS Transition) Over-the-DS(Distribution System) is allowed
 * 0 : FBT (Fast BSS Transition) Over-the-DS not allowed
 */
int fbt_over_ds(int argc, char* argv[])
{
    int value = 0;
    wiced_bool_t set = WICED_TRUE;

    if( argv[1] )
    {
        value = htod32(strtoul( argv[1], NULL, 0) );
    }
    else
    {
        set = WICED_FALSE;
    }

    if( wwd_wifi_fast_bss_transition_over_distribution_system(WWD_STA_INTERFACE, set, &value) == WWD_SUCCESS ) {
        if ( set )
        {
           WPRINT_APP_INFO((" set/reset of Fast BSS Transition over DS success \n"));
        }
        else
        {
           WPRINT_APP_INFO((" get Fast BSS Transition over DS success WLFTBT:%d \n", value));
        }
    }
    else
    {
        WPRINT_APP_INFO((" wwd_wifi_fbt_over_ds.. FAILED\n"));
    }
    return ERR_CMD_OK;
}

/* get Fast BSS Transition capabilities */
int fbt_caps(int argc, char* argv[])
{
    wiced_bool_t value = WICED_FALSE;

    if( wwd_wifi_fast_bss_transition_capabilities(WWD_STA_INTERFACE, &value) == WWD_SUCCESS )
    {
         WPRINT_APP_INFO((" WLFBT Capabilities:%d\n", value));
    }
    else
    {
         WPRINT_APP_INFO((" wwd_wifi_get_fbt_enable.. Failed:\n"));
    }

    return ERR_CMD_OK;
}

/* DHCP lease renewal offload */
int dhcp_lease_renewal (int argc, char* argv[])
{
    uint8_t hops;
    wiced_buffer_t buffer;
    uint8_t index;
    uint16_t len;
    uint8_t ip_addr_type;
    uint8_t offload_type;
    wiced_mac_t src_mac_addr;
    wiced_mac_t dst_mac_addr;
    wwd_ipv4_addr_t src_ipaddr;
    wwd_ipv4_addr_t dst_ipaddr;
    uint32_t tid;
    uint32_t lease_time_remaining;
    uint32_t renew_seconds_before_expiry;
    wwd_result_t result;
    wiced_bool_t get_or_set = WICED_FALSE;
    wiced_buffer_t response;

    if(argv[1] != NULL )
    {
        uint8_t *data = NULL;
        wwd_dltro_connect_t* connect = NULL;

        len = (int)(WLC_IOCTL_DHCP_PKT_LEN - strlen(IOVAR_STR_DLTRO) - 1);
        data = (uint8_t* ) wwd_sdpcm_get_iovar_buffer(&buffer, (uint16_t)len, IOVAR_STR_DLTRO );

        if ( data == NULL )
        {
            WPRINT_APP_INFO((" %s.. Failed to allocate buffer for =%s\n", __FUNCTION__, IOVAR_STR_DLTRO ));
            return ERR_UNKNOWN;
        }
        wwd_dltro_t *dltro = (wwd_dltro_t *)data;

        if( strcasecmp (argv[1], "connect" ) == 0 )
        {
            if ( argv[2]  && argv[3] && argv[4] && argv[5] &&
                 argv[6]  && argv[7] && argv[8] && argv[9] &&
                 argv[10] && argv[11] && argv[12] )
            {
                get_or_set = WICED_TRUE;
                connect = (wwd_dltro_connect_t *)dltro->data;
                index = atoi(argv[2]);
                hops = atoi(argv[3]);
                ip_addr_type = atoi(argv[4]);
                offload_type = atoi(argv[5]);

                wifi_utils_str_to_mac(argv[6], &src_mac_addr);
                wifi_utils_str_to_mac(argv[7], &dst_mac_addr);

                if (!ip_addr_type)
                {
                    /* IPv4 */
                    if (!str_to_ipaddr_array(argv[8], &src_ipaddr))
                    {
                        WPRINT_APP_INFO(("Error: Src IP address incorrect format\n"));
                        return ERR_UNKNOWN;
                    }

                    if (!str_to_ipaddr_array(argv[9], &dst_ipaddr))
                    {
                        WPRINT_APP_INFO(( "Error: Dest IP address incorrect format\n" ));
                        return ERR_UNKNOWN;
                    }

                    tid = atoi(argv[10]);
                    lease_time_remaining = atoi(argv[11]);
                    renew_seconds_before_expiry = atoi(argv[12]);

                    dltro->subcmd_id = WWD_DLTRO_SUBCMD_CONNECT;
                    dltro->len = dltro_connect_init(connect, index, hops, ip_addr_type,
                                 offload_type,
                                 &src_mac_addr, &dst_mac_addr,
                                 (void *)&src_ipaddr,
                                 (void *)&dst_ipaddr,
                                 tid, lease_time_remaining, renew_seconds_before_expiry);
                 }
                 else
                 {
                    WPRINT_APP_INFO((" %s.. insufficient args  for connect. error=%s\n", __FUNCTION__, argv[1]));
                    return ERR_UNKNOWN;
                 }

              }
        }
        else if ( strcasecmp (argv[1], "param") == 0 )
        {

            if( argv[2] && argv[3] )
            {
                WPRINT_APP_INFO ((" dltro param set..\n"));
                get_or_set = WICED_TRUE;
                wwd_dltro_param_t *param = (wwd_dltro_param_t *)dltro->data;
                dltro->subcmd_id = WWD_DLTRO_SUBCMD_PARAM;
                param->index = atoi(argv[2]);
                param->retry = atoi(argv[3]);
                dltro->len = sizeof(*param);
            }
            else if ( argv[2] )
            {
                WPRINT_APP_INFO ((" dltro param get..\n"));
                wwd_dltro_param_t *param = (wwd_dltro_param_t *)dltro->data;
                dltro->subcmd_id = WWD_DLTRO_SUBCMD_PARAM;
                param->index = atoi(argv[0]);
                dltro->len = sizeof(*param);
            }
            else
            {
                 WPRINT_APP_INFO((" %s.. insufficient args  for param. error=%s\n", __FUNCTION__, argv[1]));
                 return ERR_UNKNOWN;
            }
        }
        else if ( strcasecmp (argv[1], "max_dltro") == 0 )
        {
            WPRINT_APP_INFO ((" dltro max_dltro..\n"));
            dltro->subcmd_id = WWD_DLTRO_SUBCMD_MAX_DLTRO;
            dltro->len = sizeof(wwd_dltro_max_dltro_t);
        }
        else
        {
            WPRINT_APP_INFO((" %s.. unknown command. error=%s\n", __FUNCTION__, argv[1]));
            return ERR_UNKNOWN;
        }

         /* Invoke SET iovar */
         len = OFFSETOF(wwd_dltro_t, data) + dltro->len;
         dltro->subcmd_id = htod16(dltro->subcmd_id);
         dltro->len = htod16(dltro->len);

         if ( get_or_set == WICED_TRUE )
         {
            result = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, WWD_STA_INTERFACE );
            if ( result != WWD_SUCCESS )
            {
             WPRINT_APP_INFO(("DHCP lease renewal IOVAR failed..result:%d\n", result ));
             return ERR_UNKNOWN;
            }
         }
         else
         {
             result = wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, WWD_STA_INTERFACE );
             if ( result != WWD_SUCCESS )
             {
                 WPRINT_APP_INFO(("DHCP lease renewal IOVAR failed..result:%d\n", result ));
                 return ERR_UNKNOWN;
             }

             dltro = (wwd_dltro_t *)host_buffer_get_current_piece_data_pointer( response );
             dltro->len = dtoh16(dltro->len);

             if ( dltro->subcmd_id == htod16(WWD_DLTRO_SUBCMD_PARAM) )
             {
                 wwd_dltro_param_t *param1 = (wwd_dltro_param_t *)dltro->data;
                 WPRINT_APP_INFO ((" Status : %s Retry Count=%d\n", ((dltro->len >= sizeof (*param1) ) ? "DLTRO ACTIVE" : "DLTRO INACTIVE"), dtoh16(param1->retry) ));
             }
             else if ( dltro->subcmd_id == htod16 (WWD_DLTRO_SUBCMD_MAX_DLTRO ))
             {
                 wwd_dltro_max_dltro_t *param = ( wwd_dltro_max_dltro_t *)dltro->data;
                 WPRINT_APP_INFO ((" %s is %d\n", ((dltro->len >= sizeof(*param)) ? " MAX_DLTRO " : " "), dtoh16(param->max )) );
             }

             if ( result == WWD_SUCCESS )
             {
                 host_buffer_release( response, WWD_NETWORK_RX );
             }
         }
    }
    return ERR_CMD_OK;
}

static int dltro_connect_init ( wwd_dltro_connect_t* connect, uint8_t index, uint8_t hops,
            uint8_t ip_addr_type, uint8_t offload_type, wiced_mac_t *saddr,
            wiced_mac_t *daddr, void *sipaddr, void *dipaddr,
            uint32_t tid, uint32_t lease_time_remaining, uint32_t renew_seconds_before_expiry )
{
    int data_len = 0;

    connect->index = index;
    connect->ip_addr_type = ip_addr_type;
    connect->offload_type = offload_type;
    connect->tid = htod32(tid);
    connect->timer_val = htod32(lease_time_remaining);
    connect->time_before_expiry = htod32(renew_seconds_before_expiry);

    if (!ip_addr_type)
    {
        /* For IPv4 */
        connect->len = dltro_pkt_v4( &connect->data[data_len], daddr, saddr,
                                         dipaddr, sipaddr, hops, tid);
        data_len += connect->len;
    }

    return (OFFSETOF(wwd_dltro_connect_t, data) + data_len);
}

/* Encode DLTRO packet for IPv4.
 * Input 'buf' size = (ETHER_HDR_LEN + sizeof(struct ipv4_hdr) +
 * sizeof(struct bcmudp_hdr))
 * Returns the size of the encoded packet
 */
static int dltro_pkt_v4(uint8_t *buf, wiced_mac_t* daddr, wiced_mac_t* saddr,
        wwd_ipv4_addr_t* dipaddr, wwd_ipv4_addr_t* sipaddr, uint8_t hops, uint32_t tid )
{
    wiced_ether_header_t *eth;
    ipv4_hdr_t*   ip;
    bcmudp_hdr_t* udp;
    dhcpv4_pkt_t* dhcp_pkt;

    if (buf == NULL)
    {
       return 0;
    }

    /* Populate ethernet header */
    eth = (wiced_ether_header_t *)buf;
    memcpy(eth->ether_dhost, daddr, ETHER_ADDR_LEN);
    memcpy(eth->ether_shost, saddr, ETHER_ADDR_LEN);
    eth->ether_type = hton16(ETHER_TYPE_IP);

    /* IP header */
    ip = (ipv4_hdr_t *)(buf + sizeof(wiced_ether_header_t));
    memcpy(ip->dst_ip, dipaddr, IPV4_ADDR_LEN);
    memcpy(ip->src_ip, sipaddr, IPV4_ADDR_LEN);
    ip->version_ihl = (IP_VER_4 << IPV4_VER_SHIFT) | (IPV4_MIN_HEADER_LEN / 4);
    ip->tos = 0;
    ip->tot_len = hton16(sizeof(ipv4_hdr_t) + sizeof(bcmudp_hdr_t) +
                  sizeof(dhcpv4_pkt_t));

    ip->id = hton16(0);
    ip->frag = 0;
    ip->ttl = IPv4_TTL;
    ip->prot = IP_PROT_UDP;
    ip->hdr_chksum = 0;
    ip->hdr_chksum = ipv4_hdr_cksum((uint8_t* )ip, IPV4_HLEN(ip));

    /* UDP header */
    udp = (bcmudp_hdr_t *)(buf + sizeof(wiced_ether_header_t) +
                           sizeof(ipv4_hdr_t));
    udp->src_port = hton16(UDP_PORT_DHCPv4_CLIENT);
    udp->dst_port = hton16(UDP_PORT_DHCPv4_SERVER);
    udp->len = hton16(sizeof(bcmudp_hdr_t) + sizeof(dhcpv4_pkt_t));

    /* TODO: add proper checksum calculation routine */
    udp->chksum = 0;

    /* UDP packet: DHCP lease time renewal */
    dhcp_pkt = (dhcpv4_pkt_t* )(buf + sizeof(wiced_ether_header_t) +
                                sizeof(ipv4_hdr_t) + sizeof(bcmudp_hdr_t));

    memset(&dhcp_pkt->msg_type, 0, sizeof(dhcpv4_pkt_t));
    dhcp_pkt->msg_type = DHCPV4_BOOT_REQ_MSG;
    dhcp_pkt->hw_type = DHCPV4_BOOT_HW_TYPE;
    dhcp_pkt->hwaddr_len = ETHER_ADDR_LEN;
    dhcp_pkt->hops = hops;
    dhcp_pkt->tid = hton32(tid);
    dhcp_pkt->bootp_flags = DHCPV4_BOOTP_BROADCAST_FLAG;
    memcpy(dhcp_pkt->ip_addr, sipaddr, IPV4_ADDR_LEN);
    memcpy(dhcp_pkt->client_mac_addr, saddr, ETHER_ADDR_LEN);
    dhcp_pkt->magic_cookie = DHCPV4_MAGIC_COOKIE;

    dhcp_pkt->tlv_field[0] = DHCPV4_MSG_TYPE;
    dhcp_pkt->tlv_field[1] = 1;
    dhcp_pkt->tlv_field[2] = DHCPV4_REQUEST_MSG;
    dhcp_pkt->tlv_field[3] = 0xff;

    return (sizeof(wiced_ether_header_t) + sizeof(ipv4_hdr_t) +
            sizeof(bcmudp_hdr_t) + sizeof(dhcpv4_pkt_t));
}

/* calculate IPv4 header checksum
 * - input ip points to IP header in network order
 * - output cksum is in network order
 */
static uint16_t ipv4_hdr_cksum( uint8_t *ip, int ip_len)
{
   uint32_t sum = 0;
   uint8_t *ptr = ip;

   wiced_assert (" ip packet is NULL ",  ip != NULL );

   wiced_assert (" ip len is less than IPV4 Hdr Len ",  ip_len >= IPV4_MIN_HEADER_LEN );

   /* partial cksum skipping the hdr_chksum field */
   sum = ip_cksum_partial(sum, ptr, OFFSETOF(ipv4_hdr_t, hdr_chksum));
   ptr += OFFSETOF(ipv4_hdr_t, hdr_chksum) + 2;

   /* return calculated chksum */
   return ip_cksum(sum, ptr, ip_len - OFFSETOF(ipv4_hdr_t, src_ip));
}

/* calculate partial checksum */
static uint32_t ip_cksum_partial (uint32_t sum, uint8_t* val8, uint32_t count)
{
   uint32_t i;
   uint16_t *val16 = (uint16_t *)val8;

   wiced_assert (" value is NULL ",  val8 != NULL );

   /* partial chksum calculated on 16-bit values */
   wiced_assert (" count is not modulo of 2 ",  ( (count % 2) == 0 ) );
   count /= 2;

   for (i = 0; i < count; i++)
   {
      sum += *val16++;
   }
   return sum;
}

/* calculate IP checksum */
static uint16_t ip_cksum (uint32_t sum, uint8_t *val8, uint32_t count )
{
   uint16_t *val16 = (uint16_t* )val8;

   wiced_assert (" value is NULL ",  val8 != NULL );

   while (count > 1)
   {
       sum += *val16++;
       count -= 2;
   }

   /*  add left-over byte, if any */
   if (count > 0)
   {
       sum += (*(uint8_t *)val16);
   }

   /*  fold 32-bit sum to 16 bits */
   sum = (sum >> 16) + (sum & 0xffff);
   sum += (sum >> 16);
   return ((uint16_t)~sum);
}

/* enable/disable WNM */
int wnm_enable ( int argc, char* argv [] )
{
    uint32_t value = 0;
    int i = 0;

    if(argv[1] != NULL )
    {
          if( strcasecmp (argv[1], "set" ) == 0 )
          {
             value = htod32(strtoul( argv[2], NULL, 0) );

             if ( wwd_wifi_set_iovar_value ( IOVAR_STR_WNM, value, WWD_STA_INTERFACE ) != WWD_SUCCESS )
             {
                   WPRINT_APP_INFO((" Enable WNM ..Failed:\n") );
            }
          }
          else if( strcasecmp ( argv[1], "get" ) == 0 )
          {
                if ( wwd_wifi_get_iovar_value ( IOVAR_STR_WNM, &value, WWD_STA_INTERFACE ) != WWD_SUCCESS )
                {
                   WPRINT_APP_INFO((" Get WNM..Failed:\n") );
                }
                else
                {
                    for ( i = 0; (i < sizeof( wnm_capabilities_msg )/sizeof(wnm_debug_msg_t )); i++  )
                    {
                        if ( (value & wnm_capabilities_msg[i].value ) == wnm_capabilities_msg[i].value )
                        {
                            WPRINT_APP_INFO((" WNM %s :  ENABLED \n", wnm_capabilities_msg[i].string ));
                        }
                    }
                }
         }
         else
         {
                return ERR_UNKNOWN;
         }
    }

    return ERR_CMD_OK;
}

/* Send BSS Transition Query */
int wnm_bsstransition_query(int argc, char* argv[])
{
   wwd_result_t result;
   wlc_ssid_t ssid;
   wiced_buffer_t buffer;
   uint8_t *data = NULL;
   uint32_t len = 0;

   memset( &ssid, 0, sizeof(wlc_ssid_t) );
   if ( argc >= 2 )
   {
      ssid.SSID_len = (uint8_t) (strlen((const char *)&argv[2]) + 1 );
      memcpy( ssid.SSID, &argv[2], ssid.SSID_len );

      if ( ssid.SSID_len > (size_t) SSID_NAME_SIZE )
      {
        WPRINT_APP_INFO ((" SSID too long\n"));
        return ERR_UNKNOWN;
      }

      len = sizeof(ssid);
   }

   result = wwd_wifi_is_ready_to_transceive ( WWD_STA_INTERFACE );

   if ( result != WWD_SUCCESS ) {
        WPRINT_APP_INFO(("STA is not associated to an AP\n"));
        return ERR_UNKNOWN;
   }

   /* get iovar buffer */
   data = (uint8_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t)len, IOVAR_STR_BSSTRANS_QUERY );
   CHECK_IOCTL_BUFFER( data );
   if ( len > 0 )
   {
     memcpy( data , ssid.SSID, ssid.SSID_len );
   }

   result = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, NULL, WWD_STA_INTERFACE );
   if ( result != WWD_SUCCESS )
   {
       WPRINT_APP_INFO(("WNM BSS Transition Query failed..result:%d\n", result ));
       return ERR_UNKNOWN;
   }

   return ERR_CMD_OK;
}

/* Set WNM BSS Transition response policy */
int wnm_bsstransition_response( int argc, char* argv[] )
{
    uint32_t value = 0;
    int i = 0;

    if(argv[1] != NULL )
    {
        if( strcasecmp (argv[1], "set" ) == 0 )
        {
            value = htod32(strtoul( argv[2], NULL, 0) );
            if ( wwd_wifi_set_iovar_value ( IOVAR_STR_BSSTRANS_RESP, value, WWD_STA_INTERFACE ) != WWD_SUCCESS )
            {
                 WPRINT_APP_INFO((" Enable WNM BSS TRANSITION RESPONSE ..Failed:\n") );
            }
        }
        else if( strcasecmp ( argv[1], "get" ) == 0 )
        {
            if ( wwd_wifi_get_iovar_value ( IOVAR_STR_BSSTRANS_RESP, &value, WWD_STA_INTERFACE ) != WWD_SUCCESS )
            {
                 WPRINT_APP_INFO((" Get WNM BSS TRANSITION RESPONSE..Failed:\n") );
            }
            else
            {
                for ( i = 0; (i < sizeof(wnm_bss_trans_resp_msg)/sizeof(wnm_debug_msg_t )); i++  )
                {
                     if (value == wnm_bss_trans_resp_msg[i].value )
                     {
                          WPRINT_APP_INFO((" %s :  ENABLED \n", wnm_bss_trans_resp_msg[i].string ));
                     }
                 }
             }
         }
         else
         {
             return ERR_UNKNOWN;
         }
   }
   return ERR_CMD_OK;
}

/* Get/Set Protected Management Capability */
int mfp_capabilities (int argc, char* argv[] )
{
    uint32_t value = 0;

    if(argv[1] != NULL )
    {
        if( strcasecmp (argv[1], "set" ) == 0 )
        {
           value = htod32(strtoul( argv[2], NULL, 0) );

           if ( wwd_wifi_set_iovar_value ( IOVAR_STR_MFP, value, WWD_STA_INTERFACE ) != WWD_SUCCESS )
           {
               WPRINT_APP_INFO((" Set Management Frame Protection..Failed:\n") );
           }
        }
        else if( strcasecmp ( argv[1], "get" ) == 0 )
        {
            if ( wwd_wifi_get_iovar_value ( IOVAR_STR_MFP, &value, WWD_STA_INTERFACE ) != WWD_SUCCESS )
            {
               WPRINT_APP_INFO((" Get Management Frame Protection..Failed:\n") );
            }
            else
            {
                  switch(value)
                  {
                      case WL_MFP_NONE:
                            WPRINT_APP_INFO((" Management Frame Protection 0- MFP NONE \n"));
                            break;

                      case WL_MFP_CAPABLE:
                            WPRINT_APP_INFO((" Management Frame Protection 1- MFP CAPABLE \n"));
                            break;

                      case WL_MFP_REQUIRED:
                            WPRINT_APP_INFO((" Management Frame Protection 2- MFP REQUIRED \n"));
                            break;
                      default:
                            WPRINT_APP_INFO((" Management Frame Protection UNKNOWN!!\n"));
                  }
             }
        }
        else
        {
            return ERR_UNKNOWN;
        }
    }

    return ERR_CMD_OK;
}

static int
print_str_iovar(char *iovar)
{
  char buf[255];
  wwd_result_t result;

  buf[0] = 0;
  result = wwd_wifi_get_iovar_buffer(iovar, (uint8_t *)buf, sizeof(buf), WWD_STA_INTERFACE );

  if (result != WWD_SUCCESS) {
    buf[0] = 0;
  } else {
    WPRINT_APP_INFO(("%s", buf));
  }
  return result;
}

static void parse_bands_from_arg( const char *str, wiced_802_11_band_t *band, uint8_t *num_bands )
{
    uint8_t i = 0;
    const char *bands[] = { "2g", "5g" };
    wiced_802_11_band_t band_list[] = { WICED_802_11_BAND_2_4GHZ, WICED_802_11_BAND_5GHZ };

    *num_bands = 0;

    /* loop through array */
    for ( ; i < sizeof( bands )/sizeof( bands[0] ) ; i++ )
    {
        if ( strlen( bands[i] ) == strlen( str ) && 0 == strncmp( str, bands[i], strlen( str ) ) )
        {
            *num_bands = 1;
            *band = band_list[i];
            return;
        }
    }

    if ( strlen( "all" ) == strlen( str ) && 0 == strncmp( str, "all", strlen( str ) ) )
    {
        *num_bands = 2;
    }
}

static int roam_per_band_config_wrapper( int argc, char* argv[], wiced_result_t (*set_config)( int32_t trigger_level, wiced_802_11_band_t band ), wiced_result_t (*get_config)( int32_t* trigger_level, wiced_802_11_band_t band ) )
{
    int32_t trigger;
    wiced_result_t result;
    int err = ERR_CMD_OK;
    wiced_802_11_band_t band = WICED_802_11_BAND_2_4GHZ;
    wiced_bool_t five_gigahertz_get = WICED_FALSE, five_gigahertz_set = WICED_FALSE, two_point_four_gigahertz_get = WICED_FALSE, two_point_four_gigahertz_set = WICED_FALSE;
    uint8_t num_bands = 0;
    wiced_bool_t five_gigahertz_specified = WICED_FALSE;

    /* determine which operations to do */
    if ( 1 == argc )
    {
        /* get for all bands */
        five_gigahertz_get = WICED_TRUE;
        two_point_four_gigahertz_get = WICED_TRUE;
    }
    else if ( argc <= 3 )
    {
        parse_bands_from_arg( argv[1], &band, &num_bands );

        if ( 1 == num_bands && WICED_802_11_BAND_5GHZ == band )
        {
            five_gigahertz_specified = WICED_TRUE;
        }

        if ( 3 == argc )
        {
            /* set for one or all bands */
            if ( 2 == num_bands || WICED_802_11_BAND_5GHZ == band )
            {
                five_gigahertz_set = WICED_TRUE;
            }
            if ( 2 == num_bands || WICED_802_11_BAND_2_4GHZ == band )
            {
                two_point_four_gigahertz_set = WICED_TRUE;
            }

            trigger = (uint32_t)atoi( argv[2] );
        }
        else
        {
            /* set for all bands or get for one or all bands */
            if ( num_bands > 0 )
            {
                /*get */
                if ( 2 == num_bands || WICED_802_11_BAND_5GHZ == band )
                {
                    five_gigahertz_get = WICED_TRUE;
                }
                else if ( 2 == num_bands || WICED_802_11_BAND_2_4GHZ == band )
                {
                    two_point_four_gigahertz_get = WICED_TRUE;
                }
            }
            else
            {
                /* set all bands */
                five_gigahertz_set = WICED_TRUE;
                two_point_four_gigahertz_set = WICED_TRUE;
            }

            trigger = (int32_t)atoi( argv[1] );
        }
    }
    else
    {
        err = ERR_TOO_MANY_ARGS;
    }

    /* do operations */
    if ( WICED_TRUE == five_gigahertz_get )
    {
        band = WICED_802_11_BAND_5GHZ;
        result = get_config( &trigger, band );
        if ( WICED_SUCCESS == result )
        {
            WPRINT_APP_INFO( ("5g: %ld\n", trigger) );
        }
        else if  ( WICED_WLAN_BADBAND == result )
        {
            if ( WICED_TRUE == five_gigahertz_specified )
            {
                WPRINT_APP_INFO( ("5g: unsupported\n") );
            }
        }
        else
        {
            WPRINT_APP_INFO( ("5g: Error %d\n", result) );
            err = ERR_UNKNOWN;
        }
    }

    if ( WICED_TRUE == five_gigahertz_set )
    {
        band = WICED_802_11_BAND_5GHZ;
        result = set_config( trigger, band );
        if  ( WICED_WLAN_BADBAND == result )
        {
            if ( WICED_TRUE == five_gigahertz_specified )
            {
                WPRINT_APP_INFO( ("5g: unsupported\n") );
            }
        }
        else if ( WICED_SUCCESS != result )
        {
            WPRINT_APP_INFO( ("5g: Error %d\n", result) );
            err = ERR_UNKNOWN;
        }
    }

    if ( WICED_TRUE == two_point_four_gigahertz_get )
    {
        band = WICED_802_11_BAND_2_4GHZ;
        result = get_config( &trigger, band );
        if ( WICED_SUCCESS == result )
        {
            WPRINT_APP_INFO( ("2g: %ld\n", trigger) );
        }
        else
        {
            WPRINT_APP_INFO( ("2g: Error %d\n", result) );
            err = ERR_UNKNOWN;
        }
    }

    if ( WICED_TRUE == two_point_four_gigahertz_set )
    {
        band = WICED_802_11_BAND_2_4GHZ;
        result = set_config( trigger, band );
        if ( WICED_SUCCESS != result )
        {
            WPRINT_APP_INFO( ("5g: Error %d\n", result) );
            err = ERR_UNKNOWN;
        }
    }

    return err;
}

int roam_trigger( int argc, char* argv[] )
{
    return roam_per_band_config_wrapper( argc, argv, wiced_wifi_set_roam_trigger_per_band, wiced_wifi_get_roam_trigger_per_band );
}

static wiced_result_t set_roam_delta_per_band( int32_t delta, wiced_802_11_band_t band )
{
    return (wiced_result_t)wwd_wifi_set_roam_delta_per_band( delta, band );
}

static wiced_result_t get_roam_delta_per_band( int32_t *delta, wiced_802_11_band_t band )
{
    return (wiced_result_t)wwd_wifi_get_roam_delta_per_band( delta, band );
}

int roam_delta( int argc, char* argv[] )
{
    return roam_per_band_config_wrapper( argc, argv, set_roam_delta_per_band, get_roam_delta_per_band );
}

int wlver( int argc, char* argv[] )
{
    return print_str_iovar(IOVAR_STR_VERSION);
}

int clmver(int argc, char* argv[] )
{
  char version[255];
  wwd_result_t result = wwd_wifi_get_clm_version(version, sizeof(version));

  if (result != WWD_SUCCESS)
  {
      WPRINT_APP_INFO( (" Get WLAN CLM Version failed\n"));
  }
  else
  {
      WPRINT_APP_INFO(("%s\n", version));
  }
  return result;
}

int memuse(int argc, char* argv[] )
{
    return print_str_iovar(IOVAR_STR_MEMUSE);
}

static wiced_result_t print_log( void *arg ) {
    wwd_wifi_read_wlan_log( continous_wlan_buf, CONTINUOUS_WLAN_LOG_SIZE );
    return WICED_SUCCESS;
}

int wl_continuous(int argc, char* argv[] )
{
    /* Need at least start or stop keywords */
    if (argc < 1)
        return ERR_INSUFFICENT_ARGS;

    if (argc == 1) {
        if (continous_wlan_buf != NULL)
            WPRINT_APP_INFO(("Enabled\n"));
        else
            WPRINT_APP_INFO(("Disabled\n"));
        return ERR_CMD_OK;
    }
    /* Start streaming with any of these keywords */
    if (!strncmp(argv[1], "start", 3) || !strncmp(argv[1], "on", 3) || (atoi(argv[1]) == 1)) {

        if (continous_wlan_buf != NULL) {
            WPRINT_APP_INFO(("Continuous output already running!\n"));
            return ERR_CMD_OK;
        }

        continous_wlan_buf = malloc_named( "wlog_stream", CONTINUOUS_WLAN_LOG_SIZE );
        if (continous_wlan_buf == NULL) {
            WPRINT_APP_INFO(("Unable to obtain buffer\n"));
            return ERR_OUT_OF_HEAP;
        }

        WPRINT_APP_INFO(("Continuous output starting\n"));
        wiced_rtos_create_worker_thread( &log_processor, WICED_DEFAULT_WORKER_PRIORITY, EVENT_PROCESSOR_THREAD_STACK_SIZE, EVENT_PROCESSOR_THREAD_QUEUE_SIZE );
        wiced_rtos_register_timed_event( &wlog_app_timer, &log_processor, &print_log, 10, 0 );

    } else { /* Any other keywords halts the streaming */

        if (continous_wlan_buf == NULL) {
            WPRINT_APP_INFO(("Continuous output already stopped!\n"));
            return ERR_CMD_OK;
        }
        wiced_rtos_deregister_timed_event( &wlog_app_timer );
        wiced_rtos_delete_worker_thread( &log_processor );
        free( continous_wlan_buf );
        continous_wlan_buf = NULL;
        WPRINT_APP_INFO(("Continuous output stopped\n"));
    }
    return ERR_CMD_OK;
}

int mesh_set_channel( int argc, char* argv[] )
{
    wiced_buffer_t buffer;
    uint32_t*      data;
    wwd_result_t   result;
    uint32_t       channel;

    data = wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 4, "chanspec" );
    CHECK_IOCTL_BUFFER( data );

    channel  = (uint32_t)atoi( argv[1] );
    *data = 0x1000 | (channel & 0xFF);

    result = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );

    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to set channel %u - err: %u\n", (unsigned int)channel, (unsigned int)result));
        return ERR_UNKNOWN;
    }

    return ERR_CMD_OK;
}

int mesh_auto_peer( int argc, char* argv[] )
{
    wiced_buffer_t buffer;
    uint32_t*      data;
    wwd_result_t   result;

    data = wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 4, "mesh_auto_peer" );
    CHECK_IOCTL_BUFFER( data );

    *data  = (uint32_t)atoi( argv[1] );

    result = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );

    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to set mesh_auto_peer - err: %u\n", (unsigned int)result));
        return ERR_UNKNOWN;
    }

    return ERR_CMD_OK;
}

int mesh_filter( int argc, char* argv[] )
{
    wiced_buffer_t buffer;
    wiced_mac_t mac;
    char*      data;
    wwd_result_t   result;

    wifi_utils_str_to_mac(argv[1], &mac);

    data = wwd_sdpcm_get_iovar_buffer( &buffer, ETHER_ADDR_LEN, "mesh_filter" );
    CHECK_IOCTL_BUFFER( data );
    memcpy(data, (char *)&mac, ETHER_ADDR_LEN);

    result = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );

    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to set mesh filter- err: %u\n", (unsigned int)result));
        return ERR_UNKNOWN;
    }

    return ERR_CMD_OK;
}

int join_mesh( int argc, char* argv[] )
{
    wiced_buffer_t        buffer;
    uint32_t*             data;
    char*                 jparam;
    wwd_result_t          result;
    wl_join_params_t*     join_params;
    uint32_t              join_params_size;

    int                   err  = ERR_UNKNOWN;
    char*                 ssid = argv[1];

    join_params_size = WL_JOIN_PARAMS_FIXED_SIZE + WL_NUMCHANNELS * sizeof(chanspec_t);
    join_params      = malloc(join_params_size);

    if ( NULL == join_params )
    {
            WPRINT_APP_INFO(("Error allocating %u bytes for assoc params\n", (unsigned int)join_params_size));
            return ERR_OUT_OF_HEAP;
    }

    memset(join_params, 0, join_params_size);
    join_params->params.bssid.octet[0] = 0xFF;
    join_params->params.bssid.octet[1] = 0xFF;
    join_params->params.bssid.octet[2] = 0xFF;
    join_params->params.bssid.octet[3] = 0xFF;
    join_params->params.bssid.octet[4] = 0xFF;
    join_params->params.bssid.octet[5] = 0xFF;

    join_params->ssid.SSID_len = strlen(ssid);
    memcpy(join_params->ssid.SSID, ssid, join_params->ssid.SSID_len);

    data = wwd_sdpcm_get_ioctl_buffer(&buffer, sizeof(uint32_t));
    if ( NULL == data )
    {
        err = ERR_OUT_OF_HEAP;
        goto error;
    }
    *data = 2;

    result = wwd_sdpcm_send_ioctl(SDPCM_SET, WLC_SET_INFRA, buffer, NULL, WWD_STA_INTERFACE);

    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to set imode to mesh - err: %u\n", (unsigned int)result));
        goto error;
    }

    data = wwd_sdpcm_get_ioctl_buffer(&buffer, sizeof(uint32_t));
    if ( NULL == data )
    {
        err = ERR_OUT_OF_HEAP;
        goto error;
    }

    *data = 0;

    result = wwd_sdpcm_send_ioctl(SDPCM_SET, WLC_SET_AUTH, buffer, NULL, WWD_STA_INTERFACE);

    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to set auth - err: %u\n", (unsigned int)result));
        goto error;
    }

    jparam = wwd_sdpcm_get_ioctl_buffer(&buffer, join_params_size);
    if ( NULL == jparam )
    {
        err = ERR_OUT_OF_HEAP;
        goto error;
    }

    memcpy( jparam, (char *)join_params, join_params_size );
    result = wwd_sdpcm_send_ioctl( SDPCM_SET, WLC_SET_SSID, buffer, NULL, WWD_STA_INTERFACE );

    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to set SSID - err: %u\n", (unsigned int)result));
        goto error;
    }

    return ERR_CMD_OK;

error:
    free(join_params);
    return err;
}

int mesh_mcast_rebroadcast( int argc, char* argv[] )
{
    wiced_buffer_t buffer;
    uint32_t*      data;
    wwd_result_t   result;

    data = wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 4, "mesh_mcast_rebro" );
    CHECK_IOCTL_BUFFER( data );

    *data  = (uint32_t)atoi( argv[1] );

    result = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );

    if ( result != WWD_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to set mesh_mcast_rebroadcast - err: %u\n", (unsigned int)result));
        return ERR_UNKNOWN;
    }

    return ERR_CMD_OK;
}

static int print_nan_config_status ( wwd_nan_state_t* nan_state )
{

    WPRINT_APP_INFO (("  ===== NAN STATUS ===== \n" ));
    WPRINT_APP_INFO (("    NAN Enabled:                         %d\n",  nan_state->enabled ));
    WPRINT_APP_INFO (("    NAN Inited:                          %d\n",  nan_state->inited ));
    WPRINT_APP_INFO (("    NAN Joined:                          %d\n",  nan_state->joined ));
    WPRINT_APP_INFO (("    NAN Merged:                          %d\n",  nan_state->merged ));
    WPRINT_APP_INFO (("    NAN Role:                            %d\n",  nan_state->role ));
    WPRINT_APP_INFO (("    NAN chanspec[0]:                     %lx\n", nan_state->chspec[0] ));
    WPRINT_APP_INFO (("    NAN chanspec[1]:                     %lx\n", nan_state->chspec[1] ));
    WPRINT_APP_INFO (("    NAN Master Rank:                     %02x:%02x:%02x:%02x:%02x:%02x\n",
            nan_state->mr[0], nan_state->mr[1],
            nan_state->mr[2], nan_state->mr[3],
            nan_state->mr[4], nan_state->mr[5] ));
    WPRINT_APP_INFO (("    NAN Anchor Master Rank:              %02x:%02x:%02x:%02x:%02x:%02x\n",
            nan_state->amr[0], nan_state->amr[1],
            nan_state->amr[2], nan_state->amr[3],
            nan_state->amr[4], nan_state->amr[5] ));

    WPRINT_APP_INFO (("    NAN Pending TxFrames:                %lu\n", nan_state->cnt_pend_txfrm ));
    WPRINT_APP_INFO (("    NAN TX Disc/Sync BeaconCount:        %lu\n", nan_state->nan_config_status.cnt_bcn_tx ));
    WPRINT_APP_INFO (("    NAN RX Disc/Sync Beacon count:       %lu\n", nan_state->nan_config_status.cnt_bcn_rx ));
    WPRINT_APP_INFO (("    NAN TX Service/Disc Frame count:     %lu\n", nan_state->nan_config_status.cnt_svc_disc_tx ));
    WPRINT_APP_INFO (("    NAN RX Service/Disc Frame count:     %lu\n", nan_state->nan_config_status.cnt_svc_disc_rx ));
    WPRINT_APP_INFO (("    NAN Anchor Master BeaconTarget Time: %lu\n", nan_state->ambtt ));
    WPRINT_APP_INFO (("    NAN ClusterID                        %02x:%02x:%02x:%02x:%02x:%02x\n",
            nan_state->nan_config_params.cid.octet[0], nan_state->nan_config_params.cid.octet[1],
            nan_state->nan_config_params.cid.octet[2], nan_state->nan_config_params.cid.octet[3],
            nan_state->nan_config_params.cid.octet[4], nan_state->nan_config_params.cid.octet[5] ));
    WPRINT_APP_INFO (("    NAN Hop Count:                       %d\n", nan_state->nan_config_params.hop_count ));
    WPRINT_APP_INFO (("  ==============  \n" ));

    return ERR_CMD_OK;
}

static int print_nan_mac_address ( wwd_nan_cluster_id_t* ether_addr )
{
    WPRINT_APP_INFO (( " NAN MAC ADDRESS : %02x:%02x:%02x:%02x:%02x:%02x\n",
                         ether_addr->octet[0], ether_addr->octet[1],
                         ether_addr->octet[2], ether_addr->octet[3],
                         ether_addr->octet[4], ether_addr->octet[5] ));

    return ERR_CMD_OK;
}

static int print_nan_device_state( uint8_t nan_device_state )
{
    switch ( nan_device_state )
    {
        case WL_NAN_ROLE_AUTO:
            WPRINT_APP_INFO (( "WL_NAN_ROLE_AUTO\n"));
            break;
        case WL_NAN_ROLE_NON_MASTER_NON_SYNC:
            WPRINT_APP_INFO (( "WL_NAN_ROLE_NON_MASTER_NON_SYNC\n" ));
            break;
        case WL_NAN_ROLE_NON_MASTER_SYNC:
            WPRINT_APP_INFO (("WL_NAN_ROLE_NON_MASTER_SYNC\n"));
            break;
        case WL_NAN_ROLE_MASTER:
            WPRINT_APP_INFO (("WL_NAN_ROLE_MASTER \n"));
            break;
        case WL_NAN_ROLE_ANCHOR_MASTER:
            WPRINT_APP_INFO (("WL_NAN_ROLE_ANCHOR_MASTER \n"));
            break;
        default:
            break;
    }

    return ERR_CMD_OK;
}

static void app_nan_event_handler( const void *event_header, const uint8_t *event_data )
{
   UNUSED_PARAMETER(event_header);
   UNUSED_PARAMETER(event_data);
   const wwd_event_header_t *evt = (wwd_event_header_t *)event_header;
   print_nan_event(evt);
}

static void print_nan_event( const wwd_event_header_t *event_header )
{
    char *event_name = NULL;
    if (event_header->event_type == WLC_E_NAN )
    {
        switch(event_header->reason)
        {
            case WLC_E_NAN_EVENT_START:
               event_name = NAME_TO_STR(WL_NAN_EVENT_START);
               break;
            case WLC_E_NAN_EVENT_JOIN:
               event_name = NAME_TO_STR(WL_NAN_EVENT_JOIN);
               break;
            case WLC_E_NAN_EVENT_ROLE:
              event_name = NAME_TO_STR(WL_NAN_EVENT_ROLE);
              break;
            case WLC_E_NAN_EVENT_SCAN_COMPLETE:
              event_name = NAME_TO_STR(WL_NAN_EVENT_SCAN_COMPLETE);
              break;
            case WLC_E_NAN_EVENT_DISCOVERY_RESULT:
              event_name = NAME_TO_STR(WL_NAN_EVENT_DISCOVERY_RESULT);
              break;
            case WLC_E_NAN_EVENT_REPLIED:
              event_name = NAME_TO_STR(WL_NAN_EVENT_REPLIED);
              break;
            case WLC_E_NAN_EVENT_TERMINATED:
              event_name = NAME_TO_STR(WL_NAN_EVENT_TERMINATED);
              break;
            case WLC_E_NAN_EVENT_RECEIVE:
              event_name = NAME_TO_STR(WL_NAN_EVENT_RECEIVE);
              break;
            case WLC_E_NAN_EVENT_STATUS_CHG:
              event_name = NAME_TO_STR(WL_NAN_EVENT_STATUS_CHG);
              break;
            case WLC_E_NAN_EVENT_MERGE:
              event_name = NAME_TO_STR(WL_NAN_EVENT_MERGE);
              break;
            case WLC_E_NAN_EVENT_STOP:
              event_name = NAME_TO_STR(WL_NAN_EVENT_STOP);
              break;
            case WLC_E_NAN_EVENT_P2P:
              event_name = NAME_TO_STR(WL_NAN_EVENT_P2P);
              break;
            case WLC_E_NAN_EVENT_WINDOW_BEGIN_P2P:
              event_name = NAME_TO_STR(WL_NAN_EVENT_WINDOW_BEGIN_P2P);
              break;
            case WLC_E_NAN_EVENT_WINDOW_BEGIN_MESH:
              event_name = NAME_TO_STR(WL_NAN_EVENT_WINDOW_BEGIN_MESH);
              break;
            case WLC_E_NAN_EVENT_WINDOW_BEGIN_IBSS:
              event_name = NAME_TO_STR(WL_NAN_EVENT_WINDOW_BEGIN_IBSS);
              break;
            case WLC_E_NAN_EVENT_WINDOW_BEGIN_RANGING:
              event_name = NAME_TO_STR(WL_NAN_EVENT_WINDOW_BEGIN_RANGING);
              break;
            case WLC_E_NAN_EVENT_DATA_IND:
              event_name = NAME_TO_STR(WL_NAN_EVENT_DATA_IND);
              break;
            case WLC_E_NAN_EVENT_DATA_CONF:
              event_name = NAME_TO_STR(WL_NAN_EVENT_DATA_CONF);
              break;
            default:
              break;
      }
      if (event_name)
      {
         WPRINT_APP_INFO ((" event name: %s \n", event_name));
      }
      else
      {
         WPRINT_APP_INFO(("Unknown event %d\n", event_header->reason ));
      }
    }
}

int get_cur_interface( int argc, char* argv[] )
{
    wiced_interface_t interface;
    wiced_result_t result;

    result = wiced_get_default_ready_interface(&interface);
    if (result == WICED_SUCCESS)
    {
        if (interface == WICED_STA_INTERFACE)
        {
            WPRINT_APP_INFO(("Current Interface in use is STA\n"));
        }
        else if (interface == WICED_AP_INTERFACE)
        {
            WPRINT_APP_INFO(("Current Interface in use is AP\n"));
        }
        else if (interface == WICED_P2P_INTERFACE)
        {
            WPRINT_APP_INFO(("Current Interface in use is P2P\n"));
        }

        return ERR_CMD_OK;
    }
    else if (result == WICED_NOTUP)
    {
        WPRINT_APP_INFO(("Fail: Last set interface is not up\n"));

        return ERR_CMD_OK;
    }
    else
    {
        WPRINT_APP_INFO(("No interface is active\n"));
        return ERR_CMD_OK;
    }
}

int set_cur_interface( int argc, char* argv[] )
{
    if (strcmp(argv[1], "sta") == 0)
    {
        if(wiced_set_interface(WICED_STA_INTERFACE) != WICED_SUCCESS)
        {
            WPRINT_APP_INFO(("Fail: Interface STA not up\n"));
        }
    }
    else if (strcmp(argv[1], "ap") == 0)
    {
        if(wiced_set_interface(WICED_AP_INTERFACE) != WICED_SUCCESS)
        {
            WPRINT_APP_INFO(("Fail: Interface AP not up\n"));
        }
    }
    else if (strcmp(argv[1], "p2p") == 0)
    {
        if(wiced_set_interface(WICED_P2P_INTERFACE) != WICED_SUCCESS)
        {
            WPRINT_APP_INFO(("Fail: Interface P2P not up\n"));
        }
    }
    else if (strcmp(argv[1], "auto") == 0)
    {
        if(wiced_set_interface(WICED_INTERFACE_MAX) != WICED_SUCCESS)
        {
            WPRINT_APP_INFO(("Fail: Interface not set to auto\n"));
        }
    }
    else
    {
        WPRINT_APP_INFO( ("Usage: set_interface sta/ap/p2p/auto\n" ) );
    }

    return ERR_CMD_OK;
}

