# Enable/disable extra modules and features

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1
MICROPY_FLOAT_IMPL ?= double

# VFS support.
MICROPY_VFS_FAT ?= 1
MICROPY_VFS_LFS2 ?= 1

# File containing description of content to be frozen into firmware.
FROZEN_MANIFEST ?= boards/manifest.py
MICROPY_MANIFEST_MCU_CORE := $(shell echo $(MCU_CORE) | awk -F'_' '{print tolower($$2)}')
