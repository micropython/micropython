/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_gl3d.c
 * @brief   GWIN sub-system button code
 */

#include "../../gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_GL3D

#if GDISP_PIXELFORMAT != GDISP_PIXELFORMAT_RGB565
	#error "GWIN: GL3D only support GDISP_PIXELFORMAT_RGB565 color format (TinyGL limitation)"
#endif

#include "gwin_class.h"

#include "../../3rdparty/tinygl-0.4-ugfx/src/zgl.h"

// Forward definitions
static void gl3dDestroy(GWindowObject *gh);
static void gl3dRedraw(GWindowObject *gh);
static int gl3dResizeGLViewport(GLContext *c, int *xsize_ptr, int *ysize_ptr);

static const gwinVMT gl3dVMT = {
		"GL3D",					// The classname
		sizeof(GGL3DObject),	// The object size
		gl3dDestroy,			// The destroy routine
		gl3dRedraw,				// The redraw routine
		0,						// The after-clear routine
};

static bool_t	haveGLwindow = FALSE;

GHandle gwinGGL3DCreate(GDisplay *g, GGL3DObject *gl, const GWindowInit *pInit) {
	ZBuffer *	zb;
	GLContext *	glcxt;

	// Only one GL3D window allowed at a time (TinyGL limitation)
	if (haveGLwindow)
		return 0;

	if (!(gl = (GGL3DObject *)_gwindowCreate(g, &gl->g, pInit, &gl3dVMT, 0)))
		return 0;

	// Must be a multiple of 4 bytes
	gl->g.width &= ~3;
	gl->g.height &= ~3;

    zb = ZB_open(gl->g.width, gl->g.height, ZB_MODE_5R6G5B, 0, NULL, NULL, NULL);
    if (!zb) {
		if ((gl->g.flags & GWIN_FLG_DYNAMIC))
			gfxFree(gl);
		return 0;
    }

    /* initialisation of the TinyGL interpreter */
    glInit(zb);
    gl->glcxt = glcxt = gl_get_context();
    glcxt->opaque = gl;
    glcxt->gl_resize_viewport = gl3dResizeGLViewport;

    /* set the viewport : we force a call to the viewport resize routine */
    glcxt->viewport.xsize=-1;
    glcxt->viewport.ysize=-1;

    glViewport(0, 0, gl->g.width, gl->g.height);

    haveGLwindow = TRUE;
	gwinSetVisible((GHandle)gl, pInit->show);
	return (GHandle)gl;
}

static void gl3dDestroy(GWindowObject *gh) {
	(void) gh;
    glClose();
    haveGLwindow = FALSE;
}

static void gl3dRedraw(GWindowObject *gh) {
	ZBuffer *	zb;

	zb = ((GGL3DObject *)gh)->glcxt->zb;
	gdispGBlitArea(gh->display, gh->x, gh->y, zb->xsize, zb->ysize, 0, 0, zb->linesize/sizeof(color_t), (const pixel_t *)zb->pbuf);
}

static int gl3dResizeGLViewport(GLContext *c, int *xsize_ptr, int *ysize_ptr) {
	int		cx, cy;

	cx = *xsize_ptr;
	cy = *ysize_ptr;

	// We ensure that cx and cy are multiples of 4 for the zbuffer. TODO: find a better solution
	cx &= ~3;
	cy &= ~3;

	if (cx <= 0 || cy <= 0)
		return -1;

	*xsize_ptr = cx;
	*ysize_ptr = cy;

	// Resize the GWIN???

	// Resize the Z buffer
	ZB_resize(c->zb, NULL, cx, cy);
	return 0;
}

/**
 * TinyGL support routines
 */

#include <string.h>

#define NO_CLIBRARY

void tgl_warning(const char *format, ...)	{ (void)format; }
void tgl_trace(const char *format, ...)		{ (void)format; }
void tgl_fixme(const char *format, ...)		{ (void)format; }
void gl_fatal_error(char *format, ...)		{ gfxHalt(format); }
void gl_assert(int test)					{ if (!test) gfxHalt("TinyGL Assert"); }

void gl_free(void *p)						{ gfxFree(p); }
void *gl_malloc(int size)					{ return gfxAlloc(size); }

void *gl_zalloc(int size) {
	void	*p;

	p = gfxAlloc(size);
	if (p)
		memset(p, 0, size);
    return p;
}


/**
 * Pre-load TinyGL headers
 */

/**
 * TinyGL wrapper code
 */

#include "../../3rdparty/tinygl-0.4-ugfx/src/api.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/list.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/vertex.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/init.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/matrix.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/texture.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/misc.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/clear.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/light.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/clip.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/select.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/get.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/zbuffer.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/zline.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/zdither.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/ztriangle.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/zmath.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/image_util.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/arrays.c"
#include "../../3rdparty/tinygl-0.4-ugfx/src/specbuf.c"

#endif /* GFX_USE_GWIN && GWIN_NEED_GL3D */
