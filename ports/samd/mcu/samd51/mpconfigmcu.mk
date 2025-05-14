CFLAGS_MCU += -mtune=cortex-m4 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard

CFLAGS_MCU += -DCFG_TUSB_MCU=OPT_MCU_SAMD51

MPY_CROSS_MCU_ARCH = armv7m

MICROPY_HW_CODESIZE ?= 368K

MICROPY_VFS_LFS2 ?= 1
MICROPY_VFS_FAT ?= 1
FROZEN_MANIFEST ?= mcu/$(MCU_SERIES_LOWER)/manifest.py

SRC_S += shared/runtime/gchelper_thumb2.s

SRC_C += \
	fatfs_port.c \

UF2CONV_FLAGS += -f 0x55114460
