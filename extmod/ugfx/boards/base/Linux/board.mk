GFXINC  += $(GFXLIB)/boards/base/Linux
GFXSRC  +=
GFXDEFS += -DGFX_USE_OS_LINUX=TRUE
GFXLIBS += rt

include $(GFXLIB)/drivers/multiple/X/driver.mk

