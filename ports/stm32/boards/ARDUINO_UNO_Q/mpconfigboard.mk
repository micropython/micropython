# MCU settings
MCU_SERIES = u5
CMSIS_MCU = STM32U585xx
MICROPY_FLOAT_IMPL = single
AF_FILE = boards/stm32u5a5_af.csv

# No bootloader; the image runs from the start of flash.
LD_FILES = boards/stm32u585xi.ld boards/common_basic.ld
TEXT0_ADDR = 0x08000000
