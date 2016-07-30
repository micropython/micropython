/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_GDISP

/* Include the low level driver information */
#include "gdisp_driver.h"

// Number of milliseconds for the startup logo - 0 means disabled.
#if GDISP_NEED_STARTUP_LOGO
	#define GDISP_STARTUP_LOGO_TIMEOUT		1000
	#define GDISP_STARTUP_LOGO_COLOR		White
#else
	#define GDISP_STARTUP_LOGO_TIMEOUT		0
#endif

// For internal use only.
#if GDISP_NEED_TEXT_WORDWRAP
	typedef struct wrapParameters {
		GDisplay* g;
		coord_t x;
		coord_t y;
		font_t font;
		justify_t justify;
	} wrapParameters_t;
#endif

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

#if GDISP_NEED_TIMERFLUSH
	static GTimer	FlushTimer;
#endif

GDisplay	*GDISP;

#if GDISP_NEED_MULTITHREAD
	#define MUTEX_INIT(g)		gfxMutexInit(&(g)->mutex)
	#define MUTEX_ENTER(g)		gfxMutexEnter(&(g)->mutex)
	#define MUTEX_EXIT(g)		gfxMutexExit(&(g)->mutex)
	#define MUTEX_DEINIT(g)		gfxMutexDestroy(&(g)->mutex)
#else
	#define MUTEX_INIT(g)
	#define MUTEX_ENTER(g)
	#define MUTEX_EXIT(g)
	#define MUTEX_DEINIT(g)
#endif

#define NEED_CLIPPING	(GDISP_HARDWARE_CLIP != TRUE && (GDISP_NEED_VALIDATION || GDISP_NEED_CLIP))

#if !NEED_CLIPPING
	#define TEST_CLIP_AREA(g)
#elif GDISP_HARDWARE_CLIP == HARDWARE_AUTODETECT
	#define TEST_CLIP_AREA(g)																					\
			if (!gvmt(g)->setclip) {																				\
				if ((g)->p.x < (g)->clipx0) { (g)->p.cx -= (g)->clipx0 - (g)->p.x; (g)->p.x = (g)->clipx0; }	\
				if ((g)->p.y < (g)->clipy0) { (g)->p.cy -= (g)->clipy0 - (g)->p.y; (g)->p.y = (g)->clipy0; }	\
				if ((g)->p.x + (g)->p.cx > (g)->clipx1)	(g)->p.cx = (g)->clipx1 - (g)->p.x;						\
				if ((g)->p.y + (g)->p.cy > (g)->clipy1)	(g)->p.cy = (g)->clipy1 - (g)->p.y;						\
			}																									\
			if ((g)->p.cx > 0 && (g)->p.cy > 0)
#else
	#define TEST_CLIP_AREA(g)																				\
			if ((g)->p.x < (g)->clipx0) { (g)->p.cx -= (g)->clipx0 - (g)->p.x; (g)->p.x = (g)->clipx0; }	\
			if ((g)->p.y < (g)->clipy0) { (g)->p.cy -= (g)->clipy0 - (g)->p.y; (g)->p.y = (g)->clipy0; }	\
			if ((g)->p.x + (g)->p.cx > (g)->clipx1)	(g)->p.cx = (g)->clipx1 - (g)->p.x;						\
			if ((g)->p.y + (g)->p.cy > (g)->clipy1)	(g)->p.cy = (g)->clipy1 - (g)->p.y;						\
			if ((g)->p.cx > 0 && (g)->p.cy > 0)
#endif

/*==========================================================================*/
/* Internal functions.														*/
/*==========================================================================*/

#if GDISP_HARDWARE_STREAM_POS && GDISP_HARDWARE_STREAM_WRITE
	static GFXINLINE void setglobalwindow(GDisplay *g) {
		coord_t	x, y;
		x = g->p.x; y = g->p.y;
		g->p.x = g->p.y = 0;
		g->p.cx = g->g.Width; g->p.cy = g->g.Height;
		gdisp_lld_write_start(g);
		g->p.x = x; g->p.y = y;
		g->flags |= GDISP_FLG_SCRSTREAM;
	}
#endif

#if GDISP_NEED_AUTOFLUSH && GDISP_HARDWARE_FLUSH == HARDWARE_AUTODETECT
	#define autoflush_stopdone(g)	if (gvmt(g)->flush) gdisp_lld_flush(g)
#elif GDISP_NEED_AUTOFLUSH && GDISP_HARDWARE_FLUSH
	#define autoflush_stopdone(g)	gdisp_lld_flush(g)
#else
	#define autoflush_stopdone(g)
#endif

#if GDISP_HARDWARE_STREAM_POS && GDISP_HARDWARE_STREAM_WRITE
	#define autoflush(g)									\
			{												\
				if ((g->flags & GDISP_FLG_SCRSTREAM)) {		\
					gdisp_lld_write_stop(g);				\
					g->flags &= ~GDISP_FLG_SCRSTREAM;		\
				}											\
				autoflush_stopdone(g);						\
			}
#else
	#define autoflush(g)		autoflush_stopdone(g)
#endif

// drawpixel(g)
// Parameters:	x,y
// Alters:		cx, cy (if using streaming)
// Does not clip
static GFXINLINE void drawpixel(GDisplay *g) {

	// Best is hardware accelerated pixel draw
	#if GDISP_HARDWARE_DRAWPIXEL
		#if GDISP_HARDWARE_DRAWPIXEL == HARDWARE_AUTODETECT
			if (gvmt(g)->pixel)
		#endif
		{
			gdisp_lld_draw_pixel(g);
			return;
		}
	#endif

	// Next best is cursor based streaming
	#if GDISP_HARDWARE_DRAWPIXEL != TRUE && GDISP_HARDWARE_STREAM_POS && GDISP_HARDWARE_STREAM_WRITE
		#if GDISP_HARDWARE_STREAM_POS == HARDWARE_AUTODETECT
			if (gvmt(g)->writepos)
		#endif
		{
			if (!(g->flags & GDISP_FLG_SCRSTREAM))
				setglobalwindow(g);
			gdisp_lld_write_pos(g);
			gdisp_lld_write_color(g);
			return;
		}
	#endif

	// Worst is general streaming
	#if GDISP_HARDWARE_DRAWPIXEL != TRUE && GDISP_HARDWARE_STREAM_POS != TRUE && GDISP_HARDWARE_STREAM_WRITE
		// The following test is unneeded because we are guaranteed to have streaming if we don't have drawpixel
		//#if GDISP_HARDWARE_STREAM_WRITE == HARDWARE_AUTODETECT
		//	if (gvmt(g)->writestart)
		//#endif
		{
			g->p.cx = g->p.cy = 1;
			gdisp_lld_write_start(g);
			gdisp_lld_write_color(g);
			gdisp_lld_write_stop(g);
			return;
		}
	#endif
}

// drawpixel_clip(g)
// Parameters:	x,y
// Alters:		cx, cy (if using streaming)
#if NEED_CLIPPING
	static GFXINLINE void drawpixel_clip(GDisplay *g) {
		#if GDISP_HARDWARE_CLIP == HARDWARE_AUTODETECT
			if (!gvmt(g)->setclip)
		#endif
		{
			if (g->p.x < g->clipx0 || g->p.x >= g->clipx1 || g->p.y < g->clipy0 || g->p.y >= g->clipy1)
				return;
		}
		drawpixel(g);
	}
#else
	#define drawpixel_clip(g)		drawpixel(g)
#endif

// fillarea(g)
// Parameters:	x,y cx,cy and color
// Alters:		nothing
// Note:		This is not clipped
// Resets the streaming area if GDISP_HARDWARE_STREAM_WRITE and GDISP_HARDWARE_STREAM_POS is set.
static GFXINLINE void fillarea(GDisplay *g) {

	// Best is hardware accelerated area fill
	#if GDISP_HARDWARE_FILLS
		#if GDISP_HARDWARE_FILLS == HARDWARE_AUTODETECT
			if (gvmt(g)->fill)
		#endif
		{
			gdisp_lld_fill_area(g);
			return;
		}
	#endif

	// Next best is hardware streaming
	#if GDISP_HARDWARE_FILLS != TRUE && GDISP_HARDWARE_STREAM_WRITE
		#if GDISP_HARDWARE_STREAM_WRITE == HARDWARE_AUTODETECT
			if (gvmt(g)->writestart)
		#endif
		{
			uint32_t	area;

			#if GDISP_HARDWARE_STREAM_POS
				if ((g->flags & GDISP_FLG_SCRSTREAM)) {
					gdisp_lld_write_stop(g);
					g->flags &= ~GDISP_FLG_SCRSTREAM;
				}
			#endif

			area = (uint32_t)g->p.cx * g->p.cy;
			gdisp_lld_write_start(g);
			#if GDISP_HARDWARE_STREAM_POS
				#if GDISP_HARDWARE_STREAM_POS == HARDWARE_AUTODETECT
					if (gvmt(g)->writepos)
				#endif
				gdisp_lld_write_pos(g);
			#endif
			for(; area; area--)
				gdisp_lld_write_color(g);
			gdisp_lld_write_stop(g);
			return;
		}
	#endif

	// Worst is pixel drawing
	#if GDISP_HARDWARE_FILLS != TRUE && GDISP_HARDWARE_STREAM_WRITE != TRUE && GDISP_HARDWARE_DRAWPIXEL
		// The following test is unneeded because we are guaranteed to have draw pixel if we don't have streaming
		//#if GDISP_HARDWARE_DRAWPIXEL == HARDWARE_AUTODETECT
		//	if (gvmt(g)->pixel)
		//#endif
		{
			coord_t x0, y0, x1, y1;

			x0 = g->p.x;
			y0 = g->p.y;
			x1 = g->p.x + g->p.cx;
			y1 = g->p.y + g->p.cy;
			for(; g->p.y < y1; g->p.y++, g->p.x = x0)
				for(; g->p.x < x1; g->p.x++)
					gdisp_lld_draw_pixel(g);
			g->p.y = y0;
			return;
		}
	#endif
}

// Parameters:	x,y and x1
// Alters:		x,y x1,y1 cx,cy
// Assumes the window covers the screen and a write_stop() will occur later
//	if GDISP_HARDWARE_STREAM_WRITE and GDISP_HARDWARE_STREAM_POS is set.
static void hline_clip(GDisplay *g) {
	// Swap the points if necessary so it always goes from x to x1
	if (g->p.x1 < g->p.x) {
		g->p.cx = g->p.x; g->p.x = g->p.x1; g->p.x1 = g->p.cx;
	}

	// Clipping
	#if NEED_CLIPPING
		#if GDISP_HARDWARE_CLIP == HARDWARE_AUTODETECT
			if (!gvmt(g)->setclip)
		#endif
		{
			if (g->p.y < g->clipy0 || g->p.y >= g->clipy1) return;
			if (g->p.x < g->clipx0) g->p.x = g->clipx0;
			if (g->p.x1 >= g->clipx1) g->p.x1 = g->clipx1 - 1;
			if (g->p.x1 < g->p.x) return;
		}
	#endif

	// This is an optimization for the point case. It is only worthwhile however if we
	// have hardware fills or if we support both hardware pixel drawing and hardware streaming
	#if GDISP_HARDWARE_FILLS || (GDISP_HARDWARE_DRAWPIXEL && GDISP_HARDWARE_STREAM_WRITE)
		// Is this a point
		if (g->p.x == g->p.x1) {
			drawpixel(g);
			return;
		}
	#endif

	// Best is hardware accelerated area fill
	#if GDISP_HARDWARE_FILLS
		#if GDISP_HARDWARE_FILLS == HARDWARE_AUTODETECT
			if (gvmt(g)->fill)
		#endif
		{
			g->p.cx = g->p.x1 - g->p.x + 1;
			g->p.cy = 1;
			gdisp_lld_fill_area(g);
			return;
		}
	#endif

	// Next best is cursor based streaming
	#if GDISP_HARDWARE_FILLS != TRUE && GDISP_HARDWARE_STREAM_POS && GDISP_HARDWARE_STREAM_WRITE
		#if GDISP_HARDWARE_STREAM_POS == HARDWARE_AUTODETECT
			if (gvmt(g)->writepos)
		#endif
		{
			if (!(g->flags & GDISP_FLG_SCRSTREAM))
				setglobalwindow(g);
			g->p.cx = g->p.x1 - g->p.x + 1;
			gdisp_lld_write_pos(g);
			do { gdisp_lld_write_color(g); } while(--g->p.cx);
			return;
		}
	#endif

	// Next best is streaming
	#if GDISP_HARDWARE_FILLS != TRUE && GDISP_HARDWARE_STREAM_POS != TRUE && GDISP_HARDWARE_STREAM_WRITE
		#if GDISP_HARDWARE_STREAM_WRITE == HARDWARE_AUTODETECT
			if (gvmt(g)->writestart)
		#endif
		{
			g->p.cx = g->p.x1 - g->p.x + 1;
			g->p.cy = 1;
			gdisp_lld_write_start(g);
			do { gdisp_lld_write_color(g); } while(--g->p.cx);
			gdisp_lld_write_stop(g);
			return;
		}
	#endif

	// Worst is drawing pixels
	#if GDISP_HARDWARE_FILLS != TRUE && GDISP_HARDWARE_STREAM_WRITE != TRUE && GDISP_HARDWARE_DRAWPIXEL
		// The following test is unneeded because we are guaranteed to have draw pixel if we don't have streaming
		//#if GDISP_HARDWARE_DRAWPIXEL == HARDWARE_AUTODETECT
		//	if (gvmt(g)->pixel)
		//#endif
		{
			for(; g->p.x <= g->p.x1; g->p.x++)
				gdisp_lld_draw_pixel(g);
			return;
		}
	#endif
}

// Parameters:	x,y and y1
// Alters:		x,y x1,y1 cx,cy
static void vline_clip(GDisplay *g) {
	// Swap the points if necessary so it always goes from y to y1
	if (g->p.y1 < g->p.y) {
		g->p.cy = g->p.y; g->p.y = g->p.y1; g->p.y1 = g->p.cy;
	}

	// Clipping
	#if NEED_CLIPPING
		#if GDISP_HARDWARE_CLIP == HARDWARE_AUTODETECT
			if (!gvmt(g)->setclip)
		#endif
		{
			if (g->p.x < g->clipx0 || g->p.x >= g->clipx1) return;
			if (g->p.y < g->clipy0) g->p.y = g->clipy0;
			if (g->p.y1 >= g->clipy1) g->p.y1 = g->clipy1 - 1;
			if (g->p.y1 < g->p.y) return;
		}
	#endif

	// This is an optimization for the point case. It is only worthwhile however if we
	// have hardware fills or if we support both hardware pixel drawing and hardware streaming
	#if GDISP_HARDWARE_FILLS || (GDISP_HARDWARE_DRAWPIXEL && GDISP_HARDWARE_STREAM_WRITE) || (GDISP_HARDWARE_STREAM_POS && GDISP_HARDWARE_STREAM_WRITE)
		// Is this a point
		if (g->p.y == g->p.y1) {
			drawpixel(g);
			return;
		}
	#endif

	// Best is hardware accelerated area fill
	#if GDISP_HARDWARE_FILLS
		#if GDISP_HARDWARE_FILLS == HARDWARE_AUTODETECT
			if (gvmt(g)->fill)
		#endif
		{
			g->p.cy = g->p.y1 - g->p.y + 1;
			g->p.cx = 1;
			gdisp_lld_fill_area(g);
			return;
		}
	#endif

	// Next best is streaming
	#if GDISP_HARDWARE_FILLS != TRUE && GDISP_HARDWARE_STREAM_WRITE
		#if GDISP_HARDWARE_STREAM_WRITE == HARDWARE_AUTODETECT
			if (gvmt(g)->writestart)
		#endif
		{
			#if GDISP_HARDWARE_STREAM_POS
				if ((g->flags & GDISP_FLG_SCRSTREAM)) {
					gdisp_lld_write_stop(g);
					g->flags &= ~GDISP_FLG_SCRSTREAM;
				}
			#endif
			g->p.cy = g->p.y1 - g->p.y + 1;
			g->p.cx = 1;
			gdisp_lld_write_start(g);
			#if GDISP_HARDWARE_STREAM_POS
				#if GDISP_HARDWARE_STREAM_POS == HARDWARE_AUTODETECT
					if (gvmt(g)->writepos)
				#endif
				gdisp_lld_write_pos(g);
			#endif
			do { gdisp_lld_write_color(g); } while(--g->p.cy);
			gdisp_lld_write_stop(g);
			return;
		}
	#endif

	// Worst is drawing pixels
	#if GDISP_HARDWARE_FILLS != TRUE && GDISP_HARDWARE_STREAM_WRITE != TRUE && GDISP_HARDWARE_DRAWPIXEL
		// The following test is unneeded because we are guaranteed to have draw pixel if we don't have streaming
		//#if GDISP_HARDWARE_DRAWPIXEL == HARDWARE_AUTODETECT
		//	if (gvmt(g)->pixel)
		//#endif
		{
			for(; g->p.y <= g->p.y1; g->p.y++)
				gdisp_lld_draw_pixel(g);
			return;
		}
	#endif
}

// Parameters:	x,y and x1,y1
// Alters:		x,y x1,y1 cx,cy
static void line_clip(GDisplay *g) {
	int16_t dy, dx;
	int16_t addx, addy;
	int16_t P, diff, i;

	// Is this a horizontal line (or a point)
	if (g->p.y == g->p.y1) {
		hline_clip(g);
		return;
	}

	// Is this a vertical line (or a point)
	if (g->p.x == g->p.x1) {
		vline_clip(g);
		return;
	}

	// Not horizontal or vertical

	// Use Bresenham's line drawing algorithm.
	//	This should be replaced with fixed point slope based line drawing
	//	which is more efficient on modern processors as it branches less.
	//	When clipping is needed, all the clipping could also be done up front
	//	instead of on each pixel.

	if (g->p.x1 >= g->p.x) {
		dx = g->p.x1 - g->p.x;
		addx = 1;
	} else {
		dx = g->p.x - g->p.x1;
		addx = -1;
	}
	if (g->p.y1 >= g->p.y) {
		dy = g->p.y1 - g->p.y;
		addy = 1;
	} else {
		dy = g->p.y - g->p.y1;
		addy = -1;
	}

	if (dx >= dy) {
		dy <<= 1;
		P = dy - dx;
		diff = P - dx;

		for(i=0; i<=dx; ++i) {
			drawpixel_clip(g);
			if (P < 0) {
				P  += dy;
				g->p.x += addx;
			} else {
				P  += diff;
				g->p.x += addx;
				g->p.y += addy;
			}
		}
	} else {
		dx <<= 1;
		P = dx - dy;
		diff = P - dy;

		for(i=0; i<=dy; ++i) {
			drawpixel_clip(g);
			if (P < 0) {
				P  += dx;
				g->p.y += addy;
			} else {
				P  += diff;
				g->p.x += addx;
				g->p.y += addy;
			}
		}
	}
}

