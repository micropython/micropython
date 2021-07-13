USB_VID = 0x239A
USB_PID = 0xD1ED
USB_PRODUCT = "HalloWing M0 Express"
USB_MANUFACTURER = "Adafruit Industries LLC"

CHIP_VARIANT = SAMD21G18A
CHIP_FAMILY = samd21

SPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICES = "W25Q64JVxQ, GD25Q64C"
LONGINT_IMPL = NONE

# To keep the build small
CIRCUITPY_AUDIOBUSIO = 0

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_LIS3DH
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
