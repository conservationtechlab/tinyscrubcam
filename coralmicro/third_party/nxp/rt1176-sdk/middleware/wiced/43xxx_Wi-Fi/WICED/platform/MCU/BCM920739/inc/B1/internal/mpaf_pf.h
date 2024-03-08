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
 * File Name: mpaf_pf.h
 *
 * Abstract:  This file provides interface definitions for the MPAF Platform SAP
 *
 *
 * Functions:
 *
 *******************************************************************************/
#ifndef _MPAF_PF_H_
#define _MPAF_PF_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \defgroup mpaf_pfsap MPAF Platform SAP Module
 *
 * Interface to access platform related services. All the MPAF applications
 * need to use this interface to avail platform services.
 */

/**
 * \defgroup mpaf_pfsap_dev MPAF PFSAP Device Cache Interface
 *
 * The PF SAP interface to access device cache services. It provides an
 * interface to efficiently store and retrieve MPAF_DEV_INFO structures. The
 * stored devices are persistent across power cycles. The retrival is
 * faster since the device entries are cached in the RAM.
 */

/**
 * \addtogroup mpaf_pfsap
 *
 * @{
 */

#include "brcm_fw_types.h"
#include "mpaf_thread.h"
#include "mpaf_core.h"
#include "mpaf_cfa.h"
#include "mpaf.h"
#include "mpaf_device.h"
#include "mpaf_nvConfig.h"
#include "btu.h"

#ifdef MPAF_ENABLE

/*******************************************************************************
 *                   Defines & Macros
 *******************************************************************************/
#define MPAF_PFSAP_DEVICE_HANDLE_INVALID             0xFF

#define MPAF_PFSAP_250MS_IN_US                       (250*1000)

/*******************************************************************************
 *                   Type Definitions
 *******************************************************************************/

/*
 * MPAF_APP_STATUS
 * Framework status events for Apps.
 */
enum
{
    MPAF_APP_DISABLE = 0, /* Disable App */
    MPAF_APP_ENABLE, /* Enable App */
    MPAF_APP_HCI_RESET, /* HCI Reset occured */
    MPAF_APP_STACK_SHUTDOWN_CMPL, /* Stack Shutdown completed */

};
typedef UINT8 MPAF_APP_STATUS;

/*
 * MPAF_PFSAP_POST_FW_INIT_CB_FP
 * Function prototype for the post FW Init CB.
 */
typedef void (*MPAF_PFSAP_POST_FW_INIT_CB_FP)( void );

/*
 * MPAF_APP_TIMER_CB_FP
 * Function prototype for the timer call backs.
 */
typedef void (*MPAF_APP_TIMER_CB_FP)( UINT32 cBackparam );

//! Timer type
//! This enum defines the  two characteristics of timer one is execution context and other is
//! Timer triggering method(Single shot or periodic). Here we give four types using the above two characteristics
//! of timer. 1. Single shot  timer with timer context callback , 2. Single shot with Mpaf context callback
//! 3.Periodic with timer context,4. Periodic with mpaf context.
//! IF timer context is defined by the application the MPAF will invoke the callback in timer caontext otherwise will invoke it in
//! MPAF context
//! Second timer is the low resolution timer. Which used GKI timer module from stack.
enum
{
    MPAF_PF_US_SINGLESHOT_TIMER_CONTEXT,    //MPAF_SINGLESHOT_TIMER_CONTEXT
    MPAF_PF_US_SINGLESHOT_MPAF_CONTEXT,     //MPAF_SINGLESHOT_MPAF_CONTEXT
    MPAF_PF_US_PERIODIC_TIMER_CONTEXT,      // MPAF_PERIODIC_TIMER_CONTEXT
    MPAF_PF_US_PERIODIC_MPAF_CONTEXT,       //MPAF_PERIODIC_MPAF_CONTEXT
    MPAF_PF_SEC_TIMER_MPAF_CONTEXT         //GKI Stack timer
};
typedef UINT8 MPAF_PF_TIMER_TYPE;

//! Timer structures used by MPAF application
typedef struct
{
        union
        {
                TIMER_LIST_ENT sec;
                MPAF_CORE_TIMER us;
        } timer;
        MPAF_PF_TIMER_TYPE type;
        BOOL8 isCreated;
} MPAF_PF_TIMER;
/*
 * MPAF_PFSAP_APP_ENABLE_IND_FP
 * Function prototype for the app Startup/Shutdown Indication.
 */
