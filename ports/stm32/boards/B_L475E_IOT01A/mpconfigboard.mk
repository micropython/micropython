MCU_SERIES = l4
CMSIS_MCU = STM32L475xx
# The stm32l475 does not have a LDC controller which is
# the only diffrence to the stm32l476 - so reuse some files.
AF_FILE = boards/stm32l476_af.csv
LD_FILES = boards/stm32l476xg.ld boards/common_ifs.ld
TEXT0_ADDR = 0x08000000
TEXT1_ADDR = 0x08004000
OPENOCD_CONFIG = boards/openocd_stm32l4.cfg
