GFXINC  += $(GFXLIB)/boards/base/Olimex-SAM7EX256-GE12 $(GFXLIB)/boards/base/Olimex-SAM7EX256-GE8
GFXSRC  +=
GFXDEFS += -DGFX_USE_OS_CHIBIOS=TRUE
include $(GFXLIB)/drivers/gdisp/Nokia6610GE12/driver.mk
include $(GFXLIB)/drivers/gadc/AT91SAM7/driver.mk
include $(GFXLIB)/drivers/ginput/dial/GADC/driver.mk
include $(GFXLIB)/drivers/ginput/toggle/Pal/driver.mk
include $(GFXLIB)/drivers/gaudio/gadc/driver.mk
