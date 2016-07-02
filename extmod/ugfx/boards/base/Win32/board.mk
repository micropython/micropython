GFXINC  +=
GFXSRC  +=
GFXLIBS +=
include $(GFXLIB)/drivers/multiple/Win32/driver.mk
include $(GFXLIB)/drivers/gaudio/Win32/driver.mk

ifeq ($(OPT_OS),win32.raw32)
	GFXDEFS += GFX_OS_INIT_NO_WARNING=TRUE
endif
