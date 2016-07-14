/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/ginput/ginput_mouse.c
 * @brief   GINPUT mouse/touch code.
 */
#include "../../gfx.h"

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE

// Just to make code easier
#if !GFX_USE_GDISP
	#define GDISP	0
#endif

// Local Settings
#define CALIBRATION_POLL_PERIOD			20				// milliseconds
#define CALIBRATION_MINPRESS_PERIOD		300				// milliseconds
#define CALIBRATION_MAXPRESS_PERIOD		5000			// milliseconds

#define CALIBRATION_FONT				"* Double"
#define CALIBRATION_FONT2				"* Narrow"
#define CALIBRATION_BACKGROUND			Blue

#define CALIBRATION_CROSS_COLOR1		White
#define CALIBRATION_CROSS_COLOR2		RGB2COLOR(184,158,131)
#define CALIBRATION_CROSS_INNERGAP		2
#define CALIBRATION_CROSS_RADIUS		15

#define CALIBRATION_TITLE				"Calibration"
#define CALIBRATION_TITLE_Y				5
#define CALIBRATION_TITLE_HEIGHT		30
#define CALIBRATION_TITLE_COLOR			White
#define CALIBRATION_TITLE_BACKGROUND	Blue

#define CALIBRATION_ERROR_TEXT			"Calibration Failed!"
#define CALIBRATION_ERROR_DELAY			3000
#define CALIBRATION_ERROR_COLOR			Red
#define CALIBRATION_ERROR_BACKGROUND	Yellow
#define CALIBRATION_ERROR_Y				35
#define CALIBRATION_ERROR_HEIGHT		40

// Get the mouse driver interface
#include "ginput_driver_mouse.h"

// The mouse poll timer
static GTIMER_DECL(MouseTimer);

// Calibration application
#if !GINPUT_TOUCH_NOCALIBRATE
	#include <string.h>							// Required for memcpy

	static GFXINLINE void CalibrationTransform(GMouseReading *pt, const GMouseCalibration *c) {
		coord_t x, y;

		x = (coord_t) (c->ax * pt->x + c->bx * pt->y + c->cx);
		y = (coord_t) (c->ay * pt->x + c->by * pt->y + c->cy);
		
		pt->x = x;
		pt->y = y;
	}
#endif

static void SendMouseEvent(GSourceListener	*psl, GMouse *m, GMouseReading *r) {
	GEventMouse		*pe;

	// If there is no event buffer just mark a missed event
	if (!(pe = (GEventMouse *)geventGetEventBuffer(psl))) {
		// This listener is missing - save the meta events that have happened
		psl->srcflags |= ((r->buttons & GMETA_MASK)|GINPUT_MISSED_MOUSE_EVENT);
		return;
	}

	// If we haven't really moved (and there are no meta events) don't bother sending the event
	if (!(r->buttons & GMETA_MASK) && !psl->srcflags && !(psl->listenflags & GLISTEN_MOUSENOFILTER)
			&& r->x == m->r.x && r->y == m->r.y && (r->buttons & GINPUT_MOUSE_BTN_MASK) == (m->r.buttons & GINPUT_MOUSE_BTN_MASK))
		return;

	// Send the event only if we are listening for it
	if (!((r->buttons & GINPUT_MOUSE_BTN_LEFT) && (psl->listenflags & GLISTEN_MOUSEDOWNMOVES))
			&& !(!(r->buttons & GINPUT_MOUSE_BTN_LEFT) && (psl->listenflags & GLISTEN_MOUSEUPMOVES))
			&& !((r->buttons & GMETA_MASK) && (psl->listenflags & GLISTEN_MOUSEMETA)))
		return;

	#if !GINPUT_TOUCH_NOTOUCH
		pe->type = (gmvmt(m)->d.flags & GMOUSE_VFLG_TOUCH) ? GEVENT_TOUCH : GEVENT_MOUSE;
	#else
		pe->type = GEVENT_MOUSE;
	#endif
	pe->x = r->x;
	pe->y = r->y;
	pe->z = r->z;
	pe->buttons = r->buttons | psl->srcflags;
	psl->srcflags = 0;
	pe->display = m->display;
	geventSendEvent(psl);
}

