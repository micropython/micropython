/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/ginput/ginput_driver_dial.h
 * @brief   GINPUT header file for dial drivers.
 *
 * @defgroup Dial Dial
 * @ingroup GINPUT
 * @{
 */

#ifndef _LLD_GINPUT_DIAL_H
#define _LLD_GINPUT_DIAL_H

#if GINPUT_NEED_DIAL || defined(__DOXYGEN__)

#include "ginput_lld_dial_config.h"

typedef void (*DialCallbackFn)(uint16_t instance, uint16_t rawvalue);

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

	void ginput_lld_dial_init(void);
	void ginput_lld_dial_poll(DialCallbackFn fn);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GINPUT && GINPUT_NEED_TOGGLE */

#endif /* _LLD_GINPUT_TOGGLE_H */
/** @} */

