/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE

#define GMOUSE_DRIVER_VMT		GMOUSEVMT_FT5x06
#include "../../../../src/ginput/ginput_driver_mouse.h"

// Get the hardware interface
#include "gmouse_lld_FT5x06_board.h"

// Hardware definitions
#include "ft5x06.h"

static bool_t MouseInit(GMouse* m, unsigned driverinstance) {
	if (!init_board(m, driverinstance))
		return FALSE;

	aquire_bus(m);

	// Init default values. (From NHD-3.5-320240MF-ATXL-CTP-1 datasheet)
	// Valid touching detect threshold
	write_reg(m, FT5x06_ID_G_THGROUP, 0x16);

	// valid touching peak detect threshold
	write_reg(m, FT5x06_ID_G_THPEAK, 0x3C);

	// Touch focus threshold
	write_reg(m, FT5x06_ID_G_THCAL, 0xE9);

	// threshold when there is surface water
	write_reg(m, FT5x06_ID_G_THWATER, 0x01);

	// threshold of temperature compensation
	write_reg(m, FT5x06_ID_G_THTEMP, 0x01);

	// Touch difference threshold
	write_reg(m, FT5x06_ID_G_THDIFF, 0xA0);

	// Delay to enter 'Monitor' status (s)
	write_reg(m, FT5x06_ID_G_TIME_ENTER_MONITOR, 0x0A);

	// Period of 'Active' status (ms)
	write_reg(m, FT5x06_ID_G_PERIODACTIVE, 0x06);

	// Timer to enter 'idle' when in 'Monitor' (ms)
	write_reg(m, FT5x06_ID_G_PERIODMONITOR, 0x28);

	release_bus(m);
	return TRUE;
}

static bool_t read_xyz(GMouse* m, GMouseReading* pdr)
{
	// Assume not touched.
	pdr->buttons = 0;
	pdr->z = 0;
	
	aquire_bus(m);

	// Only take a reading if we are touched.
	if ((read_byte(m, FT5x06_TOUCH_POINTS) & 0x07)) {

		/* Get the X, Y, Z values */
		pdr->x = (coord_t)(read_word(m, FT5x06_TOUCH1_XH) & 0x0fff);
		pdr->y = (coord_t)read_word(m, FT5x06_TOUCH1_YH);
		pdr->z = 1;

		// Rescale X,Y if we are using self-calibration
		#if GMOUSE_FT5x06_SELF_CALIBRATE
			#if GDISP_NEED_CONTROL
				switch(gdispGGetOrientation(m->display)) {
				default:
				case GDISP_ROTATE_0:
				case GDISP_ROTATE_180:
					pdr->x = gdispGGetWidth(m->display) - pdr->x / (4096/gdispGGetWidth(m->display));
					pdr->y = pdr->y / (4096/gdispGGetHeight(m->display));
					break;
				case GDISP_ROTATE_90:
				case GDISP_ROTATE_270:
					pdr->x = gdispGGetHeight(m->display) - pdr->x / (4096/gdispGGetHeight(m->display));
					pdr->y = pdr->y / (4096/gdispGGetWidth(m->display));
					break;
				}
			#else
				pdr->x = gdispGGetWidth(m->display) - pdr->x / (4096/gdispGGetWidth(m->display));
				pdr->y = pdr->y / (4096/gdispGGetHeight(m->display));
			#endif
		#endif
	}

	release_bus(m);
	return TRUE;
}

const GMouseVMT const GMOUSE_DRIVER_VMT[1] = {{
	{
		GDRIVER_TYPE_TOUCH,
		#if GMOUSE_FT5x06_SELF_CALIBRATE
			GMOUSE_VFLG_TOUCH | GMOUSE_VFLG_ONLY_DOWN | GMOUSE_VFLG_POORUPDOWN,
		#else
			GMOUSE_VFLG_TOUCH | GMOUSE_VFLG_ONLY_DOWN | GMOUSE_VFLG_POORUPDOWN | GMOUSE_VFLG_CALIBRATE | GMOUSE_VFLG_CAL_TEST,
		#endif
		sizeof(GMouse) + GMOUSE_FT5x06_BOARD_DATA_SIZE,
		_gmouseInitDriver,
		_gmousePostInitDriver,
		_gmouseDeInitDriver
	},
	1,				// z_max - (currently?) not supported
	0,				// z_min - (currently?) not supported
	1,				// z_touchon
	0,				// z_touchoff
	{				// pen_jitter
		GMOUSE_FT5x06_PEN_CALIBRATE_ERROR,			// calibrate
		GMOUSE_FT5x06_PEN_CLICK_ERROR,				// click
		GMOUSE_FT5x06_PEN_MOVE_ERROR				// move
	},
	{				// finger_jitter
		GMOUSE_FT5x06_FINGER_CALIBRATE_ERROR,		// calibrate
		GMOUSE_FT5x06_FINGER_CLICK_ERROR,			// click
		GMOUSE_FT5x06_FINGER_MOVE_ERROR			// move
	},
	MouseInit, 		// init
	0,				// deinit
	read_xyz,		// get
	0,				// calsave
	0				// calload
}};

#endif /* GFX_USE_GINPUT && GINPUT_NEED_MOUSE */

