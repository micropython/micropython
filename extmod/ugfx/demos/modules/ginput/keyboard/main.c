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

// The handle for our console
static GHandle GW;

// THe listener
static GListener	gl;

int main(void) {
	GEventKeyboard *pk;
	unsigned		i;

	/* initialize and clear the display */
	gfxInit();

	/* Set a font */
	gwinSetDefaultFont(gdispOpenFont("*"));

	// We want to listen for keyboard events
	geventListenerInit(&gl);
	geventAttachSource(&gl, ginputGetKeyboard(0), GLISTEN_KEYTRANSITIONS|GLISTEN_KEYUP);

	/* create the console window */
	{
		GWindowInit		wi;

		gwinClearInit(&wi);
		wi.show = TRUE;
		wi.x = 0; wi.y = 0; wi.width = gdispGetWidth(); wi.height = gdispGetHeight();
		GW = gwinConsoleCreate(0, &wi);

		/* Set the fore- and background colors for the console */
		gwinSetColor(GW, Yellow);
		gwinSetBgColor(GW, Black);
		gwinClear(GW);
	}


	/* Say Hello */
	gwinPrintf(GW, "Keyboard Monitor...\n");

	while(1) {
		// Get an Event
		pk = (GEventKeyboard *)geventEventWait(&gl, TIME_INFINITE);

		if (pk->type != GEVENT_KEYBOARD)
			continue;

		gwinPrintf(GW, "KEYSTATE: 0x%04X [ %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s]",
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
			gwinPrintf(GW, " Keys:");
			for (i = 0; i < pk->bytecount; i++)
				gwinPrintf(GW, " 0x%02X", (uint8_t)pk->c[i]);
			gwinPrintf(GW, " [");
			for (i = 0; i < pk->bytecount; i++)
				gwinPrintf(GW, "%c", pk->c[i] >= ' ' && pk->c[i] <= '~' ? pk->c[i] : ' ');
			gwinPrintf(GW, "]");
		}
		gwinPrintf(GW, "\n");
	}
}

