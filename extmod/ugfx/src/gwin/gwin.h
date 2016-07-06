/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin.h
 *
 * @defgroup Window Window
 * @ingroup Windows
 *
 * @brief		The basic window implementation (base class).
 *
 * @details		GWIN provides a basic window manager which allows it to easily
 *				create and destroy different windows at runtime. Each window
 *				will have it's own properties such as colors as well as
 *				it's own drawing origin.
 *
 * @pre		GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @{
 */

#ifndef _GWIN_H
#define _GWIN_H

#include "../../gfx.h"

#if GFX_USE_GWIN || defined(__DOXYGEN__)

/* Forward declaration */
typedef struct GWindowObject *GHandle;

/**
 * @brief	A window object structure
 * @note	Do not access the members directly. Treat it as a black-box and use the method functions.
 * @{
 */
typedef struct GWindowObject {
	#if GWIN_NEED_WINDOWMANAGER
		// This MUST be the first member of the structure
		gfxQueueASyncItem	wmq;				/**< The next window (for the window manager) */
	#endif
	const struct gwinVMT*	vmt;				/**< The VMT for this GWIN */
	GDisplay *				display;			/**< The display this window is on */
	coord_t					x;					/**< The position relative to the screen */
	coord_t					y;					/**< The position relative to the screen */
	coord_t					width;				/**< The width of this window */
	coord_t					height;				/**< The height of this window */
	color_t					color;				/**< The current foreground drawing color */
	color_t					bgcolor;			/**< The current background drawing color */
	uint32_t				flags;				/**< Window flags (the meaning is private to the GWIN class) */
	#if GDISP_NEED_TEXT
		font_t				font;				/**< The current font */
	#endif
	#if GWIN_NEED_CONTAINERS
		GHandle				parent;				/**< The parent window */
	#endif
} GWindowObject, * GHandle;
/** @} */

/**
 * @brief	The structure to initialise a GWIN.
 *
 * @note	Some gwin's will need extra parameters.
 * @note	The dimensions and position may be changed to fit on the real screen.
 * @note	If you create this structure on the stack, you should always memset
 * 			it to all zero's first in case a future version of the software
 * 			add's extra fields. Alternatively you can use @p gwinClearInit()
 * 			to clear it.
 *
 * @{
 */
typedef struct GWindowInit {
	coord_t			x;								/**< The initial position relative to its parent */
	coord_t			y;								/**< The initial position relative to its parent */
	coord_t			width;							/**< The width */
	coord_t			height;							/**< The height */
	bool_t			show;							/**< Should the window be visible initially */
	#if GWIN_NEED_CONTAINERS
		GHandle		parent;							/**< The parent - must be a container or NULL */
	#endif
} GWindowInit;
/** @} */

/**
 * @brief	A window's minimized, maximized or normal size
 */
typedef enum { GWIN_NORMAL, GWIN_MAXIMIZE, GWIN_MINIMIZE } GWindowMinMax;

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------
 * Window Manager functions
 *-------------------------------------------------*/

#if GWIN_NEED_WINDOWMANAGER || defined(__DOXYGEN__)
	// Forward definition
	struct GWindowManager;

	/**
	 * @brief   Set the window manager for the GWIN system.
	 *
	 * @param[in] gwm		The window manager to use. Can be NULL to turn off the existing window manager.
	 *
	 * @note				A window manager is responsible for handling when window visibility is changed or
	 * 						a window is resized for moved. Note that only saved window states will be redrawn. Each
	 * 						window type can save different information (or none at all). See the documentation on each window
	 * 						type to see which information it saves (and can therefore be automatically redrawn).
	 * 						For window types that do not save any state information, the window manager determines what to do.
	 * 						Generally it will just clear the window to its background color.
	 *
	 * @api
	 */
	void gwinSetWindowManager(struct GWindowManager *gwm);
#endif

/*-------------------------------------------------
 * Functions that affect all windows
 *-------------------------------------------------*/

	/**
	 * @brief	Clear a GWindowInit structure to all zero's
	 * @note	This function is provided just to prevent problems
	 * 			on operating systems where using memset() causes issues
	 * 			in the users application.
	 *
	 * @param[in] pwi	The GWindowInit structure to clear
	 *
	 * @api
	 */
	void gwinClearInit(GWindowInit *pwi);

	/**
	 * @brief	Set the default foreground color for all new GWIN windows
	 *
	 * @param[in] clr	The color to be set
	 *
	 * @api
	 */
	void gwinSetDefaultColor(color_t clr);

	/**
	 * @brief	Get the default foreground color for all new GWIN windows
	 *
	 * @return	The current default color for all new GWIN windows
	 *
	 * @api
	 */
	color_t gwinGetDefaultColor(void);

	/**
	 * @brief	Set the default background color for all new GWIN windows
	 *
	 * @param[in] bgclr	The background color
	 *
	 * @api
	 */
	void gwinSetDefaultBgColor(color_t bgclr);

	/**
	 * @brief	Get the default background color for all new GWIN windows
	 *
	 * @return	The current default background color for all new GWIN windows
	 *
	 * @api
	 */
	color_t gwinGetDefaultBgColor(void);

	#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
		/**
		 * @brief	Set the default font for all new GWIN windows
		 *
		 * @param[in] font	The new font to be set
		 *
		 * @api
		 */
		void gwinSetDefaultFont(font_t font);

		/**
		 * @brief	Get the current default font
		 *
		 * @return	The current default font
		 *
		 * @api
		 */
		font_t gwinGetDefaultFont(void);
	#endif

/*-------------------------------------------------
 * Base functions
 *-------------------------------------------------*/

	/**
	 * @brief   Create a basic window.
	 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
	 *
	 * @param[in] g			The GDisplay to display this window on
	 * @param[in] pgw		The window structure to initialize. If this is NULL the structure is dynamically allocated.
	 * @param[in] pInit		How to initialise the window
	 *
	 * @note				The drawing color and the background color get set to the current defaults. If you haven't called
	 * 						@p gwinSetDefaultColor() or @p gwinSetDefaultBgColor() then these are White and Black respectively.
	 * @note				The font gets set to the current default font. If you haven't called @p gwinSetDefaultFont() then there
	 * 						is no default font and text drawing operations will no nothing.
	 * @note				A basic window does not save the drawing state. It is not automatically redrawn if the window is moved or
	 * 						its visibility state is changed.
	 *
	 * @api
	 */
	GHandle gwinGWindowCreate(GDisplay *g, GWindowObject *pgw, const GWindowInit *pInit);
	#define gwinWindowCreate(pgw, pInit)		gwinGWindowCreate(GDISP, pgw, pInit);

	/**
	 * @brief   Destroy a window (of any type). Releases any dynamically allocated memory.
	 *
	 * @param[in] gh		The window handle
	 *
	 * @api
	 */
	void gwinDestroy(GHandle gh);

	/**
	 * @brief	Get the real class name of the GHandle
	 * @details	Returns a string describing the object class.
	 *
	 * @param[in] gh	The window
	 *
	 * @return	A string describing the object class.
	 *
	 * @api
	 */
	const char* gwinGetClassName(GHandle gh);

	/**
	 * @brief	Get an ID that uniquely describes the class of the GHandle
	 *
	 * @param[in] gh	The window
	 *
	 * @api
	 */
	#define gwinGetClassID(gh)		((void *)((gh)->vmt))

	/**
	 * @brief	Get the X coordinate of the window
	 * @details	Returns the X coordinate of the origin of the window.
	 *			The coordinate is relative to the physical screen zero point.
	 *
	 * @param[in] gh	The window
	 *
	 * @api
	 */
	#define gwinGetScreenX(gh)			((gh)->x)

	/**
	 * @brief	Get the Y coordinate of the window
	 * @details	Returns the Y coordinate of the origin of the window.
	 *			The coordinate is relative to the physical screen zero point.
	 *
	 * @param[in] gh	The window
	 *
	 * @api
	 */
	#define gwinGetScreenY(gh)			((gh)->y)

	/**
	 * @brief	Get the width of the window
	 *
	 * @param[in] gh	The window
	 *
	 * @api
	 */
	#define gwinGetWidth(gh)			((gh)->width)

	/**
	 * @brief	Get the height of the window
	 *
	 * @param[in] gh	The window
	 *
	 * @api
	 */
	#define gwinGetHeight(gh)			((gh)->height)

	/**
	 * @brief	Set foreground color
	 * @details Set the color which will be used to draw
	 *
	 * @param[in] gh	The window
	 * @param[in] clr	The color to be set
	 *
	 * @api
	 */
	#define gwinSetColor(gh, clr)		(gh)->color = (clr)

	/**
	 * @brief	Set background color
	 * @details	Set the color which will be used as background
	 * @note	gwinClear() must be called to set the background color
	 *
	 * @param[in] gh	The window
	 * @param[in] bgclr	The background color
	 *
	 * @api
	 */
	#define gwinSetBgColor(gh, bgclr)	(gh)->bgcolor = (bgclr)

	/**
	 * @brief	Get the foreground color of a window
	 *
	 * @param[in] gh	The window
	 *
	 * @api
	 */
	#define gwinGetColor(gh)			(gh)->color

	/**
	 * @brief	Get the background color of a window
	 *
	 * @param[in] gh	The window
	 *
	 * @api
	 */
	#define gwinGetBgColor(gh)			(gh)->bgcolor

	/**
	 * @brief	Sets whether a window is visible or not
	 *
	 * @param[in] gh		The window
	 * @param[in] visible	Whether the window should be visible or not
	 *
	 * @note	When a window is marked as not visible, drawing operations
	 * 			on the window do nothing.
	 * @note	When a window is marked as visible, it is not automatically
	 * 			redrawn as many window types don't remember their drawing state.
	 * 			Widgets such as Buttons, Sliders etc will be redrawn.
	 * @note	If there is no window manager in use, when a window is marked
	 * 			as not visible, nothing is done to remove the window from the screen.
	 * 			When there is a window manager, it is up to the window manager to
	 * 			handle what happens.
	 * @note	Even when you mark a window as visible, it may still not be displayed
	 * 			if it's parent is invisible. When the parent becomes visible this child
	 * 			will automatically be shown because it is already marked as visible.
	 *
	 * @api
	 */
	void gwinSetVisible(GHandle gh, bool_t visible);

	/**
	 * @brief	Makes a widget become visible
	 *
	 * @param[in] gh		The window handle
	 *
	 * @api
	 */
	#define gwinShow(gh)		gwinSetVisible(gh, TRUE)

	/**
	 * @brief	Makes a widget become invisible
	 *
	 * @param[in] gh		The window handle
	 *
	 * @api
	 */
	#define gwinHide(gh)		gwinSetVisible(gh, FALSE)

	/**
	 * @brief	Gets the visibility of a window
	 * @return	TRUE if visible
	 *
	 * @note	It is possible for a child to be marked as visible by @p gwinSetVisible()
	 * 			but for this call to return FALSE if one of its parents are not visible.
	 *
	 * @param[in] gh		The window
	 *
	 * @api
	 */
	bool_t gwinGetVisible(GHandle gh);

	/**
	 * @brief	Enable or disable a window
	 *
	 * @param[in] gh		The window handle
	 * @param[in] enabled	Enable or disable the window
	 *
	 * @note	The window is automatically redrawn if it supports self-redrawing.
	 * @note	Even when you mark a window as enabled, it may still remain disabled
	 * 			if it's parent is disabled. When the parent becomes enabled this child
	 * 			will automatically be enabled because it is already marked as enabled.
	 *
	 * @api
	 */
	void gwinSetEnabled(GHandle gh, bool_t enabled);

	/**
	 * @brief	Enables a widget
	 *
	 * @param[in] gh		The window handle
	 *
	 * @api
	 */
	#define gwinEnable(gh)		gwinSetEnabled(gh, TRUE)

	/**
	 * @brief	Disables a widget
	 *
	 * @param[in] gh		The window handle
	 *
	 * @api
	 */
	#define gwinDisable(gh)		gwinSetEnabled(gh, FALSE)

	/**
	 * @brief	Gets the enabled state of a window
	 * @return	TRUE if enabled
	 *
	 * @note	It is possible for a child to be marked as enabled by @p gwinSetEnabled()
	 * 			but for this call to return FALSE if one of its parents are not enabled.
	 *
	 * @param[in] gh		The window
	 *
	 * @api
	 */
	bool_t gwinGetEnabled(GHandle gh);

	/**
	 * @brief	Move a window
	 *
	 * @param[in] gh		The window
	 * @param[in] x, y		The new position (screen relative) for this window
	 *
	 * @note	The final window position may not be the requested position. Windows
	 * 			are clipped to the screen area and the window manager may also affect the position.
	 * @note	The window is redrawn if it is visible. See the comments in @p gwinSetVisible()
	 * 			with regard to what can be redrawn and what can't.
	 * @note	It is up to the window manager to determine what happens with the screen area
	 * 			uncovered by moving the window. When there is no window manager, nothing
	 * 			is done with the uncovered area.
	 *
	 * @api
	 */
	void gwinMove(GHandle gh, coord_t x, coord_t y);

	/**
	 * @brief	Resize a window
	 *
	 * @param[in] gh				The window
	 * @param[in] width, height		The new size of the window
	 *
	 * @note	The final window size may not be the requested size. Windows
	 * 			are clipped to the screen area and the window manager may also affect the size.
	 * @note	The window is redrawn if it is visible. See the comments in @p gwinSetVisible()
	 * 			with regard to what can be redrawn and what can't.
	 * @note	It is up to the window manager to determine what happens with any screen area
	 * 			uncovered by resizing the window. When there is no window manager, nothing
	 * 			is done with the uncovered area.
	 *
	 * @api
	 */
	void gwinResize(GHandle gh, coord_t width, coord_t height);

	/**
	 * @brief	Redraw a window
	 *
	 * @param[in] gh				The window
	 *
	 * @note	This is normally never required as windows and widgets will redraw as required.
	 * 			Note that some windows are incapable of redrawing themselves as they don't save
	 * 			their drawing state.
	 *
	 * @api
	 */
	void gwinRedraw(GHandle gh);

	#if GWIN_NEED_WINDOWMANAGER || defined (__DOXYGEN__)
		/**
		 * @brief	Redraw a display
		 *
		 * @param[in] g				The display to redraw. Passing NULL will redraw all displays.
		 * @param[in] preserve		Should the redraw try to preserve existing screen data for those
		 * 							windows that can't redraw themselves?
		 *
		 * @note	This is normally never required as windows and widgets will redraw as required.
		 * @note	Some windows are incapable of redrawing themselves as they don't save
		 * 			their drawing state.
		 * @note	This does not clear the background - just redraws the gwin windows (where possible)
		 *
		 * @api
		 */
		void gwinRedrawDisplay(GDisplay *g, bool_t preserve);

		/**
		 * @brief	Minimize, Maximize or Restore a window
		 * @pre		GWIN_NEED_WINDOWMANAGER must be TRUE
		 *
		 * @param[in] gh				The window
		 * @param[in] minmax			The new minimized/maximized state
		 *
		 * @note	The final window state may not be the requested state. Window Managers
		 * 			do not need to implement changing the minmax state. If there is no
		 * 			window manager this call is ignored.
		 * @note	The window is redrawn if it is changed. See the comments in @p gwinSetVisible()
		 * 			with regard to what can be redrawn and what can't.
		 * @note	It is up to the window manager to determine what happens with any screen area
		 * 			uncovered by resizing the window.
		 * @note	When a window is minimised it may be asked to draw the window or the window
		 * 			manager may draw the minimised window.
		 *
		 * @api
		 */
		void gwinSetMinMax(GHandle gh, GWindowMinMax minmax);

		/**
		 * @brief	Get the Minimized/Maximized state of a window
		 * @pre		GWIN_NEED_WINDOWMANAGER must be TRUE
		 *
		 * @param[in] gh				The window
		 *
		 * @return	GWIN_NORMAL, GWIN_MAXIMIZE or GWIN_MINIMIZE
		 *
		 * @api
		 */
		GWindowMinMax gwinGetMinMax(GHandle gh);

		/**
		 * @brief	Raise a window to the top of the z-order
		 * @pre		GWIN_NEED_WINDOWMANAGER must be TRUE
		 *
		 * @param[in] gh				The window
		 *
		 * @note	The window z-order is only supported by some window managers. See the comments
		 * 			in @p gwinSetVisible() with regard to what can be redrawn and what can't.
		 *
		 * @api
		 */
		void gwinRaise(GHandle gh);

		/**
		 * @brief	Get the next window in the z-order
		 * @return	The next window or NULL if no more windows
		 *
		 * @param[in] gh		The previous window or NULL to get the first window
		 *
		 * @note	This returns the next window in the system from top to bottom.
		 * @note	Where there are parent child relationships, this ignores them
		 * 			and will list all windows in the system. There is no defined
		 * 			order between children of siblings and they can in fact be mixed
		 * 			in order. The only relationship honored is that parents will be
		 * 			listed before their children.
		 *
		 * @api
		 */
		GHandle gwinGetNextWindow(GHandle gh);

		/**
		 * @brief	Set a window or widget to flash
		 *
		 * @param[in] gh		The window handle
		 * @param[in] flash		Enable or disable the flashing of the window
		 *
		 * @note	The window is automatically redrawn if it supports self-redrawing.
		 * @note	When a window is set to flash, its appearance changes in some
		 * 			way every flash period (GWIN_FLASHING_PERIOD). How its appearance
		 * 			changes depends on the draw for each window/widget.
		 *
		 * @pre		Requires GWIN_NEED_FLASHING to be TRUE
		 *
		 * @api
		 */
		void gwinSetFlashing(GHandle gh, bool_t flash);

		/**
		 * @brief	Enables flashing of a window or widget
		 *
		 * @param[in] gh		The window handle
		 *
		 * @api
		 */
		#define gwinFlash(gh)		gwinSetFlashing(gh, TRUE)

		/**
		 * @brief	Disables a widget
		 *
		 * @param[in] gh		The window handle
		 *
		 * @api
		 */
		#define gwinNoFlash(gh)		gwinSetFlashing(gh, FALSE)
	#endif

	#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
		/**
		 * @brief   Set the current font for this window.
		 *
		 * @param[in] gh		The window handle
		 * @param[in] font		The font to use for text functions
		 *
		 * @api
		 */
		void gwinSetFont(GHandle gh, font_t font);
	#endif

/*-------------------------------------------------
 * Drawing functions
 *-------------------------------------------------*/

	/**
	 * @brief   Clear the window
	 * @note	Uses the current background color to clear the window
	 *
	 * @param[in] gh		The window handle
	 *
	 * @api
	 */
	void gwinClear(GHandle gh);

	/**
	 * @brief   Set a pixel in the window
	 * @note	Uses the current foreground color to set the pixel
	 * @note	May leave GDISP clipping to this window's dimensions
	 *
	 * @param[in] gh		The window handle
	 * @param[in] x,y		The coordinates of the pixel
	 *
	 * @api
	 */
	void gwinDrawPixel(GHandle gh, coord_t x, coord_t y);

	/**
	 * @brief   Draw a line in the window
	 * @note	Uses the current foreground color to draw the line
	 * @note	May leave GDISP clipping to this window's dimensions
	 *
	 * @param[in] gh		The window handle
	 * @param[in] x0,y0		The start position
	 * @param[in] x1,y1 	The end position
	 *
	 * @api
	 */
	void gwinDrawLine(GHandle gh, coord_t x0, coord_t y0, coord_t x1, coord_t y1);

	/**
	 * @brief   Draw a box in the window
	 * @note	Uses the current foreground color to draw the box
	 * @note	May leave GDISP clipping to this window's dimensions
	 *
	 * @param[in] gh		The window handle
	 * @param[in] x,y		The start position
	 * @param[in] cx,cy		The size of the box (outside dimensions)
	 *
	 * @api
	 */
	void gwinDrawBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy);

	/**
	 * @brief   Fill an rectangular area in the window
	 * @note	Uses the current foreground color to fill the box
	 * @note	May leave GDISP clipping to this window's dimensions
	 *
	 * @param[in] gh		The window handle
	 * @param[in] x,y		The start position
	 * @param[in] cx,cy		The size of the box (outside dimensions)
	 *
	 * @api
	 */
	void gwinFillArea(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy);

	/**
	 * @brief   Fill an area in the window using the supplied bitmap.
	 * @details The bitmap is in the pixel format specified by the low level driver
	 * @note	If GDISP_NEED_ASYNC is defined then the buffer must be static
	 * 			or at least retained until this call has finished the blit. You can
	 * 			tell when all graphics drawing is finished by @p gdispIsBusy() going FALSE.
	 * @note	May leave GDISP clipping to this window's dimensions
	 *
	 * @param[in] gh		The window handle
	 * @param[in] x, y		The start filled area
	 * @param[in] cx, cy	The width and height to be filled
	 * @param[in] srcx, srcy	The bitmap position to start the fill from
	 * @param[in] srccx		The width of a line in the bitmap.
	 * @param[in] buffer	The pixels to use to fill the area.
	 *
	 * @api
	 */
	void gwinBlitArea(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer);

