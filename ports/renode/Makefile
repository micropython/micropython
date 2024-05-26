# This file is part of the CircuitPython project: https://circuitpython.org
#
# SPDX-FileCopyrightText: Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
#
# SPDX-License-Identifier: MIT

include ../../py/circuitpy_mkenv.mk

CROSS_COMPILE = arm-none-eabi-

INC += \
        -I. \
        -I../.. \
        -I../lib/mp-readline \
        -I../shared/timeutils \
        -Iboards/$(BOARD) \
        -Iboards/ \
        -isystem ./../../lib/cmsis/inc \
        -I$(BUILD)

CFLAGS += -ggdb3 -Os

DISABLE_WARNINGS = -Wno-cast-align
CFLAGS += $(INC) -Wall -Werror -std=gnu11 -fshort-enums $(BASE_CFLAGS) $(CFLAGS_MOD) $(COPT) $(DISABLE_WARNINGS) -Werror=missing-prototypes

CFLAGS += \
	-march=armv6-m \
	-mthumb \
	-mabi=aapcs \
	-mcpu=cortex-m0plus \
	-msoft-float \
	-mfloat-abi=soft \
	--specs=nano.specs

# Use toolchain libm if we're not using our own.
ifndef INTERNAL_LIBM
LIBS += -lm
endif

LIBS += -lc

SRC_C += \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/pins.c \
	background.c \
	mphalport.c \


SRC_COMMON_HAL_EXPANDED = $(addprefix shared-bindings/, $(SRC_COMMON_HAL)) \
                          $(addprefix shared-bindings/, $(SRC_BINDINGS_ENUMS)) \
                          $(addprefix common-hal/, $(SRC_COMMON_HAL))

SRC_SHARED_MODULE_EXPANDED = $(addprefix shared-bindings/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE_INTERNAL))

# There may be duplicates between SRC_COMMON_HAL_EXPANDED and SRC_SHARED_MODULE_EXPANDED,
# because a few modules have files both in common-hal/ and shared-module/.
# Doing a $(sort ...) removes duplicates as part of sorting.
SRC_COMMON_HAL_SHARED_MODULE_EXPANDED = $(sort $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED))

SRC_S = supervisor/$(CPU)_cpu.s

OBJ = $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_SHARED_MODULE_EXPANDED:.c=.o))
ifeq ($(INTERNAL_LIBM),1)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
endif
OBJ += $(addprefix $(BUILD)/, $(SRC_CIRCUITPY_COMMON:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S_UPPER:.S=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))

$(BUILD)/%.o: $(BUILD)/%.S
	$(STEPECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED) $(SRC_CIRCUITPY_COMMON)

all: $(BUILD)/firmware.elf $(BUILD)/circuitpy.img

BOARD_LD := $(wildcard boards/$(BOARD)/link.ld)

ifneq ($(BOARD_LD),)
	LINKER_SCRIPTS = -Wl,-T,$(BOARD_LD)
endif

LINKER_SCRIPTS += -Wl,-T,link.ld

$(BUILD)/circuitpy.img: circuitpy/code.py
	$(STEPECHO) "Create $@"
	$(Q)dd if=/dev/zero of=$(BUILD)/circuitpy.img bs=1 count=0 seek=512K
	$(Q)mkfs.fat -n CIRCUITPY --offset=0 $(BUILD)/circuitpy.img
	$(Q)mcopy -i $(BUILD)/circuitpy.img circuitpy/* ::

ifeq ($(VALID_BOARD),)
$(BUILD)/firmware.elf: invalid-board
else
$(BUILD)/firmware.elf: $(OBJ) $(BOARD_LD) link.ld
	$(STEPECHO) "LINK $@"
	$(Q)echo $(OBJ) > $(BUILD)/firmware.objs
	$(Q)echo $(PICO_LDFLAGS) > $(BUILD)/firmware.ldflags
	$(Q)$(CC) -o $@ $(CFLAGS) @$(BUILD)/firmware.ldflags $(LINKER_SCRIPTS) -Wl,--print-memory-usage -Wl,-Map=$@.map -Wl,-cref -Wl,--gc-sections @$(BUILD)/firmware.objs -Wl,-lc
endif

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -R .dtcm_bss $^ $@

include $(TOP)/py/mkrules.mk
