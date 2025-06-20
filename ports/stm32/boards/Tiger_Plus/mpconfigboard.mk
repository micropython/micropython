MCU_SERIES = f7
CMSIS_MCU = STM32F746xx
AF_FILE = boards/stm32f746_af.csv 
#BOARD_PINS_FILE = boards/Tiger_Plus/pins.csv
#GEN_PINS_SRC = 1
LD_FILES = boards/Tiger_Plus/stm32f746_ext.ld boards/common_ifs.ld
TEXT0_ADDR = 0x08000000
TEXT1_ADDR = 0x08020000

# MicroPython settings
MICROPY_PY_LWIP = 0
MICROPY_PY_SSL = 0
MICROPY_SSL_MBEDTLS = 0

FROZEN_MANIFEST = $(BOARD_DIR)/manifest.py