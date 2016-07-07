/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_widget.c
 * @brief   GWIN sub-system widget code
 */

#include "../../gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_WIDGET

#include <string.h>

#include "gwin_class.h"

// Our listener for events for widgets
static GListener gl;

#if (GFX_USE_GINPUT && GINPUT_NEED_KEYBOARD) || GWIN_NEED_KEYBOARD
	// Our current focus window
	static GHandle				_widgetInFocus;
#endif

// Our default style - a white background theme
const GWidgetStyle WhiteWidgetStyle = {
	HTML2COLOR(0xFFFFFF),			// window background
	HTML2COLOR(0x2A8FCD),			// focused

	// enabled color set
	{
		HTML2COLOR(0x000000),		// text
		HTML2COLOR(0x404040),		// edge
		HTML2COLOR(0xE0E0E0),		// fill
		HTML2COLOR(0x00E000)		// progress - active area
	},

	// disabled color set
	{
		HTML2COLOR(0xC0C0C0),		// text
		HTML2COLOR(0x808080),		// edge
		HTML2COLOR(0xE0E0E0),		// fill
		HTML2COLOR(0xC0E0C0)		// progress - active area
	},

	// pressed color set
	{
		HTML2COLOR(0x404040),		// text
		HTML2COLOR(0x404040),		// edge
		HTML2COLOR(0x808080),		// fill
		HTML2COLOR(0x00E000)		// progress - active area
	}
};

/* Our black style */
const GWidgetStyle BlackWidgetStyle = {
	HTML2COLOR(0x000000),			// window background
	HTML2COLOR(0x2A8FCD),			// focused

	// enabled color set
	{
		HTML2COLOR(0xC0C0C0),		// text
		HTML2COLOR(0xC0C0C0),		// edge
		HTML2COLOR(0x606060),		// fill
		HTML2COLOR(0x008000)		// progress - active area
	},

	// disabled color set
	{
		HTML2COLOR(0x808080),		// text
		HTML2COLOR(0x404040),		// edge
		HTML2COLOR(0x404040),		// fill
		HTML2COLOR(0x004000)		// progress - active area
	},

	// pressed color set
	{
		HTML2COLOR(0xFFFFFF),		// text
		HTML2COLOR(0xC0C0C0),		// edge
		HTML2COLOR(0xE0E0E0),		// fill
		HTML2COLOR(0x008000)		// progress - active area
	}
};

static const GWidgetStyle *	defaultStyle = &BlackWidgetStyle;

// We use these everywhere in this file
#define gw		((GWidgetObject *)gh)
#define wvmt	((gwidgetVMT *)gh->vmt)

