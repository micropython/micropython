MCU_SERIES = c5
CMSIS_MCU = STM32C5A3xx
MICROPY_FLOAT_IMPL = single
AF_FILE = boards/stm32c5a3_af.csv
STARTUP_FILE = $(STM32LIB_CMSIS_BASE)/Source/startup_$(CMSIS_MCU_LOWER).o

# Preserve the factory TinyUF2 bootloader in the first 32 KiB.
LD_FILES = boards/SEEED_XIAO_STM32C5/stm32c5a3cg.ld boards/common_basic.ld
TEXT0_ADDR = 0x08008000
UF2_FAMILY_ID = 0x00c5c5c5
