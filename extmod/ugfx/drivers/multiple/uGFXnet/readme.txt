To use this driver:

This driver is special in that it implements both the gdisp low level driver
and a touchscreen driver.

1. Add in your gfxconf.h:
	a) #define GFX_USE_GDISP		TRUE
	b) Optionally #define GFX_USE_GINPUT			TRUE
					#define GINPUT_USE_MOUSE		TRUE
	c) Any optional high level driver defines (see gdisp.h) eg: GDISP_NEED_MULTITHREAD
	d) Optionally the following (with appropriate values):
		#define GDISP_SCREEN_WIDTH					640			// Virtual display width
		#define GDISP_SCREEN_HEIGHT					480			// Virtual display height
		#define GDISP_GFXNET_UNSAFE_SOCKETS			FALSE		// Your socket library is not thread-safe
																//		LWIP automatically sets this to TRUE
		#define GDISP_GFXNET_CUSTOM_LWIP_STARTUP	FALSE		// You want a custom Start_LWIP() function (LWIP only)
		#define GDISP_DONT_WAIT_FOR_NET_DISPLAY		FALSE		// Don't halt waiting for the first connection
		$define GDISP_GFXNET_PORT					13001		// The TCP port the display sits on

2. To your makefile add the following lines:
	include $(GFXLIB)/gfx.mk
	include $(GFXLIB)/drivers/multiple/uGFXnet/gdisp_lld.mk

3. Make sure you have networking libraries included in your Makefile.

NOTE: If you are using ChibiOS with LWIP - you will probably need to increase
	the default stack size for the lwip_thread. 512 bytes seems too small. 1024 seems to work.
