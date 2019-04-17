# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2019 Dan Halbert for Adafruit Industries
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# Select the board to build for.
ifeq ($(BOARD),)
  $(info You must provide a BOARD parameter with 'BOARD=')
  $(info Possible values are:)
  $(info $(sort $(subst /.,,$(subst boards/,,$(wildcard boards/*/.)))))
  $(error BOARD not defined)
else
  ifeq ($(wildcard boards/$(BOARD)/.),)
    $(error Invalid BOARD specified)
  endif
endif

CLI_SD := $(SD)
SD_LOWER = $(shell echo $(SD) | tr '[:upper:]' '[:lower:]')

# Build directory with SD if it's different from the default.
BUILD ?= $(if $(CLI_SD),build-$(BOARD)-$(SD_LOWER),build-$(BOARD))

include ../../py/mkenv.mk
# Board-specific
include boards/$(BOARD)/mpconfigboard.mk
# Port-specific
include mpconfigport.mk
# CircuitPython-specific
include $(TOP)/py/circuitpy_mpconfig.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

include $(TOP)/supervisor/supervisor.mk

# Include make rules and variables common across CircuitPython builds.
include $(TOP)/py/circuitpy_defns.mk

ifneq ($(SD), )
	include bluetooth/bluetooth_common.mk
endif

FROZEN_MPY_DIR = freeze

CROSS_COMPILE = arm-none-eabi-

FATFS_DIR = lib/oofatfs

INC += -I.
INC += -I../..
INC += -I$(BUILD)
INC += -I$(BUILD)/genhdr
INC += -I./../../lib/cmsis/inc
INC += -I./boards/$(BOARD)
INC += -I./modules/ubluepy
INC += -I./modules/ble
INC += -I./nrfx
INC += -I./nrfx/hal
INC += -I./nrfx/mdk
INC += -I./nrfx/drivers/include
INC += -I./bluetooth
INC += -I./peripherals
INC += -I../../lib/mp-readline
INC += -I../../lib/tinyusb/src
INC += -I../../supervisor/shared/usb

#Debugging/Optimization
ifeq ($(DEBUG), 1)
  CFLAGS += -ggdb
  # You may want to enable these flags to make setting breakpoints easier.
  CFLAGS += -fno-inline -fno-ipa-sra
else
  CFLAGS += -Os -DNDEBUG
  # TODO: Test with -flto
  ### CFLAGS += -flto
endif


CFLAGS += $(INC) -Wall -Werror -std=gnu11 -nostdlib $(BASE_CFLAGS) $(CFLAGS_MOD) $(COPT)

# Undo some warnings.
# nrfx uses undefined preprocessor variables quite casually, so we can't do warning checks for these.
CFLAGS += -Wno-undef
# nrfx does casts that increase alignment requirements.
CFLAGS += -Wno-cast-align

NRF_DEFINES += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += $(NRF_DEFINES)

CFLAGS += \
	-mthumb \
	-mabi=aapcs-linux \
	-mfloat-abi=hard \
	-mcpu=cortex-m4 \
	-mfpu=fpv4-sp-d16

# TODO: check this
CFLAGS += -D__START=main

LDFLAGS = $(CFLAGS) -nostartfiles -fshort-enums -Wl,-nostdlib -Wl,-T,$(LD_FILE) -Wl,-Map=$@.map -Wl,-cref -Wl,-gc-sections -specs=nano.specs
LIBS := -lgcc -lc

LDFLAGS += -mthumb -mcpu=cortex-m4

# Use toolchain libm if we're not using our own.
ifndef INTERNAL_LIBM
LIBS += -lm
endif

# TinyUSB defines
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_NRF5X -DCFG_TUD_CDC_RX_BUFSIZE=1024 -DCFG_TUD_CDC_TX_BUFSIZE=1024 -DCFG_TUD_MSC_BUFSIZE=4096 -DCFG_TUD_MIDI_RX_BUFSIZE=128 -DCFG_TUD_MIDI_TX_BUFSIZE=128

SRC_NRFX = $(addprefix nrfx/,\
	drivers/src/nrfx_power.c \
	drivers/src/nrfx_spim.c \
	drivers/src/nrfx_timer.c \
	drivers/src/nrfx_twim.c \
	drivers/src/nrfx_uarte.c \
	drivers/src/nrfx_gpiote.c \
	drivers/src/nrfx_rtc.c \
	)

ifdef EXTERNAL_FLASH_DEVICES
	ifeq ($(QSPI_FLASH_FILESYSTEM),1)
		SRC_NRFX += nrfx/drivers/src/nrfx_qspi.c
	endif
endif


SRC_C += \
	background.c \
	fatfs_port.c \
	mphalport.c \
	tick.c \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/pins.c \
	device/$(MCU_VARIANT)/startup_$(MCU_SUB_VARIANT).c \
	bluetooth/ble_drv.c \
	bluetooth/ble_uart.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	lib/oofatfs/ff.c \
	lib/oofatfs/option/ccsbcs.c \
	lib/timeutils/timeutils.c \
	lib/utils/buffer_helper.c \
	lib/utils/context_manager_helpers.c \
	lib/utils/interrupt_char.c \
	lib/utils/pyexec.c \
	lib/utils/stdout_helpers.c \
	lib/utils/sys_stdio_mphal.c \
	nrfx/hal/nrf_nvmc.c \
	nrfx/mdk/system_$(MCU_SUB_VARIANT).c \
	peripherals/nrf/cache.c \
	peripherals/nrf/clocks.c \
	peripherals/nrf/$(MCU_CHIP)/pins.c \
	peripherals/nrf/$(MCU_CHIP)/power.c \
        peripherals/nrf/nvm.c \
	peripherals/nrf/timers.c \
	sd_mutex.c \
	supervisor/shared/memory.c

# USB source files for nrf52840
ifeq ($(MCU_SUB_VARIANT),nrf52840)
SRC_C += \
	lib/tinyusb/src/portable/nordic/nrf5x/dcd_nrf5x.c \
	lib/tinyusb/src/portable/nordic/nrf5x/hal_nrf5x.c
endif

SRC_COMMON_HAL_EXPANDED = $(addprefix shared-bindings/, $(SRC_COMMON_HAL)) \
                          $(addprefix shared-bindings/, $(SRC_BINDINGS_ENUMS)) \
                          $(addprefix common-hal/, $(SRC_COMMON_HAL))

SRC_SHARED_MODULE_EXPANDED = $(addprefix shared-bindings/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE))

SRC_S = supervisor/cpu.s

FROZEN_MPY_PY_FILES := $(shell find -L $(FROZEN_MPY_DIR) -type f -name '*.py')
FROZEN_MPY_MPY_FILES := $(addprefix $(BUILD)/,$(FROZEN_MPY_PY_FILES:.py=.mpy))

OBJ += $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_NRFX:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_EXPANDED:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SHARED_MODULE_EXPANDED:.c=.o))
ifeq ($(INTERNAL_LIBM),1)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
endif
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))

$(BUILD)/$(FATFS_DIR)/ff.o: COPT += -Os
$(filter $(PY_BUILD)/../extmod/vfs_fat_%.o, $(PY_O)): COPT += -Os

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_MOD) $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED)
# Sources that only hold QSTRs after pre-processing.
SRC_QSTR_PREPROCESSOR +=


all: $(BUILD)/firmware.bin $(BUILD)/firmware.uf2

$(BUILD)/firmware.elf: $(OBJ)
	$(STEPECHO) "LINK $@"
	$(Q)$(CC) -o $@ $(LDFLAGS) $^ -Wl,--start-group $(LIBS) -Wl,--end-group
	$(Q)$(SIZE) $@ | $(PYTHON3) $(TOP)/tools/build_memory_info.py $(LD_FILE)

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary $^ $@
#	$(Q)$(OBJCOPY) -O binary -j .vectors -j .text -j .data $^ $@

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O ihex $^ $@
#	$(Q)$(OBJCOPY) -O ihex -j .vectors -j .text -j .data $^ $@

$(BUILD)/firmware.uf2: $(BUILD)/firmware.hex
	$(ECHO) "Create $@"
	$(PYTHON3) $(TOP)/tools/uf2/utils/uf2conv.py -f 0xADA52840 -c -o "$(BUILD)/firmware.uf2" $^



#####################
# Flash with debugger
#####################
FLASHER ?=

ifeq ($(FLASHER),)

# Also update to bootloader settting to validate application and skip checksum ( app valid = 0x0001, crc = 0x0000 )
flash: $(BUILD)/firmware.hex
	nrfjprog --program $< --sectorerase -f $(MCU_VARIANT)
	nrfjprog --erasepage $(BOOT_SETTING_ADDR) -f $(MCU_VARIANT)
	nrfjprog --memwr $(BOOT_SETTING_ADDR) --val 0x00000001 -f $(MCU_VARIANT)
	nrfjprog --reset -f $(MCU_VARIANT)

sd: $(BUILD)/firmware.hex
	nrfjprog --eraseall -f $(MCU_VARIANT)
	nrfjprog --program $(SOFTDEV_HEX) -f $(MCU_VARIANT)
	nrfjprog --program $< --sectorerase -f $(MCU_VARIANT)
	nrfjprog --reset -f $(MCU_VARIANT)

else ifeq ($(FLASHER), pyocd)

flash: $(BUILD)/firmware.hex
	pyocd-flashtool -t $(MCU_VARIANT) $< --sector_erase
	#pyocd-tool -t $(MCU_VARIANT) erase $(BOOT_SETTING_ADDR)
	pyocd-tool -t $(MCU_VARIANT) write32 $(BOOT_SETTING_ADDR) 0x00000001
	pyocd-tool -t $(MCU_VARIANT) reset

sd: $(BUILD)/firmware.hex
	pyocd-flashtool -t $(MCU_VARIANT) --chip_erase
	pyocd-flashtool -t $(MCU_VARIANT) $(SOFTDEV_HEX)
	pyocd-flashtool -t $(MCU_VARIANT) $< --sector_erase
	pyocd-tool -t $(MCU_VARIANT) reset $(BOOT_SETTING_ADDR)

endif

#####################
# Flash with DFU
#####################
.phony: dfu-gen dfu-flash

NRFUTIL = adafruit-nrfutil

ifeq ($(MCU_SUB_VARIANT),nrf52840)
	DFU_TOUCH = --touch 1200
else
	DFU_TOUCH =
endif

check_defined = \
    $(strip $(foreach 1,$1, \
    $(call __check_defined,$1,$(strip $(value 2)))))
__check_defined = \
    $(if $(value $1),, \
    $(error Undefined make flag: $1$(if $2, ($2))))

## Flash with DFU serial
dfu-flash: $(BUILD)/dfu-package.zip
	@:$(call check_defined, SERIAL, example: SERIAL=/dev/ttyUSB0)
	$(NRFUTIL) --verbose dfu serial --package $^ -p $(SERIAL) -b 115200 --singlebank $(DFU_TOUCH)

## Create DFU package file
dfu-gen: $(BUILD)/dfu-package.zip

$(BUILD)/dfu-package.zip: $(BUILD)/firmware.hex
	$(NRFUTIL) dfu genpkg --sd-req 0xFFFE --dev-type 0x0052 --application $^ $(BUILD)/dfu-package.zip


include $(TOP)/py/mkrules.mk

# Print out the value of a make variable.
# https://stackoverflow.com/questions/16467718/how-to-print-out-a-variable-in-makefile
print-%:
	@echo $* = $($*)
