# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
# SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
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
  $(error You must provide a BOARD parameter)
else
  ifeq ($(wildcard boards/$(BOARD)/.),)
    $(error Invalid BOARD specified)
  endif
endif

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

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

CROSS_COMPILE = arm-none-eabi-

INC += \
	-I. \
	-I../.. \
	-I../lib/mp-readline \
	-I../lib/timeutils \
	-I../../lib/tinyusb/src \
	-I../../supervisor/shared/usb \
	-I$(BUILD) \
	-Iboards/ \
	-Iboards/$(BOARD) \
	-Iperipherals/ \
	-Iperipherals/mimxrt10xx/ \
	-Isdk/CMSIS/Include \
	-Isdk/devices/$(CHIP_FAMILY) \
	-Isdk/devices/$(CHIP_FAMILY)/drivers \
	-Isdk/devices/$(CHIP_FAMILY)/xip \

# NDEBUG disables assert() statements. This reduces code size pretty dramatically, per tannewt.

CFLAGS += -Os -ftree-vrp -DNDEBUG -ffreestanding

# TinyUSB defines
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_MIMXRT10XX -DCFG_TUD_MIDI_RX_BUFSIZE=512 -DCFG_TUD_CDC_RX_BUFSIZE=512 -DCFG_TUD_MIDI_TX_BUFSIZE=512 -DCFG_TUD_CDC_TX_BUFSIZE=512 -DCFG_TUD_MSC_BUFSIZE=1024

#Debugging/Optimization
ifeq ($(DEBUG), 1)
  # You may want to disable -flto if it interferes with debugging.
  # CFLAGS += -flto -flto-partition=none
  # You may want to enable these flags to make setting breakpoints easier.
  CFLAGS += -fno-inline -fno-ipa-sra
else
  #CFLAGS += -flto -flto-partition=none
endif

CFLAGS += $(INC) -ggdb -Wall -Wno-cast-align -std=gnu11 -nostdlib -fshort-enums $(BASE_CFLAGS) $(CFLAGS_MOD) $(COPT)

# TODO: add these when -Werror is applied
# Disable some warnings, as do most ports. NXP SDK causes undef, tinyusb causes cast-align
# CFLAGS += -Wno-undef -Wno-cast-align

CFLAGS += \
	-mthumb \
	-mapcs \
	-mcpu=cortex-m7 \
	-mfloat-abi=hard \
	-mfpu=fpv5-sp-d16 \
	-DCPU_$(CHIP_VARIANT) \
	-DDEBUG \
	-DIMXRT10XX \
	-g3 -Wno-unused-parameter \
	-ffunction-sections -fdata-sections -fstack-usage

OPTIMIZATION_FLAGS ?= -O2 -fno-inline-functions

# option to override compiler optimization level, set in boards/$(BOARD)/mpconfigboard.mk
CFLAGS += $(OPTIMIZATION_FLAGS)

