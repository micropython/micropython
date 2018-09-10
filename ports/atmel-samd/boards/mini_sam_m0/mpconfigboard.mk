LD_FILE = boards/samd21x18-bootloader.ld
USB_VID = 0x1209
USB_PID = 0x2017
USB_PRODUCT = "Mini SAM M0"
USB_MANUFACTURER = "Benjamin Shockley"

INTERNAL_FLASH_FILESYSTEM = 1
LONGINT_IMPL = NONE

CHIP_VARIANT = SAMD21E18A
CHIP_FAMILY = samd21

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
