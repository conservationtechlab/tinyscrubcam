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

################################################################################
# CM3 specific spar rules
################################################################################
#
# GCC make steps:
#
# Common
# 1) Build Source files - recipes in the bottom of spar/makefile
# 2) Build ELF file     - recipe $(ELF_OUT) in of spar/makefile
#                         (This also builds a symbol list file output $(ELF_OUT_LIST))
#
# Configs Target (cgs)
# 3) For each section in $(UNCOMPRESSED_SECTIONS) and $(COMPRESSED)
#        -  use section_to_cgs script to extract the bytes in cgs format, compressing it or overlaying it based on how the section needs to be handled.
#              recipe $(ELF_OUT)%.cgs (regular section) or $(ELF_OUT)%.ccgs (compressed section) or $(ELF_OUT)%.ocgs (overlaid section)
# 4) Use the setup_call_cgs script to create a config for the entry point - file extension .elf.spar_setup_call.cgs (recipe $(SPAR_SETUP_CALL_CGS) in this file)
# 5) Concatenate all the config files together into the output config (recipe $(SPAR_CGS_TARGET) in spar/makefile)
# 6) Print statistics (recipe print_stats in this file)
#
#
# Hex Target (hex)
# 3) Use mergearmhex script to merge hex data for elf files (recipe $(HEX_TARGET) in spar/makefile)
#
################################################################################
# automatically generate dependency rules
MAKE_DEP_FILE = $(XCC) $(C_FLAGS) $(LIST_OPTIONS) -MM -MP -MF $(@:$(suffix $@)=.d) $<

# The assembly listing generated goes to stdout. Redirect to file.
REDIRECT_OUTPUT = > $(@:$(suffix $@)=.s)

ifeq ($(HOST_OS),Win32)
REDIRECT_ASM_OUTPUT = > NUL
else
REDIRECT_ASM_OUTPUT = > /dev/null
endif


.INTERMEDIATE: $(SPAR_SETUP_CALL_CGS)

# The list file is really a side effect of building the elf
$(ELF_OUT_LIST): $(ELF_OUT)

#$(info READ_ELF: $(READ_ELF))

$(SPAR_SETUP_CALL_CGS): $(ELF_OUT)
#	$(info SETUP_CALL_SCRIPT $(SETUP_CALL_SCRIPT))
#	$(info SPAR_CRT_SETUP $(SPAR_CRT_SETUP))
#	$(info READ_ELF $(READ_ELF) )
	$(QUIET)-$(XMD) $(@D)
	$(QUIET)$(PERL) $(SETUP_CALL_SCRIPT) $(ELF_OUT) $(READ_ELF) $(SPAR_CRT_SETUP)

# elf -> hex, in case missing; covering tier1, tier2, and any SPARs
%.hex: %.elf
	$(QUIET)-$(XMD) $(@D)
	$(QUIET)$(HEXCOPY) -O ihex $< $@

ELF_OUT_BIN = $(ELF_OUT).bin

ifneq ($(DIRECT_LOAD),)
  SECTION_TO_CGS_DL_OPTION = -d
else
  SECTION_TO_CGS_DL_OPTION =
endif

# From spar.elf, make spar.elf.SECTION.cgs, or a comment only if empty.
$(ELF_OUT)%.cgs: $(ELF_OUT)
	$(QUIET)-$(XMD) $(@D)
#	$(info OBJDUMP: $(OBJDUMP) )
#	$(info section to CGS $(SECTION_TO_CGS): $<)
	$(eval VERBOSE_TRACE = $(if $(findstring 1,$(VERBOSE)),-v 1,))
	$(QUIET)$(PERL) $(SECTION_TO_CGS) -e $< -o $(OBJDUMP) -s $* $(SECTION_TO_CGS_DL_OPTION) $(VERBOSE_TRACE) > $@

# From spar.elf, make spar.elf.SECTION.ccgs, or a comment only if empty
$(ELF_OUT)%.ccgs: $(ELF_OUT)
	$(QUIET)-$(XMD) $(@D)
#	$(info Section to CCGS $(SECTION_TO_CGS))
	$(eval VERBOSE_TRACE = $(if $(findstring 1,$(VERBOSE)),-v 1,))
	$(QUIET)$(PERL) $(SECTION_TO_CGS) -e $< -o $(OBJDUMP) -s $* -c $(COMPRESSOR) $(VERBOSE_TRACE)  > $@

# From spar.elf.xyz.hex, make spar.elf.SECTION.cgs, or a comment only if empty
$(ELF_OUT)%.ocgs: $(ELF_OUT)
	$(QUIET)-$(XMD) $(@D)
	$(eval VERBOSE_TRACE = $(if $(findstring 1,$(VERBOSE)),-v 1,))
	$(QUIET)$(PERL) $(SECTION_TO_CGS) -e $< -o $(OBJDUMP) -s $* -i $(word 2,$(subst _, ,$*)) $(VERBOSE_TRACE) > $@

# If we have overlays enabled, we need to place them somewhere. Autogenerate this scatter file.
OVERLAY_SCAT = $(BIN_OUT_DIR)/spar_ram_overlays.ld

