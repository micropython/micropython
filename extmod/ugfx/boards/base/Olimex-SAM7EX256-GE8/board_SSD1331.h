/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#include "board_uext.h"

/*
 * NOTE: This board file is for an SSD1331 running in SPI mode
 */
// The various methods of driving the SPI interface
#define SPI_METHOD_PIO		1
#define SPI_METHOD_AT91		2
#define SPI_METHOD_CHIBIOS	3			// ChibiOS SPI for AT91SAM7 requires patch 7675 or higher to work correctly

// The various methods of driving the PIO interface
#define PIO_METHOD_AT91		1
#define PIO_METHOD_CHIBIOS	2			// ChibiOS PIO for the AT91SAM7 requires patch 7669 or higher to work correctly

// What methods are we using in this file
#define SPI_METHOD			SPI_METHOD_AT91
#define PIO_METHOD			PIO_METHOD_AT91

// If using SPI_METHOD_CHIBIOS or SPI_METHOD_AT91 then this must be defined for your CPU
#define SPI_CONFIG_REGISTER		0x00000801			// For AT91SAM7:	8bit, CPOL=1, NCPHA = 0, ClockPhase=0, SCLK = 48Mhz/8 = 6MHz

//---------------------------------------------------------------------

/*
 * Pin connection for the display to the UEXT connector:
 *
 * DISPLAY VCC					UEXT PIN 1 (3.3V)
 * DISPLAY GND					UEXT PIN 2 (GND)
 * DISPLAY SCL/CLK				UEXT PIN 9 (SCK)
 * DISPLAY SDA/DIN				UEXT PIN 8 (MOSI)
 * DISPLAY RST					UEXT PIN 5 (SCL)
 * DISPLAY D/C					UEXT PIN 6 (SDA)
 * DISPLAY CS					UEXT PIN 10 (SSEL)
 */
#define PORT_RESET		UEXT_PORT_PIN5
#define PIN_RESET		UEXT_PORTPIN_PIN5
#define PORT_DC			UEXT_PORT_PIN6
#define PIN_DC			UEXT_PORTPIN_PIN6

#if PIO_METHOD == PIO_METHOD_AT91
	#define PinIsOutput(port,pin)		((port)->PIO_OER = 1 << (pin), (port)->PIO_PER = 1 << (pin), (port)->PIO_MDDR = 1 << (pin), (port)->PIO_PPUDR = 1 << (pin))
	#define PinSet(port,pin)			(port)->PIO_SODR = 1 << (pin)
	#define PinClear(port,pin)			(port)->PIO_CODR = 1 << (pin)
#elif PIO_METHOD == PIO_METHOD_CHIBIOS
	#define PinIsOutput(port,pin)		palSetPadMode((port), (pin), PAL_MODE_OUTPUT_PUSHPULL)
	#define PinSet(port,pin)			palSetPad((port), (pin))
	#define PinClear(port,pin)			palClearPad((port), (pin))
#else
	#error "SSD1331 board file: Unsupported PIO method"
#endif

#if SPI_METHOD == SPI_METHOD_PIO

	static void spi_delay(volatile unsigned long a) { while (a!=0) a--; }

	static void spi_write(uint8_t data) {
		uint8_t bit;

		for(bit = 0x80; bit; bit >>= 1) {
			if(data & bit)
				PinSet(UEXT_SPI_MOSI_PORT, UEXT_SPI_MOSI_PORTPIN);
			else
				PinClear(UEXT_SPI_MOSI_PORT, UEXT_SPI_MOSI_PORTPIN);
			spi_delay(1);
			PinClear(UEXT_SPI_SCK_PORT, UEXT_SPI_SCK_PORTPIN);
			spi_delay(1);
			PinSet(UEXT_SPI_SCK_PORT, UEXT_SPI_SCK_PORTPIN);
		}
	}

	#define SPI_INIT() {		\
		PinIsOutput	(UEXT_SPI_MOSI_PORT, UEXT_SPI_MOSI_PORTPIN);	\
		PinSet		(UEXT_SPI_MOSI_PORT, UEXT_SPI_MOSI_PORTPIN);	\
		PinIsOutput	(UEXT_SPI_SCK_PORT, UEXT_SPI_SCK_PORTPIN);		\
		PinSet		(UEXT_SPI_SCK_PORT, UEXT_SPI_SCK_PORTPIN);		\
		PinIsOutput	(UEXT_SPI_CS_PORT, UEXT_SPI_CS_PORTPIN);		\
		PinSet		(UEXT_SPI_CS_PORT, UEXT_SPI_CS_PORTPIN);		\
	}

	#define SPI_GETBUS()				PinClear(UEXT_SPI_CS_PORT, UEXT_SPI_CS_PORTPIN), gfxSleepMilliseconds(1)
	#define SPI_RELEASEBUS()			PinSet(UEXT_SPI_CS_PORT, UEXT_SPI_CS_PORTPIN)
	#define SPI_WAITCOMPLETE()
	#define SPI_WRITEBYTE(data) 		spi_write(data)

