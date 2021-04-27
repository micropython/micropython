SRC_SUPERVISOR = \
	main.c \
	supervisor/port.c \
	supervisor/shared/autoreload.c \
	supervisor/shared/background_callback.c \
	supervisor/shared/board.c \
	supervisor/shared/cpu.c \
	supervisor/shared/filesystem.c \
	supervisor/shared/flash.c \
	supervisor/shared/micropython.c \
	supervisor/shared/rgb_led_status.c \
	supervisor/shared/safe_mode.c \
	supervisor/shared/stack.c \
	supervisor/shared/status_leds.c \
	supervisor/shared/tick.c \
	supervisor/shared/translate.c

NO_USB ?= $(wildcard supervisor/usb.c)

INTERNAL_FLASH_FILESYSTEM ?= 0
CFLAGS += -DINTERNAL_FLASH_FILESYSTEM=$(INTERNAL_FLASH_FILESYSTEM)

QSPI_FLASH_FILESYSTEM ?= 0
CFLAGS += -DQSPI_FLASH_FILESYSTEM=$(QSPI_FLASH_FILESYSTEM)

SPI_FLASH_FILESYSTEM ?= 0
CFLAGS += -DSPI_FLASH_FILESYSTEM=$(SPI_FLASH_FILESYSTEM)

ifeq ($(CIRCUITPY_BLEIO),1)
	SRC_SUPERVISOR += supervisor/shared/bluetooth.c supervisor/bluetooth.c
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
	$(Q)$(PYTHON3) ../../tools/gen_nvm_devices.py $< $@

$(BUILD)/supervisor/shared/external_flash/external_flash.o: $(HEADER_BUILD)/devices.h

endif

ifeq ($(CIRCUITPY_USB),0)
  ifeq ($(wildcard supervisor/serial.c),)
    SRC_SUPERVISOR += supervisor/stub/serial.c
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

SUPERVISOR_O = $(addprefix $(BUILD)/, $(SRC_SUPERVISOR:.c=.o))

ifeq ($(CIRCUITPY_DISPLAYIO), 1)
  SRC_SUPERVISOR += \
    supervisor/shared/display.c

  ifeq ($(CIRCUITPY_TERMINALIO), 1)
    SUPERVISOR_O += $(BUILD)/autogen_display_resources.o
  endif
endif

USB_INTERFACE_NAME ?= "CircuitPython"
CFLAGS += -DUSB_INTERFACE_NAME=$(USB_INTERFACE_NAME)

ifneq ($(USB_VID),)
CFLAGS += -DUSB_VID=$(USB_VID)
CFLAGS += -DSUB_PID=$(USB_PID)
CFLAGS += -DUSB_MANUFACTURER=$(USB_MANUFACTURER)
CFLAGS += -DUSB_PRODUCT=$(USB_PRODUCT)
endif

# In the following URL, don't include the https:// prefix.
# It gets added automatically.
USB_WEBUSB_URL ?= "circuitpython.org"

ifeq ($(CIRCUITPY_USB_CDC),1)
# Inform TinyUSB there are two CDC devices.
CFLAGS += -DCFG_TUD_CDC=2
endif

USB_HIGHSPEED ?= 0

************ move these from make variables to cpp only ???
USB_CDC_EP_NUM_NOTIFICATION ?= 0
CFLAGS += -DUSB_CDC_EP_NUM_NOTIFICATION=$(USB_CDC_EP_NUM_NOTIFICATION)
USB_CDC_EP_NUM_DATA_OUT ?= 0
CFLAGS += -DUSB_CDC_EP_NUM_DATA_OUT=$(USB_CDC_EP_NUM_DATA_OUT)
USB_CDC_EP_NUM_DATA_IN ?= 0
CFLAGS += -DUSB_CDC_EP_NUM_DATA_IN=$(USB_CDC_EP_NUM_DATA_IN)

USB_CDC2_EP_NUM_NOTIFICATION ?= 0
CFLAGS += -DUSB_CDC2_EP_NUM_NOTIFICATION=$(USB_CDC2_EP_NUM_NOTIFICATION)
USB_CDC2_EP_NUM_DATA_OUT ?= 0
CFLAGS += -DUSB_CDC2_EP_NUM_DATA_OUT=$(USB_CDC2_EP_NUM_DATA_OUT)
USB_CDC2_EP_NUM_DATA_IN ?= 0
CFLAGS += -DUSB_CDC2_EP_NUM_DATA_IN=$(USB_CDC2_EP_NUM_DATA_IN)

USB_MSC_EP_NUM_OUT ?= 0
CFLAGS += -DUSB
USB_MSC_EP_NUM_IN ?= 0

USB_HID_EP_NUM_OUT ?= 0
USB_HID_EP_NUM_IN ?= 0

USB_MIDI_EP_NUM_OUT ?= 0
USB_MIDI_EP_NUM_IN ?= 0

USB_NUM_EP ?= 0

$(BUILD)/supervisor/shared/translate.o: $(HEADER_BUILD)/qstrdefs.generated.h

CIRCUITPY_DISPLAY_FONT ?= "../../tools/fonts/ter-u12n.bdf"

$(BUILD)/autogen_display_resources.c: ../../tools/gen_display_resources.py $(HEADER_BUILD)/qstrdefs.generated.h Makefile | $(HEADER_BUILD)
	$(STEPECHO) "GEN $@"
	$(Q)install -d $(BUILD)/genhdr
	$(Q)$(PYTHON3) ../../tools/gen_display_resources.py \
		--font $(CIRCUITPY_DISPLAY_FONT) \
		--sample_file $(HEADER_BUILD)/qstrdefs.generated.h \
		--output_c_file $(BUILD)/autogen_display_resources.c
