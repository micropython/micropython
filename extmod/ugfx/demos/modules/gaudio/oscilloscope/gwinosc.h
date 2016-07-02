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

#ifndef _GWINOSC_H
#define _GWINOSC_H

/**
 * --------------------------- Our Custom GWIN Oscilloscope ---------------
 *
 * This GWIN superset implements a simple audio oscilloscope using the GAUDIN device.
 *
 * It makes many assumptions.
 *
 * The GMISC module with GMISC_NEED_ARRAYOPS could be used to process the samples more
 * correctly if we were really building something generic.
 */

/* The extent of scaling for our audio data - fixed scale at the moment */
#ifndef SCOPE_Y_BITS
	#define SCOPE_Y_BITS			8					// 8 bits = 0..255
#endif

/* Trigger methods */
#define TRIGGER_NONE			0					/* No triggering */
#define TRIGGER_POSITIVERAMP	1					/* Trigger on a positive going signal */
#define TRIGGER_MINVALUE		2					/* Trigger on reaching the minimum value from the last scope */

/**
 * Which trigger we want to use.
 * Experiments suggests that TRIGGER_MINVALUE gives the best result
 */
#ifndef TRIGGER_METHOD
	#define TRIGGER_METHOD			TRIGGER_MINVALUE
#endif

/* A scope window object. Treat it as a black box */
typedef struct GScopeObject_t {
	GWindowObject		g;						// Base Class

	coord_t				*lastscopetrace;		// To store last scope trace
	ArrayDataFormat		format;					// The sample format
	coord_t				nextx;					// Where we are up to
#if TRIGGER_METHOD == TRIGGER_POSITIVERAMP
	coord_t				lasty;					// The last y value - used for trigger slope detection
#elif TRIGGER_METHOD == TRIGGER_MINVALUE
	coord_t				lasty;					// The last y value - used for trigger slope detection
	coord_t				scopemin;				// The last scopes minimum value
#endif
	} GScopeObject;

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * Create a scope window.
	 */
	GHandle gwinGScopeCreate(GDisplay *g, GScopeObject *gs, GWindowInit *pInit, uint16_t channel, uint32_t frequency, ArrayDataFormat format);
	#define gwinScopeCreate(gs,pI,ch,f,fmt)		gwinGScopeCreate(GDISP,gs,pI,ch,f,fmt)

	/**
	 * Wait for a scope trace to be ready and then draw it.
	 */
	void gwinScopeWaitForTrace(GHandle gh);

#ifdef __cplusplus
}
#endif

#endif /* _GWINOSC_H */
