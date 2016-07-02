/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

/**
 * @file    boards/addons/gdisp/board_SSD1963_fsmc.h
 * @brief   GDISP Graphic Driver subsystem board interface for the SSD1963 display.
 *
 * @note	This file contains a mix of hardware specific and operating system specific
 *			code. You will need to change it for your CPU and/or operating system.
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

static const LCD_Parameters	DisplayTimings[] = {
	// You need one of these array elements per display
	{
		480, 272,								// Panel width and height
		2, 2, 41,								// Horizontal Timings (back porch, front porch, pulse)
		CALC_PERIOD(480,2,2,41),				// Total Horizontal Period (calculated from above line)
		2, 2, 10,								// Vertical Timings (back porch, front porch, pulse)
		CALC_PERIOD(272,2,2,10),				// Total Vertical Period (calculated from above line)
		CALC_FPR(480,272,2,2,41,2,2,10,60ULL)	// FPR - the 60ULL is the frames per second. Note the ULL!
	},
};

// For a multiple display configuration we would put all this in a structure and then
//	set g->board to that structure.

/* Using FSMC A16 as RS */
#define GDISP_REG              (*((volatile uint16_t *) 0x60000000)) /* RS = 0 */
#define GDISP_RAM              (*((volatile uint16_t *) 0x60020000)) /* RS = 1 */

static GFXINLINE void init_board(GDisplay *g) {

	// As we are not using multiple displays we set g->board to NULL as we don't use it.
	g->board = 0;

	switch(g->controllerdisplay) {
	case 0:											// Set up for Display 0
		#if defined(STM32F1XX) || defined(STM32F3XX)
			/* FSMC setup for F1/F3 */
			rccEnableAHB(RCC_AHBENR_FSMCEN, 0);
		#elif defined(STM32F4XX) || defined(STM32F2XX)
			/* STM32F2-F4 FSMC init */
			rccEnableAHB3(RCC_AHB3ENR_FSMCEN, 0);
		#else
			#error "FSMC not implemented for this device"
		#endif

		/* set pins to FSMC mode */
		IOBus busD = {GPIOD, (1 << 0) | (1 << 1) | (1 << 4) | (1 << 5) | (1 << 7) | (1 << 8) |
								(1 << 9) | (1 << 10) | (1 << 11) | (1 << 14) | (1 << 15), 0};

		IOBus busE = {GPIOE, (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) |
							(1 << 13) | (1 << 14) | (1 << 15), 0};
	
		palSetBusMode(&busD, PAL_MODE_ALTERNATE(12));
		palSetBusMode(&busE, PAL_MODE_ALTERNATE(12));

		/* FSMC timing */
		FSMC_Bank1->BTCR[0+1] = (FSMC_BTR1_ADDSET_1 | FSMC_BTR1_ADDSET_3) \
				| (FSMC_BTR1_DATAST_1 | FSMC_BTR1_DATAST_3) \
				| (FSMC_BTR1_BUSTURN_1 | FSMC_BTR1_BUSTURN_3) ;
	
		/* Bank1 NOR/SRAM control register configuration
		 * This is actually not needed as already set by default after reset */
		FSMC_Bank1->BTCR[0] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
		break;
	}
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
	/* FSMC delay reduced as the controller now runs at full speed */
	FSMC_Bank1->BTCR[0+1] = FSMC_BTR1_ADDSET_0 | FSMC_BTR1_DATAST_2 | FSMC_BTR1_BUSTURN_0 ;
	FSMC_Bank1->BTCR[0] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
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

#endif /* _GDISP_LLD_BOARD_H */

