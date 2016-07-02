Description:

Driver for OLED with 4-wire serial interface and I²C/SPI interface

Know restictions:
- Driver works only with SSD1306 hooked up over I2C or SPI (include corresponding header)
- Driver is written for 128x64 pixel displays (128x32 are only partly supported and need small further work)
- after using uGFX subsystem gdisp_lld_display() has to be called "by hand" to push framebuffer to display

To use this driver:

1. 	Add in your gfxconf.h:
	a) #define GFX_USE_GDISP	TRUE

	b) Any optional high level driver defines (see gdisp.h) eg: GDISP_NEED_MULTITHREAD

	c) The following are optional - define them if you are not using the defaults below:
		#define GDISP_SCREEN_WIDTH		128
		#define GDISP_SCREEN_HEIGHT		64

2. 	If you are not using a known board then create a gdisp_lld_board.h file according to
	given example files (or just stick with them) and ensure it is on your include path.

3. 	To your makefile add the following lines:
	include $(GFXLIB)/drivers/gdisp/SSD1306/gdisp_lld.mk

4. 	Call gdisp_lld_display() every time you want to update display content
