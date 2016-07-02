/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gaudio/gaudio.h
 *
 * @addtogroup GAUDIO
 *
 * @brief	Module to handle audio recording and play-back
 *
 * @{
 */

#ifndef _GAUDIO_H
#define _GAUDIO_H

#include "../../gfx.h"

#if GFX_USE_GAUDIO || defined(__DOXYGEN__)

/* Include the driver defines */
#if GAUDIO_NEED_PLAY
	#include "gaudio_play_config.h"
#endif
#if GAUDIO_NEED_RECORD
	#include "gaudio_record_config.h"
#endif

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

// Event types for GAUDIO
#define GEVENT_AUDIO_PLAY			(GEVENT_GAUDIO_FIRST+0)
#define GEVENT_AUDIO_RECORD			(GEVENT_GAUDIO_FIRST+1)

#if GFX_USE_GEVENT || defined(__DOXYGEN__)
	/**
	 * @brief   The Audio play event structure.
	 * @{
	 */
	typedef struct GEventAudioPlay_t {
		/**
		 * @brief The type of this event (GEVENT_AUDIO_PLAY)
		 */
		GEventType				type;
		/**
		 * @brief The event flags
		 */
		uint16_t				flags;
			/**
			 * @brief   The event flag values.
			 * @{
			 */
			#define	GAUDIO_PLAY_LOSTEVENT		0x0001		/**< @brief The last GEVENT_AUDIO_PLAY event was lost */
			#define	GAUDIO_PLAY_PLAYING			0x0002		/**< @brief The audio out system is currently playing */
			#define	GAUDIO_PLAY_FREEBLOCK		0x0004		/**< @brief An audio buffer has been freed */
			/** @} */
	} GEventAudioPlay;
	/** @} */

	/**
	 * @brief   The Audio record event structure.
	 * @{
	 */
	typedef struct GEventAudioRecord_t {
		/**
		 * @brief The type of this event (GEVENT_AUDIO_RECORD)
		 */
		GEventType				type;
		/**
		 * @brief The event flags
		 */
		uint16_t				flags;
			/**
			 * @brief   The event flag values.
			 * @{
			 */
			#define	GAUDIO_RECORD_LOSTEVENT		0x0001		/**< @brief The last GEVENT_AUDIO_IN event was lost */
			#define	GAUDIO_RECORD_RECORDING		0x0002		/**< @brief The audio recording system is currently recording */
			#define	GAUDIO_RECORD_GOTBUFFER		0x0004		/**< @brief An audio buffer is ready for processing */
			#define	GAUDIO_RECORD_STALL			0x0008		/**< @brief The recording process has stalled due to no free buffers */
			/** @} */
	} GEventAudioRecord;
	/** @} */
#endif

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#if GAUDIO_NEED_PLAY || defined(__DOXYGEN__)
	/**
	 * @brief		Set the audio device to play on the specified channel and with the specified
	 * 				sample frequency.
	 * @return		TRUE is successful, FALSE if the driver doesn't accept those parameters.
	 *
	 * @param[in] channel	The audio output channel to use. Can be set from 0 to GAUDIO_PLAY_NUM_CHANNELS - 1
	 * @param[in] frequency	The audio sample rate in samples per second
	 * @param[in] format	The audio sample format
	 *
	 * @note		Some channels are mono, and some are stereo. See your driver config file
	 * 				to determine which channels to use and whether they are stereo or not.
	 * @note		Only one channel can be playing at a time. Calling this will stop any
	 * 				currently playing channel.
	 *
	 * @api
	 */
	bool_t gaudioPlayInit(uint16_t channel, uint32_t frequency, ArrayDataFormat format);

	/**
	 * @brief		Play the specified sample data.
	 * @details		The sample data is output to the audio channel. On completion the buffer is returned to the free-list.
	 * @pre			@p gaudioPlayInit must have been called first to set the channel and sample frequency.
	 *
	 * @param[in] paud	The audio sample buffer to play. It can be NULL (used to restart paused audio)
	 *
	 * @note		Calling this will cancel any pause.
	 * @note		Before calling this function the len field of the GDataBuffer structure must be
	 * 				specified (in bytes).
	 * @note		For stereo channels the sample data is interleaved in the buffer.
	 * @note		This call returns before the data has completed playing. Subject to available buffers (which
	 * 				can be obtained from the free-list), any number of buffers may be played. They will be queued
	 * 				for playing in the order they are supplied to this routine and played when previous buffers are
	 * 				complete. In this way continuous playing can be obtained without audio gaps.
	 *
	 * @api
	 */
	void gaudioPlay(GDataBuffer *paud);

	/**
	 * @brief		Pause any currently playing sounds.
	 *
	 * @note		If nothing is currently playing this routine does nothing. To restart playing call @p gaudioPlay()
	 * 				with or without a new sample buffer.
	 * @note		Some drivers will not respond until a buffer boundary.
	 *
	 * @api
	 */
	void gaudioPlayPause(void);

	/**
	 * @brief		Stop any currently playing sounds.
	 *
	 * @note		This stops any playing sounds and returns any currently queued buffers back to the free-list.
	 * @note		Some drivers will not respond until a buffer boundary.
	 *
	 * @api
	 */
	void gaudioPlayStop(void);

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
	bool_t gaudioPlaySetVolume(uint8_t vol);

	#if GFX_USE_GEVENT || defined(__DOXYGEN__)
		/**
		 * @brief   			Turn on sending results to the GEVENT sub-system.
		 * @details				Returns a GSourceHandle to listen for GEVENT_AUDIO_OUT events.
		 *
		 * @note				The audio output will not use the GEVENT system unless this is
		 * 						called first. This saves processing time if the application does
		 * 						not want to use the GEVENT sub-system for audio output.
		 * 						Once turned on it can only be turned off by calling @p gaudioPlayInit() again.
		 * @note				The audio output is capable of signaling via this method and other methods
		 * 						at the same time.
		 *
		 * @return				The GSourceHandle
		 *
		 * @api
		 */
		GSourceHandle gaudioPlayGetSource(void);
	#endif

	/**
	 * @brief		Wait for any currently playing sounds to complete
	 * @return		TRUE if there is now nothing playing or FALSE if the timeout is exceeded
	 *
	 * @param[in] ms	The maximum amount of time in milliseconds to wait for playing to complete.
	 *
	 * @api
	 */
	bool_t gaudioPlayWait(delaytime_t ms);
#endif

#if GAUDIO_NEED_RECORD || defined(__DOXYGEN__)
	/**
	 * @brief		Initialise (but not start) the Audio Recording sub-system.
	 * @details		Returns FALSE for an invalid channel or other invalid parameter.
	 *
	 * @param[in] channel		The channel to convert. Can be set from 0 to GAUDIO_RECORD_NUM_CHANNELS - 1
	 * @param[in] frequency		The sample frequency
	 * @param[in] format		The audio sample format requested
	 *
	 * @note				Only one channel is active at a time. If an audio input is running it will be stopped.
	 * 						The Event subsystem is disconnected from the audio subsystem and any binary semaphore
	 * 						event is forgotten.
	 * @note				Some channels may be stereo channels which return twice as much sample data with
	 * 						the left and right channel data interleaved. Other channels may be mono channels.
	 * 						Where stereo channels exist the low level driver may also
	 * 						offer the left and right channels separately.
	 * @note				Due to a bug in Chibi-OS each buffer on the free-list must contain an even number of
	 * 						samples and for stereo devices it must hold a number of samples that is evenly divisible by 4.
	 * 						This requirement applies only to ChibiOS where the audio driver uses
	 * 						a ChibiOS hal driver like the cpu ADC driver. This applies even it is used indirectly via
	 * 						the uGFX GADC driver.
	 * @note				The number of samples for stereo devices will be double the number of conversions.
	 * 						Make sure you allocate your buffers large enough. Each channel is then interleaved
	 * 						into the provided buffer.
	 *
	 * @return				FALSE if invalid channel or parameter
	 *
	 * @api
	 */
	bool_t gaudioRecordInit(uint16_t channel, uint32_t frequency, ArrayDataFormat format);

	/**
	 * @brief   Start the audio recording.
	 * @pre		It must have been initialised first with @p gaudioRecordInit()
	 *
	 * @api
	 */
	void gaudioRecordStart(void);

	/**
	 * @brief   Stop the audio recording.
	 *
	 * @note	All audio recording data that has not yet been retrieved is automatically
	 * 			returned to the free-list.
	 * @api
	 */
	void gaudioRecordStop(void);

	/**
	 * @brief		Get a filled audio buffer from the recording list
	 * @return		A GDataBuffer pointer or NULL if the timeout is exceeded
	 *
	 * @param[in] ms	The maximum amount of time in milliseconds to wait for data if some is not currently available.
	 *
	 * @note		After processing the audio data, your application must return the buffer to the free-list so that
	 * 				it can be used to record more audio into. This can be done via the play list using @p gaudioPlay() or
	 * 				directly using @p gfxBufferRelease().
	 * @note		A buffer may be returned to the free-list before you have finished processing it provided you finish
	 * 				processing it before GADC re-uses it. This is useful when RAM usage is critical to reduce the number
	 * 				of buffers required. It works before the free list is a FIFO queue and therefore buffers are kept
	 * 				in the queue as long as possible before they are re-used.
	 *
	 * @api
	 */
	GDataBuffer *gaudioRecordGetData(delaytime_t ms);

	#if GFX_USE_GEVENT || defined(__DOXYGEN__)
		/**
		 * @brief   			Turn on sending results to the GEVENT sub-system.
		 * @details				Returns a GSourceHandle to listen for GEVENT_AUDIO_RECORD events.
		 *
		 * @note				Audio recording will not use the GEVENT system unless this is
		 * 						called first. This saves processing time if the application does
		 * 						not want to use the GEVENT sub-system for audio recording.
		 * 						Once turned on it can only be turned off by calling @p gaudioRecordInit() again.
		 * @note				The audio input is capable of signaling via this and other methods
		 * 						at the same time.
		 *
		 * @return				The GSourceHandle
		 *
		 * @api
		 */
		GSourceHandle gaudioRecordGetSource(void);
	#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GAUDIO */

#endif /* _GAUDIO_H */
/** @} */

