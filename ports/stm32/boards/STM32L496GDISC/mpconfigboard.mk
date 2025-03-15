MCU_SERIES = l4
CMSIS_MCU = STM32L496xx
AF_FILE = boards/stm32l496_af.csv
LD_FILES = boards/stm32l496xg.ld boards/common_basic.ld
OPENOCD_CONFIG = boards/openocd_stm32l4.cfg

MICROPY_HW_ENABLE_ISR_UART_FLASH_FUNCS_IN_RAM = 1
