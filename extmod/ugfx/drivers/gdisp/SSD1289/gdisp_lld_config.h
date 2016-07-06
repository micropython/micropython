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

#define GDISP_HARDWARE_STREAM_WRITE		TRUE
#define GDISP_HARDWARE_STREAM_READ		TRUE
#define GDISP_HARDWARE_STREAM_POS		TRUE
#define GDISP_HARDWARE_CONTROL			TRUE

#if defined(GDISP_USE_DMA) && GDISP_USE_DMA
	#define GDISP_HARDWARE_FILLS		TRUE
	#if !defined(GDISP_PIXELFORMAT) || GDISP_PIXELFORMAT == 0x2565
		// Hardware BitBlts are only supported in native pixel format on this controller
		#define GDISP_HARDWARE_BITFILLS		TRUE
	#endif
#endif

#define GDISP_LLD_PIXELFORMAT			GDISP_PIXELFORMAT_RGB565

#endif	/* GFX_USE_GDISP */

#endif	/* _GDISP_LLD_CONFIG_H */
