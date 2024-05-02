#ifndef __BCS_DMA_H__
#define __BCS_DMA_H__
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
* File Name: bcs_dma.h
*
* Abstract:  DMA helper functions.
*
*******************************************************************************/

//#include "bcs_task.h"
#include "bb_types.h"
#if DMA_ENABLED
#include "arm_pl081.h"
#endif
/*******************************************************************************
* Defines & Macros
*******************************************************************************/

#define BCS_DMA_BB_CHANNEL 0


#ifdef BCS_DMA_SEPARATE_RX_TX_ADR

#define BCS_DMA_SEPARATE_RX_TX_EN	0x1

#define     BCS_DMA_RX_MEM_START    rtx_mem_start_adr  
#define     BCS_DMA_TX_MEM_START    rtx_mem_start1_adr  
#else
#define     BCS_DMA_RX_MEM_START    rtx_mem_start_adr  
#define     BCS_DMA_TX_MEM_START    rtx_mem_start_adr  
#endif

#define BCS_DMA_RXTX_TX_DISABLE_MASK      0x01
#define BCS_DMA_RXTX_RX_DISABLE_MASK       0x02
#define BCS_DMA_RXTX_BYTE_LOCATE_EN_MASK   0x04
#define BCS_DMA_RXTX_BYTE_LOCATE_VAL_MASK  0x18
#define BCS_DMA_RXTX_ENABLE_QUEUE          0x40

#define BCS_DMA_RXTX_BYTE_VAL_SHIFT  3

#define BCS_DMA_DMACC0_CONFIG_RD_MASK      0x7FBFF
#define BCS_DMA_DMACC0_CONFIG_EN_MASK      0x00001
#define BCS_DMA_DMACC0_CONFIG_ACTIVE_MASK  0x20000

#define BCS_DMA_ALIGN32(x) ((UINT8*)(((UINT32)(x)) & ~0x3))


#define DMA_RXTX_DISABLE \
    (BCS_DMA_RXTX_RX_DISABLE_MASK | \
     BCS_DMA_RXTX_TX_DISABLE_MASK) 

#define DMA_TX_DISABLE \
    (BCS_DMA_RXTX_TX_DISABLE_MASK)

#define DMA_RX_DISABLE \
    (BCS_DMA_RXTX_RX_DISABLE_MASK | \
     BCS_DMA_RXTX_BYTE_LOCATE_EN_MASK)


// Definitions carried over from legacy dma.h

#define BT_DMAC_CH_CTL_SDWIDTH_1_BYTE_REQUEST           0x00
#define BT_DMAC_CH_CTL_SDWIDTH_2_BYTE_REQUEST           0x01
#define BT_DMAC_CH_CTL_SDWIDTH_4_BYTE_REQUEST           0x02

#define BT_DMAC_CH_CTL_DBSIZE_BIT_OFFSET                15
#define BT_DMAC_CH_CTL_SBSIZE_BIT_OFFSET                12

#define BT_DMAC_CH_CTL_DWIDTH_BIT_OFFSET                21
#define BT_DMAC_CH_CTL_SWIDTH_BIT_OFFSET                18

#define BT_DMAC_CH_CTL_TERMINAL_COUNT_INT_ENABLE_MASK   0x80000000
#define BT_DMAC_CH_CTL_DI_MASK                          0x08000000
#define BT_DMAC_CH_CTL_SI_MASK                          0x04000000

#define BT_DMAC_CH_CFG_IE_MASK                  0x00004000
#define BT_DMAC_CH_CFG_ITC_MASK                 0x00008000

#define BT_DMAC_CONFIGURATION_REGISTER_E_MASK           0x1
#define BT_DMAC_CH_CFG_E_MASK                   BT_DMAC_CONFIGURATION_REGISTER_E_MASK
#define BT_DMAC_CH_CFG_SRC_PERIPHERAL_MASK      0x0000001E
#define BT_DMAC_CH_CFG_DST_PERIPHERAL_MASK      0x000003C0

