#
# $ Copyright Broadcom Corporation $
#

NAME = Peripherals_TLSF_43909_Library_$(PLATFORM)

$(NAME)_SOURCES := tlsf.c

$(eval $(call PLATFORM_LOCAL_DEFINES_INCLUDES_43909, ../..))