// Process an event
static void gwidgetEvent(void *param, GEvent *pe) {
	#define pme		((GEventMouse *)pe)
	#define pke		((GEventKeyboard *)pe)
	#define pte		((GEventToggle *)pe)
	#define pde		((GEventDial *)pe)

	GHandle				h;
	GHandle				gh;
	#if GFX_USE_GINPUT && (GINPUT_NEED_TOGGLE || GINPUT_NEED_DIAL)
		uint16_t		role;
	#endif
	(void)				param;

	// Process various events
	switch (pe->type) {

	#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE
	case GEVENT_MOUSE:
	case GEVENT_TOUCH:
		// Cycle through all windows
		for (gh = 0, h = gwinGetNextWindow(0); h; h = gwinGetNextWindow(h)) {

			// The window must be on this display and visible to be relevant
			if (h->display != pme->display || !(h->flags & GWIN_FLG_SYSVISIBLE))
				continue;

			// Is the mouse currently captured by this widget?
			if ((h->flags & (GWIN_FLG_WIDGET|GWIN_FLG_MOUSECAPTURE)) == (GWIN_FLG_WIDGET|GWIN_FLG_MOUSECAPTURE)) {
				gh = h;
				if ((pme->buttons & GMETA_MOUSE_UP)) {
					gh->flags &= ~GWIN_FLG_MOUSECAPTURE;
					if (wvmt->MouseUp)
						wvmt->MouseUp(gw, pme->x - gh->x, pme->y - gh->y);
				} else if (wvmt->MouseMove)
					wvmt->MouseMove(gw, pme->x - gh->x, pme->y - gh->y);

				// There is only ever one captured mouse. Prevent normal mouse processing if there is a captured mouse
				gh = 0;
			
				break;
			}

			// Save the highest z-order window that the mouse is over
			if (pme->x >= h->x && pme->x < h->x + h->width && pme->y >= h->y && pme->y < h->y + h->height)
				gh = h;
		}

		// Process any mouse down over the highest order window if it is an enabled widget
		if (gh && (gh->flags & (GWIN_FLG_WIDGET|GWIN_FLG_SYSENABLED)) == (GWIN_FLG_WIDGET|GWIN_FLG_SYSENABLED)) {
			if ((pme->buttons & GMETA_MOUSE_DOWN)) {
				gh->flags |= GWIN_FLG_MOUSECAPTURE;

				#if (GFX_USE_GINPUT && GINPUT_NEED_KEYBOARD) || GWIN_NEED_KEYBOARD
					// We should try and capture the focus on this window.
					// If we can't then we don't change the focus
					gwinSetFocus(gh);
				#endif

				if (wvmt->MouseDown)
					wvmt->MouseDown(gw, pme->x - gh->x, pme->y - gh->y);
			}
		}
		break;
	#endif

	#if (GFX_USE_GINPUT && GINPUT_NEED_KEYBOARD) || GWIN_NEED_KEYBOARD
	case GEVENT_KEYBOARD:
		// If Tab key pressed then set focus to next widget
		if (pke->bytecount == 1 && pke->c[0] == GKEY_TAB) {
			if (!(pke->keystate & GKEYSTATE_KEYUP))
				_gwinMoveFocus();
			break;
		}

		// Otherwise, send keyboard events only to widget in focus
		if (_widgetInFocus)
			((gwidgetVMT*)_widgetInFocus->vmt)->KeyboardEvent((GWidgetObject*)_widgetInFocus, pke);
		break;
	#endif

	#if GFX_USE_GINPUT && GINPUT_NEED_TOGGLE
	case GEVENT_TOGGLE:
		// Cycle through all windows
		for(gh = gwinGetNextWindow(0); gh; gh = gwinGetNextWindow(gh)) {

			// check if it a widget that is enabled and visible
			if ((gh->flags & (GWIN_FLG_WIDGET|GWIN_FLG_SYSENABLED|GWIN_FLG_SYSVISIBLE)) != (GWIN_FLG_WIDGET|GWIN_FLG_SYSENABLED|GWIN_FLG_SYSVISIBLE))
				continue;

			for(role = 0; role < wvmt->toggleroles; role++) {
				if (wvmt->ToggleGet(gw, role) == pte->instance) {
					if (pte->on) {
						if (wvmt->ToggleOn)
							wvmt->ToggleOn(gw, role);
					} else {
						if (wvmt->ToggleOff)
							wvmt->ToggleOff(gw, role);
					}
				}
			}
		}
		break;
	#endif

	#if GFX_USE_GINPUT && GINPUT_NEED_DIAL
	case GEVENT_DIAL:
		// Cycle through all windows
		for(gh = gwinGetNextWindow(0); gh; gh = gwinGetNextWindow(gh)) {

			// check if it a widget that is enabled and visible
			if ((gh->flags & (GWIN_FLG_WIDGET|GWIN_FLG_SYSENABLED|GWIN_FLG_SYSVISIBLE)) != (GWIN_FLG_WIDGET|GWIN_FLG_SYSENABLED|GWIN_FLG_SYSVISIBLE))
				continue;

			for(role = 0; role < wvmt->dialroles; role++) {
				if (wvmt->DialGet(gw, role) == pte->instance) {
					if (wvmt->DialMove)
						wvmt->DialMove(gw, role, pde->value, pde->maxvalue);
				}
			}
		}
		break;
	#endif

	default:
		break;
	}

	#undef pme
	#undef pte
	#undef pke
	#undef pde
}

