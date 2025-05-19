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

GENERATED_MAC_FILE   := $(SOURCE_ROOT)generated_mac_address.txt
MAC_GENERATOR        := $(TOOLS_ROOT)/mac_generator/mac_generator.pl

$(GENERATED_MAC_FILE): $(MAC_GENERATOR)
	$(QUIET)$(PERL) $<  > $@

EXTRA_PRE_BUILD_TARGETS  += $(GENERATED_MAC_FILE)

CUSTOM_CONFIG += bt_config

$(CUSTOM_CONFIG): $(CONFIG_FILE)
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,ROUTE_TO_CGS_HEX := $(ROUTE_TO_CGS_HEX))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,HOST_MCU_FAMILY := $(HOST_MCU_FAMILY))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,HEX_OUTPUT_SUFFIX := $(HEX_OUTPUT_SUFFIX))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,CGS_OUTPUT_SUFFIX := $(CGS_OUTPUT_SUFFIX))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,LINK_OUTPUT_FILE := $(LINK_OUTPUT_FILE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,PLATFORM_CONFIGS := $(PLATFORM_CONFIGS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,BASE_IN := $(BASE_IN))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,SPAR_IN := $(SPAR_IN))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,SPAR_CRT_SETUP := $(SPAR_CRT_SETUP))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,SPAR_APP_SETUP := $(SPAR_APP_SETUP))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,LINKER := $(LINKER))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,CUSTOM_LD_FLAGS := $(CUSTOM_LD_FLAGS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,BUILD_TYPE := $(BUILD_TYPE))

