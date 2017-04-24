
SOFTDEV_HEX_NAME ?=
SOFTDEV_HEX_PATH ?=

ifeq ($(SD), s110)
	SOFTDEV_VERSION = 8.0.0
	INC += -Ibluetooth/$(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)/$(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)_API/include
	CFLAGS += -DBLUETOOTH_SD_DEBUG=1
	CFLAGS += -DBLUETOOTH_SD=110
	SOFTDEV_HEX_NAME = $(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)_softdevice.hex
	SOFTDEV_HEX_PATH = bluetooth/$(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)

else ifeq ($(SD), s120)
	$(error No BLE wrapper available yet)
else ifeq ($(SD), s130)
	$(error No BLE wrapper available yet)
else ifeq ($(SD), s132)
	SOFTDEV_VERSION=3.0.0
	INC += -Ibluetooth/$(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)/$(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)_API/include
	INC += -Ibluetooth/$(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)/$(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)_API/include/$(MCU_VARIANT)
	CFLAGS += -DBLUETOOTH_SD_DEBUG=1
	CFLAGS += -DBLUETOOTH_SD=132
	SOFTDEV_HEX_NAME = $(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)_softdevice.hex
	SOFTDEV_HEX_PATH = bluetooth/$(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)
else
	$(error Incorrect softdevice set flag)
endif

define STACK_MISSING_ERROR


###### ERROR: Bluetooth LE Stack not found ############
#                                                     #
# The build target requires a Bluetooth LE stack.     #
# $(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION) Bluetooth LE stack not found.      #
#                                                     #
# Please run the download script:                     #
#                                                     #
#       bluetooth/download_ble_stack.sh               #
#                                                     #
#######################################################

endef


SOFTDEV_HEX = $(SOFTDEV_HEX_PATH)/$(SOFTDEV_HEX_NAME)

ifeq ($(shell test ! -e $(SOFTDEV_HEX) && echo -n no),no)
    $(error $(STACK_MISSING_ERROR))
endif

INC += -I./bluetooth

SRC_C += \
        bluetooth/modble.c \
        bluetooth/ble_drv.c \
        bluetooth/ble_uart.c
