To use this driver:

This driver is special in that it implements both the gdisp low level driver,
optionally a touchscreen driver, and optionally a toggle driver.

1. Add in your gfxconf.h:
	a) #define GFX_USE_GDISP			TRUE
	b) Optionally #define GFX_USE_GINPUT			TRUE
					#define GINPUT_USE_MOUSE		TRUE
					#define GINPUT_USE_TOGGLE		TRUE
	c) Any optional high level driver defines (see gdisp.h) eg: GDISP_NEED_MULTITHREAD
	d) Optionally the following (with appropriate values):
		#define GDISP_SCREEN_WIDTH	640
		#define GDISP_SCREEN_HEIGHT	480


2. To your makefile add the following lines:
	include $(GFXLIB)/gfx.mk
	include $(GFXLIB)/drivers/multiple/Win32/driver.mk

	However, consider using the Win32 board file instead as this does include all
	the possible drivers that can be used (eg. for the GAUDIO module) by using:

	include $(GFXLIB)/gfx.mk
	include $(GFXLIB)/boards/base/Win32/board.mk


3. Modify your makefile to add -lws2_32 and -lgdi32 to the DLIBS line. i.e.
	DLIBS = -lws2_32 -lgdi32
