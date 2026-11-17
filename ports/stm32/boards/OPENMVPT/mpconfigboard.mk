# This board boots through the OpenMV bootloader, which starts the firmware at
# TEXT0_ADDR.  The bootloader's USB DFU interface (37c5:9205) flashes it, to
# the FIRMWARE alt setting.

MCU_SERIES = h7
CMSIS_MCU = STM32H743xx
AF_FILE = boards/stm32h743_af.csv
LD_FILES = boards/OPENMVPT/board.ld
TEXT0_ADDR = 0x08040000

# The OpenMV firmware builds this board with SSL.
GIT_SUBMODULES += lib/mbedtls
