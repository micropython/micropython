QEMU_ARCH = arm
QEMU_MACHINE = mps3-an547

CFLAGS += -mthumb -mcpu=cortex-m55 -mfloat-abi=hard -mfpu=fpv5-d16
CFLAGS += -DQEMU_SOC_MPS3
CFLAGS += -DMICROPY_HW_MCU_NAME='"Cortex-M55"'
CFLAGS += -DCPU_FREQ_HZ=32000000

LDSCRIPT = mcu/arm/mps3.ld

SRC_BOARD_O = shared/runtime/gchelper_generic.o

MPY_CROSS_FLAGS += -march=armv7emdp

MICROPY_FLOAT_IMPL ?= double
SUPPORTS_HARDWARE_FP_DOUBLE ?= 1
