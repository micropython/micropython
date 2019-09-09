Series = kinetis
BOARD = frdmk64
Device = MK64F12

BOARD_DIR = ports/nxp/board/$(BOARD)

CFLAGS_CORE = -mthumb -mtune=cortex-m4 \
				-mcpu=cortex-m4 \
				-mfpu=fpv4-sp-d16 -mfloat-abi=hard \

CFLAGS_BOARD = -DCPU_MK64FN1M0VLL12 -DFRDM_K64F \
				-DFREEDOM \
				-D__STARTUP_CLEAR_BSS \

LINK_FILE = MK64FN1M0xxx12_flash.ld

SRC_BOARD = $(addprefix $(BOARD_DIR)/, \
	board.c \
	clock_config.c \
	hardware_init.c \
	pin_mux.c \
)

LITTLEVGL_ENABLE = 0
SDCARD_INF = sdhc

HEAP_SIZE = 0x4000