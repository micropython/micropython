MCU_SERIES = l0
CMSIS_MCU = STM32L073xx
AF_FILE = boards/stm32l072_af.csv
LD_FILES = boards/stm32l072xz.ld boards/common_basic.ld

# MicroPython settings
MICROPY_VFS_FAT = 0
MICROPY_SMALL_DATA_SECTION = 1
MP_SMALL_DATA_CONST_NONE = 0xc0
MP_SMALL_DATA_CONST_FALSE = 0xc4
MP_SMALL_DATA_CONST_TRUE = 0xcc
MP_SMALL_DATA_TYPE_TYPE = 0xd4

# Don't include default frozen modules because MCU is tight on flash space
FROZEN_MANIFEST ?=
