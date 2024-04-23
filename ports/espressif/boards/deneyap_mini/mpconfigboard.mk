USB_VID = 0x303A
USB_PID = 0x8142

USB_PRODUCT = "Deneyap Mini"
USB_MANUFACTURER = "Turkish Technology Team Foundation"

IDF_TARGET = esp32s2

CIRCUITPY_ESP_FLASH_MODE = qio
CIRCUITPY_ESP_FLASH_FREQ = 80m
CIRCUITPY_ESP_FLASH_SIZE = 4MB

# Override partition layout to preserve compatibility because the default has changed.
FLASH_SIZE_SDKCONFIG = esp-idf-config/sdkconfig-flash-4MB-no-uf2.defaults
CIRCUITPY_DUALBANK = 1
CIRCUITPY_BLEIO = 0

CIRCUITPY_ESPCAMERA = 0
