# MCU settings
MCU_SERIES = c5
CMSIS_MCU = STM32C5A3xx
MICROPY_FLOAT_IMPL = single
AF_FILE = boards/stm32c5a3_af.csv

# C5 startup files are .c in Source/ (not .s in Source/Templates/gcc/)
STARTUP_FILE = $(STM32LIB_CMSIS_BASE)/Source/startup_$(CMSIS_MCU_LOWER).o

LD_FILES = boards/stm32c5a3xg.ld boards/common_basic.ld
TEXT0_ADDR = 0x08000000
