/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GDISP

#define GDISP_DRIVER_VMT			GDISPVMT_HX8347D
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

#include "board_HX8347D.h"

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
/* Driver local functions.                                                   */
/*===========================================================================*/

#include "HX8347D.h"

#define write_reg(g, reg, data)		{ write_index(g, reg); write_data(g, data); }

static GFXINLINE void set_viewport(GDisplay* g) {
	write_reg(g, HX8347D_REG_SCL, g->p.x);
	write_reg(g, HX8347D_REG_SCH, g->p.x >> 8);
	write_reg(g, HX8347D_REG_ECL, g->p.x + g->p.cx -1);
	write_reg(g, HX8347D_REG_ECH, (g->p.x + g->p.cx -1) >> 8);
	write_reg(g, HX8347D_REG_SPL, g->p.y);
	write_reg(g, HX8347D_REG_SPH, g->p.y >> 8);
	write_reg(g, HX8347D_REG_EPL, g->p.y + g->p.cy -1);
	write_reg(g, HX8347D_REG_EPH, (g->p.y + g->p.cy -1) >> 8);
	write_index(g, HX8347D_REG_SRAMWC);
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
	gfxSleepMilliseconds(1);
	setpin_reset(g, FALSE);
	gfxSleepMilliseconds(5);

	// Get the bus for the following initialisation commands
	acquire_bus(g);

	/* Start Initial Sequence ----------------------------------------------------*/
	write_reg(g, HX8347D_REG_STBAH, 0x00);                 /* Reset Power Control 1                */
	write_reg(g, HX8347D_REG_STBAL, 0x20);                 /* Power Control 2                      */
	write_reg(g, HX8347D_REG_PTBAH, 0x0C);                 /* Power Control 1                      */
	write_reg(g, HX8347D_REG_PTBAL, 0xC4);                 /* Power Control 2                      */
	write_reg(g, HX8347D_REG_OPONN, 0x40);                 /* Source OPON_N                        */
	write_reg(g, HX8347D_REG_OPONI, 0x38);                 /* Source OPON_I                        */
	write_reg(g, HX8347D_REG_DC2, 0xA3);                   /* Display Control 2                    */

	/* Power On sequence ---------------------------------------------------------*/
	write_reg(g, HX8347D_REG_PWC2, 0x1B);                  /* Power Control 2                      */
	write_reg(g, HX8347D_REG_PWC1, 0x01);                  /* Power Control 1                      */
	write_reg(g, HX8347D_REG_VMH, 0x2F);                   /* Vcom Control 2                       */
	write_reg(g, HX8347D_REG_VML, 0x57);                   /* Vcom Control 3                       */
	write_reg(g, HX8347D_REG_VMF, 0x8D);                   /* Vcom Control 1                       */

	/* Gamma settings  -----------------------------------------------------------*/
	write_reg(g, HX8347D_REG_VRP0, 0x01);   //   default setup
	write_reg(g, HX8347D_REG_VRP1, 0x0e);   //
	write_reg(g, HX8347D_REG_VRP2, 0x11);   //
	write_reg(g, HX8347D_REG_VRP3, 0x1a);   //
	write_reg(g, HX8347D_REG_VRP4, 0x18);   //
	write_reg(g, HX8347D_REG_VRP5, 0x24);   //
	write_reg(g, HX8347D_REG_PRP0, 0x15);   //
	write_reg(g, HX8347D_REG_PRP1, 0x65);   //
	write_reg(g, HX8347D_REG_PKP0, 0x0b);   //
	write_reg(g, HX8347D_REG_PKP1, 0x18);   //
	write_reg(g, HX8347D_REG_PKP2, 0x19);   //
	write_reg(g, HX8347D_REG_PKP3, 0x1a);   //
	write_reg(g, HX8347D_REG_PKP4, 0x18);   //
	write_reg(g, HX8347D_REG_VRN0, 0x1b);   //
	write_reg(g, HX8347D_REG_VRN1, 0x27);   //
	write_reg(g, HX8347D_REG_VRN2, 0x25);   //
	write_reg(g, HX8347D_REG_VRN3, 0x2e);   //
	write_reg(g, HX8347D_REG_VRN4, 0x31);   //
	write_reg(g, HX8347D_REG_VRN5, 0x3e);   //
	write_reg(g, HX8347D_REG_PRN0, 0x1a);   //
	write_reg(g, HX8347D_REG_PRN1, 0x6a);   //
	write_reg(g, HX8347D_REG_PKN0, 0x07);   //
	write_reg(g, HX8347D_REG_PKN1, 0x05);   //
	write_reg(g, HX8347D_REG_PKN2, 0x06);   //
	write_reg(g, HX8347D_REG_PKN3, 0x0b);   //
	write_reg(g, HX8347D_REG_PKN4, 0x14);   //
	write_reg(g, HX8347D_REG_CGM, 0xcc);    //

	/* Power + Osc ---------------------------------------------------------------*/
	write_reg(g, HX8347D_REG_OSCCH, 0x36);               /* OSC Control 1                        */
	write_reg(g, HX8347D_REG_OSCCL, 0x01);               /* OSC Control 2                        */
	write_reg(g, HX8347D_REG_DMODE, 0x00);               /* Display Mode Control                 */
	write_reg(g, HX8347D_REG_PWC6, 0x88);                /* Power Control 6                      */
	gfxSleepMilliseconds(5);                             /* Delay 5 ms                           */
	write_reg(g, HX8347D_REG_PWC6, 0x80);                /* Power Control 6                      */
	gfxSleepMilliseconds(5);                             /* Delay 5 ms                           */
	write_reg(g, HX8347D_REG_PWC6, 0x90);                /* Power Control 6                      */
	gfxSleepMilliseconds(5);                             /* Delay 5 ms                           */
	write_reg(g, HX8347D_REG_PWC6, 0xD0);                /* Power Control 6                      */
	gfxSleepMilliseconds(5);                             /* Delay 5 ms                           */
	write_reg(g, HX8347D_REG_COLMOD, 0x05);              /* Colmod 16Bit/Pixel                   */
	write_reg(g, HX8347D_REG_PCH, 0x00);                 /* Panel Characteristic                 */
	write_reg(g, HX8347D_REG_DC3, 0x38);                 /* Display Control 3                    */
	gfxSleepMilliseconds(40);                            /* Delay 40 ms                          */
	write_reg(g, HX8347D_REG_DC3, 0x3C);                 /* Display Control 3                    */
	write_reg(g, HX8347D_REG_MAC, 0x08);                 /* Memory access control                */

    // Finish Init
    post_init_board(g);

 	// Release the bus
	release_bus(g);

	/* Turn on the backlight */
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
		busmode16(g);
	}
	LLDSPEC	void gdisp_lld_write_color(GDisplay *g) {
		write_ram16(g, gdispColor2Native(g->p.color));
	}
	LLDSPEC	void gdisp_lld_write_stop(GDisplay *g) {
		busmode8(g);
		release_bus(g);
	}
