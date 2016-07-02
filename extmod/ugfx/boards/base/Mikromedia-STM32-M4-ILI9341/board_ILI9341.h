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
#define SET_CS    palSetPad(GPIOE, GPIOE_LCD_CS);
#define CLR_CS    palClearPad(GPIOE, GPIOE_LCD_CS);
#define SET_RS    palSetPad(GPIOE, GPIOE_LCD_RS);
#define CLR_RS    palClearPad(GPIOE, GPIOE_LCD_RS);
#define SET_WR    palSetPad(GPIOE, GPIOE_PMWR);
#define CLR_WR    palClearPad(GPIOE, GPIOE_PMWR);
#define SET_RD    palSetPad(GPIOE, GPIOE_PMRD);
#define CLR_RD    palClearPad(GPIOE, GPIOE_PMRD);

static GFXINLINE void init_board(GDisplay *g) {

	// As we are not using multiple displays we set g->board to NULL as we don't use it.
	g->board = 0;

	switch(g->controllerdisplay) {
	case 0:											// Set up for Display 0
		/* Configure the pins to a well know state */
		SET_RS;
		SET_RD;
		SET_WR;
		CLR_CS;
		break;
	}
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	if(state) {
		// reset lcd
		palClearPad(GPIOE, GPIOE_LCD_RST);
	} else {
		palSetPad(GPIOE, GPIOE_LCD_RST);
	}
}

static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	// TODO: can probably pwm this
	if(percent) {
		// turn back light on
		palSetPad(GPIOE, GPIOE_LCD_BLED);
	} else {
		// turn off
		palClearPad(GPIOE, GPIOE_LCD_BLED);
	}
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;
}

static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
}

/**
 * @brief   Short delay
 *
 * @param[in] dly		Length of delay
 *
 * @notapi
 */
static GFXINLINE void ili9341_delay(uint16_t dly) {
  static uint16_t i;
  for(i = 0; i < dly; i++)
    asm("nop");
}

static GFXINLINE void write_index(GDisplay *g, uint16_t index) {
	(void) g;
	palWriteGroup(GPIOE, 0x00FF, 0, index);
	CLR_RS; CLR_WR; ili9341_delay(1); SET_WR; ili9341_delay(1); SET_RS;
}

static GFXINLINE void write_data(GDisplay *g, uint16_t data) {
	(void) g;
	palWriteGroup(GPIOE, 0x00FF, 0, data);
	CLR_WR; ili9341_delay(1); SET_WR; ili9341_delay(1);
}

static GFXINLINE void setreadmode(GDisplay *g) {
	(void) g;
	// change pin mode to digital input
	palSetGroupMode(GPIOE, PAL_WHOLE_PORT, 0, PAL_MODE_INPUT);
}

static GFXINLINE void setwritemode(GDisplay *g) {
	(void) g;
	// change pin mode back to digital output
	palSetGroupMode(GPIOE, PAL_WHOLE_PORT, 0, PAL_MODE_OUTPUT_PUSHPULL);
}

static GFXINLINE uint16_t read_data(GDisplay *g) {
	uint16_t	value;
	(void) g;
	CLR_RD;
	value = palReadPort(GPIOE);
	value = palReadPort(GPIOE);
	SET_RD;
	return value;
}

#endif /* _GDISP_LLD_BOARD_H */
