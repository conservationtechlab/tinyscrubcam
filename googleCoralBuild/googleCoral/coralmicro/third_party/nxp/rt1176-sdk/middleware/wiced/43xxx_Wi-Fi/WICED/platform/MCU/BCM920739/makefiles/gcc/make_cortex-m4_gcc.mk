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
# This contains the CM4 varient makefile customizations for GCC

# We link with the base elf files
#LINK_ELFS 			:= $(addprefix --just-symbols=,$(ELF_LIST))
MAKE_ASSEMBLY       = $(FROMELF)  --disassemble $@ > $(@:.elf=.asm)
################################################################################
# General link flags
################################################################################
CUSTOM_LD_FLAGS := --just-symbols=$(PATCH_ELF)
CUSTOM_LD_FLAGS  += -nostartfiles -EL -O2 -z muldefs
ifeq ($(GCC_VERSION_GREATER_THAN_5),1)
CUSTOM_LD_FLAGS  += -L $(GCC_LIBC_DIR) -L $(GCC_LIBGCC_DIR)
else
CUSTOM_LD_FLAGS  += -L $(GCC_LIB_DIR)
endif

# Pre-processed
ifeq ($(XIP_SUPPORT),1)
SPAR_LINK_LOAD_FILE = $(BIN_OUT_DIR)/$(PLATFORM)_$(SPAR_IN)_proc.preld
else
SPAR_LINK_LOAD_FILE = $(BIN_OUT_DIR)/$(PLATFORM)_$(SPAR_IN)_proc.ld
endif

################################################################################
# Code/data location
################################################################################
# This spar's CRT if it exists is responsible for moving last used ptr.

IRAM_BEGIN_CMD  := $(PERL) -nle 'printf( "0x%08X", hex($$1) )  if /^FIRST_FREE_SECTION_IN_SRAM=(0x[[:alnum:]]+)/'                                                                 $(PATCH_LST)
ifeq (1, $(OTA2_SUPPORT))
#if OTA is enabled, The IRAM_LEN_CMD should be fixed to SRAM_OTA_APP_MAX_LEN
IRAM_LEN_CMD    := $(ECHO) $(SRAM_OTA_APP_MAX_LEN)
else
IRAM_LEN_CMD    := $(PERL) -nle 'printf( "0x%08X", ($(SRAM_BEGIN_ADDR) + $(SRAM_LENGTH) - $(PATCH_TABLE_LENGTH) - hex($$1)) )  if /^FIRST_FREE_SECTION_IN_SRAM=(0x[[:alnum:]]+)/' $(PATCH_LST)
endif


IAON_BEGIN_CMD  := $(PERL) -nle 'printf( "0x%08X", hex($$1) )  if /^FIRST_FREE_SECTION_IN_AON=(0x[[:alnum:]]+)/'                                                                  $(PATCH_LST)
#We have to limit application static AON area, so that rest of the area can be used by firmware AON patches.
#IAON_LEN_CMD    := $(PERL) -nle 'printf( "0x%08X", ($(AON_BEGIN_ADDR) + $(AON_LENGTH)  - hex($$1)) )  if /^FIRST_FREE_SECTION_IN_AON=(0x[[:alnum:]]+)/'                           $(PATCH_LST)
#Limiting the application static aon length to 256, Might be there is better way to do it instead of below
IAON_LEN_CMD := $(ECHO) $(AON_FOR_APP_MAX_LEN)

PRAM_BEGIN_CMD         := $(PERL) -nle 'printf( "0x%08X", hex($$1))  if /^PATCH_ROM_START=(0x[[:alnum:]]+)/'                                                                      $(PATCH_LST)
PRAM_LEN_CMD           := $(PERL) -nle 'printf( "0x%08X", hex($$1))  if /^PATCH_ROM_SIZE=(0x[[:alnum:]]+)/'                                                                       $(PATCH_LST)
PRAM_FREE_LEN_CMD      := $(PERL) -nle 'printf( "0x%08X", ($(PROM_LENGTH) - hex($$1)) )  if /^PATCH_ROM_SIZE=(0x[[:alnum:]]+)/'                                                   $(PATCH_LST)

SPAR_LOC_PREREQ += $(PATCH_LST)

