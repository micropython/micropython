/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GINPUT_LLD_DIAL_BOARD_H
#define _GINPUT_LLD_DIAL_BOARD_H

#if GFX_USE_GINPUT && GINPUT_NEED_DIAL

/*===========================================================================*/
/* Analogue devices on this board                                            */
/*===========================================================================*/

/* Example values */
#define GINPUT_DIAL_NUM_PORTS			1
#define GINPUT_DIAL_DEVICE0				GADC_PHYSDEV_DIAL
#define GINPUT_DIAL_POLL_PERIOD			200
#define GINPUT_DIAL_CYCLE_POLL			FALSE

#endif	/* GFX_USE_GINPUT && GINPUT_NEED_DIAL */

#endif	/* _GINPUT_LLD_DIAL_BOARD_H */
