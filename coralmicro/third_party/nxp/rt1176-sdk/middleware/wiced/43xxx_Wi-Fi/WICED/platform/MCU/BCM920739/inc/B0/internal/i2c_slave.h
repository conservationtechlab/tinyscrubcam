/*******************************************************************
* THIS INFORMATION IS PROPRIETARY TO BROADCOM CORP.
* ------------------------------------------------------------------
*                                                                        
*           Copyright (c) 2015 Broadcom Corp.
*                ALL RIGHTS RESERVED                              
*                                                                       
********************************************************************
********************************************************************
*   File Name:      i2c_slave.h
*
*   Description:    This file implements the 20739 FW 
*                   interface for I2C Slave.
*                   
*   Author:         Ishfan Vakil
*   History:        01/13/2015      Created
*******************************************************************/ 

#ifndef __I2C_SLAVE_DRIVER_H__
#define __I2C_SLAVE_DRIVER_H__

//==================================================================
// Header File Includes
//==================================================================
#include "brcm_fw_types.h"

/**  \addtogroup I2C
*    \ingroup HardwareDrivers
*/
/*! @{ */
/**
*   Defines the BCM I2C Slave driver. 
*
*/

#ifdef __cplusplus
extern "C" {
#endif

// I2C Rx states
enum
{
    I2C_RX_IDLE,
    I2C_RX_WAIT_FOR_PKT,
    I2C_RX_DONE,
};

// I2C Tx states
enum
{
    I2C_TX_IDLE,
    I2C_TX_BUSY,
};

// For Driver Validation, let us use a buffer of 32 bytes.
#define MAX_PACKET_LENGTH            32
    
typedef struct I2C_SLAVE_TRANSPORT_PACKET_TAG
{
    // Rx Packet Types
    UINT8*                          rxPktPtr;        /* Rx packet pointer */
    UINT16                          rxPktLength;     /* Rx packet length */
    UINT16                          rxByteCnt;       /* total number of byte in FIFO */

    // Tx Packet Types
    UINT8*                          txPktPtr;        /* Tx packet pointer*/
    UINT16                          txPktLength;     /* Tx packet length */
    UINT8                           TxTransportHdr;
    UINT8                           RxTransportHdr;

    UINT8                           txState;
    UINT8                           rxState;
    UINT16                          bufferedPktLength;

}I2C_SLAVE_TRANSPORT_PACKET;

typedef I2C_SLAVE_TRANSPORT_PACKET tI2CSlaveDrvState;

enum
{
    I2C_SLAVE_ADDR_0_VALID = 0x01,
    I2C_SLAVE_ADDR_1_VALID = 0x02,
    I2C_SLAVE_ADDR_2_VALID = 0x04,
    I2C_SLAVE_ADDR_3_VALID = 0x08,
    I2C_SLAVE_ADDR_ALL_VALID = 0x0F
};

enum
{
    I2C_TRAN_EVENT_INVALID,
    I2C_TRAN_EVENT_MASK_RX_DONE,
    I2C_TRAN_EVENT_MASK_TX_DONE,
};

//==================================================================
// #defines
//==================================================================
#define I2C_REG                                     REG32
#define I2C_SLAVE_RX_FIFO_THRESHOLD_DEFAULT         32

#define MASTER_SLAVE_MODE_DISABLE                   0x0
#define MASTER_SLAVE_MODE_MASTER                    0x1
#define MASTER_SLAVE_MODE_SLAVE                     0x2
#define I2C_STATUS_I2C_IDLE_MASK                    0x10
#define I2C_STATUS_TX_FULL_MASK                     0x08
#define I2C_STATUS_TX_EMPTY_MASK                    0x04
#define I2C_STATUS_RX_FULL_MASK                     0x02
#define I2C_STATUS_RX_EMPTY_MASK                    0x01
                                                    
#define I2C_ISR_TX_FULL_MASK                        0x01
#define I2C_ISR_TX_AEMPTY_MASK                      0x02
#define I2C_ISR_RX_AFULL_MASK                       0x04
#define I2C_ISR_RX_EMPTY_MASK                       0x08
#define I2C_ISR_WAKEUP_MASK                         0x80
#define I2C_ISR_RX_TRANSFER_DONE_MASK               0x40  


#define I2C_ISR_RX_TRANSFER_DONE_INT                0x40  // bit 6                                                        
#define I2C_ISR_TX_UNDERRUN_INT                     0x04  // bit 2
#define I2C_ISR_RX_OVERRUN_INT                      0x02  // bit 1
#define I2C_ISR_TX_TRANSFER_DONE_INT                0x01  // bit 0

#define I2C_MASTER_CTRL_ADDR                        iic2_ctl_adr        
#define I2C_SLAVE_CTRL_ADDR                         iic2_slv_ctl_adr         
#define I2C_SLAVE_STATUS_ADDR                       iic2_slv_status_adr       
#define I2C_CTRL_TARGET_SLAVE_ADDR                  iic2_caddr_wctl_adr
#define I2C_CTRL_PTU2_SLAVE_ADDR                    iic2_saddr_wctl_adr   
#define I2C_MATER_SLAVE_ENABLE_ADDR                 iic2_rd_en_int_adr
#define I2C_BYTE_READ_WRITE_CNT_ADDR                iic2_byte_cnt_adr
#define I2C_SLAVE_TX_RX_FIFO_IO_ADDR                dp_iics_data_adr 
  
#define I2C_SLAVE_RX_FIFO_LEVEL_ADDR                dc_ptu_iics_rfl_adr
#define I2C_SLAVE_TX_FIFO_LEVEL_ADDR                dc_ptu_iics_tfl_adr    
#define I2C_SLAVE_RX_FIFO_USR_LEVEL_ADDR            dc_ptu_iics_rfc_adr

#define I2C_SLAVE_INT_RX_ENABLE_ADDR                sr_ptu_en_adr2
#define I2C_SLAVE_INT_RUN_ENABLE_ADDR               sr_ptu_en_adr1
#define I2C_SLAVE_INT_RX_STATUS_ADDR                sr_ptu_status_adr2                 
#define I2C_SLAVE_INT_RUN_STATUS_ADDR               sr_ptu_status_adr1      
#define I2C_SLAVE_TX_FIFO_ADDR                      dp_iics_data_adr
#define I2C_SLAVE_RX_FIFO_ADDR                      dp_iics_data_adr

#define I2C_SLAVE_INT_RUN_STATUS_CLR                {UINT32 u32Temp; u32Temp=REG32(sr_ptu_status_adr1);REG32(sr_ptu_status_adr1)=u32Temp;}
#define I2C_SLAVE_INT_FIFO_STATUS_CLR               {UINT32 u32Temp; u32Temp=REG32(sr_ptu_status_adr2);REG32(sr_ptu_status_adr2)=u32Temp;}

#define I2C_MASTER_CONTROL                          I2C_REG(I2C_MASTER_CTRL_ADDR)
#define I2C_SLAVE_CONTROL                           I2C_REG(I2C_SLAVE_CTRL_ADDR)
#define I2C_SLAVE_STATUS                            I2C_REG(I2C_SLAVE_STATUS_ADDR)
#define I2C_TARGET_SLAVE_ADDR                       I2C_REG(I2C_CTRL_TARGET_SLAVE_ADDR)
#define I2C_PTU2_ADDR(x)                            I2C_REG(I2C_CTRL_PTU2_SLAVE_ADDR)       = (x & 0x3FF)
#define I2C_MASTER_SLAVE_ENABLE(x)                  I2C_REG(I2C_MATER_SLAVE_ENABLE_ADDR)    = (x & 0x02)
#define I2C_MASTER_SLAVE_STATUS                     I2C_REG(I2C_MATER_SLAVE_ENABLE_ADDR)
#define I2C_READ_BYTE_CLEAR                         I2C_REG(I2C_BYTE_READ_WRITE_CNT_ADDR)   = 0
#define I2C_WRITE_BYTE_CLEAR                        I2C_REG(I2C_BYTE_READ_WRITE_CNT_ADDR)   = 0


#define I2C_SLAVE_7_BIT_ADDR(addr)                  ((addr)  & 0x07F)
#define I2C_SLAVE_10_BIT_ADDR(addr)                 (((addr) & 0x3FF) | (0x1000))
#define I2C_SLAVE_ADDR_A0                           I2C_SLAVE_7_BIT_ADDR(0x77)
#define I2C_SLAVE_ADDR_A1                           I2C_SLAVE_7_BIT_ADDR(0x76)
#define I2C_SLAVE_ADDR_A2                           I2C_SLAVE_7_BIT_ADDR(0x66)
#define I2C_SLAVE_ADDR_A3                           I2C_SLAVE_10_BIT_ADDR(0x1FA)
#define I2C_MATCH_INDEX_VALID                       0x10
#define I2C_MATCH_INDEX_MASK                        0x07
#define I2C_MATCH_INDEX_REGISTER                    REG32(iic2_match_index_adr)

#define I2C_ENABLE_MULTI_ADDR_DETECTION()     		(REG32(iic2_addr_ctl_adr) |= (0x01))
#define I2C_ENABLE_HW_ADDR_DETECTION()        		(REG32(iic2_addr_ctl_adr) |= (0x02))
#define I2C_ENABLE_FW_ADDR_DETECTION()          	(REG32(iic2_addr_ctl_adr) &= (~0x02))
#define I2C_SLAVE_SET_VALID_ADDR(x) 		        (REG32(iic2_addr_valid_adr) = (x))
#define I2C_SLAVE_CONFIGURE_ADDR(addr, val)	        (REG32(addr) = (val))

#define I2C_SLAVE_FIFO_ADDR(x)                      I2C_REG(I2C_SLAVE_TX_RX_FIFO_IO_ADDR)           = x
#define I2C_SLAVE_WATER_MARK_RX_LEVEL(x)            I2C_REG(I2C_SLAVE_RX_FIFO_LEVEL_ADDR)           = x
#define I2C_SLAVE_WATER_MARK_TX_LEVEL(x)            I2C_REG(I2C_SLAVE_TX_FIFO_LEVEL_ADDR)           = x
#define I2C_SLAVE_WATER_MARK_RX_USR_LEVEL(x)        I2C_REG(I2C_SLAVE_RX_FIFO_USR_LEVEL_ADDR)       = x
#define I2C_SLAVE_FIFO_INT_ENABLE                   I2C_REG(I2C_SLAVE_INT_RX_ENABLE_ADDR)   //sr_ptu_en_adr2
#define I2C_SLAVE_RUN_INT_ENABLE                    I2C_REG(I2C_SLAVE_INT_RUN_ENABLE_ADDR)  //sr_ptu_en_adr1
#define I2C_SLAVE_FIFO_INT_STATUS                   I2C_REG(I2C_SLAVE_INT_RX_STATUS_ADDR)   //sr_ptu_status_adr2
#define I2C_SLAVE_RUN_INT_STATUS                    I2C_REG(I2C_SLAVE_INT_RUN_STATUS_ADDR)  //sr_ptu_status_adr1

#define I2C_SLAVE_RX_FIFO                           I2C_REG(I2C_SLAVE_RX_FIFO_ADDR)
#define I2C_SLAVE_TX_FIFO(x)                        I2C_REG(I2C_SLAVE_TX_FIFO_ADDR) = x   // I2C_FIFO_REG
#define I2C_SLAVE_ISR_DSBL_RX_EMPTY()               (I2C_SLAVE_FIFO_INT_ENABLE &= (~I2C_ISR_RX_EMPTY_MASK) )
#define I2C_SLAVE_ISR_DSBL_RX_AFULL()               (I2C_SLAVE_FIFO_INT_ENABLE &= (~I2C_ISR_RX_AFULL_MASK) )
#define I2C_SLAVE_ISR_DSBL_TX_AEMPTY()              (I2C_SLAVE_FIFO_INT_ENABLE &= (~I2C_ISR_TX_AEMPTY_MASK) )
#define I2C_SLAVE_ISR_DSBL_TX_FULL()                (I2C_SLAVE_FIFO_INT_ENABLE &= (~I2C_ISR_TX_FULL_MASK) )
#define I2C_SLAVE_FIFO_INT_CLEAR(x)                 (I2C_SLAVE_INT_FIFO_STATUS  = (x) )
#define I2C_SLAVE_RUN_INT_CLEAR(x)                  (I2C_SLAVE_INT_RUN_STATUS   = (x) )
#define I2C_SLAVE_READ_RX_LEN()                     (((REG32(dp_iic_fstat_adr))& 0x3FF00000)>>20)

#define I2C_SLAVE_RX_BYTECOUNT_DEFAULT              I2C_REG(iics_rx_bytecnt_default_adr)
#define I2C_SLAVE_TX_BYTECOUNT_DEFAULT              I2C_REG(iics_tx_bytecnt_default_adr)
#define I2C_SLAVE_RX_LAST_BYTENT                    I2C_REG(iics_rx_last_bytecnt_adr)
#define I2C_SLAVE_TX_LAST_BYTENT                    I2C_REG(iics_tx_last_bytecnt_adr)

// i2c slave control mask
#define I2C_SLAVE_RX_FIFO_FLUSH_MASK                0x2000
#define I2C_SLAVE_TX_FIFO_FLUSH_MASK                0x1000
// i2c slave control bits
#define I2C_SLAVE_CNTRL_BYPASS_GLITCH_FILTER        (1<<0) // bit 0
#define I2C_SLAVE_CNTRL_ENABLE_GLITCH_FILTER        (0<<0) // bit 0
#define I2C_SLAVE_CNTRL_ADDRESSING_MODE_10          (1<<1) // bit 1
#define I2C_SLAVE_CNTRL_ADDRESSING_MODE_7           (0<<1) // bit 1
#define I2C_SLAVE_CNTRL_MEIF_MODE                   (1<<2) // bit 2
#define I2C_SLAVE_CNTRL_NORMAL_MODE                 (0<<2) // bit 2


#define I2C_HS_ENABLE_MASK                          (1<<4)
#define I2C_HS_MASTER_CODE                          0x08

// default i2c slave pads!
#define LHL_GPIO_P19_AS_SCL2            19
#define LHL_GPIO_P20_AS_SDA2            20

typedef void I2C_SLAVE_RX_CALLBACK_HANDLER(UINT8* rx_PktPointer, UINT16 rx_PktLength);

void i2c_slave_init(void);
void i2c_slave_configure_pads(void);
void i2c_slave_configure_PTU(void);
void i2c_slave_init_FW(void);
void i2c_slave_init_HW(void);
void i2c_slave_configure_device_addr(void);
void i2c_slave_register_rx_cb(I2C_SLAVE_RX_CALLBACK_HANDLER* cb);
void i2c_slave_HandleINTERRUPT_PTU(void);
void i2c_slave_process_event(UINT32 evtFlag);
void i2c_slave_handleRxEvent(void);
void i2c_slave_handleTxEvent(void);
void i2c_slave_synchronous_read( UINT8* buffer, UINT32 length );
void i2c_slave_synchronous_write( UINT8* buffer, UINT32 length );


#ifdef __cplusplus
}
#endif


/* @} */


#endif
