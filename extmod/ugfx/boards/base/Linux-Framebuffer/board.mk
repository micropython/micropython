GFXINC  += $(GFXLIB)/boards/base/Linux-Framebuffer
GFXSRC  +=
GFXDEFS += -DGFX_USE_OS_LINUX=TRUE
GFXLIBS += rt

include $(GFXLIB)/drivers/gdisp/framebuffer/driver.mk
