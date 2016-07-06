/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_tabset.c
 * @brief   GWIN sub-system tabset code.
 */

#include "../../gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_TABSET

#include "gwin_class.h"

#include <string.h>

// Some position values
#define GTABSET_BORDER			2
#define GTABSET_TXT_PAD			5

// Some color blending
#define GTABSET_TAB_CNR			8		// Diagonal corner on active tab
#define GTABSET_TOP_FADE		50		// (GTABSET_TOP_FADE/255)% fade to white for top of tab/button
#define GTABSET_BOTTOM_FADE		25		// (GTABSET_BOTTOM_FADE/255)% fade to black for bottom of tab/button
#define GTABSET_OUTLINE_FADE	128		// (GTABSET_OUTLINE_FADE/255)% fade to background for active tab edge

/* Internal state flags */
#define GWIN_TABSET_USER_FLAGS		(GWIN_TABSET_BORDER)
#if GWIN_TABSET_BORDER < GWIN_FIRST_CONTROL_FLAG
	#error "GWIN Tabset: - Flag definitions don't match"
#endif
#if GWIN_TABSET_BORDER > GWIN_LAST_CONTROL_FLAG
	#error "GWIN Tabset: - Flag definitions don't match"
#endif

/********************************************************************************************************************
 * Tab-page stuff
 */

static void FixTabSizePos(GHandle gh);

typedef GContainerObject GTabpageObject;

static coord_t TabpageBorderSize(GHandle gh)	{ (void)gh; return 0; }

static void gwinTabpageDraw_Std(GWidgetObject *gw, void *param) {
	(void)gw;
	(void)param;

	// The page is effectively transparent
}

static void TabpageDestroy(GHandle gh) {
	_gcontainerDestroy(gh);

	FixTabSizePos(gh->parent);
}

static const gcontainerVMT tabpageVMT = {
	{
		{
			"Tabpage",					// The classname
			sizeof(GTabpageObject),		// The object size
			TabpageDestroy,				// The destroy routine
			_gcontainerRedraw,			// The redraw routine
			0,							// The after-clear routine
		},
		gwinTabpageDraw_Std,			// The default drawing routine
		#if GINPUT_NEED_MOUSE
			{
				0,						// Process mouse down event
				0,						// Process mouse up events
				0,						// Process mouse move events
			},
		#endif
		#if GINPUT_NEED_KEYBOARD || GWIN_NEED_KEYBOARD
			{
				0						// Process keyboard events
			},
		#endif
		#if GINPUT_NEED_TOGGLE
			{
				0,						// 1 toggle role
				0,						// Assign Toggles
				0,						// Get Toggles
				0,						// Process toggle off events
				0,						// Process toggle on events
			},
		#endif
		#if GINPUT_NEED_DIAL
			{
				0,						// 1 dial roles
				0,						// Assign Dials
				0,						// Get Dials
				0,						// Process dial move events
			},
		#endif
	},
	TabpageBorderSize,					// The size of the left border (mandatory)
	TabpageBorderSize,					// The size of the top border (mandatory)
	TabpageBorderSize,					// The size of the right border (mandatory)
	TabpageBorderSize,					// The size of the bottom border (mandatory)
	0,									// A child has been added (optional)
	0,									// A child has been deleted (optional)
};

void gwinTabsetSetTitle(GHandle gh, const char *title, bool_t useAlloc) {
	if (gh->vmt != (gwinVMT *)&tabpageVMT)
		return;

	gwinSetText(gh, title, useAlloc);
	FixTabSizePos(gh->parent);
	gwinRedraw(gh->parent);
}

/********************************************************************************************************************
 * Tab-set stuff
 */

static coord_t	CalcTabHeight(GHandle gh) {
	GHandle		ph;
	coord_t		x, y, w;

	x = w = 0;
	y = GWIN_TABSET_TABHEIGHT;
	for(ph = gwinGetFirstChild(gh); ph; ph = gwinGetSibling(ph)) {
		if (ph->vmt == (gwinVMT *)&tabpageVMT) {
			w = gdispGetStringWidth(((GWidgetObject *)ph)->text, gh->font) + GTABSET_TXT_PAD*2;
			x += w;
			if (x > gh->width) {
				y += GWIN_TABSET_TABHEIGHT;
				x = w;
			}
		}
	}
	return y;
}

