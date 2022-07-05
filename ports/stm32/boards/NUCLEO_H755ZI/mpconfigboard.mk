USE_MBOOT ?= 0

# MCU settings
MCU_SERIES = h7
CMSIS_MCU = STM32H755xx
MICROPY_FLOAT_IMPL = double
AF_FILE = boards/stm32h755_af.csv

ifdef CFLAGS
ifeq ("$(findstring -DCORE_CM7,$(CFLAGS))","-DCORE_CM7")
H7_M7 ?= 1
endif
ifeq ("$(findstring -DCORE_CM4,$(CFLAGS))","-DCORE_CM4")
H7_M4 ?= 1
endif
endif

ifeq ($(USE_MBOOT),1)
# When using Mboot everything goes after the bootloader
ifeq ($(H7_M7),1)
LD_FILES = boards/stm32h755.ld boards/common_bl.ld
TEXT0_ADDR = 0x08020000
endif
ifeq ($(H7_M4),1)
LD_FILES = boards/stm32h755_m4.ld boards/common_bl.ld
TEXT0_ADDR = 0x08120000
MICROPY_FLOAT_IMPL = single
FROZEN_MANIFEST ?= $(BOARD_DIR)/manifest.py
endif
else
# When not using Mboot everything goes at the start of flash
ifeq ($(H7_M7),1)
LD_FILES = boards/stm32h755.ld boards/common_basic.ld
TEXT0_ADDR = 0x08000000
endif
ifeq ($(H7_M4),1)
LD_FILES = boards/stm32h755_m4.ld boards/common_basic.ld
TEXT0_ADDR = 0x08100000
MICROPY_FLOAT_IMPL = single
FROZEN_MANIFEST ?= $(BOARD_DIR)/manifest.py
endif
endif

# MicroPython settings
MICROPY_PY_LWIP = 1
MICROPY_PY_USSL = 1
MICROPY_SSL_MBEDTLS = 1
MICROPY_VFS_LFS2 = 1
