# MCU settings
MCU_SERIES = u5
CMSIS_MCU = STM32U5A5xx
MICROPY_FLOAT_IMPL = single
AF_FILE = boards/stm32u5a5_af.csv

LD_FILES = boards/stm32u5a5xj.ld boards/common_basic.ld
TEXT0_ADDR = 0x08000000

# MicroPython settings
MICROPY_HW_ENABLE_ISR_UART_FLASH_FUNCS_IN_RAM = 1
