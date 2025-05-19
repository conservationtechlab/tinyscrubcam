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

/** @file
 * Implmentation of Serial flash driver for 20739 Eval board
 */

#include <string.h>
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform.h"
#include "platform_constants.h"
#include "wiced_platform.h"
#include "spi.h"
#include "serialflash_priv.h"
#include "serialflash.h"


/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
// SERIAL FLASH COMMANDS
#define SF_WRSR                         0x01    // SST, STM, ATML, MXIC
#define SF_WRITE                        0x02    // SST, STM, ATML, MXIC
#define SF_READ                         0x03    // SST, STM, ATML, MXIC
#define SF_WRDI                         0x04    // SST, STM, ATML, MXIC
#define SF_RDSR                         0x05    // SST, STM, ATML, MXIC
#define SF_WREN                         0x06    // SST, STM, ATML, MXIC
#define SF_FAST_READ                    0x0B    // SST, STM, ATML, MXIC
#define SF_SEC_ERASE                    0x20    // SST,      ATML, MXIC
#define SF_EWSR                         0x50    // SST
#define SF_BLK_ERASE                    0xD8    // SST, STM, ATML, MXIC, or 0x52
#define SF_CHIP_ERASE                   0xC7    // SST, STM, ATML, MXIC, or 0x60
#define SF_REMS                         0x90    // SST,            MXIC, or 0xAB
#define SF_RDID1                        0x9F    //      STM, ATML, MXIC
#define SF_RDID2                        0x15    //      ATMEL AT25F512
#define SF_WRITE_INC                    0xAF    // SST
#define SF_RDP                          0xAB    //      STM,       MXIC
#define SF_DP                           0xB9    //      STM,       MXIC

// SOFTWARE STATUS REGISTER
#define SF_STAT_BUSY                    0x01
#define SF_STAT_WEL                     0x02
#define SF_STAT_BP0                     0x04
#define SF_STAT_BP1                     0x08
#define SF_STAT_RES                     0x30
#define SF_STAT_AAI                     0x40    // SST
#define SF_STAT_BPL                     0x80    // 0:read/writable, 1:read-only
#define SF_SECTOR_SIZE_4K                  4 * 1024
#define SF_SECTOR_SIZE_32K                  32 * 1024
#define SF_BLOCK_SIZE_64K                   64 * 1024

#ifdef PLATFORM_SERIAL_FLASH_SIZE
#define SF_FLASH_SIZE PLATFORM_SERIAL_FLASH_SIZE
#else
//use default 1M Byte flash size
#define SF_FLASH_SIZE 1024 * 1024
#endif


#define SF_PAGE_WRITE_SIZE              256     // MXIC, STM
#define SF_MEMCPY_BUFF_SIZE             128     // used by sfi_memcpy_a()
#define SF_BYTE_WRITE_MAX_SIZE          5       // SST

// SERIAL FLASH PROTECTED AREA
#define SF_PROTECT_NONE                 0
#define SF_PROTECT_QUARTER              1
#define SF_PROTECT_HALF                 2
#define SF_PROTECT_ALL                  3
#define SF_SW_PROTECT_BIT_MASK          0x3C     // All four protection bits
#define SF_SRP_BIT                      0x80     // Status Register Protect

// SERIAL FLASH MANUFACTURER ID
#define SF_MAKER_ATML                   0x1F    //ADESTO
#define SF_MAKER_STM                    0x20
#define SF_MAKER_SST                    0xBF
#define SF_MAKER_MXIC                   0xC2
#define SF_MAKER_WBOND                  0xEF
#define SF_MAKER_EON                    0x1C
#define SF_MAKER_AMIC                   0x37
#define SF_MAKER_AMIC                   0x37

// SERIAL FLASH DEVICE ID
#define SF_DEV_ID_SST25VF512            0x48    // 512Kb or 64KB
#define SF_DEV_ID_MX25L512              0x05
#define SF_DEV_ID_MX25L512              0x05
#define SF_DEV_ID_AT25XE021             0x43
#define SF_ADESTO_ULTRA_DEEP_PWRDN      0x79
#define SF_ADESTO_DUAL_OUT_READ         0x3B
#define SF_DSPI_READ                    SF_ADESTO_DUAL_OUT_READ

// AON_sfi_status_extended bits
#define SFI_STATUS_EXTENDED_ENABLE_DPSI                      0x01
#define SFI_STATUS_EXTENDED_USE_ADESTO_SLEEP                 0x02
#define SFI_STATUS_EXTENDED_ENABLE_VS_CACHE                  0x04
#define SFI_STATUS_EXTENDED_UBCS_RESEND_SLEEP                0x08
#define SFI_STATUS_EXTENDED_SLEEP_WHEN_BT_TRANSPORT_DETECTED 0x10

