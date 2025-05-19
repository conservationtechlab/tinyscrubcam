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
* File Name: mpaf_pf_utils.h
*
* Abstract:  This file provides definitions for the MPAF PF SAP internal use.
*
*
* Functions:
*
*******************************************************************************/
#ifndef _MPAF_PF_UTILS_H_
#define _MPAF_PF_UTILS_H_

#include "mpaf_pf.h"
#include "mpaf_utils.h"

/*******************************************************************************
*                   Defines & Macros
*******************************************************************************/

/*******************************************************************************
*                   Type Definitions
*******************************************************************************/
/*
 * MPAF_PF_GlobalVariables
 * PF _internel objects.
 */
typedef struct MPAF_PF_LOCAL_T
{
    struct slist_node_t   mpafEntryList;
    MPAF_PFSAP_INST_INFO *AppTable[MPAF_APP_ID_MAX];
    TIMER_LIST_ENT        shutdownTimer;
    UINT32                shutdownTO;
}MPAF_PF_LOCAL;

#define MPAF_PF_SHUTDOWN_TIMEOUT    5

//! MPAF EVENTS
//! Event  set  to the MPAF thread on application needs.
#define MPAF_APP_EVENT_BIT                  (1UL << 0)

//! Event set by MPAF oto to OTA to indiacte the stack initialization.
#define MPAF_OTA_STACK_INIT_CMPL_EVENT_BIT  (1UL << 1)
//! Event set by MPAF oto to OTA to indiacte apps
#define MPAF_OTA_INIT_APPS_EVENT_BIT        (1UL << 2)

/*******************************************************************************
**                              Externs
*******************************************************************************/
extern MPAF_PF_LOCAL mpaf_pf_local;

/*******************************************************************************
*                   Function Prototypes
*******************************************************************************/
void Print_mpafEntry( MPAF_PFSAP_INST_INFO * instInfo);
pmu_idle_msg_handler_result_t mpaf_pfIdleThread(INOUT MSG_t* msg);
void mpaf_pfStartIdlePollTimer(void);
void mpaf_pfStackShutdownComplete(UINT32 otaClosed);
void mpaf_hci_issue_reset_cmd(void);
//! MPAF application msg handler
//! This function pass the msg to the respective application
void mpaf_pfAppMsghandler(MPAF_GENERIC_MSG_t* app_msg);


#endif   //_MPAF_PF_UTILS_H_

