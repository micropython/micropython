MCU_SERIES = m4
MCU_VARIANT = nrf52
MCU_SUB_VARIANT = nrf52832
SOFTDEV_VERSION = 2.0.1

LD_FILE = boards/feather52/custom_nrf52832_dfu_app.ld

NRF_DEFINES += -DNRF52832_XXAA

.PHONY: dfu-gen dfu-flash

dfu-gen:
	nrfutil dfu genpkg --dev-type 0x0052 --application $(BUILD)/$(OUTPUT_FILENAME).hex $(BUILD)/dfu-package.zip

dfu-flash:
	sudo nrfutil dfu serial --package $(BUILD)/dfu-package.zip -p /dev/ttyACM3
