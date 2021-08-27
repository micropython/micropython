USB_VID = 0x239A
USB_PID = 0x80AC
USB_PRODUCT = "FeatherS2"
USB_MANUFACTURER = "UnexpectedMaker"

INTERNAL_FLASH_FILESYSTEM = 1
LONGINT_IMPL = MPZ

# The default queue depth of 16 overflows on release builds,
# so increase it to 32.
CFLAGS += -DCFG_TUD_TASK_QUEUE_SZ=32

CIRCUITPY_ESP_FLASH_MODE=qio
CIRCUITPY_ESP_FLASH_FREQ=40m
CIRCUITPY_ESP_FLASH_SIZE=16MB

CIRCUITPY_BITBANG_APA102 = 1

# Include these Python libraries in firmware.
# FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_DotStar
