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
#ifndef _SERIAL_FLASH_PRIV_H_
#define _SERIAL_FLASH_PRIV_H_

#include "brcm_fw_types.h"

#ifdef __cplusplus
extern "C"
{
#endif
/******************************************************************************
* Type Definitions
******************************************************************************/
#pragma pack(1)
typedef struct
{
    UINT8 detected : 1;     // 1, if serial flash found
    UINT8 got_reset : 1;    // 1, if spi master hardware was reset
    UINT8 dont_pwr_dn : 1;  // 1, dont deep power down even if it's available
    UINT8 pwr_is_dn : 1;    // 1, if currently deep power down
    UINT8 pwr_dn_enable : 1; // 1, if deep power down enabled
    UINT8 use_default : 1;    // 1, if default command set is used
    UINT8 reserved : 2;
    UINT8 sf_index;         // index in sf_mem_table
    INT16 delay_cnt;     // delay count for Resume from Deep Power-down. 4 instructions per loop
} SF_STATUS;

#pragma pack(1)
typedef struct
{
    UINT16 enable_dspi : 1;                      // Enable reading two bytes at a time
    UINT16 use_adesto_deep_sleep : 1;            // Enable Adesto Deep Sleep
    UINT16 enable_vs_ram_cach : 1;               // Enable VS section RAM Cache.  Need to implement in RAM.
    UINT16 micro_bcs_send_deep_sleep : 1;        // In case hardware glitch wakes sfi at power on from HID OFF
                                                 //  send deep sleep command in micro BCS if full boot is not required
    UINT16 sleep_when_bt_transport_detected : 1; // Put SF to sleep when BT Transport is detected
    UINT16 reserved : 11;                        // Reserved
} SF_STATUS_EXTENDED;



typedef union
{
    UINT8 byte;
    struct
    {
        UINT8 extra_wrsr : 1;       // workaround for atmel serial flash
        UINT8 page_size_128 : 1; // ATMEL AT25F512 page_size=128, other ATMEL and MXIC, STM page_size=256
        UINT8 poll_btw_prog : 1;    // 1, poll status 0, fixed delay
        UINT8 poll_when_wrsr : 1;   // 1, if need to be conservative
        UINT8 use_ewsr : 1;         // 1, if need ewsr before executing wrsr
        UINT8 fast_read : 1;        // 1, if fast read supported
        UINT8 page_write : 1;       // 1, if page write supported
        UINT8 deep_pwr_dn : 1;      // 1, if can do deep power down

    } attr;

} SF_ATTRIB;
#pragma pack()

typedef struct
{
    UINT8 sig_len;
    UINT8 maker_id;
    UINT8 mem_type;
    UINT8 mem_cap;
    UINT8 size;                     // real size = 64KB * (size + 1), 0 = 64KB
    UINT8 t_RDP;                  // resume from Deep Power-down, in us.
    SF_ATTRIB mem_attr;

} SF_CFG_TABLE_ENTRY;

typedef struct
{
    UINT8 wren;            //write enable
    UINT8 wrdi;             //write disable
    UINT8 rdid;             //read identification
    UINT8 rdsr;             //read status register
    UINT8 wrsr;            //write status register
    UINT8 read;            //read data byte
    UINT8 fast_read;    //read data byte at higher speed
    UINT8 pw;                //page write
    UINT8 pp;                //page program
    UINT8 aai;               //auto address increment program - SST
    UINT8 ewsr;            //enable write status register - SST
    UINT8 pe;                //page erase
    UINT8 sse;               //sub-sector erase
    UINT8 se;                //sector erase
    UINT8 be;                //block erase
    UINT8 ce;                //chip erase
    UINT8 dp;                //deep power-down
    UINT8 res;               //release from deep power-down
} SF_CFG_COMMAND;

#pragma pack(1)
typedef struct
{
    UINT8    SW_WP_Enable:1;                    // Bit 0
    UINT8    HW_WP_Enable:1;                    // Bit 1
    UINT8    SW_WP_Enable_GPIO:4;                // Bit {5:2}
    UINT8    spare:2;                             // Bit {7:6}
} FOUNDATION_CONFIG_SERIAL_FLASH_WRITE_PROTECT_t;



/*******************************************************************************
* Function Prototypes
*******************************************************************************/
void sflash_exit_deep_power_down(BOOL32 forceExitDeepPowerDown);
UINT32 sflash_check_AON_flag(UINT16 flag);
UINT32 sflash_write_split(UINT32 addr, UINT8 *buffer, UINT32 len);
UINT8 sflash_read_id(UINT8* buffer);
BOOL32 sflash_detected(void);
void sflash_enter_deep_power_down(void);
void sflash_set_protect_level(UINT8 level);
void sflash_write_status(UINT8 value);
void sflash_set_write_protect_on(void);
BOOL32 sflash_erase(UINT32 addr, UINT32 len);
void sflash_sector_erase(UINT32 addr, UINT32 len);
void sflash_chip_erase(void);
void sflash_write_block(UINT32 addr, IN UINT8 *buffer, UINT32 len);
void sflash_write_disable(void);
BOOL32 sflash_erase_split(UINT32 addr, UINT32 len);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif  // _SERIAL_FLASH_PRIV_H_

