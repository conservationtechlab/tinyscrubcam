//==================================================================================================
//                        THIS INFORMATION IS PROPRIETARY TO BROADCOM CORP
//--------------------------------------------------------------------------------------------------
//                               Copyright (c) 2011, 2012 Broadcom Corp.
//                                      ALL RIGHTS RESERVED
//==================================================================================================
//! \file
//!
//! Header for MPAF OTA internal utility function definitions
//
//==================================================================================================
#ifndef _MPAF_OTA_UTILS_H_
#define _MPAF_OTA_UTILS_H_

#include "stddef.h"  /* for offsetof() */
#include "mpaf.h"
#include "mpaf_ota.h"
#include "mpaf_pf.h"
#include "mpaf_pf_utils.h"

/*******************************************************************************
 *                   Defines & Macros
 *******************************************************************************/
#define GET_MPAF_OTASAP_INST_INFO_PTR_FROM_INQ_INFO_PTR( pInqInfo )  \
    (( MPAF_OTASAP_INST_INFO * ) \
        ((UINT32)(pInqInfo) - offsetof( MPAF_OTASAP_INST_INFO, inqInfo )));

/*
 * MPAF_OTASAP_STATE_TYPE
 * States of OTA SAP
 */
typedef enum
{
    MPAF_OTASAP_STATE_INIT, MPAF_OTASAP_STATE_STACK_INITIALIZING, MPAF_OTASAP_STATE_STACK_INIT_CMPL,

    MPAF_OTASAP_STATE__MAX
} MPAF_OTASAP_STATE_TYPE;

/*
 * MPAF_OTA_MODULE_INIT_TYPE
 * Bit indicating that a module has been initialized
 */
typedef enum
{
    MPAF_OTA_SDP_INIT, // 0

} MPAF_OTA_MODULE_INIT_TYPE;

#define MPAF_OTA_IS_SDP_INITIALIZED() \
    (otaSapCfg.moduleInitMask & (1 << MPAF_OTA_SDP_INIT))
#define MPAF_OTA_SET_SDP_INIT() \
    (otaSapCfg.moduleInitMask |= (1 << MPAF_OTA_SDP_INIT))
#define MPAF_OTA_RESET_SDP_INIT() \
    (otaSapCfg.moduleInitMask &= ~(1 << MPAF_OTA_SDP_INIT))

// if otaSapCfg.pSdpInfo is not equal to NULL, then MPAF SDP is in progress.
#define MPAF_SDP_IN_PROGRESS()    (otaSapCfg.pSdpInfo?TRUE:FALSE)

/*******************************************************************************
 *                   Type Definitions
 *******************************************************************************/

typedef struct MPAF_OTASAP_PAGESCAN_PARAM_TYPE
{
        UINT16 window;
        UINT16 interval;
} MPAF_OTASAP_PAGESCAN_PARAM;

typedef MPAF_OTASAP_PAGESCAN_PARAM MPAF_OTASAP_INQSCAN_PARAM;

typedef struct MPAF_OTASAP_DEV_INFO_TYPE
{
        MPAF_SAP_HANDLE sapHndl;
        MPAF_DEV_INFO devInfo;
        BOOL8 isValid;
} MPAF_OTASAP_DEV_INFO;

/* 
 * MPAF_OTASAP_CFG
 * Internal Config info of the OTA SAP.
 * This info is used for initialization and also to service App requests.
 */
