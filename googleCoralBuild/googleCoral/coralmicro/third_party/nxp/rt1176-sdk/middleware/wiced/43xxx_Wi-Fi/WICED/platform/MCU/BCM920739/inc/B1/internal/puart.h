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
*    File Name: puart.h
*
*    Abstract: This file defines a peripheral uart driver
*
*
********************************************************************
*/
#ifndef __PUART_DRIVER_H__
#define __PUART_DRIVER_H__

#include "brcm_fw_types.h"

extern void (*puart_rxCb)(void *);
extern void (*puart_rxIntCb)(void);
extern void (*puart_txCb)(void);


#define UART_REG            REG32
#define ROUND(x,y)          ((x+y/2)/y);
#define PUART_STRLEN(a)     strlen( (a) )


// "P_UartConfig" MUST be moved to "hidconfig.h"
// Also add an ITEM in the "config_item_tbl_hidd.h" for this config.
typedef struct 
{
    UINT32  baudrate;
    UINT16  hwFlowControlWaterMark;
    UINT32  minPktlength;
    UINT8   pUartFunction;
} puart_UartConfig;

/**  \addtogroup PeripheralUart
 *  \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines a Peripheral UART driver class. 
*/
    /*****************************************************************************/
    /*                      UART PHYSICAL MACROS                                 */
    /*****************************************************************************/
    #define P_UART_INT_STATUS_ADDR                          uart_hc_int_status_adr  
    #define P_UART_INT_ENABLE_ADDR                          uart_hc_int_enable_adr  
    #define P_UART_RX_FIFO_ADDR                             uart_hc_data_adr        
    #define P_UART_TX_FIFO_ADDR                             uart_hc_data_adr        
    #define P_UART_WATER_MARK_RX_ADDR                       dc_ptu_uart2_rfl_adr    
    #define P_UART_WATER_MARK_TX_ADDR                       dc_ptu_uart2_tfl_adr    
    #define P_UART_FLOW_CONTROL_WATER_MARK_ADDR             dc_ptu_uart2_rfc_adr    
    #define P_UART_LINE_CONTROL_ADDR                        dc_ptu_uart2_lcr_adr    
    #define P_UART_LINE_STATUS_ADDR                         dc_ptu_uart2_lsr_adr    
    #define P_UART_MODEM_CONTROL_ADDR                       dc_ptu_uart2_mcr_adr    
    #define P_UART_HIGH_BAUDRATE_DIV_ADDR                   dc_ptu_uart2_dhbr_adr   
    #define P_UART_LOW_BAUDRATE_DIV_ADDR                    dc_ptu_uart2_dlbr_adr   
    #define P_UART_FIFO_CONTROL_ADDR                        dc_ptu_uart2_fcr_adr    
    #define P_UART_MSR_LINE_ADDR                            dc_ptu_uart2_msr_adr    
    #define P_UART_PTU_HC_SEL_ADDR                          dc_ptu_hc_sel_adr       
    #define P_UART_PTU_AUX_INT_ENABLE                       REG32(sr_ptu_aux_en_adr0)
    #define P_UART_PTU_AUX_INT_STATUS                       REG32(sr_ptu_aux_status_adr0)
    /*****************************************************************************/
    /*                      UART ISR Register BITS                       */
    /*****************************************************************************/
enum
{
    P_UART_ISR_TX_FF_MASK                          = 0x01,
    P_UART_ISR_TX_FAE_MASK                         = 0x02,
    P_UART_ISR_RX_AFF_MASK                         = 0x04,
    P_UART_ISR_RX_FE_MASK                          = 0x08,
    P_UART_ISR_RX_RES_MASK                         = 0x10,
    P_UART_ISR_RX_PE_MASK                          = 0x20,
    P_UART_ISR_RX_BRK_MASK                         = 0x40,
    P_UART_ISR_RX_CTS_MASK                         = 0x80
};

