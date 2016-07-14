/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_slider.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @defgroup Slider Slider
 * @ingroup Widgets
 *
 * @brief		Slider widget.
 *
 * @details		Extended events can be enabled using @p gwinSliderSendExtendedEvents().
 *
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_SLIDER must be set to TRUE in your gfxconf.h
 * @{
 */

#ifndef _GWIN_SLIDER_H
#define _GWIN_SLIDER_H

/* This file is included within "src/gwin/gwin_widget.h" */

#define GEVENT_GWIN_SLIDER		(GEVENT_GWIN_CTRL_FIRST+1)

typedef struct GEventGWinSlider {
	GEventType		type;				// The type of this event (GEVENT_GWIN_SLIDER)
	GHandle			gwin;				// The slider that is returning results
	#if GWIN_WIDGET_TAGS
		WidgetTag	tag;				// The slider tag
	#endif
	int				position;

	uint8_t			action;
		#define GSLIDER_EVENT_SET		4		/* Slider position is set. This is the only event returned by default   */
		#define GSLIDER_EVENT_CANCEL	3		/* Slider position changing has been cancelled */
		#define GSLIDER_EVENT_START		2		/* Slider position has started changing */
		#define GSLIDER_EVENT_MOVE		1		/* Slider position has been moved */
} GEventGWinSlider;

// There are currently no GEventGWinSlider listening flags - use 0

/**
 * @brief	The internal slider object flags
 * @note	Used only for writing a custom draw routine.
 * @{
 */
#define GSLIDER_FLG_EXTENDED_EVENTS		0x01
/** @} */

// A slider window
typedef struct GSliderObject {
	GWidgetObject		w;
	#if GINPUT_NEED_TOGGLE
		uint16_t		t_dn;
		uint16_t		t_up;
	#endif
	#if GINPUT_NEED_DIAL
		uint16_t		dial;
	#endif
	coord_t				dpos;
	int					min;
	int					max;
	int					pos;
} GSliderObject;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Create a slider window.
 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] g			The GDisplay to display this window on
 * @param[in] gb		The GSliderObject structure to initialise. If this is NULL the structure is dynamically allocated.
 * @param[in] pInit		The initialization parameters to use
 *
 * @note				The drawing color and the background color get set to the current defaults. If you haven't called
 * 						@p gwinSetDefaultColor() or @p gwinSetDefaultBgColor() then these are White and Black respectively.
 * @note				The font gets set to the current default font. If you haven't called @p gwinSetDefaultFont() then there
 * 						is no default font and text drawing operations will no nothing.
 * @note				A slider remembers its normal drawing state. If there is a window manager then it is automatically
 * 						redrawn if the window is moved or its visibility state is changed.
 * @note				The initial slider range is from 0 to 100 with an initial position of 0.
 * @note				A slider supports mouse, toggle and dial input.
 * @note				When assigning a toggle, only one toggle is supported per role. If you try to assign more than
 * 						one toggle to a role it will forget the previous toggle. Two roles are supported:
 * 						Role 0 = toggle for down, Role 1 = toggle for up.
 * @note				When assigning a dial, only one dial is supported. If you try to assign more than one dial
 * 						it will forget the previous dial. Only dial role 0 is supported.
 *
 * @api
 */	
GHandle gwinGSliderCreate(GDisplay *g, GSliderObject *gb, const GWidgetInit *pInit);
#define gwinSliderCreate(w, pInit)			gwinGSliderCreate(GDISP, w, pInit)

/**
 * @brief   Set the slider range.
 *
 * @param[in] gh		The window handle (must be a slider window)
 * @param[in] min		The minimum value
 * @param[in] max		The maximum value
 * @note				Sets the position to the minimum value.
 * @note				The slider is not automatically drawn. Call gwinSliderDraw() after changing the range.
 *
 * @api
 */
void gwinSliderSetRange(GHandle gh, int min, int max);

/**
 * @brief   Set the slider position.
 *
 * @param[in] gh		The window handle (must be a slider window)
 * @param[in] pos		The new position
 * @note				If the new position is outside the slider range then the position
 * 						is set to the closest end of the range.
 * @note				The slider is not automatically drawn. Call gwinSliderDraw() after changing the position.
 *
 * @api
 */
void gwinSliderSetPosition(GHandle gh, int pos);

/**
 * @brief   Get the current slider position.
 * @return	The slider position
 *
 * @param[in] gh		The window handle (must be a slider window)
 *
 * @note	The use of a listener to get the slider position is recommended if you
 * 			want continuous updates on the slider position.
 *
 * @api
 */
#define gwinSliderGetPosition(gh)		(((GSliderObject *)(gh))->pos)

/**
 * @brief   Should the slider send extended events.
 *
 * @param[in] gh		The window handle (must be a slider window)
 * @param[in] enabled	TRUE to enable extended events, FALSE to disable them
 *
 * @note	The slider by default will only send slider events with an action of GSLIDER_EVENT_SET.
 * 			This call can be used to enable other slider action's to be sent as events
 *
 * @api
 */
void gwinSliderSendExtendedEvents(GHandle gh, bool_t enabled);

/**
 * @defgroup Renderings_Slider Renderings
 *
 * @brief				Built-in rendering functions for the slider widget.
 *
 * @details				These function may be passed to @p gwinSetCustomDraw() to get different slider drawing styles.
 *
 * @note				In your custom slider drawing function you may optionally call these
 * 						standard functions and then draw your extra details on top.
 * @note				These custom drawing routines don't have to worry about setting clipping as the framework
 * 						sets clipping to the object window prior to calling these routines.
 *
 * @{
 */

/**
 * @brief				The default rendering function for the slider widget.
 *
 * @param[in] gw		The widget object (must be a slider object).
 * @param[in] param		A parameter passed in from the user. Ignored by this function.
 *
 * @api
 */
void gwinSliderDraw_Std(GWidgetObject *gw, void *param);

#if GDISP_NEED_IMAGE || defined(__DOXYGEN__)
	/**
	 * @brief				The default rendering function
	 *
	 * @param[in] gw		The widget object (must be a slider object).
	 * @param[in] param		A parameter passed in from the user. Must be an image handle. See note below.
	 *
	 * @note				The image must be already opened before calling  @p gwinSetCustomDraw(). The image should be 3
	 * 						times the height of the button. The button image is repeated 3 times vertically, the first (top) for
	 * 						the "up" image, the 2nd for the "down" image, and the third (bottom) image for the disabled state. If
	 * 						the disabled state is never going to be used then the image can be just 2 times the button height.
	 * 						No checking is done to compare the size of the button to the size of the image.
	 * 						No text is drawn on top of the image.
	 *
	 * @pre					GDISP_NEED_IMAGE must be set to TRUE
	 *
	 * @api
	 */
void gwinSliderDraw_Image(GWidgetObject *gw, void *param);
#endif /* GDISP_NEED_IMAGE */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GWIN_SLIDER_H */
/** @} */
