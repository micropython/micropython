/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GDISP

/* This controller is only ever used with a 240 x 320 display */
#if defined(GDISP_SCREEN_HEIGHT)
	#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
	#undef GDISP_SCREEN_HEIGHT
#endif
#if defined(GDISP_SCREEN_WIDTH)
	#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
	#undef GDISP_SCREEN_WIDTH
#endif

#define GDISP_DRIVER_VMT	GDISPVMT_LGDP4532
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

#include "board_LGDP4532.h"

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
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

// Some common routines and macros
#define dummy_read(g)			{ volatile uint16_t dummy; dummy = read_data(g); (void) dummy; }
#define write_reg(g, reg, data)		{ write_index(g, reg); write_data(g, data); }

static void set_cursor(GDisplay *g) {
	switch(g->g.Orientation) {
		default:
		case GDISP_ROTATE_0:
		case GDISP_ROTATE_180:
			write_reg(g, 0x20, g->p.x);
			write_reg(g, 0x21, g->p.y);
			break;

		case GDISP_ROTATE_90:
		case GDISP_ROTATE_270:
			write_reg(g, 0x20, g->p.y);
			write_reg(g, 0x21, g->p.x);
			break;
	}
	write_index(g, 0x22);
}

static void set_viewport(GDisplay* g) {
	switch(g->g.Orientation) {
		default:
		case GDISP_ROTATE_0:
		case GDISP_ROTATE_180:
			write_reg(g, 0x50, g->p.x);
			write_reg(g, 0x51, g->p.x + g->p.cx - 1);
			write_reg(g, 0x52, g->p.y);
			write_reg(g, 0x53, g->p.y + g->p.cy - 1);
			break;

		case GDISP_ROTATE_90:
		case GDISP_ROTATE_270:
			write_reg(g, 0x50, g->p.y);
			write_reg(g, 0x51, g->p.y + g->p.cy - 1);
			write_reg(g, 0x52, g->p.x);
			write_reg(g, 0x53, g->p.x + g->p.cx - 1);
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
	gfxSleepMilliseconds(1);
	setpin_reset(g, FALSE);
	gfxSleepMilliseconds(2);

	acquire_bus(g);
	setwritemode(g);

	// chinese code starts here
        //############# void Power_Set(void) ################//
	write_reg(g, 0x00, 0x0001);
	gfxSleepMilliseconds(10);

	write_reg(g, 0x15, 0x0030);
	write_reg(g, 0x11, 0x0040);
	write_reg(g, 0x10, 0x1628);
	write_reg(g, 0x12, 0x0000);
	write_reg(g, 0x13, 0x104d);
	gfxSleepMilliseconds(10);
	write_reg(g, 0x12, 0x0010);
	gfxSleepMilliseconds(10);
	write_reg(g, 0x10, 0x2620);
	write_reg(g, 0x13, 0x344d); //304d
	gfxSleepMilliseconds(10);

	write_reg(g, 0x01, 0x0100);
	write_reg(g, 0x02, 0x0300);
	write_reg(g, 0x03, 0x1038);//0x1030
	write_reg(g, 0x08, 0x0604);
	write_reg(g, 0x09, 0x0000);
	write_reg(g, 0x0A, 0x0008);

	write_reg(g, 0x41, 0x0002);
	write_reg(g, 0x60, 0xA700);
	write_reg(g, 0x61, 0x0001);
	write_reg(g, 0x90, 0x0182);
	write_reg(g, 0x93, 0x0001);
	write_reg(g, 0xa3, 0x0010);
	gfxSleepMilliseconds(10);

	//################# void Gamma_Set(void) ####################//
	write_reg(g, 0x30, 0x0000);
	write_reg(g, 0x31, 0x0502);
	write_reg(g, 0x32, 0x0307);
	write_reg(g, 0x33, 0x0305);
	write_reg(g, 0x34, 0x0004);
	write_reg(g, 0x35, 0x0402);
	write_reg(g, 0x36, 0x0707);
	write_reg(g, 0x37, 0x0503);
	write_reg(g, 0x38, 0x1505);
	write_reg(g, 0x39, 0x1505);
	gfxSleepMilliseconds(10);

	//################## void Display_ON(void) ####################//
	write_reg(g, 0x07, 0x0001);
	gfxSleepMilliseconds(10);
	write_reg(g, 0x07, 0x0021);
	write_reg(g, 0x07, 0x0023);
	gfxSleepMilliseconds(10);
	write_reg(g, 0x07, 0x0033);
	gfxSleepMilliseconds(10);
	write_reg(g, 0x07, 0x0133);
	// chinese code ends here

	// Finish Init
	post_init_board(g);

 	// Release the bus
	release_bus(g);

	// Turn on the backlight
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
		case GDISP_CONTROL_POWER:
			if (g->g.Powermode == (powermode_t)g->p.ptr)
				return;
			switch((powermode_t)g->p.ptr) {
			case powerOff:
				acquire_bus(g);
				write_reg(g, 0x07, 0x0000);
				write_reg(g, 0x10, 0x0000);
				write_reg(g, 0x11, 0x0000);
				write_reg(g, 0x12, 0x0000);
				write_reg(g, 0x13, 0x0000);
				release_bus(g);
				set_backlight(g, 0);
				break;

			case powerOn:
				//*************Power On sequence ******************//
				acquire_bus(g);
				write_reg(g, 0x10, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
				write_reg(g, 0x11, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
				write_reg(g, 0x12, 0x0000); /* VREG1OUT voltage */
				write_reg(g, 0x13, 0x0000); /* VDV[4:0] for VCOM amplitude */
				gfxSleepMilliseconds(200);            /* Dis-charge capacitor power voltage */
				write_reg(g, 0x10, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
				write_reg(g, 0x11, 0x0147); /* DC1[2:0], DC0[2:0], VC[2:0] */
				gfxSleepMilliseconds(50);
				write_reg(g, 0x12, 0x013C); /* VREG1OUT voltage */
				gfxSleepMilliseconds(50);
				write_reg(g, 0x13, 0x0E00); /* VDV[4:0] for VCOM amplitude */
				write_reg(g, 0x29, 0x0009); /* VCM[4:0] for VCOMH */
				gfxSleepMilliseconds(50);
				write_reg(g, 0x07, 0x0173); /* 262K color and display ON */
				release_bus(g);
				set_backlight(g, g->g.Backlight);
				break;

			case powerSleep:
				acquire_bus(g);
				write_reg(g, 0x07, 0x0000); /* display OFF */
				write_reg(g, 0x10, 0x0000); /* SAP, BT[3:0], APE, AP, DSTB, SLP */
				write_reg(g, 0x11, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
				write_reg(g, 0x12, 0x0000); /* VREG1OUT voltage */
				write_reg(g, 0x13, 0x0000); /* VDV[4:0] for VCOM amplitude */
				gfxSleepMilliseconds(200); /* Dis-charge capacitor power voltage */
				write_reg(g, 0x10, 0x0002); /* SAP, BT[3:0], APE, AP, DSTB, SLP */
				release_bus(g);
				set_backlight(g, g->g.Backlight);
				break;

			case powerDeepSleep:
				acquire_bus(g);
				write_reg(g, 0x07, 0x0000); /* display OFF */
				write_reg(g, 0x10, 0x0000); /* SAP, BT[3:0], APE, AP, DSTB, SLP */
				write_reg(g, 0x11, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
				write_reg(g, 0x12, 0x0000); /* VREG1OUT voltage */
				write_reg(g, 0x13, 0x0000); /* VDV[4:0] for VCOM amplitude */
				gfxSleepMilliseconds(200); /* Dis-charge capacitor power voltage */
				write_reg(g, 0x10, 0x0004); /* SAP, BT[3:0], APE, AP, DSTB, SLP */
				release_bus(g);
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
					write_reg(g, 0x01, 0x0100);
					write_reg(g, 0x03, 0x1030);
					write_reg(g, 0x60, 0x2700);
					release_bus(g);
					g->g.Height = GDISP_SCREEN_HEIGHT;
					g->g.Width = GDISP_SCREEN_WIDTH;
					break;

				case GDISP_ROTATE_90:
					acquire_bus(g);
					write_reg(g, 0x01, 0x0100);
					write_reg(g, 0x03, 0x1038);
					write_reg(g, 0x60, 0xA700);
					release_bus(g);
					g->g.Height = GDISP_SCREEN_WIDTH;
					g->g.Width = GDISP_SCREEN_HEIGHT;
					break;

				case GDISP_ROTATE_180:
					acquire_bus(g);
					write_reg(g, 0x01, 0x0000);
					write_reg(g, 0x03, 0x1030);
					write_reg(g, 0x60, 0xa700);
					release_bus(g);
					g->g.Height = GDISP_SCREEN_HEIGHT;
					g->g.Width = GDISP_SCREEN_WIDTH;
					break;
	
				case GDISP_ROTATE_270:
					acquire_bus(g);
					write_reg(g, 0x01, 0x0000);
					write_reg(g, 0x03, 0x1038);
					write_reg(g, 0x60, 0x2700);
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
		
		default:
			return;
		}
	}
#endif

#endif /* GFX_USE_GDISP */