/*****************************************************************************/
/*                      UART LINE CONTROL REGISTER BITS                      */
/*****************************************************************************/
enum
{
    P_UART_LCR_STB_MASK                            = 0x0001,
    P_UART_LCR_PEN_MASK                            = 0x0002,
    P_UART_LCR_EPS_MASK                            = 0x0004,
    P_UART_LCR_RXEN_MASK                           = 0x0008,
    P_UART_LCR_LBC_MASK                            = 0x0010,
    P_UART_LCR_TXOEN_MASK                          = 0x0020,
    P_UART_LCR_RTSOEN_MASK                         = 0x0040,
    P_UART_LCR_SLIP_MASK                           = 0x0080,
    P_UART_LCR_SLIP_CRC_RX_ENABLE_MASK             = 0x0100,
    P_UART_LCR_SLIP_CRC_INVERT_MASK                = 0x0200,
    P_UART_LCR_SLIP_CRC_LSB_1ST_MASK               = 0x0400,
    P_UART_LCR_SLIP_CRC_TX_ENABLE_MASK             = 0x0800
};

/*****************************************************************************/
/*                      UART LINE STATUS REGISTER BITS                       */
/*****************************************************************************/
enum
{
    P_UART_LSR_TX_FIFO_NOT_EMPTY                   = 0x0004, //'0':Data NOT available, '1':Data available
    P_UART_LSR_TX_IDLE                             = 0x0008,
    P_UART_LSR_END_DELIMETER_NOT_DONE_MASK         = 0x0010
};


/*****************************************************************************/
/*                      UART MODEM CONTROL REGISTER BITS                     */
/*****************************************************************************/
enum
{
    P_UART_MCR_TXEN_MASK                           = 0x0001,
    P_UART_MCR_RTS_MASK                            = 0x0002,
    P_UART_MCR_XON_XOFF_MASK                       = 0x0004,
    P_UART_MCR_HIGH_RATE_MASK                      = 0x0008,
    P_UART_MCR_LOOPBACK_MASK                       = 0x0010,
    P_UART_MCR_AUTORTS_MASK                        = 0x0020,
    P_UART_MCR_AUTOCTS_MASK                        = 0x0040,
    P_UART_MCR_BAUD_ADJ_MASK                       = 0x0080,
    P_UART_MCR_PKT_FLOW_RX_MASK                    = 0x0100,
    P_UART_MCR_SEND_XON_NOW                        = 0x0200
};

/*****************************************************************************/
/*                      UART MODEM STATUS REGISTER BITS                      */
/*****************************************************************************/
enum
{
    P_UART_MSR_CTS_MASK							   = 0x0001,
    P_UART_MSR_RX_MASK							   = 0x0004
};

/*****************************************************************************/
/*                      UART FIFO CONTROL REGISTER BITS                      */
/*****************************************************************************/
enum
{
    P_UART_FCR_RX_FIFO_RESET_MASK                  = 0x001,
    P_UART_FCR_TX_FIFO_RESET_MASK                  = 0x002,
    P_UART_FCR_SLIP_RX_RESYNC_MASK                 = 0x004,
    P_UART_FCR_SLIP_START_END_FRAME_MASK           = 0x008
};


#if defined (BCM20703) || defined (BCM20739)
/*****************************************************************************/
/*                      PTU Auxilliary Interrupt bits                        */
/*****************************************************************************/
enum
{
    P_UART_PTU_AUX_INT_ENABLE_MASK                 = 0x40,
    P_UART_PTU_AUX_INT_STATUS_MASK                 = 0x40,
};
#endif

/*****************************************************************************/
/*                      UART ESC REGISTER BITS                          */
/*****************************************************************************/
#define P_UART_ESC_ADDR                                 dc_ptu_uart2_esc_adr

/*****************************************************************************/
/*                      UART RECEIVER MACROS                                 */
/*****************************************************************************/
#define P_UART_IN_BUFFER_LEN                            256
#define P_UART_RX_FIFO_SIZE                             P_UART_IN_BUFFER_LEN