#if (GFX_USE_GINPUT && GINPUT_NEED_KEYBOARD) || GWIN_NEED_KEYBOARD
	GHandle gwinGetFocus(void) {
		return _widgetInFocus;
	}

	bool_t gwinSetFocus(GHandle gh) {
		GHandle	oldFocus;

		// Do we already have the focus?
		if (gh == _widgetInFocus)
			return TRUE;

		// The new window must be NULLL or a visible enabled widget with a keyboard handler
		if (!gh || ((gh->flags & (GWIN_FLG_WIDGET|GWIN_FLG_ENABLED|GWIN_FLG_SYSENABLED|GWIN_FLG_VISIBLE|GWIN_FLG_SYSVISIBLE)) == (GWIN_FLG_WIDGET|GWIN_FLG_ENABLED|GWIN_FLG_SYSENABLED|GWIN_FLG_VISIBLE|GWIN_FLG_SYSVISIBLE)
						&& ((gwidgetVMT*)gh->vmt)->KeyboardEvent)) {
			// Move the current focus
			oldFocus = _widgetInFocus;
			_widgetInFocus = gh;
			if (oldFocus)	_gwinUpdate(oldFocus);
			if (gh)			_gwinUpdate(gh);
			return TRUE;
		}
		return FALSE;
	}

	void _gwinMoveFocus(void) {
		GHandle	gh;
		bool_t	looponce;

		// Find a new focus window (one may or may not exist).
		looponce = FALSE;
		for(gh = gwinGetNextWindow(_widgetInFocus); ; gh = gwinGetNextWindow(gh)) {
			if (!gh && !looponce) {
				looponce = TRUE;
				gh = gwinGetNextWindow(0);
			}
			if (gwinSetFocus(gh))
				break;
		}
	}

	void _gwinFixFocus(GHandle gh) {
		GHandle	oldFocus;

		if ((gh->flags & (GWIN_FLG_WIDGET|GWIN_FLG_ENABLED|GWIN_FLG_SYSENABLED|GWIN_FLG_VISIBLE|GWIN_FLG_SYSVISIBLE)) == (GWIN_FLG_WIDGET|GWIN_FLG_ENABLED|GWIN_FLG_SYSENABLED|GWIN_FLG_VISIBLE|GWIN_FLG_SYSVISIBLE)
				&& ((gwidgetVMT*)gh->vmt)->KeyboardEvent) {

			// We are a candidate to be able to claim the focus

			// Claim the focus if no-one else has
			if (!_widgetInFocus)
				_widgetInFocus = gh;

			return;
		}

		// We have lost any right to the focus

		// Did we have the focus
		if (gh != _widgetInFocus)
			return;

		// We did - we need to find a new focus window
		oldFocus = _widgetInFocus;
		for(gh = gwinGetNextWindow(oldFocus); gh && gh != oldFocus; gh = gwinGetNextWindow(gh)) {

			// Must be a visible enabled widget with a keyboard handler
			if ((gh->flags & (GWIN_FLG_WIDGET|GWIN_FLG_ENABLED|GWIN_FLG_SYSENABLED|GWIN_FLG_VISIBLE|GWIN_FLG_SYSVISIBLE)) == (GWIN_FLG_WIDGET|GWIN_FLG_ENABLED|GWIN_FLG_SYSENABLED|GWIN_FLG_VISIBLE|GWIN_FLG_SYSVISIBLE)
					&& ((gwidgetVMT*)gh->vmt)->KeyboardEvent) {

				// Grab the focus for the new window
				_widgetInFocus = gh;

				// This new window still needs to be marked for redraw (but don't actually do it yet).
				gh->flags |= GWIN_FLG_NEEDREDRAW;
				// RedrawPending |= DOREDRAW_VISIBLES;			- FIX LATER
				return;
			}
		}

		// No-one has the right to the focus
		_widgetInFocus = 0;
	}

	void _gwidgetDrawFocusRect(GWidgetObject *gx, coord_t x, coord_t y, coord_t cx, coord_t cy) {
		uint16_t i;
		
		// Don't do anything if we don't have the focus
		if (&gx->g != _widgetInFocus)
			return;

		// Use the very simplest possible focus rectangle for now
		for (i = 0; i < GWIN_FOCUS_HIGHLIGHT_WIDTH; i++) {
			gdispGDrawBox(gx->g.display, gx->g.x+x+i, gx->g.y+y+i, cx-2*i, cy-2*i, gx->pstyle->focus);
		}
	}

#endif

