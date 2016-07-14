/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_slider.c
 * @brief   GWIN sub-system slider code
 */

#include "../../gfx.h"

#if (GFX_USE_GWIN && GWIN_NEED_SLIDER) || defined(__DOXYGEN__)

#include "gwin_class.h"

// Calculate the slider position from the display position
static int SliderCalcPosFromDPos(GSliderObject *gsw) {
	int		halfbit;

	// Set the new position
	if (gsw->w.g.width < gsw->w.g.height) {
		if (gsw->dpos >= gsw->w.g.height-GWIN_SLIDER_DEAD_BAND)
			return gsw->min;
		if (gsw->dpos < GWIN_SLIDER_DEAD_BAND)
			return gsw->max;
		halfbit = gsw->w.g.height/2-GWIN_SLIDER_DEAD_BAND;
		if (gsw->min > gsw->max)
			halfbit = -halfbit;
		return (((int)(gsw->w.g.height-(GWIN_SLIDER_DEAD_BAND+1)-gsw->dpos))*(gsw->max-gsw->min) + halfbit)/(gsw->w.g.height-(2*GWIN_SLIDER_DEAD_BAND+1)) + gsw->min;
	}
	if (gsw->dpos >= gsw->w.g.width-GWIN_SLIDER_DEAD_BAND)
		return gsw->max;
	if (gsw->dpos < GWIN_SLIDER_DEAD_BAND)
		return gsw->min;
	halfbit = gsw->w.g.width/2-GWIN_SLIDER_DEAD_BAND;
	if (gsw->min > gsw->max)
		halfbit = -halfbit;
	return (((int)gsw->dpos-GWIN_SLIDER_DEAD_BAND)*(gsw->max-gsw->min) + halfbit)/(gsw->w.g.width-(2*GWIN_SLIDER_DEAD_BAND+1)) + gsw->min;
}

// Send the slider event
static void SendSliderEvent(GSliderObject *gsw, uint8_t action) {
	GSourceListener	*	psl;
	GEvent *			pe;
	#define pse			((GEventGWinSlider *)pe)

	// Does this slider want more than just SET events?
	if (action != GSLIDER_EVENT_SET && !(gsw->w.g.flags & GSLIDER_FLG_EXTENDED_EVENTS))
		return;

	psl = 0;
	while ((psl = geventGetSourceListener(GWIDGET_SOURCE, psl))) {
		// Work out which action to send.
		// This precedence order helps provide some protection against missed events.
		// Saving it into srcflags works regardless of if a buffer is available.
		if (psl->srcflags < action)
			psl->srcflags = action;

		// Skip sending if no buffer is available
		if (!(pe = geventGetEventBuffer(psl)))
			continue;

		// Fill in the event
		pse->type = GEVENT_GWIN_SLIDER;
		pse->gwin = (GHandle)gsw;
		pse->action = psl->srcflags;
		#if GWIN_WIDGET_TAGS
			pse->tag = gsw->w.tag;
		#endif

		// If it is a cancel or set use the defined position else use the calculated position.
		pse->position = pse->action >= GSLIDER_EVENT_CANCEL ? gsw->pos : SliderCalcPosFromDPos(gsw);

		// Cleanup and send.
		psl->srcflags = 0;
		geventSendEvent(psl);
	}

	#undef pse
}

// Reset the display position back to the value predicted by the saved slider position
static void SliderResetDisplayPos(GSliderObject *gsw) {
	if (gsw->w.g.width < gsw->w.g.height)
		gsw->dpos = gsw->w.g.height-1-(gsw->w.g.height-1)*(gsw->pos-gsw->min)/(gsw->max-gsw->min);
	else
		gsw->dpos = (gsw->w.g.width-1)*(gsw->pos-gsw->min)/(gsw->max-gsw->min);
}

