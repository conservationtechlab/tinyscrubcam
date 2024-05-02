#ifndef __BB_TYPES_H__
#define __BB_TYPES_H__
/*******************************************************************************
*
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
* File Name: bb_types.h
*
* Abstract:  
*
* Data Members:
*
*******************************************************************************/

//#include "foundation/brcm_fw_types.h"
//#include "fhsinfo.h"

/*******************************************************************************
* Defines
*******************************************************************************/

#define BT_CLK_MSB  0x8000000
#define BT_SLOT_MSB 0x4000000
#define BT_SLOT_MS3B_MASK 0x7000000

#define BCS_BROADCAST_LT_ADDRESS 0

#define BCS_FRAME_SIZE 1250

#define BCS_RF_RAMPUP 182

#define BCS_D_UD                    0x00
#define BCS_D_NULL                  0x00
#define BCS_D_POLL                  0x01
#define BCS_D_FHS                   0x02
#define BCS_D_DM1                   0x03
#define BCS_D_DH1                   0x04
#define BCS_D_DM3                   0x05
#define BCS_D_DH3                   0x06
#define BCS_D_DM5                   0x07
#define BCS_D_DH5                   0x08
#define BCS_D_2DH1                  0x09
#define BCS_D_2DH3                  0x0A
#define BCS_D_2DH5                  0x0B
#define BCS_D_3DH1                  0x0C
#define BCS_D_3DH3                  0x0D
#define BCS_D_3DH5                  0x0E
#define BCS_D_AUX1                  0x0F
#define BCS_D_HV1                   0x10
#define BCS_D_HV2                   0x11
#define BCS_D_HV3                   0x12
#define BCS_D_EV3                   0x13
#define BCS_D_EV4                   0x14
#define BCS_D_EV5                   0x15
#define BCS_D_DV                    0x16
#define BCS_D_2EV3                  0x17
#define BCS_D_2EV5                  0x18
#define BCS_D_3EV3                  0x19
#define BCS_D_3EV5                  0x1A
#define BCS_D_PS                    0x1B

#define BCS_ACL_PKT_TYPE_NULL       0x00
#define BCS_ACL_PKT_TYPE_POLL       0x01
#define BCS_ACL_PKT_TYPE_FHS        0x02
#define BCS_ACL_PKT_TYPE_DM1        0x03
#define BCS_ACL_PKT_TYPE_DH1        0x04
#define BCS_ACL_PKT_TYPE_AUX1       0x09
#define BCS_ACL_PKT_TYPE_DM3        0x0a
#define BCS_ACL_PKT_TYPE_DH3        0x0b
#define BCS_ACL_PKT_TYPE_DM5        0x0e
#define BCS_ACL_PKT_TYPE_DH5        0x0f

#define BCS_ACL_MDR_PKT_TYPE_NULL   0x00
#define BCS_ACL_MDR_PKT_TYPE_POLL   0x01
#define BCS_ACL_MDR_PKT_TYPE_FHS    0x02
#define BCS_ACL_MDR_PKT_TYPE_DM1    0x03
#define BCS_ACL_MDR_PKT_TYPE_2DH1   0x04
#define BCS_ACL_MDR_PKT_TYPE_3DH1   0x08
#define BCS_ACL_MDR_PKT_TYPE_AUX1   0x09
#define BCS_ACL_MDR_PKT_TYPE_2DH3   0x0a
#define BCS_ACL_MDR_PKT_TYPE_3DH3   0x0b
#define BCS_ACL_MDR_PKT_TYPE_2DH5   0x0e
#define BCS_ACL_MDR_PKT_TYPE_3DH5   0x0f

#define BCS_SCO_PKT_TYPE_NULL       0x00
#define BCS_SCO_PKT_TYPE_POLL       0x01
#define BCS_SCO_PKT_TYPE_FHS        0x02
#define BCS_SCO_PKT_TYPE_DM1        0x03
#define BCS_SCO_PKT_TYPE_HV1        0x05
#define BCS_SCO_PKT_TYPE_HV2        0x06
#define BCS_SCO_PKT_TYPE_HV3        0x07
#define BCS_SCO_PKT_TYPE_DV         0x08

