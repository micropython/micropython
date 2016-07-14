/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

// We need to include stdio.h below. Turn off GFILE_NEED_STDIO just for this file to prevent conflicts
#define GFILE_NEED_STDIO_MUST_BE_OFF

#include "gfx.h"

#if GFX_USE_GDISP

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <stdio.h>
#include <stdlib.h>

#define GDISP_DRIVER_VMT				GDISPVMT_X11
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

// Configuration parameters for this driver
#ifndef GDISP_FORCE_24BIT
	#define GDISP_FORCE_24BIT			FALSE
#endif
#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH			640
#endif
#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT			480
#endif
#ifndef GKEYBOARD_X_NO_LAYOUT
	/**
	 * Setting this to TRUE turns off the layout engine.
	 * In this situation "cooked" characters are returned but
	 * shift states etc are lost.
	 * As only a limited number of keyboard layouts are currently
	 * defined for X in uGFX (currently none), setting this
	 * to TRUE enables the X keyboard mapping to be pass non-English
	 * characters to uGFX or to handle non-standard keyboard layouts at
	 * the expense of losing special function keys etc.
	 */
	// We set this to TRUE by default as currently the X layout code is not complete!
	#define GKEYBOARD_X_NO_LAYOUT		TRUE
#endif
#ifndef GKEYBOARD_X_DEFAULT_LAYOUT
	#define GKEYBOARD_X_DEFAULT_LAYOUT	KeyboardLayout_X_US
#endif

// Driver status flags
#define GDISP_FLG_READY				(GDISP_FLG_DRIVER<<0)

#if GINPUT_NEED_MOUSE
	// Include mouse support code
	#define GMOUSE_DRIVER_VMT		GMOUSEVMT_X11
	#include "../../../src/ginput/ginput_driver_mouse.h"

	// Forward definitions
	static bool_t XMouseInit(GMouse *m, unsigned driverinstance);
	static bool_t XMouseRead(GMouse *m, GMouseReading *prd);

	const GMouseVMT const GMOUSE_DRIVER_VMT[1] = {{
		{
			GDRIVER_TYPE_MOUSE,
			GMOUSE_VFLG_NOPOLL|GMOUSE_VFLG_DYNAMICONLY,
				// Extra flags for testing only
				//GMOUSE_VFLG_TOUCH|GMOUSE_VFLG_SELFROTATION|GMOUSE_VFLG_DEFAULTFINGER
				//GMOUSE_VFLG_CALIBRATE|GMOUSE_VFLG_CAL_EXTREMES|GMOUSE_VFLG_CAL_TEST|GMOUSE_VFLG_CAL_LOADFREE
				//GMOUSE_VFLG_ONLY_DOWN|GMOUSE_VFLG_POORUPDOWN
			sizeof(GMouse),
			_gmouseInitDriver, _gmousePostInitDriver, _gmouseDeInitDriver
		},
		1,				// z_max
		0,				// z_min
		1,				// z_touchon
		0,				// z_touchoff
		{				// pen_jitter
			0,				// calibrate
			0,				// click
			0				// move
		},
		{				// finger_jitter
			0,				// calibrate
			2,				// click
			2				// move
		},
		XMouseInit,		// init
		0,				// deinit
		XMouseRead,		// get
		0,				// calsave
		0				// calload
	}};
#endif

