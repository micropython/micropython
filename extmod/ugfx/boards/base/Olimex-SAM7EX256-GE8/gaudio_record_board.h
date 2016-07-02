/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    boards/base/Olimex-SAM7EX256-GE8/gaudio_record_board.h
 * @brief   GAUDIO Record Driver board config file for the Olimex SAM7EX256 board
 */

#ifndef _GAUDIO_RECORD_BOARD_H
#define _GAUDIO_RECORD_BOARD_H

/*===========================================================================*/
/* Audio inputs on this board                                                */
/*===========================================================================*/

#define GAUDIO_RECORD_NUM_CHANNELS					1

/**
 * @brief	Whether each channel is mono or stereo
 */
#define GAUDIO_RECORD_CHANNEL0_IS_STEREO			FALSE

/**
 * The list of audio channels and their uses
 */
#define	GAUDIO_RECORD_MICROPHONE					0

#ifdef GAUDIO_RECORD_IMPLEMENTATION
	static uint32_t gaudio_gadc_physdevs[GAUDIO_RECORD_NUM_CHANNELS] = {
			GADC_PHYSDEV_MICROPHONE,
			};
#endif

#endif	/* _GAUDIO_RECORD_BOARD_H */
