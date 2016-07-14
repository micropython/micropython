GFXINC	+= $(GFXLIB)/boards/base/FireBull-STM32F103-FB
GFXSRC	+=
GFXDEFS += -DGFX_USE_CHIBIOS=TRUE

include $(GFXLIB)/boards/base/FireBull-STM32F103-FB/chibios_board/board.mk
include $(GFXLIB)/drivers/gdisp/SSD1289/driver.mk
include $(GFXLIB)/drivers/ginput/touch/ADS7843/driver.mk

