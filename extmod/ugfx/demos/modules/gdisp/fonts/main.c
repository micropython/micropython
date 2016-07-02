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

int main(void) {
	coord_t		width;
	font_t		font1, font2;
	coord_t		fheight1, fheight2;
	const char	*line1, *line2;
	char		buf[8];

    // Initialize and clear the display
    gfxInit();

    // Get the screen size
    width = gdispGetWidth();

    // Get the fonts we want to use
	font1 = gdispOpenFont("DejaVu*");
	font2 = gdispOpenFont("UI2*");
	//font2 = gdispOpenFont("Geosans*");
	//font2 = gdispOpenFont("Free*");
	//font2 = gdispOpenFont("Hellovetica*");
	//font2 = gdispOpenFont("babyblue*");
	//font2 = gdispOpenFont("PF Ronda*");
	//font2 = gdispOpenFont("Apple*");

	// Font 1
	line1 = "a b c d e f h i j k l m n o p q r s t u v w x y z A B C D E F G H I J K L M N O P Q R S T U V W X Y Z";
	line2 = "0 1 2 3 4 5 6 7 8 9 ~ ! @ # $ % ^ & * _ - + = ( ) { } [ ] < > | / \\ : ; , . ? ' \" `";
	fheight1 = gdispGetFontMetric(font1, fontHeight)+2;
	gdispFillStringBox(0, 0, width,  fheight1, line1, font1, Black, White, justifyCenter);
	gdispFillStringBox(0, fheight1+1, width,  fheight1, line2, font1, Black, White, justifyCenter);

	// Font 2
	fheight2 = gdispGetFontMetric(font2, fontHeight)+2;
	gdispFillStringBox(0, 2*fheight1+2, width,  fheight2, line1, font2, Black, White, justifyCenter);
	gdispFillStringBox(0, 2*fheight1+fheight2+3, width,  fheight2, line2, font2, Black, White, justifyCenter);

	// Show Sizes
	buf[0] = (fheight1-2)/10 + '0';
	buf[1] = (fheight1-2)%10 + '0';
	buf[2] = ',';
	buf[3] = ' ';
	buf[4] = (fheight2-2)/10 + '0';
	buf[5] = (fheight2-2)%10 + '0';
	buf[6] = 0;
	gdispFillStringBox(0, 2*fheight1+2*fheight2+4, width,  fheight1, buf, font1, Red, White, justifyCenter);
	
	// Wait forever
    while(TRUE) {
    	gfxSleepMilliseconds(500);
    }   
}

