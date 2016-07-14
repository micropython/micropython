/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#define GNETCODE_VERSION			GNETCODE_VERSION_1_0		// The current protocol version

// The list of possible protocol version numbers
#define GNETCODE_VERSION_1_0		0x0100		// V1.0

// The required pixel format
#define GNETCODE_PIXELFORMAT		GDISP_PIXELFORMAT_RGB565

// The default TCP/IP port
#define GNETCODE_DEFAULT_PORT		13001

/**
 * All commands are sent in 16 bit blocks (2 bytes) in network order (BigEndian)
 * Across all uGFXnet protocol versions, the stream will always start with GNETCODE_INIT (0xFFFF) and then the version number.
 */
#define GNETCODE_INIT			0xFFFF		// Followed by version,width,height,pixelformat,hasmouse
#define GNETCODE_FLUSH			0x0000		// No following data
#define GNETCODE_PIXEL			0x0001		// Followed by x,y,color
#define GNETCODE_FILL			0x0002		// Followed by x,y,cx,cy,color
#define GNETCODE_BLIT			0x0003		// Followed by x,y,cx,cy,bits
#define GNETCODE_READ			0x0004		// Followed by x,y - Response is GNETCODE_READ,color
#define GNETCODE_SCROLL			0x0005		// Followed by x,y,cx,cy,lines
#define GNETCODE_CONTROL		0x0006		// Followed by what,data - Response is GNETCODE_CONTROL,0x0000 (fail) or GNETCODE_CONTROL,0x0001 (success)
#define GNETCODE_MOUSE_X		0x0007		// This is only ever received - never sent. Response is GNETCODE_MOUSE_X,x
#define GNETCODE_MOUSE_Y		0x0008		// This is only ever received - never sent. Response is GNETCODE_MOUSE_Y,y
#define GNETCODE_MOUSE_B		0x0009		// This is only ever received - never sent. Response is GNETCODE_MOUSE_B,buttons. This is also the sync signal for mouse updates.
#define GNETCODE_KILL			0xFFFE		// This is only ever received - never sent. Response is GNETCODE_KILL,retcode
