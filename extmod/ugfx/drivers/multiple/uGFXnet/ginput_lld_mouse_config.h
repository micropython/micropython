/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _LLD_GINPUT_MOUSE_CONFIG_H
#define _LLD_GINPUT_MOUSE_CONFIG_H

// This driver supports being both a mouse or a touch device (we don't actually know which it really is)
//	When operating in mouse mode a long left button click does not generate a context click.
//	When operating in touch mode we allow sloppier clicks etc
#if 1
	#define GINPUT_MOUSE_EVENT_TYPE					GEVENT_MOUSE
	#define GINPUT_MOUSE_CLICK_TIME					TIME_INFINITE			// Long click != Context Click
	#define GINPUT_MOUSE_NEED_CALIBRATION			FALSE
	#define GINPUT_MOUSE_LLD_CALIBRATION_LOADSAVE	FALSE
	#define GINPUT_MOUSE_READ_CYCLES				1
	#define GINPUT_MOUSE_MAX_CALIBRATION_ERROR		-1
	#define GINPUT_MOUSE_MAX_CLICK_JITTER			0
	#define GINPUT_MOUSE_MAX_MOVE_JITTER			0
#else
	#define GINPUT_MOUSE_EVENT_TYPE					GEVENT_TOUCH
	#define GINPUT_MOUSE_CLICK_TIME					700						// Long click = Context Click
	#define GINPUT_MOUSE_NEED_CALIBRATION			FALSE					// Can be set to TRUE just for testing
	#define GINPUT_MOUSE_LLD_CALIBRATION_LOADSAVE	FALSE
	#define GINPUT_MOUSE_READ_CYCLES				1
	#define GINPUT_MOUSE_MAX_CALIBRATION_ERROR		2
	#define GINPUT_MOUSE_MAX_CLICK_JITTER			2
	#define GINPUT_MOUSE_MAX_MOVE_JITTER			2
#endif

// This driver supports both an "interrupt" mode, and a polled mode
#define GINPUT_MOUSE_POLL_PERIOD				TIME_INFINITE			// Interrupt driven by the Window thread
//#define GINPUT_MOUSE_POLL_PERIOD				25						// Poll driven

// This driver does not require rotation of co-ordinates for orientations other than 0.
#define GINPUT_MOUSE_NO_ROTATION				TRUE

#endif /* _LLD_GINPUT_MOUSE_CONFIG_H */
