/*******************************************************************************
* THIS INFORMATION IS PROPRIETARY TO BROADCOM CORP
*
* ------------------------------------------------------------------------------
*
* Copyright (c) 2004 Broadcom Corp.
*
*          ALL RIGHTS RESERVED
*
********************************************************************************
*
* File Name: mpaf_utils.h
*
* Abstract:  This file provides common definitions internal to MPAF
*
*
* Functions:
*
*******************************************************************************/
#ifndef _MPAF_UTILS_H_
#define _MPAF_UTILS_H_

#include "mpaf_nvConfig.h"
#include "mpaf.h"
#include "mpaf_pf.h"
#include "mpaf_trans.h"
#include "mpaf_ota_utils.h"
#include "foundation/api/debug.h"
#include "mpaf_trans_utils.h"

#include "stdio.h"
#include "stdarg.h"
#include "string.h"

/*******************************************************************************
*                   Defines & Macros
*******************************************************************************/
/*
 * Turn this on for debug prints and set the MPAF_TRACE_LEVEL_MASK to 
 * the level of interest.
 */
//#define MPAF_TRACE_ENABLE

/*
 * Turn this on for debug Asserts
 */
#ifdef MPAF_TRACE_ENABLE
#define MPAF_ASSERT_ENABLE
#endif

#ifdef MPAF_TRACE_ENABLE

#define MPAF_TRACE_LEVEL_CRIT       (0x0004)
#define MPAF_TRACE_LEVEL_HIGH       (0x0002)
#define MPAF_TRACE_LEVEL_LOW        (0x0001)

#define MPAF_TRACE_LEVEL_LOW_MASK   (MPAF_TRACE_LEVEL_CRIT |  \
                                     MPAF_TRACE_LEVEL_HIGH |  \
                                     MPAF_TRACE_LEVEL_LOW)
#define MPAF_TRACE_LEVEL_HIGH_MASK  (MPAF_TRACE_LEVEL_CRIT |  \
                                     MPAF_TRACE_LEVEL_HIGH)
#define MPAF_TRACE_LEVEL_CRIT_MASK  (MPAF_TRACE_LEVEL_CRIT)

/*
 * Change this mask for any trace level changes.
 */
#define MPAF_TRACE_LEVEL_MASK (MPAF_TRACE_LEVEL_HIGH_MASK)

#define MPAF_DBG_TRACE_BDA(level, string, bdaddr) \
    MPAF_DBG_TRACE_ARR(level,string, bdaddr, BD_ADDR_LEN)

#define MPAF_DBG_TRACE_COD(level, string, cod) \
    MPAF_DBG_TRACE_ARR(level,string, cod, DEV_CLASS_LEN)

