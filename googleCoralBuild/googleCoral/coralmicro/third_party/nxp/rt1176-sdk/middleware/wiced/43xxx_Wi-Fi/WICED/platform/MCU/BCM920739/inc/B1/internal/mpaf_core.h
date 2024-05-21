//==================================================================================================
//                        THIS INFORMATION IS PROPRIETARY TO BROADCOM CORP
//--------------------------------------------------------------------------------------------------
//                               Copyright (c) 2011, 2012 Broadcom Corp.
//                                      ALL RIGHTS RESERVED
//==================================================================================================
//! \file
//!
//!Mpaf core  interface file. This has apis which has logics and needs support from mpaf thread.
//
//==================================================================================================

#ifndef _MPAF_CORE_H_
#define _MPAF_CORE_H_

#include "mpaf_thread.h"

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
// Types and constants
//==================================================================================================
//! MPAF core cmd message codes

//! MPAF service message offset
#define MSG_MPAF_SVC_OFFSET                    (256)

//! Message posted to the MPAF thread on periodic timer expiration.
#define MSG_MPAF_CORE_TRAN_SEL                      (MSG_CODE_BASE_MPAF + 0)

//! HCI Related Messages.
#define MSG_MPAF_CORE_HCI_CMD                       (MSG_CODE_BASE_MPAF + 1)
#define MSG_MPAF_CORE_HCI_EVT                       (MSG_CODE_BASE_MPAF + 2)
#define MSG_MPAF_CORE_HCI_ACL_TX                    (MSG_CODE_BASE_MPAF + 3)
#define MSG_MPAF_CORE_HCI_ACL_RX                    (MSG_CODE_BASE_MPAF + 4)

//! defer a process to mpaf thread
//! this message will use derived class MPAF_GENERIC_MSG_t and context will
//! be a pointer to deferred call.
#define MSG_MPAF_CORE_DEFER_PROCESS                  (MSG_CODE_BASE_MPAF + 5)

//! Callback type invoked at the time of receiving any app event
typedef void (*MPAF_CORE_EV_CB)(UINT32 eventBits);

//! Generic void callback type
typedef void (*MPAF_CORE_VOID_CB)(void);

//! Callback type invoked at the time of receiving any app msg
//! If retun NULL the msg will not be freed by the caller.
//! If msg need to be freed by the caller return the msg pointer
typedef MSG_t* (*MPAF_CORE_MSG_CB)(MSG_t* msg);

//! Timer type
//! This enum defines the  two characteristics of timer one is execution context and other is
//! Timer triggering method(Single shot or periodic). Here we give four types using the above two characteristics
//! of timer. 1. Single shot  timer with timer context callback , 2. Single shot with Mpaf context callback
//! 3.Periodic with timer context,4. Periodic with mpaf context.
//! IF timer context is defined by the application the MPAF will invoke the callback in timer caontext otherwise will invoke it in
//! MPAF context
enum
{
    MPAF_SINGLESHOT_TIMER_CONTEXT,
    MPAF_SINGLESHOT_MPAF_CONTEXT,
    MPAF_PERIODIC_TIMER_CONTEXT,
    MPAF_PERIODIC_MPAF_CONTEXT
};
typedef UINT8 MPAF_TIMER_TYPE;

//! MPAF timer callback type define.
//! This is the function type invoke by either MPAF or timer during the trigger
typedef void (MPAF_TIMER_CB)(UINT32 arg);

//! Message used to pass messages from applications  to applications  the applications can use this message
//! header to prepend with its message. The length field is used by the application to identify and
//! parse the message.
typedef struct
{
    //! Timer structure used by the timer module
    OSAPI_TIMER  timer;

    //! Type to differentiate the timer characteristics
    //! If periodic MPAF reloads the timer in every overflow.
    //! If Mpaf context then the  MPAF changes the context of the timer execution.
    MPAF_TIMER_TYPE type;

    //! Timer in MSG queue
    BOOL8           inQueue;
    //! Reserved for future use
    UINT16          reserved_word;

    //! Timer callback funtion pointer
    MPAF_TIMER_CB*  timer_cb;

    // This is the argument value  passed during timer callback execution
    UINT32          arg;

    // This value is used by  periodic timers to reload the timer in the overflow.
    UINT32          timerInterval_us;
} MPAF_CORE_TIMER;

//! MPAF Filter response type
//! This enum defines the different values which can be returned by a handler function
//! for a filter.
//! MPAF_FILTER_PROCESS_NORMALLY:
//!     This indicates to the filter that the Command/Event needs to be handled normally and the
//!     filter should not free or modify the contents of the message.
//! MPAF_FILTER_SUPPRESS_AND_FREE:
//!     This indicates to the filter that the Command/Event message needs be freed by the filter.
//! MPAF_FILTER_SUPPRESS_AND_PASS_TO_MPAF_THREAD:
//!     This indicates to the filter that the Command/Event needs be forwarded to the mpaf thread
//!     for further processing.
enum
{
    MPAF_FILTER_PROCESS_NORMALLY = 0,
    MPAF_FILTER_SUPPRESS_AND_FREE = 1,
    MPAF_FILTER_SUPPRESS_AND_PASS_TO_MPAF_THREAD = 2,
    MPAF_FILTER_SUPPRESS_AND_IGNORE = 3,
};
typedef UINT8 MPAF_FILTER_RESPONSE_TYPE;

//! This this the HCI cmd handler func pointer type. This function is invoked by MPAF thread after filtering
typedef MPAF_FILTER_RESPONSE_TYPE (*MPAF_HCICMD_HANDLER_CB)(const BTHCI_CMD_HDR_t* command);