#define SF_CFG_TABLE_ENTRIES    13
#define SF_CFG_DEFAULT               SF_CFG_TABLE_ENTRIES-1
#define SF_CFG_SST_DEFAULT           SF_CFG_TABLE_ENTRIES-2
#define SF_CFG_M25P40                SF_CFG_TABLE_ENTRIES-3

#define SF_AON_SCATTER_LOADED_SIG 0xA56137D9

#define SF_WRITE_SIZE 128
#define sfi_dspi_spiffy_mode_a(i,m)   spi_sw_reset(i,(m)|SPIFFY_CFG_BIT_BE|SPIFFY_CFG_AUTOCS,SPIFFY_CLK_CFG)


/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/
extern const wiced_spi_device_t wiced_spi_flash;
extern const platform_gpio_t platform_gpio_pins[];
extern const platform_spi_t platform_spi_peripherals[];
//extern int verify_data;  // Needed for debugging


BOOL32 sf_aon_scattered_loaded_sig = SF_AON_SCATTER_LOADED_SIG;
FOUNDATION_CONFIG_SERIAL_FLASH_WRITE_PROTECT_t g_foundation_config_serialFlashWriteProtect;

UINT8 AON_sfi_sig[4] = {0, 0, 0, 0};  //In case of a scatterload reset back to 0
SF_STATUS_EXTENDED AON_sfi_status_extended = { 0, 0, 0, 0, 0, 0 };
UINT32 (*new_sfi_write)(UINT32, IN UINT8 *, UINT32) = NULL;
UINT32 (*new_sfi_read)(UINT32, UINT8 *, UINT32) = NULL;
void (*new_sfi_exit_deep_power_down)(BOOL32) = NULL;
void (*new_sfi_enter_deep_power_down)(void) = NULL;
void (*new_sfi_allow_deep_sleep)(void) = NULL;
SF_STATUS sf_status;
SF_ATTRIB sf_attr;
UINT8 sf_size;      // real size = 64KB * (size + 1), so 0 = 64KB
BOOL32 sfi_vs_location_changed = TRUE;
static UINT8 backup_buffer[SF_SECTOR_SIZE_4K];

static const SF_CFG_TABLE_ENTRY sf_mem_table[SF_CFG_TABLE_ENTRIES] =
{
    // MXIC MX25L8005, 1MB, pwr_dn(1) pg_wr(1) fst_rd(1) ewsr(0) poll(0)
    {3, SF_MAKER_MXIC,  0x20, 0x14, (1024/64 - 1), 3, .mem_attr.byte = 0xEC},
    // AT25FS010, 128KB, pwr_dn(0) pg_wr(1) fst_rd(1) ewsr(0) poll(0)
    {3, SF_MAKER_ATML, 0x66, 0x01, (128/64 - 1), 30, .mem_attr.byte = 0x6D},
    // AT25F512B, 64KB, pwr_dn(0) pg_wr(1) fst_rd(1) ewsr(0) poll(0)
    {3, SF_MAKER_ATML, 0x65, 0x00, (64/64 - 1), 30, .mem_attr.byte = 0xEE},
    // SST25VF512A, 64KB, pwr_dn(0) pg_wr(0) fst_rd(1) ewsr(1) poll(0)
    {2, SF_MAKER_SST,  0x48, 0x00, (64/64 - 1), 0xFF, .mem_attr.byte = 0x30},
    // SST25VF010A/25VF010, 128KB, pwr_dn(0) pg_wr(0) fst_rd(0) ewsr(1) poll(0)
    {2, SF_MAKER_SST,  0x49, 0x00, (128/64 - 1), 0xFF, .mem_attr.byte = 0x30},
    // MXIC MX25L512, 64KB, pwr_dn(1) pg_wr(1) fst_rd(1) ewsr(0) poll(0)
    {3, SF_MAKER_MXIC,  0x20, 0x10, (64/64 - 1), 3, .mem_attr.byte = 0xEC},
    // EON flash EN25F10, 128KByte, pwr_dn(0), pg_wr(0), fst_rd(1), ewsr(0), poll(0)
    {3, SF_MAKER_EON,  0x31, 0x11, (128/64 - 1), 3, .mem_attr.byte = 0xEC},
    // AMIC flash A25L020, 2MBit, pwr_dn(1), pg_wr(0), fst_rd(1), ewsr(0), poll(0)
    {3, SF_MAKER_AMIC,  0x30, 0x12, (128/64 - 1), 3, .mem_attr.byte = 0xB0},
    // AMIC flash A25L010, 1Mbit, pwr_dn(1), pg_wr(0), fst_rd(1), ewsr(0), poll(0)
    {3, SF_MAKER_AMIC,  0x30, 0x11, (128/64 - 1), 3, .mem_attr.byte = 0xB0},
    // AMIC flash A25L512, 512Kbit, pwr_dn(1), pg_wr(0), fst_rd(1), ewsr(0), poll(0)
    {3, SF_MAKER_AMIC,  0x30, 0x10, (128/64 - 1), 3, .mem_attr.byte = 0xB0},
    // Numonyx/ST M25P40 series, 512KB, pwr_dn(1) pg_wr(1) fst_rd(1) ewsr(0) poll(0)
    {3, SF_MAKER_STM,  0x20, 0x13, (512/64 - 1), 30, .mem_attr.byte = 0xEC},
    // Default SST - fst_rd(1) ewsr(0) poll(0)
    {3, SF_MAKER_SST,  0xFF, 0x14, (1024/64 - 1), 30, .mem_attr.byte = 0x30},
    // Default other vendors
    {3, SF_MAKER_STM,  0xFF, 0x14, (1024/64 - 1), 30, .mem_attr.byte = 0xEC},
};


