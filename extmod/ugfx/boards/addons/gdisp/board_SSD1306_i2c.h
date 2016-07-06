/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    boards/addons/gdisp/board_SSD1306_i2c.h
 * @brief   GDISP Graphic Driver subsystem board interface for the SSD1306 display.
 *
 * @note	This file contains a mix of hardware specific and operating system specific
 *			code. You will need to change it for your CPU and/or operating system.
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

// The command byte to put on the front of each page line
#define SSD1306_PAGE_PREFIX		0x40			 		// Co = 0, D/C = 1

// For a multiple display configuration we would put all this in a structure and then
//	set g->board to that structure.
#define SSD1306_RESET_PORT		GPIOB
#define SSD1306_RESET_PIN		5

/**
 * The default slave address is 0x3D, (talking about
 * only the real address part here) and the slave
 * address can be changed to 0x3C by soldering the
 * SA0 pads on the bottom side of the module.
 *
 * b7 | b6 | b5 | b4 | b3 | b2 | b1 | b0
 * --------------------------------------
 * 0  | 1  | 1  | 1  | 1  | 0  |SA0 | R/W
 */
#define SSD1306_I2C_ADDRESS   	0x3D
#define SSD1306_SDA_PORT		GPIOB
#define SSD1306_SDA_PIN			7
#define SSD1306_SCL_PORT		GPIOB
#define SSD1306_SCL_PIN			6
#define SET_RST					palSetPad(SSD1306_RESET_PORT, SSD1306_RESET_PIN);
#define CLR_RST					palClearPad(SSD1306_RESET_PORT, SSD1306_RESET_PIN);

// I2C configuration structure.
static I2CConfig i2cconfig;

#if GFX_USE_OS_CHIBIOS
	static int32_t thdPriority = 0;
#endif

static GFXINLINE void init_board(GDisplay *g) {

	// As we are not using multiple displays we set g->board to NULL as we don't use it.
	g->board = 0;

	switch(g->controllerdisplay) {
	case 0:											// Set up for Display 0
		// RESET pin.
		palSetPadMode(SSD1306_RESET_PORT, SSD1306_RESET_PIN, PAL_MODE_OUTPUT_PUSHPULL);


		/*
		 * Initializes the I2C driver 1. The I2C1 signals are routed as follows:
		 * PB6 - SCL.
		 * PB7 - SDA.
		 * Timing value comes from ST I2C config tool (xls):
		 * 0x00901D2B;		// 100kHz Standard Mode
		 * 0x00300444;		// 100kHz Fast Mode
		 * 0x0030020A;		// 400kHz Fast Mode
		 * 0x00100002;		// 800kHz Fast Mode +
		 */
		palSetPadMode(SSD1306_SCL_PORT, SSD1306_SCL_PIN, PAL_MODE_ALTERNATE(1));
		palSetPadMode(SSD1306_SDA_PORT, SSD1306_SDA_PIN, PAL_MODE_ALTERNATE(1));
		i2cconfig.timingr = 0x00100002;		// 800kHz Fast Mode+
		i2cInit();
		break;
	}
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	if(state)
		CLR_RST
	else
		SET_RST
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;
	#if GFX_USE_OS_CHIBIOS
		thdPriority = (int32_t)chThdGetPriority();
		chThdSetPriority(HIGHPRIO);
	#endif
	i2cAcquireBus(&I2CD1);
}

static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
	#if GFX_USE_OS_CHIBIOS
		chThdSetPriority(thdPriority);
	#endif
	i2cReleaseBus(&I2CD1);
}

static GFXINLINE void write_cmd(GDisplay *g, uint8_t cmd) {
	uint8_t command[2];
	(void) g;

	command[0] = 0x00;		// Co = 0, D/C = 0
	command[1] = cmd;

	i2cStart(&I2CD1, &i2cconfig);
	i2cMasterTransmitTimeout(&I2CD1, SSD1306_I2C_ADDRESS, command, 2, 0, 0, MS2ST(10));
	i2cStop(&I2CD1);
}

static GFXINLINE void write_data(GDisplay *g, uint8_t* data, uint16_t length) {
	(void) g;

	i2cStart(&I2CD1, &i2cconfig);
	i2cMasterTransmitTimeout(&I2CD1, SSD1306_I2C_ADDRESS, data, length, 0, 0, MS2ST(10));
	i2cStop(&I2CD1);
}

#endif /* _GDISP_LLD_BOARD_H */


