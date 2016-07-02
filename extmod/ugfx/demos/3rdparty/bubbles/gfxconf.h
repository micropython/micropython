/**
 * This file has a different license to the rest of the GFX system.
 * You can copy, modify and distribute this file as you see fit.
 * You do not need to publish your source modifications to this file.
 * The only thing you are not permitted to do is to relicense it
 * under a different license.
 */

#ifndef _GFXCONF_H
#define _GFXCONF_H

/* The operating system to use. One of these must be defined - preferably in your Makefile */
//#define GFX_USE_OS_CHIBIOS            FALSE
//#define GFX_USE_OS_WIN32              FALSE
//#define GFX_USE_OS_LINUX              FALSE
//#define GFX_USE_OS_OSX                FALSE

/* GFX sub-systems to turn on */
#define GFX_USE_GDISP                   TRUE

/* Features for the GDISP sub-system. */
#define GDISP_NEED_VALIDATION           FALSE
#define GDISP_NEED_CLIP                 FALSE
#define GDISP_NEED_TEXT                 FALSE
#define GDISP_NEED_CIRCLE               TRUE
#define GDISP_NEED_ELLIPSE              FALSE
#define GDISP_NEED_ARC                  FALSE
#define GDISP_NEED_SCROLL               FALSE
#define GDISP_NEED_PIXELREAD            FALSE
#define GDISP_NEED_CONTROL              FALSE
#define GDISP_NEED_MULTITHREAD          FALSE
#define GDISP_NEED_ASYNC                FALSE
#define GDISP_NEED_MSGAPI               FALSE

/* Builtin Fonts */
#define GDISP_INCLUDE_FONT_SMALL        FALSE
#define GDISP_INCLUDE_FONT_LARGER       FALSE
#define GDISP_INCLUDE_FONT_UI1          FALSE
#define GDISP_INCLUDE_FONT_UI2          FALSE
#define GDISP_INCLUDE_FONT_LARGENUMBERS FALSE

#endif /* _GFXCONF_H */