static void FixTabSizePos(GHandle gh) {
	coord_t		w, h, oldth;
	GHandle		vis, ph;

	oldth = ((GTabsetObject *)gh)->border_top;
	((GTabsetObject *)gh)->border_top = CalcTabHeight(gh);
	oldth -= ((GTabsetObject *)gh)->border_top;
	if (oldth == 0)
		return;

	vis = 0;
	w = gwinGetInnerWidth(gh);
	h = gwinGetInnerHeight(gh);
	for(ph = gwinGetFirstChild(gh); ph; ph = gwinGetSibling(ph)) {
		if (ph->vmt == (gwinVMT *)&tabpageVMT) {
			if (!vis || (ph->flags & GWIN_FLG_VISIBLE))
				vis = ph;
			gwinMove(ph, 0, 0);
			gwinResize(ph, w, h);
		} else {
			gwinMove(ph, ph->x-gh->x-((gh->flags & GWIN_TABSET_BORDER) ? GTABSET_BORDER : 0) , ph->y-oldth-gh->y);
		}
	}
	if (vis && !(vis->flags & GWIN_FLG_VISIBLE)) {
		vis->flags |= GWIN_FLG_VISIBLE;
		_gwinRippleVisibility();
	}
}

static coord_t TabSetBorderSize(GHandle gh)		{ return (gh->flags & GWIN_TABSET_BORDER) ? GTABSET_BORDER : 0; }
static coord_t TabSetBorderTop(GHandle gh)		{ return ((GTabsetObject *)gh)->border_top; }

#if GINPUT_NEED_MOUSE
	static void mouseDown(GWidgetObject *gw, coord_t mx, coord_t my) {
		GHandle		ph, gh;
		int			cnt;

		if (my < 0 || my > ((GTabsetObject *)gw)->border_top)
			return;

		// Work out which tab was pressed
		{
			coord_t		x, w, y;

			cnt = 0;
			x = w = 0;
			y = GWIN_TABSET_TABHEIGHT;
			gh = 0;
			for(ph = gwinGetFirstChild(&gw->g); ph; ph = gwinGetSibling(ph)) {
				if (ph->vmt == (gwinVMT *)&tabpageVMT) {
					w = gdispGetStringWidth(((GWidgetObject *)ph)->text, gw->g.font) + GTABSET_TXT_PAD*2;
					x += w;
					if (x > gw->g.width) {
						y += GWIN_TABSET_TABHEIGHT;
						x = w;
					}
					if (my < y && mx < x) {
						gh = ph;
						break;
					}
					cnt++;
				}
			}
			if (!gh || (gh->flags & GWIN_FLG_VISIBLE))
				return;
		}

		// Mark the existing tab as not visible
		for(ph = gwinGetFirstChild(&gw->g); ph; ph = gwinGetSibling(ph)) {
			if (ph->vmt == (gwinVMT *)&tabpageVMT && (ph->flags & GWIN_FLG_VISIBLE)) {
				// Mark this page invisible
				ph->flags &= ~GWIN_FLG_VISIBLE;
				break;
			}
		}

		// Mark this tab as visible
		gh->flags |= GWIN_FLG_VISIBLE;
		_gwinRippleVisibility();

		// Force a redraw of the whole tabset
		_gwinUpdate(&gw->g);

		// Send the Tabset Event
		{
			GSourceListener	*	psl;
			GEventGWinTabset *	pge;

			psl = 0;
			while ((psl = geventGetSourceListener(GWIDGET_SOURCE, psl))) {
				if (!(pge = (GEventGWinTabset *)geventGetEventBuffer(psl)))
					continue;
				pge->type = GEVENT_GWIN_TABSET;
				pge->gwin = &gw->g;
				#if GWIN_WIDGET_TAGS
					pge->tag = gw->tag;
				#endif
				pge->ghPage = gh;
				pge->nPage = cnt;
				geventSendEvent(psl);
			}
		}
	}
