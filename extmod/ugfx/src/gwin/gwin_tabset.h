/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_tabset.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @defgroup Tabset Tabset
 * @ingroup Containers
 *
 * @brief		Tabwidget to implement different "Tabs" or "Pages". Used to structure menus.
 *
 * @details		A tabset is a set of tabs that control visibility of a number of pages of widgets.
 *				Note: Although the tabset is implemented as a container - you don't put your controls
 *				directly on the tabset. Instead you create a page and put your widgets on the page.
 *
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_TABSET must be set to TRUE in your gfxconf.h
 * @{
 */

#ifndef _GWIN_TABSET_H
#define _GWIN_TABSET_H

/* This file is included from src/gwin/gwin_container.h */

/**
 * @brief	The Event Type for a Tabset Event
 */
#define GEVENT_GWIN_TABSET			(GEVENT_GWIN_CTRL_FIRST+5)

/**
 * @brief	A Tabset Event
 * @note	There are currently no GEventGWinTabset listening flags - use 0 as the flags to @p gwinAttachListener()
 */
typedef struct GEventGWinTabset {
	GEventType		type;				// The type of this event (GEVENT_GWIN_TABSET)
	GHandle			gwin;				// The tabset window handle
	#if GWIN_NEED_WIDGET && GWIN_WIDGET_TAGS
		WidgetTag	tag;				// The tag of the tabset
	#endif
	// Above are the generic widget event elements, below the tabset specific elements
	GHandle			ghPage;				// The tabpage window handle that has been selected
	int				nPage;				// The page number (0 to n-1) that has been selected
	} GEventGWinTabset;

/**
 * @brief	 Flags for gwinTabsetCreate()
 * @{
 */
#define GWIN_TABSET_BORDER			0x00000001		// Should the tab pages have a border?
/** @} */

typedef struct GTabsetObject {
	GContainerObject	c;
	coord_t				border_top;
	} GTabsetObject;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief				Create a tabset widget
 *
 * @details				This widget provides a set of tabs.
 *
 * @param[in] g			The GDisplay to display this window on
 * @param[in] fo		The GTabsetObject structure to initialize. If this is NULL the structure is dynamically allocated.
 * @param[in] pInit		The initialization parameters
 * @param[in] flags		Some flags, see notes.
 *
 * @note				Possible flags are: GWIN_TABSET_BORDER
 *
 * @return				NULL if there is no resulting widget. A valid GHandle otherwise.
 *
 * @api
 */
GHandle gwinGTabsetCreate(GDisplay *g, GTabsetObject *fo, GWidgetInit *pInit, uint32_t flags);
#define gwinTabsetCreate(fo, pInit, flags)	gwinGTabsetCreate(GDISP, fo, pInit, flags);

/**
 * @brief				Add a tab-page to the tabset
 * @returns				The GHandle of the tab-page container.
 *
 * @param[in] gh		The tabset handle
 * @param[in] title		The text to set. This must be a constant string unless useAlloc is set.
 * @param[in] useAlloc	If TRUE the string specified will be copied into dynamically allocated memory.
 *
 * @api
 */
GHandle gwinTabsetAddTab(GHandle gh, const char *title, bool_t useAlloc);

/**
 * @brief   			Delete a tab-page.
 * @details				Any widgets on the page will also be destroyed
 *
 * @param[in] gh		The tab-page handle
 *
 * @note				The index position of all tabs after this tab in the tabset are automatically renumbered.
 *
 * @api
 */
#define gwinTabsetDeleteTab(gh)	gwinDestroy(gh)

/**
 * @brief				Count the number of tabs in the tabset
 * @returns				The number of tabs or zero if none exist.
 *
 * @param[in] gh		The tabset handle
 *
 * @api
 */
int gwinTabsetCountTabs(GHandle gh);

/**
 * @brief				Get the GHandle of a tab based on its position
 * @returns				The GHandle of the tab-page container or NULL if that tab-page doesn't exist.
 *
 * @param[in] gh		The tabset handle
 * @param[in] index		The tab-page handle to return (0 to number of pages - 1)
 *
 * @api
 */
GHandle gwinTabsetGetTabByIndex(GHandle gh, int index);

/**
 * @brief				Get the GHandle of a tab based on its title
 * @returns				The GHandle of the tab-page container or NULL if that tab-page doesn't exist.
 *
 * @param[in] gh		The tabset handle
 * @param[in] title		The title to search for
 *
 * @api
 */
GHandle gwinTabsetGetTabByTitle(GHandle gh, const char *title);

/**
 * @brief   Set the title of a tab-page.
 *
 * @param[in] gh		The tab-page handle (NB: Use the page handle NOT the tabset handle)
 * @param[in] title		The text to set. This must be a constant string unless useAlloc is set.
 * @param[in] useAlloc	If TRUE the string specified will be copied into dynamically allocated memory.
 *
 * @note				This function should be used to change the text associated with a tab-page
 * 						rather than @p gwinSetText().
 *
 * @api
 */
void gwinTabsetSetTitle(GHandle gh, const char *title, bool_t useAlloc);

/**
 * @brief   Get the title of a tab-page.
 * @return	The title of the tab.
 *
 * @param[in] gh		The tab-page handle (NB: Use the page handle NOT the tabset handle)
 *
 * @api
 */
#define gwinTabsetGetTitle(gh)			gwinGetText(gh)

/**
 * @brief   Set the active tab in a tabset.
 *
 * @param[in] gh		The tab-page handle (NB: Use the page handle NOT the tabset handle)
 *
 * @api
 */
void gwinTabsetSetTab(GHandle gh);

/**
 * @defgroup Renderings_Tabset Renderings
 *
 * @brief				Built-in rendering functions for the tabset widget.
 *
 * @details				These function may be passed to @p gwinSetCustomDraw() to get different tabset drawing styles.
 *
 * @note				In your custom tabset drawing function you may optionally call these
 * 						standard functions and then draw your extra details on top.
 * @note				These custom drawing routines don't have to worry about setting clipping as the framework
 * 						sets clipping to the object window prior to calling these routines.
 *
 * @{
 */

/**
 * @brief				The default rendering function for the tabset widget.
 *
 * @details				Fills the client area with the background color.
 *
 * @param[in] gw		The widget object (must be a container object).
 * @param[in] param		A parameter passed in from the user. Ignored by this function.
 *
 * @api
 */
void gwinTabsetDraw_Std(GWidgetObject *gw, void *param);

/**
 * @brief				Renders the tabset but leaves the client area transparent.
 *
 * @details				Will not fill the client area at all.
 *
 * @param[in] gw		The widget object (must be a container object).
 * @param[in] param		A parameter passed in from the user. Ignored by this function.
 *
 * @api
 */
void gwinTabsetDraw_Transparent(GWidgetObject *gw, void *param);

#if GDISP_NEED_IMAGE || defined(__DOXYGEN__)
	/**
	 * @brief				Renders the tabset and uses the specified image for the client area.
	 *
	 * @details				The image will be tiled throghout the client area. Therefore, to archive the best looking result the
	 *						supplied image needs to be of the same size as the client area size of the tabset widget (inner size).
	 *
	 * @param[in] gw		The widget object (must be a tabset object).
	 * @param[in] param		A parameter passed in from the user. Must be an image handle. See note below.
	 *
	 * @note				The image must be already opened before calling  @p gwinSetCustomDraw(). The handle is passed as the parameter
	 *						to this function.
	 *
	 * @pre					GDISP_NEED_IMAGE must be set to TRUE
	 *
	 * @api
	 */
	void gwinTabsetDraw_Image(GWidgetObject *gw, void *param);
#endif /* GDISP_NEED_IMAGE */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GWIN_TABSET_H */
/** @} */

