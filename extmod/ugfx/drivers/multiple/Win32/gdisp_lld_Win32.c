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

#define GDISP_DRIVER_VMT			GDISPVMT_Win32
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

// Configuration parameters for this driver
#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH	640
#endif
#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT	480
#endif
#ifndef GDISP_WIN32_USE_INDIRECT_UPDATE
	/**
	 * Setting this to TRUE delays updating the screen
	 * to the windows paint routine. Due to the
	 * drawing lock this does not add as much speed
	 * as might be expected but it is still faster in
	 * all tested circumstances and for all operations
	 * even draw_pixel().
	 * This is probably due to drawing operations being
	 * combined as the update regions are merged.
	 * The only time you might want to turn this off is
	 * if you are debugging drawing and want to see each
	 * pixel as it is set.
	 */
	#define GDISP_WIN32_USE_INDIRECT_UPDATE		TRUE
#endif
#ifndef GKEYBOARD_WIN32_NO_LAYOUT
	/**
	 * Setting this to TRUE turns off the layout engine.
	 * In this situation "cooked" characters are returned but
	 * shift states etc are lost.
	 * As only a limited number of keyboard layouts are currently
	 * defined for Win32 in uGFX (currently only US English), setting this
	 * to TRUE enables the windows keyboard mapping to be pass non-English
	 * characters to uGFX or to handle non-standard keyboard layouts at
	 * the expense of losing special function keys etc.
	 */
	#define GKEYBOARD_WIN32_NO_LAYOUT			FALSE
#endif
#ifndef GKEYBOARD_WIN32_DEFAULT_LAYOUT
	#define GKEYBOARD_WIN32_DEFAULT_LAYOUT		KeyboardLayout_Win32_US
#endif

// How far extra windows (multiple displays) should be offset from the first.
#define DISPLAY_X_OFFSET		50
#define DISPLAY_Y_OFFSET		50

// Oops - name clashes with Win32 symbols
#undef Red
#undef Green
#undef Blue

#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <wingdi.h>
#include <assert.h>

#define GDISP_FLG_READY				(GDISP_FLG_DRIVER<<0)
#define GDISP_FLG_HASTOGGLE			(GDISP_FLG_DRIVER<<1)
#if GDISP_HARDWARE_STREAM_WRITE || GDISP_HARDWARE_STREAM_READ
	#define GDISP_FLG_WSTREAM			(GDISP_FLG_DRIVER<<3)
	#define GDISP_FLG_WRAPPED			(GDISP_FLG_DRIVER<<4)
#endif

#if GINPUT_NEED_TOGGLE
	/* Include toggle support code */
	#include "../../../src/ginput/ginput_driver_toggle.h"

	// Hack until toggle use gdriver.
	static GDisplay *toggleWindow;
#endif

#if GINPUT_NEED_MOUSE
	// Include mouse support code
	#define GMOUSE_DRIVER_VMT		GMOUSEVMT_Win32
	#include "../../../src/ginput/ginput_driver_mouse.h"

	// Forward definitions
	static bool_t Win32MouseInit(GMouse *m, unsigned driverinstance);
	static bool_t Win32MouseRead(GMouse *m, GMouseReading *prd);

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
		Win32MouseInit,	// init
		0,				// deinit
		Win32MouseRead,	// get
		0,				// calsave
		0				// calload
	}};
#endif

