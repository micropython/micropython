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

/* Using FSMC A19 (PE3) as DC */
#define GDISP_REG (*((volatile uint16_t *) 0x60000000)) /* DC = 0 */
#define GDISP_RAM (*((volatile uint16_t *) 0x60100000)) /* DC = 1 */
#define GDISP_DMA_STREAM STM32_DMA2_STREAM6

#define SET_RST palSetPad(GPIOD, 3);
#define CLR_RST palClearPad(GPIOD, 3);

/*
 * PWM configuration structure. We use timer 4 channel 2 (orange LED on board).
 * The reason for so high clock is that with any lower, onboard coil is squeaking.
 * The major disadvantage of this clock is a lack of linearity between PWM duty
 * cycle width and brightness. In fact only with low preset one sees any change
 * (eg. duty cycle between 1-20). Feel free to adjust this, maybe only my board
 * behaves like this. According to the G5126 datesheet (backlight LED driver)
 * the PWM frequency should be somewhere between 200 Hz to 200 kHz.
 */
static const PWMConfig pwmcfg = {
	20000,			/* 20 KHz PWM clock frequency. */
	100,			/* PWM period is 100 cycles. */
	0,
	{
		{PWM_OUTPUT_ACTIVE_HIGH, 0},
		{PWM_OUTPUT_ACTIVE_HIGH, 0},
		{PWM_OUTPUT_ACTIVE_HIGH, 0},
		{PWM_OUTPUT_ACTIVE_HIGH, 0}
	},
	0,
	0
};

static GFXINLINE void init_board(GDisplay *g) {

	// As we are not using multiple displays we set g->board to NULL as we don't use it.
	g->board = 0;

	switch(g->controllerdisplay) {
	case 0:											// Set up for Display 0
		#if defined(STM32F4XX) || defined(STM32F2XX)
			/* STM32F4 FSMC init */
			rccEnableAHB3(RCC_AHB3ENR_FSMCEN, 0);

			#if defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)
				if (dmaStreamAllocate(GDISP_DMA_STREAM, 0, 0, 0))
					gfxExit();
				dmaStreamSetMemory0(GDISP_DMA_STREAM, &GDISP_RAM);
				dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) | STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD | STM32_DMA_CR_DIR_M2M);
			#endif
		#else
			#error "FSMC not implemented for this device"
		#endif

		/* Group pins */
		IOBus busD = {GPIOD, (1 << 0) | (1 << 1) | (1 << 4) | (1 << 5) | (1 << 7) | (1 << 8) |
								(1 << 9) | (1 << 10) | (1 << 14) | (1 << 15), 0};

		IOBus busE = {GPIOE, (1 << 3) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) |
								(1 << 13) | (1 << 14) | (1 << 15), 0};

		/* FSMC is an alternate function 12 (AF12) */
		palSetBusMode(&busD, PAL_MODE_ALTERNATE(12));
		palSetBusMode(&busE, PAL_MODE_ALTERNATE(12));

		/* FSMC timing register configuration */
//		FSMC_Bank1->BTCR[0 + 1] = (FSMC_BTR1_ADDSET_2 | FSMC_BTR1_ADDSET_1) \
//				| (FSMC_BTR1_DATAST_2 | FSMC_BTR1_DATAST_1) \
//				| FSMC_BTR1_BUSTURN_0;
		FSMC_Bank1->BTCR[0 + 1] = (FSMC_BTR1_ADDSET_3 | FSMC_BTR1_ADDSET_0) \
				| (FSMC_BTR1_DATAST_3 | FSMC_BTR1_DATAST_0) \
				| FSMC_BTR1_BUSTURN_0;

		/* Bank1 NOR/PSRAM control register configuration
		 * Write enable, memory databus width set to 16 bit, memory bank enable */
		FSMC_Bank1->BTCR[0] = FSMC_BCR1_WREN | FSMC_BCR1_MWID_0 | FSMC_BCR1_MBKEN;

		/* Display backlight control */
		/* TIM4 is an alternate function 2 (AF2) */
		pwmStart(&PWMD4, &pwmcfg);
		palSetPadMode(GPIOD, 13, PAL_MODE_ALTERNATE(2));
		pwmEnableChannel(&PWMD4, 1, 100);
		break;
	}
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	if (state) {
		CLR_RST;
	} else {
		SET_RST;
	}
}

static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	pwmEnableChannel(&PWMD4, 1, percent);
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

#if defined(GDISP_USE_DMA)
	static GFXINLINE void dma_with_noinc(GDisplay *g, color_t *buffer, int area) {
		(void) g;
		dmaStreamSetPeripheral(GDISP_DMA_STREAM, buffer);
		dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) | STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD | STM32_DMA_CR_DIR_M2M);
		for (; area > 0; area -= 65535) {
			dmaStreamSetTransactionSize(GDISP_DMA_STREAM, area > 65535 ? 65535 : area);
			dmaStreamEnable(GDISP_DMA_STREAM);
			dmaWaitCompletion(GDISP_DMA_STREAM);
		}
	}

	static GFXINLINE void dma_with_inc(GDisplay *g, color_t *buffer, int area) {
		(void) g;
        dmaStreamSetPeripheral(GDISP_DMA_STREAM, buffer);
        dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) | STM32_DMA_CR_PINC | STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD | STM32_DMA_CR_DIR_M2M);
		for (; area > 0; area -= 65535) {
			dmaStreamSetTransactionSize(GDISP_DMA_STREAM, area > 65535 ? 65535 : area);
			dmaStreamEnable(GDISP_DMA_STREAM);
			dmaWaitCompletion(GDISP_DMA_STREAM);
		}
	}
#endif

#endif /* _GDISP_LLD_BOARD_H */

