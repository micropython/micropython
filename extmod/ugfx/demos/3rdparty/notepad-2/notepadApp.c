/*
 * File: notepadApp.c
 *
 * This file is a part of the Notepad demo application for ChibiOS/GFX
 * Copyright © 2013, Kumar Abhishek [abhishek.kakkar@edaboard.com].
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 * 	   notice, this list of conditions and the following disclaimer in the
 * 	   documentation and/or other materials provided with the distribution.
 * 	 * The name of 'Kumar Abhishek' may not be used to endorse or promote
 *     products derived from this software without specific prior
 *     written permission.
 *
 * DISCLAIMER OF WARRANTY:
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Modified by InMarket to allow it to compile on any GFX supported operating system.
 */

#include "gfx.h"

#include "notepadApp.h"
#include "toolbarIcons.h"

// Get the handle for a window object
#define H(obj)					((GHandle)&obj)

// Static objects
static GListener				gl;               // Event listener object
static GHandle					nDrawingArea;     // GWindow Drawing Area
static GHandle					gstatusConsole;	  // GConsole Handle to the Status Bar

static GEventMouse				curPtr;           // Holder for current pointer location

static font_t					font;
static coord_t 					swidth, sheight;

// The toolbar buttons - Self-Explanatory
static GButtonObject btnNew, btnOpen, btnSave;
static GButtonObject btnColorBar;
static GButtonObject btnColorBarSel;

static GButtonObject btnPencil, btnFill, btnEraser;

static GButtonObject btnClose;
/* static GButtonObject btnYes, btnNo; // Reserved for future use */

// Image object
static gdispImage toolbarImageFilmstrip;

static color_t myColors[] = { Black, Red, Green, Blue, Cyan, Magenta, Yellow, White };

static color_t selColor = Black;
static int selColorIndex = 0, selPenWidth = 1, tbMode = 1;

static NColorScheme nCurColorScheme;

static DECLARE_THREAD_FUNCTION(notepadThread, param);

// Custom drawing functions for the buttons
static void nbtnColorBarDraw(GHandle gh, bool_t enabled, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param) {
  #define ccs nCurColorScheme

  int i, j, k;

  (void)txt;
  (void)pstyle;
  (void)param;
  (void)enabled;

  ginputGetMouseStatus(0, &curPtr);

  // Draw the toolbars according to the mode
  if (tbMode == 0) {
	k = (curPtr.x - gh->x) / (NPAD_COLORBAR_WIDTH / 8);

	for (i = 0; i < 8; i++) {
	  j = gh->x + (NPAD_TOOLBAR_BTN_WIDTH / 2) + NPAD_TOOLBAR_BTN_WIDTH * i;

	  if (isdown) {
		// Update selection - this is like lazy release.
		if (k >= 0 && k <= 7) {
		  selPenWidth = k + 1;
		  ncoreSetPenWidth((uint8_t) selPenWidth);
		}

		gdispFillArea(gh->x + NPAD_TOOLBAR_BTN_WIDTH * i, gh->y,
		              NPAD_TOOLBAR_BTN_WIDTH, NPAD_TOOLBAR_BTN_HEIGHT,
		              selPenWidth - i == 1 ? ccs.toolbarBgActive : ccs.toolbarBgUnsel);
	  }
	  else {
		gdispFillArea(gh->x + NPAD_TOOLBAR_BTN_WIDTH * i, gh->y,
		              NPAD_TOOLBAR_BTN_WIDTH, NPAD_TOOLBAR_BTN_HEIGHT,
		              selPenWidth - i == 1 ? ccs.toolbarBgSel : ccs.toolbarBgUnsel);

		gdispDrawBox(gh->x + NPAD_TOOLBAR_BTN_WIDTH * i, gh->y,
		             NPAD_TOOLBAR_BTN_WIDTH, NPAD_TOOLBAR_BTN_HEIGHT,
		             selPenWidth - i == 1 ? ccs.toolbarSeparator: ccs.toolbarBgUnsel);
	  }

	  gdispFillCircle(j, gh->y + 10, i + 1, myColors[selColorIndex]);
	}

  } else {
	k = (curPtr.x - gh->x) / (NPAD_COLORBAR_WIDTH / 8);

	for (i = 0; i < 8; i++) {
	  j = gh->x + (NPAD_TOOLBAR_BTN_WIDTH / 2) + NPAD_TOOLBAR_BTN_WIDTH * i;

	  if (isdown) {
		// Update selection - this is like lazy release.
		if (k >= 0 && k <= 7) {
		  selColorIndex = k;
		  selColor = myColors[k];
		  ncoreSetPenColor(selColor);
		}

		gdispFillArea(gh->x + NPAD_TOOLBAR_BTN_WIDTH * i, gh->y,
		              NPAD_TOOLBAR_BTN_WIDTH, NPAD_TOOLBAR_BTN_HEIGHT,
		              k == i ? ccs.toolbarBgActive : ccs.toolbarBgUnsel);
	  }
	  else {
		gdispFillArea(gh->x + NPAD_TOOLBAR_BTN_WIDTH * i, gh->y,
		              NPAD_TOOLBAR_BTN_WIDTH, NPAD_TOOLBAR_BTN_HEIGHT,
		              selColorIndex == i ? ccs.toolbarBgSel : ccs.toolbarBgUnsel);

		gdispDrawBox(gh->x + NPAD_TOOLBAR_BTN_WIDTH * i, gh->y,
		             NPAD_TOOLBAR_BTN_WIDTH, NPAD_TOOLBAR_BTN_HEIGHT,
		             selColorIndex == i ? ccs.toolbarSeparator: ccs.toolbarBgUnsel);
	  }

	  gdispFillCircle(j, gh->y + (NPAD_TOOLBAR_BTN_HEIGHT / 2), 3, myColors[i] );
	}
  }


  #undef ccs
}

