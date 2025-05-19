//==================================================================================================
//                        THIS INFORMATION IS PROPRIETARY TO BROADCOM CORP
//--------------------------------------------------------------------------------------------------
//                               Copyright (c) 2011, 2012 Broadcom Corp.
//                                      ALL RIGHTS RESERVED
//==================================================================================================
//! \file
//!
//! Header for MPAF OTA Interface definitions
//
//==================================================================================================
#ifndef _MPAF_OTA_H_
#define _MPAF_OTA_H_

#include "mpaf.h"
#include "mpaf_pf.h"
#include "btm_api.h"
#include "hcimsgs.h"
#include "l2c_api.h"

#if (BLE_INCLUDED == TRUE)
#include "btm_ble_api.h"
#include "srvc_api.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*                   Defines & Macros
*******************************************************************************/

#define mpaf_otaSapSetAclIdleTimeout(hdl, to)   L2CA_SetIdleTimeout(0,to,TRUE)
#define mpaf_otaSapSetLinkSuperTout(hdl,bda,to) BTM_SetLinkSuperTout(bda,to)
#define mpaf_otaSapSetInquiryMode(hdl,mode)     BTM_SetInquiryMode(mode)
#define mpaf_otaSapReadConnectability(hdl,w,i)  BTM_ReadConnectability(w,i)
#define mpaf_otaSapSetPageScanType(hdl,type)    BTM_SetPageScanType(type)
#define mpaf_otaSapSetInquiryScanType(hdl,type) BTM_SetInquiryScanType(type)
#define mpaf_otaSapSetDefaultLinkPolicy(hdl,s)  BTM_SetDefaultLinkPolicy(s)
#define mpaf_otaSapClearAclIdleTimeout(hdl)     L2CA_ClearAclIdleTimeout()
#define mpaf_otaSapIsInquiryActive(hdl)         BTM_IsInquiryActive()
#define mpaf_otaSapReadDiscoverability(hdl,w,i) BTM_ReadDiscoverability (w,i)
#define mpaf_otaSapSetSecurityLevel(hdl,m,n,o,p,q,r,s)    \
                                                BTM_SetSecurityLevel(m,n,o,p,q,r,s)
#ifndef MPAF_STACK_MEM_SAVE
#define mpaf_otaSapBleBroadcast(b)              BTM_BleBroadcast(b);
#endif // #ifndef MPAF_STACK_MEM_SAVE
#define mpaf_otaSapBleSetAdvParams(a, b, c, d)  BTM_BleSetAdvParams(a, b, c, d)
#define mpaf_otaSapBleWriteAdvData(dm, ad)      BTM_BleWriteAdvData(dm, ad)
#define mpaf_otaSapBleWriteScanRsp(dm, ad)      BTM_BleWriteScanRsp(dm, ad)
#define mpaf_otaSapSetLocalDeviceName(n)        BTM_SetLocalDeviceName(n)
#define mpaf_otaSapBleSetRandomAddr             BTM_BleSetRandomAddr
#define mpaf_otaSapBleWriteRandomAddrToController(addr) BTM_BleWriteRandomAddrToController(addr)

#define MPAF_OTASAP_NORMAL_CONN_WINDOW      BTM_DEFAULT_CONN_WINDOW
#define MPAF_OTASAP_NORMAL_CONN_INTERVAL    BTM_DEFAULT_CONN_INTERVAL
#define MPAF_OTASAP_WIDE_CONN_WINDOW        18
#define MPAF_OTASAP_WIDE_CONN_INTERVAL      36

// SDP related
#define MPAF_OTA_SDP_MAX_DB_LEN                 1024

/*******************************************************************************
*                   Type Definitions
*******************************************************************************/
/*
 * MPAF_OTA_AUTH_EVENT
 * various authentication events indicated by the OTA to the Application.
 */
