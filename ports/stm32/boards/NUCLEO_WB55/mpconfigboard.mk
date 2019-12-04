MCU_SERIES = wb
CMSIS_MCU = STM32WB55xx
AF_FILE = boards/stm32wb55_af.csv
LD_FILES = boards/stm32wb55xg.ld boards/common_basic.ld
STARTUP_FILE = lib/stm32lib/CMSIS/STM32WBxx/Source/Templates/gcc/startup_stm32wb55xx_cm4.o

# MicroPython settings
MICROPY_PY_BLUETOOTH = 1
MICROPY_BLUETOOTH_NIMBLE = 1
MICROPY_VFS_LFS2 = 1
