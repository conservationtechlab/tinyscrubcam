/*
* $ Copyright Broadcom Corporation $
*/

/** @file
*
* WICED Firmware Upgrade 
*
* This file provides definitions and function prototypes for Common functionality used in 
* WICED Smart Ready Upgrade
*
*/

/*****************************************************************************/
/** @defgroup wiced_Firmware_upgrade   Wiced Firmware Ready Upgrade
 *
 *  WICED Smart Ready Upgrade functionality is used to store and retrieve data from
 *  the memory being upgraded. 
 *
 */
/*****************************************************************************/

#ifndef WICED_FIRMWARE_UPGRADE_H
#define WICED_FIRMWARE_UPGRADE_H

/********************************************************************************************************
* Recommended firmware upgrade 4 MBit Serila flash offsets
 * -------------------------------------------------------------------------------------------------------------------
 * |  SS1 (4K @ 0)  |  Fail safe area(4K @ 0x1000)  |  VS1 (4K @ 0x2000)  | VS2 (4K @ 0x3000)  | DS1 (248K @ 0x4000)  | DS2 (248K @ 0x42000)
 *  -------------------------------------------------------------------------------------------------------------------
 * For reference only.
 * uint32_t ss_locations       =    0x0000;
 * uint32_t vs_location1          = 0x2000;     // VS section occupies 1 sector
 * uint32_t vs_length1            = 0x1000;     // 4K = 1 SF sector
 * uint32_t vs_location2          = 0x3000;     // Double buffer for VS
 * uint32_t vs_length2            = 0x1000;     // 4K = 1 SF sector = vs_length1
 * uint32_t ds1_location          = 0x4000
 * uint32_t ds1_length            = 0x3E000     // 240K
 * uint32_t ds2_location          = 0x42000
 * uint32_t ds2_length            = 0x3E000     // 240K = ds1 length
 *******************************************************************************************************/
typedef struct
{
    uint32_t ss_loc;    /**< static section location */
    uint32_t ds1_loc;   /**< ds1 location */
    uint32_t ds1_len;   /**< ds1 length */
    uint32_t ds2_loc;   /**< ds2 location */
    uint32_t ds2_len;   /**< ds2 length */
    uint32_t vs1_loc;   /**< vendor specific location 1 */
    uint32_t vs1_len;   /**< vendor specific location 1 length */
    uint32_t vs2_loc;   /**< vendor specific location 2 */
    uint32_t vs2_len;   /**< vendor specific location 2 length */
} wiced_fw_upgrade_nv_loc_len_t;

/**
* \brief Initialize WICED Firmware Upgrade module
* \ingroup wiced_firmware_upgrade
*
* \details This function is typically called by the application during initialization
* to initialize upgrade module with serila flash offsets
*
* \param p_sflash_nv_loc_len Offsets of different sections present in serial flash
* \param sflash_size   Serila flash size present on the tag board.(default size 4MB )
*
*/
wiced_bool_t     wiced_firmware_upgrade_init(wiced_fw_upgrade_nv_loc_len_t *p_sflash_nv_loc_len, uint32_t sflash_size);

/**
* \brief Initialize NV locations
* \ingroup wiced_firmware_upgrade
*
* \details Application calls this function during the start of the firmware download
* to setup memory locations depending on which partition is being used 
*
*/
uint32_t     wiced_firmware_upgrade_init_nv_locations(void);

/**
* \brief Store memory chunk to memory
* \ingroup wiced_firmware_upgrade
*
* \details Application can call this function to store the next memory chunk in the
* none volatile memory.  Application does not need to know which type of memory is
* used or which partition is being upgraded.
*
* \param offset Offset in the memory where data need to be stored
* \param data   Pointer to the chunk of data to be stored
* \param len    Size of the memory chunk that need to be stored
*
*/
uint32_t   wiced_firmware_upgrade_store_to_nv(uint32_t offset, uint8_t *data, uint32_t len);

/**
* \brief Retrieve memory chunk from memory
* \ingroup wiced_firmware_upgrade
*
* \details Application typically calls this function when the upgrade process has
* been completed to verify that the data has been successfully stored.  Application
* does not need to know which type of memory is used or which partition is being upgraded.
*
* \param offset Offset in the memory from where data need to be retrieved
* \param data   Pointer to the location to retrieve the data
* \param len    Size of the memory chunk to be retrieved
*
*/
uint32_t   wiced_firmware_upgrade_retrieve_from_nv(uint32_t offset, uint8_t *data, uint32_t len);

/**
* \brief Retrieve memory chunk from memory
* \ingroup wiced_firmware_upgrade
*
* \details After download is completed and verified this function is called to
* switch active partitions with the one that has been receiving the new image.
*
*/
void     wiced_firmware_upgrade_finish(void);

#endif
