USB_VID = 0x239A
USB_PID = 0x8078
USB_PRODUCT = "IMXRT1010-EVK"
USB_MANUFACTURER = "NXP"

CHIP_VARIANT = MIMXRT1011DAE5A
CHIP_FAMILY = MIMXRT1011
FLASH = AT25SF128A

# Include these Python libraries in the firmware
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_ESP32SPI
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Requests
