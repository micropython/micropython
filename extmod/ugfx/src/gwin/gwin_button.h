/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_button.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @defgroup Button Button
 * @ingroup Widgets
 *
 * @brief		PushButton widget.
 *
 * @details		GWIN allows it to easily create buttons with different styles
 *				and check for different meta states such as: PRESSED, CLICKED,
 *				RELEASED etc.
 *
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_BUTTON must be set to TRUE in your gfxconf.h
 * @{
 */

#ifndef _GWIN_BUTTON_H
#define _GWIN_BUTTON_H

/* This file is included within "src/gwin/gwin_widget.h" */

/**
 * @brief	The Event Type for a Button Event
 */
#define GEVENT_GWIN_BUTTON		(GEVENT_GWIN_CTRL_FIRST+0)

/**
 * @brief	A Button Event
 * @note	There are currently no GEventGWinButton listening flags - use 0 as the flags to @p gwinAttachListener()
 */
typedef GEventGWin		GEventGWinButton;

/**
 * @brief	The internal button flags
 * @note	Used only for writing a custom draw routine.
 * @{
 */
#define GBUTTON_FLG_PRESSED		0x01
/** @} */

/**
 * @brief	The button widget structure
 * @note	Do not use the members directly - treat it as a black-box.
 */
typedef struct GButtonObject {
	GWidgetObject		w;
	#if GINPUT_NEED_TOGGLE
		uint16_t		toggle;
	#endif
} GButtonObject;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Create a button widget.
 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] g			The GDisplay to display this window on
 * @param[in] gb		The GButtonObject structure to initialise. If this is NULL the structure is dynamically allocated.
 * @param[in] pInit		The initialisation parameters
 *
 * @note				The drawing color and the background color get set to the current defaults. If you haven't called
 * 						@p gwinSetDefaultColor() or @p gwinSetDefaultBgColor() then these are White and Black respectively.
 * @note				The font gets set to the current default font. If you haven't called @p gwinSetDefaultFont() then there
 * 						is no default font and text drawing operations will no nothing.
 * @note				A button remembers its normal drawing state. If there is a window manager then it is automatically
 * 						redrawn if the window is moved or its visibility state is changed.
 * @note				A button supports mouse and a toggle input.
 * @note				When assigning a toggle, only one toggle is supported. If you try to assign more than one toggle it will
 * 						forget the previous toggle. When assigning a toggle the role parameter must be 0.
 *
 * @api
 */	
GHandle gwinGButtonCreate(GDisplay *g, GButtonObject *gb, const GWidgetInit *pInit);
#define gwinButtonCreate(gb, pInit)			gwinGButtonCreate(GDISP, gb, pInit)

/**
 * @brief	Is the button current pressed
 * @return	TRUE if the button is pressed
 *
 * @param[in] gh	The window handle (must be a button widget)
 *
 * @api
 */
bool_t gwinButtonIsPressed(GHandle gh);

/**
 * @defgroup Renderings_Button Renderings
 *
 * @brief				Built-in rendering functions for the button widget.
 *
 * @details				These function may be passed to @p gwinSetCustomDraw() to get different button drawing styles.
 *
 * @note				In your custom button drawing function you may optionally call these
 * 						standard functions and then draw your extra details on top.
 * @note				The standard functions below ignore the param parameter except for @p gwinButtonDraw_Image().
 * @note				The image custom draw function  @p gwinButtonDraw_Image() uses param to pass in the gdispImage pointer.
 * @note				These custom drawing routines don't have to worry about setting clipping as the framework
 * 						sets clipping to the object window prior to calling these routines.
 *
 * @{
 */

/**
 * @brief				The default rendering function for the button widget
 *
 * @param[in] gw		The widget object (must be a button object)
 * @param[in] param		A parameter passed in from the user. Ignored by this function.
 *
 * @api
 */
void gwinButtonDraw_Normal(GWidgetObject *gw, void *param);

