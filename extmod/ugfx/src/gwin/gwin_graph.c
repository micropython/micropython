/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_graph.c
 * @brief   GWIN sub-system button code
 */

#include "../../gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_GRAPH

#include "gwin_class.h"

#define GGRAPH_FLG_CONNECTPOINTS			(GWIN_FIRST_CONTROL_FLAG<<0)
#define GGRAPH_ARROW_SIZE					5

static const GGraphStyle GGraphDefaultStyle = {
	{ GGRAPH_POINT_DOT, 0, White },			// point
	{ GGRAPH_LINE_DOT, 2, Gray },			// line
	{ GGRAPH_LINE_SOLID, 0, White },		// x axis
	{ GGRAPH_LINE_SOLID, 0, White },		// y axis
	{ GGRAPH_LINE_NONE, 0, White, 0 },		// x grid
	{ GGRAPH_LINE_NONE, 0, White, 0 },		// y grid
	GWIN_GRAPH_STYLE_XAXIS_ARROWS|GWIN_GRAPH_STYLE_YAXIS_ARROWS		// flags
};

static const gwinVMT graphVMT = {
		"Graph",				// The classname
		sizeof(GGraphObject),	// The object size
		0,						// The destroy routine
		0,						// The redraw routine
		0,						// The after-clear routine
};

static void pointto(GGraphObject *gg, coord_t x, coord_t y, const GGraphPointStyle *style) {
	if (style->type == GGRAPH_POINT_NONE)
		return;

	// Convert to device space. Note the y-axis is inverted.
	x += gg->g.x + gg->xorigin;
	y = gg->g.y + gg->g.height - 1 - gg->yorigin - y;

	if (style->size <= 1) {
		gdispGDrawPixel(gg->g.display, x, y, style->color);
		return;
	}

	switch(style->type) {
	case GGRAPH_POINT_SQUARE:
		gdispGDrawBox(gg->g.display, x-style->size, y-style->size, 2*style->size, 2*style->size, style->color);
		break;
#if GDISP_NEED_CIRCLE
	case GGRAPH_POINT_CIRCLE:
		gdispGDrawCircle(gg->g.display, x, y, style->size, style->color);
		break;
#endif
	case GGRAPH_POINT_DOT:
	default:
		gdispGDrawPixel(gg->g.display, x, y, style->color);
		break;
	}
}

