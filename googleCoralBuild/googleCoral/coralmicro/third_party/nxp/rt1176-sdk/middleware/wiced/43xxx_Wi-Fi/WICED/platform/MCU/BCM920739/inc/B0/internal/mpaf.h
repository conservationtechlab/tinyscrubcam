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
 * File Name: mpaf.h
 *
 * Abstract:  This file provides common definitions specific to MPAF
 *
 *
 * Functions:
 *
 *******************************************************************************/
#ifndef _MPAF_H_
#define _MPAF_H_

#include "foundation/brcm_fw_types.h"
#include "foundation/thread.h"
#include "foundation/msgqueue.h"
#include "foundation/dynamic_memory.h"
#include "foundation/hal/pmu/pmu_idle.h"

#include "bt/btcore/hci/bthci.h"
#include "bt/bt.h"
#include "transport/bttransport.h"
#include "transport/uart/btuarth4.h"
#include "misc/slist.h"
#include "misc/dlist.h"
#include "../foundation/mpaf_core.h"
//==================================================================================================
// Types and constants

//! Command code for MSG_t, indicating that the message contains an HCI event for MPAF OUT Channel.
//! Such a message must be of type BTHCI_ACL_MSG_t.
#define MSG_MPAF_TRANS_DATA_OUT                (MSG_CODE_BASE_MPAF + MSG_MPAF_SVC_OFFSET + 0)

//! Command code for MSG_t, indicating that the message contains an HCI event for MPAF IN Channel.
//! Such a message must be of type BTHCI_ACL_MSG_t.
#define MSG_MPAF_TRANS_DATA_IN                 (MSG_CODE_BASE_MPAF + MSG_MPAF_SVC_OFFSET + 1)

//! Internal Documentation
#define MSG_MPAF_TRANS_DATA                    (MSG_CODE_BASE_MPAF + MSG_MPAF_SVC_OFFSET + 2)
#define MSG_MPAF_USB_HID_CTRL                  (MSG_CODE_BASE_MPAF + MSG_MPAF_SVC_OFFSET + 3)
//! USB HID Related Message
#define MSG_MPAF_HID_KB_EVT                     (MSG_CODE_BASE_MPAF + MSG_MPAF_SVC_OFFSET + 4)
#define MSG_MPAF_HID_MS_EVT                     (MSG_CODE_BASE_MPAF + MSG_MPAF_SVC_OFFSET + 5)
#define MSG_MPAF_HID_KB_CTRL                    (MSG_CODE_BASE_MPAF + MSG_MPAF_SVC_OFFSET + 6)
#define MSG_MPAF_HID_MS_CTRL                    (MSG_CODE_BASE_MPAF + MSG_MPAF_SVC_OFFSET + 7)

//! Message posted to the MPAF thread to enable/disable the specific application.
#define MSG_MPAF_ENABLE_APP                     (MSG_CODE_BASE_MPAF + MSG_MPAF_SVC_OFFSET + 8)

#define MSG_MPAF_APP_MSG                        (MSG_CODE_BASE_MPAF + MSG_MPAF_SVC_OFFSET + 9)

#define MSG_MPAF_TRANS_STATE_IND                (MSG_CODE_BASE_MPAF + MSG_MPAF_SVC_OFFSET + 10)

