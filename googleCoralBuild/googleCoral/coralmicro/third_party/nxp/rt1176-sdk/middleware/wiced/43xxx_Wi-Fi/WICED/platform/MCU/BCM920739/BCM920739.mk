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

NAME = BCM920739

# Host architecture is ARM Cortex M4
HOST_ARCH := ARM_CM4

HEX_OUTPUT_SUFFIX := .hex
CGS_OUTPUT_SUFFIX := .cgs
SPAR_CRT_SETUP := $(notdir $(APP))_spar_crt_setup
SPAR_APP_SETUP := application_setup
GLOBAL_DEFINES += SPAR_CRT_SETUP=$(SPAR_CRT_SETUP)
GLOBAL_DEFINES += SPAR_APP_SETUP=$(SPAR_APP_SETUP)
ifneq (1, $(OTA2_SUPPORT))
NO_BUILD_BOOTLOADER:=1
NO_BOOTLOADER_REQUIRED:=1
endif
# Host MCU alias for OpenOCD
HOST_OPENOCD := BCM920739
# Only ThredX RTOS and NetX_Duo supported
VALID_OSNS_COMBOS := ThreadX-NetX_Duo

include WICED/platform/MCU/$(HOST_OPENOCD)/$(BT_CHIP)$(BT_CHIP_REVISION).mk

USE_LIBC_PRINTF ?= 1
USE_PLATFORM_ALLOCATED_POOL ?= 1
USE_JLINK_FOR_TRACES ?= 0

GLOBAL_INCLUDES += . \
                   .. \
                   ../.. \
                   ../../include \
                   ../../$(HOST_ARCH) \
                   ../../$(HOST_ARCH)/CMSIS \
                   peripherals \
                      ../../../../libraries/utilities/ring_buffer \
                   ./inc/$(BT_CHIP_REVISION) \
                   WAF \
                   WWD \
                   ../../../../libraries/drivers/spi_flash

GLOBAL_INCLUDES  += inc/$(BT_CHIP_REVISION)/stack/include_wiced
GLOBAL_INCLUDES  += inc/$(BT_CHIP_REVISION)/stack/include
GLOBAL_INCLUDES  += inc/$(BT_CHIP_REVISION)/stack

GLOBAL_INCLUDES +=  inc/$(BT_CHIP_REVISION)/hal \
                    inc/$(BT_CHIP_REVISION)/internal \
                    inc/$(BT_CHIP_REVISION)/internal/chip \


# Global defines
GLOBAL_DEFINES  := USE_STDPERIPH_DRIVER \
                   _$(HOST_MCU_PART_NUMBER)_ \
                   CPU_CM4 \

           __CORE__=__ARM7EM__ \
                   __FPU_PRESENT=0 \
                   __FPU_USED=0

ifneq ($(TOOLCHAIN_NAME),IAR)
GLOBAL_DEFINES    += __ARM7EM__ \
                  __VER__=5000000
endif

GLOBAL_DEFINES   += WICED_PAYLOAD_MTU=1500

#ThreadX defines when debug build is enabled
ifeq ($(BUILD_TYPE),debug)
include $(SOURCE_ROOT)WICED/platform/MCU/BCM920739/ThreadX_Debug.mk
endif


ifeq ($(TOOLCHAIN_NAME),GCC)
GLOBAL_CXXFLAGS += $$(CPU_CXXFLAGS)  $$(ENDIAN_CXXFLAGS_LITTLE)
GLOBAL_ASMFLAGS += $$(CPU_ASMFLAGS)  $$(ENDIAN_ASMFLAGS_LITTLE)
GLOBAL_CFLAGS   += $$(CPU_CFLAGS)    $$(ENDIAN_CFLAGS_LITTLE)
GLOBAL_CFLAGS   += -DSPAR_CRT_SETUP=$(APP)_spar_crt_setup -DSPAR_APP_SETUP=$(SPAR_APP_SETUP) -DUSE_BOOL_UINT32
GLOBAL_CFLAGS   += -Dsprintf=__2sprintf -Dsnprintf=__2snprintf

ifeq (1, $(XIP_SUPPORT))
GLOBAL_CFLAGS   += -mlong-calls
endif

