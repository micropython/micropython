/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#include "stm32f4xx_fmc.h"
#include "stm32f429i_discovery_sdram.h"
#include <string.h>

#define SPI_PORT	&SPID5
#define DC_PORT		GPIOD
#define DC_PIN		GPIOD_LCD_WRX

static const SPIConfig spi_cfg = {
	NULL,
	GPIOC,
	GPIOC_SPI5_LCD_CS,
	((1 << 3) & SPI_CR1_BR) | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR
};

static const ltdcConfig driverCfg = {
	240, 320,
	10, 2,
	20, 2,
	10, 4,
	0,
	0x000000,
	{
		(LLDCOLOR_TYPE *)SDRAM_BANK_ADDR,	// frame
		240, 320,							// width, height
		240 * LTDC_PIXELBYTES,				// pitch
		LTDC_PIXELFORMAT,					// fmt
		0, 0,								// x, y
		240, 320,							// cx, cy
		LTDC_COLOR_FUCHSIA,					// defcolor
		0x980088,							// keycolor
		LTDC_BLEND_FIX1_FIX2,				// blending
		0,									// palette
		0,									// palettelen
		0xFF,								// alpha
		LTDC_LEF_ENABLE						// flags
	},
	LTDC_UNUSED_LAYER_CONFIG
};

#include "ili9341.h"

static void acquire_bus(GDisplay *g) {
	(void) g;

	spiSelect(SPI_PORT);
}

static void release_bus(GDisplay *g) {
	(void) g;

	spiUnselect(SPI_PORT);
}

static void write_index(GDisplay *g, uint8_t index) {
	static uint8_t	sindex;
	(void) g;

	palClearPad(DC_PORT, DC_PIN);
	sindex = index;
	spiSend(SPI_PORT, 1, &sindex);
}

static void write_data(GDisplay *g, uint8_t data) {
	static uint8_t	sdata;
	(void) g;

	palSetPad(DC_PORT, DC_PIN);
	sdata = data;
	spiSend(SPI_PORT, 1, &sdata);
}

static void Init9341(GDisplay *g) {
	#define REG_TYPEMASK	0xFF00
	#define REG_DATAMASK	0x00FF

	#define REG_DATA		0x0000
	#define REG_COMMAND		0x0100
	#define REG_DELAY		0x0200

	static const uint16_t initdata[] = {
			REG_COMMAND | ILI9341_CMD_RESET,
			REG_DELAY   | 5,
			REG_COMMAND | ILI9341_CMD_DISPLAY_OFF,
			REG_COMMAND | ILI9341_SET_FRAME_CTL_NORMAL, 0x00, 0x1B,
			REG_COMMAND | ILI9341_SET_FUNCTION_CTL, 0x0A, 0xA2,
			REG_COMMAND | ILI9341_SET_POWER_CTL_1, 0x10,
			REG_COMMAND | ILI9341_SET_POWER_CTL_2, 0x10,
			#if 1
				REG_COMMAND | ILI9341_SET_VCOM_CTL_1, 0x45, 0x15,
				REG_COMMAND | ILI9341_SET_VCOM_CTL_2, 0x90,
			#else
				REG_COMMAND | ILI9341_SET_VCOM_CTL_1, 0x35, 0x3E,
				REG_COMMAND | ILI9341_SET_VCOM_CTL_2, 0xBE,
			#endif
	        REG_COMMAND | ILI9341_SET_MEM_ACS_CTL, 0xC8,
			REG_COMMAND | ILI9341_SET_RGB_IF_SIG_CTL, 0xC2,
			REG_COMMAND | ILI9341_SET_FUNCTION_CTL, 0x0A, 0xA7, 0x27, 0x04,
			REG_COMMAND | ILI9341_SET_COL_ADDR, 0x00, 0x00, 0x00, 0xEF,
			REG_COMMAND | ILI9341_SET_PAGE_ADDR, 0x00, 0x00, 0x01, 0x3F,
			REG_COMMAND | ILI9341_SET_IF_CTL, 0x01, 0x00, 0x06,
			REG_COMMAND | ILI9341_SET_GAMMA, 0x01,
			REG_COMMAND | ILI9341_SET_PGAMMA,
				#if 1
					0x0F, 0x29, 0x24, 0x0C, 0x0E, 0x09, 0x4E, 0x78,
					0x3C, 0x09, 0x13, 0x05, 0x17, 0x11, 0x00,
				#else
					0x1F, 0x1a, 0x18, 0x0a, 0x0f, 0x06, 0x45, 0x87,
					0x32, 0x0a, 0x07, 0x02, 0x07, 0x05, 0x00,
				#endif
			REG_COMMAND | ILI9341_SET_NGAMMA,
				#if 1
					0x00, 0x16, 0x1B, 0x04, 0x11, 0x07, 0x31, 0x33,
					0x42, 0x05, 0x0C, 0x0A, 0x28, 0x2F, 0x0F,
				#else
					0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3a, 0x78,
					0x4d, 0x05, 0x18, 0x0d, 0x38, 0x3a, 0x1f,
				#endif
			REG_COMMAND | ILI9341_CMD_SLEEP_OFF,
			REG_DELAY   | 10,
			REG_COMMAND | ILI9341_CMD_DISPLAY_ON,
			REG_COMMAND | ILI9341_SET_MEM
	};

	const uint16_t	*p;

	acquire_bus(g);
	for(p = initdata; p < &initdata[sizeof(initdata)/sizeof(initdata[0])]; p++) {
		switch(*p & REG_TYPEMASK) {
		case REG_DATA:		write_data(g, *p);					break;
		case REG_COMMAND:	write_index(g, *p);					break;
		case REG_DELAY:		gfxSleepMilliseconds(*p & 0xFF);	break;
		}
	}
	release_bus(g);
}

