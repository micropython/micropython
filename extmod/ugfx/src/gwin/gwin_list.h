/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file	src/gwin/gwin_list.h
 * @brief	GWIN list widget header file
 *
 * @defgroup List List
 * @ingroup Widgets
 *
 * @brief		List Widget. Used to display lists of items.
 *
 * @details		Provides advanced features such as multi-selection, smooth scrolling and item icons.
 *
 * @pre			GFX_USE_GDISP must be set to TRUE in your gfxconf.h
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GDISP_NEED_TEXT must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_LIST must be set to TRUE in your gfxconf.h
 * @pre			The font you want to use must be enabled in your gfxconf.h
 *
 * @{
 */

#ifndef _GWIN_LIST_H
#define _GWIN_LIST_H

// This file is included within "src/gwin/gwin_widget.h"

/**
 * @brief	The event type for a list event
 */
#define GEVENT_GWIN_LIST	(GEVENT_GWIN_CTRL_FIRST+4)

/**
 * @brief	A list event
 */
typedef struct GEventGWinList {
	GEventType		type;		// The type of this event (GEVENT_GWIN_LIST)
	GHandle			gwin;		// The list
	#if GWIN_WIDGET_TAGS
		WidgetTag	tag;		// The list tag
	#endif
	int				item;		// The item that has been selected (or unselected in a multi-select listbox)
} GEventGWinList;

// A list window
typedef struct GListObject {
	GWidgetObject	w;

    #if GINPUT_NEED_MOUSE
        coord_t start_mouse_x;
        coord_t start_mouse_y;
        coord_t last_mouse_y;
    #endif
	#if GINPUT_NEED_TOGGLE
		uint16_t	t_up;
		uint16_t	t_dn;
	#endif

	int				cnt;		// Number of items currently in the list (quicker than counting each time)
	int				top;		// Viewing offset in pixels from the top of the list
	gfxQueueASync	list_head;	// The list of items
} GListObject;

/**
 * @brief	Enum to change the behaviour of the scroll bar
 *
 * @note	Used with @p gwinListSetScroll()
 * @note    @p scrollAlways always show the scrollbar
 * @note    @p scrollAuto show the scrollbar when there are more items on the list then fit on the screen
 * @note    @p scrollSmooth enable touch screen smooth scrolling
 */
typedef enum scroll_t { scrollAlways, scrollAuto, scrollSmooth } scroll_t;

/**
 * @brief	The internal list object flags
 * @note	Used only for writing a custom draw routine.
 * @{
 */
#define GLIST_FLG_MULTISELECT		0x01
#define GLIST_FLG_HASIMAGES			0x02
#define GLIST_FLG_SCROLLALWAYS		0x04
#define GLIST_FLG_SCROLLSMOOTH      0x08
#define GLIST_FLG_ENABLERENDER      0x10
/** @} */

/**
 * @brief	The internal list item structure
 * @note	Used only for writing a custom draw routine.
 */
typedef struct ListItem {
	gfxQueueASyncItem	q_item;		// This must be the first member in the struct

	uint16_t			flags;
		#define GLIST_FLG_SELECTED			0x0001
	uint16_t			param;		// A parameter the user can specify himself
	const char*			text;
	#if GWIN_NEED_LIST_IMAGES
		gdispImage*		pimg;
	#endif
} ListItem;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief				Create a list widget
 *
 * @note				The drawing color and the background color get set to the current defaults. If you haven't called
 *						@p gwinSetDefaultColor() or @p gwinSetDefaultBgColor() then these are Black and White.
 * @note				The font gets set to the current default font. If you haven't called @p gwinSetDefaultFont() then
 *						there is no default font and text drawing operations will not display anything.
 * @note				A list remembers its normal drawing state. If there is a window manager then it is automatically
 *						redrawn if the window is moved or its visibility state is changed.
 * @note				The list contains no elements after creation.
 * @note				A slider supports mouse, toggle. Note: toggle only works correctly for single-select lists.
 * @note				When assigning a toggle, only one toggle is supported per role. If you try to assign more than
 *						one toggle to a role, it will forget the previous toggle. Two roles are supported:
 *						Role 0 = toggle for down, role 1 = toggle for up
 *
 * @param[in] g			The GDisplay to display this window on
 * @param[in] widget	The GListObject structure to initialize. If this is NULL, the structure is dynamically allocated.
 * @param[in] pInit		The initialization parameters to use
 * @param[in] multiselect	If TRUE the list is multi-select instead of single-select.
 *
 * @return				NULL if there is no resulting drawing area, otherwise a window handle.
 *
 * @api
 */
GHandle gwinGListCreate(GDisplay *g, GListObject *widget, GWidgetInit *pInit, bool_t multiselect);
#define gwinListCreate(w, pInit, m)			gwinGListCreate(GDISP, w, pInit, m)

/**
 * @brief				Enable or disable the rendering of the list
 *
 * @details				Usually the list is being re-rendered when an item is added to the list. This can cause
 * 						flickering and performance issues when many items are added at once. This can be prevented
 *						by temporarely disabling the render using this function.
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] ena		TRUE or FALSE
 *
 * @api
 */
void gwinListEnableRender(GHandle gh, bool_t ena);

/**
 * @brief				Change the behaviour of the scroll bar
 *
 * @note				Current possible values: @p scrollAlways, @p scrollAuto and  @p scrollSmooth
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] flag		The behaviour to be set
 *
 * @api
 */
void gwinListSetScroll(GHandle gh, scroll_t flag);

/**
 * @brief				Add an item to the list
 *
 * @note				The ID you get returned is not static. If items get removed from the list, the list items get
 * 						reordered.
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] item		The string which shall be displayed in the list afterwards
 * @param[in] useAlloc	If set to TRUE, the string will be dynamically allocated. A static buffer must be passed otherwise
 *
 * @return				The current ID of the item. The ID might change if you remove items from the middle of the list
 *
 * @api
 */
int gwinListAddItem(GHandle gh, const char* item, bool_t useAlloc);

/**
 * @brief				Get the name behind an item with a given ID
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] item		The item ID
 *
 * @return				The string of the list item or NULL on error
 *
 * @api
 */
const char* gwinListItemGetText(GHandle gh, int item);

/**
 * @brief				Get the ID of an item with a given name
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] text		The item name
 *
 * @return				The id of the list item or -1 on error
 *
 * @api
 */
int gwinListFindText(GHandle gh, const char* text);

/**
 * @brief				Set the custom parameter of an item with a given ID
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] item		The item ID
 * @param[in] param		The parameter to be set
 *
 * @api
 */
void gwinListItemSetParam(GHandle gh, int item, uint16_t param);

/**
 * @brief				Get the custom parameter of an item with a given ID
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] item		The item ID
 *
 * @return				The parameter
 *
 * @api
 */
uint16_t gwinListItemGetParam(GHandle gh, int item);

/**
 * @brief				Delete all the items of the list
 *
 * @param[in] gh		The widget handle (must be a list handle)
 *
 * @api
 */
void gwinListDeleteAll(GHandle gh);

/**
 * @brief				Delete an item from the list
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] item		The item ID
 *
 * @api
 */
void gwinListItemDelete(GHandle gh, int item);

/**
 * @brief				Get the amount of items within the list
 *
 * @param[in] gh		The widget handle (must be a list handle)
 *
 * @return				The amount of items in the list
 *
 * @api
 */
int gwinListItemCount(GHandle gh);

/**
 * @brief				Check if an item with a given ID is selected
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] item		The item ID
 *
 * @return				TRUE if the item is selected, FALSE otherwise
 *
 * @api
 */
bool_t gwinListItemIsSelected(GHandle gh, int item);

/**
 * @brief				Get the ID of the selected item
 *
 * @param[in] gh		The widget handle (must be a list handle)
 *
 * @return				The ID of the selected list item for a single-select list.
 *
 * @note				It always returns -1 (nothing selected) for a multi-select list.
 * 						Instead use @p gwinListItemIsSelected() to get the selection status
 * 						for a multi-select list.
 *
 * @api
 */
int gwinListGetSelected(GHandle gh);

/**
 * @brief				Get the text of the selected item
 *
 * @param[in] gh		The widget handle (must be a list handle)
 *
 * @return				The test of the selected list item for a single-select list.
 *
 * @note				It always returns NULL (nothing selected) for a multi-select list.
 *
 * @api
 */
const char* gwinListGetSelectedText(GHandle gh);

/**
 * @brief				Set whether a specific item is selected or not
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] item		The item ID
 * @param[in] doSelect	TRUE to select the item or FALSE to deselect the item
 *
 * @note				Changing the selection using this api call will NOT send the list selection
 * 						change event.
 * @note				With a single select list selecting an item with this call will deselect
 * 						any existing selected item. De-selecting an item with this call will not
 * 						cause a new item to be automatically selected.
 * @note				De-selecting an item that is not selected will not effect any items that
 * 						are selected, even in single-select mode.
 * @api
 */
void gwinListSetSelected(GHandle gh, int item, bool_t doSelect);

/**
 * @brief				Scroll the list so the specified item is in view
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] item		The item ID
 *
 * @note				This will typically scroll the selected item to the top of the list
 * 						unless the item is in the last page of list items.
 *
 * @api
 */
void gwinListViewItem(GHandle gh, int item);

#if GWIN_NEED_LIST_IMAGES || defined(__DOXYGEN__)
	/**
	 * @brief				Set the image for a list item
	 *
	 * @pre					GWIN_NEED_LIST_IMAGES must be set to true in your gfxconf.h
	 *
	 * @param[in] gh		The widget handle (must be a list handle)
	 * @param[in] item		The item ID
	 * @param[in] pimg		The image to be displayed or NULL to turn off the image for this list item.
	 *
	 * @note				The image should be up to 4 x (the font height) and a width of (the font height).
	 * 						The 1st (top) part of the image is displayed for a selected item.
	 * 						The 2nd part of the image is displayed for an unselected item.
	 * 						The 3rd part of the image is displayed for a disabled selected item.
	 * 						The 4th part of the image is displayed for a disabled unselected item.
	 * 						If the image is less than 4 times the font height then the image use is collapsed
	 * 						into the available height. For example, an image that equals the font height will use
	 * 						the same image for all four states.
	 * @note				The image is only displayed while it is open. It is up to the application to open
	 * 						the image.
	 * @note				The same image can be used on more than one list item.
	 * @note				Images are aligned with the top (not the baseline) of the list item.
	 * @note				When any item in the list has an image attached, space is allocated to display
	 * 						the images even if the image is closed or has been removed by calling @p gwinListItemSetImage()
	 * 						with a NULL image or by calling @p gwinListItemDelete(). The only way to turn-off the image area
	 * 						for this list is to call gwinListDeleteAll().
	 *
	 */
	void gwinListItemSetImage(GHandle gh, int item, gdispImage *pimg);
#endif

/**
 * @defgroup Renderings_List Renderings
 *
 * @brief				Built-in rendering functions for the list widget.
 *
 * @details				These function may be passed to @p gwinSetCustomDraw() to get different list drawing styles.
 *
 * @note				In your custom list drawing function you may optionally call these
 * 						standard functions and then draw your extra details on top.
 * @note				These custom drawing routines don't have to worry about setting clipping as the framework
 * 						sets clipping to the object window prior to calling these routines.
 *
 * @{
 */

/**
 * @brief				The default rendering function for the list widget
 *
 * @param[in] gw		The widget object (must be a list object)
 * @param[in] param		A parameter passed in from the user. Ignored by this function.
 *
 * @api
 */
void gwinListDefaultDraw(GWidgetObject* gw, void* param);

/** @} */

#ifdef __cplusplus
}
#endif

#endif // _GWIN_LIST_H
/** @} */

