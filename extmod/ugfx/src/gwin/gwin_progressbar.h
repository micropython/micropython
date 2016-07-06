/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_progressbar.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @defgroup Progressbar Progressbar
 * @ingroup Widgets
 *
 * @brief		ProgressBar widget.
 *
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_PROGRESSBAR must be set to TRUE in your gfxconf.h
 * @{
 */

#ifndef _GWIN_PROGRESSBAR_H
#define _GWIN_PROGRESSBAR_H

/* This file is included within src/gwin/gwin_widget.h */
// A progressbar window
typedef struct GProgressbarObject {
	GWidgetObject		w;
	coord_t				dpos;
	int					min;
	int					max;
	int					res;
	int					pos;
	#if GWIN_PROGRESSBAR_AUTO
		GTimer			gt;
	#endif
} GProgressbarObject;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Create a progressbar window.
 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] g			The GDisplay to display this window on
 * @param[in] gb		The GProgressbarObject structure to initialise. If this is NULL the structure is dynamically allocated.
 * @param[in] pInit		The initialization parameters to use
 *
 * @note				The drawing color and the background color get set to the current defaults. If you haven't called
 * 						@p gwinSetDefaultColor() or @p gwinSetDefaultBgColor() then these are White and Black respectively.
 * @note				The font gets set to the current default font. If you haven't called @p gwinSetDefaultFont() then there
 * 						is no default font and text drawing operations will no nothing.
 * @note				A progressbar remembers its normal drawing state. If there is a window manager then it is automatically
 * 						redrawn if the window is moved or its visibility state is changed.
 * @note				The initial progressbar range is from 0 to 100 with an initial position of 0.
 * @note				A progressbar does not take any GINPUT inputs.
 *
 * @api
 */
GHandle gwinGProgressbarCreate(GDisplay *g, GProgressbarObject *gb, const GWidgetInit *pInit);
#define gwinProgressbarCreate(w, pInit)			gwinGProgressbarCreate(GDISP, w, pInit)

/**
 * @brief   Set the progressbar range.
 *
 * @param[in] gh		The window handle (must be a progressbar window)
 * @param[in] min		The minimum value
 * @param[in] max		The maximum value
 *
 * @note				The defaults are 0 and 100
 * @note				Sets the position to the minimum value.
 * @note				The progressbar is not automatically drawn. Call gwinProgressbarDraw() after changing the range.
 *
 * @api
 */
void gwinProgressbarSetRange(GHandle gh, int min, int max);

/**
 * @brief   Set the progressbar position.
 *
 * @param[in] gh		The window handle (must be a progressbar window)
 * @param[in] pos		The new position
 *
 * @note				If the new position is outside the progressbar range then the position
 * 						is set to the closest end of the range.
 * @note				The progressbar is not automatically drawn. Call gwinProgressbarDraw() after changing the position.
 *
 * @api
 */
void gwinProgressbarSetPosition(GHandle gh, int pos);

/**
 * @brief	Set the resolution for the incrementation and decrementation of the progressbar
 *
 * @note	Default is set to 1
 *
 * @param[in] gh		The window handle (must be a progressbar window)
 * @param[in] res		The resolution to be set
 *
 * @api
 */
void gwinProgressbarSetResolution(GHandle gh, int res);

/**
 * @brief	Increment the progressbar value
 *
 * @details	Increments by the resolution set through gwinProgressbarSetResolution()
 *
 * @param[in] gh		The window handle (must be a progressbar window)
 *
 * @api
 */
void gwinProgressbarIncrement(GHandle gh);

/**
 * @brief	Decrement the progressbar value
 *
 * @details Decrements by the resolution set through gwinProgressbarSetResolution()
 *
 * @param[in] gh		The window handle (must be a progressbar window)
 *
 * @api
 */
void gwinProgressbarDecrement(GHandle gh);

/**
 * @brief   Get the current progressbar position.
 * @return	The progressbar position
 *
 * @param[in] gh		The window handle (must be a progressbar window)
 *
 * @note				The use of a listener to get the progressbar position is recommended if you
 * 						want continuous updates on the progressbar position.
 *
 * @api
 */
#define gwinProgressbarGetPosition(gh)		(((GProgressbarObject *)(gh))->pos)

 /**
  * @brief	Reset the progressbar to the minimum position
  *
  * @param[in] gh		The window handle (must be a progressbar window)
  *
  * @api
  */
#define gwinProgressbarReset(gh)			gwinProgressbarSetPosition(gh, ((GProgressbarObject *)(gh))->min)

#if GWIN_PROGRESSBAR_AUTO || defined(__DOXYGEN__)
	/**
	 * @brief	Automatically increments the progress bar
	 *
	 * @note	The delay is generated using the GTIMER module which is based on software/virtual timer.
	 *			Therefore, the delay is totally unprecise.
	 *
	 * @note	The progressbar incrementation starts at the current level. It is not reset to the minimum value.
	 *
	 * @note	An event is generated once the maximum value has been reached (ToDo)
	 *
	 * @param[in] gh		The window handle (must be a progressbar window)
	 * @param[in] delay		The incrementation delay (in milliseconds)
	 *
	 * @api
	 */
	void gwinProgressbarStart(GHandle gh, delaytime_t delay);

	/**
	 * @brief	Stop the timer which is started by @p gwinProgressbarStart()
	 *
	 * @param[in] gh		The window handle (must be a progressbar window)
	 *
	 * @api
	 */
	void gwinProgressbarStop(GHandle gh);
#endif /* GWIN_PROGRESSBAR_AUTO */

/**
 * @defgroup Renderings_Progressbar Renderings
 *
 * @brief				Built-in rendering functions for the progressbar widget.
 *
 * @details				These function may be passed to @p gwinSetCustomDraw() to get different progressbar drawing styles.
 *
 * @note				In your custom progressbar drawing function you may optionally call these
 * 						standard functions and then draw your extra details on top.
 * @note				These custom drawing routines don't have to worry about setting clipping as the framework
 * 						sets clipping to the object window prior to calling these routines.
 *
 * @{
 */

/**
 * @brief				The default rendering function for the progressbar widget
 *
 * @param[in] gw		The widget object (must be a progressbar object)
 * @param[in] param		A parameter passed in from the user. Ignored by this function.
 *
 * @api
 */
void gwinProgressbarDraw_Std(GWidgetObject *gw, void *param);

#if GDISP_NEED_IMAGE || defined(__DOXYGEN__)
	/**
	 * @brief				Renders a progressbar using an image.
	 *
	 * @param[in] gw		The widget object (must be a progressbar handle)
	 * @param[in] param		A parameter passed in from the user. Must be an image handle. See note below.
	 *
	 * @note				The image must be already opened before calling  @p gwinSetCustomDraw().
	 * @note				The image is tiled to fill the active area of the progressbar. The normal colors
	 *						apply to the border and inactive area and the dividing line between the active
	 *						and inactive areas. No checking is done to compare the dimensions of the progressbar
	 *						to the size of the image. Note text is drawn on top of the image.
	 *
	 * @pre					GDISP_NEED_IMAGE must be set to TRUE
	 *
	 * @api
	 */
	void gwinProgressbarDraw_Image(GWidgetObject *gw, void *param);
#endif /* GDISP_NEED_IMAGE */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GWIN_PROGRESSBAR_H */
/** @} */
