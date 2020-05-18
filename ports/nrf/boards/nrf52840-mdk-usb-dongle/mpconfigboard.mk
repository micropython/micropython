MCU_SERIES = m4
MCU_VARIANT = nrf52
MCU_SUB_VARIANT = nrf52840
SOFTDEV_VERSION = 6.1.1
LD_FILES += boards/nrf52840-mdk-usb-dongle/nrf52840_open_bootloader.ld boards/nrf52840_1M_256k.ld

NRF_DEFINES += -DNRF52840_XXAA
