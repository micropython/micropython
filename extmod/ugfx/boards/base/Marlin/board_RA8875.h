/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/RA8875/board_RA8875_marlin.h
 * @brief   GDISP Graphic Driver subsystem board interface for the RA8875 display.
 */

#ifndef _BOARD_RA8875_H
#define _BOARD_RA8875_H

// For a multiple display configuration we would put all this in a structure and then
// set g->board to that structure.
#define GDISP_RAM              (*((volatile uint16_t *) 0x68000000)) /* RS = 0 */
#define GDISP_REG              (*((volatile uint16_t *) 0x68020000)) /* RS = 1 */
#define FSMC_BANK				4


static GFXINLINE void init_board(GDisplay *g) {
	// As we are not using multiple displays we set g->board to NULL as we don't use it.
	g->board = 0;

	switch(g->controllerdisplay) {
		// setup for display 0
		case 0: {		

			// enable the FSMC peripheral
			rccEnableAHB3(RCC_AHB3ENR_FSMCEN, 0);

			// setup the pin modes for FSMC
			IOBus busD = {GPIOD, (1 << 0) | (1 << 1) | (1 << 4) | (1 << 5) | (1 << 8) |
									(1 << 9) | (1 << 10) | (1 << 11) | (1 << 14) | (1 << 15), 0};

			IOBus busE = {GPIOE, (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) |
								(1 << 13) | (1 << 14) | (1 << 15), 0};

			IOBus busG = {GPIOG, (1 << 10), 0};

			palSetBusMode(&busD, PAL_MODE_ALTERNATE(12));
			palSetBusMode(&busE, PAL_MODE_ALTERNATE(12));
			palSetBusMode(&busG, PAL_MODE_ALTERNATE(12));

			// FSMC timing
			FSMC_Bank1->BTCR[FSMC_BANK+1] = (FSMC_BTR1_ADDSET_1 | FSMC_BTR1_ADDSET_3) \
					| (FSMC_BTR1_DATAST_1 | FSMC_BTR1_DATAST_3) \
					| (FSMC_BTR1_BUSTURN_1 | FSMC_BTR1_BUSTURN_3) ;

			// Bank1 NOR/SRAM control register configuration
			// This is actually not needed as already set by default after reset
			FSMC_Bank1->BTCR[FSMC_BANK] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;

			break;
		}

		// marlin does not have any secondary display so far
		default:
			break;
	}
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
	
	// FSMC delay reduced as the controller now runs at full speed
	FSMC_Bank1->BTCR[2+1] = FSMC_BTR1_ADDSET_0 | FSMC_BTR1_DATAST_2 | FSMC_BTR1_BUSTURN_0 ;
	FSMC_Bank1->BTCR[2] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	(void) state;
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;
}

static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
}

static GFXINLINE void write_index(GDisplay *g, uint16_t index) {
	(void) g;

	GDISP_REG = index;
}

static GFXINLINE void write_data(GDisplay *g, uint16_t data) {
	(void) g;

	GDISP_RAM = data;
}

static GFXINLINE void setreadmode(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setwritemode(GDisplay *g) {
	(void) g;
}

static GFXINLINE uint16_t read_data(GDisplay *g) {
	(void) g;

	return GDISP_RAM;
}

#endif /* _BOARD_RA8875_H */

