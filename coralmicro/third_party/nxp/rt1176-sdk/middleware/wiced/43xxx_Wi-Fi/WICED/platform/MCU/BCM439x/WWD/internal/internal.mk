#
# $ Copyright Broadcom Corporation $
#

NAME := BCM439x_WWD_Internal_Libraries

BCM439X_INTERNAL_LIBRARY_NAME :=BCM439X_Internal.$(HOST_ARCH).$(BUILD_TYPE).a

ifneq ($(wildcard $(CURDIR)$(BCM439X_INTERNAL_LIBRARY_NAME)),)
$(NAME)_PREBUILT_LIBRARY := $(BCM439X_INTERNAL_LIBRARY_NAME)
else
include $(CURDIR)internal_src.mk
endif
