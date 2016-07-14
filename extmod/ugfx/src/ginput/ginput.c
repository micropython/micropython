/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/ginput/ginput.c
 * @brief   GINPUT subsystem common code.
 *
 * @addtogroup GINPUT
 * @{
 */
#include "../../gfx.h"

#if GFX_USE_GINPUT

#if GINPUT_NEED_MOUSE
    extern void _gmouseInit(void);
    extern void _gmouseDeinit(void);
#endif
#if GINPUT_NEED_KEYBOARD
    extern void _gkeyboardInit(void);
    extern void _gkeyboardDeinit(void);
#endif

void _ginputInit(void)
{
    #if GINPUT_NEED_MOUSE
        _gmouseInit();
    #endif
	#if GINPUT_NEED_KEYBOARD
		_gkeyboardInit();
	#endif
	/**
	 * This should really call an init routine for each ginput sub-system.
	 * Maybe we'll do this later.
	 */
}

void _ginputDeinit(void)
{
	#if GINPUT_NEED_KEYBOARD
		_gkeyboardDeinit();
	#endif
    #if GINPUT_NEED_MOUSE
        _gmouseDeinit();
    #endif
}

#endif /* GFX_USE_GINPUT */
/** @} */

