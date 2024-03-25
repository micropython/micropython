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

# The GCC version string starts like this:
# <target>-gcc (<optional git hash>) <major>.<minor>.<patch>
GCC_VERSION = $(subst ., , $(word 3, $(shell $(CC) --version)))
GCC_MAJOR = $(word 1, $(GCC_VERSION))
GCC_MINOR = $(word 2, $(GCC_VERSION))
GCC_PATCH = $(word 3, $(GCC_VERSION))

ifeq ($(BOARD),virt)
ABI=ilp32
# GCC 10 and lower do not recognise the Zicsr extension in the
# architecture name.  "Make" unfortunately does not provide any simple way
# to perform numeric comparisons, so to keep things simple we assume that
# GCC is at least version 10 for the time being.
ifeq ($(GCC_MAJOR),10)
ARCH=rv32imac
else
# Recent GCC versions explicitly require to declare extensions.
ARCH=rv32imac_zicsr
endif
AFLAGS = -mabi=$(ABI) -march=$(ARCH)
CFLAGS += -mabi=$(ABI) -march=$(ARCH) -mcmodel=medany
CFLAGS += -DQEMU_SOC_VIRT
ifeq ($(PICOLIBC),1)
CFLAGS += -DUSE_PICOLIBC -D__TMP_FLT_EVAL_METHOD
endif
LDSCRIPT = virt.ld
LDFLAGS += -T $(LDSCRIPT) -EL
SRC_BOARD_O = shared/runtime/gchelper_generic.o setjmp.o
SRC_BOARD_O += entrypoint.o
endif

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

LDFLAGS += --gc-sections -Map=$(@:.elf=.map)
LIBS = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

SRC_COMMON_C = \
	interrupts.c \
	startup.c \
	uart.c \
	modmachine.c \
	shared/libc/string0.c \
	shared/runtime/sys_stdio_mphal.c \

SRC_RUN_C = \
	../qemu-arm/main.c \

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
