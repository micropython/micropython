#include <GL/oscontext.h>
#include "zbuffer.h"
#include "zgl.h"
#include <GL/gl.h>
#include <stdlib.h>

static int buffercnt = 0;

ostgl_context *
ostgl_create_context(const int xsize,
                     const int ysize,
                     const int depth,
                     void **framebuffers,
                     const int numbuffers)
{
  ostgl_context *context;
  int i;
  ZBuffer *zb;
   
  gl_assert(depth == 16); /* support for other depths must include bpp
                          convertion */
  gl_assert(numbuffers >= 1);
  
  context = gl_malloc(sizeof(ostgl_context));
  gl_assert(context);
  context->zbs = gl_malloc(sizeof(void*)*numbuffers);
  context->framebuffers = gl_malloc(sizeof(void*)*numbuffers);
  
  gl_assert(context->zbs != NULL && context->framebuffers != NULL);
  
  for (i = 0; i < numbuffers; i++) {
    context->framebuffers[i] = framebuffers[i];
    zb = ZB_open(xsize, ysize, ZB_MODE_5R6G5B, 0, NULL, NULL, framebuffers[i]);
    if (zb == NULL) {
      fprintf(stderr, "Error while initializing Z buffer\n");
      exit(1);
    }
    context->zbs[i] = zb;
  }
  if (++buffercnt == 1) {
    glInit(context->zbs[0]);
  }
  context->xsize = xsize;
  context->ysize = ysize;
  context->numbuffers = numbuffers;
  return context;
}

void
ostgl_delete_context(ostgl_context *context)
{
  int i;
  for (i = 0; i < context->numbuffers; i++) {
    ZB_close(context->zbs[i]);
  }
  gl_free(context->zbs);
  gl_free(context->framebuffers);
  gl_free(context);
  
  if (--buffercnt == 0) {
    glClose();
  }
}

void
ostgl_make_current(ostgl_context *oscontext, const int idx)
{
  GLContext *context = gl_get_context();
  gl_assert(idx < oscontext->numbuffers);
  context->zb = oscontext->zbs[idx];
}

void
ostgl_resize(ostgl_context *context,
             const int xsize,
             const int ysize,
             void **framebuffers)
{
  int i;
  for (i = 0; i < context->numbuffers; i++) {
    ZB_resize(context->zbs[i], framebuffers[i], xsize, ysize);
  }
}
