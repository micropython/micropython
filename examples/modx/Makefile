include ../../py/mkenv.mk

INC = -I. -I../..

CFLAGS = $(INC) -Wall -Werror -ansi -std=gnu99 -fPIC -fno-stack-protector -Os

ARCH = native

ifeq ($(ARCH), m4)
CROSS_COMPILE = arm-none-eabi-
CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
CFLAGS += -nostdlib $(CFLAGS_CORTEX_M4)
else ifeq ($(ARCH), m0)
CROSS_COMPILE = arm-none-eabi-
CFLAGS_CORTEX_M0 = -mthumb --short-enums -mtune=cortex-m0 -mcpu=cortex-m0 -mfloat-abi=soft -fno-builtin
CFLAGS += -nostdlib $(CFLAGS_CORTEX_M0)
else ifeq ($(ARCH), native)
# we're not cross compiling
ARCH = native
else
$(error Unknown architecture)
endif

BUILD=build-$(ARCH)

all: $(BUILD)/modx.mpy

clean:
	$(RM) -rf $(BUILD)

$(BUILD)/modx.mpy: $(BUILD)/modx.elf
	$(OBJCOPY) -O binary -j .all $< $@
	$(PYTHON) elftompy.py $@

$(BUILD)/modx.elf: $(BUILD)/modx.o
	$(LD) -T persistnative.ld -o $@ $<

$(BUILD)/modx.o: modx.c
	$(MKDIR) -p $(BUILD)
	$(CC) $(CFLAGS) -o $@ -c $<