static void lineto(GGraphObject *gg, coord_t x0, coord_t y0, coord_t x1, coord_t y1, const GGraphLineStyle *style) {
	coord_t	dy, dx;
	coord_t addx, addy;
	coord_t P, diff, i;
	coord_t	run_on, run_off, run;

	if (style->type == GGRAPH_LINE_NONE)
		return;

	// Convert to device space. Note the y-axis is inverted.
	x0 += gg->g.x + gg->xorigin;
	y0 = gg->g.y + gg->g.height - 1 - gg->yorigin - y0;
	x1 += gg->g.x + gg->xorigin;
	y1 = gg->g.y + gg->g.height - 1 - gg->yorigin - y1;

	if (style->size <= 0) {
		// Use the driver to draw a solid line
		gdispGDrawLine(gg->g.display, x0, y0, x1, y1, style->color);
		return;
	}

	switch (style->type) {
	case GGRAPH_LINE_DOT:
		run_on = 1;
		run_off = -style->size;
		break;

	case GGRAPH_LINE_DASH:
		run_on = style->size;
		run_off = -style->size;
		break;

	case GGRAPH_LINE_SOLID:
	default:
		// Use the driver to draw a solid line
		gdispGDrawLine(gg->g.display, x0, y0, x1, y1, style->color);
		return;
	}

	// Use Bresenham's algorithm modified to draw a stylized line
	run = 0;
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
			if (run++ >= 0) {
				if (run >= run_on)
					run = run_off;
				gdispGDrawPixel(gg->g.display, x0, y0, style->color);
			}
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
			if (run++ >= 0) {
				if (run >= run_on)
					run = run_off;
				gdispGDrawPixel(gg->g.display, x0, y0, style->color);
			}
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

GHandle gwinGGraphCreate(GDisplay *g, GGraphObject *gg, const GWindowInit *pInit) {
	if (!(gg = (GGraphObject *)_gwindowCreate(g, &gg->g, pInit, &graphVMT, 0)))
		return 0;
	gg->xorigin = gg->yorigin = 0;
	gg->lastx = gg->lasty = 0;
	gwinGraphSetStyle((GHandle)gg, &GGraphDefaultStyle);
	gwinSetVisible((GHandle)gg, pInit->show);
	_gwinFlushRedraws(REDRAW_WAIT);
	return (GHandle)gg;
}

void gwinGraphSetStyle(GHandle gh, const GGraphStyle *pstyle) {
	#define gg	((GGraphObject *)gh)

	if (gh->vmt != &graphVMT)
		return;

	gg->style.point = pstyle->point;
	gg->style.line = pstyle->line;
	gg->style.xaxis = pstyle->xaxis;
	gg->style.yaxis = pstyle->yaxis;
	gg->style.xgrid = pstyle->xgrid;
	gg->style.ygrid = pstyle->ygrid;
	gg->style.flags = pstyle->flags;

	#undef gg
}

void gwinGraphSetOrigin(GHandle gh, coord_t x, coord_t y) {
	#define gg	((GGraphObject *)gh)

	if (gh->vmt != &graphVMT)
		return;

	gg->xorigin = x;
	gg->yorigin = y;

	#undef gg
}

void gwinGraphDrawAxis(GHandle gh) {
	#define gg	((GGraphObject *)gh)
	coord_t		i, xmin, ymin, xmax, ymax;

	if (gh->vmt != &graphVMT || !_gwinDrawStart(gh))
		return;

	xmin = -gg->xorigin;
	xmax = gh->width-gg->xorigin-1;
	ymin = -gg->yorigin;
	ymax = gh->height-gg->yorigin-1;

	// x grid - this code assumes that the GGraphGridStyle is a superset of GGraphListStyle
	if (gg->style.xgrid.type != GGRAPH_LINE_NONE && gg->style.xgrid.spacing >= 2) {
		for(i = gg->style.xgrid.spacing; i <= xmax; i += gg->style.xgrid.spacing)
			lineto(gg, i, ymin, i, ymax, (GGraphLineStyle *)&gg->style.xgrid);
		for(i = -gg->style.xgrid.spacing; i >= xmin; i -= gg->style.xgrid.spacing)
			lineto(gg, i, ymin, i, ymax, (GGraphLineStyle *)&gg->style.xgrid);
	}

	// y grid - this code assumes that the GGraphGridStyle is a superset of GGraphListStyle
	if (gg->style.ygrid.type != GGRAPH_LINE_NONE && gg->style.ygrid.spacing >= 2) {
		for(i = gg->style.ygrid.spacing; i <= ymax; i += gg->style.ygrid.spacing)
			lineto(gg, xmin, i, xmax, i, (GGraphLineStyle *)&gg->style.ygrid);
		for(i = -gg->style.ygrid.spacing; i >= ymin; i -= gg->style.ygrid.spacing)
			lineto(gg, xmin, i, xmax, i, (GGraphLineStyle *)&gg->style.ygrid);
	}

	// x axis
	lineto(gg, xmin, 0, xmax, 0, &gg->style.xaxis);
	if ((gg->style.flags & GWIN_GRAPH_STYLE_XAXIS_NEGATIVE_ARROWS)) {
		if (xmin > 0 || xmin < -(GGRAPH_ARROW_SIZE+1)) {
			lineto(gg, xmin, 0, xmin+GGRAPH_ARROW_SIZE, GGRAPH_ARROW_SIZE, &gg->style.xaxis);
			lineto(gg, xmin, 0, xmin+GGRAPH_ARROW_SIZE, -GGRAPH_ARROW_SIZE, &gg->style.xaxis);
		}
	}
	if ((gg->style.flags & GWIN_GRAPH_STYLE_XAXIS_POSITIVE_ARROWS)) {
		if (xmax < 0 || xmax > (GGRAPH_ARROW_SIZE+1)) {
			lineto(gg, xmax, 0, xmax-GGRAPH_ARROW_SIZE, GGRAPH_ARROW_SIZE, &gg->style.xaxis);
			lineto(gg, xmax, 0, xmax-GGRAPH_ARROW_SIZE, -GGRAPH_ARROW_SIZE, &gg->style.xaxis);
		}
	}

	// y axis
	lineto(gg, 0, ymin, 0, ymax, &gg->style.yaxis);
	if ((gg->style.flags & GWIN_GRAPH_STYLE_YAXIS_NEGATIVE_ARROWS)) {
		if (ymin > 0 || ymin < -(GGRAPH_ARROW_SIZE+1)) {
			lineto(gg, 0, ymin, GGRAPH_ARROW_SIZE, ymin+GGRAPH_ARROW_SIZE, &gg->style.yaxis);
			lineto(gg, 0, ymin, -GGRAPH_ARROW_SIZE, ymin+GGRAPH_ARROW_SIZE, &gg->style.yaxis);
		}
	}
	if ((gg->style.flags & GWIN_GRAPH_STYLE_YAXIS_POSITIVE_ARROWS)) {
		if (ymax < 0 || ymax > (GGRAPH_ARROW_SIZE+1)) {
			lineto(gg, 0, ymax, GGRAPH_ARROW_SIZE, ymax-GGRAPH_ARROW_SIZE, &gg->style.yaxis);
			lineto(gg, 0, ymax, -GGRAPH_ARROW_SIZE, ymax-GGRAPH_ARROW_SIZE, &gg->style.yaxis);
		}
	}

	_gwinDrawEnd(gh);
	#undef gg
}

void gwinGraphStartSet(GHandle gh) {
	if (gh->vmt != &graphVMT)
		return;

	gh->flags &= ~GGRAPH_FLG_CONNECTPOINTS;
}

void gwinGraphDrawPoint(GHandle gh, coord_t x, coord_t y) {
	#define gg	((GGraphObject *)gh)

	if (gh->vmt != &graphVMT || !_gwinDrawStart(gh))
		return;

	if ((gh->flags & GGRAPH_FLG_CONNECTPOINTS)) {
		// Draw the line
		lineto(gg, gg->lastx, gg->lasty, x, y, &gg->style.line);

		// Redraw the previous point because the line may have overwritten it
		pointto(gg, gg->lastx, gg->lasty, &gg->style.point);

	} else
		gh->flags |= GGRAPH_FLG_CONNECTPOINTS;

	// Save this point for next time.
	gg->lastx = x;
	gg->lasty = y;

	// Draw this point.
	pointto(gg, x, y, &gg->style.point);

	_gwinDrawEnd(gh);
	#undef gg
}

void gwinGraphDrawPoints(GHandle gh, const point *points, unsigned count) {
	#define gg	((GGraphObject *)gh)
	unsigned		i;
	const point		*p;

	if (gh->vmt != &graphVMT || !_gwinDrawStart(gh))
		return;

	// Draw the connecting lines
	for(p = points, i = 0; i < count; p++, i++) {
		if ((gh->flags & GGRAPH_FLG_CONNECTPOINTS)) {
			// Draw the line
			lineto(gg, gg->lastx, gg->lasty, p->x, p->y, &gg->style.line);

			// Redraw the previous point because the line may have overwritten it
			if (i == 0)
				pointto(gg, gg->lastx, gg->lasty, &gg->style.point);

		} else
			gh->flags |= GGRAPH_FLG_CONNECTPOINTS;

		// Save this point for next time.
		gg->lastx = p->x;
		gg->lasty = p->y;
	}


	// Draw the points.
	for(p = points, i = 0; i < count; p++, i++)
		pointto(gg, p->x, p->y, &gg->style.point);

	_gwinDrawEnd(gh);
	#undef gg
}

#endif /* GFX_USE_GWIN && GWIN_NEED_GRAPH */
