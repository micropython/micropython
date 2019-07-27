MCU_SERIES = l0
CMSIS_MCU = STM32L072xx
MICROPY_FLOAT_IMPL = none
AF_FILE = boards/stm32l072_af.csv
LD_FILES = boards/stm32l072xz.ld boards/common_basic.ld

# Don't include default frozen modules because MCU is tight on flash space
FROZEN_MPY_DIR ?=
