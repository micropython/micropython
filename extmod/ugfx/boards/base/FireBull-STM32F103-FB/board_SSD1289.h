/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

// For a multiple display configuration we would put all this in a structure and then
//	set g->board to that structure.
#define SET_CS		palSetPad(GPIOD, 12);
#define CLR_CS		palClearPad(GPIOD, 12);
#define SET_RS		palSetPad(GPIOD, 13);
#define CLR_RS		palClearPad(GPIOD, 13);
#define SET_WR		palSetPad(GPIOD, 14);
#define CLR_WR		palClearPad(GPIOD, 14);
#define SET_RD		palSetPad(GPIOD, 15);
#define CLR_RD		palClearPad(GPIOD, 15);

static GFXINLINE void init_board(GDisplay *g)
{

	// As we are not using multiple displays we set g->board to NULL as we don't use it.
	g->board = 0;

	switch(g->controllerdisplay) {
	case 0:											// Set up for Display 0
		palSetGroupMode(GPIOE, PAL_WHOLE_PORT, 0, PAL_MODE_OUTPUT_PUSHPULL);
		palSetPadMode(GPIOD, 12, PAL_MODE_OUTPUT_PUSHPULL);
		palSetPadMode(GPIOD, 13, PAL_MODE_OUTPUT_PUSHPULL);
		palSetPadMode(GPIOD, 14, PAL_MODE_OUTPUT_PUSHPULL);
		palSetPadMode(GPIOD, 15, PAL_MODE_OUTPUT_PUSHPULL);

		// Configure the pins to a well know state
		SET_RS;
		SET_RD;
		SET_WR;
		CLR_CS;

		break;
	}
}

static GFXINLINE void post_init_board(GDisplay *g)
{
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state)
{
	(void) g;
	(void) state;

	/* Nothing to do here - reset pin tied to Vcc */
}

static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent)
{
	(void) g;
	(void) percent;

	/* Nothing to do here - Backlight always on */
}

static GFXINLINE void acquire_bus(GDisplay *g)
{
	(void) g;
}

static GFXINLINE void release_bus(GDisplay *g)
{
	(void) g;
}

static GFXINLINE void write_index(GDisplay *g, uint16_t index)
{
	(void) g;

	palWritePort(GPIOE, index);
	CLR_RS;
	CLR_WR;
	SET_WR;
	SET_RS;
}

static GFXINLINE void write_data(GDisplay *g, uint16_t data)
{
	(void) g;

	palWritePort(GPIOE, data);
	CLR_WR;
	SET_WR;
}

static GFXINLINE void setreadmode(GDisplay *g)
{
	(void) g;

	// change pin mode to digital input
	palSetGroupMode(GPIOE, PAL_WHOLE_PORT, 0, PAL_MODE_INPUT);
	CLR_RD;
}

static GFXINLINE void setwritemode(GDisplay *g)
{
	(void) g;
	
	// change pin mode back to digital output
	SET_RD;
	palSetGroupMode(GPIOE, PAL_WHOLE_PORT, 0, PAL_MODE_OUTPUT_PUSHPULL);
}

static GFXINLINE uint16_t read_data(GDisplay *g) {
	(void) g;

	return palReadPort(GPIOE);
}

#if defined(GDISP_USE_DMA)
	#error "GDISP - SSD1289: The GPIO interface does not support DMA"
#endif

#endif /* _GDISP_LLD_BOARD_H */