ifneq ($(USE_LIBC_PRINTF),1)
GLOBAL_CFLAGS += -Dprintf=WICED_BT_TRACE
endif
GLOBAL_DEFINES += PLATFORM_TRACE
endif
ifeq ($(USE_PLATFORM_ALLOCATED_POOL),1)
GLOBAL_DEFINES += WICED_USE_PLATFORM_ALLOCATED_POOL
endif

ifeq ($(USE_JLINK_FOR_TRACES),1)
GLOBAL_CFLAGS += -DENABLE_JLINK_TRACE
endif

GLOBAL_DEFINES += NO_SUPPORT_FOR_HIGHSPEED_MODE

ifeq (1, $(OTA2_SUPPORT))
SECTOR_NUMBER_SCRIPT := $(TOOLS_ROOT)/text_to_c/sector_number.pl
SECTOR_COUNT_SCRIPT  := $(TOOLS_ROOT)/text_to_c/sector_count.pl

include platforms/$(subst .,/,$(PLATFORM))/ota2_image_defines.mk
#OTA2_IMAGE_CURR_LUT_AREA_BASE
APPS_LUT_HEADER_LOC := $(OTA2_IMAGE_CURR_LUT_AREA_BASE)
# The start of the Filesystem, actually
APPS_START_SECTOR := $(shell $(PERL) $(SECTOR_NUMBER_SCRIPT) $(OTA2_IMAGE_CURR_FS_AREA_BASE) 4096)

## Testing
SECURE_FS_IMAGE :=

ifeq (1,$(ota2_xip))
GLOBAL_DEFINES		+= OTA2_XIP_SUPPORT
endif

endif

# Components
ifneq ($(TOOLCHAIN_NAME),)
ifeq ($(TOOLCHAIN_NAME),GCC)
#libc related platform specific implementation
$(NAME)_COMPONENTS += MCU/BCM920739/GCC_libc
else
#platform common GCC libc related implementation
$(NAME)_COMPONENTS += $(TOOLCHAIN_NAME)
endif
endif

$(NAME)_COMPONENTS += MCU/BCM920739/peripherals
$(NAME)_COMPONENTS += inputs/gpio_button
$(NAME)_COMPONENTS += utilities/ring_buffer
ifeq ($(USE_JLINK_FOR_TRACES),1)
$(NAME)_COMPONENTS += utilities/RTT_Logger
endif

ifdef PLATFORM_SUPPORTS_SPI_FLASH
GLOBAL_DEFINES += WICED_PLATFORM_INCLUDES_SPI_FLASH
$(NAME)_COMPONENTS += MCU/BCM920739/peripherals/spi_flash
endif

ifdef PLATFORM_FILESYSTEM_FILEX
GLOBAL_DEFINES += FILEX_FIX
endif

# Source files
$(NAME)_SOURCES := ../wiced_platform_common.c \
                  platform_init.c \
                  platform_bt_cfg.c \
                  spar_setup.c \
                   ../platform_stdio.c \
                ../platform_resource.c \
                  ../wwd_resources.c \
                  ../platform_button.c \
                 DCT/wiced_dct_ocf.c \
                 ../wiced_dct_update.c \
                 WAF/waf_platform.c \
                 WAF/wiced_waf_common.c

$(NAME)_SOURCES +=  platform_deep_sleep.c

#Enable this MACRO for SDS sleep mode
GLOBAL_DEFINES += PLATFORM_SUPPORTS_LOW_POWER_MODES
GLOBAL_DEFINES += MCU_BCM920739

ifdef	POWER_ESTIMATOR
$(NAME)_COMPONENTS += MCU/BCM920739/WPL
endif

