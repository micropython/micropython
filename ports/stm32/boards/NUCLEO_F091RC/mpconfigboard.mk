MCU_SERIES = f0
CMSIS_MCU = STM32F091xC
AF_FILE = boards/stm32f091_af.csv
LD_FILES = boards/stm32f091xc.ld boards/common_basic.ld

# Don't include default frozen modules because MCU is tight on flash space
FROZEN_MPY_DIR ?=
