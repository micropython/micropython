#include "zgl.h"
#include "msghandling.h"

void glopViewport(GLContext *c,GLParam *p)
{
  int xsize,ysize,xmin,ymin,xsize_req,ysize_req;
  
  xmin=p[1].i;
  ymin=p[2].i;
  xsize=p[3].i;
  ysize=p[4].i;

  /* we may need to resize the zbuffer */

  if (c->viewport.xmin != xmin ||
      c->viewport.ymin != ymin ||
      c->viewport.xsize != xsize ||
      c->viewport.ysize != ysize) {

    xsize_req=xmin+xsize;
    ysize_req=ymin+ysize;

    if (c->gl_resize_viewport && 
        c->gl_resize_viewport(c,&xsize_req,&ysize_req) != 0) {
      gl_fatal_error("glViewport: error while resizing display");
    }

    xsize=xsize_req-xmin;
    ysize=ysize_req-ymin;
    if (xsize <= 0 || ysize <= 0) {
      gl_fatal_error("glViewport: size too small");
    }

    tgl_trace("glViewport: %d %d %d %d\n",
              xmin, ymin, xsize, ysize);
    c->viewport.xmin=xmin;
    c->viewport.ymin=ymin;
    c->viewport.xsize=xsize;
    c->viewport.ysize=ysize;
    
    c->viewport.updated=1;
  }
}

void glopEnableDisable(GLContext *c,GLParam *p)
{
  int code=p[1].i;
  int v=p[2].i;

  switch(code) {
  case GL_CULL_FACE:
    c->cull_face_enabled=v;
    break;
  case GL_LIGHTING:
    c->lighting_enabled=v;
    break;
  case GL_COLOR_MATERIAL:
    c->color_material_enabled=v;
      break;
  case GL_TEXTURE_2D:
    c->texture_2d_enabled=v;
    break;
  case GL_NORMALIZE:
    c->normalize_enabled=v;
    break;
  case GL_DEPTH_TEST:
    c->depth_test = v;
    break;
  case GL_POLYGON_OFFSET_FILL:
    if (v) c->offset_states |= TGL_OFFSET_FILL;
    else c->offset_states &= ~TGL_OFFSET_FILL;
    break; 
  case GL_POLYGON_OFFSET_POINT:
    if (v) c->offset_states |= TGL_OFFSET_POINT;
    else c->offset_states &= ~TGL_OFFSET_POINT;
    break; 
  case GL_POLYGON_OFFSET_LINE:
    if (v) c->offset_states |= TGL_OFFSET_LINE;
    else c->offset_states &= ~TGL_OFFSET_LINE;
    break; 
  default:
    if (code>=GL_LIGHT0 && code<GL_LIGHT0+MAX_LIGHTS) {
      gl_enable_disable_light(c,code - GL_LIGHT0, v);
    } else {
      /*
      fprintf(stderr,"glEnableDisable: 0x%X not supported.\n",code);
      */
    }
    break;
  }
}

void glopShadeModel(GLContext *c,GLParam *p)
{
  int code=p[1].i;
  c->current_shade_model=code;
}

void glopCullFace(GLContext *c,GLParam *p)
{
  int code=p[1].i;
  c->current_cull_face=code;
}

void glopFrontFace(GLContext *c,GLParam *p)
{
  int code=p[1].i;
  c->current_front_face=code;
}

void glopPolygonMode(GLContext *c,GLParam *p)
{
  int face=p[1].i;
  int mode=p[2].i;
  
  switch(face) {
  case GL_BACK:
    c->polygon_mode_back=mode;
    break;
  case GL_FRONT:
    c->polygon_mode_front=mode;
    break;
  case GL_FRONT_AND_BACK:
    c->polygon_mode_front=mode;
    c->polygon_mode_back=mode;
    break;
  default:
    gl_assert(0);
  }
}

void glopHint(GLContext *c,GLParam *p)
{
	  (void)c;
	  (void)p;
#if 0
  int target=p[1].i;
  int mode=p[2].i;

  /* do nothing */
#endif
}

void 
glopPolygonOffset(GLContext *c, GLParam *p)
{
  c->offset_factor = p[1].f;
  c->offset_units = p[2].f;
}
