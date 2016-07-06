/*
 * File: notepadUIDefines.h
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

#ifndef NOTEPADUIDEFINES_H_
#define NOTEPADUIDEFINES_H_

#define NPAD_TITLETEXT_STR					"ChibiOS/GFX Notepad Demo"

/* UI Element dimenstions */
#define NPAD_TITLEBAR_HEIGHT				18
#define NPAD_TITLETEXT_START_X				5
#define NPAD_TITLETEXT_START_Y				0

#define NPAD_TOOLBAR_BTN_WIDTH				20
#define NPAD_TOOLBAR_BTN_HEIGHT				20

#define NPAD_TOOLBAR_INTERBTN_PADDING_X		2
#define NPAD_TOOLBAR_1_START_X				5
#define NPAD_TOOLBAR_1_Y					(NPAD_TITLEBAR_HEIGHT + 3)
#define NPAD_TOOLBAR_1_X(n)					(NPAD_TOOLBAR_1_START_X + \
                                            (NPAD_TOOLBAR_BTN_WIDTH + \
                                             NPAD_TOOLBAR_INTERBTN_PADDING_X) * n)

#define NPAD_TOOLBAR_INTERBTN_PADDING_Y		2
#define NPAD_TOOLBAR_2_START_Y				(NPAD_DRAWING_AREA_START_Y + 6)
#define NPAD_TOOLBAR_2_X					4
#define NPAD_TOOLBAR_2_Y(n)					(NPAD_TOOLBAR_2_START_Y + \
                                            (NPAD_TOOLBAR_BTN_HEIGHT + \
                                             NPAD_TOOLBAR_INTERBTN_PADDING_Y) * n)

#define NPAD_DRAWING_AREA_START_X			28
#define NPAD_DRAWING_AREA_START_Y			46
#define NPAD_DRAWING_AREA_END_X				10
#define NPAD_DRAWING_AREA_END_Y				24
#define NPAD_DRAWING_AREA_WIDTH				(swidth - \
                               				 NPAD_DRAWING_AREA_START_X - \
                               				 NPAD_DRAWING_AREA_END_X)
#define NPAD_DRAWING_AREA_HEIGHT			(sheight - \
                               				 NPAD_DRAWING_AREA_START_Y - \
                               				 NPAD_DRAWING_AREA_END_Y)

#define NPAD_COLORBAR_WIDTH					160
#define NPAD_COLORBAR_HEIGHT				20
#define NPAD_COLORBAR_X						(swidth - NPAD_DRAWING_AREA_END_X - NPAD_COLORBAR_WIDTH)
#define NPAD_COLORBAR_Y						NPAD_TOOLBAR_1_Y

#define NPAD_COLORBAR_SEL_WIDTH				(2 * NPAD_TOOLBAR_BTN_WIDTH)
#define NPAD_COLORBAR_SEL_HEIGHT			(NPAD_TOOLBAR_BTN_HEIGHT)
#define NPAD_COLORBAR_SEL_X					(NPAD_COLORBAR_X - NPAD_COLORBAR_SEL_WIDTH - 24)
#define NPAD_COLORBAR_SEL_Y					NPAD_TOOLBAR_1_Y

#define NPAD_ICON_WIDTH						16
#define NPAD_ICON_HEIGHT					16
#define NPAD_ICON_START(x)					((x) * NPAD_ICON_WIDTH)


#define NPAD_STATUSBAR_ICON_START_X			5
#define NPAD_STATUSBAR_ICON_START_Y			(sheight - 5 - NPAD_ICON_HEIGHT - 1)


#define NPAD_STATUSBAR_PADDING				5
#define NPAD_STATUSBAR_START_X				(NPAD_STATUSBAR_ICON_START_X + NPAD_ICON_WIDTH + NPAD_STATUSBAR_PADDING)
#define NPAD_STATUSBAR_START_Y				(NPAD_STATUSBAR_ICON_START_Y + 3)
#define NPAD_STATUSBAR_WIDTH				(swidth - NPAD_STATUSBAR_START_X - 2)
#define NPAD_STATUSBAR_HEIGHT				15

/* Color scheme definition */
typedef struct colScheme {
	color_t titleBarColor;
	color_t titleTextColor;

	color_t winBgColor;
	color_t drawingWinBorder;

	color_t toolbarSeparator;
	color_t toolbarBgUnsel;
	color_t toolbarBgActive;
	color_t toolbarBgSel;

	color_t statusBarText;
} NColorScheme;



#endif /* NOTEPADUIDEFINES_H_ */
