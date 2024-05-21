/*
********************************************************************
* THIS INFORMATION IS PROPRIETARY TO
* BROADCOM CORP.
*-------------------------------------------------------------------
*                                                                        
*           Copyright (c) 2004 Broadcom Corp.
*                      ALL RIGHTS RESERVED                              
*                                                                       
********************************************************************

 ********************************************************************
 *    File Name: error.h
 *
 *    Abstract:  Error status definition.
 *
 *    Functions:
 *            --     
 *
 *    $History:$ 
 *
 ********************************************************************
*/

#ifndef  _ERROR_H_
#define _ERROR_H_


typedef enum BT_ERROR_STATUS_TAG
{
    STATUS_RTOS_FAILURE ,  // 0 

    // This is non error value
    STATUS_OK ,			

    STATUS_RTOS_TIMEOUT ,

    STATUS_QUEUE_BASE			= 0x10,
    STATUS_QUEUE_FAIL_TO_SEND,
    STATUS_QUEUE_EMPTY,
    STATUS_QUEUE_IS_NOT_EMPTY,

    // This is not an error, it is just an indication to release memory 
    //since we are in Loopback mode
    STATUS_LOOPBACK_RELEASE_MEMORY,		

    STATUS_L2CAP_BASE				= 0x20,
    STATUS_L2CAP_INVALID_APP_ID,

    // This means that a L2CAP connection was created yet
    STATUS_L2CAP_INVALID_CID,					
    STATUS_L2CAP_REGISTERATION_ERROR,
    STATUS_L2CAP_REGISTERATION_TABLE_FULL,
    STATUS_L2CAP_SENDING_BUSY,
    STATUS_L2CAP_FAILED_TO_ALLOCATE,
    STATUS_L2CAP_PSM_NOT_SUPPORTED,
    STATUS_L2CAP_CONFIG_IN_PROCESS,
    STATUS_L2CAP_STATE_NOT_READY,
        

    // This means that the embedded host was sending too many 
    // packets without receiving num of pkt cmop event
    STATUS_EMBEDDED_HOST_PACKET_NO_CREDIT,  

    STATUS_rm_ACL_LINK_NOT_EXIST,

    STATUS_TRANSPORT_FAILED_TO_ALLOCATE,


    STAUTS_DMA_BASE                 = 0x1000,
    STATUS_DMA_BUSY,

    STATUS_GPIO_BASE                = 0x1100,
    STATUS_GPIO_ISR_REGISTER_ERROR,   


    // Make BT_STATUS as UINT32 type
    STATUS_LAST = (int)0xffffffff,
	
}BT_STATUS;


