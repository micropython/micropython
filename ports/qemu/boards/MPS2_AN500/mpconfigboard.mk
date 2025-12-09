QEMU_ARCH = arm
QEMU_MACHINE = mps2-an500

CFLAGS += -mthumb -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16
CFLAGS += -DQEMU_SOC_MPS2
CFLAGS += -DMICROPY_HW_MCU_NAME='"Cortex-M7"'

LDSCRIPT = mcu/arm/mps2.ld

SRC_BOARD_O = shared/runtime/gchelper_native.o shared/runtime/gchelper_thumb2.o

MPY_CROSS_FLAGS += -march=armv7emdp

MICROPY_FLOAT_IMPL = double
SUPPORTS_HARDWARE_FP_DOUBLE = 1
