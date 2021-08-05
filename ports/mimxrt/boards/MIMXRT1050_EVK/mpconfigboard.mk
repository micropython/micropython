MCU_SERIES = MIMXRT1052
MCU_VARIANT = MIMXRT1052DVL6B

MICROPY_FLOAT_IMPL = double

SRC_C += \
	hal/flexspi_nor_flash.c \

JLINK_PATH ?= /media/RT1050-EVK/

deploy: $(BUILD)/firmware.bin
	cp $< $(JLINK_PATH)
