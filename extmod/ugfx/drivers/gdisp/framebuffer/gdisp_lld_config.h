/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_CONFIG_H
#define _GDISP_LLD_CONFIG_H

#if GFX_USE_GDISP

/*===========================================================================*/
/* Driver hardware support.                                                  */
/*===========================================================================*/

#define GDISP_HARDWARE_DRAWPIXEL		TRUE
#define GDISP_HARDWARE_PIXELREAD		TRUE
#define GDISP_HARDWARE_CONTROL			TRUE

// Any other support comes from the board file
#include "board_framebuffer.h"

#ifndef GDISP_LLD_PIXELFORMAT
	#error "GDISP FrameBuffer: You must specify a GDISP_LLD_PIXELFORMAT in your board_framebuffer.h or your makefile"
#endif

// This driver currently only supports unpacked formats with more than 8 bits per pixel
//	that is, we only support GRAY_SCALE with 8 bits per pixel or any unpacked TRUE_COLOR format.
// Note: At the time this file is included we have not calculated all our color
//			definitions so we need to do this by hand.
#if (GDISP_LLD_PIXELFORMAT & 0x4000) && (GDISP_LLD_PIXELFORMAT & 0xFF) != 8
	#error "GDISP FrameBuffer: This driver does not support the specified GDISP_LLD_PIXELFORMAT"
#endif

#endif	/* GFX_USE_GDISP */

#endif	/* _GDISP_LLD_CONFIG_H */
