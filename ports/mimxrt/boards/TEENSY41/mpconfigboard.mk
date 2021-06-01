MCU_SERIES = MIMXRT1062
MCU_VARIANT = MIMXRT1062DVJ6A

deploy: $(BUILD)/firmware.hex
	teensy_loader_cli --mcu=imxrt1062 -v -w $<
