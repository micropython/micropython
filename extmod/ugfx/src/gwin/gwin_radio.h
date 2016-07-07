/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_radio.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @defgroup RadioButton RadioButton
 * @ingroup Widgets
 *
 * @brief		RadioButton widget.
 *
 * @details		RadioButtons need to be grouped together. This is archived by passing a group parameter through
 *				@p gwinRadioCreate().
 *
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_RADIO must be set to TRUE in your gfxconf.h
 * @{
 */

#ifndef _GWIN_RADIO_H
#define _GWIN_RADIO_H

/* This file is included within "src/gwin/gwin_widget.h" */

/**
 * @brief	The Event Type for a Radio Event
 */
#define GEVENT_GWIN_RADIO		(GEVENT_GWIN_CTRL_FIRST+3)

/**
 * @brief	A Button Event
 * @note	There are currently no GEventGWinRadio listening flags - use 0 as the flags to @p gwinAttachListener()
 */
typedef struct GEventGWinRadio {
	GEventType		type;				// The type of this event (GEVENT_GWIN_RADIO)
	GHandle			gwin;				// The radio button that has been depressed
	#if GWIN_WIDGET_TAGS
		WidgetTag	tag;				// The radio tag
	#endif
	uint16_t		group;				// The group for this radio button
} GEventGWinRadio;

/**
 * @brief	The internal radio button object flags
 * @note	Used only for writing a custom draw routine.
 * @{
 */
#define GRADIO_FLG_PRESSED		0x01
/** @} */

/**
 * @brief	The radio button widget structure
 * @note	Do not use the members directly - treat it as a black-box.
 */
typedef struct GRadioObject {
	GWidgetObject		w;
	#if GINPUT_NEED_TOGGLE
		uint16_t		toggle;
	#endif
	uint16_t			group;
} GRadioObject;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Create a radio widget.
 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] g			The GDisplay to display this window on
 * @param[in] gb		The GRadioObject structure to initialise. If this is NULL the structure is dynamically allocated.
 * @param[in] pInit		The initialisation parameters
 * @param[in] group		The group of radio buttons this radio button belongs to.
 *
 * @note				Only one radio button in any group is ever pressed at one time. Pressing one radio button will
 * 						release all others in the group.
 * @note				The drawing color and the background color get set to the current defaults. If you haven't called
 * 						@p gwinSetDefaultColor() or @p gwinSetDefaultBgColor() then these are White and Black respectively.
 * @note				The font gets set to the current default font. If you haven't called @p gwinSetDefaultFont() then there
 * 						is no default font and text drawing operations will no nothing.
 * @note				A radio button remembers its normal drawing state. If there is a window manager then it is automatically
 * 						redrawn if the window is moved or its visibility state is changed.
 * @note				A radio button supports mouse and a toggle input.
 * @note				When assigning a toggle, only one toggle is supported. If you try to assign more than one toggle it will
 * 						forget the previous toggle. When assigning a toggle the role parameter must be 0.
 *
 * @api
 */	
GHandle gwinGRadioCreate(GDisplay *g, GRadioObject *gb, const GWidgetInit *pInit, uint16_t group);
#define gwinRadioCreate(w, pInit, gr)			gwinGRadioCreate(GDISP, w, pInit, gr)

/**
 * @brief	Press this radio button (and by definition unset any others in the group)
 *
 * @param[in] gh	The window handle (must be a radio widget)
 *
 * @api
 */
void gwinRadioPress(GHandle gh);

/**
 * @brief	Is the radio button currently pressed
 * @return	TRUE if the button is pressed
 *
 * @param[in] gh	The window handle (must be a radio widget)
 *
 * @api
 */
bool_t gwinRadioIsPressed(GHandle gh);

/**
 * @brief	Find the currently pressed radio button in the specified group
 * @return	The handle of the pressed radio button or NULL if none are pressed
 *
 * @param[in] group		The radio button group to be examined
 *
 * @return	The handle of the currently pressed radio button
 *
 * @api
 */
GHandle gwinRadioGetActive(uint16_t group);

/**
 * @defgroup Renderings_Radiobutton Renderings
 *
 * @brief				Built-in rendering functions for the radiobutton widget.
 *
 * @details				These function may be passed to @p gwinSetCustomDraw() to get different radiobutton drawing styles.
 *
 * @note				In your custom radiobutton drawing function you may optionally call these
 * 						standard functions and then draw your extra details on top.
 * @note				These custom drawing routines don't have to worry about setting clipping as the framework
 * 						sets clipping to the object window prior to calling these routines.
 *
 * @{
 */

/**
 * @brief				The default rendering function for the radiobutton widget
 *
 * @param[in] gw		The widget object (must be a button radioobject)
 * @param[in] param		A parameter passed in from the user. Ignored by this function.
 *
 * @api
 */
void gwinRadioDraw_Radio(GWidgetObject *gw, void *param);

/**
 * @brief				Renders the radiobutton in form of a regular rectangular button
 *
 * @param[in] gw		The widget object (must be a button radioobject)
 * @param[in] param		A parameter passed in from the user. Ignored by this function.
 *
 * @api
 */
void gwinRadioDraw_Button(GWidgetObject *gw, void *param);

/**
 * @brief				Used to render tabbed menus.
 *
 * @details				Multiple radiobutton widgets can be placed right next to each other and be used to implement
 *						a tabbed menu using this rendering function.
 *
 * @note				This exists for legacy reasons. Have a look at the @p Tabset widget instead.
 *
 * @param[in] gw		The widget object (must be a button radioobject)
 * @param[in] param		A parameter passed in from the user. Ignored by this function.
 *
 * @api
 */
void gwinRadioDraw_Tab(GWidgetObject *gw, void *param);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GWIN_RADIO_H */
/** @} */
