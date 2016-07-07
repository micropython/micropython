/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef GAUDIO_PLAY_CONFIG_H
#define GAUDIO_PLAY_CONFIG_H

#if GFX_USE_GAUDIO && GAUDIO_NEED_PLAY

/*===========================================================================*/
/* Driver hardware support.                                                  */
/*===========================================================================*/

#define GAUDIO_PLAY_MAX_SAMPLE_FREQUENCY		44100
#define GAUDIO_PLAY_NUM_FORMATS					2
#define GAUDIO_PLAY_FORMAT1						ARRAY_DATA_16BITSIGNED
#define GAUDIO_PLAY_FORMAT2						ARRAY_DATA_8BITUNSIGNED
#define GAUDIO_PLAY_NUM_CHANNELS				2
#define GAUDIO_PLAY_CHANNEL0_IS_STEREO			FALSE
#define GAUDIO_PLAY_CHANNEL1_IS_STEREO			TRUE
#define	GAUDIO_PLAY_MONO						0
#define	GAUDIO_PLAY_STEREO						1

#endif	/* GFX_USE_GAUDIO && GAUDIO_NEED_PLAY */

#endif	/* GAUDIO_PLAY_CONFIG_H */