LD_FILES = $(wildcard boards/$(BOARD)/*.ld) $(addprefix linking/, flash/$(FLASH).ld chip_family/$(CHIP_FAMILY).ld common.ld)

LD_SCRIPT_FLAG := -Wl,-T,

LDFLAGS = $(CFLAGS) -nostartfiles -Wl,-nostdlib $(addprefix $(LD_SCRIPT_FLAG), $(LD_FILES)) -Wl,-Map=$@.map -Wl,-cref -Wl,-gc-sections -specs=nano.specs
LIBS := -lgcc -lc -lnosys -lm

# Use toolchain libm if we're not using our own.
ifndef INTERNAL_LIBM
LIBS += -lm
endif

LDFLAGS += -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mthumb -mapcs
BOOTLOADER_SIZE := 0x6000C000

SRC_SDK := \
	drivers/fsl_adc.c \
	drivers/fsl_cache.c \
	drivers/fsl_clock.c \
	drivers/fsl_common.c \
	drivers/fsl_flexspi.c \
	drivers/fsl_gpio.c \
	drivers/fsl_lpi2c.c \
	drivers/fsl_lpspi.c \
	drivers/fsl_lpuart.c \
	drivers/fsl_ocotp.c \
	drivers/fsl_pwm.c \
	drivers/fsl_snvs_hp.c \
	drivers/fsl_tempmon.c \
	drivers/fsl_trng.c \
	system_$(CHIP_FAMILY).c \

SRC_SDK := $(addprefix sdk/devices/$(CHIP_FAMILY)/, $(SRC_SDK))
$(addprefix $(BUILD)/, $(SRC_SDK:.c=.o)): CFLAGS += -Wno-undef

SRC_C += \
	background.c \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/flash_config.c \
	boards/$(BOARD)/pins.c \
	fatfs_port.c \
	lib/tinyusb/src/portable/nxp/transdimension/dcd_transdimension.c \
	mphalport.c \
	peripherals/mimxrt10xx/$(CHIP_FAMILY)/clocks.c \
	peripherals/mimxrt10xx/$(CHIP_FAMILY)/periph.c \
	peripherals/mimxrt10xx/$(CHIP_FAMILY)/pins.c \
	reset.c \
	supervisor/flexspi_nor_flash_ops.c


ifeq ($(CIRCUITPY_NETWORK),1)
CFLAGS += -DMICROPY_PY_NETWORK=1

SRC_MOD += lib/netutils/netutils.c

ifneq ($(MICROPY_PY_WIZNET5K),0)
WIZNET5K_DIR=drivers/wiznet5k
INC += -I$(TOP)/$(WIZNET5K_DIR)
CFLAGS_MOD += -DMICROPY_PY_WIZNET5K=$(MICROPY_PY_WIZNET5K) -D_WIZCHIP_=$(MICROPY_PY_WIZNET5K)
SRC_MOD += $(addprefix $(WIZNET5K_DIR)/,\
        ethernet/w$(MICROPY_PY_WIZNET5K)/w$(MICROPY_PY_WIZNET5K).c \
        ethernet/wizchip_conf.c \
        ethernet/socket.c \
        internet/dns/dns.c \
        internet/dhcp/dhcp.c \
        )

endif # MICROPY_PY_WIZNET5K
endif # CIRCUITPY_NETWORK

ifeq ($(CIRCUITPY_NETWORK),1)
ifneq ($(MICROPY_PY_WIZNET5K),0)
SRC_SHARED_MODULE += wiznet/__init__.c wiznet/wiznet5k.c
endif
endif

# TODO
#ifeq ($(CIRCUITPY_AUDIOBUSIO),1)
#SRC_C += peripherals/samd/i2s.c peripherals/samd/$(CHIP_FAMILY)/i2s.c
#endif
#
SRC_COMMON_HAL_EXPANDED = $(addprefix shared-bindings/, $(SRC_COMMON_HAL)) \
                          $(addprefix shared-bindings/, $(SRC_BINDINGS_ENUMS)) \
                          $(addprefix common-hal/, $(SRC_COMMON_HAL))

SRC_SHARED_MODULE_EXPANDED = $(addprefix shared-bindings/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE_INTERNAL))

SRC_S = \
    sdk/devices/$(CHIP_FAMILY)/gcc/startup_$(CHIP_FAMILY).S \
    supervisor/cpu.S

OBJ = $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SDK:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_EXPANDED:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SHARED_MODULE_EXPANDED:.c=.o))
ifeq ($(INTERNAL_LIBM),1)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
endif
OBJ += $(addprefix $(BUILD)/, $(SRC_CIRCUITPY_COMMON:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.S=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))

SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED)

all: $(BUILD)/firmware.bin $(BUILD)/firmware.uf2 $(BUILD)/firmware.hex

$(BUILD)/firmware.elf: $(OBJ) $(LD_FILES)
	$(STEPECHO) "LINK $@"
	$(Q)$(CC) -o $@ $(LDFLAGS) $(filter-out %.ld, $^) -Wl,--start-group $(LIBS) -Wl,--end-group

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -j .flash_config -j .ivt -j .text -j .ARM.exidx -j .data -j .itcm -j .dtcm_data $^ $@

$(BUILD)/firmware.uf2: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -j .text -j .ARM.exidx -j .data -j .itcm -j .dtcm_data $^ $@-binpart
	$(Q)$(PYTHON3) $(TOP)/tools/uf2/utils/uf2conv.py -b $(BOOTLOADER_SIZE) -f MIMXRT10XX -c -o $@ $@-binpart
	$(Q)rm $@-binpart

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O ihex -j .flash_config -j .ivt -j .text -j .ARM.exidx -j .data -j .itcm -j .dtcm_data $< $@

include $(TOP)/py/mkrules.mk

# Print out the value of a make variable.
# https://stackoverflow.com/questions/16467718/how-to-print-out-a-variable-in-makefile
print-%:
	@echo $* = $($*)