//! This this the HCI cmd filter func pointer type. This function is invoked by trasport on receiving any HCI cmd
typedef MPAF_FILTER_RESPONSE_TYPE (*MPAF_HCICMD_FILTER_CB)(const BTHCI_CMD_HDR_t* command,
                                                           MPAF_HCICMD_HANDLER_CB *handler);

//! This this the HCI event handler func pointer type. This function is invoked by MPAF thread after filtering
typedef MPAF_FILTER_RESPONSE_TYPE (*MPAF_HCIEVT_HANDLER_CB)(const BTHCI_EVENT_HDR_t* event);

//! This this the HCI event filter func pointer type. This function is invoked by lm on receiving any HCI vent
typedef MPAF_FILTER_RESPONSE_TYPE (*MPAF_HCIEVT_FILTER_CB)(const BTHCI_EVENT_HDR_t* event,
                                                           MPAF_HCIEVT_HANDLER_CB *handler);

//! This this the ACL handler func pointer type. This function is invoked by MPAF thread after filtering
typedef MPAF_FILTER_RESPONSE_TYPE (*MPAF_HCIACL_HANDLER_CB)(const BTHCI_ACL_HDR_t* event);

//! This this the ACL packet filter func pointer type. This function is invoked by lm on receiving any acl packet
typedef MPAF_FILTER_RESPONSE_TYPE (*MPAF_HCIACL_FILTER_CB)(const BTHCI_ACL_HDR_t* event,
                                        MPAF_HCIACL_HANDLER_CB *handler);

//--------------------------------------------------------------------------------------------------
// MPAF Thread initialization
//--------------------------------------------------------------------------------------------------
//! Function interface to iniitalize the mpaf core.
//! This function shall invoked before doing any operations using MPAF interfaces.
void mpaf_core_init(void);

//! This function will install an initialization function to be called by firmware once all the firmware threads have started
//! running. The installed function shall be invoked when the MPAF thread is up
void mpaf_core_installPostFWInit(MPAF_CORE_VOID_CB postFWInitFunct);

//! This function will install an initialization function to be called by firmware after selecting and initializing the transport
void mpaf_core_installPostTransSelect(MPAF_CORE_VOID_CB postFWTransSelect);


//--------------------------------------------------------------------------------------------------
// MPAF event specific functions
//--------------------------------------------------------------------------------------------------

//! This function installs a callback to be triggered whenever any MPAF-specific
//! events are set in the MPAF thread The event bit used is 32 bits
//! Msg handler to get msg passed between threads
//!Msg call back is responsible to free the buffer used by the msg
void mpaf_core_installEventMsgHandler(MPAF_CORE_EV_CB evt_cb,MPAF_CORE_MSG_CB msg_cb);


//! This function sets MPAF-specific event bits to trigger the event handler
//! callback in the MPAF thread. This function internally sends a MPAF msg to the thread.
//! On reception of the msg the MPAF thread invoked the registered callback
//! All 32 bits can be used by applications. No bits are reserved for core
void mpaf_core_setEvent(UINT32 eventBits);

//! This API is used by applications to pass message to MPAF thread
void mpaf_core_sendMsg(MSG_t* app_msg);

//--------------------------------------------------------------------------------------------------
// Timer Functions
//--------------------------------------------------------------------------------------------------

//! Create a software timer. The newly create timer is in de-activated state.
//! mpaf_activateTimer() is required to start the timer. The characteristics of the timer is
//! defined by the parameter type MPAF_TIMER_TYPE. Argument parameter carries the value which will be passed back
//! during timer expiration.
void mpaf_core_createTimer( MPAF_CORE_TIMER* timer, MPAF_TIMER_CB* callback,
                            MPAF_TIMER_TYPE type,UINT32 callbackArg );


//! Api to activate the mpaf timer. The ticks is given in micro seconds.
//! if it's a periodic timer type then this tick is used to reload the timer
void mpaf_core_startTimer( MPAF_CORE_TIMER* timer, UINT32 microseconds_until_notify );


//! Api to deactivate the timer.
void mpaf_core_stopTimer(MPAF_CORE_TIMER* timer);

//! Api to check if the timer is active..
BOOL32 mpaf_core_isTimerActive(MPAF_CORE_TIMER* timer);


//--------------------------------------------------------------------------------------------------
// HCI Command, Event, and ACL Data Filters
//--------------------------------------------------------------------------------------------------

//! Provides the MPAF with an opportunity to filter the HCI command from transport thread to MPAF thread
void mpaf_core_hci_InstallHCICmdFilter(MPAF_HCICMD_FILTER_CB hciCmdFilter);

//! Provides the MPAF with an opportunity to filter the HCI events from lm thread to MPAF thread
void mpaf_core_hci_InstallHCIEventFilter(MPAF_HCIEVT_FILTER_CB hciEvtFilter);

//! Provides the mpaf with an opportunity to filter the received ACL data, with the ACL data packet having been
//! received from over the air and destined for delivery to the host if the filter function returns TRUE
void mpaf_core_hci_InstallACLRxFilter(MPAF_HCIACL_FILTER_CB filterFunct);


//! Provides the CSA with an opportunity to filter the received ACL data, with the ACL data packet having been
//! received from the the host and destined for transmission over the air if the filter function returns TRUE
void mpaf_core_hci_InstallACLTxFilter(MPAF_HCIACL_FILTER_CB filterFunct);

#ifdef __cplusplus
}
#endif

#endif// -MPAF_CORE



