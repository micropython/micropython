/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file	src/gwin/gwin_gl3d.h
 * @brief	GWIN 3D module header file
 *
 * @defgroup 3D 3D
 * @ingroup Windows
 *
 * @brief	Widget that can be used to render OpenGL.
 *
 * @details	3D GWIN window based on OpenGL (or more exactly Tiny GL)
 *
 * @pre		GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre		GWIN_NEED_GL3D must be set to TRUE in your gfxconf.h
 *
 * @{
 */

#ifndef _GWIN_GL3D_H
#define _GWIN_GL3D_H

/* This file is included within "src/gwin/gwin.h" */


// A gl3d window
typedef struct GGL3DObject {
	GWindowObject		g;
	struct GLContext *	glcxt;
	} GGL3DObject;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Create a gl3d window.
 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] g			The GDisplay to display this window on
 * @param[in] gg		The GGL3DObject structure to initialise. If this is NULL the structure is dynamically allocated.
 * @param[in] pInit		The initialization parameters to use
 *
 * @note				The drawing color and the background color get set to the current defaults. If you haven't called
 * 						@p gwinSetDefaultColor() or @p gwinSetDefaultBgColor() then these are White and Black respectively.
 * @note				The font gets set to the current default font. If you haven't called @p gwinSetDefaultFont() then there
 * 						is no default font and text drawing operations will no nothing.
 * @note				The dimensions and position may be changed to fit on the real screen.
 *
 * @api
 */
GHandle gwinGGL3DCreate(GDisplay *g, GGL3DObject *gg, const GWindowInit *pInit);
#define gwinGL3DCreate(gg, pInit)			gwinGGL3DCreate(GDISP, gg, pInit)

/* Include the gl interface */
#include "../../3rdparty/tinygl-0.4-ugfx/include/GL/gl.h"

#ifdef __cplusplus
}
#endif

#endif	/* _GWIN_GL3D_H */
/** @} */

