USB_VID = 0x04D8
USB_PID = 0xEF67
USB_PRODUCT = "senseBox MCU"
USB_MANUFACTURER = "senseBox"

CHIP_VARIANT = SAMD21G18A
CHIP_FAMILY = samd21

INTERNAL_FLASH_FILESYSTEM = 1
LONGINT_IMPL = NONE
CIRCUITPY_FULL_BUILD = 0

# There are many pin definitions on this board; it doesn't quite fit on very large translations.
# Remove a couple of modules.
CIRCUITPY_ONEWIREIO = 0
CIRCUITPY_RAINBOWIO = 0
