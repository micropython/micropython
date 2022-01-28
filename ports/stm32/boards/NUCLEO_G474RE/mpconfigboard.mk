USE_MBOOT ?= 0

DEBUG = 1

# MCU settings
MCU_SERIES = g4
CMSIS_MCU = STM32G474xx
MICROPY_FLOAT_IMPL = single
AF_FILE = boards/stm32g474_af.csv

ifeq ($(USE_MBOOT),1)
# When using Mboot all the text goes together after the filesystem
LD_FILES = boards/stm32g474.ld boards/common_blifs.ld
TEXT0_ADDR = 0x08030000
else
# When not using Mboot the ISR text goes first, then the rest after the filesystem
LD_FILES = boards/stm32g474.ld boards/common_ifs.ld
TEXT0_ADDR = 0x08000000
TEXT1_ADDR = 0x08030000
endif
