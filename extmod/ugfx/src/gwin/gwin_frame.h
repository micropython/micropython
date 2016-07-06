/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_frame.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @defgroup Frame Frame
 * @ingroup Containers
 *
 * @brief		Container with window decoration.
 *
 * @details		A frame is a rectangular window that can have optional border as well as buttons to
 *				close, maximize and minimize it. The main purpose of this widget is to contain children. 	
 *
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_FRAME must be set to TRUE in your gfxconf.h
 * @{
 */

#ifndef _GWIN_FRAME_H
#define _GWIN_FRAME_H

/* This file is included from src/gwin/gwin_container.h */

/**
 * @brief	 Flags for gwinFrameCreate()
 * @{
 */
#define GWIN_FRAME_BORDER			0x00000000		/**< Deprecated. A border is always shown with a frame window now. */
#define GWIN_FRAME_CLOSE_BTN		0x00000001		/**< Should a close button be shown? */
#define GWIN_FRAME_MINMAX_BTN		0x00000002		/**< Should minimize and maximize buttons be shown? */
#define GWIN_FRAME_KEEPONCLOSE		0x00000004		/**< Don't automatically destroy the frame on close */
/** @} */

/**
 * @brief	The internal frame flags
 * @note	Used only for writing a custom draw routine.
 * @{
 */
#define GWIN_FRAME_CLOSE_PRESSED	0x00000008
#define GWIN_FRAME_MIN_PRESSED		0x00000010
#define GWIN_FRAME_MAX_PRESSED		0x00000020
#define GWIN_FRAME_REDRAW_FRAME		0x00000040		// Only redraw the frame
/** @} */

typedef GContainerObject GFrameObject;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief				Create a frame widget
 *
 * @details				This widget provides a window like we know it from desktop systems.
 *
 * @param[in] g			The GDisplay to display this window on
 * @param[in] fo		The GFrameObject structure to initialize. If this is NULL the structure is dynamically allocated.
 * @param[in] pInit		The initialization parameters
 * @param[in] flags		Some flags, see notes.
 *
 * @note				Possible flags are: GWIN_FRAME_CLOSE_BTN, GWIN_FRAME_MINMAX_BTN.
 * @note				These frame buttons are processed internally. The close button will invoke a gwinDestroy() which will
 *						destroy the window itself and EVERY child it contains (also children of children).
 *
 * @return				NULL if there is no resulting widget. A valid GHandle otherwise.
 *
 * @api
 */
GHandle gwinGFrameCreate(GDisplay *g, GFrameObject *fo, GWidgetInit *pInit, uint32_t flags);
#define gwinFrameCreate(fo, pInit, flags)	gwinGFrameCreate(GDISP, fo, pInit, flags);

/**
 * @defgroup Renderings_Frame Renderings
 *
 * @brief				Built-in rendering functions for the frame widget.
 *
 * @details				These function may be passed to @p gwinSetCustomDraw() to get different frame drawing styles.
 *
 * @note				In your custom frame drawing function you may optionally call these
 * 						standard functions and then draw your extra details on top.
 * @note				These custom drawing routines don't have to worry about setting clipping as the framework
 * 						sets clipping to the object window prior to calling these routines.
 *
 * @{
 */

/**
 * @brief				The default rendering function for the frame widget.
 *
 * @details				Fills the client area with the background color.
 *
 * @param[in] gw		The widget object (must be a frame widget).
 * @param[in] param		A parameter passed in from the user. Ignored by this function.
 *
 * @api
 */
void gwinFrameDraw_Std(GWidgetObject *gw, void *param);

/**
 * @brief				Renders the frame widget with a transparent client area.
 *
 * @details				Will not fill the client area at all.
 *
 * @param[in] gw		The widget object (must be a frame object).
 * @param[in] param		A parameter passed in from the user. Ignored by this function.
 *
 * @note				The image custom draw function  @p gwinFrameDraw_Image() uses param to pass in the gdispImage pointer.
 * 						The image must be already opened before calling  @p gwinSetCustomDraw().
 *
 * @api
 */
void gwinFrameDraw_Transparent(GWidgetObject *gw, void *param);

#if GDISP_NEED_IMAGE || defined(__DOXYGEN__)
	/**
	 * @brief				Renders the frame widget and uses the specified image for the client area.
	 *
	 * @details				The image will be tiled throghout the client area. Therefore, to archive the best looking result the
	 *						supplied image needs to be of the same size as the client area size of the frame widget (inner size).
	 *
	 * @param[in] gw		The widget object (must be a frame object).
	 * @param[in] param		A parameter passed in from the user. Must be an image handle. See note below.
	 *
	 * @note				The image must be already opened before calling  @p gwinSetCustomDraw(). The handle is passed as the parameter
	 *						to this function.
	 *
	 * @pre					GDISP_NEED_IMAGE must be set to TRUE
	 *
	 * @api
	 */
	void gwinFrameDraw_Image(GWidgetObject *gw, void *param);
#endif /* GDISP_NEED_IMAGE */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GWIN_FRAME_H */
/** @} */

