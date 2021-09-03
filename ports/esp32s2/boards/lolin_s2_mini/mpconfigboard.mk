USB_VID = 0x303A
USB_PID = 0x80C3
USB_PRODUCT = "S2 Mini"
USB_MANUFACTURER = "Lolin"

INTERNAL_FLASH_FILESYSTEM = 1
LONGINT_IMPL = MPZ

# The default queue depth of 16 overflows on release builds,
# so increase it to 32.
CFLAGS += -DCFG_TUD_TASK_QUEUE_SZ=32

CIRCUITPY_ESP_FLASH_MODE=qio
CIRCUITPY_ESP_FLASH_FREQ=80m
CIRCUITPY_ESP_FLASH_SIZE=4MB


# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_NeoPixel
