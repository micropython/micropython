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
GHandle		ghList1, ghList2;

static void createWidgets(void) {
	GWidgetInit	wi;

	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;

	// Create the label for the first list
	wi.g.width = 150; wi.g.height = 20; wi.g.x = 10, wi.g.y = 80;
	wi.text = "List 1: Default";
	ghLabel1 = gwinLabelCreate(0, &wi);

	// Create the label for the second list
	wi.g.width = 150; wi.g.height = 20; wi.g.x = 165, wi.g.y = 80;
	wi.text = "List 2: Smooth scrolling";
	ghLabel1 = gwinLabelCreate(0, &wi);

	// The first list widget
	wi.g.width = 150;
	wi.g.height = 100;
	wi.g.y = 100;
	wi.g.x = 10;
	wi.text = "Name of list 1";
	ghList1 = gwinListCreate(0, &wi, FALSE);

	// The second list widget
	wi.g.width = 150;
	wi.g.height = 100;
	wi.g.y = 100;
	wi.g.x = 165;
	wi.text = "Name of list 2";
	ghList2 = gwinListCreate(0, &wi, FALSE);
	gwinListSetScroll(ghList2, scrollSmooth);
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

	// Add some items to the first list widget
	gwinListAddItem(ghList1, "Item 0", FALSE);
	gwinListAddItem(ghList1, "Item 1", FALSE);
	gwinListAddItem(ghList1, "Item 2", FALSE);
	gwinListAddItem(ghList1, "Item 3", FALSE);
	gwinListAddItem(ghList1, "Item 4", FALSE);
	gwinListAddItem(ghList1, "Item 5", FALSE);
	gwinListAddItem(ghList1, "Item 6", FALSE);
	gwinListAddItem(ghList1, "Item 7", FALSE);
	gwinListAddItem(ghList1, "Item 8", FALSE);
	gwinListAddItem(ghList1, "Item 9", FALSE);
	gwinListAddItem(ghList1, "Item 10", FALSE);
	gwinListAddItem(ghList1, "Item 11", FALSE);
	gwinListAddItem(ghList1, "Item 12", FALSE);
	gwinListAddItem(ghList1, "Item 13", FALSE);

	// Add some items to the second list widget
	// This time we will disable the render until
	// all the items have been added
	gwinListEnableRender(ghList2, FALSE);
	gwinListAddItem(ghList2, "Item 0", FALSE);
	gwinListAddItem(ghList2, "Item 1", FALSE);
	gwinListAddItem(ghList2, "Item 2", FALSE);
	gwinListAddItem(ghList2, "Item 3", FALSE);
	gwinListAddItem(ghList2, "Item 4", FALSE);
	gwinListAddItem(ghList2, "Item 5", FALSE);
	gwinListAddItem(ghList2, "Item 6", FALSE);
	gwinListAddItem(ghList2, "Item 7", FALSE);
	gwinListAddItem(ghList2, "Item 8", FALSE);
	gwinListAddItem(ghList2, "Item 9", FALSE);
	gwinListAddItem(ghList2, "Item 10", FALSE);
	gwinListAddItem(ghList2, "Item 11", FALSE);
	gwinListAddItem(ghList2, "Item 12", FALSE);
	gwinListAddItem(ghList2, "Item 13", FALSE);
	gwinListEnableRender(ghList2, TRUE);

	while(1) {
		// Get an Event
		pe = geventEventWait(&gl, TIME_INFINITE);

	}

	return 0;
}