typedef enum 
{
    MPAF_OTA_AUTH_EVENT__LINKKEY_REQ,
    MPAF_OTA_AUTH_EVENT__LINKKEY_CB,
    MPAF_OTA_AUTH_EVENT__PINKEY_REQ,
    MPAF_OTA_AUTH_EVENT__AUTHORIZE_REQ,
    MPAF_OTA_AUTH_EVENT__ABORT_CB,
    MPAF_OTA_AUTH_EVENT__SSP_IO_REQ,
    MPAF_OTA_AUTH_EVENT__SSP_IO_RSP,
    MPAF_OTA_AUTH_EVENT__SSP_CFM_REQ,
    MPAF_OTA_AUTH_EVENT__SSP_KEY_NOTIF,
    MPAF_OTA_AUTH_EVENT__SSP_KEY_REQ,
    MPAF_OTA_AUTH_EVENT__SSP_KEYPRESS,
    MPAF_OTA_AUTH_EVENT__SSP_LOC_OOB,
    MPAF_OTA_AUTH_EVENT__SSP_REM_OOB,
    MPAF_OTA_AUTH_EVENT__SSP_CMPL,
    MPAF_OTA_AUTH_EVENT__SSP_UPGRADE,
    MPAF_OTA_AUTH_EVENT__CMPL,

#ifdef MPAF_CUSTOM_STACK    
    MPAF_OTA_AUTH_EVENT__LE_SEC_REQUEST,
#endif
    MPAF_OTA_AUTH_EVENT__INVALID
}MPAF_OTA_AUTH_EVENT;

/**
 * Connectable Mode of the device. The page scan parameters are chosen based on
 * this mode. The applications shall prefer these modes over altering the page
 * scan parameters using the BTM_SetConnectability() API.
 */
typedef enum
{
    MPAF_OTASAP_CONNECTABLE_MODE_NONE,      /**< Don't listen for connections. */
    MPAF_OTASAP_CONNECTABLE_MODE_NORMAL,    /**< Use the default page scan parameters. */
    MPAF_OTASAP_CONNECTABLE_MODE_WIDE,      /**< Use a larger/wider page scan window
                                                 for faster connection performance. */
    MPAF_OTASAP_CONNECTABLE_MODE_LAST
} MPAF_OTASAP_CONNECTABLE_MODE;

#if (BTM_LISBON_INCLUDED == TRUE)
/*
 * MPAF_OTASAP_SSP_EVT_DATA
 * SSP event data type which is inturn an union.
 */
#ifndef SKIP_TO_COMPILE_BTE_STACK
typedef tBTM_SP_EVT_DATA MPAF_OTASAP_SSP_EVT_DATA;
#endif
#endif

/*
 * MPAF_OTASAP_AUTH_EVENT_DATA
 * Union Data passed to the application along with the auth event.
 * Some of the events share the same data type and hence it is not repeated below.
 */
typedef union MPAF_OTASAP_AUTH_EVENT_DATA_TYPE
{
    LINK_KEY *pKey;// MPAF_OTA_AUTH_EVENT__LINKKEY_REQ,
    BOOL8 *pbAutopaired; //MPAF_OTA_AUTH_EVENT__LINKKEY_CB,
    DEV_CLASS_PTR pCod; //MPAF_OTA_AUTH_EVENT__PINKEY_REQ,
    UINT32 serviceId; // MPAF_OTA_AUTH_EVENT__AUTHORIZE_REQ,
    // DEV_CLASS_PTR pCod; //MPAF_OTA_AUTH_EVENT__ABORT_CB,  Member already Exists
#ifndef SKIP_TO_COMPILE_BTE_STACK
#if (BTM_LISBON_INCLUDED == TRUE)
    MPAF_OTASAP_SSP_EVT_DATA *pSspData; // For all SSP events.
#endif
#endif
    MPAF_STATUS status; // MPAF_OTA_AUTH_EVENT__CMPL,
#ifndef SKIP_TO_COMPILE_BTE_STACK// nara le_evt_data unused now
#ifdef MPAF_LEAPP_ENABLE
    tBTM_LE_EVT_DATA *le_evt_data;
#endif
#endif    
    
}MPAF_OTASAP_AUTH_EVENT_DATA;

