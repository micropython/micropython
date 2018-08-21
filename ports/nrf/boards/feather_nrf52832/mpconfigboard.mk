MCU_SERIES = m4
MCU_VARIANT = nrf52
MCU_SUB_VARIANT = nrf52
SD ?= s132
SOFTDEV_VERSION ?= 2.0.1

LD_FILE = boards/feather_nrf_52832/custom_nrf52832_dfu_app_$(SOFTDEV_VERSION).ld
BOOT_FILE = boards/feather_nrf_52832/bootloader/feather52_bootloader_$(SOFTDEV_VERSION)_s132_single

BOOT_SETTING_ADDR = 0x7F000
NRF_DEFINES += -DNRF52832_XXAA
