
INC += -I./$(SDK_MODULES_PATH)

include $(SDK_MODULES_PATH)build.mk

INC += -I$(SDK_ROOT)components/softdevice/s1xx_iot/headers
INC += -I$(SDK_ROOT)components/softdevice/s1xx_iot/headers/nrf52 
CFLAGS += -DBLUETOOTH_SD=100
CFLAGS += -DBLUETOOTH_SD_DEBUG=1

# softdevice .hex file
SOFTDEV_HEX ?= $(lastword $(wildcard $(SDK_ROOT)/components/softdevice/s1xx_iot/s1xx*softdevice.hex))
