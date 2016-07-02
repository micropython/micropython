/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    boards/addons/gdisp/board_SSD1289_stm32f4discovery.h
 * @brief   GDISP Graphic Driver subsystem board interface for the SSD1289 display.
 *
 * @note	This file contains a mix of hardware specific and operating system specific
 *			code. You will need to change it for your CPU and/or operating system.
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

// For a multiple display configuration we would put all this in a structure and then
//	set g->board to that structure.
#define GDISP_REG              ((volatile uint16_t *) 0x60000000)[0] /* RS = 0 */
#define GDISP_RAM              ((volatile uint16_t *) 0x60020000)[0] /* RS = 1 */
#define GDISP_DMA_STREAM		STM32_DMA2_STREAM6
#define FSMC_BANK				0

/* PWM configuration structure. We use timer 3 channel 3 */
static const PWMConfig pwmcfg = {
	100000,       /* 100 kHz PWM clock frequency. */
	100,          /* PWM period is 100 cycles. */
	0,
	{
		{PWM_OUTPUT_DISABLED, 0},
		{PWM_OUTPUT_DISABLED, 0},
		{PWM_OUTPUT_ACTIVE_HIGH, 0},
		{PWM_OUTPUT_DISABLED, 0}
	},
	0,
	0
};

static GFXINLINE void init_board(GDisplay *g) {

	// As we are not using multiple displays we set g->board to NULL as we don't use it.
	g->board = 0;

	switch(g->controllerdisplay) {
	case 0:											// Set up for Display 0
		/**
		 * Performs the following functions:
		 *		1. initialise the io port used by the display
		 *		2. initialise the reset pin (initial state not-in-reset)
		 *		3. initialise the chip select pin (initial state not-active)
		 *		4. initialise the backlight pin (initial state back-light off)
		 */

		#if defined(STM32F1XX) || defined(STM32F3XX)
			/* FSMC setup for F1/F3 */
			rccEnableAHB(RCC_AHBENR_FSMCEN, 0);

			#if defined(GDISP_USE_DMA)
				#error "GDISP: SSD1289 - DMA not implemented for F1/F3 Devices"
			#endif
		#elif defined(STM32F4XX) || defined(STM32F2XX)
			/* STM32F2-F4 FSMC init */
			rccEnableAHB3(RCC_AHB3ENR_FSMCEN, 0);

			#if defined(GDISP_USE_DMA)
				if (dmaStreamAllocate(GDISP_DMA_STREAM, 0, 0, 0)) gfxExit();
				dmaStreamSetMemory0(GDISP_DMA_STREAM, &GDISP_RAM);
				dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) | STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD | STM32_DMA_CR_DIR_M2M);
			#else
				#warning "GDISP: SSD1289 - DMA is supported for F2/F4 Devices. Define GDISP_USE_DMA in your gfxconf.h to turn this on for better performance."
			#endif
		#else
			#error "GDISP: SSD1289 - FSMC not implemented for this device"
		#endif

		/* set pins to FSMC mode */
		IOBus busD = {GPIOD, (1 << 0) | (1 << 1) | (1 << 4) | (1 << 5) | (1 << 7) | (1 << 8) |
								(1 << 9) | (1 << 10) | (1 << 11) | (1 << 14) | (1 << 15), 0};

		IOBus busE = {GPIOE, (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) |
							(1 << 13) | (1 << 14) | (1 << 15), 0};

		palSetBusMode(&busD, PAL_MODE_ALTERNATE(12));
		palSetBusMode(&busE, PAL_MODE_ALTERNATE(12));

		/* FSMC timing */
		FSMC_Bank1->BTCR[FSMC_BANK+1] = FSMC_BTR1_ADDSET_0 | FSMC_BTR1_DATAST_2 | FSMC_BTR1_BUSTURN_0 ;

		/* Bank1 NOR/SRAM control register configuration
		 * This is actually not needed as already set by default after reset */
		FSMC_Bank1->BTCR[FSMC_BANK] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;

		/* Display backlight control */
		/* TIM3 is an alternate function 2 (AF2) */
		pwmStart(&PWMD3, &pwmcfg);
		palSetPadMode(GPIOB, 0, PAL_MODE_ALTERNATE(2));
		pwmEnableChannel(&PWMD3, 2, 100);
		break;
	}
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
    pwmEnableChannel(&PWMD3, 2, percent);
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
	FSMC_Bank1->BTCR[FSMC_BANK+1] = FSMC_BTR1_ADDSET_3 | FSMC_BTR1_DATAST_3 | FSMC_BTR1_BUSTURN_0;		/* FSMC timing */
}

static GFXINLINE void setwritemode(GDisplay *g) {
	(void) g;
	FSMC_Bank1->BTCR[FSMC_BANK+1] = FSMC_BTR1_ADDSET_0 | FSMC_BTR1_DATAST_2 | FSMC_BTR1_BUSTURN_0;		/* FSMC timing */
}

static GFXINLINE uint16_t read_data(GDisplay *g) {
	(void) g;
	return GDISP_RAM;
}

#if defined(GDISP_USE_DMA) || defined(__DOXYGEN__)
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

