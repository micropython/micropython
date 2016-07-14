/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_checkbox.c
 * @brief   GWIN sub-system button code
 */

#include "../../gfx.h"

#if (GFX_USE_GWIN && GWIN_NEED_CHECKBOX) || defined(__DOXYGEN__)

#include "gwin_class.h"

// Parameters for button custom draw
#define CHK_TOP_FADE			50		// (CHK_TOP_FADE/255)% fade to white for top of button
#define CHK_BOTTOM_FADE			25		// (CHK_BOTTOM_FADE/255)% fade to black for bottom of button

// Send the checkbox event
static void SendCheckboxEvent(GWidgetObject *gw) {
	GSourceListener	*	psl;
	GEvent *			pe;
	#define pce			((GEventGWinCheckbox *)pe)

	// Trigger a GWIN Checkbox Event
	psl = 0;
	while ((psl = geventGetSourceListener(GWIDGET_SOURCE, psl))) {
		if (!(pe = geventGetEventBuffer(psl)))
			continue;
		pce->type = GEVENT_GWIN_CHECKBOX;
		pce->gwin = &gw->g;
		pce->isChecked = (gw->g.flags & GCHECKBOX_FLG_CHECKED) ? TRUE : FALSE;
		#if GWIN_WIDGET_TAGS
			pce->tag = gw->tag;
		#endif
		geventSendEvent(psl);
	}

	#undef pce
}

#if GINPUT_NEED_MOUSE
	static void CheckboxMouseDown(GWidgetObject *gw, coord_t x, coord_t y) {
		(void) x; (void) y;
		gw->g.flags ^= GCHECKBOX_FLG_CHECKED;
		_gwinUpdate((GHandle)gw);
		SendCheckboxEvent(gw);
	}
#endif

#if GINPUT_NEED_KEYBOARD || GWIN_NEED_KEYBOARD
	static void CheckboxKeyboard(GWidgetObject* gw, GEventKeyboard* pke)
	{
		// Only react on KEYDOWN events. Ignore KEYUP events.
		if (pke->keystate & GKEYSTATE_KEYUP) {
			return;
		}

		// ENTER and SPACE keys to check/uncheck the checkbox
		if (pke->c[0] == GKEY_ENTER || pke->c[0] == GKEY_SPACE) {
			gw->g.flags ^= GCHECKBOX_FLG_CHECKED;
		}

		_gwinUpdate((GHandle)gw);
	}
#endif

#if GINPUT_NEED_TOGGLE
	static void CheckboxToggleOn(GWidgetObject *gw, uint16_t role) {
		(void) role;
		gw->g.flags ^= GCHECKBOX_FLG_CHECKED;
		_gwinUpdate((GHandle)gw);
		SendCheckboxEvent(gw);
	}

	static void CheckboxToggleAssign(GWidgetObject *gw, uint16_t role, uint16_t instance) {
		(void) role;
		((GCheckboxObject *)gw)->toggle = instance;
	}

	static uint16_t CheckboxToggleGet(GWidgetObject *gw, uint16_t role) {
		(void) role;
		return ((GCheckboxObject *)gw)->toggle;
	}
#endif

