MCU_SERIES = m4
MCU_VARIANT = nrf52
MCU_SUB_VARIANT = nrf52840
SOFTDEV_VERSION = 6.1.1
LD_FILES += boards/nrf52840_1M_256k.ld

NRF_DEFINES += -DNRF52840_XXAA

# Note: This requires openocd >0.10
OPENOCD_TARGET ?= boards/$(BOARD)/nrf52-particle.cfg
