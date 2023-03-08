# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
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

HAL_DIR=hal/$(MCU_SERIES)

INC += -I. \
       -I../.. \
       -I../lib/mp-readline \
       -I../shared/timeutils \
       -Iasf4/$(CHIP_FAMILY) \
       -Iasf4/$(CHIP_FAMILY)/hal/include \
       -Iasf4/$(CHIP_FAMILY)/hal/utils/include \
       -Iasf4/$(CHIP_FAMILY)/hri \
       -Iasf4/$(CHIP_FAMILY)/hpl/core \
       -Iasf4/$(CHIP_FAMILY)/hpl/gclk \
       -Iasf4/$(CHIP_FAMILY)/hpl/pm \
       -Iasf4/$(CHIP_FAMILY)/hpl/port \
       -Iasf4/$(CHIP_FAMILY)/hpl/rtc \
       -Iasf4/$(CHIP_FAMILY)/hpl/tc \
       -Iasf4/$(CHIP_FAMILY)/include \
       -Iasf4/$(CHIP_FAMILY)/CMSIS/Include \
       -Iasf4_conf/$(CHIP_FAMILY) \
       -Iboards/$(BOARD) \
       -Iboards/ \
       -Iperipherals/ \
       -Ifreetouch \
       -I../../lib/tinyusb/src \
       -I../../supervisor/shared/usb \
       -I$(BUILD)


# NDEBUG disables assert() statements. This reduces code size pretty dramatically, per tannewt.

ifeq ($(CHIP_FAMILY), samd21)
PERIPHERALS_CHIP_FAMILY=samd21
OPTIMIZATION_FLAGS ?= -Os
# TinyUSB defines
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_SAMD21 -DCFG_TUD_MIDI_RX_BUFSIZE=128 -DCFG_TUD_CDC_RX_BUFSIZE=128 -DCFG_TUD_MIDI_TX_BUFSIZE=128 -DCFG_TUD_CDC_TX_BUFSIZE=128 -DCFG_TUD_MSC_BUFSIZE=512
endif

ifeq ($(CHIP_FAMILY), samd51)
PERIPHERALS_CHIP_FAMILY=sam_d5x_e5x
OPTIMIZATION_FLAGS ?= -Os
# TinyUSB defines
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_SAMD51 -DCFG_TUD_MIDI_RX_BUFSIZE=128 -DCFG_TUD_CDC_RX_BUFSIZE=256 -DCFG_TUD_MIDI_TX_BUFSIZE=128 -DCFG_TUD_CDC_TX_BUFSIZE=256 -DCFG_TUD_MSC_BUFSIZE=1024
endif

ifeq ($(CHIP_FAMILY), same51)
PERIPHERALS_CHIP_FAMILY=sam_d5x_e5x
OPTIMIZATION_FLAGS ?= -Os
# TinyUSB defines
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_SAME5X -DCFG_TUD_MIDI_RX_BUFSIZE=128 -DCFG_TUD_CDC_RX_BUFSIZE=256 -DCFG_TUD_MIDI_TX_BUFSIZE=128 -DCFG_TUD_CDC_TX_BUFSIZE=256 -DCFG_TUD_MSC_BUFSIZE=1024
endif

ifeq ($(CHIP_FAMILY), same54)
PERIPHERALS_CHIP_FAMILY=sam_d5x_e5x
OPTIMIZATION_FLAGS ?= -Os
# TinyUSB defines
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_SAME5X -DCFG_TUD_MIDI_RX_BUFSIZE=128 -DCFG_TUD_CDC_RX_BUFSIZE=256 -DCFG_TUD_MIDI_TX_BUFSIZE=128 -DCFG_TUD_CDC_TX_BUFSIZE=256 -DCFG_TUD_MSC_BUFSIZE=1024
endif

# option to override default optimization level, set in boards/$(BOARD)/mpconfigboard.mk
CFLAGS += $(OPTIMIZATION_FLAGS)

# Add -ftree-vrp optimization and checking to all builds. It's not enabled for -Os by default.
CFLAGS += -ftree-vrp

