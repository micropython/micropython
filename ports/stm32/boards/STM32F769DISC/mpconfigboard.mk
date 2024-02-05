# By default the filesystem is in external QSPI flash.  But by setting the
# following option this board puts some code into external flash set in XIP mode.
# USE_MBOOT must be enabled; see f769_qspi.ld for code that goes in QSPI flash
USE_QSPI_XIP ?= 0
CFLAGS += -DUSE_QSPI_XIP=$(USE_QSPI_XIP)

# MCU settings
MCU_SERIES = f7
CMSIS_MCU = STM32F769xx
MICROPY_FLOAT_IMPL = double
AF_FILE = boards/stm32f767_af.csv

ifeq ($(USE_MBOOT),1)
ifeq ($(USE_QSPI_XIP),1)

# When using Mboot and QSPI-XIP the text is split between internal and external
# QSPI flash, and the filesystem is in internal flash between the bootloader and
# the main program text.
LD_FILES = boards/STM32F769DISC/f769_qspi.ld
TEXT0_ADDR = 0x08020000
TEXT1_ADDR = 0x90000000
TEXT0_SECTIONS = .isr_vector .text .data .ARM
TEXT1_SECTIONS = .text_qspi

else

# When using Mboot but not QSPI-XIP all the text goes together after the bootloader
# (at same location as when QSPI-XIP is enabled so the same Mboot can be used for
# either configuration) and the filesystem is in external flash.
LD_FILES = boards/stm32f769.ld boards/common_blifs.ld
TEXT0_ADDR = 0x08020000

endif
else

# When not using Mboot (and so no ability to load text into QSPI) all the text goes
# together at the start of internal flash, and the filesystem is in external flash.
LD_FILES = boards/stm32f769.ld boards/common_basic.ld
TEXT0_ADDR = 0x08000000

endif

# MicroPython settings
MICROPY_PY_LWIP = 1
MICROPY_PY_SSL = 1
MICROPY_SSL_MBEDTLS = 1

FROZEN_MANIFEST = $(BOARD_DIR)/manifest.py
