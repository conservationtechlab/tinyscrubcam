#ifndef BUILDCFG_H
#define BUILDCFG_H
#include "brcm_fw_types.h"
#include <string.h>
#ifdef __cplusplus
extern "C"
{
#endif

#define memcpy   mpaf_memcpy
#define memset   mpaf_memset
#define memmove  mpaf_memmove

void * mpaf_memcpy( void *dst, const void *src, int len );
void * mpaf_memset( void * ptr, int value, int num );
void * mpaf_memmove( void * destination, const void * source, int num );

//#include "bte_glue.h"
//#include "mpaf_config.h"

#ifdef MPAF_DISABLE_BR_EDR
#   define MPAF_BLE_ONLY TRUE
#else
#   define MPAF_BLE_ONLY FALSE
#endif

#define AVCT_INCLUDED   FALSE
#define AVRC_INCLUDED   FALSE
#define AVDT_INCLUDED   FALSE

#define AT91_MAIN_INCLUDED   FALSE
#define AT91_DRV_INCLUDED   TRUE
#define AT91_LIB_INCLUDED   TRUE
#define AT91_GKI_INCLUDED   TRUE
#define UNV_INCLUDED   FALSE
#define BBY_MAIN_INCLUDED   FALSE

#define A2D_INCLUDED   FALSE 
#define A2D_SBC_INCLUDED   FALSE
#define A2D_M12_INCLUDED   FALSE
#define A2D_M24_INCLUDED   FALSE

#define VDP_INCLUDED       FALSE
#define VDP_H263_INCLUDED   FALSE
#define VDP_MPEG_INCLUDED   FALSE
#define VDP_VEND_INCLUDED   FALSE
#define MCA_INCLUDED   FALSE
#define BIP_INCLUDED   FALSE
#define BIP_INITR_INCLUDED   FALSE
#define BIP_RSPDR_INCLUDED   FALSE
#define BIP_PUSH_INCLUDED   FALSE
#define BIP_PULL_INCLUDED   FALSE
#define BIP_PRINTING_INCLUDED   FALSE
#define BIP_ARCHIVE_INCLUDED   FALSE
#define BIP_CAMERA_INCLUDED   FALSE
#define BIP_DISPLAY_INCLUDED   FALSE
#define BPP_INCLUDED   FALSE
#define BPP_SND_INCLUDED   FALSE
#define CTP_INCLUDED   FALSE
#define DUN_INCLUDED   FALSE
#define FTP_INCLUDED   FALSE
#define FTP_SERVER_INCLUDED   FALSE
#define FTP_CLIENT_INCLUDED   FALSE
#ifdef RFCOMM_STACK_ENABLE
#   define GAP_INCLUDED   TRUE
#else
#   define GAP_INCLUDED   FALSE
#endif // RFCOMM_STACK_ENABLE
#define GOEP_INCLUDED   FALSE
#define GOEP_FS_INCLUDED   FALSE
#define HCITHIN_INCLUDED   FALSE
#define HCRP_INCLUDED   FALSE
#define HCRP_CLIENT_INCLUDED   FALSE
#define HCRP_SERVER_INCLUDED   FALSE

#define HSP2_INCLUDED   FALSE
#define HFP_INCLUDED   FALSE

#define ICP_INCLUDED   FALSE
#define L2CAP_INCLUDED   TRUE
#ifdef L2CAP_UCD_ENABLE
#define L2CAP_UCD_INCLUDED   TRUE
#define L2CAP_EXTFEA_SUPPORTED_MASK L2CAP_EXTFEA_UCD_RECEPTION
#else
#define L2CAP_UCD_INCLUDED   FALSE
#endif
#define OBX_INCLUDED   FALSE
#define OBX_SERVER_INCLUDED   FALSE
#define OBX_CLIENT_INCLUDED   FALSE
#define OBX_MD5_TEST_INCLUDED   FALSE
#define OPP_INCLUDED   FALSE
#define OPP_SERVER_INCLUDED   FALSE
#define OPP_CLIENT_INCLUDED   FALSE
#ifdef RFCOMM_STACK_ENABLE
#   define RFCOMM_INCLUDED   TRUE
#else
#   define RFCOMM_INCLUDED   FALSE
#endif // RFCOMM_STACK_ENABLE
#ifdef SPP_STACK_ENABLE
#   define SPP_INCLUDED   TRUE
#else
#   define SPP_INCLUDED   FALSE
#endif // SPP_STACK_ENABLE
#define TCS_INCLUDED   FALSE
#define BNEP_INCLUDED   FALSE
#define PAN_INCLUDED   FALSE
#define SAP_SERVER_INCLUDED   FALSE
#define HID_DEV_INCLUDED   FALSE
#ifdef HID_STACK_ENABLE
#define HID_HOST_INCLUDED   TRUE
#else
#define HID_HOST_INCLUDED   FALSE
#endif
#ifdef DEBUG_UHE
#   define UHE_DEBUG   TRUE
#else
#   define UHE_DEBUG   FALSE
#endif
#define SLIP_INCLUDED   FALSE
#define SLIP_STATIS_INCLUDED   FALSE
#define SLIP_SW_FLOW_CTRL   FALSE
#define BT_TRACE_SLIP   FALSE
#define SLIP_SLEEP_TO   5000
#define SLIP_HOST_SLIDING_WINDOW_SIZE   7
#define BTU_DUAL_STACK_MM_INCLUDED   FALSE
#define BTU_DUAL_STACK_BTC_INCLUDED   FALSE
#define BTU_BTC_SNK_INCLUDED   FALSE
#define BTU_STACK_LITE_ENABLED   FALSE
#define GPS_INCLUDED   FALSE
#ifndef AMP_INCLUDED
#define AMP_INCLUDED FALSE
#endif
#if defined(BLE_STACK_ENABLE)
#   define BLE_INCLUDED TRUE
#   define SMP_INCLUDED TRUE
#else
#   define BLE_INCLUDED FALSE
#   define SMP_INCLUDED FALSE
#endif
#define SMP_HOST_ENCRYPT_INCLUDED   FALSE
#define GATTS_APPU_USE_GATT_TRACE   FALSE
#define HID_LE_INCLUDED   FALSE
#define NFC_INCLUDED   FALSE
#define RW_NDEF_INCLUDED   FALSE
#define NFC_T1T_RW_ONLY   FALSE
#define NFC_RW_ONLY   FALSE
#define NFC_I2C_PATCH_INCLUDED   FALSE
#define CE_TEST_INCLUDED   FALSE
#define LLCP_SOCKET_NFCDEP_INCLUDED   FALSE
#define NFA_INCLUDED   FALSE
#define NFA_P2P_INCLUDED   FALSE
#define NFA_CHO_INCLUDED   FALSE
#define NFA_CHO_TEST_INCLUDED   FALSE
#define NFA_SNEP_INCLUDED   FALSE
#define NFA_HCI_INCLUDED   FALSE
#define NFC_BRCM_VS_INCLUDED   FALSE
#define HCILP_INCLUDED   FALSE
#define HCISU_H4_INCLUDED   FALSE
#define SER_INCLUDED   FALSE
#define RPC_INCLUDED   FALSE
#define BT_TRACE_PROTOCOL   FALSE
#define BT_USE_TRACES   FALSE
#define BTTRC_INCLUDED   FALSE
#define BTTRC_PARSER_INCLUDED   FALSE
#define MAX_TRACE_RAM_SIZE   10000
#define ICA_INCLUDED   FALSE
#define HSA_HS_INCLUDED   FALSE
#define HSA_AG_INCLUDED   FALSE
#define MMI_INCLUDED   FALSE
#define SAP_INCLUDED   FALSE
#define BTE_BTA_CODE_INCLUDED   FALSE
#define BTA_INCLUDED   FALSE
#define BTA_AG_INCLUDED   FALSE
#define BTA_CT_INCLUDED   FALSE
#define BTA_CG_INCLUDED   FALSE
#define BTA_DG_INCLUDED   FALSE
#define BTA_FT_INCLUDED   FALSE
#define BTA_OP_INCLUDED   FALSE
#define BTA_PR_INCLUDED   FALSE
#define BTA_SS_INCLUDED   FALSE
#define BTA_DM_INCLUDED   FALSE
#define BTA_BI_INCLUDED   FALSE
#define BTA_SC_INCLUDED   FALSE
#define BTA_PAN_INCLUDED   FALSE
#define BTA_FS_INCLUDED   FALSE
#define BTA_AC_INCLUDED   FALSE
#define BTA_HD_INCLUDED   FALSE
#define BTA_HH_INCLUDED   FALSE
#define BTA_HL_INCLUDED   FALSE
#define BTA_AR_INCLUDED   FALSE
#define BTA_AV_INCLUDED   FALSE
#define BTA_AVK_INCLUDED   FALSE
#define BTA_PBS_INCLUDED   FALSE
#define BTA_PBC_INCLUDED   FALSE
#define BTA_FM_INCLUDED   FALSE
#define BTA_FMTX_INCLUDED   FALSE
#define BTA_HS_INCLUDED   FALSE
#define BTA_MSE_INCLUDED   FALSE
#define BTA_MCE_INCLUDED   FALSE
#define BTA_PLAYBACK_INCLUDED   FALSE
#define BTA_SSR_INCLUDED   FALSE
#define BTA_BUSAPP_INCLUDED   FALSE
#define BTA_MIP_INCLUDED   FALSE
#define BTA_HH_LE_INCLUDED   FALSE
#define BTA_JV_INCLUDED   FALSE
#define SDP_RAW_DATA_INCLUDED   FALSE
#define BTA_EIR_CANNED_UUID_LIST   FALSE
#define BTA_GATT_INCLUDED   FALSE
#define RSI_INCLUDED   FALSE
#define RPC_TRACE_ONLY   FALSE

#if defined(A4WP_ENABLE)
/* Default name configured in BTM used for embedded mode in the BLE advertise */
#define BTM_USE_DEF_LOCAL_NAME          TRUE
#define BTM_DEF_LOCAL_NAME              "WPT PRU"
#endif

#define BTM_DEFAULT_CONN_INTERVAL 0x0800
#define BTM_PWR_MGR_INCLUDED   FALSE
#define BTM_SET_DEV_NAME_UPON_RESET FALSE
#define BTM_SEC_MAX_DEVICE_RECORDS   2
#define BTM_INQ_DB_SIZE   7
#define GKI_BUF0_SIZE           24
#define GKI_BUF1_SIZE           32
#define GKI_BUF2_SIZE           64
#define GKI_BUF3_SIZE           264

#define GKI_BUF0_MAX                6
#define GKI_BUF1_MAX                6
#if (MPAF_LIMIT_TO_SINGLE_HCI_CMD == FALSE)
#define GKI_BUF2_MAX                25
#define GKI_BUF3_MAX                4
#else
#define GKI_BUF2_MAX                20
#define GKI_BUF3_MAX                5
#endif
#define GKI_NUM_FIXED_BUF_POOLS     4
#define GKI_NUM_TOTAL_BUF_POOLS     4

#define TCS_BCST_SETUP_INCLUDED   FALSE
#define TCS_WUG_MEMBER_INCLUDED   FALSE

#define BTU_DYNAMIC_MEMORY   FALSE
#define DEV_MGR_TASK   6

#define GKI_MAX_TASKS  1

#define GATT_DB_POOL_ID                GKI_POOL_ID_2

/* Allocate smallest possible buffer (for platforms with limited RAM) */
#define  HCI_USE_VARIABLE_SIZE_CMD_BUF TRUE

/* Enable/Disable *_DYNAMIC_MEMORY here */
#if defined(MPAF_DYNAMIC_MEMORY)
#   define L2C_DYNAMIC_MEMORY          TRUE
#   define SDP_DYNAMIC_MEMORY          TRUE
#   define RFC_DYNAMIC_MEMORY          TRUE
#   define SPP_DYNAMIC_MEMORY          TRUE
#   define HID_DYNAMIC_MEMORY          TRUE
#   define MPAF_OTA_DYNAMIC_MEMORY     TRUE
#   define UHE_APP_DYNAMIC_MEMORY      TRUE
#   define SPP_APP_DYNAMIC_MEMORY      TRUE
#   define RC_APP_DYNAMIC_MEMORY       TRUE
#   define TVWAKE_APP_DYNAMIC_MEMORY   TRUE
#   define ATCE_APP_DYNAMIC_MEMORY     TRUE

#else // !MPAF_DYNAMIC_MEMORY

#   define SDP_DYNAMIC_MEMORY          FALSE
#   define L2C_DYNAMIC_MEMORY          FALSE
#   define HID_DYNAMIC_MEMORY          FALSE
#   define RFC_DYNAMIC_MEMORY          FALSE
#   define SPP_DYNAMIC_MEMORY          FALSE
#   define MPAF_OTA_DYNAMIC_MEMORY     FALSE

#   define UHE_APP_DYNAMIC_MEMORY      FALSE
#   define SPP_APP_DYNAMIC_MEMORY      FALSE
#   define RC_APP_DYNAMIC_MEMORY       FALSE
#   define TVWAKE_APP_DYNAMIC_MEMORY   FALSE
#   define ATCE_APP_DYNAMIC_MEMORY     FALSE

#endif // MPAF_DYNAMIC_MEMORY

#ifdef SPP_STACK_ENABLE
#   define SPP_SERVER_ENABLED          TRUE  
#   define SPP_CLIENT_ENABLED          TRUE 
#endif

#ifdef RFCOMM_STACK_ENABLE
#   define PORT_MAX_RFC_PORTS          31
#   define GAP_MAX_CONNECTIONS         1
#endif // RFCOMM_STACK_ENABLE

#ifdef SDPS_STACK_ENABLE
#   define SDP_SERVER_ENABLED          TRUE
#else
#   define SDP_SERVER_ENABLED          FALSE
#endif

#ifdef SDPC_STACK_ENABLE
#   define SDP_CLIENT_ENABLED          TRUE
#else
#   define SDP_CLIENT_ENABLED          FALSE
#endif

/* The maximum length, in bytes, of an attribute. */
#define SDP_MAX_ATTR_LEN            80

/* The maximum number of attribute filters supported by SDP databases. */
#define SDP_MAX_ATTR_FILTERS        7
#define SDP_MAX_UUID_FILTERS        2

/* The size of a scratchpad buffer, in bytes, for storing the response to an attribute request. */
//#define SDP_MAX_LIST_BYTE_COUNT     768
#if SDP_DYNAMIC_MEMORY == TRUE
/* The maximum number of simultaneous client and server connections. */
#   define SDP_MAX_CONNECTIONS     g_mpaf_config_Stack.sdp.maxConn
#   define SDP_MAX_RECORDS         g_mpaf_config_Stack.sdp.maxRecords
/* The maximum number of record handles retrieved in a search. */
#define SDP_MAX_DISC_SERVER_RECS    g_mpaf_config_Stack.sdp.maxDiscRecords
#else
/* 
 * This is set to 2 because the Apps using MPAF SDP API and SPP Stack could 
 * establish SDP connection at the same time.
 */
#   define SDP_MAX_CONNECTIONS         2
#   define SDP_MAX_RECORDS             2
#   define SDP_MAX_DISC_SERVER_RECS    2
#endif

/* The MTU size for the L2CAP configuration. */
#define SDP_MTU_SIZE                64

#ifdef B3_ROM_OPTIMIZATION
#   define HID_HOST_MAX_DEVICES    4
#else
#   define HID_HOST_MAX_DEVICES    5 /* 3 for UHE, 1 for RC, 1 reserved */
#endif

#if L2C_DYNAMIC_MEMORY == TRUE
#   define MAX_L2CAP_LINKS     g_mpaf_config_Stack.l2c.maxLinks
#   define MAX_L2CAP_CHANNELS  g_mpaf_config_Stack.l2c.maxChannels
#   define MAX_L2CAP_CLIENTS   g_mpaf_config_Stack.l2c.maxClients
#else
/*
 * Maximum number of apps which are expected to be 
 * active at the same time and are RFCOMM
 */
#   define MAX_APPS_OVER_RFCOMM    2
#   define MAX_L2CAP_CLIENTS       6
#   define MAX_L2CAP_LINKS         (HID_HOST_MAX_DEVICES + MAX_APPS_OVER_RFCOMM)
#   define MAX_L2CAP_CHANNELS      ((2 * HID_HOST_MAX_DEVICES)  /* 2 channels per connection: hid_int, and hid_data */\
                                  + SDP_MAX_CONNECTIONS /* One channel per SDP connection */\
                                  + MAX_APPS_OVER_RFCOMM /* One Channel per RFCOMM conn */)