#define BCS_ESCO_PKT_TYPE_NULL      0x00
#define BCS_ESCO_PKT_TYPE_POLL      0x01
#define BCS_ESCO_PKT_TYPE_EV3       0x07
#define BCS_ESCO_PKT_TYPE_EV4       0x0c
#define BCS_ESCO_PKT_TYPE_EV5       0x0d

#define BCS_ESCO_PKT_TYPE_2EV3      0x06
#define BCS_ESCO_PKT_TYPE_3EV3      0x07
#define BCS_ESCO_PKT_TYPE_2EV5      0x0c
#define BCS_ESCO_PKT_TYPE_3EV5      0x0d

#define BCS_EDR_ESCO_PKT_TYPE       0x30

#define BCS_LINKTYPE_NULL           0
#define BCS_LINKTYPE_ACL_MDR        1
#define BCS_LINKTYPE_ACL            2
#define BCS_LINKTYPE_ESCO_MDR       3
#define BCS_LINKTYPE_ESCO           4
#define BCS_LINKTYPE_SCO            5

#define BB_TX_PKT_HDR_LT_ADDR_MASK 0x0007
#define BB_TX_PKT_HDR_BBCTL_MASK   0x0380

#define BB_USE_OTHER_TRAIN_ADD_FREQ 16

#define INVALID_SLOT 0xFFFFFFFF

#define DM1_PKT_LENGTH 17
#define DM3_PKT_LENGTH 121
#define DM5_PKT_LENGTH 224

/*******************************************************************************
* Macros
*******************************************************************************/

#define BB_PKTHDR_LT_ADDR(pktHdr) ((pktHdr) & HW_PKT_HDR_STATUS_LT_ADDR)

#define BB_PKTHDR_PKT_TYPE(pktHdr) (((pktHdr) & HW_PKT_HDR_STATUS_PKT_TYPE) >> 3)

#define BB_PKTHDR_FLOW_BIT(pktHdr) (((pktHdr) & HW_PKT_HDR_STATUS_FLOW) >> 7)

#define BB_PKTHDR_ARQN_BIT(pktHdr) (((pktHdr) & HW_PKT_HDR_STATUS_ARQN) >> 8)

#define BB_PKTHDR_HEC_OK(pktHdr) ((pktHdr) & HW_PKT_HDR_STATUS_HEC_OK)

#define BB_PKTLOG_SYNC_TIMEOUT(pktLog) ((pktLog) & HW_PKT_LOG_SYNC_TO)

#define BB_PKTLOG_SYNC_TRIGGER(pktLog) ((pktLog) & HW_PKT_LOG_SYNC_TRIGGER)

#define BB_PKTLOG_CRC_OK(pktLog) ((pktLog) & HW_PKT_LOG_CRC_OK)

#define BB_PKTLOG_PKT_TOO_LONG(pktLog) ((pktLog) & HW_PKT_LOG_PAYLOAD_TOO_LONG)

#define BB_PKTLOG_FEC_OK(pktLog)             \
    (!((pktLog) & (HW_PKT_LOG_FEC_1_3_ERROR | \
                   HW_PKT_LOG_FEC_2_3_ERROR | \
                   HW_PKT_LOG_FEC_NO_FIX_ERROR)))

#define BB_PKTLOG_1B_PYLD_LEN(pktLog) \
    (((pktLog) & HW_PKT_LOG_1BYTE_PYLD_LENGTH) >> \
      HW_PKT_LOG_PYLD_LENGTH_RIGHT_SHIFT_COUNT)

#define BB_PKTLOG_2B_PYLD_LEN(pktLog) \
    (((pktLog) & HW_PKT_LOG_2BYTE_PYLD_LENGTH) >> \
      HW_PKT_LOG_PYLD_LENGTH_RIGHT_SHIFT_COUNT)

