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

# Common Makefile definitions that can be shared across CircuitPython ports.

###
# Common compile warnings.

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


###
# Handle frozen modules.

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


###
# Propagate longint choice from .mk to C. There's no easy string comparison
# in cpp conditionals, so we #define separate names for each.
ifeq ($(LONGINT_IMPL),NONE)
CFLAGS += -DLONGINT_IMPL_NONE
endif

ifeq ($(LONGINT_IMPL),MPZ)
CFLAGS += -DLONGINT_IMPL_MPZ
endif

ifeq ($(LONGINT_IMPL),LONGLONG)
CFLAGS += -DLONGINT_IMPL_LONGLONG
endif


###
# Select which builtin modules to compile and include.

ifeq ($(CIRCUITPY_ANALOGIO),1)
SRC_PATTERNS += analogio/%
endif
ifeq ($(CIRCUITPY_AUDIOBUSIO),1)
SRC_PATTERNS += audiobusio/%
endif
ifeq ($(CIRCUITPY_AUDIOIO),1)
SRC_PATTERNS += audioio/%
endif
ifeq ($(CIRCUITPY_BITBANGIO),1)
SRC_PATTERNS += bitbangio/%
endif
ifeq ($(CIRCUITPY_BLEIO),1)
SRC_PATTERNS += bleio/%
endif
ifeq ($(CIRCUITPY_BOARD),1)
SRC_PATTERNS += board/%
endif
ifeq ($(CIRCUITPY_BUSIO),1)
SRC_PATTERNS += busio/% bitbangio/OneWire.%
endif
ifeq ($(CIRCUITPY_DIGITALIO),1)
SRC_PATTERNS += digitalio/%
endif
ifeq ($(CIRCUITPY_DISPLAYIO),1)
SRC_PATTERNS += displayio/% terminalio/% fontio/%
endif
ifeq ($(CIRCUITPY_FREQUENCYIO),1)
SRC_PATTERNS += frequencyio/%
endif
ifeq ($(CIRCUITPY_GAMEPAD),1)
SRC_PATTERNS += gamepad/%
endif
ifeq ($(CIRCUITPY_GAMEPADSHIFT),1)
SRC_PATTERNS += gamepadshift/%
endif
ifeq ($(CIRCUITPY_I2CSLAVE),1)
SRC_PATTERNS += i2cslave/%
endif
ifeq ($(CIRCUITPY_MATH),1)
SRC_PATTERNS += math/%
endif
ifeq ($(CIRCUITPY_MICROCONTROLLER),1)
SRC_PATTERNS += microcontroller/%
endif
ifeq ($(CIRCUITPY_NEOPIXEL_WRITE),1)
SRC_PATTERNS += neopixel_write/%
endif
ifeq ($(CIRCUITPY_NETWORK),1)
SRC_PATTERNS += network/% socket/%
endif
ifeq ($(CIRCUITPY_NVM),1)
SRC_PATTERNS += nvm/%
endif
ifeq ($(CIRCUITPY_OS),1)
SRC_PATTERNS += os/%
endif
ifeq ($(CIRCUITPY_PIXELBUF),1)
SRC_PATTERNS += _pixelbuf/%
endif
ifeq ($(CIRCUITPY_PULSEIO),1)
SRC_PATTERNS += pulseio/%
endif
ifeq ($(CIRCUITPY_RANDOM),1)
SRC_PATTERNS += random/%
endif
ifeq ($(CIRCUITPY_ROTARYIO),1)
SRC_PATTERNS += rotaryio/%
endif
ifeq ($(CIRCUITPY_RTC),1)
SRC_PATTERNS += rtc/%
endif
ifeq ($(CIRCUITPY_SAMD),1)
SRC_PATTERNS += samd/%
endif
ifeq ($(CIRCUITPY_STAGE),1)
SRC_PATTERNS += _stage/%
endif
ifeq ($(CIRCUITPY_STORAGE),1)
SRC_PATTERNS += storage/%
endif
ifeq ($(CIRCUITPY_STRUCT),1)
SRC_PATTERNS += struct/%
endif
ifeq ($(CIRCUITPY_SUPERVISOR),1)
SRC_PATTERNS += supervisor/%
endif
ifeq ($(CIRCUITPY_TIME),1)
SRC_PATTERNS += time/%
endif
ifeq ($(CIRCUITPY_TOUCHIO),1)
SRC_PATTERNS += touchio/%
endif
ifeq ($(CIRCUITPY_UHEAP),1)
SRC_PATTERNS += uheap/%
endif
ifeq ($(CIRCUITPY_USB_HID),1)
SRC_PATTERNS += usb_hid/%
endif
ifeq ($(CIRCUITPY_USB_MIDI),1)
SRC_PATTERNS += usb_midi/%
endif
ifeq ($(CIRCUITPY_USTACK),1)
SRC_PATTERNS += ustack/%
endif
ifeq ($(CIRCUITPY_PEW),1)
SRC_PATTERNS += _pew/%
endif

