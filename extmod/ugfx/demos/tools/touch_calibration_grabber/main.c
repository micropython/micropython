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
#include "src/ginput/ginput_driver_mouse.h"

static GConsoleObject			gc;
static font_t					font;
static coord_t					bHeight;
static GHandle					ghc;
static coord_t					swidth, sheight;

#if !GWIN_CONSOLE_USE_FLOAT
	#error "You need to enable float support for the console widget. (GWIN_CONSOLE_USE_FLOAT)"
#endif

int main(void) {
	GMouse*					m;

	// Initialize everything
	gfxInit();

	// Get the display dimensions
	swidth = gdispGetWidth();
	sheight = gdispGetHeight();

	// Create our title
	font = gdispOpenFont("UI2");
	gwinSetDefaultFont(font);
	bHeight = gdispGetFontMetric(font, fontHeight)+4;
	gdispFillStringBox(0, 0, swidth, bHeight, "Touchscreen Calibration Grabber", font, Red, White, justifyCenter);

	// Create our main display writing window
	{
		GWindowInit				wi;

		gwinClearInit(&wi);
		wi.show = TRUE; wi.x = 0; wi.y = bHeight; wi.width = swidth; wi.height = sheight-bHeight;
		ghc = gwinConsoleCreate(&gc, &wi);
	}

	// Get access to the GMouse structure
	m = (GMouse*)gdriverGetInstance(GDRIVER_TYPE_MOUSE, 0);
	if (!m) {
		gfxHalt("No mouse instance 0.");
	}

	// Print the calibration values
	gwinPrintf(ghc, "\r\n\n");
	gwinPrintf(ghc, "  ax: %f\r\n", m->caldata.ax);
	gwinPrintf(ghc, "  bx: %f\r\n", m->caldata.bx);
	gwinPrintf(ghc, "  cx: %f\r\n", m->caldata.cx);
	gwinPrintf(ghc, "  ay: %f\r\n", m->caldata.ay);
	gwinPrintf(ghc, "  by: %f\r\n", m->caldata.by);
	gwinPrintf(ghc, "  cy: %f\r\n", m->caldata.cy);

	while(1) {

		// Always sleep a bit first to enable other events. We actually don't mind missing events.
		gfxSleepMilliseconds(100);
	}
}