#endif

#define L2CAP_DESIRED_LINK_ROLE (HCI_ROLE_MASTER)

#define L2CAP_MTU_SIZE      g_mpaf_config_Stack.l2c.mtuSize
#define  L2CAP_LINK_INACTIVITY_TOUT (g_mpaf_config_Stack.l2c.inactivityTOut)

#ifdef RFCOMM_STACK_ENABLE
#   if RFC_DYNAMIC_MEMORY == TRUE
#       define MAX_RFC_PORTS       g_mpaf_config_Stack.rfc.maxPorts
#       define MAX_BD_CONNECTIONS  g_mpaf_config_Stack.rfc.maxConn

#   else
#       define MAX_RFC_PORTS               1
#   endif
#endif // #ifdef RFCOMM_STACK_ENABLE

/* The SPP default MTU. */
#define SPP_DEFAULT_MTU             GKI_BUF3_SIZE

// --- stack customizations ---

// BTM
#define BTM_SCO_INCLUDED                FALSE
#define BTM_USE_INQ_RESULTS_FILTER      FALSE
#define BTM_SEC_FORCE_RNR_FOR_DBOND     FALSE
#define BTM_PWR_MGR_INCLUDED            FALSE
#define BTM_SCO_WAKE_PARKED_LINK        FALSE
#define BTM_BUSY_LEVEL_CHANGE_INCLUDED  TRUE