# Make sure that the overlaid objects are always in a known order. All the autogenerated code
# depends on the order being known at compile time. So, sort the overlaid sources into objs.
# We don't quite care what order, as long as it is deterministic.
OVERLAY_OBJS = $(addsuffix .o,$(sort $(basename $(OVERLAY_SRC))))

# Generate trampoline functions for each exported function in the overlaid object file.
#   - Get all the exported function names in the object file using NM
#   - Append --wrap <function> for each function to wrap_flags.inc
#   - Invoke $(GEN_OVERLAY_FILES) and generate trampoline functions for each function in the object.
ifneq ($(OVERLAY_SRC),)
$(BIN_OUT_DIR)/trampoline_%.s: $(BIN_OUT_DIR)/%.o
	$(eval FUNCTIONS = $(call QUIET_SHELL, $(call PERL_FIX_QUOTES,$(PERL) -e 'open(FL, "$(NM) --defined-only -s -g $(subst /,\,$<) |");while(<FL>){chomp $$_;if($$_=~/[a-fA-F0-9]+\s+T\s+(.*)/){print " ".$$1;}}close(FL);')))
	$(QUIET)$(ECHO) "Generating trampoline functions for overlaid area $*...";
	$(QUIET)$(PERL) $(PRINT_WRAP_FLAGS) $(FUNCTIONS) >> $(BIN_OUT_DIR)/wrap_flags.inc
	$(QUIET)$(PERL) $(GEN_OVERLAY_FILES) -gen=tramp -tc=gcc -file=$< $(FUNCTIONS) > $@

# Add the trampoline function objects to files to be linked.
OBJS += $(addsuffix .o, $(addprefix $(BIN_OUT_DIR)/trampoline_, $(sort $(basename $(OVERLAY_SRC)))))

# Get the linker to pick up symbols to be wrapped from wrap_flags.inc
EXTRA_LD_FLAGS += @$(BIN_OUT_DIR)/wrap_flags.inc
endif

# If overlays are enabled, invoke $(GEN_OVERLAY_FILES) and generate execution regions in
# the main scatter file for each overlaid object file.
#$(info OVERLAY_SRC $(OVERLAY_SRC))
ifneq ($(OVERLAY_SRC),)
$(OVERLAY_SCAT): $(OBJS)
	$(QUIET)$(PERL) $(GEN_OVERLAY_FILES) -gen=scat -tc=gcc -dir=$(dir $(ELF_OUT)) $(OVERLAY_OBJS) > $@
else
$(OVERLAY_SCAT): $(OBJS)
	$(QUIET)$(ECHO)   > $@
endif

# Always autogenerate the library installers
#OBJS += $(BIN_OUT_DIR)/lib_installer.o

#$(BIN_OUT_DIR)/lib_installer.o: $(BIN_OUT_DIR)/lib_installer.c
#	$(info processing lib_installer file)

$(OUTPUT_DIR)/Modules/lib_installer.c: $(LIBS) $(filter-out $(BIN_OUT_DIR)/lib_installer.o,$(OBJS))
	$(info processing lib_installer)
#	$(info GEN_LIB_INSTALLER $(GEN_LIB_INSTALLER))
#	$(info FROMELF: $(FROMELF))
	$(eval VERBOSE_TRACE = $(if $(findstring 1,$(VERBOSE)),-v=1,))
	$(QUIET)$(PERL) $(GEN_LIB_INSTALLER) $(VERBOSE_TRACE) -tc=gcc -tool=$(OBJDUMP) $^ > $@


IAON_BEGIN_CMD :=$(call PERL_FIX_QUOTES,$(IAON_BEGIN_CMD))
IAON_LEN_CMD   :=$(call PERL_FIX_QUOTES,$(IAON_LEN_CMD))

# Force pre-processing the scatter-loader file - Getting linker to do this
# instead does not always work in Cygwin. Needs to be generated everytime before link
# The memory loacations are calculated from the base elf.
#$(SPAR_LINK_LOAD_FILE): $(APP_LD_SCRIPT) $(ELF_LIST) $(OBJS) $(ELF_LIST:.elf=.lst) $(OVERLAY_SCAT)
$(SPAR_LINK_LOAD_FILE):  $(APP_LD_SCRIPT) $(ELF_LIST) $(LINK_LIBS) $(PATCH_LST) $(OVERLAY_SCAT)
	$(info Processing SPAR_LINK_LOAD_FILE: $(SPAR_LINK_LOAD_FILE))
	$(info LINK_LD_FLAGS: $(LINK_LD_FLAGS))
	$(info $(CC) -E -x c -P $(addprefix -D,$(LINK_LD_FLAGS)) -I $(dir $(ELF_OUT)) -o $@ $<)
	$(QUIET)$(CC) -E -x c -P $(addprefix -D,$(LINK_LD_FLAGS)) -I $(dir $(ELF_OUT)) -o $@ $<



.PHONY: print_stats
print_stats: $(SPAR_CGS_TARGET)
	$(QUIET)$(PERL) $(PRINT_STATS)  $(ELF_OUT_LIST)  $(ELF_LIST)
