include ../../py/mkenv.mk
-include mpconfigport.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

BOARD ?= virt

CROSS_COMPILE ?= riscv64-unknown-elf-

GCC_VERSION = $(word 1, $(subst ., , $(shell $(CC) -dumpversion)))

# If Picolibc is available then select it explicitly.  Ubuntu 22.04 ships its
# bare metal RISC-V toolchain with Picolibc rather than Newlib, and the default
# is "nosys" so a value must be provided.  To avoid having per-distro
# workarounds, always select Picolibc if available.
PICOLIBC_SPECS = $(shell $(CC) --print-file-name=picolibc.specs)
ifeq ($(PICOLIBC_SPECS),picolibc.specs)
# Picolibc was not found.
SPECS_FRAGMENT =
else
SPECS_FRAGMENT = --specs=$(PICOLIBC_SPECS)
CFLAGS += $(SPECS_FRAGMENT)
endif

ifeq ($(BOARD),virt)
ABI = ilp32
# GCC 10 and lower do not recognise the Zicsr extension in the
# architecture name.  "Make" unfortunately does not provide any simple way
# to perform numeric comparisons, so to keep things simple we assume that
# GCC is at least version 10 for the time being.
ifeq ($(GCC_VERSION),10)
ARCH ?= rv32imac
else
# Recent GCC versions explicitly require to declare extensions.
ARCH ?= rv32imac_zicsr
ARCH_LD ?= rv32imac_zicsr
endif
AFLAGS = -mabi=$(ABI) -march=$(ARCH)
CFLAGS += $(AFLAGS)
CFLAGS += -DQEMU_SOC_VIRT
ARCH_LD ?= $(ARCH)
LDSCRIPT = virt.ld
LDFLAGS += -mabi=$(ABI) -march=$(ARCH_LD) -T $(LDSCRIPT) -Wl,-EL
SRC_BOARD_O = shared/runtime/gchelper_native.o shared/runtime/gchelper_rv32i.o
SRC_BOARD_O += entrypoint.o
endif

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

CFLAGS += $(INC) -Wall -Wpointer-arith -Wdouble-promotion -Wfloat-conversion -Werror -std=gnu99 $(COPT) \
	 -ffunction-sections -fdata-sections
CFLAGS += $(CFLAGS_EXTRA)

LD = $(CC)

# Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -g
COPT = -O0
else
COPT += -Os -DNDEBUG
endif

LDFLAGS += $(SPECS_FRAGMENT) -Wl,--gc-sections -Wl,-Map=$(@:.elf=.map)

SRC_COMMON_C = \
	interrupts.c \
	startup.c \
	uart.c \
	shared/libc/string0.c \
	shared/runtime/sys_stdio_mphal.c \

SRC_RUN_C = \
	ports/qemu-arm/main.c \

SRC_TEST_C = \
	test_main.c \
	lib/tinytest/tinytest.c \

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

all: run

# `make debug` will block QEMU until a debugger is connected to port 1234.
debug: $(BUILD)/firmware.elf
	qemu-system-riscv32 -machine $(BOARD) -bios none $(QEMU_EXTRA) -nographic -monitor null -semihosting -serial mon:stdio -S -s -kernel $<

run: $(BUILD)/firmware.elf
	qemu-system-riscv32 -machine $(BOARD) -bios none $(QEMU_EXTRA) -nographic -monitor null -semihosting -kernel $<

$(BUILD)/firmware.elf: $(LDSCRIPT) $(ALL_OBJ_RUN)
	$(Q)$(LD) $(LDFLAGS) -o $@ $(ALL_OBJ_RUN) $(LIBS)
	$(Q)$(SIZE) $@

include $(TOP)/py/mkrules.mk