#if GDISP_NEED_ARC || defined(__DOXYGEN__)
	/**
	 * @brief				Renders a rectangular button with rounded corners
	 *
	 * @param[in] gw		The widget object (must be a button object)
	 * @param[in] param		A parameter passed in from the user. Ignored by this function.
	 *
	 * @pre					GDISP_NEED_ARC must be set to TRUE
	 *
	 * @api
	 */
	void gwinButtonDraw_Rounded(GWidgetObject *gw, void *param);
#endif

#if GDISP_NEED_ELLIPSE || defined(__DOXYGEN__)
	/**
	 * @brief				Renders a button with an elliptical shape
	 *
	 * @param[in] gw		The widget object (must be a button object)
	 * @param[in] param		A parameter passed in from the user. Ignored by this function.
	 *
	 * @pre					GDISP_NEED_ELLIPSE must be set to TRUE
	 *
	 * @api
	 */
	void gwinButtonDraw_Ellipse(GWidgetObject *gw, void *param);
#endif

#if GDISP_NEED_CONVEX_POLYGON || defined(__DOXYGEN__)
	/**
	 * @brief				Renders a button in a shape of an arrow pointing up.
	 *
	 * @param[in] gw		The widget object (must be a button object)
	 * @param[in] param		A parameter passed in from the user. Ignored by this function.
	 *
	 * @pre					GDISP_NEED_CONVEX_POLYGON must be set to TRUE
	 *
	 * @api
	 */
	void gwinButtonDraw_ArrowUp(GWidgetObject *gw, void *param);

	/**
	 * @brief				Renders a button in a shape of an arrow pointing down.
	 *
	 * @param[in] gw		The widget object (must be a button object)
	 * @param[in] param		A parameter passed in from the user. Ignored by this function.
	 *
	 * @pre					GDISP_NEED_CONVEX_POLYGON must be set to TRUE
	 *
	 * @api
	 */
	void gwinButtonDraw_ArrowDown(GWidgetObject *gw, void *param);

	/**
	 * @brief				Renders a button in a shape of an arrow pointing left.
	 *
	 * @param[in] gw		The widget object (must be a button object)
	 * @param[in] param		A parameter passed in from the user. Ignored by this function.
	 *
	 * @pre					GDISP_NEED_CONVEX_POLYGON must be set to TRUE
	 *
	 * @api
	 */
	void gwinButtonDraw_ArrowLeft(GWidgetObject *gw, void *param);

	/**
	 * @brief				Renders a button in a shape of an arrow pointing right.
	 *
	 * @param[in] gw		The widget object (must be a button object)
	 * @param[in] param		A parameter passed in from the user. Ignored by this function.
	 *
	 * @pre					GDISP_NEED_CONVEX_POLYGON must be set to TRUE
	 *
	 * @api
	 */
	void gwinButtonDraw_ArrowRight(GWidgetObject *gw, void *param);
#endif

#if GDISP_NEED_IMAGE || defined(__DOXYGEN__)
	/**
	 * @brief				Renders a button using individual images for each button state.
	 *
	 * @param[in] gw		The widget object (must be a button object)
	 * @param[in] param		A parameter passed in from the user. Must be an image handle. See note below.
	 *
	 * @note				The image must be already opened before calling  @p gwinSetCustomDraw(). The image should be 3
	 * 						times the height of the button. The button image is repeated 3 times vertically, the first (top) for
	 * 						the "up" image, the 2nd for the "down" image, and the third (bottom) image for the disabled state. If
	 * 						the disabled state is never going to be used then the image can be just 2 times the button height.
	 * 						No checking is done to compare the size of the button to the size of the image.
	 * 						Note text is drawn on top of the image.
	 *
	 * @pre					GDISP_NEED_IMAGE must be set to TRUE
	 *
	 * @api
	 */
	void gwinButtonDraw_Image(GWidgetObject *gw, void *param);
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GWIN_BUTTON_H */
/** @} */

