/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GDISP

#define GDISP_DRIVER_VMT			GDISPVMT_SSD2119
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

#include "board_SSD2119.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT		240
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
#ifndef GDISP_USE_DMA
	#define GDISP_USE_DMA			FALSE
#endif
#ifndef GDISP_NO_DMA_FROM_STACK
	#define GDISP_NO_DMA_FROM_STACK	FALSE
#endif

#include "ssd2119.h"

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

// Some common routines and macros
#define dummy_read(g)				{ volatile uint16_t dummy; dummy = read_data(g); (void) dummy; }
#define write_reg(g, reg, data)		{ write_index(g, reg); write_data(g, data); }

static void set_cursor(GDisplay* g) {
	/* Reg SSD2119_REG_X_RAM_ADDR is 9 bit value
	 * Reg SSD2119_REG_Y_RAM_ADDR is an 8 bit
	 * Use a bit mask to make sure they are not set too high
	 */
	switch(g->g.Orientation) {
		default:
		case GDISP_ROTATE_0:
			write_reg(g, SSD2119_REG_X_RAM_ADDR, g->p.x & 0x01FF);
			write_reg(g, SSD2119_REG_Y_RAM_ADDR, g->p.y & 0x00FF);
			break;
		case GDISP_ROTATE_90:
			write_reg(g, SSD2119_REG_X_RAM_ADDR, g->p.y & 0x01FF);
			write_reg(g, SSD2119_REG_Y_RAM_ADDR, (GDISP_SCREEN_HEIGHT-1 - g->p.x) & 0x00FF);
			break;
		case GDISP_ROTATE_180:
			write_reg(g, SSD2119_REG_X_RAM_ADDR, (GDISP_SCREEN_WIDTH-1 - g->p.x) & 0x01FF);
			write_reg(g, SSD2119_REG_Y_RAM_ADDR, (GDISP_SCREEN_HEIGHT-1 - g->p.y) & 0x00FF);
			break;
		case GDISP_ROTATE_270:
			write_reg(g, SSD2119_REG_X_RAM_ADDR, (GDISP_SCREEN_WIDTH-1 - g->p.y) & 0x01FF);
			write_reg(g, SSD2119_REG_Y_RAM_ADDR, g->p.x & 0x00FF);
			break;
	}
	write_index(g, SSD2119_REG_RAM_DATA);
}