#if GDISP_STARTUP_LOGO_TIMEOUT > 0
	static bool_t	gdispInitDone;
	static void StartupLogoDisplay(GDisplay *g) {
		coord_t			x, y, w;
		const coord_t *	p;
		static const coord_t blks[] = {
				// u
				2, 6, 1, 10,
				3, 11, 4, 1,
				6, 6, 1, 6,
				// G
				8, 0, 1, 12,
				9, 0, 6, 1,
				9, 11, 6, 1,
				14, 6, 1, 5,
				12, 6, 2, 1,
				// F
				16, 0, 1, 12,
				17, 0, 6, 1,
				17, 6, 3, 1,
				// X
				22, 6, 7, 1,
				24, 0, 1, 6,
				22, 7, 1, 5,
				28, 0, 1, 6,
				26, 7, 1, 5,
		};

		// Get a starting position and a scale
		// Work on a 8x16 grid for each char, 4 chars (uGFX) in 1 line, using half the screen
		w = g->g.Width/(8*4*2);
		if (!w) w = 1;
		x = (g->g.Width - (8*4)*w)/2;
		y = (g->g.Height - (16*1)*w)/2;

		// Simple but crude!
		for(p = blks; p < blks+sizeof(blks)/sizeof(blks[0]); p+=4)
			gdispGFillArea(g, x+p[0]*w, y+p[1]*w, p[2]*w, p[3]*w, GDISP_STARTUP_LOGO_COLOR);
	}
#endif

#if GDISP_NEED_TIMERFLUSH
	static void FlushTimerFn(void *param) {
		GDisplay *	g;
		(void)		param;

		for(g = (GDisplay *)gdriverGetNext(GDRIVER_TYPE_DISPLAY, 0); g; g = (GDisplay *)gdriverGetNext(GDRIVER_TYPE_DISPLAY, (GDriver *)g))
			gdispGFlush(g);
	}
#endif

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

void _gdispInit(void)
{
	// GDISP_DRIVER_LIST is defined - create each driver instance
	#if defined(GDISP_DRIVER_LIST)
		{
			unsigned	i;
			typedef const GDISPVMT const GDISPVMTLIST[1];

			extern GDISPVMTLIST						  GDISP_DRIVER_LIST;
			static const GDISPVMT * const dclist[] = {GDISP_DRIVER_LIST};

			for(i = 0; i < sizeof(dclist)/sizeof(dclist[0]); i++) {
				if (!(dclist[i]->d.flags & GDISP_VFLG_DYNAMICONLY))
					gdriverRegister(&dclist[i]->d, 0);
			}
		}
	#elif GDISP_TOTAL_DISPLAYS > 1
		{
			unsigned	i;
			extern const GDISPVMT const		GDISPVMT_OnlyOne[1];

			if (!(GDISPVMT_OnlyOne->d.flags & GDISP_VFLG_DYNAMICONLY)) {
				for(i = 0; i < GDISP_TOTAL_DISPLAYS; i++)
					gdriverRegister(&GDISPVMT_OnlyOne->d, 0);
			}
		}
	#else
		{
			extern const GDISPVMT const		GDISPVMT_OnlyOne[1];

			if (!(GDISPVMT_OnlyOne->d.flags & GDISP_VFLG_DYNAMICONLY))
				gdriverRegister(&GDISPVMT_OnlyOne->d, 0);
		}
	#endif

	// Re-clear the display after the timeout if we added the logo
	#if GDISP_STARTUP_LOGO_TIMEOUT > 0
		{
			GDisplay	*g;

			gfxSleepMilliseconds(GDISP_STARTUP_LOGO_TIMEOUT);

			for(g = (GDisplay *)gdriverGetNext(GDRIVER_TYPE_DISPLAY, 0); g; g = (GDisplay *)gdriverGetNext(GDRIVER_TYPE_DISPLAY, (GDriver *)g)) {
				gdispGClear(g, GDISP_STARTUP_COLOR);
				#if GDISP_HARDWARE_FLUSH
					gdispGFlush(g);
				#endif
			}

			gdispInitDone = TRUE;
		}
	#endif

	// Start the automatic timer flush (if required)
	#if GDISP_NEED_TIMERFLUSH
		gtimerInit(&FlushTimer);
		gtimerStart(&FlushTimer, FlushTimerFn, 0, TRUE, GDISP_NEED_TIMERFLUSH);
	#endif
}

void _gdispDeinit(void)
{
	/* ToDo */
}

bool_t _gdispInitDriver(GDriver *g, void *param, unsigned driverinstance, unsigned systeminstance) {
	#define		gd		((GDisplay *)g)
	bool_t		ret;

	// Intialise fields
	gd->systemdisplay = systeminstance;
	gd->controllerdisplay = driverinstance;
	gd->flags = 0;
	gd->priv = param;
	MUTEX_INIT(gd);

	// Call the driver init
	MUTEX_ENTER(gd);
	ret = gdisp_lld_init(gd);
	MUTEX_EXIT(gd);
	return ret;

	#undef gd
}

void _gdispPostInitDriver(GDriver *g) {
	#define		gd		((GDisplay *)g)

	// Set orientation, clip
	#if defined(GDISP_DEFAULT_ORIENTATION) && GDISP_NEED_CONTROL && GDISP_HARDWARE_CONTROL
		#if GDISP_NEED_PIXMAP
			// Pixmaps should stay in their created orientation (at least initially)
			if (!(gvmt(gd)->d.flags & GDISP_VFLG_PIXMAP))
		#endif
			gdispGControl(gd, GDISP_CONTROL_ORIENTATION, (void *)GDISP_DEFAULT_ORIENTATION);
	#endif
	#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
		gdispGSetClip(gd, 0, 0, gd->g.Width, gd->g.Height);
	#endif

	// Clear the Screen
	gdispGClear(gd, GDISP_STARTUP_COLOR);

	// Display the startup logo if this is a static initialised display
	#if GDISP_STARTUP_LOGO_TIMEOUT > 0
		if (!gdispInitDone)
			StartupLogoDisplay(gd);
	#endif

	// Flush
	#if GDISP_HARDWARE_FLUSH
		gdispGFlush(gd);
	#endif

	// If this is the first driver set GDISP
	if (!GDISP)
		GDISP = gd;

	#undef gd
}

void _gdispDeInitDriver(GDriver *g) {
	#define		gd		((GDisplay *)g)

	if (GDISP == gd)
		GDISP = (GDisplay *)gdriverGetInstance(GDRIVER_TYPE_DISPLAY, 0);

	#if GDISP_HARDWARE_DEINIT
		#if GDISP_HARDWARE_DEINIT == HARDWARE_AUTODETECT
			if (gvmt(gd)->deinit)
		#endif
		{
			MUTEX_ENTER(gd);
			gdisp_lld_deinit(gd);
			MUTEX_EXIT(gd);
		}
	#endif
	MUTEX_DEINIT(gd);

	#undef gd
}

GDisplay *gdispGetDisplay(unsigned display) {
	return (GDisplay *)gdriverGetInstance(GDRIVER_TYPE_DISPLAY, display);
}

void gdispSetDisplay(GDisplay *g) {
	if (g) GDISP = g;
}

unsigned gdispGetDisplayCount(void) {
	return gdriverInstanceCount(GDRIVER_TYPE_DISPLAY);
}

coord_t gdispGGetWidth(GDisplay *g)				{ return g->g.Width; }
coord_t gdispGGetHeight(GDisplay *g)			{ return g->g.Height; }
powermode_t gdispGGetPowerMode(GDisplay *g)		{ return g->g.Powermode; }
orientation_t gdispGGetOrientation(GDisplay *g)	{ return g->g.Orientation; }
uint8_t gdispGGetBacklight(GDisplay *g)			{ return g->g.Backlight; }
uint8_t gdispGGetContrast(GDisplay *g)			{ return g->g.Contrast; }

void gdispGFlush(GDisplay *g) {
	#if GDISP_HARDWARE_FLUSH
		#if GDISP_HARDWARE_FLUSH == HARDWARE_AUTODETECT
			if (gvmt(g)->flush)
		#endif
		{
			MUTEX_ENTER(g);
			gdisp_lld_flush(g);
			MUTEX_EXIT(g);
		}
	#else
		(void) g;
	#endif
}

#if GDISP_NEED_STREAMING
	void gdispGStreamStart(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy) {
		MUTEX_ENTER(g);

		#if NEED_CLIPPING
			#if GDISP_HARDWARE_CLIP == HARDWARE_AUTODETECT
				if (!gvmt(g)->setclip)
			#endif
			// Test if the area is valid - if not then exit
			if (x < g->clipx0 || x+cx > g->clipx1 || y < g->clipy0 || y+cy > g->clipy1) {
				MUTEX_EXIT(g);
				return;
			}
		#endif

		g->flags |= GDISP_FLG_INSTREAM;

		// Best is hardware streaming
		#if GDISP_HARDWARE_STREAM_WRITE
			#if GDISP_HARDWARE_STREAM_WRITE == HARDWARE_AUTODETECT
				if (gvmt(g)->writestart)
			#endif
			{
				g->p.x = x;
				g->p.y = y;
				g->p.cx = cx;
				g->p.cy = cy;
				gdisp_lld_write_start(g);
				#if GDISP_HARDWARE_STREAM_POS
					#if GDISP_HARDWARE_STREAM_POS == HARDWARE_AUTODETECT
						if (gvmt(g)->writepos)
					#endif
					gdisp_lld_write_pos(g);
				#endif
				return;
			}
		#endif

		// Worst - save the parameters and use pixel drawing and/or area fills
		#if GDISP_HARDWARE_STREAM_WRITE != TRUE && GDISP_HARDWARE_DRAWPIXEL
			// The following test is unneeded because we are guaranteed to have draw pixel if we don't have streaming
			//#if GDISP_HARDWARE_DRAWPIXEL == HARDWARE_AUTODETECT
			//	if (gvmt(g)->pixel)
			//#endif
			{
				// Use x,y as the current position, x1,y1 as the save position and x2,y2 as the end position, cx = bufpos
				g->p.x1 = g->p.x = x;
				g->p.y1 = g->p.y = y;
				g->p.x2 = x + cx;
				g->p.y2 = y + cy;
				#if (GDISP_LINEBUF_SIZE != 0 && GDISP_HARDWARE_BITFILLS) || GDISP_HARDWARE_FILLS
					g->p.cx = 0;
					g->p.cy = 1;
				#endif
				return;
			}
		#endif

		// Don't release the mutex as gdispStreamEnd() will do that.
	}

	void gdispGStreamColor(GDisplay *g, color_t color) {
		#if !GDISP_HARDWARE_STREAM_WRITE && GDISP_LINEBUF_SIZE != 0 && GDISP_HARDWARE_BITFILLS
			coord_t	 sx1, sy1;
		#endif

		// Don't touch the mutex as we should already own it

		// Ignore this call if we are not streaming
		if (!(g->flags & GDISP_FLG_INSTREAM))
			return;

		// Best is hardware streaming
		#if GDISP_HARDWARE_STREAM_WRITE
			#if GDISP_HARDWARE_STREAM_WRITE == HARDWARE_AUTODETECT
				if (gvmt(g)->writestart)
			#endif
			{
				g->p.color = color;
				gdisp_lld_write_color(g);
				return;
			}
		#endif

		// Next best is to use bitfills with our line buffer
		#if GDISP_HARDWARE_STREAM_WRITE != TRUE && GDISP_LINEBUF_SIZE != 0 && GDISP_HARDWARE_BITFILLS
			#if GDISP_HARDWARE_BITFILLS == HARDWARE_AUTODETECT
				if (gvmt(g)->blit)
			#endif
			{
				g->linebuf[g->p.cx++] = color;
				if (g->p.cx >= GDISP_LINEBUF_SIZE) {
					sx1 = g->p.x1;
					sy1 = g->p.y1;
					g->p.x1 = 0;
					g->p.y1 = 0;
					g->p.ptr = (void *)g->linebuf;
					gdisp_lld_blit_area(g);
					g->p.x1 = sx1;
					g->p.y1 = sy1;
					g->p.x += g->p.cx;
					g->p.cx = 0;
				}

				// Just wrap at end-of-line and end-of-buffer
				if (g->p.x+g->p.cx >= g->p.x2) {
					if (g->p.cx) {
						sx1 = g->p.x1;
						sy1 = g->p.y1;
						g->p.x1 = 0;
						g->p.y1 = 0;
						g->p.ptr = (void *)g->linebuf;
						gdisp_lld_blit_area(g);
						g->p.x1 = sx1;
						g->p.y1 = sy1;
						g->p.cx = 0;
					}
					g->p.x = g->p.x1;
					if (++g->p.y >= g->p.y2)
						g->p.y = g->p.y1;
				}
			}
		#endif

		// Only slightly better than drawing pixels is to look for runs and use fillarea
		#if GDISP_HARDWARE_STREAM_WRITE != TRUE && (GDISP_LINEBUF_SIZE == 0 || GDISP_HARDWARE_BITFILLS != TRUE) && GDISP_HARDWARE_FILLS
			// We don't need to test for auto-detect on drawpixel as we know we have it because we don't have streaming.
			#if GDISP_HARDWARE_FILLS == HARDWARE_AUTODETECT
				if (gvmt(g)->fill)
			#endif
			{
				if (!g->p.cx || g->p.color == color) {
					g->p.cx++;
					g->p.color = color;
				} else {
					if (g->p.cx == 1)
						gdisp_lld_draw_pixel(g);
					else
						gdisp_lld_fill_area(g);
					g->p.x += g->p.cx;
					g->p.color = color;
					g->p.cx = 1;
				}
				// Just wrap at end-of-line and end-of-buffer
				if (g->p.x+g->p.cx >= g->p.x2) {
					if (g->p.cx) {
						if (g->p.cx == 1)
							gdisp_lld_draw_pixel(g);
						else
							gdisp_lld_fill_area(g);
						g->p.cx = 0;
					}
					g->p.x = g->p.x1;
					if (++g->p.y >= g->p.y2)
						g->p.y = g->p.y1;
				}
				return;
			}
		#endif

		// Worst is using pixel drawing
		#if GDISP_HARDWARE_STREAM_WRITE != TRUE && (GDISP_LINEBUF_SIZE == 0 || GDISP_HARDWARE_BITFILLS != TRUE) && GDISP_HARDWARE_FILLS != TRUE && GDISP_HARDWARE_DRAWPIXEL
			// The following test is unneeded because we are guaranteed to have draw pixel if we don't have streaming
			//#if GDISP_HARDWARE_DRAWPIXEL == HARDWARE_AUTODETECT
			//	if (gvmt(g)->pixel)
			//#endif
			{
				g->p.color = color;
				gdisp_lld_draw_pixel(g);

				// Just wrap at end-of-line and end-of-buffer
				if (++g->p.x >= g->p.x2) {
					g->p.x = g->p.x1;
					if (++g->p.y >= g->p.y2)
						g->p.y = g->p.y1;
				}
				return;
			}
		#endif
	}

	void gdispGStreamStop(GDisplay *g) {
		// Only release the mutex and end the stream if we are actually streaming.
		if (!(g->flags & GDISP_FLG_INSTREAM))
			return;

		// Clear the flag
		g->flags &= ~GDISP_FLG_INSTREAM;

		// The cleanup below must match the streaming code above.

		#if GDISP_HARDWARE_STREAM_WRITE
			#if GDISP_HARDWARE_STREAM_WRITE == HARDWARE_AUTODETECT
				if (gvmt(g)->writestart)
			#endif
			{
					gdisp_lld_write_stop(g);
					autoflush_stopdone(g);
					MUTEX_EXIT(g);
					return;
			}
		#endif

		#if GDISP_HARDWARE_STREAM_WRITE != TRUE && GDISP_LINEBUF_SIZE != 0 && GDISP_HARDWARE_BITFILLS
			#if GDISP_HARDWARE_BITFILLS == HARDWARE_AUTODETECT
				if (gvmt(g)->blit)
			#endif
			{
				if (g->p.cx) {
					g->p.x1 = 0;
					g->p.y1 = 0;
					g->p.ptr = (void *)g->linebuf;
					gdisp_lld_blit_area(g);
				}
				autoflush_stopdone(g);
				MUTEX_EXIT(g);
				return;
			}
		#endif

		#if GDISP_HARDWARE_STREAM_WRITE != TRUE && (GDISP_LINEBUF_SIZE == 0 || GDISP_HARDWARE_BITFILLS != TRUE) && GDISP_HARDWARE_FILLS
			// We don't need to test for auto-detect on drawpixel as we know we have it because we don't have streaming.
			#if GDISP_HARDWARE_FILLS == HARDWARE_AUTODETECT
				if (gvmt(g)->fill)
			#endif
			{
				if (g->p.cx) {
					if (g->p.cx == 1)
						gdisp_lld_draw_pixel(g);
					else
						gdisp_lld_fill_area(g);
				}
				autoflush_stopdone(g);
				MUTEX_EXIT(g);
				return;
			}
		#endif

		#if GDISP_HARDWARE_STREAM_WRITE != TRUE && (GDISP_LINEBUF_SIZE == 0 || GDISP_HARDWARE_BITFILLS != TRUE) && GDISP_HARDWARE_FILLS != TRUE
			{
				autoflush_stopdone(g);
				MUTEX_EXIT(g);
			}
		#endif
	}
