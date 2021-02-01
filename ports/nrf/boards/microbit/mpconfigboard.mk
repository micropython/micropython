MCU_SERIES = m0
MCU_VARIANT = nrf51
MCU_SUB_VARIANT = nrf51822
SOFTDEV_VERSION = 8.0.0
ifneq ($(SD),)
LD_FILES += boards/microbit/custom_nrf51822_s110_microbit.ld
endif
LD_FILES += boards/nrf51x22_256k_16k.ld
FLASHER = pyocd

CFLAGS += -DBLUETOOTH_LFCLK_RC
