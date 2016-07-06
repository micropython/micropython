/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GOS_LINUX_H
#define _GOS_LINUX_H

#if GFX_USE_OS_LINUX

// We don't put this in the general sys_options.h as it is Linux specific.
#ifndef GFX_USE_POSIX_SEMAPHORES
	#define GFX_USE_POSIX_SEMAPHORES	TRUE
#endif

#include <sys/types.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#if GFX_USE_POSIX_SEMAPHORES
	#include <semaphore.h>
#endif

/* Already defined int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, size_t */

typedef int8_t				bool_t;
typedef unsigned long		systemticks_t;
typedef void *				threadreturn_t;
typedef unsigned long		delaytime_t;
typedef pthread_t 			gfxThreadHandle;
typedef int					threadpriority_t;
typedef uint32_t			semcount_t;
typedef pthread_mutex_t		gfxMutex;

#define DECLARE_THREAD_FUNCTION(fnName, param)	threadreturn_t fnName(void *param)
#define DECLARE_THREAD_STACK(name, sz)			uint8_t name[0];
#define THREAD_RETURN(retval)					return retval

#define gfxExit()						exit(0)
#define gfxAlloc(sz)					malloc(sz)
#define gfxRealloc(p,osz,nsz)			realloc(p, nsz)
#define gfxFree(ptr)					free(ptr)
#define gfxMillisecondsToTicks(ms)		(ms)
#define gfxThreadMe()					pthread_self()
#define gfxThreadClose(th)				(void)th
#define gfxMutexInit(pmtx)				pthread_mutex_init(pmtx, 0)
#define gfxMutexDestroy(pmtx)			pthread_mutex_destroy(pmtx)
#define gfxMutexEnter(pmtx)				pthread_mutex_lock(pmtx)
#define gfxMutexExit(pmtx)				pthread_mutex_unlock(pmtx)
#define gfxSemWaitI(psem)				gfxSemWait(psem, TIME_IMMEDIATE)
#define gfxSemSignalI(psem)				gfxSemSignal(psem)

#define TIME_IMMEDIATE				0
#define TIME_INFINITE				((delaytime_t)-1)
#define MAX_SEMAPHORE_COUNT			((semcount_t)-1)
#define LOW_PRIORITY				10
#define NORMAL_PRIORITY				0
#define HIGH_PRIORITY				-10

#if GFX_USE_POSIX_SEMAPHORES
	typedef struct gfxSem {
		sem_t			sem;
		semcount_t		max;
	} gfxSem;
	#define gfxSemCounterI(psem)	gfxSemCounter(psem)
#else
	typedef struct gfxSem {
		pthread_mutex_t	mtx;
		pthread_cond_t	cond;
		semcount_t		cnt;
		semcount_t		max;
	} gfxSem;
	#define gfxSemCounterI(psem)	((psem)->cnt)
#endif

/*===========================================================================*/
/* Function declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

void gfxYield(void);
void gfxHalt(const char *msg);
void gfxSleepMilliseconds(delaytime_t ms);
void gfxSleepMicroseconds(delaytime_t ms);
systemticks_t gfxSystemTicks(void);
void gfxSystemLock(void);
void gfxSystemUnlock(void);
void gfxSemInit(gfxSem *psem, semcount_t val, semcount_t limit);
void gfxSemDestroy(gfxSem *psem);
bool_t gfxSemWait(gfxSem *psem, delaytime_t ms);
void gfxSemSignal(gfxSem *psem);
semcount_t gfxSemCounter(gfxSem *pSem);
gfxThreadHandle gfxThreadCreate(void *stackarea, size_t stacksz, threadpriority_t prio, DECLARE_THREAD_FUNCTION((*fn),p), void *param);
threadreturn_t gfxThreadWait(gfxThreadHandle thread);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_OS_LINUX */

#endif /* _GOS_LINUX_H */