static void init_board(GDisplay *g) {

	// As we are not using multiple displays we set g->board to NULL as we don't use it.
	g->board = 0;

	switch(g->controllerdisplay) {
	case 0:											// Set up for Display 0
		palSetPadMode(GPIOA, 9, PAL_MODE_ALTERNATE(7));		// UART_TX
		palSetPadMode(GPIOA, 10, PAL_MODE_ALTERNATE(7));	// UART_RX
		palSetPadMode(GPIOF, GPIOF_LCD_DCX, PAL_MODE_ALTERNATE(5));
		palSetPadMode(GPIOF, GPIOF_LCD_DE, PAL_MODE_ALTERNATE(14));

		#define STM32_SAISRC_NOCLOCK    (0 << 23)   /**< No clock.                  */
		#define STM32_SAISRC_PLL        (1 << 23)   /**< SAI_CKIN is PLL.           */
		#define STM32_SAIR_DIV2         (0 << 16)   /**< R divided by 2.            */
		#define STM32_SAIR_DIV4         (1 << 16)   /**< R divided by 4.            */
		#define STM32_SAIR_DIV8         (2 << 16)   /**< R divided by 8.            */
		#define STM32_SAIR_DIV16        (3 << 16)   /**< R divided by 16.           */

		#define STM32_PLLSAIN_VALUE                 192
		#define STM32_PLLSAIQ_VALUE                 7
		#define STM32_PLLSAIR_VALUE                 4
		#define STM32_PLLSAIR_POST                  STM32_SAIR_DIV4

		/* PLLSAI activation.*/
		RCC->PLLSAICFGR = (STM32_PLLSAIN_VALUE << 6) | (STM32_PLLSAIR_VALUE << 28) | (STM32_PLLSAIQ_VALUE << 24);
		RCC->DCKCFGR = (RCC->DCKCFGR & ~RCC_DCKCFGR_PLLSAIDIVR) | STM32_PLLSAIR_POST;
		RCC->CR |= RCC_CR_PLLSAION;

		// Initialise the SDRAM
		SDRAM_Init();

		// Clear the SDRAM
		memset((void *)SDRAM_BANK_ADDR, 0, 0x400000);

		spiStart(SPI_PORT, &spi_cfg);

		Init9341(g);
		break;
	}
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	(void) percent;
}

#endif /* _GDISP_LLD_BOARD_H */
