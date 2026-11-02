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

# Micropython's implementation of <string.h> routines is incompatible with
# "fortify source", enabled by default on gentoo's crossdev arm-none-eabi-gcc
# gcc version 12.3.1 20230526 (Gentoo 12.3.1_p20230526 p2). Unconditionally disable it.
BASE_CFLAGS += -U_FORTIFY_SOURCE

# Set a global CIRCUITPY_DEBUG flag.
# Don't just call it "DEBUG": too many libraries use plain DEBUG.
ifneq ($(DEBUG),)
CFLAGS += -DCIRCUITPY_DEBUG=$(DEBUG)
else
CFLAGS += -DCIRCUITPY_DEBUG=0
endif

CIRCUITPY_LTO ?= 0
CIRCUITPY_LTO_PARTITION ?= balanced
ifeq ($(CIRCUITPY_LTO),1)
CFLAGS += -flto=jobserver -flto-partition=$(CIRCUITPY_LTO_PARTITION) -DCIRCUITPY_LTO=1
else
CFLAGS += -DCIRCUITPY_LTO=0
endif

# Produce an object file for translate.c instead of including it in a header.
# The header version can be optimized on non-LTO builds *if* inlining is allowed
# otherwise, it blows up the binary sizes with tons of translate copies.
CIRCUITPY_TRANSLATE_OBJECT ?= $(CIRCUITPY_LTO)
CFLAGS += -DCIRCUITPY_TRANSLATE_OBJECT=$(CIRCUITPY_TRANSLATE_OBJECT)

###
# Handle frozen modules.

# To use frozen bytecode, put your .py files in a subdirectory (eg frozen/) and
# then invoke make with FROZEN_MPY_DIR=frozen or FROZEN_MPY_DIRS="dir1 dir2"
# (be sure to build from scratch).

ifneq ($(FROZEN_MPY_DIRS),)
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
CFLAGS += -DMICROPY_MODULE_FROZEN_MPY
endif

###
# Select which builtin modules to compile and include.
# Keep alphabetical.

