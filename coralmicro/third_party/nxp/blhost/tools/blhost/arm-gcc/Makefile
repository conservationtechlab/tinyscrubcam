#-----------------------------------------------
# Make command:
# make build=<build> cpu=<cpu> all
# <build>: debug or release, release by default.
# <cpu>: cpu architecure, cortex-a9 by default.
#-----------------------------------------------

#-----------------------------------------------
# setup variables
# ----------------------------------------------

BOOT_ROOT := $(abspath ../../..)
OUTPUT_ROOT := $(abspath ./)

APP_NAME = blhost

#-----------------------------------------------
# Toolchain
# ----------------------------------------------
# GNU Cross Compiler (No linux lib is available)
#PREFIX := arm-none-eabi-
# Linaro Linux-Tageted GNU C Cross Compiler (libudev is not available)
PREFIX := arm-linux-gnueabi-
# Freescale i.MX GNU C Cross Compiler
#PREFIX := arm-poky-linux-gnueabi-
#SYSROOT := /opt/fsl-imx-xwayland/5.4-zeus/sysroots/cortexa9t2hf-neon-poky-linux-gnueabi

ifneq ($(filter X86%, $(shell uname -m | tr a-z A-Z)),)
# Cross Compiling
CC := $(PREFIX)gcc
CPP := $(PREFIX)cpp
CXX := $(PREFIX)g++
AS := $(PREFIX)as
# DO NOT use ld.
LD := $(PREFIX)g++
else
# DO NOT use ld.
LD := g++
endif

#-----------------------------------------------
# Architecture
# cortex-a9 by default
#-----------------------------------------------
CPU ?= cortex-a9

#-----------------------------------------------
# Debug or Release
# Release by default
#-----------------------------------------------
build ?= release

include $(BOOT_ROOT)/mk/common.mk

#-----------------------------------------------
# Include path. Add the include paths like this:
# INCLUDES += ./include/
#-----------------------------------------------
INCLUDES += $(BOOT_ROOT)/tools/blhost/src \
			$(BOOT_ROOT)/src \
			$(BOOT_ROOT)/src/include \
			$(BOOT_ROOT)/src/blfwk \
			$(BOOT_ROOT)/src/sbloader \
			$(BOOT_ROOT)/src/bootloader \
			$(BOOT_ROOT)/src/crc \
			$(BOOT_ROOT)/src/packet \
			$(BOOT_ROOT)/src/property \
			$(BOOT_ROOT)/src/drivers/common \
			$(BOOT_ROOT)/src/bm_usb

CXXFLAGS := -D LINUX -D BOOTLOADER_HOST -D __ARM__ -std=c++11
CFLAGS   := -D LINUX -D BOOTLOADER_HOST -D __ARM__ -D _GNU_SOURCE -std=c99
LIBS     :=

SOURCES := $(BOOT_ROOT)/tools/blhost/src/blhost.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/Blob.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/Bootloader.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/BusPal.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/BusPalPeripheral.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/Command.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/DataSource.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/DataSourceImager.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/DataTarget.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/ELFSourceFile.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/ExcludesListMatcher.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/format_string.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/GHSSecInfo.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/GlobMatcher.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/hid-linux.c \
		   $(BOOT_ROOT)/src/blfwk/src/jsoncpp.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/Logging.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/options.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/SBSourceFile.cpp  \
		   $(BOOT_ROOT)/src/blfwk/src/SearchPath.cpp  \
		   $(BOOT_ROOT)/src/blfwk/src/serial.c \
		   $(BOOT_ROOT)/src/blfwk/src/SerialPacketizer.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/SourceFile.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/SRecordSourceFile.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/IntelHexSourceFile.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/StELFFile.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/StExecutableImage.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/StSRecordFile.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/StIntelHexFile.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/Updater.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/UartPeripheral.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/UsbHidPacketizer.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/UsbHidPeripheral.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/i2c.c \
		   $(BOOT_ROOT)/src/blfwk/src/I2cPeripheral.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/spi.c \
		   $(BOOT_ROOT)/src/blfwk/src/SpiPeripheral.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/utils.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/Value.cpp \
		   $(BOOT_ROOT)/src/crc/src/crc16.c \
		   $(BOOT_ROOT)/src/crc/src/crc32.c 