#if (MPAF_BLE_ONLY == FALSE)
#define BTM_EIR_SERVER_INCLUDED         TRUE    //WICED
#define BTM_EIR_CLIENT_INCLUDED         TRUE    //WICED
#else
#define BTM_EIR_SERVER_INCLUDED         FALSE
#define BTM_EIR_CLIENT_INCLUDED         FALSE
#endif
#define BTM_OOB_INCLUDED                FALSE
#define BTM_SSR_INCLUDED                FALSE
#define BTM_PRE_LISBON_INCLUDED         FALSE

#define BTM_LOCAL_IO_CAPS               BTM_IO_CAP_IO
#define BTM_SEC_SERVICE_NAME_LEN        0
#define BTM_INQ_GET_REMOTE_NAME         FALSE
#define BTM_MAX_LOC_BD_NAME_LEN         31
#define BTM_SEC_MAX_SERVICE_RECORDS     8

// BTM -- reintroduced old flags
#define BTM_AUTHORIZATION_INCLUDED      FALSE

// BTM -- new flags
#define MPAF_BTM_RLN_INCLUDED           FALSE
#define MPAF_BTM_RLINKP_INCLUDED        FALSE
#define MPAF_BTM_RRSSI_INCLUDED         FALSE
#define MPAF_BTM_RLINKQ_INCLUDED        FALSE
#define MPAF_BTM_RINQTXP_INCLUDED       FALSE
#define MPAF_BTM_QOSSU_INCLUDED         FALSE
#define MPAF_BTM_RTXPOWER_INCLUDED      FALSE
#define MPAF_BTM_RWDLINKKEY_INCLUDED    FALSE
#define MPAF_BTM_VSEREG_INCLUDED        FALSE
#define MPAF_BTM_CHGLK_INCLUDED         FALSE
#define MPAF_BTM_PERINQ_INCLUDED        FALSE
#if BTM_RNR_ENABLED == TRUE
#   define MPAF_BTM_DRNR_INCLUDED       FALSE
#define RNR_MEM_POOL_0                  32
#else
#   define MPAF_BTM_DRNR_INCLUDED       TRUE
#endif
#define MPAF_BTM_LSTOC_INCLUDED         FALSE
#define MPAF_BTM_DEFRESET_INCLUDED      FALSE
#define MPAF_BTM_RCLKOFFSET_INCLUDED    FALSE
#define MPAF_BTM_AFH_INCLUDED           FALSE
#define MPAF_BTM_MKEY_INCLUDED          FALSE
#define MPAF_BTM_SEC_FIXED_PIN_INCLUDED FALSE
#if (MPAF_BLE_ONLY == FALSE)
#define MPAF_BTM_RS_START_INCLUDED      TRUE
#else
#define MPAF_BTM_RS_START_INCLUDED      FALSE
#endif
#define MPAF_BTM_ACL_ERRCHK_INCLUDED    FALSE
#define MPAF_BTM_RRMTVER_INCLUDED       TRUE
#define MPAF_BTM_ACL_PAGEQ_INCLUDED     FALSE
#define MPAF_BTM_SP_DEBUG_INCLUDED      FALSE
#define MPAF_BTM_COMMDEFLP_INCLUDED     FALSE