//! Message used to forward an MPAF packet from any transport to the HCI thread.  Such a message
//! should have its msg.code field set to TODO-TBD, should be allocated using TODO-TBD
//! and released using TODO-TBD.  Data begins immediately after this structure, which has a size of
//! 12 bytes.  That way, the data will be aligned to a 32-bit word boundary to accommodate DMA for
//! transfer of the data payload.
typedef struct
{
        //! Message base class.
        MSG_t msg;

        //! flags to be used to share any mpaf specific info between the layers.
        //  Defined to 8bit  .
        UINT8 flags;

        //! Mask to obtain the logical qid.   The Logical Queue ID is obtained from 
        //! ( (flags & BTHCI_MPAF_MSG_QID_MASK) ) in BTHCI_MPAF_MSG_t.
        // QID will useful to refer back the logical queue information on transfer
        // completion.
#define BTHCI_MPAF_MSG_QID_MASK                               0x0F

        //! Mask to obtain the skip_header state.   The skip_header state is obtained from
        //! ( (flags & BTHCI_MPAF_MSG_SKIP_HDR_MASK) ) in BTHCI_MPAF_MSG_t.
        // If set, transport driver will skip insertion of the custom packet identifier
        // through synchronous write.
#define BTHCI_MPAF_MSG_SKIP_HDR_MASK                          0x10

        //! Mask to obtain the flex data mode.   The flex data mode. state is obtained from
        //! ( (flags & BTHCI_MPAF_MSG_FLEX_DATA_MODE) ) in BTHCI_MPAF_MSG_t.
        // If set, transport driver will send the received byte as it is without doing any insertion
        // or deletion through synchronous write.
#define BTHCI_MPAF_MSG_FLEX_DATA_MODE                         0x20

        // No pad byte: that way an event being transmitted can be sent almost entirely using DMA,
        // starting with its header and continuing through its payload until any trailing length % 4
        // bytes.  This is unlike an HCI command, whose header needs to be read before DMA can be used
        // to transfer its payload data.

        //! pti to be used to share specific pti with respet to BTHCI_MPAF_MSG_SKIP_HDR_MASK.
        //  Defined to 8bit  .
        UINT8 pti;

        //! Offset is used by the protocol driver to find the starting point of data.
        //  Defined to 16bit. This supports the zero copy feature used in BT stack.
        //  The offset length is the bytes used by the BT stack header (HCI, L2CAP, HID-RFCOMM ..)
        UINT16 offset;

        //! HCI MPAF packet header.
        BTHCI_MPAF_HDR_t hdr;
} BTHCI_MPAF_MSG_t;

/*******************************************************************************
 *                   Defines & Macros
 *******************************************************************************/

#define MPAF_RAWMODE_BUFFERSIZE  ( 512 )
#define MPAF_RAWMODE_TICK        ( 500 )

//! Macro to catch asserts during compilation time
#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]

//! Flag indicating support for stored linkkey support in NVRAM.
#define MPAF_SUPPORT_READ_STORED_LINK_KEY    (TRUE)

//! Invalid handle that would be returned by the sap in case of an open failure.
#define INVALID_SAP_HANDLE (0)

/*
 * BT stack ACL up path zero copy offset mangement macros
 */
#define MPAF_SET_ACLBUFF_OFFSET(_ptr,_offset) (*((_ptr)-1) = (_offset))
#define MPAF_GET_ACLBUFF_OFFSET(_ptr,_offset) ( (_offset)  =*((_ptr)-1))
#define MPAF_ACLBUFF_START_PTR(_ptr) (_ptr =(void*)((UINT8*)_ptr - *((UINT8*)_ptr - 1)))

/*******************************************************************************
 *                   Type Definitions
 *******************************************************************************/

//! Type definition for Sap Handle.
typedef UINT32 MPAF_SAP_HANDLE;

//! Application IDs of the known apps to MPAF.

/*
 * MPAF_APP_ID_TYPE
 * Application IDs of the known apps to MPAF
 * Maximum App ID mask (32): 0 to 31
 */
