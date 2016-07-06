/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"
#include <string.h>

#if GFX_USE_OS_FREERTOS

#if INCLUDE_vTaskDelay != 1
	#error "GOS: INCLUDE_vTaskDelay must be defined in FreeRTOSConfig.h"
#endif

#if configUSE_MUTEXES != 1
	#error "GOS: configUSE_MUTEXES must be defined in FreeRTOSConfig.h"
#endif

#if configUSE_COUNTING_SEMAPHORES != 1
	#error "GOS: configUSE_COUNTING_SEMAPHORES must be defined in FreeRTOSConfig.h"
#endif

void _gosInit(void)
{
	#if !GFX_OS_NO_INIT
		#error "GOS: Operating System initialization for FreeRTOS is not yet implemented in uGFX. Please set GFX_OS_NO_INIT to TRUE in your gfxconf.h"
	#endif
	#if !GFX_OS_INIT_NO_WARNING
		#warning "GOS: Operating System initialization has been turned off. Make sure you call vTaskStartScheduler() before gfxInit() in your application!"
	#endif
}

void _gosDeinit(void)
{
}

void* gfxRealloc(void *ptr, size_t oldsz, size_t newsz)
{
	void *np;

	if (newsz <= oldsz)
		return ptr;

	np = gfxAlloc(newsz);
	if (!np)
		return 0;

	if (oldsz) {
		memcpy(np, ptr, oldsz);
		vPortFree(ptr);
	}

	return np;
}

void gfxSleepMilliseconds(delaytime_t ms)
{
	const portTickType ticks = ms / portTICK_PERIOD_MS;
	vTaskDelay(ticks);
}

void gfxSleepMicroseconds(delaytime_t ms)
{
	const portTickType ticks = (ms / 1000) / portTICK_PERIOD_MS;

	// delay milli seconds
	vTaskDelay(ticks);

	// microsecond resolution delay is not supported in FreeRTOS
	// vUsDelay(ms%1000);
}

portTickType MS2ST(portTickType ms)
{
	return (ms / portTICK_PERIOD_MS);
}

void gfxMutexInit(xSemaphoreHandle *s)
{
	*s = xSemaphoreCreateMutex();
	#if GFX_FREERTOS_USE_TRACE
		vTraceSetMutexName(*s,"uGFXMutex"); // for FreeRTOS+Trace debug
	#endif
}

void gfxSemInit(gfxSem* psem, semcount_t val, semcount_t limit)
{
	if (val > limit)
		val = limit;

	psem->counter = val;
	psem->limit = limit;
	psem->sem = xSemaphoreCreateCounting(limit,val);

	#if GFX_FREERTOS_USE_TRACE
		vTraceSetSemaphoreName(psem->sem, "uGFXSema"); // for FreeRTOS+Trace debug
	#endif
}

void gfxSemDestroy(gfxSem* psem)
{
	vSemaphoreDelete(psem->sem);
}

bool_t gfxSemWait(gfxSem* psem, delaytime_t ms)
{
	psem->counter--;

	if (xSemaphoreTake(psem->sem, MS2ST(ms)) == pdPASS)
		return TRUE;

	psem->counter++;

	return FALSE;
}

bool_t gfxSemWaitI(gfxSem* psem)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	psem->counter--;

	if (xSemaphoreTakeFromISR(psem->sem,&xHigherPriorityTaskWoken) == pdTRUE)
		return TRUE;

	psem->counter++;

	return FALSE;
}

void gfxSemSignal(gfxSem* psem)
{
	taskENTER_CRITICAL();

	if (psem->counter < psem->limit) {
		psem->counter++;
		xSemaphoreGive(psem->sem);
	}

	taskYIELD();
	taskEXIT_CRITICAL();
}

void gfxSemSignalI(gfxSem* psem)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if (psem->counter < psem->limit) {
		psem->counter++;
		xSemaphoreGiveFromISR(psem->sem,&xHigherPriorityTaskWoken);
	}
}

gfxThreadHandle gfxThreadCreate(void *stackarea, size_t stacksz, threadpriority_t prio, DECLARE_THREAD_FUNCTION((*fn),p), void *param)
{
	xTaskHandle task = NULL;
	stacksz = (size_t)stackarea;

	if (stacksz < configMINIMAL_STACK_SIZE)
		stacksz = configMINIMAL_STACK_SIZE;

	if (xTaskCreate(fn, "uGFX_TASK", stacksz, param, prio, &task )!= pdPASS) {
		for (;;);
	}

	return task;
}

#endif /* GFX_USE_OS_FREERTOS */
