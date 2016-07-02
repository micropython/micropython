/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * The arduino GOS implementation is similar to the bare metal raw32 gos implementation.
 * It uses cooperative multi-tasking. Be careful
 * 	when writing device drivers not to disturb the assumptions this creates by performing
 * 	call-backs to uGFX code unless you define the INTERRUPTS_OFF() and INTERRUPTS_ON() macros.
 * 	It still requires some C runtime library support...
 * 		enough startup to initialise the stack, interrupts, static data etc and call main().
 * 		setjmp() and longjmp()			- for threading
 * 		memcpy()						- for heap and threading
 * 		malloc(), realloc and free()	- if GOS_RAW_HEAP_SIZE == 0
 *
 * 	You must also define the following routines in your own code so that timing functions will work...
 * 		systemticks_t gfxSystemTicks(void);
 *		systemticks_t gfxMillisecondsToTicks(delaytime_t ms);
 */
#ifndef _GOS_ARDUINO_H
#define _GOS_ARDUINO_H

#if GFX_USE_OS_ARDUINO

#include <Arduino.h>

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

typedef unsigned char	bool_t;

#if 0
	// Already defined by Arduino
	typedef signed char		int8_t;
	typedef unsigned char	uint8_t;
	typedef signed short	int16_t;
	typedef unsigned short	uint16_t;
	typedef signed int		int32_t;
	typedef unsigned int	uint32_t;
	typedef uint32_t		size_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

	void gfxHalt(const char *msg);
	void gfxExit(void);

#ifdef __cplusplus
}
#endif

/*===========================================================================*/
/* Use the generic thread handling and heap handling                         */
/*===========================================================================*/

#define GOS_NEED_X_THREADS	TRUE
#define GOS_NEED_X_HEAP		TRUE

#include "gos_x_threads.h"
#include "gos_x_heap.h"

#endif /* GFX_USE_OS_ARDUINO */
#endif /* _GOS_ARDUINO_H */
