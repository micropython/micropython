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

#define GDISP_DRIVER_VMT			GDISPVMT_ILI9481
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

#include "board_ILI9481.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT		480
#endif
#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH		320
#endif
#ifndef GDISP_INITIAL_CONTRAST
	#define GDISP_INITIAL_CONTRAST	50
#endif
#ifndef GDISP_INITIAL_BACKLIGHT
	#define GDISP_INITIAL_BACKLIGHT	100
#endif

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

// Some common routines and macros
#define dummy_read(g)						{ volatile uint16_t dummy; dummy = read_data(g); (void) dummy; }
#define write_reg(g, reg, data)				{ write_index(g, reg); write_data(g, data); }
#define write_reg2x16(g, reg, data1, data2)	{ write_index(g, reg); write_data(g, (data1)>>8); write_data(g, (uint8_t)(data1)); write_data(g, (data2)>>8); write_data(g, (uint8_t)(data2));}

static void set_viewport(GDisplay* g) {
	write_reg2x16(g, 0x2A, g->p.x, g->p.x + g->p.cx - 1);
	write_reg2x16(g, 0x2B, g->p.y, g->p.y + g->p.cy - 1);
	write_index(g, 0x2C);
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

LLDSPEC bool_t gdisp_lld_init(GDisplay *g) {
	// No private area for this controller
	g->priv = 0;

	// Initialise the board interface
	init_board(g);

	/* Hardware reset */
	setpin_reset(g, TRUE);
	gfxSleepMilliseconds(20);
	setpin_reset(g, FALSE);
	gfxSleepMilliseconds(20);

	/* Get the bus for the following initialisation commands */
	acquire_bus(g);

	/* Enable Access to all Manufacturer commands (0xB0 and higher opcodes) */
	write_reg(g, 0xB0, 0x00);

	/* Frame Memory Access and Interface Setting */
	write_index(g, 0xB3);
	write_data(g, 0x02);
	write_data(g, 0x00);
	write_data(g, 0x00);
	write_data(g, 0x10);

	/* Display Mode and Frame Memory Write Mode Setting (B4h) */
	/* Use internal clock for synchronization */
	/* Use DBI interface (only DB0-17, no HSYNC, VSYNC or CLK) */
	write_reg(g, 0xB4, 0x00);

	/* Internal Backlight Control */
/*	write_index(g, 0xB9); 	// PWM Settings for Brightness Control
	write_data(g, 0x01);	// Disabled by default.
	write_data(g, 0xFF);	// 0xFF = Max brightness
	write_data(g, 0xFF);
	write_data(g, 0x18); */

	/* Panel Driving settings */
	write_index(g, 0xC0);
	write_data(g, 0x03);
	write_data(g, 0x3B);
	write_data(g, 0x00);
	write_data(g, 0x00);
	write_data(g, 0x00);
	write_data(g, 0x01);
	write_data(g, 0x00);	/* NW */
	write_data(g, 0x43);

	/* Display timings in Operating Mode */
	write_index(g, 0xC1);
	write_data(g, 0x08);
	write_data(g, 0x15);	/* CLOCK */
	write_data(g, 0x08);
	write_data(g, 0x08);

	/* S/VCOM/Gate Driving timing setting */
	write_index(g, 0xC4);
	write_data(g, 0x15);
	write_data(g, 0x03);
	write_data(g, 0x03);
	write_data(g, 0x01);

	/* Interface Setting */
	write_reg(g, 0xC6, 0x02);

	/* Gamma Setting - should be changed if using a different panel */
	write_index(g, 0xC8);
	write_data(g, 0x0C);
	write_data(g, 0x05);
	write_data(g, 0x0A); /*0X12 */
	write_data(g, 0x6B); /*0x7D */
	write_data(g, 0x04);
	write_data(g, 0x06); /*0x08 */
	write_data(g, 0x15); /*0x0A */
	write_data(g, 0x10);
	write_data(g, 0x00);
	write_data(g, 0x60); /*0x23 */

	/* Address Mode setting */
	write_reg(g, 0x36, 0x00);

	/* Set Pixel Format = 16 bits per pixel */
	/* The driver supports upto 24 bits per pixel, with dither */
	write_reg(g, 0x3A, 0x55);

	/* Exit Idle Mode */
	write_index(g, 0x38);

	/* Power Setting */
	write_index(g, 0xD0);
	write_data(g, 0x07);
	write_data(g, 0x07); 	/* VCI = VCI1 */
	write_data(g, 0x14);	/* VRH 0x1D */
	write_data(g, 0xA2);	/* BT 0x06 */

	/* VCOM Setting */
	write_index(g, 0xD1);
	write_data(g, 0x03);
	write_data(g, 0x5A);	/* VCM  0x5A */
	write_data(g, 0x10);	/* VDV */

	/* Power Setting for Normal Mode  */
	write_index(g, 0xD2);
	write_data(g, 0x03);
	write_data(g, 0x04);	/* 0x24  */
	write_data(g, 0x04);

	/* Exit Sleep Mode */
	write_index(g, 0x11);
	gfxSleepMilliseconds(150);

	/* Display ON  */
	write_index(g, 0x29);
	gfxSleepMilliseconds(30);

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
#endif

#if GDISP_HARDWARE_STREAM_READ
	LLDSPEC	void gdisp_lld_read_start(GDisplay *g) {
		acquire_bus(g);
		set_viewport(g);
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
		case GDISP_CONTROL_POWER:
			if (g->g.Powermode == (powermode_t)g->p.ptr)
				return;
			switch((powermode_t)g->p.ptr) {
			case powerOff:
				acquire_bus(g);
				write_reg(g, 0x0010, 0x0001);	/* enter sleep mode */
				release_bus(g);
				break;
			case powerOn:
				acquire_bus(g);
				write_reg(g, 0x0010, 0x0000);	/* leave sleep mode */
				release_bus(g);
				if (g->g.Powermode != powerSleep)
					gdisp_lld_init(g);
				break;
			case powerSleep:
				acquire_bus(g);
				write_reg(g, 0x0010, 0x0001);	/* enter sleep mode */
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

				write_reg(g, 0xC0, 0x03);
				write_reg(g, 0x36, 0x00);	/* X and Y axes non-inverted */

				release_bus(g);
				g->g.Height = GDISP_SCREEN_HEIGHT;
				g->g.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_90:
				acquire_bus(g);

				write_reg(g, 0xC0, 0x02);
				write_reg(g, 0x36, 0x20);	/* Invert X and Y axes */

				release_bus(g);
				g->g.Height = GDISP_SCREEN_WIDTH;
				g->g.Width = GDISP_SCREEN_HEIGHT;
				break;
			case GDISP_ROTATE_180:
				acquire_bus(g);

				write_reg(g, 0xC0, 0x06);
				write_reg(g, 0x36, 0x00);	/* X and Y axes non-inverted */

				release_bus(g);
				g->g.Height = GDISP_SCREEN_HEIGHT;
				g->g.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_270:
				acquire_bus(g);

				write_reg(g, 0xC0, 0x07);
				write_reg(g, 0x36, 0x20);	/* Invert X and Y axes */

				release_bus(g);
				g->g.Height = GDISP_SCREEN_WIDTH;
				g->g.Width = GDISP_SCREEN_HEIGHT;
				break;
			default:
				return;
			}
			g->g.Orientation = (orientation_t)g->p.ptr;
			return;
        //case GDISP_CONTROL_BACKLIGHT:
		//case GDISP_CONTROL_CONTRAST:
        default:
            return;
		}
	}
#endif

#endif /* GFX_USE_GDISP */
