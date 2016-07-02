/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

#define GDISP_DRIVER_VMT			GDISPVMT_TestStub
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT		128
#endif
#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH		128
#endif
#ifndef GDISP_INITIAL_CONTRAST
	#define GDISP_INITIAL_CONTRAST	50
#endif
#ifndef GDISP_INITIAL_BACKLIGHT
	#define GDISP_INITIAL_BACKLIGHT	100
#endif

LLDSPEC bool_t gdisp_lld_init(GDisplay *g) {
	/* No board interface and no private driver area */
	g->priv = g->board = 0;

	/* Initialise the GDISP structure */
	g->g.Width = GDISP_SCREEN_WIDTH;
	g->g.Height = GDISP_SCREEN_HEIGHT;
	g->g.Orientation = GDISP_ROTATE_0;
	g->g.Powermode = powerOn;
	g->g.Backlight = GDISP_INITIAL_BACKLIGHT;
	g->g.Contrast = GDISP_INITIAL_CONTRAST;
	return TRUE;
}

#if GDISP_HARDWARE_DRAWPIXEL
	void gdisp_lld_draw_pixel(GDisplay *g) {
		(void) g;
	}
#endif

#if GDISP_HARDWARE_PIXELREAD
	color_t gdisp_lld_get_pixel_color(GDisplay *g) {
		(void) g;
		return 0;
	}
#endif

#endif /* GFX_USE_GDISP */
