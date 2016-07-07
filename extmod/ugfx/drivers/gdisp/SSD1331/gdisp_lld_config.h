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

// This controller supports some optimized drawing routines - but no read (at least with SPI interfacing)
#define GDISP_HARDWARE_STREAM_WRITE		TRUE
#define GDISP_HARDWARE_FILLS			TRUE
#define GDISP_HARDWARE_SCROLL			TRUE
#define GDISP_HARDWARE_CONTROL			TRUE

// This driver supports 4 pixel formats - pick the one you want.
#define GDISP_LLD_PIXELFORMAT			GDISP_PIXELFORMAT_RGB565
//#define GDISP_LLD_PIXELFORMAT			GDISP_PIXELFORMAT_BGR565
//#define GDISP_LLD_PIXELFORMAT			GDISP_PIXELFORMAT_RGB332
//#define GDISP_LLD_PIXELFORMAT			GDISP_PIXELFORMAT_BGR332

#endif	/* GFX_USE_GDISP */

#endif	/* _GDISP_LLD_CONFIG_H */
