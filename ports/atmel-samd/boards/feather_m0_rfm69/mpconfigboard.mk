USB_VID = 0x239A
USB_PID = 0x80D2
USB_PRODUCT = "Feather M0 RFM69"
USB_MANUFACTURER = "Adafruit Industries LLC"

CHIP_VARIANT = SAMD21G18A
CHIP_FAMILY = samd21

INTERNAL_FLASH_FILESYSTEM = 1
LONGINT_IMPL = NONE
CIRCUITPY_FULL_BUILD = 0

# A number of modules are removed for RFM69 to make room for frozen libraries.
# Many I/O functions are not available.
CIRCUITPY_ANALOGIO = 1
CIRCUITPY_ROTARYIO = 0
CIRCUITPY_RTC = 0
CIRCUITPY_USB_MIDI = 0
CIRCUITPY_USB_HID = 0
CIRCUITPY_TOUCHIO = 0
CIRCUITPY_BUSDEVICE = 1

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_RFM69
