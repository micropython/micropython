QEMU_ARCH = ppc64
QEMU_MACHINE = powernv9
QEMU_ARGS +=

CFLAGS += -mno-string -mno-multiple -mno-vsx -mno-altivec -mlittle-endian
CFLAGS += -mstrict-align -mhard-float -fno-stack-protector -ffreestanding
CFLAGS += -U_FORTIFY_SOURCE -DQEMU_SOC_POWERNV
CFLAGS += -DMICROPY_HW_MCU_NAME='"powernv9"' -DMICROPY_HW_FPU=1

LDSCRIPT = mcu/ppc64/powernv9.ld

SRC_BOARD_O = shared/runtime/gchelper_generic.o

MPY_CROSS_FLAGS +=

MICROPY_FLOAT_IMPL = none
