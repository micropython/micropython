/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_GADC

/* Include the driver defines */
#include "gadc_driver.h"

#if GADC_MAX_HIGH_SPEED_SAMPLERATE > GADC_MAX_SAMPLE_FREQUENCY/2
	#error "GADC: GADC_MAX_HIGH_SPEED_SAMPLERATE has been set too high. It must be less than half the maximum CPU rate"
#endif

#define GADC_HSADC_GTIMER		0x8000
#define GADC_ADC_RUNNING		0x4000
#define GADC_HSADC_CONVERTION	0x2000

typedef struct NonTimerData_t {
	gfxQueueGSyncItem		next;
	GADCCallbackFunction	callback;
	union {
		void				*param;
		gfxSem				sigdone;
	};
	GadcNonTimerJob			job;
	} NonTimerData;

static volatile uint16_t		hsFlags;
static size_t					hsBytesPerConv;
static GadcTimerJob				hsJob;
static GDataBuffer				*hsData;
static gfxQueueGSync			hsListDone;
static GADCISRCallbackFunction	hsISRcallback;
#if GFX_USE_GEVENT
	static GTimer				hsGTimer;
#endif

static GTimer					lsGTimer;
static gfxQueueGSync			lsListToDo;
static gfxQueueGSync			lsListDone;
static NonTimerData				*lsData;

void gadcGotDataI(size_t n) {
	if ((hsFlags & GADC_HSADC_CONVERTION)) {

		// A set of timer conversions is done - add them
		hsJob.done += n;

		// Are we finished yet? (or driver signalled complete now)
		if (n && hsJob.done < hsJob.todo)
			return;

		// Clear event flags we might set
		hsFlags &= ~(GADC_HSADC_GOTBUFFER|GADC_HSADC_STALL);

		// Is there any data in it
		if (!hsJob.done) {

			// Oops - no data in this buffer. Just return it to the free-list
			gfxBufferReleaseI(hsData);
			goto starttimerjob;					// Restart the timer job
		}

		// Save the buffer on the hsListDone list
		hsData->len = hsJob.done * hsBytesPerConv;
		gfxQueueGSyncPutI(&hsListDone, (gfxQueueGSyncItem *)hsData);
		hsFlags |= GADC_HSADC_GOTBUFFER;

		/* Signal a buffer completion */
		if (hsISRcallback)
			hsISRcallback();
		#if GFX_USE_GEVENT
			if (hsFlags & GADC_HSADC_GTIMER)
				gtimerJabI(&hsGTimer);
		#endif

		// Stop if we have been told to
		if (!(hsFlags & GADC_HSADC_RUNNING)) {
			gadc_lld_stop_timerI();

		// Get the next free buffer
		} else if (!(hsData = gfxBufferGetI())) {

			// Oops - no free buffers. Stall
			hsFlags &= ~GADC_HSADC_RUNNING;
			hsFlags |= GADC_HSADC_STALL;
			gadc_lld_stop_timerI();

		// Prepare the next job
		} else {

			// Return this new job
			#if GFX_USE_OS_CHIBIOS
				// ChibiOS api bug - samples must be even
				hsJob.todo = (hsData->size / hsBytesPerConv) & ~1;
			#else
				hsJob.todo = hsData->size / hsBytesPerConv;
			#endif
			hsJob.done = 0;
			hsJob.buffer = (adcsample_t *)(hsData+1);
		}

		// Start a job preferring a non-timer job
		if ((lsData = (NonTimerData *)gfxQueueGSyncGetI(&lsListToDo))) {
			hsFlags &= ~GADC_HSADC_CONVERTION;
			gadc_lld_nontimerjobI(&lsData->job);
		} else if ((hsFlags & GADC_HSADC_RUNNING)) {
			hsFlags |= GADC_HSADC_CONVERTION;
			gadc_lld_timerjobI(&hsJob);
		} else
			hsFlags &= ~GADC_ADC_RUNNING;

	} else {

		// Did it fail
		if (!n) {
			// Push it back on the head of the queue - it didn't actually get done
			gfxQueueGSyncPushI(&lsListToDo, (gfxQueueGSyncItem *)lsData);
			lsData = 0;
			goto starttimerjob;
		}

		// A non-timer job completed - signal
		if (lsData->callback) {
			// Put it on the completed list and signal the timer to do the call-backs
			gfxQueueGSyncPutI(&lsListDone, (gfxQueueGSyncItem *)lsData);
			gtimerJabI(&lsGTimer);
		} else {
			// Signal the thread directly
			gfxSemSignalI(&lsData->sigdone);
		}
		lsData = 0;

		// Start a job preferring a timer job
starttimerjob:
		if ((hsFlags & GADC_HSADC_RUNNING)) {
			hsFlags |= GADC_HSADC_CONVERTION;
			gadc_lld_timerjobI(&hsJob);
		} else if ((lsData = (NonTimerData *)gfxQueueGSyncGetI(&lsListToDo))) {
			hsFlags &= ~GADC_HSADC_CONVERTION;
			gadc_lld_nontimerjobI(&lsData->job);
		} else
			hsFlags &= ~GADC_ADC_RUNNING;
	}
}

