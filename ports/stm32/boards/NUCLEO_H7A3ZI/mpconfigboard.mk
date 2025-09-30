USE_MBOOT ?= 0

# MCU settings
MCU_SERIES = h7
CMSIS_MCU = STM32H7A3xxQ
MICROPY_FLOAT_IMPL = double
AF_FILE = boards/stm32h7a3_af.csv

ifeq ($(USE_MBOOT),1)
# When using Mboot all the text goes together after the filesystem
LD_FILES = boards/stm32h7a3.ld boards/common_blifs.ld
TEXT0_ADDR = 0x08040000
else
# When not using Mboot the ISR text goes first, then the rest after the filesystem
LD_FILES = boards/stm32h7a3.ld boards/common_ifs.ld
TEXT0_ADDR = 0x08000000
TEXT1_ADDR = 0x08040000
endif