typedef void (*MPAF_PFSAP_APP_STATUS_IND_FP)( MPAF_APP_STATUS appStat );

/**
 * \addtogroup mpaf_pfsap_dev
 *
 * @{
 */

/**
 * MPAF PF SAP device handle. It is used to refer a device (MPAF_DEV_INFO).
 * When an handle is used to perform an operation, it will be faster.
 */
typedef UINT8 MPAF_PFSAP_DEVICE_HANDLE;

/**
 * Callback used to resolve the given device information (\a devinfo). The
 * application has to use this callback to let the device cache module know
 * whether the device information is associated with it or not.
 *
 * It is mandatory if the application expects that its device information
 * can be added through any of the following:
 *  -# Add HID Device HCI command
 *  -# Add HID Device HID control command
 *
 * \param devInfo The device information to be resolved.
 *
 * \return TRUE, if the given \a devInfo is associated with it. FALSE,
 *         otherwise.
 */
typedef BOOL32 (*MPAF_PFSAP_DEVICE_RESOLVER)( const MPAF_DEV_INFO *devInfo );

/**
 * Callback used to indicate whenever a device associated with the
 * application is added. It is optional.
 *
 * \param devHdl A valid handle of the device that has been added.
 * \param devInfo The device information that was just added for the
 *                application.
 */
typedef void (*MPAF_PFSAP_DEVICE_ADDED)( MPAF_PFSAP_DEVICE_HANDLE devHdl, const MPAF_DEV_INFO *devInfo );

/**
 * Callback used to indicate whenever a device associated with the
 * application is deleted. It is optional.
 *
 * \param devHdl A valid handle of the device that has been deleted.
 * \param devInfo The device information that was just deleted.
 */
typedef void (*MPAF_PFSAP_DEVICE_DELETED)( MPAF_PFSAP_DEVICE_HANDLE devHdl, const MPAF_DEV_INFO *devInfo );

/**
 * Callback used to allocate a device entry whenever a new device is getting
 * added for the application. With the given information, the application
 * can decide where to store the device.
 *
 * It is applicable only for the applications that reserved some space using
 * #mpaf_pfSapDeviceReserve API. Even for these applications, it is
 * mandatory only when they don't want to use "global device allocation
 * strategy" for adding devices. Refer #mpaf_pfSapDeviceAdd() for more
 * information.
 *
 * \param base Device handle of the first device entry reserved to the
 *             application.
 * \param nEntries Number of device entries reserved for the application.
 * \param devInfo The device that has to be added.
 *
 * \return A valid device handle that has to be used to add the given device
 *         (\a devInfo). A valid handle should be in the range of <em>[base,
 *         base + nEntries - 1]</em> (both inclusive). If the given device (\a
 *         devInfo) should not (or cannot) be added,
 *         MPAF_PFSAP_DEVICE_HANDLE_INVALID shall be returned.
 */
typedef MPAF_PFSAP_DEVICE_HANDLE
(*MPAF_PFSAP_DEVICE_ALLOCATOR)( MPAF_PFSAP_DEVICE_HANDLE base, UINT8 nEntries, const MPAF_DEV_INFO *devInfo );

/**
 * Callback to handle matched device entries during a search
 * (#mpaf_pfSapDeviceForeach).
 *
 * \param appId Application ID associated with the matched device.
 * \param devHdl Device handle of the matched device.
 * \param devInfo Device information of the matched device.
 * \param userData Data passed (#mpaf_pfSapDeviceForeach) during the search.
 *
 * \return TRUE, if the search has to be terminated. FALSE, if the search has
 *         to continue with the remaining device entries.
 */
typedef BOOL32 (*MPAF_PFSAP_DEVICE_HANDLER)( MPAF_APP_ID appId, MPAF_PFSAP_DEVICE_HANDLE devHdl, const MPAF_DEV_INFO *devInfo, void *userData );
/** @} */ // end: mpaf_pfsap_dev
/* 
 * MPAF_PFSAP_INST_CB 
 * Instance specific Call back functions
 * This needs to be implemented by Applications
 */
