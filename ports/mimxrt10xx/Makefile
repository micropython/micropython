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

include ../../py/circuitpy_mkenv.mk

CROSS_COMPILE = arm-none-eabi-

INC += \
	-I. \
	-I../.. \
	-I../../lib/cmsis/inc \
	-I../../lib/tinyusb/src \
	-I../../supervisor/shared/usb \
	-I$(BUILD) \
	-Iboards \
	-Iboards/$(BOARD) \
	-Iperipherals/ \
	-Iperipherals/mimxrt10xx/ \
	-Isdk/CMSIS/Include/ \
	-Isdk/devices/$(CHIP_FAMILY) \
	-Isdk/devices/$(CHIP_FAMILY)/drivers \
	-Isdk/drivers/common

# NDEBUG disables assert() statements. This reduces code size pretty dramatically, per tannewt.

CFLAGS += -ftree-vrp -DNDEBUG

# TinyUSB defines
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_MIMXRT10XX -DCFG_TUD_CDC_RX_BUFSIZE=512 -DCFG_TUD_CDC_TX_BUFSIZE=512
ifeq ($(CHIP_FAMILY),$(filter $(CHIP_FAMILY),MIMXRT1011 MIMXRT1015))
CFLAGS += -DCFG_TUD_MIDI_RX_BUFSIZE=512 -DCFG_TUD_MIDI_TX_BUFSIZE=64 -DCFG_TUD_MSC_BUFSIZE=512
else
CFLAGS += -DCFG_TUD_MIDI_RX_BUFSIZE=512 -DCFG_TUD_MIDI_TX_BUFSIZE=512 -DCFG_TUD_MSC_BUFSIZE=1024
endif

#Debugging/Optimization
# Never set -fno-inline because we use inline to move small functions into routines that must be
# in RAM. If inlining is disallowed, then we may end up calling a function in flash when we cannot.
ifeq ($(DEBUG), 1)
  # You may want to enable these flags to make setting breakpoints easier.
  CFLAGS += -fno-ipa-sra
endif

CFLAGS += $(INC) -ggdb -Wall -Werror -std=gnu11 -nostdlib -fshort-enums $(BASE_CFLAGS) $(CFLAGS_MOD) $(COPT) -Werror=missing-prototypes

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
	-DIMXRT1XXX \
	-g3 -Wno-unused-parameter \
	-ffunction-sections -fdata-sections -fstack-usage

OPTIMIZATION_FLAGS ?= -O2

# option to override compiler optimization level, set in boards/$(BOARD)/mpconfigboard.mk
CFLAGS += $(OPTIMIZATION_FLAGS)

ifeq ($(CIRCUITPY_SWO_TRACE), 1)
  CFLAGS += -finstrument-functions -finstrument-functions-exclude-file-list=tinyusb -finstrument-functions-exclude-function-list='USB_OTG1_IRQHandler,usb_irq_handler,nlr_push,CLOCK_EnableClock,CLOCK_SetDiv,CLOCK_SetMux,__DMB,__ISB,__DSB,SCB_EnableICache,SCB_EnableDCache,ARM_MPU_Disable,ARM_MPU_Enable,SCB_DisableDCache,SCB_DisableICache,__enable_irq,__disable_irq,__set_MSP,port_get_raw_ticks,supervisor_ticks_ms64'
endif