typedef enum
{
    MPAF_APP_UHE,            // UHE_APP
    MPAF_APP_MAGIC_RMT,      // Remote App (LGE)
    MPAF_APP_SPP,            // SPP
    MPAF_APP_NINTENDO,       // NINTENDO_APP
    MPAF_APP_THREE_D_TV,     // THREE_D_TV_APP
    MPAF_APP_THREE_D_PC,     // THREE_D_PC_APP
    MPAF_APP_THREE_D_IR2BT,  // THREE_D_IR2BT_APP
    MPAF_APP_PNP_HEADSET,    // PNP_HEADSET_APP
    MPAF_APP_A4WP,
    MPAF_APP_3DTV_LGE,
    MPAF_APP_3DTV_PANASONIC,
    MPAF_APP_3DTV_TCL,
    MPAF_APP_WICED = 12,
    MPAF_APP_ATCE = 13,
    MPAF_APP_TVWAKE = 14,
    MPAF_APP_BLET,

    // IMPORTANT NOTE:
    // MPAF_APP_TEST and MPAF_APP_COD_ZERO are used only for testing, so 
    // the test application does not allocate the dynamic App Id.
    MPAF_APP_TEST = 15,
#if defined(MPAF_DEVICE_ALLOW_COD_ZERO)
    // It is intentional to keep both MPAF_APP_TEST and MPAF_APP_COD_ZERO to
    // have same value.
    MPAF_APP_COD_ZERO = 15,
#endif

    // IMPORTANT NOTE:
    // Replace the below Reserved Entries when adding a new App.
    MPAF_APP_HIDD, //we use common APP ID for MS/KB & RC in the device mode
    MPAF_APP_BLEAPP,
    MPAF_APP_I15DOT4,
    MPAF_APP_RF4CE,
    MPAF_APP_ZB_STACK,
    MPAF_APP_FPU_TEST,
    MPAF_APP_6LOWAPP,
    MPAF_APP_WICED_BT,
    MPAF_APP_RSVD_24,
    MPAF_APP_RSVD_25,
    MPAF_APP_RSVD_26,
    MPAF_APP_RSVD_27,
    MPAF_APP_RSVD_28,
    MPAF_APP_RSVD_29,
    MPAF_APP_RSVD_30,
    MPAF_APP_RSVD_31,

    MPAF_APP_ID_MAX = 16, /* Maximum number of active Apps supported */
    MPAF_APP_ID_INVALID = 0xFF
} MPAF_APP_ID_TYPE;
typedef UINT8 MPAF_APP_ID;

//! Various status codes returned by the framework code 
typedef enum
{
    MPAF_STATUS_SUCCESS = 0x00, MPAF_STATUS_PENDING = 0x01, MPAF_STATUS_BUSY = 0x02, MPAF_STATUS_PASS_ON = 0x03, MPAF_STATUS_FAILED = 0x04, MPAF_STATUS_INVALID_PARAMETER = 0x05, MPAF_STATUS_SDP_DONE = 0x06, MPAF_STATUS_INTERNAL_FAILURE = 0x07, MPAF_STATUS_PAGE_TIMEOUT = 0x08, MPAF_STATUS_AUTH_FAILURE = 0x09, MPAF_STATUS_CONN_TIMEOUT = 0x0A, MPAF_STATUS_MEM_CAPACITY_EXCEEDED = 0x0B, MPAF_STATUS_COMMAND_DISALLOWED = 0x0C, MPAF_STATUS_UNKNOWN_COMMAND = 0x0D,

} MPAF_STATUS_TYPE;
typedef UINT8 MPAF_STATUS;

//==================================================================================================
// Functions
//==================================================================================================

/*******************************************************************************
 * Function: mpaf_transGetMemBlock
 *
 * Abstract: Function interface to the transport to do channel specific
 *           allocation for Out Data(Host to Controller).
 *
 *
 * Input:   pktType: Packet Indicator
 *          dataPtr : in coming header pointer.
 *
 * Output:  None.
 *
 * Return:  data pointer.
 *
 * Description:
 *
 *******************************************************************************/
