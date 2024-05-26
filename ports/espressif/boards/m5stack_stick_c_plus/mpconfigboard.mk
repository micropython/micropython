CIRCUITPY_CREATOR_ID =  0x10151015
CIRCUITPY_CREATION_ID = 0x0032000A

IDF_TARGET = esp32

CIRCUITPY_ESP_FLASH_MODE = qio
CIRCUITPY_ESP_FLASH_FREQ = 80m
CIRCUITPY_ESP_FLASH_SIZE = 4MB
CIRCUITPY_ESPCAMERA = 0

# Override partition layout to preserve compatibility because the default has changed.
FLASH_SIZE_SDKCONFIG = esp-idf-config/sdkconfig-flash-4MB-no-uf2.defaults
CIRCUITPY_DUALBANK = 1
CIRCUITPY_BLEIO = 0

SRC_C += pmic/axp192/axp192.c
