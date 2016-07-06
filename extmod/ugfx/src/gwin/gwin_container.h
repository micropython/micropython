/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_container.h
 *
 * @defgroup Container Container
 * @ingroup Containers
 *
 * @brief	Basic container.
 *
 * @details	A Container is a GWindow that supports child windows. It is also
 * 			a widget in its own right and therefore can accept user input directly.
 *
 * @pre		GFX_USE_GWIN and GWIN_NEED_CONTAINERS must be set to TRUE in your gfxconf.h
 * @{
 */

#ifndef _GCONTAINER_H
#define _GCONTAINER_H

/* This file is included within "src/gwin/gwin.h" */

/**
 * @brief	The GWIN Container structure
 * @note	A container is a GWIN widget that can have children.
 * @note	Do not access the members directly. Treat it as a black-box and use the method functions.
 *
 * @{
 */
typedef GWidgetObject GContainerObject;
/** @} */

/**
 * A comment/rant on the above structure:
 * We would really like the GWidgetObject member to be anonymous. While this is
 * allowed under the C11, C99, GNU and various other standards which have been
 * around forever - compiler support often requires special flags e.g
 * gcc requires the -fms-extensions flag (no wonder the language and compilers have
 * not really progressed in 30 years). As portability is a key requirement
 * we unfortunately won't use this useful feature in case we get a compiler that
 * won't support it even with special flags.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	Get the first child window
 *
 * @return	The first child or NULL if are no children windows
 *
 * @param[in] gh	The parent container or NULL to get the first top level window
 *
 * @api
 */
GHandle gwinGetFirstChild(GHandle gh);

/**
 * @brief	Get the next child window in the z-order
 *
 * @return	The next window or NULL if no more children
 *
 * @param[in] gh		The window to obtain the next sibling of.
 *
 * @note	This returns the next window under the current parent window.
 * 			Unlike @p gwinGetNextWindow() it will only return windows that
 * 			have the same parent as the supplied window.
 *
 * @api
 */
GHandle gwinGetSibling(GHandle gh);

/**
 * @brief	Get the inner width of a container window
 *
 * @return	The inner width of a container window or zero if this is not a container
 *
 * @param[in] gh		The window
 *
 * @api
 */
coord_t gwinGetInnerWidth(GHandle gh);

/**
 * @brief	Get the inner height of a container window
 *
 * @return	The inner height of a container window or zero if this is not a container
 *
 * @param[in] gh		The window
 *
 * @api
 */
coord_t gwinGetInnerHeight(GHandle gh);


/**
 * @brief	 Flags for gwinContainerCreate()
 * @{
 */
#define GWIN_CONTAINER_BORDER		0x00000001
/** @} */

/**
 * @brief   Create a simple container.
 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] g			The GDisplay to display this window on
 * @param[in] gw		The GContainerObject structure to initialise. If this is NULL the structure is dynamically allocated.
 * @param[in] pInit		The initialisation parameters
 * @param[in] flags		Some flags, see notes
 *
 * @api
 */
GHandle gwinGContainerCreate(GDisplay *g, GContainerObject *gw, const GWidgetInit *pInit, uint32_t flags);
#define gwinContainerCreate(gc, pInit, flags)			gwinGContainerCreate(GDISP, gc, pInit, flags)


/**
 * @defgroup Renderings_Container Renderings
 *
 * @brief				Built-in rendering functions for the container widget.
 *
 * @details				These function may be passed to @p gwinSetCustomDraw() to get different container drawing styles.
 *
 * @note				In your custom container drawing function you may optionally call these
 * 						standard functions and then draw your extra details on top.
 * @note				These custom drawing routines don't have to worry about setting clipping as the framework
 * 						sets clipping to the object window prior to calling these routines.
 *
 * @{
 */

/**
 * @brief				The default rendering function for the container widget.
 *
 * @details				Fills the client area with the background color.
 *
 * @param[in] gw		The widget object (must be a container object).
 * @param[in] param		A parameter passed in from the user. Ignored by this function.
 *
 * @api
 */
void gwinContainerDraw_Std(GWidgetObject *gw, void *param);

/**
 * @brief				Renders the container but leaves the client area transparent.
 *
 * @details				Will not fill the client area at all.
 *
 * @param[in] gw		The widget object (must be a container object).
 * @param[in] param		A parameter passed in from the user. Ignored by this function.
 *
 * @api
 */
void gwinContainerDraw_Transparent(GWidgetObject *gw, void *param);

#if GDISP_NEED_IMAGE || defined(__DOXYGEN__)
	/**
	 * @brief				Renders the container and uses the specified image for the client area.
	 *
	 * @details				The image will be tiled throghout the client area. Therefore, to archive the best looking result the
	 *						supplied image needs to be of the same size as the client area size of the container widget (inner size).
	 *
	 * @param[in] gw		The widget object (must be a container object).
	 * @param[in] param		A parameter passed in from the user. Must be an image handle. See note below.
	 *
	 * @note				The image must be already opened before calling  @p gwinSetCustomDraw(). The handle is passed as the parameter
	 *						to this function.
	 *
	 * @pre					GDISP_NEED_IMAGE must be set to TRUE
	 *
	 * @api
	 */
	void gwinContainerDraw_Image(GWidgetObject *gw, void *param);
#endif /* GDISP_NEED_IMAGE */

/** @} */

#ifdef __cplusplus
}
#endif

/* Include extra container types */
#if GWIN_NEED_FRAME || defined(__DOXYGEN__)
	#include "gwin_frame.h"
#endif
#if GWIN_NEED_TABSET || defined(__DOXYGEN__)
	#include "gwin_tabset.h"
#endif

#endif /* _GCONTAINER_H */
/** @} */