static void GetMouseReading(GMouse *m) {
	GMouseReading	r;

	// Step 1 - Get the Raw Reading
	{
		m->flags &= ~GMOUSE_FLG_NEEDREAD;
		if (!gmvmt(m)->get(m, &r))
			return;
	}

	// Step 2 - Handle touch and button 0 debouncing
	{
		// Clean off button garbage
		r.buttons &= GINPUT_MOUSE_BTN_MASK;

		#if !GINPUT_TOUCH_NOTOUCH
			// If touch then calculate button 0 from z
			if ((gmvmt(m)->d.flags & GMOUSE_VFLG_TOUCH)) {
				if (gmvmt(m)->z_min <= gmvmt(m)->z_max) {
					if (r.z >= gmvmt(m)->z_touchon)			r.buttons |= GINPUT_MOUSE_BTN_LEFT;
					else if (r.z <= gmvmt(m)->z_touchoff)	r.buttons &= ~GINPUT_MOUSE_BTN_LEFT;
					else									return;				// bad transitional reading
				} else {
					if (r.z <= gmvmt(m)->z_touchon)			r.buttons |= GINPUT_MOUSE_BTN_LEFT;
					else if (r.z >= gmvmt(m)->z_touchoff)	r.buttons &= ~GINPUT_MOUSE_BTN_LEFT;
					else									return;				// bad transitional reading
				}
			}

			// Devices with poor button 0 transitioning need debouncing
			if ((gmvmt(m)->d.flags & GMOUSE_VFLG_POORUPDOWN)) {
				// Are we in a transition test
				if ((m->flags & GMOUSE_FLG_INDELTA)) {
					if (!((r.buttons ^ m->r.buttons) & GINPUT_MOUSE_BTN_LEFT)) {
						// Transition failed
						m->flags &= ~GMOUSE_FLG_INDELTA;
						return;
					}
					// Transition succeeded
					m->flags &= ~GMOUSE_FLG_INDELTA;

				// Should we start a transition test
				} else if (((r.buttons ^ m->r.buttons) & GINPUT_MOUSE_BTN_LEFT)) {
					m->flags |= GMOUSE_FLG_INDELTA;
					return;
				}
			}
		#endif

		#if !GINPUT_TOUCH_NOCALIBRATE_GUI
			// Stop here with just the raw x,y reading during calibration
			if ((m->flags & GMOUSE_FLG_IN_CAL)) {
				if ((r.buttons & GINPUT_MOUSE_BTN_LEFT)) {
					m->r.x = r.x;
					m->r.y = r.y;
				}
				m->r.buttons = r.buttons;
				return;
			}
		#endif
	}

	// Step 3 - Apply calibration, rotation and display clipping
	{
		// If the mouse is up we may need to keep our previous position
		if ((gmvmt(m)->d.flags & GMOUSE_VFLG_ONLY_DOWN) && !(r.buttons & GINPUT_MOUSE_BTN_LEFT)) {
			r.x = m->r.x;
			r.y = m->r.y;

		} else {

			#if !GINPUT_TOUCH_NOCALIBRATE
				// Do we need to calibrate the reading?
				if ((m->flags & GMOUSE_FLG_CALIBRATE))
					CalibrationTransform(&r, &m->caldata);
			#endif

			// We can't clip or rotate if we don't have a display
			if (m->display) {
				coord_t			w, h;

				// We now need display information
				w = gdispGGetWidth(m->display);
				h = gdispGGetHeight(m->display);

				#if GDISP_NEED_CONTROL
					// Do we need to rotate the reading to match the display
					if (!(gmvmt(m)->d.flags & GMOUSE_VFLG_SELFROTATION)) {
						coord_t		t;

						switch(gdispGGetOrientation(m->display)) {
							case GDISP_ROTATE_0:
								break;
							case GDISP_ROTATE_90:
								t = r.x;
								r.x = w - 1 - r.y;
								r.y = t;
								break;
							case GDISP_ROTATE_180:
								r.x = w - 1 - r.x;
								r.y = h - 1 - r.y;
								break;
							case GDISP_ROTATE_270:
								t = r.y;
								r.y = h - 1 - r.x;
								r.x = t;
								break;
							default:
								break;
						}
					}
				#endif

				// Do we need to clip the reading to the display
				if ((m->flags & GMOUSE_FLG_CLIP)) {
					if (r.x < 0)		r.x = 0;
					else if (r.x >= w)	r.x = w-1;
					if (r.y < 0)		r.y = 0;
					else if (r.y >= h)	r.y = h-1;
				}
			}
		}
	}

	// Step 4 - Apply jitter detection
	#if !GINPUT_TOUCH_NOTOUCH
	{
		const GMouseJitter	*pj;
		uint32_t			diff;

		// Are we in pen or finger mode
		pj = (m->flags & GMOUSE_FLG_FINGERMODE) ? &gmvmt(m)->finger_jitter : &gmvmt(m)->pen_jitter;

		// Is this just movement jitter
		if (pj->move > 0) {
			diff = (uint32_t)(r.x - m->r.x) * (uint32_t)(r.x - m->r.x) + (uint32_t)(r.y - m->r.y) * (uint32_t)(r.y - m->r.y);
			if (diff < (uint32_t)pj->move * (uint32_t)pj->move) {
				r.x = m->r.x;
				r.y = m->r.y;
			}
		}

		// Check if the click has moved outside the click area and if so cancel the click
		if (pj->click > 0 && (m->flags & GMOUSE_FLG_CLICK_TIMER)) {
			diff = (uint32_t)(r.x - m->clickpos.x) * (uint32_t)(r.x - m->clickpos.x) + (uint32_t)(r.y - m->clickpos.y) * (uint32_t)(r.y - m->clickpos.y);
			if (diff > (uint32_t)pj->click * (uint32_t)pj->click)
				m->flags &= ~GMOUSE_FLG_CLICK_TIMER;
		}
	}
	#endif

	// Step 5 - Click, context-click and other meta event detection
	{
		uint16_t		upbtns, dnbtns;

		// Calculate button transitions
		dnbtns = r.buttons & ~m->r.buttons;
		upbtns = ~r.buttons & m->r.buttons;

		// Left mouse down generates the Mouse-down meta event
		if ((dnbtns & GINPUT_MOUSE_BTN_LEFT))
			r.buttons |= GMETA_MOUSE_DOWN;

		// Left mouse up generates the Mouse-up meta event
		if ((upbtns & GINPUT_MOUSE_BTN_LEFT))
			r.buttons |= GMETA_MOUSE_UP;

		// Left/Right mouse down starts the click timer
		if ((dnbtns & (GINPUT_MOUSE_BTN_LEFT|GINPUT_MOUSE_BTN_RIGHT))) {
			m->clickpos.x = r.x;
			m->clickpos.y = r.y;
			m->clicktime = gfxSystemTicks();
			m->flags |= GMOUSE_FLG_CLICK_TIMER;
		}

		// Left/Right mouse up with the click timer still running may generate a click or context click
		if ((upbtns & (GINPUT_MOUSE_BTN_LEFT|GINPUT_MOUSE_BTN_RIGHT)) && (m->flags & GMOUSE_FLG_CLICK_TIMER)) {
			m->flags &= ~GMOUSE_FLG_CLICK_TIMER;
			m->clicktime = gfxSystemTicks() - m->clicktime;

			// Was this a short click?
			if (m->clicktime <= gfxMillisecondsToTicks(GINPUT_MOUSE_CLICK_TIME)) {
				if ((upbtns & GINPUT_MOUSE_BTN_RIGHT))
					r.buttons |= GMETA_MOUSE_CXTCLICK;
				if ((upbtns & GINPUT_MOUSE_BTN_LEFT))
					r.buttons |= GMETA_MOUSE_CLICK;
			}

			#if !GINPUT_TOUCH_NOTOUCH
				// Was this a long click on a touch device?
				if ((gmvmt(m)->d.flags & GMOUSE_VFLG_TOUCH) && m->clicktime >= gfxMillisecondsToTicks(GINPUT_TOUCH_CXTCLICK_TIME))
					r.buttons |= GMETA_MOUSE_CXTCLICK;
			#endif
		}
	}

	// Step 6 - Send the event to the listeners that are interested.
	{
		GSourceListener	*psl;

		// Send to the "All Mice" source listeners
		psl = 0;
		while ((psl = geventGetSourceListener((GSourceHandle)&MouseTimer, psl)))
			SendMouseEvent(psl, m, &r);

		// Send to the mouse specific source listeners
		psl = 0;
		while ((psl = geventGetSourceListener((GSourceHandle)m, psl)))
			SendMouseEvent(psl, m, &r);
	}

	// Step 7 - Finally save the results
	m->r.x = r.x;
	m->r.y = r.y;
	m->r.z = r.z;
	m->r.buttons = r.buttons;
}

