/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_OS_ARDUINO

#include <string.h>				// Prototype for memcpy()

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
		#warning "GOS: Arduino - Make sure you initialize your hardware and the C runtime before calling gfxInit() in your application!"
	#endif

	// Start the heap allocator
	_gosHeapInit();

	// Start the scheduler
	_gosThreadsInit();
}

void _gosDeinit(void)
{
	/* ToDo */
}

/*********************************************************
 * Exit everything functions
 *********************************************************/

void gfxHalt(const char *msg) {
	volatile uint32_t	dummy;
	(void)				msg;

	while(1)
		dummy++;
}

void gfxExit(void) {
	volatile uint32_t	dummy;

	while(1)
		dummy++;
}


/*********************************************************
 * Sleep functions
 *********************************************************/

systemticks_t gfxSystemTicks(void) {
	return millis();
}
systemticks_t gfxMillisecondsToTicks(delaytime_t ms) {
	return ms;
}

#endif /* GFX_USE_OS_ARDUINO */
