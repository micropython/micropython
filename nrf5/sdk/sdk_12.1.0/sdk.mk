
INC += -I./$(SDK_MODULES_PATH)

include $(SDK_MODULES_PATH)build.mk

INC += -I$(SDK_ROOT)components/softdevice/$(SD)/headers
INC += -I$(SDK_ROOT)components/softdevice/$(SD)/headers/$(MCU_VARIANT)
CFLAGS += -DBLUETOOTH_SD_DEBUG=1

# softdevice .hex file
ifeq ($(SD), s130)
CFLAGS += -DBLUETOOTH_SD=130
SOFTDEV_HEX ?= $(lastword $(wildcard $(SDK_ROOT)/components/softdevice/s130/hex/s130_nrf51_2.0.1_softdevice.hex))
else ifeq ($(SD), s132)
CFLAGS += -DBLUETOOTH_SD=132
SOFTDEV_HEX ?= $(lastword $(wildcard $(SDK_ROOT)/components/softdevice/s132/hex/s132_nrf52_3.0.0_softdevice.hex))
# Update to local linker file, special linking has to be done
LD_FILE := $(SDK_MODULES_PATH)nrf52832_aa_s132.ld
endif