static const SF_CFG_COMMAND sf_command_table[SF_CFG_TABLE_ENTRIES] =
{
    // MXIC MX25L8005, 1MB, 8MBit
    {0x06, 0x04,  0x9f, 0x05, 0x01, 0x03, 0x0b, SF_WRITE, 0x02, 0xff, 0xff, 0xff, 0x20, 0xd8, 0xd8, 0xc7, 0xb9, 0xab},
    // ATMEL AT25FS010, 128KB, 1MBit
    {0x06, 0x04,  0x9f, 0x05, 0x01, 0x03, 0x0b, SF_WRITE, 0x02, 0xff, 0xff, 0xff, 0x20, 0xd8, 0xd8, 0xc7, 0xff, 0xff},
    // ATMEL AT25F512B, 64KB, 512KBit
    {0x06, 0x04, 0x15, 0x05, 0x01, 0x03, 0xff, SF_WRITE, 0x02, 0xff, 0xff, 0xff, 0x20, 0x52, 0x52, 0x62, 0xb9, 0xab},
    // SST 25VF512A, 64KB, 512KBit
    {0x06, 0x04,  0x90, 0x05, 0x01, 0x03, 0x0b, SF_WRITE, 0x02, 0xaf, 0x50, 0xff, 0xff, 0x20, 0xd8, 0xc7, 0xff, 0xff},
    // SST 25VF010A, 128KB, 1MBit  // turn off fast_read command for this serial flash in order to compatable with 25VF010 model
    {0x06, 0x04, 0x90, 0x05, 0x01, 0x03, 0xff, SF_WRITE, 0x02, 0xaf, 0x50, 0xff, 0xff, 0x20, 0xd8, 0xc7, 0xff, 0xff},
    // MXIC MX25L512, 64KB, 512KBit
    {0x06, 0x04,  0x9f, 0x05, 0x01, 0x03, 0x0b, SF_WRITE, 0x02, 0xff, 0xff, 0xff, 0x20, 0xd8, 0xd8, 0xc7, 0xb9, 0xab},
    // EON EN25F10, 128KByte, 1Mbit
    {0x06, 0x04,  0x9f, 0x05, 0x01, 0x03, 0x0b, SF_WRITE, 0x02, 0xff, 0xff, 0xff, 0xff, 0x20, 0xd8, 0xc7, 0xb9, 0xab},
    // AMIC flash A25L020, 256KByte, 2Mbit
    {0x06, 0x04, 0x9f, 0x05, 0x01, 0x03, 0x0b, SF_WRITE, 0x02, 0xff, 0xff, 0xff, 0xff, 0x20, 0xd8, 0xc7, 0xb9, 0xab},
    // AMIC flash A25L010, 128KByte, 1Mbit
    {0x06, 0x04, 0x9f, 0x05, 0x01, 0x03, 0x0b, SF_WRITE, 0x02, 0xff, 0xff, 0xff, 0xff, 0x20, 0xd8, 0xc7, 0xb9, 0xab},
    // AMIC flash A25L512, 64KByte, 512Kbit
    {0x06, 0x04, 0x9f, 0x05, 0x01, 0x03, 0x0b, SF_WRITE, 0x02, 0xff, 0xff, 0xff, 0xff, 0x20, 0xd8, 0xc7, 0xb9, 0xab},
    // Numonyx/ST M25P40 series, xMB, xMBit
    {0x06, 0x04,  0x9f, 0x05, 0x01, 0x03, 0x0b, 0x0a, 0x02, 0xff, 0xff, 0xff, 0x20, 0xd8, 0xd8, 0xc7, 0xb9, 0xab},
    // SST default
    {0x06, 0x04,  0x9f, 0x05, 0x01, 0x03, 0x0b, SF_WRITE, 0x02, 0xad, 0x50, 0xff, 0x20, 0x20, 0x20, 0xc7, 0xff, 0xff},
    // Default other vendors
    {0x06, 0x04,  0x9f, 0x05, 0x01, 0x03, 0x0b, SF_WRITE, 0x02, 0xff, 0xff, 0xff, 0x20, 0xd8, 0xd8, 0xc7, 0xb9, 0xab}
};

