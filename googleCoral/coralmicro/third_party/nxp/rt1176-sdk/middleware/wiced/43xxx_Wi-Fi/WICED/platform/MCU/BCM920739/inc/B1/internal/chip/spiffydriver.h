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
*    File Name: spiffydriver.h
*
*    Abstract: This file defines the spiffy driver
*
*
********************************************************************
*/

#ifndef __SPIFFYD_DRIVER_H__
#define __SPIFFYD_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "brcm_fw_types.h"
#include "foundation/regmaps/regmaps.h"
#include "spiffy.h"
#include "spiffy_priv.h"
#ifdef __cplusplus
}
#endif

#include "hidddriversconfig.h"


/**  \addtogroup SpiffyDriver
* \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines a spiffy driver class. The BCM keyboard application or
* other applications use this driver to obtain the status from 
* and control the behavior of the spiffy driver. This driver
* only offers services for clock control, mode control and data 
* transfer operations. The application is responsible for 
* generating the slave/chip select signals. This could be done 
* by mapping a GPIO pin for each slave the application wants to 
* control. All data transfer operations (half/full duplex) 
* operations provided by this driver assume that the desired 
* slave has already been selected and will remain selected 
* throughout the duration of the transaction. 
*/

/// Number of spiffy blocks.
typedef enum SpiffyInstance
{
    SPIFFYD_1  = SPIFFY_BLOCK_1,
    SPIFFYD_2  = SPIFFY_BLOCK_2, 
    MAX_SPIFFYS = 2
}SpiffyInstance;


// this should be defined in spiffy_priv.h, but that could take a while
#define SPIFFY_TX_FIFO_LEN      256
#define SPIFFY_RX_FIFO_LEN      256


///
///  Available combination for SPI 1 support
///

// Coding format:
// 
// Master Mode : Always begin with MASTER1
// 
// MASTER1_P[cc]_CLK_P[oo]_MOSI_P[ii]_MISO
// 
// where 
//   cc : Clock port number
//  oo : MOSI port number
//  ii : MISO port number
// 
// 
// Slave Mode : Always begin with S1
// 
// SLAVE1_P[ss]_CS_P[cc]_CLK_P[oo]_MOSI_P[ii]_MISO
// 
// where 
//  ss : CS port number
//  cc : Clock port number
//  oo : MOSI port number
//  ii : MISO port number
// 
//

///
/// All the available GPIO combinations for SPI-1 Master mode.
///
typedef enum Spi1MasterGpioConfigType
{

    //Generate SPIFFY-1 available combinations
    //SPIFFY-1 Master Mode
    MASTER1_P03_CLK_P00_MOSI_P01_MISO = 0x00030001,
    MASTER1_P03_CLK_P00_MOSI_P05_MISO = 0x00030005,
    MASTER1_P03_CLK_P02_MOSI_P01_MISO = 0x00030201,
    MASTER1_P03_CLK_P02_MOSI_P05_MISO = 0x00030205,
    MASTER1_P03_CLK_P04_MOSI_P01_MISO = 0x00030401,
    MASTER1_P03_CLK_P04_MOSI_P05_MISO = 0x00030405,
    MASTER1_P03_CLK_P27_MOSI_P01_MISO = 0x00031b01,
    MASTER1_P03_CLK_P27_MOSI_P05_MISO = 0x00031b05,
    MASTER1_P03_CLK_P38_MOSI_P01_MISO = 0x00032601,
    MASTER1_P03_CLK_P38_MOSI_P05_MISO = 0x00032605,
    MASTER1_P07_CLK_P00_MOSI_P01_MISO = 0x00070001,
    MASTER1_P07_CLK_P00_MOSI_P05_MISO = 0x00070005,
    MASTER1_P07_CLK_P02_MOSI_P01_MISO = 0x00070201,
    MASTER1_P07_CLK_P02_MOSI_P05_MISO = 0x00070205,
    MASTER1_P07_CLK_P04_MOSI_P01_MISO = 0x00070401,
    MASTER1_P07_CLK_P04_MOSI_P05_MISO = 0x00070405,
    MASTER1_P07_CLK_P27_MOSI_P01_MISO = 0x00071b01,
    MASTER1_P07_CLK_P27_MOSI_P05_MISO = 0x00071b05,
    MASTER1_P07_CLK_P38_MOSI_P01_MISO = 0x00072601,
    MASTER1_P07_CLK_P38_MOSI_P05_MISO = 0x00072605,
    MASTER1_P24_CLK_P00_MOSI_P25_MISO = 0x00180019,
    MASTER1_P24_CLK_P02_MOSI_P25_MISO = 0x00180219,
    MASTER1_P24_CLK_P04_MOSI_P25_MISO = 0x00180419,
    MASTER1_P24_CLK_P27_MOSI_P25_MISO = 0x00181b19,
    MASTER1_P24_CLK_P38_MOSI_P25_MISO = 0x00182619,
    MASTER1_P36_CLK_P00_MOSI_P25_MISO = 0x00240019,
    MASTER1_P36_CLK_P02_MOSI_P25_MISO = 0x00240219,
    MASTER1_P36_CLK_P04_MOSI_P25_MISO = 0x00240419,
    MASTER1_P36_CLK_P27_MOSI_P25_MISO = 0x00241b19,
    MASTER1_P36_CLK_P38_MOSI_P25_MISO = 0x00242619,
    //Total SPIFFYD_1 Master Available combinations=30
} Spi1MasterGpioConfigType;


