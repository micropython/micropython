/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    boards/base/Olimex-SAM7EX256-GE8/ginput_lld_toggle_board.h
 * @brief   GINPUT Toggle low level driver source for the ChibiOS PAL hardware on the Olimex SAM7EX256 board.
 */

#ifndef _GDISP_LLD_TOGGLE_BOARD_H
#define _GDISP_LLD_TOGGLE_BOARD_H

#define GINPUT_TOGGLE_NUM_PORTS			7			// The total number of toggle inputs
#define GINPUT_TOGGLE_CONFIG_ENTRIES	2			// The total number of GToggleConfig entries

#define GINPUT_TOGGLE_SW1			0				// Switch 1
#define GINPUT_TOGGLE_SW2			1				// Switch 2
#define GINPUT_TOGGLE_LEFT			2				// Joystick Left
#define GINPUT_TOGGLE_DOWN			3				// Joystick Down
#define GINPUT_TOGGLE_UP			4				// Joystick Up
#define GINPUT_TOGGLE_RIGHT			5				// Joystick Right
#define GINPUT_TOGGLE_CENTER		6				// Joystick Center

#define GINPUT_TOGGLE_DECLARE_STRUCTURE()											\
	const GToggleConfig GInputToggleConfigTable[GINPUT_TOGGLE_CONFIG_ENTRIES] = {	\
		{AT91C_BASE_PIOB,								/* Switch 1 and Switch 2 */	\
			PIOB_SW1_MASK|PIOB_SW2_MASK,											\
			PIOB_SW1_MASK|PIOB_SW2_MASK,											\
			PAL_MODE_INPUT},														\
		{AT91C_BASE_PIOA,								/* B1..4 Joystick */		\
			PIOA_B1_MASK|PIOA_B2_MASK|PIOA_B3_MASK|PIOA_B4_MASK|PIOA_B5_MASK,		\
			PIOA_B1_MASK|PIOA_B2_MASK|PIOA_B3_MASK|PIOA_B4_MASK|PIOA_B5_MASK,		\
			PAL_MODE_INPUT},														\
	}

#endif /* _GDISP_LLD_TOGGLE_BOARD_H */