platform_spi_config_t sf_spi_config;
UINT32 spi_port = SPIFFY_BLOCK_2;  // default choose spi2

/******************************************************
 *               Function Declarations
 ******************************************************/
void sflash_set_write_protect_off(void);
void sflash_write_enable(void);
UINT8 sflash_read_status(void);
void sflash_write_status_enable(void);
void sflash_wait_time_between_program(void);
void clock_DelayMicroseconds(UINT32 microseconds);

/******************************************************
 *               Function Definitions
 ******************************************************/
void sflash_init(void)
{
    sf_status.detected = 0;
    sf_status.got_reset = 0;
    sf_status.dont_pwr_dn = 0;
    sf_status.pwr_is_dn = 0;
    sf_status.pwr_dn_enable = 0;
    sf_status.use_default = 0;
    sf_status.reserved = 0;
    sf_status.sf_index = 0;
    sf_status.delay_cnt = 10*52/4;   // worst case: 10 us delay and CPU CLK at 52 MHz
    //sfi_init and new_sfi_exit_deep_power_down may be called before scatterload.
    //Make sure the hooks are not called at this time.
    new_sfi_write = NULL;
    new_sfi_read = NULL;
    new_sfi_exit_deep_power_down = NULL;
    new_sfi_enter_deep_power_down = NULL;
    new_sfi_allow_deep_sleep = NULL;

    sf_spi_config.speed = wiced_spi_flash.speed;
    sf_spi_config.bits = wiced_spi_flash.bits; // not used
    sf_spi_config.chip_select = &platform_gpio_pins[wiced_spi_flash.chip_select],
    sf_spi_config.mode = wiced_spi_flash.mode;

    spi_port = wiced_spi_flash.port;

    spi_reset(spi_port,  sf_spi_config.speed, SPIFFY_MSB_FIRST,
             SPIFFY_SS_ACTIVE_LOW, sf_spi_config.mode);


    sf_attr.attr.poll_btw_prog = 1;
    sf_attr.attr.page_write = 1;
    memset(backup_buffer, 0xff, SF_SECTOR_SIZE_4K);
}

void sflash_deinit( void )
{
    //Currently return success.
    //Need to check if anything else needs to be done for the flash.
    return;
}

UINT8 sflash_read_id(UINT8* buffer)
{
    UINT8  temp[4];
    UINT8  rdid1 = SF_RDID1;
    UINT8  rdid2 = SF_RDID2;
    UINT32 rems = SF_REMS;
    UINT16 retry_cnt;
    if (buffer == NULL)
    {
        buffer = temp;
    }
    sflash_exit_deep_power_down(TRUE);
    for (retry_cnt = 2; retry_cnt > 0; retry_cnt--)
    {
        spi_write_then_read(spi_port, &rdid1, 1, buffer, 3);
        if (buffer[0] != 0 && buffer[0] != 0xff)
        {
            return 3;       // STM/NUMONYX and Atmel and AMIC
        }
        spi_write_then_read(spi_port, &rdid2, 1, buffer, 2);
        buffer[2] = 0x00;
        if (buffer[0] != 0 && buffer[0] != 0xff)
        {
            return 2;       // Atmel AT25F512
        }
        spi_write_then_read(spi_port, (UINT8*)&rems, 4, buffer, 2);
        buffer[2] = 0x00;
        if (buffer[0] != 0 && buffer[0] != 0xff)
        {
            return 2;       // SST
        }
        sflash_exit_deep_power_down(TRUE);  // try again, in case sFlash needs this
    }
    return 0;
}

void sflash_exit_deep_power_down(BOOL32 forceExitDeepPowerDown)
{
    INT16 i;
    UINT8 cmd = SF_RDP;
    if (new_sfi_exit_deep_power_down)
    {
        new_sfi_exit_deep_power_down(forceExitDeepPowerDown);
        return;
    }
    if (sf_status.dont_pwr_dn == 0 || forceExitDeepPowerDown == TRUE)
    {
        spi_write(spi_port, &cmd, sizeof(cmd), sizeof(cmd), TRUE, TRUE );
        if(sflash_check_AON_flag(AON_sfi_status_extended.use_adesto_deep_sleep))
        {
            //HCI firmware download can reset this.  This will make sure we can download firmware in HCI mode
            sf_status.delay_cnt = 70*52/4;   // worst case: 70 us delay and CPU CLK at 52 MHz
        }
        for(i=0; i<sf_status.delay_cnt; i++)
        {
        }
    }
    sf_status.pwr_is_dn = 0;
}

