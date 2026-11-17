USE_MBOOT ?= 0

# MCU settings
MCU_SERIES = u5
CMSIS_MCU = STM32U585xx
MICROPY_FLOAT_IMPL = single
AF_FILE = boards/stm32u5a5_af.csv

ifeq ($(USE_MBOOT),1)
# When using Mboot everything goes after the bootloader
# TODO: not tested
LD_FILES = boards/stm32h573xi.ld boards/common_bl.ld
TEXT0_ADDR = 0x08008000
else
# When not using Mboot everything goes at the start of flash
LD_FILES = boards/stm32u585xi.ld boards/common_basic.ld
TEXT0_ADDR = 0x08000000
endif

# MicroPython settings
#MICROPY_HW_ENABLE_ISR_UART_FLASH_FUNCS_IN_RAM = 1