typedef struct MPAF_OTASAP_CFG_TYPE
{
        slist_node_t initList;
        slist_node_t inqList;
        slist_node_t sdpList;

        MPAF_SAP_HANDLE appList[ MPAF_APP_ID_MAX ];

        // Settings/Parameters
        // MPAF_OTASAP_SEC_SETTING    secSetting;
        MPAF_OTASAP_PAGESCAN_PARAM pageScanParam[ 2 ];
        MPAF_OTASAP_INQSCAN_PARAM inqScanParam;
        UINT32 aclIdleTO;

        // housekeeping variables
        MPAF_OTASAP_STATE_TYPE state;
        MPAF_SAP_HANDLE lockHndl;
        MPAF_SAP_HANDLE inqHndl;
        MPAF_OTASAP_SDP_INFO *pSdpInfo;
        tSDP_DISCOVERY_DB *pSdpDb;

        UINT32 activeMask;
        UINT32 connectabilityMask[ 2 ];
        UINT32 discoverabilityMask;

        UINT8 moduleInitMask;
} MPAF_OTASAP_CFG;

typedef tBTM_FILTER_CB * MPAF_OTA_CONN_REQ_CB_FP;
typedef tBTM_BL_CHANGE_CB * MPAF_OTA_BTM_BL_CB_FP;
typedef tBTM_EVENT_CBACK * MPAF_OTA_BTM_MGMT_CB_FP;

#if( BTM_RNR_ENABLED == TRUE )
#ifndef SKIP_TO_COMPILE_BTE_STACK
typedef tBTM_RNR_DONE_CB * MPAF_OTA_RNR_CB_FP;
#endif
#endif
typedef tSDP_DISC_CMPL_CB * MPAF_OTA_SDP_CMPL_CB_FP;
#ifdef MPAF_LEAPP_ENABLE
typedef tBTM_CMPL_CB * MPAF_OTA_LE_L2CAP_SIG_CB_FP;
#endif

typedef struct MPAF_OTA_CB_INFO_TYPE
{
        MPAF_OTA_CONN_REQ_CB_FP connReqCb;
        MPAF_OTA_BTM_BL_CB_FP btmBusyLevelCb;

        // SKIP_TO_COMPILE_BTE_STACK Nara porting
        MPAF_OTA_BTM_MGMT_CB_FP p_btm_mgmt_callback; /* BTM Security management call back */

#if( BTM_RNR_ENABLED == TRUE )
#ifndef SKIP_TO_COMPILE_BTE_STACK
        MPAF_OTA_RNR_CB_FP rnrCb;
#endif
#endif
        MPAF_OTA_SDP_CMPL_CB_FP sdpCb;
#ifdef MPAF_LEAPP_ENABLE
        MPAF_OTA_LE_L2CAP_SIG_CB_FP leL2capSigCb;
#endif
} MPAF_OTA_CB_INFO;

/*******************************************************************************
 *                   Global Declarations
 *******************************************************************************/
extern MPAF_OTASAP_SEC_SETTING otaCfgSecSetting;
extern MPAF_OTASAP_CFG otaSapCfg;
extern const UINT8 INVALID_LINK_KEY[ ];
extern MPAF_OTA_CB_INFO otaCb;

/*******************************************************************************
 *                   Function Prototypes
 *******************************************************************************/
void mpaf_otaInit( void );
void mpaf_otaDeinit( void );
BOOL32 mpaf_otaSapIsEnabled( void );

/*******************************************************************************
 * Function: mpaf_otaPostFWInit
 *
 * Abstract:  Framework calls this function to init the embedded stack
 *
 * Input : none
 *
 * Output: none.
 *
 * Return: status
 *
 *******************************************************************************/
void mpaf_otaStackInit( void );

/*******************************************************************************
 * Function: mpaf_otaSapInqCmplCb
 *
 * Abstract: OTASAP registers this cb for inquiry complete event with the BTM.
 *
 * Input : event - the inquiry complete event.
 *
 * Output: none
 *
 * Return: none
 *
 *******************************************************************************/
void mpaf_otaSapInqCmplCb( void *event );

/*******************************************************************************
 * Function: mpaf_otaSapConnReqCb
 *
 * Abstract: Called when connection is requested (HCI CONNECTION REQUEST event)
 *           OTASAP registers this fn with the BTM for the Callback.
 *
 * Input : bdAddr - Bd Addr of the remote device which initiated the conn.
 *         cod - Class of Device of the remote device.
 *
 * Output: none.
 *
 * Return: TRUE to accept the connection (only if device is paired)
 *         FALSE to reject connection
 *           (if unpaired device, or max devices already connected)
 *
 *******************************************************************************/
