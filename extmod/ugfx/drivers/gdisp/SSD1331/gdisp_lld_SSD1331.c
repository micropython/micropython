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

#define GDISP_DRIVER_VMT			GDISPVMT_SSD1331
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

#include "board_SSD1331.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT		64
#endif
#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH		96
#endif
#ifndef GDISP_INITIAL_CONTRAST
	#define GDISP_INITIAL_CONTRAST	100
#endif
#ifndef GDISP_INITIAL_BACKLIGHT
	#define GDISP_INITIAL_BACKLIGHT	32
#endif

#include "SSD1331.h"

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

// Some common routines and macros
#define write_reg(g, reg, data)		{ write_cmd(g, reg); write_data(g, data); }

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

static const uint8_t initdata[] = {
	SSD1331_DISPLAY_OFF,
	SSD1331_START_LINE, 0x00,
	SSD1331_COM_OFFSET, 0x00,
	SSD1331_PIXELS_NORMAL,
	SSD1331_MULTIPLEX, 0x3F,
	SSD1331_RESET, SSD1331_RESET_OFF,
	SSD1331_POWER, SSD1331_POWER_ON,
	SSD1331_PHASE_PERIOD, 0x31,
	SSD1331_CLOCKS, 0xF0,
	SSD1331_PRECHARGE_A, 0x64,
	SSD1331_PRECHARGE_B, 0x78,
	SSD1331_PRECHARGE_C, 0x64,
	SSD1331_PRECHARGE_VOLTAGE, 0x3A,
	SSD1331_DESELECT_VOLTAGE, 0x3E,
	SSD1331_CONTRAST_A, 0x91,
	SSD1331_CONTRAST_B, 0x50,
	SSD1331_CONTRAST_C, 0x7D,
	SSD1331_BRIGHTNESS, (GDISP_INITIAL_BACKLIGHT*10)/63,
	#if GDISP_LLD_PIXELFORMAT == GDISP_PIXELFORMAT_RGB565
		SSD1331_MODE, SSD1331_MODE_16_BIT|SSD1331_MODE_COM_SPLIT,
	#elif GDISP_LLD_PIXELFORMAT == GDISP_PIXELFORMAT_BGR565
		SSD1331_MODE, SSD1331_MODE_16_BIT|SSD1331_MODE_COM_SPLIT|SSD1331_MODE_BGR,
	#elif GDISP_LLD_PIXELFORMAT == GDISP_PIXELFORMAT_RGB332
		SSD1331_MODE, SSD1331_MODE_8_BIT|SSD1331_MODE_COM_SPLIT,
	#elif GDISP_LLD_PIXELFORMAT == GDISP_PIXELFORMAT_BGR332
		SSD1331_MODE, SSD1331_MODE_8_BIT|SSD1331_MODE_COM_SPLIT|SSD1331_MODE_BGR,
	#else
		#error "SSD1331: Invalid color format"
	#endif
	SSD1331_DRAW_MODE, SSD1331_DRAW_FILLRECT,
};

LLDSPEC bool_t gdisp_lld_init(GDisplay *g) {
	unsigned		i;

	// No private area for this controller
	g->priv = 0;

	// Initialise the board interface
	init_board(g);

	// Hardware reset
	setpin_reset(g, TRUE);
	gfxSleepMilliseconds(20);
	setpin_reset(g, FALSE);
	gfxSleepMilliseconds(20);

	// Get the bus for the following initialisation commands
	acquire_bus(g);

	for(i=0;i<sizeof(initdata);i++)
		write_cmd(g, initdata[i]);

	write_cmd(g, SSD1331_DISPLAY_ON);

    // Finish Init
    post_init_board(g);

 	// Release the bus
	release_bus(g);

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
		write_cmd(g, SSD1331_COLUMN_ADDRESS);
		write_cmd(g, g->p.x);
		write_cmd(g, g->p.x + g->p.cx - 1);
		write_cmd(g, SSD1331_ROW_ADDRESS);
		write_cmd(g, g->p.y);
		write_cmd(g, g->p.y + g->p.cy - 1);
	}
	#if GDISP_LLD_PIXELFORMAT == GDISP_PIXELFORMAT_RGB565 || GDISP_LLD_PIXELFORMAT == GDISP_PIXELFORMAT_BGR565
		LLDSPEC	void gdisp_lld_write_color(GDisplay *g) {
			LLDCOLOR_TYPE	c;

			c = gdispColor2Native(g->p.color);
			write_data(g, c >> 8);
			write_data(g, c);
		}
	#else
		LLDSPEC	void gdisp_lld_write_color(GDisplay *g) {
			write_data(g, gdispColor2Native(g->p.color));
		}
	#endif
	LLDSPEC	void gdisp_lld_write_stop(GDisplay *g) {
		release_bus(g);
	}
#endif

