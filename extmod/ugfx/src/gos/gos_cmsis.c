/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"
#include <string.h>

#if GFX_USE_OS_CMSIS

void _gosHeapInit(void);

void _gosInit(void)
{
	#if !GFX_OS_NO_INIT
		osKernelInitialize();
		if (!osKernelRunning())
			osKernelStart();
	#elif !GFX_OS_INIT_NO_WARNING
		#warning "GOS: Operating System initialization has been turned off. Make sure you call osKernelInitialize() and osKernelStart() before gfxInit() in your application!"
	#endif

	// Set up the heap allocator
	_gosHeapInit();
}

void _gosDeinit(void)
{
}

void gfxMutexInit(gfxMutex* pmutex)
{
	osMutexDef_t def;
	def.mutex = pmutex->mutex;
	
	pmutex->id = osMutexCreate(&def);
}

void gfxSemInit(gfxSem* psem, semcount_t val, semcount_t limit)
{
	osSemaphoreDef_t def;
	def.semaphore = psem->semaphore;
	
	if (val > limit) val = limit;
	psem->available = limit - val;
	psem->id = osSemaphoreCreate(&def, val);
}

void gfxSemDestroy(gfxSem* psem)
{
	osSemaphoreDelete(psem->id);
}

bool_t gfxSemWait(gfxSem* psem, delaytime_t ms)
{
	if (osSemaphoreWait(psem->id, ms) > 0) {
		psem->available++;
		return TRUE;
	}
	return FALSE;
}

bool_t gfxSemWaitI(gfxSem* psem)
{
	return gfxSemWait(psem, 0);
}

void gfxSemSignal(gfxSem* psem)
{
	gfxSemSignalI(psem);
}

void gfxSemSignalI(gfxSem* psem)
{
	if (psem->available) {
		psem->available--;
		osSemaphoreRelease(psem->id);
	}
}

gfxThreadHandle gfxThreadCreate(void* stackarea, size_t stacksz, threadpriority_t prio, DECLARE_THREAD_FUNCTION((*fn),p), void* param)
{	
	osThreadDef_t def;

	(void)stackarea;
	
	def.pthread = (os_pthread)fn;
	def.tpriority = prio;
	def.instances = 1;
	def.stacksize = stacksz;

	return osThreadCreate(&def, param);
}

threadreturn_t gfxThreadWait(gfxThreadHandle thread) {
	while(osThreadGetPriority(thread) == osPriorityError)
		gfxYield();
}

#endif /* GFX_USE_OS_CMSIS */
