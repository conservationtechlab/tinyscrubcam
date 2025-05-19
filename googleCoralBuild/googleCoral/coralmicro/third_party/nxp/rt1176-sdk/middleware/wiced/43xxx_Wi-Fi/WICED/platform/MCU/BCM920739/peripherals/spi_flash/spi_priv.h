/*
 * Copyright 2021, Cypress Semiconductor Corporation or a subsidiary of 
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software"),
 * is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
#ifndef _SPI_PRIV_H_
#define _SPI_PRIV_H_

#include "brcm_fw_types.h"

#ifdef __cplusplus
extern "C"
{
#endif
/*******************************************************************************
* Defines & Macros for SPIFFY block
*******************************************************************************/
#define SPIFFY_CFG_SOFT_RESET                   0x00000001
#define SPIFFY_CFG_MODE_MASK                    0x00000006
#define SPIFFY_CFG_MODE_0                       0x00000000
#define SPIFFY_CFG_MODE_1                       0x00000002
#define SPIFFY_CFG_MODE_2                       0x00000004
#define SPIFFY_CFG_MODE_3                       0x00000006
#define SPIFFY_CFG_BYTE_ENDIANNESS_MASK         0x00000018
#define SPIFFY_CFG_BYTE_ENDIANNESS_LITTLE       0x00000000
#define SPIFFY_CFG_BYTE_ENDIANNESS_BIG_16       0x00000008
#define SPIFFY_CFG_BYTE_ENDIANNESS_BIG_32       0x00000010
#define SPIFFY_CFG_BIT_ENDIANNESS               0x00000020
#define SPIFFY_CFG_CS_POLARITY                  0x00000040
#define SPIFFY_CFG_INT_POLARITY                 0x00000080
#define SPIFFY_CFG_PROTOCOL_MASK                0x00000700
#define SPIFFY_CFG_PROTOCOL_GENERIC_SLAVE       0x00000000
#define SPIFFY_CFG_PROTOCOL_EMPSPI_SLAVE        0x00000100
#define SPIFFY_CFG_PROTOCOL_GENERIC_MASTER      0x00000200
#define SPIFFY_CFG_PROTOCOL_EMPSPI_MASTER       0x00000300
#define SPIFFY_CFG_TX_FIFO_EN                   0x00000800
#define SPIFFY_CFG_RX_FIFO_EN                   0x00001000
#define SPIFFY_CFG_FLOW_CONTROL_MASK            0x00006000
#define SPIFFY_CFG_FLOW_CONTROL_TX              0x00000000
#define SPIFFY_CFG_FLOW_CONTROL_RX              0x00002000
#define SPIFFY_CFG_FLOW_CONTROL_FD              0x00004000
#define SPIFFY_CFG_CS_OEN                       0x00008000
#define SPIFFY_CFG_CS_VAL                       0x00010000
#define SPIFFY_CFG_INT_OEN                      0x00020000
#define SPIFFY_CFG_INT_VAL                      0x00040000
#define SPIFFY_CFG_GO_TO_SLEEP                  0x00080000
#define SPIFFY_CFG_DELAY_INT                    0x00100000
#define SPIFFY_CFG_AUTO_CS                      0x00200000

// Tx Fifo Level Bitmasks
#define SPIFFY_TX_FIFO_LEVEL_MASK               0x000007FF

// Rx Fifo Level Bitmasks
#define SPIFFY_RX_FIFO_LEVEL_MASK               0x000007FF

// Interrupt Status and Enable Bitmasks
#define SPIFFY_INT_TX_AE                        0x00000001
#define SPIFFY_INT_TX_DONE                      0x00000002
#define SPIFFY_INT_TX_UNDERFLOW                 0x00000004
#define SPIFFY_INT_RX_AF                        0x00000008
#define SPIFFY_INT_RX_DONE                      0x00000010
#define SPIFFY_INT_RX_OVERFLOW                  0x00000020
#define SPIFFY_INT_CS_ACTIVE                    0x00000040
#define SPIFFY_INT_CS_INACTIVE                  0x00000080
#define SPIFFY_INT_DIRECT_WRITE_START           0x00000100
#define SPIFFY_INT_TX_EMPTY                     0x00000200
#define SPIFFY_INT_RX_FULL                      0x00000400
#define SPIFFY_INT_GENERIC_SPI_MASTER_DONE      0x00080000