static void MousePoll(void *param) {
	GMouse *	m;
	(void) 		param;

	for(m = (GMouse *)gdriverGetNext(GDRIVER_TYPE_MOUSE, 0); m; m = (GMouse *)gdriverGetNext(GDRIVER_TYPE_MOUSE, (GDriver *)m)) {
		if (!(gmvmt(m)->d.flags & GMOUSE_VFLG_NOPOLL) || (m->flags & GMOUSE_FLG_NEEDREAD))
			GetMouseReading(m);
	}
}

// Calibration user interface
#if !GINPUT_TOUCH_NOCALIBRATE_GUI
	#if !defined(GFX_USE_GDISP) || !GFX_USE_GDISP
		#error "GINPUT: GFX_USE_GDISP must be defined when calibration is required"
	#endif

	static GFXINLINE void CalibrationCrossDraw(GMouse *m, const point *pp) {
		gdispGDrawLine(m->display, pp->x-CALIBRATION_CROSS_RADIUS, pp->y, pp->x-CALIBRATION_CROSS_INNERGAP, pp->y, CALIBRATION_CROSS_COLOR1);
		gdispGDrawLine(m->display, pp->x+CALIBRATION_CROSS_INNERGAP, pp->y, pp->x+CALIBRATION_CROSS_RADIUS, pp->y, CALIBRATION_CROSS_COLOR1);
		gdispGDrawLine(m->display, pp->x, pp->y-CALIBRATION_CROSS_RADIUS, pp->x, pp->y-CALIBRATION_CROSS_INNERGAP, CALIBRATION_CROSS_COLOR1);
		gdispGDrawLine(m->display, pp->x, pp->y+CALIBRATION_CROSS_INNERGAP, pp->x, pp->y+CALIBRATION_CROSS_RADIUS, CALIBRATION_CROSS_COLOR1);
		gdispGDrawLine(m->display, pp->x-CALIBRATION_CROSS_RADIUS, pp->y+CALIBRATION_CROSS_RADIUS, pp->x-CALIBRATION_CROSS_RADIUS/2, pp->y+CALIBRATION_CROSS_RADIUS, CALIBRATION_CROSS_COLOR2);
		gdispGDrawLine(m->display, pp->x-CALIBRATION_CROSS_RADIUS, pp->y+CALIBRATION_CROSS_RADIUS/2, pp->x-CALIBRATION_CROSS_RADIUS, pp->y+CALIBRATION_CROSS_RADIUS, CALIBRATION_CROSS_COLOR2);
		gdispGDrawLine(m->display, pp->x-CALIBRATION_CROSS_RADIUS, pp->y-CALIBRATION_CROSS_RADIUS, pp->x-CALIBRATION_CROSS_RADIUS/2, pp->y-CALIBRATION_CROSS_RADIUS, CALIBRATION_CROSS_COLOR2);
		gdispGDrawLine(m->display, pp->x-CALIBRATION_CROSS_RADIUS, pp->y-CALIBRATION_CROSS_RADIUS/2, pp->x-CALIBRATION_CROSS_RADIUS, pp->y-CALIBRATION_CROSS_RADIUS, CALIBRATION_CROSS_COLOR2);
		gdispGDrawLine(m->display, pp->x+CALIBRATION_CROSS_RADIUS/2, pp->y+CALIBRATION_CROSS_RADIUS, pp->x+CALIBRATION_CROSS_RADIUS, pp->y+CALIBRATION_CROSS_RADIUS, CALIBRATION_CROSS_COLOR2);
		gdispGDrawLine(m->display, pp->x+CALIBRATION_CROSS_RADIUS, pp->y+CALIBRATION_CROSS_RADIUS/2, pp->x+CALIBRATION_CROSS_RADIUS, pp->y+CALIBRATION_CROSS_RADIUS, CALIBRATION_CROSS_COLOR2);
		gdispGDrawLine(m->display, pp->x+CALIBRATION_CROSS_RADIUS/2, pp->y-CALIBRATION_CROSS_RADIUS, pp->x+CALIBRATION_CROSS_RADIUS, pp->y-CALIBRATION_CROSS_RADIUS, CALIBRATION_CROSS_COLOR2);
		gdispGDrawLine(m->display, pp->x+CALIBRATION_CROSS_RADIUS, pp->y-CALIBRATION_CROSS_RADIUS, pp->x+CALIBRATION_CROSS_RADIUS, pp->y-CALIBRATION_CROSS_RADIUS/2, CALIBRATION_CROSS_COLOR2);
	}

	static GFXINLINE void CalibrationCrossClear(GMouse *m, const point *pp) {
		gdispGFillArea(m->display, pp->x - CALIBRATION_CROSS_RADIUS, pp->y - CALIBRATION_CROSS_RADIUS, CALIBRATION_CROSS_RADIUS*2+1, CALIBRATION_CROSS_RADIUS*2+1, CALIBRATION_BACKGROUND);
	}

	static GFXINLINE void CalibrationCalculate(GMouse *m, const point *cross, const point *points) {
		float		dx;
		coord_t		c0, c1, c2;
		(void)		m;

		// Work on x values
		c0 = cross[0].x;
		c1 = cross[1].x;
		c2 = cross[2].x;

		#if GDISP_NEED_CONTROL
			if (!(gmvmt(m)->d.flags & GMOUSE_VFLG_SELFROTATION)) {
				/* Convert all cross points back to GDISP_ROTATE_0 convention
				 * before calculating the calibration matrix.
				 */
				switch(gdispGGetOrientation(m->display)) {
				case GDISP_ROTATE_90:
					c0 = cross[0].y;
					c1 = cross[1].y;
					c2 = cross[2].y;
					break;
				case GDISP_ROTATE_180:
					c0 = c1 = c2 = gdispGGetWidth(m->display) - 1;
					c0 -= cross[0].x;
					c1 -= cross[1].x;
					c2 -= cross[2].x;
					break;
				case GDISP_ROTATE_270:
					c0 = c1 = c2 = gdispGGetHeight(m->display) - 1;
					c0 -= cross[0].y;
					c1 -= cross[1].y;
					c2 -= cross[2].y;
					break;
                default:
                    break;
				}
			}
		#endif

		/* Compute all the required determinants */
		dx  = (float)(points[0].x - points[2].x) * (float)(points[1].y - points[2].y)
				- (float)(points[1].x - points[2].x) * (float)(points[0].y - points[2].y);

		m->caldata.ax = ((float)(c0 - c2) * (float)(points[1].y - points[2].y)
							- (float)(c1 - c2) * (float)(points[0].y - points[2].y)) / dx;
		m->caldata.bx = ((float)(c1 - c2) * (float)(points[0].x - points[2].x)
							- (float)(c0 - c2) * (float)(points[1].x - points[2].x)) / dx;
		m->caldata.cx = (c0 * ((float)points[1].x * (float)points[2].y - (float)points[2].x * (float)points[1].y)
							- c1 * ((float)points[0].x * (float)points[2].y - (float)points[2].x * (float)points[0].y)
							+ c2 * ((float)points[0].x * (float)points[1].y - (float)points[1].x * (float)points[0].y)) / dx;

		// Work on y values
		c0 = cross[0].y;
		c1 = cross[1].y;
		c2 = cross[2].y;

		#if GDISP_NEED_CONTROL
			if (!(gmvmt(m)->d.flags & GMOUSE_VFLG_SELFROTATION)) {
				switch(gdispGGetOrientation(m->display)) {
				case GDISP_ROTATE_90:
					c0 = c1 = c2 = gdispGGetWidth(m->display) - 1;
					c0 -= cross[0].x;
					c1 -= cross[1].x;
					c2 -= cross[2].x;
					break;
				case GDISP_ROTATE_180:
					c0 = c1 = c2 = gdispGGetHeight(m->display) - 1;
					c0 -= cross[0].y;
					c1 -= cross[1].y;
					c2 -= cross[2].y;
					break;
				case GDISP_ROTATE_270:
					c0 = cross[0].x;
					c1 = cross[1].x;
					c2 = cross[2].x;
					break;
                default:
                    break;
				}
			}
		#endif

		m->caldata.ay = ((float)(c0 - c2) * (float)(points[1].y - points[2].y)
							- (float)(c1 - c2) * (float)(points[0].y - points[2].y)) / dx;
		m->caldata.by = ((float)(c1 - c2) * (float)(points[0].x - points[2].x)
							- (float)(c0 - c2) * (float)(points[1].x - points[2].x)) / dx;
		m->caldata.cy = (c0 * ((float)points[1].x * (float)points[2].y - (float)points[2].x * (float)points[1].y)
							- c1 * ((float)points[0].x * (float)points[2].y - (float)points[2].x * (float)points[0].y)
							+ c2 * ((float)points[0].x * (float)points[1].y - (float)points[1].x * (float)points[0].y)) / dx;
	}

	static uint32_t CalibrateMouse(GMouse *m) {
		coord_t		w, h;
		point		cross[4];		// The locations of the test points on the display
		point		points[4];		// The x, y readings obtained from the mouse for each test point
		uint32_t	err;
		#if GDISP_NEED_TEXT
			font_t		font1, font2;
		#endif

		#if GDISP_NEED_TEXT
			font1 = gdispOpenFont(CALIBRATION_FONT);
			if (!font1) font1 = gdispOpenFont("*");
			font2 = gdispOpenFont(CALIBRATION_FONT2);
			if (!font2) font2 = gdispOpenFont("*");
		#endif
		err = 0;
		w  =  gdispGGetWidth(m->display);
		h  =  gdispGGetHeight(m->display);
		#if GDISP_NEED_CLIP
			gdispGSetClip(m->display, 0, 0, w, h);
		#endif

		// Ensure we get minimally processed readings for the calibration
		m->flags |= GMOUSE_FLG_IN_CAL;

		// Set up our calibration locations
        if ((gmvmt(m)->d.flags & GMOUSE_VFLG_CAL_EXTREMES)) {
            cross[0].x = 0;		cross[0].y = 0;
            cross[1].x = w-1;	cross[1].y = 0;
            cross[2].x = w-1;	cross[2].y = h-1;
            cross[3].x = w/2;	cross[3].y = h/2;
        } else {
            cross[0].x = w/4;	cross[0].y = h/4;
            cross[1].x = w-w/4;	cross[1].y = h/4;
            cross[2].x = w-w/4;	cross[2].y = h-h/4;
            cross[3].x = w/2;	cross[3].y = h/2;
        }

		// Set up the calibration display
		gdispGClear(m->display, Blue);
		#if GDISP_NEED_TEXT
			gdispGFillStringBox(m->display,
								0, CALIBRATION_TITLE_Y, w, CALIBRATION_TITLE_HEIGHT,
								CALIBRATION_TITLE, font1,  CALIBRATION_TITLE_COLOR, CALIBRATION_TITLE_BACKGROUND,
								justifyCenter);
		#endif

		// Calculate the calibration
		{
			unsigned	i, maxpoints;

			maxpoints = (gmvmt(m)->d.flags & GMOUSE_VFLG_CAL_TEST) ? 4 : 3;

			// Loop through the calibration points
			for(i = 0; i < maxpoints; i++) {
				int32_t		px, py;
				unsigned	j;

				// Draw the current calibration point
				CalibrationCrossDraw(m, &cross[i]);

				// Get a valid "point pressed" average reading
				do {
					// Wait for the mouse to be pressed
					while(!(m->r.buttons & GINPUT_MOUSE_BTN_LEFT))
						gfxSleepMilliseconds(CALIBRATION_POLL_PERIOD);

					// Sum samples taken every CALIBRATION_POLL_PERIOD milliseconds while the mouse is down
					px = py = j = 0;
					while((m->r.buttons & GINPUT_MOUSE_BTN_LEFT)) {
						// Limit sampling period to prevent overflow
						if (j < CALIBRATION_MAXPRESS_PERIOD/CALIBRATION_POLL_PERIOD) {
							px += m->r.x;
							py += m->r.y;
							j++;
						}
						gfxSleepMilliseconds(CALIBRATION_POLL_PERIOD);
					}

					// Ignore presses less than CALIBRATION_MINPRESS_PERIOD milliseconds
				} while(j < CALIBRATION_MINPRESS_PERIOD/CALIBRATION_POLL_PERIOD);
				points[i].x = px / j;
				points[i].y = py / j;

				// Clear the current calibration point
				CalibrationCrossClear(m, &cross[i]);
			}
		}

		// Apply 3 point calibration algorithm
		CalibrationCalculate(m, cross, points);

		 /* Verification of correctness of calibration (optional) :
		 *  See if the 4th point (Middle of the screen) coincides with the calibrated
		 *  result. If point is within +/- Squareroot(ERROR) pixel margin, then successful calibration
		 *  Else return the error.
		 */
		if ((gmvmt(m)->d.flags & GMOUSE_VFLG_CAL_TEST)) {
			const GMouseJitter	*pj;

			// Are we in pen or finger mode
			pj = (m->flags & GMOUSE_FLG_FINGERMODE) ? &gmvmt(m)->finger_jitter : &gmvmt(m)->pen_jitter;

			// Transform the co-ordinates
			CalibrationTransform((GMouseReading *)&points[3], &m->caldata);

			// Do we need to rotate the reading to match the display
			#if GDISP_NEED_CONTROL
				if (!(gmvmt(m)->d.flags & GMOUSE_VFLG_SELFROTATION)) {
					coord_t		t;

					switch(gdispGGetOrientation(m->display)) {
						case GDISP_ROTATE_0:
							break;
						case GDISP_ROTATE_90:
							t = points[3].x;
							points[3].x = w - 1 - points[3].y;
							points[3].y = t;
							break;
						case GDISP_ROTATE_180:
							points[3].x = w - 1 - points[3].x;
							points[3].y = h - 1 - points[3].y;
							break;
						case GDISP_ROTATE_270:
							t = points[3].y;
							points[3].y = h - 1 - points[3].x;
							points[3].x = t;
							break;
						default:
							break;
					}
				}
			#endif

			// Is this accurate enough?
			err = (points[3].x - cross[3].x) * (points[3].x - cross[3].x) + (points[3].y - cross[3].y) * (points[3].y - cross[3].y);
			if (err > (uint32_t)pj->calibrate * (uint32_t)pj->calibrate) {
				#if GDISP_NEED_TEXT
					// No - Display error and return
					gdispGFillStringBox(m->display,
											0, CALIBRATION_ERROR_Y, w, CALIBRATION_ERROR_HEIGHT,
											CALIBRATION_ERROR_TEXT, font2,  CALIBRATION_ERROR_COLOR, CALIBRATION_ERROR_BACKGROUND,
											justifyCenter);
					gfxSleepMilliseconds(CALIBRATION_ERROR_DELAY);
				#endif
			} else
				err = 0;
		}

		// We are done calibrating
		#if GDISP_NEED_TEXT
			gdispCloseFont(font1);
			gdispCloseFont(font2);
		#endif
		m->flags &= ~GMOUSE_FLG_IN_CAL;
		m->flags |= GMOUSE_FLG_CLIP;

		// Save the calibration data (if possible)
		if (!err) {
			m->flags |= GMOUSE_FLG_CALIBRATE;

			#if GINPUT_TOUCH_USER_CALIBRATION_SAVE
				SaveMouseCalibration(gdriverGetDriverInstanceNumber((GDriver *)m), &m->caldata, sizeof(GMouseCalibration));
			#endif
			if (gmvmt(m)->calsave)
				gmvmt(m)->calsave(m, &m->caldata, sizeof(GMouseCalibration));
		}

		// Force an initial reading
		m->r.buttons = 0;
		GetMouseReading(m);

		// Clear the screen using the GWIN default background color
		#if GFX_USE_GWIN
			gdispGClear(m->display, gwinGetDefaultBgColor());
		#else
			gdispGClear(m->display, GDISP_STARTUP_COLOR);
		#endif

		return err;
	}
