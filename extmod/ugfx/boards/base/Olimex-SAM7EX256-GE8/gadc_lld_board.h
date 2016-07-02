/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    boards/base/Olimex-SAM7EX256-GE8/gadc_lld_board.h
 * @brief   GADC Driver config file.
 */

#ifndef _GADC_LLD_BOARD_H
#define _GADC_LLD_BOARD_H

#if GFX_USE_GADC

/*===========================================================================*/
/* Analogue devices on this board                                            */
/*===========================================================================*/

#define GADC_PHYSDEV_MICROPHONE			0x00000080
#define GADC_PHYSDEV_DIAL				0x00000040
#define GADC_PHYSDEV_TEMPERATURE		0x00000020

#endif	/* GFX_USE_GADC */

#endif	/* _GADC_LLD_BOARD_H */

