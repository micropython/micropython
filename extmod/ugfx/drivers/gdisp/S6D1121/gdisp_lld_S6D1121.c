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
	#undef GISP_SCREEN_HEIGHT
#endif
#if defined(GDISP_SCREEN_WIDTH)
	#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
	#undef GDISP_SCREEN_WIDTH
#endif

#define GDISP_DRIVER_VMT			GDISPVMT_S6D1121
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

#include "board_S6D1121.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT		320
#endif
#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH		240
#endif
#ifndef GDISP_INITIAL_CONTRAST
	#define GDISP_INITIAL_CONTRAST	50
#endif
#ifndef GDISP_INITIAL_BACKLIGHT
	#define GDISP_INITIAL_BACKLIGHT	100
#endif

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/* Some common routines and macros */
#define dummy_read(g)				{ volatile uint16_t dummy; dummy = read_data(g); (void) dummy; }
#define write_reg(g, reg, data)		{ write_index(g, reg); write_data(g, data); }
#define delay(us)                   gfxSleepMicroseconds(us)
#define delayms(ms)                 gfxSleepMilliseconds(ms)

static GFXINLINE void set_cursor(GDisplay *g) {
    /* R20h - 8 bit
     * R21h - 9 bit
     */
    switch(g->g.Orientation) {
    	default:
        case GDISP_ROTATE_0:
            write_reg(g, 0x20, g->p.x & 0x00FF);
            write_reg(g, 0x21, g->p.y & 0x01FF);
            break;
        case GDISP_ROTATE_90:
            write_reg(g, 0x20, g->p.y & 0x00FF);
            write_reg(g, 0x21, (GDISP_SCREEN_HEIGHT - 1 - g->p.x) & 0x01FF);
            break;
        case GDISP_ROTATE_180:
            write_reg(g, 0x20, (GDISP_SCREEN_WIDTH - 1 - g->p.x) & 0x00FF);
            write_reg(g, 0x21, (GDISP_SCREEN_HEIGHT - 1 - g->p.y) & 0x01FF);
            break;
        case GDISP_ROTATE_270:
            write_reg(g, 0x20, (GDISP_SCREEN_WIDTH - 1 - g->p.y) & 0x00FF);
            write_reg(g, 0x21, g->p.x & 0x01FF);
            break;
    }
    write_index(g, 0x22);
}

static GFXINLINE void set_viewport(GDisplay *g) {
    /* HSA / HEA are 8 bit
     * VSA / VEA are 9 bit
     * use masks 0x00FF and 0x01FF to enforce this
     */

    switch(g->g.Orientation) {
    	default:
        case GDISP_ROTATE_0:
            write_reg(g, 0x46, (((g->p.x + g->p.cx - 1) << 8) & 0xFF00 ) | (g->p.x & 0x00FF));
            write_reg(g, 0x48, g->p.y & 0x01FF);
            write_reg(g, 0x47, (g->p.y + g->p.cy - 1) & 0x01FF);
            break;
        case GDISP_ROTATE_90:
            write_reg(g, 0x46, (((g->p.y + g->p.cy - 1) << 8) & 0xFF00) | (g->p.y & 0x00FF));
            write_reg(g, 0x48, (GDISP_SCREEN_HEIGHT - (g->p.x + g->p.cx)) & 0x01FF);
            write_reg(g, 0x47, (GDISP_SCREEN_HEIGHT-1 - g->p.x) & 0x01FF);
            break;
        case GDISP_ROTATE_180:
            write_reg(g, 0x46, (((GDISP_SCREEN_WIDTH-1 - g->p.x) & 0x00FF) << 8) |
                                      ((GDISP_SCREEN_WIDTH - (g->p.x + g->p.cx)) & 0x00FF));
            write_reg(g, 0x48, (GDISP_SCREEN_HEIGHT - (g->p.y + g->p.cy)) & 0x01FF);
            write_reg(g, 0x47, (GDISP_SCREEN_HEIGHT-1 - g->p.y) & 0x01FF);
            break;
        case GDISP_ROTATE_270:
            write_reg(g, 0x46, (((GDISP_SCREEN_WIDTH-1 - g->p.y) & 0x00FF) << 8) |
                                      ((GDISP_SCREEN_WIDTH - (g->p.y + g->p.cy)) & 0x00FF));
            write_reg(g, 0x48, g->p.x & 0x01FF);
            write_reg(g, 0x47, (g->p.x + g->p.cx - 1) & 0x01FF);
            break;
    }   
}

