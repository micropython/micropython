USB_VID = 0x239A
USB_PID = 0x8019
USB_PRODUCT = "CircuitPlayground Express with displayio"
USB_MANUFACTURER = "Adafruit Industries LLC"

CHIP_VARIANT = SAMD21G18A
CHIP_FAMILY = samd21

SPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICES = "S25FL216K, GD25Q16C"

# Turn off features and optimizations for displayio build to make room for additional frozen libs.
LONGINT_IMPL = NONE
CIRCUITPY_KEYPAD = 0
CIRCUITPY_ONEWIREIO = 0
CIRCUITPY_PIXELBUF = 0
CIRCUITPY_ROTARYIO = 0
CIRCUITPY_RTC = 0
CIRCUITPY_USB_MIDI = 0

# So not all of displayio, sorry!
CIRCUITPY_VECTORIO = 0
CIRCUITPY_BITMAPTOOLS = 0
CIRCUITPY_PARALLELDISPLAY = 0

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_CircuitPlayground/frozen_cpx
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_LIS3DH
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Thermistor
