MCU_SERIES = m4
MCU_VARIANT = nrf52
MCU_SUB_VARIANT = nrf52840
SOFTDEV_VERSION ?= 6.0.0-6.alpha

LD_FILE = boards/feather52840/bluefruit_nrf52840_s140_6.0.0.ld
BOOTLOADER_FILENAME = boards/feather52840/bootloader/feather52840_bootloader_6.0.0_s140_single

NRF_DEFINES += -DNRF52840_XXAA

ifeq ($(OS),Windows_NT)
   NRFUTIL = ../../lib/nrfutil/binaries/win32/nrfutil.exe
else
   NRFUTIL = nrfutil
endif

CFLAGS += -DADAFRUIT_FEATHER52840

ifeq ($(SD), )
INC += -Idrivers/bluetooth/s140_$(MCU_VARIANT)_$(SOFTDEV_VERSION)/s140_$(MCU_SUB_VARIANT)_$(SOFTDEV_VERSION)_API/include
INC += -Idrivers/bluetooth/s140_$(MCU_VARIANT)_$(SOFTDEV_VERSION)/s140_$(MCU_SUB_VARIANT)_$(SOFTDEV_VERSION)_API/include/$(MCU_VARIANT)
endif

check_defined = \
    $(strip $(foreach 1,$1, \
    $(call __check_defined,$1,$(strip $(value 2)))))
__check_defined = \
    $(if $(value $1),, \
    $(error Undefined make flag: $1$(if $2, ($2))))

.PHONY: dfu-gen dfu-flash boot-flash

dfu-gen:
	$(NRFUTIL) dfu genpkg --sd-req 0xFFFE --dev-type 0x0052 --application $(BUILD)/$(OUTPUT_FILENAME).hex $(BUILD)/dfu-package.zip

dfu-flash:
	@:$(call check_defined, SERIAL, example: SERIAL=/dev/ttyUSB0)
	$(NRFUTIL) --verbose dfu serial --package $(BUILD)/dfu-package.zip -p $(SERIAL) -b 115200

boot-flash:
	nrfjprog --program $(BOOTLOADER_FILENAME).hex -f nrf52 --chiperase --reset
