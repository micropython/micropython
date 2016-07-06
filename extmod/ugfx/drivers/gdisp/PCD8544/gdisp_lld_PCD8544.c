/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GDISP

#define GDISP_DRIVER_VMT		GDISPVMT_PCD8544
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"
#include "board_PCD8544.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#define GDISP_SCREEN_HEIGHT		48
#define GDISP_SCREEN_WIDTH		84

#define GDISP_INITIAL_CONTRAST		51
#define GDISP_INITIAL_BACKLIGHT		100

#define GDISP_FLG_NEEDFLUSH		(GDISP_FLG_DRIVER << 0)

#include "PCD8544.h"

/*===========================================================================*/
/* Driver local routines.                                                    */
/*===========================================================================*/

// Some common routines and macros
#define RAM(g)			((uint8_t *)g->priv)

#define xyaddr(x, y)		((x) + ((y) >> 3) * GDISP_SCREEN_WIDTH)
#define xybit(y)		(1 << ((y) & 7))

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * As this controller can't update on a pixel boundary we need to maintain the
 * the entire display surface in memory so that we can do the necessary bit
 * operations. Fortunately it is a small display in monochrome.
 * Display 48 * 84 / 8 = 504
 */

#define GDISP_SCREEN_BYTES ((GDISP_SCREEN_WIDTH * GDISP_SCREEN_HEIGHT) / 8)

LLDSPEC bool_t gdisp_lld_init(GDisplay *g) {
	// The private area is the display surface.
	if (!(g->priv = gfxAlloc(GDISP_SCREEN_BYTES)))
		gfxHalt("GDISP PCD8544: Failed to allocate private memory");

	// Initialise the board interface
	init_board(g);

	// Hardware reset
	setpin_reset(g, TRUE);
	gfxSleepMilliseconds(100);
	setpin_reset(g, FALSE);
	gfxSleepMilliseconds(100);

	acquire_bus(g);

	write_cmd(g, PCD8544_SET_FUNC  		| PCD8544_H);
	write_cmd(g, PCD8544_SET_TEMP  		| PCD8544_TEMP_MODE_2);
	write_cmd(g, PCD8544_SET_BIAS  		| PCD8544_BIAS_MODE_4);
	write_cmd(g, PCD8544_SET_VOP		| (0x40));
	write_cmd(g, PCD8544_SET_FUNC);
	write_cmd(g, PCD8544_SET_DISPLAY	| PCD8544_DISPLAY_MODE_NORMAL);

	// Finish Init
	post_init_board(g);

 	// Release the bus
	release_bus(g);

	/* Turn on the back-light */
	set_backlight(g, GDISP_INITIAL_BACKLIGHT);

	/* Initialise the GDISP structure */
	g->g.Width = GDISP_SCREEN_WIDTH;
	g->g.Height = GDISP_SCREEN_HEIGHT;
	g->g.Orientation = GDISP_ROTATE_0;
	g->g.Powermode = powerOn;
	g->g.Backlight = GDISP_INITIAL_BACKLIGHT;
	g->g.Contrast = GDISP_INITIAL_CONTRAST;

	return TRUE;
}

#if GDISP_HARDWARE_FLUSH
	LLDSPEC void gdisp_lld_flush(GDisplay *g) {

		// Don't flush if we don't need it.
		if (!(g->flags & GDISP_FLG_NEEDFLUSH)) {
			return;
		}

		acquire_bus(g);

		write_cmd(g, PCD8544_SET_X | 0);  // X = 0
		write_cmd(g, PCD8544_SET_Y | 0);  // Y = 0

		write_data(g, RAM(g), GDISP_SCREEN_BYTES);

		release_bus(g);

		g->flags &= ~GDISP_FLG_NEEDFLUSH;
	}
#endif

