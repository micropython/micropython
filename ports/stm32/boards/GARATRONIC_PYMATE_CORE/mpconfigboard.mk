# MCU settings
MCU_SERIES = f4
CMSIS_MCU = STM32F412Rx
MICROPY_FLOAT_IMPL = single
AF_FILE = boards/stm32f412_af.csv
LD_FILES = boards/GARATRONIC_PYMATE_CORE/f412_qspi.ld boards/common_bl.ld
TEXT0_ADDR = 0x08000000
TEXT0_SECTIONS = .isr_vector .text .data .ARM

# MicroPython settings
MICROPY_VFS_LFS2 = 1

#I2C4 needs fmpi2c
SRC_HAL += $(STM32LIB_HAL_BASE)/Src/stm32f4xx_hal_fmpi2c.c
