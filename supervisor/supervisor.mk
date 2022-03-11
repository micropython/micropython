SRC_SUPERVISOR = \
	main.c \
	supervisor/port.c \
	supervisor/shared/background_callback.c \
	supervisor/shared/board.c \
	supervisor/shared/cpu.c \
	supervisor/shared/flash.c \
	supervisor/shared/lock.c \
	supervisor/shared/memory.c \
	supervisor/shared/micropython.c \
	supervisor/shared/reload.c \
	supervisor/shared/safe_mode.c \
	supervisor/shared/stack.c \
	supervisor/shared/status_leds.c \
	supervisor/shared/tick.c \
	supervisor/shared/traceback.c \
	supervisor/shared/translate.c

ifeq ($(DISABLE_FILESYSTEM),1)
SRC_SUPERVISOR += supervisor/stub/filesystem.c
else
SRC_SUPERVISOR += supervisor/shared/filesystem.c
endif

NO_USB ?= $(wildcard supervisor/usb.c)

INTERNAL_FLASH_FILESYSTEM ?= 0
CFLAGS += -DINTERNAL_FLASH_FILESYSTEM=$(INTERNAL_FLASH_FILESYSTEM)

QSPI_FLASH_FILESYSTEM ?= 0
CFLAGS += -DQSPI_FLASH_FILESYSTEM=$(QSPI_FLASH_FILESYSTEM)

SPI_FLASH_FILESYSTEM ?= 0
CFLAGS += -DSPI_FLASH_FILESYSTEM=$(SPI_FLASH_FILESYSTEM)

ifeq ($(CIRCUITPY_BLEIO),1)
	SRC_SUPERVISOR += supervisor/shared/bluetooth/bluetooth.c
  CIRCUITPY_CREATOR_ID ?= $(USB_VID)
  CIRCUITPY_CREATION_ID ?= $(USB_PID)
  CFLAGS += -DCIRCUITPY_CREATOR_ID=$(CIRCUITPY_CREATOR_ID)
  CFLAGS += -DCIRCUITPY_CREATION_ID=$(CIRCUITPY_CREATION_ID)
  ifeq ($(CIRCUITPY_BLE_FILE_SERVICE),1)
    SRC_SUPERVISOR += supervisor/shared/bluetooth/file_transfer.c
  endif
  ifeq ($(CIRCUITPY_SERIAL_BLE),1)
    SRC_SUPERVISOR += supervisor/shared/bluetooth/serial.c
  endif
endif

# Choose which flash filesystem impl to use.
# (Right now INTERNAL_FLASH_FILESYSTEM and (Q)SPI_FLASH_FILESYSTEM are mutually exclusive.
# But that might not be true in the future.)
ifeq ($(INTERNAL_FLASH_FILESYSTEM),1)
  ifeq ($(DISABLE_FILESYSTEM),1)
    SRC_SUPERVISOR += supervisor/stub/internal_flash.c
  else
    SRC_SUPERVISOR += supervisor/internal_flash.c
  endif
else
  CFLAGS += -DEXTERNAL_FLASH_DEVICES=$(EXTERNAL_FLASH_DEVICES) \

  SRC_SUPERVISOR += supervisor/shared/external_flash/external_flash.c
  ifeq ($(SPI_FLASH_FILESYSTEM),1)
    SRC_SUPERVISOR += supervisor/shared/external_flash/spi_flash.c
  endif
  ifeq ($(QSPI_FLASH_FILESYSTEM),1)
    SRC_SUPERVISOR += supervisor/qspi_flash.c supervisor/shared/external_flash/qspi_flash.c
  endif

$(HEADER_BUILD)/devices.h : ../../supervisor/shared/external_flash/devices.h.jinja ../../tools/gen_nvm_devices.py | $(HEADER_BUILD)
	$(STEPECHO) "GEN $@"
	$(Q)install -d $(BUILD)/genhdr
	$(Q)$(PYTHON) ../../tools/gen_nvm_devices.py $< $@

$(BUILD)/supervisor/shared/external_flash/external_flash.o: $(HEADER_BUILD)/devices.h

endif

ifeq ($(CIRCUITPY_USB),0)
  ifeq ($(wildcard supervisor/serial.c),)
    SRC_SUPERVISOR += supervisor/shared/serial.c \
                      supervisor/shared/workflow.c \

  else
    SRC_SUPERVISOR += supervisor/serial.c
  endif
