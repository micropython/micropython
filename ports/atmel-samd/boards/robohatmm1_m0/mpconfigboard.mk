LD_FILE = boards/samd21x18-bootloader-external-flash-crystalless.ld
USB_VID = 0x1209
USB_PID = 0x4D43
USB_PRODUCT = "Robo HAT MM1"
USB_MANUFACTURER = "Robotics Masters"

CHIP_VARIANT = SAMD21G18A
CHIP_FAMILY = samd21

SPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICE_COUNT = 1
EXTERNAL_FLASH_DEVICES = "W25Q64JV_IQ"
LONGINT_IMPL = MPZ

# Non-Flash Edition
#INTERNAL_FLASH_FILESYSTEM = 1
#LONGINT_IMPL = NONE

CIRCUITPY_DISPLAYIO = 0
CIRCUITPY_FREQUENCYIO = 0

CFLAGS_INLINE_LIMIT = 60
SUPEROPT_GC = 0

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_BusDevice
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
#FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_INA219
#FROZEN_MPY_DIRS += $(TOP)/frozen/RoboticsMasters_CircuitPython_MPU9250
