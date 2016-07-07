# Required include directories
BOARDINC = $(GFXLIB)/boards/base/Mikromedia-STM32-M4-ILI9341/ChibiOS_Board

# List of all the board related files.
BOARDSRC = $(BOARDINC)/board.c \
            $(BOARDINC)/flash_memory.c