#endif

void gdispGDrawPixel(GDisplay *g, coord_t x, coord_t y, color_t color) {
	MUTEX_ENTER(g);
	g->p.x		= x;
	g->p.y		= y;
	g->p.color	= color;
	drawpixel_clip(g);
	autoflush(g);
	MUTEX_EXIT(g);
}

void gdispGDrawLine(GDisplay *g, coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color) {
	MUTEX_ENTER(g);
	g->p.x = x0;
	g->p.y = y0;
	g->p.x1 = x1;
	g->p.y1 = y1;
	g->p.color = color;
	line_clip(g);
	autoflush(g);
	MUTEX_EXIT(g);
}

void gdispGClear(GDisplay *g, color_t color) {
	// Note - clear() ignores the clipping area. It clears the screen.
	MUTEX_ENTER(g);

	// Best is hardware accelerated clear
	#if GDISP_HARDWARE_CLEARS
		#if GDISP_HARDWARE_CLEARS == HARDWARE_AUTODETECT
			if (gvmt(g)->clear)
		#endif
		{
			g->p.color = color;
			gdisp_lld_clear(g);
			autoflush_stopdone(g);
			MUTEX_EXIT(g);
			return;
		}
	#endif

	// Next best is hardware accelerated area fill
	#if GDISP_HARDWARE_CLEARS != TRUE && GDISP_HARDWARE_FILLS
		#if GDISP_HARDWARE_FILLS == HARDWARE_AUTODETECT
			if (gvmt(g)->fill)
		#endif
		{
			g->p.x = g->p.y = 0;
			g->p.cx = g->g.Width;
			g->p.cy = g->g.Height;
			g->p.color = color;
			gdisp_lld_fill_area(g);
			autoflush_stopdone(g);
			MUTEX_EXIT(g);
			return;
		}
	#endif

	// Next best is streaming
	#if GDISP_HARDWARE_CLEARS != TRUE && GDISP_HARDWARE_FILLS != TRUE && GDISP_HARDWARE_STREAM_WRITE
		#if GDISP_HARDWARE_STREAM_WRITE == HARDWARE_AUTODETECT
			if (gvmt(g)->writestart)
		#endif
		{
			uint32_t	area;

			g->p.x = g->p.y = 0;
			g->p.cx = g->g.Width;
			g->p.cy = g->g.Height;
			g->p.color = color;
			area = (uint32_t)g->p.cx * g->p.cy;

			gdisp_lld_write_start(g);
			#if GDISP_HARDWARE_STREAM_POS
				#if GDISP_HARDWARE_STREAM_POS == HARDWARE_AUTODETECT
					if (gvmt(g)->writepos)
				#endif
				gdisp_lld_write_pos(g);
			#endif
			for(; area; area--)
				gdisp_lld_write_color(g);
			gdisp_lld_write_stop(g);
			autoflush_stopdone(g);
			MUTEX_EXIT(g);
			return;
		}
	#endif

	// Worst is drawing pixels
	#if GDISP_HARDWARE_CLEARS != TRUE && GDISP_HARDWARE_FILLS != TRUE && GDISP_HARDWARE_STREAM_WRITE != TRUE && GDISP_HARDWARE_DRAWPIXEL
		// The following test is unneeded because we are guaranteed to have draw pixel if we don't have streaming
		//#if GDISP_HARDWARE_DRAWPIXEL == HARDWARE_AUTODETECT
		//	if (gvmt(g)->pixel)
		//#endif
		{
			g->p.color = color;
			for(g->p.y = 0; g->p.y < g->g.Height; g->p.y++)
				for(g->p.x = 0; g->p.x < g->g.Width; g->p.x++)
					gdisp_lld_draw_pixel(g);
			autoflush_stopdone(g);
			MUTEX_EXIT(g);
			return;
		}
	#endif
}

void gdispGFillArea(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
	MUTEX_ENTER(g);
	g->p.x = x;
	g->p.y = y;
	g->p.cx = cx;
	g->p.cy = cy;
	g->p.color = color;
	TEST_CLIP_AREA(g) {
		fillarea(g);
	}
	autoflush_stopdone(g);
	MUTEX_EXIT(g);
}

void gdispGBlitArea(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
	MUTEX_ENTER(g);

	#if NEED_CLIPPING
		#if GDISP_HARDWARE_CLIP == HARDWARE_AUTODETECT
			if (!gvmt(g)->setclip)
		#endif
		{
			// This is a different clipping to fillarea(g) as it needs to take into account srcx,srcy
			if (x < g->clipx0) { cx -= g->clipx0 - x; srcx += g->clipx0 - x; x = g->clipx0; }
			if (y < g->clipy0) { cy -= g->clipy0 - y; srcy += g->clipy0 - x; y = g->clipy0; }
			if (x+cx > g->clipx1)	cx = g->clipx1 - x;
			if (y+cy > g->clipy1)	cy = g->clipy1 - y;
			if (srcx+cx > srccx) cx = srccx - srcx;
			if (cx <= 0 || cy <= 0) { MUTEX_EXIT(g); return; }
		}
	#endif

	// Best is hardware bitfills
	#if GDISP_HARDWARE_BITFILLS
		#if GDISP_HARDWARE_BITFILLS == HARDWARE_AUTODETECT
			if (gvmt(g)->blit)
		#endif
		{
			g->p.x = x;
			g->p.y = y;
			g->p.cx = cx;
			g->p.cy = cy;
			g->p.x1 = srcx;
			g->p.y1 = srcy;
			g->p.x2 = srccx;
			g->p.ptr = (void *)buffer;
			gdisp_lld_blit_area(g);
			autoflush_stopdone(g);
			MUTEX_EXIT(g);
			return;
		}
	#endif

	// Next best is hardware streaming
	#if GDISP_HARDWARE_BITFILLS != TRUE && GDISP_HARDWARE_STREAM_WRITE
		#if GDISP_HARDWARE_STREAM_WRITE == HARDWARE_AUTODETECT
			if (gvmt(g)->writestart)
		#endif
		{
			// Translate buffer to the real image data, use srcx,srcy as the end point, srccx as the buffer line gap
			buffer += srcy*srccx+srcx;
			srcx = x + cx;
			srcy = y + cy;
			srccx -= cx;

			g->p.x = x;
			g->p.y = y;
			g->p.cx = cx;
			g->p.cy = cy;
			gdisp_lld_write_start(g);
			#if GDISP_HARDWARE_STREAM_POS
				#if GDISP_HARDWARE_STREAM_POS == HARDWARE_AUTODETECT
					if (gvmt(g)->writepos)
				#endif
				gdisp_lld_write_pos(g);
			#endif
			for(g->p.y = y; g->p.y < srcy; g->p.y++, buffer += srccx) {
				for(g->p.x = x; g->p.x < srcx; g->p.x++) {
					g->p.color = *buffer++;
					gdisp_lld_write_color(g);
				}
			}
			gdisp_lld_write_stop(g);
			autoflush_stopdone(g);
			MUTEX_EXIT(g);
			return;
		}
	#endif

	// Only slightly better than drawing pixels is to look for runs and use fill area
	#if GDISP_HARDWARE_BITFILLS != TRUE && GDISP_HARDWARE_STREAM_WRITE != TRUE && GDISP_HARDWARE_FILLS
		// We don't need to test for auto-detect on drawpixel as we know we have it because we don't have streaming.
		#if GDISP_HARDWARE_FILLS == HARDWARE_AUTODETECT
			if (gvmt(g)->fill)
		#endif
		{
			// Translate buffer to the real image data, use srcx,srcy as the end point, srccx as the buffer line gap
			buffer += srcy*srccx+srcx;
			srcx = x + cx;
			srcy = y + cy;
			srccx -= cx;

			g->p.cy = 1;
			for(g->p.y = y; g->p.y < srcy; g->p.y++, buffer += srccx) {
				for(g->p.x=x; g->p.x < srcx; g->p.x += g->p.cx) {
					g->p.cx=1;
					g->p.color = *buffer++;
					while(g->p.x+g->p.cx < srcx && *buffer == g->p.color) {
						g->p.cx++;
						buffer++;
					}
					if (g->p.cx == 1) {
						gdisp_lld_draw_pixel(g);
					} else {
						gdisp_lld_fill_area(g);
					}
				}
			}
			autoflush_stopdone(g);
			MUTEX_EXIT(g);
			return;
		}
	#endif

	// Worst is drawing pixels
	#if GDISP_HARDWARE_BITFILLS != TRUE && GDISP_HARDWARE_STREAM_WRITE != TRUE && GDISP_HARDWARE_FILLS != TRUE && GDISP_HARDWARE_DRAWPIXEL
		// The following test is unneeded because we are guaranteed to have draw pixel if we don't have streaming
		//#if GDISP_HARDWARE_DRAWPIXEL == HARDWARE_AUTODETECT
		//	if (gvmt(g)->pixel)
		//#endif
		{
			// Translate buffer to the real image data, use srcx,srcy as the end point, srccx as the buffer line gap
			buffer += srcy*srccx+srcx;
			srcx = x + cx;
			srcy = y + cy;
			srccx -= cx;

			for(g->p.y = y; g->p.y < srcy; g->p.y++, buffer += srccx) {
				for(g->p.x=x; g->p.x < srcx; g->p.x++) {
					g->p.color = *buffer++;
					gdisp_lld_draw_pixel(g);
				}
			}
			autoflush_stopdone(g);
			MUTEX_EXIT(g);
			return;
		}
	#endif
}

#if GDISP_NEED_CLIP || GDISP_NEED_VALIDATION
	void gdispGSetClip(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy) {
		MUTEX_ENTER(g);

		// Best is using hardware clipping
		#if GDISP_HARDWARE_CLIP
			#if GDISP_HARDWARE_CLIP == HARDWARE_AUTODETECT
				if (gvmt(g)->setclip)
			#endif
			{
				g->p.x = x;
				g->p.y = y;
				g->p.cx = cx;
				g->p.cy = cy;
				gdisp_lld_set_clip(g);
			}
			#if GDISP_HARDWARE_CLIP == HARDWARE_AUTODETECT
				else
			#endif
		#endif

		// Worst is using software clipping
		#if GDISP_HARDWARE_CLIP != TRUE
			{
				if (x < 0) { cx += x; x = 0; }
				if (y < 0) { cy += y; y = 0; }
				if (cx <= 0 || cy <= 0 || x >= g->g.Width || y >= g->g.Height) { x = y = cx = cy = 0; }
				g->clipx0 = x;
				g->clipy0 = y;
				g->clipx1 = x+cx;	if (g->clipx1 > g->g.Width) g->clipx1 = g->g.Width;
				g->clipy1 = y+cy;	if (g->clipy1 > g->g.Height) g->clipy1 = g->g.Height;
			}
		#endif
		MUTEX_EXIT(g);
	}
#endif

#if GDISP_NEED_CIRCLE
	void gdispGDrawCircle(GDisplay *g, coord_t x, coord_t y, coord_t radius, color_t color) {
		coord_t a, b, P;

		MUTEX_ENTER(g);

		// Calculate intermediates
		a = 1;
		b = radius;
		P = 4 - radius;
		g->p.color = color;

		// Away we go using Bresenham's circle algorithm
		// Optimized to prevent double drawing
		g->p.x = x; g->p.y = y + b; drawpixel_clip(g);
		g->p.x = x; g->p.y = y - b; drawpixel_clip(g);
		g->p.x = x + b; g->p.y = y; drawpixel_clip(g);
		g->p.x = x - b; g->p.y = y; drawpixel_clip(g);
		do {
			g->p.x = x + a; g->p.y = y + b; drawpixel_clip(g);
			g->p.x = x + a; g->p.y = y - b; drawpixel_clip(g);
			g->p.x = x + b; g->p.y = y + a; drawpixel_clip(g);
			g->p.x = x - b; g->p.y = y + a; drawpixel_clip(g);
			g->p.x = x - a; g->p.y = y + b; drawpixel_clip(g);
			g->p.x = x - a; g->p.y = y - b; drawpixel_clip(g);
			g->p.x = x + b; g->p.y = y - a; drawpixel_clip(g);
			g->p.x = x - b; g->p.y = y - a; drawpixel_clip(g);
			if (P < 0)
				P += 3 + 2*a++;
			else
				P += 5 + 2*(a++ - b--);
		} while(a < b);
		g->p.x = x + a; g->p.y = y + b; drawpixel_clip(g);
		g->p.x = x + a; g->p.y = y - b; drawpixel_clip(g);
		g->p.x = x - a; g->p.y = y + b; drawpixel_clip(g);
		g->p.x = x - a; g->p.y = y - b; drawpixel_clip(g);

		autoflush(g);
		MUTEX_EXIT(g);
	}
#endif

#if GDISP_NEED_CIRCLE
	void gdispGFillCircle(GDisplay *g, coord_t x, coord_t y, coord_t radius, color_t color) {
		coord_t a, b, P;

		MUTEX_ENTER(g);

		// Calculate intermediates
		a = 1;
		b = radius;
		P = 4 - radius;
		g->p.color = color;

		// Away we go using Bresenham's circle algorithm
		// This is optimized to prevent overdrawing by drawing a line only when a variable is about to change value
		g->p.y = y; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);
		g->p.y = y+b; g->p.x = x; drawpixel_clip(g);
		g->p.y = y-b; g->p.x = x; drawpixel_clip(g);
		do {
			g->p.y = y+a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);
			g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);
			if (P < 0) {
				P += 3 + 2*a++;
			} else {
				g->p.y = y+b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g);
				g->p.y = y-b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g);
				P += 5 + 2*(a++ - b--);
			}
		} while(a < b);
		g->p.y = y+a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);
		g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);

		autoflush(g);
		MUTEX_EXIT(g);
	}
#endif

#if GDISP_NEED_ELLIPSE
	void gdispGDrawEllipse(GDisplay *g, coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		coord_t	dx, dy;
		int32_t	a2, b2;
		int32_t	err, e2;

		MUTEX_ENTER(g);

		// Calculate intermediates
		dx = 0;
		dy = b;
		a2 = a*a;
		b2 = b*b;
		err = b2-(2*b-1)*a2;
		g->p.color = color;

		// Away we go using Bresenham's ellipse algorithm
		do {
			g->p.x = x + dx; g->p.y = y + dy; drawpixel_clip(g);
			g->p.x = x - dx; g->p.y = y + dy; drawpixel_clip(g);
			g->p.x = x - dx; g->p.y = y - dy; drawpixel_clip(g);
			g->p.x = x + dx; g->p.y = y - dy; drawpixel_clip(g);

			e2 = 2*err;
			if(e2 <  (2*dx+1)*b2) {
				dx++;
				err += (2*dx+1)*b2;
			}
			if(e2 > -(2*dy-1)*a2) {
				dy--;
				err -= (2*dy-1)*a2;
			}
		} while(dy >= 0);

		autoflush(g);
		MUTEX_EXIT(g);
	}
#endif

#if GDISP_NEED_ELLIPSE
	void gdispGFillEllipse(GDisplay *g, coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		coord_t	dx, dy;
		int32_t	a2, b2;
		int32_t	err, e2;

		MUTEX_ENTER(g);

		// Calculate intermediates
		dx = 0;
		dy = b;
		a2 = a*a;
		b2 = b*b;
		err = b2-(2*b-1)*a2;
		g->p.color = color;

		// Away we go using Bresenham's ellipse algorithm
		// This is optimized to prevent overdrawing by drawing a line only when a y is about to change value
		do {
			e2 = 2*err;
			if(e2 <  (2*dx+1)*b2) {
				dx++;
				err += (2*dx+1)*b2;
			}
			if(e2 > -(2*dy-1)*a2) {
				g->p.y = y + dy; g->p.x = x - dx; g->p.x1 = x + dx; hline_clip(g);
				if (y) { g->p.y = y - dy; g->p.x = x - dx; g->p.x1 = x + dx; hline_clip(g); }
				dy--;
				err -= (2*dy-1)*a2;
			}
		} while(dy >= 0);

		autoflush(g);
		MUTEX_EXIT(g);
	}
#endif

#if GDISP_NEED_ARCSECTORS
	void gdispGDrawArcSectors(GDisplay *g, coord_t x, coord_t y, coord_t radius, uint8_t sectors, color_t color) {
		coord_t a, b, P;

		MUTEX_ENTER(g);

		// Calculate intermediates
		a = 1;              // x in many explanations
		b = radius;         // y in many explanations
		P = 4 - radius;
		g->p.color = color;

		// Away we go using Bresenham's circle algorithm
		// Optimized to prevent double drawing
		if (sectors & 0x06) { g->p.x = x; g->p.y = y - b; drawpixel_clip(g); }				// Upper upper
		if (sectors & 0x60) { g->p.x = x; g->p.y = y + b; drawpixel_clip(g); }				// Lower lower
		if (sectors & 0x81) { g->p.x = x + b; g->p.y = y; drawpixel_clip(g); }				// Right right
		if (sectors & 0x18) { g->p.x = x - b; g->p.y = y; drawpixel_clip(g); }				// Left left

		do {
			if (sectors & 0x01) { g->p.x = x + b; g->p.y = y - a; drawpixel_clip(g); }		// Upper right right
			if (sectors & 0x02) { g->p.x = x + a; g->p.y = y - b; drawpixel_clip(g); }		// Upper upper right
			if (sectors & 0x04) { g->p.x = x - a; g->p.y = y - b; drawpixel_clip(g); }		// Upper upper left
			if (sectors & 0x08) { g->p.x = x - b; g->p.y = y - a; drawpixel_clip(g); }		// Upper left  left
			if (sectors & 0x10) { g->p.x = x - b; g->p.y = y + a; drawpixel_clip(g); }		// Lower left  left
			if (sectors & 0x20) { g->p.x = x - a; g->p.y = y + b; drawpixel_clip(g); }		// Lower lower left
			if (sectors & 0x40) { g->p.x = x + a; g->p.y = y + b; drawpixel_clip(g); }		// Lower lower right
			if (sectors & 0x80) { g->p.x = x + b; g->p.y = y + a; drawpixel_clip(g); }		// Lower right right
			if (P < 0)
				P += 3 + 2*a++;
			else
				P += 5 + 2*(a++ - b--);
		} while(a < b);

		if (sectors & 0xC0) { g->p.x = x + a; g->p.y = y + b; drawpixel_clip(g); }			// Lower right
		if (sectors & 0x03) { g->p.x = x + a; g->p.y = y - b; drawpixel_clip(g); }			// Upper right
		if (sectors & 0x30) { g->p.x = x - a; g->p.y = y + b; drawpixel_clip(g); }			// Lower left
		if (sectors & 0x0C) { g->p.x = x - a; g->p.y = y - b; drawpixel_clip(g); }			// Upper left

		autoflush(g);
		MUTEX_EXIT(g);
	}
