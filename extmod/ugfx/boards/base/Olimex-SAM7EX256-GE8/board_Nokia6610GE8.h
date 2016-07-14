/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    boards/base/Olimex-SAM7EX256-GE8/board_Nokia6610GE8.h
 * @brief   GDISP Graphic Driver subsystem board interface for the Olimex SAM7-EX256 board.
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

/*
 * Set various display properties. These properties mostly depend on the exact controller chip you get.
 * The defaults should work for most controllers.
 */
//#define GDISP_GE8_BROKEN_CONTROLLER		FALSE	// Uncomment this out if you have a controller thats not window wrap broken.
//#define GDISP_SCREEN_HEIGHT				130		// The visible display height
//#define GDISP_SCREEN_WIDTH				130		// The visible display width
//#define GDISP_RAM_X_OFFSET				0		// The x offset of the visible area
//#define GDISP_RAM_Y_OFFSET				2		// The y offset of the visible area
//#define GDISP_SLEEP_SIZE					32		// The size of the sleep mode partial display
//#define GDISP_SLEEP_POS					50		// The position of the sleep mode partial display
//#define GDISP_INITIAL_CONTRAST			38		// The initial contrast percentage
//#define GDISP_INITIAL_BACKLIGHT			100		// The initial backlight percentage

// For a multiple display configuration we would put all this in a structure and then
//	set g->board to that structure.

// ******************************************************
// Pointers to AT91SAM7X256 peripheral data structures
// ******************************************************
static volatile AT91PS_PIO pPIOA = AT91C_BASE_PIOA;
static volatile AT91PS_PIO pPIOB = AT91C_BASE_PIOB;
static volatile AT91PS_SPI pSPI = AT91C_BASE_SPI0;
static volatile AT91PS_PMC pPMC = AT91C_BASE_PMC;

/* The PWM backlight control is non-linear on this board.
 * We pick values here that make it look a bit more linear.
 */
#define PWM_TOP_VALUE		500
#define PWM_BOTTOM_VALUE	200

#define PWM_VALUE(x)	(PWM_BOTTOM_VALUE+(PWM_TOP_VALUE-PWM_BOTTOM_VALUE)*(x)/100)

/* PWM configuration structure. The LCD Backlight is on PWM1/PB20 ie PWM2/PIN1 in ChibiOS speak */
static const PWMConfig pwmcfg = {
  1000000,		/* 1 MHz PWM clock frequency. Ignored as we are using PWM_MCK_DIV_n */
  1000,			/* PWM period is 1000 cycles. */
  0,
  {
   {PWM_MCK_DIV_1 | PWM_OUTPUT_ACTIVE_HIGH | PWM_OUTPUT_PIN1 | PWM_DISABLEPULLUP_PIN1, 0},
  },
};

static bool_t pwmRunning = FALSE;

/**
 * @brief   Initialise the board for the display.
 * @notes	Performs the following functions:
 *			1. initialise the spi port used by your display
 *			2. initialise the reset pin (initial state not-in-reset)
 *			3. initialise the chip select pin (initial state not-active)
 *			4. initialise the backlight pin (initial state back-light off)
 *
 * @notapi
 */