ifdef PLATFORM_SUPPORTS_FILESYSTEM
    GLOBAL_DEFINES += WICED_FILESYSTEM_SUPPORT
    ifeq ($(PLATFORM_SUPPORTS_RESSOURCE_GENFS),1)
        GLOBAL_DEFINES += USES_RESOURCE_GENERIC_FILESYSTEM
    endif

    ifdef PLATFORM_SUPPORTS_SPI_FLASH
        $(NAME)_SOURCES += platform_filesystem.c
    else
        ifdef PLATFORM_SUPPORTS_OC_FLASH
            GLOBAL_DEFINES += WICED_PLATFORM_INCLUDES_OCF_FS
            ifeq (1, $(OTA2_SUPPORT))
                GLOBAL_DEFINES += WICED_FILESYSTEM_OCF_START=$(OTA2_IMAGE_FS_DATA_AREA_BASE)
                GLOBAL_DEFINES += WICED_FILESYSTEM_OCF_SIZE=$(OTA2_IMAGE_FS_DATA_AREA_SIZE)
            else
                GLOBAL_DEFINES += WICED_FILESYSTEM_OCF_START=0x45000
                ifeq (1, $(XIP_SUPPORT))
                    GLOBAL_DEFINES += WICED_FILESYSTEM_OCF_SIZE=0x80000
                else
                    GLOBAL_DEFINES += WICED_FILESYSTEM_OCF_SIZE=0xBB000
                endif #XIP_SUPPORT
            endif #OTA2_SUPPORT

            GLOBAL_DEFINES += WICED_FILESYSTEM_OCF_END=WICED_FILESYSTEM_OCF_START+WICED_FILESYSTEM_OCF_SIZE
            $(NAME)_SOURCES += platform_filesystem_ocf.c

        endif #PLATFORM_SUPPORTS_OC_FLASH
    endif #PLATFORM_SUPPORTS_SPI_FLASH
endif #PLATFORM_SUPPORTS_FILESYSTEM

ifeq (1, $(OTA2_SUPPORT))
GLOBAL_DEFINES += OTA2_BOOTLD_MAX_LEN=$(SRAM_OTA_BOOTLD_MAX_LEN)
GLOBAL_DEFINES += OTA2_APP_MAX_LEN=$(SRAM_OTA_APP_MAX_LEN)
GLOBAL_DEFINES += OTA2_AON_FOR_APP_MAX_LEN=$(AON_FOR_APP_MAX_LEN)
endif

$(NAME)_SOURCES += threadx_stub.c

ifeq ($(WICED_NETTYPE),ROM)
$(NAME)_SOURCES += network/Netx_Duo_5.7/nxd_rom_port.c
endif

ifneq ($(WLAN_CHIP),NONE)
$(NAME)_SOURCES += WWD/wwd_platform.c \
                 WWD/wwd_$(BUS).c
endif

#for DCT with crc checking
$(NAME)_COMPONENTS  += utilities/crc
# include the ota2 specific functions
ifeq (1, $(OTA2_SUPPORT))
$(NAME)_SOURCES += DCT/wiced_dct_ocf_ota2.c
$(NAME)_COMPONENTS += filesystems/ota2
endif

$(NAME)_CFLAGS = $(COMPILER_SPECIFIC_PEDANTIC_CFLAGS)
$(NAME)_CFLAGS += -Wno-unused-function

# Add maximum and default watchdog timeouts to definitions. Warning: Do not change MAX_WATCHDOG_TIMEOUT_SECONDS
MAX_WATCHDOG_TIMEOUT_SECONDS = 22
GLOBAL_DEFINES += MAX_WATCHDOG_TIMEOUT_SECONDS=$(MAX_WATCHDOG_TIMEOUT_SECONDS)

# DCT linker script
DCT_LINK_SCRIPT += $(TOOLCHAIN_NAME)/$(BT_CHIP_REVISION)/dct$(LINK_SCRIPT_SUFFIX)
GLOBAL_DEFINES += USE_BOOL_UINT32

ifeq (1, $(SECURE_SFLASH))
GLOBAL_DEFINES += PLATFORM_SECURESFLASH_ENABLED=1

else
GLOBAL_DEFINES += PLATFORM_SECURESFLASH_ENABLED=0
endif

BASE_IN                           ?= rom
SPAR_IN                           ?= ram
GLOBAL_LINK_SCRIPT := $(OUTPUT_DIR)/$(PLATFORM)_$(SPAR_IN)_proc.ld
USES_BOOTLOADER_OTA:=0
GLOBAL_DEFINES      += WICED_DISABLE_BOOTLOADER

# EXTRA_PLATFORM_MAKEFILES is called from wiced_elf.mk
EXTRA_PLATFORM_MAKEFILES +=  $(SOURCE_ROOT)/WICED/platform/MCU/$(HOST_MCU_FAMILY)/makefiles/wiced_cgs.mk
EXTRA_PLATFORM_MAKEFILES +=  $(SOURCE_ROOT)/WICED/platform/MCU/$(HOST_MCU_FAMILY)/BCM920739_targets.mk
