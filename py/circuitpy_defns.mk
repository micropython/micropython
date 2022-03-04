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

# Common Makefile definitions that can be shared across CircuitPython ports.

###
# Common compile warnings.

BASE_CFLAGS = \
	-fsingle-precision-constant \
	-fno-strict-aliasing \
	-Wdouble-promotion \
	-Wimplicit-fallthrough=2 \
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
	-D__$(CHIP_VARIANT)__ \
	-ffunction-sections \
	-fdata-sections \
	-DCIRCUITPY_SOFTWARE_SAFE_MODE=0x0ADABEEF \
	-DCIRCUITPY_CANARY_WORD=0xADAF00 \
	-DCIRCUITPY_SAFE_RESTART_WORD=0xDEADBEEF \
	-DCIRCUITPY_BOARD_ID="\"$(BOARD)\"" \
	--param max-inline-insns-single=500

#        Use these flags to debug build times and header includes.
#        -ftime-report
#        -H


# Set a global CIRCUITPY_DEBUG flag.
# Don't just call it "DEBUG": too many libraries use plain DEBUG.
ifneq ($(DEBUG),)
CFLAGS += -DCIRCUITPY_DEBUG=$(DEBUG)
else
CFLAGS += -DCIRCUITPY_DEBUG=0
endif

###
# Handle frozen modules.

ifneq ($(FROZEN_DIR),)
# To use frozen source modules, put your .py files in a subdirectory (eg scripts/)
# and then invoke make with FROZEN_DIR=scripts (be sure to build from scratch).
CFLAGS += -DMICROPY_MODULE_FROZEN_STR
endif

# To use frozen bytecode, put your .py files in a subdirectory (eg frozen/) and
# then invoke make with FROZEN_MPY_DIR=frozen or FROZEN_MPY_DIRS="dir1 dir2"
# (be sure to build from scratch).

ifneq ($(FROZEN_MPY_DIRS),)
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
CFLAGS += -DMICROPY_MODULE_FROZEN_MPY
endif

###
# Select which builtin modules to compile and include.

ifeq ($(CIRCUITPY_AESIO),1)
SRC_PATTERNS += aesio/%
endif
ifeq ($(CIRCUITPY_ALARM),1)
SRC_PATTERNS += alarm/%
endif
ifeq ($(CIRCUITPY_ANALOGIO),1)
SRC_PATTERNS += analogio/%
endif
ifeq ($(CIRCUITPY_ATEXIT),1)
SRC_PATTERNS += atexit/%
endif
ifeq ($(CIRCUITPY_AUDIOBUSIO),1)
SRC_PATTERNS += audiobusio/%
endif
ifeq ($(CIRCUITPY_AUDIOIO),1)
SRC_PATTERNS += audioio/%
endif
ifeq ($(CIRCUITPY_AUDIOPWMIO),1)
SRC_PATTERNS += audiopwmio/%
endif
ifeq ($(CIRCUITPY_AUDIOCORE),1)
SRC_PATTERNS += audiocore/%
endif
ifeq ($(CIRCUITPY_AUDIOMIXER),1)
SRC_PATTERNS += audiomixer/%
endif
ifeq ($(CIRCUITPY_AUDIOMP3),1)
SRC_PATTERNS += audiomp3/%
endif
ifeq ($(CIRCUITPY_BITBANGIO),1)
SRC_PATTERNS += bitbangio/%
endif
# Some builds need bitbang SPI for the dotstar but don't make bitbangio available so include it separately.
ifeq ($(CIRCUITPY_BITBANG_APA102),1)
SRC_PATTERNS += bitbangio/SPI%
endif
ifeq ($(CIRCUITPY_BITMAPTOOLS),1)
SRC_PATTERNS += bitmaptools/%
endif
ifeq ($(CIRCUITPY_BITOPS),1)
SRC_PATTERNS += bitops/%
endif
ifeq ($(CIRCUITPY_BLEIO),1)
SRC_PATTERNS += _bleio/%
endif
ifeq ($(CIRCUITPY_BOARD),1)
SRC_PATTERNS += board/%
endif
ifeq ($(CIRCUITPY_BUSDEVICE),1)
SRC_PATTERNS += adafruit_bus_device/%
endif
ifeq ($(CIRCUITPY_BUSIO),1)
SRC_PATTERNS += busio/%
endif
ifeq ($(CIRCUITPY_CAMERA),1)
SRC_PATTERNS += camera/%
endif
ifeq ($(CIRCUITPY_CANIO),1)
SRC_PATTERNS += canio/%
endif
ifeq ($(CIRCUITPY_COUNTIO),1)
SRC_PATTERNS += countio/%
endif
ifeq ($(CIRCUITPY_DIGITALIO),1)
SRC_PATTERNS += digitalio/%
endif
ifeq ($(CIRCUITPY_DISPLAYIO),1)
SRC_PATTERNS += displayio/%
endif
ifeq ($(CIRCUITPY_PARALLELDISPLAY),1)
SRC_PATTERNS += paralleldisplay/%
endif
ifeq ($(CIRCUITPY_VECTORIO),1)
SRC_PATTERNS += vectorio/%
endif
ifeq ($(CIRCUITPY_FRAMEBUFFERIO),1)
SRC_PATTERNS += framebufferio/%
endif
ifeq ($(CIRCUITPY__EVE),1)
SRC_PATTERNS += _eve/%
endif
ifeq ($(CIRCUITPY_FREQUENCYIO),1)
SRC_PATTERNS += frequencyio/%
endif

