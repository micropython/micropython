/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gdisp/gdisp_pixmap.h
 *
 * @defgroup Pixmap Pixmap
 * @ingroup GDISP
 *
 * @brief   Sub-Module for dynamic framebuffers.
 *
 * @note	A Pixmap is an off-screen virtual display that can be drawn to just like any other
 * 			display. It can then be copied to a real display using the standard gdispGBlitArea() call.
 * @pre		GDISP_NEED_PIXMAP must be TRUE in your gfxconf.h
 * @{
 */

#ifndef _GDISP_PIXMAP_H
#define _GDISP_PIXMAP_H

#if (GFX_USE_GDISP && GDISP_NEED_PIXMAP) || defined(__DOXYGEN__)

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @brief	Create an off-screen pixmap that can be drawn to just like a normal display
	 *
	 * @param[in] width  	The width of the pixmap to be created
	 * @param[in] height  	The height of the pixmap to be created
	 *
	 * @return 	The created GDisplay representing the pixmap
	 *
	 * @note	Once created, an off-screen pixmap can be drawn on using the standard gdispGxxxx calls.
	 * @note	It must be destroyed using @p gdispDeleteOffscreenPixmap
	 * @note	Because the RAM for the display area is allocated, on small micros only very small pixmaps should be considered.
	 * 			For example a 100x100 at 16 bits per pixel would be 20K of RAM (plus some overheads).
	 */
	GDisplay *gdispPixmapCreate(coord_t width, coord_t height);

	/**
	 * @brief	Destroy an off-screen pixmap
	 *
	 * @param[in] g  		The pixmap virtual display to delete
	 *
	 * @note	If a normal display is passed to this routine, it will be ignored.
	 */
	void gdispPixmapDelete(GDisplay *g);

	/**
	 * @brief	Get a pointer to the pixels of the display surface.
	 * @return	The pointer to the pixmap display surface or NULL if this display is not a pixmap.
	 *
	 * @param[in] g  	The pixmap virtual display
	 *
	 * @note	The pointer returned can be used for calls to @p gdispGBlitArea() or can be read or written to directly
	 * 			by the application code. For any one particular pixmap the pointer will not change over the life of the pixmap
	 * 			(although different pixmaps will have different pixel pointers). Once a pixmap is deleted, the pixel pointer
	 * 			should not be used by the application.
	 */
	pixel_t	*gdispPixmapGetBits(GDisplay *g);

	#if GDISP_NEED_PIXMAP_IMAGE || defined(__DOXYGEN__)
		/**
		 * @brief	Get a pointer to a native format gdispImage.
		 * @return	A pointer to a NATIVE format gdispImage in memory or NULL if this display is not a pixmap.
		 * @pre		GDISP_NEED_PIXAMP_IMAGE must be TRUE in your gfxconf.h
		 *
		 * @param[in] g  	The pixmap virtual display
		 *
		 * @return 	The pointer to the native gdispImage
		 *
		 * @note	The pointer returned can be passed to @p gdispImageOpenMemory() or to @p gfileOpenMemory().
		 * @note	If you are just wanting to copy to a real display it is more efficient to use @p gdispGetPixmapBits() and @p gdispGBlitArea().
		 * @note	Like @p gdispGetPixmapBits(), the pointer returned is valid for the life of the pixmap.
		 */
		void *gdispPixmapGetMemoryImage(GDisplay *g);
	#endif
	
#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GDISP && GDISP_NEED_PIXMAP */
#endif /* _GDISP_PIXMAP_H */
/** @} */

