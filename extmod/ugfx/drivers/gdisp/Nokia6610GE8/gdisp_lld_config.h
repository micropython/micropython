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

/* This driver has problems with other orientations and requires significant
 * extra code to handle them. By default we turn this on only if the GDISP_NEED_CONTROL
 * is turned on. If you are worried about code size and don't need orientation support
 * define GDISP_NOKIA_ORIENTATION as false in your gfxconf.h.
 */
#ifndef GDISP_NOKIA_ORIENTATION
	#define GDISP_NOKIA_ORIENTATION			TRUE
#endif

#if GDISP_NOKIA_ORIENTATION && GDISP_NEED_CONTROL
	#define GDISP_HARDWARE_CONTROL			TRUE
	#define GDISP_HARDWARE_DRAWPIXEL		TRUE
	#define GDISP_HARDWARE_FILLS			TRUE
	#define GDISP_HARDWARE_BITFILLS			TRUE
#else
	#define GDISP_HARDWARE_CONTROL			TRUE
	#define GDISP_HARDWARE_STREAM_WRITE		TRUE
#endif

#define GDISP_LLD_PIXELFORMAT				GDISP_PIXELFORMAT_RGB444

#endif	/* GFX_USE_GDISP */

#endif	/* _GDISP_LLD_CONFIG_H */
