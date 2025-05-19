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

#$(info Processing wiced_cgs.mk LINK_OUTPUT_FILE: $(LINK_OUTPUT_FILE))
#$(info LINK_OUTPUT_SUFFIX: $(LINK_OUTPUT_SUFFIX))
#$(LINK_OUTPUT_FILE:.elf=.cgs): $(LINK_OUTPUT_FILE)
#	$(info Processed CGS ... )

default: help

# cgs recipe
.PHONY: package

# Include all toolchain makefiles - one of them will handle the architecture
include $(MAKEFILES_PATH)/wiced_toolchain_$(TOOLCHAIN_NAME).mk
#include $(MAKEFILES_PATH)/wiced_elf.mk

include $(SOURCE_ROOT)WICED/platform/MCU/$(HOST_MCU_FAMILY)/makefiles/tools.mk
include platforms/$(subst .,/,$(PLATFORM))/$(PLATFORM).mk
# Basic environment for SPAR builds
SDK_ROOT_DIR    := ../../..

ELF_OUT := $(LINK_OUTPUT_FILE)
BIN_OUT_DIR = $(OUTPUT_DIR)

PLATFORM_DIR := $(SOURCE_ROOT)platforms/$(PLATFORM)
PATCH_ROOT_DIR  = $(SOURCE_ROOT)WICED/platform/MCU/$(HOST_MCU_FAMILY)
PATCH_DIR       = $(PATCH_ROOT_DIR)/patches/$(BT_CHIP_REVISION)
PATCH_LIBS_DIR  = $(PATCH_ROOT_DIR)/libraries
PATCH_ELF       = $(OUTPUT_DIR)/binary/patch_$(HOST_MCU_FAMILY).elf
ELF_LIST       += $(PATCH_ELF)

ifeq (1, $(OTA2_SUPPORT))
ifeq ($(APP),ota2_btmcu_bootloader)
PATCH_LST		= $(PATCH_DIR)/patch.lst
CGS_LIST        += $(PATCH_DIR)/patch.cgs $(PLATFORM_DIR)/$(PLATFORM_CONFIGS)
else
PATCH_LST       = $(PATCH_DIR)/patch.ota.lst
CGS_LIST        += $(PATCH_DIR)/patch.ota.cgs
endif
else
PATCH_LST		= $(PATCH_DIR)/patch.lst
CGS_LIST        += $(PATCH_DIR)/patch.cgs $(PLATFORM_DIR)/$(PLATFORM_CONFIGS)
endif

PATCH_SYMDEF    = $(PATCH_DIR)/patch.symdefs

ifeq ($(XIP_SUPPORT),1)
OCF := _ocf
else
OCF :=
endif

ifeq ($(BUILD_TYPE),debug)
APP_LD_SCRIPT   = $(PATCH_ROOT_DIR)/GCC/$(BT_CHIP_REVISION)/20739$(BT_CHIP_REVISION)$(OCF)_$(SPAR_IN)_debug.ld
else
APP_LD_SCRIPT   = $(PATCH_ROOT_DIR)/GCC/$(BT_CHIP_REVISION)/20739$(BT_CHIP_REVISION)$(OCF)_$(SPAR_IN).ld
endif

ifdef PLATFORM_SUPPORTS_OC_FLASH
FS_IMAGE           := $(OUTPUT_DIR)/filesystem.bin
STAGING_DIR          := $(OUTPUT_DIR)/resources/Staging/
endif

OTA_APP_BIN	:= $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING).ota.bin

ifdef PLATFORM_SUPPORTS_OC_FLASH
EXTRA_POST_BUILD_TARGETS += $(FS_IMAGE) $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING).hex
else
EXTRA_POST_BUILD_TARGETS += $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING).hex
endif
EXTRA_PRE_BUILD_TARGETS += $(PATCH_ELF)
BROADCOM_INTERNAL :=no

CHIPLOAD_CRC_VERIFY             := -CHECKCRC
CHIPLOAD_NO_READ_VERIFY         := -NOVERIFY

#$(info CGS_LIST: $(CGS_LIST))

# Now add the optional libraries and patches this app needs to the final objs to link in.
#APP_PATCHES_AND_LIBS :=btstack_lib.a
#$(info PATCH_LIBS_DIR: $(PATCH_LIBS_DIR))
#$(info APP_PATCHES_AND_LIBS: $(APP_PATCHES_AND_LIBS))
PATCHES_LIBS += $(addprefix $(PATCH_LIBS_DIR)/,$(APP_PATCHES_AND_LIBS))
#$(info PATCHES_LIBS: $(PATCHES_LIBS))

# Include the platform/chip specific makefile
#include $(PATCH_ROOT_DIR)/$(CHIP)$(REVNUM).mk
include $(PATCH_ROOT_DIR)/20739$(BT_CHIP_REVISION).mk



# Include generic ARM core options
#include $(TC)/make_$(CHIP_CORE)_$(TC).mk
include $(SOURCE_ROOT)WICED/platform/MCU/$(HOST_MCU_FAMILY)/makefiles/gcc/make_cortex-m4_gcc.mk

# Include packaging rules
PACKAGE_OUTPUT_DIR := $(OUTPUT_DIR)
-include $(MAKEFILES_PATH)/wiced_package.mk
PACKAGE_DEVTARGET := "BTHCI"

################################################################################
# CGS generation, foo.elf.text.hex etc. -> spar.cgs
################################################################################
SPAR_CGS_TARGET = $(ELF_OUT:.elf=.cgs)