#if GFX_USE_GINPUT && GINPUT_NEED_TOGGLE
	static GHandle FindToggleUser(uint16_t instance) {
		GHandle			gh;
		uint16_t		role;

		for(gh = gwinGetNextWindow(0); gh; gh = gwinGetNextWindow(gh)) {
			if (!(gh->flags & GWIN_FLG_WIDGET))		// check if it a widget
				continue;

			for(role = 0; role < wvmt->toggleroles; role++) {
				if (wvmt->ToggleGet(gw, role) == instance)
					return gh;
			}
		}
		return 0;
	}
#endif

#if GFX_USE_GINPUT && GINPUT_NEED_DIAL
	static GHandle FindDialUser(uint16_t instance) {
		GHandle			gh;
		uint16_t		role;

		for(gh = gwinGetNextWindow(0); gh; gh = gwinGetNextWindow(gh)) {
			if (!(gh->flags & GWIN_FLG_WIDGET))		// check if it a widget
				continue;

			for(role = 0; role < wvmt->dialroles; role++) {
				if (wvmt->DialGet(gw, role) == instance)
					return gh;
			}
		}
		return 0;
	}
#endif

void _gwidgetInit(void)
{
	geventListenerInit(&gl);
	geventRegisterCallback(&gl, gwidgetEvent, 0);
	//geventAttachSource(&gl, ginputGetMouse(GMOUSE_ALL_INSTANCES), GLISTEN_MOUSEMETA|GLISTEN_MOUSEDOWNMOVES);

	#if GINPUT_NEED_KEYBOARD || GWIN_NEED_KEYBOARD
		geventAttachSource(&gl, ginputGetKeyboard(GKEYBOARD_ALL_INSTANCES), GLISTEN_KEYUP);
	#endif
}

void _gwidgetDeinit(void)
{
	/* ToDo */
}

GHandle _gwidgetCreate(GDisplay *g, GWidgetObject *pgw, const GWidgetInit *pInit, const gwidgetVMT *vmt) {
	if (!(pgw = (GWidgetObject *)_gwindowCreate(g, &pgw->g, &pInit->g, &vmt->g, GWIN_FLG_WIDGET|GWIN_FLG_ENABLED|GWIN_FLG_SYSENABLED)))
		return 0;

	#if GWIN_NEED_CONTAINERS
		// This window can't be system enabled if the parent is not enabled
		if (pgw->g.parent && !(pgw->g.parent->flags & GWIN_FLG_SYSENABLED))
			pgw->g.flags &= ~GWIN_FLG_SYSENABLED;
	#endif
	pgw->text = pInit->text ? pInit->text : "";
	pgw->fnDraw = pInit->customDraw ? pInit->customDraw : vmt->DefaultDraw;
	pgw->fnParam = pInit->customParam;
	pgw->pstyle = pInit->customStyle ? pInit->customStyle : defaultStyle;
	#if GWIN_WIDGET_TAGS
			pgw->tag = pInit->tag;
	#endif

	return 	&pgw->g;
}

void _gwidgetDestroy(GHandle gh) {
	#if GFX_USE_GINPUT && (GINPUT_NEED_TOGGLE || GINPUT_NEED_DIAL)
		uint16_t	role, instance;
	#endif

	// Make the window is invisible so it is not eligible for focus
	gh->flags &= ~GWIN_FLG_VISIBLE;
	_gwinFixFocus(gh);

	// Deallocate the text (if necessary)
	if ((gh->flags & GWIN_FLG_ALLOCTXT)) {
		gh->flags &= ~GWIN_FLG_ALLOCTXT;
		gfxFree((void *)gw->text);
	}

	#if GFX_USE_GINPUT && GINPUT_NEED_TOGGLE
		// Detach any toggles from this object
		for(role = 0; role < wvmt->toggleroles; role++) {
			instance = wvmt->ToggleGet(gw, role);
			if (instance != GWIDGET_NO_INSTANCE) {
				wvmt->ToggleAssign(gw, role, GWIDGET_NO_INSTANCE);
				if (!FindToggleUser(instance))
					geventDetachSource(&gl, ginputGetToggle(instance));
			}
		}
	#endif

	#if GFX_USE_GINPUT && GINPUT_NEED_DIAL
		// Detach any dials from this object
		for(role = 0; role < wvmt->dialroles; role++) {
			instance = wvmt->DialGet(gw, role);
			if (instance != GWIDGET_NO_INSTANCE) {
				wvmt->DialAssign(gw, role, GWIDGET_NO_INSTANCE);
				if (!FindDialUser(instance))
					geventDetachSource(&gl, ginputGetDial(instance));
			}
		}
	#endif

	// Remove any listeners on this object.
	geventDetachSourceListeners((GSourceHandle)gh);
}