#if GINPUT_NEED_MOUSE
	// Set the display position from the mouse position
	static void SetDisplayPosFromMouse(GSliderObject *gsw, coord_t x, coord_t y) {
		if (gsw->w.g.width < gsw->w.g.height)
			gsw->dpos = y < 0 ? 0 : (y >= gsw->w.g.height ? gsw->w.g.height-1 : y);
		else
			gsw->dpos = x < 0 ? 0 : (x >= gsw->w.g.width ? gsw->w.g.width-1 : x);
	}

	// A mouse up event
	static void SliderMouseUp(GWidgetObject *gw, coord_t x, coord_t y) {
		#define gsw		((GSliderObject *)gw)

		#if !GWIN_BUTTON_LAZY_RELEASE
			// Are we over the slider?
			if (x < 0 || x >= gsw->w.g.width || y < 0 || y >= gsw->w.g.height) {
				// No - restore the slider
				SliderResetDisplayPos(gsw);
				_gwinUpdate(&gsw->w.g);
				SendSliderEvent(gsw, GSLIDER_EVENT_CANCEL);
				return;
			}
		#endif

		// Set the new position
		SetDisplayPosFromMouse(gsw, x, y);
		gsw->pos = SliderCalcPosFromDPos(gsw);

		// Update the display
		#if GWIN_SLIDER_NOSNAP
			//only adjust dpos if it equals one of the end values.
			if (gsw->w.g.width < gsw->w.g.height) {
				if (gsw->pos == gsw->min)
					gsw->dpos = gsw->w.g.height-1;
				else if (gsw->pos == gsw->max)
					gsw->dpos = 0;
			} else {
				if (gsw->pos == gsw->max)
					gsw->dpos = gsw->w.g.width-1;
				else if (gsw->pos == gsw->min)
					gsw->dpos = 0;
			}
		#else
			SliderResetDisplayPos(gsw);
		#endif
		_gwinUpdate(&gsw->w.g);

		// Generate the event
		SendSliderEvent(gsw, GSLIDER_EVENT_SET);

		#undef gsw
	}

	// A mouse down event
	static void SliderMouseDown(GWidgetObject *gw, coord_t x, coord_t y) {
		#define gsw		((GSliderObject *)gw)

		// Determine the display position
		SetDisplayPosFromMouse(gsw, x, y);

		// Update the display
		_gwinUpdate(&gsw->w.g);

		// Send the event
		SendSliderEvent(gsw, GSLIDER_EVENT_START);

		#undef gsw
	}

	// A mouse move event
	static void SliderMouseMove(GWidgetObject *gw, coord_t x, coord_t y) {
		#define gsw		((GSliderObject *)gw)

		// Determine the display position
		SetDisplayPosFromMouse(gsw, x, y);

		// Update the display
		_gwinUpdate(&gsw->w.g);

		// Send the event
		SendSliderEvent(gsw, GSLIDER_EVENT_MOVE);

		#undef gsw
	}
#endif

#if GINPUT_NEED_TOGGLE
	// A toggle on has occurred
	static void SliderToggleOn(GWidgetObject *gw, uint16_t role) {
		#define gsw		((GSliderObject *)gw)

		if (role) {
			gwinSliderSetPosition(&gsw->w.g, gsw->pos+(gsw->max-gsw->min)/GWIN_SLIDER_TOGGLE_INC);
			SendSliderEvent(gsw, GSLIDER_EVENT_SET);
		} else {
			gwinSliderSetPosition(&gsw->w.g, gsw->pos-(gsw->max-gsw->min)/GWIN_SLIDER_TOGGLE_INC);
			SendSliderEvent(gsw, GSLIDER_EVENT_SET);
		}
		#undef gsw
	}

	static void SliderToggleAssign(GWidgetObject *gw, uint16_t role, uint16_t instance) {
		if (role)
			((GSliderObject *)gw)->t_up = instance;
		else
			((GSliderObject *)gw)->t_dn = instance;
	}

	static uint16_t SliderToggleGet(GWidgetObject *gw, uint16_t role) {
		return role ? ((GSliderObject *)gw)->t_up : ((GSliderObject *)gw)->t_dn;
	}
#endif

#if GINPUT_NEED_DIAL
	// A dial move event
	static void SliderDialMove(GWidgetObject *gw, uint16_t role, uint16_t value, uint16_t max) {
		#define gsw		((GSliderObject *)gw)
		(void)			role;

		// Set the new position
		gsw->pos = (uint16_t)((uint32_t)value*(gsw->max-gsw->min)/max + gsw->min);

		SliderResetDisplayPos(gsw);
		_gwinUpdate(&gsw->w.g);

		// Generate the event
		SendSliderEvent(gsw, GSLIDER_EVENT_SET);
		#undef gsw
	}

	static void SliderDialAssign(GWidgetObject *gw, uint16_t role, uint16_t instance) {
		(void) role;
		((GSliderObject *)gw)->dial = instance;
	}

	static uint16_t SliderDialGet(GWidgetObject *gw, uint16_t role) {
		(void) role;
		return ((GSliderObject *)gw)->dial;
	}
#endif

