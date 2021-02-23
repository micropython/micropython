MCU_SERIES = f3
CMSIS_MCU = STM32F303K8
AF_FILE = boards/stm32f303k8_af.csv
LD_FILES = boards/stm32f303k8.ld boards/common_basic.ld

# MicroPython settings
MICROPY_VFS_FAT = 0

# Don't include default frozen modules because MCU is tight on flash space
FROZEN_MANIFEST ?=
