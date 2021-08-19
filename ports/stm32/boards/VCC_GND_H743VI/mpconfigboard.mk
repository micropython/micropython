USE_MBOOT ?= 0

# MCU settings
MCU_SERIES = h7
CMSIS_MCU = STM32H743xx
MICROPY_FLOAT_IMPL = double
AF_FILE = boards/stm32h743_af.csv

ifeq ($(USE_MBOOT),1)
# When using Mboot everything goes after the bootloader
LD_FILES = boards/stm32h743.ld boards/common_bl.ld
TEXT0_ADDR = 0x08020000
else
# When not using Mboot everything goes at the start of flash
LD_FILES = boards/stm32h743.ld boards/common_basic.ld
TEXT0_ADDR = 0x08000000
endif

# MicroPython settings
MICROPY_VFS_LFS2 = 1
