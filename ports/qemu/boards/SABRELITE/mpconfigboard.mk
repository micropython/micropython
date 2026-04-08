QEMU_ARCH = arm
QEMU_MACHINE = sabrelite
QEMU_ARGS += -m 128M

CFLAGS += -mcpu=cortex-a9
CFLAGS += -DQEMU_SOC_IMX6
CFLAGS += -DMICROPY_HW_MCU_NAME='"Cortex-A9"'
# Cortex-A9 should support unaligned-access, but qemu doesn't seem to.
CFLAGS += -mno-unaligned-access

# If the ROMFS partition size is not enough, remember to change
# ports/qemu/mcu/arm/imx6.ld to resize the ROMFS section as well.

MICROPY_HW_ROMFS_PART0_START = 0x17C00000
MICROPY_HW_ROMFS_PART0_SIZE = 0x00400000

LDSCRIPT = mcu/arm/imx6.ld

SRC_BOARD_O = shared/runtime/gchelper_generic.o

# Use a larger heap than the default so tests run with the native emitter have
# enough memory (because emitted ARM machine code is larger than Thumb2 code).
MICROPY_HEAP_SIZE ?= 163840

# It's really armv7a but closest supported value is armv6.
MPY_CROSS_FLAGS += -march=armv6

# These tests don't work on Cortex-A9, so exclude them.
RUN_TESTS_ARGS += --exclude 'inlineasm/thumb/(asmbcc|asmbitops|asmconst|asmdiv|asmit|asmspecialregs).py'
