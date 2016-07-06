/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_radio.c
 * @brief   GWIN sub-system radio button code
 */

#include "../../gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_RADIO

#include "gwin_class.h"

#define GRADIO_TAB_CNR			8		// Diagonal corner on active tab
#define GRADIO_TOP_FADE			50		// (GRADIO_TOP_FADE/255)% fade to white for top of tab/button
#define GRADIO_BOTTOM_FADE		25		// (GRADIO_BOTTOM_FADE/255)% fade to black for bottom of tab/button
#define GRADIO_OUTLINE_FADE		128		// (GRADIO_OUTLINE_FADE/255)% fade to background for active tab edge

// Send the button event
static void SendRadioEvent(GWidgetObject *gw) {
	GSourceListener	*	psl;
	GEvent *			pe;
	#define pbe			((GEventGWinRadio *)pe)

	// Trigger a GWIN Button Event
	psl = 0;
	while ((psl = geventGetSourceListener(GWIDGET_SOURCE, psl))) {
		if (!(pe = geventGetEventBuffer(psl)))
			continue;
		pbe->type = GEVENT_GWIN_RADIO;
		pbe->gwin = (GHandle)gw;
		pbe->group = ((GRadioObject *)gw)->group;
		#if GWIN_WIDGET_TAGS
			pbe->tag = gw->tag;
		#endif
		geventSendEvent(psl);
	}

	#undef pbe
}

#if GINPUT_NEED_MOUSE
	// A mouse down has occurred over the button
	static void RadioMouseDown(GWidgetObject *gw, coord_t x, coord_t y) {
		(void) x; (void) y;

		gwinRadioPress((GHandle)gw);
	}
#endif

#if GINPUT_NEED_TOGGLE
	// A toggle on has occurred
	static void RadioToggleOn(GWidgetObject *gw, uint16_t role) {
		(void) role;

		gwinRadioPress((GHandle)gw);
	}

	static void RadioToggleAssign(GWidgetObject *gw, uint16_t role, uint16_t instance) {
		(void) role;
		((GRadioObject *)gw)->toggle = instance;
	}

	static uint16_t RadioToggleGet(GWidgetObject *gw, uint16_t role) {
		(void) role;
		return ((GRadioObject *)gw)->toggle;
	}
#endif

// The radio button VMT table
static const gwidgetVMT radioVMT = {
	{
		"Radio",				// The classname
		sizeof(GRadioObject),	// The object size
		_gwidgetDestroy,		// The destroy routine
		_gwidgetRedraw,			// The redraw routine
		0,						// The after-clear routine
	},
	gwinRadioDraw_Radio,		// The default drawing routine
	#if GINPUT_NEED_MOUSE
		{
			RadioMouseDown,			// Process mouse down events
			0,						// Process mouse up events (NOT USED)
			0,						// Process mouse move events (NOT USED)
		},
	#endif
	#if GINPUT_NEED_KEYBOARD || GWIN_NEED_KEYBOARD
		{
			0						// Process keyboard events
		},
	#endif
	#if GINPUT_NEED_TOGGLE
		{
			1,						// 1 toggle role
			RadioToggleAssign,		// Assign Toggles
			RadioToggleGet,			// Get Toggles
			0,						// Process toggle off events (NOT USED)
			RadioToggleOn,				// Process toggle on events
		},
	#endif
	#if GINPUT_NEED_DIAL
		{
			0,						// No dial roles
			0,						// Assign Dials (NOT USED)
			0,						// Get Dials (NOT USED)
			0,						// Process dial move events (NOT USED)
		},
	#endif
};

GHandle gwinGRadioCreate(GDisplay *g, GRadioObject *gw, const GWidgetInit *pInit, uint16_t group) {
	if (!(gw = (GRadioObject *)_gwidgetCreate(g, &gw->w, pInit, &radioVMT)))
		return 0;

	#if GINPUT_NEED_TOGGLE
		gw->toggle = GWIDGET_NO_INSTANCE;
	#endif
	gw->group = group;
	gwinSetVisible((GHandle)gw, pInit->g.show);
	return (GHandle)gw;
}

void gwinRadioPress(GHandle gh) {
	GHandle		gx;

	if (gh->vmt != (gwinVMT *)&radioVMT || (gh->flags & GRADIO_FLG_PRESSED))
		return;

	if ((gx = gwinRadioGetActive(((GRadioObject *)gh)->group))) {
		gx->flags &= ~GRADIO_FLG_PRESSED;
		_gwinUpdate(gx);
	}
	gh->flags |= GRADIO_FLG_PRESSED;
	_gwinUpdate(gh);
	SendRadioEvent((GWidgetObject *)gh);
}

