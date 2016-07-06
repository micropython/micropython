GFXINC  += $(GFXLIB)/boards/base/OSX
GFXSRC  +=
GFXDEFS += -DGFX_USE_OS_OSX=TRUE
GFXLIBS +=

include $(GFXLIB)/drivers/multiple/X/driver.mk

