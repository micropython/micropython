/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_keyboard.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @defgroup VirtualKeyboard VirtualKeyboard
 * @ingroup Widgets
 *
 * @brief		Keyboard widget. Used to provide a virtual on-screen keyboard.
 *
 * @details		GWIN allows it to easily create buttons with different styles
 *				and check for different meta states such as: PRESSED, CLICKED,
 *				RELEASED etc.
 *
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_BUTTON must be set to TRUE in your gfxconf.h
 * @{
 */

#ifndef _GWIN_KEYBOARD_H
#define _GWIN_KEYBOARD_H

/* This file is included within "src/gwin/gwin_widget.h" */

/**
 * @brief	The Event Type for a Button Event
 */
#define GEVENT_GWIN_KEYBOARD		(GEVENT_GWIN_CTRL_FIRST+6)

/**
 * @brief	A Keyboard Event
 * @note	There are currently no GEventGWinButton listening flags - use 0 as the flags to @p gwinAttachListener()
 */
typedef GEventGWin		GEventGWinKeyboard;

/**
 * @brief	The internal keyboard flags and other defines
 * @note	Used only for writing a custom draw routine.
 * @{
 */
#define GKEYBOARD_FLG_REVERTSET		0x01
#define GKEYBOARD_FLG_QUICKUPDATE	0x02
#define GKEY_BAD_ROWCOL				255
/** @} */

/**
 * @brief	The keyboard widget structure
 * @note	Do not use the members directly - treat it as a black-box.
 */
typedef struct GKeyboardObject {
	GWidgetObject			w;
	const struct GVKeyTable	*keytable;
	const char				**keyset;
	coord_t					keyx, keyy;
	coord_t					keycx, keycy;
	uint8_t					lastkeyrow, lastkeycol;
	uint8_t					keyrow, keycol;
	uint32_t				key;
	#if GINPUT_NEED_TOGGLE
		uint16_t	t_up;
		uint16_t	t_down;
		uint16_t	t_left;
		uint16_t	t_right;
		uint16_t	t_press;
		uint8_t		t_row;
		uint8_t		t_lastrow;
		uint8_t		t_col;
		uint8_t		t_lastcol;
	#endif
} GKeyboardObject;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Create a keyboard widget.
 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] g			The GDisplay to display this window on
 * @param[in] gb		The GKeyboardObject structure to initialise. If this is NULL the structure is dynamically allocated.
 * @param[in] pInit		The initialisation parameters
 *
 * @note				The drawing color and the background color get set to the current defaults. If you haven't called
 * 						@p gwinSetDefaultColor() or @p gwinSetDefaultBgColor() then these are White and Black respectively.
 * @note				The font gets set to the current default font. If you haven't called @p gwinSetDefaultFont() then there
 * 						is no default font and text drawing operations will no nothing.
 * @note				A keyboard remembers its normal drawing state. If there is a window manager then it is automatically
 * 						redrawn if the window is moved or its visibility state is changed.
 * @note				A keyboard supports mouse input.
 *
 * @api
 */	
GHandle gwinGKeyboardCreate(GDisplay *g, GKeyboardObject *gb, const GWidgetInit *pInit);
#define gwinKeyboardCreate(gb, pInit)			gwinGKeyboardCreate(GDISP, gb, pInit)

/**
 * @brief   Get the keyboard event source for a GWIN virtual keyboard
 * @return	The event source handle or NULL if this is not a virtual keyboard
 *
 * @param[in] gh		The GWIN virtual keyboard
 *
 * @note	Normal GINPUT Keyboard events are returned by this event source.
 */
GSourceHandle gwinKeyboardGetEventSource(GHandle gh);

/**
 * @brief   Set the layout for the virtual keyboard
 *
 * @param[in] gh		The GWIN virtual keyboard
 * @param[in] layout	The keyboard layout to use (described by gwin_keyboard_layout.h)
 *
 * @note	Changing the layout resets the keyboard to key set 0 of the keyboard and cancels any
 * 			pending shifts.
 */
void gwinKeyboardSetLayout(GHandle gh, const struct GVKeyTable *layout);

/**
 * @brief   Get the selected key when using toggle
 *
 * @param[in] gh		The GWIN virtual keyboard
 * @param[in] utf8_str	Up to 4 bytes of utf8 data
 * @return              The number of bytes written
 *
 * @note	Returns 0 for an invalid selection
 */
uint8_t gwinKeyboardGetSelected(GHandle gh, uint8_t *utf8_str);

/**
 * @defgroup Renderings_Keyboard Renderings
 *
 * @brief				Built-in rendering functions for the keyboard widget.
 *
 * @details				These function may be passed to @p gwinSetCustomDraw() to get different keyboard drawing styles.
 *
 * @note				In your custom keyboard drawing function you may optionally call these
 * 						standard functions and then draw your extra details on top.
 * @note				The built-in functions below ignore the param parameter.
 * @note				These custom drawing routines don't have to worry about setting clipping as the framework
 * 						sets clipping to the object window prior to calling these routines.
 *
 * @{
 */

/**
 * @brief				The default rendering function for the keyboard widget
 *
 * @param[in] gw		The widget object (must be a keyboard object)
 * @param[in] param		A parameter passed in from the user. Ignored by this function.
 *
 * @api
 */
void gwinKeyboardDraw_Normal(GWidgetObject *gw, void *param);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GWIN_KEYBOARD_H */
/** @} */
