MCU_SERIES = m4
MCU_VARIANT = nrf52
# Historical: nrf52 means nrf52832
MCU_SUB_VARIANT = nrf52
MCU_CHIP = nrf52832
SD ?= s132
SOFTDEV_VERSION ?= 2.0.1

LD_FILE = boards/feather_nrf52832/custom_nrf52832_dfu_app_$(SOFTDEV_VERSION).ld
BOOT_FILE = boards/feather_nrf52832/bootloader/feather52_bootloader_$(SOFTDEV_VERSION)_s132_single

BOOT_SETTING_ADDR = 0x7F000
NRF_DEFINES += -DNRF52832_XXAA -DNRF52832
