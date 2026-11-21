CMSIS_MCU = RA4M1
MCU_SERIES = m4
LD_FILES = boards/WEACT_RA4M1_CORE/ra4m1_ek.ld

# MicroPython settings
MICROPY_VFS_LFS2 = 1
MICROPY_VFS_FAT = 0
USE_FSP_LPM = 0

CFLAGS+=-DDEFAULT_DBG_CH=0 \
          -DCFG_TUD_MAX_SPEED=OPT_MODE_FULL_SPEED \
          -DCFG_TUSB_RHPORT0_MODE=OPT_MODE_DEVICE \
          -DBOARD_TUD_MAX_SPEED=OPT_MODE_FULL_SPEED \
          -DCFG_TUSB_MCU=OPT_MCU_RAXXX \
          -DBOARD_TUD_RHPORT=0

# Don't include default frozen modules because MCU is tight on flash space
FROZEN_MANIFEST ?= boards/WEACT_RA4M1_CORE/manifest.py