#endif

#if GDISP_NEED_ARCSECTORS
	void gdispGFillArcSectors(GDisplay *g, coord_t x, coord_t y, coord_t radius, uint8_t sectors, color_t color) {
		coord_t a, b, P;

		MUTEX_ENTER(g);

		// Calculate intermediates
		a = 1;              // x in many explanations
		b = radius;         // y in many explanations
		P = 4 - radius;
		g->p.color = color;

		// Away we go using Bresenham's circle algorithm
		// Optimized to prevent double drawing
		if (sectors & 0x06) { g->p.x = x; g->p.y = y - b; drawpixel_clip(g); }					// Upper upper
		if (sectors & 0x60) { g->p.x = x; g->p.y = y + b; drawpixel_clip(g); }					// Lower lower
		if (sectors & 0x81) {																	// Center right
			g->p.y = y; g->p.x = x; g->p.x1 = x + b;
			if (sectors & 0x18) g->p.x -= b;													// Left right
			hline_clip(g);
		} else if (sectors & 0x18) {															// Left center
			g->p.x = x - b; g->p.x1 = x; g->p.y = y;
			hline_clip(g);
		}

		do {
			// Top half
			switch(sectors & 0x0F) {
			case 0x01:
				g->p.y = y - a; g->p.x = x + a; g->p.x1 = x + b; hline_clip(g);
				break;
			case 0x02:
				g->p.y = y - b; g->p.x = x; g->p.x1 = x + a; hline_clip(g);
				g->p.y = y - a; g->p.x = x; g->p.x1 = x + a; hline_clip(g);
				break;
			case 0x03:
				g->p.y = y - b; g->p.x = x; g->p.x1 = x + a; hline_clip(g);
				g->p.y = y - a; g->p.x = x; g->p.x1 = x + b; hline_clip(g);
				break;
			case 0x04:
				g->p.y = y - b; g->p.x = x - a; g->p.x1 = x; hline_clip(g);
				g->p.y = y - a; g->p.x = x - a; g->p.x1 = x; hline_clip(g);
				break;
			case 0x05:
				g->p.y = y - b; g->p.x = x - a; g->p.x1 = x; hline_clip(g);
				g->p.y = y - a; g->p.x = x - a; g->p.x1 = x; hline_clip(g);
				g->p.y = y - a; g->p.x = x + a; g->p.x1 = x + b; hline_clip(g);
				break;
			case 0x06:
				g->p.y = y - b; g->p.x = x - a; g->p.x1 = x + a; hline_clip(g);
				g->p.y = y - a; g->p.x = x - a; g->p.x1 = x + a; hline_clip(g);
				break;
			case 0x07:
				g->p.y = y - b; g->p.x = x - a; g->p.x1 = x + a; hline_clip(g);
				g->p.y = y - a; g->p.x = x - a; g->p.x1 = x + b; hline_clip(g);
				break;
			case 0x08:
				g->p.y = y - a; g->p.x = x - b; g->p.x1 = x - a; hline_clip(g);
				break;
			case 0x09:
				g->p.y = y - a; g->p.x = x - b; g->p.x1 = x - a; hline_clip(g);
				g->p.y = y - a; g->p.x = x + a; g->p.x1 = x + b; hline_clip(g);
				break;
			case 0x0A:
				g->p.y = y - b; g->p.x = x; g->p.x1 = x + a; hline_clip(g);
				g->p.y = y - a; g->p.x = x - b; g->p.x1 = x - a; hline_clip(g);
				g->p.y = y - a; g->p.x = x; g->p.x1 = x + a; hline_clip(g);
				break;
			case 0x0B:
				g->p.y = y - b; g->p.x = x; g->p.x1 = x + a; hline_clip(g);
				g->p.y = y - a; g->p.x = x - b; g->p.x1 = x - a; hline_clip(g);
				g->p.y = y - a; g->p.x = x; g->p.x1 = x + b; hline_clip(g);
				break;
			case 0x0C:
				g->p.y = y - b; g->p.x = x - a; g->p.x1 = x; hline_clip(g);
				g->p.y = y - a; g->p.x = x - b; g->p.x1 = x; hline_clip(g);
				break;
			case 0x0D:
				g->p.y = y - b; g->p.x = x - a; g->p.x1 = x; hline_clip(g);
				g->p.y = y - a; g->p.x = x - b; g->p.x1 = x; hline_clip(g);
				g->p.y = y - a; g->p.x = x + a; g->p.x1 = x + b; hline_clip(g);
				break;
			case 0x0E:
				g->p.y = y - b; g->p.x = x - a; g->p.x1 = x + a; hline_clip(g);
				g->p.y = y - a; g->p.x = x - b; g->p.x1 = x + a; hline_clip(g);
				break;
			case 0x0F:
				g->p.y = y - b; g->p.x = x - a; g->p.x1 = x + a; hline_clip(g);
				g->p.y = y - a; g->p.x = x - b; g->p.x1 = x + b; hline_clip(g);
				break;
			}

			// Bottom half
			switch((sectors & 0xF0)>>4) {
			case 0x01:
				g->p.y = y + a; g->p.x = x - b; g->p.x1 = x - a; hline_clip(g);
				break;
			case 0x02:
				g->p.y = y + b; g->p.x = x - a; g->p.x1 = x; hline_clip(g);
				g->p.y = y + a; g->p.x = x - a; g->p.x1 = x; hline_clip(g);
				break;
			case 0x03:
				g->p.y = y + b; g->p.x = x - a; g->p.x1 = x; hline_clip(g);
				g->p.y = y + a; g->p.x = x - b; g->p.x1 = x; hline_clip(g);
				break;
			case 0x04:
				g->p.y = y + b; g->p.x = x; g->p.x1 = x + a; hline_clip(g);
				g->p.y = y + a; g->p.x = x; g->p.x1 = x + a; hline_clip(g);
				break;
			case 0x05:
				g->p.y = y + b; g->p.x = x; g->p.x1 = x + a; hline_clip(g);
				g->p.y = y + a; g->p.x = x - b; g->p.x1 = x - a; hline_clip(g);
				g->p.y = y + a; g->p.x = x; g->p.x1 = x + a; hline_clip(g);
				break;
			case 0x06:
				g->p.y = y + b; g->p.x = x - a; g->p.x1 = x + a; hline_clip(g);
				g->p.y = y + a; g->p.x = x - a; g->p.x1 = x + a; hline_clip(g);
				break;
			case 0x07:
				g->p.y = y + b; g->p.x = x - a; g->p.x1 = x + a; hline_clip(g);
				g->p.y = y + a; g->p.x = x - b; g->p.x1 = x + a; hline_clip(g);
				break;
			case 0x08:
				g->p.y = y + a; g->p.x = x + a; g->p.x1 = x + b; hline_clip(g);
				break;
			case 0x09:
				g->p.y = y + a; g->p.x = x - b; g->p.x1 = x - a; hline_clip(g);
				g->p.y = y + a; g->p.x = x + a; g->p.x1 = x + b; hline_clip(g);
				break;
			case 0x0A:
				g->p.y = y + b; g->p.x = x - a; g->p.x1 = x; hline_clip(g);
				g->p.y = y + a; g->p.x = x - a; g->p.x1 = x; hline_clip(g);
				g->p.y = y + a; g->p.x = x + a; g->p.x1 = x + b; hline_clip(g);
				break;
			case 0x0B:
				g->p.y = y + b; g->p.x = x - a; g->p.x1 = x; hline_clip(g);
				g->p.y = y + a; g->p.x = x - b; g->p.x1 = x; hline_clip(g);
				g->p.y = y + a; g->p.x = x + a; g->p.x1 = x + b; hline_clip(g);
				break;
			case 0x0C:
				g->p.y = y + b; g->p.x = x; g->p.x1 = x + a; hline_clip(g);
				g->p.y = y + a; g->p.x = x; g->p.x1 = x + b; hline_clip(g);
				break;
			case 0x0D:
				g->p.y = y + b; g->p.x = x; g->p.x1 = x + a; hline_clip(g);
				g->p.y = y + a; g->p.x = x - b; g->p.x1 = x - a; hline_clip(g);
				g->p.y = y + a; g->p.x = x; g->p.x1 = x + b; hline_clip(g);
				break;
			case 0x0E:
				g->p.y = y + b; g->p.x = x - a; g->p.x1 = x + a; hline_clip(g);
				g->p.y = y + a; g->p.x = x - a; g->p.x1 = x + b; hline_clip(g);
				break;
			case 0x0F:
				g->p.y = y + b; g->p.x = x - a; g->p.x1 = x + a; hline_clip(g);
				g->p.y = y + a; g->p.x = x - b; g->p.x1 = x + b; hline_clip(g);
				break;
			}

			if (P < 0)
				P += 3 + 2*a++;
			else
				P += 5 + 2*(a++ - b--);
		} while(a < b);

		// Top half
		if (sectors & 0x02)			{ g->p.y = y - a; g->p.x = x; g->p.x1 = x + a; hline_clip(g); }
		else if (sectors & 0x01)	{ g->p.y = y - a; g->p.x = x + a; drawpixel_clip(g); }
		if (sectors & 0x04)			{ g->p.y = y - a; g->p.x = x - a; g->p.x1 = x; hline_clip(g); }
		else if (sectors & 0x08)	{ g->p.y = y - a; g->p.x = x - a; drawpixel_clip(g); }

		// Bottom half
		if (sectors & 0x40)			{ g->p.y = y + a; g->p.x = x; g->p.x1 = x + a; hline_clip(g); }
		else if (sectors & 0x80)	{ g->p.y = y + a; g->p.x = x + a; drawpixel_clip(g); }
		if (sectors & 0x20)			{ g->p.y = y + a; g->p.x = x - a; g->p.x1 = x; hline_clip(g); }
		else if (sectors & 0x10)	{ g->p.y = y + a; g->p.x = x - a; drawpixel_clip(g); }

		autoflush(g);
		MUTEX_EXIT(g);
	}
#endif

#if GDISP_NEED_ARC
	#if (!GMISC_NEED_FIXEDTRIG && !GMISC_NEED_FASTTRIG) || !GFX_USE_GMISC
		#include <math.h>
	#endif

	void gdispGDrawArc(GDisplay *g, coord_t x, coord_t y, coord_t radius, coord_t start, coord_t end, color_t color) {
		coord_t a, b, P, sedge, eedge;
		uint8_t	full, sbit, ebit, tbit;

		// Normalize the angles
		if (start < 0)
			start -= (start/360-1)*360;
		else if (start >= 360)
			start %= 360;
		if (end < 0)
			end -= (end/360-1)*360;
		else if (end >= 360)
			end %= 360;

		sbit = 1<<(start/45);
		ebit = 1<<(end/45);
		full = 0;
		if (start == end) {
			full = 0xFF;
		} else if (end < start) {
			for(tbit=sbit<<1; tbit; tbit<<=1) full |= tbit;
			for(tbit=ebit>>1; tbit; tbit>>=1) full |= tbit;
		} else if (sbit < 0x80) {
			for(tbit=sbit<<1; tbit < ebit; tbit<<=1) full |= tbit;
		}
		tbit = start%45 == 0 ? sbit : 0;

		MUTEX_ENTER(g);
		g->p.color = color;

		if (full) {
			// Draw full sectors
			// Optimized to prevent double drawing
			a = 1;
			b = radius;
			P = 4 - radius;
			if (full & 0x60) { g->p.y = y+b; g->p.x = x; drawpixel_clip(g); }
			if (full & 0x06) { g->p.y = y-b; g->p.x = x; drawpixel_clip(g); }
			if (full & 0x81) { g->p.y = y; g->p.x = x+b; drawpixel_clip(g); }
			if (full & 0x18) { g->p.y = y; g->p.x = x-b; drawpixel_clip(g); }
			do {
				if (full & 0x01) { g->p.x = x+b; g->p.y = y-a; drawpixel_clip(g); }
				if (full & 0x02) { g->p.x = x+a; g->p.y = y-b; drawpixel_clip(g); }
				if (full & 0x04) { g->p.x = x-a; g->p.y = y-b; drawpixel_clip(g); }
				if (full & 0x08) { g->p.x = x-b; g->p.y = y-a; drawpixel_clip(g); }
				if (full & 0x10) { g->p.x = x-b; g->p.y = y+a; drawpixel_clip(g); }
				if (full & 0x20) { g->p.x = x-a; g->p.y = y+b; drawpixel_clip(g); }
				if (full & 0x40) { g->p.x = x+a; g->p.y = y+b; drawpixel_clip(g); }
				if (full & 0x80) { g->p.x = x+b; g->p.y = y+a; drawpixel_clip(g); }
				if (P < 0)
					P += 3 + 2*a++;
				else
					P += 5 + 2*(a++ - b--);
			} while(a < b);
			if (full & 0xC0) { g->p.x = x+a; g->p.y = y+b; drawpixel_clip(g); }
			if (full & 0x0C) { g->p.x = x-a; g->p.y = y-b; drawpixel_clip(g); }
			if (full & 0x03) { g->p.x = x+a; g->p.y = y-b; drawpixel_clip(g); }
			if (full & 0x30) { g->p.x = x-a; g->p.y = y+b; drawpixel_clip(g); }
			if (full == 0xFF) {
				autoflush(g);
				MUTEX_EXIT(g);
				return;
			}
		}

		#if GFX_USE_GMISC && GMISC_NEED_FIXEDTRIG
			sedge = NONFIXED(radius * ((sbit & 0x99) ? ffsin(start) : ffcos(start)) + FIXED0_5);
			eedge = NONFIXED(radius * ((ebit & 0x99) ? ffsin(end) : ffcos(end)) + FIXED0_5);
		#elif GFX_USE_GMISC && GMISC_NEED_FASTTRIG
			sedge = round(radius * ((sbit & 0x99) ? fsin(start) : fcos(start)));
			eedge = round(radius * ((ebit & 0x99) ? fsin(end) : fcos(end)));
		#else
			sedge = round(radius * ((sbit & 0x99) ? sin(start*GFX_PI/180) : cos(start*GFX_PI/180)));
			eedge = round(radius * ((ebit & 0x99) ? sin(end*GFX_PI/180) : cos(end*GFX_PI/180)));
		#endif
		if (sbit & 0xB4) sedge = -sedge;
		if (ebit & 0xB4) eedge = -eedge;

		if (sbit != ebit) {
			// Draw start and end sectors
			// Optimized to prevent double drawing
			a = 1;
			b = radius;
			P = 4 - radius;
			if ((sbit & 0x20) || (tbit & 0x40) || (ebit & 0x40)) { g->p.x = x; g->p.y = y+b; drawpixel_clip(g); }
			if ((sbit & 0x02) || (tbit & 0x04) || (ebit & 0x04)) { g->p.x = x; g->p.y = y-b; drawpixel_clip(g); }
			if ((sbit & 0x80) || (tbit & 0x01) || (ebit & 0x01)) { g->p.x = x+b; g->p.y = y; drawpixel_clip(g); }
			if ((sbit & 0x08) || (tbit & 0x10) || (ebit & 0x10)) { g->p.x = x-b; g->p.y = y; drawpixel_clip(g); }
			do {
				if (((sbit & 0x01) && a >= sedge) || ((ebit & 0x01) && a <= eedge)) { g->p.x = x+b; g->p.y = y-a; drawpixel_clip(g); }
				if (((sbit & 0x02) && a <= sedge) || ((ebit & 0x02) && a >= eedge)) { g->p.x = x+a; g->p.y = y-b; drawpixel_clip(g); }
				if (((sbit & 0x04) && a >= sedge) || ((ebit & 0x04) && a <= eedge)) { g->p.x = x-a; g->p.y = y-b; drawpixel_clip(g); }
				if (((sbit & 0x08) && a <= sedge) || ((ebit & 0x08) && a >= eedge)) { g->p.x = x-b; g->p.y = y-a; drawpixel_clip(g); }
				if (((sbit & 0x10) && a >= sedge) || ((ebit & 0x10) && a <= eedge)) { g->p.x = x-b; g->p.y = y+a; drawpixel_clip(g); }
				if (((sbit & 0x20) && a <= sedge) || ((ebit & 0x20) && a >= eedge)) { g->p.x = x-a; g->p.y = y+b; drawpixel_clip(g); }
				if (((sbit & 0x40) && a >= sedge) || ((ebit & 0x40) && a <= eedge)) { g->p.x = x+a; g->p.y = y+b; drawpixel_clip(g); }
				if (((sbit & 0x80) && a <= sedge) || ((ebit & 0x80) && a >= eedge)) { g->p.x = x+b; g->p.y = y+a; drawpixel_clip(g); }
				if (P < 0)
					P += 3 + 2*a++;
				else
					P += 5 + 2*(a++ - b--);
			} while(a < b);
			if (((sbit & 0x40) && a >= sedge) || ((ebit & 0x40) && a <= eedge) || ((sbit & 0x80) && a <= sedge) || ((ebit & 0x80) && a >= eedge))
				{ g->p.x = x+a; g->p.y = y+b; drawpixel_clip(g); }
			if (((sbit & 0x04) && a >= sedge) || ((ebit & 0x04) && a <= eedge) || ((sbit & 0x08) && a <= sedge) || ((ebit & 0x08) && a >= eedge))
				{ g->p.x = x-a; g->p.y = y-b; drawpixel_clip(g); }
			if (((sbit & 0x01) && a >= sedge) || ((ebit & 0x01) && a <= eedge) || ((sbit & 0x02) && a <= sedge) || ((ebit & 0x02) && a >= eedge))
				{ g->p.x = x+a; g->p.y = y-b; drawpixel_clip(g); }
			if (((sbit & 0x10) && a >= sedge) || ((ebit & 0x10) && a <= eedge) || ((sbit & 0x20) && a <= sedge) || ((ebit & 0x20) && a >= eedge))
				{ g->p.x = x-a; g->p.y = y+b; drawpixel_clip(g); }
		} else if (end < start) {
			// Draw start/end sector where it is a non-internal angle
			// Optimized to prevent double drawing
			a = 1;
			b = radius;
			P = 4 - radius;
			if ((sbit & 0x60) || (tbit & 0xC0)) { g->p.x = x; g->p.y = y+b; drawpixel_clip(g); }
			if ((sbit & 0x06) || (tbit & 0x0C)) { g->p.x = x; g->p.y = y-b; drawpixel_clip(g); }
			if ((sbit & 0x81) || (tbit & 0x03)) { g->p.x = x+b; g->p.y = y; drawpixel_clip(g); }
			if ((sbit & 0x18) || (tbit & 0x30)) { g->p.x = x-b; g->p.y = y; drawpixel_clip(g); }
			do {
				if ((sbit & 0x01) && (a >= sedge || a <= eedge)) { g->p.x = x+b; g->p.y = y-a; drawpixel_clip(g); }
				if ((sbit & 0x02) && (a <= sedge || a >= eedge)) { g->p.x = x+a; g->p.y = y-b; drawpixel_clip(g); }
				if ((sbit & 0x04) && (a >= sedge || a <= eedge)) { g->p.x = x-a; g->p.y = y-b; drawpixel_clip(g); }
				if ((sbit & 0x08) && (a <= sedge || a >= eedge)) { g->p.x = x-b; g->p.y = y-a; drawpixel_clip(g); }
				if ((sbit & 0x10) && (a >= sedge || a <= eedge)) { g->p.x = x-b; g->p.y = y+a; drawpixel_clip(g); }
				if ((sbit & 0x20) && (a <= sedge || a >= eedge)) { g->p.x = x-a; g->p.y = y+b; drawpixel_clip(g); }
				if ((sbit & 0x40) && (a >= sedge || a <= eedge)) { g->p.x = x+a; g->p.y = y+b; drawpixel_clip(g); }
				if ((sbit & 0x80) && (a <= sedge || a >= eedge)) { g->p.x = x+b; g->p.y = y+a; drawpixel_clip(g); }
				if (P < 0)
					P += 3 + 2*a++;
				else
					P += 5 + 2*(a++ - b--);
			} while(a < b);
			if (((sbit & 0x04) && (a >= sedge || a <= eedge)) || ((sbit & 0x08) && (a <= sedge || a >= eedge)))
				{ g->p.x = x-a; g->p.y = y-b; drawpixel_clip(g); }
			if (((sbit & 0x40) && (a >= sedge || a <= eedge)) || ((sbit & 0x80) && (a <= sedge || a >= eedge)))
				{ g->p.x = x+a; g->p.y = y+b; drawpixel_clip(g); }
			if (((sbit & 0x01) && (a >= sedge || a <= eedge)) || ((sbit & 0x02) && (a <= sedge || a >= eedge)))
				{ g->p.x = x+a; g->p.y = y-b; drawpixel_clip(g); }
			if (((sbit & 0x10) && (a >= sedge || a <= eedge)) || ((sbit & 0x20) && (a <= sedge || a >= eedge)))
				{ g->p.x = x-a; g->p.y = y+b; drawpixel_clip(g); }
		} else {
			// Draw start/end sector where it is a internal angle
			// Optimized to prevent double drawing
			a = 1;
			b = radius;
			P = 4 - radius;
			if (((sbit & 0x20) && !eedge) || ((sbit & 0x40) && !sedge)) { g->p.x = x; g->p.y = y+b; drawpixel_clip(g); }
			if (((sbit & 0x02) && !eedge) || ((sbit & 0x04) && !sedge)) { g->p.x = x; g->p.y = y-b; drawpixel_clip(g); }
			if (((sbit & 0x80) && !eedge) || ((sbit & 0x01) && !sedge)) { g->p.x = x+b; g->p.y = y; drawpixel_clip(g); }
			if (((sbit & 0x08) && !eedge) || ((sbit & 0x10) && !sedge)) { g->p.x = x-b; g->p.y = y; drawpixel_clip(g); }
			do {
				if (((sbit & 0x01) && a >= sedge && a <= eedge)) { g->p.x = x+b; g->p.y = y-a; drawpixel_clip(g); }
				if (((sbit & 0x02) && a <= sedge && a >= eedge)) { g->p.x = x+a; g->p.y = y-b; drawpixel_clip(g); }
				if (((sbit & 0x04) && a >= sedge && a <= eedge)) { g->p.x = x-a; g->p.y = y-b; drawpixel_clip(g); }
				if (((sbit & 0x08) && a <= sedge && a >= eedge)) { g->p.x = x-b; g->p.y = y-a; drawpixel_clip(g); }
				if (((sbit & 0x10) && a >= sedge && a <= eedge)) { g->p.x = x-b; g->p.y = y+a; drawpixel_clip(g); }
				if (((sbit & 0x20) && a <= sedge && a >= eedge)) { g->p.x = x-a; g->p.y = y+b; drawpixel_clip(g); }
				if (((sbit & 0x40) && a >= sedge && a <= eedge)) { g->p.x = x+a; g->p.y = y+b; drawpixel_clip(g); }
				if (((sbit & 0x80) && a <= sedge && a >= eedge)) { g->p.x = x+b; g->p.y = y+a; drawpixel_clip(g); }
				if (P < 0)
					P += 3 + 2*a++;
				else
					P += 5 + 2*(a++ - b--);
			} while(a < b);
			if (((sbit & 0x04) && a >= sedge && a <= eedge) || ((sbit & 0x08) && a <= sedge && a >= eedge))
				{ g->p.x = x-a; g->p.y = y-b; drawpixel_clip(g); }
			if (((sbit & 0x40) && a >= sedge && a <= eedge) || ((sbit & 0x80) && a <= sedge && a >= eedge))
				{ g->p.x = x+a; g->p.y = y+b; drawpixel_clip(g); }
			if (((sbit & 0x01) && a >= sedge && a <= eedge) || ((sbit & 0x02) && a <= sedge && a >= eedge))
				{ g->p.x = x+a; g->p.y = y-b; drawpixel_clip(g); }
			if (((sbit & 0x10) && a >= sedge && a <= eedge) || ((sbit & 0x20) && a <= sedge && a >= eedge))
				{ g->p.x = x-a; g->p.y = y+b; drawpixel_clip(g); }
		}

		autoflush(g);
		MUTEX_EXIT(g);
	}