ifeq ($(SPAR_IN), flash)
# If SPAR is in flash, IRAM is already setup, need to setup IROM
# Note that we are rounding up to UINT32 closest to 4 bytes above current image just to be safe
  IROM_BEGIN_CMD := $(PERL) -nle 'printf( "0x%08X", hex($$1) + hex($$2) + 4 )  if /^[[:space:]]+Load[[:space:]]Region[[:space:]]CM3_Ver1[[:space:]].*Base.*[[:space:]](0x[[:alnum:]]+).*[[:space:]]Size\:[[:space:]]+(0x[[:alnum:]]+)/'                                            $(PATCH_LST)
  IROM_LEN_CMD   := $(PERL) -nle 'printf( "0x%08X", ($(FLASH0_BEGIN_ADDR) + $(FLASH0_LENGTH) - hex($$1) - hex($$2) - 4))  if /^[[:space:]]+Load[[:space:]]Region[[:space:]]CM3_Ver1[[:space:]].*Base.*[[:space:]](0x[[:alnum:]]+).*[[:space:]]Size\:[[:space:]]+(0x[[:alnum:]]+)/' $(PATCH_LST)


  IRAM_BEGIN_CMD := $(PERL) -nle 'printf( "0x%08X", hex($$1_ )   if /^[[:space:]]+Execution[[:space:]]Region[[:space:]]first_free_section_in_SRAM[[:space:]].*Base:[[:space:]]+(0x[[:alnum:]]+)/'                                                                 $(PATCH_LST)
  IRAM_LEN_CMD   := $(PERL) -nle 'printf( "0x%08X", ($(SRAM_BEGIN_ADDR) + $(SRAM_LENGTH) - $(PATCH_TABLE_LENGTH) - hex($$1)) )   if /^[[:space:]]+Execution[[:space:]]Region[[:space:]]first_free_section_in_SRAM[[:space:]].*Base:[[:space:]]+(0x[[:alnum:]]+)/' $(PATCH_LST)

  SPAR_LOC_PREREQ += $(PATCH_LST)
else
  IROM_BEGIN_CMD := $(ECHO) ""
  IROM_LEN_CMD := $(ECHO) ""
endif

IROM_BEGIN_CMD   :=$(call PERL_FIX_QUOTES,$(IROM_BEGIN_CMD))
IROM_LEN_CMD     :=$(call PERL_FIX_QUOTES,$(IROM_LEN_CMD))
IRAM_BEGIN_CMD   :=$(call PERL_FIX_QUOTES,$(IRAM_BEGIN_CMD))
IRAM_LEN_CMD     :=$(call PERL_FIX_QUOTES,$(IRAM_LEN_CMD))
IAON_BEGIN_CMD   :=$(call PERL_FIX_QUOTES,$(IAON_BEGIN_CMD))
IAON_LEN_CMD     :=$(call PERL_FIX_QUOTES,$(IAON_LEN_CMD))
PRAM_BEGIN_CMD   :=$(call PERL_FIX_QUOTES,$(PRAM_BEGIN_CMD))
PRAM_LEN_CMD     :=$(call PERL_FIX_QUOTES,$(PRAM_LEN_CMD))
PRAM_FREE_LEN_CMD :=$(call PERL_FIX_QUOTES,$(PRAM_FREE_LEN_CMD))

IRAM_BEGIN = $(call QUIET_SHELL, $(IRAM_BEGIN_CMD))
IRAM_LEN = $(call QUIET_SHELL, $(IRAM_LEN_CMD))

IROM_BEGIN = $(call QUIET_SHELL, $(IROM_BEGIN_CMD))
IROM_LEN = $(call QUIET_SHELL, $(IROM_LEN_CMD))

IAON_BEGIN = $(call QUIET_SHELL, $(IAON_BEGIN_CMD))
IAON_LEN = $(call QUIET_SHELL, $(IAON_LEN_CMD))

PRAM_BEGIN  = $(call QUIET_SHELL, $(PRAM_BEGIN_CMD))
PRAM_LEN  = $(call QUIET_SHELL, $(PRAM_LEN_CMD))
PRAM_FREE_LEN  = $(call QUIET_SHELL, $(PRAM_FREE_LEN_CMD))