ifeq ($(CIRCUITPY_AESIO),1)
SRC_PATTERNS += aesio/%
endif
ifeq ($(CIRCUITPY_AIRLIFT),1)
SRC_PATTERNS += _airlift/%
endif
ifeq ($(CIRCUITPY_ALARM),1)
SRC_PATTERNS += alarm/__init__.c alarm/SleepMemory.c alarm/pin/% alarm/time/%
endif
ifeq ($(CIRCUITPY_ALARM_TOUCH),1)
SRC_PATTERNS += alarm/touch/%
endif
ifeq ($(CIRCUITPY_ANALOGBUFIO),1)
SRC_PATTERNS += analogbufio/%
endif
ifeq ($(CIRCUITPY_ANALOGIO),1)
SRC_PATTERNS += analogio/%
endif
ifeq ($(CIRCUITPY_ASYNC_SPI),1)
SRC_PATTERNS += async_spi/%
endif
ifeq ($(CIRCUITPY_ATEXIT),1)
SRC_PATTERNS += atexit/%
endif
ifeq ($(CIRCUITPY_AUDIOBUSIO),1)
SRC_PATTERNS += audiobusio/%
endif
ifeq ($(CIRCUITPY_AUDIOI2SIN),1)
SRC_PATTERNS += audioi2sin/%
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
ifeq ($(CIRCUITPY_AUDIODELAYS),1)
SRC_PATTERNS += audiodelays/%
endif
ifeq ($(CIRCUITPY_AUDIOFILTERS),1)
SRC_PATTERNS += audiofilters/%
endif
ifeq ($(CIRCUITPY_AUDIOFREEVERB),1)
SRC_PATTERNS += audiofreeverb/%
endif
ifeq ($(CIRCUITPY_AUDIOMIXER),1)
SRC_PATTERNS += audiomixer/%
endif
ifeq ($(CIRCUITPY_AUDIOMP3),1)
SRC_PATTERNS += audiomp3/%
endif
ifeq ($(CIRCUITPY_AUDIOSPEED),1)
SRC_PATTERNS += audiospeed/%
endif
ifeq ($(CIRCUITPY_AUDIOTAP),1)
SRC_PATTERNS += audiotap/%
endif
ifeq ($(CIRCUITPY_AUDIOFILEWRITER),1)
SRC_PATTERNS += audiofilewriter/%
endif
ifeq ($(CIRCUITPY_AURORA_EPAPER),1)
SRC_PATTERNS += aurora_epaper/%
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
ifeq ($(CIRCUITPY_BITMAPFILTER),1)
SRC_PATTERNS += bitmapfilter/%
endif
ifeq ($(CIRCUITPY_BITOPS),1)
SRC_PATTERNS += bitops/%
endif
ifeq ($(CIRCUITPY_BLEIO_NATIVE),1)
SRC_PATTERNS += _bleio/%
endif
ifeq ($(CIRCUITPY_BOARD),1)
SRC_PATTERNS += board/%
endif
ifeq ($(CIRCUITPY_BUSDEVICE),1)
SRC_PATTERNS += adafruit_bus_device/%
endif
ifeq ($(CIRCUITPY_BUSDISPLAY),1)
SRC_PATTERNS += busdisplay/%
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
ifeq ($(CIRCUITPY_CODEOP),1)
SRC_PATTERNS += codeop/%
endif
ifeq ($(CIRCUITPY_COUNTIO),1)
SRC_PATTERNS += countio/%
endif
ifeq ($(CIRCUITPY_CYW43),1)
SRC_PATTERNS += cyw43/%
endif
ifeq ($(CIRCUITPY_DIGITALIO),1)
SRC_PATTERNS += digitalio/%
endif
ifeq ($(CIRCUITPY_DISPLAYIO),1)
SRC_PATTERNS += displayio/%
endif
ifeq ($(CIRCUITPY_DOTCLOCKFRAMEBUFFER),1)
SRC_PATTERNS += dotclockframebuffer/%
endif
ifeq ($(CIRCUITPY_DUALBANK),1)
SRC_PATTERNS += dualbank/%
endif
ifeq ($(CIRCUITPY__EVE),1)
SRC_PATTERNS += _eve/%
endif
ifeq ($(CIRCUITPY_EMMCIO),1)
SRC_PATTERNS += emmcio/%
endif
ifeq ($(CIRCUITPY_EPAPERDISPLAY),1)
SRC_PATTERNS += epaperdisplay/%
endif
ifeq ($(CIRCUITPY_ESPCAMERA),1)
SRC_PATTERNS += espcamera/%
endif
ifeq ($(CIRCUITPY_ESPIDF),1)
SRC_PATTERNS += espidf/%
endif
ifeq ($(CIRCUITPY_ESPNOW),1)
SRC_PATTERNS += espnow/%
endif
ifeq ($(CIRCUITPY_ESPULP),1)
SRC_PATTERNS += espulp/%
endif
ifeq ($(CIRCUITPY_MII),1)
SRC_PATTERNS += mii/%
endif
ifeq ($(CIRCUITPY_FLOPPYIO),1)
SRC_PATTERNS += floppyio/%
endif
ifeq ($(CIRCUITPY_FOURWIRE),1)
SRC_PATTERNS += fourwire/%
endif
ifeq ($(CIRCUITPY_FRAMEBUFFERIO),1)
SRC_PATTERNS += framebufferio/%
endif
ifeq ($(CIRCUITPY_FREQUENCYIO),1)
SRC_PATTERNS += frequencyio/%
endif
ifeq ($(CIRCUITPY_FUTURE),1)
SRC_PATTERNS += __future__/%
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
ifeq ($(CIRCUITPY_HARDWAREKEY),1)
SRC_PATTERNS += hardwarekey/%
endif
ifeq ($(CIRCUITPY_HASHLIB),1)
SRC_PATTERNS += hashlib/%
endif
ifeq ($(CIRCUITPY_HMAC),1)
SRC_PATTERNS += hmac/%
endif
ifeq ($(CIRCUITPY_I2CDISPLAYBUS),1)
SRC_PATTERNS += i2cdisplaybus/%
endif
ifeq ($(CIRCUITPY_I2CTARGET),1)
SRC_PATTERNS += i2ctarget/%
endif
ifeq ($(CIRCUITPY_I2CIOEXPANDER),1)
SRC_PATTERNS += i2cioexpander/%
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
ifeq ($(CIRCUITPY_JPEGIO),1)
SRC_PATTERNS += jpegio/%
endif
ifeq ($(CIRCUITPY_KEYPAD),1)
SRC_PATTERNS += keypad/%
endif
ifeq ($(CIRCUITPY_KEYPAD_DEMUX),1)
SRC_PATTERNS += keypad_demux/%
endif
ifeq ($(CIRCUITPY_LOCALE),1)
SRC_PATTERNS += locale/%
endif
ifeq ($(CIRCUITPY_MATH),1)
SRC_PATTERNS += math/%
endif
ifeq ($(CIRCUITPY_MAX3421E),1)
SRC_PATTERNS += max3421e/%
endif
ifeq ($(CIRCUITPY_MCP4822),1)
SRC_PATTERNS += mcp4822/%
endif
ifeq ($(CIRCUITPY_MDNS),1)
SRC_PATTERNS += mdns/%
endif
ifeq ($(CIRCUITPY_MEMORYMAP),1)
SRC_PATTERNS += memorymap/%
endif
ifeq ($(CIRCUITPY_MEMORYMONITOR),1)
SRC_PATTERNS += memorymonitor/%
endif
ifeq ($(CIRCUITPY_MICROCONTROLLER),1)
SRC_PATTERNS += microcontroller/%
endif
ifeq ($(CIRCUITPY_MIPICSI),1)
SRC_PATTERNS += mipicsi/%
endif
ifeq ($(CIRCUITPY_MIPIDSI),1)
SRC_PATTERNS += mipidsi/%
endif
ifeq ($(CIRCUITPY_MSGPACK),1)
SRC_PATTERNS += msgpack/%
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
ifeq ($(CIRCUITPY_PARALLELDISPLAYBUS),1)
SRC_PATTERNS += paralleldisplaybus/%
endif
ifeq ($(CIRCUITPY_PEW),1)
SRC_PATTERNS += _pew/%
endif
ifeq ($(CIRCUITPY_PIXELBUF),1)
SRC_PATTERNS += adafruit_pixelbuf/%
endif
ifeq ($(CIRCUITPY_PIXELMAP),1)
SRC_PATTERNS += _pixelmap/%
endif
ifeq ($(CIRCUITPY_PICODVI),1)
SRC_PATTERNS += picodvi/%
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
ifeq ($(CIRCUITPY_QRIO),1)
SRC_PATTERNS += qrio/%
endif
ifeq ($(CIRCUITPY_RAINBOWIO),1)
SRC_PATTERNS += rainbowio/%
endif
ifeq ($(CIRCUITPY_RANDOM),1)
SRC_PATTERNS += random/%
endif
ifeq ($(CIRCUITPY_RCLCPY),1)
SRC_PATTERNS += rclcpy/%
endif
ifeq ($(CIRCUITPY_RGBMATRIX),1)
SRC_PATTERNS += rgbmatrix/%
endif
ifeq ($(CIRCUITPY_QSPIBUS),1)
SRC_PATTERNS += qspibus/%
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
ifeq ($(CIRCUITPY_SPITARGET),1)
SRC_PATTERNS += spitarget/%
endif
ifeq ($(CIRCUITPY_SSL),1)
SRC_PATTERNS += ssl/%
endif
ifeq ($(CIRCUITPY_STAGE),1)
SRC_PATTERNS += _stage/%
endif
ifeq ($(CIRCUITPY_PICOGAME),1)
SRC_PATTERNS += picogame/%
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
ifeq ($(CIRCUITPY_FONTIO),1)
SRC_PATTERNS += fontio/%
endif
ifeq ($(CIRCUITPY_LVFONTIO),1)
SRC_PATTERNS += lvfontio/%
endif
ifeq ($(CIRCUITPY_TILEPALETTEMAPPER),1)
SRC_PATTERNS += tilepalettemapper/%
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
ifeq ($(CIRCUITPY_PYUSB),1)
SRC_PATTERNS += usb/%
endif
ifeq ($(CIRCUITPY_USB_CDC),1)
SRC_PATTERNS += usb_cdc/%
endif
ifeq ($(CIRCUITPY_USB_HID),1)
SRC_PATTERNS += usb_hid/%
endif
ifeq ($(CIRCUITPY_USB_VIDEO),1)
SRC_PATTERNS += usb_video/%
endif
ifeq ($(CIRCUITPY_USB_AUDIO),1)
SRC_PATTERNS += usb_audio/%
endif
ifeq ($(CIRCUITPY_USB_HOST),1)
SRC_PATTERNS += usb_host/%
endif
ifeq ($(CIRCUITPY_USB_HOST_AUDIO),1)
SRC_PATTERNS += usb_host_audio/%
endif
ifeq ($(CIRCUITPY_USB_HOST_BULK),1)
SRC_PATTERNS += usb_host_bulk/%
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
ifeq ($(CIRCUITPY_VECTORIO),1)
SRC_PATTERNS += vectorio/%
endif
ifeq ($(CIRCUITPY_VIDEOCORE),1)
SRC_PATTERNS += videocore/%
endif
ifeq ($(CIRCUITPY_WARNINGS),1)
SRC_PATTERNS += warnings/%
endif
ifeq ($(CIRCUITPY_WATCHDOG),1)
SRC_PATTERNS += watchdog/%
endif
ifeq ($(CIRCUITPY_WIFI),1)
SRC_PATTERNS += wifi/%
endif
ifeq ($(CIRCUITPY_WIZNET),1)
SRC_PATTERNS += wiznet/%
endif
ifeq ($(CIRCUITPY_ZLIB),1)
SRC_PATTERNS += zlib/%
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
	analogbufio/BufferedIn.c \
	analogbufio/__init__.c \
	analogio/AnalogIn.c \
	analogio/AnalogOut.c \
	analogio/__init__.c \
	async_spi/SPI.c \
	async_spi/__init__.c \
	audiobusio/I2SOut.c \
	audiobusio/PDMIn.c \
	audiobusio/__init__.c \
	audioi2sin/I2SIn.c \
	audioi2sin/__init__.c \
	audioio/AudioOut.c \
	audioio/__init__.c \
	audiopwmio/PWMAudioOut.c \
	audiopwmio/__init__.c \
	board/__init__.c \
	busio/I2C.c \
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
	dotclockframebuffer/DotClockFramebuffer.c \
	dotclockframebuffer/__init__.c \
	dualbank/__init__.c \
	emmcio/EMMC.c \
	emmcio/__init__.c \
	mii/MDIO.c \
	mii/Ethernet.c \
	mii/RMII.c \
	mii/__init__.c \
	floppyio/__init__.c \
	frequencyio/FrequencyIn.c \
	frequencyio/__init__.c \
	imagecapture/ParallelImageCapture.c \
	imagecapture/__init__.c \
	gnss/__init__.c \
	gnss/GNSS.c \
	gnss/PositionFix.c \
	gnss/SatelliteSystem.c \
	i2ctarget/I2CTarget.c \
	i2ctarget/__init__.c \
	max3421e/Max3421E.c \
	mcp4822/__init__.c \
	mcp4822/MCP4822.c \
	memorymap/__init__.c \
	memorymap/AddressRange.c \
	microcontroller/__init__.c \
	microcontroller/Pin.c \
	microcontroller/Processor.c \
	mdns/__init__.c \
	mdns/Server.c \
	mdns/RemoteService.c \
	mipicsi/__init__.c \
	mipicsi/Camera.c \
	mipidsi/Bus.c \
	mipidsi/Display.c \
	mipidsi/__init__.c \
	neopixel_write/__init__.c \
	nvm/ByteArray.c \
	nvm/__init__.c \
	os/__init__.c \
	paralleldisplaybus/ParallelBus.c \
	ps2io/Ps2.c \
	ps2io/__init__.c \
	pulseio/PulseIn.c \
	pulseio/PulseOut.c \
	pulseio/__init__.c \
	pwmio/PWMOut.c \
	pwmio/__init__.c \
	qspibus/QSPIBus.c \
	qspibus/__init__.c \
	rclcpy/__init__.c \
	rclcpy/Node.c \
	rclcpy/Publisher.c \
	rgbmatrix/RGBMatrix.c \
	rgbmatrix/__init__.c \
	rotaryio/IncrementalEncoder.c \
	rotaryio/__init__.c \
	rtc/RTC.c \
	rtc/__init__.c \
	sdioio/SDCard.c \
	sdioio/__init__.c \
	hardwarekey/HardwareKey.c \
	hardwarekey/__init__.c \
	socketpool/__init__.c \
	socketpool/SocketPool.c \
	socketpool/Socket.c \
	spitarget/SPITarget.c \
	spitarget/__init__.c \
	usb_host/__init__.c \
	usb_host/Port.c \
	usb_host_bulk/__init__.c \
	usb_host_bulk/InStream.c \
	watchdog/WatchDogMode.c \
	watchdog/WatchDogTimer.c \
	watchdog/__init__.c \
	wifi/Monitor.c \
	wifi/Network.c \
	wifi/Radio.c \
	wifi/ScannedNetworks.c \
	wifi/__init__.c \

