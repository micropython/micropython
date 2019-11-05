USB_VID = 0x239A
USB_PID = 0x8046
USB_PRODUCT = "Circuit Playground Bluefruit"
USB_MANUFACTURER = "Adafruit Industries LLC"

MCU_SERIES = m4
MCU_VARIANT = nrf52
MCU_SUB_VARIANT = nrf52840
MCU_CHIP = nrf52840
SD ?= s140
SOFTDEV_VERSION ?= 6.1.0

BOOT_SETTING_ADDR = 0xFF000

ifeq ($(SD),)
	LD_FILE = boards/nrf52840_1M_256k.ld
else
	LD_FILE = boards/adafruit_$(MCU_SUB_VARIANT)_$(SD_LOWER)_v$(firstword $(subst ., ,$(SOFTDEV_VERSION))).ld
	CIRCUITPY_BLEIO = 1
endif

NRF_DEFINES += -DNRF52840_XXAA -DNRF52840

QSPI_FLASH_FILESYSTEM = 1
EXTERNAL_FLASH_DEVICE_COUNT = 1
EXTERNAL_FLASH_DEVICES = "GD25Q16C"

# Allocate two, not just one I2C peripheral for CPB, so that we have both
# on-board and off-board I2C available.
# When SPIM3 becomes available we'll be able to have two I2C and two SPI peripherals.
# We use a CFLAGS define here because there are include order issues
# if we try to include "mpconfigport.h" into nrfx_config.h .
CFLAGS += -DCIRCUITPY_NRF_NUM_I2C=2
