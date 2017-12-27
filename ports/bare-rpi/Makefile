include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

CROSS_COMPILE = arm-none-eabi-

OBJCPY=$(CROSS_COMPILE)objcopy
INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

CFLAGS = $(INC) -Wall -std=c99 -nostdlib -marm -mcpu=arm1176jzf-s $(COPT)

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -Os -DNDEBUG
endif

CCVERSION:=$(shell $(CC) --version | sed 's/[ ]/\n/g' | grep "\." | xargs | awk '{ print $$1 }')
ARMGCCLIBPATH=/usr/lib/gcc/arm-none-eabi/$(CCVERSION)

LDFLAGS = -nostdlib -T kernel.ld -Map=$@.map --cref
LIBS = -L$(ARMGCCLIBPATH) -lgcc

SRC_LIB = $(addprefix lib/,\
	libc/string0.c )

SRC_C = \
	main.c \
    uart.c \
    gpio.c \

SRC_S = \
    start.s

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o) $(SRC_LIB:.c=.o))

all: $(BUILD)/firmware.img

list: $(BUILD)/firmware.elf
	$(CROSS_COMPILE)objdump -d $(BUILD)/firmware.elf > $(BUILD)/firmware.list

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.img: $(BUILD)/firmware.elf
	$(OBJCPY) $(BUILD)/firmware.elf -O binary $(BUILD)/firmware.img

include $(TOP)/py/mkrules.mk
