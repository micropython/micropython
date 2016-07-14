#ifndef NGLX_H
#define NGLX_H

#include <microwin/nano-X.h>
#include <GL/gl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *NGLXContext;
typedef GR_DRAW_ID NGLXDrawable;

extern NGLXContext nglXCreateContext( NGLXContext shareList, int flags );

extern void nglXDestroyContext( NGLXContext ctx );

extern int nglXMakeCurrent( NGLXDrawable drawable,
			    NGLXContext ctx);

extern void nglXSwapBuffers( NGLXDrawable drawable );

#ifdef __cplusplus
}
#endif

#endif