ifeq ($(CIRCUITPY_FUTURE),1)
SRC_PATTERNS += __future__/%
endif

ifeq ($(CIRCUITPY_GAMEPADSHIFT),1)
SRC_PATTERNS += gamepadshift/%
endif
ifeq ($(CIRCUITPY_GETPASS),1)
SRC_PATTERNS += getpass/%
endif
ifeq ($(CIRCUITPY_GIFIO),1)
SRC_PATTERNS += gifio/%
endif
ifeq ($(CIRCUITPY_GNSS),1)
SRC_PATTERNS += gnss/%
endif
ifeq ($(CIRCUITPY_I2CPERIPHERAL),1)
SRC_PATTERNS += i2cperipheral/%
endif
ifeq ($(CIRCUITPY_IMAGECAPTURE),1)
SRC_PATTERNS += imagecapture/%
endif
ifeq ($(CIRCUITPY_IPADDRESS),1)
SRC_PATTERNS += ipaddress/%
endif
ifeq ($(CIRCUITPY_IS31FL3741),1)
SRC_PATTERNS += is31fl3741/%
endif
ifeq ($(CIRCUITPY_KEYPAD),1)
SRC_PATTERNS += keypad/%
endif
ifeq ($(CIRCUITPY_MATH),1)
SRC_PATTERNS += math/%
endif
ifeq ($(CIRCUITPY_MEMORYMONITOR),1)
SRC_PATTERNS += memorymonitor/%
endif
ifeq ($(CIRCUITPY_MICROCONTROLLER),1)
SRC_PATTERNS += microcontroller/%
endif
ifeq ($(CIRCUITPY_NEOPIXEL_WRITE),1)
SRC_PATTERNS += neopixel_write/%
endif
ifeq ($(CIRCUITPY_NVM),1)
SRC_PATTERNS += nvm/%
endif
ifeq ($(CIRCUITPY_ONEWIREIO),1)
SRC_PATTERNS += onewireio/%
endif
ifeq ($(CIRCUITPY_OS),1)
SRC_PATTERNS += os/%
endif
ifeq ($(CIRCUITPY_DUALBANK),1)
SRC_PATTERNS += dualbank/%
endif
ifeq ($(CIRCUITPY_PIXELBUF),1)
SRC_PATTERNS += adafruit_pixelbuf/%
endif
ifeq ($(CIRCUITPY_QRIO),1)
SRC_PATTERNS += qrio/%
endif
ifeq ($(CIRCUITPY_RAINBOWIO),1)
SRC_PATTERNS += rainbowio/%
endif
ifeq ($(CIRCUITPY_RGBMATRIX),1)
SRC_PATTERNS += rgbmatrix/%
endif
ifeq ($(CIRCUITPY_PS2IO),1)
SRC_PATTERNS += ps2io/%
endif
ifeq ($(CIRCUITPY_PULSEIO),1)
SRC_PATTERNS += pulseio/%
endif
ifeq ($(CIRCUITPY_PWMIO),1)
SRC_PATTERNS += pwmio/%
endif
ifeq ($(CIRCUITPY_RANDOM),1)
SRC_PATTERNS += random/%
endif
ifeq ($(CIRCUITPY_RP2PIO),1)
SRC_PATTERNS += rp2pio/%
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
ifeq ($(CIRCUITPY_SDCARDIO),1)
SRC_PATTERNS += sdcardio/%
endif
ifeq ($(CIRCUITPY_SDIOIO),1)
SRC_PATTERNS += sdioio/%
endif
ifeq ($(CIRCUITPY_SHARPDISPLAY),1)
SRC_PATTERNS += sharpdisplay/%
endif
ifeq ($(CIRCUITPY_SOCKETPOOL),1)
SRC_PATTERNS += socketpool/%
endif
ifeq ($(CIRCUITPY_SSL),1)
SRC_PATTERNS += ssl/%
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
ifeq ($(CIRCUITPY_SYNTHIO),1)
SRC_PATTERNS += synthio/%
endif
ifeq ($(CIRCUITPY_TERMINALIO),1)
SRC_PATTERNS += terminalio/% fontio/%
endif
ifeq ($(CIRCUITPY_TIME),1)
SRC_PATTERNS += time/%
endif
ifeq ($(CIRCUITPY_TOUCHIO),1)
SRC_PATTERNS += touchio/%
endif
ifeq ($(CIRCUITPY_TRACEBACK),1)
SRC_PATTERNS += traceback/%
endif
ifeq ($(CIRCUITPY_UHEAP),1)
SRC_PATTERNS += uheap/%
endif
ifeq ($(CIRCUITPY_USB_CDC),1)
SRC_PATTERNS += usb_cdc/%
endif
ifeq ($(CIRCUITPY_USB_HID),1)
SRC_PATTERNS += usb_hid/%
endif
ifeq ($(CIRCUITPY_USB_MIDI),1)
SRC_PATTERNS += usb_midi/%
endif
ifeq ($(CIRCUITPY_USB_VENDOR),1)
SRC_PATTERNS += usb_vendor/%
endif
ifeq ($(CIRCUITPY_USTACK),1)
SRC_PATTERNS += ustack/%
endif
ifeq ($(CIRCUITPY_VIDEOCORE),1)
SRC_PATTERNS += videocore/%
endif
ifeq ($(CIRCUITPY_WATCHDOG),1)
SRC_PATTERNS += watchdog/%
endif
ifeq ($(CIRCUITPY_WIFI),1)
SRC_PATTERNS += wifi/%
endif
ifeq ($(CIRCUITPY_PEW),1)
SRC_PATTERNS += _pew/%
endif
ifeq ($(CIRCUITPY_MSGPACK),1)
SRC_PATTERNS += msgpack/%
endif

