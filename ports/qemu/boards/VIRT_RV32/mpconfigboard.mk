QEMU_ARCH = riscv32
QEMU_MACHINE = virt

CFLAGS += -DQEMU_SOC_VIRT
CFLAGS += -DMICROPY_HW_MCU_NAME='"$(RV32_ARCH)"'

LDSCRIPT = mcu/rv32/virt.ld

SRC_BOARD_O += shared/runtime/gchelper_native.o shared/runtime/gchelper_rv32i.o

MPY_CROSS_FLAGS += -march=rv32imc -march-flags=zba