static void nbtnColorBarSelDraw(GHandle gh, bool_t enabled, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param) {
#define ccs nCurColorScheme

  int i, j = 0, k;
  color_t ca, cb;
  GEventMouse ptr;

  (void)txt;
  (void)pstyle;
  (void)param;
  (void)enabled;

  // Get a copy of the pointer location
  ginputGetMouseStatus(0, &ptr);

  // Get which button the pointer is on right now
  k = (ptr.x - gh->x) / NPAD_TOOLBAR_BTN_WIDTH;

  gdispDrawBox(gh->x, gh->y, gh->width, gh->height, ccs.toolbarBgUnsel);
  gdispDrawBox(gh->x + 1, gh->y + 1, gh->width - 2, gh->height - 2, ccs.toolbarBgUnsel);

  for (i = 0; i < 2; i++) {
  	if (isdown) {
  	  // Update selection - this is like lazy release.
  	  if (k == 0 || k == 1) {
  		tbMode = k;
  		j = 1;
  	  }

  	  ca = (tbMode == i ? ccs.toolbarBgActive : ccs.toolbarBgUnsel);
  	}
  	else {
  	  ca = (tbMode == i ? ccs.toolbarBgSel : ccs.toolbarBgUnsel);
  	}

  	cb = (tbMode == i ? ccs.toolbarSeparator : ccs.toolbarBgUnsel);

  	gdispFillArea(gh->x + NPAD_TOOLBAR_BTN_WIDTH * i,
				  gh->y,
				  NPAD_TOOLBAR_BTN_WIDTH,
				  NPAD_TOOLBAR_BTN_HEIGHT,
				  ca);

  	gdispImageSetBgColor(&toolbarImageFilmstrip, ca);
  	gdispDrawBox(gh->x + NPAD_TOOLBAR_BTN_WIDTH * i, gh->y,
  	             NPAD_TOOLBAR_BTN_WIDTH, NPAD_TOOLBAR_BTN_HEIGHT, cb);

  	/* Draw both the icons */
  	gwinImageDraw(gh, &toolbarImageFilmstrip,
  	              2 + NPAD_TOOLBAR_BTN_WIDTH * i,
  	              2,
  	              NPAD_ICON_WIDTH,
  	              NPAD_ICON_HEIGHT,
  	              NPAD_ICON_START(3 + i),
  	              0);
  }

  if (j)
	gwinButtonDraw(H(btnColorBar));

  #undef ccs
}