#if GINPUT_NEED_KEYBOARD
	#define GKEYBOARD_DRIVER_VMT		GKEYBOARDVMT_Win32
	#include "../../../src/ginput/ginput_driver_keyboard.h"

	#if !GKEYBOARD_WIN32_NO_LAYOUT
		#if GKEYBOARD_LAYOUT_OFF
			#error "The Win32 keyboard driver is using the layout engine. Please set GKEYBOARD_LAYOUT_OFF=FALSE or GKEYBOARD_WIN32_NO_LAYOUT=TRUE."
		#endif

		#include "../../../src/ginput/ginput_keyboard_microcode.h"

		// Forward definitions
		extern uint8_t	GKEYBOARD_WIN32_DEFAULT_LAYOUT[];

		// This is the layout code for the English US keyboard.
		//	We make it public so that a user can switch to a different layout if required.
		uint8_t	KeyboardLayout_Win32_US[] = {
			KMC_HEADERSTART, KMC_HEADER_ID1, KMC_HEADER_ID2, KMC_HEADER_VER_1,

			// Transient Shifters: SHIFT, CTRL, ALT, WINKEY
			/*  1 */KMC_RECORDSTART, 9,													// SHIFT (left & Right)
				KMC_TEST_CODETABLE, 2, VK_SHIFT, VK_LSHIFT,
				KMC_TEST_LASTCODE, 0x00,
				KMC_ACT_STATEBIT, GKEYSTATE_SHIFT_L_BIT|KMC_BIT_CLEAR,
				KMC_ACT_DONE,
			/*  2 */KMC_RECORDSTART, 9,
				KMC_TEST_CODETABLE, 2, VK_SHIFT, VK_LSHIFT,
				KMC_TEST_STATEBIT, GKEYSTATE_SHIFT_L_BIT|KMC_BIT_CLEAR,
				KMC_ACT_STATEBIT, GKEYSTATE_SHIFT_L_BIT,
				KMC_ACT_DONE,
			/*  3 */KMC_RECORDSTART, 7,
				KMC_TEST_CODE, VK_RSHIFT,
				KMC_TEST_LASTCODE, 0x00,
				KMC_ACT_STATEBIT, GKEYSTATE_SHIFT_R_BIT|KMC_BIT_CLEAR,
				KMC_ACT_DONE,
			/*  4 */KMC_RECORDSTART, 7,
				KMC_TEST_CODE, VK_RSHIFT,
				KMC_TEST_STATEBIT, GKEYSTATE_SHIFT_R_BIT|KMC_BIT_CLEAR,
				KMC_ACT_STATEBIT, GKEYSTATE_SHIFT_R_BIT,
				KMC_ACT_DONE,
			/*  5 */KMC_RECORDSTART, 9,													// CONTROL (left & Right)
				KMC_TEST_CODETABLE, 2, VK_CONTROL, VK_LCONTROL,
				KMC_TEST_LASTCODE, 0x00,
				KMC_ACT_STATEBIT, GKEYSTATE_CTRL_L_BIT|KMC_BIT_CLEAR,
				KMC_ACT_DONE,
			/*  6 */KMC_RECORDSTART, 9,
				KMC_TEST_CODETABLE, 2, VK_CONTROL, VK_LCONTROL,
				KMC_TEST_STATEBIT, GKEYSTATE_CTRL_L_BIT|KMC_BIT_CLEAR,
				KMC_ACT_STATEBIT, GKEYSTATE_CTRL_L_BIT,
				KMC_ACT_DONE,
			/*  7 */KMC_RECORDSTART, 7,
				KMC_TEST_CODE, VK_RCONTROL,
				KMC_TEST_LASTCODE, 0x00,
				KMC_ACT_STATEBIT, GKEYSTATE_CTRL_R_BIT|KMC_BIT_CLEAR,
				KMC_ACT_DONE,
			/*  8 */KMC_RECORDSTART, 7,
				KMC_TEST_CODE, VK_RCONTROL,
				KMC_TEST_STATEBIT, GKEYSTATE_CTRL_R_BIT|KMC_BIT_CLEAR,
				KMC_ACT_STATEBIT, GKEYSTATE_CTRL_R_BIT,
				KMC_ACT_DONE,
			/*  9 */KMC_RECORDSTART, 9,													// ALT (left & Right)
				KMC_TEST_CODETABLE, 2, VK_MENU, VK_LMENU,
				KMC_TEST_LASTCODE, 0x00,
				KMC_ACT_STATEBIT, GKEYSTATE_ALT_L_BIT|KMC_BIT_CLEAR,
				KMC_ACT_DONE,
			/* 10 */KMC_RECORDSTART, 9,
				KMC_TEST_CODETABLE, 2, VK_MENU, VK_LMENU,
				KMC_TEST_STATEBIT, GKEYSTATE_ALT_L_BIT|KMC_BIT_CLEAR,
				KMC_ACT_STATEBIT, GKEYSTATE_ALT_L_BIT,
				KMC_ACT_DONE,
			/* 11 */KMC_RECORDSTART, 7,
				KMC_TEST_CODE, VK_RMENU,
				KMC_TEST_LASTCODE, 0x00,
				KMC_ACT_STATEBIT, GKEYSTATE_ALT_R_BIT|KMC_BIT_CLEAR,
				KMC_ACT_DONE,
			/* 12 */KMC_RECORDSTART, 7,
				KMC_TEST_CODE, VK_RMENU,
				KMC_TEST_STATEBIT, GKEYSTATE_ALT_R_BIT|KMC_BIT_CLEAR,
				KMC_ACT_STATEBIT, GKEYSTATE_ALT_R_BIT,
				KMC_ACT_DONE,
			/* 13 */KMC_RECORDSTART, 9,													// WinKey (left or right)
				KMC_TEST_CODETABLE, 2, VK_LWIN, VK_RWIN,
				KMC_TEST_LASTCODE, 0x00,
				KMC_ACT_STATEBIT, GKEYSTATE_WINKEY_BIT|KMC_BIT_CLEAR,
				KMC_ACT_DONE,
			/* 14 */KMC_RECORDSTART, 9,
				KMC_TEST_CODETABLE, 2, VK_LWIN, VK_RWIN,
				KMC_TEST_STATEBIT, GKEYSTATE_WINKEY_BIT|KMC_BIT_CLEAR,
				KMC_ACT_STATEBIT, GKEYSTATE_WINKEY_BIT,
				KMC_ACT_DONE,

			// Locking Shifters: CAPSLOCK, NUMLOCK and SCROLLLOCK
			/* 15 */KMC_RECORDSTART, 7,													// CAPSLOCK (keyup only)
				KMC_TEST_CODE, VK_CAPITAL,
				KMC_TEST_LASTCODE, 0x00,
				KMC_ACT_STATEBIT, GKEYSTATE_CAPSLOCK_BIT|KMC_BIT_INVERT,
				KMC_ACT_DONE,
			/* 16 */KMC_RECORDSTART, 7,													// NUMLOCK (keyup only)
				KMC_TEST_CODE, VK_NUMLOCK,
				KMC_TEST_LASTCODE, 0x00,
				KMC_ACT_STATEBIT, GKEYSTATE_NUMLOCK_BIT|KMC_BIT_INVERT,
				KMC_ACT_DONE,
			/* 17 */KMC_RECORDSTART, 7,													// SCROLLLOCK (keyup only)
				KMC_TEST_CODE, VK_SCROLL,
				KMC_TEST_LASTCODE, 0x00,
				KMC_ACT_STATEBIT, GKEYSTATE_SCROLLLOCK_BIT|KMC_BIT_INVERT,
				KMC_ACT_DONE,

			// Keyup, Repeat
			/* 18 */KMC_RECORDSTART, 18,												// Clear any shifter keys that got through
				KMC_TEST_CODETABLE, 14, VK_SHIFT, VK_LSHIFT, VK_RSHIFT,
										VK_CONTROL, VK_LCONTROL, VK_RCONTROL,
										VK_MENU, VK_LMENU, VK_RMENU,
										VK_LWIN, VK_RWIN,
										VK_CAPITAL, VK_NUMLOCK, VK_SCROLL,
				KMC_ACT_RESET,
				KMC_ACT_STOP,
			/* 19 */KMC_RECORDSTART, 4,													// Skip special codes 0x00 (Keyup) & 0x01 (Repeat)
				KMC_TEST_CODERANGE, 0x00, 0x01,
				KMC_ACT_STOP,
			/* 20 */KMC_RECORDSTART, 6,													// Keyup
				KMC_ACT_STATEBIT, GKEYSTATE_KEYUP_BIT|KMC_BIT_CLEAR,
				KMC_TEST_LASTCODE, 0x00,
				KMC_ACT_STATEBIT, GKEYSTATE_KEYUP_BIT,
			/* 21 */KMC_RECORDSTART, 6,													// Repeat
				KMC_ACT_STATEBIT, GKEYSTATE_REPEAT_BIT|KMC_BIT_CLEAR,
				KMC_TEST_LASTCODE, 0x01,
				KMC_ACT_STATEBIT, GKEYSTATE_REPEAT_BIT,

			// 0 - 9
			/* 22 */KMC_RECORDSTART, 7,													// Alt 0-9
				KMC_TEST_ALT,
				KMC_TEST_CODERANGE, '0', '9',
				KMC_ACT_CHARADD,  10,
				KMC_ACT_STOP,
			/* 23 */KMC_RECORDSTART, 17,												// Shifted 0-9
				KMC_TEST_SHIFT,
				KMC_TEST_CODERANGE, '0', '9',
				KMC_ACT_CHARTABLE,  10, ')', '!', '@', '#', '$', '%', '^', '&', '*', '(',
				KMC_ACT_DONE,
			/* 24 */KMC_RECORDSTART, 5,													// 0 - 9
				KMC_TEST_CODERANGE, '0', '9',
				KMC_ACT_CHARCODE,
				KMC_ACT_DONE,

			// A - Z
			/* 25 */KMC_RECORDSTART, 7,													// Control A-Z
				KMC_TEST_CTRL,
				KMC_TEST_CODERANGE, 'A', 'Z',
				KMC_ACT_CHARRANGE, 1,
				KMC_ACT_DONE,
			/* 26 */KMC_RECORDSTART, 7,													// No Caps A-Z
				KMC_TEST_NOCAPS,
				KMC_TEST_CODERANGE, 'A', 'Z',
				KMC_ACT_CHARRANGE, 'a',
				KMC_ACT_DONE,
			/* 27 */KMC_RECORDSTART, 5,													// Caps A-Z
				KMC_TEST_CODERANGE, 'A', 'Z',
				KMC_ACT_CHARCODE,
				KMC_ACT_DONE,

			// Number pad
			/* 28 */KMC_RECORDSTART, 7,													// Alt Number pad
				KMC_TEST_ALT,
				KMC_TEST_CODERANGE, VK_NUMPAD0, VK_NUMPAD9,
				KMC_ACT_CHARADD,  10,
				KMC_ACT_STOP,
			/* 29 */KMC_RECORDSTART, 5,
				KMC_TEST_ALT,
				KMC_TEST_CODERANGE, VK_MULTIPLY, VK_DIVIDE,
				KMC_ACT_STOP,
			/* 30 */KMC_RECORDSTART, 7,													// Number pad with Numlock
				KMC_TEST_NUMLOCK,
				KMC_TEST_CODERANGE, VK_NUMPAD0, VK_NUMPAD9,
				KMC_ACT_CHARRANGE, '0',
				KMC_ACT_DONE,
			/* 31 */KMC_RECORDSTART, 13,
				KMC_TEST_NUMLOCK,
				KMC_TEST_CODERANGE, VK_MULTIPLY, VK_DIVIDE,
				KMC_ACT_CHARTABLE, 6, '*', '+', GKEY_ENTER, '-', '.', '/',
				KMC_ACT_DONE,
			/* 32 */KMC_RECORDSTART, 4,													// Number pad with no Numlock
				KMC_TEST_CODE, VK_NUMPAD5,
				KMC_ACT_RESET,
				KMC_ACT_STOP,
			/* 33 */KMC_RECORDSTART, 12,
				KMC_TEST_CODERANGE, VK_MULTIPLY, VK_DIVIDE,
				KMC_ACT_CHARTABLE, 6, '*', '+', GKEY_ENTER, '-', GKEY_DEL, '/',
				KMC_ACT_DONE,
			/* 34 */KMC_RECORDSTART, 18,
				KMC_TEST_CODERANGE, VK_NUMPAD0, VK_NUMPAD9,
				KMC_ACT_STATEBIT, GKEYSTATE_SPECIAL_BIT,
				KMC_ACT_CHARTABLE, 10, GKEY_INSERT, GKEY_END, GKEY_DOWN, GKEY_PAGEDOWN, GKEY_LEFT, '5', GKEY_RIGHT, GKEY_HOME, GKEY_UP, GKEY_PAGEUP,
				KMC_ACT_DONE,

			// Symbols
			/* 35 */KMC_RECORDSTART, 14,												// Shifted Symbols
				KMC_TEST_SHIFT,
				KMC_TEST_CODERANGE, VK_OEM_1, VK_OEM_3,
				KMC_ACT_CHARTABLE, 7, ':', '+', '<', '_', '>', '?', '~',
				KMC_ACT_DONE,
			/* 36 */KMC_RECORDSTART, 11,
				KMC_TEST_SHIFT,
				KMC_TEST_CODERANGE, VK_OEM_4, VK_OEM_7,
				KMC_ACT_CHARTABLE, 4, '{', '|', '}', '"',
				KMC_ACT_DONE,
			/* 37 */KMC_RECORDSTART, 13,												// Non-shifted Symbols
				KMC_TEST_CODERANGE, VK_OEM_1, VK_OEM_3,
				KMC_ACT_CHARTABLE, 7, ';', '=', ',', '-', '.', '/', '`',
				KMC_ACT_DONE,
			/* 38 */KMC_RECORDSTART, 10,
				KMC_TEST_CODERANGE, VK_OEM_4, VK_OEM_7,
				KMC_ACT_CHARTABLE, 4, '[', '\\', ']', '\'',
				KMC_ACT_DONE,

			// Special Keys
			// Extra special keys like Media and Browser keys are still to be implemented.
			/* 39 */KMC_RECORDSTART, 17,												// Normal Control Type Keys
				KMC_TEST_CODETABLE, 6, VK_BACK, VK_TAB, VK_RETURN, VK_ESCAPE, VK_SPACE, VK_DELETE,
				KMC_ACT_CHARTABLE,  6, GKEY_BACKSPACE, GKEY_TAB, GKEY_ENTER, GKEY_ESC, GKEY_SPACE, GKEY_DEL,
				KMC_ACT_DONE,
			/* 40 */KMC_RECORDSTART, 35,												// Special Keys
				KMC_TEST_CODETABLE, 14, VK_PRIOR, VK_NEXT,
										VK_HOME, VK_END,
										VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN,
										VK_INSERT,
										VK_SNAPSHOT, VK_SLEEP, VK_PAUSE, VK_CANCEL,
										VK_APPS,
				KMC_ACT_STATEBIT, GKEYSTATE_SPECIAL_BIT,
				KMC_ACT_CHARTABLE,  14, GKEY_PAGEUP, GKEY_PAGEDOWN,
										GKEY_HOME, GKEY_END,
										GKEY_LEFT, GKEY_RIGHT, GKEY_UP, GKEY_DOWN,
										GKEY_INSERT,
										GKEY_PRINTSCREEN, GKEY_SLEEP, GKEY_CTRLPAUSE, GKEY_CTRLBREAK,
										GKEY_RIGHTCLICKKEY,
				KMC_ACT_DONE,
			/* 41 */KMC_RECORDSTART, 8,													// F1 .. F15
				KMC_TEST_CODERANGE, VK_F1, VK_F15,
				KMC_ACT_STATEBIT, GKEYSTATE_SPECIAL_BIT,
				KMC_ACT_CHARRANGE, GKEY_FN1,
				KMC_ACT_DONE,

			// Anything else
			/* 40 */KMC_RECORDSTART, 1,													// Just send the scan code to the user
				KMC_ACT_DONE,

			// EOF
			KMC_RECORDSTART, 0
		};
	#elif !GKEYBOARD_LAYOUT_OFF
		#warning "The WIN32 keyboard driver is not using the layout engine. If no other keyboard is using it consider defining GKEYBOARD_LAYOUT_OFF=TRUE to save code size."
	#endif

	// Forward definitions
	static bool_t Win32KeyboardInit(GKeyboard *k, unsigned driverinstance);
	static int Win32KeyboardGetData(GKeyboard *k, uint8_t *pch, int sz);

	const GKeyboardVMT const GKEYBOARD_DRIVER_VMT[1] = {{
		{
			GDRIVER_TYPE_KEYBOARD,
			GKEYBOARD_VFLG_NOPOLL,			//  GKEYBOARD_VFLG_DYNAMICONLY
			sizeof(GKeyboard),
			_gkeyboardInitDriver, _gkeyboardPostInitDriver, _gkeyboardDeInitDriver
		},

	 	// The Win32 keyboard layout
		#if GKEYBOARD_WIN32_NO_LAYOUT
			0,
		#else
			GKEYBOARD_WIN32_DEFAULT_LAYOUT,
		#endif

		Win32KeyboardInit,		// init
		0,						// deinit
		Win32KeyboardGetData,	// getdata
		0						// putdata		void	(*putdata)(GKeyboard *k, char ch);		Optional
	}};

	static int			keypos;
	static uint8_t		keybuffer[8];
	static GKeyboard	*keyboard;