#endif

#if GDISP_NEED_ARC
	void gdispGFillArc(GDisplay *g, coord_t x, coord_t y, coord_t radius, coord_t start, coord_t end, color_t color) {
		coord_t a, b, P;
		coord_t	sy, ey;
		fixed	sxa, sxb, sxd, exa, exb, exd;
		uint8_t	qtr;

		MUTEX_ENTER(g);

		// Do the trig to get the formulas for the start and end lines.
		sxa = exa = FIXED(x)+FIXED0_5;
		#if GFX_USE_GMISC && GMISC_NEED_FIXEDTRIG
			sxb = radius*ffcos(start);	sy = -NONFIXED(radius*ffsin(start) + FIXED0_5);
			exb = radius*ffcos(end);	ey = -NONFIXED(radius*ffsin(end) + FIXED0_5);
		#elif GFX_USE_GMISC && GMISC_NEED_FASTTRIG
			sxb = FP2FIXED(radius*fcos(start));	sy = -round(radius*fsin(start));
			exb = FP2FIXED(radius*fcos(end));	ey = -round(radius*fsin(end));
		#else
			sxb = FP2FIXED(radius*cos(start*GFX_PI/180));	sy = -round(radius*sin(start*GFX_PI/180));
			exb = FP2FIXED(radius*cos(end*GFX_PI/180));	ey = -round(radius*sin(end*GFX_PI/180));
		#endif
		sxd = sy ? sxb/sy : sxb;
		exd = ey ? exb/ey : exb;

		// Calculate which quarters and which direction we are traveling
		qtr = 0;
		if (sxb > 0)	qtr |= 0x01;		// S1=0001(1), S2=0000(0), S3=0010(2), S4=0011(3)
		if (sy > 0) 	qtr |= 0x02;
		if (exb > 0)	qtr |= 0x04;		// E1=0100(4), E2=0000(0), E3=1000(8), E4=1100(12)
		if (ey > 0) 	qtr |= 0x08;
		if (sy > ey)	qtr |= 0x10;		// order of start and end lines

		// Calculate intermediates
		a = 1;
		b = radius;
		P = 4 - radius;
		g->p.color = color;
		sxb += sxa;
		exb += exa;

		// Away we go using Bresenham's circle algorithm
		// This is optimized to prevent overdrawing by drawing a line only when a variable is about to change value

		switch(qtr) {
		case 0:		// S2E2 sy <= ey
		case 1:		// S1E2 sy <= ey
			if (ey && sy) {
				g->p.x = x; g->p.x1 = x;									// E2S
				sxa -= sxd; exa -= exd;
			} else if (sy) {
				g->p.x = x-b; g->p.x1 = x;								// C2S
				sxa -= sxd;
			} else if (ey) {
				g->p.x = x; g->p.x1 = x+b;								// E2C
				exa -= exd;
			} else {
				g->p.x = x-b; g->p.x1 = x+b;								// C2C
			}
			g->p.y = y;
			hline_clip(g);
			do {
				if (-a >= ey) {
					g->p.y = y-a; g->p.x = NONFIXED(exa); g->p.x1 = NONFIXED(sxa); hline_clip(g);		// E2S
					sxa -= sxd; exa -= exd;
				} else if (-a >= sy) {
					g->p.y = y-a; g->p.x = x-b; g->p.x1 = NONFIXED(sxa); hline_clip(g);				// C2S
					sxa -= sxd;
				} else if (qtr & 1) {
					g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);							// C2C
				}
				if (P < 0) {
					P += 3 + 2*a++;
				} else {
					if (-b >= ey) {
						g->p.y = y-b; g->p.x = NONFIXED(exb); g->p.x1 = NONFIXED(sxb); hline_clip(g);	// E2S
						sxb += sxd; exb += exd;
					} else if (-b >= sy) {
						g->p.y = y-b; g->p.x = x-a; g->p.x1 = NONFIXED(sxb); hline_clip(g);			// C2S
						sxb += sxd;
					} else if (qtr & 1) {
						g->p.y = y-b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g);						// C2C
					}
					P += 5 + 2*(a++ - b--);
				}
			} while(a < b);
			if (-a >= ey) {
				g->p.y = y-a; g->p.x = NONFIXED(exa); g->p.x1 = NONFIXED(sxa); hline_clip(g);			// E2S
			} else if (-a >= sy) {
				g->p.y = y-a; g->p.x = x-b; g->p.x1 = NONFIXED(sxa); hline_clip(g);					// C2S
			} else if (qtr & 1) {
				g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);								// C2C
			}
			break;

		case 2:		// S3E2 sy <= ey
		case 3:		// S4E2 sy <= ey
		case 6:		// S3E1 sy <= ey
		case 7:		// S4E1 sy <= ey
		case 18:	// S3E2 sy > ey
		case 19:	// S4E2 sy > ey
		case 22:	// S3E1 sy > ey
		case 23:	// S4E1 sy > ey
			g->p.y = y; g->p.x = x; g->p.x1 = x+b; hline_clip(g);								// SE2C
			sxa += sxd; exa -= exd;
			do {
				if (-a >= ey) {
					g->p.y = y-a; g->p.x = NONFIXED(exa); g->p.x1 = x+b; hline_clip(g);		// E2C
					exa -= exd;
				} else if (!(qtr & 4)) {
					g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);					// C2C
				}
				if (a <= sy) {
					g->p.y = y+a; g->p.x = NONFIXED(sxa); g->p.x1 = x+b; hline_clip(g);		// S2C
					sxa += sxd;
				} else if (!(qtr & 1)) {
					g->p.y = y+a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);					// C2C
				}
				if (P < 0) {
					P += 3 + 2*a++;
				} else {
					if (-b >= ey) {
						g->p.y = y-b; g->p.x = NONFIXED(exb); g->p.x1 = x+a; hline_clip(g);		// E2C
						exb += exd;
					} else if (!(qtr & 4)) {
						g->p.y = y-b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g);					// C2C
					}
					if (b <= sy) {
						g->p.y = y+b; g->p.x = NONFIXED(sxb); g->p.x1 = x+a; hline_clip(g);		// S2C
						sxb -= sxd;
					} else if (!(qtr & 1)) {
						g->p.y = y+b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g); 				// C2C
					}
					P += 5 + 2*(a++ - b--);
				}
			} while(a < b);
			if (-a >= ey) {
				g->p.y = y-a; g->p.x = NONFIXED(exa); g->p.x1 = x+b; hline_clip(g);				// E2C
			} else if (!(qtr & 4)) {
				g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);							// C2C
			}
			if (a <= sy) {
				g->p.y = y+a; g->p.x = NONFIXED(sxa); g->p.x1 = x+a; hline_clip(g);				// S2C
			} else if (!(qtr & 1)) {
				g->p.y = y+a; g->p.x = x-b; g->p.x1 = x+a; hline_clip(g);							// C2C
			}
			break;

		case 4:		// S2E1 sy <= ey
		case 5:		// S1E1 sy <= ey
			g->p.y = y; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);									// C2C
			do {
				if (-a >= ey) {
					g->p.y = y-a; g->p.x = x-b; g->p.x1 = NONFIXED(sxa); hline_clip(g);				// C2S
					g->p.y = y-a; g->p.x = NONFIXED(exa); g->p.x1 = x+b; hline_clip(g);				// E2C
					sxa -= sxd; exa -= exd;
				} else if (-a >= sy) {
					g->p.y = y-a; g->p.x = x-b; g->p.x1 = NONFIXED(sxa); hline_clip(g);				// C2S
					sxa -= sxd;
				} else if (qtr & 1) {
					g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);							// C2C
				}
				g->p.y = y+a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);								// C2C
				if (P < 0) {
					P += 3 + 2*a++;
				} else {
					if (-b >= ey) {
						g->p.y = y-b; g->p.x = x-a; g->p.x1 = NONFIXED(sxb); hline_clip(g);			// C2S
						g->p.y = y-b; g->p.x = NONFIXED(exb); g->p.x1 = x+a; hline_clip(g);			// E2C
						sxb += sxd; exb += exd;
					} else if (-b >= sy) {
						g->p.y = y-b; g->p.x = x-a; g->p.x1 = NONFIXED(sxb); hline_clip(g);			// C2S
						sxb += sxd;
					} else if (qtr & 1) {
						g->p.y = y-b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g);						// C2C
					}
					g->p.y = y+b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g);							// C2C
					P += 5 + 2*(a++ - b--);
				}
			} while(a < b);
			if (-a >= ey) {
				g->p.y = y-a; g->p.x = x-b; g->p.x1 = NONFIXED(sxa); hline_clip(g);					// C2S
				g->p.y = y-a; g->p.x = NONFIXED(exa); g->p.x1 = x+b; hline_clip(g);					// E2C
			} else if (-a >= sy) {
				g->p.y = y-a; g->p.x = x-b; g->p.x1 = NONFIXED(sxa); hline_clip(g);					// C2S
			} else if (qtr & 1) {
				g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);								// C2C
			}
			g->p.y = y+b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g);									// C2C
			break;

		case 8:		// S2E3 sy <= ey
		case 9:		// S1E3 sy <= ey
		case 12:	// S2E4 sy <= ey
		case 13:	// S1E4 sy <= ey
		case 24:	// S2E3 sy > ey
		case 25:	// S1E3 sy > ey
		case 28:	// S2E3 sy > ey
		case 29:	// S1E3 sy > ey
			g->p.y = y; g->p.x = x-b; g->p.x1 = x; hline_clip(g);								// C2SE
			sxa -= sxd; exa += exd;
			do {
				if (-a >= sy) {
					g->p.y = y-a; g->p.x = x-b; g->p.x1 = NONFIXED(sxa); hline_clip(g);		// C2S
					sxa -= sxd;
				} else if (qtr & 1) {
					g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);					// C2C
				}
				if (a <= ey) {
					g->p.y = y+a; g->p.x = x-b; g->p.x1 = NONFIXED(exa); hline_clip(g);		// C2E
					exa += exd;
				} else if (qtr & 4) {
					g->p.y = y+a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);					// C2C
				}
				if (P < 0) {
					P += 3 + 2*a++;
				} else {
					if (-b >= sy) {
						g->p.y = y-b; g->p.x = x-a; g->p.x1 = NONFIXED(sxb); hline_clip(g);		// C2S
						sxb += sxd;
					} else if (qtr & 1) {
						g->p.y = y-b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g);					// C2C
					}
					if (b <= ey) {
						g->p.y = y+b; g->p.x = x-a; g->p.x1 = NONFIXED(exb); hline_clip(g);		// C2E
						exb -= exd;
					} else if (qtr & 4) {
						g->p.y = y+b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g); 				// C2C
					}
					P += 5 + 2*(a++ - b--);
				}
			} while(a < b);
			if (-a >= sy) {
				g->p.y = y-a; g->p.x = x-b; g->p.x1 = NONFIXED(sxa); hline_clip(g);				// C2S
			} else if (qtr & 1) {
				g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);							// C2C
			}
			if (a <= ey) {
				g->p.y = y+a; g->p.x = x-b; g->p.x1 = NONFIXED(exa); hline_clip(g);				// C2E
			} else if (qtr & 4) {
				g->p.y = y+a; g->p.x = x-b; g->p.x1 = x+a; hline_clip(g);							// C2C
			}
			break;

		case 10:	// S3E3 sy <= ey
		case 14:	// S3E4 sy <= ey
			g->p.y = y; g->p.x = x; drawpixel_clip(g);													// S2E
			sxa += sxd; exa += exd;
			do {
				if (a <= sy) {
					g->p.y = y+a; g->p.x = NONFIXED(sxa); g->p.x1 = NONFIXED(exa); hline_clip(g);		// S2E
					sxa += sxd; exa += exd;
				} else if (a <= ey) {
					g->p.y = y+a; g->p.x = x-b; g->p.x1 = NONFIXED(exa); hline_clip(g);				// C2E
					exa += exd;
				} else if (qtr & 4) {
					g->p.y = y+a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);							// C2C
				}
				if (P < 0) {
					P += 3 + 2*a++;
				} else {
					if (b <= sy) {
						g->p.y = y+b; g->p.x = NONFIXED(sxb); g->p.x1 = NONFIXED(exb); hline_clip(g);		// S2E
						sxb -= sxd; exb -= exd;
					} else if (b <= ey) {
						g->p.y = y+b; g->p.x = x-a; g->p.x1 = NONFIXED(exb); hline_clip(g);				// C2E
						exb -= exd;
					} else if (qtr & 4) {
						g->p.y = y+b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g);							// C2C
					}
					P += 5 + 2*(a++ - b--);
				}
			} while(a < b);
			if (a <= sy) {
				g->p.y = y+a; g->p.x = NONFIXED(sxa); g->p.x1 = NONFIXED(exa); hline_clip(g);		// S2E
			} else if (a <= ey) {
				g->p.y = y+a; g->p.x = x-b; g->p.x1 = NONFIXED(exa); hline_clip(g);				// C2E
			} else if (qtr & 4) {
				g->p.y = y+a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);							// C2C
			}
			break;

		case 11:	// S4E3 sy <= ey
		case 15:	// S4E4 sy <= ey
			g->p.y = y; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);									// C2C
			do {
				g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);								// C2C
				if (a <= sy) {
					g->p.y = y+a; g->p.x = x-b; g->p.x1 = NONFIXED(exa); hline_clip(g);				// C2E
					g->p.y = y+a; g->p.x = NONFIXED(sxa); g->p.x1 = x+b; hline_clip(g);				// S2C
					sxa += sxd; exa += exd;
				} else if (a <= ey) {
					g->p.y = y+a; g->p.x = x-b; g->p.x1 = NONFIXED(exa); hline_clip(g);				// C2E
					exa += exd;
				} else if (qtr & 4) {
					g->p.y = y+a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);							// C2C
				}
				if (P < 0) {
					P += 3 + 2*a++;
				} else {
					g->p.y = y-b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g);							// C2C
					if (b <= sy) {
						g->p.y = y+b; g->p.x = x-a; g->p.x1 = NONFIXED(exb); hline_clip(g);			// C2E
						g->p.y = y+b; g->p.x = NONFIXED(sxb); g->p.x1 = x+a; hline_clip(g);			// S2C
						sxb -= sxd; exb -= exd;
					} else if (b <= ey) {
						g->p.y = y+b; g->p.x = x-a; g->p.x1 = NONFIXED(exb); hline_clip(g);			// C2E
						exb -= exd;
					} else if (qtr & 4) {
						g->p.y = y+b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g);						// C2C
					}
					P += 5 + 2*(a++ - b--);
				}
			} while(a < b);
			g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);									// C2C
			if (a <= sy) {
				g->p.y = y+a; g->p.x = x-b; g->p.x1 = NONFIXED(exa); hline_clip(g);					// C2E
				g->p.y = y+a; g->p.x = NONFIXED(sxa); g->p.x1 = x+b; hline_clip(g);					// S2C
			} else if (a <= ey) {
				g->p.y = y+a; g->p.x = x-b; g->p.x1 = NONFIXED(exa); hline_clip(g);					// C2E
			} else if (qtr & 4) {
				g->p.y = y+a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);								// C2C
			}
			break;

		case 16:	// S2E2	sy > ey
		case 20:	// S2E1 sy > ey
			g->p.y = y; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);									// C2C
			sxa -= sxd; exa -= exd;
			do {
				if (-a >= sy) {
					g->p.y = y-a; g->p.x = x-b; g->p.x1 = NONFIXED(sxa); hline_clip(g);				// C2S
					g->p.y = y-a; g->p.x = NONFIXED(exa); g->p.x1 = x+b; hline_clip(g);				// E2C
					sxa -= sxd; exa -= exd;
				} else if (-a >= ey) {
					g->p.y = y-a; g->p.x = NONFIXED(exa); g->p.x1 = x+b; hline_clip(g);				// E2C
					exa -= exd;
				} else if (!(qtr & 4)){
					g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g); 						// C2C
				}
				g->p.y = y+a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g); 							// C2C
				if (P < 0) {
					P += 3 + 2*a++;
				} else {
					if (-b >= sy) {
						g->p.y = y-b; g->p.x = x-a; g->p.x1 = NONFIXED(sxb); hline_clip(g);			// C2S
						g->p.y = y-b; g->p.x = NONFIXED(exb); g->p.x1 = x+a; hline_clip(g);			// E2C
						sxb += sxd; exb += exd;
					} else if (-b >= ey) {
						g->p.y = y-b; g->p.x = NONFIXED(exb); g->p.x1 = x+a; hline_clip(g);			// E2C
						exb += exd;
					} else if (!(qtr & 4)){
						g->p.y = y-b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g); 					// C2C
					}
					g->p.y = y+b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g); 						// C2C
					P += 5 + 2*(a++ - b--);
				}
			} while(a < b);
			if (-a >= sy) {
				g->p.y = y-a; g->p.x = x-b; g->p.x1 = NONFIXED(sxa); hline_clip(g);					// C2S
				g->p.y = y-a; g->p.x = NONFIXED(exa); g->p.x1 = x+b; hline_clip(g);					// E2C
			} else if (-a >= ey) {
				g->p.y = y-a; g->p.x = NONFIXED(exa); g->p.x1 = x+b; hline_clip(g);					// E2C
			} else if (!(qtr & 4)){
				g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g); 							// C2C
			}
			g->p.y = y+a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g); 								// C2C
			break;

		case 17:	// S1E2 sy > ey
		case 21:	// S1E1 sy > ey
			if (sy) {
				g->p.x = x; g->p.x1 = x;																// E2S
				sxa -= sxd; exa -= exd;
			} else {
				g->p.x = x; g->p.x1 = x+b;															// E2C
				exa -= exd;
			}
			g->p.y = y;
			hline_clip(g);
			do {
				if (-a >= sy) {
					g->p.y = y-a; g->p.x = NONFIXED(exa); g->p.x1 = NONFIXED(sxa); hline_clip(g);		// E2S
					sxa -= sxd; exa -= exd;
				} else if (-a >= ey) {
					g->p.y = y-a; g->p.x = NONFIXED(exa); g->p.x1 = x+b; hline_clip(g);				// E2C
					exa -= exd;
				} else if (!(qtr & 4)) {
					g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);							// C2C
				}
				if (P < 0) {
					P += 3 + 2*a++;
				} else {
					if (-b >= sy) {
						g->p.y = y-b; g->p.x = NONFIXED(exb); g->p.x1 = NONFIXED(sxb); hline_clip(g);	// E2S
						sxb += sxd; exb += exd;
					} else if (-b >= ey) {
						g->p.y = y-b; g->p.x = NONFIXED(exb); g->p.x1 = x+a; hline_clip(g);			// E2C
						exb += exd;
					} else if (!(qtr & 4)) {
						g->p.y = y-b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g);						// C2C
					}
					P += 5 + 2*(a++ - b--);
				}
			} while(a < b);
			if (-a >= sy) {
				g->p.y = y-a; g->p.x = NONFIXED(exa); g->p.x1 = NONFIXED(sxa); hline_clip(g);			// E2S
			} else if (-a >= ey) {
				g->p.y = y-a; g->p.x = NONFIXED(exa); g->p.x1 = x+b; hline_clip(g);					// E2C
			} else if (!(qtr & 4)) {
				g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);								// C2C
			}
			break;

		case 26:	// S3E3 sy > ey
		case 27:	// S4E3 sy > ey
			g->p.y = y; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);									// C2C
			do {
				g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);								// C2C
				if (a <= ey) {
					g->p.y = y+a; g->p.x = x-b; g->p.x1 = NONFIXED(exa); hline_clip(g);				// C2E
					g->p.y = y+a; g->p.x = NONFIXED(sxa); g->p.x1 = x+b; hline_clip(g);				// S2C
					sxa += sxd; exa += exd;
				} else if (a <= sy) {
					g->p.y = y+a; g->p.x = NONFIXED(sxa); g->p.x1 = x+b; hline_clip(g);				// S2C
					sxa += sxd;
				} else if (!(qtr & 1)) {
					g->p.y = y+a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);							// C2C
				}
				if (P < 0) {
					P += 3 + 2*a++;
				} else {
					g->p.y = y-b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g);							// C2C
					if (b <= ey) {
						g->p.y = y+b; g->p.x = x-a; g->p.x1 = NONFIXED(exb); hline_clip(g);			// C2E
						g->p.y = y+b; g->p.x = NONFIXED(sxb); g->p.x1 = x+a; hline_clip(g);			// S2C
						sxb -= sxd; exb -= exd;
					} else if (b <= sy) {
						g->p.y = y+b; g->p.x = NONFIXED(sxb); g->p.x1 = x+a; hline_clip(g);			// S2C
						sxb -= sxd;
					} else if (!(qtr & 1)) {
						g->p.y = y+b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g);						// C2C
					}
					P += 5 + 2*(a++ - b--);
				}
			} while(a < b);
			g->p.y = y-a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);									// C2C
			if (a <= ey) {
				g->p.y = y+a; g->p.x = x-b; g->p.x1 = NONFIXED(exa); hline_clip(g);					// C2E
				g->p.y = y+a; g->p.x = NONFIXED(sxa); g->p.x1 = x+b; hline_clip(g);					// S2C
			} else if (a <= sy) {
				g->p.y = y+a; g->p.x = NONFIXED(sxa); g->p.x1 = x+b; hline_clip(g);					// S2C
			} else if (!(qtr & 4)) {
				g->p.y = y+a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);								// C2C
			}
			break;

		case 30:	// S3E4 sy > ey
		case 31:	// S4E4 sy > ey
			do {
				if (a <= ey) {
					g->p.y = y+a; g->p.x = NONFIXED(sxa); g->p.x1 = NONFIXED(exa); hline_clip(g);		// S2E
					sxa += sxd; exa += exd;
				} else if (a <= sy) {
					g->p.y = y+a; g->p.x = NONFIXED(sxa); g->p.x1 = x+b; hline_clip(g);				// S2C
					sxa += sxd;
				} else if (!(qtr & 1)) {
					g->p.y = y+a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);							// C2C
				}
				if (P < 0) {
					P += 3 + 2*a++;
				} else {
					if (b <= ey) {
						g->p.y = y+b; g->p.x = NONFIXED(sxb); g->p.x1 = NONFIXED(exb); hline_clip(g);	// S2E
						sxb -= sxd; exb -= exd;
					} else if (b <= sy) {
						g->p.y = y+b; g->p.x = NONFIXED(sxb); g->p.x1 = x+a; hline_clip(g);			// S2C
						sxb -= sxd;
					} else if (!(qtr & 1)) {
						g->p.y = y+b; g->p.x = x-a; g->p.x1 = x+a; hline_clip(g);						// C2C
					}
					P += 5 + 2*(a++ - b--);
				}
			} while(a < b);
			if (a <= ey) {
				g->p.y = y+a; g->p.x = NONFIXED(sxa); g->p.x1 = x+b; hline_clip(g);				// S2C
			} else if (a <= sy) {
				g->p.y = y+a; g->p.x = NONFIXED(sxa); g->p.x1 = x+b; hline_clip(g);					// S2C
			} else if (!(qtr & 4)) {
				g->p.y = y+a; g->p.x = x-b; g->p.x1 = x+b; hline_clip(g);								// C2C
			}
			break;
		}

		autoflush(g);
		MUTEX_EXIT(g);
	}
