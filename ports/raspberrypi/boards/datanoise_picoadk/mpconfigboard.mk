USB_VID = 0x2E8A
USB_PID = 0x104B
USB_PRODUCT = "PicoADK"
USB_MANUFACTURER = "Datanoise"

CHIP_VARIANT = RP2040
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "GD25Q32C,W25Q32JVxQ"

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