# All possible sources are listed here, and are filtered by SRC_PATTERNS in SRC_COMMON_HAL
SRC_COMMON_HAL_ALL = \
	_bleio/Adapter.c \
	_bleio/Attribute.c \
	_bleio/Characteristic.c \
	_bleio/CharacteristicBuffer.c \
	_bleio/Connection.c \
	_bleio/Descriptor.c \
	_bleio/PacketBuffer.c \
	_bleio/Service.c \
	_bleio/UUID.c \
	_bleio/__init__.c \
	_pew/PewPew.c \
	_pew/__init__.c \
	alarm/SleepMemory.c \
	alarm/__init__.c \
	alarm/pin/PinAlarm.c \
	alarm/time/TimeAlarm.c \
	alarm/touch/TouchAlarm.c \
	analogio/AnalogIn.c \
	analogio/AnalogOut.c \
	analogio/__init__.c \
	audiobusio/I2SOut.c \
	audiobusio/PDMIn.c \
	audiobusio/__init__.c \
	audioio/AudioOut.c \
	audioio/__init__.c \
	audiopwmio/PWMAudioOut.c \
	audiopwmio/__init__.c \
	board/__init__.c \
	busio/I2C.c \
	busio/SPI.c \
	busio/UART.c \
	busio/__init__.c \
	camera/__init__.c \
	camera/Camera.c \
	canio/CAN.c \
	canio/Listener.c \
	canio/__init__.c \
	countio/Counter.c \
	countio/__init__.c \
	digitalio/DigitalInOut.c \
	digitalio/__init__.c \
	dualbank/__init__.c \
	frequencyio/FrequencyIn.c \
	frequencyio/__init__.c \
	imagecapture/ParallelImageCapture.c \
	imagecapture/__init__.c \
	gnss/__init__.c \
	gnss/GNSS.c \
	gnss/PositionFix.c \
	gnss/SatelliteSystem.c \
	i2cperipheral/I2CPeripheral.c \
	i2cperipheral/__init__.c \
	microcontroller/Pin.c \
	microcontroller/Processor.c \
	microcontroller/__init__.c \
	neopixel_write/__init__.c \
	nvm/ByteArray.c \
	nvm/__init__.c \
	os/__init__.c \
	paralleldisplay/ParallelBus.c \
	ps2io/Ps2.c \
	ps2io/__init__.c \
	pulseio/PulseIn.c \
	pulseio/PulseOut.c \
	pulseio/__init__.c \
	pwmio/PWMOut.c \
	pwmio/__init__.c \
	rgbmatrix/RGBMatrix.c \
	rgbmatrix/__init__.c \
	rotaryio/IncrementalEncoder.c \
	rotaryio/__init__.c \
	rtc/RTC.c \
	rtc/__init__.c \
	sdioio/SDCard.c \
	sdioio/__init__.c \
	socketpool/__init__.c \
	socketpool/SocketPool.c \
	socketpool/Socket.c \
	ssl/__init__.c \
	ssl/SSLContext.c \
	ssl/SSLSocket.c \
	supervisor/Runtime.c \
	supervisor/__init__.c \
	watchdog/WatchDogMode.c \
	watchdog/WatchDogTimer.c \
	watchdog/__init__.c \
	wifi/Monitor.c \
	wifi/Network.c \
	wifi/Radio.c \
	wifi/ScannedNetworks.c \
	wifi/__init__.c \

