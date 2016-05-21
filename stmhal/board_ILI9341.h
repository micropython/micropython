/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#include STM32_HAL_H
#include "py/mphal.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "spi.h"


SPI_HandleTypeDef ili_spi;

static GFXINLINE void init_board(GDisplay *g) {
	// As we are not using multiple displays we set g->board to NULL as we don't use it.

	#if MICROPY_HW_UGFX_INTERFACE == SPI
	ili_spi = MICROPY_HW_UGFX_SPI;
	
	// init the SPI bus
    ili_spi.Init.Mode = SPI_MODE_MASTER;

    // compute the baudrate prescaler from the desired baudrate
    // select a prescaler that yields at most the desired baudrate
    uint spi_clock;
    if ((ili_spi.Instance == SPI2) || (ili_spi.Instance == SPI3)) {
        // SPI2/3 is on APB1
        spi_clock = HAL_RCC_GetPCLK1Freq();
    } else {
        // SPI1 and SPI4 are on APB2
        spi_clock = HAL_RCC_GetPCLK2Freq();
    }
    uint br_prescale = spi_clock / 10000000;
    if (br_prescale <= 2) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; }
    else if (br_prescale <= 4) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; }
    else if (br_prescale <= 8) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; }
    else if (br_prescale <= 16) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; }
    else if (br_prescale <= 32) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; }
    else if (br_prescale <= 64) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64; }
    else if (br_prescale <= 128) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128; }
    else { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; }

    // data is sent bigendian, latches on rising clock
    ili_spi.Init.CLKPolarity = SPI_POLARITY_LOW;
    ili_spi.Init.CLKPhase = SPI_PHASE_1EDGE;
    ili_spi.Init.Direction = SPI_DIRECTION_2LINES;
    ili_spi.Init.DataSize = SPI_DATASIZE_8BIT;
    ili_spi.Init.NSS = SPI_NSS_SOFT;
    ili_spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    ili_spi.Init.TIMode = SPI_TIMODE_DISABLED;
    ili_spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    ili_spi.Init.CRCPolynomial = 0;


    // init the SPI bus
    spi_init(&ili_spi, false);

    // set the pins to default values
    GPIO_set_pin(MICROPY_HW_UGFX_PORT_CS, MICROPY_HW_UGFX_PIN_CS);
    GPIO_set_pin(MICROPY_HW_UGFX_PORT_RST, MICROPY_HW_UGFX_PIN_RST);
    GPIO_set_pin(MICROPY_HW_UGFX_PORT_A0, MICROPY_HW_UGFX_PIN_A0);
    GPIO_set_pin(MICROPY_HW_UGFX_PORT_BL, MICROPY_HW_UGFX_PIN_BL);

    // init the pins to be push/pull outputs
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Pull = GPIO_NOPULL;

    GPIO_InitStructure.Pin = MICROPY_HW_UGFX_PIN_CS;
    HAL_GPIO_Init(MICROPY_HW_UGFX_PORT_CS, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = MICROPY_HW_UGFX_PIN_RST;
    HAL_GPIO_Init(MICROPY_HW_UGFX_PORT_RST, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = MICROPY_HW_UGFX_PIN_A0;
    HAL_GPIO_Init(MICROPY_HW_UGFX_PORT_A0, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = MICROPY_HW_UGFX_PIN_BL;
    HAL_GPIO_Init(MICROPY_HW_UGFX_PORT_BL, &GPIO_InitStructure);
	
	#else
		#error "Parallel not yet implemented"
	#endif
	
	#ifdef MICROPY_HW_UGFX_PIN_MODE
	GPIO_InitStructure.Pin = MICROPY_HW_UGFX_PIN_MODE;
	HAL_GPIO_Init(MICROPY_HW_UGFX_PORT_MODE, &GPIO_InitStructure);
	MICROPY_HW_UGFX_SET_MODE;
	#endif

	
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	if(state) {
		// reset lcd
		GPIO_clear_pin(MICROPY_HW_UGFX_PORT_RST, MICROPY_HW_UGFX_PIN_RST);
	} else {
		GPIO_set_pin(MICROPY_HW_UGFX_PORT_RST, MICROPY_HW_UGFX_PIN_RST);
	}
}


static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	// TODO: can probably pwm this
	if(percent) {
		// turn back light on
		GPIO_set_pin(MICROPY_HW_UGFX_PORT_BL, MICROPY_HW_UGFX_PIN_BL);
	} else {
		// turn off
		GPIO_clear_pin(MICROPY_HW_UGFX_PORT_BL, MICROPY_HW_UGFX_PIN_BL);
	}
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;
}

static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
	GPIO_set_pin(MICROPY_HW_UGFX_PORT_CS, MICROPY_HW_UGFX_PIN_CS);  //CS high
}

static GFXINLINE void write_index(GDisplay *g, uint16_t index) {
	(void) g;
	
	GPIO_clear_pin(MICROPY_HW_UGFX_PORT_CS, MICROPY_HW_UGFX_PIN_CS);  //CS low
	GPIO_clear_pin(MICROPY_HW_UGFX_PORT_A0, MICROPY_HW_UGFX_PIN_A0);  //CMD low
	HAL_SPI_Transmit(&ili_spi, &index, 1, 1000);
	GPIO_set_pin(MICROPY_HW_UGFX_PORT_A0, MICROPY_HW_UGFX_PIN_A0);  //CMD high
}

static GFXINLINE void write_data(GDisplay *g, uint16_t data) {
	(void) g;
	HAL_SPI_Transmit(&ili_spi, &data, 1, 1000);
}

static GFXINLINE void setreadmode(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setwritemode(GDisplay *g) {
	(void) g;
}

static GFXINLINE uint16_t read_data(GDisplay *g) {
	(void) g;
	return 0;
}

#endif /* _GDISP_LLD_BOARD_H */