ifeq ($(HOST_OS),Win32)
 $(eval LINK_LD_FLAGS = PRAM_BEGIN=$(PRAM_BEGIN) PRAM_LEN=$(PRAM_LEN) PRAM_FREE_LEN=$(PRAM_FREE_LEN) IRAM_BEGIN=$(IRAM_BEGIN) IRAM_LEN=$(IRAM_LEN) IROM_BEGIN=$(IROM_BEGIN) IROM_LEN=$(IROM_LEN) IAON_BEGIN=$(IAON_BEGIN) IAON_LEN=$(IAON_LEN) )
else
 LINK_LOC_CMD   := $(PERL) -e 'printf("PRAM_BEGIN=$(PRAM_BEGIN) PRAM_LEN=$(PRAM_LEN) PRAM_FREE_LEN=$(PRAM_FREE_LEN) IRAM_BEGIN=$(IRAM_BEGIN) IRAM_LEN=$(IRAM_LEN) IROM_BEGIN=$(IROM_BEGIN) IROM_LEN=$(IROM_LEN) IAON_BEGIN=$(IAON_BEGIN) IAON_LEN=$(IAON_LEN)")'
 LINK_LOC_CMD   :=$(call PERL_FIX_QUOTES,$(LINK_LOC_CMD))
 LINK_LD_FLAGS = $(call QUIET_SHELL, $(LINK_LOC_CMD))
endif

# Add overlay info to linker script if required.
ifneq ($(OVERLAY_SRC),)
  OVERLAY_FLAGS += OVERLAY_AREA_LENGTH=$(OVERLAY_AREA_LENGTH)
  APP_SRC += spar_overlay_manager.c
else
  OVERLAY_FLAGS += OVERLAY_AREAS=
endif

################################################################################
# General compiler flags
################################################################################
CPP_FLAGS      += -fno-exceptions -fno-rtti

# Common C flags
COMMON_FLAGS    =  -c -DCOMPILER_ARM $(CORE_COMMON_OPTIONS) -Os -g -Wa,-adhln -ffunction-sections \
					  -ffreestanding -DSPAR_CRT_SETUP=$(SPAR_CRT_SETUP) -DSPAR_APP_SETUP=$(SPAR_APP_SETUP)

# Add some more hacky flags
COMMON_FLAGS += -D__TARGET_CPU_CORTEX_M4 -D__ARMCC_VERSION=400677

# Add the common C flags for the chip
C_SPECIFIC_FLAGS   += -funsigned-char -fshort-wchar @$(PATCH_ROOT_DIR)/$(TC)/$(CHIP)$(REVNUM).cflag -DTOOLCHAIN_wiced=1

################################################################################
# General assembler flags
################################################################################
A_SPECIFIC_FLAGS +=

################################################################################
# Final linker, compiler and assembler options
################################################################################
C_FLAGS += $(C_SPECIFIC_FLAGS) $(COMMON_FLAGS) $(INCS_OPTS)
ASM_FLAGS += $(COMMON_FLAGS) $(A_SPECIFIC_FLAGS) $(INCS_OPTS)
LIBS_LINKER_OPTIONS = --start-group $(WICED_SDK_LINK_FILES) $(LINK_LIBS) -lgcc -lc --end-group
################################################################################
# Sections extraction, foo.elf -> foo.elf.text.hex etc., to make a CGS
################################################################################
ifneq ($(SPAR_IN), flash)
# enable compressed sections if compression is enabled.
ifeq ($(COMPRESSION_ENABLED),y)
  UNCOMPRESSED_SECTIONS +=
  COMPRESSED_SECTIONS += .text .rodata .data .setup .aon .pram_text .pram_rodata
else
# We will not extract sections for a flash image, else the only sections we want to extract
# Keep all sections uncompressed by default
  UNCOMPRESSED_SECTIONS +=  .text .rodata .data .setup .aon .pram_text .pram_rodata

  COMPRESSED_SECTIONS +=
endif
ifneq ($(OVERLAY_SRC),)
  # Add the overlaid sections to extract
  OVERLAID_SECTIONS += $(addprefix .,$(basename $(OVERLAY_SRC)))
endif
endif

ELF_OUT_LIST = $(ELF_OUT:.elf=.list)

