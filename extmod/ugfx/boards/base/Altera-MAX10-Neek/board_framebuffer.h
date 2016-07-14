/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */
#include <system.h>
#include <io.h>

#define SCREEN_WIDTH			800
#define SCREEN_HEIGHT			480
#define GDISP_LLD_PIXELFORMAT	GDISP_PIXELFORMAT_RGB888

#ifdef GDISP_DRIVER_VMT

	static void board_init(GDisplay* g, fbInfo* fbi)
	{
		g->g.Width = SCREEN_WIDTH;
		g->g.Height = SCREEN_HEIGHT;
		g->g.Backlight = 100;
		g->g.Contrast = 50;
		fbi->linelen = g->g.Width * sizeof(LLDCOLOR_TYPE);									// bytes per row
		fbi->pixels = gfxAlloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(LLDCOLOR_TYPE));		// pointer to the memory frame buffer
		if (!fbi->pixels) {
			gfxHalt("Couldn't allocate memory for framebuffer\r\n");
		}

		// Let the framebuffer reader know where to find the framebuffer
		IOWR(FRAMEBUFFER_READER_BASE, 0, (alt_u32*)fbi->pixels);

		// Make sure the MSB is set so we bypass the data cache
		fbi->pixels = (void*)((char*)fbi->pixels + 0x80000000);
	}

	#if GDISP_HARDWARE_FLUSH
		static void board_flush(GDisplay* g)
		{
			(void) g;
		}
	#endif

	#if GDISP_NEED_CONTROL
		static void board_backlight(GDisplay* g, uint8_t percent)
		{
			(void) g;
			(void) percent;
		}

		static void board_contrast(GDisplay* g, uint8_t percent)
		{
			(void) g;
			(void) percent;
		}

		static void board_power(GDisplay* g, powermode_t pwr)
		{
			(void) g;
			(void) pwr;
		}
	#endif

#endif /* GDISP_LLD_BOARD_IMPLEMENTATION */
