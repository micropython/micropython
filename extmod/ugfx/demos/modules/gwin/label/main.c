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

GListener	gl;
GHandle		ghLabel1, ghLabel2;

static void createWidgets(void) {
	GWidgetInit	wi;

	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;

	// Create the IP label
	wi.g.width = 200; wi.g.height = 20; wi.g.x = 10, wi.g.y = 80;
	wi.text = "192.168.1.42";
	ghLabel1 = gwinLabelCreate(0, &wi);
	gwinLabelSetAttribute(ghLabel1, 100, "Current IP:");

	// Create the DHCP label
	wi.g.width = 200; wi.g.height = 20; wi.g.x = 10, wi.g.y = 100;
	wi.text = "Off";
	ghLabel2 = gwinLabelCreate(0, &wi);
	gwinLabelSetAttribute(ghLabel2, 100, "DHCP:");
}

int main(void) {
	GEvent* pe;

	// Initialize the display
	gfxInit();

	// Set the widget defaults
	gwinSetDefaultFont(gdispOpenFont("*"));
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
	}

	return 0;
}
