MCU_SERIES = MIMXRT1062
MCU_VARIANT = MIMXRT1062DVJ6A

deploy: $(BUILD)/firmware.hex
	$(BOARD_LOADER) --mcu=imxrt1062 -v -w $<
	sleep 1
	
	$(MPREMOTE) connect $(PORT) run $(BOARD_DIR)/format.py