$(echo PERIPHERALS_CHIP_FAMILY=$(PERIPHERALS_CHIP_FAMILY))
#Debugging/Optimization
ifeq ($(DEBUG), 1)
  CFLAGS += -ggdb3 -Og -Os
  # You may want to enable these flags to make setting breakpoints easier.
  # CFLAGS += -fno-inline -fno-ipa-sra
  ifeq ($(CHIP_FAMILY), samd21)
    CFLAGS += -DENABLE_MICRO_TRACE_BUFFER
  endif
else
  CFLAGS += -DNDEBUG
  # -finline-limit can shrink the image size.
  # -finline-limit=80 or so is similar to not having it on.
  # There is no simple default value, though.

  # Do a default shrink for small builds.
  ifndef CFLAGS_INLINE_LIMIT
    ifeq ($(CIRCUITPY_FULL_BUILD),0)
      CFLAGS_INLINE_LIMIT = 50
    endif
  endif

  ifdef CFLAGS_INLINE_LIMIT
    CFLAGS += -finline-limit=$(CFLAGS_INLINE_LIMIT)
  endif

  ifeq ($(CIRCUITPY_FULL_BUILD),0)
    CFLAGS += --param inline-unit-growth=15 --param max-inline-insns-auto=20
  endif

  ifdef CFLAGS_BOARD
    CFLAGS += $(CFLAGS_BOARD)
  endif
endif

CFLAGS += $(INC) -Wall -Werror -std=gnu11 -nostdlib -fshort-enums $(BASE_CFLAGS) $(CFLAGS_MOD) $(COPT) -Werror=missing-prototypes

ifeq ($(CHIP_FAMILY), samd21)
CFLAGS += \
	-mthumb \
	-mabi=aapcs-linux \
	-mcpu=cortex-m0plus \
	-msoft-float \
	-mfloat-abi=soft \
	-DSAMD21
LIBS := libs/libgcc-12.1.0-Os-v6-m-nofp.a -lc
else
LIBS := -lgcc -lc
endif
ifeq ($(CHIP_FAMILY), samd51)
CFLAGS += \
	-mthumb \
	-mabi=aapcs-linux \
	-mcpu=cortex-m4 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	-DSAM_D5X_E5X -DSAMD51
endif
ifeq ($(CHIP_FAMILY), same54)
CFLAGS += \
	-mthumb \
	-mabi=aapcs-linux \
	-mcpu=cortex-m4 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	-DSAM_D5X_E5X -DSAME54
endif
ifeq ($(CHIP_FAMILY), same51)
CFLAGS += \
	-mthumb \
	-mabi=aapcs-linux \
	-mcpu=cortex-m4 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	-DSAM_D5X_E5X -DSAME51
endif

# GCC 11 adds stringop bounds checks that trigger when writing a memory region
# we know is ok. It's not clear how to give the compiler the info it needs so
# disable the checks for now.
# See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99578
CFLAGS += -Wno-stringop-overread -Wno-stringop-overflow

LDFLAGS = $(CFLAGS) -nostartfiles -Wl,-nostdlib -Wl,-T,$(GENERATED_LD_FILE) -Wl,-Map=$@.map -Wl,-cref -Wl,-gc-sections -specs=nano.specs

# Use toolchain libm if we're not using our own.
ifndef INTERNAL_LIBM
LIBS += -lm
endif

ifeq ($(CHIP_FAMILY), samd21)
LDFLAGS += -mthumb -mcpu=cortex-m0plus -Lasf/thirdparty/CMSIS/Lib/GCC/
BOOTLOADER_SIZE := 0x2000
else ifeq ($(CHIP_FAMILY), samd51)
LDFLAGS += -mthumb -mcpu=cortex-m4
BOOTLOADER_SIZE := 0x4000
else ifeq ($(CHIP_FAMILY), same54)
LDFLAGS += -mthumb -mcpu=cortex-m4
BOOTLOADER_SIZE := 0x4000
else ifeq ($(CHIP_FAMILY), same51)
LDFLAGS += -mthumb -mcpu=cortex-m4
BOOTLOADER_SIZE := 0x4000
endif

