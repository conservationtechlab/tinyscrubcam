//==================================================================================================
//                        THIS INFORMATION IS PROPRIETARY TO BROADCOM CORP
//--------------------------------------------------------------------------------------------------
//                               Copyright (c) 2011 Broadcom Corp.
//                                      ALL RIGHTS RESERVED
//==================================================================================================
//! \file
//!
//! Thread creation and management interface for the ThreadX RTOS.
//
//==================================================================================================


#ifndef _BRCM_TX_THREAD_H_
#define _BRCM_TX_THREAD_H_


#include "brcm_fw_types.h"
#include "tx_api.h"

//==================================================================================================
// Types
//==================================================================================================

//! Thread control block.  To create a thread, simply provide one of these structures, along with
//! the paramters for other thread properties, to thread_Create.
typedef struct THREAD
{
    //! The underlying ThreadX thread control block.
    TX_THREAD tx_thread;

    //! Pointer for maintaining a list of threads.  While ThreadX does have its own list, at a call
    //! to thread_Create, thread creation needs to be deferred until after tx_application_define
    //! (boot_init and fw_init, from the firmware's perspective above the RTOS abstraction layer) so
    //! that stacks can be dynamically allocated, even for threads that needed to be created before
    //! the dynamic memory management module had been initialized.  This pointer allows the thread
    //! service in the foundation to maintain its own list before a thread can be created with the
    //! RTOS.
    struct THREAD* next;

    //! Variable used to keep track of whether a thread has been created and started in ThreadX yet.
    BOOL8 running;
} THREAD_t;


#endif // _BRCM_TX_THREAD_H_

