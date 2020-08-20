USB_VID = 0x239A
USB_PID = 0x8092
USB_PRODUCT = "Feather M7 1011"
USB_MANUFACTURER = "Adafruit"

CHIP_VARIANT = MIMXRT1011DAE5A
CHIP_FAMILY = MIMXRT1011
FLASH = W25Q32JV

# Include these Python libraries in the firmware
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_ESP32SPI
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Requests
