/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_container.c
 * @brief   GWIN sub-system container code
 */

#include "../../gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_CONTAINERS

#include "gwin_class.h"

void _gcontainerInit(void)
{
}

void _gcontainerDeinit(void)
{
}

GHandle _gcontainerCreate(GDisplay *g, GContainerObject *pgc, const GWidgetInit *pInit, const gcontainerVMT *vmt) {
	if (!(pgc = (GContainerObject *)_gwidgetCreate(g, (GWidgetObject *)pgc, pInit, &vmt->gw)))
		return 0;

	pgc->g.flags |= GWIN_FLG_CONTAINER;

	return 	&pgc->g;
}

void _gcontainerDestroy(GHandle gh) {
	GHandle		child;

	while((child = gwinGetFirstChild(gh)))
		gwinDestroy(child);
	_gwidgetDestroy(gh);
}

GHandle gwinGetFirstChild(GHandle gh) {
	GHandle		child;

	for(child = gwinGetNextWindow(0); child; child = gwinGetNextWindow(child))
		if (child->parent == gh)
			return child;
	return 0;
}

GHandle gwinGetSibling(GHandle gh) {
	GHandle		child;

	for(child = gwinGetNextWindow(gh), gh = gh->parent; child; child = gwinGetNextWindow(child))
		if (child->parent == gh)
			return child;
	return 0;
}

coord_t gwinGetInnerWidth(GHandle gh) {
	if (!(gh->flags & GWIN_FLG_CONTAINER))
		return 0;
	return gh->width - ((const gcontainerVMT *)gh->vmt)->LeftBorder(gh) - ((const gcontainerVMT *)gh->vmt)->RightBorder(gh);
}

coord_t gwinGetInnerHeight(GHandle gh) {
	if (!(gh->flags & GWIN_FLG_CONTAINER))
		return 0;
	return gh->height - ((const gcontainerVMT *)gh->vmt)->TopBorder(gh) - ((const gcontainerVMT *)gh->vmt)->BottomBorder(gh);
}

#endif /* GFX_USE_GWIN && GWIN_NEED_CONTAINERS */
/** @} */

/*-----------------------------------------------
 * The simplest container type - a container
 *-----------------------------------------------
 *
 * @defgroup Containers Containers
 * @ingroup GWIN
 *
 * @{
 */

#if GFX_USE_GWIN && GWIN_NEED_CONTAINER

#if GWIN_CONTAINER_BORDER != GWIN_FIRST_CONTROL_FLAG
	#error "GWIN Container: - Flag definitions don't match"
#endif

#define BORDER_WIDTH		2

static coord_t ContainerBorderSize(GHandle gh)	{ return (gh->flags & GWIN_CONTAINER_BORDER) ? BORDER_WIDTH : 0; }

// The container VMT table
static const gcontainerVMT containerVMT = {
	{
		{
			"Container",				// The classname
			sizeof(GContainerObject),	// The object size
			_gcontainerDestroy,			// The destroy routine
			_gcontainerRedraw,			// The redraw routine
			0,							// The after-clear routine
		},
		gwinContainerDraw_Std,			// The default drawing routine
		#if GINPUT_NEED_MOUSE
			{
				0, 0, 0,				// No mouse
			},
		#endif
		#if GINPUT_NEED_KEYBOARD || GWIN_NEED_KEYBOARD
			{
				0						// Process keyboard events
			},
		#endif
		#if GINPUT_NEED_TOGGLE
			{
				0, 0, 0, 0, 0,			// No toggles
			},
		#endif
		#if GINPUT_NEED_DIAL
			{
				0, 0, 0, 0,				// No dials
			},
		#endif
	},
	ContainerBorderSize,				// The size of the left border (mandatory)
	ContainerBorderSize,				// The size of the top border (mandatory)
	ContainerBorderSize,				// The size of the right border (mandatory)
	ContainerBorderSize,				// The size of the bottom border (mandatory)
	0,									// A child has been added (optional)
	0,									// A child has been deleted (optional)
};

GHandle gwinGContainerCreate(GDisplay *g, GContainerObject *gc, const GWidgetInit *pInit, uint32_t flags) {
	if (!(gc = (GContainerObject *)_gcontainerCreate(g, gc, pInit, &containerVMT)))
		return 0;

	gc->g.flags |= (flags & GWIN_CONTAINER_BORDER);

	gwinSetVisible((GHandle)gc, pInit->g.show);
	return (GHandle)gc;
}

void gwinContainerDraw_Transparent(GWidgetObject *gw, void *param) {
	(void)param;

	if (gw->g.vmt != (gwinVMT *)&containerVMT)
		return;

	if ((gw->g.flags & GWIN_CONTAINER_BORDER))
		gdispGDrawBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width, gw->g.height, (gw->g.flags & GWIN_FLG_SYSENABLED) ? gw->pstyle->enabled.edge : gw->pstyle->disabled.edge);

	// Don't touch the client area
}

void gwinContainerDraw_Std(GWidgetObject *gw, void *param) {
	(void)param;

	if (gw->g.vmt != (gwinVMT *)&containerVMT)
		return;

	gdispGFillArea(gw->g.display, gw->g.x, gw->g.y, gw->g.width, gw->g.height, gw->pstyle->background);
	gwinContainerDraw_Transparent(gw, param);
}

#if GDISP_NEED_IMAGE
	void gwinContainerDraw_Image(GWidgetObject *gw, void *param) {
		#define gi			((gdispImage *)param)
		coord_t				x, y, iw, ih, mx, my;

		if (gw->g.vmt != (gwinVMT *)&containerVMT)
			return;

		// Draw the frame
		gwinContainerDraw_Transparent(gw, param);

		// Draw the client area by tiling the image
		mx = gw->g.x+gw->g.width;
		my = gw->g.y+gw->g.height;
		y = gw->g.y;
		if ((gw->g.flags & GWIN_CONTAINER_BORDER)) {
			mx--;
			my--;
			y++;
		}
		for(ih=gi->height; y < my; y += ih) {
			if (ih > my - y)
				ih = my - y;
			x = gw->g.x;
			if ((gw->g.flags & GWIN_CONTAINER_BORDER))
				x++;
			for(iw=gi->width; x < mx; x += iw) {
				if (iw > mx - x)
					iw = mx - x;
				gdispGImageDraw(gw->g.display, gi, x, y, iw, ih, 0, 0);
			}
		}

		#undef gi
	}
#endif

#endif
