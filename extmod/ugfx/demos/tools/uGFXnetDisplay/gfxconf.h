/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GFXCONF_H
#define _GFXCONF_H

/* The operating system to use. One of these must be defined - preferably in your Makefile */
//#define GFX_USE_OS_CHIBIOS	FALSE
//#define GFX_USE_OS_WIN32		FALSE
//#define GFX_USE_OS_LINUX		FALSE
//#define GFX_USE_OS_OSX		FALSE

/* GFX sub-systems to turn on */
#define GFX_USE_GDISP				TRUE
#define GFX_USE_GEVENT				TRUE
#define GFX_USE_GTIMER				TRUE
#define GFX_USE_GINPUT				TRUE

/* Features for the GDISP sub-system. */
#define GDISP_NEED_VALIDATION		TRUE
#define GDISP_NEED_TEXT				TRUE
#define GDISP_NEED_CONTROL			TRUE
#define GDISP_NEED_SCROLL			TRUE
#define GDISP_NEED_PIXELREAD		TRUE
#define GDISP_NEED_STREAMING		TRUE

/* Builtin Fonts */
#define GDISP_INCLUDE_FONT_UI2		TRUE

/* Features for the GINPUT sub-system. */
#define GINPUT_NEED_MOUSE			TRUE

/* We need to use this specific pixel format to match the network protocol */
#define GDISP_PIXELFORMAT			GDISP_PIXELFORMAT_RGB565

#endif /* _GFXCONF_H */