SRC_COMMON_HAL = $(filter $(SRC_PATTERNS), $(SRC_COMMON_HAL_ALL))

ifeq ($(CIRCUITPY_BLEIO_HCI),1)
# HCI device-specific HAL and helper sources.
SRC_DEVICES_HAL += \
	_bleio/att.c \
	_bleio/hci.c \
    _bleio/Adapter.c \
	_bleio/Attribute.c \
	_bleio/Characteristic.c \
	_bleio/CharacteristicBuffer.c \
	_bleio/Connection.c \
	_bleio/Descriptor.c \
	_bleio/PacketBuffer.c \
	_bleio/Service.c \
	_bleio/UUID.c \
	_bleio/__init__.c
# HCI device-specific bindings.
SRC_DEVICES_BINDINGS += \
	supervisor/bluetooth.c
endif

# These don't have corresponding files in each port but are still located in
# shared-bindings to make it clear what the contents of the modules are.
# All possible sources are listed here, and are filtered by SRC_PATTERNS.
SRC_BINDINGS_ENUMS = \
$(filter $(SRC_PATTERNS), \
	picogame/Bitmap.c \
	picogame/Sprite.c \
	picogame/StripDraw.c \
	picogame/Triangles.c \
	picogame/Framebuffer.c \
	_bleio/Address.c \
	_bleio/Attribute.c \
	_bleio/ScanEntry.c \
	_eve/__init__.c \
	__future__/__init__.c \
	camera/ImageFormat.c \
	canio/Match.c \
	codeop/__init__.c \
	countio/Edge.c \
	digitalio/DigitalInOutProtocol.c \
	digitalio/Direction.c \
	digitalio/DriveMode.c \
	digitalio/Pull.c \
	displayio/Colorspace.c \
	fontio/Glyph.c \
	imagecapture/ParallelImageCapture.c \
	locale/__init__.c \
	math/__init__.c \
	mipicsi/BayerOrder.c \
	mipicsi/PixelFormat.c \
	microcontroller/ResetReason.c \
	microcontroller/RunMode.c \
	msgpack/__init__.c \
	msgpack/ExtType.c \
	paralleldisplaybus/__init__.c \
	qrio/PixelPolicy.c \
	qrio/QRInfo.c \
	rgbmatrix/RowAddressMode.c \
	supervisor/RunReason.c \
	supervisor/Runtime.c \
	supervisor/StatusBar.c \
	wifi/AuthMode.c \
	wifi/Packet.c \
	wifi/PowerManagement.c \
)

