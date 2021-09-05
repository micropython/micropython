MCU_SERIES = MIMXRT1052
MCU_VARIANT = MIMXRT1052DVL6B

MICROPY_FLOAT_IMPL = double

SRC_C += \
	hal/flexspi_hyper_flash.c \

JLINK_PATH ?= /media/RT1050-EVKB/

deploy: $(BUILD)/firmware.bin
	cp $< $(JLINK_PATH)
