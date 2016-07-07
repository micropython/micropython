/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file	src/gwin/gwin_graph.h
 * @brief	GWIN GRAPH module header file
 *
 * @defgroup Graph Graph
 * @ingroup Windows
 *
 * @brief	Graph window. Used to display highly customizable graphs.
 *
 * @details	GWIN allows it to easily draw graphs.
 *
 * @pre		GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre		GWIN_NEED_GRAPH must be set to TRUE in your gfxconf.h
 *
 * @{
 */

#ifndef _GWIN_GRAPH_H
#define _GWIN_GRAPH_H

/* This file is included within "src/gwin/gwin.h" */

typedef enum GGraphPointType_e {
	GGRAPH_POINT_NONE, GGRAPH_POINT_DOT, GGRAPH_POINT_SQUARE, GGRAPH_POINT_CIRCLE
	} GGraphPointType;

typedef struct GGraphPointStyle_t {
	GGraphPointType		type;
	coord_t				size;
	color_t				color;
	} GGraphPointStyle;

typedef enum GGraphLineType_e {
	GGRAPH_LINE_NONE, GGRAPH_LINE_SOLID, GGRAPH_LINE_DOT, GGRAPH_LINE_DASH
	} GGraphLineType;

typedef struct GGraphLineStyle_t {
	GGraphLineType		type;
	coord_t				size;
	color_t				color;
	} GGraphLineStyle;

typedef struct GGraphGridStyle_t {
	GGraphLineType		type;
	coord_t				size;
	color_t				color;
	coord_t				spacing;
	} GGraphGridStyle;

typedef struct GGraphStyle_t {
	GGraphPointStyle	point;
	GGraphLineStyle		line;
	GGraphLineStyle		xaxis;
	GGraphLineStyle		yaxis;
	GGraphGridStyle		xgrid;
	GGraphGridStyle		ygrid;
	uint16_t			flags;
		#define GWIN_GRAPH_STYLE_XAXIS_POSITIVE_ARROWS	0x0001
		#define GWIN_GRAPH_STYLE_XAXIS_NEGATIVE_ARROWS	0x0002
		#define GWIN_GRAPH_STYLE_YAXIS_POSITIVE_ARROWS	0x0004
		#define GWIN_GRAPH_STYLE_YAXIS_NEGATIVE_ARROWS	0x0008
		#define GWIN_GRAPH_STYLE_POSITIVE_AXIS_ARROWS	(GWIN_GRAPH_STYLE_XAXIS_POSITIVE_ARROWS|GWIN_GRAPH_STYLE_YAXIS_POSITIVE_ARROWS)
		#define GWIN_GRAPH_STYLE_NEGATIVE_AXIS_ARROWS	(GWIN_GRAPH_STYLE_XAXIS_NEGATIVE_ARROWS|GWIN_GRAPH_STYLE_YAXIS_NEGATIVE_ARROWS)
		#define GWIN_GRAPH_STYLE_XAXIS_ARROWS			(GWIN_GRAPH_STYLE_XAXIS_POSITIVE_ARROWS|GWIN_GRAPH_STYLE_XAXIS_NEGATIVE_ARROWS)
		#define GWIN_GRAPH_STYLE_YAXIS_ARROWS			(GWIN_GRAPH_STYLE_YAXIS_POSITIVE_ARROWS|GWIN_GRAPH_STYLE_YAXIS_NEGATIVE_ARROWS)
		#define GWIN_GRAPH_STYLE_ALL_AXIS_ARROWS		(GWIN_GRAPH_STYLE_XAXIS_ARROWS|GWIN_GRAPH_STYLE_YAXIS_ARROWS)
} GGraphStyle;

// A graph window
typedef struct GGraphObject {
	GWindowObject		g;
	GGraphStyle			style;
	coord_t				xorigin, yorigin;
	coord_t				lastx, lasty;
	} GGraphObject;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Create a graph window.
 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] g			The GDisplay to display this window on
 * @param[in] gg		The GGraphObject structure to initialise. If this is NULL the structure is dynamically allocated.
 * @param[in] pInit		The initialization parameters to use
 *
 * @note				The drawing color and the background color get set to the current defaults. If you haven't called
 * 						@p gwinSetDefaultColor() or @p gwinSetDefaultBgColor() then these are White and Black respectively.
 * @note				The font gets set to the current default font. If you haven't called @p gwinSetDefaultFont() then there
 * 						is no default font and text drawing operations will no nothing.
 * @note				The dimensions and position may be changed to fit on the real screen.
 * @note				A graph does not save the drawing state. It is not automatically redrawn if the window is moved or
 * 						its visibility state is changed.
 * @note				The coordinate system within the window for graphing operations (but not for any other drawing
 * 						operation) is relative to the bottom left corner and then shifted right and up by the specified
 * 						graphing x and y origin. Note that this system is inverted in the y direction relative to the display.
 * 						This gives the best graphing arrangement ie. increasing y values are closer to the top of the display.
 *
 * @api
 */
GHandle gwinGGraphCreate(GDisplay *g, GGraphObject *gg, const GWindowInit *pInit);
#define gwinGraphCreate(gg, pInit)			gwinGGraphCreate(GDISP, gg, pInit)

/**
 * @brief   Set the style of the graphing operations.
 *
 * @param[in] gh		The window handle (must be a graph window)
 * @param[in] pstyle	The graph style to set.
 * @note				The graph is not automatically redrawn. The new style will apply to any new drawing operations.
 *
 * @api
 */
void gwinGraphSetStyle(GHandle gh, const GGraphStyle *pstyle);

/**
 * @brief   Set the origin for graphing operations.
 *
 * @param[in] gh		The window handle (must be a graph window)
 * @param[in] x, y		The new origin for the graph (in graph coordinates relative to the bottom left corner).
 * @note				The graph is not automatically redrawn. The new origin will apply to any new drawing operations.
 *
 * @api
 */
void gwinGraphSetOrigin(GHandle gh, coord_t x, coord_t y);

/**
 * @brief   Draw the axis and the background grid.
 *
 * @param[in] gh		The window handle (must be a graph window)
 * @note				The graph is not automatically cleared. You must do that first by calling gwinClear().
 *
 * @api
 */
void gwinGraphDrawAxis(GHandle gh);

/**
 * @brief   Start a new set of graphing data.
 * @details	This prevents a line being drawn from the last data point to the next point to be drawn.
 *
 * @param[in] gh		The window handle (must be a graph window)
 *
 * @api
 */
void gwinGraphStartSet(GHandle gh);

/**
 * @brief   Draw a graph point.
 * @details	A graph point and a line connecting to the previous point will be drawn.
 *
 * @param[in] gh		The window handle (must be a graph window)
 * @param[in] x, y		The new point for the graph.
 *
 * @api
 */
void gwinGraphDrawPoint(GHandle gh, coord_t x, coord_t y);

/**
 * @brief   Draw multiple graph points.
 * @details	A graph point and a line connecting to each previous point will be drawn.
 *
 * @param[in] gh		The window handle (must be a graph window)
 * @param[in] points	The array of points for the graph.
 * @param[in] count		The number of points in the array.
 * @note				This is slightly more efficient than calling gwinGraphDrawPoint() repeatedly.
 *
 * @api
 */
void gwinGraphDrawPoints(GHandle gh, const point *points, unsigned count);

#ifdef __cplusplus
}
#endif

#endif	/* _GWIN_GRAPH_H */
/** @} */