#endif

static const gcontainerVMT tabsetVMT = {
	{
		{
			"Tabset",					// The classname
			sizeof(GTabsetObject),		// The object size
			_gcontainerDestroy,			// The destroy routine
			_gcontainerRedraw,			// The redraw routine
			0,							// The after-clear routine
		},
		gwinTabsetDraw_Std,				// The default drawing routine
		#if GINPUT_NEED_MOUSE
			{
				mouseDown,				// Process mouse down event
				0,						// Process mouse up events
				0,						// Process mouse move events
			},
		#endif
		#if GINPUT_NEED_TOGGLE
			{
				0,						// 1 toggle role
				0,						// Assign Toggles
				0,						// Get Toggles
				0,						// Process toggle off events
				0,						// Process toggle on events
			},
		#endif
		#if GINPUT_NEED_DIAL
			{
				0,						// 1 dial roles
				0,						// Assign Dials
				0,						// Get Dials
				0,						// Process dial move events
			},
		#endif
	},
	TabSetBorderSize,					// The size of the left border (mandatory)
	TabSetBorderTop,					// The size of the top border (mandatory)
	TabSetBorderSize,					// The size of the right border (mandatory)
	TabSetBorderSize,					// The size of the bottom border (mandatory)
	0,									// A child has been added (optional)
	0,									// A child has been deleted (optional)
};

