# Makefile fragment for generating native .mpy files from C source
# MPY_DIR must be set to the top of the MicroPython source tree

BUILD ?= build

ECHO = @echo
RM = /bin/rm
MKDIR = /bin/mkdir
PYTHON = python3
MPY_CROSS = $(MPY_DIR)/mpy-cross/build/mpy-cross
MPY_TOOL = $(PYTHON) $(MPY_DIR)/tools/mpy-tool.py
MPY_LD = $(PYTHON) $(MPY_DIR)/tools/mpy_ld.py

Q = @
ifeq ("$(origin V)", "command line")
ifeq ($(V),1)
Q =
MPY_LD += '-vvv'
endif
endif

ARCH_UPPER = $(shell echo $(ARCH) | tr '[:lower:]' '[:upper:]')
CONFIG_H = $(BUILD)/$(MOD).config.h

CFLAGS += -I. -I$(MPY_DIR)
CFLAGS += -std=c99
CFLAGS += -Os
CFLAGS += -Wall -Werror -DNDEBUG
CFLAGS += -DNO_QSTR
CFLAGS += -DMICROPY_ENABLE_DYNRUNTIME
CFLAGS += -DMP_CONFIGFILE='<$(CONFIG_H)>'
CFLAGS += -fpic -fno-common
CFLAGS += -U _FORTIFY_SOURCE # prevent use of __*_chk libc functions
#CFLAGS += -fdata-sections -ffunction-sections

MPY_CROSS_FLAGS += -march=$(ARCH)

SRC_O += $(addprefix $(BUILD)/, $(patsubst %.c,%.o,$(filter %.c,$(SRC))) $(patsubst %.S,%.o,$(filter %.S,$(SRC))))
SRC_MPY += $(addprefix $(BUILD)/, $(patsubst %.py,%.mpy,$(filter %.py,$(SRC))))

################################################################################
# Architecture configuration

ifeq ($(ARCH),x86)

# x86
CROSS =
CFLAGS += -m32 -fno-stack-protector
MICROPY_FLOAT_IMPL ?= double

else ifeq ($(ARCH),x64)

# x64
CROSS =
CFLAGS += -fno-stack-protector
MICROPY_FLOAT_IMPL ?= double

else ifeq ($(ARCH),armv6m)

# thumb
CROSS = arm-none-eabi-
CFLAGS += -mthumb -mcpu=cortex-m0
MICROPY_FLOAT_IMPL ?= none

else ifeq ($(ARCH),armv7m)

# thumb
CROSS = arm-none-eabi-
CFLAGS += -mthumb -mcpu=cortex-m3
MICROPY_FLOAT_IMPL ?= none

else ifeq ($(ARCH),armv7emsp)

# thumb
CROSS = arm-none-eabi-
CFLAGS += -mthumb -mcpu=cortex-m4
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
MICROPY_FLOAT_IMPL ?= float

else ifeq ($(ARCH),armv7emdp)

# thumb
CROSS = arm-none-eabi-
CFLAGS += -mthumb -mcpu=cortex-m7
CFLAGS += -mfpu=fpv5-d16 -mfloat-abi=hard
MICROPY_FLOAT_IMPL ?= double

else ifeq ($(ARCH),xtensa)

# xtensa
CROSS = xtensa-lx106-elf-
CFLAGS += -mforce-l32
MICROPY_FLOAT_IMPL ?= none

else ifeq ($(ARCH),xtensawin)

# xtensawin
CROSS = xtensa-esp32-elf-
CFLAGS +=
MICROPY_FLOAT_IMPL ?= float

else
$(error architecture '$(ARCH)' not supported)
endif

MICROPY_FLOAT_IMPL_UPPER = $(shell echo $(MICROPY_FLOAT_IMPL) | tr '[:lower:]' '[:upper:]')
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_$(MICROPY_FLOAT_IMPL_UPPER)

CFLAGS += $(CFLAGS_EXTRA)

################################################################################
# Build rules

.PHONY: all clean

all: $(MOD).mpy

clean:
	$(RM) -rf $(BUILD) $(CLEAN_EXTRA)

# Create build destination directories first
BUILD_DIRS = $(sort $(dir $(CONFIG_H) $(SRC_O) $(SRC_MPY)))
$(CONFIG_H) $(SRC_O) $(SRC_MPY): | $(BUILD_DIRS)
$(BUILD_DIRS):
	$(Q)$(MKDIR) -p $@

# Preprocess all source files to generate $(CONFIG_H)
$(CONFIG_H): $(SRC)
	$(ECHO) "GEN $@"
	$(Q)$(MPY_LD) --arch $(ARCH) --preprocess -o $@ $^

# Build .o from .c source files
$(BUILD)/%.o: %.c $(CONFIG_H) Makefile
	$(ECHO) "CC $<"
	$(Q)$(CROSS)gcc $(CFLAGS) -o $@ -c $<

# Build .o from .S source files
$(BUILD)/%.o: %.S $(CONFIG_H) Makefile
	$(ECHO) "AS $<"
	$(Q)$(CROSS)gcc $(CFLAGS) -o $@ -c $<

# Build .mpy from .py source files
$(BUILD)/%.mpy: %.py
	$(ECHO) "MPY $<"
	$(Q)$(MPY_CROSS) $(MPY_CROSS_FLAGS) -o $@ $<

# Build native .mpy from object files
$(BUILD)/$(MOD).native.mpy: $(SRC_O)
	$(ECHO) "LINK $<"
	$(Q)$(MPY_LD) --arch $(ARCH) --qstrs $(CONFIG_H) -o $@ $^

# Build final .mpy from all intermediate .mpy files
$(MOD).mpy: $(BUILD)/$(MOD).native.mpy $(SRC_MPY)
	$(ECHO) "GEN $@"
	$(Q)$(MPY_TOOL) --merge -o $@ $^