#if GINPUT_NEED_KEYBOARD
	// Include mouse support code
	#define GKEYBOARD_DRIVER_VMT		GKEYBOARDVMT_X
	#include "../../../src/ginput/ginput_driver_keyboard.h"

	#if !GKEYBOARD_X_NO_LAYOUT
		#if GKEYBOARD_LAYOUT_OFF
			#error "The X keyboard driver is using the layout engine. Please set GKEYBOARD_LAYOUT_OFF=FALSE or GKEYBOARD_X_NO_LAYOUT=TRUE."
		#endif

		// Forward definitions
		extern uint8_t	GKEYBOARD_X_DEFAULT_LAYOUT[];

		#include "../../../src/ginput/ginput_keyboard_microcode.h"
		#include <X11/keysym.h>

		// This is the layout code for the English US keyboard.
		//	We make it public so that a user can switch to a different layout if required.
		uint8_t	KeyboardLayout_X_US[] = {
			KMC_HEADERSTART, KMC_HEADER_ID1, KMC_HEADER_ID2, KMC_HEADER_VER_1,

			// TODO
			#error "The code to do keyboard layouts in X is not complete."

			// Transient Shifters: SHIFT, CTRL, ALT, WINKEY
			// Locking Shifters: CAPSLOCK, NUMLOCK and SCROLLLOCK
			// Keyup, Repeat
			// 0 - 9
			// A - Z
			// Number pad
			// Symbols
			// Special Keys
			// Anything else
			// EOF
			KMC_RECORDSTART, 0
		};
	#elif !GKEYBOARD_LAYOUT_OFF
		#warning "The X keyboard driver is not using the layout engine. If no other keyboard is using it consider defining GKEYBOARD_LAYOUT_OFF=TRUE to save code size."
	#endif

	// Forward definitions
	static bool_t XKeyboardInit(GKeyboard *k, unsigned driverinstance);
	static int XKeyboardGetData(GKeyboard *k, uint8_t *pch, int sz);

	const GKeyboardVMT const GKEYBOARD_DRIVER_VMT[1] = {{
		{
			GDRIVER_TYPE_KEYBOARD,
			GKEYBOARD_VFLG_NOPOLL,			//  GKEYBOARD_VFLG_DYNAMICONLY
			sizeof(GKeyboard),
			_gkeyboardInitDriver, _gkeyboardPostInitDriver, _gkeyboardDeInitDriver
		},
	 	// The default keyboard layout
		#if GKEYBOARD_X_NO_LAYOUT
			0,
		#else
			GKEYBOARD_X_DEFAULT_LAYOUT,
		#endif
		XKeyboardInit,			// init
		0,						// deinit
		XKeyboardGetData,		// getdata
		0						// putdata		void	(*putdata)(GKeyboard *k, char ch);		Optional
	}};

	static int			keypos;
	static uint8_t		keybuffer[8];
	static GKeyboard	*keyboard;
#endif

static bool_t			initdone;
static Display			*dis;
static int				scr;
static XEvent			evt;
static Colormap			cmap;
static XVisualInfo		vis;
static XContext			cxt;
static Atom				wmDelete;

typedef struct xPriv {
	Pixmap			pix;
	GC 				gc;
	Window			win;
	#if GINPUT_NEED_MOUSE
		coord_t		mousex, mousey;
		uint16_t	buttons;
		GMouse *	mouse;
	#endif
} xPriv;