typedef struct MPAF_PFSAP_INST_CB_TYPE
{
        MPAF_PFSAP_POST_FW_INIT_CB_FP postFwInit;
        MPAF_PFSAP_APP_STATUS_IND_FP appStatus;
        MPAF_APP_TIMER_CB_FP timerCB;
        MPAF_PFSAP_DEVICE_RESOLVER devResolver;
        MPAF_PFSAP_DEVICE_ADDED devAdded;
        MPAF_PFSAP_DEVICE_DELETED devDeleted;
        MPAF_PFSAP_DEVICE_ALLOCATOR devAllocator;
} MPAF_PFSAP_INST_CB;

//==================================================================================================
// Types and constants

//! Result of a call to a mpaf messaage handler callback. Each application can register a msg handler callback.
//! when any msg recieved by MPAF has to passed to respective application. It uses the registered callback to
//! pass the msg. If the msg is handled and and the msg memory is frred the function return the status 
//! MPAF_APP_MSG_HANDLER_RESULT_HANDLED_AND_RELEASED , if the callback ahndles and not freeing the msg then it returns with
//! MPAF_APP_MSG_HANDLER_RESULT_HANDLED_CALLER_TO_RELEASE 
typedef enum mpaf_msg_handler_result
{
    //! Result indicating that the message was handled and released.  If not realeased, then the
    //! message was forwarded elsewhere, and that the caller should take no further action.
    MPAF_APP_MSG_HANDLER_RESULT_HANDLED_AND_RELEASED,

    //! Result indicating that the message was handled, and that the caller should release it.  In
    //! that case, the message will be released using dynamic_memory_Release.  Deferring release to
    //! the caller when the message was allocated from the dynamic memory pools saves code space.
    MPAF_APP_MSG_HANDLER_RESULT_HANDLED_CALLER_TO_RELEASE
} mpaf_msg_handler_result_t;

//==================================================================================================
//! Function pointer of the mpaf app mesg handler
typedef mpaf_msg_handler_result_t (*MPAF_MSG_HANDLER_FP)( INOUT MPAF_GENERIC_MSG_t* msg );

/*
 * MPAF_EVT_HNDLR_FP
 * Function prototype for the App event handler.
 */
typedef void (*MPAF_EVT_HNDLR_FP)( UINT32 evtMask );

typedef pmu_idle_msg_handler_result_t (*MPAF_PF_IDLE_POLL_CB_FP)( INOUT MSG_t* msg );

/* 
 * MPAF_PFSAP_INST_INFO 
 * Instance specific Information maitained by framework
 * Apps needs to allocate this space and framework just manages it.
 * This is to avoid any dynamic Allocation inside
 */
typedef struct MPAF_PFSAP_INST_INFO_TYPE
{
        slist_node_t slist;
        const MPAF_PFSAP_INST_CB * callBacks;
        UINT32 Events;
        UINT32 AciveEvents;
        MPAF_PF_IDLE_POLL_CB_FP idlePollHndlr;
        MPAF_APP_ID appId;
        MPAF_HCICMD_FILTER_CB hciCmdFilter;
        MPAF_HCIEVT_FILTER_CB hciEvtFilter;
        MPAF_EVT_HNDLR_FP evtHndlr;
        MPAF_MSG_HANDLER_FP msgHndlr;
} MPAF_PFSAP_INST_INFO;

//! BTU Initialization function to be called when the firmware initialization is complete.
//! The BTU init function should register for the HCI Command, Event and Data filters.
typedef void (*mpaf_btu_init_t)( void );

typedef MPAF_FILTER_RESPONSE_TYPE (*mpaf_stack_cmd_filter_t)( const BTHCI_CMD_HDR_t *p_cmd, MPAF_HCICMD_HANDLER_CB *p_handler );

//! Global MPAF structure which is used to store the list of functions which would be registered
//! dynamically based on the config requirements. This will also help in patching post tapeout.
typedef struct
{
        //! MPAF BTU init function
        mpaf_btu_init_t btu_init;
        mpaf_stack_cmd_filter_t stack_cmd_filter;
} MPAF_FUNC_LIST_t;

typedef BOOL32 (*tMPAF_HCICMD_HDLR_FCN)( UINT8 *p_params );

typedef struct
{
        tMPAF_HCICMD_HDLR_FCN handler;
        UINT16 ocf;
} tMPAF_HCICMD_HDLR;

extern MPAF_FUNC_LIST_t mpaf_func_list;

// Functions which are currently defined to existing functions but could be modified in the future.

