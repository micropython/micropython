/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GINPUT_LLD_TOGGLE_CONFIG_H
#define _GINPUT_LLD_TOGGLE_CONFIG_H

#if GFX_USE_GINPUT && GINPUT_NEED_TOGGLE

#define GINPUT_TOGGLE_POLL_PERIOD		TIME_INFINITE	// We are interrupt driven (or polled - either works here)

// This driver is unique in that it can support 8 buttons per window across multiple windows.
// GINPUT_TOGGLE_CONFIG_ENTRIES just must be less than the number of GDISP windows (GDISP_DRIVER_COUNT_WIN32).
// DEPRECATED: Currently only supports a single window with buttons until toggles use GDriver
#ifndef GINPUT_TOGGLE_CONFIG_ENTRIES
	#define GINPUT_TOGGLE_CONFIG_ENTRIES	1			// The total number of GToggleConfig entries
#endif

// The total number of toggle inputs
#define GINPUT_TOGGLE_NUM_PORTS		(8 * GINPUT_TOGGLE_CONFIG_ENTRIES)

#define GINPUT_TOGGLE_SW1			0				// Switch 1	- Toggle
#define GINPUT_TOGGLE_SW2			1				// Switch 2	- Toggle
#define GINPUT_TOGGLE_SW3			2				// Switch 3	- Toggle
#define GINPUT_TOGGLE_SW4			3				// Switch 4	- Toggle

#define GINPUT_TOGGLE_MOMENTARY1	4				// Switch 5	- Momentary
#define GINPUT_TOGGLE_MOMENTARY2	5				// Switch 6	- Momentary
#define GINPUT_TOGGLE_MOMENTARY3	6				// Switch 7	- Momentary
#define GINPUT_TOGGLE_MOMENTARY4	7				// Switch 8	- Momentary

// This pattern of switch and momentary action is repeated across all windows.

#endif	/* GFX_USE_GDISP && GINPUT_NEED_TOGGLE */

#endif	/* _GINPUT_LLD_TOGGLE_CONFIG_H */
