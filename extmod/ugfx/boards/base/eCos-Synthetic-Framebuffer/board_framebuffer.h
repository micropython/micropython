/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * Note: this implementation makes a number of design choices...
 *  1/ We are using the macro definitions for eCos framebuffer driver. If you want to use the
 *  	function's instead you could use the g->board variable to store your cyg_fb pointer.
 *  2/ We assume the initialisation succeeds. It is probably a fatal error if it doesn't.
 *  3/ We hard-code in this file the pixel format, whether flushing is required and the FRAMEBUF device.
 *  	Please adjust them for your hardware.
 */

// Set this to your frame buffer pixel format.
#ifndef GDISP_LLD_PIXELFORMAT
	#define GDISP_LLD_PIXELFORMAT		GDISP_PIXELFORMAT_RGB565
#endif

// Uncomment this if your frame buffer device requires flushing ("Synch" in eCos speak)
#define GDISP_HARDWARE_FLUSH		TRUE

#ifdef GDISP_DRIVER_VMT

	#include <cyg/io/framebuf.h>

	// SET THIS HERE!!!
	// This must also match the pixel format above
	#define FRAMEBUF		fb0

	static void board_init(GDisplay *g, fbInfo *fbi) {
		// Initialize the frame buffer device - we assume everything is going to succeed.
		CYG_FB_ON(FRAMEBUF);
		#if (CYG_FB_FLAGS0(FRAMEBUF) & CYG_FB_FLAGS0_MUST_BE_ON)
			CYG_FB_FILL_BLOCK(FRAMEBUF, 0, 0, CYG_FB_WIDTH(FRAMEBUF), CYG_FB_HEIGHT(FRAMEBUF), CYG_FB_MAKE_COLOUR(FRAMEBUF, 0, 0, 0));
		#endif

		// Set the details of the frame buffer
		#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_VIEWPORT
			g->g.Width = CYG_FB_VIEWPORT_WIDTH(FRAMEBUF);
			g->g.Height = CYG_FB_VIEWPORT_HEIGHT(FRAMEBUF);
		#else
			g->g.Width = CYG_FB_WIDTH(FRAMEBUF);
			g->g.Height = CYG_FB_HEIGHT(FRAMEBUF);
		#endif
		g->g.Backlight = 100;
		g->g.Contrast = 50;
		fbi->linelen = CYG_FB_STRIDE(FRAMEBUF);							// bytes per row
		fbi->pixels = CYG_FB_BASE(FRAMEBUF);							// pointer to the memory frame buffer
	}

	#if GDISP_HARDWARE_FLUSH
		static void board_flush(GDisplay *g) {
			(void) g;

			// You might want to replace CYG_FB_UPDATE_NOW with CYG_FB_UPDATE_VERTICAL_RETRACE
			// if you are not using uGFX's auto flush or timer flush mechanisms and frame synchronisation
			// is important for your display.
			CYG_FB_SYNCH(FRAMEBUF, CYG_FB_UPDATE_NOW);
		}
	#endif

	#if GDISP_NEED_CONTROL
		static void board_backlight(GDisplay *g, uint8_t percent) {
			(void) g;
			#if (CYG_FB_FLAGS0(FRAMEBUF) & CYG_FB_FLAGS0_BACKLIGHT)
				cyg_fb_ioctl_backlight backlight;
				size_t len = sizeof(cyg_fb_ioctl_backlight);

				if (CYG_FB_IOCTL(FRAMEBUF, CYG_FB_IOCTL_BACKLIGHT_GET, &backlight, &len) || !backlight.fbbl_max)
					return;
				if (backlight.fbbl_max == 1)
					backlight.fbbl_current = percent ? 1 : 0;
				else
					backlight.fbbl_current = (((uint32_t)percent)*backlight.fbbl_max)/100;
				CYG_FB_IOCTL(FRAMEBUF, CYG_FB_IOCTL_BACKLIGHT_SET, &backlight, &len);
			#endif
		}

		static void board_contrast(GDisplay *g, uint8_t percent) {
			(void) g;
			(void) percent;
		}

		static void board_power(GDisplay *g, powermode_t pwr) {
			// Not implemented yet.
			(void) g;
			(void) pwr;
		}
	#endif

#endif /* GDISP_DRIVER_VMT */
