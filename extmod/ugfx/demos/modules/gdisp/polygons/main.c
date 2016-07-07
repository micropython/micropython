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
#include <string.h>

#define FRAME_DELAY		100			/* The interframe delay in milliseconds */

// Our shape definition
static const point shape[] = {
		{ 10, 10 },
		{ 10, 15 },
		{ 17, 17 },
		{ 20, 5 },
		{ 10, 0 },
		{ 3, -10},
		{ -7, -15},
		{ -15, -15},
		{ -13, -12 },
		{ -8, 0 },
};

#define NUM_POINTS	(sizeof(shape)/sizeof(shape[0]))

// Where we put our calculated points
static point result[NUM_POINTS];
static point oldresult[NUM_POINTS];

// Make fixed point and floating point options look pretty similar
#if GMISC_NEED_MATRIXFLOAT2D
	static MatrixFloat2D	m1, m2;
	typedef float			mtype;
	#define FP2MTYPE(f)		(f)
	#define INT2MTYPE(i)	(i)
#elif GMISC_NEED_MATRIXFIXED2D
	static MatrixFixed2D	m1, m2;
	typedef fixed			mtype;
	#define FP2MTYPE(f)		FP2FIXED(f)
	#define INT2MTYPE(i)	FIXED(i)
#else
	#error "Either GMISC_NEED_MATRIXFLOAT2D or GMISC_NEED_MATRIXFIXED2D must be defined for this demo"
#endif


int main(void) {
	coord_t		width, height;
	mtype	scalex, scaley;
	mtype	scaleincx, scaleincy;
	mtype	translatex, translatey;
	mtype	transincx, transincy;
	mtype	transmaxx, transmaxy;
	mtype	transminx, transminy;
	int		angle;
	int		angleinc;

    // Initialize and clear the display
    gfxInit();
    gdispClear(White);

    // Get the screen size
    width = gdispGetWidth();
    height = gdispGetHeight();

    // Initial position, rotation and scale
   	scalex = scaley = FP2MTYPE(1.0);
    translatex = INT2MTYPE(width/2);
    translatey = INT2MTYPE(height/2);
    angle = 0;

    // How much to change the position, rotation and scale each cycle
   	scaleincx = scaleincy = FP2MTYPE(0.5);
    transincx = INT2MTYPE(2);
    transincy = INT2MTYPE(-2);
    angleinc = 2;
    transmaxx = INT2MTYPE(width - 15);
    transminx = INT2MTYPE(15);
    transmaxy = INT2MTYPE(height - 15);
    transminy = INT2MTYPE(15);

    while(TRUE) {

    	// Move to the new location, rotation and scale
		scalex += scaleincx;
		scaley += scaleincy;
		angle += angleinc;
		if (angle >= 360)
			angle -= 360;
		translatex += transincx;
		translatey += transincy;

		// Adjust our speed etc
		if (scalex > FP2MTYPE(10.0))		scaleincx = FP2MTYPE(-0.3);
		else if (scalex < FP2MTYPE(1.0))	scaleincx = FP2MTYPE(0.5);
		if (scaley > FP2MTYPE(7.0))			scaleincy = FP2MTYPE(-0.25);
		else if (scaley < FP2MTYPE(1.0))	scaleincy = FP2MTYPE(0.3);
		if (translatex > transmaxx || translatex < transminx)		transincx = -transincx;
		if (translatey > transmaxy || translatey < transminy)		transincy = -transincy;

		// Build a matrix of all the operations we want to do and then apply it to all the shape points
		#if GMISC_NEED_MATRIXFLOAT2D
	    	gmiscMatrixFloat2DApplyScale(&m1, 0, scalex, scaley);
	    	gmiscMatrixFloat2DApplyRotation(&m2, &m1, angle);
	    	gmiscMatrixFloat2DApplyTranslation(&m1, &m2, translatex, translatey);
	    	gmiscMatrixFloat2DApplyToPoints(result, shape, &m1, NUM_POINTS);
		#else
	    	gmiscMatrixFixed2DApplyScale(&m1, 0, scalex, scaley);
	    	gmiscMatrixFixed2DApplyRotation(&m2, &m1, angle);
	    	gmiscMatrixFixed2DApplyTranslation(&m1, &m2, translatex, translatey);
	    	gmiscMatrixFixed2DApplyToPoints(result, shape, &m1, NUM_POINTS);
		#endif

	    // Clear the old shape
    	gdispDrawPoly(0, 0, oldresult, NUM_POINTS, White);
    	gdispFillConvexPoly(0, 0, oldresult, NUM_POINTS, White);

    	// Play the new shape position
    	// NOTE: Because our shape is not a true convex polygon, we get some frames
    	//			where the line outline and the filled shape don't match. This is
    	//			expected due to the shapes non-convex nature and was left in the
    	//			demo to demonstrate this exact point.
    	gdispFillConvexPoly(0, 0, result, NUM_POINTS, Red);
    	gdispDrawPoly(0, 0, result, NUM_POINTS, Blue);

    	// Make sure the display is updated
    	gdispFlush();

    	// Save the results so we can clear them next cycle
    	memcpy(oldresult, result, sizeof(result));

    	// Delay for a while
    	gfxSleepMilliseconds(FRAME_DELAY);
    }   
}

