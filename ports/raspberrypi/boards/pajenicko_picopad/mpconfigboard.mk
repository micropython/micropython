USB_VID = 0x2E8A
USB_PID = 0x1063

USB_PRODUCT = "PicoPad"
USB_MANUFACTURER = "Pajenicko s.r.o."

CHIP_VARIANT = RP2040
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "W25Q16JVxQ"

CIRCUITPY_USB_HOST = 0

CIRCUITPY_KEYPAD = 1
CIRCUITPY_STAGE = 1
CIRCUITPY_PICOGAME = 1
CIRCUITPY_PICOGAME_RGB444 = 1
CIRCUITPY_AUDIOIO = 1
CIRCUITPY_AUDIOEFFECTS = 0

# Peripherals this board physically lacks: no FT8xx EVE display, no camera for the
# qrio QR *decoder* (QR generation = pure-Python adafruit_miniqr, unaffected), and no
# DVI/HDMI connector. Dropping them frees flash for the picogame engine.
CIRCUITPY__EVE = 0
CIRCUITPY_QRIO = 0

CIRCUITPY_CYW43 = 1
CIRCUITPY_SSL = 1
CIRCUITPY_HASHLIB = 1
CIRCUITPY_WEB_WORKFLOW = 1
CIRCUITPY_MDNS = 1
CIRCUITPY_SOCKETPOOL = 1
CIRCUITPY_WIFI = 1


# Pimoroni PicoSystem peripherals are compatible, we can use of existing ugame.py
FROZEN_MPY_DIRS += $(TOP)/frozen/circuitpython-stage/picosystem

CFLAGS += \
    -DCYW43_PIN_WL_DYNAMIC=0 \
	-DCYW43_DEFAULT_PIN_WL_HOST_WAKE=24 \
	-DCYW43_DEFAULT_PIN_WL_REG_ON=23 \
	-DCYW43_DEFAULT_PIN_WL_CLOCK=29 \
	-DCYW43_DEFAULT_PIN_WL_DATA_IN=24 \
	-DCYW43_DEFAULT_PIN_WL_DATA_OUT=24 \
	-DCYW43_DEFAULT_PIN_WL_CS=25 \
	-DCYW43_WL_GPIO_COUNT=3 \
	-DCYW43_WL_GPIO_LED_PIN=0

# Must be accompanied by a linker script change
CFLAGS += -DCIRCUITPY_FIRMWARE_SIZE='(1536 * 1024)'
