/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_progressbar.c
 * @brief   GWIN sub-system progressbar code
 */

#include "../../gfx.h"

#if (GFX_USE_GWIN && GWIN_NEED_PROGRESSBAR) || defined(__DOXYGEN__)

#include "gwin_class.h"

// Reset the display position back to the value predicted by the saved progressbar position
static void PBResetDisplayPos(GProgressbarObject *gsw) {
	if (gsw->w.g.width < gsw->w.g.height)
		gsw->dpos = gsw->w.g.height-1-((gsw->w.g.height-1)*(gsw->pos-gsw->min))/(gsw->max-gsw->min);
	else
		gsw->dpos = ((gsw->w.g.width-1)*(gsw->pos-gsw->min))/(gsw->max-gsw->min);
}

// We have to deinitialize the timer which auto updates the progressbar if any
static void PBDestroy(GHandle gh) {
	#if GWIN_PROGRESSBAR_AUTO
		gtimerStop(&((GProgressbarObject *)gh)->gt);
		gtimerDeinit(&((GProgressbarObject *)gh)->gt);
	#endif

	_gwidgetDestroy(gh);
}

// The progressbar VMT table
static const gwidgetVMT progressbarVMT = {
	{
		"Progressbar",				// The classname
		sizeof(GProgressbarObject),	// The object size
		PBDestroy,				// The destroy routine
		_gwidgetRedraw,			// The redraw routine
		0,						// The after-clear routine
	},
	gwinProgressbarDraw_Std,			// The default drawing routine
	#if GINPUT_NEED_MOUSE
		{
			0,						// Process mouse down events (NOT USED)
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
			0,						// Process toggle off events (NOT USED)
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
};

GHandle gwinGProgressbarCreate(GDisplay *g, GProgressbarObject *gs, const GWidgetInit *pInit) {
	if (!(gs = (GProgressbarObject *)_gwidgetCreate(g, &gs->w, pInit, &progressbarVMT)))
		return 0;

	gs->min = 0;
	gs->max = 100;
	gs->res = 1;
	gs->pos = 0;

	#if GWIN_PROGRESSBAR_AUTO
		gtimerInit(&gs->gt);
	#endif

	PBResetDisplayPos(gs);
	gwinSetVisible((GHandle)gs, pInit->g.show);

	return (GHandle)gs;
}

void gwinProgressbarSetRange(GHandle gh, int min, int max) {
	#define gsw		((GProgressbarObject *)gh)

	if (gh->vmt != (gwinVMT *)&progressbarVMT)
		return;

	if (min == max)		// prevent divide by 0 errors.
		max++;
	gsw->min = min;
	gsw->max = max;
	gsw->pos = min;

	PBResetDisplayPos(gsw);

	#undef gsw
}

void gwinProgressbarSetPosition(GHandle gh, int pos) {
	#define gsw		((GProgressbarObject *)gh)

	if (gh->vmt != (gwinVMT *)&progressbarVMT)
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

	PBResetDisplayPos(gsw);
	_gwinUpdate(gh);

	#undef gsw
}

void gwinProgressbarSetResolution(GHandle gh, int resolution) {
	#define gsw		((GProgressbarObject *)gh)

	if (gh->vmt != (gwinVMT *)&progressbarVMT)
		return;

	if (resolution <= 0)
		resolution = 1;

	gsw->res = resolution;

	#undef gsw
}

void gwinProgressbarIncrement(GHandle gh) {
	#define gsw		((GProgressbarObject *)gh)

	if (gh->vmt != (gwinVMT *)&progressbarVMT)
		return;

	if (gsw->max - gsw->pos > gsw->res)
		gsw->pos += gsw->res;
	else
		gsw->pos = gsw->max;

	PBResetDisplayPos(gsw);
	_gwinUpdate(gh);

	#undef gsw
}

void gwinProgressbarDecrement(GHandle gh) {
	#define gsw		((GProgressbarObject *)gh)

	if (gh->vmt != (gwinVMT *)&progressbarVMT)
		return;

	if (gsw->pos > gsw->res)
		gsw->pos -= gsw->min;
	else
		gsw->pos = gsw->min;

	gsw->pos -= gsw->res;

	PBResetDisplayPos(gsw);
	_gwinUpdate(gh);

	#undef gsw
}

#if GWIN_PROGRESSBAR_AUTO
	// used by gwinProgressbarStart();
	static void _progressbarCallback(void *param) {
		#define gsw		((GProgressbarObject *)gh)
		GHandle gh = (GHandle)param;

		if (gh->vmt != (gwinVMT *)&progressbarVMT)
			return;

		gwinProgressbarIncrement(gh);

		if (gsw->pos >= gsw->max)
			gtimerStop(&gsw->gt);

		#undef gsw
	}

	void gwinProgressbarStart(GHandle gh, delaytime_t delay) {
		#define gsw		((GProgressbarObject *)gh)

		if (gh->vmt != (gwinVMT *)&progressbarVMT)
			return;

		gtimerStart(&gsw->gt, _progressbarCallback, gh, TRUE, delay);

		#undef gsw
	}

	void gwinProgressbarStop(GHandle gh) {
		#define gsw		((GProgressbarObject *)gh)

		if (gh->vmt != (gwinVMT *)&progressbarVMT)
			return;

		gtimerStop(&gsw->gt);

		#undef gsw
	}
#endif /* GWIN_PROGRESSBAR_AUTO */

/*----------------------------------------------------------
 * Custom Draw Routines
 *----------------------------------------------------------*/

void gwinProgressbarDraw_Std(GWidgetObject *gw, void *param) {
	#define gsw			((GProgressbarObject *)gw)

	const GColorSet *	pcol;
	(void)				param;

	if (gw->g.vmt != (gwinVMT *)&progressbarVMT)
		return;

	// get the colors right
	if ((gw->g.flags & GWIN_FLG_SYSENABLED))
		pcol = &gw->pstyle->enabled;
	else
		pcol = &gw->pstyle->disabled;

	// Vertical progressbar
	if (gw->g.width < gw->g.height) {
		if (gsw->dpos != gw->g.height-1)
			gdispGFillArea(gw->g.display, gw->g.x, gw->g.y+gsw->dpos, gw->g.width, gw->g.height - gsw->dpos, pcol->progress);				// Active Area
		if (gsw->dpos != 0)
			gdispGFillArea(gw->g.display, gw->g.x, gw->g.y, gw->g.width, gsw->dpos, pcol->fill);											// Inactive area
		gdispGDrawBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width, gw->g.height, pcol->edge);												// Edge
		gdispGDrawLine(gw->g.display, gw->g.x, gw->g.y+gsw->dpos, gw->g.x+gw->g.width-1, gw->g.y+gsw->dpos, pcol->edge);					// Thumb

	// Horizontal progressbar
	} else {
		if (gsw->dpos != gw->g.width-1)
			gdispGFillArea(gw->g.display, gw->g.x+gsw->dpos, gw->g.y, gw->g.width-gsw->dpos, gw->g.height, pcol->fill);						// Inactive area
		if (gsw->dpos != 0)
			gdispGFillArea(gw->g.display, gw->g.x, gw->g.y, gsw->dpos, gw->g.height, pcol->progress);										// Active Area
		gdispGDrawBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width, gw->g.height, pcol->edge);												// Edge
		gdispGDrawLine(gw->g.display, gw->g.x+gsw->dpos, gw->g.y, gw->g.x+gsw->dpos, gw->g.y+gw->g.height-1, pcol->edge);					// Thumb
	}
	gdispGDrawStringBox(gw->g.display, gw->g.x+1, gw->g.y+1, gw->g.width-2, gw->g.height-2, gw->text, gw->g.font, pcol->text, justifyCenter);

	#undef gsw
}

#if GDISP_NEED_IMAGE
void gwinProgressbarDraw_Image(GWidgetObject *gw, void *param) {
	#define gsw			((GProgressbarObject *)gw)
	#define gi			((gdispImage *)param)
	const GColorSet *	pcol;
	coord_t				z, v;

	if (gw->g.vmt != (gwinVMT *)&progressbarVMT)
		return;

	if ((gw->g.flags & GWIN_FLG_SYSENABLED))
		pcol = &gw->pstyle->enabled;
	else
		pcol = &gw->pstyle->disabled;

	// Vertical progressbar
	if (gw->g.width < gw->g.height) {
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

	// Horizontal progressbar
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
}
#endif /* GDISP_NEED_IMAGE */

#endif /* GFX_USE_GWIN && GWIN_NEED_BUTTON */
