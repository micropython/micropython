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

#define GROUP1		0	// this will be the first radio button group
#define GROUP2		1	// this will be the second radio button group

static GListener	gl;
static GHandle		ghRadio11, ghRadio12, ghRadio13;	// the first group
static GHandle		ghRadio21, ghRadio22, ghRadio23;	// the second group

static void createWidgets(void) {
	GWidgetInit	wi;

	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;

	// create Radio11
	wi.g.y = 10; wi.g.x = 10; wi.g.width = 80; wi.g.height = 20; wi.text = "Radio 1";
	ghRadio11 = gwinRadioCreate(0, &wi, GROUP1);

	// create Radio12
	wi.g.y = 50; wi.g.x = 10; wi.g.width = 80; wi.g.height = 20; wi.text = "Radio 2";
	ghRadio12 = gwinRadioCreate(0, &wi, GROUP1);

	// create Radio13
	wi.g.y = 90; wi.g.x = 10; wi.g.width = 80; wi.g.height = 20; wi.text = "Radio 3";
	ghRadio13 = gwinRadioCreate(0, &wi, GROUP1);

	// create Radio21
	wi.g.y = 10; wi.g.x = 120; wi.g.width = 80; wi.g.height = 20; wi.text = "Radio 1";
	ghRadio21 = gwinRadioCreate(0, &wi, GROUP2);

	// create Radio22
	wi.g.y = 50; wi.g.x = 120; wi.g.width = 80; wi.g.height = 20; wi.text = "Radio 2";
	ghRadio22 = gwinRadioCreate(0, &wi, GROUP2);

	// create Radio23
	wi.g.y = 90; wi.g.x = 120; wi.g.width = 80; wi.g.height = 20; wi.text = "Radio 3";
	ghRadio23 = gwinRadioCreate(0, &wi, GROUP2);

	// Check the first radio buttons
	gwinRadioPress(ghRadio11);
	gwinRadioPress(ghRadio21);
}

int main(void) {
	GEvent* pe;

	// Initialize the display
	gfxInit();

	// Set the widget defaults
	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gdispClear(White);

	// create the widget
	createWidgets();

	// We want to listen for widget events
	geventListenerInit(&gl);
	gwinAttachListener(&gl);

	while(1) {
		// Get an Event
		pe = geventEventWait(&gl, TIME_INFINITE);

		switch(pe->type) {
			case GEVENT_GWIN_RADIO:
				//printf("group: %u   radio: %s\n", ((GEventGWinRadio *)pe)->group, gwinGetText(((GEventGWinRadio *)pe)->gwin));
				break;

			default:
				break;
		}
	}

	return 0;
}

