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
 * This demo demonstrates the use of the GAUDOUT module to play a wave file.
 *
 */
#include "gfx.h"

/* Specify our timing parameters */
#define MY_AUDIO_CHANNEL				0								/* Use channel 0 */
#define MY_AUDIO_CHANNEL_IS_STEREO		GAUDOUT_CHANNEL0_IS_STEREO		/* Is it stereo? */

// Storage for the wave header
static char whdr[32];


/*
 * Application entry point.
 */
int main(void) {
	font_t			font;
	GFILE			*f;
	char 			*errmsg;
	uint32_t		toplay;
	uint32_t		frequency;
	ArrayDataFormat datafmt;
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
	if (!(f = gfileOpen("allwrong.wav", "r"))) {
		errmsg = "Err: Open WAV";
		goto theend;
	}

	// Read the wave header
	if (gfileRead(f, whdr, 20) != 20) {
		errmsg = "Err: Read hdr";
		goto theend;
	}

	// Check the wave header
	if (whdr[0] != 'R' || whdr[1] != 'I' || whdr[2] != 'F' || whdr[3] != 'F'				// RIFF label
			|| whdr[8] != 'W' || whdr[9] != 'A' || whdr[10] != 'V' || whdr[11] != 'E'		// WAVE label
			|| whdr[12] != 'f' || whdr[13] != 'm' || whdr[14] != 't' || whdr[15] != ' '		// fmt label
			) {
		errmsg = "Err: Bad header";
		goto theend;
	}

	// Read the fmt block
	len = (((uint32_t)(uint8_t)whdr[16])<<0) | (((uint32_t)(uint8_t)whdr[17])<<8)
					| (((uint32_t)(uint8_t)whdr[18])<<16) | (((uint32_t)(uint8_t)whdr[19])<<24);
	if (len > sizeof(whdr) || len < 16) {
		errmsg = "Err: Bad fmt len";
		goto theend;
	}
	if (gfileRead(f, whdr, len) != len) {
		errmsg = "Err: Read fmt block";
		goto theend;
	}
	if (whdr[0] != 1 || whdr[1] != 0												// PCM format
			|| (whdr[14] != 8 && whdr[14] != 16) || whdr[15] != 0					// 8 or 16 bits per sample
			) {
		errmsg = "Err: Bad fmt block";
		goto theend;
	}

	// Check stereo/mono
	#if MY_AUDIO_CHANNEL_IS_STEREO
		if (whdr[2] != 2 || whdr[3] != 0) {
			errmsg = "Err: Not stereo";
			goto theend;
		}
	#else
		if (whdr[2] != 1 || whdr[3] != 0) {
			errmsg = "Err: Not mono";
			goto theend;
		}
	#endif

	// Get the sample frequency (little endian format) and format
	frequency = (((uint32_t)(uint8_t)whdr[4])<<0) | (((uint32_t)(uint8_t)whdr[5])<<8)
					| (((uint32_t)(uint8_t)whdr[6])<<16) | (((uint32_t)(uint8_t)whdr[7])<<24);
	datafmt = whdr[14] == 8 ? ARRAY_DATA_8BITUNSIGNED : ARRAY_DATA_16BITSIGNED;

	// Initialise the audio output device
	if (!gaudioPlayInit(MY_AUDIO_CHANNEL, frequency, datafmt)) {
		// For this demo we don't try and do any format conversions if the driver won't accept
		//	what we initially request.
		errmsg = "Err: Bad format/freq";
		goto theend;
	}

	// Get our current file position
	len += 20;

	// Read RIFF blocks until we get to the data RIFF block (contains the audio)
	while(TRUE) {
		if (gfileRead(f, whdr, 8) != 8) {
			errmsg = "Err: Read block";
			goto theend;
		}

		// Get the block length (little endian format)
		toplay = (((uint32_t)(uint8_t)whdr[4])<<0) | (((uint32_t)(uint8_t)whdr[5])<<8)
						| (((uint32_t)(uint8_t)whdr[6])<<16) | (((uint32_t)(uint8_t)whdr[7])<<24);

		// Stop scanning when this is a data block
		if (whdr[0] == 'd' && whdr[1] == 'a' && whdr[2] == 't' && whdr[3] == 'a')
			break;

		// Seek to the next block header
		len += toplay + 8;
		if (!gfileSetPos(f, len)) {
			errmsg = "Err: No data";
			goto theend;
		}
	}

	// Play the file
	gdispDrawString(0, gdispGetHeight()/2, "Playing...", font, Yellow);
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