// The slider VMT table
static const gwidgetVMT sliderVMT = {
	{
		"Slider",				// The classname
		sizeof(GSliderObject),	// The object size
		_gwidgetDestroy,		// The destroy routine
		_gwidgetRedraw,			// The redraw routine
		0,						// The after-clear routine
	},
	gwinSliderDraw_Std,			// The default drawing routine
	#if GINPUT_NEED_MOUSE
		{
			SliderMouseDown,		// Process mouse down events
			SliderMouseUp,			// Process mouse up events
			SliderMouseMove,		// Process mouse move events
		},
	#endif
	#if GINPUT_NEED_KEYBOARD || GWIN_NEED_KEYBOARD
		{
			0						// Process keyboard events
		},
	#endif
	#if GINPUT_NEED_TOGGLE
		{
			2,						// 1 toggle role
			SliderToggleAssign,		// Assign Toggles
			SliderToggleGet,		// Get Toggles
			0,						// Process toggle off events (NOT USED)
			SliderToggleOn,			// Process toggle on events
		},
	#endif
	#if GINPUT_NEED_DIAL
		{
			1,						// 1 dial roles
			SliderDialAssign,		// Assign Dials
			SliderDialGet,			// Get Dials
			SliderDialMove,			// Process dial move events
		},
	#endif
};

GHandle gwinGSliderCreate(GDisplay *g, GSliderObject *gs, const GWidgetInit *pInit) {
	if (!(gs = (GSliderObject *)_gwidgetCreate(g, &gs->w, pInit, &sliderVMT)))
		return 0;
	#if GINPUT_NEED_TOGGLE
		gs->t_dn = GWIDGET_NO_INSTANCE;
		gs->t_up = GWIDGET_NO_INSTANCE;
	#endif
	#if GINPUT_NEED_DIAL
		gs->dial = GWIDGET_NO_INSTANCE;
	#endif
	gs->min = 0;
	gs->max = 100;
	gs->pos = 0;
	SliderResetDisplayPos(gs);
	gwinSetVisible((GHandle)gs, pInit->g.show);
	return (GHandle)gs;
}

void gwinSliderSetRange(GHandle gh, int min, int max) {
	#define gsw		((GSliderObject *)gh)

	if (gh->vmt != (gwinVMT *)&sliderVMT)
		return;

	if (min == max)		// prevent divide by 0 errors.
		max++;
	gsw->min = min;
	gsw->max = max;
	gsw->pos = min;
	SliderResetDisplayPos(gsw);
	#undef gsw
}

void gwinSliderSetPosition(GHandle gh, int pos) {
	#define gsw		((GSliderObject *)gh)

	if (gh->vmt != (gwinVMT *)&sliderVMT)
		return;

	if (gsw->min <= gsw->max) {
		if (pos < gsw->min) gsw->pos = gsw->min;
		else if (pos > gsw->max) gsw->pos = gsw->max;
		else gsw->pos = pos;
	} else {
		if (pos > gsw->min) gsw->pos = gsw->min;
		else if (pos < gsw->max) gsw->pos = gsw->max;
		else gsw->pos = pos;
	}
	SliderResetDisplayPos(gsw);
	_gwinUpdate(gh);

	#undef gsw
}

void gwinSliderSendExtendedEvents(GHandle gh, bool_t enabled) {
	if (gh->vmt != (gwinVMT *)&sliderVMT)
		return;

	if (enabled)
		gh->flags |= GSLIDER_FLG_EXTENDED_EVENTS;
	else
		gh->flags &= ~GSLIDER_FLG_EXTENDED_EVENTS;
}

/*----------------------------------------------------------
 * Custom Draw Routines
 *----------------------------------------------------------*/

