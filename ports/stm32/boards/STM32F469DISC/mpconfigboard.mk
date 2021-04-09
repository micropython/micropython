# MCU settings
MCU_SERIES = f4
CMSIS_MCU = STM32F469xx
MICROPY_FLOAT_IMPL = double
AF_FILE = boards/stm32f469_af.csv

ifeq ($(USE_MBOOT),1)
# When using Mboot all the text goes together after the filesystem
LD_FILES = boards/stm32f469xi.ld boards/common_blifs.ld
TEXT0_ADDR = 0x08020000
else ifeq ($(USE_DBOOT),1)
# When using dual Bootloader all the text goes together after the filesystem
LD_FILES = boards/stm32f469xi_dboot.ld boards/common_blifs.ld
TEXT0_ADDR = 0x08020000
else
# When not using Mboot the ISR text goes first, then the rest after the filesystem
LD_FILES = boards/stm32f469xi.ld boards/common_ifs.ld
TEXT0_ADDR = 0x08000000
TEXT1_ADDR = 0x08008000
endif

# MicroPython settings
MICROPY_PY_USSL = 1
MICROPY_SSL_MBEDTLS = 1
