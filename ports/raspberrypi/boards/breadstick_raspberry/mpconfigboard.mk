USB_VID = 0x2E8A
USB_PID = 0x105E
USB_PRODUCT = "Raspberry Breadstick"
USB_MANUFACTURER = "Breadstick Innovations"

CHIP_VARIANT = RP2040
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "W25Q128JVxQ"

CIRCUITPY_BITBANG_APA102 = 1

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_LSM6DS/
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_DotStar/
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Register/
# FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_FancyLED/