static void set_viewport(GDisplay* g) {
	/* Reg 0x44 - Vertical RAM address position
	 * 		Upper Byte - VEA
	 * 		Lower Byte - VSA
	 * 		0 <= VSA <= VEA <= 0xEF
	 * Reg 0x45,0x46 - Horizontal RAM address position
	 * 		Lower 9 bits gives 0-511 range in each value, HSA and HEA respectively
	 * 		0 <= HSA <= HEA <= 0x13F
	 */
	switch(g->g.Orientation) {
		default:
		case GDISP_ROTATE_0:
			write_reg(g, SSD2119_REG_V_RAM_POS,   (((g->p.y + g->p.cy - 1) << 8) & 0xFF00 ) | (g->p.y & 0x00FF));
			write_reg(g, SSD2119_REG_H_RAM_START, (g->p.x & 0x01FF));
			write_reg(g, SSD2119_REG_H_RAM_END,   (g->p.x + g->p.cx - 1) & 0x01FF);
			break;
		case GDISP_ROTATE_90:
			write_reg(g, SSD2119_REG_V_RAM_POS,   (((GDISP_SCREEN_HEIGHT-1 - g->p.x) & 0x00FF) << 8) | ((GDISP_SCREEN_HEIGHT - (g->p.x + g->p.cx)) & 0x00FF));
			write_reg(g, SSD2119_REG_H_RAM_START, (g->p.y & 0x01FF));
			write_reg(g, SSD2119_REG_H_RAM_END,   (g->p.y + g->p.cy - 1) & 0x01FF);
			break;
		case GDISP_ROTATE_180:
			write_reg(g, SSD2119_REG_V_RAM_POS,   (((GDISP_SCREEN_HEIGHT-1 - g->p.y) & 0x00FF) << 8) | ((GDISP_SCREEN_HEIGHT - (g->p.y + g->p.cy)) & 0x00FF));
			write_reg(g, SSD2119_REG_H_RAM_START, (GDISP_SCREEN_WIDTH - (g->p.x + g->p.cx)) & 0x01FF);
			write_reg(g, SSD2119_REG_H_RAM_END,   (GDISP_SCREEN_WIDTH-1 - g->p.x) & 0x01FF);
			break;
		case GDISP_ROTATE_270:
			write_reg(g, SSD2119_REG_V_RAM_POS,   (((g->p.x + g->p.cx - 1) << 8) & 0xFF00 ) | (g->p.x & 0x00FF));
			write_reg(g, SSD2119_REG_H_RAM_START, (GDISP_SCREEN_WIDTH - (g->p.y + g->p.cy)) & 0x01FF);
			write_reg(g, SSD2119_REG_H_RAM_END,   (GDISP_SCREEN_WIDTH-1 - g->p.y) & 0x01FF);
			break;
	}
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

LLDSPEC bool_t gdisp_lld_init(GDisplay* g) {
	// no private area for this controller
	g->priv = 0;

	// initialise the board interface
	init_board(g);

	// Hardware reset
	setpin_reset(g, TRUE);
	gfxSleepMilliseconds(20);
	setpin_reset(g, FALSE);
	gfxSleepMilliseconds(20);

	// Get the bus for the following initialisation commands
	acquire_bus(g);

	// Enter sleep mode (if we are not already there).
	write_reg(g, SSD2119_REG_SLEEP_MODE_1, 0x0001);
	gfxSleepMicroseconds(5);

	// Set initial power parameters.
	write_reg(g, SSD2119_REG_PWR_CTRL_5, 0x00B2);
	gfxSleepMicroseconds(5);
	write_reg(g, SSD2119_REG_VCOM_OTP_1, 0x0006);
	gfxSleepMicroseconds(5);

	// Start the oscillator.
	write_reg(g, SSD2119_REG_OSC_START, 0x0001);
	gfxSleepMicroseconds(5);

	// Set pixel format and basic display orientation (scanning direction).
	write_reg(g, SSD2119_REG_OUTPUT_CTRL, 0x30EF);
	gfxSleepMicroseconds(5);
	write_reg(g, SSD2119_REG_LCD_DRIVE_AC_CTRL, 0x0600);
	gfxSleepMicroseconds(5);

	// Exit sleep mode.
	write_reg(g, SSD2119_REG_SLEEP_MODE_1, 0x0000);
	gfxSleepMicroseconds(5);

	// Configure pixel color format and MCU interface parameters.
	write_reg(g, SSD2119_REG_ENTRY_MODE, 0x6830); // ENTRY_MODE_DEFAULT
	gfxSleepMicroseconds(5);

	// Set analog parameters.
	write_reg(g, SSD2119_REG_SLEEP_MODE_2, 0x0999);
	gfxSleepMicroseconds(5);
	write_reg(g, SSD2119_REG_ANALOG_SET, 0x3800);
	gfxSleepMicroseconds(5);

	// Enable the display.
	write_reg(g, SSD2119_REG_DISPLAY_CTRL, 0x0033);
	gfxSleepMicroseconds(5);

	// Set VCIX2 voltage to 6.1V.
	write_reg(g, SSD2119_REG_PWR_CTRL_2, 0x0005);
	gfxSleepMicroseconds(5);

	// Configure gamma correction.
	write_reg(g, SSD2119_REG_GAMMA_CTRL_1, 0x0000);
	gfxSleepMicroseconds(5);
	write_reg(g, SSD2119_REG_GAMMA_CTRL_2, 0x0303);
	gfxSleepMicroseconds(5);
	write_reg(g, SSD2119_REG_GAMMA_CTRL_3, 0x0407);
	gfxSleepMicroseconds(5);
	write_reg(g, SSD2119_REG_GAMMA_CTRL_4, 0x0301);
	gfxSleepMicroseconds(5);
	write_reg(g, SSD2119_REG_GAMMA_CTRL_5, 0x0301);
	gfxSleepMicroseconds(5);
	write_reg(g, SSD2119_REG_GAMMA_CTRL_6, 0x0403);
	gfxSleepMicroseconds(5);
	write_reg(g, SSD2119_REG_GAMMA_CTRL_7, 0x0707);
	gfxSleepMicroseconds(5);
	write_reg(g, SSD2119_REG_GAMMA_CTRL_8, 0x0400);
	gfxSleepMicroseconds(5);
	write_reg(g, SSD2119_REG_GAMMA_CTRL_9, 0x0a00);
	gfxSleepMicroseconds(5);
	write_reg(g, SSD2119_REG_GAMMA_CTRL_10, 0x1000);
	gfxSleepMicroseconds(5);

	// Configure Vlcd63 and VCOMl.
	write_reg(g, SSD2119_REG_PWR_CTRL_3, 0x000A);
	gfxSleepMicroseconds(5);
	write_reg(g, SSD2119_REG_PWR_CTRL_4, 0x2E00);
	gfxSleepMicroseconds(5);

	// Set the display size and ensure that the GRAM window is set to allow access to the full display buffer.
	write_reg(g, SSD2119_REG_V_RAM_POS, (GDISP_SCREEN_HEIGHT - 1) << 8);
	gfxSleepMicroseconds(5);
	write_reg(g, SSD2119_REG_H_RAM_START, 0x0000);
	gfxSleepMicroseconds(5);
	write_reg(g, SSD2119_REG_H_RAM_END, GDISP_SCREEN_WIDTH - 1);
	gfxSleepMicroseconds(5);

	write_reg(g, SSD2119_REG_X_RAM_ADDR, 0x00);
	gfxSleepMicroseconds(5);
	write_reg(g, SSD2119_REG_Y_RAM_ADDR, 0x00);
	gfxSleepMicroseconds(5);

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
	LLDSPEC void gdisp_lld_write_start(GDisplay* g) {
		acquire_bus(g);
		set_viewport(g);
	}
	LLDSPEC void gdisp_lld_write_color(GDisplay* g) {
		write_data(g, gdispColor2Native(g->p.color));
	}
	LLDSPEC void gdisp_lld_write_stop(GDisplay* g) {
		release_bus(g);
	}
	LLDSPEC void gdisp_lld_write_pos(GDisplay* g) {
		set_cursor(g);
	}
#endif

#if GDISP_HARDWARE_STREAM_READ
	LLDSPEC void gdisp_lld_read_start(GDisplay* g) {
		acquire_bus(g);
		set_viewport(g);
		set_cursor(g);
		setreadmode(g);
		dummy_read(g);
	}
	LLDSPEC color_t gdisp_lld_read_color(GDisplay* g) {
		uint16_t	data;

		data = read_data(g);
		return gdispNative2Color(data);
	}
	LLDSPEC void gdisp_lld_read_stop(GDisplay* g) {
		setwritemode(g);
		release_bus(g);
	}
#endif

#if GDISP_HARDWARE_FILLS && GDISP_USE_DMA
	LLDSPEC void gdisp_lld_fill_area(GDisplay* g) {
		#if GDISP_NO_DMA_FROM_STACK
			static LLDCOLOR_TYPE	c;
		#else
			LLDCOLOR_TYPE	c;
		#endif

		c = gdispColor2Native(g->p.color);
		acquire_bus(g);
		set_viewport(g);
		set_cursor(g);	
		dma_with_noinc(g, &c, g->p.cx * g->p.cy);
		release_bus(g);
	}
#endif

#if GDISP_HARDWARE_BITFILLS && GDISP_USE_DMA
	#if GDISP_PIXELFORMAT != GDISP_LLD_PIXELFORMAT
		#error "GDISP: SSD2119: BitBlit is only available in RGB565 pixel format"
	#endif

	LLDSPEC void gdisp_lld_blit_area(GDisplay* g) {
		pixel_t*	buffer;
		coord_t		ycnt;

		buffer = (pixel_t*)g->p.ptr + g->p.x1 + g->p.y1 * g->p.x2;

		acquire_bus(g);
		set_viewport(g);
		set_cursor(g);

		if (g->p.x2 == g->p.cx) {
			dma_with_inc(g, buffer, g->p.cx * g->p.cy);
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
			case powerDeepSleep:
				acquire_bus(g);
				write_reg(g, SSD2119_REG_SLEEP_MODE_1,	0x0001);	// Enter sleep mode
				write_reg(g, SSD2119_REG_SLEEP_MODE_2, 0x2999);		// Enable deep sleep function
				write_reg(g, SSD2119_REG_DISPLAY_CTRL,	0x0000);	// Display off
				if ((powermode_t)g->p.ptr == powerOff)
					write_reg(g, SSD2119_REG_OSC_START,	0x0000);	// Turn off oscillator
				release_bus(g);
				set_backlight(g, 0);
				break;
			case powerSleep:
				acquire_bus(g);
				write_reg(g, SSD2119_REG_SLEEP_MODE_1, 0x0001);		// Enter sleep mode
				write_reg(g, SSD2119_REG_DISPLAY_CTRL, 0x0000);		// Display off
				release_bus(g);
				set_backlight(g, 0);
				break;
			case powerOn:
				acquire_bus(g);
				if (g->g.Powermode == powerOff) {
					write_reg(g, SSD2119_REG_OSC_START, 0x0001);	// Start the oscillator
					gfxSleepMicroseconds(5);
					write_reg(g, SSD2119_REG_SLEEP_MODE_2, 0x0999);	// Disable deep sleep function
				} else if (g->g.Powermode == powerDeepSleep)
					write_reg(g, SSD2119_REG_SLEEP_MODE_2, 0x0999);	// Disable deep sleep function
				write_reg(g, SSD2119_REG_SLEEP_MODE_1, 0x0000);		// Leave sleep mode
				write_reg(g, SSD2119_REG_DISPLAY_CTRL, 0x0033);		// Display on
				release_bus(g);
				gfxSleepMicroseconds(25);
				set_backlight(g, g->g.Backlight);
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
				write_reg(g, SSD2119_REG_ENTRY_MODE, 0x6830);
				release_bus(g);
				g->g.Height = GDISP_SCREEN_HEIGHT;
				g->g.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_90:
				acquire_bus(g);
				/* ID = 01 AM = 1 */
				write_reg(g, SSD2119_REG_ENTRY_MODE, 0x6818);
				release_bus(g);
				g->g.Height = GDISP_SCREEN_WIDTH;
				g->g.Width = GDISP_SCREEN_HEIGHT;
				break;
			case GDISP_ROTATE_180:
				acquire_bus(g);
				/* ID = 00 AM = 0 */
				write_reg(g, SSD2119_REG_ENTRY_MODE, 0x6800);
				release_bus(g);
				g->g.Height = GDISP_SCREEN_HEIGHT;
				g->g.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_270:
				acquire_bus(g);
				/* ID = 10 AM = 1 */
				write_reg(g, SSD2119_REG_ENTRY_MODE, 0x6828);
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
