MCU_SERIES = l0
CMSIS_MCU = STM32L073xx
AF_FILE = boards/stm32l072_af.csv
LD_FILES = boards/stm32l072xz.ld boards/common_basic.ld

# MicroPython settings
MICROPY_VFS_FAT = 0

# Don't include default frozen modules because MCU is tight on flash space
FROZEN_MANIFEST ?=

# LTO reduces final binary size, may be slower to build depending on gcc version and hardware
LTO ?= 1