#ifdef SECURE_CONNECTIONS
#define BB_PKTLOG_MIC_ERROR(pktLog) ((pktLog) & HW_PKT_LOG_MIC_ERROR)
#endif

/*******************************************************************************
* Type Definitions
*******************************************************************************/

typedef struct
{
    UINT8 ltAddr:3;
    UINT8 type:4;
    UINT8 flow:1;
    UINT8 arqn:1;
    UINT8 seqn:1;
    UINT8 pktValid:1;
    UINT8 reserved:5;
} BB_RX_PACKET_HEADER_TYPE;

typedef struct
{
    UINT16 llid:2;
    UINT16 flow:1;
    UINT16 length:10;
    UINT16 reserved:3;
} BB_RX_PAYLOAD_HEADER_TYPE;

typedef struct
{
    UINT8 ltAddr:3;
    UINT8 type:4;
    UINT8 flow:1;
    UINT8 arqn:1;
    UINT8 seqn:1;
    UINT8 hwPktIndx:5;
    UINT8 reserved:1;
} BB_TX_PACKET_HEADER_TYPE;

typedef struct
{
    UINT16 llid:2;
    UINT16 flow:1;
    UINT16 length:10;
    UINT16 reserved:3;
} BB_TX_PAYLOAD_HEADER_TYPE;

typedef enum
{
    PKT_HDR_NOT_RECEIVED,
    PKT_HDR_BAD,
    PKT_HDR_GOOD,
} PKT_HDR_STATUS;

typedef enum
{
    PKT_PYLD_NOT_RECEIVED,
    PKT_PYLD_BAD,
    PKT_PYLD_GOOD,
    PKT_PYLD_GOOD_NODATA,
    PKT_PYLD_GOOD_RETRY,
} PKT_PYLD_STATUS;

typedef struct
{
    union
    {
        UINT16 halfword;
        BB_RX_PACKET_HEADER_TYPE ps;
    } pktHdr;
    union
    {
        UINT16 halfword;
        BB_RX_PAYLOAD_HEADER_TYPE ps;
    } pyldHdr;
    PKT_HDR_STATUS pktHdrStatus;
    PKT_PYLD_STATUS pktPyldStatus;
    UINT8 pktSlots;
    BOOL8 pktProcessing;
} BB_RX_PACKET_TYPE;

typedef struct
{
    union
    {
        UINT16 halfword;
        BB_TX_PACKET_HEADER_TYPE ps;
    } pktHdr;
    union
    {
        UINT16 halfword;
        BB_TX_PAYLOAD_HEADER_TYPE ps;
    } pyldHdr;
    UINT8 pktSlots;
    BOOL8 sendAclOverScoFrame;
    BOOL8 waitAck;
    BOOL8 pktAcked;
} BB_TX_PACKET_TYPE;

typedef enum
{
    ALL_TYPES_ALLOWED,
    LMP_ONLY,
    L2CAP_ONLY,
} BB_DATA_TYPE;

typedef struct
{
    BB_RX_PACKET_HEADER_TYPE ps;
    BOOL32 flag;
} BB_STORED_DM1_PACKET_TYPE;

typedef struct
{
    BB_RX_PACKET_TYPE rx;
    BB_TX_PACKET_TYPE tx;
    UINT8 *fifoTxData;
    UINT8 ltCh;
    UINT8 linkType;
    UINT8 nextRxSeqn;
    BB_DATA_TYPE bbDataType;
    BB_STORED_DM1_PACKET_TYPE storedDM1;
} BASEBAND_STRUCT;

//typedef struct
//{
//    UINT32 accessCode[2];
//    UINT8 ltAddr;
//    FHS_INFO fhsInfo;
//} BB_FHS_INFO;

typedef enum
{
    TRAIN_A,
    TRAIN_B,
} BB_TRAIN;

#endif // __BB_TYPES_H__