//L2CAP
// Custom flags 
#define MPAF_L2CAP_BIDIR_QOS_INCLUDED       TRUE
#define MPAF_L2CAP_EXTD_FLOW_INCLUDED       TRUE
#define MPAF_L2CAP_FCS_INCLUDED             TRUE
#define MPAF_L2CAP_XMIT_HOLD_Q_INCLUDED     FALSE
#define MPAF_L2CAP_RCV_HOLD_Q_INCLUDED      FALSE
#define MPAF_L2CAP_LINK_XMIT_Q_INCLUDED     FALSE
#define MPAF_L2CAP_BUF_QUOTA_MGMT_INCLUDED  TRUE
#define MPAF_L2CAP_CHNL_PRIORITY_INCLUDED   FALSE
#define MPAF_L2CAP_ACL_PRIORITY_INCLUDED    FALSE

// Reconfiguration 
#define L2CAP_FCR_INCLUDED              FALSE
#define L2CAP_HOST_FLOW_CTRL            FALSE
#define L2CAP_WAKE_PARKED_LINK          FALSE
#define L2CAP_NON_FLUSHABLE_PB_INCLUDED FALSE
#define L2CAP_HCI_FLOW_CONTROL_DEBUG    FALSE
#define L2CAP_ROUND_ROBIN_CHANNEL_SERVICE   FALSE
#if !defined(BLE_STACK_ENABLE)
#   define L2CAP_NUM_FIXED_CHNLS        0
#endif

