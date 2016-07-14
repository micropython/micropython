/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

/*
 * Set various display properties. These properties mostly depend on the exact controller chip you get.
 * The defaults should work for most controllers.
 */
//#define GDISP_SCREEN_HEIGHT				130		// The visible display height
//#define GDISP_SCREEN_WIDTH				130		// The visible display width
//#define GDISP_RAM_X_OFFSET				0		// The x offset of the visible area
//#define GDISP_RAM_Y_OFFSET				2		// The y offset of the visible area
//#define GDISP_SLEEP_POS					50		// The position of the sleep mode partial display
//#define GDISP_INITIAL_CONTRAST			50		// The initial contrast percentage
//#define GDISP_INITIAL_BACKLIGHT			100		// The initial backlight percentage

static GFXINLINE void init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	(void) state;
}

static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	(void) percent;
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;
}

static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
}

static GFXINLINE void write_index(GDisplay *g, uint16_t index) {
	(void) g;
	(void) index;
}

static GFXINLINE void write_data(GDisplay *g, uint16_t data) {
	(void) g;
	(void) data;
}

#endif /* _GDISP_LLD_BOARD_H */
