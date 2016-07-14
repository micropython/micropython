GFXINC  += $(GFXLIB)/boards/base/eCos-Synthetic-Framebuffer
GFXSRC  +=
GFXDEFS += -DGFX_USE_OS_ECOS=TRUE

include $(GFXLIB)/drivers/gdisp/framebuffer/driver.mk