// BTU -- new flags
#define MPAF_BTU_CMDCMPLQ_INCLUDED      TRUE
#define MPAF_BTU_DUMMYEVTS_INCLUDED     FALSE

#if defined(BLE_STACK_ENABLE)
/* Optmization/feature flags */

#if defined(BLE_GATT_SERVER_ENABLE)
#define MPAF_GATTS_ENABLE                TRUE
#define MPAF_BTM_ADVERTISE_ENABLED       TRUE
#else
#define MPAF_GATTS_ENABLE                FALSE
#endif

#define MPAF_GATTC_ENABLE                TRUE

#if defined(BLE_HIDH_ENABLE)
#define BLE_HIDH_INCLUDED                TRUE
#else
#define BLE_HIDH_INCLUDED                FALSE
#endif

#if MPAF_GATTS_ENABLE == TRUE
#define MPAF_DIS_SRV_ENABLE              TRUE
#define MPAF_BAT_SRV_ENABLE              TRUE
#define MPAF_GAP_SRV_ENABLE              TRUE
#else
#define MPAF_DIS_SRV_ENABLE              FALSE
#define MPAF_BAT_SRV_ENABLE              FALSE
#define MPAF_GAP_SRV_ENABLE              FALSE
#endif

#if defined(A4WP_ENABLE)
#define A4WP_PRU_ENABLE                  TRUE
#define A4WP_PTU_ENABLE                  FALSE
/* Flag to enable independent testing of the A4WP profile application features
 Setting to FALSE will enable the command mode testing.
 */
#define A4WP_APP_STAND_ALONE_MODE        TRUE

// Flag to enable the A4WP FSM
#define A4WP_FSM_ENABLE                  TRUE

/* Non resolvable static random address support for A4WP */
#define MPAF_BTM_PRIVACY_ENABLED         TRUE
#else
#define MPAF_BTM_PRIVACY_ENABLED         FALSE
#endif

#else
#define BLE_HIDH_INCLUDED                FALSE
#endif /* BLE_STACK_ENABLE */

/* Performance optimization */
// this flag need to be enabled always. applcations are implemeted with this change
#define MPAF_ZERO_COPY_ACL_UP_PATH       TRUE

#define MPAF_LEGACY_TVWAKE_FEATURE       TRUE

#define MPAF_DISABLE_WARNING             TRUE

#ifndef MPAF_OPT_RAM_USAGE_FOR_A4WP

/* Long buffers for SDP and GATT attribute read operations */
#define MPAF_LONG_POOL_ENABLE            TRUE

#else
// RAM/ROM optimization changes:

// Long read is not a scenario in A4WP only
#define MPAF_LONG_POOL_ENABLE            FALSE

#define GATT_CL_MAX_LCB                  4

// Not used
#define GATT_MAX_SCCB                    1

#undef MPAF_BAT_SRV_ENABLE
#define MPAF_BAT_SRV_ENABLE              FALSE

// Reconfigure the GATT items
// Alternatively, this can be configured using the menuconfig for specific target
#undef GATT_MAX_SR_PROFILES
#undef GATT_MAX_PHY_CHANNEL
#undef GATT_MAX_BG_CONN_DEV
#undef GATT_MAX_ATTR_LEN

#define GATT_MAX_SR_PROFILES            4
#define GATT_MAX_PHY_CHANNEL            1
#define GATT_MAX_BG_CONN_DEV            1
#define GATT_MAX_ATTR_LEN               32

#undef GKI_BUF3_SIZE
#define GKI_BUF3_SIZE                   170

#undef GKI_BUF2_MAX
#undef GKI_BUF3_MAX
// Reconfigured the GKI buffers
#define GKI_BUF2_MAX                    16
#define GKI_BUF3_MAX                    4

#endif /* MPAF_OPT_RAM_USAGE_FOR_A4WP */