static void ProcessEvent(GDisplay *g, xPriv *priv) {
	switch(evt.type) {
	case MapNotify:
		XSelectInput(dis, evt.xmap.window,
				StructureNotifyMask|ExposureMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|KeyPressMask|KeyReleaseMask|KeymapStateMask);
		g->flags |= GDISP_FLG_READY;
		break;
	case UnmapNotify:
		XCloseDisplay(dis);
		exit(0);
		break;
	case ClientMessage:
		if ((Atom)evt.xclient.data.l[0] == wmDelete) {
			XCloseDisplay(dis);
			exit(0);
		}
		break;
	case Expose:
		XCopyArea(dis, priv->pix, evt.xexpose.window, priv->gc,
			evt.xexpose.x, evt.xexpose.y,
			evt.xexpose.width, evt.xexpose.height,
			evt.xexpose.x, evt.xexpose.y);
		break;
	#if GINPUT_NEED_MOUSE
		case ButtonPress:
			priv->mousex = evt.xbutton.x;
			priv->mousey = evt.xbutton.y;
			switch(evt.xbutton.button){
			case 1:	priv->buttons |= GINPUT_MOUSE_BTN_LEFT;		break;
			case 2:	priv->buttons |= GINPUT_MOUSE_BTN_MIDDLE;	break;
			case 3:	priv->buttons |= GINPUT_MOUSE_BTN_RIGHT;	break;
			case 4:	priv->buttons |= GINPUT_MOUSE_BTN_4;		break;
			}
			_gmouseWakeup(priv->mouse);
			break;
		case ButtonRelease:
			priv->mousex = evt.xbutton.x;
			priv->mousey = evt.xbutton.y;
			switch(evt.xbutton.button){
			case 1:	priv->buttons &= ~GINPUT_MOUSE_BTN_LEFT;	break;
			case 2:	priv->buttons &= ~GINPUT_MOUSE_BTN_MIDDLE;	break;
			case 3:	priv->buttons &= ~GINPUT_MOUSE_BTN_RIGHT;	break;
			case 4:	priv->buttons &= ~GINPUT_MOUSE_BTN_4;		break;
			}
			_gmouseWakeup(priv->mouse);
			break;
		case MotionNotify:
			priv->mousex = evt.xmotion.x;
			priv->mousey = evt.xmotion.y;
			_gmouseWakeup(priv->mouse);
			break;
	#endif
	#if GINPUT_NEED_KEYBOARD
		case KeymapNotify:
			XRefreshKeyboardMapping(&evt.xmapping);
			break;
		case KeyPress:
			#if !GKEYBOARD_X_NO_LAYOUT
				// TODO
				#error "The code to do keyboard layouts in X is not complete."
			#endif
			 /* ignore these when there is no layout engine */
			break;
		case KeyRelease:
			#if !GKEYBOARD_X_NO_LAYOUT
				// TODO
				#error "The code to do keyboard layouts in X is not complete."
			#endif
			if (keyboard && !keyboard->pLayout && keypos < (int)sizeof(keybuffer)) {
				int		len;

				len = XLookupString(&evt.xkey, (char *)(keybuffer+keypos), sizeof(keybuffer)-keypos, /*&keysym*/0, NULL);
				if (len > 0) {
					keypos += len;
					_gkeyboardWakeup(keyboard);
				}
			}
			break;
	#endif
	}
}

/* this is the X11 thread which keeps track of all events */
static DECLARE_THREAD_STACK(waXThread, 1024);
static DECLARE_THREAD_FUNCTION(ThreadX, arg) {
	GDisplay	*g;
	(void)arg;

	while(1) {
		gfxSleepMilliseconds(100);
		while(XPending(dis)) {
			XNextEvent(dis, &evt);
			XFindContext(evt.xany.display, evt.xany.window, cxt, (XPointer*)&g);
			ProcessEvent(g, (xPriv *)g->priv);
		}
	}
	return 0;
}

static int FatalXIOError(Display *d) {
	(void) d;

	/* The window has closed */
	fprintf(stderr, "GFX Window closed!\n");
	exit(0);
}

