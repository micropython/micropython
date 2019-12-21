include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

ARCH = $(shell uname -m)
ifneq ("$(ARCH)", "ppc64")
ifneq ("$(ARCH)", "ppc64le")
	CROSS_COMPILE ?= powerpc64le-linux-
endif
endif

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

CFLAGS = $(INC) -g -Wall -std=c99 $(COPT)
CFLAGS += -mno-string -mno-multiple -mno-vsx -mno-altivec -nostdlib
CFLAGS += -mlittle-endian -mstrict-align -msoft-float
CFLAGS += -Os
CFLAGS += -fdata-sections -ffunction-sections -fno-stack-protector -ffreestanding
CFLAGS += -U_FORTIFY_SOURCE

LDFLAGS = -N -T powerpc.lds -nostdlib

LIBS =

SRC_C = \
	main.c \
	uart_core.c \
	uart_potato.c \
	uart_lpc_serial.c \
	lib/utils/printf.c \
	lib/utils/stdout_helpers.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	$(BUILD)/_frozen_mpy.c \

OBJ = $(PY_CORE_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(BUILD)/head.o

all: $(BUILD)/firmware.elf $(BUILD)/firmware.map $(BUILD)/firmware.bin

$(BUILD)/_frozen_mpy.c: frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)$(MPY_TOOL) -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=mpz $< > $@

$(BUILD)/firmware.elf: $(OBJ) powerpc.lds
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary $^ $(BUILD)/firmware.bin

$(BUILD)/firmware.map: $(BUILD)/firmware.elf
	$(Q)nm $^ | sort > $(BUILD)/firmware.map

include $(TOP)/py/mkrules.mk