/*******************************************************************************
 *                   Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 * Function: mpaf_pfSapOpen
 *
 * Abstract: API to open a platform sap instance by an Application
 *           Application is expected to store the handle for any follow up
 *           request to the framework
 *
 * Input : instInfo - Sap Instance Info Storage space
 *         appId    - ID of the App for association
 *         callBacks - pointer to the sap callback structure.
 *
 * Output: none.
 *
 * Return: sap instance handle
 *
 *******************************************************************************/
MPAF_SAP_HANDLE mpaf_pfSapOpen( MPAF_PFSAP_INST_INFO * instInfo, MPAF_APP_ID appId, const MPAF_PFSAP_INST_CB * callBacks );
/*******************************************************************************
 * Function: mpaf_pfSapClose
 *
 * Abstract: API to close a platform sap instance by an Application
 *
 * Input : sapHndl - Instance Handle to this SAP.
 *
 * Output: None
 *
 * Return: status
 *
 *******************************************************************************/
MPAF_STATUS mpaf_pfSapClose( MPAF_SAP_HANDLE sapHndl );

/*******************************************************************************
 * Function: mpaf_pfSapThreadSetEvtMask
 *
 * Abstract: API to modify the active Event mask for an App
 *
 * Input : sapHndl  - Instance Handle to this SAP.
 *         evtMask  - Active Event Masks
 *
 * Output: none.
 *
 * Return: status
 *
 *******************************************************************************/
MPAF_STATUS mpaf_pfSapThreadSetEvtMask( MPAF_SAP_HANDLE sapHndl, UINT32 evtMask );

/*******************************************************************************
 * Function: mpaf_SetAppEvent
 *
 * Abstract: API to set an event for an app. Allows Events between APPs
 *
 * Input : appId   - ID of the App to set the event for.
 *         evtMask - Active Event Masks
 *
 * Output: none.
 *
 * Return: status
 *
 *******************************************************************************/
MPAF_STATUS mpaf_SetAppEvent( MPAF_APP_ID appId, UINT32 evt );

/*******************************************************************************
 * Function: mpaf_pfSapInstallIdlePoll
 *
 * Abstract: API to install an idle thread poll function.
 *
 * Input : sapHndl       - Instance Handle to this SAP.
 *         idlePollHndlr - Idle Poll Handler Specific to the APP.
 *
 * Output: none.
 *
 * Return: status
 *
 *******************************************************************************/
MPAF_STATUS mpaf_pfSapInstallIdlePoll( MPAF_SAP_HANDLE sapHndl, MPAF_PF_IDLE_POLL_CB_FP idlePollHndlr );

/*******************************************************************************
 * Function: mpaf_pfSapUninstallIdlePoll
 *
 * Abstract: API to uninstall an idle thread poll function.
 *
 * Input : sapHndl       - Instance Handle to this SAP.
 *
 * Output: none.
 *
 * Return: status
 *
 *******************************************************************************/
MPAF_STATUS mpaf_pfSapUninstallIdlePoll( MPAF_SAP_HANDLE sapHndl );

/*******************************************************************************
 * Function: mpaf_pfSapInstallHciCmdFilter
 *
 * Abstract: API to install an HCI Command filter Chain.
 *
 * Input : sapHndl      - Instance Handle to this SAP.*
 *         hciCmdFilter - HCI Command Filter.
 *
 * Output: none.
 *
 * Return: status
 *
 *******************************************************************************/
MPAF_STATUS mpaf_pfSapInstallHciCmdFilter( MPAF_SAP_HANDLE sapHndl, MPAF_HCICMD_FILTER_CB hciCmdFilter );

/*******************************************************************************
 * Function: mpaf_pfSapUninstallHciCmdFilter
 *
 * Abstract: API to install an HCI Command filter Chain.
 *
 * Input : sapHndl      - Instance Handle to this SAP.*
 *
 * Output: none.
 *
 * Return: status
 *
 *******************************************************************************/
MPAF_STATUS mpaf_pfSapUninstallHciCmdFilter( MPAF_SAP_HANDLE sapHndl );

/*******************************************************************************
 * Function: mpaf_pfSapInstallHciEvtFilter
 *
 * Abstract: API to install an HCI Command filter Chain.
 *           This filter installation is applicable only when the Stack is NOT initialized.
 *           For getting events after stack init, the application needs to register stackEvtInd
 *           with the OTA Sap.
 *
 * Input : sapHndl      - Instance Handle to this SAP.
 *         hciEvtFilter - HCI Event Filter.
 *
 * Output: none.
 *
 * Return: status
 *
 *******************************************************************************/

