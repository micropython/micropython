/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/ginput/ginput_driver_mouse.h
 * @brief   GINPUT LLD header file for mouse/touch drivers.
 *
 * @defgroup Mouse Mouse
 * @ingroup GINPUT
 * @{
 */

#ifndef _LLD_GINPUT_MOUSE_H
#define _LLD_GINPUT_MOUSE_H

#if GINPUT_NEED_MOUSE //|| defined(__DOXYGEN__)

// Include the GDRIVER infrastructure
#include "../gdriver/gdriver.h"

typedef struct GMouseReading {
	coord_t		x, y, z;
	uint16_t	buttons;
	} GMouseReading;

#if !GINPUT_TOUCH_NOCALIBRATE
	typedef struct GMouseCalibration {
		float	ax;
		float	bx;
		float	cx;
		float	ay;
		float	by;
		float	cy;
	} GMouseCalibration;
#endif

typedef struct GMouse {
	GDriver								d;					// The driver overheads and vmt
	GMouseReading						r;					// The current position and state
	uint16_t							flags;				// Flags
			#define GMOUSE_FLG_CLICK_TIMER		0x0001				// Currently timing a click
			#define GMOUSE_FLG_INDELTA			0x0002				// Currently in a up/down transition test
			#define GMOUSE_FLG_CLIP				0x0004				// Clip reading to the display
			#define GMOUSE_FLG_CALIBRATE		0x0008				// Calibrate readings
			#define GMOUSE_FLG_IN_CAL			0x0010				// Currently in calibration routine
			#define GMOUSE_FLG_FINGERMODE		0x0020				// Mouse is currently in finger mode
			#define GMOUSE_FLG_NEEDREAD			0x0040				// The mouse needs reading
			#define GMOUSE_FLG_DRIVER_FIRST		0x0100				// The first flag available for the driver
	point								clickpos;			// The position of the last click event
	systemticks_t						clicktime;			// The time of the last click event
	GDisplay *							display;			// The display the mouse is associated with
	#if !GINPUT_TOUCH_NOCALIBRATE
		GMouseCalibration				caldata;			// The calibration data
	#endif
	// Other driver specific fields may follow.
} GMouse;

typedef struct GMouseJitter {
	coord_t		calibrate;									// Maximum error for a calibration to succeed
	coord_t		click;										// Movement allowed without discarding the CLICK or CLICKCXT event
	coord_t		move;										// Movement allowed without discarding the MOVE event
} GMouseJitter;

typedef struct GMouseVMT {
	GDriverVMT	d;											// Device flags are part of the general vmt
		#define GMOUSE_VFLG_TOUCH			0x0001			// This is a touch device (rather than a mouse). Button 1 is calculated from z value.
		#define GMOUSE_VFLG_NOPOLL			0x0002			// Do not poll this device - it is purely interrupt driven
		#define GMOUSE_VFLG_SELFROTATION	0x0004			// This device returns readings that are aligned with the display orientation
		#define GMOUSE_VFLG_DEFAULTFINGER	0x0008			// Default to finger mode
		#define GMOUSE_VFLG_CALIBRATE		0x0010			// This device requires calibration
		#define GMOUSE_VFLG_CAL_EXTREMES	0x0020			// Use edge to edge calibration
		#define GMOUSE_VFLG_CAL_TEST		0x0040			// Test the results of the calibration
		#define GMOUSE_VFLG_ONLY_DOWN		0x0100			// This device returns a valid position only when the mouse is down
		#define GMOUSE_VFLG_POORUPDOWN		0x0200			// Position readings during up/down are unreliable
		#define GMOUSE_VFLG_DYNAMICONLY		0x8000			// This mouse driver should not be statically initialized eg Win32
	coord_t		z_max;										// TOUCH: Maximum possible z value (fully touched)
	coord_t		z_min;										// TOUCH: Minimum possible z value (touch off screen). Note may also be > z_max
	coord_t		z_touchon;									// TOUCH: z values between z_max and this are a solid touch on
	coord_t		z_touchoff;									// TOUCH: z values between z_min and this are a solid touch off

	GMouseJitter	pen_jitter;								// PEN MODE: Jitter settings
	GMouseJitter	finger_jitter;							// FINGER MODE: Jitter settings

	bool_t	(*init)(GMouse *m, unsigned driverinstance);	// Required
	void	(*deinit)(GMouse *m);							// Optional
	bool_t	(*get)(GMouse *m, GMouseReading *prd);			// Required
	void	(*calsave)(GMouse *m, const void *buf, size_t sz);	// Optional
	bool_t	(*calload)(GMouse *m, void *buf, size_t sz);	// Optional
} GMouseVMT;

#define gmvmt(m)		((const GMouseVMT const *)((m)->d.vmt))

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

// If we are not using multiple mice then hard-code the VMT name
#if !defined(GINPUT_MOUSE_DRIVER_LIST)
	#undef GMOUSE_DRIVER_VMT
	#define GMOUSE_DRIVER_VMT		GMOUSEVMT_OnlyOne
#endif

#ifdef __cplusplus
extern "C" {
#endif
	/**
	 * @brief	Initialize a mouse driver
	 *
	 * @param[in] g					The mouse driver
	 * @param[in] display			The display to which the mouse shall be assigned
	 * @param[in] driverinstance	The driver instance		ToDo: Add some more details
	 * @param[in] systeminstance	The mouse instance		ToDo: Add some more details
	 *
	 * @return	TRUE on success, FALSE otherwise
	 * @note	This routine is provided by the high level code for
	 * 			use in the driver VMT's GMouseVMT.d structure.
	 *
	 * @notapi
	 */
	bool_t _gmouseInitDriver(GDriver *g, void *display, unsigned driverinstance, unsigned systeminstance);

	/**
	 * @brief	Routine that is called after initialization
	 *
	 * @param[in] g		The mouse driver
	 * @note	This routine is provided by the high level code for
	 * 			use in the driver VMT's GMouseVMT.d structure.
	 *
	 * @notapi
	 */
	void _gmousePostInitDriver(GDriver *g);

	/**
	 * @brief	Deinitialize a mouse driver
	 *
	 * @param[in] g		The mouse driver
	 * @note	This routine is provided by the high level code for
	 * 			use in the driver VMT's GMouseVMT.d structure.
	 *
	 * @notapi
	 */
	void _gmouseDeInitDriver(GDriver *g);

	/**
	 * @brief	Wakeup the high level code so that it attempts another read
	 *
	 * @note	This routine is provided to low level drivers by the high level code
	 *
	 * @notapi
	 */
	void _gmouseWakeup(GMouse *m);

	/**
	 * @brief	Wakeup the high level code so that it attempts another read
	 *
	 * @note	This routine is provided to low level drivers by the high level code
	 *
	 * @iclass
	 * @notapi
	 */
	void _gmouseWakeupI(GMouse *m);

#ifdef __cplusplus
}
#endif

#endif /* GINPUT_NEED_MOUSE */

#endif /* _LLD_GINPUT_MOUSE_H */
/** @} */
