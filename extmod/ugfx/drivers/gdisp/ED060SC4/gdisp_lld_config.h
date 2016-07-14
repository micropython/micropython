/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/* Supported features of the driver for ED060SC4 PrimeView E-Ink panel. */

#ifndef _GDISP_LLD_CONFIG_H
#define _GDISP_LLD_CONFIG_H

#if GFX_USE_GDISP

#define GDISP_HARDWARE_FLUSH			TRUE		// This controller requires flushing
#define GDISP_HARDWARE_DRAWPIXEL		TRUE
#define GDISP_HARDWARE_CLEARS           TRUE
#define GDISP_HARDWARE_CONTROL          TRUE

#define GDISP_LLD_PIXELFORMAT			GDISP_PIXELFORMAT_MONO

#endif

#endif