ifeq ($(CIRCUITPY_BLEIO_HCI),1)
# Common _bleio bindings used by HCI.
SRC_BINDINGS_ENUMS += \
	_bleio/Address.c \
	_bleio/Adapter.c \
	_bleio/Attribute.c \
	_bleio/Characteristic.c \
	_bleio/CharacteristicBuffer.c \
	_bleio/Connection.c \
	_bleio/Descriptor.c \
	_bleio/PacketBuffer.c \
	_bleio/ScanEntry.c \
	_bleio/Service.c \
	_bleio/UUID.c \
	_bleio/__init__.c
endif

ifeq ($(CIRCUITPY_SAFEMODE_PY),1)
SRC_BINDINGS_ENUMS += \
	supervisor/SafeModeReason.c
endif

SRC_BINDINGS_ENUMS += \
	util.c

SRC_SHARED_MODULE_ALL = \
	_airlift/__init__.c \
	_bleio/Address.c \
	_bleio/Attribute.c \
	_bleio/ScanEntry.c \
	_bleio/ScanResults.c \
	_eve/__init__.c \
	adafruit_pixelbuf/PixelBuf.c \
	adafruit_pixelbuf/__init__.c \
	_pixelmap/PixelMap.c \
	_pixelmap/__init__.c \
	_stage/Layer.c \
	_stage/Text.c \
	_stage/__init__.c \
	picogame/__init__.c \
	picogame/Scene.c \
	picogame/Tilemap.c \
	picogame/Particles.c \
	picogame/Canvas.c \
	aesio/__init__.c \
	aesio/aes.c \
	atexit/__init__.c \
	audiocore/RawSample.c \
	audiocore/WaveFile.c \
	audiocore/__init__.c \
	audiospeed/Resampler.c \
	audiospeed/SpeedChanger.c \
	audiospeed/__init__.c \
	audiotap/Tap.c \
	audiotap/__init__.c \
	audiodelays/Echo.c \
	audiodelays/Chorus.c \
	audiodelays/Flanger.c \
	audiodelays/PitchShift.c \
	audiodelays/GranularPitchShift.c \
	audiodelays/MultiTapDelay.c \
	audiodelays/__init__.c \
	audiofilters/Distortion.c \
	audiofilters/Filter.c \
	audiofilters/Phaser.c \
	audiofilters/__init__.c \
	audiofreeverb/__init__.c \
	audiofreeverb/Freeverb.c \
	audiofilewriter/AudioFileWriter.c \
	audiofilewriter/__init__.c \
	audioio/__init__.c \
	audiomixer/Mixer.c \
	audiomixer/MixerVoice.c \
	audiomixer/__init__.c \
	audiomp3/MP3Decoder.c \
	audiomp3/__init__.c \
	audiopwmio/__init__.c \
	aurora_epaper/aurora_framebuffer.c \
	aurora_epaper/__init__.c \
	bitbangio/I2C.c \
	bitbangio/SPI.c \
	bitbangio/__init__.c \
	bitmaptools/__init__.c \
	bitmapfilter/__init__.c \
	bitops/__init__.c \
	board/__init__.c \
	adafruit_bus_device/__init__.c \
	adafruit_bus_device/i2c_device/I2CDevice.c \
	adafruit_bus_device/spi_device/SPIDevice.c \
	busdisplay/__init__.c \
	busdisplay/BusDisplay.c \
	canio/Match.c \
	canio/Message.c \
	canio/RemoteTransmissionRequest.c \
	displayio/Bitmap.c \
	displayio/ColorConverter.c \
	displayio/Group.c \
	displayio/OnDiskBitmap.c \
	displayio/Palette.c \
	displayio/TileGrid.c \
	displayio/area.c \
	displayio/__init__.c \
	dotclockframebuffer/__init__.c \
	emmcio/__init__.c \
	epaperdisplay/__init__.c \
	epaperdisplay/EPaperDisplay.c \
	i2cioexpander/IOExpander.c \
	i2cioexpander/IOPin.c \
	i2cioexpander/__init__.c \
	floppyio/__init__.c \
	fontio/BuiltinFont.c \
	fontio/__init__.c \
	lvfontio/OnDiskFont.c\
	lvfontio/__init__.c \
	fourwire/__init__.c \
	fourwire/FourWire.c \
	framebufferio/FramebufferDisplay.c \
	framebufferio/__init__.c \
	getpass/__init__.c \
	gifio/__init__.c \
	gifio/GifWriter.c \
	gifio/OnDiskGif.c \
	i2cdisplaybus/__init__.c \
	i2cdisplaybus/I2CDisplayBus.c \
	imagecapture/ParallelImageCapture.c \
	ipaddress/IPv4Address.c \
	ipaddress/__init__.c \
	is31fl3741/IS31FL3741.c \
	is31fl3741/FrameBuffer.c \
	is31fl3741/__init__.c \
	jpegio/__init__.c \
	jpegio/JpegDecoder.c \
	keypad/__init__.c \
	keypad/Event.c \
	keypad/EventQueue.c \
	keypad/KeyMatrix.c \
	keypad/ShiftRegisterKeys.c \
	keypad/Keys.c \
	max3421e/__init__.c \
	max3421e/Max3421E.c \
	memorymonitor/__init__.c \
	memorymonitor/AllocationAlarm.c \
	memorymonitor/AllocationSize.c \
	network/__init__.c \
	msgpack/__init__.c \
	onewireio/__init__.c \
	onewireio/OneWire.c \
	os/__init__.c \
	paralleldisplaybus/ParallelBus.c \
	qrio/__init__.c \
	qrio/QRDecoder.c \
	rainbowio/__init__.c \
	random/__init__.c \
	rgbmatrix/RGBMatrix.c \
	rgbmatrix/__init__.c \
	rotaryio/IncrementalEncoder.c \
	sdcardio/SDCard.c \
	sdcardio/__init__.c \
	hardwarekey/HardwareKey.c \
	sharpdisplay/SharpMemoryFramebuffer.c \
	sharpdisplay/__init__.c \
	socket/__init__.c \
	storage/__init__.c \
	struct/__init__.c \
	supervisor/__init__.c \
	supervisor/StatusBar.c \
	synthio/Biquad.c \
	synthio/LFO.c \
	synthio/Math.c \
	synthio/MidiTrack.c \
	synthio/Note.c \
	synthio/Synthesizer.c \
	synthio/__init__.c \
	terminalio/Terminal.c \
	terminalio/__init__.c \
	tilepalettemapper/__init__.c \
	tilepalettemapper/TilePaletteMapper.c \
	time/__init__.c \
	traceback/__init__.c \
	uheap/__init__.c \
	usb/__init__.c \
	usb/core/__init__.c \
	usb/core/Device.c \
	usb/util/__init__.c \
	usb_host_audio/USBIn.c \
	usb_host_audio/__init__.c \
	ustack/__init__.c \
	vectorio/Circle.c \
	vectorio/Polygon.c \
	vectorio/Rectangle.c \
	vectorio/VectorShape.c \
	vectorio/__init__.c \
	videocore/VideoBuffer.c \
	videocore/__init__.c \
	warnings/__init__.c \
	watchdog/__init__.c \
	zlib/__init__.c \

