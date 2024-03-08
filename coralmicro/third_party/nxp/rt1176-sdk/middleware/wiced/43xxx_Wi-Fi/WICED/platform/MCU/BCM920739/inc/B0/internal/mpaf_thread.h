//==================================================================================================
//                        THIS INFORMATION IS PROPRIETARY TO BROADCOM CORP
//--------------------------------------------------------------------------------------------------
//                               Copyright (c) 2011 Broadcom Corp.
//                                      ALL RIGHTS RESERVED
//==================================================================================================
//! \file
//!
//! Header for Bluetooth Host Controller Interface and Link Manager thread infrastructure.
//
//==================================================================================================


#ifndef _MPAF_THREAD_H_
#define _MPAF_THREAD_H_


#include "foundation/brcm_fw_types.h"
#include "foundation/msgqueue.h"
#include "foundation/thread.h"

//! MPAF thread object.  This defines the basic MPAF thread management class
//! 
typedef struct MPAF_TASK
{
    //! Thread control block for the mpaf thread.  After mpaf_Init has been called, this
    //! member would be initialized.
    THREAD_t    thread;

    //! Message queue for use by the MPAF thread.  The message queue will be serviced by
    //! mpaf_Main   Generally, the specific transport can just use bttransport_Main, which will
    //! execute whenever there is work to do (whenever a message arrives), in
    //! which the mpaf implemenation can do its work.
    MSGQUEUE_t  msgqueue;
    //! Config item used to configure the MPAF thread stack size.  When a config item is not used,
    //! the default value will be used.
    UINT16      stackSize;

} MPAF_TASK_t;

//! MPAF Generic Msg handler type.
//! Applications can define the handler which will be called in mpaf context when the 
//! generic msg is received.
typedef void (*mpaf_generic_msg_handler_t) (UINT32 context);

//! MPAF Message format used to send an application specific context.
//! The Message code will define the context usage.
typedef struct
{
    //! Message base class.
    MSG_t           msg;
    mpaf_generic_msg_handler_t handler;
    UINT32          context;
    
} MPAF_GENERIC_MSG_t;

//==================================================================================================
// Functions
//==================================================================================================

//! Initializes the infrastructure for the thread in which the MPAF will run.
extern void mpaf_thread_Init(void);

//! Sends the specified message to the MPAFsubsystem, which run in a thread.  MPAF, its applications 
//! and the embedded BT stack subsystems share a single thread with each other.  
//! The dynamic allocation mechanism that
//! will eventually be used to release the message actually depends on the type of MSG_t indicated
//! by msg->code.  Each message type documents its own allocation and release semantics.
extern void mpaf_thread_SendMessageToThread(TAKES MSG_t* msg);

//! Sends the specified message code to the MPAF or application
//! subsystem, which are running in their own thread. This function takes code as input and creates
//! the msg and call msgqueue_Put with the specified msg and the MPAF thread's message queue.
extern void mpaf_thread_SendMessageCodeToThread(UINT32 msgCode);

//==================================================================================================
//! \internal
//! Sends the generic message to the MPAF
//! subsystem, which are running in their own thread. This function takes code, handler and context as inputs
//! and creates the msg and call msgqueue_Put with the specified msg and the MPAF thread's message queue.
extern void mpaf_thread_SendGenericMessageToThread(UINT32 msgCode, UINT32 handler, UINT32 context);

//==================================================================================================
//! Checks whether if it is being run from MPAF context.
//! This check is needed if we want to enforce running from MPAF thread.
BOOL32 mpaf_thread_IsMpafContext(void);

#ifdef FIX_CQ_3462777
//! Delete the MPAF thread.
extern void mpaf_thread_Delete(void);

//! Re-initializes the MPAF thread after the resume from SDS test mode.
extern void mpaf_thread_Reinit(void);
#endif

#endif //_MPAF_THREAD_H_