void* mpaf_transGetMemBlock( UINT8 pktType, UINT8* transHdr );
/*******************************************************************************
 * Function: mpaf_transMemFree
 *
 * Abstract: Function interface to the core to free the in bound Data pointer
 *           in case of a error situation.
 *
 *
 * Input:   pktType: Type of the packet to support multiple free variants.
 *          dataPtr.
 *
 * Output:  None.
 *
 * Return:  None.
 *
 * Description:
 *
 *******************************************************************************/
void mpaf_transMemFree( UINT8 pktType, UINT8 *dataPtr );

/*******************************************************************************
 * Function: mpaf_transProcHostData
 *
 * Abstract: Function interface to the core to indicate the Out Data arrival.
 *
 *
 * Input:   dataPtr : in coming data pointer.
 *          dataLen : data length
 *
 * Output:  None.
 *
 * Return:  None.
 *
 * Description:
 *
 *******************************************************************************/
void mpaf_transProcHostData( MSG_t* msg );

/*******************************************************************************
 * Function: mpaf_copy_config_data
 *
 * Abstract: Copies the MPAF config data to respective MPAF variables. These
 *           copied values will be used later for further processing.
 *
 * Input/Output:  pData - Pointer to the start of the MPAF Config Region.
 *
 * Return:  None.
 *
 *******************************************************************************/
void mpaf_copy_config_data( UINT8 *pData );

/*******************************************************************************
 * Function: mpaf_process_config_data
 *
 * Abstract: Function interface to process the config data.
 *
 *
 * Input/Output: None.
 *
 * Return:  None.
 *
 *******************************************************************************/
void mpaf_process_config_data( void );

/*******************************************************************************
 * Function: mpaf_enable_through_uhe_config
 *
 * Abstract: This function enables MPAF and MPAF UHE application. It is expected
 *           to be called (only when UHE is enabled) by the UHE config
 *           processing logic.  It is provided for backward compatability
 *           reasons. For example, by just having UHE configuration, the MPAF
 *           can be enabled internally.
 *
 * Input/Output: None
 *
 * Return:  None
 *
 *******************************************************************************/
void mpaf_enable_through_uhe_config( void );

/*******************************************************************************
 * Function: mpaf_mm_sbrk
 *
 * Abstract: Allocates memory from the heap (governed by mm_init/mm_top).
 *           It utilizes the mm_sbrk() API to allocate the memory. The
 *           memory allocated using this method cannot be freed. If the
 *           allocation would result in exceeding the memory configured
 *           for MPAF, then the system will spin indefinitely.
 *
 * Input : size - Number of bytes requested to be allocated.
 *
 * Output: None.
 *
 * Return: Address of the newly allocated memory block (which is of at least
 *         'size' bytes).
 *
 * NOTE: This function will never return if the requested number of bytes
 *       cannot be allocated. To avoid getting into this trap, either the
 *       memory usage has to be optimized or MPAF needs to be configured
 *       to use more memory.
 *******************************************************************************/
void *mpaf_mm_sbrk( UINT32 size );

/*******************************************************************************
 * Function:  mpaf_get_app_id_from_handle
 *
 * Abstract:  This function is called to get the Application Id for the allocated
 *            Application handle.
 *
 * Input/Output:  app_handle - Application handle dynamically allocated during
 *                             installation.
 *
 * Return:  returns the actual App Id.
 *
 * Description:
 *
 *******************************************************************************/
MPAF_APP_ID mpaf_get_app_id_from_handle( UINT8 app_handle );

/*******************************************************************************
 * Function:  mpaf_get_handle_from_app_id
 *
 * Abstract:  This function is called to get the matching application handle
 *            for the installed appliction Id.
 *
 * Input/Output:  i_app_id - Installed App Id of the application.
 *
 * Return:  returns the dynamic App Id mapping to the installed App Id.
 *
 * Description: This function may be used to get the dynamic App Id when the
 *              installed App Id is known, mostly from the NV data.
 *
 *******************************************************************************/
UINT8 mpaf_get_handle_from_app_id( MPAF_APP_ID app_id );

#endif   //_MPAF_H_

