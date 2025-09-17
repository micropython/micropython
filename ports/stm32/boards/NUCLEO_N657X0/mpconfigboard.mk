# Without mboot, the main firmware must fit in 512k flash, will be copied to SRAM by
# the hardware bootloader, and will run from SRAM.  With mboot, the main firmware can
# be much larger and will run from flash via XSPI in memory-mapped mode.
USE_MBOOT ?= 1

MCU_SERIES = n6
CMSIS_MCU = STM32N657xx
AF_FILE = boards/stm32n657_af.csv
ifeq ($(BUILDING_MBOOT),1)
SYSTEM_FILE = $(STM32LIB_CMSIS_BASE)/Source/Templates/system_stm32$(MCU_SERIES)xx_fsbl.o
else
SYSTEM_FILE = $(STM32LIB_CMSIS_BASE)/Source/Templates/system_stm32$(MCU_SERIES)xx_s.o
endif
STM32_N6_HEADER_VERSION = 2.1
DKEL = $(STM32_CUBE_PROGRAMMER)/bin/ExternalLoader/MX25UM51245G_STM32N6570-NUCLEO.stldr

ifeq ($(USE_MBOOT),1)
LD_FILES = boards/stm32n657x0.ld boards/common_n6_flash.ld
TEXT0_ADDR = 0x70080000
else
LD_FILES = boards/stm32n657x0.ld boards/common_basic.ld
TEXT0_ADDR = 0x34180400
endif

# MicroPython settings
MICROPY_FLOAT_IMPL = double
