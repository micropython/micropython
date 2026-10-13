# This board boots through the OpenMV bootloader, which starts the firmware at
# TEXT0_ADDR.  The bootloader's USB DFU interface (37c5:9203) flashes it, to
# the FIRMWARE alt setting.

MCU_SERIES = f7
CMSIS_MCU = STM32F765xx
AF_FILE = boards/stm32f767_af.csv
LD_FILES = boards/OPENMV3/board.ld
TEXT0_ADDR = 0x08020000

# The OpenMV firmware builds this board with SSL.
GIT_SUBMODULES += lib/mbedtls
