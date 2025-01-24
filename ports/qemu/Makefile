################################################################################
# Initial setup of Makefile environment

BOARD ?= MPS2_AN385
BOARD_DIR ?= boards/$(BOARD)

ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

# Make the build directory reflect the board.
BUILD ?= build-$(BOARD)

include ../../py/mkenv.mk
-include mpconfigport.mk

# Include board specific .mk file.
include $(BOARD_DIR)/mpconfigboard.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

ifeq ($(QEMU_ARCH),arm)
MICROPY_HEAP_SIZE ?= 143360
ifeq ($(BOARD),MICROBIT)
FROZEN_MANIFEST ?= "require('unittest'); freeze('test-frzmpy', ('frozen_const.py'))"
else
FROZEN_MANIFEST ?= "require('unittest'); freeze('test-frzmpy', ('frozen_asm_thumb.py', 'frozen_const.py', 'frozen_viper.py', 'native_frozen_align.py'))"
endif
endif
ifeq ($(QEMU_ARCH),riscv32)
MICROPY_HEAP_SIZE ?= 143360
FROZEN_MANIFEST ?= "require('unittest'); freeze('test-frzmpy', ('frozen_asm_rv32.py', 'frozen_const.py', 'frozen_viper.py', 'native_frozen_align.py'))"
endif

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

GIT_SUBMODULES += lib/berkeley-db-1.xx

CFLAGS += -DMICROPY_HEAP_SIZE=$(MICROPY_HEAP_SIZE)

################################################################################
# ARM specific settings

ifeq ($(QEMU_ARCH),arm)

CROSS_COMPILE ?= arm-none-eabi-

LDFLAGS += -nostdlib
LIBS = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

SRC_C += \
	mcu/arm/errorhandler.c \
	mcu/arm/startup.c \
	shared/runtime/semihosting_arm.c \

endif

################################################################################
# RISC-V 32-bit specific settings

ifeq ($(QEMU_ARCH),riscv32)

CROSS_COMPILE ?= riscv64-unknown-elf-

GCC_VERSION = $(word 1, $(subst ., , $(shell $(CC) -dumpversion)))

RV32_ABI = ilp32

QEMU_ARGS += -bios none

# GCC 10 and lower do not recognise the Zicsr extension in the architecture name.
ifeq ($(shell test $(GCC_VERSION) -le 10; echo $$?),0)
RV32_ARCH ?= rv32imac
else
# Recent GCC versions explicitly require to declare extensions.
RV32_ARCH ?= rv32imac_zicsr
endif

AFLAGS += -mabi=$(RV32_ABI) -march=$(RV32_ARCH)
CFLAGS += $(AFLAGS)
LDFLAGS += -mabi=$(RV32_ABI) -march=$(RV32_ARCH) -Wl,-EL

SRC_C += \
	mcu/rv32/interrupts.c \
	mcu/rv32/startup.c \

SRC_BOARD_O += mcu/rv32/entrypoint.o

endif

################################################################################
# Project specific settings and compiler/linker flags

QEMU_SYSTEM = qemu-system-$(QEMU_ARCH)
QEMU_ARGS += -machine $(QEMU_MACHINE) -nographic -monitor null -semihosting
QEMU_ARGS += $(QEMU_EXTRA)

# Specifying QEMU_DEBUG=1 will block qemu until a debugger is connected.
ifeq ($(QEMU_DEBUG),1)
QEMU_DEBUG_ARGS ?= -s
QEMU_ARGS += -S $(QEMU_DEBUG_ARGS) $(QEMU_DEBUG_EXTRA)
endif

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

CFLAGS += -DMICROPY_HW_BOARD_NAME='"$(QEMU_MACHINE)"'
CFLAGS += $(INC) -Wall -Wpointer-arith -Wdouble-promotion -Wfloat-conversion -Werror -std=gnu99 $(COPT) \
	 -ffunction-sections -fdata-sections
CFLAGS += $(CFLAGS_EXTRA)

LDFLAGS += -T $(LDSCRIPT) -Wl,--gc-sections -Wl,-Map=$(@:.elf=.map)

# Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -g
COPT = -O0
else
COPT += -Os -DNDEBUG
endif

# If Picolibc is available then select it explicitly.  Ubuntu 22.04 ships its
# bare metal RISC-V toolchain with Picolibc rather than Newlib, and the default
# is "nosys" so a value must be provided.  To avoid having per-distro
# workarounds, always select Picolibc if available.
PICOLIBC_SPECS = $(shell $(CC) --print-file-name=picolibc.specs)
ifeq ($(PICOLIBC_SPECS),picolibc.specs)
# Picolibc was not found.
else
$(info picolibc used $(PICOLIBC_SPECS))
SPECS_FRAGMENT = --specs=$(PICOLIBC_SPECS)
CFLAGS += $(SPECS_FRAGMENT)
LDFLAGS += $(SPECS_FRAGMENT)
endif

RUN_TESTS_FULL_ARGS = -t execpty:"$(QEMU_SYSTEM) $(QEMU_ARGS) -serial pty -kernel ../ports/qemu/$<" $(RUN_TESTS_ARGS)

################################################################################
# Source files and libraries

SRC_C += \
	main.c \
	uart.c \
	mphalport.c \
	shared/libc/string0.c \
	shared/readline/readline.c \
	shared/runtime/interrupt_char.c \
	shared/runtime/pyexec.c \
	shared/runtime/stdout_helpers.c \
	shared/runtime/sys_stdio_mphal.c \

LIB_SRC_C += $(SRC_LIB_LIBM_C)
LIB_SRC_C += $(SRC_LIB_LIBM_SQRT_SW_C)

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_BOARD_O))
OBJ += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(LIB_SRC_C)

################################################################################
# Main targets

all: $(BUILD)/firmware.elf

.PHONY: repl
repl: $(BUILD)/firmware.elf
	$(ECHO) "Use machine.reset() to exit"
	$(QEMU_SYSTEM) $(QEMU_ARGS) -serial mon:stdio -kernel $<

.PHONY: run
run: $(BUILD)/firmware.elf
	$(QEMU_SYSTEM) $(QEMU_ARGS) -serial pty -kernel $<

.PHONY: test
test: $(BUILD)/firmware.elf
	cd $(TOP)/tests && ./run-tests.py $(RUN_TESTS_FULL_ARGS) $(RUN_TESTS_EXTRA)

.PHONY: test_full
test_full: $(BUILD)/firmware.elf
	cd $(TOP)/tests && ./run-tests.py $(RUN_TESTS_FULL_ARGS)
	cd $(TOP)/tests && ./run-tests.py $(RUN_TESTS_FULL_ARGS) --via-mpy
	cd $(TOP)/tests && ./run-tests.py $(RUN_TESTS_FULL_ARGS) --via-mpy --emit native

# "btree" currently does not build for rv32imc (Picolibc TLS incompatibility).
.PHONY: test_natmod
test_natmod: $(BUILD)/firmware.elf
	$(eval DIRNAME=ports/$(notdir $(CURDIR)))
	cd $(TOP)/tests && \
		for natmod in deflate framebuf heapq random_basic re; do \
		./run-natmodtests.py -p -d execpty:"$(QEMU_SYSTEM) $(QEMU_ARGS) -serial pty -kernel ../$(DIRNAME)/$<" extmod/$$natmod*.py; \
		done

$(BUILD)/firmware.elf: $(LDSCRIPT) $(OBJ)
	$(Q)$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

################################################################################
# Remaining make rules

include $(TOP)/py/mkrules.mk