// Clock Config Bitmasks and defines
#define SPIFFY_CLK_USE_TPORT                    0x00000001
#define SPIFFY_CLK_TPORT_CLOCK_DIV_MASK         0x0000003F
#define SPIFFY_CLK_TPORT_CLOCK_DIV_H_SHIFT      1
#define SPIFFY_CLK_TPORT_CLOCK_DIV_L_SHIFT      7


// cr_pulse_reset_peri_adr[6]: spiffy_rst_clock_domain
#define CR_PULSE_RESET_SPIFFY_TRANSPORT_CLK_DOMAIN (1 << 6)
// cr_pulse_reset_peri_adr[24]: spiffy2_rst_clock_domain
#define CR_PULSE_RESET_SPIFFY2_TRANSPORT_CLK_DOMAIN (1 << 24)

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

#define SPIFFY_MASTER_CLK_FREQ_IN_HZ            24000000
#define SPIFFY_MAX_INTEGRAL_DIVISOR_FREQ_IN_HZ  12000000
#define SPIFFY_MIN_INTEGRAL_DIVISOR_FREQ_IN_HZ    187500



#if defined (BCM4314) || defined (BCM4334) || defined (BCM43341) \
    || defined SPIFFY_16BYTE_FIFO || defined(BCM43242) || defined(BCM20703)
// for 4314 This should be defined in ptu related .h
#define HW_PTU_HC_SEL_SPIFFY    0x20
#endif

//SPIFFY DETECT
#define HAVE_NO_SPIFFY 0
#define HAVE_SPIFFY1 0x01
#define HAVE_SPIFFY2 0x02
#define HAVE_SPIFFY1_N_SPIFFY2 0x03

#define SPIFFY_CFG_MODE_BITMASK 1
#define SPIFFY_CFG_CS_POLARITY_BITMASK 6
#define SPIFFY_CFG_BIT_ENDIANNESS_BITMASK 5

#define DC_PTU_HC_SEL_SPIFFY            (0x20)
#define DC_PTU_AUX_HC_SEL_SPIFFY        (0x20)
#define SPIFFY_CFG_SOFTRESET            (0x1u)
#define SPIFFY_INT_CLEAR_32_BITS        (0x0u)
#define PULSE_RESET_PERI_SPIFFY         (0x1u << 6)
#define PULSE_RESET_PTU_AUX_SPIFFY      (0x1u << 3)
#define SF_CFG_TABLE_ENTRIES    13


/******************************************************************************
* Type Definitions
******************************************************************************/


/*******************************************************************************
* Function Prototypes
*******************************************************************************/
void spi_soft_reset(UINT32 new_cfg_reg_value, tSPIFFY_REG *spiffyRegPtr);
void spi_disableFifos(tSPIFFY_REG *spiffyRegPtr);
void spi_blockingSend(tSPIFFY_REG *spiffyRegPtr,UINT32 len);
void spi_masterEnableTxOnly(tSPIFFY_REG *spiffyRegPtr);
void spi_masterEnableFdShortTx(tSPIFFY_REG *spiffyRegPtr);
void spi_drainRxFifo(UINT32 count, tSPIFFY_REG *spiffyRegPtr);
void spi_clearDoneBit(tSPIFFY_REG *spiffyRegPtr);
void spi_waitForDoneBit(tSPIFFY_REG *spiffyRegPtr);
tSPIFFY_REG * spi_sw_reset(UINT32 inst, UINT32 cfg, UINT32 tportdiv);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //  _SPI_PRIV_H_
