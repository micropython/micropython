USB_VID = 0x2E8A
USB_PID = 0x1043
USB_PRODUCT = "ARCHI"
USB_MANUFACTURER = "NEWSAN"

CHIP_VARIANT = RP2040
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "W25Q32JVxQ"

CIRCUITPY__EVE = 1
CIRCUITPY_PICODVI = 1

FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_MPU6050
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Pixel_Framebuf
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_LED_Animation
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Motor
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Register
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_seesaw
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_framebuf
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SimpleIO
