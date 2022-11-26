MCU_SERIES = f4
CMSIS_MCU = STM32F405xx
AF_FILE = boards/stm32f405_af.csv
ifeq ($(USE_MBOOT),1)
# When using Mboot all the text goes together after the filesystem
LD_FILES = boards/stm32f405.ld boards/common_blifs.ld
TEXT0_ADDR = 0x08020000
else
# When not using Mboot the ISR text goes first, then the rest after the filesystem
LD_FILES = boards/stm32f405.ld boards/common_ifs.ld
TEXT0_ADDR = 0x08000000
TEXT1_ADDR = 0x08020000
endif

# MicroPython settings
MICROPY_VFS_LFS2 = 1

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
FROZEN_MANIFEST ?= $(BOARD_DIR)/manifest.py