LD_FILES = $(wildcard boards/$(BOARD)/*.ld) $(addprefix linking/, flash/$(FLASH).ld chip_family/$(CHIP_FAMILY).ld common.ld)

LD_SCRIPT_FLAG := -Wl,-T,

LDFLAGS = $(CFLAGS) -nostartfiles -Wl,-nostdlib $(addprefix $(LD_SCRIPT_FLAG), $(LD_FILES)) -Wl,-Map=$@.map -Wl,-cref -Wl,-gc-sections -specs=nano.specs
LIBS := -lgcc -lc -lnosys -lm

# Use toolchain libm if we're not using our own.
ifndef INTERNAL_LIBM
LIBS += -lm
endif

ifndef CHIP_CORE
CHIP_CORE = $(CHIP_FAMILY)
endif

# If not empty, then it is 10xx.
ifneq ($(findstring MIMXRT10, $(CHIP_FAMILY)),)
CFLAGS += -DIMXRT10XX=1 -DIMXRT11XX=0
MIMXRT10xx = $(CHIP_FAMILY)
BOOTLOADER_SIZE := 0x6000C000
else
CFLAGS += -DIMXRT11XX=1 -DIMXRT10XX=0
MIMXRT11xx = $(CHIP_FAMILY)
BOOTLOADER_SIZE := 0x3000C000
endif

LDFLAGS += -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mthumb -mapcs

SRC_SDK := \
	devices/$(CHIP_FAMILY)/drivers/fsl_clock.c \
	devices/$(CHIP_FAMILY)/system_$(CHIP_CORE).c \
	devices/$(CHIP_FAMILY)/xip/fsl_flexspi_nor_boot.c \
	drivers/cache/armv7-m7/fsl_cache.c \
	drivers/common/fsl_common_arm.c \
	drivers/common/fsl_common.c \
	drivers/flexspi/fsl_flexspi.c \
	drivers/igpio/fsl_gpio.c \
	drivers/lpi2c/fsl_lpi2c.c \
	drivers/lpspi/fsl_lpspi.c \
	drivers/lpuart/fsl_lpuart.c \
	drivers/ocotp/fsl_ocotp.c \
	drivers/pwm/fsl_pwm.c \
	drivers/sai/fsl_sai.c \
	drivers/snvs_hp/fsl_snvs_hp.c \
	drivers/snvs_lp/fsl_snvs_lp.c \
	drivers/trng/fsl_trng.c \

ifeq ($(CIRCUITPY_ANALOGIO), 1)
SRC_SDK +=	drivers/adc_12b1msps_sar/fsl_adc.c \
	drivers/tempmon/fsl_tempmon.c
endif

ifeq ($(CHIP_FAMILY), MIMXRT1176)
SRC_SDK += devices/$(CHIP_FAMILY)/drivers/fsl_anatop_ai.c \
	devices/$(CHIP_FAMILY)/drivers/fsl_dcdc.c \
	devices/$(CHIP_FAMILY)/drivers/fsl_pmu.c
endif

SRC_SDK := $(addprefix sdk/, $(SRC_SDK))

$(addprefix $(BUILD)/, $(SRC_SDK:.c=.o)): CFLAGS += -Wno-array-bounds

SRC_C += \
	background.c \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/flash_config.c \
	boards/$(BOARD)/pins.c \
	lib/tinyusb/src/portable/chipidea/ci_hs/dcd_ci_hs.c \
	mphalport.c \
	peripherals/mimxrt10xx/$(CHIP_FAMILY)/clocks.c \
	peripherals/mimxrt10xx/$(CHIP_FAMILY)/periph.c \
	peripherals/mimxrt10xx/$(CHIP_FAMILY)/pins.c \
	peripherals/mimxrt10xx/pins.c \
	reset.c \
	supervisor/flexspi_nor_flash_ops.c

ifeq ($(CIRCUITPY_USB_HOST), 1)
SRC_C += \
	lib/tinyusb/src/portable/chipidea/ci_hs/hcd_ci_hs.c \
	lib/tinyusb/src/portable/ehci/ehci.c \

endif

SRC_COMMON_HAL_EXPANDED = $(addprefix shared-bindings/, $(SRC_COMMON_HAL)) \
                          $(addprefix shared-bindings/, $(SRC_BINDINGS_ENUMS)) \
                          $(addprefix common-hal/, $(SRC_COMMON_HAL))

SRC_SHARED_MODULE_EXPANDED = $(addprefix shared-bindings/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE_INTERNAL))

SRC_S = \
    sdk/devices/$(CHIP_FAMILY)/gcc/startup_$(CHIP_CORE).S \
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

ifeq ($(VALID_BOARD),)
$(BUILD)/firmware.elf: invalid-board
else
$(BUILD)/firmware.elf: $(OBJ) $(LD_FILES)
	$(STEPECHO) "LINK $@"
	$(Q)$(CC) -o $@ $(LDFLAGS) $(filter-out %.ld, $^) -Wl,--print-memory-usage -Wl,--start-group $(LIBS) -Wl,--end-group
endif

# -R excludes sections from the output files.
$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -R .stack -R .dtcm_bss $^ $@

$(BUILD)/firmware.uf2: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -R .stack -R .dtcm_bss -R .ivt -R .flash_config $^ $@-binpart
	$(Q)$(PYTHON) $(TOP)/tools/uf2/utils/uf2conv.py -b $(BOOTLOADER_SIZE) -f MIMXRT10XX -c -o $@ $@-binpart
	$(Q)rm $@-binpart

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O ihex -R .stack -R .dtcm_bss $< $@

include $(TOP)/py/mkrules.mk


# Flash using jlink
define jlink_script
halt
loadfile $^
r
go
exit
endef
export jlink_script

JLINKEXE = JLinkExe
flash-jlink: $(BUILD)/firmware.elf
	@echo "$$jlink_script" > $(BUILD)/firmware.jlink
	$(JLINKEXE) -device $(CHIP_FAMILY)xxx5A -if swd -JTAGConf -1,-1 -speed auto -CommandFile $(BUILD)/firmware.jlink

flash: flash-jlink
