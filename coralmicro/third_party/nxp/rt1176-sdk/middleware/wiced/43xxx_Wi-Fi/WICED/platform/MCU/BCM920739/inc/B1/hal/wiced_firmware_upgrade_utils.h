/*
* $ Copyright Broadcom Corporation $
*/

/** @file
*
* WICED Firmware Upgrade Utils 
*
* This file provides prototypes of utility functions that are used in 
* WICED Smart Ready Upgrade
*
*/

#ifndef WICED_FIRMWARE_UPGRADE_UTILS_H
#define WICED_FIRMWARE_UPGRADE_UTILS_H

//constants used in CRC calculation
#define WICED_FW_UPGRADE_POLYNOMIAL              0x04C11DB7
#define WICED_FW_UPGRADE_WIDTH                   (8 * sizeof(UINT32))
#define WICED_FW_UPGRADE_TOPBIT                  (1 << (WICED_FW_UPGRADE_WIDTH - 1))
#define WICED_FW_UPGRADE_INITIAL_REMAINDER       0xFFFFFFFF
#define WICED_FW_UPGRADE_FINAL_XOR_VALUE         0xFFFFFFFF
#define WICED_FW_UPGRADE_REFLECT_DATA(X)         ((UINT8) wiced_firmware_upgrade_utils_reflect((X), 8))
#define WICED_FW_UPGRADE_REFLECT_REMAINDER(X)    ((UINT32) wiced_firmware_upgrade_utils_reflect((X), WICED_FW_UPGRADE_WIDTH))
#define WICED_FW_UPGRADE_CHECK_VALUE             0xCBF43926

#endif
