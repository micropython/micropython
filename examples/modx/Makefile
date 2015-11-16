include ../../py/mkenv.mk

CROSS = 0

ifeq ($(CROSS), 1)
CROSS_COMPILE = arm-none-eabi-
endif

INC = -I. -I../..

CFLAGS = $(INC) -Wall -Werror -ansi -std=gnu99 -fPIC -fno-stack-protector -Os

ifeq ($(CROSS), 1)
CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
CFLAGS += -nostdlib $(CFLAGS_CORTEX_M4)
endif

all: modx.mpy

modx.mpy: modx.elf
	$(OBJCOPY) -O binary -j .all $< $@
	$(PYTHON) elftompy.py $@

modx.elf: modx.o
	$(LD) -T persistnative.ld -o $@ $<
