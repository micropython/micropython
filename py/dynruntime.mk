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

CFLAGS_ARCH += -fpic -fno-common
CFLAGS_ARCH += -U_FORTIFY_SOURCE # prevent use of __*_chk libc functions
#CFLAGS_ARCH += -fdata-sections -ffunction-sections

MPY_CROSS_FLAGS += -march=$(ARCH)

SRC_O += $(addprefix $(BUILD)/, $(patsubst %.c,%.o,$(filter %.c,$(SRC))) $(patsubst %.S,%.o,$(filter %.S,$(SRC))))
SRC_MPY += $(addprefix $(BUILD)/, $(patsubst %.py,%.mpy,$(filter %.py,$(SRC))))

CLEAN_EXTRA += $(MOD).mpy .mpy_ld_cache

################################################################################
# Architecture configuration

ifeq ($(ARCH),x86)

# x86
CROSS =
CFLAGS_ARCH += -m32 -fno-stack-protector
MICROPY_FLOAT_IMPL ?= double

else ifeq ($(ARCH),x64)

# x64
CROSS =
CFLAGS_ARCH += -fno-stack-protector
MICROPY_FLOAT_IMPL ?= double

else ifeq ($(ARCH),armv6m)

# thumb
CROSS = arm-none-eabi-
CFLAGS_ARCH += -mthumb -mcpu=cortex-m0
MICROPY_FLOAT_IMPL ?= none

else ifeq ($(ARCH),armv7m)

# thumb
CROSS = arm-none-eabi-
CFLAGS_ARCH += -mthumb -mcpu=cortex-m3
MICROPY_FLOAT_IMPL ?= none

else ifeq ($(ARCH),armv7emsp)

# thumb
CROSS = arm-none-eabi-
CFLAGS_ARCH += -mthumb -mcpu=cortex-m4
CFLAGS_ARCH += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
MICROPY_FLOAT_IMPL ?= float

else ifeq ($(ARCH),armv7emdp)

# thumb
CROSS = arm-none-eabi-
CFLAGS_ARCH += -mthumb -mcpu=cortex-m7
CFLAGS_ARCH += -mfpu=fpv5-d16 -mfloat-abi=hard
MICROPY_FLOAT_IMPL ?= double

else ifeq ($(ARCH),xtensa)

# xtensa
CROSS = xtensa-lx106-elf-
CFLAGS_ARCH += -mforce-l32
MICROPY_FLOAT_IMPL ?= none

else ifeq ($(ARCH),xtensawin)

# xtensawin
CROSS = xtensa-esp32-elf-
MICROPY_FLOAT_IMPL ?= float

else ifeq ($(ARCH),rv32imc)

# rv32imc
CROSS = riscv64-unknown-elf-
CFLAGS_ARCH += -march=rv32imac -mabi=ilp32 -mno-relax
# If Picolibc is available then select it explicitly.  Ubuntu 22.04 ships its
# bare metal RISC-V toolchain with Picolibc rather than Newlib, and the default
# is "nosys" so a value must be provided.  To avoid having per-distro
# workarounds, always select Picolibc if available.
PICOLIBC_SPECS := $(shell $(CROSS)gcc --print-file-name=picolibc.specs)
ifneq ($(PICOLIBC_SPECS),picolibc.specs)
CFLAGS_ARCH += -specs=$(PICOLIBC_SPECS)
USE_PICOLIBC := 1
PICOLIBC_ARCH := rv32imac
PICOLIBC_ABI := ilp32
endif

MICROPY_FLOAT_IMPL ?= none

else
$(error architecture '$(ARCH)' not supported)
endif

MICROPY_FLOAT_IMPL_UPPER = $(shell echo $(MICROPY_FLOAT_IMPL) | tr '[:lower:]' '[:upper:]')
CFLAGS += $(CFLAGS_ARCH) -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_$(MICROPY_FLOAT_IMPL_UPPER)

ifeq ($(LINK_RUNTIME),1)
# All of these picolibc-specific directives are here to work around a
# limitation of Ubuntu 22.04's RISC-V bare metal toolchain.  In short, the
# specific version of GCC in use (10.2.0) does not seem to take into account
# extra paths provided by an explicitly passed specs file when performing name
# resolution via `--print-file-name`.
#
# If Picolibc is used and libc.a fails to resolve, then said file's path will
# be computed by searching the Picolibc libraries root for a libc.a file in a
# subdirectory whose path is built using the current `-march` and `-mabi`
# flags that are passed to GCC.  The `PICOLIBC_ROOT` environment variable is
# checked to override the starting point for the library file search, and if
# it is not set then the default value is used, assuming that this is running
# on an Ubuntu 22.04 machine.
#
# This should be revised when the CI base image is updated to a newer Ubuntu
# version (that hopefully contains a newer RISC-V compiler) or to another Linux
# distribution.
ifeq ($(USE_PICOLIBC),1)
LIBM_NAME := libc.a
else
LIBM_NAME := libm.a
endif
LIBGCC_PATH := $(realpath $(shell $(CROSS)gcc $(CFLAGS) --print-libgcc-file-name))
LIBM_PATH := $(realpath $(shell $(CROSS)gcc $(CFLAGS) --print-file-name=$(LIBM_NAME)))
ifeq ($(USE_PICOLIBC),1)
ifeq ($(LIBM_PATH),)
# The CROSS toolchain prefix usually ends with a dash, but that may not be
# always the case.  If the prefix ends with a dash it has to be taken out as
# Picolibc's architecture directory won't have it in its name.  GNU Make does
# not have any facility to perform character-level text manipulation so we
# shell out to sed.
CROSS_PREFIX := $(shell echo $(CROSS) | sed -e 's/-$$//')
PICOLIBC_ROOT ?= /usr/lib/picolibc/$(CROSS_PREFIX)/lib
LIBM_PATH := $(PICOLIBC_ROOT)/$(PICOLIBC_ARCH)/$(PICOLIBC_ABI)/$(LIBM_NAME)
endif
endif
MPY_LD_FLAGS += $(addprefix -l, $(LIBGCC_PATH) $(LIBM_PATH))
endif

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
	$(Q)$(MPY_LD) --arch $(ARCH) --qstrs $(CONFIG_H) $(MPY_LD_FLAGS) -o $@ $^

# Build final .mpy from all intermediate .mpy files
$(MOD).mpy: $(BUILD)/$(MOD).native.mpy $(SRC_MPY)
	$(ECHO) "GEN $@"
	$(Q)$(MPY_TOOL) --merge -o $@ $^