#endif

void _gmouseInit(void) {
	// GINPUT_MOUSE_DRIVER_LIST is defined - create each driver instance
	#if defined(GINPUT_MOUSE_DRIVER_LIST)
		{
			int		i;
			typedef const GMouseVMT const GMOUSEVMTLIST[1];

			extern GMOUSEVMTLIST GINPUT_MOUSE_DRIVER_LIST;
			static const GMouseVMT * const dclist[] = {GINPUT_MOUSE_DRIVER_LIST};

			for(i = 0; i < sizeof(dclist)/sizeof(dclist[0]); i++) {
                if (!(dclist[i]->d.flags & GMOUSE_VFLG_DYNAMICONLY))
					gdriverRegister(&dclist[i]->d, GDISP);
			}
		}

	// One and only one mouse
	#else
		{
			extern const GMouseVMT const GMOUSEVMT_OnlyOne[1];

            if (!(GMOUSEVMT_OnlyOne->d.flags & GMOUSE_VFLG_DYNAMICONLY))
					gdriverRegister(&GMOUSEVMT_OnlyOne->d, GDISP);
		}
	#endif

}

void _gmouseDeinit(void) {
	gtimerDeinit(&MouseTimer);
}

bool_t _gmouseInitDriver(GDriver *g, void *display, unsigned driverinstance, unsigned systeminstance) {
    #define m   ((GMouse *)g)
    (void) systeminstance;

	// The initial display is passed in the parameter for mice
	m->display = display;

	#if !GINPUT_TOUCH_NOTOUCH
		// Should this mouse start in finger mode? (according to the VMT)
		if ((gmvmt(m)->d.flags & GMOUSE_VFLG_DEFAULTFINGER))
			m->flags |= GMOUSE_FLG_FINGERMODE;
	#endif

	// Init the mouse
    if (!gmvmt(m)->init((GMouse *)g, driverinstance))
        return FALSE;

	// Ensure the Poll timer is started
	if (!gtimerIsActive(&MouseTimer))
		gtimerStart(&MouseTimer, MousePoll, 0, TRUE, GINPUT_MOUSE_POLL_PERIOD);

    return TRUE;

    #undef m
}