# LPC USB Serial I/O is not supported by ARM Linux blhost.
#SOURCES += $(BOOT_ROOT)/src/blfwk/src/LpcUsbSio.cpp \
		   $(BOOT_ROOT)/src/blfwk/src/LpcUsbSioPeripheral.cpp
#CFLAGS += -D LPCUSBSIO
#CXXFLAGS += -D LPCUSBSIO
#LIBS += $(BOOT_ROOT)/src/middleware/lpcusbsio/bin/arm_linux/lpcusbsio.a

INCLUDES := $(foreach includes, $(INCLUDES), -I $(includes))

ifeq "$(build)" "debug"
DEBUG_OR_RELEASE := Debug
CFLAGS += -g
CXXFLAGS += -g
LDFLAGS += -g
else
DEBUG_OR_RELEASE := Release
endif

ifneq ($(filter X86%, $(shell uname -m | tr a-z A-Z)),)
# GNU Cross Compiler
# Linaro Linux-Tageted GNU C Cross Compiler
CFLGAS += -mcpu=$(CPU)
CXXFLGAS += -mcpu=$(CPU)
LDFLGAS += -mcpu=$(CPU)
# Freescale i.MX GNU C Cross Compiler
# CFLGAS += -mfloat-abi=hard -mcpu=$(CPU) --sysroot=$(SYSROOT)
# CXXFLGAS += -mfloat-abi=hard -mcpu=$(CPU) --sysroot=$(SYSROOT)
# LDFLGAS += -mfloat-abi=hard -mcpu=$(CPU) --sysroot=$(SYSROOT)
endif

TARGET_OUTPUT_ROOT := $(OUTPUT_ROOT)/$(DEBUG_OR_RELEASE)
MAKE_TARGET := $(TARGET_OUTPUT_ROOT)/$(APP_NAME)

OBJS_ROOT = $(TARGET_OUTPUT_ROOT)/obj

# Strip sources.
SOURCES := $(strip $(SOURCES))

# Convert sources list to absolute paths and root-relative paths.
SOURCES_ABS := $(foreach s,$(SOURCES),$(abspath $(s)))
SOURCES_REL := $(subst $(BOOT_ROOT)/,,$(SOURCES_ABS))

# Get a list of unique directories containing the source files.
SOURCE_DIRS_ABS := $(sort $(foreach f,$(SOURCES_ABS),$(dir $(f))))
SOURCE_DIRS_REL := $(subst $(BOOT_ROOT)/,,$(SOURCE_DIRS_ABS))

OBJECTS_DIRS := $(addprefix $(OBJS_ROOT)/,$(SOURCE_DIRS_REL))

# Filter source files list into separate source types.
C_SOURCES = $(filter %.c,$(SOURCES_REL))
CXX_SOURCES = $(filter %.cpp,$(SOURCES_REL))
ASM_s_SOURCES = $(filter %.s,$(SOURCES_REL))
ASM_S_SOURCES = $(filter %.S,$(SOURCES_REL))

# Convert sources to objects.
OBJECTS_C := $(addprefix $(OBJS_ROOT)/,$(C_SOURCES:.c=.o))
OBJECTS_CXX := $(addprefix $(OBJS_ROOT)/,$(CXX_SOURCES:.cpp=.o))
OBJECTS_ASM := $(addprefix $(OBJS_ROOT)/,$(ASM_s_SOURCES:.s=.o))
OBJECTS_ASM_S := $(addprefix $(OBJS_ROOT)/,$(ASM_S_SOURCES:.S=.o))

# Complete list of all object files.
OBJECTS_ALL := $(sort $(OBJECTS_C) $(OBJECTS_CXX) $(OBJECTS_ASM) $(OBJECTS_ASM_S))

