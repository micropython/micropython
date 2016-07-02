/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GAUDIO_RECORD_BOARD_H
#define _GAUDIO_RECORD_BOARD_H

/*===========================================================================*/
/* Audio inputs on this board                                                */
/*===========================================================================*/

#define GAUDIO_RECORD_NUM_CHANNELS					1

#define GAUDIO_RECORD_CHANNEL0_IS_STEREO			FALSE

#define	GAUDIO_RECORD_MICROPHONE					0

#ifdef GAUDIO_RECORD_LLD_IMPLEMENTATION
	static uint32_t gaudio_gadc_physdevs[GAUDIO_RECORD_NUM_CHANNELS] = {
			GADC_PHYSDEV_MICROPHONE,
			};
#endif

#endif	/* _GAUDIO_RECORD_BOARD_H */
