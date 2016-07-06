/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

/* This controller is only ever used with a 240 x 320 display */
#if defined(GDISP_SCREEN_HEIGHT)
	#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
	#undef GDISP_SCREEN_HEIGHT
#endif
#if defined(GDISP_SCREEN_WIDTH)
	#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
	#undef GDISP_SCREEN_WIDTH
#endif

#define GDISP_DRIVER_VMT			GDISPVMT_ILI93xx
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

#include "board_ILI93xx.h"

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
#define dummy_read(g)               { volatile uint16_t dummy; dummy = read_data(g); (void) dummy; }
#define write_reg(g, reg, data)     { write_index(g, reg); write_data(g, data); }

static GFXINLINE uint16_t read_reg(GDisplay *g, uint32_t reg) {
  write_index(g, reg);
  return read_data(g);
 }

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

   unsigned short DeviceCode;

   // No private area for this controller
   g->priv = 0;

   // Initialise the board interface
   init_board(g);

   /* Hardware reset */
   setpin_reset(g, TRUE);
   gfxSleepMilliseconds(1);
   setpin_reset(g, FALSE);
   gfxSleepMilliseconds(10);
   setpin_reset(g, TRUE);
   gfxSleepMilliseconds(50);

   acquire_bus(g);
   setreadmode(g);
   DeviceCode = read_reg(g, 0x00);
   setwritemode(g);

   if( DeviceCode == 0x9320 || DeviceCode == 0x9300 )
   {
      write_reg(g, 0x00, 0x0000);
      write_reg(g, 0x01, 0x0100); /* Driver Output Contral */
      write_reg(g, 0x02, 0x0700); /* LCD Driver Waveform Contral */
      write_reg(g, 0x03, 0x1038); /* Set the scan mode */
      write_reg(g, 0x04, 0x0000); /* Scalling Contral */
      write_reg(g, 0x08, 0x0202); /* Display Contral 2 */
      write_reg(g, 0x09, 0x0000); /* Display Contral 3 */
      write_reg(g, 0x0a, 0x0000); /* Frame Cycle Contal */
      write_reg(g, 0x0c, (1<<0)); /* Extern Display Interface Contral 1 */
      write_reg(g, 0x0d, 0x0000); /* Frame Maker Position */
      write_reg(g, 0x0f, 0x0000); /* Extern Display Interface Contral 2 */
      gfxSleepMilliseconds(50);
      write_reg(g, 0x07, 0x0101); /* Display Contral */
      gfxSleepMilliseconds(50);
      write_reg(g, 0x10, (1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4)); /* Power Control 1 */
      write_reg(g, 0x11, 0x0007);                              /* Power Control 2 */
      write_reg(g, 0x12, (1<<8)|(1<<4)|(0<<0));                /* Power Control 3 */
      write_reg(g, 0x13, 0x0b00);                              /* Power Control 4 */
      write_reg(g, 0x29, 0x0000);                              /* Power Control 7 */
      write_reg(g, 0x2b, (1<<14)|(1<<4));
      write_reg(g, 0x50, 0);       /* Set X Start */
      write_reg(g, 0x51, 239);     /* Set X End */
      write_reg(g, 0x52, 0);       /* Set Y Start */
      write_reg(g, 0x53, 319);     /* Set Y End */
      gfxSleepMilliseconds(50);

      write_reg(g, 0x60, 0x2700); /* Driver Output Control */
      write_reg(g, 0x61, 0x0001); /* Driver Output Control */
      write_reg(g, 0x6a, 0x0000); /* Vertical Srcoll Control */

      write_reg(g, 0x80, 0x0000); /* Display Position? Partial Display 1 */
      write_reg(g, 0x81, 0x0000); /* RAM Address Start? Partial Display 1 */
      write_reg(g, 0x82, 0x0000); /* RAM Address End-Partial Display 1 */
      write_reg(g, 0x83, 0x0000); /* Displsy Position? Partial Display 2 */
      write_reg(g, 0x84, 0x0000); /* RAM Address Start? Partial Display 2 */
      write_reg(g, 0x85, 0x0000); /* RAM Address End? Partial Display 2 */

      write_reg(g, 0x90, (0<<7)|(16<<0)); /* Frame Cycle Contral */
      write_reg(g, 0x92, 0x0000);         /* Panel Interface Contral 2 */
      write_reg(g, 0x93, 0x0001);         /* Panel Interface Contral 3 */
      write_reg(g, 0x95, 0x0110);         /* Frame Cycle Contral */
      write_reg(g, 0x97, (0<<8));
      write_reg(g, 0x98, 0x0000);         /* Frame Cycle Contral */
      write_reg(g, 0x07, 0x0133);
   } 
   else if( DeviceCode == 0x9325 || DeviceCode == 0x9328)
   {

      write_reg(g, 0x00e7, 0x0010);
      write_reg(g, 0x0000, 0x0001);    /* start internal osc */
      write_reg(g, 0x0001, 0x0100);
      write_reg(g, 0x0002, 0x0700);    /* power on sequence */
      write_reg(g, 0x0003, (1<<12)|(1<<5)|(1<<4)|(0<<3) );     /* importance */
      write_reg(g, 0x0004, 0x0000);
      write_reg(g, 0x0008, 0x0207);
      write_reg(g, 0x0009, 0x0000);
      write_reg(g, 0x000a, 0x0000);    /* display setting */
      write_reg(g, 0x000c, 0x0001);    /* display setting */
      write_reg(g, 0x000d, 0x0000);
      write_reg(g, 0x000f, 0x0000);
      /* Power On sequence */
      write_reg(g, 0x0010, 0x0000);
      write_reg(g, 0x0011, 0x0007);
      write_reg(g, 0x0012, 0x0000);
      write_reg(g, 0x0013, 0x0000);
      gfxSleepMilliseconds(50);  /* delay 50 ms */
      write_reg(g, 0x0010, 0x1590);
      write_reg(g, 0x0011, 0x0227);
      gfxSleepMilliseconds(50);  /* delay 50 ms */
      write_reg(g, 0x0012, 0x009c);
      gfxSleepMilliseconds(50);  /* delay 50 ms */
      write_reg(g, 0x0013, 0x1900);
      write_reg(g, 0x0029, 0x0023);
      write_reg(g, 0x002b, 0x000e);
      gfxSleepMilliseconds(50);  /* delay 50 ms */
      write_reg(g, 0x0020, 0x0000);
      write_reg(g, 0x0021, 0x0000);
      gfxSleepMilliseconds(50);  /* delay 50 ms */
      write_reg(g, 0x0030, 0x0007);
      write_reg(g, 0x0031, 0x0707);
      write_reg(g, 0x0032, 0x0006);
      write_reg(g, 0x0035, 0x0704);
      write_reg(g, 0x0036, 0x1f04);
      write_reg(g, 0x0037, 0x0004);
      write_reg(g, 0x0038, 0x0000);
      write_reg(g, 0x0039, 0x0706);
      write_reg(g, 0x003c, 0x0701);
      write_reg(g, 0x003d, 0x000f);
      gfxSleepMilliseconds(50);  /* delay 50 ms */
      write_reg(g, 0x0050, 0x0000);
      write_reg(g, 0x0051, 0x00ef);
      write_reg(g, 0x0052, 0x0000);
      write_reg(g, 0x0053, 0x013f);
      write_reg(g, 0x0060, 0xa700);
      write_reg(g, 0x0061, 0x0001);
      write_reg(g, 0x006a, 0x0000);
      write_reg(g, 0x0080, 0x0000);
      write_reg(g, 0x0081, 0x0000);
      write_reg(g, 0x0082, 0x0000);
      write_reg(g, 0x0083, 0x0000);
      write_reg(g, 0x0084, 0x0000);
      write_reg(g, 0x0085, 0x0000);

      write_reg(g, 0x0090, 0x0010);
      write_reg(g, 0x0092, 0x0000);
      write_reg(g, 0x0093, 0x0003);
      write_reg(g, 0x0095, 0x0110);
      write_reg(g, 0x0097, 0x0000);
      write_reg(g, 0x0098, 0x0000);
      /* display on sequence */
      write_reg(g, 0x0007, 0x0133);

      write_reg(g, 0x0020, 0x0000);
      write_reg(g, 0x0021, 0x0000);
   }

   gfxSleepMilliseconds(100);   /* delay 50 ms */


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
      //write_data(g, COLOR2NATIVE(g->p.color));
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
					write_reg(g, 0x60, 0xa700);
					release_bus(g);
					g->g.Height = GDISP_SCREEN_HEIGHT;
					g->g.Width = GDISP_SCREEN_WIDTH;
					break;

				case GDISP_ROTATE_90:
					acquire_bus(g);
					write_reg(g, 0x01, 0x0100);
					write_reg(g, 0x03, 0x1038);
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
					write_reg(g, 0x01, 0x0000);
					write_reg(g, 0x03, 0x1038);
					write_reg(g, 0x60, 0xa700);
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
