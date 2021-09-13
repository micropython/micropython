MCU_SERIES = MIMXRT1062
MCU_VARIANT = MIMXRT1062DVJ6A

MICROPY_FLOAT_IMPL = double
MICROPY_PY_MACHINE_SDCARD = 1

deploy: $(BUILD)/firmware.hex
	teensy_loader_cli --mcu=imxrt1062 -v -w $<

SRC_C += \
	hal/flexspi_nor_flash.c
