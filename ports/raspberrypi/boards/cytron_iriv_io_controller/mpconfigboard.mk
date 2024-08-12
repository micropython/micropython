USB_VID = 0x2E8A
USB_PID = 0x1093
USB_PRODUCT = "IRIV IO Controller"
USB_MANUFACTURER = "Cytron"

CHIP_VARIANT = RP2350
CHIP_PACKAGE = A
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "W25Q16JVxQ"

CIRCUITPY__EVE = 1
CIRCUITPY_ALARM = 0
CIRCUITPY_RGBMATRIX = 0
CIRCUITPY_USB_HOST = 0

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SimpleIO
FROZEN_MPY_DIRS += $(TOP)/frozen/circuitpython-pcf85063a
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Register
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Wiznet5k
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Requests
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_FakeRequests
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_ConnectionManager
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Ticks
