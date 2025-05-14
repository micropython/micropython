MCU_SERIES = wl
CMSIS_MCU = STM32WL55xx
AF_FILE = boards/stm32wl55_af.csv
STARTUP_FILE = $(STM32LIB_CMSIS_BASE)/Source/Templates/gcc/startup_stm32wl55xx_cm4.o
LD_FILES = boards/stm32wl55xc.ld boards/common_basic.ld
TEXT0_ADDR = 0x08000000

# MicroPython settings
MICROPY_VFS_FAT = 0
MICROPY_VFS_LFS2 = 1
MICROPY_HW_ENABLE_ISR_UART_FLASH_FUNCS_IN_RAM = 1

# Board-specific manifest (doesn't include default modules, adds LoRa driver).
FROZEN_MANIFEST ?= $(BOARD_DIR)/manifest.py
