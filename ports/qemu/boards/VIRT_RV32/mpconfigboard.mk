QEMU_ARCH = riscv32
QEMU_MACHINE = virt

CFLAGS += -DQEMU_SOC_VIRT
CFLAGS += -DMICROPY_HW_MCU_NAME='"$(RV32_ARCH)"'

LDSCRIPT = mcu/rv32/virt.ld

# If the ROMFS partition size is not enough, remember to change
# ports/qemu/mcu/rv32/virt.ld to resize the ROMFS section as well.

MICROPY_HW_ROMFS_PART0_START = 0x80610000
MICROPY_HW_ROMFS_PART0_SIZE = 0x00400000

SRC_BOARD_O += shared/runtime/gchelper_native.o shared/runtime/gchelper_rv32i.o

MPY_CROSS_FLAGS += -march=rv32imc -march-flags=zba