#if (MPAF_TRACE_LEVEL_MASK & MPAF_TRACE_LEVEL_CRIT) 
#define MPAF_DBG_TRACE_CRIT(_string)         debug_Printf(_string##"\r\n")
#define MPAF_DBG_TRACE_T1_CRIT(_string, _param)   \
                       debug_Printf(_string##" 0x%x\r\n", _param);
#define MPAF_DBG_TRACE_ARR_CRIT(_string, array, len) \
                        mpaf_trace_array(_string, array, len);
#else
#define MPAF_DBG_TRACE_CRIT(_string)
#define MPAF_DBG_TRACE_T1_CRIT(_string, _param)
#define MPAF_DBG_TRACE_ARR_CRIT(_string, array, len)
#endif

#if (MPAF_TRACE_LEVEL_MASK & MPAF_TRACE_LEVEL_HIGH) 
#define MPAF_DBG_TRACE_HIGH(_string)   debug_Printf(_string##"\r\n")
#define MPAF_DBG_TRACE_T1_HIGH(_string, _param)   \
                        debug_Printf(_string##" 0x%x\r\n", _param);
#define MPAF_DBG_TRACE_ARR_HIGH(_string, array, len) \
                        mpaf_trace_array(_string, array, len);
#else
#define MPAF_DBG_TRACE_HIGH(_string)
#define MPAF_DBG_TRACE_T1_HIGH(_string, _param)
#define MPAF_DBG_TRACE_ARR_HIGH(_string, array, len)
#endif

#if (MPAF_TRACE_LEVEL_MASK & MPAF_TRACE_LEVEL_LOW)
#define MPAF_DBG_TRACE_LOW(_string)   debug_Printf(_string##"\r\n")
#define MPAF_DBG_TRACE_T1_LOW(_string, _param)   \
                       debug_Printf(_string##" 0x%x\r\n", _param);
#define MPAF_DBG_TRACE_ARR_LOW(_string, array, len) \
                        mpaf_trace_array(_string, array, len);
#else
#define MPAF_DBG_TRACE_LOW(_string)
#define MPAF_DBG_TRACE_T1_LOW(_string, _param)
#define MPAF_DBG_TRACE_ARR_LOW(_string, array, len)
#endif

#define MPAF_DBG_TRACE(_level, _string)     \
                        MPAF_DBG_TRACE_##_level("\r\nMPAF:"##_string)
#define MPAF_DBG_TRACE_T1(_level, _string, _param)     \
                        MPAF_DBG_TRACE_T1_##_level("\r\nMPAF:"##_string, _param)
#define MPAF_DBG_TRACE_ARR(_level, _string, array, len)     \
                            MPAF_DBG_TRACE_ARR_##_level(_string, array, len)

#define MPAF_DBG_STRACE(_string) debug_Printf(_string)

#else //MPAF_TRACE_ENABLE
#define MPAF_DBG_TRACE(_level, _string)
#define MPAF_DBG_TRACE_T1(_level, _string, _param)
#define MPAF_DBG_STRACE(_string)
#define MPAF_DBG_TRACE_ARR(_level, _string, array, len)
#define MPAF_DBG_TRACE_BDA(_level, string, bdaddr)
#define MPAF_DBG_TRACE_COD(_level, string, cod)

#endif //MPAF_TRACE_ENABLE



#ifdef MPAF_ASSERT_ENABLE

void mpaf_assert_fail( char*   file,   int  line , UINT32 status );

#define MPAF_ASSERT( _status ) \
do{                            \
    if((_status) == FALSE)     \
    {                          \
        mpaf_assert_fail(__FILE__, __LINE__, _status); \
    }                          \
}while(0)
#else
#define MPAF_ASSERT( _status ) 
#endif

#define MPAF_BTW_ENABLED             (0x01)
#define MPAF_BTU_HCI_RESET_PENDING   (0x02)
#define MPAF_PF_DISABLE_APPS         (0x04)
#define MPAF_PF_HOST_HCI_RESET       (0x08)
// This bit is set if the Application(s) decides to shutdown OTA and the command complete event
// for the hci reset should not be sent to the Host.
#define MPAF_PF_APP_HCI_RESET        (0x10)


typedef enum
{
    MPAF_HCIPKT_CMD,
    MPAF_HCIPKT_EVENT,
    MPAF_HCIPKT_ACL_TX, // Host -> Controller -> Over the Air
    MPAF_HCIPKT_ACL_RX, // Over the Air -> Controller -> Host
    MPAF_HCIPKT_SCO_TX, // Host -> Controller -> Over the Air
    MPAF_HCIPKT_SCO_RX  // Over the Air -> Controller -> Host
}MPAF_HCIPKT_TYPE;

/*******************************************************************************
*                   Type Definitions
*******************************************************************************/
extern UINT8 mpaf_flags;

/*
 * Platform  parameters
 */
typedef UINT8 tMPAF_NV_MRU_INFO[MPAF_NV_CFGID_DEV_MAX];

/*******************************************************************************
*                               Externs
*******************************************************************************/

/*******************************************************************************
*                   Global Declarations
*******************************************************************************/

/*******************************************************************************
*                   Function Prototypes
*******************************************************************************/

BOOL32 mpaf_init(void);
void mpaf_debug_InitUART(void);
void mpaf_trace_array(const char *_string, const UINT8* array, const UINT16 len);
void mpaf_csaEventHandlerCB (UINT32 event);

/* 
 * Transport owned prototypes shared across other MPAF components
 */
void mpaf_transInit(void);
void mpaf_transTransQInit(void);
void mpaf_transPostFWInit(void);
void mpaf_transProcEvts(UINT32 evtId);
void mpaf_transProcOut( UINT8 portId, UINT16 ep,
                        UINT8* dataPtr, UINT16 dataLen);
void mpaf_transProcHidCtrl(MSG_t *);
void mpaf_sendTrasStausInd(void);

MPAF_FILTER_RESPONSE_TYPE mpaf_transAclTxFilter(const BTHCI_ACL_HDR_t* acltx,
                                                MPAF_HCIACL_HANDLER_CB *handler);
void mpaf_sendDevSatusInd(MSG_t* devStateMsg);


/* 
 * PF owned prototypes shared across other MPAF components
 */
void mpaf_pf_init(void);
void mpaf_pfPostFWinit(void);
void mpaf_PostFWinit(void);
void mpaf_PostTransSelect(void);
void mpaf_pfEnableApp(MPAF_APP_STATUS appStatus);
void mpaf_pfEnableSpecificApp(IN MSG_t *msg);
void mpaf_pfProcAppEvts(UINT32 events);



MPAF_FILTER_RESPONSE_TYPE mpaf_HciEvtFilter( const BTHCI_EVENT_HDR_t *p_event,
                                             MPAF_HCIEVT_HANDLER_CB *p_handler);
MPAF_FILTER_RESPONSE_TYPE mpaf_HciCmdFilter(const BTHCI_CMD_HDR_t *p_cmd,
                                            MPAF_HCICMD_HANDLER_CB *p_handler);
MPAF_FILTER_RESPONSE_TYPE mpaf_df_hci_cmd_filter( const BTHCI_CMD_HDR_t *p_cmd,
                                                  MPAF_HCICMD_HANDLER_CB *p_handler );
MPAF_FILTER_RESPONSE_TYPE mpaf_hci_event_filter(const BTHCI_EVENT_HDR_t *p_event,
                                                MPAF_HCIEVT_HANDLER_CB *p_handler);
void mpaf_hci_send_cmd_complete(UINT32 opcode, UINT32 status);

void btu_mpaf_init(void);
void btu_mpaf_cfa_host_cmd_pending(void);
void btu_mpaf_process_timer_event (void);

extern UINT8* sdp_findHidReportDescriptor (UINT8 *p, UINT16* p_len);

/* 
 * OTA owned prototypes shared across other MPAF components
 */
void mpaf_otaProcEvents(UINT32 events);
BOOL32 btu_mpaf_IsEmbeddedStackEnable(void);
//! MPAF msg handler
//! This function handles the MSG codes owned by MPAF
MSG_t* mpaf_msgHandler(MSG_t* msg);

//! MPAF event handler
//! This function handles the events owned by MPAF
void mpaf_eventHandler(UINT32 events);

#endif   //_MPAF_UTILS_H_