#endif

#if GDISP_NEED_CONTROL && GDISP_HARDWARE_CONTROL
	LLDSPEC void gdisp_lld_control(GDisplay *g) {
		switch(g->p.x) {
		case GDISP_CONTROL_ORIENTATION:
			if (g->g.Orientation == (orientation_t)g->p.ptr)
				return;
			switch((orientation_t)g->p.ptr) {
				case GDISP_ROTATE_0:
					acquire_bus(g);
					write_reg(g, HX8347D_REG_MAC, 0x08);                    /* Memory access control */
					release_bus(g);
					g->g.Height = GDISP_SCREEN_HEIGHT;
					g->g.Width = GDISP_SCREEN_WIDTH;
					break;

				case GDISP_ROTATE_90:
					acquire_bus(g);
					write_reg(g, HX8347D_REG_MAC, 0x68);                    /* Memory access control */
					release_bus(g);
					g->g.Height = GDISP_SCREEN_WIDTH;
					g->g.Width = GDISP_SCREEN_HEIGHT;
					break;

				case GDISP_ROTATE_180:
					acquire_bus(g);
					write_reg(g, HX8347D_REG_MAC, 0xc8);                    /* Memory access control */
					release_bus(g);
					g->g.Height = GDISP_SCREEN_HEIGHT;
					g->g.Width = GDISP_SCREEN_WIDTH;
					break;

				case GDISP_ROTATE_270:
					acquire_bus(g);
					write_reg(g, HX8347D_REG_MAC, 0xa8);                    /* Memory access control */
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
