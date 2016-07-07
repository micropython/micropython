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
static coord_t					bWidth, bWidth2, bHeight;
static GHandle					ghc;
static coord_t					swidth, sheight;

static void DrawHeader(const char *title, bool_t btnNext, bool_t btnPrev, bool_t btnPlusMinus) {
	#if GDISP_NEED_CLIP
		gdispSetClip(0, 0, swidth, sheight);
	#endif
	gdispFillStringBox(0, 0, swidth, bHeight, "Touch Calibration", font, Red, White, justifyLeft);
	if (btnNext)
		gdispFillStringBox(swidth-1*bWidth, 0, bWidth  , bHeight, "Next", font, Black, Gray, justifyCenter);
	if (btnPrev)
		gdispFillStringBox(swidth-2*bWidth, 0, bWidth-1, bHeight, "Prev", font, Black, Gray, justifyCenter);
	if (btnPlusMinus) {
		gdispFillStringBox(swidth-2*bWidth-1*bWidth2, 0, bWidth2-1, bHeight, "+", font, Black, Gray, justifyCenter);
		gdispFillStringBox(swidth-2*bWidth-2*bWidth2, 0, bWidth2-1, bHeight, "-", font, Black, Gray, justifyCenter);
	}
	gwinClear(ghc);
	gwinSetColor(ghc, Yellow);
	gwinPrintf(ghc, "\n%s\n\n", title);
	gwinSetColor(ghc, White);
}

#define BTN_NONE	0
#define BTN_NEXT	1
#define BTN_PREV	2
#define BTN_PLUS	3
#define BTN_MINUS	4

static int CheckButtons(GEventMouse *pem) {
	if (pem->y < bHeight && (pem->buttons & GMETA_MOUSE_UP)) {
		if (pem->x >= swidth-1*bWidth)
			return BTN_NEXT;
		if (pem->x >= swidth-2*bWidth)
			return BTN_PREV;
		if (pem->x >= swidth-2*bWidth-1*bWidth2)
			return BTN_PLUS;
		if (pem->x >= swidth-2*bWidth-2*bWidth2)
			return BTN_MINUS;
	}
	return BTN_NONE;
}

/*------------------------------------------------------------------------*
 * GINPUT Touch Driver Calibrator.                                        *
 *------------------------------------------------------------------------*/
