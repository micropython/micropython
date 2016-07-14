/*
 * Copyright (c) 2012, 2013, Joel Bodenmann aka Tectu <joel@unormal.org>
 * Copyright (c) 2012, 2013, Andrew Hannam aka inmarket
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * This demo demonstrates the use of the GAUDOUT module to play a audio file of arbitrary format
 * eg. WAV, MP3. It is designed to work only with intelligent codecs like the VS1053 that can interpret
 * the audio data themselves.
 *
 */
#include "gfx.h"

/* Specify our timing parameters */
#define MY_AUDIO_CHANNEL				0								/* Use channel 0 */
#define MY_PLAY_FILE					"allwrong.wav"

/*
 * Application entry point.
 */
int main(void) {
	font_t			font;
	GFILE			*f;
	char 			*errmsg;
	uint32_t		toplay;
	uint32_t		len;
	GDataBuffer		*pd;

	// Initialise everything
	gfxInit();
	errmsg = 0;

	// Any font will do
	font = gdispOpenFont("*");

	// Allocate audio buffers - 4 x 512 byte buffers.
	//	You may need to increase this for slower cpu's.
	//	You may be able to decrease this for low latency operating systems.
	if (!gfxBufferAlloc(4, 512)) {
		errmsg = "Err: No Memory";
		goto theend;
	}

repeatplay:
	// Open the wave file
	if (!(f = gfileOpen(MY_PLAY_FILE, "r"))) {
		errmsg = "Err: Open WAV";
		goto theend;
	}

	// Initialise the audio output device - bitrate is ignored
	if (!gaudioPlayInit(MY_AUDIO_CHANNEL, 22000, GAUDIO_PLAY_FORMAT_FILE)) {
		errmsg = "Err: Bad format/freq";
		goto theend;
	}

	// Play the file
	gdispDrawString(0, gdispGetHeight()/2, "Playing...", font, Yellow);
	toplay = gfileGetSize(f);
	while(toplay) {
		// Get a buffer to put the data into
		pd = gfxBufferGet(TIME_INFINITE);		// This should never fail as we are waiting forever

		// How much data can we put in
		len = toplay > pd->size ? pd->size : toplay;
		pd->len = len;
		toplay -= len;

		// Read the data
		if (gfileRead(f, pd+1, len) != len) {
			errmsg = "Err: Read fail";
			goto theend;
		}

		gaudioPlay(pd);
	}
	gfileClose(f);

	// Wait for the play to finish
	gaudioPlayWait(TIME_INFINITE);
	gdispDrawString(0, gdispGetHeight()/2+10, "Done", font, Green);

	// Repeat the whole thing
	gfxSleepMilliseconds(1500);
	gdispClear(Black);
	goto repeatplay;

	// The end
theend:
	if (errmsg)
		gdispDrawString(0, gdispGetHeight()/2, errmsg, font, Red);

	while(TRUE)
		gfxSleepMilliseconds(1000);
}
