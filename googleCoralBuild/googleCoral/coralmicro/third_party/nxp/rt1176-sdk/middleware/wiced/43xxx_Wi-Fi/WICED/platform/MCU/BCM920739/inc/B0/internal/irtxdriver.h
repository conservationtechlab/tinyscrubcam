/*
********************************************************************
* THIS INFORMATION IS PROPRIETARY TO
* BROADCOM CORP.
*-------------------------------------------------------------------
*                                                                        
*           Copyright (c) 2011 Broadcom Corp.
*                      ALL RIGHTS RESERVED                              
*                                                                       
********************************************************************

********************************************************************
*    File Name: irtxdriver.h
*
*    Abstract: This file implements an IrTx driver
*
*
********************************************************************/
#ifndef __IR_TX_DRIVER_H__
#define __IR_TX_DRIVER_H__

#include "brcm_fw_types.h"
#include "gpiodriver.h"

/**  \addtogroup irTx
 *  \ingroup HardwareDrivers
*/

/*! @{ */
/**
* The 20730/20733 includes an IRTX Hw block. The IrTx HW 
* can be used to transmit IR Data. This Class defines
* an IrTx driver that can be used by applications such as 
* a Remote Controller app or other related applications 
* to transmit IR commands to the Host.
*
* To use the IRTX class: 
*  
*  (1) Get the IrTx class driver instance first.
*  
*  example:
*   IrTx *irTxDriver = IrTx::getInstance();
*  
*  (2) Configure the IrTx Port and Pin to be used
*        
*  example:
*   BYTE port = 0;
*   BYTE pin  = 4;
*   irTxDriver->setIrTxPortPin(port, pin);
*  
*  (3) Transmit an IR Command using the senData function
*  
*  example:
*  irTxDriver->sendData(testCmdArray , pktLen, irtxClockSet);
*  
*  Please refer to function description for more details.
*  
*/  
#pragma pack(1)
typedef PACKED struct
{
    UINT32  modulateFreq;
    UINT8  clockSrcFreq;
    UINT8  clockSrc;
    BOOL8   invertOutput;
    UINT8   extendedSettings;  
    UINT8   pwmDutyCycleHighCount;
    UINT8   pwmDutyCycleLowCount;
}IR_TX_CLOCK_SETTING;
#pragma pack()

enum
{   
    IR_TX_HW_FIFO_SIZE  = 8,    // IrTx HW Fifo Size
};

// Mia IR Control masks
enum
{
    HW_MIA_IR_CTL_INIT_ENTRY_INDEX_MASK             = 0x0700,
    HW_MIA_IR_CTL_INIT_ENTRY_INDEX_SHIFT            = 8,

    HW_MIA_IR_CTL_RAW_DATA_MASK                     = 0x0040,
    HW_MIA_IR_CTL_RAW_DATA_SHIFT                    = 6,

    HW_MIA_IR_CTL_CARRIER_MODULATE_MASK             = 0x0020,
    HW_MIA_IR_CTL_CARRIER_MODULATE_ENABLE           = 0x0020,
    HW_MIA_IR_CTL_CARRIER_MODULATE_DISABLE          = 0x0000,
    HW_MIA_IR_CTL_CARRIER_MODULATE_SHIFT            = 5,

    HW_MIA_IR_CTL_MODULATE_CLK_SRC_MASK             = 0x0010,
    HW_MIA_IR_CTL_MODULATE_CLK_SRC_ACLK0            = 0x0000,
    HW_MIA_IR_CTL_MODULATE_CLK_SRC_ACLK1            = 0x0010,
    HW_MIA_IR_CTL_MODULATE_CLK_SRC_SHIFT            = 4,

    HW_MIA_IR_CTL_INVERT_OUTPUT_MASK                = 0x0008,
    HW_MIA_IR_CTL_INVERT_OUTPUT_ENABLE              = 0x0008,
    HW_MIA_IR_CTL_INVERT_OUTPUT_DISABLE             = 0x0000,
    HW_MIA_IR_CTL_INVERT_OUTPUT_SHIFT               = 3,

    HW_MIA_IR_CTL_RAW_BIT_SRC_FINAL_MASK            = 0x0004,
    HW_MIA_IR_CTL_RAW_BIT_SRC_FINAL_CMD_BIT_15      = 0x0004,
    HW_MIA_IR_CTL_RAW_BIT_SRC_PRE_BIT               = 0x0000,
    HW_MIA_IR_CTL_RAW_BIT_SRC_FINAL_SHIFT           = 2,

