/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_GEVENT || defined(__DOXYGEN__)

#if GEVENT_ASSERT_NO_RESOURCE
	#define GEVENT_ASSERT(x)		assert(x)
#else
	#define GEVENT_ASSERT(x)
#endif

/* Flags in the listener structure */
#define GLISTENER_EVENTBUSY			0x0001			// The event buffer is busy
#define GLISTENER_WAITING			0x0002			// The listener is waiting for a signal
#define GLISTENER_WITHSOURCE		0x0004			// The listener is being looked at by a source for a possible event
#define GLISTENER_PENDING			0x0008			// There is an event waiting ready to go without a current listener

/* This mutex protects access to our tables */
static gfxMutex	geventMutex;

/* Our table of listener/source pairs */
static GSourceListener		Assignments[GEVENT_MAX_SOURCE_LISTENERS];

/* Send an exit event if possible. */
/* We already have the geventMutex */
static void doExitEvent(GListener *pl) {
	// Don't do the exit if someone else currently has the event lock
	if ((pl->flags & (GLISTENER_WAITING|GLISTENER_EVENTBUSY)) == GLISTENER_WAITING) {
		pl->flags |= GLISTENER_EVENTBUSY;							// Event buffer is in use
		pl->event.type = GEVENT_EXIT;								// Set up the EXIT event
		pl->flags &= ~GLISTENER_WAITING;							// Wake up the listener (with data)
		gfxSemSignal(&pl->waitqueue);
	}
}

/* Loop through the assignment table deleting this listener/source pair. */
/*	Null is treated as a wildcard. */
/* We already have the geventMutex */
static void deleteAssignments(GListener *pl, GSourceHandle gsh) {
	GSourceListener *psl;

	for(psl = Assignments; psl < Assignments+GEVENT_MAX_SOURCE_LISTENERS; psl++) {
		if ((!pl || psl->pListener == pl) && (!gsh || psl->pSource == gsh)) {
			doExitEvent(psl->pListener);
			psl->pListener = 0;
			psl->pSource = 0;
		}
	}
}

void _geventInit(void)
{
	gfxMutexInit(&geventMutex);
}

void _geventDeinit(void)
{
	gfxMutexDestroy(&geventMutex);	
}

void geventListenerInit(GListener *pl) {
	gfxSemInit(&pl->waitqueue, 0, MAX_SEMAPHORE_COUNT);		// Next wait'er will block
	pl->callback = 0;										// No callback active
	pl->event.type = GEVENT_NULL;							// Always safety
	pl->flags = 0;
}

bool_t geventAttachSource(GListener *pl, GSourceHandle gsh, uint32_t flags) {
	GSourceListener *psl, *pslfree;

	// Safety first
	if (!pl || !gsh) {
		GEVENT_ASSERT(FALSE);
		return FALSE;
	}

	gfxMutexEnter(&geventMutex);

	// Check if this pair is already in the table (scan for a free slot at the same time)
	pslfree = 0;
	for(psl = Assignments; psl < Assignments+GEVENT_MAX_SOURCE_LISTENERS; psl++) {
		
		if (pl == psl->pListener && gsh == psl->pSource) {
			// Just update the flags
			psl->listenflags = flags;
			gfxMutexExit(&geventMutex);
			return TRUE;
		}
		if (!pslfree && !psl->pListener)
			pslfree = psl;
	}
	
	// A free slot was found - allocate it
	if (pslfree) {
		pslfree->pListener = pl;
		pslfree->pSource = gsh;
		pslfree->listenflags = flags;
		pslfree->srcflags = 0;
	}
	gfxMutexExit(&geventMutex);
	GEVENT_ASSERT(pslfree != 0);
	return pslfree != 0;
}

void geventDetachSource(GListener *pl, GSourceHandle gsh) {
	if (pl) {
		gfxMutexEnter(&geventMutex);
		deleteAssignments(pl, gsh);
		if (!gsh)
			doExitEvent(pl);
		gfxMutexExit(&geventMutex);
	}
}

