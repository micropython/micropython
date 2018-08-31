MCU_SERIES = m4
MCU_VARIANT = nrf52
# Historical: nrf52 means nrf52832
MCU_SUB_VARIANT = nrf52
MCU_CHIP = nrf52832
SD ?= s132
SOFTDEV_VERSION ?= 5.0.0

ifeq ($(SD),)
    LD_FILE = boards/nrf52832_512k_64k.ld
else
    LD_FILE = boards/nrf52832_512k_64k_s132_$(SOFTDEV_VERSION).ld
endif

NRF_DEFINES += -DNRF52832_XXAA -DNRF52832