static void nToolbarImageButtonDraw(GHandle gh, bool_t isenabled, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param) {
  (void)txt;  (void)pstyle; (void)isenabled;

  color_t cl = isdown ? nCurColorScheme.toolbarBgActive : nCurColorScheme.toolbarBgUnsel;

  gdispImageSetBgColor(&toolbarImageFilmstrip, cl);
  gdispFillArea(gh->x, gh->y, gh->width, gh->height, cl);
  gwinImageDraw(gh, &toolbarImageFilmstrip, 2, 2, NPAD_ICON_WIDTH, NPAD_ICON_HEIGHT, NPAD_ICON_START((int) param), 0);

  if (isdown || ((int)param - 5) == ncoreGetMode())
	gdispDrawBox(gh->x, gh->y, gh->width, gh->height, nCurColorScheme.toolbarSeparator);
}

static void nCloseButtonDraw(GHandle gh, bool_t isenabled, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param) {
  (void) isenabled;
  (void) isdown;
  (void) txt;
  (void) pstyle;
  (void) param;
  gwinImageDraw(gh, &toolbarImageFilmstrip, 0, 0, NPAD_ICON_WIDTH, NPAD_ICON_HEIGHT, NPAD_ICON_START(8), 0);
}

static void initButtons(void) {

  /* Init all the button objects */
  gwinCreateButton(&btnNew, NPAD_TOOLBAR_1_X(0), NPAD_TOOLBAR_1_Y, NPAD_TOOLBAR_BTN_WIDTH, NPAD_TOOLBAR_BTN_HEIGHT, font, GBTN_CUSTOM);
  gwinCreateButton(&btnOpen, NPAD_TOOLBAR_1_X(1), NPAD_TOOLBAR_1_Y, NPAD_TOOLBAR_BTN_WIDTH, NPAD_TOOLBAR_BTN_HEIGHT, font, GBTN_CUSTOM);
  gwinCreateButton(&btnSave, NPAD_TOOLBAR_1_X(2), NPAD_TOOLBAR_1_Y, NPAD_TOOLBAR_BTN_WIDTH, NPAD_TOOLBAR_BTN_HEIGHT, font, GBTN_CUSTOM);

  gwinCreateButton(&btnColorBarSel, NPAD_COLORBAR_SEL_X, NPAD_COLORBAR_SEL_Y, NPAD_COLORBAR_SEL_WIDTH, NPAD_COLORBAR_SEL_HEIGHT, font, GBTN_CUSTOM);
  gwinCreateButton(&btnColorBar, NPAD_COLORBAR_X, NPAD_TOOLBAR_1_Y, NPAD_COLORBAR_WIDTH, NPAD_COLORBAR_HEIGHT, font, GBTN_CUSTOM);

  gwinCreateButton(&btnPencil, NPAD_TOOLBAR_2_X, NPAD_TOOLBAR_2_Y(0), NPAD_TOOLBAR_BTN_WIDTH, NPAD_TOOLBAR_BTN_HEIGHT, font, GBTN_CUSTOM);
  gwinCreateButton(&btnEraser, NPAD_TOOLBAR_2_X, NPAD_TOOLBAR_2_Y(1), NPAD_TOOLBAR_BTN_WIDTH, NPAD_TOOLBAR_BTN_HEIGHT, font, GBTN_CUSTOM);
  gwinCreateButton(&btnFill, NPAD_TOOLBAR_2_X, NPAD_TOOLBAR_2_Y(2), NPAD_TOOLBAR_BTN_WIDTH, NPAD_TOOLBAR_BTN_HEIGHT, font, GBTN_CUSTOM);

  gwinCreateButton(&btnClose, swidth - 18, 0, 16, 16, font, GBTN_CUSTOM);

  /* Attach custom renders */
  gwinSetButtonCustom(H(btnNew), nToolbarImageButtonDraw, (void *)0);
  gwinSetButtonCustom(H(btnOpen), nToolbarImageButtonDraw, (void *)1);
  gwinSetButtonCustom(H(btnSave), nToolbarImageButtonDraw, (void *)2);

  gwinSetButtonCustom(H(btnColorBar), nbtnColorBarDraw, (void *)0);
  gwinSetButtonCustom(H(btnColorBarSel), nbtnColorBarSelDraw, (void *)0);

  gwinSetButtonCustom(H(btnPencil), nToolbarImageButtonDraw, (void *)5);
  gwinSetButtonCustom(H(btnEraser), nToolbarImageButtonDraw, (void *)6);
  gwinSetButtonCustom(H(btnFill), nToolbarImageButtonDraw, (void *)7);
  gwinSetButtonCustom(H(btnClose), nCloseButtonDraw, (void *)0);

  /* Attach listeners */
  geventAttachSource(&gl, gwinGetButtonSource(H(btnNew)), 0);
  geventAttachSource(&gl, gwinGetButtonSource(H(btnOpen)), 0);
  geventAttachSource(&gl, gwinGetButtonSource(H(btnSave)), 0);

  geventAttachSource(&gl, gwinGetButtonSource(H(btnColorBar)), 0);
  geventAttachSource(&gl, gwinGetButtonSource(H(btnColorBarSel)), 0);

  geventAttachSource(&gl, gwinGetButtonSource(H(btnPencil)), 0);
  geventAttachSource(&gl, gwinGetButtonSource(H(btnEraser)), 0);
  geventAttachSource(&gl, gwinGetButtonSource(H(btnFill)), 0);
  geventAttachSource(&gl, gwinGetButtonSource(H(btnClose)), 0);

  /* Give listeners access to touch events */
  gwinAttachButtonMouse(H(btnNew), 0);
  gwinAttachButtonMouse(H(btnOpen), 0);
  gwinAttachButtonMouse(H(btnSave), 0);

  gwinAttachButtonMouse(H(btnColorBar), 0);
  gwinAttachButtonMouse(H(btnColorBarSel), 0);

  gwinAttachButtonMouse(H(btnPencil), 0);
  gwinAttachButtonMouse(H(btnEraser), 0);
  gwinAttachButtonMouse(H(btnFill), 0);
  gwinAttachButtonMouse(H(btnClose), 0);

}

