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

static GListener	gl;
static GHandle		ghSlider1, ghSlider2, ghConsole;

static void createWidgets(void) {
	GWidgetInit	wi;

	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;

	// create Slider1
	wi.g.y = 10; wi.g.x = 10; wi.g.width = gdispGetWidth()-20; wi.g.height = 20; wi.text = "S1";
	ghSlider1 = gwinSliderCreate(0, &wi);

	// create Slider2
	wi.g.y = 40; wi.g.x = 10; wi.g.width = 20; wi.g.height = gdispGetHeight() - 50; wi.text = "S2";
	ghSlider2 = gwinSliderCreate(0, &wi);

	// Set slider 2 to return extended events
	gwinSliderSendExtendedEvents(ghSlider2, TRUE);

	// Some options to try
	//gwinSliderSetRange(ghSlider1, 0, 70000);
	//gwinSliderSetRange(ghSlider2, -4, 4);
	//gwinSliderSetRange(ghSlider1, 4, -4);

	// Console to display slider events
	wi.g.y = 40; wi.g.x = 40; wi.g.width = gdispGetWidth()-50; wi.g.height = gdispGetHeight()-50;
	ghConsole = gwinConsoleCreate(0, &wi.g);
}

int main(void) {
	GEventGWinSlider *	pe;
	const char *		sAction;

	// Initialize the display
	gfxInit();

	// Set the widget defaults
	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gdispClear(White);

	// create the widget
	createWidgets();
	gwinSetColor(ghConsole, Green);
	gwinSetBgColor(ghConsole, White);
	gwinClear(ghConsole);

	// We want to listen for widget events
	geventListenerInit(&gl);
	gwinAttachListener(&gl);

	while(1) {
		// Get an Event (assume it is a slider event)
		pe = (GEventGWinSlider *)geventEventWait(&gl, TIME_INFINITE);

		switch(pe->type) {
			case GEVENT_GWIN_SLIDER:
				switch(pe->action) {
				case GSLIDER_EVENT_SET:		sAction = "SET";		break;
				case GSLIDER_EVENT_CANCEL:	sAction = "CANCEL";		break;
				case GSLIDER_EVENT_MOVE:	sAction = "MOVE";		break;
				case GSLIDER_EVENT_START:	sAction = "START";		break;
				default:					sAction = "????";		break;
				}
				gwinPrintf(ghConsole, "Slider %s = %d %s\n", gwinGetText(pe->gwin), pe->position, sAction);
				break;

			default:
				// Oops - not a slider event.
				break;
		}
	}

	return 0;
}