UINT32 sflash_check_AON_flag(UINT16 flag)
{
    return ((sf_aon_scattered_loaded_sig == SF_AON_SCATTER_LOADED_SIG) && (flag == 1));
}
void sflash_write_status_enable(void)
{
    UINT8 cmd = SF_EWSR;
    spi_write(spi_port, &cmd, sizeof(cmd), sizeof(cmd), TRUE, TRUE );
}
void sflash_set_write_protect_off(void)
{
    // Disable the Software Protected Mode ( SPM ) feature by clearing the Block Protect bits
    sflash_set_protect_level(SF_PROTECT_NONE);
}
void sflash_write_enable(void)
{
    UINT8 cmd = SF_WREN;
    spi_write(spi_port, &cmd, sizeof(cmd), sizeof(cmd), TRUE, TRUE );
}
UINT8 sflash_read_status(void)
{
    UINT8 status;
    UINT8 cmd = SF_RDSR;
    spi_write_then_read(spi_port, &cmd, 1, &status, 1);
    return status;
}
void sflash_set_protect_level(UINT8 level)
{
    UINT8 status;
    level = (UINT8)((level & (SF_SW_PROTECT_BIT_MASK >> 2)) << 2);
    status = (UINT8)(sflash_read_status() & (~SF_SW_PROTECT_BIT_MASK));
    sflash_write_status(status | level);
    if (sf_attr.attr.extra_wrsr)
    {
        sflash_write_status(status | level);
    }
}

BOOL32 sflash_detected(void)
{
    UINT8 i, len;
    sf_status.detected = 0;

    if ((len = sflash_read_id(AON_sfi_sig)) != 0)
    {
        for (i = 0; i < SF_CFG_SST_DEFAULT; i++)
        {
            if (sf_mem_table[i].sig_len  == len &&
                sf_mem_table[i].maker_id == AON_sfi_sig[0] &&
                sf_mem_table[i].mem_type == AON_sfi_sig[1] &&
                sf_mem_table[i].mem_cap  == AON_sfi_sig[2])
            {
                sf_size = sf_mem_table[i].size;
                sf_attr.byte = sf_mem_table[i].mem_attr.byte;
                sf_status.pwr_is_dn = 0;      // not in deep power down
                sf_status.dont_pwr_dn = 1;    // can't go deep power down during boot up to reduce boot time.  Need to enable after boot up.
                sf_status.detected = 1;
                sf_status.sf_index = i;
                return TRUE;
            }
        }
        // use default command set if SF returned right manufacture code
        if( AON_sfi_sig[0] == SF_MAKER_SST )
        {
            sf_status.sf_index = SF_CFG_SST_DEFAULT;
        }
        else
        {
            // all others
            sf_status.sf_index = SF_CFG_DEFAULT;
        }
        // Default setting
        sf_attr.byte = sf_mem_table[sf_status.sf_index].mem_attr.byte;
        sf_size = AON_sfi_sig[2] & 0x0F;
        sf_status.pwr_is_dn = 0;      // not in deep power down
        sf_status.dont_pwr_dn = 1;    // can't go deep power down during boot up to reduce boot time.  Need to enable after boot up.
        sf_status.detected = 1;
        sf_status.use_default = 1;
        //Enable Adesto extended features later as a patch
        return TRUE;
    }
    return FALSE;
}


UINT32 sflash_read(UINT32 addr, UINT8 *buffer, UINT32 len )
{
    UINT8 saddr[5];
    UINT32 slen;
    UINT8* p = buffer;
    UINT32 totalLen = len;
    UINT32 max_read_len = 200;

    if (new_sfi_read)
    {
        return new_sfi_read(addr, buffer, len);
    }
    if (!sf_status.detected && !sflash_detected())
        return 0;
    if (len == 0)
        return 0;
    sflash_exit_deep_power_down(FALSE);

    while(len > 0)
    {
        if(len < max_read_len)
        {
            slen = len;
        }
        else
        {
            slen = max_read_len;
        }
        saddr[1] = ((UINT8*)&addr)[2];
        saddr[2] = ((UINT8*)&addr)[1];
        saddr[3] = ((UINT8*)&addr)[0];

        if (sf_attr.attr.fast_read)
        {
            saddr[0] = SF_FAST_READ;
            saddr[4] = 0x55;
            spi_write_then_read(spi_port, saddr, 5, p, slen);
        }
        else
        {
            saddr[0] = SF_READ;
            spi_write_then_read(spi_port, saddr, 4, p, slen);
        }
        addr += slen;
        p += slen;
        len -= slen;
    }

    sflash_enter_deep_power_down();
    return totalLen;
}