UINT8 mpaf_otaSapConnReqCb( BD_ADDR bdAddr, DEV_CLASS cod );

/*******************************************************************************
 * Function: mpaf_otaSapAuthorizeCb
 *
 * Abstract: OTASAP registers this cb for Authorization req event with the BTM.
 *
 * Input : bd_addr   - Bd Addr of the remote device which initiated the conn.
 *         cod - COD of the remote device
 *         bd_name   - Name of the remote device
 *         service_name  -
 *         service_id    -
 *         is_originator -
 *
 * Output: none
 *
 * Return: BTM_SUCCESS
 *
 *******************************************************************************/
UINT8 mpaf_otaSapAuthorizeCb( BD_ADDR bd_addr, DEV_CLASS cod, tBTM_BD_NAME bd_name, UINT8 *service_name, UINT8 service_id, BOOLEAN is_originator );

/*******************************************************************************
 * Function: mpaf_otaSapPinCb
 *
 * Abstract: OTASAP registers this fn as cb for Pin key request by default.
 *
 * Input : bd_addr - Bd Addr of the remote device which initiated the conn.
 *         dev_class - COD of the remote device
 *         bd_name - Name of the remote device
 *
 * Output: none.
 *
 * Return: BTM_SUCCESS
 *
 *******************************************************************************/
UINT8 mpaf_otaSapPinCb( BD_ADDR bd_addr, DEV_CLASS dev_class, tBTM_BD_NAME bd_name );

/*******************************************************************************
 * Function: mpaf_otaSapLinkkeyReqCb
 *
 * Abstract: OTASAP registers this fn as cb for linkkey request by default.
 *
 * Input : bd_addr - Bd Addr of the remote device which initiated the conn.
 *         key - linkkey
 *
 * Output: none.
 *
 * Return: BTM_SUCCESS - if linkkey is present
 *         BTM_UNKNOWN_ADDR - otherwise
 *
 *******************************************************************************/
UINT8 mpaf_otaSapLinkkeyReqCb( BD_ADDR bd_addr, LINK_KEY key );

/*******************************************************************************
 * Function: mpaf_otaSapLinkkeyCb
 *
 * Abstract: OTASAP registers this fn as cb for linkkey event by default.
 *
 * Input : bd_addr - Bd Addr of the remote device which initiated the conn.
 *         dev_class - COD of the remote device
 *         bd_name - Name of the remote device
 *         key - linkkey generated for the connection
 *         key_type - type of the linkkey
 *
 * Output: none.
 *
 * Return: BTM_SUCCESS
 *
 *******************************************************************************/
UINT8 mpaf_otaSapLinkkeyCb( BD_ADDR bd_addr, DEV_CLASS dev_class, tBTM_BD_NAME bd_name, LINK_KEY key, UINT8 key_type );

/*******************************************************************************
 * Function: mpaf_otaSapAuthCmplCb
 *
 * Abstract: OTASAP registers this fn as cb for Authentication complete event
 *
 * Input : bd_addr - Bd Addr of the remote device which initiated the conn.
 *         dev_class - COD of the remote device
 *         bd_name - Name of the remote device
 *         result - Authentication Status
 *
 * Output: none.
 *
 * Return: BTM_SUCCESS
 *
 *******************************************************************************/
UINT8 mpaf_otaSapAuthCmplCb( BD_ADDR bd_addr, DEV_CLASS dev_class, tBTM_BD_NAME bd_name, int result );

/*******************************************************************************
 * Function: mpaf_otaSapAbortCb
 *
 * Abstract: OTASAP registers this fn as cb for Abort event
 *
 * Input : bd_addr - Bd Addr of the remote device which initiated the conn.
 *         dev_class - COD of the remote device
 *         bd_name - Name of the remote device
 *
 * Output: none.
 *
 * Return: BTM_SUCCESS
 *
 *******************************************************************************/
