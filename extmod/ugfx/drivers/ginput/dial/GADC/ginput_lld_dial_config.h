/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GINPUT_LLD_DIAL_CONFIG_H
#define _GINPUT_LLD_DIAL_CONFIG_H

#if GFX_USE_GINPUT && GINPUT_NEED_DIAL

#define GINPUT_DIAL_MAX_VALUE	((1<<GADC_BITS_PER_SAMPLE)-1)

/* Include the user supplied board definitions */
#include "ginput_lld_dial_board.h"

#endif	/* GFX_USE_GDISP && GINPUT_NEED_DIAL */

#endif	/* _GINPUT_LLD_DIAL_CONFIG_H */
