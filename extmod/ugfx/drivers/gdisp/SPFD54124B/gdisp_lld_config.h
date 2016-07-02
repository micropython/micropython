/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/* 
 *	Driver for display from Nokia 1616 phone
 *	based on the work of axillent@gmail.com (http://radiokot.ru/articles/53/)
 *	adapted by shilow@ukr.net with the help of Federico Reds <fede.677387@hotmail.it>
 */

#ifndef _GDISP_LLD_CONFIG_H
#define _GDISP_LLD_CONFIG_H

#if GFX_USE_GDISP 

/*===========================================================================*/
/* Driver hardware support.                                                  */
/*===========================================================================*/

#define GDISP_HARDWARE_CONTROL		TRUE
#define GDISP_HARDWARE_STREAM_WRITE	TRUE

#define GDISP_LLD_PIXELFORMAT		GDISP_PIXELFORMAT_RGB565

#define SPFD54124B_SEND_CMD		0x00
#define SPFD54124B_SEND_DATA		0x100

#define SPFD54124B_CMD_NOP		0x00
#define SPFD54124B_CMD_SLPIN		0x11
#define SPFD54124B_CMD_SLPOUT		0x11
#define SPFD54124B_CMD_NORON		0x13

#define SPFD54124B_CMD_INVOFF		0x20
#define SPFD54124B_CMD_DISPON		0x29
#define SPFD54124B_CMD_CASET		0x2A
#define SPFD54124B_CMD_RASET		0x2B
#define SPFD54124B_CMD_RAMWR		0x2C
#define SPFD54124B_CMD_RGBSET		0x2D
#define SPFD54124B_CMD_PTLAR		0x30

#define SPFD54124B_CMD_MADCTR		0x36
#define SPFD54124B_CMD_VSCSAD		0x37
#define SPFD54124B_CMD_COLMOD		0x3A

// MCU interface 12bit
#define SPFD54124B_CMD_COLMOD_MCU12bit	0x03
// MCU interface 16bit
#define SPFD54124B_CMD_COLMOD_MCU16bit	0x05
// MCU interface 18bit
#define SPFD54124B_CMD_COLMOD_MCU18bit	0x06
// RGB interface 16bit
#define SPFD54124B_CMD_COLMOD_RGB16bit	0x50
// RGB interface 18bit
#define SPFD54124B_CMD_COLMOD_RGB18bit	0x60

// Row Address Order
#define SPFD54124B_CMD_MADCTR_MY	(1 << 7)
// Column Address Order
#define SPFD54124B_CMD_MADCTR_MX	(1 << 6)
// Row/Column Exchange
#define SPFD54124B_CMD_MADCTR_MV	(1 << 5)
// Vertical Refresh Order
#define SPFD54124B_CMD_MADCTR_ML	(1 << 4)
// RGB-BGR ORDER
#define SPFD54124B_CMD_MADCTR_RGB	(1 << 3)

#endif	/* GFX_USE_GDISP */

#endif	/* _GDISP_LLD_CONFIG_H */

