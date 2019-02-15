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
include ../circuitpy-common/mpconfig_circuitpy.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

include $(TOP)/supervisor/supervisor.mk

CROSS_COMPILE = arm-none-eabi-

HAL_DIR=hal/$(MCU_SERIES)

INC += -I. \
       -I../.. \
       -I../lib/mp-readline \
       -I../lib/timeutils \
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

BASE_CFLAGS = \
	-fsingle-precision-constant \
	-fno-strict-aliasing \
	-Wdouble-promotion \
	-Wno-endif-labels \
	-Wstrict-prototypes \
	-Werror-implicit-function-declaration \
	-Wfloat-equal \
	-Wundef \
	-Wshadow \
	-Wwrite-strings \
	-Wsign-compare \
	-Wmissing-format-attribute \
	-Wno-deprecated-declarations \
	-Wnested-externs \
	-Wunreachable-code \
	-Wcast-align \
	-Wno-error=lto-type-mismatch \
	-D__$(CHIP_VARIANT)__ \
	-ffunction-sections \
	-fdata-sections \
	-fshort-enums \
	-DCIRCUITPY_SOFTWARE_SAFE_MODE=0x0ADABEEF \
	-DCIRCUITPY_CANARY_WORD=0xADAF00 \
	-DCIRCUITPY_SAFE_RESTART_WORD=0xDEADBEEF \
	--param max-inline-insns-single=500

#        Use these flags to debug build times and header includes.
#        -ftime-report
#        -H

# NDEBUG disables assert() statements. This reduces code size pretty dramatically, per tannewt.

ifeq ($(CHIP_FAMILY), samd21)
CFLAGS += -Os -DNDEBUG
# TinyUSB defines
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_SAMD21 -DCFG_TUD_MIDI_RX_BUFSIZE=128 -DCFG_TUD_CDC_RX_BUFSIZE=128 -DCFG_TUD_MIDI_TX_BUFSIZE=128 -DCFG_TUD_CDC_TX_BUFSIZE=128 -DCFG_TUD_MSC_BUFSIZE=512
endif

ifeq ($(CHIP_FAMILY), samd51)
CFLAGS += -Os -DNDEBUG
# TinyUSB defines
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_SAMD51 -DCFG_TUD_MIDI_RX_BUFSIZE=128 -DCFG_TUD_CDC_RX_BUFSIZE=256 -DCFG_TUD_MIDI_TX_BUFSIZE=128 -DCFG_TUD_CDC_TX_BUFSIZE=256 -DCFG_TUD_MSC_BUFSIZE=1024
endif

#Debugging/Optimization
ifeq ($(DEBUG), 1)
  # Turn on Python modules useful for debugging (e.g. uheap, ustack).
  CFLAGS += -ggdb
  # You may want to disable -flto if it interferes with debugging.
  # CFLAGS += -flto
  # You may want to enable these flags to make setting breakpoints easier.
  CFLAGS += -fno-inline -fno-ipa-sra
  ifeq ($(CHIP_FAMILY), samd21)
    CFLAGS += -DENABLE_MICRO_TRACE_BUFFER
  endif
else
  # -finline-limit can shrink the image size.
  # -finline-limit=80 or so is similar to not having it on.
  # There is no simple default value, though.
  ifdef INTERNAL_FLASH_FILESYSTEM
    CFLAGS += -finline-limit=50
  endif
  ifdef CFLAGS_INLINE_LIMIT
    CFLAGS += -finline-limit=$(CFLAGS_INLINE_LIMIT)
  endif
  CFLAGS += -flto
endif

CFLAGS += $(INC) -Wall -Werror -std=gnu11 -nostdlib $(BASE_CFLAGS) $(CFLAGS_MOD) $(COPT)

ifeq ($(CHIP_FAMILY), samd21)
CFLAGS += \
	-mthumb \
	-mabi=aapcs-linux \
	-mcpu=cortex-m0plus \
	-msoft-float \
	-mfloat-abi=soft \
	-DSAMD21
endif
ifeq ($(CHIP_FAMILY), samd51)
CFLAGS += \
	-mthumb \
	-mabi=aapcs-linux \
	-mcpu=cortex-m4 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	-DSAMD51
endif

ifneq ($(FROZEN_DIR),)
# To use frozen source modules, put your .py files in a subdirectory (eg scripts/)
# and then invoke make with FROZEN_DIR=scripts (be sure to build from scratch).
CFLAGS += -DMICROPY_MODULE_FROZEN_STR
CFLAGS += -Wno-error=lto-type-mismatch
endif

