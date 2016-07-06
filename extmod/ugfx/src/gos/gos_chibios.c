/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_OS_CHIBIOS

#include <string.h>

#if CH_KERNEL_MAJOR == 2

	#if !CH_USE_MUTEXES
		#error "GOS: CH_USE_MUTEXES must be defined in chconf.h"
	#endif
	#if !CH_USE_SEMAPHORES
		#error "GOS: CH_USE_SEMAPHORES must be defined in chconf.h"
	#endif
	
#elif (CH_KERNEL_MAJOR == 3) || (CH_KERNEL_MAJOR == 4)

	#if !CH_CFG_USE_MUTEXES
		#error "GOS: CH_CFG_USE_MUTEXES must be defined in chconf.h"
	#endif
	#if !CH_CFG_USE_SEMAPHORES
		#error "GOS: CH_CFG_USE_SEMAPHORES must be defined in chconf.h"
	#endif

#else
	#error "GOS: Unsupported version of ChibiOS"
#endif

void _gosInit(void)
{
	#if !GFX_OS_NO_INIT
		/* Don't Initialize if the user already has */
		#if CH_KERNEL_MAJOR == 2
			if (!chThdSelf()) {
				halInit();
				chSysInit();
			}
		#elif (CH_KERNEL_MAJOR == 3) || (CH_KERNEL_MAJOR == 4)
			if (!chThdGetSelfX()) {
				halInit();
				chSysInit();
			}
		#endif
	#elif !GFX_OS_INIT_NO_WARNING
		#warning "GOS: Operating System initialization has been turned off. Make sure you call halInit() and chSysInit() before gfxInit() in your application!"
	#endif
}

void _gosDeinit(void)
{
	/* ToDo */
}

void *gfxRealloc(void *ptr, size_t oldsz, size_t newsz)
{
	void *np;

	if (newsz <= oldsz)
		return ptr;

	np = gfxAlloc(newsz);
	if (!np)
		return 0;

	if (oldsz)
		memcpy(np, ptr, oldsz);

	return np;
}

void gfxSleepMilliseconds(delaytime_t ms)
{
	switch(ms) {
		case TIME_IMMEDIATE:	chThdYield();				return;
		case TIME_INFINITE:		chThdSleep(TIME_INFINITE);	return;
		default:				chThdSleepMilliseconds(ms);	return;
	}
}

void gfxSleepMicroseconds(delaytime_t ms)
{
	switch(ms) {
		case TIME_IMMEDIATE:								return;
		case TIME_INFINITE:		chThdSleep(TIME_INFINITE);	return;
		default:				chThdSleepMicroseconds(ms);	return;
	}
}

void gfxSemInit(gfxSem *psem, semcount_t val, semcount_t limit)
{
	if (val > limit)
		val = limit;

	psem->limit = limit;
	
	#if CH_KERNEL_MAJOR == 2
		chSemInit(&psem->sem, val);
	#elif (CH_KERNEL_MAJOR == 3) || (CH_KERNEL_MAJOR == 4)
		chSemObjectInit(&psem->sem, val);
	#endif
}

void gfxSemDestroy(gfxSem *psem)
{
	chSemReset(&psem->sem, 1);
}

bool_t gfxSemWait(gfxSem *psem, delaytime_t ms)
{
	#if CH_KERNEL_MAJOR == 2
		switch(ms) {
		case TIME_IMMEDIATE:	return chSemWaitTimeout(&psem->sem, TIME_IMMEDIATE) != RDY_TIMEOUT;
		case TIME_INFINITE:		chSemWait(&psem->sem);	return TRUE;
		default:				return chSemWaitTimeout(&psem->sem, MS2ST(ms)) != RDY_TIMEOUT;
		}
	#elif (CH_KERNEL_MAJOR == 3) || (CH_KERNEL_MAJOR == 4)
		switch(ms) {
		case TIME_IMMEDIATE:	return chSemWaitTimeout(&psem->sem, TIME_IMMEDIATE) != MSG_TIMEOUT;
		case TIME_INFINITE:		chSemWait(&psem->sem);	return TRUE;
		default:				return chSemWaitTimeout(&psem->sem, MS2ST(ms)) != MSG_TIMEOUT;
		}
	#endif
}

bool_t gfxSemWaitI(gfxSem *psem)
{
	if (chSemGetCounterI(&psem->sem) <= 0)
		return FALSE;
	chSemFastWaitI(&psem->sem);
	return TRUE;
}

void gfxSemSignal(gfxSem *psem)
{
	chSysLock();

	if (gfxSemCounterI(psem) < psem->limit)
		chSemSignalI(&psem->sem);

	chSchRescheduleS();
	chSysUnlock();
}

void gfxSemSignalI(gfxSem *psem)
{
	if (gfxSemCounterI(psem) < psem->limit)
		chSemSignalI(&psem->sem);
}

gfxThreadHandle gfxThreadCreate(void *stackarea, size_t stacksz, threadpriority_t prio, DECLARE_THREAD_FUNCTION((*fn),p), void *param)
{
	if (!stackarea) {
		if (!stacksz) stacksz = 256;
#if (CH_KERNEL_MAJOR == 2) || (CH_KERNEL_MAJOR == 3)
		return chThdCreateFromHeap(0, stacksz, prio, (tfunc_t)fn, param);
#elif CH_KERNEL_MAJOR == 4
		return chThdCreateFromHeap(0, stacksz, "ugfx", prio, (tfunc_t)fn, param);
#endif
	}

	if (!stacksz)
		return 0;

	return chThdCreateStatic(stackarea, stacksz, prio, (tfunc_t)fn, param);
}

#endif /* GFX_USE_OS_CHIBIOS */