#endif

#if GDISP_NEED_ARC || GDISP_NEED_ARCSECTORS
	void gdispGDrawRoundedBox(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t radius, color_t color) {
		if (2*radius > cx || 2*radius > cy) {
			gdispGDrawBox(g, x, y, cx, cy, color);
			return;
		}

		#if GDISP_NEED_ARCSECTORS
			gdispGDrawArcSectors(g, x+radius, y+radius, radius, 0x0C, color);
			gdispGDrawArcSectors(g, x+cx-1-radius, y+radius, radius, 0x03, color);
			gdispGDrawArcSectors(g, x+cx-1-radius, y+cy-1-radius, radius, 0xC0, color);
			gdispGDrawArcSectors(g, x+radius, y+cy-1-radius, radius, 0x30, color);
		#else
			gdispGDrawArc(g, x+radius, y+radius, radius, 90, 180, color);
			gdispGDrawArc(g, x+cx-1-radius, y+radius, radius, 0, 90, color);
			gdispGDrawArc(g, x+cx-1-radius, y+cy-1-radius, radius, 270, 360, color);
			gdispGDrawArc(g, x+radius, y+cy-1-radius, radius, 180, 270, color);
		#endif
		gdispGDrawLine(g, x+radius+1, y, x+cx-2-radius, y, color);
		gdispGDrawLine(g, x+cx-1, y+radius+1, x+cx-1, y+cy-2-radius, color);
		gdispGDrawLine(g, x+radius+1, y+cy-1, x+cx-2-radius, y+cy-1, color);
		gdispGDrawLine(g, x, y+radius+1, x, y+cy-2-radius, color);
	}
#endif

#if GDISP_NEED_ARC || GDISP_NEED_ARCSECTORS
	void gdispGFillRoundedBox(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t radius, color_t color) {
		coord_t radius2;

		radius2 = radius*2;
		if (radius2 > cx || radius2 > cy) {
			gdispGFillArea(g, x, y, cx, cy, color);
			return;
		}
		#if GDISP_NEED_ARCSECTORS
			gdispGFillArcSectors(g, x+radius, y+radius, radius, 0x0C, color);
			gdispGFillArcSectors(g, x+cx-1-radius, y+radius, radius, 0x03, color);
			gdispGFillArcSectors(g, x+cx-1-radius, y+cy-1-radius, radius, 0xC0, color);
			gdispGFillArcSectors(g, x+radius, y+cy-1-radius, radius, 0x30, color);
		#else
			gdispGFillArc(g, x+radius, y+radius, radius, 90, 180, color);
			gdispGFillArc(g, x+cx-1-radius, y+radius, radius, 0, 90, color);
			gdispGFillArc(g, x+cx-1-radius, y+cy-1-radius, radius, 270, 360, color);
			gdispGFillArc(g, x+radius, y+cy-1-radius, radius, 180, 270, color);
		#endif
		gdispGFillArea(g, x+radius+1, y, cx-radius2, radius, color);
		gdispGFillArea(g, x+radius+1, y+cy-radius, cx-radius2, radius, color);
		gdispGFillArea(g, x, y+radius, cx, cy-radius2, color);
	}
#endif

#if GDISP_NEED_PIXELREAD
	color_t gdispGGetPixelColor(GDisplay *g, coord_t x, coord_t y) {
		color_t		c;

		/* Always synchronous as it must return a value */
		MUTEX_ENTER(g);
		#if GDISP_HARDWARE_PIXELREAD
			#if GDISP_HARDWARE_PIXELREAD == HARDWARE_AUTODETECT
				if (gvmt(g)->get)
			#endif
			{
				// Best is direct pixel read
				g->p.x = x;
				g->p.y = y;
				c = gdisp_lld_get_pixel_color(g);
				MUTEX_EXIT(g);
				return c;
			}
		#endif
		#if GDISP_HARDWARE_PIXELREAD != TRUE && GDISP_HARDWARE_STREAM_READ
			#if GDISP_HARDWARE_STREAM_READ == HARDWARE_AUTODETECT
				if (gvmt(g)->readcolor)
			#endif
			{
				// Next best is hardware streaming
				g->p.x = x;
				g->p.y = y;
				g->p.cx = 1;
				g->p.cy = 1;
				gdisp_lld_read_start(g);
				c = gdisp_lld_read_color(g);
				gdisp_lld_read_stop(g);
				MUTEX_EXIT(g);
				return c;
			}
		#endif
		#if GDISP_HARDWARE_PIXELREAD != TRUE && GDISP_HARDWARE_STREAM_READ != TRUE
			#if !GDISP_HARDWARE_PIXELREAD && !GDISP_HARDWARE_STREAM_READ
				// Worst is "not possible"
				#error "GDISP: GDISP_NEED_PIXELREAD has been set but there is no hardware support for reading the display"
			#endif
			MUTEX_EXIT(g);
			return 0;
		#endif
	}
#endif

