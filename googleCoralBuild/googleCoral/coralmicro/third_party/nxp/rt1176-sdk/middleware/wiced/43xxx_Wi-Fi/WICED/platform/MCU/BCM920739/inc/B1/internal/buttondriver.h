/*******************************************************************************
* ------------------------------------------------------------------------------
*
* Copyright (c), 2011 BROADCOM Corp.
*
*          ALL RIGHTS RESERVED
*
********************************************************************************
*
* File Name: buttondriver.h
*
* Abstract: This file defines the button interface provided by the 20703 core.
* Buttons are implemented as keys on the 20703. A scan matrix of 1 row, n cols
* is created and scanned by the key scan hardware. The hardware reports the
* state of the buttons as keys. This information is translated by the button
* interface into a byte representing various buttons. 
*******************************************************************************/

#ifndef __BUTTON_DRIVER_H__
#define __BUTTON_DRIVER_H__

#include "keyscan.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration
/** \addtogroup  ButtonDriver
* \ingroup HardwareDrivers
*/
/*! @{ */
/**
* This file defines the button interface provided by the 20703 core.
* Buttons are implemented as keys on the 20703. A scan matrix of 1 row, n cols
* is created and scanned by the key scan hardware. The hardware reports the
* state of the buttons as keys. This information is translated by the button
* interface into a byte representing various buttons.
*/

/*******************************************************************************
* Types and Defines
*******************************************************************************/
/// Disable wakeup from all buttons. Clear all pending events.
void btn_wakeupDisable(void);

/// Enable wakeup from all buttons. Also clear any pending events.
void btn_wakeupEnable(void);

/// Return the current button state as a bit mapped value. 0 for a button indicates that
/// the button is not pressed, 1 indicates the button is pressed.
/// \return a 32 bit map representing the current state of the buttons. 
UINT16 btn_getCurrentState(void);

/// Flush any queued button activity in FW or HW and returns the current 
/// state of the buttons. Interpretation of the return value is the same
/// as getCurrentState().
/// \return a 32 bit map representing the current state of the buttons. 
UINT16 btn_flush(void);

/// Returns whether there are any pending unprocessed button events.
/// \return TRUE if there are any pending unprocessed button events, FALSE otherwise. 
BOOL32 btn_eventsPending(void);

/// Register for notification of changes.
/// Once registered, you CAN NOT UNREGISTER; registration is meant to
/// be a startup activity
/// \param userfn points to the function to call when the interrupt
/// comes and matches one of the masks (if you pass a method, it must
/// be static). The function does not need to clear the interrupt
/// status; this will be done by KeyscanDriver::lhlInterrupt(). The
/// function will be called ONCE per interrupt.
/// \param userdata will be passed back to userfn as-is; it can be used to 
/// carry the "this", for example
void btn_registerForInterrupt(void (*userfn)(void*), void* userdata);

// Initialize the button driver
void btn_init(void);

//Process the provided button(keyscan) event
void btn_processEvent(KeyEvent *event);

// Button run time data
typedef struct ButtonData
{
    // Current state of the buttons. Note that this is in report format
    // not the input format from the keyscan driver
    UINT16 curState;
    // Add more members to the report here, for future enhancements.
} ButtonData;

#ifdef __cplusplus
 }
#endif

 /* @}  */
#endif



