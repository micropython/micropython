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

/* Note:
 * 	The VS1053 has an internal processor which can decode many file formats directly.
 * 	If you want to use anything other than raw PCM then you should use GAUDIO_PLAY_FORMAT_FILE and pipe
 * 	the entire file as if it was sound data. It doesn't matter if you choose the mono or stereo channel
 * 	as the codec chip automatically detects the real format itself.
 * 	No testing is made of the file format - if there is an error it can only be detected by the codec chip
 * 	and its behaviour is undefined (we haven't tested).
 * 	Note that some formats require a firmware patch to be installed to play correctly.
 * 	In this case define VS1053_FIRMWARE_PATCH as TRUE in your gfxconf.h file and include the patch file
 * 	in your project directory. The patch file MUST be called "vs1053_patch.plg".
 */
#define GAUDIO_PLAY_MAX_SAMPLE_FREQUENCY		48000
#define GAUDIO_PLAY_NUM_FORMATS					3
#define GAUDIO_PLAY_FORMAT1						ARRAY_DATA_16BITSIGNED
#define GAUDIO_PLAY_FORMAT2						ARRAY_DATA_8BITUNSIGNED
#define GAUDIO_PLAY_FORMAT3						ARRAY_DATA_UNKNOWN
#define GAUDIO_PLAY_FORMAT_FILE					ARRAY_DATA_UNKNOWN
#define GAUDIO_PLAY_NUM_CHANNELS				2
#define GAUDIO_PLAY_CHANNEL0_IS_STEREO			FALSE
#define GAUDIO_PLAY_CHANNEL1_IS_STEREO			TRUE
#define	GAUDIO_PLAY_MONO						0
#define	GAUDIO_PLAY_STEREO						1

#endif	/* GFX_USE_GAUDIO && GAUDIO_NEED_PLAY */

#endif	/* GAUDIO_PLAY_CONFIG_H */