#endif

static DWORD			winThreadId;
static volatile bool_t	QReady;
static HANDLE			drawMutex;

/*===========================================================================*/
/* Driver local routines    .                                                */
/*===========================================================================*/

#if GINPUT_NEED_TOGGLE
	#define WIN32_BUTTON_AREA		16
#else
	#define WIN32_BUTTON_AREA		0
#endif

#define APP_NAME "uGFX"

typedef struct winPriv {
	HWND			hwnd;
	HDC				dcBuffer;
	HBITMAP			dcBitmap;
	HBITMAP 		dcOldBitmap;
	#if GINPUT_NEED_MOUSE
		coord_t		mousex, mousey;
		uint16_t	mousebuttons;
		GMouse		*mouse;
	#endif
	#if GINPUT_NEED_TOGGLE
		uint8_t		toggles;
	#endif
	#if GDISP_HARDWARE_STREAM_WRITE || GDISP_HARDWARE_STREAM_READ
		coord_t		x0, y0, x1, y1;
		coord_t		x, y;
	#endif
} winPriv;


static LRESULT myWindowProc(HWND hWnd,	UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HDC				dc;
	PAINTSTRUCT		ps;
	GDisplay *		g;
	winPriv *		priv;
	#if GINPUT_NEED_TOGGLE
		HBRUSH		hbrOn, hbrOff;
		HPEN		pen;
		RECT		rect;
		HGDIOBJ		old;
		POINT 		p;
		coord_t		pos;
		uint8_t		bit;
	#endif

	switch (Msg) {
	case WM_CREATE:
		// Get our GDisplay structure and attach it to the window
		g = (GDisplay *)((LPCREATESTRUCT)lParam)->lpCreateParams;
		priv = (winPriv *)g->priv;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)g);

		// Fill in the private area
		priv->hwnd = hWnd;
		dc = GetDC(hWnd);
		priv->dcBitmap = CreateCompatibleBitmap(dc, g->g.Width, g->g.Height);
		priv->dcBuffer = CreateCompatibleDC(dc);
		ReleaseDC(hWnd, dc);
		priv->dcOldBitmap = SelectObject(priv->dcBuffer, priv->dcBitmap);

		// Mark the window as ready to go
		g->flags |= GDISP_FLG_READY;
		break;

	#if GINPUT_NEED_MOUSE || GINPUT_NEED_TOGGLE
		case WM_LBUTTONDOWN:
			// Get our GDisplay structure
			g = (GDisplay *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			priv = (winPriv *)g->priv;

			// Handle mouse down on the window
			#if GINPUT_NEED_MOUSE
				if ((coord_t)HIWORD(lParam) < GDISP_SCREEN_HEIGHT) {
					priv->mousebuttons |= GINPUT_MOUSE_BTN_LEFT;
					goto mousemove;
				}
			#endif

			// Handle mouse down on the toggle area
			#if GINPUT_NEED_TOGGLE
				if ((coord_t)HIWORD(lParam) >= GDISP_SCREEN_HEIGHT && (g->flags & GDISP_FLG_HASTOGGLE)) {
					bit = 1 << ((coord_t)LOWORD(lParam)*8/g->g.Width);
					priv->toggles ^= bit;
					rect.left = 0;
					rect.right = GDISP_SCREEN_WIDTH;
					rect.top = GDISP_SCREEN_HEIGHT;
					rect.bottom = GDISP_SCREEN_HEIGHT + WIN32_BUTTON_AREA;
					InvalidateRect(hWnd, &rect, FALSE);
					UpdateWindow(hWnd);
					#if GINPUT_TOGGLE_POLL_PERIOD == TIME_INFINITE
						ginputToggleWakeup();
					#endif
				}
			#endif
			break;

		case WM_LBUTTONUP:
			// Get our GDisplay structure
			g = (GDisplay *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			priv = (winPriv *)g->priv;

			// Handle mouse up on the toggle area
			#if GINPUT_NEED_TOGGLE
				if ((g->flags & GDISP_FLG_HASTOGGLE)) {
					if ((priv->toggles & 0x0F)) {
						priv->toggles &= ~0x0F;
						rect.left = 0;
						rect.right = GDISP_SCREEN_WIDTH;
						rect.top = GDISP_SCREEN_HEIGHT;
						rect.bottom = GDISP_SCREEN_HEIGHT + WIN32_BUTTON_AREA;
						InvalidateRect(hWnd, &rect, FALSE);
						UpdateWindow(hWnd);
						#if GINPUT_TOGGLE_POLL_PERIOD == TIME_INFINITE
							ginputToggleWakeup();
						#endif
					}
				}
			#endif

			// Handle mouse up on the window
			#if GINPUT_NEED_MOUSE
				if ((coord_t)HIWORD(lParam) < GDISP_SCREEN_HEIGHT) {
					priv->mousebuttons &= ~GINPUT_MOUSE_BTN_LEFT;
					goto mousemove;
				}
			#endif
			break;
	#endif

	#if GINPUT_NEED_MOUSE
		case WM_MBUTTONDOWN:
			g = (GDisplay *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			priv = (winPriv *)g->priv;
			if ((coord_t)HIWORD(lParam) < GDISP_SCREEN_HEIGHT) {
				priv->mousebuttons |= GINPUT_MOUSE_BTN_MIDDLE;
				goto mousemove;
			}
			break;
		case WM_MBUTTONUP:
			g = (GDisplay *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			priv = (winPriv *)g->priv;
			if ((coord_t)HIWORD(lParam) < GDISP_SCREEN_HEIGHT) {
				priv->mousebuttons &= ~GINPUT_MOUSE_BTN_MIDDLE;
				goto mousemove;
			}
			break;
		case WM_RBUTTONDOWN:
			g = (GDisplay *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			priv = (winPriv *)g->priv;
			if ((coord_t)HIWORD(lParam) < GDISP_SCREEN_HEIGHT) {
				priv->mousebuttons |= GINPUT_MOUSE_BTN_RIGHT;
				goto mousemove;
			}
			break;
		case WM_RBUTTONUP:
			g = (GDisplay *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			priv = (winPriv *)g->priv;
			if ((coord_t)HIWORD(lParam) < GDISP_SCREEN_HEIGHT) {
				priv->mousebuttons &= ~GINPUT_MOUSE_BTN_RIGHT;
				goto mousemove;
			}
			break;
		case WM_MOUSEMOVE:
			g = (GDisplay *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			priv = (winPriv *)g->priv;
			if ((coord_t)HIWORD(lParam) >= GDISP_SCREEN_HEIGHT)
				break;
		mousemove:
			priv->mousex = (coord_t)LOWORD(lParam);
			priv->mousey = (coord_t)HIWORD(lParam);
			if ((gmvmt(priv->mouse)->d.flags & GMOUSE_VFLG_NOPOLL))		// For normal setup this is always TRUE
				_gmouseWakeup(priv->mouse);
			break;
	#endif

	#if GINPUT_NEED_KEYBOARD
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
			// A layout is being used: Send scan codes to the keyboard buffer
			if (keyboard && keyboard->pLayout && keypos < (int)sizeof(keybuffer)-1 && (wParam & 0xFF) > 0x01) {
				if (Msg == WM_KEYUP || Msg == WM_SYSKEYUP)
					keybuffer[keypos++] = 0x00;			// Keyup
				else if (HIWORD(lParam) & KF_REPEAT)
					keybuffer[keypos++] = 0x01;			// Repeat
				keybuffer[keypos++] = wParam;
				if ((gkvmt(keyboard)->d.flags & GKEYBOARD_VFLG_NOPOLL))		// For normal setup this is always TRUE
					_gkeyboardWakeup(keyboard);
			}
			return 0;
		case WM_CHAR:
			// A layout is not being used: Send character codes to the keyboard buffer
			if (keyboard && !keyboard->pLayout && keypos < (int)sizeof(keybuffer)) {
				wchar_t	w;
				int		len;

				// Convert from a UTF16 character to a UTF8 string.
				w = wParam;
				len = WideCharToMultiByte(CP_UTF8, 0, &w, 1, (char *)(keybuffer+keypos), sizeof(keybuffer)-keypos, 0, 0);
				keypos += len;
				if (len && (gkvmt(keyboard)->d.flags & GKEYBOARD_VFLG_NOPOLL))		// For normal setup this is always TRUE
					_gkeyboardWakeup(keyboard);
			}
			return 0;
		/*
		case WM_DEADCHAR:
		case WM_SYSCHAR:
		case WM_SYSDEADCHAR:
			break;
		*/
	#endif

	case WM_ERASEBKGND:
		// Pretend we have erased the background.
		// We know we don't really need to do this as we
		// redraw the entire surface in the WM_PAINT handler.
		return TRUE;

	case WM_PAINT:
		// Get our GDisplay structure
		g = (GDisplay *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		priv = (winPriv *)g->priv;

		// Paint the main window area
		WaitForSingleObject(drawMutex, INFINITE);
		dc = BeginPaint(hWnd, &ps);
		BitBlt(dc, ps.rcPaint.left, ps.rcPaint.top,
			ps.rcPaint.right - ps.rcPaint.left,
			(ps.rcPaint.bottom > GDISP_SCREEN_HEIGHT ? GDISP_SCREEN_HEIGHT : ps.rcPaint.bottom) - ps.rcPaint.top,
			priv->dcBuffer, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

		// Paint the toggle area
		#if GINPUT_NEED_TOGGLE
			if (ps.rcPaint.bottom >= GDISP_SCREEN_HEIGHT && (g->flags & GDISP_FLG_HASTOGGLE)) {
				pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
				hbrOn = CreateSolidBrush(RGB(0, 0, 255));
				hbrOff = CreateSolidBrush(RGB(128, 128, 128));
				old = SelectObject(dc, pen);
				MoveToEx(dc, 0, GDISP_SCREEN_HEIGHT, &p);
				LineTo(dc, GDISP_SCREEN_WIDTH, GDISP_SCREEN_HEIGHT);
				for(pos = 0, bit=1; pos < GDISP_SCREEN_WIDTH; pos=rect.right, bit <<= 1) {
					rect.left = pos;
					rect.right = pos + GDISP_SCREEN_WIDTH/8;
					rect.top = GDISP_SCREEN_HEIGHT;
					rect.bottom = GDISP_SCREEN_HEIGHT + WIN32_BUTTON_AREA;
					FillRect(dc, &rect, (priv->toggles & bit) ? hbrOn : hbrOff);
					if (pos > 0) {
						MoveToEx(dc, rect.left, rect.top, &p);
						LineTo(dc, rect.left, rect.bottom);
					}
				}
				DeleteObject(hbrOn);
				DeleteObject(hbrOff);
				SelectObject(dc, old);
			}
		#endif
		EndPaint(hWnd, &ps);
		ReleaseMutex(drawMutex);
		break;

	case WM_DESTROY:
		// Get our GDisplay structure
		g = (GDisplay *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		priv = (winPriv *)g->priv;

		// Restore the window and free our bitmaps
		SelectObject(priv->dcBuffer, priv->dcOldBitmap);
		DeleteDC(priv->dcBuffer);
		DeleteObject(priv->dcBitmap);

		// Cleanup the private area
		gfxFree(priv);

		// Quit the application
		PostQuitMessage(0);

		// Actually the above doesn't work (who knows why)
		ExitProcess(0);
		break;

	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	return 0;
}

static DWORD WINAPI WindowThread(void *param) {
	(void)param;
	MSG msg;

	// Establish this thread as a message queue thread
	winThreadId = GetCurrentThreadId();
	PeekMessage(&msg, 0, WM_USER, WM_USER, PM_NOREMOVE);
	QReady = TRUE;

	// Create the window class
	{
		WNDCLASS		wc;
		ATOM			winClass;

		wc.style           = CS_HREDRAW | CS_VREDRAW; // | CS_OWNDC;
		wc.lpfnWndProc     = (WNDPROC)myWindowProc;
		wc.cbClsExtra      = 0;
		wc.cbWndExtra      = 0;
		wc.hInstance       = GetModuleHandle(0);
		wc.hIcon           = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor         = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground   = GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName    = 0;
		wc.lpszClassName   = APP_NAME;
		winClass = RegisterClass(&wc);
		assert(winClass != 0);
	}

	do {
		// This is a high priority task - make sure other tasks get a go.
		Sleep(1);

		while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			// Is this our special thread message to create a new window?
			if (!msg.hwnd && msg.message == WM_USER) {
				RECT		rect;
				GDisplay	*g;

				g = (GDisplay *)msg.lParam;

				// Set the window rectangle
				rect.top = 0; rect.bottom = g->g.Height;
				rect.left = 0; rect.right = g->g.Width;
				#if GINPUT_NEED_TOGGLE
					if ((g->flags & GDISP_FLG_HASTOGGLE))
						rect.bottom += WIN32_BUTTON_AREA;
				#endif
				AdjustWindowRect(&rect, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU, 0);

				// Create the window
				msg.hwnd = CreateWindow(APP_NAME, "", WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_BORDER, msg.wParam*DISPLAY_X_OFFSET, msg.wParam*DISPLAY_Y_OFFSET,
								rect.right-rect.left, rect.bottom-rect.top, 0, 0,
								GetModuleHandle(0), g);
				assert(msg.hwnd != 0);

			// Or just a normal window message
			} else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	} while (msg.message != WM_QUIT);
	ExitProcess(0);
	return msg.wParam;
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

LLDSPEC bool_t gdisp_lld_init(GDisplay *g) {
	winPriv	*	priv;
	char		buf[132];

	// Initialise the window thread and the window class (if it hasn't been done already)
	if (!QReady) {
		HANDLE			hth;

		// Create the draw mutex
		drawMutex = CreateMutex(0, FALSE, 0);

		// Create the thread
		if (!(hth = CreateThread(0, 0, WindowThread, 0, CREATE_SUSPENDED, 0)))
			return FALSE;
		SetThreadPriority(hth, THREAD_PRIORITY_ABOVE_NORMAL);
		ResumeThread(hth);
		CloseHandle(hth);

		// Wait for our thread to be ready
		while (!QReady)
			Sleep(1);
	}

	// Initialise the GDISP structure
	g->g.Orientation = GDISP_ROTATE_0;
	g->g.Powermode = powerOn;
	g->g.Backlight = 100;
	g->g.Contrast = 50;
	g->g.Width = GDISP_SCREEN_WIDTH;
	g->g.Height = GDISP_SCREEN_HEIGHT;

	// Turn on toggles for the first GINPUT_TOGGLE_CONFIG_ENTRIES windows
	#if GINPUT_NEED_TOGGLE
		if (g->controllerdisplay < GINPUT_TOGGLE_CONFIG_ENTRIES) {
			g->flags |= GDISP_FLG_HASTOGGLE;
			toggleWindow = g;
		}
	#endif

	// Create a private area for this window
	priv = gfxAlloc(sizeof(winPriv));
	assert(priv != 0);
	memset(priv, 0, sizeof(winPriv));
	g->priv = priv;
	#if GDISP_HARDWARE_STREAM_WRITE || GDISP_HARDWARE_STREAM_READ
		// Initialise with an invalid window
		g->flags &= ~GDISP_FLG_WSTREAM;
	#endif
	g->board = 0;			// no board interface for this controller

	// Create the window in the message thread
	PostThreadMessage(winThreadId, WM_USER, (WPARAM)g->controllerdisplay, (LPARAM)g);

	// Wait for the window creation to complete (for safety)
	while(!(((volatile GDisplay *)g)->flags & GDISP_FLG_READY))
		Sleep(1);

	// Create the associated mouse
	#if GINPUT_NEED_MOUSE
		priv->mouse = (GMouse *)gdriverRegister((const GDriverVMT const *)GMOUSE_DRIVER_VMT, g);
	#endif

	sprintf(buf, APP_NAME " - %u", g->systemdisplay+1);
	SetWindowText(priv->hwnd, buf);
	ShowWindow(priv->hwnd, SW_SHOW);
	UpdateWindow(priv->hwnd);

	return TRUE;
}

#if GDISP_HARDWARE_FLUSH
	LLDSPEC void gdisp_lld_flush(GDisplay *g) {
		winPriv	*	priv;

		priv = g->priv;
		UpdateWindow(priv->hwnd);
	}
#endif

#if GDISP_HARDWARE_STREAM_WRITE || GDISP_HARDWARE_STREAM_READ
	void BAD_PARAMETER(const char *msg) {
		fprintf(stderr, "%s\n", msg);
	}
#endif

#if GDISP_HARDWARE_STREAM_WRITE
	LLDSPEC	void gdisp_lld_write_start(GDisplay *g) {
		winPriv	*	priv;

		if (g->flags & GDISP_FLG_WSTREAM)
			BAD_PARAMETER("write_start: already in streaming mode");
		if (g->p.cx <= 0 || g->p.cy <= 0 || g->p.x < 0 || g->p.y < 0 || g->p.x+g->p.cx > g->g.Width || g->p.y+g->p.cy > g->g.Height)
			BAD_PARAMETER("write_start: bad window parameter");

		priv = g->priv;
		priv->x0 = g->p.x;	priv->x1 = g->p.x + g->p.cx - 1;
		priv->y0 = g->p.y;	priv->y1 = g->p.y + g->p.cy - 1;
		#if GDISP_HARDWARE_STREAM_POS
			priv->x = g->p.x-1;			// Make sure these values are invalid (for testing)
			priv->y = g->p.y-1;
		#else
			priv->x = g->p.x;
			priv->y = g->p.y;
		#endif
		g->flags |= GDISP_FLG_WSTREAM;
		g->flags &= ~GDISP_FLG_WRAPPED;
	}
	LLDSPEC	void gdisp_lld_write_color(GDisplay *g) {
		winPriv	*	priv;
		int			x, y;
		COLORREF	color;

		priv = g->priv;
		color = gdispColor2Native(g->p.color);

		if (!(g->flags & GDISP_FLG_WSTREAM))
			BAD_PARAMETER("write_color: not in streaming mode");
		if (priv->x < priv->x0 || priv->x > priv->x1 || priv->y < priv->y0 || priv->y > priv->y1)
			BAD_PARAMETER("write_color: cursor outside streaming area");
		if (g->flags & GDISP_FLG_WRAPPED) {
			BAD_PARAMETER("write_color: Warning - Area wrapped.");
			g->flags &= ~GDISP_FLG_WRAPPED;
		}

		#if GDISP_NEED_CONTROL
			switch(g->g.Orientation) {
			case GDISP_ROTATE_0:
			default:
				x = priv->x;
				y = priv->y;
				break;
			case GDISP_ROTATE_90:
				x = priv->y;
				y = g->g.Width - 1 - priv->x;
				break;
			case GDISP_ROTATE_180:
				x = g->g.Width - 1 - priv->x;
				y = g->g.Height - 1 - priv->y;
				break;
			case GDISP_ROTATE_270:
				x = g->g.Height - 1 - priv->y;
				y = priv->x;
				break;
			}
		#else
			x = priv->x;
			y = priv->y;
		#endif

		// Draw the pixel on the screen and in the buffer.
		WaitForSingleObject(drawMutex, INFINITE);
		SetPixel(priv->dcBuffer, x, y, color);
		#if GDISP_WIN32_USE_INDIRECT_UPDATE
			ReleaseMutex(drawMutex);
			{
				RECT	r;
				r.left = x; r.right = x+1;
				r.top = y; r.bottom = y+1;
				InvalidateRect(priv->hwnd, &r, FALSE);
			}
		#else
			{
				HDC		dc;
				dc = GetDC(priv->hwnd);
				SetPixel(dc, x, y, color);
				ReleaseDC(priv->hwnd, dc);
				ReleaseMutex(drawMutex);
			}
		#endif

		// Update the cursor
		if (++priv->x > priv->x1) {
			priv->x = priv->x0;
			if (++priv->y > priv->y1) {
				g->flags |= GDISP_FLG_WRAPPED;
				priv->y = priv->y0;
			}
		}
	}
	LLDSPEC	void gdisp_lld_write_stop(GDisplay *g) {
		if (!(g->flags & GDISP_FLG_WSTREAM))
			BAD_PARAMETER("write_stop: not in streaming mode");
		g->flags &= ~GDISP_FLG_WSTREAM;
	}
	#if GDISP_HARDWARE_STREAM_POS
		LLDSPEC void gdisp_lld_write_pos(GDisplay *g) {
			winPriv	*	priv;

			priv = g->priv;

			if (!(g->flags & GDISP_FLG_WSTREAM))
				BAD_PARAMETER("write_pos: not in streaming mode");
			if (g->p.x < priv->x0 || g->p.x > priv->x1 || g->p.y < priv->y0 || g->p.y > priv->y1)
				BAD_PARAMETER("write_color: new cursor outside streaming area");
			priv->x = g->p.x;
			priv->y = g->p.y;
		}
	#endif
#endif

#if GDISP_HARDWARE_STREAM_READ
	LLDSPEC	void gdisp_lld_read_start(GDisplay *g) {
		winPriv	*	priv;

		if (g->flags & GDISP_FLG_WSTREAM)
			BAD_PARAMETER("read_start: already in streaming mode");
		if (g->p.cx <= 0 || g->p.cy <= 0 || g->p.x < 0 || g->p.y < 0 || g->p.x+g->p.cx > g->g.Width || g->p.y+g->p.cy > g->g.Height)
			BAD_PARAMETER("read_start: bad window parameter");

		priv = g->priv;
		priv->x0 = g->p.x;	priv->x1 = g->p.x + g->p.cx - 1;
		priv->y0 = g->p.y;	priv->y1 = g->p.y + g->p.cy - 1;
		priv->x = g->p.x;
		priv->y = g->p.y;
		g->flags |= GDISP_FLG_WSTREAM;
		g->flags &= ~GDISP_FLG_WRAPPED;
	}
	LLDSPEC	color_t gdisp_lld_read_color(GDisplay *g) {
		winPriv	*	priv;
		COLORREF	color;

		priv = g->priv;

		if (!(g->flags & GDISP_FLG_WSTREAM))
			BAD_PARAMETER("read_color: not in streaming mode");
		if (priv->x < priv->x0 || priv->x > priv->x1 || priv->y < priv->y0 || priv->y > priv->y1)
			BAD_PARAMETER("read_color: cursor outside streaming area");
		if (g->flags & GDISP_FLG_WRAPPED) {
			BAD_PARAMETER("read_color: Warning - Area wrapped.");
			g->flags &= ~GDISP_FLG_WRAPPED;
		}

		WaitForSingleObject(drawMutex, INFINITE);
		#if GDISP_NEED_CONTROL
			switch(g->g.Orientation) {
			case GDISP_ROTATE_0:
			default:
				color = GetPixel(priv->dcBuffer, priv->x, priv->y);
				break;
			case GDISP_ROTATE_90:
				color = GetPixel(priv->dcBuffer, priv->y, g->g.Width - 1 - priv->x);
				break;
			case GDISP_ROTATE_180:
				color = GetPixel(priv->dcBuffer, g->g.Width - 1 - priv->x, g->g.Height - 1 - priv->y);
				break;
			case GDISP_ROTATE_270:
				color = GetPixel(priv->dcBuffer, g->g.Height - 1 - priv->y, priv->x);
				break;
			}
		#else
			color = GetPixel(priv->dcBuffer, priv->x, priv->y);
		#endif
		ReleaseMutex(drawMutex);

		// Update the cursor
		if (++priv->x > priv->x1) {
			priv->x = priv->x0;
			if (++priv->y > priv->y1) {
				g->flags |= GDISP_FLG_WRAPPED;
				priv->y = priv->y0;
			}
		}

		return gdispNative2Color(color);
	}
	LLDSPEC	void gdisp_lld_read_stop(GDisplay *g) {
		if (!(g->flags & GDISP_FLG_WSTREAM))
			BAD_PARAMETER("write_stop: not in streaming mode");
		g->flags &= ~GDISP_FLG_WSTREAM;
	}
#endif

#if GDISP_HARDWARE_DRAWPIXEL
	LLDSPEC void gdisp_lld_draw_pixel(GDisplay *g) {
		winPriv	*	priv;
		int			x, y;
		COLORREF	color;

		priv = g->priv;
		color = gdispColor2Native(g->p.color);

		#if GDISP_NEED_CONTROL
			switch(g->g.Orientation) {
			case GDISP_ROTATE_0:
			default:
				x = g->p.x;
				y = g->p.y;
				break;
			case GDISP_ROTATE_90:
				x = g->p.y;
				y = g->g.Width - 1 - g->p.x;
				break;
			case GDISP_ROTATE_180:
				x = g->g.Width - 1 - g->p.x;
				y = g->g.Height - 1 - g->p.y;
				break;
			case GDISP_ROTATE_270:
				x = g->g.Height - 1 - g->p.y;
				y = g->p.x;
				break;
			}
		#else
			x = g->p.x;
			y = g->p.y;
		#endif

		// Draw the pixel on the screen and in the buffer.
		WaitForSingleObject(drawMutex, INFINITE);
		SetPixel(priv->dcBuffer, x, y, color);
		#if GDISP_WIN32_USE_INDIRECT_UPDATE
			ReleaseMutex(drawMutex);
			{
				RECT	r;
				r.left = x; r.right = x+1;
				r.top = y; r.bottom = y+1;
				InvalidateRect(priv->hwnd, &r, FALSE);
			}
		#else
			{
				HDC		dc;
				dc = GetDC(priv->hwnd);
				SetPixel(dc, x, y, color);
				ReleaseDC(priv->hwnd, dc);
				ReleaseMutex(drawMutex);
			}
		#endif
	}
#endif

/* ---- Optional Routines ---- */

#if GDISP_HARDWARE_FILLS
	LLDSPEC void gdisp_lld_fill_area(GDisplay *g) {
		winPriv	*	priv;
		RECT		rect;
		HBRUSH		hbr;
		COLORREF	color;

		priv = g->priv;
		color = gdispColor2Native(g->p.color);
		hbr = CreateSolidBrush(color);

		#if GDISP_NEED_CONTROL
			switch(g->g.Orientation) {
			case GDISP_ROTATE_0:
			default:
				rect.top = g->p.y;
				rect.bottom = rect.top + g->p.cy;
				rect.left = g->p.x;
				rect.right = rect.left + g->p.cx;
				break;
			case GDISP_ROTATE_90:
				rect.bottom = g->g.Width - g->p.x;
				rect.top = rect.bottom - g->p.cx;
				rect.left = g->p.y;
				rect.right = rect.left + g->p.cy;
				break;
			case GDISP_ROTATE_180:
				rect.bottom = g->g.Height - g->p.y;
				rect.top = rect.bottom - g->p.cy;
				rect.right = g->g.Width - g->p.x;
				rect.left = rect.right - g->p.cx;
				break;
			case GDISP_ROTATE_270:
				rect.top = g->p.x;
				rect.bottom = rect.top + g->p.cx;
				rect.right = g->g.Height - g->p.y;
				rect.left = rect.right - g->p.cy;
				break;
			}
		#else
			rect.top = g->p.y;
			rect.bottom = rect.top + g->p.cy;
			rect.left = g->p.x;
			rect.right = rect.left + g->p.cx;
		#endif


		WaitForSingleObject(drawMutex, INFINITE);
		FillRect(priv->dcBuffer, &rect, hbr);
		#if GDISP_WIN32_USE_INDIRECT_UPDATE
			ReleaseMutex(drawMutex);
			InvalidateRect(priv->hwnd, &rect, FALSE);
		#else
			{
				HDC		dc;
				dc = GetDC(priv->hwnd);
				FillRect(dc, &rect, hbr);
				ReleaseDC(priv->hwnd, dc);
				ReleaseMutex(drawMutex);
			}
		#endif

		DeleteObject(hbr);
	}
#endif

#if GDISP_HARDWARE_BITFILLS && GDISP_NEED_CONTROL
	static pixel_t *rotateimg(GDisplay *g, const pixel_t *buffer) {
		pixel_t	*dstbuf;
		pixel_t	*dst;
		const pixel_t	*src;
		size_t	sz;
		coord_t	i, j;

		// Allocate the destination buffer
		sz = (size_t)g->p.cx * (size_t)g->p.cy;
		if (!(dstbuf = (pixel_t *)malloc(sz * sizeof(pixel_t))))
			return 0;

		// Copy the bits we need
		switch(g->g.Orientation) {
		case GDISP_ROTATE_0:
		default:
			return 0;					// not handled as it doesn't need to be.
		case GDISP_ROTATE_90:
			for(src = buffer+g->p.x1, j = 0; j < g->p.cy; j++, src += g->p.x2 - g->p.cx) {
				dst = dstbuf+sz-g->p.cy+j;
				for(i = 0; i < g->p.cx; i++, dst -= g->p.cy)
					*dst = *src++;
			}
			break;
		case GDISP_ROTATE_180:
			for(dst = dstbuf+sz, src = buffer+g->p.x1, j = 0; j < g->p.cy; j++, src += g->p.x2 - g->p.cx)
				for(i = 0; i < g->p.cx; i++)
					*--dst = *src++;
			break;
		case GDISP_ROTATE_270:
			for(src = buffer+g->p.x1, j = 0; j < g->p.cy; j++, src += g->p.x2 - g->p.cx) {
				dst = dstbuf+g->p.cy-j-1;
				for(i = 0; i < g->p.cx; i++, dst += g->p.cy)
					*dst = *src++;
			}
			break;
		}
		return dstbuf;
	}
#endif

#if GDISP_HARDWARE_BITFILLS
	#if COLOR_SYSTEM != GDISP_COLORSYSTEM_TRUECOLOR || COLOR_TYPE_BITS <= 8
		#error "GDISP Win32: This driver's bitblit currently only supports true-color with bit depths > 8 bits."
	#endif

	LLDSPEC void gdisp_lld_blit_area(GDisplay *g) {
		winPriv	*		priv;
		pixel_t	*		buffer;
		RECT			rect;
		BITMAPV4HEADER	bmpInfo;

		// Make everything relative to the start of the line
		priv = g->priv;
		buffer = g->p.ptr;
		buffer += g->p.x2*g->p.y1;

		memset(&bmpInfo, 0, sizeof(bmpInfo));
		bmpInfo.bV4Size = sizeof(bmpInfo);
		bmpInfo.bV4Planes = 1;
		bmpInfo.bV4BitCount = COLOR_TYPE_BITS;
		bmpInfo.bV4AlphaMask = 0;
		bmpInfo.bV4RedMask		= RGB2COLOR(255,0,0);
		bmpInfo.bV4GreenMask	= RGB2COLOR(0,255,0);
		bmpInfo.bV4BlueMask		= RGB2COLOR(0,0,255);
		bmpInfo.bV4V4Compression = BI_BITFIELDS;
		bmpInfo.bV4XPelsPerMeter = 3078;
		bmpInfo.bV4YPelsPerMeter = 3078;
		bmpInfo.bV4ClrUsed = 0;
		bmpInfo.bV4ClrImportant = 0;
		bmpInfo.bV4CSType = 0; //LCS_sRGB;

		#if GDISP_NEED_CONTROL
			switch(g->g.Orientation) {
			case GDISP_ROTATE_0:
			default:
				bmpInfo.bV4SizeImage = (g->p.cy*g->p.x2) * sizeof(pixel_t);
				bmpInfo.bV4Width = g->p.x2;
				bmpInfo.bV4Height = -g->p.cy; /* top-down image */
				rect.top = g->p.y;
				rect.bottom = rect.top+g->p.cy;
				rect.left = g->p.x;
				rect.right = rect.left+g->p.cx;
				break;
			case GDISP_ROTATE_90:
				if (!(buffer = rotateimg(g, buffer))) return;
				bmpInfo.bV4SizeImage = (g->p.cy*g->p.cx) * sizeof(pixel_t);
				bmpInfo.bV4Width = g->p.cy;
				bmpInfo.bV4Height = -g->p.cx; /* top-down image */
				rect.bottom = g->g.Width - g->p.x;
				rect.top = rect.bottom-g->p.cx;
				rect.left = g->p.y;
				rect.right = rect.left+g->p.cy;
				break;
			case GDISP_ROTATE_180:
				if (!(buffer = rotateimg(g, buffer))) return;
				bmpInfo.bV4SizeImage = (g->p.cy*g->p.cx) * sizeof(pixel_t);
				bmpInfo.bV4Width = g->p.cx;
				bmpInfo.bV4Height = -g->p.cy; /* top-down image */
				rect.bottom = g->g.Height-1 - g->p.y;
				rect.top = rect.bottom-g->p.cy;
				rect.right = g->g.Width - g->p.x;
				rect.left = rect.right-g->p.cx;
				break;
			case GDISP_ROTATE_270:
				if (!(buffer = rotateimg(g, buffer))) return;
				bmpInfo.bV4SizeImage = (g->p.cy*g->p.cx) * sizeof(pixel_t);
				bmpInfo.bV4Width = g->p.cy;
				bmpInfo.bV4Height = -g->p.cx; /* top-down image */
				rect.top = g->p.x;
				rect.bottom = rect.top+g->p.cx;
				rect.right = g->g.Height - g->p.y;
				rect.left = rect.right-g->p.cy;
				break;
			}
		#else
			bmpInfo.bV4SizeImage = (g->p.cy*g->p.x2) * sizeof(pixel_t);
			bmpInfo.bV4Width = g->p.x2;
			bmpInfo.bV4Height = -g->p.cy; /* top-down image */
			rect.top = g->p.y;
			rect.bottom = rect.top+g->p.cy;
			rect.left = g->p.x;
			rect.right = rect.left+g->p.cx;
		#endif

		WaitForSingleObject(drawMutex, INFINITE);
		SetDIBitsToDevice(priv->dcBuffer, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, 0, 0, 0, rect.bottom-rect.top, buffer, (BITMAPINFO*)&bmpInfo, DIB_RGB_COLORS);
		#if GDISP_WIN32_USE_INDIRECT_UPDATE
			ReleaseMutex(drawMutex);
			InvalidateRect(priv->hwnd, &rect, FALSE);
		#else
			{
				HDC		dc;
				dc = GetDC(priv->hwnd);
				SetDIBitsToDevice(dc, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, 0, 0, 0, rect.bottom-rect.top, buffer, (BITMAPINFO*)&bmpInfo, DIB_RGB_COLORS);
				ReleaseDC(priv->hwnd, dc);
				ReleaseMutex(drawMutex);
			}
		#endif

		#if GDISP_NEED_CONTROL
			if (buffer != (pixel_t *)g->p.ptr)
				free(buffer);
		#endif
	}
#endif

#if GDISP_HARDWARE_PIXELREAD
	LLDSPEC	color_t gdisp_lld_get_pixel_color(GDisplay *g) {
		winPriv	*	priv;
		COLORREF	color;

		priv = g->priv;

		WaitForSingleObject(drawMutex, INFINITE);
		#if GDISP_NEED_CONTROL
			switch(g->g.Orientation) {
			case GDISP_ROTATE_0:
			default:
				color = GetPixel(priv->dcBuffer, g->p.x, g->p.y);
				break;
			case GDISP_ROTATE_90:
				color = GetPixel(priv->dcBuffer, g->p.y, g->g.Width - 1 - g->p.x);
				break;
			case GDISP_ROTATE_180:
				color = GetPixel(priv->dcBuffer, g->g.Width - 1 - g->p.x, g->g.Height - 1 - g->p.y);
				break;
			case GDISP_ROTATE_270:
				color = GetPixel(priv->dcBuffer, g->g.Height - 1 - g->p.y, g->p.x);
				break;
			}
		#else
			color = GetPixel(priv->dcBuffer, g->p.x, g->p.y);
		#endif
		ReleaseMutex(drawMutex);

		return gdispNative2Color(color);
	}
#endif

#if GDISP_NEED_SCROLL && GDISP_HARDWARE_SCROLL
	LLDSPEC void gdisp_lld_vertical_scroll(GDisplay *g) {
		winPriv	*	priv;
		RECT		rect;
		coord_t		lines;

		priv = g->priv;

		#if GDISP_NEED_CONTROL
			switch(g->g.Orientation) {
			case GDISP_ROTATE_0:
			default:
				rect.top = g->p.y;
				rect.bottom = rect.top+g->p.cy;
				rect.left = g->p.x;
				rect.right = rect.left+g->p.cx;
				lines = -g->p.y1;
				goto vertical_scroll;
			case GDISP_ROTATE_90:
				rect.bottom = g->g.Width - g->p.x;
				rect.top = rect.bottom-g->p.cx;
				rect.left = g->p.y;
				rect.right = rect.left+g->p.cy;
				lines = -g->p.y1;
				goto horizontal_scroll;
			case GDISP_ROTATE_180:
				rect.bottom = g->g.Height - g->p.y;
				rect.top = rect.bottom-g->p.cy;
				rect.right = g->g.Width - g->p.x;
				rect.left = rect.right-g->p.cx;
				lines = g->p.y1;
			vertical_scroll:
				if (lines > 0) {
					rect.bottom -= lines;
				} else {
					rect.top -= lines;
				}
				if (g->p.cy >= lines && g->p.cy >= -lines) {
					WaitForSingleObject(drawMutex, INFINITE);
					ScrollDC(priv->dcBuffer, 0, lines, &rect, 0, 0, 0);
					#if GDISP_WIN32_USE_INDIRECT_UPDATE
						ReleaseMutex(drawMutex);
						InvalidateRect(priv->hwnd, &rect, FALSE);
					#else
						{
							HDC		dc;
							dc = GetDC(priv->hwnd);
							ScrollDC(dc, 0, lines, &rect, 0, 0, 0);
							ReleaseDC(priv->hwnd, dc);
							ReleaseMutex(drawMutex);
						}
					#endif
				}
				break;
			case GDISP_ROTATE_270:
				rect.top = g->p.x;
				rect.bottom = rect.top+g->p.cx;
				rect.right = g->g.Height - g->p.y;
				rect.left = rect.right-g->p.cy;
				lines = g->p.y1;
			horizontal_scroll:
				if (lines > 0) {
					rect.right -= lines;
				} else {
					rect.left -= lines;
				}
				if (g->p.cy >= lines && g->p.cy >= -lines) {
					WaitForSingleObject(drawMutex, INFINITE);
					ScrollDC(priv->dcBuffer, lines, 0, &rect, 0, 0, 0);
					#if GDISP_WIN32_USE_INDIRECT_UPDATE
						ReleaseMutex(drawMutex);
						InvalidateRect(priv->hwnd, &rect, FALSE);
					#else
						{
							HDC		dc;
							dc = GetDC(priv->hwnd);
							ScrollDC(dc, lines, 0, &rect, 0, 0, 0);
							ReleaseDC(priv->hwnd, dc);
							ReleaseMutex(drawMutex);
						}
					#endif
				}
				break;
			}
		#else
			rect.top = g->p.y;
			rect.bottom = rect.top+g->p.cy;
			rect.left = g->p.x;
			rect.right = rect.left+g->p.cx;
			lines = -g->p.y1;
			if (lines > 0) {
				rect.bottom -= lines;
			} else {
				rect.top -= lines;
			}
			if (g->p.cy >= lines && g->p.cy >= -lines) {
				WaitForSingleObject(drawMutex, INFINITE);
				ScrollDC(priv->dcBuffer, 0, lines, &rect, 0, 0, 0);
				#if GDISP_WIN32_USE_INDIRECT_UPDATE
					ReleaseMutex(drawMutex);
					InvalidateRect(priv->hwnd, &rect, FALSE);
				#else
					{
						HDC		dc;
						dc = GetDC(priv->hwnd);
						ScrollDC(dc, 0, lines, &rect, 0, 0, 0);
						ReleaseDC(priv->hwnd, dc);
						ReleaseMutex(drawMutex);
					}
				#endif
			}
		#endif
	}
#endif

#if GDISP_NEED_CONTROL && GDISP_HARDWARE_CONTROL
	LLDSPEC void gdisp_lld_control(GDisplay *g) {
		switch(g->p.x) {
		case GDISP_CONTROL_ORIENTATION:
			if (g->g.Orientation == (orientation_t)g->p.ptr)
				return;
			switch((orientation_t)g->p.ptr) {
				case GDISP_ROTATE_0:
				case GDISP_ROTATE_180:
					g->g.Width = GDISP_SCREEN_WIDTH;
					g->g.Height = GDISP_SCREEN_HEIGHT;
					break;
				case GDISP_ROTATE_90:
				case GDISP_ROTATE_270:
					g->g.Height = GDISP_SCREEN_WIDTH;
					g->g.Width = GDISP_SCREEN_HEIGHT;
					break;
				default:
					return;
			}
			g->g.Orientation = (orientation_t)g->p.ptr;
			return;
/*
		case GDISP_CONTROL_POWER:
		case GDISP_CONTROL_BACKLIGHT:
		case GDISP_CONTROL_CONTRAST:
*/
		}
	}
#endif

#if GINPUT_NEED_MOUSE
	static bool_t Win32MouseInit(GMouse *m, unsigned driverinstance) {
		(void)	m;
		(void)	driverinstance;
		return TRUE;
	}
	static bool_t Win32MouseRead(GMouse *m, GMouseReading *pt) {
		GDisplay *	g;
		winPriv	*	priv;

		g = m->display;
		priv = g->priv;

		pt->x = priv->mousex;
		pt->y = priv->mousey;
		pt->z = (priv->mousebuttons & GINPUT_MOUSE_BTN_LEFT) ? 1 : 0;
		pt->buttons = priv->mousebuttons;

		#if GDISP_NEED_CONTROL
			// If the self-rotation has been set in the VMT then do that here (TESTING ONLY)
			if ((gmvmt(m)->d.flags & GMOUSE_VFLG_SELFROTATION)) {		// For normal setup this is always False
				coord_t		t;

				switch(gdispGGetOrientation(m->display)) {
					case GDISP_ROTATE_0:
					default:
						break;
					case GDISP_ROTATE_90:
						t = pt->x;
						pt->x = g->g.Width - 1 - pt->y;
						pt->y = t;
						break;
					case GDISP_ROTATE_180:
						pt->x = g->g.Width - 1 - pt->x;
						pt->y = g->g.Height - 1 - pt->y;
						break;
					case GDISP_ROTATE_270:
						t = pt->y;
						pt->y = g->g.Height - 1 - pt->x;
						pt->x = t;
						break;
				}
			}
		#endif

		return TRUE;
	}
#endif /* GINPUT_NEED_MOUSE */

#if GINPUT_NEED_KEYBOARD
	static bool_t Win32KeyboardInit(GKeyboard *k, unsigned driverinstance) {
		(void)	driverinstance;

		// Only one please
		if (keyboard)
			return FALSE;

		keyboard = k;
		return TRUE;
	}

	static int Win32KeyboardGetData(GKeyboard *k, uint8_t *pch, int sz) {
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
#endif

#if GINPUT_NEED_TOGGLE
	#if GINPUT_TOGGLE_CONFIG_ENTRIES > 1
		#error "GDISP Win32: GINPUT_TOGGLE_CONFIG_ENTRIES must be 1 until Toggles can use GDriver"
	#endif

	const GToggleConfig GInputToggleConfigTable[GINPUT_TOGGLE_CONFIG_ENTRIES];

	void ginput_lld_toggle_init(const GToggleConfig *ptc) {
		// Save the associated window struct
		//ptc->id = &GDISP_WIN32[ptc - GInputToggleConfigTable];
		((GToggleConfig *)ptc)->id = 0;

		// We have 8 buttons per window.
		((GToggleConfig *)ptc)->mask = 0xFF;

		// No inverse or special mode
		((GToggleConfig *)ptc)->invert = 0x00;
		((GToggleConfig *)ptc)->mode = 0;
	}
	unsigned ginput_lld_toggle_getbits(const GToggleConfig *ptc) {
		(void)		ptc;

		// This should use ID
		if (!toggleWindow)
			return 0;
		return ((winPriv *)toggleWindow->priv)->toggles;
		//return ((winPriv *)((GDisplay *)(ptc->id))->priv)->toggles;
	}
#endif /* GINPUT_NEED_TOGGLE */

#endif /* GFX_USE_GDISP */
