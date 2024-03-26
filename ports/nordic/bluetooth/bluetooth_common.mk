ifeq ($(SD), s140)
	CFLAGS += -DBLUETOOTH_SD=140
	CFLAGS += -DBLE_API_VERSION=4
else
$(error Incorrect softdevice set flag)
endif

CFLAGS += -DBLUETOOTH_SD_DEBUG=1
CFLAGS += -DSOFTDEVICE_PRESENT

INC += -Ibluetooth/$(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)/$(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)_API/include
INC += -Ibluetooth/$(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)/$(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)_API/include/$(MCU_VARIANT)

SOFTDEV_HEX_NAME = $(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)_softdevice.hex
SOFTDEV_HEX_PATH = bluetooth/$(SD)_$(MCU_VARIANT)_$(SOFTDEV_VERSION)
SOFTDEV_HEX = $(SOFTDEV_HEX_PATH)/$(SOFTDEV_HEX_NAME)

define STACK_MISSING_ERROR


###### ERROR: Bluetooth LE Stack not found ############
#                                                     #
# The build target requires a Bluetooth LE stack.     #
# $(SOFTDEV_HEX) not found. #
#                                                     #
# Please run the download script:                     #
#                                                     #
#      bluetooth/download_ble_stack.sh                #
#                                                     #
#######################################################

endef

ifeq ($(shell test ! -e $(SOFTDEV_HEX) && echo -n no),no)
    $(error $(STACK_MISSING_ERROR))
endif