    HW_MIA_IR_CTL_RAW_BIT_SRC_PRE_MASK              = 0x0002,
    HW_MIA_IR_CTL_RAW_BIT_SRC_PRE_PUART_TXD         = 0x0002,
    HW_MIA_IR_CTL_RAW_BIT_SRC_PRE_IR_CTL_6          = 0x0000,
    HW_MIA_IR_CTL_RAW_BIT_SRC_PRE_SHIFT             = 1,

    HW_MIA_IR_CTL_RESET_MASK                        = 0x0001,
    HW_MIA_IR_CTL_RESET_ACTIVE                      = 0x0001,
    HW_MIA_IR_CTL_RESET_INACTIVE                    = 0x0000,
    HW_MIA_IR_CTL_RESET_SHIFT                       = 0,

    HW_MIA_IR_BUF_CTL_WRITE_ENABLE_ALL_MASK         = 0x00FF0000,
    HW_MIA_IR_BUF_CTL_WRITE_ENABLE_ENTRY_0_SHIFT    = 16,
    HW_MIA_IR_BUF_CTL_INT_ENABLE_ALL_MASK           = 0x0000FF00,
    HW_MIA_IR_BUF_CTL_INT_ENABLE_ENTRY_0_SHIFT      = 8,
    HW_MIA_IR_BUF_CTL_READY_BIT_ALL_MASK            = 0x000000FF,
    HW_MIA_IR_BUF_CTL_READY_BIT_ENTRY_0_SHIFT       = 0,

    HW_MIA_IR_CMD_IR_WAIT_TIME_MASK                 = 0x7FFF,
    HW_MIA_IR_CMD_IR_RAW_BIT_MASK                   = 0x8000,

    HW_MIA_IR_CMD0_FIFO_INDEX_MASK                  = 0x70000,
    HW_MIA_IR_CMD0_FIFO_INDEX_SHIFT                 = 16,
    
    HW_MIA_IR_INT_STATUS_ALL_MASK                   = 0x00FF,
    HW_MIA_IR_INT_STATUS_ENTRY_0_SHIFT              = 0,
};

// IR Tx Status enum
typedef enum IR_TX_STATUS_TAG
{
    IR_TRANSMIT_FAIL,
    IR_TRANSMIT_SUCCESS
}   IR_TX_STATUS;

//IrTx State M/C variables
typedef enum IR_TX_STATE_TAG
{
    IR_TX_IDLE,
    IR_TX_PAYLOAD,
    IR_TX_BUSY,
    IR_TX_DONE,          
}IR_TX_STATE;

// Mia IR Control extended masks
enum
{
    HW_MIA_IR_CTL_EXTEND_MODULATE_SRC_PWM0  = 0x00,
    HW_MIA_IR_CTL_EXTEND_MODULATE_SRC_PWM1  = 0x01,
    HW_MIA_IR_CTL_EXTEND_MODULATE_SRC_PWM2  = 0x02,
    HW_MIA_IR_CTL_EXTEND_MODULATE_SRC_PWM3  = 0x03,
    HW_MIA_IR_CTL_EXTEND_MODULATE_SRC_MASK  = 0x03,
    HW_MIA_IR_CTL_EXTEND_ACTIVE_HIGH        = 0x00,
    HW_MIA_IR_CTL_EXTEND_ACTIVE_LOW         = 0x04,
    HW_MIA_IR_CTL_EXTEND_ACTIVE_MASK        = 0x04,
    HW_MIA_IR_CTL_EXTEND_IDLE_LOW           = 0x00,
    HW_MIA_IR_CTL_EXTEND_IDLE_HIGH          = 0x08,
    HW_MIA_IR_CTL_EXTEND_IDLE_MASK          = 0x08,
    HW_MIA_IR_CTL_EXTEND_IR_CYCLE_USEC      = 0x00,
    HW_MIA_IR_CTL_EXTEND_IR_CYCLE_T         = 0x10,
    HW_MIA_IR_CTL_EXTEND_IR_CYCLE_MASK      = 0x10,
    HW_MIA_IR_CTL_EXTEND_ACLK0_BUSY_MASK    = 0x80,

};

#ifdef __cplusplus
extern "C" {
#endif

void  irtx_doneInterruptHandler(void);
void  irtx_doneHandler(void);
void  irtx_updateBufferControl(UINT32 readyUpdate);
void  irtx_configure(IR_TX_CLOCK_SETTING irtxClockSet);
void  irtx_init(void);
void  irtx_processTxData(void);
void    irtx_setIrTxPortPin(BYTE port, BYTE pin);
IR_TX_STATUS   irtx_sendData(const UINT16* sendBuff, UINT32 pktLen, IR_TX_CLOCK_SETTING irtxClockSet);
BOOL8   irtx_isAvailable(void);
void    irtx_abortCurrentTransaction(void);

#ifdef __cplusplus
}
#endif

/* @} */
#endif
