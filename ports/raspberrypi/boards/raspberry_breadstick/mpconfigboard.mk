USB_VID = 0x2E8A
USB_PID = 0x105E
USB_PRODUCT = "Raspberry Breadstick"
USB_MANUFACTURER = "J&R Creative Technologies Inc."

CHIP_VARIANT = RP2040
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "W25Q16JVxQ"

CIRCUITPY__EVE = 1
CIRCUITPY_PICODVI = 1

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_LSM6DS/
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_DotStar/