/* Our module initialiser */
void _gadcInit(void)
{
	gadc_lld_init();

	gfxQueueGSyncInit(&hsListDone);
	#if GFX_USE_GEVENT
		gtimerInit(&hsGTimer);
	#endif
	gtimerInit(&lsGTimer);
	gfxQueueGSyncInit(&lsListToDo);
	gfxQueueGSyncInit(&lsListDone);
}

void _gadcDeinit(void)
{
	/* commented stuff is ToDo */

	// gadc_lld_deinit();
	gfxQueueGSyncDeinit(&hsListDone);
	#if GFX_USE_GEVENT
		gtimerDeinit(&hsGTimer);
	#endif	
	gtimerDeinit(&lsGTimer);
	gfxQueueGSyncDeinit(&lsListToDo);
	gfxQueueGSyncDeinit(&lsListDone);
}

#if GFX_USE_GEVENT
	static void HighSpeedGTimerCallback(void *param) {
		(void) param;
		GSourceListener	*psl;
		GEventADC		*pe;

		psl = 0;
		while ((psl = geventGetSourceListener((GSourceHandle)(&hsGTimer), psl))) {
			if (!(pe = (GEventADC *)geventGetEventBuffer(psl))) {
				// This listener is missing - save this.
				psl->srcflags |= GADC_HSADC_LOSTEVENT;
				continue;
			}

			pe->type = GEVENT_ADC;
			pe->flags = (hsFlags & (GADC_HSADC_RUNNING|GADC_HSADC_GOTBUFFER|GADC_HSADC_STALL)) | psl->srcflags;
			psl->srcflags = 0;
			geventSendEvent(psl);
		}
	}
#endif

void gadcHighSpeedInit(uint32_t physdev, uint32_t frequency)
{
	if ((hsFlags & GADC_HSADC_RUNNING))
		gadcHighSpeedStop();

	/* Just save the details and reset everything for now */
	hsJob.physdev = physdev;
	hsJob.frequency = frequency;
	hsISRcallback = 0;
	hsBytesPerConv = gadc_lld_samplesperconversion(physdev) * sizeof(adcsample_t);
}

#if GFX_USE_GEVENT
	GSourceHandle gadcHighSpeedGetSource(void) {
		if (!gtimerIsActive(&hsGTimer))
			gtimerStart(&hsGTimer, HighSpeedGTimerCallback, 0, TRUE, TIME_INFINITE);
		hsFlags |= GADC_HSADC_GTIMER;
		return (GSourceHandle)&hsGTimer;
	}
#endif

void gadcHighSpeedSetISRCallback(GADCISRCallbackFunction isrfn) {
	hsISRcallback = isrfn;
}

GDataBuffer *gadcHighSpeedGetData(delaytime_t ms) {
	return (GDataBuffer *)gfxQueueGSyncGet(&hsListDone, ms);
}

GDataBuffer *gadcHighSpeedGetDataI(void) {
	return (GDataBuffer *)gfxQueueGSyncGetI(&hsListDone);
}

