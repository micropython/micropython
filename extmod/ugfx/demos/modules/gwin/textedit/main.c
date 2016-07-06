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

static GHandle		ghConsole;
static GHandle		ghTextedit1;
static GHandle		ghTextedit2;
static GHandle		ghTextedit3;
static GListener	gl;
#if GWIN_NEED_KEYBOARD
	static GHandle		ghKeyboard;
#endif

static void guiCreate(void)
{
	GWidgetInit	wi;
	gwinWidgetClearInit(&wi);

	// Console
	wi.g.show = TRUE;
	wi.g.x = 0;
	wi.g.y = 0;
	wi.g.width = gdispGetWidth()/2;
	wi.g.height = gdispGetHeight();
	ghConsole = gwinConsoleCreate(0, &wi.g);
	gwinSetColor(ghConsole, Yellow);
	gwinSetBgColor(ghConsole, Black);
	gwinSetFont(ghConsole, gdispOpenFont("UI2"));
	gwinClear(ghConsole);

	// TextEdit1
	wi.g.show = TRUE;
	wi.g.x = gdispGetWidth()/2 + 10;
	wi.g.y = 20;
	wi.g.width = 200;
	wi.g.height = 35;
	wi.text = "Use the TAB key";
	ghTextedit1 = gwinTexteditCreate(0, &wi, 100);

	// TextEdit2
	wi.g.show = TRUE;
	wi.g.x = gdispGetWidth()/2 + 10;
	wi.g.y = 100;
	wi.g.width = 200;
	wi.g.height = 35;
	wi.text = "to switch between";
	ghTextedit2 = gwinTexteditCreate(0, &wi, 20);
	//gwinTexteditSetBorder(ghTextedit2, FALSE);

	// TextEdit3
	wi.g.show = TRUE;
	wi.g.x = gdispGetWidth()/2 + 10;
	wi.g.y = 180;
	wi.g.width = 200;
	wi.g.height = 35;
	wi.text = "the different widgets";
	ghTextedit3 = gwinTexteditCreate(0, &wi, 100);
	//gwinTexteditSetBorder(ghTextedit3, TRUE);

	// Virtual keyboard
#if GWIN_NEED_KEYBOARD
	wi.g.show = TRUE;
	wi.g.x = 0;
	wi.g.y = gdispGetHeight()*3/4;
	wi.g.width = gdispGetWidth();
	wi.g.height = gdispGetHeight()/4;
	ghKeyboard = gwinKeyboardCreate(0, &wi);
#endif

}

int main(void) {
	GEventKeyboard* pk;
	unsigned i;

	gfxInit();

	gdispClear(Silver);
	gwinSetDefaultFont(gdispOpenFont("DejaVuSans16"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gwinSetDefaultColor(Black);
	gwinSetDefaultBgColor(White);

	geventListenerInit(&gl);
	geventAttachSource(&gl, ginputGetKeyboard(0), GLISTEN_KEYTRANSITIONS|GLISTEN_KEYUP);

	guiCreate();

	gwinPrintf(ghConsole, "Keyboard Monitor...\n");

	while(1) {
		// Get an Event
		pk = (GEventKeyboard *)geventEventWait(&gl, TIME_INFINITE);

		if (pk->type != GEVENT_KEYBOARD)
			continue;

		gwinPrintf(ghConsole, "KEYSTATE: 0x%04X [ %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s]",
			pk->keystate,
			(!pk->keystate ? "NONE " : ""),
			((pk->keystate & GKEYSTATE_KEYUP) ? "KEYUP " : ""),
			((pk->keystate & GKEYSTATE_REPEAT) ? "REPEAT " : ""),
			((pk->keystate & GKEYSTATE_SPECIAL) ? "SPECIAL " : ""),
			((pk->keystate & GKEYSTATE_RAW) ? "RAW " : ""),
			((pk->keystate & GKEYSTATE_SHIFT_L) ? "LSHIFT " : ""),
			((pk->keystate & GKEYSTATE_SHIFT_R) ? "RSHIFT " : ""),
			((pk->keystate & GKEYSTATE_CTRL_L) ? "LCTRL " : ""),
			((pk->keystate & GKEYSTATE_CTRL_R) ? "RCTRL " : ""),
			((pk->keystate & GKEYSTATE_ALT_L) ? "LALT " : ""),
			((pk->keystate & GKEYSTATE_ALT_R) ? "RALT " : ""),
			((pk->keystate & GKEYSTATE_FN) ? "FN " : ""),
			((pk->keystate & GKEYSTATE_COMPOSE) ? "COMPOSE " : ""),
			((pk->keystate & GKEYSTATE_WINKEY) ? "WINKEY " : ""),
			((pk->keystate & GKEYSTATE_CAPSLOCK) ? "CAPSLOCK " : ""),
			((pk->keystate & GKEYSTATE_NUMLOCK) ? "NUMLOCK " : ""),
			((pk->keystate & GKEYSTATE_SCROLLLOCK) ? "SCROLLLOCK " : "")
			);
		if (pk->bytecount) {
			gwinPrintf(ghConsole, " Keys:");
			for (i = 0; i < pk->bytecount; i++)
				gwinPrintf(ghConsole, " 0x%02X", (uint8_t)pk->c[i]);
			gwinPrintf(ghConsole, " [");
			for (i = 0; i < pk->bytecount; i++)
				gwinPrintf(ghConsole, "%c", pk->c[i] >= ' ' && pk->c[i] <= '~' ? pk->c[i] : ' ');
			gwinPrintf(ghConsole, "]");
		}
		gwinPrintf(ghConsole, "\n");
	}
}

