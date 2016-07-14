/**
 * This file has a different license to the rest of the uGFX system.
 * You can copy, modify and distribute this file as you see fit.
 * You do not need to publish your source modifications to this file.
 * The only thing you are not permitted to do is to relicense it
 * under a different license.
 */

/**
 * Copy this file into your project directory and rename it as gfxconf.h
 * Edit your copy to turn on the uGFX features you want to use.
 * The values below are the defaults. You should delete anything
 * you are leaving as default.
 *
 * Please use spaces instead of tabs in this file.
 */

#ifndef _GFXCONF_H
#define _GFXCONF_H

/* The operating system to use. One of these must be defined - preferably in your Makefile */
//#define GFX_USE_OS_CHIBIOS                         TRUE
//#define GFX_USE_OS_WIN32                           TRUE
//#define GFX_USE_OS_LINUX                           TRUE
//#define GFX_USE_OS_OSX                             TRUE

#define GFX_USE_GDISP                                TRUE

#define GDISP_NEED_VALIDATION                        TRUE
#define GDISP_NEED_CLIP                              TRUE
//#define GDISP_NEED_SCROLL                            TRUE
#define GDISP_NEED_TEXT                              TRUE
    #define GDISP_INCLUDE_FONT_UI2                   TRUE

//#define GDISP_NEED_CONTROL                           TRUE
//#define GDISP_DEFAULT_ORIENTATION                    GDISP_ROTATE_LANDSCAPE
#define GDISP_NEED_MULTITHREAD                       TRUE

#define GFX_USE_GWIN                                 TRUE
#define GWIN_NEED_WINDOWMANAGER                      TRUE
#define GWIN_NEED_CONSOLE                            TRUE
#define GWIN_NEED_WIDGET                             TRUE
    #define GWIN_NEED_KEYBOARD                       TRUE

#define GFX_USE_GEVENT                               TRUE
#define GFX_USE_GTIMER                               TRUE

#define GFX_USE_GQUEUE                               TRUE
#define GQUEUE_NEED_ASYNC                            TRUE

#define GFX_USE_GINPUT                               TRUE
#define GINPUT_NEED_MOUSE                            TRUE
//#define GINPUT_NEED_KEYBOARD                         TRUE

#endif /* _GFXCONF_H */