/*****************************************************************************/
/*                      UART TRANSMITTER MACROS                              */
/*****************************************************************************/
// Ethan's Note for USB and UART share the same FIFO case:
// Although the Tx FIFO is X Bytes and we set the Tx watermark to 1, we have found that we can not write
// X or even X - 1  bytes after getting the FAE interrupt as we will overwrite the data that's already in the FIFO
// at slow baudrates such as 115200. At 230400 or faster, this problem does not happen. In my testing I found X - 3
// or less is working but just to be safe, only write X -4 (see uart_bridge.h for original comment).
#define P_UART_OUT_BUFFER_LEN                           256
#define P_UART_TX_FIFO_SIZE                             P_UART_OUT_BUFFER_LEN

#define P_UART_WATER_MARK_RX_LEVEL_HIGHEST              (P_UART_RX_FIFO_SIZE - 1)
#define P_UART_WATER_MARK_TX_LEVEL_HIGHEST              (P_UART_TX_FIFO_SIZE - 1)
#define P_UART_WATER_MARK_RX_LEVEL_ONE_BYTE             1


/*****************************************************************************/
/*                             UART MACROS                                   */
/*****************************************************************************/
#define P_UART_INT_ENABLE                           UART_REG(P_UART_INT_ENABLE_ADDR)
#define P_UART_INT_STATUS                           UART_REG(P_UART_INT_STATUS_ADDR)
#define P_UART_INT_CLEAR(x)                         UART_REG(P_UART_INT_STATUS_ADDR)              = x
#define P_UART_INT_DISABLE                          UART_REG(P_UART_INT_DISABLE_ADDR)

#define P_UART_RX_FIFO()                            UART_REG(P_UART_RX_FIFO_ADDR)
#define P_UART_TX_FIFO(x)                           UART_REG(P_UART_TX_FIFO_ADDR)                 = x   // P_UART_FIFO_REG

#define P_UART_LINE_CONTROL(x)                      UART_REG(P_UART_LINE_CONTROL_ADDR)            = x
#define P_UART_LINE_CONTROL_GET()                   UART_REG(P_UART_LINE_CONTROL_ADDR)
#define P_UART_MODEM_CONTROL(x)                     UART_REG(P_UART_MODEM_CONTROL_ADDR)           = x
#define P_UART_MODEM_CONTROL_GET()                  UART_REG(P_UART_MODEM_CONTROL_ADDR)
#define P_UART_FIFO_CONTROL(x)                      UART_REG(P_UART_FIFO_CONTROL_ADDR)            = x

#define P_UART_MSR_STATUS()                         UART_REG(P_UART_MSR_LINE_ADDR)

#define P_UART_HIGH_BAUDRATE_DIV(x)                 UART_REG(P_UART_HIGH_BAUDRATE_DIV_ADDR)       = x
#define P_UART_LOW_BAUDRATE_DIV(x)                  UART_REG(P_UART_LOW_BAUDRATE_DIV_ADDR)        = x
#define P_UART_WATER_MARK_RX_LEVEL(x)               UART_REG(P_UART_WATER_MARK_RX_ADDR)           = x
#define P_UART_WATER_MARK_TX_LEVEL(x)               UART_REG(P_UART_WATER_MARK_TX_ADDR)           = x
#define P_UART_FLOW_CONTROL_WATER_MARK_LEVEL(x)     UART_REG(P_UART_FLOW_CONTROL_WATER_MARK_ADDR) = x

#define P_UART_ESC_CHAR                             UART_REG(P_UART_ESC_ADDR)


enum
{
    P_UART_CLK                                 = 24000000,
    P_UART_SAMPLE_CLOCK                        = 16,
    DEFAULT_P_UART_BAUDRATE                    = 115200,
    DEFAULT_P_UART_HWFLOWCONTROLWATERMARK      = 13 ,     // Peer flow off waterline
    DEFAULT_P_UART_MIN_PKT_LENGTH              = 1
};

     
typedef enum P_UART_RX_STATE_TAG
{    
    P_UART_RX_IDLE,
    P_UART_RX_TYPE,
    P_UART_RX_PAYLOAD,
    P_UART_RX_READ_DONE,
    P_UART_RX_RESYNC,
    P_UART_DROP_PACKET,
} P_UART_RX_STATE;

