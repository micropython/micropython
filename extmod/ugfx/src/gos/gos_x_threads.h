/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * This threading implementation supports most 32 bit processors with or without an
 * 	underlying operating system. It uses cooperative multi-tasking. Be careful
 * 	when writing device drivers not to disturb the assumptions this creates by performing
 * 	call-backs from interrupt handlers to uGFX code unless you define the INTERRUPTS_OFF()
 * 	and INTERRUPTS_ON() macros.
 * 	It still requires some C runtime library support for the setjmp implementation...
 * 		setjmp() and longjmp()			- for threading
 * 		memcpy()						- for heap and threading
 *
 * 	You must also define the following routines in your own code so that timing functions will work...
 * 		systemticks_t gfxSystemTicks(void);
 *		systemticks_t gfxMillisecondsToTicks(delaytime_t ms);
 */
#ifndef _GOS_X_THREADS_H
#define _GOS_X_THREADS_H

#if GOS_NEED_X_THREADS

typedef uint32_t		delaytime_t;
typedef uint32_t		systemticks_t;
typedef short			semcount_t;
typedef int				threadreturn_t;
typedef int				threadpriority_t;

#define DECLARE_THREAD_FUNCTION(fnName, param)	threadreturn_t fnName(void *param)
#define DECLARE_THREAD_STACK(name, sz)			uint8_t name[sz];
#define THREAD_RETURN(retval)					return retval

#define TIME_IMMEDIATE				0
#define TIME_INFINITE				((delaytime_t)-1)
#define MAX_SEMAPHORE_COUNT			0x7FFF
#define LOW_PRIORITY				0
#define NORMAL_PRIORITY				1
#define HIGH_PRIORITY				2

typedef struct {
	semcount_t		cnt;
	semcount_t		limit;
} gfxSem;

typedef uint32_t		gfxMutex;
typedef void *			gfxThreadHandle;

#ifdef __cplusplus
extern "C" {
#endif

	// Required timing functions - supplied by the user or the operating system
	systemticks_t gfxSystemTicks(void);
	systemticks_t gfxMillisecondsToTicks(delaytime_t ms);

	// Sleep Functions
	void gfxSleepMilliseconds(delaytime_t ms);
	void gfxSleepMicroseconds(delaytime_t ms);
	void gfxYield(void);

	// System Locking
	void gfxSystemLock(void);
	void gfxSystemUnlock(void);

	// Mutexes
	void gfxMutexInit(gfxMutex *pmutex);
	#define gfxMutexDestroy(pmutex)
	void gfxMutexEnter(gfxMutex *pmutex);
	void gfxMutexExit(gfxMutex *pmutex);

	// Semaphores
	void gfxSemInit(gfxSem *psem, semcount_t val, semcount_t limit);
	#define gfxSemDestroy(psem)
	bool_t gfxSemWait(gfxSem *psem, delaytime_t ms);
	bool_t gfxSemWaitI(gfxSem *psem);
	void gfxSemSignal(gfxSem *psem);
	void gfxSemSignalI(gfxSem *psem);

	// Deprecated Semaphore functions (they still work here)
	#define gfxSemCounter(psem)			((psem)->cnt)
	#define gfxSemCounterI(psem)		((psem)->cnt)

	// Threads
	gfxThreadHandle gfxThreadCreate(void *stackarea, size_t stacksz, threadpriority_t prio, DECLARE_THREAD_FUNCTION((*fn),p), void *param);
	#define gfxThreadClose(thread)
	threadreturn_t gfxThreadWait(gfxThreadHandle thread);
	gfxThreadHandle gfxThreadMe(void);

	/** The following is not part of the public ugfx API has some operating systems
	 * 	simply do not provide this capability.
	 * 	For RAW32 we need it anyway so we might as well declare it here.
	 */
	void gfxThreadExit(threadreturn_t ret);

#ifdef __cplusplus
}
#endif

#endif /* GOS_NEED_X_THREADS */
#endif /* _GOS_X_THREADS_H */
