QEMU_ARCH = aarch64
QEMU_MACHINE = virt

CFLAGS += -DQEMU_SOC_VIRT_AARCH64
CFLAGS += -DMICROPY_HW_MCU_NAME='"qemu-aarch64-virt"'
CFLAGS += -march=armv8-a -mstrict-align

LDSCRIPT = mcu/aarch64/virt.ld

SRC_BOARD_O += shared/runtime/gchelper_native.o shared/runtime/gchelper_aarch64.o

MICROPY_HEAP_SIZE ?= 1048576
CFLAGS += -DMICROPY_GC_ALLOC_THRESHOLD=0

MICROPY_FLOAT_IMPL = double
SUPPORTS_HARDWARE_FP_DOUBLE = 1
