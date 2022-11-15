# Makefile directives for BlueKitchen BTstack

ifeq ($(MICROPY_BLUETOOTH_BTSTACK),1)

GIT_SUBMODULES += lib/btstack

MICROPY_BLUETOOTH_BTSTACK_USB ?= 0
MICROPY_BLUETOOTH_BTSTACK_H4 ?= 1
MICROPY_BLUETOOTH_BTSTACK_CONFIG_FILE ?= '"extmod/btstack/btstack_config_common.h"'

BTSTACK_EXTMOD_DIR = extmod/btstack

SRC_EXTMOD_C += $(BTSTACK_EXTMOD_DIR)/modbluetooth_btstack.c

INC += -I$(TOP)/$(BTSTACK_EXTMOD_DIR)

CFLAGS_EXTMOD += -DMICROPY_BLUETOOTH_BTSTACK=1
CFLAGS_EXTMOD += -DMICROPY_BLUETOOTH_BTSTACK_CONFIG_FILE=$(MICROPY_BLUETOOTH_BTSTACK_CONFIG_FILE)
CFLAGS_EXTMOD += -DMICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS=1
CFLAGS_EXTMOD += -DMICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING=1

BTSTACK_DIR = $(TOP)/lib/btstack

ifneq ($(wildcard $(BTSTACK_DIR)/src),)

include $(BTSTACK_DIR)/src/Makefile.inc
include $(BTSTACK_DIR)/src/ble/Makefile.inc

INC += -I$(BTSTACK_DIR)/src
INC += -I$(BTSTACK_DIR)/3rd-party/bluedroid/decoder/include
INC += -I$(BTSTACK_DIR)/3rd-party/bluedroid/encoder/include
INC += -I$(BTSTACK_DIR)/3rd-party/md5
INC += -I$(BTSTACK_DIR)/3rd-party/yxml

SRC_BTSTACK_C = \
	$(addprefix lib/btstack/src/, $(SRC_FILES)) \
	$(addprefix lib/btstack/src/ble/, $(filter-out %_tlv.c, $(SRC_BLE_FILES))) \

ifeq ($(MICROPY_BLUETOOTH_BTSTACK_USB),1)
ifeq ($(MICROPY_BLUETOOTH_BTSTACK_H4),1)
	$(error Cannot enable both MICROPY_BLUETOOTH_BTSTACK_USB and MICROPY_BLUETOOTH_BTSTACK_H4)
endif
endif

ifneq ($(MICROPY_BLUETOOTH_BTSTACK_USB),1)
ifneq ($(MICROPY_BLUETOOTH_BTSTACK_H4),1)
	$(error Must enable one of MICROPY_BLUETOOTH_BTSTACK_USB or MICROPY_BLUETOOTH_BTSTACK_H4)
endif
endif

ifeq ($(MICROPY_BLUETOOTH_BTSTACK_USB),1)
SRC_BTSTACK_C += \
	lib/btstack/platform/libusb/hci_transport_h2_libusb.c

CFLAGS_EXTMOD += -DMICROPY_BLUETOOTH_BTSTACK_USB=1

CFLAGS_THIRDPARTY += $(shell pkg-config libusb-1.0 --cflags)
LDFLAGS_THIRDPARTY += $(shell pkg-config libusb-1.0 --libs)
endif

ifeq ($(MICROPY_BLUETOOTH_BTSTACK_H4),1)
SRC_BTSTACK_C += \
	lib/btstack/src/hci_transport_h4.c \
	lib/btstack/chipset/zephyr/btstack_chipset_zephyr.c

SRC_BTSTACK_C += \
	extmod/btstack/btstack_hci_uart.c \

CFLAGS_EXTMOD += -DMICROPY_BLUETOOTH_BTSTACK_H4=1
endif

ifeq ($(MICROPY_BLUETOOTH_BTSTACK_ENABLE_CLASSIC),1)
include $(BTSTACK_DIR)/src/classic/Makefile.inc
SRC_BTSTACK_C += \
	$(addprefix lib/btstack/src/classic/, $(SRC_CLASSIC_FILES))
endif

SRC_THIRDPARTY_C += $(SRC_BTSTACK_C)

# Suppress some warnings.
BTSTACK_WARNING_CFLAGS = -Wno-old-style-definition -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-fallthrough
ifneq ($(CC),clang)
BTSTACK_WARNING_CFLAGS += -Wno-format
endif
$(BUILD)/lib/btstack/src/%.o: CFLAGS += $(BTSTACK_WARNING_CFLAGS)
$(BUILD)/lib/btstack/platform/%.o: CFLAGS += $(BTSTACK_WARNING_CFLAGS)

endif
endif
