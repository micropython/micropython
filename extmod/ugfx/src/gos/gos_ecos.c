/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_OS_ECOS

void _gosInit(void)
{
	#if !GFX_OS_NO_INIT
		#error "GOS: Operating System initialization for eCos is not yet implemented in uGFX. Please set GFX_OS_NO_INIT to TRUE in your gfxconf.h"
	#endif
	#if !GFX_OS_INIT_NO_WARNING
		#warning "GOS: Operating System initialization has been turned off. Make sure you call cyg_scheduler_start() before gfxInit() in your application!"
	#endif
}

void _gosDeinit(void)
{
	/* ToDo */
}

void gfxSleepMilliseconds(delaytime_t ms)
{
	switch(ms) {
		case TIME_IMMEDIATE:	cyg_thread_yield();								return;
		case TIME_INFINITE:		cyg_thread_suspend(cyg_thread_self());			return;
		default:				cyg_thread_delay(gfxMillisecondsToTicks(ms));	return;
	}
}

void gfxSleepMicroseconds(delaytime_t ms)
{
	switch(ms) {
		case TIME_IMMEDIATE:														return;
		case TIME_INFINITE:		cyg_thread_suspend(cyg_thread_self());				return;
		default:				cyg_thread_delay(gfxMillisecondsToTicks(ms/1000));	return;
	}
}

void gfxSemInit(gfxSem *psem, semcount_t val, semcount_t limit)
{
	if (val > limit)
		val = limit;

	psem->limit = limit;
	cyg_semaphore_init(&psem->sem, val);
}

void gfxSemDestroy(gfxSem *psem)
{
	cyg_semaphore_destroy(&psem->sem);
}

bool_t gfxSemWait(gfxSem *psem, delaytime_t ms)
{
	switch(ms) {
	case TIME_IMMEDIATE:	return cyg_semaphore_trywait(&psem->sem);
	case TIME_INFINITE:		return cyg_semaphore_wait(&psem->sem);
	default:				return cyg_semaphore_timed_wait(&psem->sem, gfxMillisecondsToTicks(ms)+cyg_current_time());
	}
}

bool_t gfxSemWaitI(gfxSem *psem)
{
	return cyg_semaphore_trywait(&psem->sem);
}

void gfxSemSignal(gfxSem *psem)
{
	if (psem->limit == MAX_SEMAPHORE_COUNT)
		cyg_semaphore_post(&psem->sem);
	else {
		cyg_scheduler_lock();
		if (gfxSemCounterI(psem) < psem->limit)
			cyg_semaphore_post(&psem->sem);
		cyg_scheduler_unlock();
	}
}

void gfxSemSignalI(gfxSem *psem)
{
	if (psem->limit == MAX_SEMAPHORE_COUNT || gfxSemCounterI(psem) < psem->limit)
		cyg_semaphore_post(&psem->sem);
}

semcount_t gfxSemCounterI(gfxSem *psem) {
	semcount_t	cnt;

	cyg_semaphore_peek(&psem->sem, &cnt);
	return cnt;
}

gfxThreadHandle gfxThreadCreate(void *stackarea, size_t stacksz, threadpriority_t prio, DECLARE_THREAD_FUNCTION((*fn),p), void *param)
{
	gfxThreadHandle		th;

	if (!stackarea) {
		if (!stacksz) stacksz = CYGNUM_HAL_STACK_SIZE_TYPICAL;
		if (!(stackarea = gfxAlloc(stacksz+sizeof(cyg_thread))))
			return 0;
	}

	if (!stacksz)
		return 0;

    cyg_thread_create(prio, fn, param, "uGFX", (((cyg_thread *)stackarea)+1), stacksz, &th, (cyg_thread *)stackarea);
    cyg_thread_resume(th);
	return th;
}

#endif /* GFX_USE_OS_ECOS */