/*new bt stack configuration*/
#ifdef MPAF_USE_4_1_STACK
#undef     BTM_BUSY_LEVEL_CHANGE_INCLUDED
#undef     BTM_SCO_INCLUDED
#undef     L2CAP_MTU_SIZE
#undef     MAX_RFC_PORTS
#undef     MAX_BD_CONNECTIONS
#undef     A2D_INCLUDED
#undef     A2D_SBC_INCLUDED
#undef     A2D_M12_INCLUDED
#undef     A2D_M12_INCLUDED
#undef     A2D_M24_INCLUDED
#undef     AVDT_INCLUDED
#undef     AVRC_INCLUDED
#undef     AVCT_INCLUDED
#undef     GATT_MAX_APPS
#undef     GATT_MAX_SR_PROFILES
#undef     GATT_MAX_SR_PROFILES
#undef     GATT_MAX_PHY_CHANNEL
#undef     GATT_MAX_MTU_SIZE
#undef     GKI_NUM_FIXED_BUF_POOLS
#undef     GKI_NUM_TOTAL_BUF_POOLS
#undef     GKI_BUF0_SIZE
#undef     GKI_BUF0_MAX
#undef     GKI_BUF1_SIZE
#undef     GKI_BUF1_MAX
#undef     GKI_BUF2_SIZE
#undef     GKI_BUF2_MAX
#undef     GKI_BUF3_SIZE
#undef     GKI_BUF3_MAX
#undef     BTM_INQ_DB_SIZE
#undef     BTM_SEC_MAX_DEVICE_RECORDS
#undef     BTM_SEC_MAX_SERVICE_RECORDS
#undef     BTM_MAX_LOC_BD_NAME_LEN
#undef     MAX_TRACE_RAM_SIZE
#undef    SDP_MAX_CONNECTIONS
#undef    SDP_MAX_RECORDS
#undef    SDP_MAX_DISC_SERVER_RECS
#undef    SDP_MAX_ATTR_LEN
#undef    SDP_MAX_UUID_FILTERS
#undef    SDP_MAX_ATTR_FILTERS
#undef    L2C_DYNAMIC_MEMORY
#undef    SDP_DYNAMIC_MEMORY
#undef    SPP_DYNAMIC_MEMORY
#undef    RFC_DYNAMIC_MEMORY
#undef    MAX_L2CAP_CLIENTS
#undef    MAX_L2CAP_LINKS
#undef    MAX_L2CAP_CHANNELS
#undef    MPAF_ZERO_COPY_ACL_UP_PATH
#undef    L2CAP_UCD_INCLUDED
#undef    MPAF_LONG_POOL_ENABLE
#undef    L2CAP_FCR_INCLUDED
#undef    GATT_MAX_ATTR_LEN
#undef    L2CAP_LINK_INACTIVITY_TOUT
#undef    BTM_OOB_INCLUDED
#undef    HID_DEV_INCLUDED
#undef    HID_HOST_INCLUDED
#undef    HID_DYNAMIC_MEMORY

#define BTEWICED                    TRUE

#define BTM_CMD_POOL_ID                 GKI_POOL_ID_1
#define BTM_CLB_INCLUDED                FALSE
#define BTM_CLB_RX_INCLUDED             FALSE
#define BTM_TBFC_INCLUDED               FALSE
#define BTM_INQ_DB_INCLUDED             FALSE
#define BTM_BUSY_LEVEL_CHANGE_INCLUDED  FALSE
#define BTM_ALLOW_CONN_IF_NONDISCOVER   TRUE
#define BTM_MAX_REM_BD_NAME_LEN         10
#define BTM_DUMO_ADDR_CENTRAL_ENABLED   FALSE
#define BTM_SCO_INCLUDED                TRUE
#define BTM_SCO_HCI_INCLUDED            TRUE       /* TRUE includes SCO over HCI code */
#define BTM_INTERNAL_LINKKEY_STORAGE_INCLUDED   FALSE
#define BTM_BLE_PRIVACY_SPT                     TRUE
#define BTM_USE_CONTROLLER_PRIVATE_ADDRESS      TRUE
#define BTM_BLE_PRIVATE_ADDR_INT        (p_btm_cfg_settings->rpa_refresh_timeout)

/* When automatic inquiry scan is enabled, this sets the inquiry scan window. */
#ifndef BTM_DEFAULT_DISC_WINDOW
#define BTM_DEFAULT_DISC_WINDOW     0x0012
#endif

#ifdef MPAF_DYNAMIC_MEMORY
#define BTU_DYNAMIC_CB_INCLUDED         TRUE
#else
#define BTU_DYNAMIC_CB_INCLUDED         FALSE
#endif

#define L2CAP_CMD_POOL_ID               GKI_POOL_ID_1
#define L2CAP_FCR_INCLUDED              TRUE
#define L2CAP_UCD_INCLUDED              FALSE
#define L2CAP_MTU_SIZE                  ((UINT16)(HCI_ACL_POOL_BUF_SIZE - BT_HDR_SIZE - 8))     /* ACL bufsize minus BT_HDR, and l2cap/hci packet headers */
#define L2CAP_LE_COC_INCLUDED           TRUE
#define L2CAP_LINK_INACTIVITY_TOUT      3

#define SMP_LE_SC_INCLUDED              TRUE
#define SMP_LE_SC_OOB_INCLUDED          TRUE