void sflash_enter_deep_power_down(void)
{
    UINT8 cmd = SF_DP;
    if(new_sfi_enter_deep_power_down)
    {
        new_sfi_enter_deep_power_down();
        return;
    }
    if(sflash_check_AON_flag(AON_sfi_status_extended.use_adesto_deep_sleep))
    {
        //Adesto Ultra Deep Sleep
        cmd = SF_ADESTO_ULTRA_DEEP_PWRDN;
    }
    else
    {
        //Normal Deep Sleep
        cmd = SF_DP;
    }
    if (sf_attr.attr.deep_pwr_dn && sf_status.pwr_dn_enable && !sf_status.dont_pwr_dn)
    {
        spi_write(spi_port, &cmd, sizeof(cmd), sizeof(cmd), TRUE, TRUE );
        sf_status.pwr_is_dn = 1;
    }
}

UINT32 sflash_write_split(UINT32 addr, UINT8 *buffer, UINT32 len)
{
    if (!sf_status.detected)
        return 0;
    if (len == 0)
        return 0;
    if (new_sfi_write)
        return new_sfi_write(addr, buffer, len);

     sflash_exit_deep_power_down(TRUE);

    if ( g_foundation_config_serialFlashWriteProtect.SW_WP_Enable ) // SW WP or (SW + HW) WP
    {
        sflash_set_write_protect_off();
    }
    else
    {
        if (sf_attr.attr.use_ewsr)
        {
            sflash_write_status_enable();
        }
        else
        {
            sflash_write_enable();      // STM, ATML, or MXIC use WREN
            while(!(sflash_read_status() & SF_STAT_WEL))
                ;
        }
    }
    if (sf_attr.attr.page_write)
    {
        // for ATML, MXIC or STM's page programming mode
        UINT32 pages;
        UINT32 leading_bytes;   // # of bytes to next page boundary
        UINT32 trailing_bytes;  // # of bytes left after the last full page
        UINT32 page_size;
        if(sf_attr.attr.page_size_128)
            page_size = SF_PAGE_WRITE_SIZE/2;
        else
#ifdef SPIFFY_FIFO_SIZE_256
            page_size = SF_PAGE_WRITE_SIZE/2;
#else
            page_size = SF_PAGE_WRITE_SIZE;
#endif
        // the number of leading bytes cannot be zero
        // calculate the number of bytes between the addr and the page boundary
        leading_bytes = page_size - (addr % page_size);
        // if there are more bytes available in the page than len, use len
        if (leading_bytes > len)
            leading_bytes = len;
        trailing_bytes = len - leading_bytes;
        pages = trailing_bytes / page_size;
        trailing_bytes = trailing_bytes % page_size;
        sflash_write_block(addr, buffer, leading_bytes);
        addr += leading_bytes;
        buffer += leading_bytes;
        while (pages--)
        {
            sflash_write_block(addr, buffer, page_size);
            addr += page_size;
            buffer += page_size;
        }
        sflash_write_block(addr, buffer, trailing_bytes);
    }
    else
    {
        // for SST's byte programming single byte mode
        // As of 2011-06-15
        // SST parts are not consistent with AAI commands. Some parts use 0xAF
        // while others use 0xAD. Furthermore, the number of cycles for AAI commands
        // are different for different families of SST parts - some write one byte
        // at one time while others expect 2 or more bytes at a time.
        // See SST25WF512 / SST25WF010 / SST25WF020 / SST25WF040 vs
        // SST25VF512A and SST25VF010A
        // All parts however support identical byte write commands. Since byte
        // write is the lowest common write function amongst all these
        // we will use byte write instead of AAI. This will make writes slower than
        // before for SST SPI serial flash parts but since writes are relatively
        // infrequent, this may be OK in the interest of having support for
        // a larger number of SPI flash parts.
        // This however may change in the future because SST is expected to
        // have "commands identical to other vendors". I am not sure how similar to
        // other vendors' parts though. Since this write byte is still like the
        // page program mechanisms of other parts (MXIC, Numonyx etc), this may
        // still work, although less optimal than it currently is already.
        UINT32 i;
        UINT8 data[SF_BYTE_WRITE_MAX_SIZE];
        data[0] =  SF_WRITE;
        for(i = 0; i < len; i++)
        {
            sflash_write_enable();      // STM, ATML, or MXIC use WREN
            data[1] = ((UINT8*)&addr)[2];
            data[2] = ((UINT8*)&addr)[1];
            data[3] = ((UINT8*)&addr)[0];
            data[4] = *buffer++;
            addr++;
            // Check status after a few uS - will likely succeed the first time
            while(!(sflash_read_status() & SF_STAT_WEL));
            spi_write(spi_port, data, SF_BYTE_WRITE_MAX_SIZE, SF_BYTE_WRITE_MAX_SIZE, TRUE, TRUE );
            while (sflash_read_status() & SF_STAT_BUSY);
        }
        sflash_write_disable();
    }

    if ( g_foundation_config_serialFlashWriteProtect.SW_WP_Enable ) // SW WP or (SW + HW) WP
    {
        sflash_set_write_protect_on();
    }
    sflash_enter_deep_power_down();
    return len;
}

