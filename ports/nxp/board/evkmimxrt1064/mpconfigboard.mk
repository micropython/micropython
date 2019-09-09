Series := imx
BOARD := evkmimxrt1064
Device := MIMXRT1064

BOARD_DIR = ports/nxp/board/$(BOARD)

CFLAGS_CORE = -mthumb -mtune=cortex-m7 \
				-mcpu=cortex-m7 \
				-mfpu=fpv5-d16 -mfloat-abi=hard \
				-fsingle-precision-constant -Wdouble-promotion

CFLAGS_BOARD = -DCPU_MIMXRT1064DVL6A -D__MPU_PRESENT=1 \
				-DXIP_EXTERNAL_FLASH=1 \
				-DLV_CONF_INCLUDE_SIMPLE=1 \
		 		-DXIP_BOOT_HEADER_ENABLE=1 \
		 		-DXIP_BOOT_HEADER_DCD_ENABLE=1 \
				-D__STARTUP_CLEAR_BSS \
				-D__STARTUP_INITIALIZE_NONCACHEDATA \
		 		-DSKIP_SYSCLK_INIT \
				-DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1 \

LINK_FILE = MIMXRT1064xxxxx_flexspi_nor_sdram.ld

SRC_BOARD = $(addprefix $(BOARD_DIR)/, \
	board.c \
	port-pad.c \
	clock_config.c \
	evkmimxrt1064_flexspi_nor_config.c \
	evkmimxrt1064_sdram_ini_dcd.c \
	hardware_init.c \
	pin_mux.c \
	littlevgl_support.c \
)
#	fsl_clock.c 

# If use little vgl Set this variable
LITTLEVGL_ENABLE = 1

HEAP_SIZE = 0x8000
SDCARD_INF = usdhc

BOARD_FSL_DRIVERS = \
	fsl_elcdif.c 