/*-------------------------------------------------
 * Circle, ellipse, arc and arc-sectors functions
 *-------------------------------------------------*/

	#if GDISP_NEED_CIRCLE || defined(__DOXYGEN__)
		/**
		 * @brief   Draw a circle in the window.
		 * @note	Uses the current foreground color to draw the circle
		 * @note	May leave GDISP clipping to this window's dimensions
		 *
		 * @param[in] gh		The window handle
		 * @param[in] x, y		The center of the circle
		 * @param[in] radius	The radius of the circle
		 *
		 * @api
		 */
		void gwinDrawCircle(GHandle gh, coord_t x, coord_t y, coord_t radius);

		/**
		 * @brief   Draw a filled circle in the window.
		 * @note	Uses the current foreground color to draw the filled circle
		 * @note	May leave GDISP clipping to this window's dimensions
		 *
		 * @param[in] gh		The window handle
		 * @param[in] x, y		The center of the circle
		 * @param[in] radius	The radius of the circle
		 *
		 * @api
		 */
		void gwinFillCircle(GHandle gh, coord_t x, coord_t y, coord_t radius);
	#endif

	#if GDISP_NEED_ELLIPSE || defined(__DOXYGEN__)
		/**
		 * @brief   Draw an ellipse.
		 * @note	Uses the current foreground color to draw the ellipse
		 * @note	May leave GDISP clipping to this window's dimensions
		 *
		 * @param[in] gh		The window handle
		 * @param[in] x,y		The center of the ellipse
		 * @param[in] a,b		The dimensions of the ellipse
		 *
		 * @api
		 */
		void gwinDrawEllipse(GHandle gh, coord_t x, coord_t y, coord_t a, coord_t b);

		/**
		 * @brief   Draw an filled ellipse.
		 * @note	Uses the current foreground color to draw the filled ellipse
		 * @note	May leave GDISP clipping to this window's dimensions
		 *
		 * @param[in] gh		The window handle
		 * @param[in] x,y		The center of the ellipse
		 * @param[in] a,b		The dimensions of the ellipse
		 *
		 * @api
		 */
		void gwinFillEllipse(GHandle gh, coord_t x, coord_t y, coord_t a, coord_t b);
	#endif

	#if GDISP_NEED_ARC || defined(__DOXYGEN__)
		/*
		 * @brief	Draw an arc in the window.
		 * @note	Uses the current foreground color to draw the arc
		 * @note	May leave GDISP clipping to this window's dimensions
		 *
		 * @param[in] gh		The window handle
		 * @param[in] x,y		The center point
		 * @param[in] radius	The radius of the arc
		 * @param[in] start		The start angle (0 to 360)
		 * @param[in] end		The end angle (0 to 360)
		 *
		 * @api
		 */
		void gwinDrawArc(GHandle gh, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle);

		/*
		 * @brief	Draw a filled arc in the window.
		 * @note	Uses the current foreground color to draw the filled arc
		 * @note	May leave GDISP clipping to this window's dimensions
		 *
		 * @param[in] gh		The window handle
		 * @param[in] x,y		The center point
		 * @param[in] radius	The radius of the arc
		 * @param[in] start		The start angle (0 to 360)
		 * @param[in] end		The end angle (0 to 360)
		 *
		 * @api
		 */
		void gwinFillArc(GHandle gh, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle);
	#endif

	#if GDISP_NEED_ARCSECTORS || defined(__DOXYGEN__)
		/*
		 * @brief	Draw a selection of 45 degree arcs of a circle in the window.
		 * @note	Uses the current foreground color to draw the arc sector
		 * @note	May leave GDISP clipping to this window's dimensions
		 *
		 * @param[in] gh		The window handle
		 * @param[in] x,y		The center of the circle
		 * @param[in] radius	The radius of the circle
		 * @param[in] sectors	Bits determine which sectors are drawn.
		 * 						Bits go anti-clockwise from the 0 degree mark (y = 0, x is positive), as follows:
		 *  						bit 0 - upper right right		  -----
		 *  						bit 1 - upper upper right		 /2   1\
		 *  						bit 2 - upper upper left		/3     0\
		 *  						bit 3 - upper left  left		\4     7/
		 *  						bit 4 - lower left  left		 \5   6/
		 *  						bit 5 - lower lower left		  -----
		 *  						bit 6 - lower lower right
		 *  						bit 7 - lower left  left
		 *
		 * @api
		 */
		void gwinDrawArcSectors(GHandle gh, coord_t x, coord_t y, coord_t radius, uint8_t sectors);

		/*
		 * @brief	Draw a filled selection of 45 degree arcs of a circle in the window.
		 * @note	Uses the current foreground color to draw the arc sector
		 * @note	May leave GDISP clipping to this window's dimensions
		 *
		 * @param[in] gh		The window handle
		 * @param[in] x,y		The center of the circle
		 * @param[in] radius	The radius of the circle
		 * @param[in] sectors	Bits determine which sectors are drawn.
		 * 						Bits go anti-clockwise from the 0 degree mark (y = 0, x is positive), as follows:
		 *  						bit 0 - upper right right		  -----
		 *  						bit 1 - upper upper right		 /2   1\
		 *  						bit 2 - upper upper left		/3     0\
		 *  						bit 3 - upper left  left		\4     7/
		 *  						bit 4 - lower left  left		 \5   6/
		 *  						bit 5 - lower lower left		  -----
		 *  						bit 6 - lower lower right
		 *  						bit 7 - lower left  left
		 *
		 * @api
		 */
		void gwinFillArcSectors(GHandle gh, coord_t x, coord_t y, coord_t radius, uint8_t sectors);
	#endif

