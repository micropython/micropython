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

#include <stdio.h>

/*
 * This demo demonstrates two ways to talk to multiple displays.
 *
 * Method 1 is the preferred way however Method 2 would be useful
 * when quickly converting existing single display applications.
 *
 * Note you can combine the methods using method 2 for the first display
 * and method 1 for any extra displays.
 */

#define USE_METHOD_1		FALSE

#if USE_METHOD_1
	int main(void) {
		coord_t		width, height;
		coord_t		display, i, j, cnt;
		font_t		f;
		GDisplay	*g;
		char		buf[16];

		/* Initialize and clear the display */
		gfxInit();

		/* Get a font to write with */
		f = gdispOpenFont("*");

		/* Cycle through each display */
		cnt = gdispGetDisplayCount();
		for(display = 0; display < cnt; display++) {

			// Get the specified display
			g = gdispGetDisplay(display);

			// Get the screen size
			width = gdispGGetWidth(g);
			height = gdispGGetHeight(g);

			/* Draw draw draw */
			sprintg(buf, "Display %u", display);
			if (width < 128) {
				gdispGDrawBox(g, 0, 0, width/2, height/2, Yellow);
				gdispGFillStringBox(g, 0, height/2, width, height/2, buf, f, Black, Blue, justifyCenter);
			} else {
				gdispGDrawBox(g, 10, 10, width/2, height/2, Yellow);
				gdispGFillStringBox(g, width/2, height/2, width/2-10, height/2-10, buf, f, White, Blue, justifyCenter);
			}
			gdispGDrawLine(g, 5, 30, width-50, height-40, Red);

			for(i = 5, j = 0; i < width && j < height; i += 7, j += i/20)
				gdispGDrawPixel(g, i, j, White);
		}

		while(TRUE) {
			gfxSleepMilliseconds(500);
		}
	}
#else
	int main(void) {
		coord_t		width, height;
		coord_t		display, i, j, cnt;
		font_t		f;
		char		buf[16];

		/* Initialize and clear the display */
		gfxInit();

		/* Get a font to write with */
		f = gdispOpenFont("*");

		/* Cycle through each display */
		cnt = gdispGetDisplayCount();
		for(display = 0; display < cnt; display++) {

			// Set the default display to the specified display
			gdispSetDisplay(gdispGetDisplay(display));

			// Get the screen size
			width = gdispGetWidth();
			height = gdispGetHeight();

			/* Draw draw draw */
			sprintg(buf, "Display %u", display);
			if (width < 128) {
				gdispDrawBox(0, 0, width/2, height/2, Yellow);
				gdispFillStringBox(0, height/2, width, height/2, buf, f, Black, Blue, justifyCenter);
			} else {
				gdispDrawBox(10, 10, width/2, height/2, Yellow);
				gdispFillStringBox(width/2, height/2, width/2-10, height/2-10, buf, f, White, Blue, justifyCenter);
			}
			gdispDrawLine(5, 30, width-50, height-40, Red);

			for(i = 5, j = 0; i < width && j < height; i += 7, j += i/20)
				gdispDrawPixel(i, j, White);
		}

		while(TRUE) {
			gfxSleepMilliseconds(500);
		}
	}
#endif

