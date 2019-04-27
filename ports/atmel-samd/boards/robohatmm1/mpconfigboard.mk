LD_FILE = boards/samd21x18-bootloader-external-flash-crystalless.ld
USB_VID = 0x1209
USB_PID = 0x4D43
USB_PRODUCT = "Robo HAT MM1"
USB_MANUFACTURER = "Robotics Masters"

# Non-Flash Edition
#INTERNAL_FLASH_FILESYSTEM = 1
#LONGINT_IMPL = NONE

# SPI-Flash Edition
SPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICE_COUNT = 1
EXTERNAL_FLASH_DEVICES = "W25Q64JV_IQ"
LONGINT_IMPL = MPZ

CHIP_VARIANT = SAMD21G18A
CHIP_FAMILY = samd21

# Make room for frozen Libraries
CIRCUITPY_DISPLAYIO = 0
CIRCUITPY_FREQUENCYIO = 0

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_BusDevice
#FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_INA219
#FROZEN_MPY_DIRS += $(TOP)/frozen/RoboticsMasters_CircuitPython_MPU9250
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
#FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Crickit
#FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Motor
#FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_seesaw

#Adding per @danh to reduce memory usage and get the latest changes in
CFLAGS_INLINE_LIMIT = 55
