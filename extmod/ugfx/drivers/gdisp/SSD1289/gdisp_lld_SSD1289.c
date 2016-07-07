/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GDISP

#define GDISP_DRIVER_VMT			GDISPVMT_SSD1289
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

#include "board_SSD1289.h"

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
#ifndef GDISP_USE_DMA
	#define GDISP_USE_DMA			FALSE
#endif
#ifndef GDISP_NO_DMA_FROM_STACK
	#define GDISP_NO_DMA_FROM_STACK	FALSE
#endif

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

// Some common routines and macros
#define dummy_read(g)				{ volatile uint16_t dummy; dummy = read_data(g); (void) dummy; }
#define write_reg(g, reg, data)		{ write_index(g, reg); write_data(g, data); }

static void set_cursor(GDisplay *g) {
	/*
	 * Reg 0x004E is an 8 bit value - start x position
	 * Reg 0x004F is 9 bit - start y position
	 * Use a bit mask to make sure they are not set too high
	 */
	switch(g->g.Orientation) {
		default:
		case GDISP_ROTATE_0:
			write_reg(g, 0x4e, g->p.x & 0x00FF);
			write_reg(g, 0x4f, g->p.y & 0x01FF);
			break;
		case GDISP_ROTATE_90:
			write_reg(g, 0x4e, g->p.y & 0x00FF);
			write_reg(g, 0x4f, (GDISP_SCREEN_HEIGHT-1-g->p.x) & 0x01FF);
			break;
		case GDISP_ROTATE_180:
			write_reg(g, 0x4e, (GDISP_SCREEN_WIDTH-1-g->p.x) & 0x00FF);
			write_reg(g, 0x4f, (GDISP_SCREEN_HEIGHT-1-g->p.y) & 0x01FF);
			break;
		case GDISP_ROTATE_270:
			write_reg(g, 0x4e, (GDISP_SCREEN_WIDTH-1-g->p.y) & 0x00FF);
			write_reg(g, 0x4f, g->p.x & 0x01FF);
			break;
	}
	write_index(g, 0x22);
}

static void set_viewport(GDisplay* g) {
	/* Reg 0x44 - Horizontal RAM address position
	 * 		Upper Byte - HEA
	 * 		Lower Byte - HSA
	 * 		0 <= HSA <= HEA <= 0xEF
	 * Reg 0x45,0x46 - Vertical RAM address position
	 * 		Lower 9 bits gives 0-511 range in each value
	 * 		0 <= Reg(0x45) <= Reg(0x46) <= 0x13F
	 * Use a bit mask to make sure they are not set too high
	 */
	switch(g->g.Orientation) {
		default:
		case GDISP_ROTATE_0:
			write_reg(g, 0x44, (((g->p.x+g->p.cx-1) << 8) & 0xFF00 ) | (g->p.x & 0x00FF));
			write_reg(g, 0x45, g->p.y & 0x01FF);
			write_reg(g, 0x46, (g->p.y+g->p.cy-1) & 0x01FF);
			break;
		case GDISP_ROTATE_90:
			write_reg(g, 0x44, (((g->p.y+g->p.cy-1) << 8) & 0xFF00 ) | (g->p.y & 0x00FF));
			write_reg(g, 0x45, (GDISP_SCREEN_HEIGHT-(g->p.x+g->p.cx)) & 0x01FF);
			write_reg(g, 0x46, (GDISP_SCREEN_HEIGHT-1-g->p.x) & 0x01FF);
			break;
		case GDISP_ROTATE_180:
			write_reg(g, 0x44, (((GDISP_SCREEN_WIDTH-1-g->p.x) & 0x00FF) << 8) | ((GDISP_SCREEN_WIDTH - (g->p.x+g->p.cx)) & 0x00FF));
			write_reg(g, 0x45, (GDISP_SCREEN_HEIGHT-(g->p.y+g->p.cy)) & 0x01FF);
			write_reg(g, 0x46, (GDISP_SCREEN_HEIGHT-1-g->p.y) & 0x01FF);
			break;
		case GDISP_ROTATE_270:
			write_reg(g, 0x44, (((GDISP_SCREEN_WIDTH-1-g->p.y) & 0x00FF) << 8) | ((GDISP_SCREEN_WIDTH-(g->p.y+g->p.cy)) & 0x00FF));
			write_reg(g, 0x45, g->p.x & 0x01FF);
			write_reg(g, 0x46, (g->p.x+g->p.cx-1) & 0x01FF);
			break;
	}
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

	// Hardware reset
	setpin_reset(g, TRUE);
	gfxSleepMilliseconds(20);
	setpin_reset(g, FALSE);
	gfxSleepMilliseconds(20);

	// Get the bus for the following initialisation commands
	acquire_bus(g);
	
	write_reg(g, 0x00, 0x0001);		gfxSleepMicroseconds(5);
	write_reg(g, 0x03, 0xA8A4);    	gfxSleepMicroseconds(5);
	write_reg(g, 0x0C, 0x0000);    	gfxSleepMicroseconds(5);
	write_reg(g, 0x0D, 0x080C);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x0E, 0x2B00);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x1E, 0x00B0);    	gfxSleepMicroseconds(5);
	write_reg(g, 0x01, 0x2B3F);		gfxSleepMicroseconds(5);
    write_reg(g, 0x02, 0x0600);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x10, 0x0000);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x11, 0x6070);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x05, 0x0000);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x06, 0x0000);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x16, 0xEF1C);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x17, 0x0003);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x07, 0x0133);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x0B, 0x0000);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x0F, 0x0000);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x41, 0x0000);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x42, 0x0000);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x48, 0x0000);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x49, 0x013F);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x4A, 0x0000);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x4B, 0x0000);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x44, 0xEF00);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x45, 0x0000);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x46, 0x013F);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x30, 0x0707);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x31, 0x0204);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x32, 0x0204);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x33, 0x0502);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x34, 0x0507);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x35, 0x0204);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x36, 0x0204);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x37, 0x0502);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x3A, 0x0302);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x3B, 0x0302);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x23, 0x0000);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x24, 0x0000);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x25, 0x8000);    	gfxSleepMicroseconds(5);
    write_reg(g, 0x4f, 0x0000);		gfxSleepMicroseconds(5);
    write_reg(g, 0x4e, 0x0000);		gfxSleepMicroseconds(5);

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
		#if !GDISP_HARDWARE_STREAM_POS
			set_cursor(g);
		#endif
	}
	LLDSPEC	void gdisp_lld_write_color(GDisplay *g) {
		write_data(g, gdispColor2Native(g->p.color));
	}
	LLDSPEC	void gdisp_lld_write_stop(GDisplay *g) {
		release_bus(g);
	}
	#if GDISP_HARDWARE_STREAM_POS
		LLDSPEC void gdisp_lld_write_pos(GDisplay *g) {
			set_cursor(g);
		}
	#endif
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

