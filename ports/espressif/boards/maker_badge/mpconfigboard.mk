USB_VID = 0x239A
USB_PID = 0x2030
USB_PRODUCT = "Maker badge"
USB_MANUFACTURER = "Czech maker"

IDF_TARGET = esp32s2

INTERNAL_FLASH_FILESYSTEM = 1
LONGINT_IMPL = MPZ

# The default queue depth of 16 overflows on release builds,
# so increase it to 32.
CFLAGS += -DCFG_TUD_TASK_QUEUE_SZ=32

CIRCUITPY_ESP32_CAMERA = 0

CIRCUITPY_ESP_FLASH_MODE=dio
CIRCUITPY_ESP_FLASH_FREQ=40m
CIRCUITPY_ESP_FLASH_SIZE=4MB

CIRCUITPY_MODULE=wroom

FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Requests
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_BusDevice
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Register
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Display_Text
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_UC8151D
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SSD1680
