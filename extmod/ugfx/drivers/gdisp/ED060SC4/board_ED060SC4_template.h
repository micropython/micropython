/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

/*
 * @brief	Optional parameters that can be put in this file.
 * @note	The values listed below are the defaults.
 *
 * @note	#define GDISP_SCREEN_HEIGHT		600
 * @note	#define GDISP_SCREEN_WIDTH		800
 *
 * @note	Number of pixels per byte<br>
 * 			#define EINK_PPB				4
 *
 * @note	Delay for generating clock pulses.
 *			Unit is approximate clock cycles of the CPU (0 to 15).
 *			This should be atleast 50 ns.<br>
 *			#define EINK_CLOCKDELAY			0
 *
 * @note	Width of one framebuffer block.
 *			Must be divisible by EINK_PPB and evenly divide GDISP_SCREEN_WIDTH.<br>
 *			#define EINK_BLOCKWIDTH			20
 *
 * @note
 * @note	Height of one framebuffer block.
 *			Must evenly divide GDISP_SCREEN_WIDTH.<br>
 *			#define EINK_BLOCKHEIGHT		20
 *
 * @note	Number of block buffers to use for framebuffer emulation.<br>
 * 			#define EINK_NUMBUFFERS			40
 *
 * @note	Do a "blinking" clear, i.e. clear to opposite polarity first.
 *			This reduces the image persistence.<br>
 *			#define EINK_BLINKCLEAR			TRUE
 *
 * @note	Number of passes to use when clearing the display<br>
 *			#define EINK_CLEARCOUNT			10
 *
 * @note	Number of passes to use when writing to the display<br>
 *			#define EINK_WRITECOUNT			4
 */

static GFXINLINE void init_board(GDisplay *g) {
	(void) g;
}

static void eink_delay(int us) {
	(void) us;
}

static GFXINLINE void setpower_vdd(GDisplay *g, bool_t on) {
	(void) g;
	(void) on;
}

static GFXINLINE void setpower_vneg(GDisplay *g, bool_t on) {
	(void) g;
	(void) on;
}

static GFXINLINE void setpower_vpos(GDisplay *g, bool_t on) {
	(void) g;
	(void) on;
}

static GFXINLINE void setpin_le(GDisplay *g, bool_t on) {
	(void) g;
	(void) on;
}

static GFXINLINE void setpin_oe(GDisplay *g, bool_t on) {
	(void) g;
	(void) on;
}

static GFXINLINE void setpin_cl(GDisplay *g, bool_t on) {
	(void) g;
	(void) on;
}

static GFXINLINE void setpin_sph(GDisplay *g, bool_t on) {
	(void) g;
	(void) on;
}

static GFXINLINE void setpins_data(GDisplay *g, uint8_t value) {
	(void) g;
	(void) value;
}

static GFXINLINE void setpin_ckv(GDisplay *g, bool_t on) {
	(void) g;
	(void) on;
}

static GFXINLINE void setpin_gmode(GDisplay *g, bool_t on) {
	(void) g;
	(void) on;
}

static GFXINLINE void setpin_spv(GDisplay *g, bool_t on) {
	(void) g;
	(void) on;
}

#endif /* _GDISP_LLD_BOARD_H */
