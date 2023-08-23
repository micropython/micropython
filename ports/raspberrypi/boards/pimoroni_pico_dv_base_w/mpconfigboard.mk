USB_VID = 0x2E8A
USB_PID = 0x105A
USB_PRODUCT = "Pimoroni Pico DV Demo Base for Pico W"
USB_MANUFACTURER = "Pimoroni"

CHIP_VARIANT = RP2040
CHIP_FAMILY = rp2

EXTERNAL_FLASH_DEVICES = "W25Q16JVxQ"

CIRCUITPY__EVE = 1
CIRCUITPY_PICODVI = 1

# Must be accompanied by a linker script change
CFLAGS += -DCIRCUITPY_FIRMWARE_SIZE='(1536 * 1024)'

# Include these Python libraries in firmware.
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_SD
