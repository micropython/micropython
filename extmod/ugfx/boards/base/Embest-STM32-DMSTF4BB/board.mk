GFXINC	+= $(GFXLIB)/boards/base/Embest-STM32-DMSTF4BB
GFXSRC	+=
GFXDEFS += -DGFX_USE_CHIBIOS=TRUE

include $(GFXLIB)/drivers/gdisp/SSD2119/driver.mk
include $(GFXLIB)/drivers/ginput/touch/STMPE811/driver.mk

