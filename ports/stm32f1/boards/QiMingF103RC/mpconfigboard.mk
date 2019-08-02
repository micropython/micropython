MCU_SERIES = f1
CMSIS_MCU = STM32F103xE
MICROPY_FLOAT_IMPL = signle
STARTUP_FILE = boards/startup_stm32f103xe.o
AF_FILE = boards/stm32f103rx_af.csv
LD_FILES = boards/stm32f103xc.ld boards/common_basic.ld

# Don't include default frozen modules because MCU is tight on flash space
FROZEN_MPY_DIR ?=