#define BT_DMAC_CH_CFG_PERPH_BIT_MASK       0x7FE
#define BT_DMAC_CH_CFG_SRC_PERPH_BIT_OFFSET     1
#define BT_DMAC_CH_CFG_DST_PERPH_BIT_OFFSET     6
#define BT_DMAC_CH_CFG_FLOW_CTL_BIT_OFFSET      11

#define BT_DMAC_CH_CFG_FLOW_MEM_TO_PERPH        (0x1 << BT_DMAC_CH_CFG_FLOW_CTL_BIT_OFFSET)
#define BT_DMAC_CH_CFG_FLOW_PERPH_TO_MEM        (0x2 << BT_DMAC_CH_CFG_FLOW_CTL_BIT_OFFSET)
#define BT_DMAC_CH_CFG_PER_FLOW_MEM_TO_PERPH    (0x5 << BT_DMAC_CH_CFG_FLOW_CTL_BIT_OFFSET)
#define BT_DMAC_CH_CFG_PER_FLOW_PERPH_TO_MEM    (0x6 << BT_DMAC_CH_CFG_FLOW_CTL_BIT_OFFSET)

#define CHANNEL_BANK_REG32_SPACING  ( (dmacc1srcaddr_adr - dmacc0srcaddr_adr) / sizeof(UINT32) )

typedef enum
{
    BT_DMAC_CH_CTL_SDBSIZE_1_BYTE_REQUEST,   
    BT_DMAC_CH_CTL_SDBSIZE_4_BYTE_REQUEST,   
    BT_DMAC_CH_CTL_SDBSIZE_8_BYTE_REQUEST,   
    BT_DMAC_CH_CTL_SDBSIZE_16_BYTE_REQUEST,  
    BT_DMAC_CH_CTL_SDBSIZE_32_BYTE_REQUEST,  
    BT_DMAC_CH_CTL_SDBSIZE_64_BYTE_REQUEST,  
    BT_DMAC_CH_CTL_SDBSIZE_128_BYTE_REQUEST, 
    BT_DMAC_CH_CTL_SDBSIZE_256_BYTE_REQUEST, 
    BT_DMAC_DBSIZE_1_WIDTH = BT_DMAC_CH_CTL_SDBSIZE_1_BYTE_REQUEST,
    BT_DMAC_DBSIZE_4_WIDTH = BT_DMAC_CH_CTL_SDBSIZE_4_BYTE_REQUEST,
    BT_DMAC_DBSIZE_8_WIDTH = BT_DMAC_CH_CTL_SDBSIZE_8_BYTE_REQUEST,
    BT_DMAC_DBSIZE_16_WIDTH = BT_DMAC_CH_CTL_SDBSIZE_16_BYTE_REQUEST,
    BT_DMAC_DBSIZE_32_WIDTH = BT_DMAC_CH_CTL_SDBSIZE_32_BYTE_REQUEST,
    BT_DMAC_DBSIZE_64_WIDTH = BT_DMAC_CH_CTL_SDBSIZE_64_BYTE_REQUEST,
    BT_DMAC_DBSIZE_128_WIDTH = BT_DMAC_CH_CTL_SDBSIZE_128_BYTE_REQUEST,
    BT_DMAC_DBSIZE_256_WIDTH = BT_DMAC_CH_CTL_SDBSIZE_256_BYTE_REQUEST
} DMA_BURST_SIZE;

typedef enum
{
    DMAC_LINE_REQUEST_TRANS_PERPH_TO_MEM,   //   0  
    DMAC_LINE_REQUEST_TRANS_MEM_TO_PERPH,   //   1  
    DMAC_LINE_REQUEST_TRANS_ADVANCED_AUDIO, //   2  
    DMAC_LINE_REQUEST_BASEBAND,             //   3  

} DMA_PERIPHERAL;

typedef enum 
{
    DMA_BASEBAND_TX = 0,
    DMA_BASEBAND_LOGICAL_RX = DMA_BASEBAND_TX,
} DMA_CHANNEL;

