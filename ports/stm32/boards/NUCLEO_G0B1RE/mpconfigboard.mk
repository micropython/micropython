MCU_SERIES = g0
CMSIS_MCU = STM32G0B1xx
AF_FILE = boards/stm32g0b1_af.csv
LD_FILES = boards/stm32g0b1xe.ld boards/common_basic.ld

# LTO reduces final binary size, may be slower to build depending on gcc version and hardware
LTO ?= 1
