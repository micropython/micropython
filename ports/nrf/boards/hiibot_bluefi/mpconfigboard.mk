USB_VID = 0x239A
USB_PID = 0x80B2
USB_PRODUCT = "HiiBot BlueFi"
USB_MANUFACTURER = "HiiBot"

MCU_CHIP = nrf52840

QSPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICE_COUNT = 1
EXTERNAL_FLASH_DEVICES = "W25Q16JV_IQ"

# Allocate two, not just one I2C peripheral for Bluefi, so that we have both
# on-board and off-board I2C available.
# When SPIM3 becomes available we'll be able to have two I2C and two SPI peripherals.
# We use a CFLAGS define here because there are include order issues
# if we try to include "mpconfigport.h" into nrfx_config.h .
CFLAGS += -DCIRCUITPY_NRF_NUM_I2C=2
