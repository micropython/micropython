# This board boots through the OpenMV bootloader, which starts the firmware at
# TEXT0_ADDR.  The bootloader's USB DFU interface (37c5:9202) flashes it, to
# the FIRMWARE alt setting.

MCU_SERIES = f4
CMSIS_MCU = STM32F427xx
AF_FILE = boards/stm32f427_af.csv
LD_FILES = boards/OPENMV2/board.ld
TEXT0_ADDR = 0x08010000

# The OpenMV firmware builds this board with SSL.
GIT_SUBMODULES += lib/mbedtls