GHandle gwinGTabsetCreate(GDisplay *g, GTabsetObject *fo, GWidgetInit *pInit, uint32_t flags) {
	if (!(fo = (GTabsetObject *)_gcontainerCreate(g, (GContainerObject *)fo, pInit, &tabsetVMT)))
		return 0;

	// Set Tabset specific stuff
	fo->c.g.flags |= flags & GWIN_TABSET_USER_FLAGS;
	fo->border_top = GWIN_TABSET_TABHEIGHT;

	gwinSetVisible(&fo->c.g, pInit->g.show);

	return &fo->c.g;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// API calls
///////////////////////////////////////////////////////////////////////////////////////////////////

GHandle gwinTabsetAddTab(GHandle gh, const char *title, bool_t useAlloc) {
	GWidgetInit		wi;

	if (gh->vmt != (gwinVMT *)&tabsetVMT)
		return 0;

	// Set up the init structure
	gwinWidgetClearInit(&wi);
	wi.g.x = wi.g.y = 0;
	wi.g.width = gwinGetInnerWidth(gh);
	wi.g.height = gwinGetInnerHeight(gh);
	wi.g.show = !gwinTabsetCountTabs(gh);
	wi.g.parent = gh;

	// Create the page
	if (!(gh = _gcontainerCreate(gh->display, 0, &wi, &tabpageVMT)))
		return 0;

	// Set the text and visibility
	gwinSetText(gh, title, useAlloc);
	FixTabSizePos(gh->parent);

	gwinSetVisible(gh, wi.g.show);
	gwinRedraw(gh->parent);
	return gh;
}

int gwinTabsetCountTabs(GHandle gh) {
	int		cnt;

	if (gh->vmt != (gwinVMT *)&tabsetVMT)
		return 0;

	for(cnt = 0, gh = gwinGetFirstChild(gh); gh; gh = gwinGetSibling(gh)) {
		if (gh->vmt == (gwinVMT *)&tabpageVMT)
			cnt++;
	}
	return cnt;
}

GHandle gwinTabsetGetTabByIndex(GHandle gh, int index) {
	if (gh->vmt != (gwinVMT *)&tabsetVMT)
		return 0;

	for(gh = gwinGetFirstChild(gh); gh; gh = gwinGetSibling(gh)) {
		if (gh->vmt == (gwinVMT *)&tabpageVMT && !index--)
			return gh;
	}
	return 0;
}

GHandle gwinTabsetGetTabByTitle(GHandle gh, const char *title) {
	if (gh->vmt != (gwinVMT *)&tabsetVMT)
		return 0;

	for(gh = gwinGetFirstChild(gh); gh; gh = gwinGetSibling(gh)) {
		if (gh->vmt == (gwinVMT *)&tabpageVMT && !strcmp(title, ((GWidgetObject *)gh)->text))
			return gh;
	}
	return 0;
}

void gwinTabsetSetTab(GHandle gh) {
	GHandle ph;

	if (gh->vmt != (gwinVMT *)&tabpageVMT || (gh->flags & GWIN_FLG_VISIBLE))
		return;

	// We alter the visibility flags here directly as we know we are going to redraw everything
	for(ph = gwinGetFirstChild(gh->parent); ph; ph = gwinGetSibling(ph)) {
		if (ph->vmt == (gwinVMT *)&tabpageVMT && (ph->flags & GWIN_FLG_VISIBLE)) {
			// Mark this page invisible
			ph->flags &= ~GWIN_FLG_VISIBLE;
			break;
		}
	}

	// Mark this tab as visible
	gh->flags |= GWIN_FLG_VISIBLE;
	_gwinRippleVisibility();

	// Force a redraw of the tabset
	gwinRedraw(gh->parent);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Default render routines                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////

#if GWIN_FLAT_STYLING
	static void fgarea(GWidgetObjset *gw, const char *text, coord_t y, coord_t x, coord_t w) {
		const GColorSet *	pcol;

		pcol = (gw->g.flags & GWIN_FLG_SYSENABLED) ? &gw->pstyle->pressed : &gw->pstyle->disabled;

		gdispGDrawBox(gw->g.display, gw->g.x+x, gw->g.y+y, w, GWIN_TABSET_TABHEIGHT, pcol->edge);
		gdispGFillStringBox(gw->g.display, gw->g.x+x+1, gw->g.y+y+1, w-2, GWIN_TABSET_TABHEIGHT-1, text, gw->g.font, pcol->text, pcol->fill, justifyCenter);
	}
	static void bgarea(GWidgetObjset *gw, const char *text, coord_t y, coord_t x, coord_t w) {
		const GColorSet *	pcol;

		pcol = (gw->g.flags & GWIN_FLG_SYSENABLED) ? &gw->pstyle->enabled : &gw->pstyle->disabled;

		gdispGFillStringBox(gw->g.display, gw->g.x+x, gw->g.y+y, w-1, GWIN_TABSET_TABHEIGHT, text, gw->g.font, pcol->text, pcol->fill, justifyCenter);
		gdispGDrawLine(gw->g.display, gw->g.x+x+w-1, gw->g.y+y, gw->g.x+x+w-1, gw->g.y+y+GWIN_TABSET_TABHEIGHT-1, pcol->edge);
		gdispGDrawLine(gw->g.display, gw->g.x+x, gw->g.y+y+GWIN_TABSET_TABHEIGHT-1, gw->g.x+x+w-2, gw->g.y+y+GWIN_TABSET_TABHEIGHT-1, pcol->edge);
	}
	static void ntarea(GWidgetObjset *gw, coord_t y, coord_t x, coord_t w) {
		const GColorSet *	pcol;

		pcol = (gw->g.flags & GWIN_FLG_SYSENABLED) ? &gw->pstyle->pressed : &gw->pstyle->disabled;

		gdispGFillArea(gw->g.display, gw->g.x+x, gw->g.y, w+y, GWIN_TABSET_TABHEIGHT-1, gw->g.bgcolor);
		gdispGDrawLine(gw->g.display, gw->g.x+x, gw->g.y+y+GWIN_TABSET_TABHEIGHT-1, gw->g.x+x+w-1, gw->g.y+y+GWIN_TABSET_TABHEIGHT-1, pcol->edge);
	}
#else
	static void fgarea(GWidgetObject *gw, const char *text, coord_t y, coord_t x, coord_t w) {
		const GColorSet *	pcol;
		color_t				tcol;

		pcol = (gw->g.flags & GWIN_FLG_SYSENABLED) ? &gw->pstyle->pressed : &gw->pstyle->disabled;

		tcol = gdispBlendColor(pcol->edge, gw->pstyle->background, GTABSET_OUTLINE_FADE);
		gdispGFillStringBox(gw->g.display, gw->g.x+x, gw->g.y+y, w, GWIN_TABSET_TABHEIGHT, text, gw->g.font, pcol->text, gw->g.bgcolor, justifyCenter);
		gdispGDrawLine(gw->g.display, gw->g.x+x, gw->g.y+y, gw->g.x+x+w-(GTABSET_TAB_CNR+1), gw->g.y+y, tcol);
		gdispGDrawLine(gw->g.display, gw->g.x+x+w-(GTABSET_TAB_CNR+1), gw->g.y+y, gw->g.x+x+w-1, gw->g.y+y+GTABSET_TAB_CNR, tcol);
		gdispGDrawLine(gw->g.display, gw->g.x+x+w-1, gw->g.y+y+GTABSET_TAB_CNR, gw->g.x+x+w-1, gw->g.y+y+GWIN_TABSET_TABHEIGHT-1, tcol);
		if (!x)
			gdispGDrawLine(gw->g.display, gw->g.x, gw->g.y+y, gw->g.x, gw->g.y+y+GWIN_TABSET_TABHEIGHT-1, tcol);
	}
	static void bgarea(GWidgetObject *gw, const char *text, coord_t y, coord_t x, coord_t w) {
		const GColorSet *	pcol;
		fixed				alpha;
		coord_t				i;
		color_t				tcol, bcol;

		pcol = (gw->g.flags & GWIN_FLG_SYSENABLED) ? &gw->pstyle->enabled : &gw->pstyle->disabled;

		/* Fill the box blended from variants of the fill color */
		tcol = gdispBlendColor(White, pcol->fill, GTABSET_TOP_FADE);
		bcol = gdispBlendColor(Black, pcol->fill, GTABSET_BOTTOM_FADE);
		for(alpha = 0, i = 0; i < GWIN_TABSET_TABHEIGHT; i++, alpha += FIXED(255)/GWIN_TABSET_TABHEIGHT)
			gdispGDrawLine(gw->g.display, gw->g.x+x, gw->g.y+y+i, gw->g.x+x+w-2, gw->g.y+y+i, gdispBlendColor(bcol, tcol, NONFIXED(alpha)));
		gdispGDrawLine(gw->g.display, gw->g.x+x+w-1, gw->g.y+y, gw->g.x+x+w-1, gw->g.y+y+GWIN_TABSET_TABHEIGHT-1, pcol->edge);
		gdispGDrawStringBox(gw->g.display, gw->g.x+x+1, gw->g.y+y+1, w-2, GWIN_TABSET_TABHEIGHT-2, text, gw->g.font, pcol->text, justifyCenter);
	}
	static void ntarea(GWidgetObject *gw, coord_t y, coord_t x, coord_t w) {
		const GColorSet *	pcol;

		pcol = (gw->g.flags & GWIN_FLG_SYSENABLED) ? &gw->pstyle->pressed : &gw->pstyle->disabled;

		gdispGFillArea(gw->g.display, gw->g.x+x, gw->g.y+y, w, GWIN_TABSET_TABHEIGHT-1, gw->g.bgcolor);
		gdispGDrawLine(gw->g.display, gw->g.x+x, gw->g.y+y+GWIN_TABSET_TABHEIGHT-1, gw->g.x+x+w-1, gw->g.y+y+GWIN_TABSET_TABHEIGHT-1, pcol->edge);
	}
#endif

static coord_t drawtabs(GWidgetObject *gw) {
	GHandle		ph;
	coord_t		x, y, w;

	x = w = 0;
	y = 0;
	for(ph = gwinGetFirstChild(&gw->g); ph; ph = gwinGetSibling(ph)) {
		if (ph->vmt == (gwinVMT *)&tabpageVMT) {
			w = gdispGetStringWidth(((GWidgetObject *)ph)->text, gw->g.font) + GTABSET_TXT_PAD*2;
			if (x+w > gw->g.width) {
				ntarea(gw, y, x, gw->g.width - x);
				y += GWIN_TABSET_TABHEIGHT;
				x = 0;
			}
			if (ph->flags & GWIN_FLG_VISIBLE)
				fgarea(gw, ((GWidgetObject *)ph)->text, y, x, w);
			else
				bgarea(gw, ((GWidgetObject *)ph)->text, y, x, w);
			x += w;
		}
	}
	if (x < gw->g.width)
		ntarea(gw, y, x, gw->g.width - x);
	return y + GWIN_TABSET_TABHEIGHT;
}

static void drawborder(GWidgetObject *gw, coord_t y) {
	if ((gw->g.flags & GWIN_CONTAINER_BORDER)) {
		const GColorSet *	pcol;
		coord_t				x, w;

		pcol = (gw->g.flags & GWIN_FLG_SYSENABLED) ? &gw->pstyle->enabled : &gw->pstyle->disabled;
		x = gw->g.x+gw->g.width-1;
		w = gw->g.y+gw->g.height-1;
		gdispGDrawLine(gw->g.display, gw->g.x, gw->g.y+y, gw->g.x, w-1, pcol->edge);
		gdispGDrawLine(gw->g.display, gw->g.x, w, x, w, pcol->edge);
		gdispGDrawLine(gw->g.display, x, gw->g.y+y, x, w-1, pcol->edge);
	}
}

void gwinTabsetDraw_Transparent(GWidgetObject *gw, void *param) {
	(void)				param;

	if (gw->g.vmt != (gwinVMT *)&tabsetVMT)
		return;

	drawborder(gw, drawtabs(gw));

	// Don't touch the client area
}

void gwinTabsetDraw_Std(GWidgetObject *gw, void *param) {
	coord_t		y;
	(void)		param;

	if (gw->g.vmt != (gwinVMT *)&tabsetVMT)
		return;

	// Draw the frame
	y = drawtabs(gw);
	drawborder(gw, y);

	// Draw the client area
	if ((gw->g.flags & GWIN_CONTAINER_BORDER))
		gdispGFillArea(gw->g.display, gw->g.x+1, gw->g.y+y, gw->g.width-2, gw->g.height-y-1, gw->pstyle->background);
	else
		gdispGFillArea(gw->g.display, gw->g.x, gw->g.y+y, gw->g.width, gw->g.height-y, gw->pstyle->background);
}

#if GDISP_NEED_IMAGE
	void gwinTabsetDraw_Image(GWidgetObject *gw, void *param) {
		#define gi			((gdispImage *)param)
		coord_t				x, y, iw, ih, mx, my;

		if (gw->g.vmt != (gwinVMT *)&tabsetVMT)
			return;

		// Draw the frame
		y = drawtabs(gw);
		drawborder(gw, y);

		// Draw the client area by tiling the image
		mx = gw->g.x+gw->g.width;
		my = gw->g.y+gw->g.height;
		if ((gw->g.flags & GWIN_CONTAINER_BORDER)) {
			mx -= 2;
			my -= 1;
		}
		for(y = gw->g.y+y, ih = gi->height; y < my; y += ih) {
			if (ih > my - y)
				ih = my - y;
			x = gw->g.x;
			if ((gw->g.flags & GWIN_CONTAINER_BORDER))
				x++;
			for(iw = gi->width; x < mx; x += iw) {
				if (iw > mx - x)
					iw = mx - x;
				gdispGImageDraw(gw->g.display, gi, x, y, iw, ih, 0, 0);
			}
		}

		#undef gi
	}
#endif

#endif  /* (GFX_USE_GWIN && GWIN_NEED_TABSET) || defined(__DOXYGEN__) */
