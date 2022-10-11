USB_VID = 0x303A
USB_PID = 0x80F9
USB_PRODUCT = "Cytron Maker Feather AIoT S3"
USB_MANUFACTURER = "Cytron"

IDF_TARGET = esp32s3

CIRCUITPY_ESP_FLASH_MODE = dio
CIRCUITPY_ESP_FLASH_FREQ = 80m
CIRCUITPY_ESP_FLASH_SIZE = 8MB

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
