#
# $ Copyright Broadcom Corporation $
#

NAME := BCM4390A1_ROM

ifneq ($(wildcard $(CURDIR)BCM4390A1_ROM.$(HOST_ARCH).$(BUILD_TYPE).a),)
$(NAME)_PREBUILT_LIBRARY := BCM4390A1_ROM.$(HOST_ARCH).$(BUILD_TYPE).a
else
include $(CURDIR)4390A1_src.mk
endif