UINT8 mpaf_otaSapAbortCb( BD_ADDR bd_addr, DEV_CLASS dev_class, tBTM_BD_NAME bd_name );

#if (BTM_LISBON_INCLUDED == TRUE)
/*******************************************************************************
 * Function: mpaf_otaSapSimplePairingCb
 *
 * Abstract: OTASAP registers this fn as cb for Simple Pairing events
 *
 * Input : event - Simple Pairing event type
 *         p_data - Simple Pairing event data
 *
 * Output: none.
 *
 * Return: BTM_SUCCESS
 *
 *******************************************************************************/
#ifndef SKIP_TO_COMPILE_BTE_STACK
UINT8 mpaf_otaSapSimplePairingCb( tBTM_SP_EVT event, tBTM_SP_EVT_DATA *p_data );
#endif
#endif /* (BTM_LISBON_INCLUDED == TRUE) */

#if (BLE_INCLUDED == TRUE)
#if (SMP_INCLUDED == TRUE)

/*******************************************************************************
 * Function: mpaf_otaSapLeSimplePairingCb
 *
 * Abstract: OTASAP registers this fn as cb for LE Simple Pairing events
 *
 * Input : event - LE Simple Pairing event type
 *         bda   - BD address of the remote device.
 * Output:
 *         pData - LE Simple Pairing event data
 *
 * Return: BTM_SUCCESS
 *
 *******************************************************************************/
#ifndef SKIP_TO_COMPILE_BTE_STACK
UINT8 mpaf_otaSapLeSimplePairingCb(tBTM_LE_EVT event, BD_ADDR bda, tBTM_LE_EVT_DATA *p_data);
#endif
#endif /* (SMP_INCLUDED == TRUE) */

/*******************************************************************************
 * Function: mpaf_otaSapLeKeyCb
 *
 * Abstract: OTASAP registers this fn as cb for LE local Keys
 *
 * Input : key_type - Type of the key.
 *         p_key    - Key details.
 * Output: none.
 *
 * Return: none.
 *
 *******************************************************************************/
#ifndef SKIP_TO_COMPILE_BTE_STACK
void mpaf_otaSapLeKeyCb(UINT8 key_type, tBTM_BLE_LOCAL_KEYS *p_key);
#endif

/*******************************************************************************
 * Function: mpaf_otaSapLeSelConnCb
 *
 * Abstract: OTASAP registers this fn as cb for LE incoming connections from BG
 *           list devices
 *
 * Input : bda   - BD address of the remote device.
 *         p_remote_name - LE device name
 *
 * Return: TRUE to accept connection, FALSE otherwise
 *
 *******************************************************************************/
BOOLEAN mpaf_otaSapLeSelConnCb(BD_ADDR bda, UINT8 *p_remote_name);
#endif /* (BLE_INCLUDED == TRUE) */

/*******************************************************************************
 * Function: mpaf_otaSapBtmBusyLevelCb
 *
 * Abstract: OTASAP registers this fn as cb for BTM busy level  event.
 *
 * Input : tBTM_BL_EVENT_DATA
 *
 * Output: none.
 *
 * Return: none
 *
 *******************************************************************************/
void mpaf_otaSapBtmBusyLevelCb( tBTM_BL_EVENT_DATA *p_data );

// SKIP_TO_COMPILE_BTE_STACK Nara porting
tBTM_STATUS mpaf_otaSapBtmMgmtCb( tBTM_EVENT event, tBTM_EVENT_DATA *p_event_data );

/*******************************************************************************
 * Function: mpaf_otaSapRNRCb
 *
 * Abstract: OTASAP registers this fn as cb for Remote Name Req Complete
 *
 * Input : bdAddr - Bluetooth Address of the peer device
 *         bdName - Remote Name
 *.........status - Status of the RNR
 *
 * Output: none.
 *
 * Return: none
 *
 *******************************************************************************/
