GFXINC	+= $(GFXLIB)/boards/base/Marlin
GFXSRC	+=
GFXDEFS += -DGFX_USE_CHIBIOS=TRUE

include $(GFXLIB)/boards/base/Marlin/chibios_board/board.mk
include $(GFXLIB)/drivers/gdisp/RA8875/driver.mk
include $(GFXLIB)/drivers/ginput/touch/FT5x06/driver.mk

