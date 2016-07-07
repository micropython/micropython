/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gaudio/gaudio_driver_play.h
 * @brief   GAUDIO - Audio play driver header file.
 *
 * @defgroup GAUDIO_Driver_Playback Driver Playback
 * @ingroup GAUDIO
 *
 * @brief Playback driver interface for the GAUDIO module.
 *
 * @{
 */

#ifndef _GAUDIO_PLAY_LLD_H
#define _GAUDIO_PLAY_LLD_H

#include "../../gfx.h"

#if (GFX_USE_GAUDIO && GAUDIO_NEED_PLAY) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief				Get a block of audio data to play
 * @return				A pointer to the GAaudioData structure or NULL if none is currently available
 *
 * @note				Defined in the high level GAUDIO code for use by the GAUDIO play drivers.
 *
 * @iclass
 * @notapi
 */
GDataBuffer *gaudioPlayGetDataBlockI(void);

/**
 * @brief				Release a block of audio data to the free list
 *
 * @param[in] paud		The GDataBuffer block to be released.
 *
 * @note				Defined in the high level GAUDIO code for use by the GAUDIO play drivers.
 *
 * @iclass
 * @notapi
 */
void gaudioPlayReleaseDataBlockI(GDataBuffer *paud);

/**
 * @brief				Signal that all playing has now stopped
 *
 * @note				Defined in the high level GAUDIO code for use by the GAUDIO play drivers.
 *
 * @iclass
 * @notapi
 */
void gaudioPlayDoneI(void);

/**
 * @brief				Initialise the play driver
 * @return				TRUE if the channel, frequency and format are valid.
 *
 * @param[in] channel	The channel to use (see the driver for the available channels provided)
 * @param[in] frequency	The sample frequency to use
 * @param[in] format	The sample format
 *
 * @note				The driver will always have been stopped and de-init before this is called.
 *
 * @api
 */
bool_t gaudio_play_lld_init(uint16_t channel, uint32_t frequency, ArrayDataFormat format);

/**
 * @brief				Start the audio output playing
 *
 * @note				This may be called at any stage including while the driver
 * 						is already playing. The driver should check for data blocks
 * 						to play using @p gaudioPlayGetDataBlockI().
 *
 * @api
 */
void gaudio_play_lld_start(void);

/**
 * @brief				Stop the audio output playing.
 *
 * @note				Some drivers may only stop playing at a data block boundary.
 * @note				It is possible but unlikely for it to be called when playing has already stopped.
 * @note				It should not return until all active buffers (currently in use by the driver)
 * 						have been returned to the free-list and @p gaudioPlayDoneI() has been called.
 *
 * @api
 */
void gaudio_play_lld_stop(void);

/**
 * @brief				Set the output volume.
 * @return				TRUE if successful.
 *
 * @param[in] vol		0->255 (0 = muted)
 *
 * @note				Some drivers may not support this. They will return FALSE.
 * @note				For stereo devices, both channels are set to the same volume.
 *
 * @api
 */
bool_t gaudio_play_lld_set_volume(uint8_t vol);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GAUDIO && GAUDIO_NEED_PLAY */

#endif /* _GAUDIO_PLAY_LLD_H */
/** @} */
