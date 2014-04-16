include ../py/mkenv.mk
-include mpconfigport.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

CROSS_COMPILE = arm-none-eabi-

CFLAGS_CORTEX_M3 = -mthumb -mcpu=cortex-m3
CFLAGS = -I. -I$(PY_SRC) -Wall -ansi -std=gnu99 $(CFLAGS_CORTEX_M3) $(COPT)

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -g -DPENDSV_DEBUG
COPT = -O0
else
COPT += -Os -DNDEBUG
endif

#LDFLAGS = --nostdlib -T stm32f405.ld -Map=$(@:.elf=.map) --cref
LDFLAGS = -lm -T generic-m-hosted.ld # -Map=$(@:.elf=.map) --cref
LIBS =

# uncomment this if you want libgcc
#LIBS += $(shell $(CC) -print-libgcc-file-name)

SRC_C = \
	help.c \
	math.c \

	#gccollect.c \
	#pyexec.c \

SRC_S = \
	gchelper.s \

OBJ =
OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))

all: $(BUILD)/flash.elf

run:
	qemu-system-arm -cpu cortex-m3 -nographic -monitor null -serial null -semihosting -kernel $(BUILD)/flash.elf

$(BUILD)/flash.elf: $(OBJ)
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) main.c -o $@ $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

include ../py/mkrules.mk
