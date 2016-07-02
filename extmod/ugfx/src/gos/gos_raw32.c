/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

// We need to include stdio.h below for Win32 emulation. Turn off GFILE_NEED_STDIO just for this file to prevent conflicts
#define GFILE_NEED_STDIO_MUST_BE_OFF

#include "../../gfx.h"

#if GFX_USE_OS_RAW32

void _gosHeapInit(void);
void _gosThreadsInit(void);

/*********************************************************
 * Initialise
 *********************************************************/

void _gosInit(void)
{
	/* No initialization of the operating system itself is needed as there isn't one.
	 * On the other hand the C runtime should still already be initialized before
	 * getting here!
	 */
	#if !GFX_OS_INIT_NO_WARNING
		#warning "GOS: Raw32 - Make sure you initialize your hardware and the C runtime before calling gfxInit() in your application!"
	#endif

	// Set up the heap allocator
	_gosHeapInit();

	// Start the scheduler
	_gosThreadsInit();
}

void _gosDeinit(void)
{
	/* ToDo */
}

/*********************************************************
 * For Win32 emulation - automatically add the tick functions
 * the user would normally have to provide for bare metal.
 *********************************************************/

#if defined(WIN32)
	// Win32 nasty stuff - we have conflicting definitions for Red, Green & Blue
	#ifndef _WIN32_WINNT
		#define _WIN32_WINNT 0x0501			// Windows XP and up
	#endif
	#undef Red
	#undef Green
	#undef Blue
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#undef WIN32_LEAN_AND_MEAN
	#define Blue			HTML2COLOR(0x0000FF)
	#define Red				HTML2COLOR(0xFF0000)
	#define Green			HTML2COLOR(0x008000)

	#include <stdio.h>
	systemticks_t gfxSystemTicks(void)						{ return GetTickCount(); }
	systemticks_t gfxMillisecondsToTicks(delaytime_t ms)	{ return ms; }
#endif

/*********************************************************
 * Exit everything functions
 *********************************************************/

void gfxHalt(const char *msg) {
	#if defined(WIN32)
		fprintf(stderr, "%s\n", msg);
		ExitProcess(1);
	#else
		volatile uint32_t	dummy;
		(void)				msg;

		while(1)
			dummy++;
	#endif
}

void gfxExit(void) {
	#if defined(WIN32)
		ExitProcess(0);
	#else
		volatile uint32_t	dummy;

		while(1)
			dummy++;
	#endif
}

#endif /* GFX_USE_OS_RAW32 */
