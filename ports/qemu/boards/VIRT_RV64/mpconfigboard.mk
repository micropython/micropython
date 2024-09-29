QEMU_ARCH = riscv64
QEMU_MACHINE = virt

CFLAGS += -DQEMU_SOC_VIRT
CFLAGS += -DMICROPY_HW_MCU_NAME='"$(RV64_ARCH)"'

LDSCRIPT = mcu/rv64/virt.ld

SRC_BOARD_O += shared/runtime/gchelper_native.o shared/runtime/gchelper_rv64i.o

MPY_CROSS_FLAGS += -march=rv64imc
