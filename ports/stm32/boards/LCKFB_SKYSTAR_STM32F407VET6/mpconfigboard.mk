# MicroPython board configuration for LCKFB-SkyStar-STM32F407VET6
# Author: LCKFB-YZH

MCU_SERIES = f4
CMSIS_MCU = STM32F407xx
AF_FILE = boards/stm32f405_af.csv
LD_FILES = boards/LCKFB_SKYSTAR_STM32F407VET6/stm32f407vet6.ld boards/common_ifs.ld
TEXT0_ADDR = 0x08000000
TEXT1_ADDR = 0x08020000