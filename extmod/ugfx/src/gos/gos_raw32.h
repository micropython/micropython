/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * The raw32 GOS implementation supports any 32 bit processor with or without an
 * 	underlying operating system. It uses cooperative multi-tasking. Be careful
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
#ifndef _GOS_RAW32_H
#define _GOS_RAW32_H

#if GFX_USE_OS_RAW32

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

typedef unsigned char	bool_t;

#if __STDC_VERSION__ >= 199901L
	#include <stdint.h>
#elif defined(__GNUC__) || defined(__GNUG__)
	typedef __INT8_TYPE__	int8_t;
	typedef __UINT8_TYPE__	uint8_t;
	typedef __INT16_TYPE__	int16_t;
	typedef __UINT16_TYPE__	uint16_t;
	typedef __INT32_TYPE__	int32_t;
	typedef __UINT32_TYPE__	uint32_t;
#else
	typedef signed char		int8_t;
	typedef unsigned char	uint8_t;
	typedef signed short	int16_t;
	typedef unsigned short	uint16_t;
	typedef signed int		int32_t;
	typedef unsigned int	uint32_t;
#endif

#if defined(__STDC__)
	#include <stddef.h>
#else
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

#endif /* GFX_USE_OS_RAW32 */
#endif /* _GOS_RAW32_H */