void mpaf_otaSapRNRCb( BD_ADDR bdAddr, UINT8* bdName, UINT8 status );

/*******************************************************************************
 * Function: mpaf_otaSapGetDeviceInfoPtr
 *
 * Abstract: This function returns the entry with the matching bdaddr
 *
 * Input : bdAddr - Bluetooth Address of the device to be searched.
 *
 * Output: pNvTag: Nv Tag for the bdAddr is returned.
 *
 * Return: Device List Entry of the next free entry or the matching entry.
 *
 *******************************************************************************/
MPAF_OTASAP_DEV_INFO * mpaf_otaSapGetDeviceInfoPtr( BD_ADDR bdAddr, UINT32 *pNvTag );

/*******************************************************************************
 * Function: mpaf_otaSapPostStackInitCb
 *
 * Abstract: This function indicates to the OtaSap that the stack is initialized.
 *
 * Input : none
 *
 * Output: none.
 *
 * Return: none
 *
 *******************************************************************************/
void mpaf_otaSapPostStackInitCb( void );

/*******************************************************************************
 * Function: mpaf_otaSapPostStackInitComplete
 *
 * Abstract: This function is called after Stack Init is complete and it will trigger the cb to the apps.
 *
 * Input : none
 *
 * Output: none.
 *
 * Return: none
 *
 *******************************************************************************/
void mpaf_otaSapPostStackInitComplete( void );

/*******************************************************************************
 * Function: mpaf_otaSapTriggerPostStackInitCb
 *
 * Abstract: This function indicates to the application that the stack is initialized.
 *
 * Input : none
 *
 * Output: none.
 *
 * Return: none
 *
 *******************************************************************************/
void mpaf_otaSapTriggerPostStackInitCb( void );

#if (BTM_LISBON_INCLUDED == TRUE)

#ifndef SKIP_TO_COMPILE_BTE_STACK
void mpaf_otaSapIoCapReq( MPAF_OTASAP_INST_INFO *pInstInfo, tBTM_SP_EVT_DATA *p_data );

void mpaf_otaSapUserCfmReq( MPAF_OTASAP_INST_INFO *pInstInfo, tBTM_SP_EVT_DATA *p_data );

void mpaf_otaSapSspCmpl( MPAF_OTASAP_INST_INFO *pInstInfo, tBTM_SP_EVT_DATA *p_data );
#endif

#endif

void mpaf_otaSapInqNext( void );

/*******************************************************************************
 **
 ** Function         mpaf_otaStackEvtHandler
 **
 ** Description      The applications can register to receive events before the
 **                  BTU stack/BTM gets them. This case is applicable only after Stack Initialization
 **
 ** Returns          TRUE
 **                      Event will be processed by the App and not sent to stack.
 **
 **                  FALSE
 **                      Event will be passed on to the Stack/BTU/BTM
 **
 *******************************************************************************/
BOOL32 mpaf_otaStackEvtHandler( const BTHCI_EVENT_HDR_t *p_event );

// SDP related 
MPAF_STATUS mpaf_otaSapSendSdpReq( MPAF_OTASAP_SDP_INFO *pSdpInfo );
void mpaf_sdp_cb( UINT16 sdpResult );
#ifdef MPAF_LEAPP_ENABLE
void mpaf_leL2capSig_cb(void * p);
#endif
void mpaf_otaSapSdpNext( void );
MPAF_STATUS mpaf_sdpInit( void );
MPAF_STATUS mpaf_sdpDeinit( void );

const UINT8* SDP_GetRawServicesPointer( void );
UINT16 SDP_GetRawServicesLength( void );
void SDP_Done( void );

// HCI related
void mpaf_hci_SetInqPowerLevel( INT8 powerLevel );

#endif   //_MPAF_OTA_UTILS_H_

