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

#define GDISP_DRIVER_VMT			GDISPVMT_ILI9320
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

#include "board_ILI9320.h"

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

#define dummy_read(g)				{ volatile uint16_t dummy; dummy = read_data(g); (void) dummy; }
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

static void set_viewport(GDisplay *g) {
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
	uint16_t cver;

	// No private area for this controller
	g->priv = 0;

	// Initialise the board interface
	init_board(g);

	/* Hardware reset */
	setpin_reset(g, TRUE);
	gfxSleepMicroseconds(1000);
	setpin_reset(g, FALSE);
	gfxSleepMicroseconds(1000);

	acquire_bus(g);
	write_index(g, 0);				// Get controller version
	setreadmode(g);
	dummy_read(g);
    cver = read_data(g);
	setwritemode(g);
    write_reg(g, 0x00, 0x0001); //start Int. osc
    gfxSleepMicroseconds(500);
    write_reg(g, 0x01, 0x0100); //Set SS bit (shift direction of outputs is from S720 to S1)
    write_reg(g, 0x02, 0x0700); //select  the line inversion
    write_reg(g, 0x03, 0x1038); //Entry mode(Horizontal : increment,Vertical : increment, AM=1)
    write_reg(g, 0x04, 0x0000); //Resize control(No resizing)
    write_reg(g, 0x08, 0x0202); //front and back porch 2 lines
    write_reg(g, 0x09, 0x0000); //select normal scan
    write_reg(g, 0x0A, 0x0000); //display control 4
    write_reg(g, 0x0C, 0x0000); //system interface(2 transfer /pixel), internal sys clock,
    write_reg(g, 0x0D, 0x0000); //Frame marker position
    write_reg(g, 0x0F, 0x0000); //selects clk, enable and sync signal polarity,
    write_reg(g, 0x10, 0x0000); //
    write_reg(g, 0x11, 0x0000); //power control 2 reference voltages = 1:1,
    write_reg(g, 0x12, 0x0000); //power control 3 VRH
    write_reg(g, 0x13, 0x0000); //power control 4 VCOM amplitude
    gfxSleepMicroseconds(500);
    write_reg(g, 0x10, 0x17B0); //power control 1 BT,AP
    write_reg(g, 0x11, 0x0137); //power control 2 DC,VC
    gfxSleepMicroseconds(500);
    write_reg(g, 0x12, 0x0139); //power control 3 VRH
    gfxSleepMicroseconds(500);
    write_reg(g, 0x13, 0x1d00); //power control 4 vcom amplitude
    write_reg(g, 0x29, 0x0011); //power control 7 VCOMH
    gfxSleepMicroseconds(500);
    write_reg(g, 0x30, 0x0007);
    write_reg(g, 0x31, 0x0403);
    write_reg(g, 0x32, 0x0404);
    write_reg(g, 0x35, 0x0002);
    write_reg(g, 0x36, 0x0707);
    write_reg(g, 0x37, 0x0606);
    write_reg(g, 0x38, 0x0106);
    write_reg(g, 0x39, 0x0007);
    write_reg(g, 0x3c, 0x0700);
    write_reg(g, 0x3d, 0x0707);
    write_reg(g, 0x20, 0x0000); //starting Horizontal GRAM Address
    write_reg(g, 0x21, 0x0000); //starting Vertical GRAM Address
    write_reg(g, 0x50, 0x0000); //Horizontal GRAM Start Position
    write_reg(g, 0x51, 0x00EF); //Horizontal GRAM end Position
    write_reg(g, 0x52, 0x0000); //Vertical GRAM Start Position
    write_reg(g, 0x53, 0x013F); //Vertical GRAM end Position
	switch (cver) {
		case 0x9320:
        	write_reg(g, 0x60, 0x2700); //starts scanning from G1, and 320 drive lines
        	break;
      	case 0x9325:
     		write_reg(g, 0x60, 0xA700); //starts scanning from G1, and 320 drive lines
			break;
	}

    write_reg(g, 0x61, 0x0001); //fixed base display
    write_reg(g, 0x6a, 0x0000); //no scroll
    write_reg(g, 0x90, 0x0010); //set Clocks/Line =16, Internal Operation Clock Frequency=fosc/1,
    write_reg(g, 0x92, 0x0000); //set gate output non-overlap period=0
    write_reg(g, 0x93, 0x0003); //set Source Output Position=3
    write_reg(g, 0x95, 0x0110); //RGB interface(Clocks per line period=16 clocks)
    write_reg(g, 0x97, 0x0110); //set Gate Non-overlap Period 0 locksc
    write_reg(g, 0x98, 0x0110); //
    write_reg(g, 0x07, 0x0173); //display On

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
					gfxSleepMicroseconds(2000);            /* Dis-charge capacitor power voltage */
					write_reg(g, 0x10, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
					write_reg(g, 0x11, 0x0147); /* DC1[2:0], DC0[2:0], VC[2:0] */
					gfxSleepMicroseconds(500);
					write_reg(g, 0x12, 0x013C); /* VREG1OUT voltage */
					gfxSleepMicroseconds(500);
					write_reg(g, 0x13, 0x0E00); /* VDV[4:0] for VCOM amplitude */
					write_reg(g, 0x29, 0x0009); /* VCM[4:0] for VCOMH */
					gfxSleepMicroseconds(500);
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
					gfxSleepMicroseconds(2000); /* Dis-charge capacitor power voltage */
					write_reg(g, 0x10, 0x0002); /* SAP, BT[3:0], APE, AP, DSTB, SLP */
					release_bus(g);

					set_backlight(g, 0);
					break;

				case powerDeepSleep:
					acquire_bus(g);
					write_reg(g, 0x07, 0x0000); /* display OFF */
					write_reg(g, 0x10, 0x0000); /* SAP, BT[3:0], APE, AP, DSTB, SLP */
					write_reg(g, 0x11, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
					write_reg(g, 0x12, 0x0000); /* VREG1OUT voltage */
					write_reg(g, 0x13, 0x0000); /* VDV[4:0] for VCOM amplitude */
					gfxSleepMicroseconds(2000); /* Dis-charge capacitor power voltage */
					write_reg(g, 0x10, 0x0004); /* SAP, BT[3:0], APE, AP, DSTB, SLP */
					release_bus(g);

					set_backlight(g, 0);
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
					write_reg(g, 0x03, 0x1038);
					write_reg(g, 0x60, 0x2700);
					release_bus(g);

					g->g.Height = GDISP_SCREEN_HEIGHT;
					g->g.Width = GDISP_SCREEN_WIDTH;
					break;

				case GDISP_ROTATE_90:
					acquire_bus(g);
					write_reg(g, 0x01, 0x0000);
					write_reg(g, 0x03, 0x1030);
					write_reg(g, 0x60, 0x2700);
					release_bus(g);

					g->g.Height = GDISP_SCREEN_WIDTH;
					g->g.Width = GDISP_SCREEN_HEIGHT;
					break;

				case GDISP_ROTATE_180:
					acquire_bus(g);
					write_reg(g, 0x01, 0x0000);
					write_reg(g, 0x03, 0x1030);
					write_reg(g, 0x60, 0x2700);
					release_bus(g);

					g->g.Height = GDISP_SCREEN_HEIGHT;
					g->g.Width = GDISP_SCREEN_WIDTH;
					break;

				case GDISP_ROTATE_270:
					acquire_bus(g);
					write_reg(g, 0x01, 0x0100);
					write_reg(g, 0x03, 0x1038);
					write_reg(g, 0x60, 0xA700);
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