/*-------------------------------------------------
 * Pixel read-back functions
 *-------------------------------------------------*/

	#if GDISP_NEED_PIXELREAD || defined(__DOXYGEN__)
		/**
		 * @brief   Get the color of a pixel in the window.
		 * @return  The color of the pixel.
		 * @note	May leave GDISP clipping to this window's dimensions
		 *
		 * @param[in] gh		The window handle
		 * @param[in] x,y		The position in the window
		 *
		 * @api
		 */
		color_t gwinGetPixelColor(GHandle gh, coord_t x, coord_t y);
	#endif

/*-------------------------------------------------
 * Text functions
 *-------------------------------------------------*/

	#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
		/**
		 * @brief   Draw a text character at the specified position in the window.
		 * @pre		The font must have been set.
		 * @note	Uses the current foreground color to draw the character
		 * @note	May leave GDISP clipping to this window's dimensions
		 *
		 * @param[in] gh		The window handle
		 * @param[in] x,y		The position for the text
		 * @param[in] c			The character to draw
		 *
		 * @api
		 */
		void gwinDrawChar(GHandle gh, coord_t x, coord_t y, char c);

		/**
		 * @brief   Draw a text character with a filled background at the specified position in the window.
		 * @pre		The font must have been set.
		 * @note	Uses the current foreground color to draw the character and fills the background using the background drawing color
		 * @note	May leave GDISP clipping to this window's dimensions
		 *
		 * @param[in] gh		The window handle
		 * @param[in] x,y		The position for the text
		 * @param[in] c			The character to draw
		 *
		 * @api
		 */
		void gwinFillChar(GHandle gh, coord_t x, coord_t y, char c);

		/**
		 * @brief   Draw a text string in the window
		 * @pre		The font must have been set.
		 * @note	Uses the current foreground color to draw the character
		 * @note	May leave GDISP clipping to this window's dimensions
		 *
		 * @param[in] gh		The window handle
		 * @param[in] x,y		The position for the text
		 * @param[in] str		The string to draw
		 *
		 * @api
		 */
		void gwinDrawString(GHandle gh, coord_t x, coord_t y, const char *str);

		/**
		 * @brief   Draw a text string with a filled background in the window
		 * @pre		The font must have been set.
		 * @note	Uses the current foreground color to draw the character and fills the background using the background drawing color
		 * @note	May leave GDISP clipping to this window's dimensions
		 *
		 * @param[in] gh		The window handle
		 * @param[in] x,y		The position for the text
		 * @param[in] str		The string to draw
		 *
		 * @api
		 */
		void gwinFillString(GHandle gh, coord_t x, coord_t y, const char *str);

		/**
		 * @brief   Draw a text string verticly centered within the specified box.
		 * @pre		The font must have been set.
		 * @note	Uses the current foreground color to draw the character.
		 * @note    The specified box does not need to align with the window box
		 * @note	May leave GDISP clipping to this window's dimensions
		 *
		 * @param[in] gh		The window handle
		 * @param[in] x,y		The position for the text (need to define top-right or base-line - check code)
		 * @param[in] cx,cy		The width and height of the box
		 * @param[in] str		The string to draw
		 * @param[in] justify	Justify the text left, center or right within the box
		 *
		 * @api
		 */
		void gwinDrawStringBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, justify_t justify);

		/**
		 * @brief   Draw a text string verticly centered within the specified filled box.
		 * @pre		The font must have been set.
		 * @note	Uses the current foreground color to draw the character and fills the background using the background drawing color
		 * @note    The entire box is filled. Note this box does not need to align with the window box
		 * @note	May leave GDISP clipping to this window's dimensions
		 *
		 * @param[in] gh		The window handle
		 * @param[in] x,y		The position for the text (need to define top-right or base-line - check code)
		 * @param[in] cx,cy		The width and height of the box
		 * @param[in] str		The string to draw
		 * @param[in] justify	Justify the text left, center or right within the box
		 *
		 * @api
		 */
		void gwinFillStringBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, justify_t justify);
	#endif

