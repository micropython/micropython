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

/**
 * --------------------------- Our Custom GWIN Oscilloscope ---------------
 *
 * This GWIN superset implements a simple audio oscilloscope using the GADC high speed device.
 */
#include "gfx.h"
#include "gwinosc.h"

/* Include internal GWIN routines so we can build our own superset class */
#include "src/gwin/gwin_class.h"

/* How many flat-line sample before we trigger */
#define FLATLINE_SAMPLES		8

static void _destroy(GHandle gh) {
	gadcHighSpeedStop();
	if (((GScopeObject *)gh)->lastscopetrace) {
		gfxFree(((GScopeObject *)gh)->lastscopetrace);
		((GScopeObject *)gh)->lastscopetrace = 0;
	}
}

static const gwinVMT scopeVMT = {
		"Scope",				// The classname
		sizeof(GScopeObject),	// The object size
		_destroy,				// The destroy routine
		0,						// The redraw routine
		0,						// The after-clear routine
};

GHandle gwinGScopeCreate(GDisplay *g, GScopeObject *gs, GWindowInit *pInit, uint32_t physdev, uint32_t frequency) {
	/* Initialise the base class GWIN */
	if (!(gs = (GScopeObject *)_gwindowCreate(g, &gs->g, pInit, &scopeVMT, 0)))
		return 0;
	gs->nextx = 0;
	if (!(gs->lastscopetrace = gfxAlloc(gs->g.width * sizeof(coord_t))))
		return 0;
#if TRIGGER_METHOD == TRIGGER_POSITIVERAMP
	gs->lasty = gs->g.height/2;
#elif TRIGGER_METHOD == TRIGGER_MINVALUE
	gs->lasty = gs->g.height/2;
	gs->scopemin = 0;
#endif

	/* Start the GADC high speed converter */
	gadcHighSpeedInit(physdev, frequency);
	gadcHighSpeedStart();

	gwinSetVisible((GHandle)gs, pInit->show);
	return (GHandle)gs;
}

void gwinScopeWaitForTrace(GHandle gh) {
	#define 		gs	((GScopeObject *)(gh))
	int				i;
	coord_t			x, y;
	coord_t			yoffset;
	adcsample_t		*pa;
	coord_t			*pc;
	GDataBuffer		*pd;
	uint8_t			shr;
#if TRIGGER_METHOD == TRIGGER_POSITIVERAMP
	bool_t			rdytrigger;
	int				flsamples;
#elif TRIGGER_METHOD == TRIGGER_MINVALUE
	bool_t			rdytrigger;
	int				flsamples;
	coord_t			scopemin;
#endif

	if (gh->vmt != &scopeVMT)
		return;

	/* Wait for a set of conversions */
	pd = gadcHighSpeedGetData(TIME_INFINITE);

	/* Ensure we are drawing in the right area */
	#if GDISP_NEED_CLIP
		gdispGSetClip(gh->display, gh->x, gh->y, gh->width, gh->height);
	#endif

	shr = 16 - gfxSampleFormatBits(GADC_SAMPLE_FORMAT);
	yoffset = gh->height/2;
	if (!gfxSampleFormatIsSigned(GADC_SAMPLE_FORMAT))
		yoffset += (1<<SCOPE_Y_BITS)/2;
	x = gs->nextx;
	pc = gs->lastscopetrace+x;
	pa = (adcsample_t *)(pd+1);
#if TRIGGER_METHOD == TRIGGER_POSITIVERAMP
	rdytrigger = FALSE;
	flsamples = 0;
#elif TRIGGER_METHOD == TRIGGER_MINVALUE
	rdytrigger = FALSE;
	flsamples = 0;
	scopemin = 0;
#endif

	for(i = pd->len/sizeof(adcsample_t); i; i--) {

		/* Calculate the new scope value - re-scale using simple shifts for efficiency, re-center and y-invert */
		y = yoffset - (((coord_t)(*pa++) << shr) >> (16-SCOPE_Y_BITS));

#if TRIGGER_METHOD == TRIGGER_MINVALUE
		/* Calculate the scopemin ready for the next trace */
		if (y > scopemin)
			scopemin = y;
#endif

		/* Have we reached the end of a scope trace? */
		if (x >= gh->width) {

#if TRIGGER_METHOD == TRIGGER_POSITIVERAMP || TRIGGER_METHOD == TRIGGER_MINVALUE
			/* Handle triggering - we trigger on the next sample minimum (y value maximum) or a flat-line */

			#if TRIGGER_METHOD == TRIGGER_MINVALUE
				/* Arm when we reach the sample minimum (y value maximum) of the previous trace */
				if (!rdytrigger && y >= gs->scopemin)
					rdytrigger = TRUE;
			#endif

			if (y == gs->lasty) {
				/* Trigger if we get too many flat-line samples regardless of the armed state */
				if (++flsamples < FLATLINE_SAMPLES)
					continue;
				flsamples = 0;
			} else if (y > gs->lasty) {
				gs->lasty = y;
				flsamples = 0;
				#if TRIGGER_METHOD == TRIGGER_POSITIVERAMP
					/* Arm the trigger when samples fall (y increases) ie. negative slope */
					rdytrigger = TRUE;
				#endif
				continue;
			} else {
				/* If the trigger is armed, Trigger when samples increases (y decreases) ie. positive slope */
				gs->lasty = y;
				flsamples = 0;
				if (!rdytrigger)
					continue;
			}

			/* Ready for a the next trigger cycle */
			rdytrigger = FALSE;
#endif

			/* Prepare for a scope trace */
			x = 0;
			pc = gs->lastscopetrace;
		}

		/* Clear the old scope pixel and then draw the new scope value */
		gdispGDrawPixel(gh->display, gh->x+x, gh->y+pc[0], gh->bgcolor);
		gdispGDrawPixel(gh->display, gh->x+x, gh->y+y, gh->color);

		/* Save the value */
		*pc++ = y;
		x++;
		#if TRIGGER_METHOD == TRIGGER_POSITIVERAMP || TRIGGER_METHOD == TRIGGER_MINVALUE
			gs->lasty = y;
		#endif
	}
	gs->nextx = x;
#if TRIGGER_METHOD == TRIGGER_MINVALUE
	gs->scopemin = scopemin;
#endif

	gfxBufferRelease(pd);

	#undef gs
}