typedef enum
{
    BT_SUCCESS                                                      = 0x00,
    BT_ERROR_CODE_UNKNOWN_HCI_COMMAND                               = 0x01,
    BT_ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                     = 0x02,
    BT_ERROR_CODE_HARDWARE_FAILURE                                  = 0x03,
    BT_ERROR_CODE_PAGE_TIMEOUT                                      = 0x04,
    BT_ERROR_CODE_AUTHENTICATION_FAILURE                            = 0x05,
    BT_ERROR_CODE_PIN_MISSING                                       = 0x06,
    BT_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED                          = 0x07,
    BT_ERROR_CODE_CONNECTION_TIMEOUT                                = 0x08,
    BT_ERROR_CODE_CONNECTION_LIMIT_EXCEEDED                         = 0x09,
    BT_ERROR_CODE_SCO_CONNECTION_LIMIT_TO_A_DEVICE_EXCEEDED         = 0x0A,
    BT_ERROR_CODE_ACL_CONNECTION_ALREADY_EXISTS                     = 0x0B,
    BT_ERROR_CODE_COMMAND_DISALLOWED                                = 0x0C,
    BT_ERROR_CODE_CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES      = 0x0D,
    BT_ERROR_CODE_CONNECTION_REJECTED_DUE_TO_SECURITY_REASONS       = 0x0E,
    BT_ERROR_CODE_CONNECTION_REJECTED_DUE_TO_UNACCEPTABLE_BD_ADDR   = 0x0F,
    BT_ERROR_CODE_CONNECTION_ACCEPT_TIMEOUT_EXCEEDED                = 0x10,
    BT_ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE            = 0x11,
    BT_ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS                    = 0x12,
    BT_ERROR_CODE_REMOTE_USER_TERMINATED_CONNECTION                 = 0x13,
    BT_ERROR_CODE_REMOTE_END_DUE_TO_LOW_RESOURCES                   = 0x14,
    BT_ERROR_CODE_REMOTE_END_DUE_TO_POWER_OFF                       = 0x15,
    BT_ERROR_CODE_CONNECTION_TERMINATED_BY_LOCAL_HOST               = 0x16,
    BT_ERROR_CODE_REPEATED_ATTEMPTS                                 = 0x17,
    BT_ERROR_CODE_PAIRING_NOT_ALLOWED                               = 0x18,
    BT_ERROR_CODE_UNKNOWN_LMP_PDU                                   = 0x19,
    BT_ERROR_CODE_UNSUPPORTED_REMOTE_FEATURE                        = 0x1A,
    BT_ERROR_CODE_SCO_OFFSET_REJECTED                               = 0x1B,
    BT_ERROR_CODE_SCO_INTERVAL_REJECTED                             = 0x1C,
    BT_ERROR_CODE_SCO_AIR_MODE_REJECTED                             = 0x1D,
    BT_ERROR_CODE_INVALID_LMP_PARAMETER                             = 0x1E,
    BT_ERROR_CODE_UNSPECIFIED_ERROR                                 = 0x1F,
    BT_ERROR_CODE_UNSUPPORTED_LMP_PARAMETER_VALUE                   = 0x20,
    BT_ERROR_CODE_ROLE_CHANGE_NOT_ALLOWD                            = 0x21,
    BT_ERROR_CODE_LMP_RESPONSE_TIMEOUT                              = 0x22,
    BT_ERROR_CODE_ERROR_TRANSACTION_COLLISION                       = 0x23,
    BT_ERROR_CODE_LMP_PDU_NOT_ALLOWED                               = 0x24,
    BT_ERROR_CODE_ENCRYPTION_MODE_NOT_ACCEPTABLE                    = 0x25,
    BT_ERROR_CODE_UNIT_KEY_USED                                     = 0x26,
    BT_ERROR_CODE_QOS_NOT_SUPPORTED                                 = 0x27,
    BT_ERROR_CODE_INSTANT_PASSED                                    = 0x28,
    BT_ERROR_CODE_PAIRING_W_UNIT_KEY_NOT_SUPPORTED                  = 0x29,
    BT_ERROR_CODE_DIFFERENT_TRANSACTION_COLLISION                   = 0x2A,
    BT_ERROR_CODE_QoS_UNACCEPTABLE_PARAMETER                        = 0x2C,
    BT_ERROR_CODE_QoS_REJECTED                                      = 0x2D,
    BT_ERROR_CODE_CHANNEL_CLASSIFICATION_NOT_SUPPORTED              = 0x2E,
    BT_ERROR_CODE_INSUFFICIENT_SECURITY                             = 0x2F,
    BT_ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE                  = 0x30,
    BT_ERROR_CODE_ROLE_SWITCH_PENDING                               = 0x32,
    BT_ERROR_CODE_RESERVED_SLOT_VIOLATION                           = 0x34,
    BT_ERROR_CODE_ROLE_SWITCH_FAILED                                = 0x35,
    BT_ERROR_CODE_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE               = 0x36,
    BT_ERROR_CODE_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST              = 0x37,
    BT_ERROR_CODE_HOST_BUSY_TO_DO_SIMPLE_PAIRING                    = 0x38,
#ifdef ULP_ENABLE
    BT_ERROR_CODE_LE_UNACCEPTABLE_CONNECTION_INTERVAL               = 0x3B,
    BT_ERROR_CODE_LE_DIRECTED_ADVERTISING_TIMEOUT                   = 0x3C,
    BT_ERROR_CODE_LE_CONNECTION_TERMINATED_DUE_TO_MIC_FAILURE       = 0x3D,
    BT_ERROR_CODE_LE_CONNECTION_FAILED_TO_BE_FULLY_ESTABLISHED      = 0x3E,
#endif
    BT_ERROR_CODE_COARSE_CLOCK_ADJUSTMENT_REJECTED_BUT_WILL_TRY_DRAGGING = 0x40,
#ifdef TBFC
    BFC_GENERAL_FAILURE                                             = 0x50,
    BFC_REMOTE_NEVER_SUSPENDABLE                                    = 0x51,
    BFC_REMOTE_NOT_CURRENTLY_SUSPENDABLE                            = 0x52,
    BFC_CONNECTION_CURRENTLY_SUSPENDED                              = 0x53,
    BFC_RESUME_FAIL                                                 = 0x54,
#endif
    BT_ERROR_CODE_REJECT_BY_MASK                                    = 0xFF,
} BT_ERROR;


/*
*******************************************************************************
*
* $HISTORY:$
*
*******************************************************************************
*/
#endif	// _ERROR_H_