void gadcHighSpeedStart(void) {
	// Safety first
	if (!hsJob.frequency || !hsBytesPerConv)
		return;

	gfxSystemLock();
	if (!(hsFlags & GADC_HSADC_RUNNING)) {
		if (!(hsData = gfxBufferGetI())) {
			// Oops - no free buffers. Stall
			hsFlags |= GADC_HSADC_STALL;
			#if GFX_USE_GEVENT
				if (hsFlags & GADC_HSADC_GTIMER)
					gtimerJabI(&hsGTimer);
			#endif

		// Prepare the next job
		} else {

			#if GFX_USE_OS_CHIBIOS
				// ChibiOS api bug - samples must be even
				hsJob.todo = (hsData->size / hsBytesPerConv) & ~1;
			#else
				hsJob.todo = hsData->size / hsBytesPerConv;
			#endif
			hsJob.done = 0;
			hsJob.buffer = (adcsample_t *)(hsData+1);
			hsFlags |= GADC_HSADC_RUNNING;

			// Start the timer
			gadc_lld_start_timerI(hsJob.frequency);

			// If nothing is running start the job
			if (!(hsFlags & GADC_ADC_RUNNING)) {
				hsFlags |= (GADC_HSADC_CONVERTION|GADC_ADC_RUNNING);
				gadc_lld_timerjobI(&hsJob);
			}
		}
	}
	gfxSystemUnlock();
}

void gadcHighSpeedStop(void) {
	// Stop it and wait for completion
	hsFlags &= ~GADC_HSADC_RUNNING;
	while ((hsFlags & GADC_HSADC_CONVERTION))
		gfxYield();
}

static void LowSpeedGTimerCallback(void *param) {
	(void) param;
	NonTimerData		*pdata;

	// Look for completed non-timer jobs and call the call-backs for each
	while ((pdata = (NonTimerData *)gfxQueueGSyncGet(&lsListDone, TIME_IMMEDIATE))) {
		pdata->callback(pdata->job.buffer, pdata->param);
		gfxFree(pdata);
	}
}

void gadcLowSpeedGet(uint32_t physdev, adcsample_t *buffer) {
	NonTimerData ndata;

	// Prepare the job
	gfxSemInit(&ndata.sigdone, 0, 1);
	ndata.job.physdev = physdev;
	ndata.job.buffer = buffer;
	ndata.callback = 0;

	// Activate it
	gfxSystemLock();
	if (!(hsFlags & GADC_ADC_RUNNING)) {
		// Nothing is running - start the job
		lsData = &ndata;
		hsFlags |= GADC_ADC_RUNNING;
		hsFlags &= ~GADC_HSADC_CONVERTION;
		gadc_lld_nontimerjobI(&ndata.job);
	} else {
		// Just put it on the queue
		gfxQueueGSyncPutI(&lsListToDo, (gfxQueueGSyncItem *)&ndata);
	}
	gfxSystemUnlock();

	// Wait for it to complete
	gfxSemWait(&ndata.sigdone, TIME_INFINITE);
	gfxSemDestroy(&ndata.sigdone);
}

bool_t gadcLowSpeedStart(uint32_t physdev, adcsample_t *buffer, GADCCallbackFunction fn, void *param) {
	NonTimerData *pdata;

	/* Start the Low Speed Timer */
	if (!gtimerIsActive(&lsGTimer))
		gtimerStart(&lsGTimer, LowSpeedGTimerCallback, 0, TRUE, TIME_INFINITE);

	// Prepare the job
	if (!(pdata = gfxAlloc(sizeof(NonTimerData))))
		return FALSE;
	pdata->job.physdev = physdev;
	pdata->job.buffer = buffer;
	pdata->callback = fn;
	pdata->param = param;

	// Activate it
	gfxSystemLock();
	if (!(hsFlags & GADC_ADC_RUNNING)) {
		// Nothing is running - start the job
		lsData = pdata;
		hsFlags |= GADC_ADC_RUNNING;
		hsFlags &= ~GADC_HSADC_CONVERTION;
		gadc_lld_nontimerjobI(&pdata->job);
	} else {
		// Just put it on the queue
		gfxQueueGSyncPutI(&lsListToDo, (gfxQueueGSyncItem *)pdata);
	}
	gfxSystemUnlock();
	return TRUE;
}

#endif /* GFX_USE_GADC */
