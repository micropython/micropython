MCU_SERIES = SAMD51
CMSIS_MCU = ATSAMD51P20A
LD_FILES = boards/samd51x20a.ld sections.ld
TEXT0 = 0x4000

# The ?='s allow overriding in mpconfigboard.mk.
# MicroPython settings
MICROPY_VFS_LFS1 ?= 1

FROZEN_MANIFEST = $(BOARD_DIR)/manifest.py

# Greater room for code in flash
MICROPY_HW_CODESIZE ?= 1008K