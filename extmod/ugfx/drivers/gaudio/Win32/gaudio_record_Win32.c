/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

// We need to include stdio.h below. Turn off GFILE_NEED_STDIO just for this file to prevent conflicts
#define GFILE_NEED_STDIO_MUST_BE_OFF

#include "gfx.h"

#if GFX_USE_GAUDIO && GAUDIO_NEED_RECORD

/* Include the driver defines */
#include "../../../src/gaudio/gaudio_driver_record.h"

#undef Red
#undef Green
#undef Blue
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>

#define MAX_WAVE_HEADERS		2				// Larger numbers enable more buffering which is good for ensuring
												// there are no skips due to data not being available, however larger
												// numbers of buffers chews buffers on the free-list.

static HWAVEIN		ah = 0;
static volatile int	nQueuedBuffers;
static bool_t		isRunning;
static WAVEHDR		WaveHdrs[MAX_WAVE_HEADERS];
static HANDLE		waveThread;
static DWORD		threadID;

/**************************** waveProc() *******************************
 * We don't use CALLBACK_FUNCTION because it is restricted to calling only
 * a few particular Windows functions, namely some of the time functions,
 * and a few of the Low Level MIDI API. If you violate this rule, your app can
 * hang inside of the callback). One of the Windows API that a callback can't
 * call is waveInAddBuffer() which is what we need to use whenever we receive a
 * MM_WIM_DATA. My callback would need to defer that job to another thread
 * anyway, so instead just use CALLBACK_THREAD here instead.
 *************************************************************************/

static bool_t getbuffer(WAVEHDR *pwh) {
	GDataBuffer *paud;

	// Get the next data block to send
	gfxSystemLock();
	paud = gaudioRecordGetFreeBlockI();
	if (!paud && !nQueuedBuffers)
		gaudioRecordDoneI();
	gfxSystemUnlock();
	if (!paud)
		return FALSE;

	// Prepare the wave header for Windows
	pwh->dwUser = (DWORD_PTR)paud;
	pwh->lpData = (LPSTR)(paud+1);			// The data is on the end of the structure
	pwh->dwBufferLength = paud->size;
	pwh->dwFlags = 0;
	if (waveInPrepareHeader(ah, pwh, sizeof(WAVEHDR))) {
		fprintf(stderr, "GAUDIO: Failed to prepare a record buffer");
		exit(-1);
	}

	// Send it to windows
	if (waveInAddBuffer(ah, pwh, sizeof(WAVEHDR))) {
		fprintf(stderr, "GAUDIO: Failed to add the record buffer");
		exit(-1);
	}

	nQueuedBuffers++;
	return TRUE;
}

static DWORD WINAPI waveProc(LPVOID arg) {
	MSG			msg;
	WAVEHDR		*pwh;
	GDataBuffer	*paud;
	(void)		arg;

	while (GetMessage(&msg, 0, 0, 0)) {
		switch (msg.message) {
			case MM_WIM_DATA:
				pwh = (WAVEHDR *)msg.lParam;

				// Windows - Let go!
				waveInUnprepareHeader(ah, pwh, sizeof(WAVEHDR));

				// Save the buffer in the audio record list
				paud = (GDataBuffer *)pwh->dwUser;
				paud->len = pwh->dwBytesRecorded;
				gfxSystemLock();
				gaudioRecordSaveDataBlockI(paud);
				gfxSystemUnlock();
				pwh->lpData = 0;
				nQueuedBuffers--;

				// Are we stopping?
				if (!isRunning) {
					// Have we finished yet?
					if (!nQueuedBuffers) {
						gfxSystemLock();
						gaudioRecordDoneI();
						gfxSystemUnlock();
					}
	                break;
				}

				// Try and get a new block
				getbuffer(pwh);
                break;
		}
	}
	return 0;
}

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

bool_t gaudio_record_lld_init(uint16_t channel, uint32_t frequency, ArrayDataFormat format) {
	WAVEFORMATEX	wfx;

	if (format != ARRAY_DATA_8BITUNSIGNED && format != ARRAY_DATA_16BITSIGNED)
		return FALSE;

	if (!waveThread) {
		if (!(waveThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)waveProc, 0, 0, &threadID))) {
			fprintf(stderr, "GAUDIO: Can't create WAVE recording thread\n");
			exit(-1);
		}
		CloseHandle(waveThread);
	}

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = channel == GAUDIO_RECORD_STEREO ? 2 : 1;
	wfx.nSamplesPerSec = frequency;
	wfx.nBlockAlign = wfx.nChannels * (format == ARRAY_DATA_8BITUNSIGNED ? 1 : 2);
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.wBitsPerSample = (format == ARRAY_DATA_8BITUNSIGNED ? 8 : 16);
	wfx.cbSize = 0;

	if (ah) {
		waveInClose(ah);
		ah = 0;
	}
	if (waveInOpen(&ah, WAVE_MAPPER, &wfx, (DWORD_PTR)threadID, 0, CALLBACK_THREAD)) {
		fprintf(stderr, "GAUDIN: Can't open WAVE recording device\n");
		exit(-1);
	}

	return TRUE;
}

void gaudio_record_lld_start(void) {
	WAVEHDR		*pwh;

	if (!ah)
		return;

	while (nQueuedBuffers < MAX_WAVE_HEADERS) {
		// Find the empty one - there will always be at least one.
		for(pwh = WaveHdrs; pwh->lpData; pwh++);

		// Grab the next audio block from the free-list
		if (!getbuffer(pwh))
			break;
	}
	if (!isRunning) {
		isRunning = TRUE;
		waveInStart(ah);
	}
}

void gaudio_record_lld_stop(void) {
	isRunning = FALSE;
	waveInReset(ah);
	while(nQueuedBuffers) Sleep(1);
}

#endif /* GFX_USE_GAUDIO && GAUDIO_NEED_RECORD */