MPAF_STATUS mpaf_pfSapInstallHciEvtFilter( MPAF_SAP_HANDLE sapHndl, MPAF_HCIEVT_FILTER_CB hciEvtFilter );

/*******************************************************************************
 * Function: mpaf_pfSapUninstallHciEvtFilter
 *
 * Abstract: API to install an HCI Command filter Chain.
 *
 * Input : sapHndl      - Instance Handle to this SAP.
 * Output: none.
 *
 * Return: status
 *
 *******************************************************************************/
MPAF_STATUS mpaf_pfSapUninstallHciEvtFilter( MPAF_SAP_HANDLE sapHndl );

/*******************************************************************************
 * Function: mpaf_pfSapInitTimer
 *
 * Abstract: API to Init  a Timer. If the timer structure is allocated from dynamic memory alloc.
 *        then make sure the memory block is intialize to zero.
 *
 * Input : sapHndl - Instance Handle to this SAP.
 *         p_tle   - Pointer to timer List Entry
 *         TimerCb - Pointer if not NULL then overrides the default timer handler.
 *         type -type          timer  type single shot, periodic, mpaf or timer context, sec time
 * Output: none.
 *
 * Return: status
 *
 *******************************************************************************/
MPAF_STATUS mpaf_pfSapInitTimer( MPAF_SAP_HANDLE sapHndl, MPAF_PF_TIMER * p_tle, MPAF_APP_TIMER_CB_FP TimerCb, UINT32 cBackparam, MPAF_PF_TIMER_TYPE type );

/*******************************************************************************
 * Function: mpaf_pfSapStartTimer
 *
 * Abstract: API to Start a Timer.
 *               This api start the timer for the given duration.
 *               If the given timer is already running this function ignores the call.
 *               If we need ot restart the timer ,then call stop timer before calling this api.
 *
 * Input : sapHndl - Instance Handle to this SAP.
 *         p_tle   - Pointer to timer List Entry
 *         timeOut - Timeout value.
 * Output: none.
 *
 * Return: status
 *
 *******************************************************************************/
MPAF_STATUS mpaf_pfSapStartTimer( MPAF_SAP_HANDLE sapHndl, MPAF_PF_TIMER * p_tle, UINT32 timeout );

/*******************************************************************************
 * Function: mpaf_pfSapStopTimer
 *
 * Abstract: API to Stop a Timer.
 *
 * Input : sapHndl - Instance Handle to this SAP.
 *         timerCb - App specific TimeOutCB.
 *
 * Output: none.
 *
 * Return: status
 *
 *******************************************************************************/
MPAF_STATUS mpaf_pfSapStopTimer( MPAF_SAP_HANDLE sapHndl, MPAF_PF_TIMER * p_tle );

/*******************************************************************************
 * Function: mpaf_pfSapIsTimerActive
 *
 * Abstract: API to find whether timer is active
 *
 * Input : sapHndl - Instance Handle to this SAP.
 *
 * Output: none.
 *
 * Return:  BOOL - active or not
 *
 *******************************************************************************/
BOOL32 mpaf_pfSapIsTimerActive( MPAF_SAP_HANDLE sapHndl, MPAF_PF_TIMER * p_tle );

/*******************************************************************************
 **
 ** Function         mpaf_pfSapIsBTWEnabled
 **
 ** Description      Checks wether BTW enabled
 **
 ** Returns          True if BTE active
 **
 *******************************************************************************/
BOOL32 mpaf_pfSapIsBTWEnabled( void );

/*******************************************************************************
 * Function: mpaf_pfSapConfigVSRead
 *
 * Abstract: API to read data from NV ram . If length exceeds 255 it reads from the next vsid.
 * Input :   sapHndl -APP PF saphandle
 *              vsid : Location Id
 *              len        :Length of byte to writen
 *              buff       pointer of the data to be writen
 * Output: none.
 *
 * Return: Length of bytes read
 *
 *******************************************************************************/
UINT16 mpaf_pfSapConfigVSRead( MPAF_SAP_HANDLE sapHndl, UINT16 vsid, UINT16 len, UINT8* buff );