# Make the spar-setup call CGS element
SPAR_SETUP_CALL_CGS = $(ELF_OUT:.elf=.elf.spar_setup_call.cgs)
# cgs file for each section we need to extract
SECTIONS_CGS_LIST += $(patsubst %, $(ELF_OUT)%.cgs, $(UNCOMPRESSED_SECTIONS))
SECTIONS_CGS_LIST += $(patsubst %, $(ELF_OUT)%.ccgs, $(COMPRESSED_SECTIONS))
SECTIONS_CGS_LIST += $(patsubst %, $(ELF_OUT)%.ocgs, $(OVERLAID_SECTIONS))



# Include defalt platform.cgs from the current directory if
# nothing is specified. This may come in from WICED SDK platform directory.
PLATFORM_CGS_PATH ?= platform.cgs


#$(BUILD_STRING): $(OBJS) $(OUTPUT_DIR)/Modules/lib_installer.o #$(SPAR_CGS_TARGET)
#	$(info CLEANED_BUILD_STRING cgs.mk: $(OBJS))


OUTPUT_NAME:=$(BUILD_STRING)
PLATFORM_BOOTP:= 20739_OCF_$(BT_CHIP_REVISION).btp
PLATFORM_MINIDRIVER  := uart_main_$(BT_CHIP_REVISION).hex
PLATFORM_BAUDRATE  := 115200
# DCT build takes the COMPILER_SPECIFIC_DEBUG_CFLAGS flag even in release build,
# DCT files needs to be built with always "-O0" option, where as other .c files
# should be built with "-Os" option,so when -debug is enabled  in order to fix
# this we have used the order in which the COMPILER_SPECIFIC_DEBUG_CFLAGS and
# COMPILER_SPECIFIC_STANDARD_CFLAGS are used while building the DCT files and
# other .c file in wiced_elf.mk, since the compiler takes more latest -O option
# in the command, so now for DCT it will be "-Os -O0" and for other .c files it
# will be "-O0 -Os" in the command.So we need to take care here when there is some
# compiler flags order change in wiced_elf.mk.
ifeq ($(BUILD_TYPE),debug)
COMPILER_SPECIFIC_DEBUG_CFLAGS += -Os
COMPILER_SPECIFIC_STANDARD_CFLAGS += -O0
endif

# DCT build takes the COMPILER_SPECIFIC_DEBUG_CFLAGS flag even in release build,
# DCT files needs to be built with always "-O0" option, where as other .c files
# should be built with "-Os" option,so when -debug is enabled  in order to fix
# this we have used the order in which the COMPILER_SPECIFIC_DEBUG_CFLAGS and
# COMPILER_SPECIFIC_STANDARD_CFLAGS are used while building the DCT files and
# other .c file in wiced_elf.mk, since the compiler takes more latest -O option
# in the command, so now for DCT it will be "-Os -O0" and for other .c files it
# will be "-O0 -Os" in the command.So we need to take care here when there is some
# compiler flags order change in wiced_elf.mk.
ifeq ($(BUILD_TYPE),debug)
COMPILER_SPECIFIC_DEBUG_CFLAGS += -Os
COMPILER_SPECIFIC_STANDARD_CFLAGS += -O0
endif

#include platforms/$(subst .,/,$(PLATFORM))/$(PLATFORM).mk

$(if $(WLAN_CHIP),,$(error No WLAN_CHIP has been defined))
$(if $(WLAN_CHIP_REVISION),,$(error No WLAN_CHIP_REVISION has been defined))

WIFI_IMAGE_NAME		:= $(WLAN_CHIP)$(WLAN_CHIP_REVISION).bin
DCT_IMAGE_NAME		:= $(OUTPUT_DIR)/DCT.bin
DOWNLOAD_DCT_HEX	:= $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING)_append_dct.hex
DOWNLOAD_APP_HEX    := $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING).hex
ifdef PLATFORM_SUPPORTS_OC_FLASH
DOWNLOAD_FILESYSTEM_HEX	:= $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING)_append_filesystem.hex
endif

ifdef PLATFORM_SUPPORTS_OC_FLASH
DOWNLOAD_FILE_NAME := $(DOWNLOAD_FILESYSTEM_HEX)
else
DOWNLOAD_FILE_NAME := $(DOWNLOAD_DCT_HEX)
endif

ifeq (1, $(OTA2_SUPPORT))
include platforms/$(subst .,/,$(PLATFORM))/ota2_image_defines.mk
SECTOR_NUMBER_SCRIPT := $(TOOLS_ROOT)/text_to_c/sector_number.pl
SFLASH_DCT_LOC                     := $(OTA2_IMAGE_CURR_DCT_1_AREA_BASE)
SFLASH_DCT2_LOC                   := $(OTA2_IMAGE_CURR_DCT_2_AREA_BASE)
SFLASH_FS_LOC                      := $(OTA2_IMAGE_CURR_FS_AREA_BASE)
DCT_IMAGE_PLATFORM          := $(FINAL_DCT_FILE)
SFLASH_DCT_START_SECTOR	:= $(shell $(PERL) $(SECTOR_NUMBER_SCRIPT) $(SFLASH_DCT_LOC) 4096)

APP0                    := $(OTA_APP_BIN)

OTA2_FS_IMAGE_FILE				:=$(OUTPUT_DIR)/OTA2_fs_image_file
OTA2_FS_IMAGE_CONFIG_FILE		:=$(OTA2_FS_IMAGE_FILE).cfg
OTA2_FS_IMAGE_BIN_FILE			:=$(OTA2_FS_IMAGE_FILE).bin

endif

# Target FINAL_OUTPUT_FILE is not used for Iwa platform. It causes build error for XIP build. So removing this target.
FINAL_OUTPUT_FILE :=

