# This board requires a bootloader, either mboot or OpenMV's bootloader.
USE_MBOOT = 1

MCU_SERIES = n6
CMSIS_MCU = STM32N657xx
AF_FILE = boards/stm32n657_af.csv
ifeq ($(BUILDING_MBOOT),1)
SYSTEM_FILE = $(STM32LIB_CMSIS_BASE)/Source/Templates/system_stm32$(MCU_SERIES)xx_fsbl.o
else
SYSTEM_FILE = $(STM32LIB_CMSIS_BASE)/Source/Templates/system_stm32$(MCU_SERIES)xx_s.o
endif
STM32_N6_HEADER_VERSION = 2.3
DKEL = $(STM32_CUBE_PROGRAMMER)/bin/ExternalLoader/MX25UM51245G_STM32N6570-NUCLEO.stldr

LD_FILES = boards/OPENMV_N6/board.ld boards/common_n6_flash.ld
TEXT0_ADDR = 0x70080000

# MicroPython settings
MICROPY_FLOAT_IMPL = double
MICROPY_PY_BLUETOOTH ?= 1
MICROPY_BLUETOOTH_NIMBLE ?= 1
MICROPY_BLUETOOTH_BTSTACK ?= 0
MICROPY_PY_LWIP ?= 1
MICROPY_PY_NETWORK_CYW43 ?= 1
MICROPY_PY_SSL ?= 1
MICROPY_SSL_MBEDTLS ?= 1
MICROPY_VFS_LFS2 ?= 1

# Board specific frozen modules
FROZEN_MANIFEST ?= $(BOARD_DIR)/manifest.py