/*******************************************************************************
 * Function: mpaf_pfSapConfigVSWrite
 *
 * Abstract: API  to write data in to NV Ram. If size is greater than 255 bytes it writes the excess
 *                  data in to the consecutive  vsids
 * Input : sapHndl -APP PF sap handle
 *             vsid : Location Id
 *             len        :Length of byte to read
 *             buff       pointer of the data to be copied
 * Output: none.
 *
 * Return: Status
 *
 *******************************************************************************/
UINT32 mpaf_pfSapConfigVSWrite( MPAF_SAP_HANDLE sapHndl, UINT16 vsid, UINT16 len, UINT8* buff );

/*******************************************************************************
 * Function: mpaf_memAlloc
 *
 * Abstract: API to get the buffer from generic pool. This buffer can be used by application for its internal
 purpose.
 * Input : size : .Size of buffer to be allocated
 *
 * Output: None
 *
 * Return: Pointer of the allocated buffer. If fails return NULL.
 *
 *******************************************************************************/
void* mpaf_memAlloc( UINT32 size );

/*******************************************************************************
 * Function: mpaf_memFree
 *
 * Abstract: API to free the buffer allocated from generic buff pool
 *
 * Input : memory_ptr: Pointer to be freed.
 *
 * Output: None
 *
 * Return: None.
 *
 *******************************************************************************/
void mpaf_memFree( void *memoryBlock );

/*******************************************************************************
 * Function: mpaf_pfSapExecuteHciCmd
 *
 * Abstract: API to send Hci cmd to lm thread.
 *
 * Input : command: cmd structure pointer
 *
 * Output: None
 *
 * Return: None.
 *
 *******************************************************************************/
void mpaf_pfSapExecuteHciCmd( const BTHCI_CMD_HDR_t* command );

/*******************************************************************************
 * Function: mpaf_pfSapSendHciEvent
 *
 * Abstract: API to send Hci event to lm thread.
 *
 * Input : event: event structure pointer
 *
 * Output: None
 *
 * Return: None.
 *
 *******************************************************************************/
void mpaf_pfSapSendHciEvent( const BTHCI_EVENT_HDR_t* event );

/*******************************************************************************
 * Function: mpaf_pfSapSendACLTx
 *
 * Abstract: API to send acl packet to lm thread.Once thisfunction has been called, the caller no longer owns
 *               the acl data buffer block  and may not use or release it.
 *
 * Input : acl: acl structure pointer
 *
 * Output: None
 *
 * Return: None.
 *
 *******************************************************************************/
void mpaf_pfSapSendACLTx( BTHCI_ACL_HDR_t* acl );

/*******************************************************************************
 * Function: mpaf_pfSapAllocACLDown
 *
 * Abstract: API to get the buffer from acl down pool. This buffer should be used by application to send
 data to lm threas(ACL packet).
 * Input : size : .Size of buffer to be allocated
 *
 * Output: None
 *
 * Return: Pointer of the allocated buffer. If fails return NULL.
 *
 *******************************************************************************/
void * mpaf_pfSapAllocACLDown( UINT32 payloadSize );

/*******************************************************************************
 * Function: mpaf_pfSapAllocBleACLDown
 *
 * Abstract: API to get the buffer from ULP acl down pool. This buffer should be used by application to send
 data to lm threas(ACL packet).
 * Input : size : .Size of buffer to be allocated
 *
 * Output: None
 *
 * Return: Pointer of the allocated buffer. If fails return NULL.
 * Note: mpaf_pfSapFreeACLBuffer shoul be used to free the buffer
 *
 *******************************************************************************/
void * mpaf_pfSapAllocBleACLDown( UINT32 payloadSize );

/*******************************************************************************
 * Function: mpaf_pfSapFreeACLBuffer
 *
 * Abstract: API to free the buffer allocated from acl pool
 *
 * Input : mem_ptr: Pointer to be freed.
 *
 * Output: None
 *
 * Return: None.
 *
 *******************************************************************************/
void mpaf_pfSapFreeACLBuffer( void* mem_ptr );

//! This API is used by applications to pass message from one to same or to other applications. 
//! This API takes two param, app_id and app_msg. The app_id says which application this msg to pass to.
//! The app msg has the message appended to the defined MPAF_GENERIC_MSG, its taken as base class for the messsage.
//! The code for the MPAF thread shall be set by the pf sap and context field is used by MPAF to save the appid to pass the info
MPAF_STATUS mpaf_SendAppMsg( MPAF_APP_ID app_id, MPAF_GENERIC_MSG_t* app_msg );