/*-------------------------------------------------
 * Polygon functions
 *-------------------------------------------------*/

	#if GDISP_NEED_CONVEX_POLYGON || defined(__DOXYGEN__)
		/**
		 * @brief   Draw an enclosed polygon (convex, non-convex or complex).
		 *
		 * @note	Uses the current foreground color.
		 *
		 * @param[in] gh		The window handle
		 * @param[in] tx, ty	Transform all points in pntarray by tx, ty
		 * @param[in] pntarray	An array of points
		 * @param[in] cnt		The number of points in the array
		 *
		 * @api
		 */
		void gwinDrawPoly(GHandle gh, coord_t tx, coord_t ty, const point *pntarray, unsigned cnt);

		/**
		 * @brief   Fill a convex polygon
		 * @details Doesn't handle non-convex or complex polygons.
		 *
		 * @note	Uses the current foreground color.
		 *
		 * @param[in] gh		The window handle
		 * @param[in] tx, ty	Transform all points in pntarray by tx, ty
		 * @param[in] pntarray	An array of points
		 * @param[in] cnt		The number of points in the array
		 *
		 * @note	Convex polygons are those that have no internal angles. That is;
		 * 			you can draw a line from any point on the polygon to any other point
		 * 			on the polygon without it going outside the polygon. In our case we generalise
		 * 			this a little by saying that an infinite horizontal line (at any y value) will cross
		 * 			no more than two edges on the polygon. Some non-convex polygons do fit this criteria
		 * 			and can therefore be drawn.
		 * @note	This routine is designed to be very efficient with even simple display hardware.
		 *
		 * @api
		 */
		void gwinFillConvexPoly(GHandle gh, coord_t tx, coord_t ty, const point *pntarray, unsigned cnt);
	
		/**
		 * @brief	Draw a thick line in the window
		 * @details	The line thickness is specified in pixels. The line ends can
		 *		be selected to be either flat or round.
		 * @note	Uses gdispGFillConvexPoly() internally to perform the drawing.
		 * @note	Uses the current foreground color to draw the line
		 * 
		 * @param[in] gh		The window handle
		 * @param[in] x0,y0		The start position
		 * @param[in] x1,y1		The end position
		 * @param[in] width		The width of the line
		 * @param[in] round		Use round ends for the line
		 * 
		 * @api
		 */
		void gwinDrawThickLine(GHandle gh, coord_t x0, coord_t y0, coord_t x1, coord_t y1, coord_t width, bool_t round);
	#endif

