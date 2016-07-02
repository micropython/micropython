/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gos/gos_cmsis.h
 * @brief   GOS - Operating System Support header file for CMSIS RTOS.
 */

#ifndef _GOS_CMSIS_H
#define _GOS_CMSIS_H

#if GFX_USE_OS_CMSIS

#include <stdbool.h>
#include "cmsis_os.h"

#ifndef GFX_OS_HEAP_SIZE
	#define GFX_OS_HEAP_SIZE 10240
#endif

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

typedef bool				bool_t;

#define TIME_IMMEDIATE		0
#define TIME_INFINITE		osWaitForever
typedef uint32_t			delaytime_t;
typedef uint32_t			systemticks_t;
typedef uint16_t			semcount_t;
typedef void				threadreturn_t;
typedef osPriority			threadpriority_t;

#define MAX_SEMAPHORE_COUNT	osFeature_Semaphore
#define LOW_PRIORITY		osPriorityLow
#define NORMAL_PRIORITY		osPriorityNormal
#define HIGH_PRIORITY		osPriorityHigh

typedef struct gfxSem {
	uint32_t		semaphore[2];
	osSemaphoreId	id;
	semcount_t		available;
} gfxSem;

typedef struct gfxMutex {
	uint32_t mutex[4];
	osMutexId id;
} gfxMutex;

typedef osThreadId			gfxThreadHandle;

#define DECLARE_THREAD_STACK(name, sz)			uint8_t name[1];	// Some compilers don't allow zero sized arrays. Let's waste one byte
#define DECLARE_THREAD_FUNCTION(fnName, param)	threadreturn_t fnName(void* param)
#define THREAD_RETURN(retval)

/*===========================================================================*/
/* Function declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#define gfxExit()					os_error(0)
#define gfxHalt(msg)				os_error(1)
#define gfxSystemTicks()			osKernelSysTick()
#define gfxMillisecondsToTicks(ms)	osKernelSysTickMicroSec(1000*ms)
#define gfxSystemLock()				osKernelInitialize()
#define gfxSystemUnlock()			osKernelStart()
#define gfxSleepMilliseconds(ms) 	osDelay(ms)

void gfxMutexInit(gfxMutex* pmutex);
#define gfxMutexDestroy(pmutex)		osMutexDelete((pmutex)->id)
#define gfxMutexEnter(pmutex)		osMutexWait((pmutex)->id, TIME_INFINITE)
#define gfxMutexExit(pmutex)		osMutexRelease((pmutex)->id)

void gfxSemInit(gfxSem* psem, semcount_t val, semcount_t limit);
void gfxSemDestroy(gfxSem* psem);
bool_t gfxSemWait(gfxSem* psem, delaytime_t ms);
bool_t gfxSemWaitI(gfxSem* psem);
void gfxSemSignal(gfxSem* psem);
void gfxSemSignalI(gfxSem* psem);

gfxThreadHandle gfxThreadCreate(void* stackarea, size_t stacksz, threadpriority_t prio, DECLARE_THREAD_FUNCTION((*fn),p), void* param);
#define gfxYield()					osThreadYield()
#define gfxThreadMe()				osThreadGetId()
#define gfxThreadClose(thread)		{}

#ifdef __cplusplus
}
#endif

/*===========================================================================*/
/* Use the generic heap handling                                             */
/*===========================================================================*/

#define GOS_NEED_X_HEAP TRUE
#include "gos_x_heap.h"

#endif /* GFX_USE_OS_CMSIS */
#endif /* _GOS_CMSIS_H */
