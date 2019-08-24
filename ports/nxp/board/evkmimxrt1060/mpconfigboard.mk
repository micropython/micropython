Series := imx
BOARD := evkmimxrt1060
Device := MIMXRT1062

BOARD_DIR = ports/nxp/board/$(BOARD)

CFLAGS_CORE = -mthumb -mtune=cortex-m7 \
				-mcpu=cortex-m7 \
				-mfpu=fpv5-d16 -mfloat-abi=hard \
				-fsingle-precision-constant -Wdouble-promotion

CFLAGS_BOARD = -DCPU_MIMXRT1062DVL6A -D__MPU_PRESENT=1 \
				-DXIP_EXTERNAL_FLASH=1 \
		 		-DXIP_BOOT_HEADER_ENABLE=1 \
				-DXIP_BOOT_HEADER_DCD_ENABLE=1 \
				-DSKIP_SYSCLK_INIT \
				-DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1 \

LINK_FILE = MIMXRT1062xxxxx_flexspi_nor_sdram.ld

SRC_BOARD = $(addprefix $(BOARD_DIR)/, \
	board.c \
	port-pad.c \
	clock_config.c \
	evkmimxrt1060_flexspi_nor_config.c \
	evkmimxrt1060_sdram_ini_dcd.c \
	hardware_init.c \
	pin_mux.c \
	littlevgl_support.c \
)

LITTLEVGL_ENABLE = 1

HEAP_SIZE = 0x8000

BOARD_FSL_DRIVERS = \
	fsl_elcdif.c 