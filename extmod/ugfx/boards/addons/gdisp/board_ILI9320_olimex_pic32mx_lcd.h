/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    boards/addons/gdisp/board_ILI9320_olimex_pic32mx_lcd.h
 * @brief   GDISP Graphic Driver subsystem board SPI interface for the ILI9325 display.
 *
 * @note	This file contains a mix of hardware specific and operating system specific
 *			code. You will need to change it for your CPU and/or operating system.
 */

#ifndef GDISP_LLD_BOARD_H
#define GDISP_LLD_BOARD_H

#ifndef noinline
#define noinline __attribute__((noinline))
#endif

static void init_board(GDisplay *g) {

	// As we are not using multiple displays we set g->board to NULL as we don't use it.
	g->board = 0;

	switch(g->controllerdisplay) {
	case 0:											// Set up for Display 0
		// RST
		palSetPadMode(IOPORTA, 7, PAL_MODE_OUTPUT);
		palClearPad(IOPORTA, 7);

		// RS
		palSetPadMode(IOPORTA, 10, PAL_MODE_OUTPUT);
		palSetPad(IOPORTA, 10);

		// CS
		palSetPadMode(IOPORTA, 9, PAL_MODE_OUTPUT);
		palClearPad(IOPORTA, 9);

		// Backlight
		palSetPadMode(IOPORTD, 3, PAL_MODE_OUTPUT);
		palSetPad(IOPORTD, 3);

		// PMP setup
		PMMODE = 0;
		PMAEN = 0;
		PMCON = 0;
		PMMODEbits.MODE = 2;
		PMMODEbits.WAITB = 0;
		PMMODEbits.WAITM = 1;
		PMMODEbits.WAITE = 0;
		PMCONbits.CSF = 0;
		PMCONbits.PTRDEN = 1;
		PMCONbits.PTWREN = 1;
		PMMODEbits.MODE16 = 1;
		PMCONbits.PMPEN = 1;

		palClearPad(IOPORTA, 9);
		break;
	}
}

#define PmpWaitBusy()   do {} while (PMMODEbits.BUSY)

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static noinline void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	if (state)
		palClearPad(IOPORTA, 7);
	else
		palSetPad(IOPORTA, 7);
}

static void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	if (percentage)
		palClearPad(IOPORTD, 3);
	else
		palSetPad(IOPORTD, 3);
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;
}

static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
}

static noinline void write_index(GDisplay *g, uint16_t index) {
	volatile uint16_t dummy;
	(void) g;

	PmpWaitBusy();
	palClearPad(IOPORTA, 10);
	PMDIN = index;
	PmpWaitBusy();
	palSetPad(IOPORTA, 10);

	dummy = PMDIN;
	(void)dummy;
}

static noinline void write_data(GDisplay *g, uint16_t data) {
	(void) g;
	PMDIN = data;
	PmpWaitBusy();
}

static GFXINLINE void setreadmode(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setwritemode(GDisplay *g) {
	(void) g;
}

static noinline uint16_t read_data(GDisplay *g) {
	(void) g;
	PmpWaitBusy();
	return PMDIN;
}

#endif /* GDISP_LLD_BOARD_H */
