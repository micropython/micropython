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

#include "gfx.h"

// We get nasty and look at some internal structures - get the relevant information
#include "src/gdriver/gdriver.h"
#include "src/ginput/ginput_driver_mouse.h"

#include <string.h>

static GConsoleObject			gc;
static GListener				gl;
static font_t					font;
static coord_t					bHeight;
static GHandle					ghc;
static coord_t					swidth, sheight;

/*------------------------------------------------------------------------*
 * GINPUT Touch Driver Calibrator.                                        *
 *------------------------------------------------------------------------*/
int main(void) {
	GSourceHandle			gs;
	GEventMouse				*pem;
	GMouse *				m;
	GMouseVMT *				vmt;

	gfxInit();		// Initialize the display

	// Get the display dimensions
	swidth = gdispGetWidth();
	sheight = gdispGetHeight();

	// Create our title
	font = gdispOpenFont("UI2");
	gwinSetDefaultFont(font);
	bHeight = gdispGetFontMetric(font, fontHeight)+4;
	gdispFillStringBox(0, 0, swidth, bHeight, "Raw Touch Readings", font, Red, White, justifyCenter);

	// Create our main display writing window
	{
		GWindowInit				wi;

		gwinClearInit(&wi);
		wi.show = TRUE; wi.x = 0; wi.y = bHeight; wi.width = swidth; wi.height = sheight-bHeight;
		ghc = gwinConsoleCreate(&gc, &wi);
	}

	// Initialize the listener
	geventListenerInit(&gl);

	// Copy the current mouse's VMT so we can play with it.
	m = (GMouse *)gdriverGetInstance(GDRIVER_TYPE_MOUSE, 0);
	if (!m) gfxHalt("No mouse instance 0");
	vmt = gfxAlloc(sizeof(GMouseVMT));
	if (!vmt) gfxHalt("Could not allocate memory for mouse VMT");
	memcpy(vmt, m->d.vmt, sizeof(GMouseVMT));

	// Swap VMT's on the current mouse to our RAM copy
	m->d.vmt = (const GDriverVMT *)vmt;

	// Listen for events
	gs = ginputGetMouse(0);
	geventAttachSource(&gl, gs, GLISTEN_MOUSEDOWNMOVES|GLISTEN_MOUSEMETA);

	// Make sure we are in uncalibrated pen mode
	m->flags &= ~(GMOUSE_FLG_CALIBRATE|GMOUSE_FLG_CLIP|GMOUSE_FLG_FINGERMODE);

	// Pretend we are a mouse, turn off all touch processing, turn off move and click filtering
	vmt->d.flags &= ~(GMOUSE_VFLG_TOUCH | GMOUSE_VFLG_ONLY_DOWN | GMOUSE_VFLG_POORUPDOWN);
	vmt->pen_jitter.move = 0;
	vmt->pen_jitter.click = 0;

	// For this test turn on ALL mouse movement events
	geventAttachSource(&gl, gs, GLISTEN_MOUSEDOWNMOVES|GLISTEN_MOUSEUPMOVES|GLISTEN_MOUSEMETA|GLISTEN_MOUSENOFILTER);

	while(1) {
		pem = (GEventMouse *)geventEventWait(&gl, TIME_INFINITE);
		gwinPrintf(ghc, "%u, %u z=%u b=0x%04x\n", pem->x, pem->y, pem->z, pem->buttons & ~GINPUT_MISSED_MOUSE_EVENT);

		// Always sleep a bit first to enable other events. We actually don't mind missing events.
		gfxSleepMilliseconds(100);
	}
}