bool_t gwinRadioIsPressed(GHandle gh) {
	if (gh->vmt != (gwinVMT *)&radioVMT)
		return FALSE;

	return (gh->flags & GRADIO_FLG_PRESSED) ? TRUE : FALSE;
}

GHandle gwinRadioGetActive(uint16_t group) {
	GHandle		gh;

	for(gh = gwinGetNextWindow(0); gh; gh = gwinGetNextWindow(gh)) {
		if (gh->vmt == (gwinVMT *)&radioVMT && ((GRadioObject *)gh)->group == group && (gh->flags & GRADIO_FLG_PRESSED))
			return gh;
	}
	return 0;
}

/*----------------------------------------------------------
 * Custom Draw Routines
 *----------------------------------------------------------*/

static const GColorSet *getDrawColors(GWidgetObject *gw) {
	if (!(gw->g.flags & GWIN_FLG_SYSENABLED))	return &gw->pstyle->disabled;
	if ((gw->g.flags & GRADIO_FLG_PRESSED))	return &gw->pstyle->pressed;
	return &gw->pstyle->enabled;
}

void gwinRadioDraw_Radio(GWidgetObject *gw, void *param) {
	#define gcw			((GRadioObject *)gw)
	coord_t				ld, df;
	const GColorSet *	pcol;
	(void)				param;

	if (gw->g.vmt != (gwinVMT *)&radioVMT) return;
	pcol = getDrawColors(gw);

	ld = gw->g.width < gw->g.height ? gw->g.width : gw->g.height;

	#if GDISP_NEED_CIRCLE
		df = (ld-1)/2;
		gdispGFillArea(gw->g.display, gw->g.x, gw->g.y, ld, ld, gw->pstyle->background);
		gdispGDrawCircle(gw->g.display, gw->g.x+df, gw->g.y+df, df, pcol->edge);

		if (gw->g.flags & GRADIO_FLG_PRESSED)
			gdispGFillCircle(gw->g.display, gw->g.x+df, gw->g.y+df, df <= 2 ? 1 : (df-2), pcol->fill);
	#else
		gdispGFillArea(gw->g.display, gw->g.x+1, gw->g.y+1, ld, ld-2, gw->pstyle->background);
		gdispGDrawBox(gw->g.display, gw->g.x, gw->g.y, ld, ld, pcol->edge);

		df = ld < 4 ? 1 : 2;
		if (gw->g.flags & GRADIO_FLG_PRESSED)
			gdispGFillArea(gw->g.display, gw->g.x+df, gw->g.y+df, ld-2*df, ld-2*df, pcol->fill);
	#endif

	gdispGFillStringBox(gw->g.display, gw->g.x+ld+1, gw->g.y, gw->g.width-ld-1, gw->g.height, gw->text, gw->g.font, pcol->text, gw->pstyle->background, justifyLeft);
	#undef gcw
}

#if GWIN_FLAT_STYLING
	void gwinRadioDraw_Button(GWidgetObject *gw, void *param) {
		const GColorSet *	pcol;
		(void)				param;

		if (gw->g.vmt != (gwinVMT *)&radioVMT) return;
		pcol = getDrawColors(gw);

		#if GWIN_NEED_FLASHING
			// Flash only the on state.
			pcol = _gwinGetFlashedColor(gw, pcol, FALSE);
		#endif

		gdispGFillStringBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width-1, gw->g.height-1, gw->text, gw->g.font, pcol->text, pcol->fill, justifyCenter);
		gdispGDrawLine(gw->g.display, gw->g.x+gw->g.width-1, gw->g.y, gw->g.x+gw->g.width-1, gw->g.y+gw->g.height-1, pcol->edge);
		gdispGDrawLine(gw->g.display, gw->g.x, gw->g.y+gw->g.height-1, gw->g.x+gw->g.width-2, gw->g.y+gw->g.height-1, pcol->edge);
	}
	void gwinRadioDraw_Tab(GWidgetObject *gw, void *param) {
		const GColorSet *	pcol;
		(void)				param;

		if (gw->g.vmt != (gwinVMT *)&radioVMT)	return;
		pcol = getDrawColors(gw);

		#if GWIN_NEED_FLASHING
			// Flash only the on state.
			pcol = _gwinGetFlashedColor(gw, pcol, FALSE);
		#endif

		if ((gw->g.flags & GRADIO_FLG_PRESSED)) {
			gdispGDrawBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width, gw->g.height, pcol->edge);
			gdispGFillStringBox(gw->g.display, gw->g.x+1, gw->g.y+1, gw->g.width-2, gw->g.height-1, gw->text, gw->g.font, pcol->text, pcol->fill, justifyCenter);
		} else {
			gdispGFillStringBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width-1, gw->g.height-1, gw->text, gw->g.font, pcol->text, pcol->fill, justifyCenter);
			gdispGDrawLine(gw->g.display, gw->g.x+gw->g.width-1, gw->g.y, gw->g.x+gw->g.width-1, gw->g.y+gw->g.height-1, pcol->edge);
			gdispGDrawLine(gw->g.display, gw->g.x, gw->g.y+gw->g.height-1, gw->g.x+gw->g.width-2, gw->g.y+gw->g.height-1, pcol->edge);
		}
	}
