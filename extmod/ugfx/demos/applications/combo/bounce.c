/*
 * Copyright (c) 2012, 2013, Joel Bodenmann aka Tectu <joel@unormal.org>
 * Copyright (c) 2012, 2013, Andrew Hannam aka inmarket
 * Derived from the 2011 IOCCC submission by peter.eastman@gmail.com
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
#include <math.h>

#include "tasks.h"

static volatile bool_t	run;
static GHandle			gh;
static gfxThreadHandle	thread;

/**
 * NOTE:
 *
 * This demo uses floating point operations. Don't expect it to work with any
 * speed unless your processor has an FPU.
 *
 * If you see garbage inside the ball as it is running rather than the red and yellow
 * checkerboard pattern then the fast invsqrt() function in GMISC does not work on
 * your processor.
 *
 * You can modify the implementation of invsqrt() by firstly defining
 * 		#define GMISC_INVSQRT_MIXED_ENDIAN	TRUE
 * in your gfxconf.h file.
 *
 * If it still doesn't work then instead define
 * 		#define GMISC_INVSQRT_REAL_SLOW		TRUE
 * in your gfxconf.h file. This should always work although it will probably be slow.
 */
#define BALLCOLOR1		Red
#define BALLCOLOR2		Yellow
#define WALLCOLOR		HTML2COLOR(0x303030)
#define BACKCOLOR		HTML2COLOR(0xC0C0C0)
#define FLOORCOLOR		HTML2COLOR(0x606060)
#define SHADOWALPHA		(255-255*0.2)

static DECLARE_THREAD_FUNCTION(task, param) {
	coord_t		width, height, x, y, radius, ballx, bally, dx, floor;
	coord_t		minx, miny, maxx, maxy, winx, winy;
	coord_t		ballcx, ballcy;
	color_t		colour;
	float		ii, spin, dy, spinspeed, h, f, g;
	(void)		param;

	winx = gwinGetScreenX(gh);
	winy = gwinGetScreenY(gh);
	width = gwinGetWidth(gh);
	height = gwinGetHeight(gh);

	radius=height/5+height%2+1;	// The ball radius
	ii = 1.0/radius;			// radius as easy math
	floor=height/5-1;			// floor position
	spin=0.0;					// current spin angle on the ball
	spinspeed=0.1;				// current spin speed of the ball
	ballx=width/2;				// ball x position (relative to the ball center)
	bally=height/4;				// ball y position (relative to the ball center)
	dx=.01*width;				// motion in the x axis
	dy=0.0;						// motion in the y axis
	ballcx = 12*radius/5;		// ball x diameter including the shadow
	ballcy = 21*radius/10;		// ball y diameter including the shadow


	minx = miny = 0; maxx = width; maxy = height;		// The clipping window for this frame.

	while(run) {
		// Draw one frame
		gdispStreamStart(winx+minx, winy+miny, maxx-minx, maxy-miny);
		for (y=miny; h = (bally-y)*ii, y<maxy; y++) {
			for (x=minx; x < maxx; x++) {
				g=(ballx-x)*ii;
				f=-.3*g+.954*h;
				if (g*g < 1-h*h) {
					/* The inside of the ball */
					if ((((int)(9-spin+(.954*g+.3*h)*invsqrt(1-f*f))+(int)(2+f*2))&1))
						colour = BALLCOLOR1;
					else
						colour = BALLCOLOR2;
				} else {
					// The background (walls and floor)
					if (y > height-floor) {
						if (x < height-y || height-y > width-x)
							colour = WALLCOLOR;
						else
							colour = FLOORCOLOR;
					} else if (x<floor || x>width-floor)
						colour = WALLCOLOR;
					else
						colour = BACKCOLOR;

					// The ball shadow is darker
					if (g*(g+.4)+h*(h+.1) < 1)
						colour = gdispBlendColor(colour, Black, SHADOWALPHA);
				}
				gdispStreamColor(colour);	/* pixel to the LCD */
			}
		}
		gdispStreamStop();

		// Force a display update if the controller supports it
		gdispFlush();

		// Calculate the new frame size (note this is a drawing optimisation only)
		minx = ballx - radius; miny = bally - radius;
		maxx = minx + ballcx; maxy = miny + ballcy;
		if (dx > 0) maxx += dx; else minx += dx;
		if (dy > 0) maxy += dy; else miny += dy;
		if (minx < 0) minx = 0;
		if (maxx > width) maxx = width;
		if (miny < 0) miny = 0;
		if (maxy > height) maxy = height;

		// Motion
		spin += spinspeed;
		ballx += dx; bally += dy;
		dx = ballx < radius || ballx > width-radius ? spinspeed=-spinspeed,-dx : dx;
		dy = bally > height-1.75*floor ? -.04*height : dy+.002*height;

		// Give someone else a go on cooperative os's like RAW32
		gfxYield();
	}
	return 0;
}

void doBounce(GHandle parent, bool_t start) {
	if (start) {
		run = TRUE;
		gh = parent;
		thread = gfxThreadCreate(0, 0x200, LOW_PRIORITY, task, 0);
	} else if (run) {
		run = FALSE;
		gfxThreadWait(thread);
		gfxYield();
	}
}

