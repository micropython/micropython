/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

// We need to include stdio.h below. Turn off GFILE_NEED_STDIO just for this file to prevent conflicts
#define GFILE_NEED_STDIO_MUST_BE_OFF

#include "gfx.h"

#if GFX_USE_GAUDIO && GAUDIO_NEED_PLAY

/* Include the driver defines */
#include "../../../src/gaudio/gaudio_driver_play.h"

#undef Red
#undef Green
#undef Blue
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>

#define MAX_WAVE_HEADERS		2				// Larger numbers enable more buffering which is good for ensuring
												// there are no skips due to data not being available, however larger
												// numbers of buffers also create higher latency.

static HWAVEOUT		ah = 0;
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
 * call is waveOutUnPrepareBuffer() which is what we need to use whenever we receive a
 * MM_WOM_DONE. My callback would need to defer that job to another thread
 * anyway, so instead just use CALLBACK_THREAD here instead.
 *************************************************************************/

static bool_t senddata(WAVEHDR *pwh) {
	GDataBuffer *paud;

	// Get the next data block to send
	gfxSystemLock();
	paud = gaudioPlayGetDataBlockI();
	if (!paud && !nQueuedBuffers)
		gaudioPlayDoneI();
	gfxSystemUnlock();
	if (!paud)
		return FALSE;

	// Prepare the wave header for Windows
	pwh->dwUser = (DWORD_PTR)paud;
	pwh->lpData = (LPSTR)(paud+1);			// The data is on the end of the structure
	pwh->dwBufferLength = paud->len;
	pwh->dwFlags = 0;
	pwh->dwLoops = 0;
	if (waveOutPrepareHeader(ah, pwh, sizeof(WAVEHDR))) {
		fprintf(stderr, "GAUDIO: Failed to prepare a play buffer");
		exit(-1);
	}

	// Send it to windows
	if (waveOutWrite(ah, pwh, sizeof(WAVEHDR))) {
		fprintf(stderr, "GAUDIO: Failed to write the play buffer");
		exit(-1);
	}

	nQueuedBuffers++;
	return TRUE;
}

static DWORD WINAPI waveProc(LPVOID arg) {
	MSG			msg;
	WAVEHDR		*pwh;
	(void)		arg;

	while (GetMessage(&msg, 0, 0, 0)) {
		switch (msg.message) {
			case MM_WOM_DONE:
				pwh = (WAVEHDR *)msg.lParam;

				// Windows - Let go!
				waveOutUnprepareHeader(ah, pwh, sizeof(WAVEHDR));

				// Give the buffer back to the Audio Free List
				gfxSystemLock();
				gaudioPlayReleaseDataBlockI((GDataBuffer *)pwh->dwUser);
				gfxSystemUnlock();
				pwh->lpData = 0;
				nQueuedBuffers--;

				// Are we stopping?
				if (!isRunning) {
					// Have we finished yet?
					if (!nQueuedBuffers) {
						gfxSystemLock();
						gaudioPlayDoneI();
						gfxSystemUnlock();
					}
	                break;
				}

				// Try and get a new block
				senddata(pwh);
				break;
		}
	}
	return 0;
}

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

bool_t gaudio_play_lld_init(uint16_t channel, uint32_t frequency, ArrayDataFormat format) {
	WAVEFORMATEX	wfx;

	if (format != ARRAY_DATA_8BITUNSIGNED && format != ARRAY_DATA_16BITSIGNED)
		return FALSE;

	if (!waveThread) {
		if (!(waveThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)waveProc, 0, 0, &threadID))) {
			fprintf(stderr, "GAUDIO: Can't create WAVE play-back thread\n");
			exit(-1);
		}
		CloseHandle(waveThread);
	}

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = channel == GAUDIO_PLAY_STEREO ? 2 : 1;
	wfx.nSamplesPerSec = frequency;
	wfx.nBlockAlign = wfx.nChannels * (format == ARRAY_DATA_8BITUNSIGNED ? 1 : 2);
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.wBitsPerSample = (format == ARRAY_DATA_8BITUNSIGNED ? 8 : 16);
	wfx.cbSize = 0;

	if (ah) {
		waveOutClose(ah);
		ah = 0;
	}
	if (waveOutOpen(&ah, WAVE_MAPPER, &wfx, (DWORD_PTR)threadID, 0, CALLBACK_THREAD)) {
		fprintf(stderr, "GAUDIO: Can't open WAVE play-back device\n");
		exit(-1);
	}

	return TRUE;
}

bool_t gaudio_play_lld_set_volume(uint8_t vol) {
	if (!ah)
		return FALSE;
	return waveOutSetVolume(ah, (((uint16_t)vol)<<8)|vol) != 0;
}

void gaudio_play_lld_start(void) {
	WAVEHDR		*pwh;

	isRunning = TRUE;
	while (nQueuedBuffers < MAX_WAVE_HEADERS) {
		// Find the empty one - there will always be at least one.
		for(pwh = WaveHdrs; pwh->lpData; pwh++);

		// Grab the next audio block from the Audio Out Queue
		if (!senddata(pwh))
			break;
	}
}

void gaudio_play_lld_stop(void) {
	isRunning = FALSE;
	waveOutReset(ah);
	while(nQueuedBuffers) Sleep(1);
}

#endif /* GFX_USE_GAUDIO && GAUDIO_NEED_PLAY */