# To use frozen bytecode, put your .py files in a subdirectory (eg frozen/) and
# then invoke make with FROZEN_MPY_DIR=frozen or FROZEN_MPY_DIRS="dir1 dir2"
# (be sure to build from scratch).

ifneq ($(FROZEN_MPY_DIRS),)
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
CFLAGS += -DMICROPY_MODULE_FROZEN_MPY
CFLAGS += -Wno-error=lto-type-mismatch
endif

LDFLAGS = $(CFLAGS) -nostartfiles -fshort-enums -Wl,-nostdlib -Wl,-T,$(LD_FILE) -Wl,-Map=$@.map -Wl,-cref -Wl,-gc-sections -specs=nano.specs
LIBS := -lgcc -lc

# Use toolchain libm if we're not using our own.
ifndef INTERNAL_LIBM
LIBS += -lm
endif

# Propagate longint choice from .mk to C. There's no easy string comparison
# in cpp coniditionals, so we #define separate names for each.
ifeq ($(LONGINT_IMPL),NONE)
CFLAGS += -DLONGINT_IMPL_NONE
endif

ifeq ($(LONGINT_IMPL),MPZ)
CFLAGS += -DLONGINT_IMPL_MPZ
endif

ifeq ($(LONGINT_IMPL),LONGLONG)
CFLAGS += -DLONGINT_IMPL_LONGLONG
endif

ifeq ($(CHIP_FAMILY), samd21)
LDFLAGS += -mthumb -mcpu=cortex-m0plus -Lasf/thirdparty/CMSIS/Lib/GCC/
BOOTLOADER_SIZE := 0x2000
else ifeq ($(CHIP_FAMILY), samd51)
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
	hpl/rtc/hpl_rtc.c \
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

endif

SRC_ASF := $(addprefix asf4/$(CHIP_FAMILY)/, $(SRC_ASF))

SRC_C = \
	audio_dma.c \
	background.c \
	bindings/samd/Clock.c \
	bindings/samd/__init__.c \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/pins.c \
	fatfs_port.c \
	freetouch/adafruit_ptc.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	lib/oofatfs/ff.c \
	lib/oofatfs/option/ccsbcs.c \
	lib/timeutils/timeutils.c \
	lib/tinyusb/src/portable/microchip/$(CHIP_FAMILY)/dcd_$(CHIP_FAMILY).c \
	lib/utils/buffer_helper.c \
	lib/utils/context_manager_helpers.c \
	lib/utils/interrupt_char.c \
	lib/utils/pyexec.c \
	lib/utils/stdout_helpers.c \
	lib/utils/sys_stdio_mphal.c \
	mphalport.c \
	peripherals/samd/$(CHIP_FAMILY)/adc.c \
	peripherals/samd/$(CHIP_FAMILY)/cache.c \
	peripherals/samd/$(CHIP_FAMILY)/clocks.c \
	peripherals/samd/$(CHIP_FAMILY)/dma.c \
	peripherals/samd/$(CHIP_FAMILY)/events.c \
	peripherals/samd/$(CHIP_FAMILY)/external_interrupts.c \
	peripherals/samd/$(CHIP_FAMILY)/pins.c \
	peripherals/samd/$(CHIP_FAMILY)/sercom.c \
	peripherals/samd/$(CHIP_FAMILY)/timers.c \
	peripherals/samd/clocks.c \
	peripherals/samd/dma.c \
	peripherals/samd/events.c \
	peripherals/samd/external_interrupts.c \
	peripherals/samd/sercom.c \
	peripherals/samd/timers.c \
	reset.c \
	supervisor/shared/memory.c \
	tick.c \

ifeq ($(CIRCUITPY_NETWORK),1)
CFLAGS += -DCIRCUITPY_NETWORK=1 -DMICROPY_PY_NETWORK=1

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