/*
 * MPAF_OTASAP_SEC_CB_INFO
 * Security callbacks to be registered by application.
 */
#ifndef SKIP_TO_COMPILE_BTE_STACK
//#ifndef BTEWICED //NARA actual fix
typedef tBTM_APPL_INFO  MPAF_OTASAP_SEC_CB_INFO;
//#endif
#endif

/*
 * MPAF_OTASAP_INQ_PARAM
 * input parmeters to the inquiry request.
 */
typedef tBTM_INQ_PARMS  MPAF_OTASAP_INQ_PARAM;

/*
 * MPAF_OTASAP_UUID
 * UUID struct for SDP.
 */
typedef tSDP_UUID MPAF_OTASAP_UUID;

/*
 * MPAF_OTASAP_SDP_PARAM
 * input parmeters to the SDP request.
 */
typedef struct
{
    BD_ADDR bda;
    UINT8 numUuid;
    UINT8 numAttr;
    MPAF_OTASAP_UUID *pUuid;
    UINT16 *pAttr;
}MPAF_OTASAP_SDP_PARAM;

/*
 * MPAF_ROLE_SWITCH_CMPL
 * Role Switch Event data.
 */
typedef tBTM_BL_ROLE_CHG_DATA MPAF_ROLE_SWITCH_CMPL;

/*
 * MPAF_OTASAP_POST_STACK_INIT_CB_FP
 * Function prototype for the post Stack Init CB.
 */
typedef void (*MPAF_OTASAP_POST_STACK_INIT_CB_FP)( void );

/*
 * MPAF_OTASAP_CONN_STATUS_IND_CB_FP
 * Function prototype for the Connection Status Indication callbacks.
 */
typedef void (*MPAF_OTASAP_CONN_STATUS_IND_CB_FP)( BD_ADDR bdAddr, 
                                                   DEV_CLASS cod, 
                                                   BD_NAME bdName, 
                                                   BOOL8 bNew );

/*
 * MPAF_OTASAP_CONN_REQ_CB_FP
 * Function prototype for the Connection Request Callback.
 */
typedef UINT32 (*MPAF_OTASAP_CONN_REQ_CB_FP)( BD_ADDR bdAddr,
                                            DEV_CLASS cod, UINT32 nvTag);

/*
 * MPAF_OTASAP_AUTH_STATUS_CB_FP
 * Function prototype for the Authentication Status Callback.
 */
typedef MPAF_STATUS (*MPAF_OTASAP_AUTH_STATUS_CB_FP)(
                                                 MPAF_OTA_AUTH_EVENT event,
                                                 BD_ADDR bdAddr,
                                                 MPAF_OTASAP_AUTH_EVENT_DATA data);

/*
 * MPAF_OTASAP_ROLE_CHANGE_IND_CB_FP
 * Function prototype for the Role Change Indication Callback.
 */
typedef void (*MPAF_OTASAP_ROLE_CHANGE_IND_CB_FP)( 
                                          MPAF_ROLE_SWITCH_CMPL *pRoleChgCmpl );

/*
 * MPAF_OTASAP_STACK_ACCESS_CB_FP
 * Function prototype for the Stack Access Granted Callback
 */
typedef void (*MPAF_OTASAP_STACK_ACCESS_CB_FP)( MPAF_STATUS status );

/*
 * MPAF_OTASAP_INQUIRY_RESULT_CB_FP
 * Function prototype for the Inquiry Result Callback
 */
typedef void (*MPAF_OTASAP_INQUIRY_RESULT_CB_FP)(
                                 tBTM_INQ_RESULTS *p_inq_results, UINT8 *p_eir);

/*
 * MPAF_OTASAP_INQUIRY_CMPL_CB_FP
 * Function prototype for the Inquiry Complete Callback
 * event will be in the format tBTM_INQUIRY_CMPL
 */
typedef void (*MPAF_OTASAP_INQUIRY_CMPL_CB_FP)( void *event ); 

