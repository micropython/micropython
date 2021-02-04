# Enable/disable extra modules and features

# wiznet5k module for ethernet support; valid values are:
#   0    : no Wiznet support
#   5200 : support for W5200 module
#   5500 : support for W5500 module
MICROPY_PY_WIZNET5K ?= 0

# cc3k module for wifi support
MICROPY_PY_CC3K ?= 0

# VFS FAT FS support
MICROPY_VFS_FAT ?= 1

# Encrypted/signed bootloader support (ensure the MBOOT_PACK_xxx values match stm32/mboot/Makefile)
MBOOT_ENABLE_PACKING ?= 0
MBOOT_PACK_CHUNKSIZE ?= 16384
MBOOT_PACK_KEYS_FILE ?= $(BOARD_DIR)/mboot_keys.h
