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

/* The handles for our three consoles */
GHandle GW1, GW2, GW3;

int main(void) {
	uint8_t i;
	font_t	font1, font2;

	/* initialize and clear the display */
	gfxInit();

	/* Set some fonts */
	font1 = gdispOpenFont("UI2");
	font2 = gdispOpenFont("DejaVu Sans 12");
	gwinSetDefaultFont(font1);

	/* create the three console windows */
	{
		GWindowInit		wi;

		gwinClearInit(&wi);
		wi.show = TRUE;
		wi.x = 0; wi.y = 0; wi.width = gdispGetWidth(); wi.height = gdispGetHeight()/2;
		GW1 = gwinConsoleCreate(0, &wi);
		wi.y = gdispGetHeight()/2; wi.width = gdispGetWidth()/2; wi.height = gdispGetHeight();
		GW2 = gwinConsoleCreate(0, &wi);
		wi.x = gdispGetWidth()/2; wi.height = gdispGetHeight();
		GW3 = gwinConsoleCreate(0, &wi);
	}

	/* Use a special font for GW1 */
	gwinSetFont(GW1, font2);

	/* Set the fore- and background colors for each console */
	gwinSetColor(GW1, Green);
	gwinSetBgColor(GW1, Black);
	gwinSetColor(GW2, White);
	gwinSetBgColor(GW2, Blue);
	gwinSetColor(GW3, Black);
	gwinSetBgColor(GW3, Red);

	/* clear all console windows - to set background */
	gwinClear(GW1);
	gwinClear(GW2);
	gwinClear(GW3);

	/* Output some data on the first console */
	for(i = 0; i < 10; i++) {
		gwinPrintf(GW1, "Hello \033buGFX\033B!\n");
	}

	/* Output some data on the second console - Fast */
	for(i = 0; i < 32; i++) {
		gwinPrintf(GW2, "Message Nr.: \0331\033b%d\033B\033C\n", i+1);
	}

	/* Output some data on the third console - Slowly */
	for(i = 0; i < 32; i++) {
		gwinPrintf(GW3, "Message Nr.: \033u%d\033U\n", i+1);
		gfxSleepMilliseconds(500);
	}

	/* Make console 3 invisible and then visible again to demonstrate the history buffer */
	gwinPrintf(GW2, "Making red window \033uinvisible\033U\n");
	gwinSetVisible(GW3, FALSE);
	gfxSleepMilliseconds(1000);
	gwinPrintf(GW2, "Making red window \033uvisible\033U\n");
	gwinSetVisible(GW3, TRUE);
	gwinPrintf(GW3, "\033bI'm back!!!\033B\n", i+1);

	while(TRUE) {
		gfxSleepMilliseconds(500);
	}
}