#ifdef RFCOMM_STACK_ENABLE
#define RFCOMM_USE_EXTERNAL_SCN         TRUE
#define RFCOMM_CMD_POOL_ID              GKI_POOL_ID_1
#define RFCOMM_CMD_POOL_BUF_SIZE        (p_btm_cfg_buf_pools[RFCOMM_CMD_POOL_ID].buf_size)
#define RFCOMM_DATA_POOL_ID             GKI_POOL_ID_2
#define RFCOMM_DATA_POOL_BUF_SIZE       (p_btm_cfg_buf_pools[RFCOMM_DATA_POOL_ID].buf_size)
#if (defined(BTU_DYNAMIC_CB_INCLUDED)  && (BTU_DYNAMIC_CB_INCLUDED == TRUE))
#define MAX_RFC_PORTS                   (p_btm_cfg_settings->rfcomm_cfg.max_ports)
#define MAX_BD_CONNECTIONS              (p_btm_cfg_settings->rfcomm_cfg.max_links)
#else
#define MAX_RFC_PORTS                   1
#endif
#define PORT_RX_CRITICAL_WM             ((UINT32)(L2CAP_MTU_SIZE-L2CAP_MIN_OFFSET-RFCOMM_DATA_OVERHEAD)*PORT_RX_BUF_CRITICAL_WM)
#define PORT_RX_LOW_WM                  ((UINT32)(L2CAP_MTU_SIZE-L2CAP_MIN_OFFSET-RFCOMM_DATA_OVERHEAD)*PORT_RX_BUF_LOW_WM)
#define PORT_RX_HIGH_WM                 ((UINT32)(L2CAP_MTU_SIZE-L2CAP_MIN_OFFSET-RFCOMM_DATA_OVERHEAD)*PORT_RX_BUF_HIGH_WM)
#define PORT_RX_BUF_LOW_WM              2
#define PORT_RX_BUF_HIGH_WM             3
#define PORT_RX_BUF_CRITICAL_WM         5
#define PORT_TX_HIGH_WM                 ((UINT32)(L2CAP_MTU_SIZE-L2CAP_MIN_OFFSET-RFCOMM_DATA_OVERHEAD)*PORT_TX_BUF_HIGH_WM)
#define PORT_TX_CRITICAL_WM             ((UINT32)(L2CAP_MTU_SIZE-L2CAP_MIN_OFFSET-RFCOMM_DATA_OVERHEAD)*PORT_TX_BUF_CRITICAL_WM)
#define PORT_TX_BUF_HIGH_WM             3
#define PORT_TX_BUF_CRITICAL_WM         5
#define PORT_CREDIT_RX_LOW              2
#define PORT_CREDIT_RX_MAX              3
#endif

/* HID definitions */
#ifndef HID_DEV_INCLUDED
#define HID_DEV_INCLUDED                TRUE
#define HID_DEV_MAX_DESCRIPTOR_SIZE     200
#define HID_DEV_SET_CONN_MODE           FALSE
#endif

#ifndef HID_HOST_INCLUDED
#define HID_HOST_INCLUDED               TRUE
#define BR_HIDH_INCLUDED                TRUE
#define BLE_HIDH_INCLUDED               TRUE
#endif

/* AVDT/A2DP/AVRC definitions */
#define A2D_INCLUDED   TRUE
#define A2D_SBC_INCLUDED   TRUE
#define A2D_M12_INCLUDED   TRUE
#define A2D_M24_INCLUDED   TRUE
#define AVDT_INCLUDED   TRUE
#define AVDT_REPORTING                  FALSE
#define AVDT_MULTIPLEXING               FALSE
#define AVDT_NUM_LINKS                  (p_btm_cfg_settings->avdt_cfg.max_links)
#define AVDT_CMD_POOL_ID                GKI_POOL_ID_1
#define AVDT_DATA_POOL_ID               GKI_POOL_ID_3
#define AVDT_DATA_POOL_SIZE             (p_btm_cfg_buf_pools[AVDT_DATA_POOL_ID].buf_size)

#define AVRC_INCLUDED   TRUE
#define AVCT_INCLUDED   TRUE
#define AVCT_NUM_LINKS                  (p_btm_cfg_settings->avrc_cfg.max_links)
#define AVCT_NUM_CONN                   (avct_cb.num_conn)
#define AVRC_SEC_MASK                   (p_btm_cfg_settings->security_requirement_mask)
#define AVRC_CONTROL_MTU                (L2CAP_MTU_SIZE)
#define AVRC_BROWSE_MTU                 (L2CAP_MTU_SIZE)

#define GATT_FIXED_DB                   TRUE
#define GATT_MAX_APPS                   3
#define GATT_MAX_SR_PROFILES            3
#define GATT_MAX_ATTR_LEN               (p_btm_cfg_settings->gatt_cfg.max_attr_len)
#define GATT_MAX_MTU_SIZE               (p_btm_cfg_settings->gatt_cfg.max_mtu_size)
#define GATT_CL_MAX_LCB                 (p_btm_cfg_settings->gatt_cfg.client_max_links)
#define GATT_MAX_SCCB                   (p_btm_cfg_settings->gatt_cfg.server_max_links)

#define GATT_MAX_PHY_CHANNEL            (GATT_CL_MAX_LCB + GATT_MAX_SCCB)

#define GATTP_TRANSPORT_SUPPORTED       GATT_TRANSPORT_LE_BR_EDR
#define GATTC_NOTIF_TIMEOUT             3

#define SIM_ACCESS_INCLUDED             FALSE
#define SAP_CLIENT_INCLUDED             FALSE

#define BLE_BRCM_INCLUDED                 TRUE
#define BLE_DATA_LEN_EXT_INCLUDED       TRUE

