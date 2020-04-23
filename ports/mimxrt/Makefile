BOARD ?= TEENSY40
BOARD_DIR ?= boards/$(BOARD)
BUILD ?= build-$(BOARD)

CROSS_COMPILE ?= arm-none-eabi-

ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

include ../../py/mkenv.mk
include $(BOARD_DIR)/mpconfigboard.mk

# Qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h
QSTR_GLOBAL_DEPENDENCIES = $(BOARD_DIR)/mpconfigboard.h

# Include py core make definitions
include $(TOP)/py/py.mk

GIT_SUBMODULES = lib/tinyusb lib/nxp_driver

MCU_DIR = lib/nxp_driver/sdk/devices/$(MCU_SERIES)
LD_FILES = boards/$(MCU_SERIES).ld $(TOP)/$(MCU_DIR)/gcc/$(MCU_SERIES)xxxxx_flexspi_nor.ld

# mcu driver cause following warnings
#CFLAGS += -Wno-error=float-equal -Wno-error=nested-externs
CFLAGS += -Wno-error=unused-parameter

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(BOARD_DIR)
INC += -I$(TOP)/lib/cmsis/inc
INC += -I$(TOP)/$(MCU_DIR)
INC += -I$(TOP)/$(MCU_DIR)/drivers
INC += -I$(TOP)/$(MCU_DIR)/project_template
INC += -I$(TOP)/lib/tinyusb/src
INC += -I$(TOP)/lib/tinyusb/hw
INC += -I$(TOP)/lib/tinyusb/hw/bsp/teensy_40

CFLAGS_MCU = -mtune=cortex-m7 -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16
CFLAGS = $(INC) -Wall -Werror -Wdouble-promotion -Wfloat-conversion -std=c99 -nostdlib -mthumb $(CFLAGS_MCU)
CFLAGS += -DCPU_$(MCU_SERIES) -DCPU_$(MCU_VARIANT)
CFLAGS += -DXIP_EXTERNAL_FLASH=1 \
	-DXIP_BOOT_HEADER_ENABLE=1 \
	-DCFG_TUSB_MCU=OPT_MCU_MIMXRT10XX \
	-D__STARTUP_CLEAR_BSS \
	-D__START=main \
	-DCPU_HEADER_H='<$(MCU_SERIES).h>'

LDFLAGS = $(addprefix -T,$(LD_FILES)) -Map=$@.map --cref
LIBS = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

# Tune for Debugging or Optimization
ifeq ($(DEBUG),1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -Os -DNDEBUG
LDFLAGS += --gc-sections
CFLAGS += -fdata-sections -ffunction-sections
endif

# TinyUSB Stack source
SRC_TINYUSB_C += \
	lib/tinyusb/src/tusb.c \
	lib/tinyusb/src/common/tusb_fifo.c \
	lib/tinyusb/src/device/usbd.c \
	lib/tinyusb/src/device/usbd_control.c \
	lib/tinyusb/src/class/msc/msc_device.c \
	lib/tinyusb/src/class/cdc/cdc_device.c \
	lib/tinyusb/src/class/dfu/dfu_rt_device.c \
	lib/tinyusb/src/class/hid/hid_device.c \
	lib/tinyusb/src/class/midi/midi_device.c \
	lib/tinyusb/src/class/usbtmc/usbtmc_device.c \
	lib/tinyusb/src/class/vendor/vendor_device.c \
	lib/tinyusb/src/portable/nxp/transdimension/dcd_transdimension.c

SRC_TINYUSB_IMX_C += \
	$(MCU_DIR)/system_$(MCU_SERIES).c \
	$(MCU_DIR)/xip/fsl_flexspi_nor_boot.c \
	$(MCU_DIR)/project_template/clock_config.c \
	$(MCU_DIR)/drivers/fsl_clock.c \
	$(MCU_DIR)/drivers/fsl_gpio.c \
	$(MCU_DIR)/drivers/fsl_common.c \
	$(MCU_DIR)/drivers/fsl_lpuart.c \
	$(MCU_DIR)/drivers/fsl_flexram.c \

SRC_C = \
	main.c \
	tusb_port.c \
	board_init.c \
	$(BOARD_DIR)/flash_config.c \
	modutime.c \
	modmachine.c \
	mphalport.c \
	lib/mp-readline/readline.c \
	lib/libc/string0.c \
	lib/utils/gchelper_native.c \
	lib/utils/printf.c \
	lib/utils/pyexec.c \
	lib/utils/stdout_helpers.c \
	$(SRC_TINYUSB_C) \
	$(SRC_TINYUSB_IMX_C) \

SRC_SS = $(MCU_DIR)/gcc/startup_$(MCU_SERIES).S

SRC_S = lib/utils/gchelper_m3.s \

# List of sources for qstr extraction
SRC_QSTR += modutime.c modmachine.c

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SS:.S=.o))

# Workaround for bug in older gcc, warning on "static usbd_device_t _usbd_dev = { 0 };"
$(BUILD)/lib/tinyusb/src/device/usbd.o: CFLAGS += -Wno-missing-braces

all: $(BUILD)/firmware.hex

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary $^ $@

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O ihex -R .eeprom $< $@

include $(TOP)/py/mkrules.mk
