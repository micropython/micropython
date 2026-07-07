# Makefile fragment for generating native .mpy files from C source
# MPY_DIR must be set to the top of the MicroPython source tree

BUILD ?= build-$(ARCH)
CC = $(CROSS)gcc

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

CLEAN_EXTRA += $(MOD).mpy .mpy_ld_cache-$(ARCH)

################################################################################
# Architecture configuration

ifeq ($(ARCH),x86)

# x86
CROSS = i686-linux-gnu-
CFLAGS_ARCH += -fno-stack-protector
MICROPY_FLOAT_IMPL ?= double

else ifeq ($(ARCH),x64)

# x64
CROSS = x86_64-linux-gnu-
CFLAGS_ARCH += -fno-stack-protector
MICROPY_FLOAT_IMPL ?= double

else ifeq ($(ARCH),armv6m)

# thumb
CROSS = arm-none-eabi-
CFLAGS_ARCH += -mthumb -mcpu=cortex-m0
MICROPY_FLOAT_IMPL ?= float

else ifeq ($(ARCH),armv7m)

# thumb
CROSS = arm-none-eabi-
CFLAGS_ARCH += -mthumb -mcpu=cortex-m3
MICROPY_FLOAT_IMPL ?= float

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
MICROPY_FLOAT_IMPL ?= none
PICOLIBC_BASE := riscv64-unknown-elf
PICOLIBC_TARGET := rv32imac/ilp32

else ifeq ($(ARCH),rv64imc)

# rv64imc
CROSS = riscv64-unknown-elf-
CFLAGS_ARCH += -march=rv64imac -mabi=lp64 -mno-relax
MICROPY_FLOAT_IMPL ?= none
PICOLIBC_BASE := riscv64-unknown-elf
PICOLIBC_TARGET := rv64imac/lp64

else
$(error architecture '$(ARCH)' not supported)
endif

ifneq ($(findstring -musl,$(shell $(CC) -dumpmachine)),)
USE_MUSL := 1
endif

ifeq ($(ARCH),$(filter $(ARCH),rv32imc rv64imc))
# If Picolibc is available then select it explicitly.  Ubuntu 24.04 ships its
# bare metal RISC-V toolchain with Picolibc rather than Newlib, and the default
# is "nosys" so a value must be provided.  To avoid having per-distro
# workarounds, always select Picolibc if available.
PICOLIBC_SPECS := $(shell $(CC) --print-file-name=picolibc.specs)
ifneq ($(PICOLIBC_SPECS),picolibc.specs)
# LLVM toolchains supporting more than one target seem to ignore the `-march`
# flag passed when looking up the specs file, so if your system has Picolibc
# libraries for more than one architectures supported by the compiler the
# lookup will return the first available file.
#
# For example, on Ubuntu 24.02 if you have both `picolibc-aarch64-linux-gnu`
# and `picolibc-riscv64-unknown-elf` packages installed, the Qualcomm LLVM
# toolchain (which supports both AArch64 and RISC-V 64) will always return the
# AArch64 picolibc specs even when building for RISC-V.
ifeq ($(shell grep -q "$(PICOLIBC_BASE)" "$(PICOLIBC_SPECS)"; echo $$?),0)
CFLAGS_ARCH += -specs=$(PICOLIBC_SPECS)
USE_PICOLIBC := 1
endif
endif
endif

MICROPY_FLOAT_IMPL_UPPER = $(shell echo $(MICROPY_FLOAT_IMPL) | tr '[:lower:]' '[:upper:]')
CFLAGS += $(CFLAGS_ARCH) -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_$(MICROPY_FLOAT_IMPL_UPPER)
CFLAGS += $(CFLAGS_EXTRA)

ifeq ($(LINK_RUNTIME),1)
# All of these picolibc-specific directives are here to work around a
# limitation of Ubuntu 24.04's RISC-V bare metal toolchain.  In short, the
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
# on an Ubuntu 24.04 machine.
#
# This should be revised when the CI base image is updated to a newer Ubuntu
# version (that hopefully contains a newer RISC-V compiler) or to another Linux
# distribution.
ifeq ($(USE_PICOLIBC),1)
LIBM_NAME := libc.a
else ifeq ($(USE_MUSL),1)
LIBM_NAME := libc.a
else
LIBM_NAME := libm.a
endif
# Clang will output the path to libclang_rt.builtins.a instead.  The problem is
# that some symbols are duplicated between the builtins library and libc.a.  In
# these cases let's leave it to the user to figure out how to handle this for
# the time being.
TOOLCHAIN_LIBGCC := $(realpath $(shell $(CC) $(CFLAGS) --print-libgcc-file-name))
ifneq ($(findstring clang,$(shell $(CC) --version)),clang)
LIBGCC_PATH = $(TOOLCHAIN_LIBGCC)
else
ifneq ($(LINK_CLANG_CLANGRT),0)
LIBGCC_PATH = $(TOOLCHAIN_LIBGCC)
else
LIBGCC_PATH =
endif
endif
LIBM_PATH := $(realpath $(shell $(CC) $(CFLAGS) --print-file-name=$(LIBM_NAME)))
ifeq ($(USE_PICOLIBC),1)
ifeq ($(LIBM_PATH),)
PICOLIBC_ROOT ?= /usr/lib/picolibc/$(PICOLIBC_BASE)/lib
LIBM_PATH := $(PICOLIBC_ROOT)/$(PICOLIBC_TARGET)/$(LIBM_NAME)
endif
endif
ifneq ($(LINK_CLANG_LIBC),)
ifeq ($(findstring clang,$(shell $(CC) --version)),clang)
LIBC_PATH := $(realpath $(shell $(CC) $(CFLAGS) --print-file-name=libc.a))
else
LIBC_PATH =
endif
else
LIBC_PATH =
endif
MPY_LD_FLAGS += $(addprefix -l, $(LIBGCC_PATH) $(LIBM_PATH) $(LIBC_PATH))
endif
ifneq ($(MPY_EXTERN_SYM_FILE),)
MPY_LD_FLAGS += --externs "$(realpath $(MPY_EXTERN_SYM_FILE))"
endif
ifneq ($(ARCH_FLAGS),)
MPY_LD_FLAGS += --arch-flags "$(ARCH_FLAGS)"
endif

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
	$(Q)$(CC) $(CFLAGS) -o $@ -c $<

# Build .o from .S source files
$(BUILD)/%.o: %.S $(CONFIG_H) Makefile
	$(ECHO) "AS $<"
	$(Q)$(CC) $(CFLAGS) -o $@ -c $<

# Build .mpy from .py source files
$(BUILD)/%.mpy: %.py
	$(ECHO) "MPY $<"
	$(Q)$(MPY_CROSS) $(MPY_CROSS_FLAGS) -o $@ $<

# Build native .mpy from object files
$(BUILD)/$(MOD).mpy: $(SRC_O)
	$(ECHO) "LINK $<"
	$(Q)$(MPY_LD) --arch $(ARCH) --qstrs $(CONFIG_H) $(MPY_LD_FLAGS) -o $@ $^

# Build final .mpy from all intermediate .mpy files
$(MOD).mpy: $(BUILD)/$(MOD).mpy $(SRC_MPY)
	$(ECHO) "GEN $@"
	$(Q)$(MPY_TOOL) --merge -o $@ $^
