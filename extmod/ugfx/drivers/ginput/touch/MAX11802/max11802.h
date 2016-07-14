/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _MAX11802_H
#define _MAX11802_H

#define MAX11802_CMD_XPOSITION 	((0x52 << 1) | 1)
#define MAX11802_CMD_YPOSITION 	((0x54 << 1) | 1)
#define MAX11802_CMD_ZPOSITION 	((0x56 << 1) | 1)

// LSB of register addresses specifies read (1) or write (0)
#define MAX11802_CMD_MEASUREXY 	(0x70 << 1)
#define MAX11802_CMD_MEASUREXYZ	(0x72 << 1)
#define MAX11802_CMD_GEN_WR		(0x01 << 1)				// General config register
#define MAX11802_CMD_RES_WR		(0x02 << 1)
#define MAX11802_CMD_AVG_WR    	(0x03 << 1)
#define MAX11802_CMD_SAMPLE_WR	(0x04 << 1)
#define MAX11802_CMD_TIMING_WR	(0x05 << 1)
#define MAX11802_CMD_DELAY_WR  	(0x06 << 1)
#define MAX11802_CMD_TDPULL_WR	(0x07 << 1)
#define MAX11802_CMD_MDTIM_WR	(0x08 << 1)
#define MAX11802_CMD_APCONF_WR	(0x09 << 1)
#define MAX11802_CMD_MODE_WR   	(0x0B << 1)
#define MAX11802_CMD_MODE_RD   	((0x0B << 1) | 1)
#define MAX11802_CMD_GSR_RD   	((0x00 << 1) | 1)		// General status register - read-only

#endif /* _MAX11802_H */
