/*
 * File: notepadCore.c
 *
 * This file is a part of the Notepad demo application for ChibiOS/GFX
 * Copyright © 2013, Kumar Abhishek [abhishek.kakkar@edaboard.com].
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 * 	   notice, this list of conditions and the following disclaimer in the
 * 	   documentation and/or other materials provided with the distribution.
 * 	 * The name of 'Kumar Abhishek' may not be used to endorse or promote
 *     products derived from this software without specific prior
 *     written permission.
 *
 * DISCLAIMER OF WARRANTY:
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
 *
 * Modified by InMarket to allow it to compile on any GFX supported operating system.
 */

#include <stdlib.h>

#include "gfx.h"

#include "notepadCore.h"
#include "notepadUIDefines.h"

#define PEN_IN_DRAWING_AREA(ev)		((ev.x >= ncoreDrawingArea->x) && (ev.x <= (ncoreDrawingArea->x + ncoreDrawingArea->width)) && \
									 (ev.y >= ncoreDrawingArea->y) && (ev.y <= (ncoreDrawingArea->y + ncoreDrawingArea->height)))

/* This is the drawing core */
static DECLARE_THREAD_STACK(waDrawThread, NCORE_THD_STACK_SIZE);

static uint8_t 					nPenWidth = 1;
static uint8_t 					nMode = NCORE_MODE_DRAW;

static gfxThreadHandle			nThd;

static GHandle					ncoreDrawingArea = NULL;
static GHandle					nStatusConsole = NULL;

static volatile bool_t			doExit;

static void draw_point(coord_t x, coord_t y) {
  color_t c = ncoreDrawingArea->color;

  if (nMode == NCORE_MODE_DRAW)
	c = ncoreDrawingArea->color;
  else if (nMode == NCORE_MODE_ERASE)
	c = ncoreDrawingArea->bgcolor;

  if (nPenWidth == 1)
	gdispDrawPixel(x, y, c);
  else
	gdispFillCircle(x, y, nPenWidth, c);
}

/* Bresenham's Line Drawing Algorithm
   Modified version to draw line of variable thickness */
static void draw_line(coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
  int16_t dy, dx;
  int16_t addx, addy;
  int16_t P, diff, i;
  
  if (x1 >= x0) {
	  dx = x1 - x0;
	  addx = 1;
  } else {
	  dx = x0 - x1;
	  addx = -1;
  }
  if (y1 >= y0) {
	  dy = y1 - y0;
	  addy = 1;
  } else {
	  dy = y0 - y1;
	  addy = -1;
  }

  if (dx >= dy) {
	  dy *= 2;
	  P = dy - dx;
	  diff = P - dx;

	  for(i=0; i<=dx; ++i) {
		  draw_point(x0, y0);
		  if (P < 0) {
			  P  += dy;
			  x0 += addx;
		  } else {
			  P  += diff;
			  x0 += addx;
			  y0 += addy;
		  }
	  }
  } else {
	  dx *= 2;
	  P = dx - dy;
	  diff = P - dy;

	  for(i=0; i<=dy; ++i) {
		  draw_point(x0, y0);
		  if (P < 0) {
			  P  += dx;
			  y0 += addy;
		  } else {
			  P  += diff;
			  x0 += addx;
			  y0 += addy;
		  }
	  }
  }
}

/* Core thread */
static DECLARE_THREAD_FUNCTION(ncoreDrawThread, msg) {

  GEventMouse ev, evPrev;
  coord_t dx, dy;

  int state = 0, dist;

  (void)msg;

  ginputGetMouseStatus(0, &evPrev);

  while (!doExit) {

	ginputGetMouseStatus(0, &ev);
	switch(state) {
	  case 0:	if (ev.meta == GMETA_MOUSE_DOWN) {
				  state = 1;
				  if (nMode == NCORE_MODE_FILL && PEN_IN_DRAWING_AREA(ev)) {
					// Set bgcolor to current color, clear the display.
					ncoreDrawingArea->bgcolor = ncoreDrawingArea->color;
					gwinClear(ncoreDrawingArea);
				  }
				}
				else
				  gfxYield();
				break;


	  case 1:   if (ev.meta == GMETA_MOUSE_UP) {
				  state = 0;
				  //gwinPrintf(nStatusConsole, "\nPen Up: (%d, %d)", ev.x, ev.y);
				  break;
				}

				dx = abs(ev.x - evPrev.x);
				dy = abs(ev.y - evPrev.y);

				dist = dx * dx + dy * dy;

				if (dist > 0)
				{
				   gdispSetClip(ncoreDrawingArea->x,
				                ncoreDrawingArea->y,
				                ncoreDrawingArea->width,
				                ncoreDrawingArea->height);

				   if (PEN_IN_DRAWING_AREA(ev)){
					// Do Interpolation
					if (dist <= 2) {
					  draw_point(ev.x, ev.y);
					}
					else if (dist <= 5) {
					  // Line drawing does not give good results for this case.
					  // So draw two pixels directly
					  draw_point(ev.x, ev.y);
					  draw_point((ev.x + evPrev.x) / 2,	(ev.y + evPrev.y) / 2);
					}
					else if (dx * dx <= MAX_DX && dy * dy <= MAX_DY) {
					  draw_line(ev.x, ev.y,	evPrev.x, evPrev.y);
					}
				  }

				  //gwinPrintf(nStatusConsole, "\nPen Down: (%d, %d)", ev.x, ev.y);
				}
				break;
	}
	evPrev = ev;
  }

  return 0;
}

/* Spawn the core thread */
void ncoreSpawnDrawThread(GHandle drawingArea, GHandle statusConsole) {

  ncoreDrawingArea = drawingArea;
  nStatusConsole = statusConsole;
  doExit = FALSE;

  nThd = gfxThreadCreate(waDrawThread,
                           sizeof(waDrawThread),
                           NCORE_THD_PRIO,
                           ncoreDrawThread,
                           NULL);

}

/* Terminate the core thread, wait for control release */
void ncoreTerminateDrawThread(void) {
  doExit = TRUE;
  gfxThreadWait(nThd);
  nThd = 0;
}

/* Get and set the pen width
 * Brush is cicular, width is pixel radius */
void ncoreSetPenWidth(uint8_t penWidth) { nPenWidth = penWidth; }
uint8_t ncoreGetPenWidth(void) 			{ return nPenWidth; }

/* Get and set the drawing color */
void ncoreSetPenColor(color_t penColor) { gwinSetColor(ncoreDrawingArea, penColor); }
color_t ncoreGetPenColor(void) 			{ return ncoreDrawingArea->color; }

/* Set mode */
void ncoreSetMode(uint8_t mode)			{ nMode = mode; }
uint8_t ncoreGetMode(void)				{ return nMode; }