# All possible sources are listed here, and are filtered by SRC_PATTERNS.
SRC_SHARED_MODULE = $(filter $(SRC_PATTERNS), $(SRC_SHARED_MODULE_ALL))

SRC_COMMON_HAL_EXPANDED = $(addprefix shared-bindings/, $(SRC_COMMON_HAL)) \
                          $(addprefix shared-bindings/, $(SRC_BINDINGS_ENUMS)) \
                          $(addprefix common-hal/, $(SRC_COMMON_HAL)) \
						  $(addprefix devices/ble_hci/common-hal/, $(SRC_DEVICES_HAL)) \
						  $(addprefix devices/ble_hci/, $(SRC_DEVICES_BINDINGS))

SRC_SHARED_MODULE_EXPANDED = $(addprefix shared-bindings/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE_INTERNAL))

# There may be duplicates between SRC_COMMON_HAL_EXPANDED and SRC_SHARED_MODULE_EXPANDED,
# because a few modules have files both in common-hal/ and shared-module/.
# Doing a $(sort ...) removes duplicates as part of sorting.
SRC_COMMON_HAL_SHARED_MODULE_EXPANDED = $(sort $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED))

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

# The strip-rendering Display backend sends through the display bus; nothing port-specific.
ifeq ($(CIRCUITPY_PICOGAME_FAST_DISPLAY),1)
SRC_SHARED_MODULE_ALL += picogame/Display.c
endif