///
/// All the available GPIO combinations for SPI-1 SLAVE mode.
///
typedef enum Spi2SlaveGpioConfigType
{
    //SPIFFY-2 Slave available combinations
    SLAVE1_P02_CS_P03_CLK_P00_MOSI_P01_MISO = 0x02030001,
    SLAVE1_P02_CS_P03_CLK_P00_MOSI_P05_MISO = 0x02030005,
    SLAVE1_P02_CS_P03_CLK_P00_MOSI_P25_MISO = 0x02030019,
    SLAVE1_P02_CS_P03_CLK_P04_MOSI_P01_MISO = 0x02030401,
    SLAVE1_P02_CS_P03_CLK_P04_MOSI_P05_MISO = 0x02030405,
    SLAVE1_P02_CS_P03_CLK_P04_MOSI_P25_MISO = 0x02030419,
    SLAVE1_P02_CS_P07_CLK_P00_MOSI_P01_MISO = 0x02070001,
    SLAVE1_P02_CS_P07_CLK_P00_MOSI_P05_MISO = 0x02070005,
    SLAVE1_P02_CS_P07_CLK_P00_MOSI_P25_MISO = 0x02070019,
    SLAVE1_P02_CS_P07_CLK_P04_MOSI_P01_MISO = 0x02070401,
    SLAVE1_P02_CS_P07_CLK_P04_MOSI_P05_MISO = 0x02070405,
    SLAVE1_P02_CS_P07_CLK_P04_MOSI_P25_MISO = 0x02070419,
    SLAVE1_P06_CS_P03_CLK_P00_MOSI_P01_MISO = 0x06030001,
    SLAVE1_P06_CS_P03_CLK_P00_MOSI_P05_MISO = 0x06030005,
    SLAVE1_P06_CS_P03_CLK_P00_MOSI_P25_MISO = 0x06030019,
    SLAVE1_P06_CS_P03_CLK_P04_MOSI_P01_MISO = 0x06030401,
    SLAVE1_P06_CS_P03_CLK_P04_MOSI_P05_MISO = 0x06030405,
    SLAVE1_P06_CS_P03_CLK_P04_MOSI_P25_MISO = 0x06030419,
    SLAVE1_P06_CS_P07_CLK_P00_MOSI_P01_MISO = 0x06070001,
    SLAVE1_P06_CS_P07_CLK_P00_MOSI_P05_MISO = 0x06070005,
    SLAVE1_P06_CS_P07_CLK_P00_MOSI_P25_MISO = 0x06070019,
    SLAVE1_P06_CS_P07_CLK_P04_MOSI_P01_MISO = 0x06070401,
    SLAVE1_P06_CS_P07_CLK_P04_MOSI_P05_MISO = 0x06070405,
    SLAVE1_P06_CS_P07_CLK_P04_MOSI_P25_MISO = 0x06070419,
    SLAVE1_P26_CS_P24_CLK_P27_MOSI_P01_MISO = 0x1a181b01,
    SLAVE1_P26_CS_P24_CLK_P27_MOSI_P05_MISO = 0x1a181b05,
    SLAVE1_P26_CS_P24_CLK_P27_MOSI_P25_MISO = 0x1a181b19,
    SLAVE1_P26_CS_P24_CLK_P33_MOSI_P01_MISO = 0x1a182101,
    SLAVE1_P26_CS_P24_CLK_P33_MOSI_P05_MISO = 0x1a182105,
    SLAVE1_P26_CS_P24_CLK_P33_MOSI_P25_MISO = 0x1a182119,
    SLAVE1_P26_CS_P24_CLK_P38_MOSI_P01_MISO = 0x1a182601,
    SLAVE1_P26_CS_P24_CLK_P38_MOSI_P05_MISO = 0x1a182605,
    SLAVE1_P26_CS_P24_CLK_P38_MOSI_P25_MISO = 0x1a182619,
    SLAVE1_P26_CS_P36_CLK_P27_MOSI_P01_MISO = 0x1a241b01,
    SLAVE1_P26_CS_P36_CLK_P27_MOSI_P05_MISO = 0x1a241b05,
    SLAVE1_P26_CS_P36_CLK_P27_MOSI_P25_MISO = 0x1a241b19,
    SLAVE1_P26_CS_P36_CLK_P33_MOSI_P01_MISO = 0x1a242101,
    SLAVE1_P26_CS_P36_CLK_P33_MOSI_P05_MISO = 0x1a242105,
    SLAVE1_P26_CS_P36_CLK_P33_MOSI_P25_MISO = 0x1a242119,
    SLAVE1_P26_CS_P36_CLK_P38_MOSI_P01_MISO = 0x1a242601,
    SLAVE1_P26_CS_P36_CLK_P38_MOSI_P05_MISO = 0x1a242605,
    SLAVE1_P26_CS_P36_CLK_P38_MOSI_P25_MISO = 0x1a242619,
    SLAVE1_P32_CS_P24_CLK_P27_MOSI_P01_MISO = 0x20181b01,
    SLAVE1_P32_CS_P24_CLK_P27_MOSI_P05_MISO = 0x20181b05,
    SLAVE1_P32_CS_P24_CLK_P27_MOSI_P25_MISO = 0x20181b19,
    SLAVE1_P32_CS_P24_CLK_P33_MOSI_P01_MISO = 0x20182101,
    SLAVE1_P32_CS_P24_CLK_P33_MOSI_P05_MISO = 0x20182105,
    SLAVE1_P32_CS_P24_CLK_P33_MOSI_P25_MISO = 0x20182119,
    SLAVE1_P32_CS_P24_CLK_P38_MOSI_P01_MISO = 0x20182601,
    SLAVE1_P32_CS_P24_CLK_P38_MOSI_P05_MISO = 0x20182605,
    SLAVE1_P32_CS_P24_CLK_P38_MOSI_P25_MISO = 0x20182619,
    SLAVE1_P32_CS_P36_CLK_P27_MOSI_P01_MISO = 0x20241b01,
    SLAVE1_P32_CS_P36_CLK_P27_MOSI_P05_MISO = 0x20241b05,
    SLAVE1_P32_CS_P36_CLK_P27_MOSI_P25_MISO = 0x20241b19,
    SLAVE1_P32_CS_P36_CLK_P33_MOSI_P01_MISO = 0x20242101,
    SLAVE1_P32_CS_P36_CLK_P33_MOSI_P05_MISO = 0x20242105,
    SLAVE1_P32_CS_P36_CLK_P33_MOSI_P25_MISO = 0x20242119,
    SLAVE1_P32_CS_P36_CLK_P38_MOSI_P01_MISO = 0x20242601,
    SLAVE1_P32_CS_P36_CLK_P38_MOSI_P05_MISO = 0x20242605,
    SLAVE1_P32_CS_P36_CLK_P38_MOSI_P25_MISO = 0x20242619,
    SLAVE1_P39_CS_P24_CLK_P27_MOSI_P01_MISO = 0x27181b01,
    SLAVE1_P39_CS_P24_CLK_P27_MOSI_P05_MISO = 0x27181b05,
    SLAVE1_P39_CS_P24_CLK_P27_MOSI_P25_MISO = 0x27181b19,
    SLAVE1_P39_CS_P24_CLK_P33_MOSI_P01_MISO = 0x27182101,
    SLAVE1_P39_CS_P24_CLK_P33_MOSI_P05_MISO = 0x27182105,
    SLAVE1_P39_CS_P24_CLK_P33_MOSI_P25_MISO = 0x27182119,
    SLAVE1_P39_CS_P24_CLK_P38_MOSI_P01_MISO = 0x27182601,
    SLAVE1_P39_CS_P24_CLK_P38_MOSI_P05_MISO = 0x27182605,
    SLAVE1_P39_CS_P24_CLK_P38_MOSI_P25_MISO = 0x27182619,
    SLAVE1_P39_CS_P36_CLK_P27_MOSI_P01_MISO = 0x27241b01,
    SLAVE1_P39_CS_P36_CLK_P27_MOSI_P05_MISO = 0x27241b05,
    SLAVE1_P39_CS_P36_CLK_P27_MOSI_P25_MISO = 0x27241b19,
    SLAVE1_P39_CS_P36_CLK_P33_MOSI_P01_MISO = 0x27242101,
    SLAVE1_P39_CS_P36_CLK_P33_MOSI_P05_MISO = 0x27242105,
    SLAVE1_P39_CS_P36_CLK_P33_MOSI_P25_MISO = 0x27242119,
    SLAVE1_P39_CS_P36_CLK_P38_MOSI_P01_MISO = 0x27242601,
    SLAVE1_P39_CS_P36_CLK_P38_MOSI_P05_MISO = 0x27242605,
    SLAVE1_P39_CS_P36_CLK_P38_MOSI_P25_MISO = 0x27242619,
    //Total SPIFFYD_1 Slave Available combinations=78
} Spi1SlaveGpioConfigType;

