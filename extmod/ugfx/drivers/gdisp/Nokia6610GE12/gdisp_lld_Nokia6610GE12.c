/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GDISP

#if defined(GDISP_SCREEN_HEIGHT)
	#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
	#undef GDISP_SCREEN_HEIGHT
#endif
#if defined(GDISP_SCREEN_WIDTH)
	#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
	#undef GDISP_SCREEN_WIDTH
#endif

#define GDISP_DRIVER_VMT			GDISPVMT_Nokia6610GE12
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

#include "board_Nokia6610GE12.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#include "GE12.h"

#define GDISP_SCAN_LINES			132
#define GDISP_SLEEP_SIZE			32		/* Sleep mode window lines - this must be 32 on this controller */

// Set parameters if they are not already set
#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT		130
#endif
#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH		130
#endif
#ifndef GDISP_RAM_X_OFFSET
	#define GDISP_RAM_X_OFFSET		0		/* Offset in RAM of visible area */
#endif
#ifndef GDISP_RAM_Y_OFFSET
	#define GDISP_RAM_Y_OFFSET		2		/* Offset in RAM of visible area */
#endif
#ifndef GDISP_SLEEP_POS
	#define GDISP_SLEEP_POS			((GDISP_SCAN_LINES-GDISP_SLEEP_SIZE)/2 & ~3)
#endif
#ifndef GDISP_INITIAL_CONTRAST
	#define GDISP_INITIAL_CONTRAST	50
#endif
#ifndef GDISP_INITIAL_BACKLIGHT
	#define GDISP_INITIAL_BACKLIGHT	100
#endif

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

// Use the priv pointer itself to save our color. This save allocating ram for it
//	and works provided sizeof(uint16_t) <= sizeof(void *)
#define savecolor(g)					(*(uint16_t *)&g->priv)

#define GDISP_FLG_ODDBYTE				(GDISP_FLG_DRIVER<<0)

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

// Some macros just to make reading the code easier
#define delayms(ms)						gfxSleepMilliseconds(ms)
#define write_data2(g, d1, d2)			{ write_data(g, d1); write_data(g, d2); }
#define write_data3(g, d1, d2, d3)		{ write_data(g, d1); write_data(g, d2); write_data(g, d3); }
#define write_reg(g, cmd, d1)			{ write_index(g, cmd); write_data(g, d1); }
#define write_reg2(g, cmd, d1, d2)		{ write_index(g, cmd); write_data2(g, d1, d2); }
#define write_reg3(g, cmd, d1, d2, d3)	{ write_index(g, cmd); write_data3(g, d1, d2, d3); }

