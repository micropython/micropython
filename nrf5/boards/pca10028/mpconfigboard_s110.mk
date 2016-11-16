MCU_SERIES = m0
MCU_VARIANT = NRF51
LD_FILE = boards/nrf51822_ac_s110.ld
NRF_SOFTDEVICE = NRF_S110_SOFTDEVICE


SRC_C += \
	hal/hal_uart.c \
	softdevice/modble.c \
	softdevice/softdevice.c

CFLAGS += -I./softdevice
CFLAGS += -I./softdevice/s110/headers
CFLAGS += -DBLUETOOTH_SD=110
CFLAGS += -DBLUETOOTH_SD_DEBUG=1