LLDSPEC bool_t gdisp_lld_init(GDisplay *g) {
	XSizeHints				*pSH;
	XSetWindowAttributes	xa;
	XTextProperty			WindowTitle;
	char *					WindowTitleText;
	xPriv					*priv;

	if (!initdone) {
		gfxThreadHandle			hth;

		initdone = TRUE;
		#if GFX_USE_OS_LINUX || GFX_USE_OS_OSX
			XInitThreads();
		#endif

		dis = XOpenDisplay(0);
		scr = DefaultScreen(dis);
		cxt = XUniqueContext();
		wmDelete = XInternAtom(dis, "WM_DELETE_WINDOW", False);
		XSetIOErrorHandler(FatalXIOError);

		#if GDISP_FORCE_24BIT
			if (!XMatchVisualInfo(dis, scr, 24, TrueColor, &vis)) {
				fprintf(stderr, "Your display has no TrueColor mode\n");
				XCloseDisplay(dis);
				return FALSE;
			}
			cmap = XCreateColormap(dis, RootWindow(dis, scr),
					vis.visual, AllocNone);
		#else
			vis.visual = CopyFromParent;
			vis.depth = DefaultDepth(dis, scr);
			cmap = DefaultColormap(dis, scr);
		#endif
		fprintf(stderr, "Running GFX Window in %d bit color\n", vis.depth);

		if (!(hth = gfxThreadCreate(waXThread, sizeof(waXThread), HIGH_PRIORITY, ThreadX, 0))) {
			fprintf(stderr, "Cannot start X Thread\n");
			XCloseDisplay(dis);
			exit(0);
		}
		#if GFX_USE_OS_LINUX || GFX_USE_OS_OSX
			pthread_detach(hth);
		#endif
		gfxThreadClose(hth);
	}

	g->priv = gfxAlloc(sizeof(xPriv));
	priv = (xPriv *)g->priv;
	g->board = 0;					// No board interface for this driver

	xa.colormap = cmap;
	xa.border_pixel = 0xFFFFFF;
	xa.background_pixel = 0x000000;

	priv->win = XCreateWindow(dis, RootWindow(dis, scr), 16, 16,
			GDISP_SCREEN_WIDTH, GDISP_SCREEN_HEIGHT,
			0, vis.depth, InputOutput, vis.visual,
			CWBackPixel|CWColormap|CWBorderPixel, &xa);
	XSync(dis, TRUE);

	XSaveContext(dis, priv->win, cxt, (XPointer)g);
	XSetWMProtocols(dis, priv->win, &wmDelete, 1);

	{
		char					buf[132];
		sprintf(buf, "uGFX - %u", g->systemdisplay+1);
		WindowTitleText = buf;
		XStringListToTextProperty(&WindowTitleText, 1, &WindowTitle);
		XSetWMName(dis, priv->win, &WindowTitle);
		XSetWMIconName(dis, priv->win, &WindowTitle);
		XSync(dis, TRUE);
	}

	pSH = XAllocSizeHints();
	pSH->flags = PSize | PMinSize | PMaxSize;
	pSH->min_width = pSH->max_width = pSH->base_width = GDISP_SCREEN_WIDTH;
	pSH->min_height = pSH->max_height = pSH->base_height = GDISP_SCREEN_HEIGHT;
	XSetWMNormalHints(dis, priv->win, pSH);
	XFree(pSH);
	XSync(dis, TRUE);

	priv->pix = XCreatePixmap(dis, priv->win,
				GDISP_SCREEN_WIDTH, GDISP_SCREEN_HEIGHT, vis.depth);
	XSync(dis, TRUE);

	priv->gc = XCreateGC(dis, priv->win, 0, 0);
	XSetBackground(dis, priv->gc, BlackPixel(dis, scr));
	XSync(dis, TRUE);

	// Create the associated mouse before the map
	#if GINPUT_NEED_MOUSE
		priv->mouse = (GMouse *)gdriverRegister((const GDriverVMT const *)GMOUSE_DRIVER_VMT, g);
	#endif

	XSelectInput(dis, priv->win, StructureNotifyMask);
	XMapWindow(dis, priv->win);

	// Wait for the window creation to complete (for safety)
	while(!(((volatile GDisplay *)g)->flags & GDISP_FLG_READY))
		gfxSleepMilliseconds(100);

	/* Initialise the GDISP structure to match */
    g->g.Orientation = GDISP_ROTATE_0;
    g->g.Powermode = powerOn;
    g->g.Backlight = 100;
    g->g.Contrast = 50;
    g->g.Width = GDISP_SCREEN_WIDTH;
    g->g.Height = GDISP_SCREEN_HEIGHT;

    return TRUE;
}

LLDSPEC void gdisp_lld_draw_pixel(GDisplay *g)
{
	xPriv *	priv = (xPriv *)g->priv;
	XColor	col;

	col.red = RED_OF(g->p.color) << 8;
	col.green = GREEN_OF(g->p.color) << 8;
	col.blue = BLUE_OF(g->p.color) << 8;
	XAllocColor(dis, cmap, &col);
	XSetForeground(dis, priv->gc, col.pixel);
	XDrawPoint(dis, priv->pix, priv->gc, (int)g->p.x, (int)g->p.y );
	XDrawPoint(dis, priv->win, priv->gc, (int)g->p.x, (int)g->p.y );
	XFlush(dis);
}

#if GDISP_HARDWARE_FILLS
	LLDSPEC void gdisp_lld_fill_area(GDisplay *g) {
		xPriv *	priv = (xPriv *)g->priv;
		XColor	col;

		col.red = RED_OF(g->p.color) << 8;
		col.green = GREEN_OF(g->p.color) << 8;
		col.blue = BLUE_OF(g->p.color) << 8;
		XAllocColor(dis, cmap, &col);
		XSetForeground(dis, priv->gc, col.pixel);
		XFillRectangle(dis, priv->pix, priv->gc, g->p.x, g->p.y, g->p.cx, g->p.cy);
		XFillRectangle(dis, priv->win, priv->gc, g->p.x, g->p.y, g->p.cx, g->p.cy);
		XFlush(dis);
	}