#if( BTM_RNR_ENABLED == TRUE )
/*
 * MPAF_OTASAP_RNR_CB_FP
 * Function prototype for the Remote Name Request Complete Callback
 * event will be in the format tBTM_INQUIRY_CMPL
 */
#ifndef SKIP_TO_COMPILE_BTE_STACK
typedef tBTM_RNR_DONE_CB * MPAF_OTASAP_RNR_CB_FP;
#endif
#endif

/*
 * MPAF_OTASAP_STACK_EVT_HANDLER_CB.
 *  This handler will be called only when the Embedded stack is initialized.
 *  The events are given to the application before the stack processes it.
 * Return: 
 *      MPAF_STATUS_PASS_ON : If the application wants the stack to process the event
 *      MPAF_STATUS_SUCCESS : If the application does not want the stack/other apps to
 *                                              process the event.
 */
typedef MPAF_STATUS (*MPAF_OTASAP_STACK_EVT_HANDLER_CB)(
                                                      const void* event);
#ifdef MPAF_LEAPP_ENABLE
/*
 * LEL2CAP_MSGHANDLER
 * Function prototype for the L2CAP raw data Callback
 * It has been defined in bleapp stack now.
 */
typedef MPAF_STATUS (*MPAF_OTASAP_LEL2CAP_SIG_CB_FP)(UINT8 *p);
#endif

/* 
 * MPAF_OTASAP_INST_CB 
 * Instance specific Call back functions
 * This needs to be implemented by Applications
 */
typedef tBTM_EVENT_CBACK *         MPAF_OTASAP_BTM_MGMT_CB_FP;

typedef struct MPAF_OTASAP_INST_CB_TYPE
{
    MPAF_OTASAP_POST_STACK_INIT_CB_FP postStackInitCb;
    MPAF_OTASAP_CONN_STATUS_IND_CB_FP connStatusIndCb;
    MPAF_OTASAP_CONN_REQ_CB_FP        connReqCb;
    MPAF_OTASAP_AUTH_STATUS_CB_FP     authStatusCb;
    MPAF_OTASAP_ROLE_CHANGE_IND_CB_FP roleChangeIndCb;
    MPAF_OTASAP_STACK_ACCESS_CB_FP    stackAccessGrantedCb;
    MPAF_OTASAP_INQUIRY_RESULT_CB_FP  inquiryResultCb;
    MPAF_OTASAP_INQUIRY_CMPL_CB_FP    inquiryCmplCb;
#if( BTM_RNR_ENABLED == TRUE )
#ifndef SKIP_TO_COMPILE_BTE_STACK
    MPAF_OTASAP_RNR_CB_FP             rnrCb;
#endif
#endif
    // Note:
    // Register this callback would enable the Application to get all the events before the 
    // stack gets and the App can override or just do a passive sniff of the event.
    MPAF_OTASAP_STACK_EVT_HANDLER_CB  stackEvtInd;

#ifdef MPAF_LEAPP_ENABLE
    MPAF_OTASAP_LEL2CAP_SIG_CB_FP L2CAPSigHandler;
#endif

    //SKIP_TO_COMPILE_BTE_STACK porting 
    MPAF_OTASAP_BTM_MGMT_CB_FP   p_btm_mgmt_callback;  /* BTM Security management call back */
} MPAF_OTASAP_INST_CB;


/* 
 * MPAF_OTASAP_SEC_SETTING
 * Security info specific to an app framework manages the security callbacks 
 * based on these parameters.
 * This data becomes redundant if app registers security callbacks using
 * mpaf_otaSapInstallSecCallBacks()
 */
typedef struct MPAF_OTASAP_SEC_SETTING_TYPE
{
#if (BTM_LISBON_INCLUDED == TRUE)
    // IO Capability Req Params
    UINT8 ioCap;
    BOOL8 bOobDataPresent;
    BOOL8 bAuthReq;

    // User Cfm Request Params
    BOOL8 bJustWorks;
#endif
    // Pin code Details
    UINT8 pinCode[PIN_CODE_LEN];
    UINT32 trustedMask[BTM_SEC_SERVICE_ARRAY_SIZE];
    UINT8 pinLen;

} MPAF_OTASAP_SEC_SETTING;