ifeq ($(CIRCUITPY_BLEIO_HCI),1)
# Helper code for _bleio HCI.
SRC_C += \
	common-hal/_bleio/att.c \
	common-hal/_bleio/hci.c \

endif


SRC_COMMON_HAL = $(filter $(SRC_PATTERNS), $(SRC_COMMON_HAL_ALL))

# These don't have corresponding files in each port but are still located in
# shared-bindings to make it clear what the contents of the modules are.
# All possible sources are listed here, and are filtered by SRC_PATTERNS.
SRC_BINDINGS_ENUMS = \
$(filter $(SRC_PATTERNS), \
	_bleio/Address.c \
	_bleio/Attribute.c \
	_bleio/ScanEntry.c \
	_eve/__init__.c \
	__future__/__init__.c \
	camera/ImageFormat.c \
	canio/Match.c \
	countio/Edge.c \
	digitalio/Direction.c \
	digitalio/DriveMode.c \
	digitalio/Pull.c \
	displayio/Colorspace.c \
	fontio/Glyph.c \
	imagecapture/ParallelImageCapture.c \
	math/__init__.c \
	microcontroller/ResetReason.c \
	microcontroller/RunMode.c \
	msgpack/__init__.c \
	msgpack/ExtType.c \
	paralleldisplay/__init__.c \
	paralleldisplay/ParallelBus.c \
	qrio/PixelPolicy.c \
	qrio/QRInfo.c \
	supervisor/RunReason.c \
	wifi/AuthMode.c \
	wifi/Packet.c \
)

SRC_BINDINGS_ENUMS += \
	util.c