typedef enum P_UART_TX_STATE_TAG
{
    P_UART_TX_IDLE,             // TX State M/C is idle
    P_UART_TX_BUSY,             // TX is busy
    P_UART_TX_WAIT_FIFO_EMPTY,
    P_UART_TX_DONE,
    P_UART_TX_WRONG_STATE       
} P_UART_TX_STATE;

typedef struct HCI_CMD_TAG
{
    UINT8   opcode;
    UINT16  length;
} P_UART_HCI_CMD;

typedef enum P_UART_FUNCTION_TAG
{
    P_UART_HCI,             
    P_UART_HID,             
    P_UART_SENSOR,
} P_UART_FUNCTION;

typedef UINT8 P_UART_TRANSPORT_PACKET_TYPE; 

//UART2 transport packet type defines
typedef struct P_UART_TRANSPORT_PACKET_TAG
{
    P_UART_RX_STATE                     RxState;
    P_UART_TX_STATE                     TxState;
    P_UART_FUNCTION                     peripheralUartFunction;

    // Rx Packet Types ******************************/
    P_UART_TRANSPORT_PACKET_TYPE        rxPktType;
    UINT8*                              rxPktPtr;
    UINT32                              rxPktLen;
    BOOL8                               rxPktSLIPHasCRC;
    UINT8                               rxPktSLIPSeqNo;
    // Tx Packet Types******************************/
    P_UART_TRANSPORT_PACKET_TYPE        txPktType;
    UINT8*                              txPktPtr;
    UINT32                              txPktLen;
    UINT8*                              txACLSegRootPtr;
    UINT8                               txPktSLIPSeqNo;
    // Resync types*********************************/
    UINT8                               resyncCurrentByte;
    UINT8                               resyncCurrentStream;
    BOOL8                               resynchSentHWErrorEvent;

} P_UART_TRANSPORT_PACKET;

typedef struct 
{
    P_UART_TRANSPORT_PACKET    p_uart;
} tPuartDrvState;
    

#ifdef __cplusplus
extern "C" {
#endif

void puart_interruptHandler(void);

void puart_init(void);
void puart_interruptHandler(void);
// Handle HCI tx and Rx
int puart_hciRxHandler(void* unused);
int puart_hciTxHandler(void* unused);
// Handle HID tx amd Rx
int puart_hidRxHandler(void* unused);
int puart_hidTxHandler(void* unused);
// Handle BLE rx
int puart_bleRxHandler(void *unused);
void puart_calculateBaudrate(UINT8* dhbr, UINT8* dlbr, UINT32 baudrate, UINT32 clk);

void puart_enableInterrupt(void);
void puart_flowOn(void);
void puart_flowOff(void);
void puart_disableTx(void);
void puart_enableTx(void);
void puart_setBaudrate( UINT8 dhbr, UINT8 dlbr, UINT32 baudrate );
void puart_synchronousRead( UINT8* buffer, UINT32 length );
void puart_synchronousWrite( UINT8* buffer, UINT32 length );
BOOL32 puart_rxFifoNotEmpty(void);

int puart_rxHandler(void* unused);
int puart_txHandler(void* unused);

void puart_print(char * string);
void puart_write(UINT8 byte);
BOOL32 puart_read(UINT8* readbyte);

BOOL32 puart_selectUartPads(UINT8 rxdPortPin, UINT8 txdPortPin, UINT8 ctsPortPin, UINT8 rtsPortPin);    
BOOL32 puart_checkRxdPortPin(UINT8 rxdPortPin);
BOOL32 puart_checkTxdPortPin(UINT8 txdPortPin);
BOOL32 puart_checkCtsPortPin(UINT8 ctsPortPin);
BOOL32 puart_checkRtsPortPin(UINT8 rtsPortPin);

#define P_UART_RX_FIFO_NOT_EMPTY()      puart_rxFifoNotEmpty()
#define P_UART_TX_FIFO_IS_EMPTY()    ( ( REG32(P_UART_LINE_STATUS_ADDR) & ( P_UART_LSR_TX_FIFO_NOT_EMPTY | P_UART_LSR_TX_IDLE ) ) == P_UART_LSR_TX_IDLE )

#ifdef __cplusplus
}
#endif

/// @}
#endif



