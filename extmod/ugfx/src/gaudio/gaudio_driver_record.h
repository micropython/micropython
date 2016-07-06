/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gaudio/gaudio_driver_record.h
 * @brief   GAUDIO - Audio Recording driver header file.
 *
 * @defgroup GAUDIO_Driver_Record Driver Recording
 * @ingroup GAUDIO
 *
 * @brief Recording driver interface for the GAUDIO module.
 *
 * @{
 */

#ifndef _GAUDIO_RECORD_LLD_H
#define _GAUDIO_RECORD_LLD_H

#include "../../gfx.h"

#if (GFX_USE_GAUDIO && GAUDIO_NEED_RECORD) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

/**
 * @brief				Get a free block of audio data that we can record into
 * @return				A pointer to the GAaudioData structure or NULL if none is currently available
 *
 * @note				Defined in the high level GAUDIO code for use by the GAUDIO record drivers.
 *
 * @iclass
 * @notapi
 */
#define gaudioRecordGetFreeBlockI()		gfxBufferGetI()

/**
 * @brief				Save a block of recorded audio data ready for the application
 *
 * @param[in] paud		The GDataBuffer block with data.
 *
 * @note				Defined in the high level GAUDIO code for use by the GAUDIO record drivers.
 *
 * @iclass
 * @notapi
 */
void gaudioRecordSaveDataBlockI(GDataBuffer *paud);

/**
 * @brief				Signal that all recording has now stopped
 *
 * @note				Defined in the high level GAUDIO code for use by the GAUDIO record drivers.
 *
 * @iclass
 * @notapi
 */
void gaudioRecordDoneI(void);

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief				Initialise the record driver
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
bool_t gaudio_record_lld_init(uint16_t channel, uint32_t frequency, ArrayDataFormat format);

/**
 * @brief				Start the audio recording
 *
 * @api
 */
void gaudio_record_lld_start(void);

/**
 * @brief				Stop the audio recording.
 *
 * @note				Some drivers may only stop recording at a data block boundary.
 * @note				This routine should not return until any currently active buffers have been
 * 						saved (even if with zero length) and @p gaudioRecordDoneI() has been called.
 *
 * @api
 */
void gaudio_record_lld_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GAUDIO && GAUDIO_NEED_RECORD */

#endif /* _GAUDIO_RECORD_LLD_H */
/** @} */
