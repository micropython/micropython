
SDK_MODULES ?=

ifeq ($(SD), s1xx)
	SDK_MODULES = iot_0.9.0
else ifeq ($(SD), s110)
    SDK_MODULES = sdk_10.0.0
else ifeq ($(SD), s120)
	SDK_MODULES = sdk_10.0.0
	$(error No supported BLE wrapper)
else ifeq ($(SD), s130)
    SDK_MODULES = sdk_10.0.0
else ifeq ($(SD), s132)
	SDK_MODULES = sdk_12.1.0
	SDK_COMPONENTS = 0
else
	$(error No SDK configured for this SD)
endif

SDK_MODULES_PATH = sdk/$(SDK_MODULES)/

include $(SDK_MODULES_PATH)sdk.mk

INC += -I./sdk

SRC_C += \
        sdk/modble.c \
        sdk/ble_drv.c