/* 
 * MPAF_OTASAP_INQ_INFO
 * Information specific to the inquiry initiated by the App (if any).
 */
typedef struct MPAF_OTASAP_INQ_INFO_TYPE
{
    slist_node_t node;
    MPAF_OTASAP_INQ_PARAM inqParam;
    BOOL8         isPending;
} MPAF_OTASAP_INQ_INFO;

/*
 * MPAF_OTASAP_SDP_CB
 *  This handler will be called when the CB for SDP API initiated by the app completes.
 *  The Application registers this cb everytime it invokes mpaf_sdpGetRecord()
 *
 * Return:
 *      MPAF_STATUS_SDP_DONE - If the application has no pending SDP req to be issued.
 *      MPAF_STATUS_PENDING - otherwise
 */
typedef MPAF_STATUS (*MPAF_OTASAP_SDP_CB)(MPAF_STATUS sdpResult, const UINT8 *pRawService,
                                          UINT16 len, void *context);

/* 
 * MPAF_OTASAP_SDP_INFO_TYPE
 * Information specific to the SDP req which the app initiates(if any).
 */
typedef struct MPAF_OTASAP_SDP_INFO_TYPE
{
    slist_node_t node;
    MPAF_OTASAP_SDP_PARAM param;
    void *context;
    MPAF_OTASAP_SDP_CB sdpCb;
} MPAF_OTASAP_SDP_INFO;

/* 
 * MPAF_OTASAP_COD_LIST
 * The COD and COD-Mask which the application is interested in.
 */
typedef tBTM_COD_COND MPAF_OTASAP_COD_LIST;

/* 
 * MPAF_OTASAP_INST_INFO
 * Instance specific Information maitained by framework
 * Apps needs to allocate this space and framework just manages it.
 * This is to avoid any dynamic Allocation inside
 */
typedef struct MPAF_OTASAP_INST_INFO_TYPE
{
    slist_node_t node;
    const MPAF_OTASAP_INST_CB * pCallBacks;
    MPAF_OTASAP_SEC_SETTING   * pSecSetting;
    MPAF_OTASAP_COD_LIST      * pCodList;
    MPAF_OTASAP_INQ_INFO        inqInfo;

    MPAF_APP_ID appId;

} MPAF_OTASAP_INST_INFO;

/*
 * MPAF_OTASAP_RNR_PARAM
 * input parmeters to the Remote Name request.
 */
typedef struct
{
    UINT16         clock_offset;
    BD_ADDR        remote_bd_addr;
    UINT8          page_scan_rep_mode;
    UINT8          page_scan_mode;
#if (BLE_INCLUDED == TRUE)
    tBT_DEVICE_TYPE   device_type;
#endif
} MPAF_OTASAP_RNR_PARAM;

/*******************************************************************************
*                   Global Declarations
*******************************************************************************/

/*******************************************************************************
*                   Function Prototypes
*******************************************************************************/

/*******************************************************************************
* Function: mpaf_otaSapOpen
*
* Abstract: API to open a OTA sap instance by an Application.
*           Application is expected to store the handle for any follow up
*           request to the framework
*
* Input : pInstInfo - Sap Instance Info Storage space
*         appId    - ID of the App for association
*         pCallBacks - Call back structure pointer
*         pCodList - The list of CODs which the application is interested in.
*       
* Output: none.
*
* Return: sap instance handle
*
*******************************************************************************/
MPAF_SAP_HANDLE mpaf_otaSapOpen( MPAF_OTASAP_INST_INFO * pInstInfo, 
                                 MPAF_APP_ID             appId,
                                 const MPAF_OTASAP_INST_CB   * pCallBacks,
                                 MPAF_OTASAP_COD_LIST *pCodList );

