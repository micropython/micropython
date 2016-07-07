GFXINC	+= $(GFXLIB)/boards/base/Olimex-STM32-LCD
GFXSRC	+=
GFXDEFS += -DGFX_USE_CHIBIOS=TRUE

include $(GFXLIB)/boards/base/Olimex-STM32-LCD/chibios_board/board.mk
include $(GFXLIB)/drivers/gdisp/ILI9320/driver.mk
include $(GFXLIB)/drivers/ginput/touch/MCU/driver.mk

