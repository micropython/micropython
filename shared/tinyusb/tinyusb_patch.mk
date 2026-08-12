# Patch tinyusb for known issues.
#
# This file should be included immediately after including `$(TOP)/lib/tinyusb/src/tinyusb.mk`.

TINYUSB_CDC_DEVICE_ORIGINAL = $(TOP)/lib/tinyusb/src/class/cdc/cdc_device.c
TINYUSB_CDC_DEVICE_PATCH_FILE = $(TOP)/shared/tinyusb/cdc_device.c.patch
TINYUSB_CDC_DEVICE_PATCHED = $(BUILD)/cdc_device.c

TINYUSB_SRC_C := $(filter-out src/class/cdc/cdc_device.c, $(TINYUSB_SRC_C))

$(TINYUSB_CDC_DEVICE_PATCHED): $(TINYUSB_CDC_DEVICE_ORIGINAL) $(TINYUSB_CDC_DEVICE_PATCH_FILE) | $(HEADER_BUILD)
	$(ECHO) "Create $@"
	$(Q)$(CP) $< $@
	$(Q)patch --quiet $@ $(TINYUSB_CDC_DEVICE_PATCH_FILE)

$(TINYUSB_CDC_DEVICE_PATCHED:.c=.o): CFLAGS += -I$(TOP)/lib/tinyusb/src/class/cdc

OBJ += $(TINYUSB_CDC_DEVICE_PATCHED:.c=.o)
