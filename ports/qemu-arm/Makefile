BOARD ?= mps2-an385

# Make the build directory reflect the board.
BUILD ?= build-$(BOARD)

include ../../py/mkenv.mk
-include mpconfigport.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1
FROZEN_MANIFEST ?= "freeze('test-frzmpy')"

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

CFLAGS += -DMICROPY_HW_BOARD_NAME='"$(BOARD)"'
QEMU_ARGS += -machine $(BOARD) -nographic -monitor null -semihosting

ifeq ($(BOARD),netduino2)
CFLAGS += -mthumb -mcpu=cortex-m3 -mfloat-abi=soft
CFLAGS += -DQEMU_SOC_STM32
CFLAGS += -DMICROPY_HW_MCU_NAME='"STM32"'
LDSCRIPT = stm32.ld
SRC_BOARD_O = shared/runtime/gchelper_native.o shared/runtime/gchelper_thumb2.o
MPY_CROSS_FLAGS += -march=armv7m
endif

ifeq ($(BOARD),microbit)
CFLAGS += -mthumb -mcpu=cortex-m0 -mfloat-abi=soft
CFLAGS += -DQEMU_SOC_NRF51
CFLAGS += -DMICROPY_HW_MCU_NAME='"nRF51"'
LDSCRIPT = nrf51.ld
QEMU_ARGS += -global nrf51-soc.flash-size=1048576 -global nrf51-soc.sram-size=262144
SRC_BOARD_O = shared/runtime/gchelper_native.o shared/runtime/gchelper_thumb1.o
MPY_CROSS_FLAGS += -march=armv7m
endif

ifeq ($(BOARD),mps2-an385)
CFLAGS += -mthumb -mcpu=cortex-m3 -mfloat-abi=soft
CFLAGS += -DQEMU_SOC_MPS2
CFLAGS += -DMICROPY_HW_MCU_NAME='"Cortex-M3"'
LDSCRIPT = mps2.ld
SRC_BOARD_O = shared/runtime/gchelper_native.o shared/runtime/gchelper_thumb2.o
MPY_CROSS_FLAGS += -march=armv7m
endif

ifeq ($(BOARD),sabrelite)
CFLAGS += -mcpu=cortex-a9
CFLAGS += -DQEMU_SOC_IMX6
CFLAGS += -DMICROPY_HW_MCU_NAME='"Cortex-A9"'
LDSCRIPT = imx6.ld
QEMU_ARGS += -m 128M
SRC_BOARD_O = shared/runtime/gchelper_generic.o
# It's really armv7a but closest supported value is armv6.
MPY_CROSS_FLAGS += -march=armv6
# Cortex-A9 should support unaligned-access, but qemu doesn't seem to.
CFLAGS += -mno-unaligned-access
# These don't work on Cortex-A9.
TESTS_EXCLUDE = --exclude '(asmdiv|asmspecialregs).py'
endif

CROSS_COMPILE ?= arm-none-eabi-

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

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

SRC_COMMON_C = \
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

SRC_RUN_C = \
	main.c \

LIB_SRC_C += $(SRC_LIB_LIBM_C)
LIB_SRC_C += $(SRC_LIB_LIBM_SQRT_SW_C)

OBJ_COMMON =
OBJ_COMMON += $(PY_O)
OBJ_COMMON += $(addprefix $(BUILD)/, $(SRC_COMMON_C:.c=.o))
OBJ_COMMON += $(addprefix $(BUILD)/, $(SRC_BOARD_O))
OBJ_COMMON += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))

OBJ_RUN =
OBJ_RUN += $(addprefix $(BUILD)/, $(SRC_RUN_C:.c=.o))

ALL_OBJ_RUN = $(OBJ_COMMON) $(OBJ_RUN)

OBJ_TEST =
OBJ_TEST += $(addprefix $(BUILD)/, $(SRC_TEST_C:.c=.o))

ALL_OBJ_TEST = $(OBJ_COMMON) $(OBJ_TEST)

# All object files, needed to get dependencies correct
OBJ = $(OBJ_COMMON) $(OBJ_RUN) $(OBJ_TEST)

# List of sources for qstr extraction
SRC_QSTR += $(SRC_COMMON_C) $(SRC_RUN_C) $(LIB_SRC_C)

all: $(BUILD)/firmware.elf

.PHONY: repl
repl: $(BUILD)/firmware.elf
	$(ECHO) "Use machine.reset() to exit"
	qemu-system-arm $(QEMU_ARGS) -serial mon:stdio -kernel $<

.PHONY: run
run: $(BUILD)/firmware.elf
	qemu-system-arm $(QEMU_ARGS) -serial pty -kernel $<

.PHONY: test
test: $(BUILD)/firmware.elf
	$(eval DIRNAME=ports/$(notdir $(CURDIR)))
	cd $(TOP)/tests && ./run-tests.py --target qemu-arm --device execpty:"qemu-system-arm $(QEMU_ARGS) -serial pty -kernel ../$(DIRNAME)/$<" $(TESTS_EXCLUDE)

## `$(LD)` doesn't seem to like `--specs` for some reason, but we can just use `$(CC)` here.
$(BUILD)/firmware.elf: $(LDSCRIPT) $(ALL_OBJ_RUN)
	$(Q)$(LD) $(LDFLAGS) -o $@ $(ALL_OBJ_RUN) $(LIBS)
	$(Q)$(SIZE) $@

include $(TOP)/py/mkrules.mk
