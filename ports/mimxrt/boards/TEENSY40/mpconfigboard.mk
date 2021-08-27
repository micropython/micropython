MCU_SERIES = MIMXRT1062
MCU_VARIANT = MIMXRT1062DVJ6A

MICROPY_FLOAT_IMPL = double

SRC_C += \
	hal/flexspi_nor_flash.c \

deploy: $(BUILD)/firmware.hex
	teensy_loader_cli --mcu=imxrt1062 -v -w $<
