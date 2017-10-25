MCU_SERIES = f4
CMSIS_MCU = STM32F401xE
AF_FILE = boards/stm32f401_af.csv
LD_FILE = boards/stm32f401xd.ld

# Don't include default frozen modules because MCU is tight on flash space
FROZEN_MPY_DIR ?=
