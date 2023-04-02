MCU_SERIES = g0
CMSIS_MCU = STM32G0B1xx
AF_FILE = boards/stm32g0b1_af.csv

ifeq ($(USE_MBOOT),1)
LD_FILES = boards/stm32g0b1xe.ld boards/common_bl.ld
TEXT0_ADDR = 0x08008000
else
LD_FILES = boards/stm32g0b1xe.ld boards/common_basic.ld
endif

# LTO reduces final binary size, may be slower to build depending on gcc version and hardware
LTO ?= 1
