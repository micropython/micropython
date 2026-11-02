USB_VID = 0x239A
USB_PID = 0x816C
USB_PRODUCT = "Fruit Jam"
USB_MANUFACTURER = "Adafruit"

CHIP_VARIANT = RP2350
CHIP_PACKAGE = B
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "W25Q128JVxQ"

CIRCUITPY_SDIOIO = 1

CIRCUITPY_PICOGAME = 1
CIRCUITPY_PICOGAME_FRAMEBUFFER = 1

# CIRCUITPY_DISPLAY_FONT = $(TOP)/tools/fonts/unifont-16.0.02-all.bdf
# CIRCUITPY_FONT_EXTRA_CHARACTERS = "🖮🖱️"

# USB video (UVC) cameras have configuration descriptors of several KB.
# TinyUSB skips enumerating any device whose descriptor doesn't fit here.
CFLAGS += -DCFG_TUH_ENUMERATION_BUFSIZE=4096
# Enable host isochronous IN support (UVC video, UAC audio). The packet ring
# is only allocated when CIRCUITPY_USB_HOST_ISO_BUFFER_SIZE is set in
# settings.toml.
CFLAGS += -DPIO_USB_HOST_ISOCHRONOUS=1

# USB host bulk input, for SDR devices
CIRCUITPY_USB_HOST_BULK = 1

# USB microphones as audio sources. Needs the isochronous ring above.
CIRCUITPY_USB_HOST_AUDIO = 1

# The ESP32-C6 co-processor, shared by the core and ESP32SPI. The core uses it
# for web workflow. The flags are set one by one, because CIRCUITPY_NETWORK
# would also turn on ssl, ipaddress and so on.
CIRCUITPY_AIRLIFT = 1
CIRCUITPY_SOCKETPOOL = 1
CIRCUITPY_WEB_WORKFLOW = 1
CIRCUITPY_MDNS = 0
CIRCUITPY_SSL = 0
