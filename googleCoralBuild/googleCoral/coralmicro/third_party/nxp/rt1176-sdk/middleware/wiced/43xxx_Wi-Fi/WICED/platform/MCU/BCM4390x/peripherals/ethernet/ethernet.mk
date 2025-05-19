#
# $ Copyright Broadcom Corporation $
#

NAME = Ethernet_43909_Library_$(PLATFORM)

# Enable ethernet and set maximum used buffer size.
# Buffer size calculated as value stored to GMAC FRM_LENGTH register (&regs->rxmaxlength), increased on HWRXOFF (30) to accomodate status header.
GLOBAL_DEFINES := WICED_USE_ETHERNET_INTERFACE \
                  WICED_ETHERNET_BUFFER_SIZE=1580

$(NAME)_SOURCES := etc.c \
                   etcgmac.c \
                   platform_ethernet.c

$(eval $(call PLATFORM_LOCAL_DEFINES_INCLUDES_43909, ../..))
