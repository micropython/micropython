/* simple glx like driver for TinyGL and Nano X */
#include <GL/gl.h>
#include <GL/nglx.h>
#include <microwin/nano-X.h>
#include "zgl.h"

typedef struct {
    GLContext *gl_context;
    int xsize,ysize;
    GR_DRAW_ID drawable;
    GR_GC_ID gc;
    int pixtype; /* pixel type in TinyGL */
} TinyNGLXContext;

NGLXContext nglXCreateContext(NGLXContext shareList, int flags)
{
  TinyNGLXContext *ctx;

  if (shareList != NULL) {
    gl_fatal_error("No sharing available in TinyGL");
  }
  ctx=gl_malloc(sizeof(TinyNGLXContext));
  if (!ctx)
      return NULL;
  ctx->gl_context=NULL;
  return (NGLXContext) ctx;
}

void glXDestroyContext( NGLXContext ctx1 )
{
  TinyNGLXContext *ctx = (TinyNGLXContext *) ctx1;
  if (ctx->gl_context != NULL) {
    glClose();
  }
  gl_free(ctx);
}


/* resize the glx viewport : we try to use the xsize and ysize
   given. We return the effective size which is guaranted to be smaller */

static int glX_resize_viewport(GLContext *c,int *xsize_ptr,int *ysize_ptr)
{
  TinyNGLXContext *ctx;
  int xsize,ysize;
  
  ctx=(TinyNGLXContext *)c->opaque;

  xsize=*xsize_ptr;
  ysize=*ysize_ptr;

  /* we ensure that xsize and ysize are multiples of 2 for the zbuffer. 
     TODO: find a better solution */
  xsize&=~3;
  ysize&=~3;

  if (xsize == 0 || ysize == 0) return -1;

  *xsize_ptr=xsize;
  *ysize_ptr=ysize;

  ctx->xsize=xsize;
  ctx->ysize=ysize;

  /* resize the Z buffer */
  ZB_resize(c->zb,NULL,xsize,ysize);
  return 0;
}

/* we assume here that drawable is a window */
int nglXMakeCurrent( NGLXDrawable drawable,
                     NGLXContext ctx1)
{
  TinyNGLXContext *ctx = (TinyNGLXContext *) ctx1;
  int mode, xsize, ysize;
  ZBuffer *zb;
  GR_WINDOW_INFO win_info;

  if (ctx->gl_context == NULL) {
      /* create the TinyGL context */
      GrGetWindowInfo(drawable, &win_info);

      xsize = win_info.width;
      ysize = win_info.height;

      /* currently, we only support 16 bit rendering */
      mode = ZB_MODE_5R6G5B;
      zb=ZB_open(xsize,ysize,mode,0,NULL,NULL,NULL);
      if (zb == NULL) {
          fprintf(stderr, "Error while initializing Z buffer\n");
          exit(1);
      }

      ctx->pixtype = MWPF_TRUECOLOR565;

      /* create a gc */
      ctx->gc = GrNewGC();
      
      /* initialisation of the TinyGL interpreter */
      glInit(zb);
      ctx->gl_context=gl_get_context();
      ctx->gl_context->opaque=(void *) ctx;
      ctx->gl_context->gl_resize_viewport=glX_resize_viewport;

      /* set the viewport : we force a call to glX_resize_viewport */
      ctx->gl_context->viewport.xsize=-1;
      ctx->gl_context->viewport.ysize=-1;
      
      glViewport(0, 0, xsize, ysize);
  }
  
  return 1;
}

void nglXSwapBuffers( NGLXDrawable drawable )
{
    GLContext *gl_context;
    TinyNGLXContext *ctx;
    
    /* retrieve the current NGLXContext */
    gl_context=gl_get_context();
    ctx=(TinyNGLXContext *)gl_context->opaque;
    
    GrArea(drawable, ctx->gc, 0, 0, ctx->xsize, 
           ctx->ysize, ctx->gl_context->zb->pbuf, ctx->pixtype);
}


