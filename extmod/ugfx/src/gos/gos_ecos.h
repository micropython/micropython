/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GOS_ECOS_H
#define _GOS_ECOS_H

#if GFX_USE_OS_ECOS

#include <cyg/hal/hal_arch.h>
#include <cyg/kernel/kapi.h>
#include <stdlib.h>

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

typedef cyg_bool_t			bool_t;
typedef cyg_int8			int8_t;
typedef cyg_uint8	 		uint8_t;
typedef cyg_int16	 		int16_t;
typedef cyg_uint16	 		uint16_t;
typedef cyg_int32 			int32_t;
typedef cyg_uint32 			uint32_t;
typedef cyg_uint32 			size_t;

#define TRUE				-1
#define FALSE				0
#define TIME_IMMEDIATE		0
#define TIME_INFINITE		0xFFFFFFFF

typedef cyg_ucount32		delaytime_t;
typedef cyg_tick_count_t	systemticks_t;
typedef cyg_count32 		semcount_t;
typedef void				threadreturn_t;
typedef cyg_addrword_t		threadpriority_t;
typedef cyg_handle_t		gfxThreadHandle;

#define MAX_SEMAPHORE_COUNT			0x7FFFFFFF
#define LOW_PRIORITY				(CYGNUM_KERNEL_SCHED_PRIORITIES-2)
#define NORMAL_PRIORITY				(CYGNUM_KERNEL_SCHED_PRIORITIES/2)
#define HIGH_PRIORITY				0

#define DECLARE_THREAD_STACK(name, sz)			struct { cyg_thread t; unsigned char stk[sz]; } name[1]
#define DECLARE_THREAD_FUNCTION(fnName, param)	threadreturn_t fnName(cyg_addrword_t param)
#define THREAD_RETURN(retval)

typedef struct {
	cyg_sem_t	sem;
	semcount_t	limit;
	} gfxSem;

typedef cyg_mutex_t		gfxMutex;


/*===========================================================================*/
/* Function declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#define gfxSystemTicks()			cyg_current_time()
#define gfxExit()					exit(0)
#define gfxHalt(msg)				exit(-1)
#define gfxYield()					cyg_thread_yield()

#define gfxMillisecondsToTicks(ms)	(((ms)*(CYGNUM_HAL_RTC_DENOMINATOR*1000))/(CYGNUM_HAL_RTC_NUMERATOR/1000))
void gfxSleepMilliseconds(delaytime_t ms);
void gfxSleepMicroseconds(delaytime_t ms);

#define gfxAlloc(sz)					malloc(sz)
#define gfxFree(ptr)					free(ptr)
#define gfxRealloc(ptr, oldsz, newsz)	realloc(ptr, newsz)

#define gfxSystemLock()					cyg_scheduler_lock()
#define gfxSystemUnlock()				cyg_scheduler_unlock()

#define gfxMutexInit(pmutex)			cyg_mutex_init(pmutex)
#define gfxMutexExit(pmutex)			cyg_mutex_unlock(pmutex)
#define gfxMutexDestroy(pmutex)			cyg_mutex_destroy(pmutex)
#define gfxMutexEnter(pmutex)			cyg_mutex_lock(pmutex)

void gfxSemInit(gfxSem *psem, semcount_t val, semcount_t limit);
void gfxSemDestroy(gfxSem *psem);
bool_t gfxSemWait(gfxSem *psem, delaytime_t ms);
bool_t gfxSemWaitI(gfxSem *psem);
void gfxSemSignal(gfxSem *psem);
void gfxSemSignalI(gfxSem *psem);
semcount_t gfxSemCounterI(gfxSem *psem);
#define gfxSemCounter(psem)			gfxSemCounterI(psem)

gfxThreadHandle gfxThreadCreate(void *stackarea, size_t stacksz, threadpriority_t prio, DECLARE_THREAD_FUNCTION((*fn),p), void *param);
#define gfxThreadWait(thread)		NOTIMPLEMENTED_YET
#define gfxThreadMe()				cyg_thread_self()
#define gfxThreadClose(thread)		(void)thread

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_OS_ECOS */
#endif /* _GOS_ECOS_H */
