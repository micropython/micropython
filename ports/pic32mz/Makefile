include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

CROSS_COMPILE ?= xc32-

MCU = 32MZ2048EFM144
LD_FILE = generic_pic32mz.ld

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

CFLAGS = -mprocessor=$(MCU)
CFLAGS += $(INC) -std=gnu99 -nostdlib -Wfatal-errors -Winline -Wall -no-legacy-libc -finline

MIN_HEAP_SIZE = 0
MIN_STACK_SIZE = 0x400
#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -g
else
CFLAGS += -O1
endif

LDFLAGS = -mprocessor=$(MCU)
LDFLAGS+= -no-legacy-libc 
LDFLAGS+= -Wl,--script="$(LD_FILE)"
LDFLAGS+= -Wl,--gc-sections,--no-code-in-dinit,--no-dinit-in-serial-mem,-Map=$@.map
LDFLAGS+= -Wl,--defsym=_min_heap_size=$(MIN_HEAP_SIZE),--defsym=_min_stack_size=$(MIN_STACK_SIZE)
LIBS = -lc -lm

SRC_C = \
	mphalport.c \
	main.c \
	lib/utils/pyexec.c \
	lib/utils/sys_stdio_mphal.c \
	lib/mp-readline/readline.c \
	board.c \
	help.c \
	delay.c \
	interrupt.c \
	sysclk.c \
	timer.c \
	uart.c \
	gpio.c \

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
	$(Q)$(CROSS_COMPILE)gcc $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)size $@

$(PY_BUILD)/gc.o: CFLAGS += -O1
$(PY_BUILD)/vm.o: CFLAGS += -O1

include $(TOP)/py/mkrules.mk