/*******************************************************************************
* Function: mpaf_otaSapClose
*
* Abstract: API to close a OTA sap instance by an Application
*
* Input : sapHndl - Instance Handle to this SAP.
*
* Output: None
*
* Return: status
*
*******************************************************************************/
MPAF_STATUS mpaf_otaSapClose( MPAF_SAP_HANDLE sapHndl );


/*******************************************************************************
* Function: mpaf_otaSapSetSecuritySettings
*
* Abstract: API to set custom Security Settings for the App.
*
* Input : sapHndl  - Instance Handle to this SAP.
*         pSecSetting - Security Settings specific to the App.
*         secMode  - Security Mode
*
* Output: none.
*
* Return: status
*
*******************************************************************************/
MPAF_STATUS mpaf_otaSapSetSecuritySettings(
                                          MPAF_SAP_HANDLE sapHndl,
                                          MPAF_OTASAP_SEC_SETTING *pSecSetting);

/*******************************************************************************
* Function: mpaf_otaSapStartInquiry
*
* Abstract: API to Start Inquiry
*
* Input : sapHndl  - Instance Handle to this SAP.
*
* Output: none.
*
* Return: status
*
*******************************************************************************/
MPAF_STATUS mpaf_otaSapStartInquiry( MPAF_SAP_HANDLE sapHndl,
                                     MPAF_OTASAP_INQ_PARAM *inqParam);

/*******************************************************************************
* Function: mpaf_otaSapStopInquiry
*
* Abstract: API to Stop Inquiry
*
* Input : sapHndl  - Instance Handle to this SAP.
*
* Output: none.
*
* Return: status
*
*******************************************************************************/
MPAF_STATUS mpaf_otaSapStopInquiry( MPAF_SAP_HANDLE sapHndl );

/*******************************************************************************
* Function: mpaf_otaSapSetConnectableMode
*
* Abstract: Changes the page scan parameters based on the given \a mode. It is
*           guaranteed that the device will use at least the page scan parameters
*           that are as big as the parameters associated with the \a mode.
*           MPAF_OTASAP_CONNECTABLE_MODE_NONE is used to turn off the page scan.
*           It shall be noted that the page scan may not be turned off if there
*           are conflicts of interest between applications.
*
* Input : sapHndl      - Instance Handle to this SAP.
*         mode         - Connectability mode.
*
* Output: none.
*
* Return: status
*
*******************************************************************************/
MPAF_STATUS mpaf_otaSapSetConnectableMode( MPAF_SAP_HANDLE sapHndl,
                                           MPAF_OTASAP_CONNECTABLE_MODE mode );

/*******************************************************************************
* Function: mpaf_otaSapSetDiscoverability
*
* Abstract: API to set Inquiry Scan (Discoverability).
*           OTASAP will control the Inquiry Scan based on requests
*           from all the Apps.
*
* Input : sapHndl       - Instance Handle to this SAP.
*         bDiscoverable - Discoverable Setting
*              TRUE - Discoverable
*              FALSE - Not Discoverable
*
* Output: none.
*
* Return: status
*
*******************************************************************************/
MPAF_STATUS mpaf_otaSapSetDiscoverability( MPAF_SAP_HANDLE sapHndl,
                                           BOOL8 bDiscoverable );

/*******************************************************************************
* Function: mpaf_otaSapLockStackAccess
*
* Abstract: API to get and lock stack access.
*
* Input : sapHndl  - Instance Handle to this SAP.
*
* Output: none.
*
* Return: status
*
*******************************************************************************/
MPAF_STATUS mpaf_otaSapLockStackAccess( MPAF_SAP_HANDLE sapHndl );

/*******************************************************************************
* Function: mpaf_otaSapUnlockStackAccess
*
* Abstract: API to unlock stack access.
*
* Input : sapHndl  - Instance Handle to this SAP.
*
* Output: none.
*
* Return: status
*
*******************************************************************************/
MPAF_STATUS mpaf_otaSapUnlockStackAccess( MPAF_SAP_HANDLE sapHndl );

