/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gtimer/gtimer.h
 *
 * @addtogroup GTIMER
 *
 * @brief	Module which provides software based timers for user-space applications
 *
 * @details	The reason why ChibiOS/GFX has it's own timer abstraction is because
 *			virtual timers provided by ChibiOS/RT are interrupt context only.
 *			While great for what they are designed for, they make coding of the input
 *			drivers much more complex.
 *			For non-performance critical drivers like these input drivers,  it would also
 *			hog an in-ordinate amount of critical (interrupt locked) system time.
 *			This contrary to the goals of a real-time operating system. So a user-land
 *			(thread based) timer mechanism is also required.
 *
 * @pre		GFX_USE_GTIMER must be set to TRUE in your gfxconf.h
 *
 * @{
 */

#ifndef _GTIMER_H
#define _GTIMER_H

#include "../../gfx.h"

#if GFX_USE_GTIMER || defined(__DOXYGEN__)

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

/* Data part of a static GTimer initialiser */
#define _GTIMER_DATA() {0,0,0,0,0,0,0}

/* Static GTimer initialiser */
#define GTIMER_DECL(name) GTimer name = _GTIMER_DATA()

/* A callback function (executed in a thread context) */
typedef void (*GTimerFunction)(void *param);

/**
 * @brief	 A GTimer structure
 */
typedef struct GTimer_t {
	GTimerFunction		fn;
	void				*param;
	systemticks_t		when;
	systemticks_t		period;
	uint16_t			flags;
	struct GTimer_t		*next;
	struct GTimer_t		*prev;
} GTimer;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialise a timer
 *
 * @param[in] pt 	Pointer to a GTimer structure
 *
 * @api
 */
void gtimerInit(GTimer* pt);

/**
 * @brief	Deinitialise a timer
 *
 * @param[in] pt	Pointer to a GTimer structure
 *
 * @api
 */
void gtimerDeinit(GTimer* pt);

/**
 * @brief   Set a timer going or alter its properties if it is already going.
 *
 * @param[in] pt	Pointer to a GTimer structure
 * @param[in] fn		The callback function
 * @param[in] param		The parameter to pass to the callback function
 * @param[in] periodic	Is the timer a periodic timer? FALSE is a once-only timer.
 * @param[in] millisec	The timer period. The following special values are allowed:
 *							TIME_IMMEDIATE	causes the callback function to be called asap.
 *											A periodic timer with this value will fire once only.
 *							TIME_INFINITE	never timeout (unless triggered by gtimerJab or gtimerJabI)
 *
 * @note				If the timer is already active its properties are updated with the new parameters.
 *						The current period will be immediately canceled (without the callback function being
 *						called) and the timer will be restart with the new timer properties.
 * @note				The callback function should be careful not to over-run the thread stack.
 *						Define a new value for the macro GTIME_THREAD_STACK_SIZE if you want to
 *						change the default size.
 * @note				The callback function should return as quickly as possible as all
 *						timer callbacks are performed by a single thread. If a callback function
 *						takes too long it could affect the timer response for other timers.
 * @note				A timer callback function is not a replacement for a dedicated thread if the
 *						function wants to perform computationally expensive stuff.
 * @note				As the callback function is called on GTIMER's thread, the function must make sure it uses
 *						appropriate synchronisation controls such as semaphores or mutexes around any data
 *						structures it shares with other threads such as the main application thread.
 *
 * @api
 */
void gtimerStart(GTimer *pt, GTimerFunction fn, void *param, bool_t periodic, delaytime_t millisec);

/**
 * @brief   Stop a timer (periodic or otherwise)
 *
 * @param[in] pt		Pointer to a GTimer structure
 *
 * @note				If the timer is not active this does nothing.
 *
 * @api
 */
void gtimerStop(GTimer *pt);

/**
 * @brief   Test if a timer is currently active
 *
 * @param[in] pt		Pointer to a GTimer structure
 *
 * @return	TRUE if active, FALSE otherwise
 *
 * @api
 */
bool_t gtimerIsActive(GTimer *pt);

/**
 * @brief   			Jab a timer causing the current period to immediate expire
 * @details				The callback function will be called as soon as possible.
 *
 * @pre					Use from a normal thread context.
 *
 * @param[in] pt		Pointer to a GTimer structure
 *
 * @note				If the timer is not active this does nothing.
 * @note				Repeated Jabs before the callback function actually happens are ignored.
 *
 * @api
 */
void gtimerJab(GTimer *pt);

/**
 * @brief   			Jab a timer causing the current period to immediate expire
 * @details				The callback function will be called as soon as possible.
 *
 * @pre					Use from an interrupt routine context.
 *
 * @param[in] pt		Pointer to a GTimer structure
 *
 * @note				If the timer is not active this does nothing.
 * @note				Repeated Jabs before the callback function actually happens are ignored.
 *
 * @iclass
 * @api
 */
void gtimerJabI(GTimer *pt);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GTIMER */

#endif /* _GTIMER_H */
/** @} */

