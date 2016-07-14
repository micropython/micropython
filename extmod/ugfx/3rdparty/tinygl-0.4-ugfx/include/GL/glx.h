#ifndef GLX_H
#define GLX_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>


/* the following comes from Mesa */

#ifdef __cplusplus
extern "C" {
#endif


#define GLX_VERSION_1_1		1


/*
 * Tokens for glXChooseVisual and glXGetConfig:
 */
enum _GLX_CONFIGS {
	GLX_USE_GL		= 1,
	GLX_BUFFER_SIZE		= 2,
	GLX_LEVEL		= 3,
	GLX_RGBA		= 4,
	GLX_DOUBLEBUFFER	= 5, 
	GLX_STEREO		= 6,
	GLX_AUX_BUFFERS		= 7,
	GLX_RED_SIZE		= 8,
	GLX_GREEN_SIZE		= 9,
	GLX_BLUE_SIZE		= 10,
	GLX_ALPHA_SIZE		= 11,
	GLX_DEPTH_SIZE		= 12,
	GLX_STENCIL_SIZE	= 13,
	GLX_ACCUM_RED_SIZE	= 14,
	GLX_ACCUM_GREEN_SIZE	= 15,
	GLX_ACCUM_BLUE_SIZE	= 16,
	GLX_ACCUM_ALPHA_SIZE	= 17,

	/* GLX_EXT_visual_info extension */
	GLX_X_VISUAL_TYPE_EXT		= 0x22,
	GLX_TRANSPARENT_TYPE_EXT	= 0x23,
	GLX_TRANSPARENT_INDEX_VALUE_EXT	= 0x24,
	GLX_TRANSPARENT_RED_VALUE_EXT	= 0x25,
	GLX_TRANSPARENT_GREEN_VALUE_EXT	= 0x26,
	GLX_TRANSPARENT_BLUE_VALUE_EXT	= 0x27,
	GLX_TRANSPARENT_ALPHA_VALUE_EXT	= 0x28
};


/*
 * Error codes returned by glXGetConfig:
 */
#define GLX_BAD_SCREEN		1
#define GLX_BAD_ATTRIBUTE	2
#define GLX_NO_EXTENSION	3
#define GLX_BAD_VISUAL		4
#define GLX_BAD_CONTEXT		5
#define GLX_BAD_VALUE       	6
#define GLX_BAD_ENUM		7


/*
 * GLX 1.1 and later:
 */
#define GLX_VENDOR		1
#define GLX_VERSION		2
#define GLX_EXTENSIONS 		3


/*
 * GLX_visual_info extension
 */
#define GLX_TRUE_COLOR_EXT		0x8002
#define GLX_DIRECT_COLOR_EXT		0x8003
#define GLX_PSEUDO_COLOR_EXT		0x8004
#define GLX_STATIC_COLOR_EXT		0x8005
#define GLX_GRAY_SCALE_EXT		0x8006
#define GLX_STATIC_GRAY_EXT		0x8007
#define GLX_NONE_EXT			0x8000
#define GLX_TRANSPARENT_RGB_EXT		0x8008
#define GLX_TRANSPARENT_INDEX_EXT	0x8009


typedef void *GLXContext;
typedef Pixmap GLXPixmap;
typedef Drawable GLXDrawable;
typedef XID GLXContextID;


extern XVisualInfo* glXChooseVisual( Display *dpy, int screen,
				     int *attribList );

extern GLXContext glXCreateContext( Display *dpy, XVisualInfo *vis,
				    GLXContext shareList, Bool direct );

extern void glXDestroyContext( Display *dpy, GLXContext ctx );

extern Bool glXMakeCurrent( Display *dpy, GLXDrawable drawable,
			    GLXContext ctx);

extern void glXCopyContext( Display *dpy, GLXContext src, GLXContext dst,
			    GLuint mask );

extern void glXSwapBuffers( Display *dpy, GLXDrawable drawable );

extern GLXPixmap glXCreateGLXPixmap( Display *dpy, XVisualInfo *visual,
				     Pixmap pixmap );

extern void glXDestroyGLXPixmap( Display *dpy, GLXPixmap pixmap );

extern Bool glXQueryExtension( Display *dpy, int *errorb, int *event );

extern Bool glXQueryVersion( Display *dpy, int *maj, int *min );

extern Bool glXIsDirect( Display *dpy, GLXContext ctx );

extern int glXGetConfig( Display *dpy, XVisualInfo *visual,
			 int attrib, int *value );

extern GLXContext glXGetCurrentContext( void );

extern GLXDrawable glXGetCurrentDrawable( void );

extern void glXWaitGL( void );

extern void glXWaitX( void );

extern void glXUseXFont( Font font, int first, int count, int list );


/* GLX 1.1 and later */
extern const char *glXQueryExtensionsString( Display *dpy, int screen );

extern const char *glXQueryServerString( Display *dpy, int screen, int name );

extern const char *glXGetClientString( Display *dpy, int name );

#ifdef __cplusplus
}
#endif

#endif
