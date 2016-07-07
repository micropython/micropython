/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file	src/gwin/gwin_image.h
 * @brief	GWIN image widget header file.
 *
 * @defgroup ImageBox ImageBox
 * @ingroup Widgets
 *
 * @brief		ImageBox widget. Used to display images within the @p GWIN widget system.
 *
 * @details		GWIN allos it to create an image widget. The widget
 *				takes no user input.
 *
 * @pre			GFX_USE_GDISP must be set to TRUE in your gfxconf.h
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GDISP_NEED_IMAGE must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_IMAGE must be set to TRUE in your gfxconf.h
 * @pre			At least one image type must be enabled in your gfxconf.h
 *
 * @{
 */

#ifndef _GWIN_IMAGE_H
#define _GWIN_IMAGE_H

// This file is included within "src/gwin/gwin.h"

// An image window
typedef struct GImageObject {
	GWindowObject	g;
	gdispImage		image;			// The image itself
	#if GWIN_NEED_IMAGE_ANIMATION
		GTimer			timer;		// Timer used for animated images
	#endif
} GImageObject;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief				Create an image widget.
 * @details				Display's a picture.
 * @return				NULL if there is no resultant drawing area, otherwise the widget handle.
 *
 * @param[in] g			The GDisplay to display this window on
 * @param[in] widget	The image widget structure to initialise. If this is NULL, the structure is dynamically allocated.
 * @param[in] pInit		The initialization parameters to use.
 *
 * @note				The default background color gets set to the current default one.
 * @note				An image window knows how to redraw.
 *
 * @api
 */
GHandle gwinGImageCreate(GDisplay *g, GImageObject *widget, GWindowInit *pInit);
#define gwinImageCreate(w, pInit)			gwinGImageCreate(GDISP, w, pInit)

/**
 * @brief				Opens the image using a GFILE
 * @return				TRUE if the image can be opened
 *
 * @param[in] gh		The widget (must be an image widget)
 * @param[in] f			The open (for reading) GFILE to use
 *
 * @api
 */
bool_t gwinImageOpenGFile(GHandle gh, GFILE *f);

/**
 * @brief				Opens the image using the specified filename
 * @return				TRUE if the open succeeds
 *
 * @param[in] gh		The widget (must be an image widget)
 * @param[in] filename	The filename to open
 *
 * @api
 */
#define gwinImageOpenFile(gh, filename)			gwinImageOpenGFile((gh), gfileOpen((filename), "rb"))

	/**
	 * @brief				Sets the input routines that support reading the image from memory
	 *						in RAM or flash.
	 * @pre					GFILE_NEED_MEMFS must be TRUE
	 * @return				TRUE if the IO open function succeeds
	 *
	 * @param[in] gh		The widget (must be an image widget)
	 * @param[in] ptr		A pointer to the image in RAM or Flash
	 *
	 * @api
	 */
#define gwinImageOpenMemory(gh, ptr)			gwinImageOpenGFile((gh), gfileOpenMemory((void *)(ptr), "rb"))

/**
 * @brief				Sets the input routines that support reading the image from a BaseFileStream (eg. an SD-Card).
 * @return				TRUE if the IO open function succeeds
 * @pre					GFILE_NEED_CHIBIOSFS and GFX_USE_OS_CHIBIOS must be TRUE
 *
 * @param[in] gh		The widget (must be an image widget)
 * @param[in] streamPtr	A pointer to the (open) BaseFileStream object.
 *
 * @api
 */
#define gwinImageOpenStream(gh, streamPtr)		gwinImageOpenGFile((gh), gfileOpenBaseFIleStream((streamPtr), "rb"))

/**
 * @brief				Cache the image.
 * @details				Decodes and caches the current frame into RAM.
 *
 * @param[in] gh		The widget (must be an image widget)
 *
 * @return				GDISP_IMAGE_ERR_OK (0) on success or an error code.
 *
 * @api
 */
gdispImageError gwinImageCache(GHandle gh);

#ifdef __cplusplus
}
#endif

#endif // _GWIN_IMAGE_H
/** @} */

