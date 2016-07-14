# Required include directories
BOARDINC = $(GFXLIB)/boards/base/Mikromedia-Plus-STM32-M4/ChibiOS_Board

# List of all the board related files.
BOARDSRC = $(BOARDINC)/board.c
#            $(BOARDINC)/flash_memory.c		- To be tested before it is added as standard