/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_frame.c
 * @brief   GWIN sub-system frame code.
 */

#include "../../gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_FRAME

#include "gwin_class.h"

/* Some position values */
#define FRM_BUTTON_X	18					// Button Width
#define FRM_BUTTON_Y	18					// Button Height
#define FRM_BUTTON_I	3					// Button inner margin
#define FRM_BUTTON_T	2					// Gap from top of window to button
#define FRM_BUTTON_B	2					// Gap from button to the bottom of the frame title area
#define FRM_BORDER_L	2					// Left Border
#define FRM_BORDER_R	2					// Right Border
#define FRM_BORDER_T	(FRM_BUTTON_Y+FRM_BUTTON_T+FRM_BUTTON_B)	// Top Border (Title area)
#define FRM_BORDER_B	2					// Bottom Border

/* Internal state flags */
#define GWIN_FRAME_USER_FLAGS		(GWIN_FRAME_CLOSE_BTN|GWIN_FRAME_MINMAX_BTN|GWIN_FRAME_KEEPONCLOSE)

#if GWIN_FRAME_CLOSE_BTN < GWIN_FIRST_CONTROL_FLAG
	#error "GWIN Frame: - Flag definitions don't match"
#endif
#if GWIN_FRAME_REDRAW_FRAME > GWIN_LAST_CONTROL_FLAG
	#error "GWIN Frame: - Flag definitions don't match"
#endif

static coord_t FrameBorderSizeL(GHandle gh)	{ (void)gh; return FRM_BORDER_L; }
static coord_t FrameBorderSizeR(GHandle gh)	{ (void)gh; return FRM_BORDER_R; }
static coord_t FrameBorderSizeT(GHandle gh)	{ (void)gh; return FRM_BORDER_T; }
static coord_t FrameBorderSizeB(GHandle gh)	{ (void)gh; return FRM_BORDER_B; }

static void forceFrameRedraw(GWidgetObject *gw) {
	// Force a redraw of just the frame.
	// This is a big naughty but who really cares.
	gw->g.flags |= GWIN_FRAME_REDRAW_FRAME;
	gw->fnDraw(gw, gw->fnParam);
	gw->g.flags &= ~GWIN_FRAME_REDRAW_FRAME;
}

