
SOFTDEV_HEX_NAME ?=

ifeq ($(SD), s110)
	INC += -I$(SDK_ROOT)components/softdevice/$(SD)/headers
	CFLAGS += -DBLUETOOTH_SD_DEBUG=1
	CFLAGS += -DBLUETOOTH_SD=110
	SOFTDEV_HEX_NAME = s110_nrf51_8.0.0_softdevice.hex
else ifeq ($(SD), s120)
	$(error No BLE wrapper available yet)
else ifeq ($(SD), s130)
	$(error No BLE wrapper available yet)
else ifeq ($(SD), s132)
	INC += -I$(SDK_ROOT)components/softdevice/$(SD)/headers
	INC += -I$(SDK_ROOT)components/softdevice/$(SD)/headers/$(MCU_VARIANT)
	CFLAGS += -DBLUETOOTH_SD_DEBUG=1
	CFLAGS += -DBLUETOOTH_SD=132
	SOFTDEV_HEX_NAME = s132_nrf52_3.0.0_softdevice.hex
#else ifeq ($(SD), s1xx)
#	include bluetooth/iot_0.9.0/sdk.mk
else
	$(error Incorrect softdevice set flag)
endif

SOFTDEV_HEX = $(lastword $(wildcard $(SDK_ROOT)/components/softdevice/$(SD)/hex/$(SOFTDEV_HEX_NAME)))

INC += -I./bluetooth

SRC_C += \
        bluetooth/modble.c \
        bluetooth/ble_drv.c \
        bluetooth/ble_uart.c