SRC_ASF := \
	gcc/gcc/startup_$(CHIP_FAMILY).c \
	gcc/system_$(CHIP_FAMILY).c \
	hal/src/hal_adc_sync.c \
	hal/src/hal_atomic.c \
	hal/src/hal_calendar.c \
	hal/src/hal_dac_sync.c \
	hal/src/hal_delay.c \
	hal/src/hal_flash.c \
	hal/src/hal_i2c_m_sync.c \
	hal/src/hal_io.c \
	hal/src/hal_sleep.c \
	hal/src/hal_spi_m_sync.c \
	hal/src/hal_timer.c \
	hal/src/hal_usart_async.c \
	hpl/adc/hpl_adc.c \
	hpl/core/hpl_init.c \
	hpl/dac/hpl_dac.c \
	hpl/gclk/hpl_gclk.c \
	hpl/nvmctrl/hpl_nvmctrl.c \
	hpl/pm/hpl_pm.c \
	hpl/sercom/hpl_sercom.c \
	hpl/systick/hpl_systick.c \
	hal/utils/src/utils_list.c \
	hal/utils/src/utils_ringbuffer.c \

ifeq ($(CHIP_FAMILY), samd21)
SRC_ASF += \
	hpl/core/hpl_core_m0plus_base.c \
	hpl/sysctrl/hpl_sysctrl.c \

else ifeq ($(CHIP_FAMILY), samd51)
SRC_ASF += \
	hal/src/hal_rand_sync.c \
	hpl/core/hpl_core_m4.c \
	hpl/mclk/hpl_mclk.c \
	hpl/osc32kctrl/hpl_osc32kctrl.c \
	hpl/oscctrl/hpl_oscctrl.c \
	hpl/trng/hpl_trng.c \

else ifeq ($(CHIP_FAMILY), same54)
SRC_ASF += \
	hal/src/hal_rand_sync.c \
	hpl/core/hpl_core_m4.c \
	hpl/mclk/hpl_mclk.c \
	hpl/osc32kctrl/hpl_osc32kctrl.c \
	hpl/oscctrl/hpl_oscctrl.c \
	hpl/trng/hpl_trng.c \

# Ignore these errors
$(BUILD)/asf4/same54/hpl/sercom/hpl_sercom.o: CFLAGS += -Wno-maybe-uninitialized

else ifeq ($(CHIP_FAMILY), same51)
SRC_ASF += \
	hal/src/hal_rand_sync.c \
	hpl/core/hpl_core_m4.c \
	hpl/mclk/hpl_mclk.c \
	hpl/osc32kctrl/hpl_osc32kctrl.c \
	hpl/oscctrl/hpl_oscctrl.c \
	hpl/trng/hpl_trng.c \

endif

ifeq ($(CIRCUITPY_SDIOIO),1)
SRC_ASF += \
	hal/src/hal_mci_sync.c \
	hpl/sdhc/hpl_sdhc.c \

$(BUILD)/asf4/$(CHIP_FAMILY)/hpl/sdhc/hpl_sdhc.o: CFLAGS += -Wno-cast-align -Wno-implicit-fallthrough
endif

SRC_ASF := $(addprefix asf4/$(CHIP_FAMILY)/, $(SRC_ASF))
$(patsubst %.c,$(BUILD)/%.o,$(SRC_ASF)): CFLAGS += -Wno-missing-prototypes

SRC_PERIPHERALS := \
	peripherals/samd/$(PERIPHERALS_CHIP_FAMILY)/adc.c \
	peripherals/samd/$(PERIPHERALS_CHIP_FAMILY)/cache.c \
	peripherals/samd/$(PERIPHERALS_CHIP_FAMILY)/clocks.c \
	peripherals/samd/$(PERIPHERALS_CHIP_FAMILY)/dma.c \
	peripherals/samd/$(PERIPHERALS_CHIP_FAMILY)/events.c \
	peripherals/samd/$(PERIPHERALS_CHIP_FAMILY)/external_interrupts.c \
	peripherals/samd/$(PERIPHERALS_CHIP_FAMILY)/pins.c \
	peripherals/samd/$(PERIPHERALS_CHIP_FAMILY)/sercom.c \
	peripherals/samd/$(PERIPHERALS_CHIP_FAMILY)/timers.c \
	peripherals/samd/clocks.c \
	peripherals/samd/dma.c \
	peripherals/samd/events.c \
	peripherals/samd/external_interrupts.c \
	peripherals/samd/sercom.c \
	peripherals/samd/timers.c \