void _gmousePostInitDriver(GDriver *g) {
    #define     m   ((GMouse *)g)

	#if !GINPUT_TOUCH_STARTRAW
		m->flags |= GMOUSE_FLG_CLIP;
	#endif

    #if !GINPUT_TOUCH_NOCALIBRATE && !GINPUT_TOUCH_STARTRAW
        if ((gmvmt(m)->d.flags & GMOUSE_VFLG_CALIBRATE)) {
            #if GINPUT_TOUCH_USER_CALIBRATION_LOAD
                if (LoadMouseCalibration(gdriverGetDriverInstanceNumber((GDriver *)m), &m->caldata, sizeof(GMouseCalibration)))
                    m->flags |= GMOUSE_FLG_CALIBRATE;
                else
            #endif
            if (gmvmt(m)->calload && gmvmt(m)->calload(m, &m->caldata, sizeof(GMouseCalibration)))
                m->flags |= GMOUSE_FLG_CALIBRATE;
			#if !GINPUT_TOUCH_NOCALIBRATE_GUI
				else
					while (CalibrateMouse(m));
			#endif
        }
    #endif

    // Get the first reading
    GetMouseReading(m);

    #undef m
}

void _gmouseDeInitDriver(GDriver *g) {
    (void) g;
}

GSourceHandle ginputGetMouse(unsigned instance) {
	if (instance == GMOUSE_ALL_INSTANCES)
		return (GSourceHandle)&MouseTimer;
    return (GSourceHandle)gdriverGetInstance(GDRIVER_TYPE_MOUSE, instance);
}

