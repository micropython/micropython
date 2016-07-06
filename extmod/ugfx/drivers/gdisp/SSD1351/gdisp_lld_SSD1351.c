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

#define GDISP_DRIVER_VMT			GDISPVMT_SSD1351
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

#include "board_SSD1351.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT		128
#endif
#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH		128
#endif
#ifndef GDISP_INITIAL_CONTRAST
	#define GDISP_INITIAL_CONTRAST	100
#endif
#ifndef GDISP_INITIAL_BACKLIGHT
	#define GDISP_INITIAL_BACKLIGHT	100
#endif

#include "SSD1351.h"

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

// Some common routines and macros
#define dummy_read(g)				{ volatile uint16_t dummy; dummy = read_data(g); (void) dummy; }
#define write_reg(g, reg, data)		{ write_cmd(g, reg); write_data(g, data); }

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
	gfxSleepMilliseconds(20);
	setpin_reset(g, FALSE);
	gfxSleepMilliseconds(20);

	// Get the bus for the following initialisation commands
	acquire_bus(g);

	write_reg(g, 0xFD, 0x12); // unlock OLED driver IC
	write_reg(g, 0xFD, 0xB1); // make commands A1, B1, B3, BB, BE, C1 accesible in unlocked state
	write_cmd(g, 0xAE); // sleep mode ON (display off)
	write_reg(g, 0xB3, 0xF1); // Front clock divider / osc freq - Osc = 0xF; div = 2
	write_reg(g, 0xCA, 127); // set MUX ratio
	write_reg(g, 0xA0, 0b01110100);	 // Set re-map / color depth
	// [0] : address increment (0: horizontal, 1: vertical, reset 0)
	// [1] : column remap (0: 0..127, 1: 127..0, reset 0)
	// [2] : color remap (0: A->B->C, 1: C->B->A, reset 0)
	// [3] : reserved
	// [4] : column scan direction (0: top->down, 1: bottom->up, reset 0)
	// [5] : odd/even split COM (0: disable, 1: enable, reset 1)
	// [6..7] : color depth (00,01: 65k, 10: 262k, 11: 262k format 2)

	write_cmd(g, 0x15); // Set Column address
	write_data(g, 0x00); // start
	write_data(g, 0x7F); // end

	write_cmd(g, 0x75); // set row address
	write_data(g, 0x00); // start
	write_data(g, 0x7F); // end

	write_reg(g, 0xA1, 0x00); // set display start line - 0
	write_reg(g, 0xA2, 0x00); // set display offset - 0
	write_reg(g, 0xB5, 0x00); // set GPIO - both HiZ, input disabled
	write_reg(g, 0xAB, 0x01); // enable internal VDD regulator
	write_reg(g, 0xB1, 0x32); // set reset / pre-charge period - phase 2: 3 DCLKs, phase 1: 5 DCLKs
	write_reg(g, 0xBE, 0x05); // set VComH voltage - 0.82*Vcc
	write_reg(g, 0xBB, 0x17); // set pre-charge voltage - 0.6*Vcc
	write_cmd(g, 0xA6); // set display mode: reset to normal display

	write_cmd(g, 0xC1); // set contrast current for A,B,C
	write_data(g, 0xC8);
	write_data(g, 0x80);
	write_data(g, 0xC8);

	write_reg(g, 0xC7, 0x0F); // master contrast current control - no change

	write_cmd(g, 0xB4); // set segment low voltage
	write_data(g, 0xA0); // external VSL
	write_data(g, 0xB5); // hard value
	write_data(g, 0x55); // hard value

	write_reg(g, 0xB6, 0x01); // set second pre-charge period - 1 DCLKs
	write_cmd(g, 0xAF); // sleep mode OFF (display on)
	write_cmd(g, 0x5C); // write to RAM

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
		write_cmd(g, SSD1351_SET_COLUMN_ADDRESS);
		write_data(g, g->p.x);
		write_data(g, g->p.x + g->p.cx - 1);
		write_cmd(g, SSD1351_SET_ROW_ADDRESS);
		write_data(g, g->p.y);
		write_data(g, g->p.y + g->p.cy - 1);
		write_cmd(g, SSD1351_WRITE_RAM);
	}
	LLDSPEC	void gdisp_lld_write_color(GDisplay *g) {
		LLDCOLOR_TYPE	c;

		c = gdispColor2Native(g->p.color);
		write_data(g, c >> 8);
		write_data(g, c & 0xFF);
	}
	LLDSPEC	void gdisp_lld_write_stop(GDisplay *g) {
		release_bus(g);
	}
#endif

#if GDISP_HARDWARE_STREAM_READ
	#error "SSD1351 - Stream Read is not supported yet"
	LLDSPEC	void gdisp_lld_read_start(GDisplay *g) {
		acquire_bus(g);
		//set_viewport(g);
		//write_index(g, 0x2E);
		setreadmode(g);
		//dummy_read(g);
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
	#error "SSD1351 - Hardware control is not supported yet"
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
				//TODO
				release_bus(g);
				break;
			case powerOn:
				acquire_bus(g);
				//TODO
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