#if GDISP_NEED_SCROLL
	void gdispGVerticalScroll(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
		coord_t		abslines;
		#if GDISP_HARDWARE_SCROLL != TRUE
			coord_t 	fy, dy, ix, fx, i, j;
		#endif

		if (!lines) return;

		MUTEX_ENTER(g);
		#if NEED_CLIPPING
			#if GDISP_HARDWARE_CLIP == HARDWARE_AUTODETECT
				if (!gvmt(g)->setclip)
			#endif
			{
				if (x < g->clipx0) { cx -= g->clipx0 - x; x = g->clipx0; }
				if (y < g->clipy0) { cy -= g->clipy0 - y; y = g->clipy0; }
				if (cx <= 0 || cy <= 0 || x >= g->clipx1 || y >= g->clipy1) { MUTEX_EXIT(g); return; }
				if (x+cx > g->clipx1)	cx = g->clipx1 - x;
				if (y+cy > g->clipy1)	cy = g->clipy1 - y;
			}
		#endif

		abslines = lines < 0 ? -lines : lines;
		if (abslines >= cy) {
			abslines = cy;
			cy = 0;
		} else {
			// Best is hardware scroll
			#if GDISP_HARDWARE_SCROLL
				#if GDISP_HARDWARE_SCROLL == HARDWARE_AUTODETECT
					if (gvmt(g)->vscroll)
				#endif
				{
					g->p.x = x;
					g->p.y = y;
					g->p.cx = cx;
					g->p.cy = cy;
					g->p.y1 = lines;
					g->p.color = bgcolor;
					gdisp_lld_vertical_scroll(g);
					cy -= abslines;
				}
				#if GDISP_HARDWARE_SCROLL == HARDWARE_AUTODETECT
					else
				#endif
			#elif GDISP_LINEBUF_SIZE == 0
				#error "GDISP: GDISP_NEED_SCROLL is set but there is no hardware support and GDISP_LINEBUF_SIZE is zero."
			#endif

			// Scroll Emulation
			#if GDISP_HARDWARE_SCROLL != TRUE
				{
					cy -= abslines;
					if (lines < 0) {
						fy = y+cy-1;
						dy = -1;
					} else {
						fy = y;
						dy = 1;
					}
					// Move the screen - one line at a time
					for(i = 0; i < cy; i++, fy += dy) {

						// Handle where the buffer is smaller than a line
						for(ix=0; ix < cx; ix += GDISP_LINEBUF_SIZE) {

							// Calculate the data we can move in one operation
							fx = cx - ix;
							if (fx > GDISP_LINEBUF_SIZE)
								fx = GDISP_LINEBUF_SIZE;

							// Read one line of data from the screen

							// Best line read is hardware streaming
							#if GDISP_HARDWARE_STREAM_READ
								#if GDISP_HARDWARE_STREAM_READ == HARDWARE_AUTODETECT
									if (gvmt(g)->readstart)
								#endif
								{
									g->p.x = x+ix;
									g->p.y = fy+lines;
									g->p.cx = fx;
									g->p.cy = 1;
									gdisp_lld_read_start(g);
									for(j=0; j < fx; j++)
										g->linebuf[j] = gdisp_lld_read_color(g);
									gdisp_lld_read_stop(g);
								}
								#if GDISP_HARDWARE_STREAM_READ == HARDWARE_AUTODETECT
									else
								#endif
							#endif

							// Next best line read is single pixel reads
							#if GDISP_HARDWARE_STREAM_READ != TRUE && GDISP_HARDWARE_PIXELREAD
								#if GDISP_HARDWARE_PIXELREAD == HARDWARE_AUTODETECT
									if (gvmt(g)->get)
								#endif
								{
									for(j=0; j < fx; j++) {
										g->p.x = x+ix+j;
										g->p.y = fy+lines;
										g->linebuf[j] = gdisp_lld_get_pixel_color(g);
									}
								}
								#if GDISP_HARDWARE_PIXELREAD == HARDWARE_AUTODETECT
									else {
										// Worst is "not possible"
										MUTEX_EXIT(g);
										return;
									}
								#endif
							#endif

							// Worst is "not possible"
							#if !GDISP_HARDWARE_STREAM_READ && !GDISP_HARDWARE_PIXELREAD
								#error "GDISP: GDISP_NEED_SCROLL is set but there is no hardware support for scrolling or reading pixels."
							#endif

							// Write that line to the new location

							// Best line write is hardware bitfills
							#if GDISP_HARDWARE_BITFILLS
								#if GDISP_HARDWARE_BITFILLS == HARDWARE_AUTODETECT
									if (gvmt(g)->blit)
								#endif
								{
									g->p.x = x+ix;
									g->p.y = fy;
									g->p.cx = fx;
									g->p.cy = 1;
									g->p.x1 = 0;
									g->p.y1 = 0;
									g->p.x2 = fx;
									g->p.ptr = (void *)g->linebuf;
									gdisp_lld_blit_area(g);
								}
								#if GDISP_HARDWARE_BITFILLS == HARDWARE_AUTODETECT
									else
								#endif
							#endif

							// Next best line write is hardware streaming
							#if GDISP_HARDWARE_BITFILLS != TRUE && GDISP_HARDWARE_STREAM_WRITE
								#if GDISP_HARDWARE_STREAM_WRITE == HARDWARE_AUTODETECT
									if (gvmt(g)->writestart)
								#endif
								{
									g->p.x = x+ix;
									g->p.y = fy;
									g->p.cx = fx;
									g->p.cy = 1;
									gdisp_lld_write_start(g);
									#if GDISP_HARDWARE_STREAM_POS
										gdisp_lld_write_pos(g);
									#endif
									for(j = 0; j < fx; j++) {
										g->p.color = g->linebuf[j];
										gdisp_lld_write_color(g);
									}
									gdisp_lld_write_stop(g);
								}
								#if GDISP_HARDWARE_STREAM_WRITE == HARDWARE_AUTODETECT
									else
								#endif
							#endif

							// Next best line write is drawing pixels in combination with filling
							#if GDISP_HARDWARE_BITFILLS != TRUE && GDISP_HARDWARE_STREAM_WRITE != TRUE && GDISP_HARDWARE_FILLS && GDISP_HARDWARE_DRAWPIXEL
								// We don't need to test for auto-detect on drawpixel as we know we have it because we don't have streaming.
								#if GDISP_HARDWARE_FILLS == HARDWARE_AUTODETECT
									if (gvmt(g)->fill)
								#endif
								{
									g->p.y = fy;
									g->p.cy = 1;
									g->p.x = x+ix;
									g->p.cx = 1;
									for(j = 0; j < fx; ) {
										g->p.color = g->linebuf[j];
										if (j + g->p.cx < fx && g->linebuf[j] == g->linebuf[j + g->p.cx])
											g->p.cx++;
										else if (g->p.cx == 1) {
											gdisp_lld_draw_pixel(g);
											j++;
											g->p.x++;
										} else {
											gdisp_lld_fill_area(g);
											j += g->p.cx;
											g->p.x += g->p.cx;
											g->p.cx = 1;
										}
									}
								}
								#if GDISP_HARDWARE_FILLS == HARDWARE_AUTODETECT
									else
								#endif
							#endif

							// Worst line write is drawing pixels
							#if GDISP_HARDWARE_BITFILLS != TRUE && GDISP_HARDWARE_STREAM_WRITE != TRUE && GDISP_HARDWARE_FILLS != TRUE && GDISP_HARDWARE_DRAWPIXEL
								// The following test is unneeded because we are guaranteed to have draw pixel if we don't have streaming
								//#if GDISP_HARDWARE_DRAWPIXEL == HARDWARE_AUTODETECT
								//	if (gvmt(g)->pixel)
								//#endif
								{
									g->p.y = fy;
									for(g->p.x = x+ix, j = 0; j < fx; g->p.x++, j++) {
										g->p.color = g->linebuf[j];
										gdisp_lld_draw_pixel(g);
									}
								}
							#endif
						}
					}
				}
			#endif
		}

		/* fill the remaining gap */
		g->p.x = x;
		g->p.y = lines > 0 ? (y+cy) : y;
		g->p.cx = cx;
		g->p.cy = abslines;
		g->p.color = bgcolor;
		fillarea(g);
		autoflush_stopdone(g);
		MUTEX_EXIT(g);
	}
#endif

#if GDISP_NEED_CONTROL
	#if GDISP_HARDWARE_CONTROL
		void gdispGControl(GDisplay *g, unsigned what, void *value) {
			#if GDISP_HARDWARE_CONTROL == HARDWARE_AUTODETECT
				if (!gvmt(g)->control)
					return;
			#endif
			MUTEX_ENTER(g);
			g->p.x = what;
			g->p.ptr = value;
			if (what == GDISP_CONTROL_ORIENTATION) {
				switch ((orientation_t) value) {
				case GDISP_ROTATE_LANDSCAPE:
					g->p.ptr = g->g.Width >= g->g.Height ? (void *)GDISP_ROTATE_0 : (void *)GDISP_ROTATE_90;
					break;
				case GDISP_ROTATE_PORTRAIT:
					g->p.ptr = g->g.Width >= g->g.Height ? (void *)GDISP_ROTATE_90 : (void *)GDISP_ROTATE_0;
					break;
				default:
					break;
				}
			}
			gdisp_lld_control(g);
			#if GDISP_NEED_CLIP || GDISP_NEED_VALIDATION
				if (what == GDISP_CONTROL_ORIENTATION) {
					// Best is hardware clipping
					#if GDISP_HARDWARE_CLIP
						#if GDISP_HARDWARE_CLIP == HARDWARE_AUTODETECT
							if (gvmt(g)->setclip)
						#endif
						{
							g->p.x = 0;
							g->p.y = 0;
							g->p.cx = g->g.Width;
							g->p.cy = g->g.Height;
							gdisp_lld_set_clip(g);
						}
						#if GDISP_HARDWARE_CLIP == HARDWARE_AUTODETECT
							else
						#endif
					#endif

					// Worst is software clipping
					#if GDISP_HARDWARE_CLIP != TRUE
						{
							g->clipx0 = 0;
							g->clipy0 = 0;
							g->clipx1 = g->g.Width;
							g->clipy1 = g->g.Height;
						}
					#endif
				}
			#endif
			MUTEX_EXIT(g);
		}
	#else
		void gdispGControl(GDisplay *g, unsigned what, void *value) {
			(void)g;
			(void)what;
			(void)value;
			/* Ignore everything */
		}
	#endif
#endif

#if GDISP_NEED_QUERY
	#if GDISP_HARDWARE_QUERY
		void *gdispGQuery(GDisplay *g, unsigned what) {
			void *res;

			#if GDISP_HARDWARE_QUERY == HARDWARE_AUTODETECT
				if (!gvmt(g)->query)
					return -1;
			#endif
			MUTEX_ENTER(g);
			g->p.x = (coord_t)what;
			res = gdisp_lld_query(g);
			MUTEX_EXIT(g);
			return res;
		}
	#else
		void *gdispGQuery(GDisplay *g, unsigned what) {
			(void) what;
			return (void *)-1;
		}
	#endif
#endif

/*===========================================================================*/
/* High Level Driver Routines.                                               */
/*===========================================================================*/

void gdispGDrawBox(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
	if (cx <= 0 || cy <= 0) return;
	cx = x+cx-1; cy = y+cy-1;			// cx, cy are now the end point.

	MUTEX_ENTER(g);

	g->p.color = color;

	if (cx - x > 2) {
		g->p.x = x; g->p.y = y; g->p.x1 = cx; hline_clip(g);
		if (y != cy) {
			g->p.x = x; g->p.y = cy; g->p.x1 = cx; hline_clip(g);
			if (cy - y > 2) {
				y++; cy--;
				g->p.x = x; g->p.y = y; g->p.y1 = cy; vline_clip(g);
				g->p.x = cx; g->p.y = y; g->p.y1 = cy; vline_clip(g);
			}
		}
	} else {
		g->p.x = x; g->p.y = y; g->p.y1 = cy; vline_clip(g);
		if (x != cx) {
			g->p.x = cx; g->p.y = y; g->p.y1 = cy; vline_clip(g);
		}
	}

	autoflush(g);
	MUTEX_EXIT(g);
}

#if GDISP_NEED_CONVEX_POLYGON
	void gdispGDrawPoly(GDisplay *g, coord_t tx, coord_t ty, const point *pntarray, unsigned cnt, color_t color) {
		const point	*epnt, *p;

		epnt = &pntarray[cnt-1];

		MUTEX_ENTER(g);
		g->p.color = color;
		for(p = pntarray; p < epnt; p++) {
			g->p.x=tx+p->x; g->p.y=ty+p->y; g->p.x1=tx+p[1].x; g->p.y1=ty+p[1].y; line_clip(g);
		}
		g->p.x=tx+p->x; g->p.y=ty+p->y; g->p.x1=tx+pntarray->x; g->p.y1=ty+pntarray->y; line_clip(g);

		autoflush(g);
		MUTEX_EXIT(g);
	}

	void gdispGFillConvexPoly(GDisplay *g, coord_t tx, coord_t ty, const point *pntarray, unsigned cnt, color_t color) {
		const point	*lpnt, *rpnt, *epnts;
		fixed		lx, rx, lk, rk;
		coord_t		y, ymax, lxc, rxc;

		epnts = &pntarray[cnt-1];

		/* Find a top point */
		rpnt = pntarray;
		for(lpnt=pntarray+1; lpnt <= epnts; lpnt++) {
			if (lpnt->y < rpnt->y)
				rpnt = lpnt;
		}
		lx = rx = FIXED(rpnt->x);
		y = rpnt->y;

		/* Work out the slopes of the two attached line segs */
		for (lpnt = rpnt <= pntarray ? epnts : rpnt-1; lpnt->y == y; cnt--) {
			if (!cnt) return;
			lx = FIXED(lpnt->x);
			lpnt = lpnt <= pntarray ? epnts : lpnt-1;
		}
		for (rpnt = rpnt >= epnts ? pntarray : rpnt+1; rpnt->y == y; cnt--) {
			if (!cnt) return;
			rx = FIXED(rpnt->x);
			rpnt = rpnt >= epnts ? pntarray : rpnt+1;
		}
		lk = (FIXED(lpnt->x) - lx) / (lpnt->y - y);
		rk = (FIXED(rpnt->x) - rx) / (rpnt->y - y);

		// Add error correction for rounding
		lx += FIXED0_5;
		rx += FIXED0_5;

		// Do all the line segments
		MUTEX_ENTER(g);
		g->p.color = color;
		while(1) {
			/* Determine our boundary */
			ymax = rpnt->y < lpnt->y ? rpnt->y : lpnt->y;

			/* Scan down the line segments until we hit a boundary */
			for(; y < ymax; y++) {
				lxc = NONFIXED(lx);
				rxc = NONFIXED(rx);
				/*
				 * Doesn't print the right hand point in order to allow polygon joining.
				 * Also ensures that we draw from left to right with the minimum number
				 * of pixels.
				 */
				if (lxc < rxc) {
					g->p.x=tx+lxc; g->p.y=ty+y; g->p.x1=tx+rxc-1; hline_clip(g);
				} else if (lxc > rxc) {
					g->p.x=tx+rxc; g->p.y=ty+y; g->p.x1=tx+lxc-1; hline_clip(g);
				}

				lx += lk;
				rx += rk;
			}

			if (!cnt) {
				autoflush(g);
				MUTEX_EXIT(g);
				return;
			}
			cnt--;

			/* Replace the appropriate point */
			if (ymax == lpnt->y) {
				for (lpnt = lpnt <= pntarray ? epnts : lpnt-1; lpnt->y == y; cnt--) {
					if (!cnt) {
						autoflush(g);
						MUTEX_EXIT(g);
						return;
					}
					lx = FIXED(lpnt->x);
					lpnt = lpnt <= pntarray ? epnts : lpnt-1;
				}
				lk = (FIXED(lpnt->x) - lx) / (lpnt->y - y);
				lx += FIXED0_5;
			} else {
				for (rpnt = rpnt >= epnts ? pntarray : rpnt+1; rpnt->y == y; cnt--) {
					if (!cnt) {
						autoflush(g);
						MUTEX_EXIT(g);
						return;
					}
					rx = FIXED(rpnt->x);
					rpnt = rpnt >= epnts ? pntarray : rpnt+1;
				}
				rk = (FIXED(rpnt->x) - rx) / (rpnt->y - y);
				rx += FIXED0_5;
			}
		}
	}

	static int32_t rounding_div(const int32_t n, const int32_t d)
	{
		if ((n < 0) != (d < 0))
			return (n - d/2) / d;
		else
			return (n + d/2) / d;
	}

	/* Find a vector (nx, ny) that is perpendicular to (dx, dy) and has length
	 * equal to 'norm'. */
	static void get_normal_vector(coord_t dx, coord_t dy, coord_t norm, coord_t *nx, coord_t *ny)
	{
		int32_t dx2, dy2, len_sq, norm_sq, norm_sq2;
		int div, step, best, delta, abs_delta;

		dx2 = dx; dy2 = dy;
		norm_sq = (int32_t)norm * norm;
		norm_sq2 = norm_sq * 512;

		/* Scale dx2 and dy2 so that
		 *     len_sq / 2 <= norm_sq * 512 <= len_sq * 2.
		 * The scaling by 512 is to yield higher accuracy in division later. */
		len_sq = dx2 * dx2 + dy2 * dy2;

		if (len_sq < norm_sq2)
		{
			while (len_sq && len_sq < norm_sq2)
			{
				len_sq <<= 2; dx2 <<= 1; dy2 <<= 1;
			}
		}
		else if (len_sq > norm_sq2)
		{
			while (len_sq && len_sq > norm_sq2)
			{
				len_sq >>= 2; dx2 >>= 1; dy2 >>= 1;
			}
		}

		/* Now find the divider div so that
		 *     len_sq / div^2 == norm_sq   i.e.  div = sqrt(len_sq / norm_sq)
		 *
		 * This is done using bisection search to avoid the need for floating
		 * point sqrt.
		 *
		 * Based on previous scaling, we know that
		 *     len_sq / 2 <= norm_sq * 512   <=>   div <= sqrt(1024) = 32
		 *     len_sq * 2 >= norm_sq * 512   <=>   div >= sqrt(256) = 16
		 */
		div = 24; step = 8;
		best = 256;

		for (;;)
		{
			dx = dx2 / div;
			dy = dy2 / div;
			len_sq = dx*dx + dy*dy;

			delta = len_sq - norm_sq;

			abs_delta = (delta >= 0) ? delta : -delta;

			if (abs_delta < best)
			{
				*nx = dy;
				*ny = -dx;
				best = abs_delta;
			}

			if (delta > 0)
				div += step;
			else if (delta < 0)
				div -= step;
			else if (delta == 0)
				break;

			if (step == 0)
				break;
			else
				step >>= 1; /* Do one round with step = 0 to calculate final result. */
		}
	}

	void gdispGDrawThickLine(GDisplay *g, coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color, coord_t width, bool_t round) {
		coord_t dx, dy, nx = 0, ny = 0;

		/* Compute the direction vector for the line */
		dx = x1 - x0;
		dy = y1 - y0;

		/* Draw a small dot if the line length is zero. */
		if (dx == 0 && dy == 0)
			dx += 1;

		/* Compute a normal vector with length 'width'. */
		get_normal_vector(dx, dy, width, &nx, &ny);

		/* Handle 1px wide lines gracefully */
		if (nx == 0 && ny == 0)
			nx = 1;

		/* Offset the x0,y0 by half the width of the line. This way we
		 * can keep the width of the line accurate even if it is not evenly
		 * divisible by 2.
		 */
		{
			x0 -= rounding_div(nx, 2);
			y0 -= rounding_div(ny, 2);
		}

		/* Fill in the point array */
		if (!round) {
			/* We use 4 points for the basic line shape:
			 *
			 *  pt1                                      pt2
			 * (+n) ------------------------------------ (d+n)
			 *   |                                       |
			 * (0,0) ----------------------------------- (d)
			 *  pt0                                      pt3
			 */
			point pntarray[4];

			pntarray[0].x = 0;
			pntarray[0].y = 0;
			pntarray[1].x = nx;
			pntarray[1].y = ny;
			pntarray[2].x = dx + nx;
			pntarray[2].y = dy + ny;
			pntarray[3].x = dx;
			pntarray[3].y = dy;

			gdispGFillConvexPoly(g, x0, y0, pntarray, 4, color);
		} else {
			/* We use 4 points for basic shape, plus 4 extra points for ends:
			 *
			 *           pt3 ------------------ pt4
			 *          /                         \
			 *        pt2                        pt5
			 *         |                          |
			 *        pt1                        pt6
			 *         \                         /
			 *          pt0 -------------------pt7
			 */
			point pntarray[8];
			coord_t nx2, ny2;

			/* Magic numbers:
			 * 75/256  = sin(45) / (1 + sqrt(2))		diagonal octagon segments
			 * 106/256 = 1 / (1 + sqrt(2))				octagon side
			 * 53/256  = 0.5 / (1 + sqrt(2))			half of octagon side
			 * 150/256 = 1 - 1 / (1 + sqrt(2))	  		octagon height minus one side
			 */

			/* Rotate the normal vector 45 deg counter-clockwise and reduce
			 * to 1 / (1 + sqrt(2)) length, for forming octagonal ends. */
			nx2 = rounding_div((nx * 75 + ny * 75), 256);
			ny2 = rounding_div((-nx * 75 + ny * 75), 256);

			/* Offset and extend the line so that the center of the octagon
			 * is at the specified points. */
			x0 += ny * 53 / 256;
			y0 -= nx * 53 / 256;
			dx -= ny * 106 / 256;
			dy += nx * 106 / 256;

			/* Now fill in the points by summing the calculated vectors. */
			pntarray[0].x = 0;
			pntarray[0].y = 0;
			pntarray[1].x = nx2;
			pntarray[1].y = ny2;
			pntarray[2].x = nx2 + nx * 106/256;
			pntarray[2].y = ny2 + ny * 106/256;
			pntarray[3].x = nx;
			pntarray[3].y = ny;
			pntarray[4].x = dx + nx;
			pntarray[4].y = dy + ny;
			pntarray[5].x = dx + nx - nx2;
			pntarray[5].y = dy + ny - ny2;
			pntarray[6].x = dx + nx * 150/256 - nx2;
			pntarray[6].y = dy + ny * 150/256 - ny2;
			pntarray[7].x = dx;
			pntarray[7].y = dy;

			gdispGFillConvexPoly(g, x0, y0, pntarray, 8, color);
		}
	}
