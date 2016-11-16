OUTPUT_FILENAME = firmware

## Create binary .bin file from the .out file
binary:
	$(OBJCOPY) -O binary $(BUILD)/$(OUTPUT_FILENAME).elf $(BUILD)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
hex: 
	$(OBJCOPY) -O ihex $(BUILD)/$(OUTPUT_FILENAME).elf $(BUILD)/$(OUTPUT_FILENAME).hex

