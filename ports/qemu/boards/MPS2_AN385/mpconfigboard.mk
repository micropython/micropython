QEMU_ARCH = arm
QEMU_MACHINE = mps2-an385

CFLAGS += -mthumb -mcpu=cortex-m3 -mfloat-abi=soft
CFLAGS += -DQEMU_SOC_MPS2
CFLAGS += -DMICROPY_HW_MCU_NAME='"Cortex-M3"'

MICROPY_HW_ROMFS_PART0_START = 0x21000000
MICROPY_HW_ROMFS_PART0_SIZE = 0x00400000

LDSCRIPT = mcu/arm/mps2.ld

SRC_BOARD_O = shared/runtime/gchelper_native.o shared/runtime/gchelper_thumb2.o

MPY_CROSS_FLAGS += -march=armv7m
