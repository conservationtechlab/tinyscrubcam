#
# $ Copyright Broadcom Corporation $
#

NAME = Peripherals_Shared_43909_Library_$(PLATFORM)

$(NAME)_SOURCES := m2m_hnddma.c \
                   hnddma.c \
                   hndpmu.c \
                   siutils.c \
                   wiced_osl.c

$(eval $(call PLATFORM_LOCAL_DEFINES_INCLUDES_43909, ../..))
