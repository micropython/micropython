GFXINC  += $(GFXLIB)/boards/base/Mikromedia-Plus-STM32-M4
GFXSRC  +=
GFXDEFS += -DGFX_USE_OS_CHIBIOS=TRUE
include $(GFXLIB)/drivers/gdisp/SSD1963/driver.mk
include $(GFXLIB)/drivers/ginput/touch/STMPE610/driver.mk
include $(GFXLIB)/drivers/gaudio/vs1053/driver.mk
