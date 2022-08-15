# This is the default variant when you `make` the Unix port.

FROZEN_MANIFEST ?= $(VARIANT_DIR)/manifest.py

MICROPY_ROM_TEXT_COMPRESSION = 1
MICROPY_VFS_FAT ?= 1
MICROPY_VFS_LFS1 ?= 1
MICROPY_VFS_LFS2 ?= 1
