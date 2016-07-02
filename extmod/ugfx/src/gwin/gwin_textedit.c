/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file	src/gwin/gwin_textedit.c
 * @brief	GWIN TextEdit widget header file
 */

#include "../../gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_TEXTEDIT

#include "gwin_class.h"
#include <string.h>

// Some settings
#define TEXT_PADDING_LEFT		4
#define CURSOR_PADDING_LEFT		0
#define CURSOR_EXTRA_HEIGHT		1

// Macros to assist in data type conversions
#define gh2obj ((GTexteditObject *)gh)
#define gw2obj ((GTexteditObject *)gw)

static bool_t resizeText(GWidgetObject* gw, size_t pos, int32_t diff) {
	char	*p, *q;
	size_t	sz;

	p = (char *)gw->text;
	sz = strlen(p)+1;
	if (diff < 0)
		memcpy(p+pos, p+pos-diff, sz-pos+diff);
	if (!(p = gfxRealloc(p, sz, sz+diff)))
		return FALSE;
	gw->text = p;
	if (diff > 0) {
		q = p + sz;
		p += pos;
		while(--q >= p)
			q[diff] = q[0];
	}
	return TRUE;
}

// Function that allows to set the cursor to any position in the string
// This should be optimized. Currently it is an O(n^2) problem and therefore very
// slow. An optimized version would copy the behavior of mf_get_string_width()
// and do the comparation directly inside of that loop so we only iterate
// the string once.
static void TextEditMouseDown(GWidgetObject* gw, coord_t x, coord_t y) {
	uint16_t i = 0;

	(void)y;

	// Directly jump to the end of the string
	if (x > gdispGetStringWidth(gw->text, gw->g.font)) {
		gw2obj->cursorPos = strlen(gw->text);

	// Otherwise iterate through each character and get the size in pixels to compare
	} else {
		i = 1;
		while (gdispGetStringWidthCount(gw->text, gw->g.font, i) < x) {
			i++;
		}

		gw2obj->cursorPos = i-1;
	}

	_gwinUpdate((GHandle)gw);
}

#if (GFX_USE_GINPUT && GINPUT_NEED_KEYBOARD) || GWIN_NEED_KEYBOARD
	static void TextEditKeyboard(GWidgetObject* gw, GEventKeyboard* pke) {
		// Only react on KEYDOWN events. Ignore KEYUP events.
		if ((pke->keystate & GKEYSTATE_KEYUP) || !pke->bytecount)
			return;

		// Is it a special key?
		if (pke->keystate & GKEYSTATE_SPECIAL) {

			// Arrow keys to move the cursor
			switch ((uint8_t)pke->c[0]) {
			case GKEY_LEFT:
				if (!gw2obj->cursorPos)
					return;
				gw2obj->cursorPos--;
				break;
			case GKEY_RIGHT:
				if (!gw->text[gw2obj->cursorPos])
					return;
				gw2obj->cursorPos++;
				break;
			case GKEY_HOME:
				if (!gw2obj->cursorPos)
					return;
				gw2obj->cursorPos = 0;
				break;
			case GKEY_END:
				if (!gw->text[gw2obj->cursorPos])
					return;
				gw2obj->cursorPos = strlen(gw->text);
				break;
			default:
				return;
			}

		} else {

			// Normal key press
			switch((uint8_t)pke->c[0]) {
			case GKEY_BACKSPACE:
				// Backspace
				if (!gw2obj->cursorPos)
					return;
				gw2obj->cursorPos--;
				resizeText(gw, gw2obj->cursorPos, -1);
				break;
			case GKEY_TAB:
			case GKEY_LF:
			case GKEY_CR:
				// Move to the next field
				_gwinMoveFocus();
				return;
			case GKEY_DEL:
				// Delete
				if (!gw->text[gw2obj->cursorPos])
					return;
				resizeText(gw, gw2obj->cursorPos, -1);
				break;
			default:
				// Ignore any other control characters
				if ((uint8_t)pke->c[0] < GKEY_SPACE)
					return;

				// Keep the edit length to less than the maximum
				if (gw2obj->maxSize && gw2obj->cursorPos+pke->bytecount > gw2obj->maxSize)
					return;

				// Make space
				resizeText(gw, gw2obj->cursorPos, pke->bytecount);

				// Insert the character
				memcpy((char *)gw->text+gw2obj->cursorPos, pke->c, pke->bytecount);
				gw2obj->cursorPos += pke->bytecount;
				break;
			}
		}

		_gwinUpdate((GHandle)gw);
	}
#endif

