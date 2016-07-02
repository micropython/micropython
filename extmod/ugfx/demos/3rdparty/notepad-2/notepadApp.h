/*
 * File: notepadApp.h
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

#ifndef NOTEPADAPP_H_
#define NOTEPADAPP_H_

#include "notepadCore.h"
#include "notepadUIDefines.h"

#define NPAD_THD_PRIO		NORMAL_PRIORITY
#define NPAD_THD_WA_SIZE	512

void nSetColorScheme(NColorScheme sch);
NColorScheme nGetColorScheme(void);


gfxThreadHandle nLaunchNotepadApp(void);

#endif /* NOTEPADAPP_H_ */
