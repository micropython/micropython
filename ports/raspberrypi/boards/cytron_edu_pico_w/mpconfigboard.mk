USB_VID = 0x2E8A
USB_PID = 0x1074
USB_PRODUCT = "Cytron EDU PICO for Pico W"
USB_MANUFACTURER = "Cytron"

CHIP_VARIANT = RP2040
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "W25Q16JVxQ"

CIRCUITPY__EVE = 1

CIRCUITPY_CYW43 = 1
CIRCUITPY_SSL = 1
CIRCUITPY_HASHLIB = 1
CIRCUITPY_WEB_WORKFLOW = 1
CIRCUITPY_MDNS = 1
CIRCUITPY_SOCKETPOOL = 1
CIRCUITPY_WIFI = 1

CIRCUITPY_PICODVI = 1

CFLAGS += -DCYW43_PIN_WL_HOST_WAKE=24 -DCYW43_PIN_WL_REG_ON=23 -DCYW43_WL_GPIO_COUNT=3 -DCYW43_WL_GPIO_LED_PIN=0
# Must be accompanied by a linker script change
CFLAGS += -DCIRCUITPY_FIRMWARE_SIZE='(1536 * 1024)'

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Motor
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SimpleIO
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_APDS9960
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_framebuf
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SSD1306
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_DisplayIO_SSD1306
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_ImageLoad
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_AHTx0
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SD
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_HTTPServer