GEvent *geventEventWait(GListener *pl, delaytime_t timeout) {
	gfxMutexEnter(&geventMutex);
	// Don't allow waiting if we are on callbacks or if there is already a thread waiting
	if (pl->callback || (pl->flags & GLISTENER_WAITING)) {
		gfxMutexExit(&geventMutex);
		return 0;
	}

	// Check to see if there is a pending event ready for us
	if ((pl->flags & GLISTENER_PENDING)) {
		pl->flags &= ~GLISTENER_PENDING;				// We have now got this
		pl->flags |= GLISTENER_EVENTBUSY;				// Event buffer is definitely busy
		gfxMutexExit(&geventMutex);
		return &pl->event;
	}

	// No - wait for one.
	pl->flags &= ~GLISTENER_EVENTBUSY;				// Event buffer is definitely not busy
	pl->flags |= GLISTENER_WAITING;					// We will now be waiting on the thread
	gfxMutexExit(&geventMutex);
	if (gfxSemWait(&pl->waitqueue, timeout))
		return &pl->event;

	// Timeout - clear the waiting flag.
	// We know this is safe as any other thread will still think there is someone waiting.
	gfxMutexEnter(&geventMutex);
	pl->flags &= ~GLISTENER_WAITING;
	gfxMutexExit(&geventMutex);
	return 0;
}

void geventEventComplete(GListener *pl) {
	pl->flags &= ~GLISTENER_EVENTBUSY;
}

void geventRegisterCallback(GListener *pl, GEventCallbackFn fn, void *param) {
	if (pl) {
		gfxMutexEnter(&geventMutex);
		doExitEvent(pl);
		pl->param = param;						// Set the param
		pl->callback = fn;						// Set the callback function
		if (fn)
			pl->flags &= ~GLISTENER_EVENTBUSY;	// The event buffer is immediately available
		gfxMutexExit(&geventMutex);
	}
}

GSourceListener *geventGetSourceListener(GSourceHandle gsh, GSourceListener *lastlr) {
	GSourceListener *psl;

	// Safety first
	if (!gsh)
		return 0;

	gfxMutexEnter(&geventMutex);

	// Unlock the last listener event buffer if it wasn't used.
	if (lastlr && lastlr->pListener && (lastlr->pListener->flags & GLISTENER_WITHSOURCE))
		lastlr->pListener->flags &= ~(GLISTENER_WITHSOURCE|GLISTENER_EVENTBUSY);
		
	// Loop through the table looking for attachments to this source
	for(psl = lastlr ? (lastlr+1) : Assignments; psl < Assignments+GEVENT_MAX_SOURCE_LISTENERS; psl++) {
		if (gsh == psl->pSource) {
			gfxMutexExit(&geventMutex);
			return psl;
		}
	}
	gfxMutexExit(&geventMutex);
	return 0;
}

GEvent *geventGetEventBuffer(GSourceListener *psl) {
	gfxMutexEnter(&geventMutex);
	if ((psl->pListener->flags & GLISTENER_EVENTBUSY)) {
		// Oops - event buffer is still in use
		gfxMutexExit(&geventMutex);
		return 0;
	}

	// Allocate the event buffer
	psl->pListener->flags |= (GLISTENER_WITHSOURCE|GLISTENER_EVENTBUSY);
	gfxMutexExit(&geventMutex);
	return &psl->pListener->event;
}

void geventSendEvent(GSourceListener *psl) {
	gfxMutexEnter(&geventMutex);
	if (psl->pListener->callback) {

		// Mark it back as free and as sent. This is early to be marking as free but it protects
		//	if the callback alters the listener in any way
		psl->pListener->flags &= ~(GLISTENER_WITHSOURCE|GLISTENER_EVENTBUSY|GLISTENER_PENDING);
		gfxMutexExit(&geventMutex);

		// Do the callback
		psl->pListener->callback(psl->pListener->param, &psl->pListener->event);

	} else {
		// Wake up the listener
		psl->pListener->flags &= ~GLISTENER_WITHSOURCE;
		if ((psl->pListener->flags & GLISTENER_WAITING)) {
			psl->pListener->flags &= ~(GLISTENER_WAITING|GLISTENER_PENDING);
			gfxSemSignal(&psl->pListener->waitqueue);
		} else
			psl->pListener->flags |= GLISTENER_PENDING;

		// The listener thread will free the event buffer when ready
		gfxMutexExit(&geventMutex);
	}
}

void geventDetachSourceListeners(GSourceHandle gsh) {
	gfxMutexEnter(&geventMutex);
	deleteAssignments(0, gsh);
	gfxMutexExit(&geventMutex);
}

#endif /* GFX_USE_GEVENT */
