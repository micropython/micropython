LD_FILE = boards/samd21x18-bootloader.ld
USB_VID = 0x239A
USB_PID = 0x8028
USB_PRODUCT = "pIRKey M0"
USB_MANUFACTURER = "Adafruit Industries LLC"

INTERNAL_FLASH_FILESYSTEM = 1
LONGINT_IMPL = NONE

CHIP_VARIANT = SAMD21E18A
CHIP_FAMILY = samd21

CFLAGS_INLINE_LIMIT = 45

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_DotStar
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_HID
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_IRRemote