int main(void) {
	GSourceHandle			gs;
	GEventMouse				*pem;
	bool_t					isFirstTime;
	bool_t					isCalibrated;
	bool_t					isTouch;
	bool_t					isFinger;
	const char *			isFingerText;
	const char *			deviceText;
	GMouse *				m;
	GMouseVMT *				vmt;
	GMouseJitter *			pjit;
	uint32_t				calerr;

	gfxInit();		// Initialize the display

	// Get the display dimensions
	swidth = gdispGetWidth();
	sheight = gdispGetHeight();

	// Create our title
	font = gdispOpenFont("UI2");
	gwinSetDefaultFont(font);
	bWidth = gdispGetStringWidth("Next", font);
	bHeight = gdispGetStringWidth("Prev", font);
	if (bHeight > bWidth) bWidth = bHeight;
	bWidth += 4;
	bWidth2 = gdispGetStringWidth("+", font)*2;
	bHeight = gdispGetStringWidth("-", font)*2;
	if (bHeight > bWidth2) bWidth2 = bHeight;
	bWidth2 += 4;
	bHeight = gdispGetFontMetric(font, fontHeight)*2+2;

	// Create our main display window
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

	// Get initial display settings for buttons
	isFirstTime = TRUE;
	isCalibrated = (vmt->d.flags & GMOUSE_VFLG_CALIBRATE) ? FALSE : TRUE;
	calerr = 0;

	/*
	 * Test: Device Type
	 */

StepDeviceType:
	DrawHeader("1. Device Type", isCalibrated, isCalibrated && !isFirstTime, isCalibrated);

	// Get the type of device and the current mode
	isTouch = (vmt->d.flags & GMOUSE_VFLG_TOUCH) ? TRUE : FALSE;
	isFinger = (m->flags & GMOUSE_FLG_FINGERMODE) ? TRUE : FALSE;
	pjit = isFinger ? &vmt->finger_jitter : &vmt->pen_jitter;
	isFingerText = isFinger ? "finger" : "pen";
	deviceText = isTouch ? isFingerText : "mouse";

	gwinPrintf(ghc, "This is detected as a %s device\n\n", isTouch ? "TOUCH" : "MOUSE");
	gwinPrintf(ghc, "It is currently in %s mode\n\n", isFinger ? "FINGER" : "PEN");

	if (!isCalibrated)
		gwinPrintf(ghc, "Press and release your %s to move on to the next test.\n", deviceText);
	else {
		gwinPrintf(ghc, "Press + for pen or - for finger.\n");
		if (isFirstTime)
			gwinPrintf(ghc, "Press Next to continue.\n");
		else
			gwinPrintf(ghc, "Press Next or Back to continue.\n");
	}

	while(1) {
		pem = (GEventMouse *)geventEventWait(&gl, TIME_INFINITE);
		if (isCalibrated) {
			switch (CheckButtons(pem)) {
			case BTN_NEXT:
				break;
			case BTN_PREV:
				if (!isFirstTime)
					goto StepDrawing;
				continue;
			case BTN_PLUS:
				m->flags &= ~GMOUSE_FLG_FINGERMODE;
				goto StepDeviceType;
			case BTN_MINUS:
				m->flags |= GMOUSE_FLG_FINGERMODE;
				goto StepDeviceType;
			default:
				continue;
			}
			break;
		}
		if ((pem->buttons & GMETA_MOUSE_UP))
			break;
	}

	/*
	 * Test: Mouse raw reading
	 */

StepRawReading:
	DrawHeader("2. Raw Mouse Output", FALSE, FALSE, FALSE);
	if (isTouch)
		gwinPrintf(ghc, "Press and hold on the surface.\n\n");
	else
		gwinPrintf(ghc, "Press and hold the mouse button.\n\n");
	gwinPrintf(ghc, "The raw values coming from your mouse driver will display.\n\n");
	gwinPrintf(ghc, "Make sure the x and y values change as you move.\n\n");

	gwinPrintf(ghc, "Release your %s to move on to the next test.\n", deviceText);

	// Make sure we are in uncalibrated mode
	m->flags &= ~(GMOUSE_FLG_CALIBRATE|GMOUSE_FLG_CLIP);

	// For this test turn on ALL mouse movement events
	geventAttachSource(&gl, gs, GLISTEN_MOUSEDOWNMOVES|GLISTEN_MOUSEUPMOVES|GLISTEN_MOUSEMETA|GLISTEN_MOUSENOFILTER);

	while(1) {
		// Always sleep a bit first to enable other events. We actually don't
		// mind missing events for this test.
		gfxSleepMilliseconds(100);
		pem = (GEventMouse *)geventEventWait(&gl, TIME_INFINITE);
		gwinPrintf(ghc, "%u, %u z=%u b=0x%04x\n", pem->x, pem->y, pem->z, pem->buttons & ~GINPUT_MISSED_MOUSE_EVENT);
		if ((pem->buttons & GMETA_MOUSE_UP))
			break;
	}

	// Reset to calibrated condition
	if (isCalibrated) {
		m->flags |= GMOUSE_FLG_CLIP;
		if ((vmt->d.flags & GMOUSE_VFLG_CALIBRATE))
			m->flags |= GMOUSE_FLG_CALIBRATE;
	}

	// Reset to just changed movements.
	geventAttachSource(&gl, gs, GLISTEN_MOUSEDOWNMOVES|GLISTEN_MOUSEMETA);

	/*
	 * Test: Calibration
	 */

StepCalibrate:
	DrawHeader("3. Calibration Jitter", isCalibrated, isCalibrated, isCalibrated);
	if ((vmt->d.flags & GMOUSE_VFLG_CALIBRATE)) {
		gwinPrintf(ghc, "You will be presented with a number of points to touch.\nPress them in turn.\n\n"
				"If the calibration repeatedly fails, increase the jitter for %s calibration and try again.\n\n", isFingerText);
		gwinPrintf(ghc, "Pressing the surface for longer gives more accurate results.\n\n");
		if (calerr)
			gwinPrintf(ghc, "Last calibration error ^ 2 = %u\n", calerr);
		gwinPrintf(ghc, "Calibration jitter (%s) = %u\n", isFingerText, pjit->calibrate);
		if (isCalibrated)
			gwinPrintf(ghc, "Press + or - to adjust.\n");
	} else {
		gwinPrintf(ghc, "This device does not need calibration.\n\n");
	}
	if (isCalibrated)
		gwinPrintf(ghc, "Press Next or Back to continue.\n");
	else
		gwinPrintf(ghc, "Press and release your %s to move on to start calibration.\n", deviceText);

	while(1) {
		pem = (GEventMouse *)geventEventWait(&gl, TIME_INFINITE);
		if (isCalibrated) {
			switch (CheckButtons(pem)) {
			case BTN_NEXT:
				break;
			case BTN_PREV:
				goto StepRawReading;
			case BTN_PLUS:
				gwinPrintf(ghc, "Calibration jitter (%s) = %u", isFingerText, ++pjit->calibrate);
				continue;
			case BTN_MINUS:
				gwinPrintf(ghc, "Calibration jitter (%s) = %u", isFingerText, --pjit->calibrate);
				continue;
			default:
				continue;
			}
			break;
		}
		if ((pem->buttons & GMETA_MOUSE_UP))
			break;
	}

	// Calibrate
	if ((vmt->d.flags & GMOUSE_VFLG_CALIBRATE)) {
		calerr = ginputCalibrateMouse(0);
		if (calerr)
			goto StepCalibrate;
		isCalibrated = TRUE;
	}

	/*
	 * Test: Mouse coords
	 */

StepMouseCoords:
	DrawHeader("4. Show Mouse Coordinates", TRUE, TRUE, TRUE);
	if (isTouch)
		gwinPrintf(ghc, "Press and hold on the surface.\n\n");
	else
		gwinPrintf(ghc, "Press and hold the mouse button.\n\n");
	gwinPrintf(ghc, "Check the coordinates against where it should be on the screen.\n\n");
	gwinPrintf(ghc, "X should be 0 to %u\nY should be 0 to %u\n\n", swidth-1, sheight-1);
	gwinPrintf(ghc, "Press + to retry using extremes or - for normal calibration.\n");
	gwinPrintf(ghc, "Press Next or Back to continue.\n");

	// For this test normal mouse movement events
	geventAttachSource(&gl, gs, GLISTEN_MOUSEDOWNMOVES|GLISTEN_MOUSEMETA);

	while(1) {
		// Always sleep a bit first to enable other events. We actually don't
		// mind missing events for this test.
		gfxSleepMilliseconds(100);
		pem = (GEventMouse *)geventEventWait(&gl, TIME_INFINITE);

		switch (CheckButtons(pem)) {
		case BTN_NEXT:
			break;
		case BTN_PREV:
			goto StepCalibrate;
		case BTN_PLUS:
			vmt->d.flags |= GMOUSE_VFLG_CAL_EXTREMES;
			goto StepCalibrate;
		case BTN_MINUS:
			vmt->d.flags &= ~GMOUSE_VFLG_CAL_EXTREMES;
			goto StepCalibrate;
		default:
			gwinPrintf(ghc, "%u, %u\n", pem->x, pem->y);
			continue;
		}
		break;
	}

	// Reset to just changed movements.
	geventAttachSource(&gl, gs, GLISTEN_MOUSEDOWNMOVES|GLISTEN_MOUSEMETA);

	/*
	 * Test: Mouse movement jitter
	 */

StepMovementJitter:
	DrawHeader("5. Movement Jitter", TRUE, TRUE, TRUE);
	if (isTouch)
		gwinPrintf(ghc, "Press firmly on the surface and move around as if to draw.\n\n");
	else
		gwinPrintf(ghc, "Press and hold the mouse button and move around as if to draw.\n\n");

	gwinPrintf(ghc, "Dots will display in this window. Ensure that when you stop moving your %s that "
			"new dots stop displaying.\nNew dots should only display when your %s is moving.\n\n"
			"Adjust %s movement jitter to the smallest value that this reliably works for.\n\n", deviceText, deviceText, isFingerText);
	gwinPrintf(ghc, "Movement jitter (%s) = %u\n", isFingerText, pjit->move);
	gwinPrintf(ghc, "Press + or - to adjust.\n");
	gwinPrintf(ghc, "Press Next or Back to continue.\n\n");

	while(1) {
		pem = (GEventMouse *)geventEventWait(&gl, TIME_INFINITE);
		switch (CheckButtons(pem)) {
		case BTN_NEXT:
			break;
		case BTN_PREV:
			goto StepMouseCoords;
		case BTN_PLUS:
			gwinPrintf(ghc, "Movement jitter (%s) = %u", isFingerText, ++pjit->move);
			continue;
		case BTN_MINUS:
			gwinPrintf(ghc, "Movement jitter (%s) = %u", isFingerText, --pjit->move);
			continue;
		default:
			if ((pem->buttons & GINPUT_MOUSE_BTN_LEFT))
				gwinPrintf(ghc, ".");
			continue;
		}
		break;
	}

	/*
	 * Test: Click Jitter
	 */

StepClickJitter:
	gwinClear(ghc);
	gwinSetColor(ghc, Yellow);
	gwinPrintf(ghc, "\n6. Click Jitter\n\n");

	gwinSetColor(ghc, White);
	if (isTouch)
		gwinPrintf(ghc, "Press and release the touch surface to \"click\".\nTry both short and long presses.\n");
	else
		gwinPrintf(ghc, "Click the mouse with the left and right buttons.\n\n");
	gwinPrintf(ghc, "A yellow dash is a left (or short) click.\n"
			"A red x is a right (or long) click.\n\n"
			"Adjust %s click jitter to the smallest value that this reliably works for.\n"
			"Note: moving your %s during a click cancels it.\n\n", isFingerText, deviceText);
	gwinPrintf(ghc, "Click jitter (%s) = %u\n", isFingerText, pjit->click);
	gwinPrintf(ghc, "Press + or - to adjust.\n");
	gwinPrintf(ghc, "Press Next or Back to continue.\n\n");

	while(1) {
		pem = (GEventMouse *)geventEventWait(&gl, TIME_INFINITE);
		switch (CheckButtons(pem)) {
		case BTN_NEXT:
			break;
		case BTN_PREV:
			goto StepMovementJitter;
		case BTN_PLUS:
			gwinPrintf(ghc, "Click jitter (%s) = %u", isFingerText, ++pjit->click);
			continue;
		case BTN_MINUS:
			gwinPrintf(ghc, "Click jitter (%s) = %u", isFingerText, --pjit->click);
			continue;
		default:
			if ((pem->buttons & GMETA_MOUSE_CLICK)) {
				gwinSetColor(ghc, Yellow);
				gwinPrintf(ghc, "-");
			}
			if ((pem->buttons & GMETA_MOUSE_CXTCLICK)) {
				gwinSetColor(ghc, Red);
				gwinPrintf(ghc, "x");
			}
			continue;
		}
		break;
	}

	/*
	 * Test: Polling frequency
	 */

StepDrawing:
	gwinClear(ghc);
	gwinSetColor(ghc, Yellow);
	gwinPrintf(ghc, "\n7. Drawing\n\n");

	gwinSetColor(ghc, White);
	gwinPrintf(ghc, "Press firmly on the surface (or press and hold the mouse button) and move around as if to draw.\n\n");
	gwinPrintf(ghc, "A green line will follow your %s.\n\n", deviceText);
	gwinPrintf(ghc, "Pressing Next will start the tests again but with the option of changing pen/finger mode.\n\n");
	gwinPrintf(ghc, "Press Next or Back to continue.\n\n");

	while(1) {
		pem = (GEventMouse *)geventEventWait(&gl, TIME_INFINITE);
		if (pem->y < bHeight && pem->x >= swidth-2*bWidth) {
			if ((pem->buttons & GMETA_MOUSE_UP)) {
				if (pem->x >= swidth-bWidth)
					break;
				goto StepClickJitter;
			}
		}
		gdispDrawPixel(pem->x, pem->y, Green);
	}

	// Can't let this really exit
	isFirstTime = FALSE;
	goto StepDeviceType;
}
