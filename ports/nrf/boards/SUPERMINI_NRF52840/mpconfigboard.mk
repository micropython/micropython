MCU_SERIES = m4
MCU_VARIANT = nrf52
MCU_SUB_VARIANT = nrf52840
SOFTDEV_VERSION = 6.1.1
SD=s140
LD_FILES += $(BOARD_DIR)/bootloader.ld boards/nrf52840_1M_256k.ld

NRF_DEFINES += -DNRF52840_XXAA

MICROPY_VFS_LFS2 = 1
FS_SIZE = 256k

# DEBUG ?= 1

FROZEN_MANIFEST ?= $(BOARD_DIR)/manifest.py

uf2: hex
	python3 $(TOP)/tools/uf2conv.py -c -o $(BUILD)/firmware.uf2 -f 0xADA52840 $(BUILD)/firmware.hex