ifeq ($(CIRCUITPY_ANALOGIO),1)
SRC_DIRS += analogio/
endif
ifeq ($(CIRCUITPY_AUDIOBUSIO),1)
SRC_DIRS += audiobusio/
endif
ifeq ($(CIRCUITPY_BITBANGIO),1)
SRC_DIRS += bitbangio/
endif
ifeq ($(CIRCUITPY_BOARD),1)
SRC_DIRS += board/
endif
ifeq ($(CIRCUITPY_BUSIO),1)
SRC_DIRS += busio/ bitbangio/OneWire
endif
ifeq ($(CIRCUITPY_DIGITALIO),1)
SRC_DIRS += digitalio/
endif
ifeq ($(CIRCUITPY_DISPLAYIO),1)
SRC_DIRS += displayio/
endif
ifeq ($(CIRCUITPY_GAMEPAD),1)
SRC_DIRS += gamepad/
endif
ifeq ($(CIRCUITPY_I2CSLAVE),1)
SRC_DIRS += i2cslave/
endif
ifeq ($(CIRCUITPY_MATH),1)
SRC_DIRS += math/
endif
ifeq ($(CIRCUITPY_MICROCONTROLLER),1)
SRC_DIRS += microcontroller/
endif
ifeq ($(CIRCUITPY_NEOPIXEL_WRITE),1)
SRC_DIRS += neopixel_write/
endif
ifeq ($(CIRCUITPY_NETWORK),1)
SRC_DIRS += network/
endif
ifeq ($(CIRCUITPY_NVM),1)
SRC_DIRS += nvm/
endif
ifeq ($(CIRCUITPY_OS),1)
SRC_DIRS += os/
endif
ifeq ($(CIRCUITPY_PIXELBUF),1)
SRC_DIRS += _pixelbuf/
endif
ifeq ($(CIRCUITPY_PULSEIO),1)
SRC_DIRS += pulseio/
endif
ifeq ($(CIRCUITPY_RANDOM),1)
SRC_DIRS += random/
endif
ifeq ($(CIRCUITPY_ROTARYIO),1)
SRC_DIRS += rotaryio/
endif
ifeq ($(CIRCUITPY_RTC),1)
SRC_DIRS += rtc/
endif
ifeq ($(CIRCUITPY_SAMD),1)
SRC_DIRS += samd/
endif
ifeq ($(CIRCUITPY_STAGE),1)
SRC_DIRS += _stage/
endif
ifeq ($(CIRCUITPY_STORAGE),1)
SRC_DIRS += storage/
endif
ifeq ($(CIRCUITPY_STRUCT),1)
SRC_DIRS += struct/
endif
ifeq ($(CIRCUITPY_SUPERVISOR),1)
SRC_DIRS += supervisor/
endif
ifeq ($(CIRCUITPY_TIME),1)
SRC_DIRS += time/
endif
ifeq ($(CIRCUITPY_TOUCHIO),1)
SRC_DIRS += touchio/
endif
ifeq ($(CIRCUITPY_UHEAP),1)
SRC_DIRS += uheap/
endif
ifeq ($(CIRCUITPY_USB_HID),1)
SRC_DIRS += usb_hid/
endif
ifeq ($(CIRCUITPY_USB_MIDI),1)
SRC_DIRS += usb_midi/
endif
ifeq ($(CIRCUITPY_USTACK),1)
SRC_DIRS += ustack/
endif

# All possible sources are listed here, and are filtered by SRC_DIRS.
SRC_COMMON_HAL = \
$(filter $(patsubst %,%%,$(SRC_DIRS)), \
	analogio/AnalogIn.c \
	analogio/AnalogOut.c \
	analogio/__init__.c \
	audioio/__init__.c \
	audioio/AudioOut.c \
	board/__init__.c \
	busio/I2C.c \
	busio/SPI.c \
	busio/UART.c \
	busio/__init__.c \
	digitalio/DigitalInOut.c \
	digitalio/__init__.c \
	displayio/ParallelBus.c \
	i2cslave/I2CSlave.c \
	i2cslave/__init__.c \
	microcontroller/Pin.c \
	microcontroller/Processor.c \
	microcontroller/__init__.c \
	neopixel_write/__init__.c \
	nvm/ByteArray.c \
	nvm/__init__.c \
	os/__init__.c \
	pulseio/PWMOut.c \
	pulseio/PulseIn.c \
	pulseio/PulseOut.c \
	pulseio/__init__.c \
	rotaryio/IncrementalEncoder.c \
	rotaryio/__init__.c \
	rtc/RTC.c \
	rtc/__init__.c \
	supervisor/Runtime.c \
	supervisor/__init__.c \
	time/__init__.c \
	touchio/TouchIn.c \
	touchio/__init__.c \
)

# These don't have corresponding files in each port but are still located in
# shared-bindings to make it clear what the contents of the modules are.
# All possible sources are listed here, and are filtered by SRC_DIRS.
SRC_BINDINGS_ENUMS = \
$(filter $(patsubst %,%%,$(SRC_DIRS)), \
	digitalio/Direction.c \
	digitalio/DriveMode.c \
	digitalio/Pull.c \
	displayio/Glyph.c \
	microcontroller/RunMode.c \
	math/__init__.c \
	supervisor/__init__.c \
)

SRC_BINDINGS_ENUMS += \
	help.c \
	util.c

