/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gos/gos_win32.h
 * @brief   GOS - Operating System Support header file for WIN32.
 */

#ifndef _GOS_WIN32_H
#define _GOS_WIN32_H

#if GFX_USE_OS_WIN32

#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x0501			// Windows XP and up
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

#include <malloc.h>

/* Stop cygwin from defining these types */
#define __int8_t_defined

/**
 * size_t
 * TRUE, FALSE
 * are already defined by Win32
 */
typedef __int8				bool_t;
typedef __int8				int8_t;
typedef unsigned __int8		uint8_t;
typedef __int16				int16_t;
typedef unsigned __int16	uint16_t;
typedef __int32				int32_t;
typedef unsigned __int32	uint32_t;
typedef DWORD				delaytime_t;
typedef DWORD				systemticks_t;
typedef LONG				semcount_t;
typedef DWORD				threadreturn_t;
typedef int					threadpriority_t;

#define DECLARE_THREAD_FUNCTION(fnName, param)	threadreturn_t WINAPI fnName(void *param)
#define DECLARE_THREAD_STACK(name, sz)			uint8_t name[0];
#define THREAD_RETURN(retval)					return retval

#define TIME_IMMEDIATE				0
#define TIME_INFINITE				INFINITE
#define MAX_SEMAPHORE_COUNT			((semcount_t)(((unsigned long)((semcount_t)(-1))) >> 1))
#define LOW_PRIORITY				THREAD_PRIORITY_BELOW_NORMAL
#define NORMAL_PRIORITY				THREAD_PRIORITY_NORMAL
#define HIGH_PRIORITY				THREAD_PRIORITY_ABOVE_NORMAL

typedef HANDLE gfxSem;
typedef HANDLE gfxMutex;
typedef HANDLE gfxThreadHandle;

#define gfxExit()						ExitProcess(0)
#define gfxAlloc(sz)					malloc(sz)
#define gfxRealloc(p,osz,nsz)			realloc(p, nsz)
#define gfxFree(ptr)					free(ptr)
#define gfxSleepMilliseconds(ms)		Sleep(ms)
#define gfxYield()						Sleep(0)
#define gfxSystemTicks()				GetTickCount()
#define gfxMillisecondsToTicks(ms)		(ms)
#define gfxMutexInit(pmutex)			*(pmutex) = CreateMutex(0, FALSE, 0)
#define gfxMutexDestroy(pmutex)			CloseHandle(*(pmutex))
#define gfxMutexEnter(pmutex) 			WaitForSingleObject(*(pmutex), INFINITE)
#define gfxMutexExit(pmutex)			ReleaseMutex(*(pmutex))
#define gfxSemInit(psem, val, limit)	*(psem) = CreateSemaphore(0, val, limit, 0)
#define gfxSemDestroy(psem)				CloseHandle(*(psem))
#define gfxSemWaitI(psem)				gfxSemWait((psem), TIME_IMMEDIATE)
#define gfxSemSignal(psem)				ReleaseSemaphore(*(psem), 1, 0)
#define gfxSemSignalI(psem)				ReleaseSemaphore(*(psem), 1, 0)
#define gfxSemCounterI(psem)			gfxSemCounter(psem)
#define gfxThreadMe()					GetCurrentThread()
#define gfxThreadClose(thread)			CloseHandle(thread)

/*===========================================================================*/
/* Function declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

void gfxHalt(const char *msg);
void gfxSleepMicroseconds(delaytime_t ms);
bool_t gfxSemWait(gfxSem *psem, delaytime_t ms);
semcount_t gfxSemCounter(gfxSem *pSem);
void gfxSystemLock(void);
void gfxSystemUnlock(void);
gfxThreadHandle gfxThreadCreate(void *stackarea, size_t stacksz, threadpriority_t prio, DECLARE_THREAD_FUNCTION((*fn),p), void *param);
threadreturn_t gfxThreadWait(gfxThreadHandle thread);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_OS_WIN32 */
#endif /* _GOS_WIN32_H */

