LD_FILE = boards/samd21x18-bootloader.ld
USB_VID = 0x239A
USB_PID = 0x8028
USB_PRODUCT = "pIRKey M0"
USB_MANUFACTURER = "Adafruit Industries LLC"

INTERNAL_FLASH_FILESYSTEM = 1
LONGINT_IMPL = NONE

# A number of modules are removed for pIRKey to make room for frozen libraries.
# Many I/O functions are not available.
# math is very large and is also removed.
CIRCUITPY_ANALOGIO = 0
CIRCUITPY_MATH = 0
CIRCUITPY_NEOPIXEL_WRITE = 0
CIRCUITPY_ROTARYIO = 0
CIRCUITPY_RTC = 0
CIRCUITPY_SAMD = 0
CIRCUITPY_USB_MIDI = 1
CIRCUITPY_TOUCHIO = 0
CIRCUITPY_SMALL_BUILD = 1

CHIP_VARIANT = SAMD21E18A
CHIP_FAMILY = samd21

CFLAGS_INLINE_LIMIT = 35

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_DotStar
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_HID
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_IRRemote
