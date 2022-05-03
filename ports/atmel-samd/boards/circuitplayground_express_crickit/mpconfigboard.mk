USB_VID = 0x239A
USB_PID = 0x8019
USB_PRODUCT = "CircuitPlayground Express with Crickit libraries"
USB_MANUFACTURER = "Adafruit Industries LLC"

CHIP_VARIANT = SAMD21G18A
CHIP_FAMILY = samd21

SPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICES = "S25FL216K, GD25Q16C"

# Turn off features and optimizations for Crickit build to make room for additional frozen libs.
LONGINT_IMPL = NONE

CIRCUITPY_BUSDEVICE = 1
CIRCUITPY_DISPLAYIO = 0
CIRCUITPY_KEYPAD = 0
CIRCUITPY_ONEWIREIO = 0

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_CircuitPlayground/frozen_cpx
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Crickit
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_LIS3DH
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Motor
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_seesaw
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Thermistor
