MCU_SERIES = f1
CMSIS_MCU = STM32F103xE
MICROPY_FLOAT_IMPL = signle
STARTUP_FILE = boards/startup_stm32f103xe.o
AF_FILE = boards/stm32f103_af.csv
LD_FILES = boards/QiMing_F103RC/stm32f103rc.ld boards/common_ifs.ld
TEXT0_ADDR = 0x08000000
TEXT1_ADDR = 0x08020000

# Don't include default frozen modules because MCU is tight on flash space
FROZEN_MPY_DIR ?=