#elif SPI_METHOD == SPI_METHOD_AT91

	#if UEXT_SPI_SCK_PORTPIN == 22
		// Assume it is on SPI1
		#define UEXT_SPI_DEV		AT91C_BASE_SPI1
		#define UEXT_SPI_ID			AT91C_ID_SPI1
		#define UEXT_SPI_PORT		AT91C_BASE_PIOA
		#define UEXT_SPI_PERIPH		PIO_BSR
	#else
		// Assume it is on SPI0
		#define UEXT_SPI_DEV		AT91C_BASE_SPI0
		#define UEXT_SPI_ID			AT91C_ID_SPI0
		#define UEXT_SPI_PORT		AT91C_BASE_PIOA
		#define UEXT_SPI_PERIPH		PIO_ASR
	#endif

	#define SPI_INIT() {											\
		UEXT_SPI_PORT->PIO_MDDR			= (1<<UEXT_SPI_CS_PORTPIN) | (1<<UEXT_SPI_SCK_PORTPIN) | (1<<UEXT_SPI_MOSI_PORTPIN) | (1<<UEXT_SPI_MISO_PORTPIN); \
		UEXT_SPI_PORT->PIO_PPUER		= 1<<UEXT_SPI_MISO_PORTPIN;	\
		UEXT_SPI_PORT->PIO_PDR			= (1<<UEXT_SPI_CS_PORTPIN) | (1<<UEXT_SPI_SCK_PORTPIN) | (1<<UEXT_SPI_MOSI_PORTPIN) | (1<<UEXT_SPI_MISO_PORTPIN); \
		UEXT_SPI_PORT->UEXT_SPI_PERIPH	= (1<<UEXT_SPI_CS_PORTPIN) | (1<<UEXT_SPI_SCK_PORTPIN) | (1<<UEXT_SPI_MOSI_PORTPIN) | (1<<UEXT_SPI_MISO_PORTPIN); \
		AT91C_BASE_PMC->PMC_PCER		= 1 << UEXT_SPI_ID;			\
		UEXT_SPI_DEV->SPI_CR			= 0x81;						\
		UEXT_SPI_DEV->SPI_CR			= 0x81;						\
		UEXT_SPI_DEV->SPI_CR			= 0x01;						\
		UEXT_SPI_DEV->SPI_MR			= 0x000E0011;				\
		UEXT_SPI_DEV->SPI_CSR[0]		= SPI_CONFIG_REGISTER;		\
	}

	#define SPI_WAITCOMPLETE()			while(!(UEXT_SPI_DEV->SPI_SR & AT91C_SPI_TXEMPTY))
	#define SPI_GETBUS()				gfxSleepMilliseconds(1)
	#define SPI_RELEASEBUS()
	#define SPI_WRITEBYTE(data) {									\
		while(!(UEXT_SPI_DEV->SPI_SR & AT91C_SPI_TDRE));			\
		UEXT_SPI_DEV->SPI_TDR = data;								\
	}

#elif SPI_METHOD == SPI_METHOD_CHIBIOS

	static const SPIConfig spiconfig = {
		0,
		/* HW dependent part.*/
		UEXT_SPI_CS_PORT,
		UEXT_SPI_CS_PORTPIN,
		SPI_CONFIG_REGISTER
	};

	#define SPI_INIT()

	#define SPI_GETBUS() {											\
		spiStart(UEXT_SPI, &spiconfig);								\
		spiSelect(UEXT_SPI);										\
	}

	#define SPI_RELEASEBUS() {										\
		spiUnselect(UEXT_SPI);										\
		spiStop(UEXT_SPI);											\
	}

	#define SPI_WAITCOMPLETE()
	#define SPI_WRITEBYTE(data)				spiSend(UEXT_SPI, 1, &data)

#else
	#error "SSD1331 board file: Unsupported SPI method"
#endif

static GFXINLINE void init_board(GDisplay *g) {
	(void) g;

	PinIsOutput	(PORT_DC, PIN_DC);
	PinIsOutput	(PORT_RESET, PIN_RESET);
	PinSet		(PORT_RESET, PIN_RESET);

	SPI_INIT();
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	(void) state;

	if (state)
		PinClear(PORT_RESET, PIN_RESET);
	else
		PinSet(PORT_RESET, PIN_RESET);
}

static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	(void) percent;
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;

	SPI_GETBUS();
}

static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;

	SPI_WAITCOMPLETE();
	SPI_RELEASEBUS();
}

static GFXINLINE void write_cmd(GDisplay *g, uint8_t cmd) {
	(void) g;

	// Command mode please
	SPI_WAITCOMPLETE();
	PinClear(PORT_DC, PIN_DC);

	SPI_WRITEBYTE(cmd);
}

static GFXINLINE void write_data(GDisplay *g, uint8_t data) {
	(void) g;

	// Data mode please
	SPI_WAITCOMPLETE();
	PinSet(PORT_DC, PIN_DC);

	SPI_WRITEBYTE(data);
}

#endif /* _GDISP_LLD_BOARD_H */
