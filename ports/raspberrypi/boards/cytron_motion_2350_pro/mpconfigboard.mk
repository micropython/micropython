USB_VID = 0x2E8A
USB_PID = 0x1096
USB_PRODUCT = "MOTION 2350 Pro"
USB_MANUFACTURER = "Cytron"

CHIP_VARIANT = RP2350
CHIP_PACKAGE = A
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "W25Q16JVxQ"

CIRCUITPY__EVE = 1
CIRCUITPY_ALARM = 0

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Motor
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SimpleIO