static GFXINLINE void set_viewport(GDisplay* g) {
	write_reg2(g, CASET, GDISP_RAM_X_OFFSET+g->p.x, GDISP_RAM_X_OFFSET+g->p.x+g->p.cx-1);			// Column address set
	write_reg2(g, PASET, GDISP_RAM_Y_OFFSET+g->p.y, GDISP_RAM_Y_OFFSET+g->p.y+g->p.cy-1);			// Page address set
	write_index(g, RAMWR);
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

LLDSPEC bool_t gdisp_lld_init(GDisplay *g) {
	// No private area for this controller
	g->priv = 0;

	// Initialise the board interface
	init_board(g);

	// Hardware reset
	setpin_reset(g, TRUE);
	delayms(20);
	setpin_reset(g, FALSE);
	delayms(20);

	acquire_bus(g);

	write_index(g, SLEEPOUT);								// Sleep out
	write_reg(g, COLMOD, 0x03);							// Color Interface Pixel Format - 0x03 = 12 bits-per-pixel
	write_reg(g, MADCTL, 0x00);							// Memory access controller
	write_reg(g, SETCON, 128*GDISP_INITIAL_CONTRAST/101-64);			// Write contrast
	delayms(20);

    // Finish Init
    post_init_board(g);

	release_bus(g);
	
	/* Turn on the back-light */
	set_backlight(g, GDISP_INITIAL_BACKLIGHT);

	/* Initialise the GDISP structure to match */
	g->g.Width = GDISP_SCREEN_WIDTH;
	g->g.Height = GDISP_SCREEN_HEIGHT;
	g->g.Orientation = GDISP_ROTATE_0;
	g->g.Powermode = powerOn;
	g->g.Backlight = GDISP_INITIAL_BACKLIGHT;
	g->g.Contrast = GDISP_INITIAL_CONTRAST;
	return TRUE;
}

#if GDISP_HARDWARE_STREAM_WRITE
	LLDSPEC	void gdisp_lld_write_start(GDisplay *g) {
		acquire_bus(g);
		set_viewport(g);
		g->flags &= ~GDISP_FLG_ODDBYTE;
	}
	LLDSPEC	void gdisp_lld_write_color(GDisplay *g) {
		uint16_t	c;

		c = gdispColor2Native(g->p.color);
		if ((g->flags & GDISP_FLG_ODDBYTE)) {
			// Write the pair of pixels to the display
			write_data3(g, ((savecolor(g) >> 4) & 0xFF),
					(((savecolor(g) << 4) & 0xF0)|((c >> 8) & 0x0F)),
					(c & 0xFF));
			g->flags &= ~GDISP_FLG_ODDBYTE;
		} else {
			savecolor(g) = c;
			g->flags |= GDISP_FLG_ODDBYTE;
		}
	}
	LLDSPEC	void gdisp_lld_write_stop(GDisplay *g) {
		if ((g->flags & GDISP_FLG_ODDBYTE)) {
			write_data2(g, ((savecolor(g) >> 4) & 0xFF), ((savecolor(g) << 4) & 0xF0));
			write_index(g, NOP);
		}
		release_bus(g);
	}
#endif

#if GDISP_NEED_CONTROL && GDISP_HARDWARE_CONTROL
	LLDSPEC void gdisp_lld_control(GDisplay *g) {
		/* The hardware is capable of supporting...
		 * 	GDISP_CONTROL_POWER				- supported
		 * 	GDISP_CONTROL_ORIENTATION		- supported
		 * 	GDISP_CONTROL_BACKLIGHT			- supported
		 * 	GDISP_CONTROL_CONTRAST			- supported
		 */
		switch(g->p.x) {
		case GDISP_CONTROL_POWER:
			if (g->g.Powermode == (powermode_t)g->p.ptr)
				return;
			switch((powermode_t)g->p.ptr) {
			case powerOff:
				acquire_bus(g);
				write_index(g, SLEEPIN);
				release_bus(g);
				break;
			case powerOn:
				acquire_bus(g);
				write_index(g, SLEEPOUT);
				delayms(20);
				write_index(g, NORON);							// Set Normal mode (my)
				release_bus(g);
				break;
			case powerSleep:
				acquire_bus(g);
				write_index(g, SLEEPOUT);
				delayms(20);
				write_reg2(g, PTLAR, GDISP_SLEEP_POS, GDISP_SLEEP_POS+GDISP_SLEEP_SIZE)
				write_index(g, PTLON);
				release_bus(g);
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
				acquire_bus(g);
				write_reg(g, MADCTL, 0x00);
				release_bus(g);
				g->g.Height = GDISP_SCREEN_HEIGHT;
				g->g.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_90:
				acquire_bus(g);
				write_reg(g, MADCTL, 0xA0);					// MY, MX, V, LAO, RGB, X, X, X
				release_bus(g);
				g->g.Height = GDISP_SCREEN_WIDTH;
				g->g.Width = GDISP_SCREEN_HEIGHT;
				break;
			case GDISP_ROTATE_180:
				acquire_bus(g);
				write_reg(g, MADCTL, 0xC0);
				release_bus(g);
				g->g.Height = GDISP_SCREEN_HEIGHT;
				g->g.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_270:
				acquire_bus(g);
				write_reg(g, MADCTL, 0x60);
				release_bus(g);
				g->g.Height = GDISP_SCREEN_WIDTH;
				g->g.Width = GDISP_SCREEN_HEIGHT;
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
			acquire_bus(g);
			write_reg(g, SETCON,(unsigned)128*(unsigned)g->p.ptr/101-64);
			release_bus(g);
			g->g.Contrast = (unsigned)g->p.ptr;
			return;
		}
	}
#endif

#endif /* GFX_USE_GDISP */
