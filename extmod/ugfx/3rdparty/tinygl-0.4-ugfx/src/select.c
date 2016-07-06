#include "zgl.h"

int glRenderMode(int mode)
{
  GLContext *c=gl_get_context();
  int result=0;
  
  switch(c->render_mode) {
  case GL_RENDER:
    break;
  case GL_SELECT:
    if (c->select_overflow) {
      result=-c->select_hits;
    } else {
      result=c->select_hits;
    }
    c->select_overflow=0;
    c->select_ptr=c->select_buffer;
    c->name_stack_size=0;
    break;
  default:
    gl_assert(0);
  }
  switch(mode) {
  case GL_RENDER:
    c->render_mode=GL_RENDER;
    break;
  case GL_SELECT:
    c->render_mode=GL_SELECT;
    gl_assert( c->select_buffer != NULL);
    c->select_ptr=c->select_buffer;
    c->select_hits=0;
    c->select_overflow=0;
    c->select_hit=NULL;
    break;
  default:
    gl_assert(0);
  }
  return result;
}

void glSelectBuffer(int size,unsigned int *buf)
{
  GLContext *c=gl_get_context();

  gl_assert(c->render_mode != GL_SELECT);
  
  c->select_buffer=buf;
  c->select_size=size;
}


void glopInitNames(GLContext *c,GLParam *p)
{
	  (void)p;
  if (c->render_mode == GL_SELECT) {
    c->name_stack_size=0;
    c->select_hit=NULL;
  }
}

void glopPushName(GLContext *c,GLParam *p)
{
  if (c->render_mode == GL_SELECT) {
    gl_assert(c->name_stack_size<MAX_NAME_STACK_DEPTH);
    c->name_stack[c->name_stack_size++]=p[1].i;
    c->select_hit=NULL;
  }
}

void glopPopName(GLContext *c,GLParam *p)
{
	  (void)p;
  if (c->render_mode == GL_SELECT) {
    gl_assert(c->name_stack_size>0);
    c->name_stack_size--;
    c->select_hit=NULL;
  }
}

void glopLoadName(GLContext *c,GLParam *p)
{
  if (c->render_mode == GL_SELECT) {
   gl_assert(c->name_stack_size>0);
   c->name_stack[c->name_stack_size-1]=p[1].i;
   c->select_hit=NULL;
  }
}

void gl_add_select(GLContext *c,unsigned int zmin,unsigned int zmax)
{
  unsigned int *ptr;
  int n,i;

  if (!c->select_overflow) {
    if (c->select_hit==NULL) {
      n=c->name_stack_size;
      if ((c->select_ptr-c->select_buffer+3+n) > 
	  c->select_size) {
	c->select_overflow=1;
      } else {
	ptr=c->select_ptr;
	c->select_hit=ptr;
	*ptr++=c->name_stack_size;
	*ptr++=zmin;
	*ptr++=zmax;
	for(i=0;i<n;i++) *ptr++=c->name_stack[i];
	c->select_ptr=ptr;
	c->select_hits++;
      }
    } else {
      if (zmin<c->select_hit[1]) c->select_hit[1]=zmin;
      if (zmax>c->select_hit[2]) c->select_hit[2]=zmax;
    }
  }
}
