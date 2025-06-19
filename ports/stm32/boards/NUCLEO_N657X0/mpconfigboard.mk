MCU_SERIES = n6
CMSIS_MCU = STM32N657xx
AF_FILE = boards/stm32n657_af.csv
SYSTEM_FILE = $(STM32LIB_CMSIS_BASE)/Source/Templates/system_stm32$(MCU_SERIES)xx_fsbl.o
STM32_N6_HEADER_VERSION = 2.1
DKEL = $(CUBE_PROG_BASE)/bin/ExternalLoader/MX25UM51245G_STM32N6570-NUCLEO.stldr

ifeq ($(USE_MBOOT),1)
LD_FILES = boards/stm32n657x0.ld boards/common_bl.ld
TEXT0_ADDR = 0x08008000
else
LD_FILES = boards/stm32n657x0.ld boards/common_basic.ld
TEXT0_ADDR = 0x34180400
endif