# All possible sources are listed here, and are filtered by SRC_DIRS.
SRC_SHARED_MODULE = \
$(filter $(patsubst %,%%,$(SRC_DIRS)), \
	_pixelbuf/PixelBuf.c \
	_pixelbuf/__init__.c \
	_stage/Layer.c \
	_stage/Text.c \
	_stage/__init__.c \
	audiobusio/__init__.c \
	audiobusio/I2SOut.c \
	audiobusio/PDMIn.c \
	audioio/__init__.c \
	audioio/Mixer.c \
	audioio/RawSample.c \
	audioio/WaveFile.c \
	bitbangio/I2C.c \
	bitbangio/OneWire.c \
	bitbangio/SPI.c \
	bitbangio/__init__.c \
	busio/OneWire.c \
	displayio/Bitmap.c \
	displayio/BuiltinFont.c \
	displayio/ColorConverter.c \
	displayio/Display.c \
	displayio/FourWire.c \
	displayio/Group.c \
	displayio/OnDiskBitmap.c \
	displayio/Palette.c \
	displayio/Shape.c \
	displayio/TileGrid.c \
	displayio/__init__.c \
	gamepad/GamePad.c \
	gamepad/__init__.c \
	os/__init__.c \
	random/__init__.c \
	socket/__init__.c \
	network/__init__.c \
	storage/__init__.c \
	struct/__init__.c \
	terminalio/Terminal.c \
	terminalio/__init__.c \
	uheap/__init__.c \
	ustack/__init__.c \
)

ifeq ($(INTERNAL_LIBM),1)
SRC_LIBM = \
$(addprefix lib/,\
	libm/math.c \
	libm/roundf.c \
	libm/fmodf.c \
	libm/nearbyintf.c \
	libm/ef_sqrt.c \
	libm/kf_rem_pio2.c \
	libm/kf_sin.c \
	libm/kf_cos.c \
	libm/kf_tan.c \
	libm/ef_rem_pio2.c \
	libm/sf_sin.c \
	libm/sf_cos.c \
	libm/sf_tan.c \
	libm/sf_frexp.c \
	libm/sf_modf.c \
	libm/sf_ldexp.c \
	libm/asinfacosf.c \
	libm/atanf.c \
	libm/atan2f.c \
	)
endif

ifeq ($(CIRCUITPY_NETWORK),1)
ifneq ($(MICROPY_PY_WIZNET5K),0)
SRC_SHARED_MODULE += wiznet/__init__.c wiznet/wiznet5k.c
endif
endif

# The smallest SAMD51 packages don't have I2S. Everything else does.
ifeq ($(CIRCUITPY_AUDIOBUSIO),1)
SRC_C += peripherals/samd/i2s.c peripherals/samd/$(CHIP_FAMILY)/i2s.c
endif

SRC_COMMON_HAL_EXPANDED = $(addprefix shared-bindings/, $(SRC_COMMON_HAL)) \
                          $(addprefix shared-bindings/, $(SRC_BINDINGS_ENUMS)) \
                          $(addprefix common-hal/, $(SRC_COMMON_HAL))

SRC_SHARED_MODULE_EXPANDED = $(addprefix shared-bindings/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE))

SRC_S = supervisor/$(CHIP_FAMILY)_cpu.s

OBJ = $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_ASF:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_EXPANDED:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SHARED_MODULE_EXPANDED:.c=.o))
ifeq ($(INTERNAL_LIBM),1)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
endif
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))

SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED) $(STM_SRC_C)
# Sources that only hold QSTRs after pre-processing.
SRC_QSTR_PREPROCESSOR += peripherals/samd/$(CHIP_FAMILY)/clocks.c

all: $(BUILD)/firmware.bin $(BUILD)/firmware.uf2

$(BUILD)/firmware.elf: $(OBJ)
	$(STEPECHO) "LINK $@"
	$(Q)$(CC) -o $@ $(LDFLAGS) $^ -Wl,--start-group $(LIBS) -Wl,--end-group
	$(Q)$(SIZE) $@ | $(PYTHON3) $(TOP)/tools/build_memory_info.py $(LD_FILE)

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(STEPECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -j .vectors -j .text -j .data $^ $@

$(BUILD)/firmware.uf2: $(BUILD)/firmware.bin
	$(STEPECHO) "Create $@"
	$(Q)$(PYTHON3) $(TOP)/tools/uf2/utils/uf2conv.py -b $(BOOTLOADER_SIZE) -c -o $@ $^

include $(TOP)/py/mkrules.mk
