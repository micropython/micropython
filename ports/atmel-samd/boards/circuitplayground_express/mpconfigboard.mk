USB_VID = 0x239A
USB_PID = 0x8019
USB_PRODUCT = "CircuitPlayground Express"
USB_MANUFACTURER = "Adafruit Industries LLC"

CHIP_VARIANT = SAMD21G18A
CHIP_FAMILY = samd21

SPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICES = "S25FL216K, GD25Q16C"
LONGINT_IMPL = MPZ

# Make room for frozen libs.
CIRCUITPY_BITMAPTOOLS = 0
CIRCUITPY_DISPLAYIO = 0
CIRCUITPY_FREQUENCYIO = 0
CIRCUITPY_I2CPERIPHERAL = 0
CIRCUITPY_MSGPACK = 0
MICROPY_PY_ASYNC_AWAIT = 0

SUPEROPT_GC = 0
CFLAGS_INLINE_LIMIT = 55

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_CircuitPlayground
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_HID
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_LIS3DH
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Thermistor