//! This API resisters the event and mesage handlers in the MPAF thread.
//! The pf sap stores this callback pointers to the applications instance info and invoke these respective handlers
//! duirg event or message reception. The event mask field carried eevent mask for which the event handler need to be invoked.
//! to unregister call the same funtion with NULL in the handlers
MPAF_STATUS mpaf_pfSapRegisterMpafThreadHndlrs( MPAF_SAP_HANDLE sapHndl, MPAF_EVT_HNDLR_FP evtHndlr, UINT32 evtMask, MPAF_MSG_HANDLER_FP msgHndlr );

/**
 * \addtogroup mpaf_pfsap_dev
 *
 * @{
 */

/**
 * Returns the number of device entries reserved for the given application
 * (\a pfSapHdl).
 *
 * \param pfSapHdl PF SAP handle of the application.
 *
 * \return Total number of device entries reserved for the application.
 */
UINT8 mpaf_pfSapDeviceGetReserved( MPAF_SAP_HANDLE pfSapHdl );

/**
 * Reserves \a nEntries in the device database for the given application
 * (\a pfSapHdl). The reservation info is stored in NVRAM and hence it
 * persists across power cycles.
 *
 * Preferred way of using this API:
 * \code
 *      if (mpaf_pfSapDeviceGetReserved(your_pf_sap_handle) == 0)
 *      {
 *          mpaf_pfSapDeviceReserve(your_pf_sap_handle, your_nentries);
 *      }
 * \endcode
 *
 * \param pfSapHdl PF SAP handle of the application.
 * \param nEntries Number of entries to be reserved.
 *
 * \return MPAF_STATUS_SUCCESS, if the operation was successful. An MPAF error
 *         code, otherwise.
 */
MPAF_STATUS mpaf_pfSapDeviceReserve( MPAF_SAP_HANDLE pfSapHdl, UINT8 nEntries );

/**
 * Adds the given device information (\a devInfo) to the device database and
 * associates it with the given application (\a pfSapHdl).
 *
 * The 'add' operation uses the following rules to add a device:
 *   -# If the device is already present in the database and it is owned by
 *      the given application, then the device information is updated in the
 *      NVRAM with the \a devInfo.
 *   -# If the device is already present but it is owned by some other
 *      application, the add operation fails.
 *   -# If the given application (\a pfSapHdl) has reserved some space and it
 *      has provided a valid #MPAF_PFSAP_DEVICE_ALLOCATOR, it will be used to
 *      allocate an entry for this new device. The application has complete
 *      control over its reserved space.
 *   -# If the application has reserved some space but didn't provide a valid
 *      #MPAF_PFSAP_DEVICE_ALLOCATOR, the "global device allocation strategy"
 *      will be used in its <em>reserved space</em>.
 *   -# All other devices will use "global device allocation strategy" in
 *      common unreserved space.
 *
 * Global Device Allocation Strategy:
 *   -# If there is space, the new device will be allocated a device entry.
 *   -# If there is no space, a least recently used device will be replaced
 *      with the new device irrespective of the owner of the device.
 *
 * Device Added/Deleted Callbacks invocation rules:
 *   -# If the device add operation actually updates an existing device
 *      information, no callbacks will be invoked.
 *   -# If the add operation, replaces an existing device, the
 *      #MPAF_PFSAP_DEVICE_DELETED callback of the device getting replaced will
 *      be invoked first and then the #MPAF_PFSAP_DEVICE_ADDED for the new device
 *      will follow on.
 *   -# In other successful add cases, only the #MPAF_PFSAP_DEVICE_ADDED will be
 *      invoked.
 *
 * \param pfSapHdl PF SAP handle of the application.
 * \param devInfo Device information to be added.
 *
 * \return A valid device handle of the added device, if the operation was
 *         successful. MPAF_PFSAP_DEVICE_HANDLE_INVALID, otherwise.
 *
 * \note The \a devInfo structure is expected to have at least a valid BD
 *       address and Class of Device (COD) fields.
 */
MPAF_PFSAP_DEVICE_HANDLE mpaf_pfSapDeviceAdd( MPAF_SAP_HANDLE pfSapHdl, const MPAF_DEV_INFO *devInfo );

