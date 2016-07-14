/*
 * File: main.c
 *
 * This file is a part of the Notepad demo application for ChibiOS/GFX
 * Copyright © 2013, Kumar Abhishek [abhishek.kakkar@edaboard.com].
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *	* Redistributions in binary form must reproduce the above copyright
 * 	  notice, this list of conditions and the following disclaimer in the
 * 	  documentation and/or other materials provided with the distribution.
 *	* The name of 'Kumar Abhishek' may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
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

const NColorScheme schemeDefault = {
  .titleBarColor = HTML2COLOR(0x3064D8),
  .titleTextColor = White,

  .winBgColor = HTML2COLOR(0xFFECB0),
  .drawingWinBorder = HTML2COLOR(0xCEBA77),

  .toolbarSeparator = HTML2COLOR(0xC09028),
  .toolbarBgUnsel = HTML2COLOR(0xF0D484),
  .toolbarBgActive = HTML2COLOR(0xC8B060),
  .toolbarBgSel = HTML2COLOR(0xDCC06C),

  .statusBarText = HTML2COLOR(0x000000)
};

const NColorScheme schemeDefault2 = {
  .titleBarColor = HTML2COLOR(0x3064D8),
  .titleTextColor = White,

  .winBgColor = HTML2COLOR(0xC8D8F8),
  .drawingWinBorder = HTML2COLOR(0x8098E0),

  .toolbarSeparator = HTML2COLOR(0x5078F0),
  .toolbarBgUnsel = HTML2COLOR(0xA0B8F0),
  .toolbarBgActive = HTML2COLOR(0x6084F0),
  .toolbarBgSel = HTML2COLOR(0x88A8F8),

  .statusBarText = HTML2COLOR(0x000000)
};

const char *tsCalibRead(uint16_t instance) {
  // This will perform a on-spot calibration
  // Unless you read and add the co-efficients here
  (void) instance;
  return NULL;
}

int main(void) {

  font_t font = gdispOpenFont("UI2");

  /* initialize the hardware and the OS */
  gfxInit();

  /* Calibrate the touchscreen */
  ginputSetMouseCalibrationRoutines(0, NULL, tsCalibRead, FALSE);
  ginputGetMouse(0);

  // Set the color scheme
  nSetColorScheme(schemeDefault);

  while (TRUE) {
	gfxThreadWait(nLaunchNotepadApp());

	gdispSetClip(0, 0, gdispGetWidth(), gdispGetHeight());
	gdispClear(Black);
	gdispDrawString(3, 3, "Notepad Terminated.", font, White);
	gdispDrawString(3, 20, "Relaunching Notepad App...", font, White);

	gfxSleepMilliseconds(1000);
  }

	return 0;
}
