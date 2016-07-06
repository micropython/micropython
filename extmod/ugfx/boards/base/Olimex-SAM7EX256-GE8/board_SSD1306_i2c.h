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
 * NOTE: This board file is for an SSD1306 running in I2C mode
 * 		 You will need to use a different board file for an SSD1306 running in SPI mode
 */

#define I2C_ADDRESS				(0x78/2)			// SSD1306 supports either 0x78 or 0x7A (including the write bit)
#define I2C_RATE				400000				// 400kHz is I2C FAST mode

// The various methods of driving the I2C interface
#define I2C_METHOD_CHIBIOS		3

// The various supported CPU's
#define CPU_AT91				1

// What methods are we using in this file
#define I2C_METHOD			I2C_METHOD_CHIBIOS
#define CPU_METHOD			CPU_AT91

// If using SPI_METHOD_CHIBIOS the CPU specific i2cconfig structure must be defined for your CPU
#if CPU_METHOD == CPU_AT91
	#define I2C_CONFIG_REGISTER		((((MCK/I2C_RATE/2 - 3)>>8)<<16)|(((MCK/I2C_RATE/2 - 3)&0xFF)<<8)|(((MCK/I2C_RATE/2 - 3)&0xFF)<<0))			// For AT91SAM7:	8bit, CPOL=1, NCPHA = 0, ClockPhase=0, SCLK = 48Mhz/8 = 6MHz
	#if I2C_METHOD == I2C_METHOD_CHIBIOS
		// ChibiOS - Yuck! This structure is different on each platform.
		//			 This is the one for the AT91
		static const I2CConfig i2cconfig = {
			I2C_CONFIG_REGISTER
		};
	#endif
#else
	#error "SSD1306: Unsupported CPU"
#endif



//---------------------------------------------------------------------

/*
 * Pin connection for the display to the UEXT connector:
 *
 * DISPLAY VCC					UEXT PIN 1 (3.3V)
 * DISPLAY GND					UEXT PIN 2 (GND)
 * DISPLAY SCL					UEXT PIN 5 (SCL)
 * DISPLAY SDA					UEXT PIN 6 (SDA)
 */

#if I2C_METHOD == I2C_METHOD_CHIBIOS

	/* ChibiOS can't send a separate data prefix byte in the same transaction as the real data.
	 * Nor does it support an address prefix to the data (such as an EEPROM would use).
	 * Instead we tell the driver to put it into the RAM buffer at the beginning of each page (Yuck!).
	 *
	 * Prefix bytes:
	 * 0x40 means all following bytes in this transaction go into the GDRAM.
	 * 0x00 means the following byte (singular) is a command byte.
	 */
	#define SSD1306_PAGE_PREFIX		0x40

	#define I2C_INIT()

	#define I2C_GETBUS() {											\
		i2cAcquireBus(UEXT_I2C);									\
		i2cStart(UEXT_I2C, &i2cconfig);								\
	}

	#define I2C_RELEASEBUS() {										\
		i2cStop(UEXT_I2C);											\
		i2cReleaseBus(UEXT_I2C);									\
	}

	#define I2C_WAITCOMPLETE()
	#define I2C_WRITECMDBYTE(cmd)			{						\
		uint8_t	data[2];											\
		data[0] = 0;												\
		data[1] = cmd;												\
		i2cMasterTransmitTimeout (UEXT_I2C, I2C_ADDRESS, data, 2, 0, 0, TIME_INFINITE);	\
	}
	#define I2C_WRITEBYTES(pdata, len)		i2cMasterTransmitTimeout (UEXT_I2C, I2C_ADDRESS, pdata, length, 0, 0, TIME_INFINITE)

#else
	#error "SSD1306 board file: Unsupported I2C method"
#endif

static GFXINLINE void init_board(GDisplay *g) {
	(void) g;

	I2C_INIT();
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
	(void) percent;
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;

	I2C_GETBUS();
}

static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;

	I2C_WAITCOMPLETE();
	I2C_RELEASEBUS();
}

static GFXINLINE void write_cmd(GDisplay *g, uint8_t cmd) {
	(void) g;

	I2C_WAITCOMPLETE();

	// Command mode please
	I2C_WRITECMDBYTE(cmd);
}

static GFXINLINE void write_data(GDisplay *g, uint8_t* data, uint16_t length) {
	(void) g;

	I2C_WAITCOMPLETE();

	// Data mode please
	I2C_WRITEBYTES(data, length);
}

#endif /* _GDISP_LLD_BOARD_H */