void _gwidgetRedraw(GHandle gh) {
	if (!(gh->flags & GWIN_FLG_SYSVISIBLE))
		return;

	gw->fnDraw(gw, gw->fnParam);
}

void _gwinSendEvent(GHandle gh, GEventType type) {
	GSourceListener	*	psl;
	GEventGWin *		pge;

	// Trigger a GWIN Event
	psl = 0;
	while ((psl = geventGetSourceListener(GWIDGET_SOURCE, psl))) {
		if (!(pge = (GEventGWin *)geventGetEventBuffer(psl)))
			continue;
		pge->type = type;
		pge->gwin = gh;
		#if GWIN_WIDGET_TAGS
			pge->tag = (gh->flags & GWIN_FLG_WIDGET) ? ((GWidgetObject *)gh)->tag : 0;
		#endif
		geventSendEvent(psl);
	}
}

void gwinWidgetClearInit(GWidgetInit *pwi) {
	char		*p;
	unsigned	len;

	for(p = (char *)pwi, len = sizeof(GWidgetInit); len; len--)
		*p++ = 0;
}

void gwinSetDefaultStyle(const GWidgetStyle *pstyle, bool_t updateAll) {
	if (!pstyle)
		pstyle = &BlackWidgetStyle;

	if (updateAll) {
		GHandle						gh;

		for(gh = gwinGetNextWindow(0); gh; gh = gwinGetNextWindow(gh)) {
			if ((gh->flags & GWIN_FLG_WIDGET) && ((GWidgetObject *)gh)->pstyle == defaultStyle)
				gwinSetStyle(gh, pstyle);
			else
				gwinRedraw(gh);
		}
	}
	gwinSetDefaultBgColor(pstyle->background);
	defaultStyle = pstyle;
}

/**
 * @brief   Get the current default style.
 *
 * @api
 */
const GWidgetStyle *gwinGetDefaultStyle(void) {
	return defaultStyle;
}

void gwinSetText(GHandle gh, const char *text, bool_t useAlloc) {
	if (!(gh->flags & GWIN_FLG_WIDGET))
		return;

	// Dispose of the old string
	if ((gh->flags & GWIN_FLG_ALLOCTXT)) {
		gh->flags &= ~GWIN_FLG_ALLOCTXT;
		if (gw->text) {
			gfxFree((void *)gw->text);
			gw->text = "";
		}
	}

	// Alloc the new text if required
	if (!text || !*text)
		gw->text = "";
	else if (useAlloc) {
		char *str;

		if ((str = gfxAlloc(strlen(text)+1))) {
			gh->flags |= GWIN_FLG_ALLOCTXT;
			strcpy(str, text);
		}
		gw->text = (const char *)str;
	} else
		gw->text = text;
	_gwinUpdate(gh);
}

const char *gwinGetText(GHandle gh) {
	if (!(gh->flags & GWIN_FLG_WIDGET))
		return 0;

	return gw->text;
}

bool_t gwinIsWidget(GHandle gh) {
	if (gh->flags & GWIN_FLG_WIDGET) {
		return TRUE;
	}

	return FALSE;
}

void gwinSetStyle(GHandle gh, const GWidgetStyle *pstyle) {
	if (!(gh->flags & GWIN_FLG_WIDGET))
		return;

	gw->pstyle = pstyle ? pstyle : defaultStyle;
	gh->bgcolor = pstyle->background;
	gh->color = pstyle->enabled.text;

	_gwinUpdate(gh);
}

const GWidgetStyle *gwinGetStyle(GHandle gh) {
	if (!(gh->flags & GWIN_FLG_WIDGET))
		return 0;

	return gw->pstyle;
}

