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

# Include makefile.env -- You can include tools.inc from anywhere and
# it will still find makefile.env (which sits in the same directory)
# $(lastword a b c) doesn't work at least on Make 3.80, so use word+words

THIS_DIR := $(dir $(word $(words $(MAKEFILE_LIST)), $(MAKEFILE_LIST)))

ifneq ($(wildcard $(dir $(MAKE))make.exe),)
PERL_FIX_QUOTES = $(subst !@%^,',$(subst ',",$(subst ",!@%^,$(1))))
#$(info $(PERL_FIX_QUOTES))
#$(info PERL-1)
else
PERL_FIX_QUOTES = $(1)
#$(info $(PERL_FIX_QUOTES))
#$(info PERL-2)
endif

WICED_TOOLS_DIR		:= $(TOOLS_ROOT)/BT/wiced_tools
SPAR_SCRIPTS_DIR    := $(TOOLS_ROOT)/BT/scripts/

ifeq ($(GCC_VERSION_GREATER_THAN_5),1)
GCC_LIBGCC_DIR      := $(MAKEFILES_PATH)/../ARM_GNU/$(HOST_OS)/lib/gcc/arm-none-eabi/7.2.1/thumb/v7-m
GCC_LIBC_DIR        := $(MAKEFILES_PATH)/../ARM_GNU/$(HOST_OS)/arm-none-eabi/lib/thumb/v7-m
else
GCC_LIB_DIR			:= $(MAKEFILES_PATH)/../ARM_GNU/lib/thumb/v7m
endif

SECTION_TO_CGS      := $(SPAR_SCRIPTS_DIR)section_to_cgs.pl
GEN_OVERLAY_FILES   := $(SPAR_SCRIPTS_DIR)generateoverlayfiles.pl
GEN_LIB_INSTALLER   := $(SPAR_SCRIPTS_DIR)generatelibinstaller.pl
HEXMERGE            := $(SPAR_SCRIPTS_DIR)mergearmhex.pl
PRINT_WRAP_FLAGS    := $(SPAR_SCRIPTS_DIR)print_wrap_flags.pl
GET_SUB_DIRS        := $(SPAR_SCRIPTS_DIR)get_sub_dirs.pl
SETUP_CALL_SCRIPT   := $(SPAR_SCRIPTS_DIR)setup_call_cgs.pl

XMD     			:= $(COMMON_TOOLS_PATH)mkdir -p
XLD                 := $(TOOLCHAIN_PATH)arm-none-eabi-ld
ACC                 := $(TOOLCHAIN_PATH)arm-none-eabi-gcc $(GCC_FLAGS)
XCC                 := $(ACC)
READ_ELF            := $(TOOLCHAIN_PATH)arm-none-eabi-readelf
MD5SUM  			:= $(COMMON_TOOLS_PATH)md5sum
COPY 				:= $(COMMON_TOOLS_PATH)cp

# Builds the final CGS file
CGS_FULL_NAME := $(WICED_TOOLS_DIR)/CGS/$(HOST_OS)/cgs
AFT:= $(WICED_TOOLS_DIR)/aft/aft.pl
CHIPLOAD_FULL_NAME := $(WICED_TOOLS_DIR)/ChipLoad/$(HOST_OS)/ChipLoad
HEX_TO_BIN_FULL_NAME := $(WICED_TOOLS_DIR)/IntelHexToBin/$(HOST_OS)/ihex2bin
APPEND_TO_INTEL_HEX := $(WICED_TOOLS_DIR)/IntelHexToBin/$(HOST_OS)/AppendToIntelHex
MERGE_INTEL_HEX_FULL_NAME := $(WICED_TOOLS_DIR)/IntelHexToBin/$(HOST_OS)/MergeIntelHex

#overwrite LINKER related MACROS
LDR     := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)ld$(EXECUTABLE_SUFFIX)"
LINKER  := $(LDR)
COMPILER_SPECIFIC_LINK_MAP         =  -Map $(1)
COMPILER_SPECIFIC_LINK_FILES       =  --start-group $(1) -lgcc -lc --end-group
COMPILER_SPECIFIC_LINK_SCRIPT_DEFINE_OPTION = -T
COMPILER_SPECIFIC_LINK_SCRIPT      =  $(addprefix -T ,$(1))
COMPILER_SPECIFIC_DEBUG_LDFLAGS    := --gc-sections --cref
COMPILER_SPECIFIC_RELEASE_LDFLAGS  := --gc-sections  --cref

#$(info LINKER: $(LINKER))

