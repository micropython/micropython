MCU_SERIES = f4
CMSIS_MCU = STM32F411xE
AF_FILE = boards/stm32f411_af.csv
LD_FILES = boards/stm32f411.ld boards/common_ifs.ld
TEXT0_ADDR = 0x08000000
TEXT1_ADDR = 0x08020000

# Provide different variants for the downloads page.
BOARD_VARIANTS += "dp thread dp-thread network"

ifeq ($(BOARD_VARIANT),dp)
MICROPY_FLOAT_IMPL=double
endif

ifeq ($(BOARD_VARIANT),thread)
CFLAGS += -DMICROPY_PY_THREAD=1
endif

ifeq ($(BOARD_VARIANT),dp-thread)
MICROPY_FLOAT_IMPL=double
CFLAGS += -DMICROPY_PY_THREAD=1
endif

ifeq ($(BOARD_VARIANT),network)
MICROPY_PY_NETWORK_WIZNET5K=5200
endif

# PYB-specific frozen modules
FROZEN_MANIFEST ?= boards/PYBV10/manifest.py