#-------------------------------------------------------------------------------
# Default target
#-------------------------------------------------------------------------------

# Note that prerequisite order is important here. The subdirectories must be built first, or you
# may end up with files in the current directory not getting added to libraries. This would happen
# if subdirs modified the library file after local files were compiled but before they were added
# to the library.
.PHONY: all
all: $(MAKE_TARGET)

## Recipe to create the output object file directories.
$(OBJECTS_DIRS) :
	$(at)mkdir -p $@

# Object files depend on the directories where they will be created.
#
# The dirs are made order-only prerequisites (by being listed after the '|') so they won't cause
# the objects to be rebuilt, as the modification date on a directory changes whenver its contents
# change. This would cause the objects to always be rebuilt if the dirs were normal prerequisites.
$(OBJECTS_ALL): | $(OBJECTS_DIRS)

#-------------------------------------------------------------------------------
# Pattern rules for compilation
#-------------------------------------------------------------------------------
# We cd into the source directory before calling the appropriate compiler. This must be done
# on a single command line since make calls individual recipe lines in separate shells, so
# '&&' is used to chain the commands.
#
# Generate make dependencies while compiling using the -MMD option, which excludes system headers.
# If system headers are included, there are path problems on cygwin. The -MP option creates empty
# targets for each header file so that a rebuild will be forced if the file goes missing, but
# no error will occur.

# Compile C sources.
$(OBJS_ROOT)/%.o: $(BOOT_ROOT)/%.c
	@$(call printmessage,c,Compiling, $(subst $(BOOT_ROOT)/,,$<))
	$(at)$(CC) $(CFLAGS) $(SYSTEM_INC) $(INCLUDES) $(DEFINES) -MMD -MF $(basename $@).d -MP -o $@ -c $<

# Compile C++ sources.
$(OBJS_ROOT)/%.o: $(BOOT_ROOT)/%.cpp
	@$(call printmessage,cxx,Compiling, $(subst $(BOOT_ROOT)/,,$<))
	$(at)$(CXX) $(CXXFLAGS) $(SYSTEM_INC) $(INCLUDES) $(DEFINES) -MMD -MF $(basename $@).d -MP -o $@ -c $<

# For .S assembly files, first run through the C preprocessor then assemble.
$(OBJS_ROOT)/%.o: $(BOOT_ROOT)/%.S
	@$(call printmessage,asm,Assembling, $(subst $(BOOT_ROOT)/,,$<))
	$(at)$(CPP) -D__LANGUAGE_ASM__ $(INCLUDES) $(DEFINES) -o $(basename $@).s $< \
	&& $(AS) $(ASFLAGS) $(INCLUDES) -MD $(OBJS_ROOT)/$*.d -o $@ $(basename $@).s

# Assembler sources.
$(OBJS_ROOT)/%.o: $(BOOT_ROOT)/%.s
	@$(call printmessage,asm,Assembling, $(subst $(BOOT_ROOT)/,,$<))
	$(at)$(AS) $(ASFLAGS) $(INCLUDES) -MD $(basename $@).d -o $@ $<

#------------------------------------------------------------------------
# Build the tagrget
#------------------------------------------------------------------------

# Wrap the link objects in start/end group so that ld re-checks each
# file for dependencies.  Otherwise linking static libs can be a pain
# since order matters.
$(MAKE_TARGET): $(OBJECTS_ALL)
	@$(call printmessage,link,Linking, $(APP_NAME))
	$(at)$(LD) $(LDFLAGS) \
          $(OBJECTS_ALL) $(LIBS) \
          -lc -lstdc++ -lm -ludev \
          -o $@
	@echo "Output binary:" ; echo "  $(APP_NAME)"

#-------------------------------------------------------------------------------
# Clean
#-------------------------------------------------------------------------------
.PHONY: clean cleanall
cleanall: clean
clean:
	$(at)rm -rf $(OBJECTS_ALL) $(OBJECTS_DIRS) $(MAKE_TARGET) $(APP_NAME)

# Include dependency files.
-include $(OBJECTS_ALL:.o=.d)

