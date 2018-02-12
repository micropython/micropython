ifeq ($(SD), s132)
	SOFTDEV_VERSION_LONG = $(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)
	CFLAGS += -DBLUETOOTH_SD=132

ifeq ($(SOFTDEV_VERSION), 2.0.1)
	CFLAGS += -DBLE_API_VERSION=2
else ifeq ($(SOFTDEV_VERSION), 5.0.0)
	CFLAGS += -DBLE_API_VERSION=4
endif
else ifeq ($(SD), s140)
	SOFTDEV_VERSION_LONG = $(SD)_$(MCU_SUB_VARIANT)_$(SOFTDEV_VERSION)

	CFLAGS += -DBLUETOOTH_SD=140
	CFLAGS += -DBLE_API_VERSION=4
else
$(error Incorrect softdevice set flag)
endif

CFLAGS += -DBLUETOOTH_SD_DEBUG=1

INC += -Idrivers/bluetooth/$(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)/$(SOFTDEV_VERSION_LONG)_API/include
INC += -Idrivers/bluetooth/$(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)/$(SOFTDEV_VERSION_LONG)_API/include/$(MCU_VARIANT)

SOFTDEV_HEX_NAME = $(SOFTDEV_VERSION_LONG)_softdevice.hex
SOFTDEV_HEX_PATH = drivers/bluetooth/$(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)
SOFTDEV_HEX = $(SOFTDEV_HEX_PATH)/$(SOFTDEV_HEX_NAME)

define STACK_MISSING_ERROR


###### ERROR: Bluetooth LE Stack not found ############
#                                                     #
# The build target requires a Bluetooth LE stack.     #
# $(SOFTDEV_VERSION_LONG) Bluetooth LE stack not found.      #
#                                                     #
# Please run the download script:                     #
#                                                     #
#      drivers/bluetooth/download_ble_stack.sh        #
#                                                     #
#######################################################

endef

ifeq ($(shell test ! -e $(SOFTDEV_HEX) && echo -n no),no)
    $(error $(STACK_MISSING_ERROR))
endif
