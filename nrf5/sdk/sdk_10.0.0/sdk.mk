
INC += -I./$(SDK_MODULES_PATH)

# Nothing to build from SDK.
# include $(SDK_MODULES_PATH)build.mk

INC += -I$(SDK_ROOT)components/softdevice/$(SD)/headers
CFLAGS += -DBLUETOOTH_SD_DEBUG=1

# softdevice .hex file
ifeq ($(SD), s110)
CFLAGS += -DBLUETOOTH_SD=110
SOFTDEV_HEX ?= $(lastword $(wildcard $(SDK_ROOT)/components/softdevice/s110/hex/s110_nrf51_8.0.0_softdevice.hex))
else ifeq ($(SD), s120)
CFLAGS += -DBLUETOOTH_SD=120
SOFTDEV_HEX ?= $(lastword $(wildcard $(SDK_ROOT)/components/softdevice/s120/hex/s120_nrf51_2.1.0_softdevice.hex))
endif