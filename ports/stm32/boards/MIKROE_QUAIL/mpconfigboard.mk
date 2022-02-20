MCU_SERIES = f4
CMSIS_MCU = STM32F427xx
LD_FILES = boards/stm32f427xi.ld boards/common_ifs.ld
TEXT0_ADDR = 0x08000000
TEXT1_ADDR = 0x08020000

# According to the datasheet, page 75, table 12, the alternate functions
# of STM32F427xx and STM32F429xx are exactly the same.
# See https://www.st.com/resource/en/datasheet/stm32f427vi.pdf.
AF_FILE = boards/stm32f429_af.csv