// The checkbox VMT table
static const gwidgetVMT checkboxVMT = {
	{
		"Checkbox",				// The classname
		sizeof(GCheckboxObject),// The object size
		_gwidgetDestroy,		// The destroy routine
		_gwidgetRedraw,			// The redraw routine
		0,						// The after-clear routine
	},
	gwinCheckboxDraw_CheckOnLeft,	// The default drawing routine
	#if GINPUT_NEED_MOUSE
		{
			CheckboxMouseDown,		// Process mouse down events
			0,						// Process mouse up events (NOT USED)
			0,						// Process mouse move events (NOT USED)
		},
	#endif
	#if GINPUT_NEED_KEYBOARD || GWIN_NEED_KEYBOARD
		{
			CheckboxKeyboard		// Process keyboard events
		},
	#endif
	#if GINPUT_NEED_TOGGLE
		{
			1,						// 1 toggle role
			CheckboxToggleAssign,	// Assign Toggles
			CheckboxToggleGet,		// Get Toggles
			0,						// Process toggle off events (NOT USED)
			CheckboxToggleOn,		// Process toggle on events
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

GHandle gwinGCheckboxCreate(GDisplay *g, GCheckboxObject *gb, const GWidgetInit *pInit) {
	if (!(gb = (GCheckboxObject *)_gwidgetCreate(g, &gb->w, pInit, &checkboxVMT)))
		return 0;

	#if GINPUT_NEED_TOGGLE
		gb->toggle = GWIDGET_NO_INSTANCE;
	#endif
	gwinSetVisible((GHandle)gb, pInit->g.show);
	return (GHandle)gb;
}

void gwinCheckboxCheck(GHandle gh, bool_t isChecked) {
	if (gh->vmt != (gwinVMT *)&checkboxVMT)
		return;

	if (isChecked) {
		if ((gh->flags & GCHECKBOX_FLG_CHECKED)) return;
		gh->flags |= GCHECKBOX_FLG_CHECKED;
	} else {
		if (!(gh->flags & GCHECKBOX_FLG_CHECKED)) return;
		gh->flags &= ~GCHECKBOX_FLG_CHECKED;
	}
	_gwinUpdate(gh);
	SendCheckboxEvent((GWidgetObject *)gh);
}

bool_t gwinCheckboxIsChecked(GHandle gh) {
	if (gh->vmt != (gwinVMT *)&checkboxVMT)
		return FALSE;

	return (gh->flags & GCHECKBOX_FLG_CHECKED) ? TRUE : FALSE;
}

/*----------------------------------------------------------
 * Custom Draw Routines
 *----------------------------------------------------------*/

static const GColorSet *getCheckboxColors(GWidgetObject *gw) {
	if (!(gw->g.flags & GWIN_FLG_SYSENABLED))	return &gw->pstyle->disabled;
	if ((gw->g.flags & GCHECKBOX_FLG_CHECKED))	return &gw->pstyle->pressed;
	return &gw->pstyle->enabled;
}

void gwinCheckboxDraw_CheckOnLeft(GWidgetObject *gw, void *param) {
	#define gcw			((GCheckboxObject *)gw)
	coord_t				ld, df;
	const GColorSet *	pcol;
	(void)				param;

	if (gw->g.vmt != (gwinVMT *)&checkboxVMT) return;
	pcol = getCheckboxColors(gw);

	// Get the dimension of the check box (sans text)
	ld = gw->g.width < gw->g.height ? gw->g.width : gw->g.height;

	// Draw the empty check box
	gdispGFillArea(gw->g.display, gw->g.x+1, gw->g.y+1, ld, ld-2, gw->pstyle->background);
	gdispGDrawBox(gw->g.display, gw->g.x, gw->g.y, ld, ld, pcol->edge);

	// Draw the check
	df = ld < 4 ? 1 : 2;
	if (gw->g.flags & GCHECKBOX_FLG_CHECKED)
		gdispGFillArea(gw->g.display, gw->g.x+df, gw->g.y+df, ld-2*df, ld-2*df, pcol->fill);

	// Render highlighted border if focused
	_gwidgetDrawFocusRect(gw, 1, 1, ld-2, ld-2);

	// Draw the text
	gdispGFillStringBox(gw->g.display, gw->g.x+ld+1, gw->g.y, gw->g.width-ld-1, gw->g.height, gw->text, gw->g.font, pcol->text, gw->pstyle->background, justifyLeft);
	#undef gcw
}

void gwinCheckboxDraw_CheckOnRight(GWidgetObject *gw, void *param) {
	#define gcw			((GCheckboxObject *)gw)
	coord_t				ep, ld, df;
	const GColorSet *	pcol;
	(void)				param;

	if (gw->g.vmt != (gwinVMT *)&checkboxVMT) return;
	pcol = getCheckboxColors(gw);

	// Get the dimension of the check box (sans text)
	ld = gw->g.width < gw->g.height ? gw->g.width : gw->g.height;

	// Get the position of the check box
	ep = gw->g.width-ld;

	// Draw the empty check box
	gdispGFillArea(gw->g.display, gw->g.x+ep-1, gw->g.y+1, ld, ld-2, gw->pstyle->background);
	gdispGDrawBox(gw->g.display, gw->g.x+ep, gw->g.y, ld, ld, pcol->edge);

	// Draw the check
	df = ld < 4 ? 1 : 2;
	if (gw->g.flags & GCHECKBOX_FLG_CHECKED)
		gdispGFillArea(gw->g.display, gw->g.x+ep+df, gw->g.y+df, ld-2*df, ld-2*df, pcol->fill);

	// Render highlighted border if focused
	_gwidgetDrawFocusRect(gw, ep+1, 1, ld-2, ld-2);

	// Draw the text
	gdispGFillStringBox(gw->g.display, gw->g.x, gw->g.y, ep-1, gw->g.height, gw->text, gw->g.font, pcol->text, gw->pstyle->background, justifyRight);
	#undef gcw
}

#if GWIN_FLAT_STYLING
	void gwinCheckboxDraw_Button(GWidgetObject *gw, void *param) {
		const GColorSet *	pcol;
		(void)				param;

		if (gw->g.vmt != (gwinVMT *)&checkboxVMT)	return;
		pcol = getCheckboxColors(gw);

		#if GWIN_NEED_FLASHING
			// Flash the on and off state.
			pcol = _gwinGetFlashedColor(gw, pcol, TRUE);
		#endif

		gdispGFillStringBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width-1, gw->g.height-1, gw->text, gw->g.font, pcol->text, pcol->fill, justifyCenter);
		gdispGDrawLine(gw->g.display, gw->g.x+gw->g.width-1, gw->g.y, gw->g.x+gw->g.width-1, gw->g.y+gw->g.height-1, pcol->edge);
		gdispGDrawLine(gw->g.display, gw->g.x, gw->g.y+gw->g.height-1, gw->g.x+gw->g.width-2, gw->g.y+gw->g.height-1, pcol->edge);
	}
#else
	void gwinCheckboxDraw_Button(GWidgetObject *gw, void *param) {
		const GColorSet *	pcol;
		fixed				alpha;
		fixed				dalpha;
		coord_t				i;
		color_t				tcol, bcol;
		(void)				param;

		if (gw->g.vmt != (gwinVMT *)&checkboxVMT)	return;
		pcol = getCheckboxColors(gw);

		#if GWIN_NEED_FLASHING
			// Flash the on and off state.
			pcol = _gwinGetFlashedColor(gw, pcol, TRUE);
		#endif

		/* Fill the box blended from variants of the fill color */
		tcol = gdispBlendColor(White, pcol->fill, CHK_TOP_FADE);
		bcol = gdispBlendColor(Black, pcol->fill, CHK_BOTTOM_FADE);
		dalpha = FIXED(255)/gw->g.height;
		for(alpha = 0, i = 0; i < gw->g.height; i++, alpha += dalpha)
			gdispGDrawLine(gw->g.display, gw->g.x, gw->g.y+i, gw->g.x+gw->g.width-2, gw->g.y+i, gdispBlendColor(bcol, tcol, NONFIXED(alpha)));

		gdispGDrawStringBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width-1, gw->g.height-1, gw->text, gw->g.font, pcol->text, justifyCenter);
		gdispGDrawLine(gw->g.display, gw->g.x+gw->g.width-1, gw->g.y, gw->g.x+gw->g.width-1, gw->g.y+gw->g.height-1, pcol->edge);
		gdispGDrawLine(gw->g.display, gw->g.x, gw->g.y+gw->g.height-1, gw->g.x+gw->g.width-2, gw->g.y+gw->g.height-1, pcol->edge);
	}
#endif

#endif /* (GFX_USE_GWIN && GWIN_NEED_CHECKBOX) */
