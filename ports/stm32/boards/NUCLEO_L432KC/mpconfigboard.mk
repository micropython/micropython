MCU_SERIES = l4
CMSIS_MCU = STM32L432xx
AF_FILE = boards/stm32l432_af.csv
LD_FILES = boards/stm32l432.ld boards/common_basic.ld
OPENOCD_CONFIG = boards/openocd_stm32l4.cfg

# MicroPython settings
MICROPY_VFS_FAT = 0
MICROPY_VFS_LFS1 ?= 1
MICROPY_HW_ENABLE_ISR_UART_FLASH_FUNCS_IN_RAM = 1

# Don't include default frozen modules because MCU is tight on flash space
FROZEN_MANIFEST ?=

# LTO reduces final binary size, may be slower to build depending on gcc version and hardware
LTO ?= 1