else
  SRC_SUPERVISOR += \
    lib/tinyusb/src/class/cdc/cdc_device.c \
    lib/tinyusb/src/common/tusb_fifo.c \
    lib/tinyusb/src/device/usbd.c \
    lib/tinyusb/src/device/usbd_control.c \
    lib/tinyusb/src/tusb.c \
    supervisor/shared/serial.c \
    supervisor/shared/workflow.c \
    supervisor/usb.c \
    supervisor/shared/usb/usb_desc.c \
    supervisor/shared/usb/usb.c \

  ifeq ($(CIRCUITPY_USB_CDC), 1)
    SRC_SUPERVISOR += \
      shared-bindings/usb_cdc/__init__.c \
      shared-bindings/usb_cdc/Serial.c \
      shared-module/usb_cdc/__init__.c \
      shared-module/usb_cdc/Serial.c \

  endif

  ifeq ($(CIRCUITPY_USB_HID), 1)
    SRC_SUPERVISOR += \
      lib/tinyusb/src/class/hid/hid_device.c \
      shared-bindings/usb_hid/__init__.c \
      shared-bindings/usb_hid/Device.c \
      shared-module/usb_hid/__init__.c \
      shared-module/usb_hid/Device.c \

  endif

  ifeq ($(CIRCUITPY_USB_MIDI), 1)
    SRC_SUPERVISOR += \
      lib/tinyusb/src/class/midi/midi_device.c \
      shared-bindings/usb_midi/__init__.c \
      shared-bindings/usb_midi/PortIn.c \
      shared-bindings/usb_midi/PortOut.c \
      shared-module/usb_midi/__init__.c \
      shared-module/usb_midi/PortIn.c \
      shared-module/usb_midi/PortOut.c \

  endif

  ifeq ($(CIRCUITPY_USB_MSC), 1)
    SRC_SUPERVISOR += \
      lib/tinyusb/src/class/msc/msc_device.c \
      supervisor/shared/usb/usb_msc_flash.c \

  endif

  ifeq ($(CIRCUITPY_USB_VENDOR), 1)
    SRC_SUPERVISOR += \
      lib/tinyusb/src/class/vendor/vendor_device.c \

  endif
endif

SRC_TINYUSB = $(filter lib/tinyusb/%.c, $(SRC_SUPERVISOR))
$(patsubst %.c,$(BUILD)/%.o,$(SRC_TINYUSB)): CFLAGS += -Wno-missing-prototypes

SUPERVISOR_O = $(addprefix $(BUILD)/, $(SRC_SUPERVISOR:.c=.o))

ifeq ($(CIRCUITPY_DISPLAYIO), 1)
  SRC_SUPERVISOR += \
    supervisor/shared/display.c

  ifeq ($(CIRCUITPY_TERMINALIO), 1)
    SUPERVISOR_O += $(BUILD)/autogen_display_resources.o
  endif
endif

# Preserve double quotes in these values by single-quoting them.

USB_INTERFACE_NAME ?= "CircuitPython"
CFLAGS += -DUSB_INTERFACE_NAME='$(USB_INTERFACE_NAME)'

ifneq ($(USB_VID),)
CFLAGS += -DUSB_VID=$(USB_VID)
CFLAGS += -DUSB_PID=$(USB_PID)
CFLAGS += -DUSB_MANUFACTURER='$(USB_MANUFACTURER)'
USB_MANUFACTURER_8 := "$(shell echo $(USB_MANUFACTURER) | cut -c 1-8)"
# Length-limited versions of strings for MSC names.
CFLAGS += -DUSB_MANUFACTURER_8='$(USB_MANUFACTURER_8)'
USB_PRODUCT_16 := "$(shell echo $(USB_PRODUCT) | cut -c 1-16)"
CFLAGS += -DUSB_PRODUCT_16='$(USB_PRODUCT_16)'
CFLAGS += -DUSB_PRODUCT='$(USB_PRODUCT)'

endif

# In the following URL, don't include the https:// prefix.
# It gets added automatically.
USB_WEBUSB_URL ?= "circuitpython.org"

ifeq ($(CIRCUITPY_USB_CDC),1)
# Inform TinyUSB there will be up to two CDC devices.
CFLAGS += -DCFG_TUD_CDC=2
endif

USB_HIGHSPEED ?= 0
CFLAGS += -DUSB_HIGHSPEED=$(USB_HIGHSPEED)

$(BUILD)/supervisor/shared/translate.o: $(HEADER_BUILD)/qstrdefs.generated.h

CIRCUITPY_DISPLAY_FONT ?= "../../tools/fonts/ter-u12n.bdf"

$(BUILD)/autogen_display_resources.c: ../../tools/gen_display_resources.py $(HEADER_BUILD)/qstrdefs.generated.h Makefile | $(HEADER_BUILD)
	$(STEPECHO) "GEN $@"
	$(Q)install -d $(BUILD)/genhdr
	$(Q)$(PYTHON) ../../tools/gen_display_resources.py \
		--font $(CIRCUITPY_DISPLAY_FONT) \
		--sample_file $(HEADER_BUILD)/qstrdefs.generated.h \
		--output_c_file $(BUILD)/autogen_display_resources.c