SRC_SHARED_MODULE_ALL = \
	_bleio/Address.c \
	_bleio/Attribute.c \
	_bleio/ScanEntry.c \
	_bleio/ScanResults.c \
	_eve/__init__.c \
	adafruit_pixelbuf/PixelBuf.c \
	adafruit_pixelbuf/__init__.c \
	_stage/Layer.c \
	_stage/Text.c \
	_stage/__init__.c \
	aesio/__init__.c \
	aesio/aes.c \
	atexit/__init__.c \
	audiocore/RawSample.c \
	audiocore/WaveFile.c \
	audiocore/__init__.c \
	audioio/__init__.c \
	audiomixer/Mixer.c \
	audiomixer/MixerVoice.c \
	audiomixer/__init__.c \
	audiomp3/MP3Decoder.c \
	audiomp3/__init__.c \
	audiopwmio/__init__.c \
	bitbangio/I2C.c \
	bitbangio/SPI.c \
	bitbangio/__init__.c \
	bitmaptools/__init__.c \
	bitops/__init__.c \
	board/__init__.c \
	adafruit_bus_device/__init__.c \
	adafruit_bus_device/i2c_device/I2CDevice.c \
	adafruit_bus_device/spi_device/SPIDevice.c \
	canio/Match.c \
	canio/Message.c \
	canio/RemoteTransmissionRequest.c \
	displayio/Bitmap.c \
	displayio/ColorConverter.c \
	displayio/Display.c \
	displayio/EPaperDisplay.c \
	displayio/FourWire.c \
	displayio/Group.c \
	displayio/I2CDisplay.c \
	displayio/OnDiskBitmap.c \
	displayio/Palette.c \
	displayio/Shape.c \
	displayio/TileGrid.c \
	displayio/area.c \
	displayio/__init__.c \
	fontio/BuiltinFont.c \
	fontio/__init__.c \
	framebufferio/FramebufferDisplay.c \
	framebufferio/__init__.c \
	gamepadshift/GamePadShift.c \
	gamepadshift/__init__.c \
	getpass/__init__.c \
	gifio/__init__.c \
	gifio/GifWriter.c \
	imagecapture/ParallelImageCapture.c \
	ipaddress/IPv4Address.c \
	ipaddress/__init__.c \
	is31fl3741/IS31FL3741.c \
	is31fl3741/FrameBuffer.c \
	is31fl3741/__init__.c \
	keypad/__init__.c \
	keypad/Event.c \
	keypad/EventQueue.c \
	keypad/KeyMatrix.c \
	keypad/ShiftRegisterKeys.c \
	keypad/Keys.c \
	memorymonitor/__init__.c \
	memorymonitor/AllocationAlarm.c \
	memorymonitor/AllocationSize.c \
	network/__init__.c \
	msgpack/__init__.c \
	onewireio/__init__.c \
	onewireio/OneWire.c \
	os/__init__.c \
	paralleldisplay/ParallelBus.c \
	qrio/__init__.c \
	qrio/QRDecoder.c \
	rainbowio/__init__.c \
	random/__init__.c \
	rgbmatrix/RGBMatrix.c \
	rgbmatrix/__init__.c \
	rotaryio/IncrementalEncoder.c \
	sdcardio/SDCard.c \
	sdcardio/__init__.c \
	sharpdisplay/SharpMemoryFramebuffer.c \
	sharpdisplay/__init__.c \
	socket/__init__.c \
	storage/__init__.c \
	struct/__init__.c \
	synthio/MidiTrack.c \
	synthio/__init__.c \
	terminalio/Terminal.c \
	terminalio/__init__.c \
	time/__init__.c \
	traceback/__init__.c \
	uheap/__init__.c \
	ustack/__init__.c \
	vectorio/Circle.c \
	vectorio/Polygon.c \
	vectorio/Rectangle.c \
	vectorio/VectorShape.c \
	vectorio/__init__.c \

# All possible sources are listed here, and are filtered by SRC_PATTERNS.
SRC_SHARED_MODULE = $(filter $(SRC_PATTERNS), $(SRC_SHARED_MODULE_ALL))

# Use the native touchio if requested. This flag is set conditionally in, say, mpconfigport.h.
# The presence of common-hal/touchio/* does not imply it's available for all chips in a port,
# so there is an explicit flag. For example, SAMD21 touchio is native, but SAMD51 is not.
ifeq ($(CIRCUITPY_TOUCHIO_USE_NATIVE),1)
SRC_COMMON_HAL_ALL += \
	touchio/TouchIn.c \
	touchio/__init__.c
else
SRC_SHARED_MODULE_ALL += \
	touchio/TouchIn.c \
	touchio/__init__.c
endif

