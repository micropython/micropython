USB_VID = 0x303A
USB_PID = 0x80C3
USB_PRODUCT = "S2 Mini"
USB_MANUFACTURER = "Lolin"

IDF_TARGET = esp32s2

CIRCUITPY_ESP_FLASH_MODE = qio
CIRCUITPY_ESP_FLASH_FREQ = 80m
CIRCUITPY_ESP_FLASH_SIZE = 4MB


# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
