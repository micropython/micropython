USB_VID = 0x239A
USB_PID = 0x8046
USB_PRODUCT = "Circuit Playground Bluefruit"
USB_MANUFACTURER = "Adafruit Industries LLC"

MCU_CHIP = nrf52840

QSPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICE_COUNT = 1
EXTERNAL_FLASH_DEVICES = "GD25Q16C"

# Allocate two, not just one I2C peripheral for CPB, so that we have both
# on-board and off-board I2C available.
# We use a CFLAGS define here because there are include order issues
# if we try to include "mpconfigport.h" into nrfx_config.h .
CFLAGS += -DCIRCUITPY_NRF_NUM_I2C=2
