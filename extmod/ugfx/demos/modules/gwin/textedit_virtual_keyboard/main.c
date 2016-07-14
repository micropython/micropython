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
static GHandle		ghTextedit1;
static GHandle		ghTextedit2;
static GHandle		ghTextedit3;
static GHandle		ghKeyboard;

static void guiCreate(void)
{
	GWidgetInit	wi;
	gwinWidgetClearInit(&wi);

	// TextEdit1
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 20;
	wi.g.width = 200;
	wi.g.height = 35;
	wi.text = "Use the TAB key";
	ghTextedit1 = gwinTexteditCreate(0, &wi, 100);

	// TextEdit2
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 100;
	wi.g.width = 200;
	wi.g.height = 35;
	wi.text = "to switch between";
	ghTextedit2 = gwinTexteditCreate(0, &wi, 20);

	// TextEdit3
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 180;
	wi.g.width = 200;
	wi.g.height = 35;
	wi.text = "the different widgets";
	ghTextedit3 = gwinTexteditCreate(0, &wi, 100);

	// Virtual keyboard
	wi.g.show = TRUE;
	wi.g.x = 0;
	wi.g.y = (gdispGetHeight()/2);
	wi.g.width = gdispGetWidth();
	wi.g.height = gdispGetHeight()/2;
	ghKeyboard = gwinKeyboardCreate(0, &wi);
}

int main(void)
{
	gfxInit();

	gdispClear(Silver);
	gwinSetDefaultFont(gdispOpenFont("DejaVuSans16"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gwinSetDefaultColor(Black);
	gwinSetDefaultBgColor(White);

	geventListenerInit(&gl);
	geventAttachSource(&gl, ginputGetKeyboard(0), 0);

	guiCreate();

	while(1) {
		gfxSleepMilliseconds(500);
	}
}