#if GINPUT_NEED_MOUSE
	static void FrameMouseDown(GWidgetObject *gw, coord_t x, coord_t y) {
		coord_t		pos;

		// We must be clicking on the frame button area to be of interest
		if (y < FRM_BUTTON_T || y >= FRM_BUTTON_T+FRM_BUTTON_Y)
			return;

		pos = gw->g.width - (FRM_BORDER_R+FRM_BUTTON_X);
		if ((gw->g.flags & GWIN_FRAME_CLOSE_BTN)) {
			if (x >= pos && x < pos+FRM_BUTTON_X) {
				// Close is pressed - force redraw the frame only
				gw->g.flags |= GWIN_FRAME_CLOSE_PRESSED;
				forceFrameRedraw(gw);
				return;
			}
			pos -= FRM_BUTTON_X;
		}
		if ((gw->g.flags & GWIN_FRAME_MINMAX_BTN)) {
			if (x >= pos && x < pos+FRM_BUTTON_X) {
				// Close is pressed - force redraw the frame only
				gw->g.flags |= GWIN_FRAME_MAX_PRESSED;
				forceFrameRedraw(gw);
				return;
			}
			pos -= FRM_BUTTON_X;
			if (x >= pos && x < pos+FRM_BUTTON_X) {
				// Close is pressed - force redraw the frame only
				gw->g.flags |= GWIN_FRAME_MIN_PRESSED;
				forceFrameRedraw(gw);
				return;
			}
			pos -= FRM_BUTTON_X;
		}
	}

	static void FrameMouseUp(GWidgetObject *gw, coord_t x, coord_t y) {
		#if GWIN_BUTTON_LAZY_RELEASE
			if ((gw->g.flags & GWIN_FRAME_CLOSE_PRESSED)) {
				// Close is released - destroy the window
				gw->g.flags &= ~(GWIN_FRAME_CLOSE_PRESSED|GWIN_FRAME_MAX_PRESSED|GWIN_FRAME_MIN_PRESSED);
				forceFrameRedraw(gw);
				_gwinSendEvent(&gw->g, GEVENT_GWIN_CLOSE);
				if (!(gw->g.flags & GWIN_FRAME_KEEPONCLOSE))
					_gwinDestroy(&gw->g, REDRAW_INSESSION);
				return;
			}
			if ((gw->g.flags & GWIN_FRAME_MAX_PRESSED)) {
				// Max is released - maximize the window
				gw->g.flags &= ~(GWIN_FRAME_CLOSE_PRESSED|GWIN_FRAME_MAX_PRESSED|GWIN_FRAME_MIN_PRESSED);
				forceFrameRedraw(gw);
				gwinSetMinMax(&gw->g, GWIN_MAXIMIZE);
				return;
			}
			if ((gw->g.flags & GWIN_FRAME_MIN_PRESSED)) {
				// Min is released - minimize the window
				gw->g.flags &= ~(GWIN_FRAME_CLOSE_PRESSED|GWIN_FRAME_MAX_PRESSED|GWIN_FRAME_MIN_PRESSED);
				forceFrameRedraw(gw);
				gwinSetMinMax(&gw->g, GWIN_MINIMIZE);
				return;
			}
		#else
			// If nothing is pressed we have nothing to do.
			if (!(gw->g.flags & (GWIN_FRAME_CLOSE_PRESSED|GWIN_FRAME_MAX_PRESSED|GWIN_FRAME_MIN_PRESSED)))
				return;

			// We must be releasing over the button
			if (y >= FRM_BUTTON_T && y < FRM_BUTTON_T+FRM_BUTTON_Y) {
				coord_t		pos;

				pos = gw->g.width - (FRM_BORDER_R+FRM_BUTTON_X);
				if ((gw->g.flags & GWIN_FRAME_CLOSE_BTN)) {
					if ((gw->g.flags & GWIN_FRAME_CLOSE_PRESSED) && x >= pos && x <= pos+FRM_BUTTON_X) {
						// Close is released - destroy the window. This is tricky as we already have the drawing lock.
						gw->g.flags &= ~(GWIN_FRAME_CLOSE_PRESSED|GWIN_FRAME_MAX_PRESSED|GWIN_FRAME_MIN_PRESSED);
						forceFrameRedraw(gw);
						_gwinSendEvent(&gw->g, GEVENT_GWIN_CLOSE);
						if (!(gw->g.flags & GWIN_FRAME_KEEPONCLOSE))
							_gwinDestroy(&gw->g, REDRAW_INSESSION);
						return;
					}
					pos -= FRM_BUTTON_X;
				}
				if ((gw->g.flags & GWIN_FRAME_MINMAX_BTN)) {
					if ((gw->g.flags & GWIN_FRAME_MAX_PRESSED) && x >= pos && x <= pos+FRM_BUTTON_X) {
						// Max is released - maximize the window
						gw->g.flags &= ~(GWIN_FRAME_CLOSE_PRESSED|GWIN_FRAME_MAX_PRESSED|GWIN_FRAME_MIN_PRESSED);
						forceFrameRedraw(gw);
						gwinSetMinMax(&gw->g, GWIN_MAXIMIZE);
						return;
					}
					pos -= FRM_BUTTON_X;
					if ((gw->g.flags & GWIN_FRAME_MIN_PRESSED) && x >= pos && x <= pos+FRM_BUTTON_X) {
						// Min is released - minimize the window
						gw->g.flags &= ~(GWIN_FRAME_CLOSE_PRESSED|GWIN_FRAME_MAX_PRESSED|GWIN_FRAME_MIN_PRESSED);
						forceFrameRedraw(gw);
						gwinSetMinMax(&gw->g, GWIN_MINIMIZE);
						return;
					}
					pos -= FRM_BUTTON_X;
				}
			}

			// Clear any flags and redraw the frame
			gw->g.flags &= ~(GWIN_FRAME_CLOSE_PRESSED|GWIN_FRAME_MAX_PRESSED|GWIN_FRAME_MIN_PRESSED);
			forceFrameRedraw(gw);
		#endif
	}
#endif

