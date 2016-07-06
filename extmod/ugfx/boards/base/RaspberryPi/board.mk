GFXINC  += $(GFXLIB)/boards/base/RaspberryPi
GFXSRC  += $(GFXLIB)/boards/base/RaspberryPi/rpi_mailbox.c

include $(GFXLIB)/drivers/gdisp/framebuffer/driver.mk
