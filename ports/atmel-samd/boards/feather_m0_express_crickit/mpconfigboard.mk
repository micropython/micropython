LD_FILE = boards/samd21x18-bootloader-external-flash.ld
USB_VID = 0x239A
USB_PID = 0x8023
USB_PRODUCT = "Feather M0 Express"
USB_MANUFACTURER = "Adafruit Industries LLC"

SPI_FLASH_FILESYSTEM = 1
LONGINT_IMPL = MPZ

CHIP_VARIANT = SAMD21G18A
CHIP_FAMILY = samd21

CFLAGS_INLINE_LIMIT = 55

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_BusDevice
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Crickit
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Motor
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_seesaw