void ginputSetMouseDisplay(unsigned instance, GDisplay *g) {
	GMouse *m;

	if (!(m = (GMouse *)gdriverGetInstance(GDRIVER_TYPE_MOUSE, instance)))
		return;

	m->display = g ? g : GDISP;
}

GDisplay *ginputGetMouseDisplay(unsigned instance) {
	GMouse *m;

	if (!(m = (GMouse *)gdriverGetInstance(GDRIVER_TYPE_MOUSE, instance)))
		return 0;

	return m->display;
}

bool_t ginputGetMouseStatus(unsigned instance, GEventMouse *pe) {
	GMouse *m;

	// Win32 threads don't seem to recognise priority and/or pre-emption
	// so we add a sleep here to prevent 100% polled applications from locking up.
	gfxSleepMilliseconds(1);

	if (!(m = (GMouse *)gdriverGetInstance(GDRIVER_TYPE_MOUSE, instance)))
		return FALSE;

	#if !GINPUT_TOUCH_NOCALIBRATE_GUI
		if ((m->flags & GMOUSE_FLG_IN_CAL))
			return FALSE;
	#endif

	#if !GINPUT_TOUCH_NOTOUCH
		pe->type = (gmvmt(m)->d.flags & GMOUSE_VFLG_TOUCH) ? GEVENT_TOUCH : GEVENT_MOUSE;
	#else
		pe->type = GEVENT_MOUSE;
	#endif
	pe->x = m->r.x;
	pe->y = m->r.y;
	pe->z = m->r.z;
	pe->buttons = m->r.buttons;
	pe->display = m->display;
	return TRUE;
}