# All possible sources are listed here, and are filtered by SRC_PATTERNS.
SRC_COMMON_HAL = \
$(filter $(SRC_PATTERNS), \
	analogio/AnalogIn.c \
	analogio/AnalogOut.c \
	analogio/__init__.c \
	audiobusio/__init__.c \
	audiobusio/I2SOut.c \
	audiobusio/PDMIn.c \
	audioio/__init__.c \
	audioio/AudioOut.c \
	bleio/__init__.c \
	bleio/Adapter.c \
	bleio/Broadcaster.c \
	bleio/Characteristic.c \
	bleio/CharacteristicBuffer.c \
	bleio/Descriptor.c \
	bleio/Peripheral.c \
	bleio/Scanner.c \
	bleio/Service.c \
	bleio/UUID.c \
	board/__init__.c \
	busio/I2C.c \
	busio/SPI.c \
	busio/UART.c \
	busio/__init__.c \
	digitalio/DigitalInOut.c \
	digitalio/__init__.c \
	displayio/ParallelBus.c \
	frequencyio/__init__.c \
	frequencyio/FrequencyIn.c \
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
# All possible sources are listed here, and are filtered by SRC_PATTERNS.
SRC_BINDINGS_ENUMS = \
$(filter $(SRC_PATTERNS), \
	digitalio/Direction.c \
	digitalio/DriveMode.c \
	digitalio/Pull.c \
	fontio/Glyph.c \
	microcontroller/RunMode.c \
	math/__init__.c \
	supervisor/__init__.c \
)

SRC_BINDINGS_ENUMS += \
	help.c \
	util.c

SRC_BINDINGS_ENUMS += \
$(filter $(SRC_PATTERNS), \
	bleio/Address.c \
	bleio/AddressType.c \
	bleio/AdvertisementData.c \
	bleio/ScanEntry.c \
)

# All possible sources are listed here, and are filtered by SRC_PATTERNS.
SRC_SHARED_MODULE = \
$(filter $(SRC_PATTERNS), \
	_pixelbuf/PixelBuf.c \
	_pixelbuf/__init__.c \
	_stage/Layer.c \
	_stage/Text.c \
	_stage/__init__.c \
	audioio/__init__.c \
	audioio/Mixer.c \
	audioio/RawSample.c \
	audioio/WaveFile.c \
	bitbangio/I2C.c \
	bitbangio/OneWire.c \
	bitbangio/SPI.c \
	bitbangio/__init__.c \
	board/__init__.c \
	busio/OneWire.c \
	displayio/Bitmap.c \
	displayio/ColorConverter.c \
	displayio/Display.c \
	displayio/FourWire.c \
	displayio/Group.c \
	displayio/OnDiskBitmap.c \
	displayio/Palette.c \
	displayio/Shape.c \
	displayio/TileGrid.c \
	displayio/__init__.c \
	fontio/BuiltinFont.c \
	fontio/__init__.c \
	gamepad/GamePad.c \
	gamepad/__init__.c \
	gamepadshift/GamePadShift.c \
	gamepadshift/__init__.c \
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
	_pew/__init__.c \
	_pew/PewPew.c \
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
