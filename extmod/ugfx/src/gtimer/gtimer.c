/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_GTIMER || defined(__DOXYGEN__)

#define GTIMER_FLG_PERIODIC		0x0001
#define GTIMER_FLG_INFINITE		0x0002
#define GTIMER_FLG_JABBED		0x0004
#define GTIMER_FLG_SCHEDULED	0x0008

/* Don't rework this macro to use a ternary operator - the gcc compiler stuffs it up */
#define TimeIsWithin(x, start, end)	((end >= start && x >= start && x <= end) || (end < start && (x >= start || x <= end)))

/* This mutex protects access to our tables */
static gfxMutex			mutex;
static gfxThreadHandle	hThread = 0;
static GTimer			*pTimerHead = 0;
static gfxSem			waitsem;
static systemticks_t	ticks2ms;
static DECLARE_THREAD_STACK(waTimerThread, GTIMER_THREAD_WORKAREA_SIZE);

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

static DECLARE_THREAD_FUNCTION(GTimerThreadHandler, arg) {
	GTimer			*pt;
	systemticks_t	tm;
	systemticks_t	nxtTimeout;
	systemticks_t	lastTime;
	GTimerFunction	fn;
	void			*param;
	(void)			arg;

	nxtTimeout = TIME_INFINITE;
	lastTime = 0;
	while(1) {
		/* Wait for work to do. */
		gfxYield();					// Give someone else a go no matter how busy we are
		gfxSemWait(&waitsem, nxtTimeout);
		
	restartTimerChecks:
	
		// Our reference time
		tm = gfxSystemTicks();
		nxtTimeout = TIME_INFINITE;
		
		/* We need to obtain the mutex */
		gfxMutexEnter(&mutex);

		if (pTimerHead) {
			pt = pTimerHead;
			do {
				// Do we have something to do for this timer?
				if ((pt->flags & GTIMER_FLG_JABBED) || (!(pt->flags & GTIMER_FLG_INFINITE) && TimeIsWithin(pt->when, lastTime, tm))) {
				
					// Is this timer periodic?
					if ((pt->flags & GTIMER_FLG_PERIODIC) && pt->period != TIME_IMMEDIATE) {
						// Yes - Update ready for the next period
						if (!(pt->flags & GTIMER_FLG_INFINITE)) {
							// We may have skipped a period.
							// We use this complicated formulae rather than a loop
							//	because the gcc compiler stuffs up the loop so that it
							//	either loops forever or doesn't get executed at all.
							pt->when += ((tm + pt->period - pt->when) / pt->period) * pt->period;
						}

						// We are definitely no longer jabbed
						pt->flags &= ~GTIMER_FLG_JABBED;
						
					} else {
						// No - get us off the timers list
						if (pt->next == pt)
							pTimerHead = 0;
						else {
							pt->next->prev = pt->prev;
							pt->prev->next = pt->next;
							if (pTimerHead == pt)
								pTimerHead = pt->next;
						}
						pt->flags = 0;
					}
					
					// Call the callback function
					fn = pt->fn;
					param = pt->param;
					gfxMutexExit(&mutex);
					fn(param);
					
					// We no longer hold the mutex, the callback function may have taken a while
					// and our list may have been altered so start again!
					goto restartTimerChecks;
				}
				
				// Find when we next need to wake up
				if (!(pt->flags & GTIMER_FLG_INFINITE) && pt->when - tm < nxtTimeout)
					nxtTimeout = (pt->when - tm)/ticks2ms;
				pt = pt->next;
			} while(pt != pTimerHead);
		}

		// Ready for the next loop
		lastTime = tm;
		gfxMutexExit(&mutex);
	}
	THREAD_RETURN(0);
}

void _gtimerInit(void)
{
	gfxSemInit(&waitsem, 0, 1);
	gfxMutexInit(&mutex);
	ticks2ms = gfxMillisecondsToTicks(1);
}

void _gtimerDeinit(void)
{
	gfxSemDestroy(&waitsem);
	gfxMutexDestroy(&mutex);
	// Need to destroy GTimer thread here
}

void gtimerInit(GTimer* pt)
{
	pt->flags = 0;
}

void gtimerDeinit(GTimer* pt)
{
	gtimerStop(pt);
}

void gtimerStart(GTimer *pt, GTimerFunction fn, void *param, bool_t periodic, delaytime_t millisec) {
	gfxMutexEnter(&mutex);
	
	// Start our thread if not already going
	if (!hThread) {
		hThread = gfxThreadCreate(waTimerThread, GTIMER_THREAD_WORKAREA_SIZE, GTIMER_THREAD_PRIORITY, GTimerThreadHandler, 0);
		if (hThread) {gfxThreadClose(hThread);}		// We never really need the handle again
	}

	// Is this already scheduled?
	if (pt->flags & GTIMER_FLG_SCHEDULED) {
		// Cancel it!
		if (pt->next == pt)
			pTimerHead = 0;
		else {
			pt->next->prev = pt->prev;
			pt->prev->next = pt->next;
			if (pTimerHead == pt)
				pTimerHead = pt->next;
		}
	}
	
	// Set up the timer structure
	pt->fn = fn;
	pt->param = param;
	pt->flags = GTIMER_FLG_SCHEDULED;
	if (periodic)
		pt->flags |= GTIMER_FLG_PERIODIC;
	if (millisec == TIME_INFINITE) {
		pt->flags |= GTIMER_FLG_INFINITE;
		pt->period = TIME_INFINITE;
	} else {
		pt->period = gfxMillisecondsToTicks(millisec);
		pt->when = gfxSystemTicks() + pt->period;
	}

	// Just pop it on the end of the queue
	if (pTimerHead) {
		pt->next = pTimerHead;
		pt->prev = pTimerHead->prev;
		pt->prev->next = pt;
		pt->next->prev = pt;
	} else
		pt->next = pt->prev = pTimerHead = pt;

	// Bump the thread
	if (!(pt->flags & GTIMER_FLG_INFINITE))
		gfxSemSignal(&waitsem);
	gfxMutexExit(&mutex);
}

void gtimerStop(GTimer *pt) {
	gfxMutexEnter(&mutex);
	if (pt->flags & GTIMER_FLG_SCHEDULED) {
		// Cancel it!
		if (pt->next == pt)
			pTimerHead = 0;
		else {
			pt->next->prev = pt->prev;
			pt->prev->next = pt->next;
			if (pTimerHead == pt)
				pTimerHead = pt->next;
		}
		// Make sure we know the structure is dead!
		pt->flags = 0;
	}
	gfxMutexExit(&mutex);
}

bool_t gtimerIsActive(GTimer *pt) {
	return (pt->flags & GTIMER_FLG_SCHEDULED) ? TRUE : FALSE;
}

void gtimerJab(GTimer *pt) {
	gfxMutexEnter(&mutex);
	
	// Jab it!
	pt->flags |= GTIMER_FLG_JABBED;

	// Bump the thread
	gfxSemSignal(&waitsem);
	gfxMutexExit(&mutex);
}

void gtimerJabI(GTimer *pt) {
	// Jab it!
	pt->flags |= GTIMER_FLG_JABBED;

	// Bump the thread
	gfxSemSignalI(&waitsem);
}

#endif /* GFX_USE_GTIMER */