void gwinSliderDraw_Std(GWidgetObject *gw, void *param) {
	#define gsw			((GSliderObject *)gw)
	const GColorSet *	pcol;
	(void)				param;

	if (gw->g.vmt != (gwinVMT *)&sliderVMT)
		return;

	if ((gw->g.flags & GWIN_FLG_SYSENABLED))
		pcol = &gw->pstyle->enabled;
	else
		pcol = &gw->pstyle->disabled;

	// Vertical slider
	if (gw->g.width < gw->g.height) {
		if (gsw->dpos != gw->g.height-1)
			gdispGFillArea(gw->g.display, gw->g.x, gw->g.y+gsw->dpos, gw->g.width, gw->g.height - gsw->dpos, pcol->progress);		// Active area
		if (gsw->dpos != 0)
			gdispGFillArea(gw->g.display, gw->g.x, gw->g.y, gw->g.width, gsw->dpos, pcol->fill);									// Inactive area
		gdispGDrawBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width, gw->g.height, pcol->edge);										// Edge
		gdispGDrawLine(gw->g.display, gw->g.x, gw->g.y+gsw->dpos, gw->g.x+gw->g.width-1, gw->g.y+gsw->dpos, pcol->edge);			// Thumb
		if (gsw->dpos >= 2)
			gdispGDrawLine(gw->g.display, gw->g.x, gw->g.y+gsw->dpos-2, gw->g.x+gw->g.width-1, gw->g.y+gsw->dpos-2, pcol->edge);	// Thumb
		if (gsw->dpos <= gw->g.height-2)
			gdispGDrawLine(gw->g.display, gw->g.x, gw->g.y+gsw->dpos+2, gw->g.x+gw->g.width-1, gw->g.y+gsw->dpos+2, pcol->edge);	// Thumb

	// Horizontal slider
	} else {
		if (gsw->dpos != gw->g.width-1)
			gdispGFillArea(gw->g.display, gw->g.x+gsw->dpos, gw->g.y, gw->g.width-gsw->dpos, gw->g.height, pcol->fill);				// Inactive area
		if (gsw->dpos != 0)
			gdispGFillArea(gw->g.display, gw->g.x, gw->g.y, gsw->dpos, gw->g.height, pcol->progress);								// Active area
		gdispGDrawBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width, gw->g.height, pcol->edge);										// Edge
		gdispGDrawLine(gw->g.display, gw->g.x+gsw->dpos, gw->g.y, gw->g.x+gsw->dpos, gw->g.y+gw->g.height-1, pcol->edge);			// Thumb
		if (gsw->dpos >= 2)
			gdispGDrawLine(gw->g.display, gw->g.x+gsw->dpos-2, gw->g.y, gw->g.x+gsw->dpos-2, gw->g.y+gw->g.height-1, pcol->edge);	// Thumb
		if (gsw->dpos <= gw->g.width-2)
			gdispGDrawLine(gw->g.display, gw->g.x+gsw->dpos+2, gw->g.y, gw->g.x+gsw->dpos+2, gw->g.y+gw->g.height-1, pcol->edge);	// Thumb
	}

	// Draw the string
	gdispGDrawStringBox(gw->g.display, gw->g.x+1, gw->g.y+1, gw->g.width-2, gw->g.height-2, gw->text, gw->g.font, pcol->text, justifyCenter);

	#undef gsw
}

#if GDISP_NEED_IMAGE
void gwinSliderDraw_Image(GWidgetObject *gw, void *param) {
	#define gsw			((GSliderObject *)gw)
	#define gi			((gdispImage *)param)
	const GColorSet *	pcol;
	coord_t				z, v;

	if (gw->g.vmt != (gwinVMT *)&sliderVMT)
		return;

	if ((gw->g.flags & GWIN_FLG_SYSENABLED))
		pcol = &gw->pstyle->enabled;
	else
		pcol = &gw->pstyle->disabled;

	if (gw->g.width < gw->g.height) {			// Vertical slider
		if (gsw->dpos != 0)							// The unfilled area
			gdispGFillArea(gw->g.display, gw->g.x+1, gw->g.y+1, gw->g.width-2, gsw->dpos-1, gw->pstyle->enabled.progress);	// Inactive area
		if (gsw->dpos != gw->g.height-1) {			// The filled area
			for(z=gw->g.height, v=gi->height; z > gsw->dpos;) {
				z -= v;
				if (z < gsw->dpos) {
					v -= gsw->dpos - z;
					z = gsw->dpos;
				}
				gdispGImageDraw(gw->g.display, gi, gw->g.x+1, gw->g.y+z+1, gw->g.width-1, v-2, 0, gi->height-v);
			}
		}
		gdispGDrawBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width, gw->g.height, pcol->edge);								// Edge
		gdispGDrawLine(gw->g.display, gw->g.x+1, gw->g.y+gsw->dpos, gw->g.x+gw->g.width-2, gw->g.y+gsw->dpos, pcol->edge);	// Thumb

	// Horizontal slider
	} else {
		if (gsw->dpos != gw->g.width-1)				// The unfilled area
			gdispGFillArea(gw->g.display, gw->g.x+gsw->dpos+1, gw->g.y+1, gw->g.width-gsw->dpos-2, gw->g.height-2, gw->pstyle->enabled.progress);	// Inactive area
		if (gsw->dpos != 0) {						// The filled area
			for(z=0, v=gi->width; z < gsw->dpos; z += v) {
				if (z+v > gsw->dpos)
					v -= z+v - gsw->dpos;
				gdispGImageDraw(gw->g.display, gi, gw->g.x+z+1, gw->g.y+1, v-1, gw->g.height-2, 0, 0);
			}
		}
		gdispGDrawBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width, gw->g.height, pcol->edge);								// Edge
		gdispGDrawLine(gw->g.display, gw->g.x+gsw->dpos, gw->g.y+1, gw->g.x+gsw->dpos, gw->g.y+gw->g.height-2, pcol->edge);	// Thumb
	}
	gdispGDrawStringBox(gw->g.display, gw->g.x+1, gw->g.y+1, gw->g.width-2, gw->g.height-2, gw->text, gw->g.font, pcol->text, justifyCenter);

	#undef gsw
	#undef gi
}
#endif /* GDISP_NEED_IMAGE */

#endif /* GFX_USE_GWIN && GWIN_NEED_BUTTON */