/*******************************************************************************
* Function: mpaf_otaSapSecBond
*
* Abstract: API to initiate Security Bonding with Peer Device.
*
* Input : sapHndl  - Instance Handle to this SAP.
*         remoteBdAddr - BdAddr of remote Device.
*
* Output: none.
*
* Return: status
*
*******************************************************************************/
MPAF_STATUS mpaf_otaSapSecBond( MPAF_SAP_HANDLE sapHndl,
                                BD_ADDR remoteBdAddr);


/*******************************************************************************
* Function: mpaf_otaSapStartAuthentication
*
* Abstract: API to let the applications to start authentication.
*
* Input : sapHndl  - Instance Handle to this SAP.
*         bdAddr      - Bluetooth Address of the remote device.
*         hciHandle   - Hci handle of the connection with the remote bdAddr
*
* Output:  None
*
* Return: status
*
*******************************************************************************/
MPAF_STATUS mpaf_otaSapStartAuthentication(MPAF_SAP_HANDLE sapHndl,
                                      BD_ADDR bdAddr,
                                      UINT32 hciHandle);


/*******************************************************************************
* Function: mpaf_otaSapCheckCodMatch
*
* Abstract: API to to check if the CODs match the criteria.
*
* Input : dc1- Class of Device for dev1.
*         dc2- Class of Device for dev2
*         dm - Device Class Mask
*
* Output:  pDevInfo   - Device Info
*
* Return: status
*
*******************************************************************************/

MPAF_STATUS mpaf_otaSapCheckCodMatch(DEV_CLASS dc1,
                                     DEV_CLASS dc2, DEV_CLASS dm);

/*******************************************************************************
* Function: mpaf_sdpGetRecord
*
* Abstract: API to get the SDP record for the specified UUID list and Attribute list.
*
* Input : sapHndl   - Instance Handle to this SAP.
*         pSdpInfo  - Ptr to app's global SDP info struct.
*                     This contains the params for doing the sdp req.
*
* Output:  None
*
* Return: status
*
*******************************************************************************/
MPAF_STATUS mpaf_sdpGetRecord(MPAF_SAP_HANDLE sapHndl,
                              MPAF_OTASAP_SDP_INFO *pSdpInfo);


/*******************************************************************************
* Function: mpaf_otaSapRemoteNameRequest
*
* Abstract: API to get the remote name of the intended device.
*
* Input : sapHndl  - Instance Handle to this SAP.
*         remoteBdAddr - BdAddr of Device.
*
* Output: none.
*
* Return: status
*
*******************************************************************************/
MPAF_STATUS mpaf_otaSapRemoteNameRequest (MPAF_SAP_HANDLE sapHndl,
                                          MPAF_OTASAP_RNR_PARAM *p_rnr);

/*******************************************************************************
* Function: mpaf_otaSapUpdatePageScanParam
*
* Abstract: API to upadte Page scan parameters to updated vendor specific register
*          .
*
* Input : sapHndl      - Instance Handle to this SAP.(for later use)
*         window       - Page scan window time.
*         interval     - Page scan interval time.
*
* Output: none.
*
* Return: none.
*
*******************************************************************************/

void mpaf_otaSapUpdatePageScanParam(MPAF_SAP_HANDLE sapHndl,UINT16 window,
                                       UINT16 interval);

/*******************************************************************************
* Function: mpaf_otaSapUpdateInqScanParam
*
* Abstract: API to upadte Inquiry scan parameters
*          .
*
* Input : sapHndl      - Instance Handle to this SAP.(for later use)
*         window       - Page scan window time.
*         interval     - Page scan interval time.
*
* Output: none.
*
* Return: none.
*
*******************************************************************************/


void mpaf_otaSapUpdateInqScanParam(MPAF_SAP_HANDLE sapHndl,UINT16 window,
                                      UINT16 interval);

#ifdef __cplusplus
}
#endif
#endif   //_MPAF_OTA_H_
