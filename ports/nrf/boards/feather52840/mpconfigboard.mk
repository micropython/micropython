MCU_SERIES = m4
MCU_VARIANT = nrf52
MCU_SUB_VARIANT = nrf52840
SOFTDEV_VERSION ?= 6.0.0

LD_FILE = boards/feather52840/bluefruit_nrf52840_s140_$(SOFTDEV_VERSION).ld
BOOT_FILE = boards/feather52840/bootloader/feather52840_bootloader_$(SOFTDEV_VERSION)_s140_single

NRF_DEFINES += -DNRF52840_XXAA

ifeq ($(OS),Windows_NT)
   NRFUTIL = ../../lib/nrfutil/binaries/win32/nrfutil.exe
else
   NRFUTIL = nrfutil
endif

CFLAGS += -DADAFRUIT_FEATHER52840

check_defined = \
    $(strip $(foreach 1,$1, \
    $(call __check_defined,$1,$(strip $(value 2)))))
__check_defined = \
    $(if $(value $1),, \
    $(error Undefined make flag: $1$(if $2, ($2))))

.PHONY: dfu-gen dfu-flash dfu-bootloader uf2 bootloader


uf2:
	$(ECHO) "Create $(OUTPUT_FILENAME).uf2"
	$(PYTHON2) $(TOP)/tools/uf2/utils/uf2conv.py -c -o "$(BUILD)/$(OUTPUT_FILENAME).uf2" "$(BUILD)/$(OUTPUT_FILENAME).hex"

# Flash with DFU 
dfu-gen:
	$(NRFUTIL) dfu genpkg --sd-req 0xFFFE --dev-type 0x0052 --application $(BUILD)/$(OUTPUT_FILENAME).hex $(BUILD)/dfu-package.zip

dfu-flash:
	@:$(call check_defined, SERIAL, example: SERIAL=/dev/ttyUSB0)
	$(NRFUTIL) --verbose dfu serial --package $(BUILD)/dfu-package.zip -p $(SERIAL) -b 115200 --singlebank

dfu-bootloader:
	@:$(call check_defined, SERIAL, example: SERIAL=/dev/ttyACM0)
	$(NRFUTIL) --verbose dfu serial --package $(BOOT_FILE).zip -p $(SERIAL) -b 115200

# Flash with jlink	
bootloader:
	nrfjprog --program $(BOOT_FILE).hex -f nrf52 --chiperase --reset
