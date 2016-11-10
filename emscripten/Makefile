include ../py/mkenv.mk

CROSS = 0
# clang has slightly different options to GCC
CLANG = 0

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

ifeq ($(CROSS), 1)
CROSS_COMPILE = arm-none-eabi-
endif

ifeq ($(EMSCRIPTEN), 1)
CC = emcc -g4
CPP = clang -E
CLANG = 1
SIZE = echo
LD = emcc -g4
endif

INC += -I.
INC += -I..
INC += -I../lib/mp-readline
INC += -I../stmhal
INC += -I$(BUILD)

ifeq ($(CROSS), 1)
DFU = ../tools/dfu.py
PYDFU = ../tools/pydfu.py
CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
CFLAGS = $(INC) -Wall -Werror -ansi -std=gnu99 -nostdlib $(CFLAGS_CORTEX_M4) $(COPT)
else
CFLAGS = -m32 $(INC) -Wall -Werror -ansi -std=gnu99 $(COPT)
endif

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0
else
CFLAGS += -Os -DNDEBUG
endif

CFLAGS += -D MICROPY_NLR_SETJMP=1
CFLAGS += -D MICROPY_USE_INTERNAL_PRINTF=0

ifeq ($(CROSS), 1)
LDFLAGS = -nostdlib -T stm32f405.ld -Map=$@.map --cref
else
ifeq ($(CLANG), 1)
LD = $(CC)
LDFLAGS = -m32 -Wl,-map,$@.map -Wl,-dead_strip -Wl,-no_pie
else
LD = gcc
LDFLAGS = -m32 -Wl,-Map=$@.map,--cref
endif
endif
LIBS =

SRC_C = \
	main.c \
	stdio_core.c \
	lib/utils/stdout_helpers.c \
	lib/utils/pyexec.c \
	lib/mp-readline/readline.c \
	$(BUILD)/_frozen_mpy.c \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

ifeq ($(CROSS), 1)
all: $(BUILD)/firmware.dfu
else
ifeq ($(EMSCRIPTEN), 1)
all: $(BUILD)/firmware.js
else
all: $(BUILD)/firmware.elf
endif
endif

$(BUILD)/_frozen_mpy.c: frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)../tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=none $< > $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.js: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary -j .isr_vector -j .text -j .data $^ $(BUILD)/firmware.bin

$(BUILD)/firmware.dfu: $(BUILD)/firmware.bin
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(DFU) -b 0x08000000:$(BUILD)/firmware.bin $@

deploy: $(BUILD)/firmware.dfu
	$(ECHO) "Writing $< to the board"
	$(Q)$(PYTHON) $(PYDFU) -u $<

# Run emulation build on a POSIX system with suitable terminal settings
run:
	stty raw opost -echo
	build/firmware.elf
	@echo Resetting terminal...
# This sleep is useful to spot segfaults
	sleep 1
	reset

test: $(BUILD)/firmware.elf
	$(Q)/bin/echo -e "print('hello world!', list(x+1 for x in range(10)), end='eol\\\\n')\\r\\n\\x04" | $(BUILD)/firmware.elf | tail -n2 | grep "^hello world! \\[1, 2, 3, 4, 5, 6, 7, 8, 9, 10\\]eol"

include ../py/mkrules.mk