enum
{
    MASTER1_CONFIG = 1,
    SLAVE1_CONFIG  = 2,
};

/// Type Definitions
typedef enum SPI_ENDIAN
{
    /// Transmit most significant bit first
    SPI_MSB_FIRST,
    /// Transmit least significant bit first
    SPI_LSB_FIRST
} SPI_ENDIAN;

/// Slave Select mode (output from master)
typedef enum SPI_SS_MODE
{
    /// Slave select normal
    SPI_SS_NORMAL,
    /// Slave select goes inactive between bytes
    SPI_SS_INACTIVE_BTW_BYTES
}SPI_SS_MODE;

/// Slave select polarity (output from master)
typedef enum SPI_SS_POLARITY
{
    /// Slave select active low
    SPI_SS_ACTIVE_LOW,
    /// Slave select active high
    SPI_SS_ACTIVE_HIGH,
}SPI_SS_POLARITY;

/// Clock polarity and phase
/// If CPOL=0, base value of the clock is zero
/// If CPOL=1, base value of the clock is one
/// If CPHA=0, sample on leading (first) clock edge
/// If CPHA=1, sample on trailing (second) clock edge 
typedef enum SPI_MODE
{
    /// CPOL = 0, CPHA = 0 
    /// Data read on clock's rising edge, data changed on a falling edge
    SPI_MODE_0,
    /// CPOL = 0, CPHA = 1 
    /// Data read on clock's falling edge, data changed on a rising edge
    SPI_MODE_1,
    /// CPOL = 1, CPHA = 0 
    /// Data read on clock's falling edge, data changed on a rising edge
    SPI_MODE_2,
    /// CPOL = 1, CPHA = 1 
    /// Data read on clock's rising edge, data changed on a falling edge
    SPI_MODE_3
}SPI_MODE;

