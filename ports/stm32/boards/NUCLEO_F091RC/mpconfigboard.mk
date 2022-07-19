MCU_SERIES = f0
CMSIS_MCU = STM32F091xC
AF_FILE = boards/stm32f091_af.csv
LD_FILES = boards/stm32f091xc.ld boards/common_basic.ld

# MicroPython settings
MICROPY_VFS_FAT = 0
MICROPY_VFS_LFS1 ?= 1

# Don't include default frozen modules because MCU is tight on flash space
FROZEN_MANIFEST ?=

# LTO reduces final binary size, may be slower to build depending on gcc version and hardware
LTO ?= 1