#if GDISP_HARDWARE_FILLS && GDISP_USE_DMA
	LLDSPEC void gdisp_lld_fill_area(GDisplay *g) {
		#if GDISP_NO_DMA_FROM_STACK
			static LLDCOLOR_TYPE	c;
		#else
			LLDCOLOR_TYPE	c;
		#endif

		c = gdispColor2Native(g->p.color);
		acquire_bus(g);
		set_viewport(g);
		set_cursor(g);
		dma_with_noinc(g, &c, g->p.cx*g->p.cy);
		release_bus(g);
	}
#endif

#if GDISP_HARDWARE_BITFILLS && GDISP_USE_DMA
	#if GDISP_PIXELFORMAT != GDISP_LLD_PIXELFORMAT
		#error "GDISP: SSD1289: BitBlit is only available in RGB565 pixel format"
	#endif

	LLDSPEC void gdisp_lld_blit_area(GDisplay *g) {
		pixel_t		*buffer;
		coord_t		ycnt;

		buffer = (pixel_t *)g->p.ptr + g->p.x1 + g->p.y1 * g->p.x2;

		acquire_bus(g);
		set_viewport(g);
		set_cursor(g);
		if (g->p.x2 == g->p.cx) {
			dma_with_inc(g, buffer, g->p.cx*g->p.cy);
		} else {
			for (ycnt = g->p.cy; ycnt; ycnt--, buffer += g->p.x2)
				dma_with_inc(g, buffer, g->p.cy);
		}
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
				write_reg(g, 0x10, 0x0000);	// leave sleep mode
				write_reg(g, 0x07, 0x0000);	// halt operation
				write_reg(g, 0x00, 0x0000);	// turn off oscillator
				write_reg(g, 0x10, 0x0001);	// enter sleep mode
				release_bus(g);
				break;
			case powerOn:
				acquire_bus(g);
				write_reg(g, 0x10, 0x0000);	// leave sleep mode
				write_reg(g, 0x00, 0x0001);	// turn on oscillator
				gfxSleepMicroseconds(5);
				release_bus(g);
				break;
			case powerSleep:
				acquire_bus(g);
				write_reg(g, 0x10, 0x0001);	// enter sleep mode
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
				/* ID = 11 AM = 0 */
				write_reg(g, 0x11, 0x6070);
				release_bus(g);
				g->g.Height = GDISP_SCREEN_HEIGHT;
				g->g.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_90:
				acquire_bus(g);
				/* ID = 01 AM = 1 */
				write_reg(g, 0x11, 0x6058);
				release_bus(g);
				g->g.Height = GDISP_SCREEN_WIDTH;
				g->g.Width = GDISP_SCREEN_HEIGHT;
				break;
			case GDISP_ROTATE_180:
				acquire_bus(g);
				/* ID = 00 AM = 0 */
				write_reg(g, 0x11, 0x6040);
				release_bus(g);
				g->g.Height = GDISP_SCREEN_HEIGHT;
				g->g.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_270:
				acquire_bus(g);
				/* ID = 10 AM = 1 */
				write_reg(g, 0x11, 0x6068);
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
