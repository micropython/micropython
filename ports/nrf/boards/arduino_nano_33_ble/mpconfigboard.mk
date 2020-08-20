USB_VID = 0x2341
USB_PID = 0x805A
USB_PRODUCT = "Arduino_Nano_33_BLE"
USB_MANUFACTURER = "Arduino"

MCU_CHIP = nrf52840

INTERNAL_FLASH_FILESYSTEM = 1

# Allocate two, not just one I2C peripheral, so that we have both
# on-board and off-board I2C available.
# When SPIM3 becomes available we'll be able to have two I2C and two SPI peripherals.
# We use a CFLAGS define here because there are include order issues
# if we try to include "mpconfigport.h" into nrfx_config.h .
CFLAGS += -DCIRCUITPY_NRF_NUM_I2C=2