#else
	void gwinRadioDraw_Button(GWidgetObject *gw, void *param) {
		const GColorSet *	pcol;
		fixed				alpha;
		fixed				dalpha;
		coord_t				i;
		color_t				tcol, bcol;
		(void)				param;

		if (gw->g.vmt != (gwinVMT *)&radioVMT) return;
		pcol = getDrawColors(gw);
	
		/* Fill the box blended from variants of the fill color */
		tcol = gdispBlendColor(White, pcol->fill, GRADIO_TOP_FADE);
		bcol = gdispBlendColor(Black, pcol->fill, GRADIO_BOTTOM_FADE);
		dalpha = FIXED(255)/gw->g.height;
		for(alpha = 0, i = 0; i < gw->g.height; i++, alpha += dalpha)
			gdispGDrawLine(gw->g.display, gw->g.x, gw->g.y+i, gw->g.x+gw->g.width-2, gw->g.y+i, gdispBlendColor(bcol, tcol, NONFIXED(alpha)));

		gdispGDrawStringBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width-1, gw->g.height-1, gw->text, gw->g.font, pcol->text, justifyCenter);
		gdispGDrawLine(gw->g.display, gw->g.x+gw->g.width-1, gw->g.y, gw->g.x+gw->g.width-1, gw->g.y+gw->g.height-1, pcol->edge);
		gdispGDrawLine(gw->g.display, gw->g.x, gw->g.y+gw->g.height-1, gw->g.x+gw->g.width-2, gw->g.y+gw->g.height-1, pcol->edge);
	}
	void gwinRadioDraw_Tab(GWidgetObject *gw, void *param) {
		const GColorSet *	pcol;
		fixed				alpha;
		fixed				dalpha;
		coord_t				i;
		color_t				tcol, bcol;
		(void)				param;

		if (gw->g.vmt != (gwinVMT *)&radioVMT)	return;
		pcol = getDrawColors(gw);

		if ((gw->g.flags & GRADIO_FLG_PRESSED)) {
			tcol = gdispBlendColor(pcol->edge, gw->pstyle->background, GRADIO_OUTLINE_FADE);
			gdispGFillStringBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width, gw->g.height, gw->text, gw->g.font, pcol->text, gw->g.bgcolor, justifyCenter);
			gdispGDrawLine(gw->g.display, gw->g.x, gw->g.y, gw->g.x+gw->g.width-(GRADIO_TAB_CNR+1), gw->g.y, tcol);
			gdispGDrawLine(gw->g.display, gw->g.x+gw->g.width-(GRADIO_TAB_CNR+1), gw->g.y, gw->g.x+gw->g.width-1, gw->g.y+GRADIO_TAB_CNR, tcol);
			gdispGDrawLine(gw->g.display, gw->g.x+gw->g.width-1, gw->g.y+GRADIO_TAB_CNR, gw->g.x+gw->g.width-1, gw->g.y+gw->g.height-1, tcol);
		} else {
			/* Fill the box blended from variants of the fill color */
			tcol = gdispBlendColor(White, pcol->fill, GRADIO_TOP_FADE);
			bcol = gdispBlendColor(Black, pcol->fill, GRADIO_BOTTOM_FADE);
			dalpha = FIXED(255)/gw->g.height;
			for(alpha = 0, i = 0; i < gw->g.height; i++, alpha += dalpha)
				gdispGDrawLine(gw->g.display, gw->g.x, gw->g.y+i, gw->g.x+gw->g.width-2, gw->g.y+i, gdispBlendColor(bcol, tcol, NONFIXED(alpha)));
			gdispGDrawLine(gw->g.display, gw->g.x+gw->g.width-1, gw->g.y, gw->g.x+gw->g.width-1, gw->g.y+gw->g.height-1, pcol->edge);
			gdispGDrawStringBox(gw->g.display, gw->g.x+1, gw->g.y+1, gw->g.width-2, gw->g.height-2, gw->text, gw->g.font, pcol->text, justifyCenter);
		}
	}
#endif

#endif /* GFX_USE_GWIN && GWIN_NEED_BUTTON */
