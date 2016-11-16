MCU_SERIES = m0
MCU_VARIANT = NRF51
LD_FILE = boards/nrf51822_ac_s130.ld
NRF_SOFTDEVICE = NRF_S130_SOFTDEVICE

SRC_C += \
	hal/hal_uart.c \
	softdevice/modble.c \
	softdevice/softdevice.c

CFLAGS += -I./softdevice
CFLAGS += -I./softdevice/s130/headers 
CFLAGS += -DBLUETOOTH_SD=130
CFLAGS += -DBLUETOOTH_SD_DEBUG=1
