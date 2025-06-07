CFLAGS_MCU += -mtune=cortex-m0plus -mcpu=cortex-m0plus -msoft-float

CFLAGS_MCU += -DCFG_TUSB_MCU=OPT_MCU_SAMD21

MPY_CROSS_MCU_ARCH = armv6m

MICROPY_HW_CODESIZE ?= 191K

ifeq ($(MICROPY_HW_CODESIZE), 232K)
FROZEN_MANIFEST ?= mcu/$(MCU_SERIES_LOWER)/manifest.py
MICROPY_HW_VFSROMSIZE ?= 16K
else
MICROPY_HW_VFSROMSIZE ?= 15K
endif

MICROPY_VFS_LFS1 ?= 1

SRC_S += shared/runtime/gchelper_thumb1.s

UF2CONV_FLAGS += -f 0x68ed2b88
