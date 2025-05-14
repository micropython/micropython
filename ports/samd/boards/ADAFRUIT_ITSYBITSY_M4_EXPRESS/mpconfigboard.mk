MCU_SERIES = SAMD51
CMSIS_MCU = SAMD51G19A
LD_FILES = boards/samd51x19a.ld sections.ld
TEXT0 = 0x4000

# The ?='s allow overriding in mpconfigboard.mk.
# MicroPython settings
MICROPY_HW_CODESIZE ?= 496K
