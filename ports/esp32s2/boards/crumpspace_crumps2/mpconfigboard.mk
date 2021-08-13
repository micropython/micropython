USB_VID = 0x1209
USB_PID = 0x3141
USB_PRODUCT = "CrumpS2"
USB_MANUFACTURER = "CrumpSpace"

INTERNAL_FLASH_FILESYSTEM = 1
LONGINT_IMPL = MPZ

# The default queue depth of 16 overflows on release builds,
# so increase it to 32.
CFLAGS += -DCFG_TUD_TASK_QUEUE_SZ=32

CIRCUITPY_ESP_FLASH_MODE=dio
CIRCUITPY_ESP_FLASH_FREQ=40m
CIRCUITPY_ESP_FLASH_SIZE=4MB

CIRCUITPY_BITBANG_APA102 = 1

CIRCUITPY_MODULE=wrover

# Include these Python libraries in firmware.
# FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_DotStar
