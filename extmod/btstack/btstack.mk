# Makefile directives for BlueKitchen BTstack

ifeq ($(MICROPY_BLUETOOTH_BTSTACK),1)

BTSTACK_EXTMOD_DIR = extmod/btstack

EXTMOD_SRC_C += extmod/btstack/modbluetooth_btstack.c

INC += -I$(TOP)/$(BTSTACK_EXTMOD_DIR)

CFLAGS_MOD += -DMICROPY_BLUETOOTH_BTSTACK=1

BTSTACK_DIR = $(TOP)/lib/btstack

include $(BTSTACK_DIR)/src/Makefile.inc
include $(BTSTACK_DIR)/src/ble/Makefile.inc

INC += -I$(BTSTACK_DIR)/src
INC += -I$(BTSTACK_DIR)/3rd-party/bluedroid/decoder/include
INC += -I$(BTSTACK_DIR)/3rd-party/bluedroid/encoder/include
INC += -I$(BTSTACK_DIR)/3rd-party/md5
INC += -I$(BTSTACK_DIR)/3rd-party/yxml

SRC_BTSTACK = \
	$(addprefix lib/btstack/src/, $(SRC_FILES)) \
	$(addprefix lib/btstack/src/ble/, $(filter-out %_tlv.c, $(SRC_BLE_FILES))) \
	lib/btstack/platform/embedded/btstack_run_loop_embedded.c \

ifeq ($MICROPY_BLUETOOTH_BTSTACK_ENABLE_CLASSIC,1)
include $(BTSTACK_DIR)/src/classic/Makefile.inc
SRC_BTSTACK += \
	$(addprefix lib/btstack/src/classic/, $(SRC_CLASSIC_FILES))
endif

SRC_LIB += $(SRC_BTSTACK)

#$(BUILD)/lib/btstack/src/classic/btstack_link_key_db_static.o: CFLAGS += -Wno-error=pointer-arith

# Incorrect %u, should be %lu.
$(BUILD)/lib/btstack/src/classic/a2dp_source.o: CFLAGS += -Wno-error=format=
$(BUILD)/lib/btstack/src/classic/btstack_sbc_decoder_bluedroid.o: CFLAGS += -Wno-error=format=
$(BUILD)/lib/btstack/src/classic/btstack_link_key_db_tlv.o: CFLAGS += -Wno-error=format=
$(BUILD)/lib/btstack/src/classic/goep_client.o: CFLAGS += -Wno-error=format=
$(BUILD)/lib/btstack/src/ble/le_device_db_tlv.o: CFLAGS += -Wno-error=format=



endif