UINT32 sflash_write(UINT32 addr, UINT8 *buffer, UINT32 len)
{
   UINT32 address =addr;
   UINT8 *b_ptr = buffer;
   UINT32 length = len;
   UINT32 ret = 0;

    if (!sf_status.detected)
        return 0;
    if (len == 0)
        return 0;

    while ( length > SF_WRITE_SIZE ) {
      ret += sflash_write_split(address, b_ptr, SF_WRITE_SIZE);
      address = address + SF_WRITE_SIZE;
      b_ptr = b_ptr + SF_WRITE_SIZE;
      length = length - SF_WRITE_SIZE;
    }

    ret += sflash_write_split(address, b_ptr, length);

    return ret;
}

void sflash_write_disable(void)
{
    UINT8 cmd = SF_WRDI;
    spi_write(spi_port, &cmd, sizeof(cmd), sizeof(cmd), TRUE, TRUE );
}

wiced_bool_t sflash_erase(UINT32 addr, UINT32 len)
{

    UINT32 sector_start_addr, current_sector_start_addr, last_sector_start_addr;
    UINT32 current_sector_freebytes, last_sector_bytes;
    UINT32 sectors;
    UINT32 remaining_bytes, current_sector_bytes;
    UINT32 sector_size = SF_SECTOR_SIZE_4K;
    UINT32 temp;

    if (len == 0)
        return FALSE;
    if ((addr + len) > SF_FLASH_SIZE)
        return FALSE;

    if ((addr % sector_size) == 0)
    {
        if (len < sector_size) {
            current_sector_bytes = len;
            current_sector_start_addr = addr;
            sectors = 0;
            sector_start_addr = 0;
            last_sector_bytes = 0;
            last_sector_start_addr = 0;
        } else {
            current_sector_bytes = 0;
            current_sector_start_addr = 0;
            sectors = len / sector_size;
            sector_start_addr = addr;
            last_sector_bytes = len % sector_size;
            last_sector_start_addr = addr + (sectors * sector_size);
        }
    }
    else {
        current_sector_freebytes = sector_size - (addr % sector_size);
        if (len <= current_sector_freebytes)
        {
            current_sector_start_addr = addr;
            current_sector_bytes = len;

            sectors = 0;
            sector_start_addr = 0;

            last_sector_bytes = 0;
            last_sector_start_addr = 0;

        } else {
            remaining_bytes = len - current_sector_freebytes;

            current_sector_bytes = current_sector_freebytes;
            current_sector_start_addr = addr;

            sectors = remaining_bytes / sector_size;
            sector_start_addr  = addr + current_sector_freebytes;

            last_sector_bytes = remaining_bytes % sector_size;
            last_sector_start_addr = addr + current_sector_freebytes + (sectors * sector_size);
        }
    }
// Needed for debugging
#if 0
    printf("%s: [current_sector_start_addr %d, current_sector_bytes %d]  [sector_start_addr %d, sectors %d] [last_sector_start_addr %d, last_sector_bytes %d]\n",__func__,
         (int) current_sector_start_addr, (int)current_sector_bytes, (int) sector_start_addr,  (int)sectors, (int) last_sector_start_addr, (int)last_sector_bytes);
#endif
    if (current_sector_bytes) {
        temp = (current_sector_start_addr/sector_size)*sector_size;
        memset(backup_buffer,0,sector_size);
        sflash_read(temp, backup_buffer, sector_size);
#if 0 // Needed for debugging
        if (verify_data)
        {
            for (i = 0; i < (int)sector_size; i++)
                printf("%d, ",(int)backup_buffer[i]);
        }
#endif
        memset(backup_buffer + (current_sector_start_addr - temp), 0xff, current_sector_bytes);
        if (!sflash_erase_split(temp, sector_size))
            return FALSE;
        sflash_write(temp, backup_buffer, sector_size);
#if 0 // needed for debugging
        if (verify_data)
        {
            memset(backup_buffer,0,sector_size);
            printf("\n\n\n\n%s: Reading data after erasing\n",__func__);
            sflash_read(temp, backup_buffer, sector_size);
            printf("%s: Read data completed after erasing \n",__func__);
            for (i = 0; i < (int)sector_size; i++)
                printf("%d, ",backup_buffer[i]);
        }
#endif
    }

    if (sectors) {
        if (!sflash_erase_split(sector_start_addr, sector_size * sectors))
            return FALSE;
    }

    if (last_sector_bytes) {
        temp = (last_sector_start_addr/sector_size)*sector_size;
        sflash_read(temp, backup_buffer, sector_size);
        memset(backup_buffer + (last_sector_start_addr - temp), 0xff, last_sector_bytes);
        if (!sflash_erase_split(temp, sector_size))
            return FALSE;
        sflash_write(temp, backup_buffer, sector_size);
    }
    return TRUE;
}

