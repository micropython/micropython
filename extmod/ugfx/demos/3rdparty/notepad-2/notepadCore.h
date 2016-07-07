/*
 * File: notepadCore.h
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

#ifndef NOTEPADCORE_H_
#define NOTEPADCORE_H_

/* Configuration */
#define	NCORE_THD_STACK_SIZE	256
#define NCORE_THD_PRIO			NORMAL_PRIORITY

#define NCORE_MODE_DRAW			0
#define NCORE_MODE_ERASE		1
#define NCORE_MODE_FILL			2

#define MAX_DX			5000
#define MAX_DY			5000

/* Spawn the notepad core thread */
void ncoreSpawnDrawThread(GHandle drawingArea, GHandle statusConsole);

/* Terminate the core thread, wait for control release */
void ncoreTerminateDrawThread(void);

/* Get and set the pen width
 * Brush is cicular, width is pixel radius */
void ncoreSetPenWidth(uint8_t penWidth);
uint8_t ncoreGetPenWidth(void);

/* Get and set the drawing color */
void ncoreSetPenColor(color_t penColor);
color_t ncoreGetPenColor(void);

/* Get and set the pen mode */
void ncoreSetMode(uint8_t mode);
uint8_t ncoreGetMode(void);

#endif /* NOTEPADCORE_H_ */