// pull configure for input-pin
// - Master input pin
//      -- MISO
//  
// - Slave Input pin
//      -- CLOCK
//      -- MOSI
//      -- CS
enum
{
    INPUT_PIN_PULL_UP   = 0x0400, // pull up for MISO if master mode,for MOSI if Slave mode
    INPUT_PIN_PULL_DOWN = 0x0200, // pull DOWN for MISO if master mode,for MOSI if Slave mode
    INPUT_PIN_FLOATING  = 0x0,    // FLOAT for MISO if master mode,for MOSI if Slave mode
};

// 
// bit[31:24] o fSPIFFYD_PORT_COMBINATION_CONFIG
// will be use as spiffy-1master/spiffy-1slave/spiffy-2master
// mode selection
// 
//
enum
{
    SPIFFYD_CS_SHIFT     = 24,
    SPIFFYD_CLK_SHIFT    = 16,
    SPIFFYD_MOSI_SHIFT   = 8,
    SPIFFYD_MISO_SHIFT   = 0,
    SPIFFYD_PIN_MASK     = 0xff,
};

// internal use for iocfg_premux MASK
enum
{
    SPIFFYD_BIT_11      =            (1<<11),
    SPIFFYD_BIT_10      =            (1<<10),
    SPIFFYD_BIT_8       =            (1<<8),
    SPIFFYD_BIT_7       =            (1<<7),
    SPIFFYD_BIT_6       =            (1<<6),
    SPIFFYD_BIT_5       =            (1<<5),
    SPIFFYD_BIT_4       =            (1<<4),
    SPIFFYD_BIT_3       =            (1<<3),
    SPIFFYD_BIT_2       =            (1<<2),
    SPIFFYD_BIT_0       =            (1<<0),

