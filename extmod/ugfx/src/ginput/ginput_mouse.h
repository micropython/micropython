/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/ginput/ginput_mouse.h
 *
 * @defgroup Mouse Mouse
 * @ingroup GINPUT
 *
 * @brief   Sub-Module to handle touchscreens and mices.
 *
 * @details Both resistive and capacitive touchscreens are supported.
 *
 * @pre		GFX_USE_GINPUT must be set to TRUE in your gfxconf.h
 * @pre		GINPUT_NEED_MOUSE must be set to TRUE in your gfxconf.h
 *
 * @{
 */

#ifndef _GINPUT_MOUSE_H
#define _GINPUT_MOUSE_H

#if GINPUT_NEED_MOUSE || defined(__DOXYGEN__)

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

/* This type definition is also used by touch */
typedef struct GEventMouse_t {
	GEventType		type;							// The type of this event (GEVENT_MOUSE or GEVENT_TOUCH)
	coord_t			x, y, z;						// The position of the mouse.
													//		- For touch devices, Z is the current pressure if supported (values are device specific)
													//		- For mice, Z is the 3rd dimension if supported (values are device specific)
	uint16_t		buttons;						// A bit is set if the button is down or a meta event has occurred.
		#define GINPUT_MOUSE_BTN_MASK		0x000F		// The "button is down" mask
		#define GINPUT_MOUSE_BTN_LEFT		0x0001		// The left mouse button is currently down
		#define GINPUT_MOUSE_BTN_RIGHT		0x0002		// The right mouse button is currently down
		#define GINPUT_MOUSE_BTN_MIDDLE		0x0004		// The middle mouse button is currently down
		#define GINPUT_MOUSE_BTN_4			0x0008		// The 4th mouse button is currently down
		#define GINPUT_TOUCH_PRESSED		0x0001		// The touch surface is currently touched

		#define GMETA_MASK					0x00F0		// The "button transition" mask
		#define GMETA_NONE					0x0000		// No "button transition" events
		#define GMETA_MOUSE_DOWN			0x0010		// Left mouse/touch has just gone down
		#define GMETA_MOUSE_UP				0x0020		// Left mouse/touch has just gone up
		#define GMETA_MOUSE_CLICK			0x0040		// Left mouse/touch has just gone through a click (short down - up cycle)
		#define GMETA_MOUSE_CXTCLICK		0x0080		// Right mouse has just been depressed or touch has gone through a long click

		#define GINPUT_MISSED_MOUSE_EVENT	0x8000		// Oops - a mouse event has previously been missed

	GDisplay *			display;		// The display this mouse is currently associated with.
} GEventMouse;

// Mouse/Touch Listen Flags - passed to geventAddSourceToListener()
#define GLISTEN_MOUSEMETA			0x0001			// Create events for meta events such as CLICK and CXTCLICK
#define GLISTEN_MOUSEDOWNMOVES		0x0002			// Creates mouse move events when the primary mouse button is down (touch is on the surface)
#define GLISTEN_MOUSEUPMOVES		0x0004			// Creates mouse move events when the primary mouse button is up (touch is off the surface - if the hardware allows).
#define	GLISTEN_MOUSENOFILTER		0x0008			// Don't filter out mouse moves where the position hasn't changed.
#define GLISTEN_TOUCHMETA			GLISTEN_MOUSEMETA
#define GLISTEN_TOUCHDOWNMOVES		GLISTEN_MOUSEDOWNMOVES
#define GLISTEN_TOUCHUPMOVES		GLISTEN_MOUSEUPMOVES
#define	GLISTEN_TOUCHNOFILTER		GLISTEN_MOUSENOFILTER

// Event types for the mouse ginput source
#define GEVENT_MOUSE		(GEVENT_GINPUT_FIRST+0)
#define GEVENT_TOUCH		(GEVENT_GINPUT_FIRST+1)

// All mice
#define GMOUSE_ALL_INSTANCES		((unsigned)-1)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @brief	Get the Source handler for a mouse using the instance number
	 *
	 * @param[in] instance		The mouse instance number
	 *
	 * @return		The source handle of the mouse or NULL
	 * @note		You can use the special value of GMOUSE_ALL_INSTANCES to
	 * 				get a source handle that returns events for all mice rather
	 * 				than a specific mouse. Using GMOUSE_ALL_INSTANCES will always
	 * 				return a valid spurce handle even if there are currently no mice
	 * 				in the system.
	 */
	GSourceHandle ginputGetMouse(unsigned instance);

	/**
	 * @brief	Should this device be in Pen mode or Finger mode
	 * @note	A touch device (and even theoritically a mouse) can operate
	 * 			in either pen or finger mode. In finger mode typically a
	 * 			touch device will be far more tolerant of movement and other
	 * 			inaccuracies. Each driver specifies its own tolerances for
	 * 			pen versus finger mode.
	 *
	 * @param[in] instance		The ID of the mouse input instance
	 * @param[in] on			If true then finger mode is turned on.
	 */
	void ginputSetFingerMode(unsigned instance, bool_t on);

	/**
	 * @brief	Assign the display associated with the mouse
	 * @note	This only needs to be called if the mouse is associated with a display
	 * 			other than the current default display. It must be called before
	 * 			@p ginputGetMouse() if the new display is to be used during the calibration
	 * 			process. Other than calibration the display is used for range checking,
	 * 			and may also be used to display a mouse pointer.
	 *
	 * @param[in] instance		The ID of the mouse input instance
	 * @param[in] g				The GDisplay to which this mouse belongs
	 */
	void ginputSetMouseDisplay(unsigned instance, GDisplay *g);

	/**
	 * @brief	Get the display currently associated with the mouse
	 * @return	A pointer to the display
	 *
	 * @param[in] instance		The ID of the mouse input instance
	 */
	GDisplay *ginputGetMouseDisplay(unsigned instance);

	/**
	 * @brief	Get the current mouse position and button status
	 * @note	Unlinke a listener event, this status cannot record meta events such as
	 *			"CLICK".
	 *
	 * @param[in] instance	The ID of the mouse input instance
	 * @param[in] pmouse	The mouse event
	 *
	 * @return	FALSE on an error (eg. invalid instance)
	 */
	bool_t ginputGetMouseStatus(unsigned instance, GEventMouse *pmouse);

	/**
	 * @brief	Performs a calibration
	 *
	 * @param[in] instance	The ID of the mouse input instance
	 *
	 * @return	The calibration error squared if calibration fails, or 0 on success or if the driver dosen't need calibration.
	 * @note	An invalid instance will also return 0.
	 */
	uint32_t ginputCalibrateMouse(unsigned instance);

    /**
     * @brief   Load a set of mouse calibration data
     * @return  A pointer to the data or NULL on failure
     *
	 * @param[in] instance		The mouse input instance number
	 * @param[in] data			Where the data should be placed
	 * @param[in] sz    		The size in bytes of the data to retrieve.
	 *
     * @note    This routine is provided by the user application. It is only
     *          called if GINPUT_TOUCH_USER_CALIBRATION_LOAD has been set to TRUE in the
     *          users gfxconf.h file.
     */
    bool_t LoadMouseCalibration(unsigned instance, void *data, size_t sz);

    /**
     * @brief   Save a set of mouse calibration data
     * @return  TRUE if the save operation was successful.
     *
	 * @param[in] instance		The mouse input instance number
	 * @param[in] data          The data to save
	 * @param[in] sz    		The size in bytes of the data to retrieve.
	 *
     * @note    This routine is provided by the user application. It is only
     *          called if GINPUT_TOUCH_USER_CALIBRATION_SAVE has been set to TRUE in the
     *          users gfxconf.h file.
     */
    bool_t SaveMouseCalibration(unsigned instance, const void *data, size_t sz);

#ifdef __cplusplus
}
#endif

#endif /* GINPUT_NEED_MOUSE */

#endif /* _GINPUT_MOUSE_H */
/** @} */

