GFXINC  += $(GFXLIB)/boards/base/Mikromedia-STM32-M4-ILI9341
GFXSRC  +=
GFXDEFS += -DGFX_USE_OS_CHIBIOS=TRUE
include $(GFXLIB)/drivers/gdisp/ILI9341/driver.mk
include $(GFXLIB)/drivers/ginput/touch/MCU/driver.mk
include $(GFXLIB)/drivers/gaudio/vs1053/driver.mk