void gwinSetCustomDraw(GHandle gh, CustomWidgetDrawFunction fn, void *param) {
	if (!(gh->flags & GWIN_FLG_WIDGET))
		return;

	gw->fnDraw = fn ? fn : wvmt->DefaultDraw;
	gw->fnParam = param;
	_gwinUpdate(gh);
}

bool_t gwinAttachListener(GListener *pl) {
	return geventAttachSource(pl, GWIDGET_SOURCE, 0);
}

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE
	bool_t DEPRECATED("This call can now be removed. Attaching the mouse to GWIN is now automatic.") gwinAttachMouse(uint16_t instance) {
		// This is now a NULL event because we automatically attach to all mice in the system.
		(void) instance;
		return TRUE;
	}
#endif

#if GFX_USE_GINPUT && GINPUT_NEED_TOGGLE
	bool_t gwinAttachToggle(GHandle gh, uint16_t role, uint16_t instance) {
		GSourceHandle	gsh;
		uint16_t		oi;

		// Is this a widget
		if (!(gh->flags & GWIN_FLG_WIDGET))
			return FALSE;

		// Is the role valid
		if (role >= wvmt->toggleroles)
			return FALSE;

		// Is this a valid device
		if (!(gsh = ginputGetToggle(instance)))
			return FALSE;

		// Is this already done?
		oi = wvmt->ToggleGet(gw, role);
		if (instance == oi)
			return TRUE;

		// Remove the old instance
		if (oi != GWIDGET_NO_INSTANCE) {
			wvmt->ToggleAssign(gw, role, GWIDGET_NO_INSTANCE);
			if (!FindToggleUser(oi))
				geventDetachSource(&gl, ginputGetToggle(oi));
		}

		// Assign the new
		wvmt->ToggleAssign(gw, role, instance);
		return geventAttachSource(&gl, gsh, GLISTEN_TOGGLE_ON|GLISTEN_TOGGLE_OFF);
	}

	bool_t gwinDetachToggle(GHandle gh, uint16_t role) {
		uint16_t		oi;

		// Is this a widget
		if (!(gh->flags & GWIN_FLG_WIDGET))
			return FALSE;

		// Is the role valid
		if (role >= ((gwidgetVMT *)gh->vmt)->toggleroles)
			return FALSE;

		oi = ((gwidgetVMT *)gh->vmt)->ToggleGet(gw, role);
		
		// Remove the instance
		if (oi != GWIDGET_NO_INSTANCE) {
			((gwidgetVMT *)gh->vmt)->ToggleAssign(gw, role, GWIDGET_NO_INSTANCE);
			if (!FindToggleUser(oi))		
				geventDetachSource(&gl, ginputGetToggle(oi));
		}
		return TRUE;
	}
	
#endif

#if GFX_USE_GINPUT && GINPUT_NEED_DIAL
	bool_t gwinAttachDial(GHandle gh, uint16_t role, uint16_t instance) {
		GSourceHandle	gsh;
		uint16_t		oi;

		if (!(gh->flags & GWIN_FLG_WIDGET))
			return FALSE;

		// Is the role valid
		if (role >= wvmt->dialroles)
			return FALSE;

		// Is this a valid device
		if (!(gsh = ginputGetDial(instance)))
			return FALSE;

		// Is this already done?
		oi = wvmt->DialGet(gw, role);
		if (instance == oi)
			return TRUE;

		// Remove the old instance
		if (oi != GWIDGET_NO_INSTANCE) {
			wvmt->DialAssign(gw, role, GWIDGET_NO_INSTANCE);
			if (!FindDialUser(oi))
				geventDetachSource(&gl, ginputGetDial(oi));
		}

		// Assign the new
		wvmt->DialAssign(gw, role, instance);
		return geventAttachSource(&gl, gsh, 0);
	}
#endif

#if GWIN_WIDGET_TAGS
	void gwinSetTag(GHandle gh, WidgetTag tag) {
		if ((gh->flags & GWIN_FLG_WIDGET))
			gw->tag = tag;
	}

	WidgetTag gwinGetTag(GHandle gh) {
		return ((gh->flags & GWIN_FLG_WIDGET)) ? gw->tag : 0;
	}
#endif

#undef gw
#undef wvmt
#endif /* GFX_USE_GWIN && GWIN_NEED_WIDGET */
/** @} */
