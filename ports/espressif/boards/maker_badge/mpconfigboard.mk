USB_VID = 0x239A
USB_PID = 0x2030
USB_PRODUCT = "Maker badge"
USB_MANUFACTURER = "Czech maker"

CIRCUITPY_ESPCAMERA = 0

CIRCUITPY_ESP_FLASH_MODE=dio
CIRCUITPY_ESP_FLASH_FREQ=40m
CIRCUITPY_ESP_FLASH_SIZE=4MB

IDF_TARGET = esp32s2

FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Requests
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_BusDevice
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Register
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Display_Text
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_UC8151D
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SSD1680
