MCU_SERIES = SAME54
CMSIS_MCU = SAME54P20A
LD_FILES = boards/samd51x20a.ld sections.ld
TEXT0 = 0x4000

# The ?='s allow overriding in mpconfigboard.mk.
# MicroPython settings
MICROPY_HW_CODESIZE ?= 1008K