# On a port with async SPI, busio.SPI is shared code on top of common-hal/async_spi.
ifeq ($(CIRCUITPY_ASYNC_SPI),1)
SRC_SHARED_MODULE_ALL += \
	busio/SPI.c
else
SRC_COMMON_HAL_ALL += \
	busio/SPI.c
endif

ifeq ($(CIRCUITPY_SSL_MBEDTLS),0)
SRC_COMMON_HAL_ALL += \
	ssl/__init__.c \
	ssl/SSLContext.c \
	ssl/SSLSocket.c
else
SRC_SHARED_MODULE_ALL += \
	ssl/__init__.c \
	ssl/SSLContext.c \
	ssl/SSLSocket.c
endif

ifeq ($(CIRCUITPY_KEYPAD_DEMUX),1)
SRC_SHARED_MODULE_ALL += \
	keypad_demux/__init__.c \
	keypad_demux/DemuxKeyMatrix.c
endif

ifeq ($(CIRCUITPY_BLEIO_HCI),1)
# Add HCI device-specific includes to search path.
INC += -I$(TOP)/devices/ble_hci
# Add HCI shared modules to build.
SRC_SHARED_MODULE += \
	_bleio/Address.c \
	_bleio/Attribute.c \
	_bleio/ScanEntry.c \
	_bleio/ScanResults.c
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
$(BUILD)/lib/mp3/src/buffers.o: CFLAGS += -include "shared-module/audiomp3/__init__.h" -D'MPDEC_ALLOCATOR(x)=mp3_alloc(x)' -D'MPDEC_FREE(x)=mp3_free(x)' -fwrapv
ifeq ($(CIRCUITPY_AUDIOMP3_USE_PORT_ALLOCATOR),1)
SRC_COMMON_HAL_ALL += \
	audiomp3/__init__.c