    MASTER_1_CLOCK_IOPREMUX_MASK    =  ((SPIFFYD_BIT_8) | (SPIFFYD_BIT_11)),
    MASTER_1_MISO_IOPREMUX_MASK     =   SPIFFYD_BIT_10,
    SLAVE_1_CLOCK_IOPREMUX_MASK     =  ((SPIFFYD_BIT_8) | (SPIFFYD_BIT_11)),

    SLAVE_1_MOSI_IOPREMUX_MASK      =   (SPIFFYD_BIT_4 | SPIFFYD_BIT_5),
    SLAVE_1_CS_IOPREMUX_MASK        =   (SPIFFYD_BIT_3 | SPIFFYD_BIT_2),

    // iocfg_premux[10]	Spiffy1 master MISO input routing		
    //	MISO from P5 when iocfg_premux[11]=1 and iocfg_premux[10]=1		
    //	MISO from P1 when iocfg_premux[11]=1 and iocfg_premux[10]=0		
    //    else MISO from P25.
    IOCFG_PREMUX_MISO_ROUTING_P5        =  SPIFFYD_BIT_10,

    //
    //iocfg_premux[3:2]	Spiffy1 slave Chip Select input routing		
    //	When low lhl pad bank selected (see iocfg_premux[11]),		
    //		2'b00	CS from P2
    //		2'b01	CS from P6
    //		else    RESERVED	
    //	When high lhl pad bank selected,		
    //		2'b00	CS from P26
    //		2'b01	CS from P32
    //		2'b10	CS from P39
    //		2'b11	RESERVED

