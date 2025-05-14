CMSIS_MCU = RA4M1
MCU_SERIES = m4
LD_FILES = boards/EK_RA4M1/ra4m1_ek.ld

# MicroPython settings
MICROPY_VFS_FAT = 1
USE_FSP_LPM = 0

CFLAGS+=-DDEFAULT_DBG_CH=0

# Don't include default frozen modules because MCU is tight on flash space
FROZEN_MANIFEST ?= boards/EK_RA4M1/manifest.py
