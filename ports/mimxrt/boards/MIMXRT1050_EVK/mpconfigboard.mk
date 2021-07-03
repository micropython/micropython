MCU_SERIES = MIMXRT1052
MCU_VARIANT = MIMXRT1052DVL6B

MICROPY_FLOAT_IMPL = double
MICROPY_PY_MACHINE_SDCARD = 1
MICROPY_HW_FLASH_TYPE ?= hyperflash
MICROPY_HW_FLASH_SIZE ?= 0x4000000  # 64MB

MICROPY_HW_SDRAM_AVAIL = 1
MICROPY_HW_SDRAM_SIZE  = 0x2000000  # 32MB

MICROPY_PY_LWIP = 1
MICROPY_PY_USSL = 1
MICROPY_SSL_MBEDTLS = 1

ENET_PHY = KSZ8081
	
JLINK_PATH ?= /media/RT1050-EVK/

deploy: $(BUILD)/firmware.bin
	cp $< $(JLINK_PATH)
