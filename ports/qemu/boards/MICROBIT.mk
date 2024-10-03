QEMU_ARCH = arm
QEMU_MACHINE = microbit
QEMU_ARGS += -global nrf51-soc.flash-size=1048576 -global nrf51-soc.sram-size=262144

CFLAGS += -mthumb -mcpu=cortex-m0 -mfloat-abi=soft
CFLAGS += -DQEMU_SOC_NRF51
CFLAGS += -DMICROPY_HW_MCU_NAME='"nRF51"'

LDSCRIPT = mcu/arm/nrf51.ld

SRC_BOARD_O = shared/runtime/gchelper_native.o shared/runtime/gchelper_thumb1.o

MPY_CROSS_FLAGS += -march=armv7m

# These RV32 tests don't run on Thumb, so exclude them.
RUN_TESTS_ARGS = --exclude 'inlineasm/rv32imc'
