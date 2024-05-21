/*
 ****************************************************************
 * THIS INFORMATION IS PROPRIETARY TO CYPRESS SEMICONDUCTOR.
 *---------------------------------------------------------------
 *           Copyright (c) 2016 Cypress Semiconductor.
 *                      ALL RIGHTS RESERVED
 ****************************************************************

 ****************************************************************
 * File Name: spiffy_common.h
 *
 * Abstract: abstract info for SPIFFY Controller for 20739.
 *
 * Note:
 *
 ****************************************************************
 */

#include "brcm_fw_types.h"

#define SPIFFY_CFG_SPIMODE_11           (0x3u << 1)
#define SPIFFY_CFG_FLOWCTL_TX_RX        (0x2u << 13)
#define SPIFFY_CFG_AUTOCS               (0x1u << 21)
#define SPIFFY_CFG_BIT_BE               (0x1u << 5)
#define SPIFFY_CFG_PROTOCOL_GM          (0x2u << 8)

#define SPIFFY_TPORT_DIV_0              (0x0) //24MHZ
#define SPIFFY_TPORT_DIV_1              ((0x1u << 7) | (0x1u << 1)) //12MHZ
#define SPIFFY_TPORT_DIV_2              ((0x2u << 7) | (0x2u << 1)) //8MHZ
#define SPIFFY_TPORT_DIV_3              ((0x3u << 7) | (0x3u << 1))
#define SPIFFY_TPORT_DIV_4              ((0x4u << 7) | (0x4u << 1))
#define SPIFFY_TPORT_DIV_5              ((0x5u << 7) | (0x5u << 1))
#define SPIFFY_TPORT_DIV_6              ((0x6u << 7) | (0x6u << 1))
#define SPIFFY_TPORT_DIV_7              ((0x7u << 7) | (0x7u << 1))

#define SPIFFY_INT_GENERIC_SPI_MASTER_DONE      0x00080000

// Memory mapped structure of spiffy block
typedef struct
{
    volatile UINT32 cfg;                // 0x0000
    volatile UINT32 transmission_len;   // 0x0004
    volatile UINT32 tx_fifo_level;      // 0x0008
    volatile UINT32 tx_ae_level;        // 0x000C
    volatile UINT32 tx_dma_len;         // 0x0010
    volatile UINT32 rx_len;             // 0x0014
    volatile UINT32 rx_fifo_level;      // 0x0018
    volatile UINT32 rx_af_level;        // 0x001C
    volatile UINT32 rx_dma_len;         // 0x0020
    volatile UINT32 int_status;         // 0x0024
    volatile UINT32 int_enable;         // 0x0028
    volatile UINT32 status;             // 0x002C
    volatile UINT32 clock_cfg;          // 0x0030
#if defined(SPIFFY_DSPI_QSPI_DBIC) || defined(QSPI_FLASH)
    volatile UINT32 dbic_length;        // 0x0034
    volatile UINT32 qspi_length;        // 0x0038
    volatile UINT32 ahb_master;         // 0x003c
    volatile UINT32 ahb_master_code;    // 0x0040
    volatile UINT32 tx_test;            // 0x0044
    volatile UINT32 unused_48_80[(0x80-0x48)/sizeof(UINT32)];
    volatile UINT32 RxFIFO[16];         // 0x0080..0x00BF
    volatile UINT32 TxFIFO[16];         // 0x00C0..0x00FF
    volatile UINT32 rx_fifo;            // 0x0100
    volatile UINT32 tx_fifo;            // 0x0104
#elif defined(SPIFFY_16BYTE_FIFO)
    volatile UINT32 unused_34_80[(0x80-0x34)/sizeof(UINT32)];
    volatile UINT32 rx_fifo;            // 0x0080
    volatile UINT32 RxFIFO[15];         // 0x0084..0x00BF
    volatile UINT32 tx_fifo;            // 0x00C0
    volatile UINT32 TxFIFO[15];         // 0x00C4..0x00FF
#else
    volatile UINT32 unused_34_100[(0x100-0x34)/sizeof(UINT32)];
    volatile UINT32 rx_fifo;            // 0x0100
    volatile UINT32 tx_fifo;            // 0x0104 or 0x120
#endif
}
tSPIFFY_REG;

typedef enum
{
    SPIFFY_BLOCK_1 = 0,
    SPIFFY_BLOCK_2 = 1
} SPIFFY_BLOCK;

/// Number of spiffy blocks.
typedef enum SpiffyInstance
{
    SPIFFYD_1  = SPIFFY_BLOCK_1,
    SPIFFYD_2  = SPIFFY_BLOCK_2,
    MAX_SPIFFYS = 2
} SpiffyInstance;

extern tSPIFFY_REG * spiffy_sw_reset(UINT32 inst, UINT32 cfg, UINT32 tportdiv);
extern void spiffyd_txData(SpiffyInstance instance, UINT32 txLen, const UINT8* txBuf);
extern void spiffyd_exchangeData(SpiffyInstance instance, UINT32 len, const UINT8* txBuf, UINT8* rxBuf);
extern void spiffy_disableFifos(tSPIFFY_REG *spiffyRegPtr);
extern void spiffy_masterEnableTxOnly(tSPIFFY_REG *spiffyRegPtr);
extern void spiffy_masterEnableFd(tSPIFFY_REG *spiffyRegPtr);