static const gwidgetVMT texteditVMT = {
	{
		"TextEdit",					// The class name
		sizeof(GTexteditObject),	// The object size
		_gwidgetDestroy,			// The destroy routine
		_gwidgetRedraw, 			// The redraw routine
		0,							// The after-clear routine
	},
	gwinTexteditDefaultDraw,		// default drawing routine
	#if GINPUT_NEED_MOUSE
		{
			TextEditMouseDown,		// Process mouse down events (NOT USED)
			0,						// Process mouse up events (NOT USED)
			0,						// Process mouse move events (NOT USED)
		},
	#endif
	#if (GFX_USE_GINPUT && GINPUT_NEED_KEYBOARD) || GWIN_NEED_KEYBOARD
		{
			TextEditKeyboard		// Process keyboard key down events
		},
	#endif
	#if GINPUT_NEED_TOGGLE
		{
			0,						// No toggle role
			0,						// Assign Toggles (NOT USED)
			0,						// Get Toggles (NOT USED)
			0,						// Process toggle off event (NOT USED)
			0,						// Process toggle on event (NOT USED)
		},
	#endif
	#if GINPUT_NEED_DIAL
		{
			0,						// No dial roles
			0,						// Assign Dials (NOT USED)
			0, 						// Get Dials (NOT USED)
			0,						// Procees dial move events (NOT USED)
		},
	#endif
};

GHandle gwinGTexteditCreate(GDisplay* g, GTexteditObject* wt, GWidgetInit* pInit, size_t maxSize)
{
	char	*p;

	// Create the underlying widget
	if (!(wt = (GTexteditObject*)_gwidgetCreate(g, &wt->w, pInit, &texteditVMT)))
		return 0;

	wt->maxSize = maxSize;

	// Reallocate the text (if necessary)
	if (!(wt->w.g.flags & GWIN_FLG_ALLOCTXT)) {
		if (!(p = gfxAlloc(wt->maxSize+1)))
			return 0;
		strncpy(p, wt->w.text, wt->maxSize);
		wt->w.text = p;
		wt->w.g.flags |= GWIN_FLG_ALLOCTXT;
	}

	// Set text and cursor position
	wt->cursorPos = strlen(wt->w.text);

	gwinSetVisible(&wt->w.g, pInit->g.show);

	return (GHandle)wt;
}

void gwinTexteditDefaultDraw(GWidgetObject* gw, void* param)
{
	const char*			p;
	coord_t				cpos, tpos;
	const GColorSet*	pcol;

	(void)param;

	// Is it a valid handle?
	if (gw->g.vmt != (gwinVMT*)&texteditVMT)
		return;

	// Retrieve colors
	if ((gw->g.flags & GWIN_FLG_SYSENABLED))
		pcol = &gw->pstyle->enabled;
	else
		pcol = &gw->pstyle->disabled;

	// Adjust the text position so the cursor fits in the window
	p = gw->text;
	if (!gw2obj->cursorPos)
		tpos = 0;
	else {
		for(cpos = gw2obj->cursorPos; ; p++, cpos--) {
			tpos = gdispGetStringWidthCount(p, gw->g.font, cpos);
			if (tpos < gw->g.width-(TEXT_PADDING_LEFT+CURSOR_PADDING_LEFT))
				break;
		}
	}

	// Render background and string
	#if TEXT_PADDING_LEFT
		gdispGFillArea(gw->g.display, gw->g.x, gw->g.y, TEXT_PADDING_LEFT, gw->g.height, pcol->fill);
	#endif
	gdispGFillStringBox(gw->g.display, gw->g.x + TEXT_PADDING_LEFT, gw->g.y, gw->g.width-TEXT_PADDING_LEFT, gw->g.height, p, gw->g.font, pcol->text, pcol->fill, justifyLeft);

	// Render cursor (if focused)
	if (gwinGetFocus() == (GHandle)gw) {
		// Calculate cursor stuff

		// Draw cursor
		tpos += gw->g.x + CURSOR_PADDING_LEFT + TEXT_PADDING_LEFT + gdispGetFontMetric(gw->g.font, fontBaselineX)/2;
		cpos = (gw->g.height - gdispGetFontMetric(gw->g.font, fontHeight))/2 - CURSOR_EXTRA_HEIGHT;
		gdispGDrawLine(gw->g.display, tpos, gw->g.y + cpos, tpos, gw->g.y + gw->g.height - cpos, pcol->edge);
	}

	// Render border
	gdispGDrawBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width, gw->g.height, pcol->edge);

	// Render highlighted border if focused
	_gwidgetDrawFocusRect(gw, 0, 0, gw->g.width, gw->g.height);

}

#undef gh2obj
#undef gw2obj

#endif // GFX_USE_GWIN && GWIN_NEED_TEXTEDIT