static void drawButtons(void) {
  gwinButtonDraw(H(btnNew));
  gwinButtonDraw(H(btnOpen));
  gwinButtonDraw(H(btnSave));

  gwinButtonDraw(H(btnColorBar));
  gwinButtonDraw(H(btnColorBarSel));

  gwinButtonDraw(H(btnClose));
}

static void drawVButtons(void) {
  gwinButtonDraw(H(btnPencil));
  gwinButtonDraw(H(btnEraser));
  gwinButtonDraw(H(btnFill));
}

static DECLARE_THREAD_STACK(waNotepadThread, NPAD_THD_WA_SIZE);
static DECLARE_THREAD_FUNCTION(notepadThread, param) {

  GEventMouse		*pem;
  GEventGWinButton	*peb;
  GHandle			ghc;

  (void)param;

  /* Get the display dimensions */
  swidth = gdispGetWidth();
  sheight = gdispGetHeight();

  font = gdispOpenFont("UI2");

  /* Initialize the mouse */
  geventListenerInit(&gl);
  ginputGetMouse(0);

  initButtons();

  /* Configure the GIF decoder with the toolbar Icon images */
  gdispImageSetMemoryReader(&toolbarImageFilmstrip, toolbarIcons);
  gdispImageOpen(&toolbarImageFilmstrip);

  /* Set clip to the entire screen */
  gdispSetClip(0, 0, swidth, sheight);

  /* Clear the screen with the window background
   * Also, draw the title bars */
  gdispClear(nCurColorScheme.winBgColor);
  gdispDrawBox(0, 0, swidth, sheight, nCurColorScheme.titleBarColor);
  gdispFillArea(0, 0, swidth, NPAD_TITLEBAR_HEIGHT, nCurColorScheme.titleBarColor);
  gdispDrawStringBox(NPAD_TITLETEXT_START_X,
                     NPAD_TITLETEXT_START_Y,
                     swidth,
                     NPAD_TITLEBAR_HEIGHT,
                     NPAD_TITLETEXT_STR,
                     font,
                     nCurColorScheme.titleTextColor,
                     justifyLeft);

  /* Create the drawing window, draw its border */
  gdispDrawBox(NPAD_DRAWING_AREA_START_X - 1,
               NPAD_DRAWING_AREA_START_Y - 1,
  			   NPAD_DRAWING_AREA_WIDTH + 2,
  			   NPAD_DRAWING_AREA_HEIGHT + 2,
  			   nCurColorScheme.drawingWinBorder);

  nDrawingArea = gwinCreateWindow(NULL,
								  NPAD_DRAWING_AREA_START_X,
								  NPAD_DRAWING_AREA_START_Y,
								  NPAD_DRAWING_AREA_WIDTH,
								  NPAD_DRAWING_AREA_HEIGHT);

  /* Create the bottom status bar console */
  ghc = gwinCreateConsole(NULL,
                          NPAD_STATUSBAR_START_X,
						  NPAD_STATUSBAR_START_Y,
                          NPAD_STATUSBAR_WIDTH,
                          NPAD_STATUSBAR_HEIGHT,
                          font);

  gdispImageDraw(&toolbarImageFilmstrip,
                 NPAD_STATUSBAR_ICON_START_X,
                 NPAD_STATUSBAR_ICON_START_Y,
                 NPAD_ICON_WIDTH,
                 NPAD_ICON_HEIGHT,
                 NPAD_ICON_START(12),
                 0);

  gwinSetBgColor(ghc, nCurColorScheme.winBgColor);
  gwinSetColor(ghc, Black);

  gstatusConsole = ghc;

  /* draw the buttons */
  gwinSetColor(nDrawingArea, Black);
  gwinSetBgColor(nDrawingArea, White);

  gwinClear(nDrawingArea);
  gwinClear(ghc);

  drawButtons();
  drawVButtons();

  gwinPrintf(gstatusConsole, "Welcome to ChibiOS/GFX Notepad demo.");

  ncoreSpawnDrawThread(nDrawingArea, gstatusConsole);

  while(TRUE) {
	  pem = (GEventMouse *) geventEventWait(&gl, TIME_INFINITE);

	  /* button pressed... */
	  if (pem->type == GEVENT_GWIN_BUTTON) {
		peb = (GEventGWinButton *)pem;

		if (peb->button == H(btnNew)) {
		  // Reset all the settings
		  selColorIndex = 0;
		  selPenWidth = 0;
		  ncoreSetMode(NCORE_MODE_DRAW);

		  gwinSetColor(nDrawingArea, Black);
		  gwinSetBgColor(nDrawingArea, White);

		  // Refresh the buttons
		  drawButtons();
		  drawVButtons();

		  gwinClear(nDrawingArea);
		  gwinPrintf(gstatusConsole, "\nScreen Cleared.");
		}
		else if (peb->button == H(btnOpen)) {
		  gwinPrintf(gstatusConsole, "\nFile Open not implemented.");
		}
		else if (peb->button == H(btnSave)) {
		  gwinPrintf(gstatusConsole, "\nFile Save not implemented.");
		}
		else if (peb->button == H(btnPencil)) {
		  ncoreSetMode(NCORE_MODE_DRAW);
		  drawVButtons();
		  gwinPrintf(gstatusConsole, "\nPencil Tool Selected.");
		}
		else if (peb->button == H(btnEraser)) {
		  ncoreSetMode(NCORE_MODE_ERASE);
		  drawVButtons();
		  gwinPrintf(gstatusConsole, "\nEraser Tool Selected.");
		}
		else if (peb->button == H(btnFill)) {
		  ncoreSetMode(NCORE_MODE_FILL);
		  drawVButtons();
		  gwinPrintf(gstatusConsole, "\nFill Tool Selected.");
		}
		else if (peb->button == H(btnClose)) {
		  break;
		}
	  }
  }

  gwinDestroyWindow(ghc);
  // No need to destroy the buttons as they are statically allocated
  gdispCloseFont(font);
  ncoreTerminateDrawThread();
  gdispImageClose(&toolbarImageFilmstrip);

  return 0;
}

// Public methods
void nSetColorScheme(NColorScheme sch)  {  nCurColorScheme = sch; }
NColorScheme nGetColorScheme(void)      { return nCurColorScheme; }

gfxThreadHandle nLaunchNotepadApp(void) {

  return gfxThreadCreate(waNotepadThread,
						   sizeof(waNotepadThread),
						   NPAD_THD_PRIO,
						   notepadThread, NULL);
}
