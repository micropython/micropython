BOARD ?= MPS2_AN385

# Make the build directory reflect the board.
BUILD ?= build-$(BOARD)

include ../../py/mkenv.mk
-include mpconfigport.mk

# Include board specific .mk file.
include boards/$(BOARD).mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1
FROZEN_MANIFEST ?= "freeze('test-frzmpy')"

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

CROSS_COMPILE ?= arm-none-eabi-

QEMU_SYSTEM = qemu-system-$(QEMU_ARCH)
QEMU_ARGS += -machine $(QEMU_MACHINE) -nographic -monitor null -semihosting

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

CFLAGS += -DMICROPY_HW_BOARD_NAME='"$(QEMU_MACHINE)"'
CFLAGS += $(INC) -Wall -Wpointer-arith -Wdouble-promotion -Wfloat-conversion -Werror -std=gnu99 $(COPT) \
	 -ffunction-sections -fdata-sections
CFLAGS += $(CFLAGS_EXTRA)

# Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -g
COPT = -O0
else
COPT += -Os -DNDEBUG
endif

## With CoudeSourcery it's actually a little different, you just need `-T generic-m-hosted.ld`.
## Although for some reason `$(LD)` will not find that linker script, it works with `$(CC)`.
## It turns out that this is specific to CoudeSourcery, and ARM version of GCC ships something
## else instead and according to the following files, this is what we need to pass to `$(CC).
## - gcc-arm-none-eabi-4_8-2014q1/share/gcc-arm-none-eabi/samples/src/makefile.conf
## - gcc-arm-none-eabi-4_8-2014q1/share/gcc-arm-none-eabi/samples/src/qemu/Makefile
LDFLAGS= -T $(LDSCRIPT) --gc-sections -Map=$(@:.elf=.map)
LIBS = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

SRC_C = \
	main.c \
	startup.c \
	uart.c \
	mphalport.c \
	shared/libc/string0.c \
	shared/readline/readline.c \
	shared/runtime/interrupt_char.c \
	shared/runtime/pyexec.c \
	shared/runtime/semihosting_arm.c \
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
	$(eval DIRNAME=ports/$(notdir $(CURDIR)))
	cd $(TOP)/tests && ./run-tests.py --target qemu-arm --device execpty:"$(QEMU_SYSTEM) $(QEMU_ARGS) -serial pty -kernel ../$(DIRNAME)/$<" $(RUN_TESTS_ARGS) $(RUN_TESTS_EXTRA)

## `$(LD)` doesn't seem to like `--specs` for some reason, but we can just use `$(CC)` here.
$(BUILD)/firmware.elf: $(LDSCRIPT) $(OBJ)
	$(Q)$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

include $(TOP)/py/mkrules.mk