    // not modify iocfg_premux[11] in miso configuration
    // let clock configure handle this bit
    IOCFG_PREMUX_CS_ROUTING_P2          =  0,
    IOCFG_PREMUX_CS_ROUTING_P6          =  SPIFFYD_BIT_2,
    IOCFG_PREMUX_CS_ROUTING_P26         =  0,
    IOCFG_PREMUX_CS_ROUTING_P32         =  SPIFFYD_BIT_2,
    IOCFG_PREMUX_CS_ROUTING_P39         =  SPIFFYD_BIT_3, 


    //
    //	iocfg_premux[8]		Spiffy1 input (slave) or feedback (master) clock routing	
    //		SCLK from P7 when iocfg_premux[11]=1 and iocfg_premux[8]=1		
    //		SCLK from P3 when iocfg_premux[11]=1 and iocfg_premux[8]=0		
    //		SCLK from P36 when iocfg_premux[11]=0 and iocfg_premux[8]=1		
    //		SCLK from P24 when iocfg_premux[11]=0 and iocfg_premux[8]=0		
    //				
    //		When spiffy1 is master, SCLK above must be programmed to match		
    //		the pad choice programming (iocfg_PX) for SCLK output.		
    //

    IOCFG_PREMUX_CLK_ROUTING_P7         = (SPIFFYD_BIT_8 | SPIFFYD_BIT_11),
    IOCFG_PREMUX_CLK_ROUTING_P3         = SPIFFYD_BIT_11,
    IOCFG_PREMUX_CLK_ROUTING_P36        = SPIFFYD_BIT_8,
    IOCFG_PREMUX_CLK_ROUTING_P24        =   0,

    //
    //iocfg_premux[5:4]	Spiffy1 slave MOSI input routing		
    //	When low lhl pad bank selected (see iocfg_premux[11]),		
    //		2'b00	MOSI from P0
    //		2'b01	MOSI from P4
    //		else    RESERVED	
    //	When high lhl pad bank selected:		
    //		2'b00	MOSI from P27
    //		2'b01   MOSI from P33	
    //		2'b10   MOSI from P38	
    //		2'b11   RESERVED	
    //
    IOCFG_PREMUX_MOSI_ROUTING_P0        =  0,
    IOCFG_PREMUX_MOSI_ROUTING_P4        =  SPIFFYD_BIT_4,
    IOCFG_PREMUX_MOSI_ROUTING_P27       =   0,
    IOCFG_PREMUX_MOSI_ROUTING_P33       =  SPIFFYD_BIT_4,
    IOCFG_PREMUX_MOSI_ROUTING_P38       =  SPIFFYD_BIT_5,
};

enum
{
    // for the GPIO function
    OUTPUT_FN_SEL_3                     =  0x30,
    OUTPUT_FN_SEL_2                     =  0x20,

    // main60hz_ctl[4]=1       - P24 presents m_spi_clk (from spiffy1) when output data muxing is 3			
    // main60hz_ctl[4:3]=2'b10 - P27 presents m_spi_mosi (from spiffy1) when output data muxing is 3         
    MAIN60HZ_SPI_MASK                   = (SPIFFYD_BIT_3 | SPIFFYD_BIT_4),
    MAIN60HZ_SPI_MISO_P27               = SPIFFYD_BIT_4,
    MAIN60HZ_SPI_MISO_P2                = SPIFFYD_BIT_4,
    MAIN60HZ_SPI_MISO_P24               = SPIFFYD_BIT_4,
};

typedef struct
{
    UINT32              curClkSpeed;
    SPIFFY_ENDIAN       curEndian;
    SPIFFY_MODE         curMode;
    SPIFFY_SS_POLARITY  curPolarity;    
} SpiffyState;

