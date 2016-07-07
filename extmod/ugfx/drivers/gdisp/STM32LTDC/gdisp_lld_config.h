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

#define	LTDC_USE_DMA2D						TRUE
#define GDISP_HARDWARE_DRAWPIXEL			TRUE
#define GDISP_HARDWARE_PIXELREAD			TRUE
#define GDISP_HARDWARE_CONTROL				TRUE

// Both these pixel formats are supported - pick one.
// RGB565 obviously is faster and uses less RAM but with lower color resolution than RGB888
#define GDISP_LLD_PIXELFORMAT				GDISP_PIXELFORMAT_RGB565
//#define GDISP_LLD_PIXELFORMAT				GDISP_PIXELFORMAT_RGB888


/*===========================================================================*/
/* Don't change stuff below this line. Please.                               */
/*===========================================================================*/

#if LTDC_USE_DMA2D
	// DMA2D supports accelerated fills
 	#define GDISP_HARDWARE_FILLS		TRUE

	// Accelerated bitfills are also possible but only for GDISP_ROTATE_0
	//	and if no color translation is required (for now)
	#if !GDISP_NEED_CONTROL && GDISP_PIXELFORMAT == GDISP_LLD_PIXELFORMAT
 		#define GDISP_HARDWARE_BITFILLS	TRUE
	#endif
#endif /* GDISP_USE_DMA2D */

#endif	/* GFX_USE_GDISP */

#endif	/* _GDISP_LLD_CONFIG_H */
