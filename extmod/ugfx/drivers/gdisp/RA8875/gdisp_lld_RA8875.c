/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

#define GDISP_DRIVER_VMT			GDISPVMT_RA8875
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

/* include the users board interface */
#include "board_RA8875.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT		272
#endif
#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH		480
#endif
#ifndef GDISP_INITIAL_CONTRAST
	#define GDISP_INITIAL_CONTRAST	50
#endif
#ifndef GDISP_INITIAL_BACKLIGHT
	#define GDISP_INITIAL_BACKLIGHT	74
#endif

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

// Some common routines and macros
#define dummy_read(g)					{ volatile uint16_t dummy; dummy = read_data(g); (void) dummy; }
#define write_reg8(g, reg, data)		{ write_index(g, reg); write_data(g, data); }
#define write_reg16(g, reg, data)		{ write_index(g, reg); write_data(g, data); write_index(g, reg+1); write_data(g, (data)>>8); }
#define write_reg8x2(g, reg, d1, d2)	{ write_index(g, reg); write_data(g, d1); write_data(g, d2); }

static GFXINLINE void set_cursor(GDisplay *g) {
	write_reg16(g, 0x46, g->p.x);
	write_reg16(g, 0x48, g->p.y);
	write_index(g, 0x02);
}

static GFXINLINE void set_viewport(GDisplay* g) {
	write_reg16(g, 0x30, g->p.x);				//HSAW0 & HSAW1
	write_reg16(g, 0x34, g->p.x+g->p.cx-1);		//HEAW0 & HEAW1
	write_reg16(g, 0x32, g->p.y);				//VSAW0 & VSAW1
	write_reg16(g, 0x36, g->p.y+g->p.cy-1);		//VEAW0 & VEAW1
}

// On this controller the back-light is controlled by the controllers internal PWM
//	which is why it is in this file rather than the board file.
static GFXINLINE void set_backlight(GDisplay* g, uint8_t percent) {
	uint8_t temp;

	//Work in progress: the RA8875 has a built-in PWM, its output can
	//be used by a Dynamic Background Control or by a host (user)

	// Enable PWM1
	write_index(g, 0x8a);						//MCLR
	setreadmode(g);
	temp = read_data(g);
	setwritemode(g);
	temp |= 1<<7 ;
	write_data(g, temp);

	// PWM1 function select
	write_index(g, 0x8a);						//MCLR
	setreadmode(g);
	temp = read_data(g);
	setwritemode(g);
	temp &= ~(1<<4);
	write_data(g, temp);

	// PWM1 Clock ratio
	write_index(g, 0x8a);						//MCLR
	setreadmode(g);
	temp = read_data(g);
	setwritemode(g);
	temp &= 0xf0;
	temp |= 0x0b & 0x0f;
	write_data(g, temp);

	// PWM1 Write duty cycle
	write_reg8(g, 0x8b, 54+percent);			// PTNO: Also change percent to range from 0x00 to 0xFF
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

	// Soft reset
	write_reg8x2(g, 0x01, 0x01, 0x00);	gfxSleepMilliseconds(1);

	// PLL config
	write_reg8(g, 0x88, 0x08);			gfxSleepMilliseconds(1);
	write_reg8(g, 0x89, 0x02);			gfxSleepMilliseconds(1);

	write_reg8(g, 0x10, 0x0F);			//SYSR   bit[4:3]=00 256 color  bit[2:1]=  00 8bit MPU interface
										//		 0x0F = 16bit MCU interface and 65k color display

	write_reg8(g, 0x04, 0x82);			gfxSleepMilliseconds(1);	//set PCLK inverse

	// Horizontal set
	write_reg8(g, 0x14, GDISP_SCREEN_WIDTH/8-1);	//HDWR: Horizontal Display Width Setting Bit[6:0] - pixels = (HDWR + 1)*8
    write_reg8(g, 0x15, 0x00);						//Horizontal Non-Display Period Fine Tuning Option Register (HNDFTR) - HNDFT = [3:0]
    write_reg8(g, 0x16, 0x01);						//HNDR: Horizontal Non-Display Period Bit[4:0] - pixels = (HNDR + 1)*8
    write_reg8(g, 0x17, 0x00);						//HSTR: HSYNC Start Position[4:0] - Position(PCLK) = (HSTR + 1)*8
    write_reg8(g, 0x18, 0x05);						//HPWR: HSYNC Polarity, The period width of HSYNC. Width [4:0] width(PCLK) = (HPWR + 1)*8

    // Vertical set
    write_reg16(g, 0x19, GDISP_SCREEN_HEIGHT-1);	//VDHR0,1: Vertical Display Height = VDHR + 1
    write_reg16(g, 0x1b, 0x0002);					//VNDR0,1: Vertical Non-Display Period Bit = (VSTR + 1)
    write_reg16(g, 0x1d, 0x0007);					//VSTR0,1: VSYNC Start Position = (VSTR + 1)
    write_reg8(g, 0x1f, 0x09);						//VPWR: VSYNC Polarity, VSYNC Pulse Width[6:0] - Width(PCLK) = (VPWR + 1)

	// Active window  set
	write_reg16(g, 0x30, 0);						//HSAW0 & HSAW1
	write_reg16(g, 0x34, GDISP_SCREEN_WIDTH-1);		//HEAW0 & HEAW1
	write_reg16(g, 0x32, 0);						//VSAW0 & VSAW1
	write_reg16(g, 0x36, GDISP_SCREEN_HEIGHT-1);	//VEAW0 & VEAW1

	// Display ON
	write_reg8(g, 0x01, 0x80);						//PWRR

	// GPO0 DISP high
	write_reg8(g, 0x13, 0x01);						//GPO

	// Set initial back-light
	set_backlight(g, GDISP_INITIAL_BACKLIGHT);
	
	// Change timings for faster access
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

		#if 0
			case GDISP_CONTROL_ORIENTATION:
				if (g->g.Orientation == (orientation_t)g->p.ptr)
					return;
				switch((orientation_t)g->p.ptr) {
				case GDISP_ROTATE_0:
					acquire_bus(g);
					// TODO
					release_bus(g);
					g->g.Height = GDISP_SCREEN_HEIGHT;
					g->g.Width = GDISP_SCREEN_WIDTH;
					break;
				case GDISP_ROTATE_90:
					acquire_bus(g);
					// TODO
					release_bus(g);
					g->g.Height = GDISP_SCREEN_WIDTH;
					g->g.Width = GDISP_SCREEN_HEIGHT;
					break;
				case GDISP_ROTATE_180:
					acquire_bus(g);
					// TODO
					release_bus(g);
					g->g.Height = GDISP_SCREEN_HEIGHT;
					g->g.Width = GDISP_SCREEN_WIDTH;
					break;
				case GDISP_ROTATE_270:
					acquire_bus(g);
					// TODO
					release_bus(g);
					g->g.Height = GDISP_SCREEN_WIDTH;
					g->g.Width = GDISP_SCREEN_HEIGHT;
					break;
				default:
					return;
				}
				g->g.Orientation = (orientation_t)g->p.ptr;
				return;
		#endif

        case GDISP_CONTROL_BACKLIGHT:
            if ((unsigned)g->p.ptr > 100)
            	g->p.ptr = (void *)100;
			acquire_bus(g);
            set_backlight(g, (unsigned)g->p.ptr);
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