/**
 * Updates the device entry identified by the \a devHdl with the given
 * \a devInfo. The BD address and COD cannot be changed.
 *
 * \param pfSapHdl PF SAP handle of the application.
 * \param devHdl A valid device handle.
 * \param devInfo Device information to be updated.
 *
 * \return MPAF_STATUS_SUCCESS, if the operation was successful. An MPAF error
 *         code, otherwise.
 */
MPAF_STATUS mpaf_pfSapDeviceUpdate( MPAF_SAP_HANDLE pfSapHdl, MPAF_PFSAP_DEVICE_HANDLE devHdl, const MPAF_DEV_INFO *devInfo );

/**
 * Deletes the given device entry (\a devHdl) from the NVRAM device database.
 *
 * \param pfSapHdl PF SAP handle of the application.
 * \param devHdl A valid device handle.
 *
 * \return MPAF_STATUS_SUCCESS, if the operation was successful. An MPAF error
 *         code, otherwise.
 */
MPAF_STATUS mpaf_pfSapDeviceDelete( MPAF_SAP_HANDLE pfSapHdl, MPAF_PFSAP_DEVICE_HANDLE devHdl );

/**
 * Returns the device info structure associated with the given device handle
 * (\a devHdl).
 *
 * \param pfSapHdl PF SAP handle of the application.
 * \param devHdl A valid device handle.
 *
 * \return A valid device info structure associated with the \a devHdl. NULL,
 *         otherwise.
 */
const MPAF_DEV_INFO *mpaf_pfSapDeviceGet( MPAF_SAP_HANDLE pfSapHdl, MPAF_PFSAP_DEVICE_HANDLE devHdl );

/**
 * Searches the device database for the given BD address (\a bda).
 *
 * \param pfSapHdl PF SAP handle of the application.
 * \param bda BD address to be searched.
 * \param p_devInfo Output pointer to store the matched device info structure.
 *                  NULL can be used if device info structure is not needed.
 *
 * \return A valid device handle if the search operation was successful.
 *         MPAF_PFSAP_DEVICE_HANDLE_INVALID, otherwise.
 */
MPAF_PFSAP_DEVICE_HANDLE mpaf_pfSapDeviceFind( MPAF_SAP_HANDLE pfSapHdl, const UINT8 *bda, const MPAF_DEV_INFO **p_devInfo );

/**
 * Invokes the given device handler callback (\a devHandler) for each valid
 * device entry associated with the application (\a pfSapHdl). If the
 * \a devHandler returns TRUE, no further calls will be made even if there are
 * valid device entries remainding for the application.
 *
 * \param pfSapHdl PF SAP handle of the application.
 * \param devHandler Device handler to be called for each valid device entry.
 * \param userData Any data that needs to be passed to the \a devHandler.
 *
 * \return Total number of times the \a devHandler was called.
 *
 * \note The return value <em>0</em> indicates that there are no matching
 *       devices for the given application. This return value can be treated as
 *       the total number of devices associated with the application if and
 *       only if the \a devHandler always returns FALSE (to continue the
 *       search instead of prematurely terminating it).
 */
UINT8 mpaf_pfSapDeviceForeach( MPAF_SAP_HANDLE pfSapHdl, MPAF_PFSAP_DEVICE_HANDLER devHandler, void *userData );

/*******************************************************************************
 * Function: mpaf_EnableApp
 *
 * Abstract: API to enable/disable the specific APP
 *
 * Input : appId   - ID of the App.
 *         appStatus - Enable/Disable the application
 *
 * Output: none.
 *
 * Return: status
 *
 *******************************************************************************/
MPAF_STATUS mpaf_EnableApp( MPAF_APP_ID appId, MPAF_APP_STATUS appStatus );

#define mpaf_pfSapNvmRead(pfSapHdl, vsId, len, data)    mpaf_cfa_ConfigVSRead(vsId, len, data)
#define mpaf_pfSapNvmWrite(pfSapHdl, vsId, len, data)   mpaf_cfa_ConfigVSWrite(vsId, len, data)
#define mpaf_pfSapNvmDelete(pfSapHdl, vsId)             mpaf_cfa_ConfigVSDelete(vsId)

/** @} */   // end: mpaf_pfsap_dev
/** @} */// end: mpaf_pfsap
#ifdef __cplusplus
}
#endif

#endif

#endif   //_MPAF_PF_H_
