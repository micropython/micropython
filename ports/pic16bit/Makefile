include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

XC16 = /opt/microchip/xc16/v1.35
CROSS_COMPILE ?= $(XC16)/bin/xc16-

PARTFAMILY = dsPIC33F
PART = 33FJ256GP506

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(XC16)/include
INC += -I$(XC16)/support/$(PARTFAMILY)/h

CFLAGS_PIC16BIT = -mcpu=$(PART) -mlarge-code
CFLAGS = $(INC) -Wall -Werror -std=gnu99 -nostdlib $(CFLAGS_PIC16BIT) $(COPT)

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -O1 -DNDEBUG
endif

LDFLAGS = --heap=0 -nostdlib -T $(XC16)/support/$(PARTFAMILY)/gld/p$(PART).gld -Map=$@.map --cref -p$(PART)
LIBS = -L$(XC16)/lib -L$(XC16)/lib/$(PARTFAMILY) -lc -lm -lpic30

SRC_C = \
	main.c \
	board.c \
	pic16bit_mphal.c \
	modpyb.c \
	modpybled.c \
	modpybswitch.c \
	lib/utils/pyexec.c \
	lib/utils/sys_stdio_mphal.c \
	lib/mp-readline/readline.c \

SRC_S = \
#	gchelper.s \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o))

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C)
# Append any auto-generated sources that are needed by sources listed in
# SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

all: $(BUILD)/firmware.hex

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(CROSS_COMPILE)bin2hex $<

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)size $@

$(PY_BUILD)/gc.o: CFLAGS += -O1
$(PY_BUILD)/vm.o: CFLAGS += -O1

include $(TOP)/py/mkrules.mk