static const gcontainerVMT frameVMT = {
	{
		{
			"Frame",					// The classname
			sizeof(GFrameObject),		// The object size
			_gcontainerDestroy,			// The destroy routine
			_gcontainerRedraw,			// The redraw routine
			0,							// The after-clear routine
		},
		gwinFrameDraw_Std,				// The default drawing routine
		#if GINPUT_NEED_MOUSE
			{
				FrameMouseDown,			// Process mouse down event
				FrameMouseUp,			// Process mouse up events
				0,						// Process mouse move events
			},
		#endif
		#if GINPUT_NEED_KEYBOARD || GWIN_NEED_KEYBOARD
			{
				0						// Process keyboard events
			},
		#endif
		#if GINPUT_NEED_TOGGLE
			{
				0,						// 1 toggle role
				0,						// Assign Toggles
				0,						// Get Toggles
				0,						// Process toggle off events
				0,						// Process toggle on events
			},
		#endif
		#if GINPUT_NEED_DIAL
			{
				0,						// 1 dial roles
				0,						// Assign Dials
				0,						// Get Dials
				0,						// Process dial move events
			},
		#endif
	},
	FrameBorderSizeL,					// The size of the left border (mandatory)
	FrameBorderSizeT,					// The size of the top border (mandatory)
	FrameBorderSizeR,					// The size of the right border (mandatory)
	FrameBorderSizeB,					// The size of the bottom border (mandatory)
	0,									// A child has been added (optional)
	0,									// A child has been deleted (optional)
};

GHandle gwinGFrameCreate(GDisplay *g, GFrameObject *fo, GWidgetInit *pInit, uint32_t flags) {
	if (!(fo = (GFrameObject *)_gcontainerCreate(g, (GContainerObject *)fo, pInit, &frameVMT)))
		return 0;

	// Make sure we only have "safe" flags.
	flags &= GWIN_FRAME_USER_FLAGS;

	/* Apply flags. We apply these here so the controls above are outside the child area */
	fo->g.flags |= flags;

	gwinSetVisible(&fo->g, pInit->g.show);

	return &fo->g;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Default render routines                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////

void gwinFrameDraw_Transparent(GWidgetObject *gw, void *param) {
	const GColorSet		*pcol;
	coord_t				pos;
	color_t				contrast;
	color_t				btn;
	(void)param;

	if (gw->g.vmt != (gwinVMT *)&frameVMT)
		return;

	pcol = 	(gw->g.flags & GWIN_FLG_SYSENABLED) ? &gw->pstyle->enabled : &gw->pstyle->disabled;
	contrast = gdispContrastColor(pcol->edge);
	btn = gdispBlendColor(pcol->edge, contrast, 128);

	// Render the frame
	gdispGFillStringBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width, FRM_BORDER_T, gw->text, gw->g.font, contrast, pcol->edge, justifyCenter);
	gdispGFillArea(gw->g.display, gw->g.x, gw->g.y+FRM_BORDER_T, FRM_BORDER_L, gw->g.height-(FRM_BORDER_T+FRM_BORDER_B), pcol->edge);
	gdispGFillArea(gw->g.display, gw->g.x+gw->g.width-FRM_BORDER_R, gw->g.y+FRM_BORDER_T, FRM_BORDER_R, gw->g.height-(FRM_BORDER_T+FRM_BORDER_B), pcol->edge);
	gdispGFillArea(gw->g.display, gw->g.x, gw->g.y+gw->g.height-FRM_BORDER_B, gw->g.width, FRM_BORDER_B, pcol->edge);

	// Add the buttons
	pos = gw->g.x+gw->g.width - (FRM_BORDER_R+FRM_BUTTON_X);

	if ((gw->g.flags & GWIN_FRAME_CLOSE_BTN)) {
		if ((gw->g.flags & GWIN_FRAME_CLOSE_PRESSED))
			gdispFillArea(pos, gw->g.y+FRM_BUTTON_T, FRM_BUTTON_X, FRM_BUTTON_Y, btn);
		gdispDrawLine(pos+FRM_BUTTON_I, gw->g.y+(FRM_BUTTON_T+FRM_BUTTON_I), pos+(FRM_BUTTON_X-FRM_BUTTON_I-1), gw->g.y+(FRM_BUTTON_T+FRM_BUTTON_Y-FRM_BUTTON_I-1), contrast);
		gdispDrawLine(pos+(FRM_BUTTON_X-FRM_BUTTON_I-1), gw->g.y+(FRM_BUTTON_T+FRM_BUTTON_I), pos+FRM_BUTTON_I, gw->g.y+(FRM_BUTTON_T+FRM_BUTTON_Y-FRM_BUTTON_I-1), contrast);
		pos -= FRM_BUTTON_X;
	}

	if ((gw->g.flags & GWIN_FRAME_MINMAX_BTN)) {
		if ((gw->g.flags & GWIN_FRAME_MAX_PRESSED))
			gdispFillArea(pos, gw->g.y+FRM_BUTTON_T, FRM_BUTTON_X, FRM_BUTTON_Y, btn);
		// the symbol
		gdispDrawBox(pos+FRM_BUTTON_I, gw->g.y+(FRM_BUTTON_T+FRM_BUTTON_I), FRM_BUTTON_X-2*FRM_BUTTON_I, FRM_BUTTON_Y-2*FRM_BUTTON_I, contrast);
		gdispDrawLine(pos+(FRM_BUTTON_I+1), gw->g.y+(FRM_BUTTON_T+FRM_BUTTON_I+1), pos+(FRM_BUTTON_X-FRM_BUTTON_I-2), gw->g.y+(FRM_BUTTON_T+FRM_BUTTON_I+1), contrast);
		gdispDrawLine(pos+(FRM_BUTTON_I+1), gw->g.y+(FRM_BUTTON_T+FRM_BUTTON_I+2), pos+(FRM_BUTTON_X-FRM_BUTTON_I-2), gw->g.y+(FRM_BUTTON_T+FRM_BUTTON_I+2), contrast);
		pos -= FRM_BUTTON_X;
		if ((gw->g.flags & GWIN_FRAME_MIN_PRESSED))
			gdispFillArea(pos, gw->g.y+FRM_BUTTON_T, FRM_BUTTON_X, FRM_BUTTON_Y, btn);
		gdispDrawLine(pos+FRM_BUTTON_I, gw->g.y+(FRM_BUTTON_T+FRM_BUTTON_Y-FRM_BUTTON_I-1), pos+(FRM_BUTTON_X-FRM_BUTTON_I-1), gw->g.y+(FRM_BUTTON_T+FRM_BUTTON_Y-FRM_BUTTON_I-1), contrast);
		pos -= FRM_BUTTON_X;
	}

	// Don't touch the client area
}

