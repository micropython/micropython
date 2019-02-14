# By default this board is configured to use mboot which must be deployed first
USE_MBOOT ?= 1

MCU_SERIES = f7
CMSIS_MCU = STM32F769xx
MICROPY_FLOAT_IMPL = double
AF_FILE = boards/stm32f767_af.csv

ifeq ($(USE_MBOOT),1)
# When using Mboot all the text goes together after the filesystem
LD_FILES = boards/stm32f769.ld boards/common_blifs.ld
TEXT0_ADDR = 0x08020000
else
# When not using Mboot the ISR text goes first, then the rest after the filesystem
LD_FILES = boards/stm32f769.ld boards/common_ifs.ld
TEXT0_ADDR = 0x08000000
TEXT1_ADDR = 0x08020000
endif
