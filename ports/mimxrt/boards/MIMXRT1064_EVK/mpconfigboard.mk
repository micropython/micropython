MCU_SERIES = MIMXRT1064
MCU_VARIANT = MIMXRT1064DVL6A

MICROPY_FLOAT_IMPL = double

SRC_C += \
	hal/flexspi_hyper_flash.c \

JLINK_PATH ?= /media/RT1064-EVK/

CFLAGS += -DBOARD_FLASH_SIZE=0x400000

deploy: $(BUILD)/firmware.bin
	cp $< $(JLINK_PATH)