void gwinFrameDraw_Std(GWidgetObject *gw, void *param) {
	(void)param;

	if (gw->g.vmt != (gwinVMT *)&frameVMT)
		return;

	// Draw the frame
	gwinFrameDraw_Transparent(gw, param);

	// Drop out if that is all we want to draw
	if ((gw->g.flags & GWIN_FRAME_REDRAW_FRAME))
		return;

	// Draw the client area
	gdispGFillArea(gw->g.display, gw->g.x + FRM_BORDER_L, gw->g.y + FRM_BORDER_T, gw->g.width - (FRM_BORDER_L+FRM_BORDER_R), gw->g.height - (FRM_BORDER_T+FRM_BORDER_B), gw->pstyle->background);
}

#if GDISP_NEED_IMAGE
	void gwinFrameDraw_Image(GWidgetObject *gw, void *param) {
		#define gi			((gdispImage *)param)
		coord_t				x, y, iw, ih, mx, my;

		if (gw->g.vmt != (gwinVMT *)&frameVMT)
			return;

		// Draw the frame
		gwinFrameDraw_Transparent(gw, param);

		// Drop out if that is all we want to draw
		if ((gw->g.flags & GWIN_FRAME_REDRAW_FRAME))
			return;

		// Draw the client area by tiling the image
		mx = gw->g.x+gw->g.width - FRM_BORDER_R;
		my = gw->g.y+gw->g.height - FRM_BORDER_B;
		for(y = gw->g.y+FRM_BORDER_T, ih = gi->height; y < my; y += ih) {
			if (ih > my - y)
				ih = my - y;
			for(x = gw->g.x+FRM_BORDER_L, iw = gi->width; x < mx; x += iw) {
				if (iw > mx - x)
					iw = mx - x;
				gdispGImageDraw(gw->g.display, gi, x, y, iw, ih, 0, 0);
			}
		}

		#undef gi
	}
#endif

#endif  /* (GFX_USE_GWIN && GWIN_NEED_FRAME) || defined(__DOXYGEN__) */