#if !GINPUT_TOUCH_NOTOUCH
	void ginputSetFingerMode(unsigned instance, bool_t on) {
		GMouse *m;

		if (!(m = (GMouse *)gdriverGetInstance(GDRIVER_TYPE_MOUSE, instance)))
			return;

		if (on)
			m->flags |= GMOUSE_FLG_FINGERMODE;
		else
			m->flags &= ~GMOUSE_FLG_FINGERMODE;

	}
#endif

#if !GINPUT_TOUCH_NOCALIBRATE_GUI
	uint32_t ginputCalibrateMouse(unsigned instance) {
		GMouse *m;

		// Find the instance
		if (!(m = (GMouse *)gdriverGetInstance(GDRIVER_TYPE_MOUSE, instance)))
			return 0;

		// Check it needs calibration
        if (!(gmvmt(m)->d.flags & GMOUSE_VFLG_CALIBRATE))
        	return 0;

		return CalibrateMouse(m);
	}
#endif

/* Wake up the mouse driver from an interrupt service routine (there may be new readings available) */
void _gmouseWakeup(GMouse *m) {
	if (m)
		m->flags |= GMOUSE_FLG_NEEDREAD;
	gtimerJab(&MouseTimer);
}

/* Wake up the mouse driver from an interrupt service routine (there may be new readings available) */
void _gmouseWakeupI(GMouse *m) {
	if (m)
		m->flags |= GMOUSE_FLG_NEEDREAD;
	gtimerJabI(&MouseTimer);
}

#endif /* GFX_USE_GINPUT && GINPUT_NEED_MOUSE */
/** @} */
