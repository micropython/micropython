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
#include "tasks.h"

static volatile bool_t	run;
static GHandle			gh;
static gfxThreadHandle	thread;

static void mandelbrot(float x1, float y1, float x2, float y2) {
	unsigned int i,j, width, height;
	uint16_t iter;
	float fwidth, fheight;
	
	float sy = y2 - y1;
	float sx = x2 - x1;
	const int MAX = 512;
	
	width = (unsigned int)gwinGetWidth(gh);
	height = (unsigned int)gwinGetHeight(gh);
	fwidth = width;
	fheight = height;
	
	for(i = 0; i < width; i++) {
		for(j = 0; j < height; j++) {
			float cy = j * sy / fheight + y1;
			float cx = i * sx / fwidth + x1;
			float x=0.0f, y=0.0f, xx=0.0f, yy=0.0f;
			gfxYield();
			for(iter=0; iter <= MAX && xx+yy<4.0f; iter++) {
				xx = x*x;
				yy = y*y;
				y = 2.0f*x*y + cy;
				x = xx - yy + cx;
			}
			gwinSetColor(gh, RGB2COLOR(iter<<7, iter<<4, iter));
			gwinDrawPixel(gh, i, j);
		}
	}
}

static DECLARE_THREAD_FUNCTION(task, param) {
	float cx, cy;
	float zoom = 1.0f;
	(void) param;

	/* where to zoom in */
	cx = -0.086f;
	cy = 0.85f;

	while(run) {
		mandelbrot(-2.0f*zoom+cx, -1.5f*zoom+cy, 2.0f*zoom+cx, 1.5f*zoom+cy);

		zoom *= 0.7f;
		if(zoom <= 0.00001f)
			zoom = 1.0f;	
	}
	return 0;
}

void doMandlebrot(GHandle parent, bool_t start) {
	if (start) {
		run = TRUE;
		gh = parent;
		thread = gfxThreadCreate(0, 0x400, LOW_PRIORITY, task, 0);
	} else if (run) {
		run = FALSE;
		gfxThreadWait(thread);
	}
}