#ifdef __cplusplus
extern "C" {
#endif

void spiffyd_init(SpiffyInstance instance);
void spiffyd_configure(SpiffyInstance instance, UINT32 clkSpeedInHz, SPI_ENDIAN endian,
                       SPI_SS_POLARITY polarity, SPI_MODE mode);
void spiffyd_reset(SpiffyInstance instance);
void spiffyd_active(SpiffyInstance instance);
void spiffyd_idle(SpiffyInstance instance);
void spiffyd_txByte(SpiffyInstance instance, UINT8 b);
void spiffyd_txHalfWord(SpiffyInstance instance, UINT16 h);
void spiffyd_txWord(SpiffyInstance instance, UINT32 w);
void spiffyd_txData(SpiffyInstance instance, UINT32 txLen, const UINT8* txBuf);
void spiffyd_exchangeData(SpiffyInstance instance, UINT32 len, const UINT8* txBuf, UINT8* rxBuf);
void spiffyd_rxData(SpiffyInstance instance, UINT32 rxLen, UINT8* rxBuf);
void spiffyd_slaveEnableTx(SpiffyInstance instance);
void spiffyd_slaveDisableTx(SpiffyInstance instance);
void spiffyd_slaveEnableRx(SpiffyInstance instance);
void spiffyd_slaveDisableRx(SpiffyInstance instance);
void spiffyd_slaveTxData(SpiffyInstance instance, UINT32 txLen, const UINT8* txBuf);
void spiffyd_slaveTxByte(SpiffyInstance instance, UINT8 b);
void spiffyd_slaveTxHalfWord(SpiffyInstance instance, UINT16 h);
void spiffyd_slaveTxWord(SpiffyInstance instance, UINT32 w);
UINT32 spiffyd_slaveGetTxFifoCount(SpiffyInstance instance);
SPIFFY_STATUS spiffyd_slaveRxData(SpiffyInstance instance, UINT32 rxLen, UINT8* rxBuf);
UINT32 spiffyd_slaveGetRxFifoCount(SpiffyInstance instance);

void spiffyd_master1GpioConfig(Spi1MasterGpioConfigType gpioConfig);
void spiffyd_slave1GpioConfig(Spi1SlaveGpioConfigType gpioConfig);

void spiffyd_master1ConfigClk(UINT8 clkPin);
void spiffyd_master1ConfigMosi(UINT8 mosiPin);
void spiffyd_master1ConfigMiso(UINT8 misoPin);
void spiffyd_slave1ConfigClk(UINT8 clkPin);
void spiffyd_slave1ConfigMosi(UINT8 mosiPin);
void spiffyd_slave1ConfigMiso(UINT8 misoPin);
void spiffyd_slave1ConfigCs(UINT8 csPin);

/////////////////////////////////////////////////////////////////////
/// Convert P0-P39 to internal Port/Pin style
/// \param spiPin - P0-P39 
/// \return - internal Port number
//////////////////////////////////////////////////////////////////
INLINE UINT8 spiffyd_getGpioPort(UINT8 spiPin)
{
    return spiPin/GPIO_MAX_PINS_PER_PORT;
}

/////////////////////////////////////////////////////////////////////
/// Convert P0-P39 to internal Port/Pin style
/// \param spiPin - P0-P39 
/// \return - internal pin number
//////////////////////////////////////////////////////////////////
INLINE UINT8 spiffyd_getGpioPin(UINT8 spiPin)
{
    return spiPin%GPIO_MAX_PINS_PER_PORT;
}

/////////////////////////////////////////////////////////////////////
/// Given a spiffy instance, returns the spiffy control block
/// \param instance - The instance of the spiffy control block to get
/// \return - The spiffy control block for the given instance
//////////////////////////////////////////////////////////////////
INLINE tSPIFFY_REG* spiffyd_getSpiffyBlkPtr(SpiffyInstance instance)
{
    tSPIFFY_REG* sb = (instance == SPIFFYD_1) ? (tSPIFFY_REG *)spiffy_cfg_adr : (tSPIFFY_REG *)spiffy2_spiffy_cfg_adr;
    return sb;
}

#ifdef __cplusplus
}
#endif


/* @} */

#endif