$(patsubst %.c,$(BUILD)/%.o,$(SRC_PERIPHERALS)): CFLAGS += -Wno-missing-prototypes

SRC_C += \
	audio_dma.c \
	background.c \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/pins.c \
	eic_handler.c \
	lib/tinyusb/src/portable/microchip/samd/dcd_samd.c \
	mphalport.c \
	reset.c \
	timer_handler.c \
	$(SRC_PERIPHERALS) \

$(BUILD)/lib/tinyusb/src/portable/microchip/samd/dcd_samd.o: CFLAGS += -Wno-missing-prototypes

# This is an OR because it filters to any 1s and then checks to see if it is not
# empty.
ifneq (,$(filter 1,$(CIRCUITPY_PWMIO) $(CIRCUITPY_AUDIOIO) $(CIRCUITPY_RGBMATRIX)))
SRC_C += shared_timers.c
endif

ifeq ($(CIRCUITPY_SAMD),1)
SRC_C += bindings/samd/Clock.c bindings/samd/__init__.c
endif

ifeq ($(CIRCUITPY_SDIOIO),1)
SRC_C += ports/atmel-samd/sd_mmc/sd_mmc.c
endif

ifeq ($(CIRCUITPY_TOUCHIO),1)
SRC_C += freetouch/adafruit_ptc.c
endif

# The smallest SAMD51 packages don't have I2S. Everything else does.
ifeq ($(CIRCUITPY_AUDIOBUSIO),1)
SRC_C += peripherals/samd/i2s.c peripherals/samd/$(PERIPHERALS_CHIP_FAMILY)/i2s.c
endif

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

SRC_S = supervisor/$(CHIP_FAMILY)_cpu.s

OBJ = $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_ASF:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_SHARED_MODULE_EXPANDED:.c=.o))
ifeq ($(INTERNAL_LIBM),1)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
endif
OBJ += $(addprefix $(BUILD)/, $(SRC_CIRCUITPY_COMMON:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))

SRC_QSTR += $(HEADER_BUILD)/sdiodata.h
$(HEADER_BUILD)/sdiodata.h: tools/mksdiodata.py | $(HEADER_BUILD)
	$(Q)$(PYTHON) $< > $@

SRC_QSTR += $(HEADER_BUILD)/candata.h
$(HEADER_BUILD)/candata.h: tools/mkcandata.py | $(HEADER_BUILD)
	$(Q)$(PYTHON) $< > $@

SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED)
# Sources that only hold QSTRs after pre-processing.
SRC_QSTR_PREPROCESSOR += peripherals/samd/$(PERIPHERALS_CHIP_FAMILY)/clocks.c

all: $(BUILD)/firmware.bin $(BUILD)/firmware.uf2

$(BUILD)/firmware.elf: $(OBJ) $(GENERATED_LD_FILE)
	$(STEPECHO) "LINK $@"
	$(Q)echo $(OBJ) > $(BUILD)/firmware.objs
	$(Q)$(CC) -o $@ $(LDFLAGS) @$(BUILD)/firmware.objs -Wl,--start-group $(LIBS) -Wl,--end-group
	$(Q)$(SIZE) $@ | $(PYTHON) $(TOP)/tools/build_memory_info.py $(GENERATED_LD_FILE) $(BUILD)

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -j .vectors -j .text -j .data $^ $@

$(BUILD)/firmware.uf2: $(BUILD)/firmware.bin
	$(STEPECHO) "Create $@"
	$(Q)$(PYTHON) $(TOP)/tools/uf2/utils/uf2conv.py -b $(BOOTLOADER_SIZE) -c -o $@ $^

include $(TOP)/py/mkrules.mk

# Print out the value of a make variable.
# https://stackoverflow.com/questions/16467718/how-to-print-out-a-variable-in-makefile
print-%:
	@echo $* = $($*)