# Override the BD_ADDR parameter if provided on the command line.
ifeq ($(BT_DEVICE_ADDRESS),random)
BT_DEVICE_ADDRESS_OVERRIDE := -O DLConfigBD_ADDRBase:$(shell $(call CONV_SLASHES,$(PERL)) -e '$(PERL_ESC_DOLLAR)r=int(rand(32768));printf "$(CHIP)$(CHIP_REV)0%04X",$(PERL_ESC_DOLLAR)r;')
else
ifneq ($(BT_DEVICE_ADDRESS),)
BT_DEVICE_ADDRESS_OVERRIDE := -O DLConfigBD_ADDRBase:$(BT_DEVICE_ADDRESS)
endif
endif

 $(PATCH_ELF) : $(PATCH_DIR)/patch.elf
	$(OBJCOPY) --strip-symbols=$(PATCH_DIR)/sym_remove.txt --redefine-syms=$(PATCH_DIR)/sym_rename.txt $< $(PATCH_ELF)

ifeq (1, $(OTA2_SUPPORT))
$(OTA_APP_BIN):  $(SPAR_CGS_TARGET)
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(ECHO) Creating OTA binary ...
	$(QUIET)$(COPY) -f $(SOURCE_ROOT)platforms/$(PLATFORM)/*.hdf $(OUTPUT_DIR)/binary/
	$(QUIET)$(call CONV_SLASHES,$(CGS_FULL_NAME)) -D $(SOURCE_ROOT)platforms/$(PLATFORM) -O DLConfigFixedHeader:0 -O ConfigDSLocation:0 -B $(SOURCE_ROOT)platforms/$(PLATFORM)/$(PLATFORM_BOOTP) -I $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING).ota.hex --cgs-files $< > build/$(OUTPUT_NAME)/ota2bincgs2hex.log 2>&1 && $(ECHO) OTA2 Hex Conversion complete || $(ECHO) $(QUOTES_FOR_ECHO)**** OTA2 Hex Conversion failed ****$(QUOTES_FOR_ECHO)  && $(call CONV_SLASHES,$(PERL)) $(AFT) 5 build/$(OUTPUT_NAME)/ota2bincgs2hex.log
	$(QUIET)$(call CONV_SLASHES,$(HEX_TO_BIN_FULL_NAME)) $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING).ota.hex $(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING).ota.bin

ifeq ($(XIP_SUPPORT),1)
	$(QUIET)$(ECHO) Creating XIP binary ...
	$(OBJCOPY) -j .flash -O ihex $(ELF_OUT) build/$(OUTPUT_NAME)/binary/$(OUTPUT_NAME)-flash-code.ota.hex
	$(QUIET)$(call CONV_SLASHES,$(HEX_TO_BIN_FULL_NAME)) $(OUTPUT_DIR)/binary/$(OUTPUT_NAME)-flash-code.ota.hex $(XIP_OUTPUT_FILE)
endif

endif

ifeq (1, $(OTA2_SUPPORT))
$(DOWNLOAD_APP_HEX): $(OTA_APP_BIN)
else
$(DOWNLOAD_APP_HEX): $(SPAR_CGS_TARGET)
endif
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(ECHO) Converting ELF to ASM...
	$(QUIET)$(OBJDUMP)  --disassemble $(@:.hex=.elf) > $(@:.hex=.asm)
	$(QUIET)$(ECHO) Converting CGS to HEX...
	$(QUIET)$(COPY) -f $(SOURCE_ROOT)platforms/$(PLATFORM)/*.hdf $(OUTPUT_DIR)/binary/

ifeq ($(XIP_SUPPORT),1)
	$(OBJCOPY) -j .flash -O ihex $(ELF_OUT) build/$(OUTPUT_NAME)/binary/$(OUTPUT_NAME)-flash-code.hex
	$(QUIET)$(call CONV_SLASHES,$(CGS_FULL_NAME)) -D $(SOURCE_ROOT)platforms/$(PLATFORM) $(BT_DEVICE_ADDRESS_OVERRIDE) -B $(SOURCE_ROOT)platforms/$(PLATFORM)/$(PLATFORM_BOOTP) -I build/$(OUTPUT_NAME)/binary/$(OUTPUT_NAME)-temp.hex --cgs-files $(SPAR_CGS_TARGET) > build/$(OUTPUT_NAME)/cgs2hex.log 2>&1 && $(ECHO) Conversion complete || $(ECHO) $(QUOTES_FOR_ECHO)**** Conversion failed ****$(QUOTES_FOR_ECHO)  && $(call CONV_SLASHES,$(PERL)) $(AFT) 5 build/$(OUTPUT_NAME)/cgs2hex.log
	$(QUIET)$(call CONV_SLASHES,$(MERGE_INTEL_HEX_FULL_NAME)) build/$(OUTPUT_NAME)/binary/$(OUTPUT_NAME)-temp.hex build/$(OUTPUT_NAME)/binary/$(OUTPUT_NAME)-flash-code.hex $@
else
	$(QUIET)$(call CONV_SLASHES,$(CGS_FULL_NAME)) -D $(SOURCE_ROOT)platforms/$(PLATFORM) $(BT_DEVICE_ADDRESS_OVERRIDE) -B $(SOURCE_ROOT)platforms/$(PLATFORM)/$(PLATFORM_BOOTP) -I $@ --cgs-files $(SPAR_CGS_TARGET) > build/$(OUTPUT_NAME)/cgs2hex.log 2>&1 && $(ECHO) Conversion complete || $(ECHO) $(QUOTES_FOR_ECHO)**** Conversion failed ****$(QUOTES_FOR_ECHO)  && $(call CONV_SLASHES,$(PERL)) $(AFT) 5 build/$(OUTPUT_NAME)/cgs2hex.log
endif

$(DOWNLOAD_DCT_HEX): $(DOWNLOAD_APP_HEX) $(DCT_IMAGE_NAME)
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(call CONV_SLASHES,$(APPEND_TO_INTEL_HEX)) -I $(DCT_OCF_START_ADDR) $(DCT_IMAGE_NAME) $< $@ > $(OUTPUT_DIR)/binary/append_dct.log 2>&1 && $(ECHO) $(QUOTES_FOR_ECHO)dct append complete$(QUOTES_FOR_ECHO)  || $(ECHO) $(QUOTES_FOR_ECHO)**** dct append failed ****$(QUOTES_FOR_ECHO)

ifdef PLATFORM_SUPPORTS_OC_FLASH
$(DOWNLOAD_FILESYSTEM_HEX): $(DOWNLOAD_DCT_HEX) $(FS_IMAGE)
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(call CONV_SLASHES,$(APPEND_TO_INTEL_HEX)) -I $(FILESYSTEM_OCF_START_ADDR) $(FS_IMAGE) $< $@ > $(OUTPUT_DIR)/binary/append_filesystem.log 2>&1 && $(ECHO) $(QUOTES_FOR_ECHO)filesystem append complete$(QUOTES_FOR_ECHO)  || $(ECHO) $(QUOTES_FOR_ECHO)**** filesystem append failed ****$(QUOTES_FOR_ECHO)

ifeq (1, $(OTA2_SUPPORT))
#check ota2_image is passed in the build string
override OTA2_IMAGE_TARGET	:= $(if $(findstring ota2_image, $(MAKECMDGOALS)),1)

$(FS_IMAGE): $(STRIPPED_LINK_OUTPUT_FILE)  $(STAGING_DIR).d
	$(QUIET)$(ECHO) Creating Filesystem
	$(QUIET)$(COMMON_TOOLS_PATH)mk_wicedfs32 $(FS_IMAGE) $(STAGING_DIR)
	$(QUIET)$(ECHO) done.


ifeq (1, $(OTA2_IMAGE_TARGET))

FILESYSTEM_IMAGE := $(OTA2_FS_IMAGE_BIN_FILE)

$(OTA2_FS_IMAGE_BIN_FILE):$(FS_IMAGE)
	$(QUIET)$(ECHO) Building OTA2 FS Image Info File $(OTA2_FS_IMAGE_CONFIG_FILE)
	$(QUIET)$(call WRITE_FILE_CREATE, $(OTA2_FS_IMAGE_CONFIG_FILE) ,FACTORY_RESET=0x00)
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_FS_IMAGE_CONFIG_FILE) ,MAJOR_VERSION=$(APP_VERSION_FOR_OTA2_MAJOR))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_FS_IMAGE_CONFIG_FILE) ,MINOR_VERSION=$(APP_VERSION_FOR_OTA2_MINOR))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_FS_IMAGE_CONFIG_FILE) ,PLATFORM_NAME=$(PLATFORM).$(APPS_CHIP_REVISION))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_FS_IMAGE_CONFIG_FILE) ,FILESYSTEM_LOC=$(OTA2_IMAGE_CURR_FS_AREA_BASE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_FS_IMAGE_CONFIG_FILE) ,FILESYSTEM_FILE=$(if $(SECURE_FS_IMAGE),$(call CONV_SLASHES,$(SECURE_FS_IMAGE)),$(call CONV_SLASHES,$(FS_IMAGE))))
	$(QUIET)$(ECHO) Building OTA2 Fs Image File $(OTA2_FS_IMAGE_BIN_FILE)
	$(COMMON_TOOLS_PATH)mk_wiced_ota2_image32 $(OTA2_FS_IMAGE_CONFIG_FILE) $(OTA2_FS_IMAGE_BIN_FILE) -v $(VERBOSE)
	$(QUIET)$(ECHO) OTA2 Fs Image File Done
endif #OTA2_IMAGE_TARGET

endif #OTA2_SUPPORT

endif #PLATFORM_SUPPORTS_OC_FLASH


ifeq (1,$(OTA2_SUPPORT))
# create Bootloader.hex
# Append Failsafe.bin
# Append LUT.bin
# Append DCT.bin
# Append Extract.bin
# Append FS.bin
# Append OTAAPP.bin
# Append OTAIMAGE.bin
#

BOOTLOADER_TARGET := waf.ota2_btmcu_bootloader-$(PLATFORM)
OTA2_FAILSAFE_TARGET := waf.ota2_failsafe-$(PLATFORM)
OTA2_BTSRV_TARGET := waf.ota2_bt_service_app-$(PLATFORM)
OTA2_EXTRACT_TARGET := snip.ota2_extract-$(PLATFORM)

LINK_APPS_FILE            :=$(OUTPUT_DIR)/APPS$(LINK_OUTPUT_SUFFIX)
STRIPPED_LINK_APPS_FILE   :=$(LINK_APPS_FILE:$(LINK_OUTPUT_SUFFIX)=.stripped$(LINK_OUTPUT_SUFFIX))
FINAL_APPS_FILE           :=$(LINK_APPS_FILE:$(LINK_OUTPUT_SUFFIX)=$(FINAL_OUTPUT_SUFFIX))

ota2_dependant_components: $(FINAL_APPS_FILE) build_ota2_failsafe build_ota2_bootloader build_ota2_bt_service_app

HEX_ADD_SCRIPT				:= $(SOURCE_ROOT)/WICED/platform/MCU/$(HOST_MCU_FAMILY)/makefiles/hex_addition.pl

# PLEASE make sure that following images are created before reach here

OTA2_BOOTLOADER_IMAGE	:= $(BUILD_DIR)/waf.ota2_btmcu_bootloader-$(PLATFORM)/binary/waf.ota2_btmcu_bootloader-$(PLATFORM).hex
OTA2_BTSRV_IMAGE		:= $(BUILD_DIR)/waf.ota2_bt_service_app-$(PLATFORM)/binary/waf.ota2_bt_service_app-$(PLATFORM).ota.bin
OTA2_FAILSAFE_IMAGE		:= $(BUILD_DIR)/waf.ota2_failsafe-$(PLATFORM)/binary/waf.ota2_failsafe-$(PLATFORM).ota.bin
OTA2_LUT_IMAGE			:= $(OUTPUT_DIR)/APPS.bin
OTA2_DCT_IMAGE			:= $(DCT_IMAGE_NAME)
OTA2_EXTRACT_IMAGE		:= $(OTA_APP)
OTA2_FS_IMAGE			:= $(FS_IMAGE)
OTA2_APP0_IMAGE			:= $(OTA_APP_BIN)
ifeq ($(XIP_SUPPORT),1)
OTA2_APP0_XIP_IMAGE	:= $(XIP_OUTPUT_FILE)
endif
#OTA2_IMAGE				:= $(OUTPUT_DIR)/OTA2_image_file.bin

OTA2_FRAPP_APPEND_IMAGE	:= $(OUTPUT_DIR)/binary/frapp_append.hex
OTA2_FAILSAFE_APPEND_IMAGE	:= $(OUTPUT_DIR)/binary/failsafe_append.hex
OTA2_LUT_APPEND_IMAGE		:= $(OUTPUT_DIR)/binary/lut_append.hex
OTA2_DCT_APPEND_IMAGE		:= $(OUTPUT_DIR)/binary/dct_append.hex
OTA2_EXTRACT_APPEND_IMAGE	:= $(OUTPUT_DIR)/binary/extract_append.hex
OTA2_FS_APPEND_IMAGE		:= $(OUTPUT_DIR)/binary/fs_append.hex
OTA2_APP0_APPEND_IMAGE		:= $(OUTPUT_DIR)/binary/app0_append.hex
OTA2_DOWNLOAD_TARGET		:= $(OUTPUT_DIR)/binary/ota2_target_download.hex

$(eval FRAPP_FLASH_ADDR 	= $(shell $(PERL) $(HEX_ADD_SCRIPT) $(OTA2_IMAGE_FLASH_BASE_ADDRESS) $(OTA2_IMAGE_FR_APP_AREA_BASE) ))
$(eval FAILSAFE_FLASH_ADDR 	= $(shell $(PERL) $(HEX_ADD_SCRIPT) $(OTA2_IMAGE_FLASH_BASE_ADDRESS) $(OTA2_IMAGE_FAILSAFE_APP_AREA_BASE) ))
$(eval LUT_FLASH_ADDR		= $(shell $(PERL) $(HEX_ADD_SCRIPT) $(OTA2_IMAGE_FLASH_BASE_ADDRESS) $(OTA2_IMAGE_CURR_LUT_AREA_BASE) ))
$(eval DCT_FLASH_ADDR		= $(shell $(PERL) $(HEX_ADD_SCRIPT) $(OTA2_IMAGE_FLASH_BASE_ADDRESS) $(OTA2_IMAGE_CURR_DCT_1_AREA_BASE) ))
$(eval EXTRACT_FLASH_ADDR	= $(shell $(PERL) $(HEX_ADD_SCRIPT) $(OTA2_IMAGE_FLASH_BASE_ADDRESS) $(OTA2_IMAGE_CURR_OTA_APP_AREA_BASE) ))
$(eval FS_FLASH_ADDR		= $(shell $(PERL) $(HEX_ADD_SCRIPT) $(OTA2_IMAGE_FLASH_BASE_ADDRESS) $(OTA2_IMAGE_FS_DATA_AREA_BASE) ))
$(eval APP0_FLASH_ADDR		= $(shell $(PERL) $(HEX_ADD_SCRIPT) $(OTA2_IMAGE_FLASH_BASE_ADDRESS) $(OTA2_IMAGE_CURR_APP0_AREA_BASE) ))
ifeq ($(XIP_SUPPORT),1)
$(eval APP0_XIP_ADDR			= $(shell $(PERL) $(HEX_ADD_SCRIPT) $(OTA2_IMAGE_FLASH_BASE_ADDRESS) $(OTA2_IMAGE_APP0_XIP_AREA_BASE) ))
endif
$(eval OTA_FLASH_ADDR		= $(shell $(PERL) $(HEX_ADD_SCRIPT) $(OTA2_IMAGE_FLASH_BASE_ADDRESS) $(OTA2_IMAGE_STAGING_AREA_BASE) ))

$(OTA2_DOWNLOAD_TARGET): $(DOWNLOAD_APP_HEX)
ifeq ($(OTA_APP),)
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(ECHO) ""
	$(QUIET)$(ECHO) "   The OTA2 extraction application ***MUST*** be included in your OTA2 update"
	$(QUIET)$(ECHO) "   Please build apps/snip/ota2_extract (or your ota2 extraction application)"
	$(QUIET)$(ECHO) "   And add these lines to your <application>.mk file (see apps/snip/ota2_example/ota2_example.mk):"
	$(QUIET)$(ECHO) ""
	$(QUIET)$(ECHO) "   OTA_APPLICATION	:= snip.ota2_extract-$$(PLATFORM)"
	$(QUIET)$(ECHO) "   OTA_APP    := build/$$(OTA_APPLICATION)/binary/$$(OTA_APPLICATION).ota.bin"
	$(QUIET)$(ECHO) ""
	$(error        OTA2 Extract Image File Not defined !!!)
else
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(ECHO) Appending $(OTA2_BTSRV_IMAGE) at $(FRAPP_FLASH_ADDR) ...
	$(QUIET)$(call CONV_SLASHES,$(APPEND_TO_INTEL_HEX)) -I $(FRAPP_FLASH_ADDR) $(OTA2_BTSRV_IMAGE) $(OTA2_BOOTLOADER_IMAGE) $(OTA2_FRAPP_APPEND_IMAGE) > $(OUTPUT_DIR)/binary/append_frapp.log 2>&1 && $(ECHO) $(QUOTES_FOR_ECHO)failsafe append complete$(QUOTES_FOR_ECHO)  || $(ECHO) $(QUOTES_FOR_ECHO)**** failsafe append failed ****$(QUOTES_FOR_ECHO)
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(ECHO) Appending $(OTA2_FAILSAFE_IMAGE) at $(FAILSAFE_FLASH_ADDR) ...
	$(QUIET)$(call CONV_SLASHES,$(APPEND_TO_INTEL_HEX)) -I $(FAILSAFE_FLASH_ADDR) $(OTA2_FAILSAFE_IMAGE) $(OTA2_FRAPP_APPEND_IMAGE) $(OTA2_FAILSAFE_APPEND_IMAGE) > $(OUTPUT_DIR)/binary/append_failsafe.log 2>&1 && $(ECHO) $(QUOTES_FOR_ECHO)failsafe append complete$(QUOTES_FOR_ECHO)  || $(ECHO) $(QUOTES_FOR_ECHO)**** failsafe append failed ****$(QUOTES_FOR_ECHO)
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(ECHO) Appending $(OTA2_LUT_IMAGE) at $(LUT_FLASH_ADDR) ...
	$(QUIET)$(call CONV_SLASHES,$(APPEND_TO_INTEL_HEX)) -I $(LUT_FLASH_ADDR) $(OTA2_LUT_IMAGE) $(OTA2_FAILSAFE_APPEND_IMAGE) $(OTA2_LUT_APPEND_IMAGE) > $(OUTPUT_DIR)/binary/append_lut.log 2>&1 && $(ECHO) $(QUOTES_FOR_ECHO)lut append complete$(QUOTES_FOR_ECHO)  || $(ECHO) $(QUOTES_FOR_ECHO)**** lut append failed ****$(QUOTES_FOR_ECHO)
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(ECHO) Appending $(OTA2_DCT_IMAGE) at $(DCT_FLASH_ADDR) ...
	$(QUIET)$(call CONV_SLASHES,$(APPEND_TO_INTEL_HEX)) -I $(DCT_FLASH_ADDR) $(OTA2_DCT_IMAGE) $(OTA2_LUT_APPEND_IMAGE) $(OTA2_DCT_APPEND_IMAGE) > $(OUTPUT_DIR)/binary/append_dct.log 2>&1 && $(ECHO) $(QUOTES_FOR_ECHO)dct append complete$(QUOTES_FOR_ECHO)  || $(ECHO) $(QUOTES_FOR_ECHO)**** dct append failed ****$(QUOTES_FOR_ECHO)
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(ECHO) Appending $(OTA2_EXTRACT_IMAGE) at $(EXTRACT_FLASH_ADDR) ...
	$(QUIET)$(call CONV_SLASHES,$(APPEND_TO_INTEL_HEX)) -I $(EXTRACT_FLASH_ADDR) $(OTA2_EXTRACT_IMAGE) $(OTA2_DCT_APPEND_IMAGE) $(OTA2_EXTRACT_APPEND_IMAGE) > $(OUTPUT_DIR)/binary/append_extract.log 2>&1 && $(ECHO) $(QUOTES_FOR_ECHO)extract append complete$(QUOTES_FOR_ECHO)  || $(ECHO) $(QUOTES_FOR_ECHO)**** extract append failed ****$(QUOTES_FOR_ECHO)
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(ECHO) Appending $(OTA2_FS_IMAGE) at $(FS_FLASH_ADDR) ...
	$(QUIET)$(call CONV_SLASHES,$(APPEND_TO_INTEL_HEX)) -I $(FS_FLASH_ADDR) $(OTA2_FS_IMAGE) $(OTA2_EXTRACT_APPEND_IMAGE) $(OTA2_FS_APPEND_IMAGE) > $(OUTPUT_DIR)/binary/append_fs.log 2>&1 && $(ECHO) $(QUOTES_FOR_ECHO)fs append complete$(QUOTES_FOR_ECHO)  || $(ECHO) $(QUOTES_FOR_ECHO)**** fs append failed ****$(QUOTES_FOR_ECHO)
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(ECHO) Appending $(OTA2_APP0_IMAGE) at $(APP0_FLASH_ADDR) ...
ifeq ($(XIP_SUPPORT),1)
	$(QUIET)$(call CONV_SLASHES,$(APPEND_TO_INTEL_HEX)) -I $(APP0_FLASH_ADDR) $(OTA2_APP0_IMAGE) $(OTA2_FS_APPEND_IMAGE) $(OTA2_APP0_APPEND_IMAGE) > $(OUTPUT_DIR)/binary/append_app0.log 2>&1 && $(ECHO) $(QUOTES_FOR_ECHO)app0 append complete$(QUOTES_FOR_ECHO)  || $(ECHO) $(QUOTES_FOR_ECHO)**** app0 append failed ****$(QUOTES_FOR_ECHO)
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(ECHO) Appending $(OTA2_APP0_XIP_IMAGE) at $(APP0_XIP_ADDR) ...
	$(QUIET)$(call CONV_SLASHES,$(APPEND_TO_INTEL_HEX)) -I $(APP0_XIP_ADDR) $(OTA2_APP0_XIP_IMAGE) $(OTA2_APP0_APPEND_IMAGE) $(OTA2_DOWNLOAD_TARGET) > $(OUTPUT_DIR)/binary/append_app0xip.log 2>&1 && $(ECHO) $(QUOTES_FOR_ECHO)app0xip append complete$(QUOTES_FOR_ECHO)  || $(ECHO) $(QUOTES_FOR_ECHO)**** app0xip append failed ****$(QUOTES_FOR_ECHO)
else
	$(QUIET)$(call CONV_SLASHES,$(APPEND_TO_INTEL_HEX)) -I $(APP0_FLASH_ADDR) $(OTA2_APP0_IMAGE) $(OTA2_FS_APPEND_IMAGE) $(OTA2_DOWNLOAD_TARGET) > $(OUTPUT_DIR)/binary/append_app0.log 2>&1 && $(ECHO) $(QUOTES_FOR_ECHO)app0 append complete$(QUOTES_FOR_ECHO)  || $(ECHO) $(QUOTES_FOR_ECHO)**** app0 append failed ****$(QUOTES_FOR_ECHO)
endif #XIP_SUPPORT
#	$(QUIET)$(ECHO_BLANK_LINE)
#	$(QUIET)$(ECHO) Appending $(OTA2_IMAGE) at $(OTA_FLASH_ADDR) ...
#	$(QUIET)$(call CONV_SLASHES,$(APPEND_TO_INTEL_HEX)) -I $(OTA_FLASH_ADDR) $(OTA2_IMAGE) $(OTA2_APP0_APPEND_IMAGE) $(OTA2_DOWNLOAD_TARGET) > $(OUTPUT_DIR)/binary/append_ota.log 2>&1 && $(ECHO) $(QUOTES_FOR_ECHO)ota append complete$(QUOTES_FOR_ECHO)  || $(ECHO) $(QUOTES_FOR_ECHO)**** ota append failed ****$(QUOTES_FOR_ECHO)
endif

#
# build bootloader
# build failsafe
# build extractor
#
#


build_ota2_bootloader:
ifeq ($(XIP_SUPPORT),1)
	$(QUIET)$(ECHO) Building Bootloader $(BOOTLOADER_TARGET) ota2_xip=1
	$(QUIET)$(MAKE) -r -f $(SOURCE_ROOT)Makefile $(BOOTLOADER_TARGET) ota2_xip=1 -I$(OUTPUT_DIR)  SFLASH= EXTERNAL_WICED_GLOBAL_DEFINES=$(EXTERNAL_WICED_GLOBAL_DEFINES) SUB_BUILD=bootloader $(BOOTLOADER_REDIRECT)
else
	$(QUIET)$(ECHO) Building Bootloader $(BOOTLOADER_TARGET)
	$(QUIET)$(MAKE) -r -f $(SOURCE_ROOT)Makefile $(BOOTLOADER_TARGET) -I$(OUTPUT_DIR)  SFLASH= EXTERNAL_WICED_GLOBAL_DEFINES=$(EXTERNAL_WICED_GLOBAL_DEFINES) SUB_BUILD=bootloader $(BOOTLOADER_REDIRECT)
endif
	$(QUIET)$(ECHO) Finished Building Bootloader
	$(QUIET)$(ECHO_BLANK_LINE)

build_ota2_failsafe:
	$(QUIET)$(ECHO) Building OTA2 Failsafe
	$(QUIET)$(MAKE) -r -f $(SOURCE_ROOT)Makefile $(OTA2_FAILSAFE_TARGET) -I$(OUTPUT_DIR)  SFLASH= EXTERNAL_WICED_GLOBAL_DEFINES=$(EXTERNAL_WICED_GLOBAL_DEFINES) SUB_BUILD=bootloader $(BOOTLOADER_REDIRECT)
	$(QUIET)$(ECHO) Finished Building OTA2 Failsafe
	$(QUIET)$(ECHO_BLANK_LINE)

build_ota2_bt_service_app:
	$(QUIET)$(ECHO) Building OTA2 factory reset
	$(QUIET)$(MAKE) -r -f $(SOURCE_ROOT)Makefile $(OTA2_BTSRV_TARGET) -I$(OUTPUT_DIR)  SFLASH= EXTERNAL_WICED_GLOBAL_DEFINES=$(EXTERNAL_WICED_GLOBAL_DEFINES) SUB_BUILD=bootloader $(BOOTLOADER_REDIRECT)
	$(QUIET)$(ECHO) Finished Building OTA2 bt service app
	$(QUIET)$(ECHO_BLANK_LINE)


ota2_apps_download: download ota2_dependant_components
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(ECHO) OTA2 DOWNLOAD is completed for IWA platform

endif

CHIPLOAD_PLATFORM_ARGS := -BLUETOOLMODE -BAUDRATE $(PLATFORM_BAUDRATE) $(CHIPLOAD_REBAUD) $(CHIPLOAD_NO_READ_VERIFY) $(CHIPLOAD_CRC_VERIFY)

ifeq (1,$(OTA2_SUPPORT))
download: $(OTA2_DOWNLOAD_TARGET)
else
download: $(DOWNLOAD_FILE_NAME)
endif
	$(QUIET)$(ECHO_BLANK_LINE)
#	$(QUIET)$(eval UART:=$(shell $(CAT) < com_port.txt))
	$(QUIET)$(if $(UART), \
	        $(ECHO) Downloading application... && $(call CONV_SLASHES,$(CHIPLOAD_FULL_NAME)) $(CHIPLOAD_PLATFORM_ARGS) -PORT $(UART) -MINIDRIVER $(SOURCE_ROOT)platforms/$(PLATFORM)/$(PLATFORM_MINIDRIVER) -BTP $(SOURCE_ROOT)platforms/$(PLATFORM)/$(PLATFORM_BOOTP) -CONFIG $< -LOGTO build/$(OUTPUT_NAME)/logto.log > build/$(OUTPUT_NAME)/download.log 2>&1 \
	        		&& $(ECHO) Download complete && $(ECHO_BLANK_LINE) && $(ECHO) $(QUOTES_FOR_ECHO)Application running$(QUOTES_FOR_ECHO) \
	        		|| $(ECHO) $(QUOTES_FOR_ECHO)****Download failed ****$(QUOTES_FOR_ECHO) && $(call CONV_SLASHES,$(PERL)) $(AFT) 4 build/$(OUTPUT_NAME)/download.log , \
	        	$(ECHO) Download failed. Please add COM port number to ;make target string)


PKG_PLATFORM_BINARY_ROOT := $(SOURCE_ROOT)platforms/$(PLATFORM)

package: $(RELEASE_PACKAGE)
	$(QUIET)$(ECHO) Created package successfully

$(RELEASE_PACKAGE): create_package_descriptor package_apps

package_apps: $(DOWNLOAD_FILE_NAME)
	$(QUIET)$(ECHO) Adding binaries to the package...
	$(call ADD_TO_PACKAGE, $(PKG_PLATFORM_BINARY_ROOT)/$(PLATFORM_BOOTP), , , btp)
	$(call ADD_TO_PACKAGE, $(PKG_PLATFORM_BINARY_ROOT)/$(PLATFORM_MINIDRIVER), , , mdrv)
	$(call ADD_TO_PACKAGE, $(DOWNLOAD_FILE_NAME), , , app)
	$(call ADD_TO_PACKAGE, $(PKG_PLATFORM_BINARY_ROOT)/$(PLATFORM_IDFILE), , , id)

run:
	$(QUIET)$(ECHO) Ignoring 'run' - Target will reset after download...

$(OUTPUT_DIR)/Modules/lib_installer.o: $(OUTPUT_DIR)/Modules/lib_installer.c
#	$(info Generated lib_installer.c)
	$(CC) $(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(COMPILER_SPECIFIC_DEPS_FLAG) $(COMPILER_SPECIFIC_STANDARD_CFLAGS)  $(COMPILER_SPECIFIC_RELEASE_CFLAGS) -o $@ $< $(COMPILER_SPECIFIC_STDOUT_REDIRECT)

$(OUTPUT_DIR)/Modules/lib_installer.c_opts: #$(CONFIG_FILE)
#	$(QUIET)$$(call WRITE_FILE_CREATE, $$@, $(subst $(COMMA),$$(COMMA), $(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(COMPILER_SPECIFIC_DEPS_FLAG) $(COMPILER_SPECIFIC_STANDARD_CFLAGS) $(if $(findstring debug,$($(1)_BUILD_TYPE)), $(COMPILER_SPECIFIC_DEBUG_CFLAGS), $(COMPILER_SPECIFIC_RELEASE_CFLAGS)) $($(1)_CFLAGS) $($(1)_INCLUDES) $($(1)_DEFINES) $(WICED_SDK_INCLUDES) $(WICED_SDK_DEFINES)))
#	$(QUIET)$$(call WRITE_FILE_CREATE, $$@, $(subst $(COMMA),$$(COMMA), $(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(COMPILER_SPECIFIC_DEPS_FLAG) $(COMPILER_SPECIFIC_STANDARD_CFLAGS)  $(COMPILER_SPECIFIC_RELEASE_CFLAGS) ))

#Final CGS Target: All the CGS files are merged here
$(SPAR_CGS_TARGET): $(CGS_LIST) $(SECTIONS_CGS_LIST) $(SPAR_SETUP_CALL_CGS)
	$(QUIET)-$(XMD) $(@D)
ifeq ($(HOST_OS),Win32)
	$(QUIET)$(ECHO) Making combined CGS image on $(HOST_OS)
	$(call PERL_FIX_QUOTES, $(PERL) -e "foreach(@ARGV){open(FL,$$_);while(<FL>){print;}close(FL);}" $^) > $@
else
	$(QUIET)$(ECHO) Making combined CGS image on $(HOST_OS)
	$(PERL) -e 'foreach(@ARGV){open(FL,$$_);while(<FL>){print;}close(FL);}'  $^ > $@
endif
	$(QUIET)$(ECHO) OK, made $(CURDIR)/$@. MD5 sum is:
	$(QUIET)$(MD5SUM) $@
#	$(QUIET)$(XRM) $(ELF_OUT)*.cgs $(ELF_OUT_SYM) $(ELF_OUT_BIN)
	$(QUIET)$(ECHO)




#include $(TC)/rules_$(CHIP_CORE)_$(TC).mk
include $(SOURCE_ROOT)WICED/platform/MCU/$(HOST_MCU_FAMILY)/makefiles/gcc/rules_cortex-m4_gcc.mk
#$(info OBJS: $(OBJS))