endif
endif

ifeq ($(CIRCUITPY_GIFIO),1)
SRC_MOD += $(addprefix lib/AnimatedGIF/, \
	gif.c \
)
$(BUILD)/lib/AnimatedGIF/gif.o: CFLAGS += -DCIRCUITPY -Wno-unused-but-set-variable
endif

ifeq ($(CIRCUITPY_JPEGIO),1)
SRC_MOD += lib/tjpgd/src/tjpgd.c
$(BUILD)/lib/tjpgd/src/tjpgd.o: CFLAGS += -Wno-shadow -Wno-cast-align
endif

ifeq ($(CIRCUITPY_HASHLIB_MBEDTLS_ONLY),1)
# mbedtls 4.x moved the crypto implementations out of lib/mbedtls/library into the
# tf-psa-crypto submodule and made the legacy mbedtls_sha256_*() headers private, so
# hashlib goes through the PSA hash API. That needs the PSA core, whose driver dispatch
# layer is generated rather than shipped -- see the same rule in ports/raspberrypi/Makefile.
SRC_MOD += \
	$(addprefix lib/mbedtls/tf-psa-crypto/core/, \
		psa_crypto.c \
		psa_crypto_client.c \
		psa_crypto_slot_management.c \
		psa_util.c \
	) \
	$(addprefix lib/mbedtls/tf-psa-crypto/drivers/builtin/src/, \
		psa_crypto_hash.c \
		psa_util_internal.c \
		sha1.c \
		sha256.c \
	) \
	$(addprefix lib/mbedtls/tf-psa-crypto/platform/, \
		platform.c \
		platform_util.c \
	) \
	lib/mbedtls/tf-psa-crypto/utilities/constant_time.c \
	lib/mbedtls_config/hashlib_psa_port.c

MBEDTLS_HASHLIB_GEN := $(BUILD)/tf-psa-crypto
MBEDTLS_HASHLIB_WRAPPERS_H := $(MBEDTLS_HASHLIB_GEN)/psa_crypto_driver_wrappers.h
MBEDTLS_HASHLIB_WRAPPERS_C := $(MBEDTLS_HASHLIB_GEN)/psa_crypto_driver_wrappers_no_static.c
SRC_MOD += $(MBEDTLS_HASHLIB_WRAPPERS_C:$(BUILD)/%=%)

# The script writes both files at once; naming both as targets would run it twice.
$(MBEDTLS_HASHLIB_WRAPPERS_C): $(MBEDTLS_HASHLIB_WRAPPERS_H)
	@true

$(MBEDTLS_HASHLIB_WRAPPERS_H): $(TOP)/lib/mbedtls/tf-psa-crypto/scripts/generate_driver_wrappers.py
	$(STEPECHO) "GEN $@"
	$(Q)mkdir -p $(MBEDTLS_HASHLIB_GEN)
	$(Q)PYTHONPATH=$(TOP)/lib/mbedtls/framework/scripts $(PYTHON) $< $(MBEDTLS_HASHLIB_GEN)

# psa_crypto.c and friends #include the generated header.
$(addprefix $(BUILD)/, $(SRC_MOD:.c=.o)): $(MBEDTLS_HASHLIB_WRAPPERS_H)

