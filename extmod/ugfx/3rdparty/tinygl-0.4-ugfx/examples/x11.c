#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <GL/glx.h> 
#include <GL/gl.h> 
#include "ui.h"


#ifndef M_PI
#  define M_PI 3.14159265
#endif

static int attributeList[] = { GLX_RGBA, GLX_DOUBLEBUFFER, None };

static Bool WaitForNotify(Display *d, XEvent *e, char *arg) 
{
  return (e->type == MapNotify) && (e->xmap.window == (Window)arg); 
}

Display *dpy;
Window win;

void tkSwapBuffers(void)
{
    glXSwapBuffers(dpy,win);
}

int ui_loop(int argc, char **argv, const char *name)
{
  XVisualInfo *vi;
  Colormap cmap;
  XSetWindowAttributes swa;
  XSizeHints hint;
  GLXContext cx;
  XEvent event;
  int k, width, height;
  char buf[80];
  XEvent xev;
  KeySym keysym;
  XComposeStatus status;
  
  /* get a connection */
  dpy = XOpenDisplay(NULL);
  if (dpy == NULL) {
      fprintf(stderr,"Could not open X display\n");
      exit(1);
  }
  
  /* get an appropriate visual */
  vi = glXChooseVisual(dpy, DefaultScreen(dpy), attributeList);
  if (vi == NULL) {
      fprintf(stderr, "No suitable visual for glx\n");
      exit(1);
  }
      
  /* create a GLX context */
  cx = glXCreateContext(dpy, vi, 0, GL_TRUE);

  /* create a color map */
  cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen),
			 vi->visual, AllocNone);

  /* create a window */
  width = 400;
  height = 300;
  hint.x = 0;
  hint.y = 0;
  hint.width = width;
  hint.height = height;
  hint.flags = PPosition | PSize;
  swa.colormap = cmap;
  swa.border_pixel = 0;
  swa.event_mask = StructureNotifyMask;
  win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, width, height,
		      0, vi->depth, InputOutput, vi->visual,
		      CWBorderPixel|CWColormap|CWEventMask, &swa);
  XSetStandardProperties (dpy, win, name, name, None, NULL, 0, &hint);

  XMapWindow(dpy, win);
  XIfEvent(dpy, &event, WaitForNotify, (char*)win);
  XSelectInput(dpy, win, KeyPressMask | StructureNotifyMask | ExposureMask);

  /* connect the context to the window */
  glXMakeCurrent(dpy, win, cx);

  init();
  reshape(width, height);

  while (1) {
    if (XPending(dpy) > 0) {
      XNextEvent(dpy,&xev);
      switch(xev.type) {
      case KeyPress:
	XLookupString((XKeyEvent *)&xev,buf,80,&keysym,&status);
        switch(keysym) {
        case XK_Up:
            k = KEY_UP;
            break;
        case XK_Down:
            k = KEY_DOWN;
            break;
        case XK_Left:
            k = KEY_LEFT;
            break;
        case XK_Right:
            k = KEY_RIGHT;
            break;
        case XK_Escape:
            k = KEY_ESCAPE;
            break;
        default:
            k = keysym;
        }
        key(k, 0);
	break;
      case ConfigureNotify:
	{
	  int width,height;
	  width = xev.xconfigure.width;
	  height = xev.xconfigure.height;
	  glXWaitX();
          reshape(width, height);
	}
	break;
      }
    } else {
      idle();
    }
  }
  return 0;
}


