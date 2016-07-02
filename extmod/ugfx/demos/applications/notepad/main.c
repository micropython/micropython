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

#define COLOR_SIZE	20
#define PEN_SIZE	20
#define OFFSET		3

#define COLOR_BOX(a)		(ev.x >= a && ev.x <= a + COLOR_SIZE)
#define PEN_BOX(a)			(ev.y >= a && ev.y <= a + COLOR_SIZE)
#define GET_COLOR(a)		(COLOR_BOX(a * COLOR_SIZE + OFFSET))
#define GET_PEN(a)			(PEN_BOX(a * 2 * PEN_SIZE + OFFSET))
#define DRAW_COLOR(a)		(a * COLOR_SIZE + OFFSET)
#define DRAW_PEN(a)			(a * 2 * PEN_SIZE + OFFSET)
#define DRAW_AREA(x, y)		(x >= PEN_SIZE + OFFSET + 3 && x <= gdispGetWidth() && \
							 y >= COLOR_SIZE + OFFSET + 3 && y <= gdispGetHeight())

void drawScreen(void) {
	char *msg = "uGFX";
	font_t		font1, font2;

	font1 = gdispOpenFont("DejaVuSans24*");
	font2 = gdispOpenFont("DejaVuSans12*");

	gdispClear(White);
	gdispDrawString(gdispGetWidth()-gdispGetStringWidth(msg, font1)-3, 3, msg, font1, Black);
	
	/* colors */
	gdispFillArea(0 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, Black);	/* Black */
	gdispFillArea(1 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, Red);		/* Red */
	gdispFillArea(2 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, Yellow);	/* Yellow */
	gdispFillArea(3 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, Green);	/* Green */
	gdispFillArea(4 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, Blue);		/* Blue */
	gdispDrawBox (5 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, Black);	/* White */

	/* pens */	
	gdispFillStringBox(OFFSET * 2, DRAW_PEN(1), PEN_SIZE, PEN_SIZE, "1", font2, White, Black, justifyCenter);
	gdispFillStringBox(OFFSET * 2, DRAW_PEN(2), PEN_SIZE, PEN_SIZE, "2", font2, White, Black, justifyCenter);
	gdispFillStringBox(OFFSET * 2, DRAW_PEN(3), PEN_SIZE, PEN_SIZE, "3", font2, White, Black, justifyCenter);
	gdispFillStringBox(OFFSET * 2, DRAW_PEN(4), PEN_SIZE, PEN_SIZE, "4", font2, White, Black, justifyCenter);
	gdispFillStringBox(OFFSET * 2, DRAW_PEN(5), PEN_SIZE, PEN_SIZE, "5", font2, White, Black, justifyCenter);
	
	gdispCloseFont(font1);
	gdispCloseFont(font2);
}

GEventMouse		ev;

int main(void) {
	color_t color = Black;
	uint16_t pen = 0;

	gfxInit();
	ginputGetMouse(0);

	drawScreen();

	while (TRUE) {
		ginputGetMouseStatus(0, &ev);
		if (!(ev.buttons & GINPUT_MOUSE_BTN_LEFT))
			continue;

		/* inside color box ? */
		if(ev.y >= OFFSET && ev.y <= COLOR_SIZE) {
			     if(GET_COLOR(0)) 	color = Black;
			else if(GET_COLOR(1))	color = Red;
			else if(GET_COLOR(2))	color = Yellow;
			else if(GET_COLOR(3))	color = Green;
			else if(GET_COLOR(4))	color = Blue;
			else if(GET_COLOR(5))	color = White;
		
		/* inside pen box ? */
		} else if(ev.x >= OFFSET && ev.x <= PEN_SIZE) {
			     if(GET_PEN(1))		pen = 0;
			else if(GET_PEN(2))		pen = 1;
			else if(GET_PEN(3))		pen = 2;
			else if(GET_PEN(4))		pen = 3;
			else if(GET_PEN(5))		pen = 4;		

		/* inside drawing area ? */
		} else if(DRAW_AREA(ev.x, ev.y)) {
			if(pen == 0)
				gdispDrawPixel(ev.x, ev.y, color);
			else
				gdispFillCircle(ev.x, ev.y, pen, color);
		}
	}
}

