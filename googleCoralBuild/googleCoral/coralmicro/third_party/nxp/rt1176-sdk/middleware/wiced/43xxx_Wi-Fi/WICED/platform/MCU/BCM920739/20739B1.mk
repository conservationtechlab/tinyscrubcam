#
# Copyright 2021, Cypress Semiconductor Corporation or a subsidiary of 
 # Cypress Semiconductor Corporation. All Rights Reserved.
 # This software, including source code, documentation and related
 # materials ("Software"), is owned by Cypress Semiconductor Corporation
 # or one of its subsidiaries ("Cypress") and is protected by and subject to
 # worldwide patent protection (United States and foreign),
 # United States copyright laws and international treaty provisions.
 # Therefore, you may use this Software only as provided in the license
 # agreement accompanying the software package from which you
 # obtained this Software ("EULA").
 # If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 # non-transferable license to copy, modify, and compile the Software
 # source code solely for use in connection with Cypress's
 # integrated circuit products. Any reproduction, modification, translation,
 # compilation, or representation of this Software except as specified
 # above is prohibited without the express written permission of Cypress.
 #
 # Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 # EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 # WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 # reserves the right to make changes to the Software without notice. Cypress
 # does not assume any liability arising out of the application or use of the
 # Software or any product or circuit described in the Software. Cypress does
 # not authorize its products for use in any products where a malfunction or
 # failure of the Cypress product may reasonably be expected to result in
 # significant property damage, injury or death ("High Risk Product"). By
 # including Cypress's product in a High Risk Product, the manufacturer
 # of such system or application assumes all risk of such use and in doing
 # so agrees to indemnify Cypress against all liability.
#


# Define the base chip family name
BASE_NAME = 20739

# Define patch entries
NUM_PATCH_ENTRIES = 256
PATCH_ENTRY_SIZE  = 4

# Define 20739 common limits
# Begin address of ROM
IROM_BEGIN_ADDR		= 0x00000000
# Available ROM = 2M
IROM_LENGTH			= 0x00200000
# Begin address of RAM
SRAM_BEGIN_ADDR		= 0x00200000
# Available RAM = 448K
SRAM_LENGTH			= 0x00070000
#Begin Address of PATCH ROM
PROM_BEGIN_ADDR		= 0x00270400
#Begin Address of PATCH ROM
PROM_LENGTH			= 0x0000FC00
# Length of patch table = 1024 bytes
PATCH_TABLE_LENGTH	= ($(NUM_PATCH_ENTRIES) * $(PATCH_ENTRY_SIZE))
# Begin address of AON.
AON_BEGIN_ADDR      = 0x280000
# Length of AON
AON_LENGTH          = 0x4000
# Begin address of flash0
FLASH0_BEGIN_ADDR	= 0x00500000
# Available flash = 1M
FLASH0_LENGTH		= 0x00100000
# Available AON size for application 256B
AON_FOR_APP_MAX_LEN = 0x00000100
ifeq (1, $(OTA2_SUPPORT))
ifeq ($(XIP_SUPPORT),1)
# Available OTA application size 56K
SRAM_OTA_APP_MAX_LEN = 0x0000E000
else
# Available OTA application size 132K
SRAM_OTA_APP_MAX_LEN = 0x00021000
endif
# Available OTA bootloader size 16K
SRAM_OTA_BOOTLD_MAX_LEN = 0x00004000
endif
ifeq (1, $(OTA2_SUPPORT))
# OCF DCT START ADDR.
# This value should be aligned with OTA2_IMAGE_CURR_DCT_1_AREA_BASE in ota2_image_defines.mk
DCT_OCF_START_ADDR	= 0x00537000
else
# OCF DCT START ADDR
DCT_OCF_START_ADDR	= 0x0053D000
endif
# OCF FILE SYSTEM START ADDR.
ifeq (1, $(OTA2_SUPPORT))
# This value should be aligned with OTA2_IMAGE_FS_DATA_AREA_BASE in ota2_image_defines.mk
FILESYSTEM_OCF_START_ADDR	= 0x00546000
else
FILESYSTEM_OCF_START_ADDR	= 0x00545000
endif
ifeq (1, $(OTA2_SUPPORT))
# This value should be aligned with OTA2_IMAGE_FS_DATA_AREA_SIZE in ota2_image_defines.mk
# MAX FILESYSTEM SIZE 424K (0x0006A000 Bytes)
FILSYSTEM_OCF_MAX_SIZE	= 0x6A000
else
ifeq ($(XIP_SUPPORT),1)
# MAX FILESYSTEM SIZE 512KB
FILSYSTEM_OCF_MAX_SIZE	= 0x80000
else
# MAX FILESYSTEM SIZE 747K (765952 Bytes)
FILSYSTEM_OCF_MAX_SIZE	= 0xBB000
endif
endif

ifeq ($(XIP_SUPPORT),1)
# OCF exe code start address
XIP_OCF_START_ADDR       = 0x005CD000
# OCF exe code max size 236KB
XIP_OCF_MAX_SIZE       = 0x32FFF
endif

FILESYSTEM_OCF_START_OFFSET = FILESYSTEM_OCF_START_ADDR - FLASH0_BEGIN_ADDR
FILESYSTEM_OCF_END_OFFSET = FILESYSTEM_OCF_START_OFFSET + FILSYSTEM_OCF_MAX_SIZE
# This is a cortex m4
CHIP_CORE           = cortex-m4
# Core specific options for this chip
CORE_COMMON_OPTIONS = -mcpu=$(CHIP_CORE) -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb -mlittle-endian
