MCU_SERIES = SAMD51
CMSIS_MCU = SAMD51P19A
LD_FILES = boards/samd51x19a.ld sections.ld
TEXT0 = 0x4000

# The ?='s allow overriding in mpconfigboard.mk.
# MicroPython settings
# The size of a MCU flash filesystem will be
# 1008k - MICROPY_HW_CODESIZE - MICROPY_HW_ROMFS_BYTES
# The default for MICROPY_HW_ROMFS_BYTES is 64K
MICROPY_HW_CODESIZE ?= 432K
