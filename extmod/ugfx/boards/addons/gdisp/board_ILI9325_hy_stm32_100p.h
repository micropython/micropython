/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/*
	driver quickly hacked together from a chinese sourcecode that came
	with the board and existing ili9320 code by Chris van Dongen (sjaak)
	(sjaak2002 at msn.com)
	
	Also added rotation for 180 and 270 degrees and minor tweaks to
	setcursor
	
	Added code comes without warranty and free bugs. Feel free to use 
	or misuse the added code :D
*/


/**
 * @file    boards/addons/gdisp/board_ILI9325_hy_stm32_100p.h
 * @brief   GDISP Graphic Driver subsystem board SPI interface for the ILI9325 display.
 *
 * @note	This file contains a mix of hardware specific and operating system specific
 *			code. You will need to change it for your CPU and/or operating system.
 */

#ifndef GDISP_LLD_BOARD_H
#define GDISP_LLD_BOARD_H

// For a multiple display configuration we would put all this in a structure and then
//	set g->board to that structure.
#define GDISP_REG              (*((volatile uint16_t *) 0x60000000)) /* RS = 0 */
#define GDISP_RAM              (*((volatile uint16_t *) 0x60020000)) /* RS = 1 */

static GFXINLINE void init_board(GDisplay *g) {

	// As we are not using multiple displays we set g->board to NULL as we don't use it.
	g->board = 0;

	switch(g->controllerdisplay) {
	case 0:											// Set up for Display 0
		/* FSMC setup for F1 */
		rccEnableAHB(RCC_AHBENR_FSMCEN, 0);

		/* set pin modes */
	/*	IOBus busD = {GPIOD, PAL_WHOLE_PORT, 0};
		IOBus busE = {GPIOE, PAL_WHOLE_PORT, 0};
		palSetBusMode(&busD, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
		palSetBusMode(&busE, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
		palSetPadMode(GPIOE, GPIOE_TFT_RST, PAL_MODE_OUTPUT_PUSHPULL);
		palSetPadMode(GPIOD, GPIOD_TFT_LIGHT, PAL_MODE_OUTPUT_PUSHPULL); */

		const unsigned char FSMC_Bank = 0;

		/* FSMC timing */
		FSMC_Bank1->BTCR[FSMC_Bank+1] = (6) | (10 << 8) | (10 << 16);

		/* Bank1 NOR/SRAM control register configuration
		 * This is actually not needed as already set by default after reset */
		FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
		break;
	}
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	if(state)
		palClearPad(GPIOE, GPIOE_TFT_RST);
	else
		palSetPad(GPIOE, GPIOE_TFT_RST);
}

static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	(void)percent;
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

#endif /* GDISP_LLD_BOARD_H */
