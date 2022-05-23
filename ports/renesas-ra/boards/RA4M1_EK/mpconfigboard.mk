CMSIS_MCU = RA4M1
MCU_SERIES = m4
LD_FILES = boards/RA4M1_EK/ra4m1_ek.ld

# MicroPython settings
MICROPY_VFS_FAT = 1

# Don't include default frozen modules because MCU is tight on flash space
FROZEN_MANIFEST ?= boards/RA4M1_EK/manifest.py