#endif

#if GDISP_NEED_TEXT
	#include "mcufont/mcufont.h"

	#if GDISP_NEED_ANTIALIAS && GDISP_HARDWARE_PIXELREAD
		static void drawcharline(int16_t x, int16_t y, uint8_t count, uint8_t alpha, void *state) {
			#define GD	((GDisplay *)state)
			if (y < GD->t.clipy0 || y >= GD->t.clipy1 || x+count <= GD->t.clipx0 || x >= GD->t.clipx1)
				return;
			if (x < GD->t.clipx0) {
				count -= GD->t.clipx0 - x;
				x = GD->t.clipx0;
			}
			if (x+count > GD->t.clipx1)
				count = GD->t.clipx1 - x;
			if (alpha == 255) {
				GD->p.x = x; GD->p.y = y; GD->p.x1 = x+count-1; GD->p.color = GD->t.color;
				hline_clip(GD);
			} else {
				for (; count; count--, x++) {
					GD->p.x = x; GD->p.y = y;
					GD->p.color = gdispBlendColor(GD->t.color, gdisp_lld_get_pixel_color(GD), alpha);
					drawpixel_clip(GD);
				}
			}
			#undef GD
		}
	#else
		static void drawcharline(int16_t x, int16_t y, uint8_t count, uint8_t alpha, void *state) {
			#define GD	((GDisplay *)state)
			if (y < GD->t.clipy0 || y >= GD->t.clipy1 || x+count <= GD->t.clipx0 || x >= GD->t.clipx1)
				return;
			if (x < GD->t.clipx0) {
				count -= GD->t.clipx0 - x;
				x = GD->t.clipx0;
			}
			if (x+count > GD->t.clipx1)
				count = GD->t.clipx1 - x;
			if (alpha > 0x80) {			// A best approximation when using anti-aliased fonts but we can't actually draw them anti-aliased
				GD->p.x = x; GD->p.y = y; GD->p.x1 = x+count-1; GD->p.color = GD->t.color;
				hline_clip(GD);
			}
			#undef GD
		}
	#endif

	#if GDISP_NEED_ANTIALIAS
		static void fillcharline(int16_t x, int16_t y, uint8_t count, uint8_t alpha, void *state) {
			#define GD	((GDisplay *)state)
			if (y < GD->t.clipy0 || y >= GD->t.clipy1 || x+count <= GD->t.clipx0 || x >= GD->t.clipx1)
				return;
			if (x < GD->t.clipx0) {
				count -= GD->t.clipx0 - x;
				x = GD->t.clipx0;
			}
			if (x+count > GD->t.clipx1)
				count = GD->t.clipx1 - x;
			if (alpha == 255) {
				GD->p.color = GD->t.color;
			} else {
				GD->p.color = gdispBlendColor(GD->t.color, GD->t.bgcolor, alpha);
			}
			GD->p.x = x; GD->p.y = y; GD->p.x1 = x+count-1;
			hline_clip(GD);
			#undef GD
		}
	#else
		#define fillcharline	drawcharline
	#endif

	/* Callback to render characters. */
	static uint8_t drawcharglyph(int16_t x, int16_t y, mf_char ch, void *state) {
		#define GD	((GDisplay *)state)
			return mf_render_character(GD->t.font, x, y, ch, drawcharline, state);
		#undef GD
	}

	/* Callback to render characters. */
	static uint8_t fillcharglyph(int16_t x, int16_t y, mf_char ch, void *state) {
		#define GD	((GDisplay *)state)
			return mf_render_character(GD->t.font, x, y, ch, fillcharline, state);
		#undef GD
	}

	/* Callback to render string boxes with word wrap. */
	#if GDISP_NEED_TEXT_WORDWRAP
		static bool mf_countline_callback(mf_str line, uint16_t count, void *state) {
			int *linecount = (int*)state;
			(*linecount)++;

			return TRUE;
		}
		static bool mf_drawline_callback(mf_str line, uint16_t count, void *state) {
			wrapParameters_t* wrapParameters = (wrapParameters_t*)state;

			mf_render_aligned(wrapParameters->font, wrapParameters->x, wrapParameters->y, wrapParameters->justify, line, count, drawcharglyph, wrapParameters->g);

			wrapParameters->y += wrapParameters->font->line_height;
			return TRUE;
		}
		static bool mf_fillline_callback(mf_str line, uint16_t count, void *state) {
			wrapParameters_t* wrapParameters = (wrapParameters_t*)state;

			mf_render_aligned(wrapParameters->font, wrapParameters->x, wrapParameters->y, wrapParameters->justify, line, count, fillcharglyph, wrapParameters->g);

			wrapParameters->y += wrapParameters->font->line_height;
			return TRUE;
		}	
	#endif

	void gdispGDrawChar(GDisplay *g, coord_t x, coord_t y, uint16_t c, font_t font, color_t color) {
		MUTEX_ENTER(g);
		g->t.font = font;
		g->t.clipx0 = x;
		g->t.clipy0 = y;
		g->t.clipx1 = x + mf_character_width(font, c) + font->baseline_x;
		g->t.clipy1 = y + font->height;
		g->t.color = color;
		mf_render_character(font, x, y, c, drawcharline, g);
		autoflush(g);
		MUTEX_EXIT(g);
	}

	void gdispGFillChar(GDisplay *g, coord_t x, coord_t y, uint16_t c, font_t font, color_t color, color_t bgcolor) {
		MUTEX_ENTER(g);
		g->p.cx = mf_character_width(font, c) + font->baseline_x;
		g->p.cy = font->height;
		g->t.font = font;
		g->t.clipx0 = g->p.x = x;
		g->t.clipy0 = g->p.y = y;
		g->t.clipx1 = g->p.x+g->p.cx;
		g->t.clipy1 = g->p.y+g->p.cy;
		g->t.color = color;
		g->t.bgcolor = g->p.color = bgcolor;

		TEST_CLIP_AREA(g) {
			fillarea(g);
			mf_render_character(font, x, y, c, fillcharline, g);
		}
		autoflush(g);
		MUTEX_EXIT(g);
	}

	void gdispGDrawString(GDisplay *g, coord_t x, coord_t y, const char *str, font_t font, color_t color) {
		MUTEX_ENTER(g);
		g->t.font = font;
		g->t.clipx0 = x;
		g->t.clipy0 = y;
		g->t.clipx1 = x + mf_get_string_width(font, str, 0, 0);
		g->t.clipy1 = y + font->height;
		g->t.color = color;

		mf_render_aligned(font, x+font->baseline_x, y, MF_ALIGN_LEFT, str, 0, drawcharglyph, g);
		autoflush(g);
		MUTEX_EXIT(g);
	}

	void gdispGFillString(GDisplay *g, coord_t x, coord_t y, const char *str, font_t font, color_t color, color_t bgcolor) {
		MUTEX_ENTER(g);
		g->p.cx = mf_get_string_width(font, str, 0, 0) + font->baseline_x;
		g->p.cy = font->height;
		g->t.font = font;
		g->t.clipx0 = g->p.x = x;
		g->t.clipy0 = g->p.y = y;
		g->t.clipx1 = g->p.x+g->p.cx;
		g->t.clipy1 = g->p.y+g->p.cy;
		g->t.color = color;
		g->t.bgcolor = g->p.color = bgcolor;

		TEST_CLIP_AREA(g) {
			fillarea(g);
			mf_render_aligned(font, x+font->baseline_x, y, MF_ALIGN_LEFT, str, 0, fillcharglyph, g);
		}

		autoflush(g);
		MUTEX_EXIT(g);
	}

	void gdispGDrawStringBox(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, justify_t justify) {
		#if GDISP_NEED_TEXT_WORDWRAP
			wrapParameters_t wrapParameters;
			uint16_t nbrLines;
		#endif

		MUTEX_ENTER(g);

		g->t.font = font;
		g->t.clipx0 = x;
		g->t.clipy0 = y;
		g->t.clipx1 = x+cx;
		g->t.clipy1 = y+cy;
		g->t.color = color;

		/* Select the anchor position */
		switch(justify) {
		case justifyCenter:
			x += (cx + 1) / 2;
			break;
		case justifyRight:
			x += cx;
			break;
		default:	// justifyLeft
			x += font->baseline_x;
			break;
		}

		/* Render */
		#if GDISP_NEED_TEXT_WORDWRAP
			wrapParameters.x = x;
			wrapParameters.y = y;
			wrapParameters.font = font;
			wrapParameters.justify = justify;
			wrapParameters.g = g;

			// Count the number of lines
			nbrLines = 0;
			mf_wordwrap(font, cx, str, mf_countline_callback, &nbrLines);
			wrapParameters.y += (cy+1 - nbrLines*font->height)/2;
			
			mf_wordwrap(font, cx, str, mf_fillline_callback, &wrapParameters);
		#else
			y += (cy+1 - font->height)/2;
			mf_render_aligned(font, x, y, justify, str, 0, drawcharglyph, g);
		#endif

		autoflush(g);
		MUTEX_EXIT(g);
	}
	
	void gdispGFillStringBoxNoWrap(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, color_t bgcolor, justify_t justify) {

		MUTEX_ENTER(g);

		g->p.cx = cx;
		g->p.cy = cy;
		g->t.font = font;
		g->t.clipx0 = g->p.x = x;
		g->t.clipy0 = g->p.y = y;
		g->t.clipx1 = x+cx;
		g->t.clipy1 = y+cy;
		g->t.color = color;
		g->t.bgcolor = g->p.color = bgcolor;

		TEST_CLIP_AREA(g) {

			// background fill
			fillarea(g);

			/* Select the anchor position */
			switch(justify & 0x3) {
			case justifyCenter:
				x += (cx + 1) / 2;
				break;
			case justifyRight:
				x += cx;
				break;
			default:	// justifyLeft
				x += font->baseline_x;
				break;
			}

			/* Render */			
			y += (cy+1 - font->height)/2;
			mf_render_aligned(font, x, y, justify, str, 0, fillcharglyph, g);

		}
		
		autoflush(g);
		MUTEX_EXIT(g);
	}

	void gdispGFillStringBox(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, color_t bgcolor, justify_t justify) {
		#if GDISP_NEED_TEXT_WORDWRAP
			wrapParameters_t wrapParameters;
			uint16_t nbrLines;
		#endif

		MUTEX_ENTER(g);

		g->p.cx = cx;
		g->p.cy = cy;
		g->t.font = font;
		g->t.clipx0 = g->p.x = x;
		g->t.clipy0 = g->p.y = y;
		g->t.clipx1 = x+cx;
		g->t.clipy1 = y+cy;
		g->t.color = color;
		g->t.bgcolor = g->p.color = bgcolor;

		TEST_CLIP_AREA(g) {

			// background fill
			fillarea(g);

			/* Select the anchor position */
			switch(justify & 0x3) {
			case justifyCenter:
				x += (cx + 1) / 2;
				break;
			case justifyRight:
				x += cx;
				break;
			default:	// justifyLeft
				x += font->baseline_x;
				break;
			}

			/* Render */
			#if GDISP_NEED_TEXT_WORDWRAP
				wrapParameters.x = x;
				wrapParameters.y = y;
				wrapParameters.font = font;
				wrapParameters.justify = justify & 0x3;
				wrapParameters.g = g;


				// Count the number of lines
				nbrLines = 0;
				mf_wordwrap(font, cx, str, mf_countline_callback, &nbrLines);
				if (!(justify & 4))
					wrapParameters.y += (cy+1 - nbrLines*font->height)/2;
				mf_wordwrap(font, cx, str, mf_fillline_callback, &wrapParameters);
			#else
				y += (cy+1 - font->height)/2;
				mf_render_aligned(font, x, y, justify & 0x3, str, 0, fillcharglyph, g);
			#endif
		}

		autoflush(g);
		MUTEX_EXIT(g);
	}

	coord_t gdispGetFontMetric(font_t font, fontmetric_t metric) {
		/* No mutex required as we only read static data */
		switch(metric) {
		case fontHeight:			return font->height;
		case fontDescendersHeight:	return font->height - font->baseline_y;
		case fontLineSpacing:		return font->line_height;
		case fontCharPadding:		return 0;
		case fontMinWidth:			return font->min_x_advance;
		case fontMaxWidth:			return font->max_x_advance;
		case fontBaselineX:			return font->baseline_x;
		case fontBaselineY:			return font->baseline_y;
		}
		return 0;
	}

	coord_t gdispGetCharWidth(char c, font_t font) {
		/* No mutex required as we only read static data */
		return mf_character_width(font, c);
	}

	coord_t gdispGetStringWidthCount(const char* str, font_t font, uint16_t count) {
		if (!str)
			return 0;

		// No mutex required as we only read static data
		#if GDISP_NEED_TEXT_KERNING
			return mf_get_string_width(font, str, count, TRUE);
		#else
			return mf_get_string_width(font, str, count, FALSE);
		#endif
	}

	coord_t gdispGetStringWidth(const char* str, font_t font) {
		return gdispGetStringWidthCount(str, font, 0);
	}
#endif

color_t gdispBlendColor(color_t fg, color_t bg, uint8_t alpha)
{
	uint16_t fg_ratio = alpha + 1;
	uint16_t bg_ratio = 256 - alpha;
	uint16_t r, g, b;

	r = RED_OF(fg) * fg_ratio;
	g = GREEN_OF(fg) * fg_ratio;
	b = BLUE_OF(fg) * fg_ratio;

	r += RED_OF(bg) * bg_ratio;
	g += GREEN_OF(bg) * bg_ratio;
	b += BLUE_OF(bg) * bg_ratio;

	r >>= 8;
	g >>= 8;
	b >>= 8;

	return RGB2COLOR(r, g, b);
}

color_t gdispContrastColor(color_t color) {
	uint16_t r, g, b;

	r = RED_OF(color) > 128 ? 0 : 255;
	g = GREEN_OF(color) > 128 ? 0 : 255;
	b = BLUE_OF(color) > 128 ? 0 : 255;

	return RGB2COLOR(r, g, b);
}

#if (!defined(gdispPackPixels) && !defined(GDISP_PIXELFORMAT_CUSTOM))
	void gdispPackPixels(pixel_t *buf, coord_t cx, coord_t x, coord_t y, color_t color) {
		/* No mutex required as we only read static data */
		#if defined(GDISP_PIXELFORMAT_RGB888)
			#error "GDISP: Packed pixels not supported yet"
		#elif defined(GDISP_PIXELFORMAT_RGB444)
			#error "GDISP: Packed pixels not supported yet"
		#elif defined(GDISP_PIXELFORMAT_RGB666)
			#error "GDISP: Packed pixels not supported yet"
		#elif
			#error "GDISP: Unsupported packed pixel format"
		#endif
	}
#endif

#endif /* GFX_USE_GDISP */
