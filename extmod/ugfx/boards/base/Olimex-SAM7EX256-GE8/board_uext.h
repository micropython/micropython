/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    boards/base/Olimex-SAM7EX256-GE8/board_uext.h
 * @brief   Definitions describing the UEXT interface
 */

#ifndef _BOARD_UEXT_H
#define _BOARD_UEXT_H

/*
 * The UEXT official pin definitions
 *
 *            +-------+
 * 3.3V     1 | +   + | 2      GND
 * TX       3 | +   + | 4      RX
 * SCL      5   +   + | 6      SDA
 * MISO     7 | +   + | 8      MOSI
 * SCK      9 | +   + | 10     SSEL/CS
 *            +-------+
 *
 * Note many small displays using SPI will incorrectly use the SCL/SDA terminology on their pins.
 * They should be connected to SCK/MOSI respectively
 */

// These definitions match the physical arrangement of the UEXT on your board
#define UEXT_PORT_PIN3			AT91C_BASE_PIOA
#define UEXT_PORTPIN_PIN3		6
#define UEXT_PORT_PIN4			AT91C_BASE_PIOA
#define UEXT_PORTPIN_PIN4		5
#define UEXT_PORT_PIN5			AT91C_BASE_PIOA
#define UEXT_PORTPIN_PIN5		11
#define UEXT_PORT_PIN6			AT91C_BASE_PIOA
#define UEXT_PORTPIN_PIN6		10
#define UEXT_PORT_PIN7			AT91C_BASE_PIOA
#define UEXT_PORTPIN_PIN7		24
#define UEXT_PORT_PIN8			AT91C_BASE_PIOA
#define UEXT_PORTPIN_PIN8		23
#define UEXT_PORT_PIN9			AT91C_BASE_PIOA
#define UEXT_PORTPIN_PIN9		22
#define UEXT_PORT_PIN10			AT91C_BASE_PIOA
#define UEXT_PORTPIN_PIN10		21

// The ports that the various serial devices use for UEXT on your board
#define UEXT_SERIAL				(&SD2)			// TX = UEXT PIN 3, RX = UEXT PIN 4
#define UEXT_SPI				(&SPID2)		// MISO = UEXT PIN 7, MOSI = UEXT PIN 8, SCK = UEXT PIN 9, CS = UEXT PIN 10
#define UEXT_I2C				(&I2CD1)		// SCL/TWCK = UEXT PIN 5, SDA/TWD = UEXT PIN 6

// We now define generic names for the UEXT pins
#define UEXT_SPI_MISO_PORT		UEXT_PORT_PIN7
#define UEXT_SPI_MISO_PORTPIN	UEXT_PORTPIN_PIN7
#define UEXT_SPI_MOSI_PORT		UEXT_PORT_PIN8
#define UEXT_SPI_MOSI_PORTPIN	UEXT_PORTPIN_PIN8
#define UEXT_SPI_SCK_PORT		UEXT_PORT_PIN9
#define UEXT_SPI_SCK_PORTPIN	UEXT_PORTPIN_PIN9
#define UEXT_SPI_CS_PORT		UEXT_PORT_PIN10
#define UEXT_SPI_CS_PORTPIN		UEXT_PORTPIN_PIN10

#define UEXT_I2C_SCL_PORT		UEXT_PORT_PIN5
#define UEXT_I2C_SCL_PORTPIN	UEXT_PORTPIN_PIN5
#define UEXT_I2C_SDA_PORT		UEXT_PORT_PIN6
#define UEXT_I2C_SDA_PORTPIN	UEXT_PORTPIN_PIN6

#define UEXT_SERIAL_TX_PORT		UEXT_PORT_PIN3
#define UEXT_SERIAL_TX_PORTPIN	UEXT_PORTPIN_PIN3
#define UEXT_SERIAL_RX_PORT		UEXT_PORT_PIN4
#define UEXT_SERIAL_RX_PORTPIN	UEXT_PORTPIN_PIN4

#define UEXT_I2C_TWCK_PORT		UEXT_PORT_PIN5
#define UEXT_I2C_TWCK_PORTPIN	UEXT_PORTPIN_PIN5
#define UEXT_I2C_TWD_PORT		UEXT_PORT_PIN6
#define UEXT_I2C_TWD_PORTPIN	UEXT_PORTPIN_PIN6

#endif /* _BOARD_UEXT_H */
