MCU_SERIES = f4
CMSIS_MCU = STM32F411xE
AF_FILE = boards/stm32f411_af.csv
LD_FILES = boards/stm32f411.ld boards/common_ifs.ld
TEXT0_ADDR = 0x08000000
TEXT1_ADDR = 0x08020000

MICROPY_VFS_LFS2 = 1

# Settings for version, spi flash and HSE xtal.
# These are used in variant configs and/or on make command line.
# If provided on make command line the build folder name will be updated to match
# When set in variant they're included after this file so the following ifdef blocks are ignored.

ifdef BOARD_VERSION
BUILD := $(BUILD)_V$(BOARD_VERSION)
endif

ifdef SPI_FLASH_SIZE_MB
BUILD := $(BUILD)_FLASH_$(SPI_FLASH_SIZE_MB)M
endif

ifdef XTAL_FREQ_MHZ
BUILD := $(BUILD)_XTAL_$(XTAL_FREQ_MHZ)M
endif

# Blackpill v3.1 board by default
BOARD_VERSION ?= 31

# No flash chip in default build - use internal flash
SPI_FLASH_SIZE_MB ?= 0

# 25Mhz HSE crystal by default.
XTAL_FREQ_MHZ ?= 25

CFLAGS += -DWEACT_F411_V$(BOARD_VERSION)=1
CFLAGS += -DMICROPY_HW_SPIFLASH_SIZE_BYTES="($(SPI_FLASH_SIZE_MB) * 1024 * 1024)"
CFLAGS += -DMICROPY_HW_HSE_SPEED_MHZ="($(XTAL_FREQ_MHZ))"
