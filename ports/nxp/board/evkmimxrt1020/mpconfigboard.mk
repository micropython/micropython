Series = imx
BOARD = evkmimxrt1020
Device = MIMXRT1021

BOARD_DIR = ports/nxp/board/$(BOARD)

CFLAGS_CORE = -mthumb -mtune=cortex-m7 \
				-mcpu=cortex-m7 \
				-mfpu=fpv5-d16 -mfloat-abi=hard \
				-fsingle-precision-constant -Wdouble-promotion

CFLAGS_BOARD = -DCPU_MIMXRT1021DAG5A -D__MPU_PRESENT=1 \
				-DXIP_EXTERNAL_FLASH=1 \
		 		-DXIP_BOOT_HEADER_ENABLE=1 \
		 		-DXIP_BOOT_HEADER_DCD_ENABLE=1 \
		 		-DSKIP_SYSCLK_INIT \
				-DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1 \

LINK_FILE = MIMXRT1021xxxxx_flexspi_nor_sdram.ld

SRC_BOARD = $(addprefix $(BOARD_DIR)/, \
	board.c \
	port-pad.c \
	clock_config.c \
	evkmimxrt1020_flexspi_nor_config.c \
	evkmimxrt1020_sdram_ini_dcd.c \
	hardware_init.c \
	pin_mux.c \
)

LITTLEVGL_ENABLE = 0

HEAP_SIZE = 0x8000