LLDSPEC bool_t gdisp_lld_init(GDisplay *g) {
	// No private area for this controller
	g->priv = 0;

	// Initialise the board interface
	init_board(g);

	/* Hardware reset */
	setpin_reset(g, TRUE);
	delayms(20);
	setpin_reset(g, TRUE);
	delayms(20);

	/* Get the bus for the following initialisation commands */
	acquire_bus(g);

	write_reg(g, 0x11, 0x2004);
	write_reg(g, 0x13, 0xCC00);
	write_reg(g, 0x15, 0x2600);
	write_reg(g, 0x14, 0x252A);
	write_reg(g, 0x12, 0x0033);
	write_reg(g, 0x13, 0xCC04);

	delayms(1);

	write_reg(g, 0x13, 0xCC06);

	delayms(1);

	write_reg(g, 0x13, 0xCC4F);

	delayms(1);

	write_reg(g, 0x13, 0x674F);
	write_reg(g, 0x11, 0x2003);

	delayms(1);

	// Gamma Setting
	write_reg(g, 0x30, 0x2609);
	write_reg(g, 0x31, 0x242C);
	write_reg(g, 0x32, 0x1F23);
	write_reg(g, 0x33, 0x2425);
	write_reg(g, 0x34, 0x2226);
	write_reg(g, 0x35, 0x2523);
	write_reg(g, 0x36, 0x1C1A);
	write_reg(g, 0x37, 0x131D);
	write_reg(g, 0x38, 0x0B11);
	write_reg(g, 0x39, 0x1210);
	write_reg(g, 0x3A, 0x1315);
	write_reg(g, 0x3B, 0x3619);
	write_reg(g, 0x3C, 0x0D00);
	write_reg(g, 0x3D, 0x000D);

	write_reg(g, 0x16, 0x0007);
	write_reg(g, 0x02, 0x0013);
	write_reg(g, 0x03, 0x0003);
	write_reg(g, 0x01, 0x0127);

	delayms(1);

	write_reg(g, 0x08, 0x0303);
	write_reg(g, 0x0A, 0x000B);
	write_reg(g, 0x0B, 0x0003);
	write_reg(g, 0x0C, 0x0000);
	write_reg(g, 0x41, 0x0000);
	write_reg(g, 0x50, 0x0000);
	write_reg(g, 0x60, 0x0005);
	write_reg(g, 0x70, 0x000B);
	write_reg(g, 0x71, 0x0000);
	write_reg(g, 0x78, 0x0000);
	write_reg(g, 0x7A, 0x0000);
	write_reg(g, 0x79, 0x0007);
	write_reg(g, 0x07, 0x0051);

	delayms(1);

	write_reg(g, 0x07,0x0053);
	write_reg(g, 0x79,0x0000);

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

#if GDISP_HARDWARE_STREAM_WRITE
	LLDSPEC	void gdisp_lld_write_start(GDisplay *g) {
		acquire_bus(g);
		set_viewport(g);
	}
	LLDSPEC	void gdisp_lld_write_color(GDisplay *g) {
		write_data(g, gdispColor2Native(g->p.color));
	}
	LLDSPEC	void gdisp_lld_write_stop(GDisplay *g) {
		release_bus(g);
	}
	LLDSPEC void gdisp_lld_write_pos(GDisplay *g) {
		set_cursor(g);
	}
#endif

#if GDISP_HARDWARE_STREAM_READ
	LLDSPEC	void gdisp_lld_read_start(GDisplay *g) {
		acquire_bus(g);
		set_viewport(g);
		set_cursor(g);
		setreadmode(g);
		dummy_read(g);
	}
	LLDSPEC	color_t gdisp_lld_read_color(GDisplay *g) {
		uint16_t	data;

		data = read_data(g);
		return gdispNative2Color(data);
	}
	LLDSPEC	void gdisp_lld_read_stop(GDisplay *g) {
		setwritemode(g);
		release_bus(g);
	}
#endif

#if GDISP_NEED_CONTROL && GDISP_HARDWARE_CONTROL
	LLDSPEC void gdisp_lld_control(GDisplay *g) {
		switch(g->p.x) {
		#if 0
			case GDISP_CONTROL_POWER:
				if (g->g.Powermode == (powermode_t)g->p.ptr)
					return;
				switch((powermode_t)g->p.ptr) {
				case powerOff:
					acquire_bus(g);
					// TODO
					release_bus(g);
					break;
				case powerOn:
					acquire_bus(g);
					// TODO
					release_bus(g);
					break;
				case powerSleep:
					acquire_bus(g);
					// TODO
					release_bus(g);
					break;
				default:
					return;
				}
				g->g.Powermode = (powermode_t)g->p.ptr;
				return;
		#endif

		case GDISP_CONTROL_ORIENTATION:
			if (g->g.Orientation == (orientation_t)g->p.ptr)
				return;
			switch((orientation_t)g->p.ptr) {
			case GDISP_ROTATE_0:
				acquire_bus(g);
				write_reg(g, 0x03, 0b0011);
				release_bus(g);
				g->g.Height = GDISP_SCREEN_HEIGHT;
				g->g.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_90:
				acquire_bus(g);
				write_reg(g, 0x03, 0b1001);
				release_bus(g);
				g->g.Height = GDISP_SCREEN_WIDTH;
				g->g.Width = GDISP_SCREEN_HEIGHT;
				break;
			case GDISP_ROTATE_180:
				acquire_bus(g);
				write_reg(g, 0x03, 0b0000);
				release_bus(g);
				g->g.Height = GDISP_SCREEN_HEIGHT;
				g->g.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_270:
				acquire_bus(g);
				write_reg(g, 0x03, 0b1010);
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
            if ((unsigned)g->p.ptr > 100)
            	g->p.ptr = (void *)100;
            set_backlight(g, (unsigned)g->p.ptr);
            g->g.Backlight = (unsigned)g->p.ptr;
            return;

		//case GDISP_CONTROL_CONTRAST:
        default:
            return;
		}
	}
#endif

#endif /* GFX_USE_GDISP */