CFLAGS += \
	  -I$(MBEDTLS_HASHLIB_GEN) \
	  -isystem $(TOP)/lib/mbedtls/tf-psa-crypto/include \
	  -isystem $(TOP)/lib/mbedtls/tf-psa-crypto/drivers/builtin/include \
	  -isystem $(TOP)/lib/mbedtls/tf-psa-crypto/drivers/builtin/src \
	  -isystem $(TOP)/lib/mbedtls/tf-psa-crypto/core \
	  -isystem $(TOP)/lib/mbedtls/tf-psa-crypto/dispatch \
	  -isystem $(TOP)/lib/mbedtls/tf-psa-crypto/extras \
	  -isystem $(TOP)/lib/mbedtls/tf-psa-crypto/platform \
	  -isystem $(TOP)/lib/mbedtls/tf-psa-crypto/utilities \
	  -DTF_PSA_CRYPTO_CONFIG_FILE='"$(TOP)/lib/mbedtls_config/tf_psa_crypto_config_hashlib.h"' \

endif

ifeq ($(CIRCUITPY_HASHLIB_MBEDTLS),1)
SRC_SHARED_MODULE_ALL += \
	hashlib/Hash.c \
	hashlib/__init__.c
else
SRC_COMMON_HAL_ALL += \
	hashlib/Hash.c \
	hashlib/__init__.c
endif

ifeq ($(CIRCUITPY_HMAC),1)
SRC_SHARED_MODULE_ALL += \
	hmac/HMAC.c \
	hmac/__init__.c
endif

ifeq ($(CIRCUITPY_RGBMATRIX),1)
SRC_MOD += $(addprefix lib/protomatter/src/, \
	core.c \
)
$(BUILD)/lib/protomatter/src/core.o: CFLAGS += -include "shared-module/rgbmatrix/allocator.h" -DCIRCUITPY -Wno-missing-braces -Wno-missing-prototypes
endif

ifeq ($(CIRCUITPY_ZLIB),1)
SRC_MOD += $(addprefix lib/uzlib/, \
	tinflate.c \
	tinfzlib.c \
	tinfgzip.c \
	adler32.c \
	crc32.c \
)
$(BUILD)/lib/uzlib/tinflate.o: CFLAGS += -Wno-missing-braces -Wno-missing-prototypes
endif

# All possible sources are listed here, and are filtered by SRC_PATTERNS.
SRC_SHARED_MODULE_INTERNAL = \
$(filter $(SRC_PATTERNS), \
	_airlift/network.c \
	_airlift/socketpool.c \
	_airlift/transport.c \
	displayio/bus_core.c \
	displayio/display_core.c \
	usb/utf16le.c \
)

ifeq ($(INTERNAL_LIBM),1)
SRC_LIBM = \
$(addprefix lib/,\
	libm/math.c \
	libm/roundf.c \
	libm/fabsf.c \
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

# Sources used in all ports except unix.
SRC_CIRCUITPY_COMMON = \
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

ifeq ($(CIRCUITPY_LIBC_STRING0),1)
SRC_CIRCUITPY_COMMON += shared/libc/string0.c
endif

ifeq ($(CIRCUITPY_QRIO),1)
SRC_CIRCUITPY_COMMON += lib/quirc/lib/decode.c lib/quirc/lib/identify.c lib/quirc/lib/quirc.c lib/quirc/lib/version_db.c
$(BUILD)/lib/quirc/lib/%.o: CFLAGS += -Wno-type-limits -Wno-shadow -Wno-sign-compare -include shared-module/qrio/quirc_alloc.h
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

# Ignore these errors
$(BUILD)/lib/libm/kf_rem_pio2.o: CFLAGS += -Wno-maybe-uninitialized

# Fetch only submodules needed for this particular port.
.PHONY: fetch-port-submodules
fetch-port-submodules:
	$(PYTHON) $(TOP)/tools/ci_fetch_deps.py $(shell basename $(CURDIR))

# Fetch only submodules needed for this particular board.
.PHONY: fetch-board-submodules
fetch-board-submodules:
	$(PYTHON) $(TOP)/tools/ci_fetch_deps.py $(BOARD)

.PHONY: invalid-board
invalid-board:
	$(Q)if [ -z "$(BOARD)" ] ; then echo "ERROR: No BOARD specified" ; else echo "ERROR: Invalid BOARD $(BOARD) specified"; fi && \
	echo "Valid boards:" && \
	printf '%s\n' $(ALL_BOARDS_IN_PORT) | column -xc $$(tput cols || echo 80) 1>&2 && \
	false

# Print out the value of a make variable.
# https://stackoverflow.com/questions/16467718/how-to-print-out-a-variable-in-makefile
print-%:
	@echo "$* = "$($*)