#if DMA_ENABLED

typedef DMA_TRANSFER_REQUEST_t DMA_REQ_t;
typedef void (*DMA_ISR_DONE_CB)(void);
/*******************************************************************************
* Function Prototypes
*******************************************************************************/

// Internal BB DMA functions

//! Function to initialize BB DMA data structures
void _dmaReqInit(DMA_REQ_t* dmaReq, UINT32 chan,
    UINT32 srcAddr, UINT32 dstAddr, 
    BOOL32 srcIncr, BOOL32 dstIncr, DMA_ISR_DONE_CB cb,
    UINT32 dWidthMask, UINT32 sWidthMask,
    UINT32 dBSizeMask, UINT32 sBSizeMask, 
    UINT32 lineRequest, UINT32 flowControl,
    UINT32* sizeReg, BOOL32 interrupt);
//! Function to set DMA source (for TX DMA)
void _dmaReqSetSrc(DMA_REQ_t* dmaReq, UINT32 src);
//! Function to set DMA destination (for RX DMA)
void _dmaReqSetDst(DMA_REQ_t* dmaReq, UINT32 dst);
//! Function to set length (placeholder only)
void _dmaReqSetLength(DMA_TRANSFER_REQUEST_t* dmaReq, UINT32 length);
//! Function to send the DMA request to HW
void _dmaReqSend(DMA_TRANSFER_REQUEST_t* request);
//! Function to check if DMA transfer is complete
BOOL32 _dmaChanIsXferComplete( UINT32 channel );
#endif

// BCS DMA APIs
void bcs_dmaBlockDisable(void);
void bcs_dmaBlockEnable(void);
void bcs_dmaDisable(void);
UINT8 *bcs_dmaGetRxBuffer(void);
void bcs_dmaInit(void);
void bcs_dmaReset(void);
BOOL32 bcs_dmaRxBufferAvailable(void);
void bcs_dmaRxBufferRecycle(UINT8 *fifoRxData);
UINT8 *bcs_dmaRxDisable(void);
void bcs_dmaRxEnable(void);
void bcs_dmaRxEnableEir(UINT8 *fifoRxData);
void bcs_dmaTxDisable(void);
void bcs_dmaTxEnable(BASEBAND_STRUCT *bb);
void bcs_dmaTxEnableEir(UINT8 *buffer, UINT32 length);

/*******************************************************************************
* Global Declarations
*******************************************************************************/

extern UINT8 *dmaActiveRxBuffer;

/*******************************************************************************
* Inline Function Definitions
*******************************************************************************/

/*******************************************************************************
* Function: bcs_dmaIsDisabled
*
* Abstract: Return true if both receive and transmit DMA are disabled, false
*           otherwise.
*
* Input/Output: Trivial
*
* Return: Boolean
*
*******************************************************************************/
extern BOOL32 bcs_dmaIsDisabled(void);

/*******************************************************************************
* Function: bcs_dmaIsRxEnabled
*
* Abstract: Return true if receive DMA is enabled, false otherwise.
*
* Input/Output: Trivial
*
* Return: Boolean
*
*******************************************************************************/
extern BOOL32 bcs_dmaIsRxEnabled(void);

/*******************************************************************************
* Function: bcs_dmaIsTxEnabled
*
* Abstract: Return true if transmit DMA is enabled, false otherwise.
*
* Input/Output: Trivial
*
* Return: Boolean
*
*******************************************************************************/
extern BOOL32 bcs_dmaIsTxEnabled(void);

/*******************************************************************************
* Function: bcs_dmaIsTransferComplete
*
* Abstract: Return the DMA terminal count status for the baseband DMA channel,
*           which indicates whether the DMA transfer is complete.
*
* Input/Output: Trivial
*
* Return: Boolean
*
*******************************************************************************/
extern UINT32 bcs_dmaIsTransferComplete(void);

#endif
