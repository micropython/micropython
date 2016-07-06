/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE

#define GMOUSE_DRIVER_VMT		GMOUSEVMT_MCU
#include "../../../../src/ginput/ginput_driver_mouse.h"

// Get the hardware interface
#include "gmouse_lld_MCU_board.h"

const GMouseVMT const GMOUSE_DRIVER_VMT[1] = {{
	{
		GDRIVER_TYPE_TOUCH,
		GMOUSE_VFLG_TOUCH|GMOUSE_VFLG_CALIBRATE|GMOUSE_VFLG_CAL_TEST
			|GMOUSE_VFLG_ONLY_DOWN|GMOUSE_VFLG_POORUPDOWN,
		// Extra flags for testing only
		//GMOUSE_VFLG_DEFAULTFINGER|GMOUSE_VFLG_CAL_EXTREMES					- Possible
		//GMOUSE_VFLG_NOPOLL|GMOUSE_VFLG_DYNAMICONLY|GMOUSE_VFLG_SELFROTATION|GMOUSE_VFLG_CAL_LOADFREE - unlikely
		sizeof(GMouse) + GMOUSE_MCU_BOARD_DATA_SIZE,
		_gmouseInitDriver, _gmousePostInitDriver, _gmouseDeInitDriver
	},
	GMOUSE_MCU_Z_MAX,			// z_max
	GMOUSE_MCU_Z_MIN,			// z_min
	GMOUSE_MCU_Z_TOUCHON,		// z_touchon
	GMOUSE_MCU_Z_TOUCHOFF,		// z_touchoff
	{				// pen_jitter
		GMOUSE_MCU_PEN_CALIBRATE_ERROR,			// calibrate
		GMOUSE_MCU_PEN_CLICK_ERROR,				// click
		GMOUSE_MCU_PEN_MOVE_ERROR				// move
	},
	{				// finger_jitter
		GMOUSE_MCU_FINGER_CALIBRATE_ERROR,		// calibrate
		GMOUSE_MCU_FINGER_CLICK_ERROR,			// click
		GMOUSE_MCU_FINGER_MOVE_ERROR			// move
	},
	init_board,		// init
	0,				// deinit
	read_xyz,		// get
	0,				// calsave
	0				// calload
}};

#endif /* GFX_USE_GINPUT && GINPUT_NEED_MOUSE */