# If supporting _bleio via HCI, make devices/ble_hci/common-hal/_bleio be includable,
# and use C source files in devices/ble_hci/common-hal.
ifeq ($(CIRCUITPY_BLEIO_HCI),1)
INC += -I$(TOP)/devices/ble_hci
DEVICES_MODULES += $(TOP)/devices/ble_hci
endif

ifeq ($(CIRCUITPY_AUDIOMP3),1)
SRC_MOD += $(addprefix lib/mp3/src/, \
	bitstream.c \
	buffers.c \
	dct32.c \
	dequant.c \
	dqchan.c \
	huffman.c \
	hufftabs.c \
	imdct.c \
	mp3dec.c \
	mp3tabs.c \
	polyphase.c \
	scalfact.c \
	stproc.c \
	subband.c \
	trigtabs.c \
)
$(BUILD)/lib/mp3/src/buffers.o: CFLAGS += -include "py/misc.h" -D'MPDEC_ALLOCATOR(x)=m_malloc(x,0)' -D'MPDEC_FREE(x)=m_free(x)'
endif
ifeq ($(CIRCUITPY_RGBMATRIX),1)
SRC_MOD += $(addprefix lib/protomatter/src/, \
	core.c \
)
$(BUILD)/lib/protomatter/src/core.o: CFLAGS += -include "shared-module/rgbmatrix/allocator.h" -DCIRCUITPY -Wno-missing-braces -Wno-missing-prototypes
endif

# All possible sources are listed here, and are filtered by SRC_PATTERNS.
SRC_SHARED_MODULE_INTERNAL = \
$(filter $(SRC_PATTERNS), \
	displayio/display_core.c \
)

SRC_COMMON_HAL_INTERNAL = \
$(filter $(SRC_PATTERNS), \
	_bleio/ \
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
ifeq ($(CIRCUITPY_ULAB),1)
SRC_LIBM += \
$(addprefix lib/,\
	libm/acoshf.c \
	libm/asinhf.c \
	libm/atanhf.c \
	libm/erf_lgamma.c \
	libm/log1pf.c \
	libm/sf_erf.c \
	libm/wf_lgamma.c \
	libm/wf_tgamma.c \
	)
endif
$(patsubst %.c,$(BUILD)/%.o,$(SRC_LIBM)): CFLAGS += -Wno-missing-prototypes
endif

SRC_CIRCUITPY_COMMON = \
	shared/libc/string0.c \
	shared/readline/readline.c \
	lib/oofatfs/ff.c \
	lib/oofatfs/ffunicode.c \
	shared/timeutils/timeutils.c \
	shared/runtime/buffer_helper.c \
	shared/runtime/context_manager_helpers.c \
	shared/runtime/interrupt_char.c \
	shared/runtime/pyexec.c \
	shared/runtime/stdout_helpers.c \
	shared/runtime/sys_stdio_mphal.c

ifeq ($(CIRCUITPY_QRIO),1)
SRC_CIRCUITPY_COMMON += lib/quirc/lib/decode.c lib/quirc/lib/identify.c lib/quirc/lib/quirc.c lib/quirc/lib/version_db.c
$(BUILD)/lib/quirc/lib/%.o: CFLAGS += -Wno-shadow -Wno-sign-compare -include shared-module/qrio/quirc_alloc.h
endif

ifdef LD_TEMPLATE_FILE
# Generate a linker script (.ld file) from a template, for those builds that use it.
GENERATED_LD_FILE = $(BUILD)/$(notdir $(patsubst %.template.ld,%.ld,$(LD_TEMPLATE_FILE)))
#
# ld_defines.pp is generated from ld_defines.c. See py/mkrules.mk.
# Run gen_ld_files.py over ALL *.template.ld files, not just LD_TEMPLATE_FILE,
# because it may include other template files.
$(GENERATED_LD_FILE): $(BUILD)/ld_defines.pp boards/*.template.ld
	$(STEPECHO) "GEN $@"
	$(Q)$(PYTHON) $(TOP)/tools/gen_ld_files.py --defines $< --out_dir $(BUILD) boards/*.template.ld
endif

.PHONY: check-release-needs-clean-build

check-release-needs-clean-build:
	@echo "RELEASE_NEEDS_CLEAN_BUILD = $(RELEASE_NEEDS_CLEAN_BUILD)"
