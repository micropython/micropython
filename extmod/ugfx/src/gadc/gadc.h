/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gadc/gadc.h
 *
 * @addtogroup GADC
 *
 * @brief	Module to abstract the very variable ADC interfaces of the underlying systems
 *
 * @details	The reason why ChibiOS/GFX has it's own ADC abstraction is because
 *			the Chibi-OS drivers are very CPU specific and do not
 *			provide a way across all hardware platforms to create periodic
 *			ADC conversions. There are also issues with devices with different
 *			characteristics or periodic requirements on the same ADC
 *			device (but different channels). This layer attempts to solve these
 *			problems to provide a architecture neutral API. It also provides extra
 *			features such as multi-buffer chaining for high speed ADC sources.
 *			It provides one high speed virtual ADC device (eg a microphone) and
 *			numerous low speed (less than 100Hz) virtual ADC devices (eg dials,
 *			temperature sensors etc). The high speed device has timer based polling
 *			to ensure exact conversion periods and a buffer management system.
 *			The low speed devices are assumed to be non-critical timing devices
 *			and do not have any buffer management.
 *			Note that while only one high speed device has been provided it can
 *			be used to read multiple physical ADC channels on the one physical
 *			ADC device.
 *			All callback routines are thread based unlike the Chibi-OS interrupt based
 *			routines.
 *
 * @{
 */

#ifndef _GADC_H
#define _GADC_H

#include "../../gfx.h"

#if GFX_USE_GADC || defined(__DOXYGEN__)

/* Include the driver defines */
#include "gadc_lld_config.h"

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

// Event types for GADC
#define GEVENT_ADC			(GEVENT_GADC_FIRST+0)

/**
 * @brief   The High Speed ADC event structure.
 * @{
 */
typedef struct GEventADC_t {
	#if GFX_USE_GEVENT || defined(__DOXYGEN__)
		/**
		 * @brief The type of this event (GEVENT_ADC)
		 */
		GEventType		type;
	#endif

	/**
	 * @brief The event flags
	 */
	uint16_t		flags;
		/**
		 * @brief   The event flag values.
		 * @{
		 */
		#define	GADC_HSADC_LOSTEVENT		0x0001		/**< @brief The last GEVENT_HSDADC event was lost */
		#define	GADC_HSADC_RUNNING			0x0002		/**< @brief The High Speed ADC is currently running */
		#define	GADC_HSADC_GOTBUFFER		0x0004		/**< @brief A buffer is ready for processing */
		#define	GADC_HSADC_STALL			0x0008		/**< @brief The High Speed ADC has stalled due to no free buffers */
		/** @} */
} GEventADC;
/** @} */

/**
 * @brief A callback function (executed in a thread context) for a low speed conversion
 */
typedef void (*GADCCallbackFunction)(adcsample_t *buffer, void *param);

/**
 * @brief A callback function (executed in an ISR context) for a high speed conversion
 */
typedef void (*GADCISRCallbackFunction)(void);

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief				Initialise the high speed ADC.
 * @details				Initialises but does not start the conversions.
 *
 * @param[in] physdev			A value passed to describe which physical ADC devices/channels to use.
 * @param[in] frequency			The frequency to create ADC conversions
 *
 * @note				If the high speed ADC is running it will be stopped. The Event subsystem is
 * 						disconnected from the high speed ADC and any binary semaphore event is forgotten.
 * @note				ChibiOS ONLY: Due to a bug in ChibiOS each buffer on the free-list must contain an even number of
 * 						samples and for multi-channel devices it must hold a number of samples that is evenly divisible
 * 						by 2 times the number of active channels.
 * @note				The physdev parameter may be used to turn on more than one ADC channel.
 * 						Each channel is then interleaved into the provided buffer. Make sure your buffers all hold
 * 						a number of samples evenly divisible by the number of active channels.
 * 						As an example, if physdev turns on 2 devices then the buffer contains
 * 						alternate device samples and the buffer must contain multiples of 2 samples.
 * 						The exact meaning of physdev is hardware dependent.
 * @note				While the high speed ADC is running, low speed conversions can only occur at
 * 						the frequency of the high speed events. Thus if high speed events are
 * 						being created at 50Hz (eg 100 samples/buffer, frequency = 5kHz) then the maximum
 * 						frequency for low speed conversions will be 50Hz.
 * @note				Only a single sample format is supported - that provided by the GADC driver. That sample
 * 						format applies to both high speed and low speed sampling.
 *
 * @api
 */
void gadcHighSpeedInit(uint32_t physdev, uint32_t frequency);

#if GFX_USE_GEVENT || defined(__DOXYGEN__)
	/**
	 * @brief   			Turn on sending results to the GEVENT sub-system.
	 * @details				Returns a GSourceHandle to listen for GEVENT_ADC events.
	 *
	 * @note				The high speed ADC will not use the GEVENT system unless this is
	 * 						called first. This saves processing time if the application does
	 * 						not want to use the GEVENT sub-system for the high speed ADC.
	 * 						Once turned on it can only be turned off by calling @p gadcHighSpeedInit() again.
	 * @note				The high speed ADC is capable of signalling via this method, an ISR callback and a
	 * 						binary semaphore at the same time.
	 *
	 * @return				The GSourceHandle
	 *
	 * @api
	 */
	GSourceHandle gadcHighSpeedGetSource(void);
#endif

/**
 * @brief				Allow retrieving of results from the high speed ADC using an ISR callback.
 *
 * @param[in] isrfn			The callback function (called in an ISR context).
 *
 * @note				Passing a NULL for isrfn will turn off signalling via this method as will calling
 * 						@p gadcHighSpeedInit().
 * @note				The high speed ADC is capable of signalling via this method, a blocked thread and the GEVENT
 * 						sub-system at the same time.
 *
 * @api
 */
void gadcHighSpeedSetISRCallback(GADCISRCallbackFunction isrfn);

/**
 * @brief		Get a filled buffer from the ADC
 * @return		A GDataBuffer pointer or NULL if the timeout is exceeded
 *
 * @param[in] ms	The maximum amount of time in milliseconds to wait for data if some is not currently available.
 *
 * @note		After processing the data, your application must return the buffer to the free-list so that
 * 				it can be used again. This can be done using @p gfxBufferRelease().
 * @note		A buffer may be returned to the free-list before you have finished processing it provided you finish
 * 				processing it before GADC re-uses it. This is useful when RAM usage is critical to reduce the number
 * 				of buffers required. It works before the free list is a FIFO queue and therefore buffers are kept
 * 				in the queue as long as possible before they are re-used.
 * @note		The function ending with "I" is the interrupt class function.
 * @api
 * @{
 */
GDataBuffer *gadcHighSpeedGetData(delaytime_t ms);
GDataBuffer *gadcHighSpeedGetDataI(void);
/** @} */

/**
 * @brief   Start the high speed ADC conversions.
 * @pre		It must have been initialised first with @p gadcHighSpeedInit()
 *
 * @api
 */
void gadcHighSpeedStart(void);

/**
 * @brief   Stop the high speed ADC conversions.
 *
 * @api
 */
void gadcHighSpeedStop(void);

/**
 * @brief	Perform a single low speed ADC conversion
 * @details	Blocks until the conversion is complete
 * @pre		This should not be called from within a GTimer callback as this routine
 * 			blocks until the conversion is ready.
 *
 * @param[in] physdev		A value passed to describe which physical ADC devices/channels to use.
 * @param[in] buffer		The static buffer to put the ADC samples into.
 *
 * @note	This may take a while to complete if the high speed ADC is running as the
 * 			conversion is interleaved with the high speed ADC conversions on a buffer
 * 			completion.
 * @note	The result buffer must be large enough to store one sample per device
 * 			described by the 'physdev' parameter.
 * @note	Specifying more than one device in physdev is possible but discouraged as the
 * 			calculations to ensure the high speed ADC correctness will be incorrect. Symptoms
 * 			from over-running the high speed ADC include high speed device stalling or samples being lost.
 *
 * @api
 */
void gadcLowSpeedGet(uint32_t physdev, adcsample_t *buffer);

/**
 * @brief	Perform a low speed ADC conversion with callback (in a thread context)
 * @details	Returns FALSE if internal memory allocation fails
 *
 * @param[in] physdev		A value passed to describe which physical ADC devices/channels to use.
 * @param[in] buffer		The static buffer to put the ADC samples into.
 * @param[in] fn			The callback function to call when the conversion is complete.
 * @param[in] param			A parameter to pass to the callback function.
 *
 * @return					FALSE if no free low speed ADC slots.
 *
 * @note	This may be safely called from within a GTimer callback.
 * @note	The callback may take a while to occur if the high speed ADC is running as the
 * 			conversion is interleaved with the high speed ADC conversions on a buffer
 * 			completion.
 * @note	The result buffer must be large enough to store one sample per device
 * 			described by the 'physdev' parameter.
 * @note	Specifying more than one device in physdev is possible but discouraged as the
 * 			calculations to ensure the high speed ADC correctness will be incorrect. Symptoms
 * 			from over-running the high speed ADC include high speed samples being lost.
 *
 * @api
 */
bool_t gadcLowSpeedStart(uint32_t physdev, adcsample_t *buffer, GADCCallbackFunction fn, void *param);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GADC */

#endif /* _GADC_H */
/** @} */