static GFXINLINE void init_board(GDisplay *g) {

	// As we are not using multiple displays we set g->board to NULL as we don't use it.
	g->board = 0;

	switch(g->controllerdisplay) {
	case 0:											// Set up for Display 0
		// *********************************************************************************************
		// InitSpi( )
		//
		// Sets up SPI channel 0 for communications to Nokia 6610 LCD Display
		//
		// I/O ports used:	PA2  = LCD Reset (set to low to reset)
		//					PA12 = LCD chip select (set to low to select the LCD chip)
		//					PA16 = SPI0_MISO Master In - Slave Out (not used in LCD interface)
		//					PA17 = SPI0_MOSI Master Out - Slave In pin (Serial Data to LCD slave)
		//					PA18 = SPI0_SPCK Serial Clock (to LCD slave)
		//					PB20 = backlight control (normally PWM control, 1 = full on)
		//
		// *********************************************************************************************}

		/* This code should really use the ChibiOS driver for these functions */

		// Pin for backlight
		pPIOB->PIO_CODR   = PIOB_LCD_BL_MASK;    // Set PB20 to LOW
		pPIOB->PIO_OER    = PIOB_LCD_BL_MASK;    // Configure PB20 as output

		// Reset pin
		pPIOA->PIO_SODR   = PIOA_LCD_RESET_MASK;     // Set PA2 to HIGH
		pPIOA->PIO_OER    = PIOA_LCD_RESET_MASK;     // Configure PA2 as output

		// CS pin - this is driven automatically by the SPI hardware itself

		// Init SPI0
		// Disable the following pins from PIO control (will be used instead by the SPI0 peripheral)
		// BIT12 = PA12 -> SPI0_NPCS0 chip select
		// BIT16 = PA16 -> SPI0_MISO Master In - Slave Out (not used in LCD interface)
		// BIT17 = PA17 -> SPI0_MOSI Master Out - Slave In pin (Serial Data to LCD slave)
		// BIT18 = PA18 -> SPI0_SPCK Serial Clock (to LCD slave)
		pPIOA->PIO_PDR = (1<<12) | (1<<16) | (1<<17) | (1<<18);
		pPIOA->PIO_ASR = (1<<12) | (1<<16) | (1<<17) | (1<<18);

		//enable the clock of SPI
		pPMC->PMC_PCER = 1 << AT91C_ID_SPI0;

		// Fixed mode
		pSPI->SPI_CR      = 0x81;               //SPI Enable, Software reset
		pSPI->SPI_CR      = 0x81;               //SPI Enable, Software reset - 2nd write as per errata
		pSPI->SPI_CR      = 0x01;               //SPI Enable
		pSPI->SPI_MR      = 0x000E0011;			//Master mode, fixed select, disable decoder, PCS=1110
		pSPI->SPI_CSR[0]  = 0x00000311;			//9bit, CPOL=1, ClockPhase=0, SCLK = 48Mhz/3 = 16MHz

		/* Display backlight control at 100% */
		pwmRunning = FALSE;
		palSetPad(IOPORT2, PIOB_LCD_BL);
		break;
	}
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	if (state)
		palClearPad(IOPORT1, PIOA_LCD_RESET);
	else
		palSetPad(IOPORT1, PIOA_LCD_RESET);
}

static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	if (percent == 100) {
		/* Turn the pin on - No PWM */
		if (pwmRunning) {
			pwmStop(&PWMD2);
			pwmRunning = FALSE;
		}
		palSetPad(IOPORT2, PIOB_LCD_BL);
	} else if (percent == 0) {
		/* Turn the pin off - No PWM */
		if (pwmRunning) {
			pwmStop(&PWMD2);
			pwmRunning = FALSE;
		}
		palClearPad(IOPORT2, PIOB_LCD_BL);
	} else {
		/* Use the PWM */
		if (!pwmRunning) {
			pwmStart(&PWMD2, &pwmcfg);
			pwmRunning = TRUE;
		}
		pwmEnableChannel(&PWMD2, 0, PWM_VALUE(percent));
	}
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;
}

static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
}

static GFXINLINE void write_index(GDisplay *g, uint16_t index) {
	(void) g;
	// wait for the previous transfer to start
	while(!(pSPI->SPI_SR & AT91C_SPI_TDRE));
	// send the command
	pSPI->SPI_TDR = index & 0xFF;
}

static GFXINLINE void write_data(GDisplay *g, uint16_t data) {
	(void) g;
	// wait for the previous transfer to start
	while(!(pSPI->SPI_SR & AT91C_SPI_TDRE));
	// send the data
	pSPI->SPI_TDR = data | 0x0100;
}

#endif /* _GDISP_LLD_BOARD_H */