#if GDISP_HARDWARE_DRAWPIXEL
	LLDSPEC void gdisp_lld_draw_pixel(GDisplay *g) {
		coord_t x, y;

		#if GDISP_NEED_CONTROL
			switch(g->g.Orientation) {
			default:
			case GDISP_ROTATE_0:
				x = g->p.x;
				y = g->p.y;
				break;
			case GDISP_ROTATE_90:
				x = g->p.y;
				y = g->g.Width - g->p.x - 1;
				break;
			case GDISP_ROTATE_180:
				x = g->g.Width  - g->p.x - 1;
				y = g->g.Height - g->p.y - 1;
				break;
			case GDISP_ROTATE_270:
				x = g->g.Height - g->p.y - 1;
				y = g->p.x;
				break;
			}
		#else
			x = g->p.x;
			y = g->p.y;
		#endif

		if (g->p.color != Black) {
			RAM(g)[xyaddr(x, y)] |= xybit(y);
		} else {
			RAM(g)[xyaddr(x, y)] &= ~xybit(y);
		}

		g->flags |= GDISP_FLG_NEEDFLUSH;
	}
#endif

#if GDISP_HARDWARE_PIXELREAD
	LLDSPEC color_t gdisp_lld_get_pixel_color(GDisplay *g) {
		coord_t		x, y;

		switch(g->g.Orientation) {
		default:
		case GDISP_ROTATE_0:
			x = g->p.x;
			y = g->p.y;
			break;
		case GDISP_ROTATE_90:
			x = g->p.y;
			y = GDISP_SCREEN_HEIGHT-1 - g->p.x;
			break;
		case GDISP_ROTATE_180:
			x = GDISP_SCREEN_WIDTH-1 - g->p.x;
			y = GDISP_SCREEN_HEIGHT-1 - g->p.y;
			break;
		case GDISP_ROTATE_270:
			x = GDISP_SCREEN_WIDTH-1 - g->p.y;
			y = g->p.x;
			break;
		}
		return (RAM(g)[xyaddr(x, y)] & xybit(y)) ? White : Black;
	}
#endif

#if GDISP_NEED_CONTROL
	LLDSPEC void gdisp_lld_control(GDisplay *g) {
		switch(g->p.x) {
		case GDISP_CONTROL_POWER:
			if (g->g.Powermode == (powermode_t)g->p.ptr)
				return;
			switch((powermode_t)g->p.ptr) {
			case powerOff:
			case powerSleep:
			case powerDeepSleep:
				write_cmd(g, PCD8544_SET_FUNC | PCD8544_PD);
				break;
			case powerOn:
				write_cmd(g, PCD8544_SET_FUNC);
				break;
			default:
				return;
			}
			g->g.Powermode = (powermode_t)g->p.ptr;
			return;

		case GDISP_CONTROL_ORIENTATION:
			if (g->g.Orientation == (orientation_t)g->p.ptr)
				return;
			switch((orientation_t)g->p.ptr) {
				case GDISP_ROTATE_0:
				case GDISP_ROTATE_180:
					if (g->g.Orientation == GDISP_ROTATE_90 || g->g.Orientation == GDISP_ROTATE_270) {
						coord_t		tmp;

						tmp = g->g.Width;
						g->g.Width = g->g.Height;
						g->g.Height = tmp;
					}
					break;
				case GDISP_ROTATE_90:
				case GDISP_ROTATE_270:
					if (g->g.Orientation == GDISP_ROTATE_0 || g->g.Orientation == GDISP_ROTATE_180) {
						coord_t		tmp;

						tmp = g->g.Width;
						g->g.Width = g->g.Height;
						g->g.Height = tmp;
					}
					break;
				default:
					return;
			}
			g->g.Orientation = (orientation_t)g->p.ptr;
			return;

		case GDISP_CONTROL_BACKLIGHT:
			if ((unsigned)g->p.ptr > 100) g->p.ptr = (void *)100;
			set_backlight(g, (unsigned)g->p.ptr);
			g->g.Backlight = (unsigned)g->p.ptr;
			return;

		case GDISP_CONTROL_CONTRAST:
			if ((unsigned)g->p.ptr > 100) g->p.ptr = (void *)100;
			write_cmd(g, PCD8544_SET_VOP | (unsigned)g->p.ptr);
			g->g.Contrast = (unsigned)g->p.ptr;
			return;
		}
	}
#endif

#endif // GFX_USE_GDISP