#endif

#if 0 && GDISP_HARDWARE_BITFILLS
	LLDSPEC void gdisp_lld_blit_area(GDisplay *g) {
		// Start of Bitblit code

		//XImage			bitmap;
		//pixel_t			*bits;
		//	bits = malloc(vis.depth * GDISP_SCREEN_WIDTH * GDISP_SCREEN_HEIGHT);
		//	bitmap = XCreateImage(dis, vis, vis.depth, ZPixmap,
		//				0, bits, GDISP_SCREEN_WIDTH, GDISP_SCREEN_HEIGHT,
		//				0, 0);
	}
#endif

#if GDISP_HARDWARE_PIXELREAD
	LLDSPEC	color_t gdisp_lld_get_pixel_color(GDisplay *g) {
		xPriv *	priv = (xPriv *)g->priv;
		XColor	color;
		XImage *img;

		img = XGetImage (dis, priv->pix, g->p.x, g->p.y, 1, 1, AllPlanes, XYPixmap);
		color.pixel = XGetPixel (img, 0, 0);
		XFree(img);
		XQueryColor(dis, cmap, &color);
		return RGB2COLOR(color.red>>8, color.green>>8, color.blue>>8);
	}
#endif

#if GDISP_NEED_SCROLL && GDISP_HARDWARE_SCROLL
	LLDSPEC void gdisp_lld_vertical_scroll(GDisplay *g) {
		xPriv *	priv = (xPriv *)g->priv;

		if (g->p.y1 > 0) {
			XCopyArea(dis, priv->pix, priv->pix, priv->gc, g->p.x, g->p.y+g->p.y1, g->p.cx, g->p.cy-g->p.y1, g->p.x, g->p.y);
			XCopyArea(dis, priv->pix, priv->win, priv->gc, g->p.x, g->p.y, g->p.cx, g->p.cy-g->p.y1, g->p.x, g->p.y);
		} else {
			XCopyArea(dis, priv->pix, priv->pix, priv->gc, g->p.x, g->p.y, g->p.cx, g->p.cy+g->p.y1, g->p.x, g->p.y-g->p.y1);
			XCopyArea(dis, priv->pix, priv->win, priv->gc, g->p.x, g->p.y-g->p.y1, g->p.cx, g->p.cy+g->p.y1, g->p.x, g->p.y-g->p.y1);
		}
	}
#endif

#if GINPUT_NEED_MOUSE
	static bool_t XMouseInit(GMouse *m, unsigned driverinstance) {
		(void)	m;
		(void)	driverinstance;
		return TRUE;
	}
	static bool_t XMouseRead(GMouse *m, GMouseReading *pt) {
		xPriv	*	priv;

		priv = m->display->priv;
		pt->x = priv->mousex;
		pt->y = priv->mousey;
		pt->z = (priv->buttons & GINPUT_MOUSE_BTN_LEFT) ? 1 : 0;
		pt->buttons = priv->buttons;
		return TRUE;
	}
#endif /* GINPUT_NEED_MOUSE */

#if GINPUT_NEED_KEYBOARD
	static bool_t XKeyboardInit(GKeyboard *k, unsigned driverinstance) {
		(void)	driverinstance;

		// Only one please
		if (keyboard)
			return FALSE;

		keyboard = k;
		return TRUE;
	}

	static int XKeyboardGetData(GKeyboard *k, uint8_t *pch, int sz) {
		int		i, j;
		(void)	k;

		if (!keypos)
			return 0;

		for(i = 0; i < keypos && i < sz; i++)
			pch[i] = keybuffer[i];
		keypos -= i;
		for(j=0; j < keypos; j++)
			keybuffer[j] = keybuffer[i+j];
		return i;
	}
#endif /* GINPUT_NEED_KEYBOARD */

#endif /* GFX_USE_GDISP */