#if GDISP_HARDWARE_FILLS
	LLDSPEC void gdisp_lld_fill_area(GDisplay *g) {
		uint8_t		a6, b6, c6;

		acquire_bus(g);

		if (g->p.color == Black) {
			// Use clear window command
			write_cmd(g, SSD1331_DRAW_CLEAR);
			write_cmd(g, g->p.x);
			write_cmd(g, g->p.y);
			write_cmd(g, g->p.x + g->p.cx - 1);
			write_cmd(g, g->p.y + g->p.cy - 1);
			release_bus(g);
			return;
		}

		#if GDISP_LLD_PIXELFORMAT == GDISP_PIXELFORMAT_RGB565 || GDISP_LLD_PIXELFORMAT == GDISP_PIXELFORMAT_RGB332
			c6 = RED_OF(g->p.color) >> 2;
			#if COLOR_BITS_R < 5
				if (c6 & 0x20) c6 |= (1<<(6-COLOR_BITS_R))-1;
			#endif
			b6 = GREEN_OF(g->p.color) >> 2;
			#if COLOR_BITS_G < 6
				if (b6 & 0x20) b6 |= (1<<(6-COLOR_BITS_G))-1;
			#endif
			a6 = BLUE_OF(g->p.color) >> 2;
			#if COLOR_BITS_B < 5
				if (a6 & 0x20) a6 |= (1<<(6-COLOR_BITS_B))-1;
			#endif
		#else
			c6 = BLUE_OF(g->p.color) >> 2;
			#if COLOR_BITS_B < 5
				if (c6 & 0x20) c6 |= (1<<(6-COLOR_BITS_B))-1;
			#endif
			b6 = GREEN_OF(g->p.color) >> 2;
			#if COLOR_BITS_G < 6
				if (b6 & 0x20) b6 |= (1<<(6-COLOR_BITS_G))-1;
			#endif
			a6 = RED_OF(g->p.color) >> 2;
			#if COLOR_BITS_R < 5
				if (a6 & 0x20) a6 |= (1<<(6-COLOR_BITS_R))-1;
			#endif
		#endif
		if (g->p.cx == 1 || g->p.cy == 1) {
			// Use Line command
			write_cmd(g, SSD1331_DRAW_LINE);
			write_cmd(g, g->p.x);
			write_cmd(g, g->p.y);
			write_cmd(g, g->p.x + g->p.cx - 1);
			write_cmd(g, g->p.y + g->p.cy - 1);
		} else {
			// Use Rectangle command

			write_cmd(g, SSD1331_DRAW_RECT);
			write_cmd(g, g->p.x);
			write_cmd(g, g->p.y);
			write_cmd(g, g->p.x + g->p.cx - 1);
			write_cmd(g, g->p.y + g->p.cy - 1);
			//outline
			write_cmd(g, c6);
			write_cmd(g, b6);
			write_cmd(g, a6);
			//fill
		}

		write_cmd(g, c6);
		write_cmd(g, b6);
		write_cmd(g, a6);
		release_bus(g);
	}
#endif

#if GDISP_NEED_SCROLL && GDISP_HARDWARE_SCROLL
	LLDSPEC void gdisp_lld_vertical_scroll(GDisplay *g) {
		acquire_bus(g);
		write_cmd(g, SSD1331_DRAW_COPY);
		write_cmd(g, g->p.x);
		write_cmd(g, g->p.y);
		write_cmd(g, g->p.x + g->p.cx - 1);
		write_cmd(g, g->p.y + g->p.cy - 1);
		write_cmd(g, g->p.x);
		write_cmd(g, g->p.y - lines);
		release_bus(g);
	}
#endif

#if GDISP_NEED_CONTROL && GDISP_HARDWARE_CONTROL
	LLDSPEC void gdisp_lld_control(GDisplay *g) {
		switch(g->p.x) {
		case GDISP_CONTROL_POWER:
			if (g->g.Powermode == (powermode_t)g->p.ptr)
				return;
			switch((powermode_t)g->p.ptr) {
			case powerOff:
			case powerSleep:
			case powerDeepSleep:
				acquire_bus(g);
				write_cmd(g, SSD1331_DISPLAY_OFF);
				release_bus(g);
				break;
			case powerOn:
				acquire_bus(g);
				write_cmd(g, SSD1331_DISPLAY_ON);
				release_bus(g);
				break;
			default:
				return;
			}
			g->g.Powermode = (powermode_t)g->p.ptr;
			return;

		/*
		case GDISP_CONTROL_ORIENTATION:
			if (g->g.Orientation == (orientation_t)g->p.ptr)
				return;
			switch((orientation_t)g->p.ptr) {
			case GDISP_ROTATE_0:
				acquire_bus(g);
				//TODO
				release_bus(g);
				g->g.Height = GDISP_SCREEN_HEIGHT;
				g->g.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_90:
				acquire_bus(g);
				//TODO
				release_bus(g);
				g->g.Height = GDISP_SCREEN_WIDTH;
				g->g.Width = GDISP_SCREEN_HEIGHT;
				break;
			case GDISP_ROTATE_180:
				acquire_bus(g);
				//TODO
				release_bus(g);
				g->g.Height = GDISP_SCREEN_HEIGHT;
				g->g.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_270:
				acquire_bus(g);
				//TODO
				release_bus(g);
				g->g.Height = GDISP_SCREEN_WIDTH;
				g->g.Width = GDISP_SCREEN_HEIGHT;
				break;
			default:
				return;
			}
			g->g.Orientation = (orientation_t)g->p.ptr;
			return;
		*/

        case GDISP_CONTROL_BACKLIGHT:
            if ((unsigned)g->p.ptr > 100)
            	g->p.ptr = (void *)100;
			acquire_bus(g);
            write_cmd(g, SSD1331_BRIGHTNESS);
            write_cmd(g, ((unsigned)g->p.ptr*10)/63);
			release_bus(g);
            g->g.Backlight = (unsigned)g->p.ptr;
            return;

		//case GDISP_CONTROL_CONTRAST:
        default:
            return;
		}
	}
#endif

#endif /* GFX_USE_GDISP */