BOOL32 sflash_erase_split(UINT32 addr, UINT32 len)
{
    if (!sf_status.detected && !sflash_detected())
        return FALSE;

    sflash_exit_deep_power_down(TRUE);

    if ( g_foundation_config_serialFlashWriteProtect.SW_WP_Enable ) // SW WP or (SW + HW) WP
    {
        sflash_set_write_protect_off();
    }

    sflash_write_enable();

    if(len <= SF_BLOCK_SIZE_64K)
    {
        sflash_sector_erase(addr, len);
    }
    else
    {
        sflash_chip_erase();
    }

    while (sflash_read_status() & SF_STAT_BUSY)
        ;

    if ( g_foundation_config_serialFlashWriteProtect.SW_WP_Enable ) // SW WP or (SW + HW) WP
    {
        sflash_set_write_protect_on();
    }

    sflash_write_disable();

    sflash_enter_deep_power_down();

    return TRUE;
}

void sflash_write_status(UINT8 value)
{
    UINT8 data[2];
    if (sf_attr.attr.use_ewsr)
    {
        sflash_write_status_enable();
    }
    else
    {
        sflash_write_enable();
        while(!(sflash_read_status() & SF_STAT_WEL))
            ;
    }
    if (sf_attr.attr.poll_when_wrsr)
    {
        while (sflash_read_status() & SF_STAT_BUSY)
            ;
    }
    data[0] = SF_WRSR;
    data[1] = value;
    spi_write(spi_port, data, sizeof(data), sizeof(data), TRUE, TRUE );
    if (sf_attr.attr.poll_when_wrsr)
    {
        while (sflash_read_status() & SF_STAT_BUSY)
            ;
    }
}

void sflash_write_block(UINT32 addr, IN UINT8 *buffer, UINT32 len)
{
    UINT32 cmd;
    if (len == 0)
        return;
    sflash_write_enable();
    cmd = (UINT32)(((UINT8*)&addr)[0] << 24 | ((UINT8*)&addr)[1] << 16 |
          ((UINT8*)&addr)[2] <<  8 | sf_command_table[sf_status.sf_index].pw);//SF_WRITE;
    spi_write(spi_port, (UINT8*)&cmd, len + sizeof(cmd), sizeof(cmd),  TRUE, FALSE);
    spi_write(spi_port, buffer, len + sizeof(cmd), len, FALSE, TRUE);
    sflash_wait_time_between_program();
}

void sflash_wait_time_between_program(void)
{
    if (sf_attr.attr.poll_btw_prog)
    {
        while (sflash_read_status() & SF_STAT_BUSY)
            ;
    }
    else
    {
        // wait 18 us +/- 2us
        clock_DelayMicroseconds(18);
    }
}


void sflash_sector_erase(UINT32 addr, UINT32 len)
{
    UINT8 buffer[4];

    if(len <= SF_SECTOR_SIZE_4K)
    {
        buffer[0] = sf_command_table[sf_status.sf_index].sse;
    }
    else if(len <= SF_SECTOR_SIZE_32K)
    {
        buffer[0] = sf_command_table[sf_status.sf_index].se;
    }
    else if(len <= SF_BLOCK_SIZE_64K)
    {
        buffer[0] = sf_command_table[sf_status.sf_index].be;
    }

    buffer[3] = addr & 0xFF;
    buffer[2] = (addr >> 8) & 0xFF;
    buffer[1] = (addr >> 16) & 0xFF;

    spi_write(spi_port, buffer, sizeof(buffer), sizeof(buffer), TRUE, TRUE);
}

void sflash_set_write_protect_on(void)
{
    // SW write protect mode only
    // Enable the Software Protected Mode ( SPM ) feature by writing to the Block Protect bits
    // The sflash_set_protect_level() function shifts the bits into position
    sflash_set_protect_level( SF_PROTECT_ALL );
}


void sflash_chip_erase(void)
{
    UINT8 cmd = sf_command_table[sf_status.sf_index].ce;
    spi_write(spi_port, &cmd, sizeof(cmd), sizeof(cmd), TRUE, TRUE );
}