#define GKI_NUM_FIXED_BUF_POOLS         WICED_BT_CFG_NUM_BUF_POOLS
#define GKI_NUM_TOTAL_BUF_POOLS         (p_btm_cfg_settings->max_number_of_buffer_pools)
#define GKI_BUF0_SIZE                   (p_btm_cfg_buf_pools[GKI_POOL_ID_0].buf_size)
#define GKI_BUF0_MAX                    (p_btm_cfg_buf_pools[GKI_POOL_ID_0].buf_count)
#define GKI_BUF1_SIZE                   (p_btm_cfg_buf_pools[GKI_POOL_ID_1].buf_size)
#define GKI_BUF1_MAX                    (p_btm_cfg_buf_pools[GKI_POOL_ID_1].buf_count)
#define GKI_BUF2_SIZE                   (p_btm_cfg_buf_pools[GKI_POOL_ID_2].buf_size)
#define GKI_BUF2_MAX                    (p_btm_cfg_buf_pools[GKI_POOL_ID_2].buf_count)
#define GKI_BUF3_SIZE                   (p_btm_cfg_buf_pools[GKI_POOL_ID_3].buf_size)
#define GKI_BUF3_MAX                    (p_btm_cfg_buf_pools[GKI_POOL_ID_3].buf_count)

#define GKI_DYNAMIC_POOL_CFG            TRUE
#define GKI_DYNAMIC_MEMORY              FALSE
#define GKI_USE_DYNAMIC_BUFFERS         TRUE

#define HCIC_INCLUDED                   TRUE
#define HCI_CMD_POOL_ID                 GKI_POOL_ID_1
#define HCI_CMD_POOL_BUF_SIZE           (p_btm_cfg_buf_pools[HCI_CMD_POOL_ID].buf_size)
#define HCI_ACL_POOL_ID                 GKI_POOL_ID_2
#define HCI_ACL_POOL_BUF_SIZE           (p_btm_cfg_buf_pools[HCI_ACL_POOL_ID].buf_size)
#define HCI_SCO_POOL_ID                 GKI_POOL_ID_1
#define HCI_SCO_POOL_BUF_SIZE           (p_btm_cfg_buf_pools[HCI_SCO_POOL_ID].buf_size)
#define HCI_USE_VARIABLE_SIZE_CMD_BUF   TRUE

#define BTM_INQ_DB_SIZE                 1
#define BTM_SEC_MAX_DEVICE_RECORDS      (p_btm_cfg_settings->max_simultaneous_links)
#define BTM_SEC_HOST_PRIVACY_ADDR_RESOLUTION_TABLE_SIZE    (p_btm_cfg_settings->addr_resolution_db_size)

#define BTM_SEC_MAX_SERVICE_RECORDS     4
#define BTM_SEC_SERVICE_NAME_LEN        0
#define BTM_MAX_LOC_BD_NAME_LEN         0
#define BTM_MAX_PM_RECORDS              1
#define BTM_MAX_VSE_CALLBACKS           1
#define BTM_BLE_MAX_BG_CONN_DEV_NUM     2
#define BTM_OOB_INCLUDED                TRUE
#define BTM_BR_SC_INCLUDED              TRUE
#define BTM_CROSS_TRANSP_KEY_DERIVATION TRUE
#define BT_BRCM_VS_INCLUDED             TRUE

#define MAX_TRACE_RAM_SIZE              10

#ifdef SDPS_STACK_ENABLE
#define SDP_RAW_DATA_SERVER             TRUE
#endif
#define SDP_MAX_LIST_BYTE_COUNT         (p_btm_cfg_buf_pools[SDP_POOL_ID].buf_size)
#ifdef SDPC_STACK_ENABLE
#define SDP_POOL_ID                     GKI_POOL_ID_3
#define SDP_MAX_CONNECTIONS             1
#define SDP_MAX_RECORDS                 3
#define SDP_MAX_REC_ATTR                8
#define SDP_MAX_UUID_FILTERS            3
#define SDP_MAX_ATTR_FILTERS            12
#define SDP_MAX_PROTOCOL_PARAMS         2
#endif

#define L2C_DYNAMIC_MEMORY          FALSE
#define SDP_DYNAMIC_MEMORY          FALSE
#define RFC_DYNAMIC_MEMORY          FALSE
#define SPP_DYNAMIC_MEMORY          FALSE
#define HID_DYNAMIC_MEMORY          FALSE

#if (defined(BTU_DYNAMIC_CB_INCLUDED)  && (BTU_DYNAMIC_CB_INCLUDED == TRUE))
#define MAX_L2CAP_CLIENTS               (btu_cb.l2c_cfg_max_clients)
#define MAX_L2CAP_LINKS                 (btu_cb.l2c_cfg_max_links)
#define MAX_L2CAP_CHANNELS              (btu_cb.l2c_cfg_max_channels)

/* Connection Oriented Channel configuration */
#define MAX_L2CAP_BLE_CLIENTS           (p_btm_cfg_settings->l2cap_application.max_le_psm)
#define MAX_L2CAP_BLE_CHANNELS          (p_btm_cfg_settings->l2cap_application.max_le_channels)

#else /* BTU_DYNAMIC_CB_INCLUDED  */
#define MAX_L2CAP_CLIENTS               3
#define MAX_L2CAP_LINKS                 1
#define MAX_L2CAP_CHANNELS              4
#endif /* BTU_DYNAMIC_CB_INCLUDED */

#define GAP_CONN_INCLUDED               FALSE

#define L2C_DEF_NUM_BLE_BUF_SHARED  1

#define MPAF_ZERO_COPY_ACL_UP_PATH      FALSE

#define WICED_BTE_LIB TRUE // btewiced defined this in embedded/wiced/lib/bluetooth_bte/bluetooth_bte.mk

#define MPAF_QUICKACCESS_API TRUE //too reduce boot initialization time

#endif /* MPAF_USE_4_1_STACK */

#ifdef __cplusplus
}
#endif

#endif