/*-------------------------------------------------
 * Image functions
 *-------------------------------------------------*/

	#if GDISP_NEED_IMAGE || defined(__DOXYGEN__)
		/**
		 * @brief	Draw the image
		 * @return	GDISP_IMAGE_ERR_OK (0) on success or an error code.
		 *
		 * @param[in] gh		The window handle
		 * @param[in] img   	The image structure
		 * @param[in] x,y		The window location to draw the image
		 * @param[in] cx,cy		The area on the screen to draw
		 * @param[in] sx,sy		The image position to start drawing at
		 *
		 * @pre		gdispImageOpen() must have returned successfully.
		 *
		 * @note	If sx,sy + cx,cy is outside the image boundaries the area outside the image
		 * 			is simply not drawn.
		 * @note	If @p gdispImageCache() has been called first for this frame, this routine will draw using a
		 * 			fast blit from the cached frame. If not, it reads the input and decodes it as it
		 * 			is drawing. This may be significantly slower than if the image has been cached (but
		 * 			uses a lot less RAM)
		 *
		 * @api
		 */
		gdispImageError gwinDrawImage(GHandle gh, gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy);
	#endif

#ifdef __cplusplus
}
#endif

/*-------------------------------------------------
 * Additional functionality
 *-------------------------------------------------*/

	/* Include widgets */
	#if GWIN_NEED_WIDGET || defined(__DOXYGEN__)
		#include "gwin_widget.h"
	#endif

	/* Include containers */
	#if GWIN_NEED_CONTAINERS || defined(__DOXYGEN__)
		#include "gwin_container.h"
	#endif

	/* Include vanilla window objects */
	#if GWIN_NEED_CONSOLE || defined(__DOXYGEN__)
		#include "gwin_console.h"
	#endif
	#if GWIN_NEED_GRAPH || defined(__DOXYGEN__)
		#include "gwin_graph.h"
	#endif
	#if GWIN_NEED_IMAGE || defined(__DOXYGEN__)
		#include "gwin_image.h"
	#endif
	#if GWIN_NEED_GL3D || defined(__DOXYGEN__)
		#include "gwin_gl3d.h"
	#endif

#endif /* GFX_USE_GWIN */

#endif /* _GWIN_H */
/** @} */
