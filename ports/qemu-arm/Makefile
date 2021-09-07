include ../../py/mkenv.mk
-include mpconfigport.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

# include py core make definitions
include $(TOP)/py/py.mk

BOARD ?= mps2-an385

ifeq ($(BOARD),netduino2)
CFLAGS += -mthumb -mcpu=cortex-m3 -mfloat-abi=soft
CFLAGS += -DQEMU_SOC_STM32
LDSCRIPT = stm32.ld
SRC_BOARD_O = shared/runtime/gchelper_native.o shared/runtime/gchelper_m3.o
MPY_CROSS_FLAGS += -march=armv7m
endif

ifeq ($(BOARD),microbit)
CFLAGS += -mthumb -mcpu=cortex-m0 -mfloat-abi=soft
CFLAGS += -DQEMU_SOC_NRF51
LDSCRIPT = nrf51.ld
QEMU_EXTRA = -global nrf51-soc.flash-size=1048576 -global nrf51-soc.sram-size=262144
SRC_BOARD_O = shared/runtime/gchelper_native.o shared/runtime/gchelper_m0.o
MPY_CROSS_FLAGS += -march=armv7m
endif

ifeq ($(BOARD),mps2-an385)
CFLAGS += -mthumb -mcpu=cortex-m3 -mfloat-abi=soft
CFLAGS += -DQEMU_SOC_MPS2
LDSCRIPT = mps2.ld
SRC_BOARD_O = shared/runtime/gchelper_native.o shared/runtime/gchelper_m3.o
MPY_CROSS_FLAGS += -march=armv7m
endif

ifeq ($(BOARD),sabrelite)
CFLAGS += -mcpu=cortex-a9
CFLAGS += -DQEMU_SOC_IMX6
LDSCRIPT = imx6.ld
QEMU_EXTRA = -m 128M
SRC_BOARD_O = shared/runtime/gchelper_generic.o
# It's really armv7a but closest supported value is armv6.
MPY_CROSS_FLAGS += -march=armv6
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
	moduos.c \
	modmachine.c \
	shared/libc/string0.c \
	shared/runtime/sys_stdio_mphal.c \

SRC_RUN_C = \
	main.c \

SRC_TEST_C = \
	test_main.c \
	lib/tinytest/tinytest.c \

LIB_SRC_C += $(addprefix lib/,\
	libm/math.c \
	libm/fmodf.c \
	libm/nearbyintf.c \
	libm/ef_sqrt.c \
	libm/kf_rem_pio2.c \
	libm/kf_sin.c \
	libm/kf_cos.c \
	libm/kf_tan.c \
	libm/ef_rem_pio2.c \
	libm/sf_sin.c \
	libm/sf_cos.c \
	libm/sf_tan.c \
	libm/sf_frexp.c \
	libm/sf_modf.c \
	libm/sf_ldexp.c \
	libm/asinfacosf.c \
	libm/atanf.c \
	libm/atan2f.c \
	libm/roundf.c \
	)

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

ifneq ($(FROZEN_MANIFEST),)
CFLAGS += -DMICROPY_MODULE_FROZEN_STR
CFLAGS += -DMICROPY_MODULE_FROZEN_MPY
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
endif

all: run

run: $(BUILD)/firmware.elf
	qemu-system-arm -machine $(BOARD) $(QEMU_EXTRA) -nographic -monitor null -semihosting -kernel $<

## `$(LD)` doesn't seem to like `--specs` for some reason, but we can just use `$(CC)` here.
$(BUILD)/firmware.elf: $(LDSCRIPT) $(ALL_OBJ_RUN)
	$(Q)$(LD) $(LDFLAGS) -o $@ $(ALL_OBJ_RUN) $(LIBS)
	$(Q)$(SIZE) $@

include $(TOP)/py/mkrules.mk
