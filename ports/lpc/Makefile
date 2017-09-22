include ../../py/mkenv.mk

BOARD=lpc_board_ciaa_edu_4337

MPY_CROSS=$(TOP)/mpy-cross/mpy-cross

-include boards/$(BOARD)/board.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

CROSS_COMPILE = arm-none-eabi-

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -Iboards/$(BOARD)
INC += -Iboards/$(BOARD)/inc
INC += -Ichips/$(CHIP)/inc

-include chips/$(CHIP)/chip.mk

OPENOCD = openocd
OPENOCD_CONFIG = boards/$(BOARD)/openocd.cfg
DFU = dfu-util
DFUHDR = $(TOP)/tools/dfuheader.py
CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mabi=aapcs -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
CFLAGS = $(INC) -Wall -Werror -std=c99 -nostdlib $(CFLAGS_CORTEX_M4) $(COPT)
LDFLAGS = -nostdlib -Lboards/$(BOARD) -Lchips/$(CHIP) -T chips/$(CHIP)/linker.lds -Map=$@.map --cref --gc-sections

# Tune for Debugging or Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

CFLAGS += $(BOARD_CFLAGS) $(CHIP_CFLAGS)

LIBS = $(shell $(CROSS_COMPILE)gcc $(CFLAGS_CORTEX_M4)  --print-libgcc-file-name)

SRC_C = \
	main.c \
	mphalport.c \
	$(wildcard boards/$(BOARD)/src/*.c) \
	$(wildcard chips/$(CHIP)/src/*.c) \
	lib/utils/stdout_helpers.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

all: $(BUILD)/firmware.bin

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -j .isr_vector -j .text -j .data $^ $(BUILD)/firmware.bin

$(BUILD)/firmware.dfu: $(BUILD)/firmware.bin
	$(ECHO) "Create $@"
	$(Q)$(DFUHDR) $^ $@

deploy-dfu: $(BUILD)/firmware.dfu
	$(ECHO) "Writing $< to the board via DFU"
	$(Q)$(DFU) -R -D $^

deploy-openocd: $(BUILD)/firmware.bin
	$(ECHO) "Writing $< to the board via OpenOCD"
	$(Q)$(OPENOCD) -f $(OPENOCD_CONFIG) -c "lpc_flash $<"

include $(TOP)/py/mkrules.mk
