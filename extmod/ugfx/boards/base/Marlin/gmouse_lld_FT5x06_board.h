/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GINPUT_LLD_MOUSE_BOARD_H
#define _GINPUT_LLD_MOUSE_BOARD_H

// Resolution and Accuracy Settings
#define GMOUSE_FT5x06_PEN_CALIBRATE_ERROR		8
#define GMOUSE_FT5x06_PEN_CLICK_ERROR			6
#define GMOUSE_FT5x06_PEN_MOVE_ERROR			4
#define GMOUSE_FT5x06_FINGER_CALIBRATE_ERROR	14
#define GMOUSE_FT5x06_FINGER_CLICK_ERROR		18
#define GMOUSE_FT5x06_FINGER_MOVE_ERROR			14

// How much extra data to allocate at the end of the GMouse structure for the board's use
#define GMOUSE_FT5x06_BOARD_DATA_SIZE			0

// Set this to TRUE if you want self-calibration.
//	NOTE:	This is not as accurate as real calibration.
//			It requires the orientation of the touch panel to match the display.
//			It requires the active area of the touch panel to exactly match the display size.
#define GMOUSE_FT5x06_SELF_CALIBRATE			FALSE

/* I2C interface #2 - Touchscreen controller */
static const I2CConfig i2ccfg2 = {
    OPMODE_I2C,
    400000,
    FAST_DUTY_CYCLE_2,
};

static bool_t init_board(GMouse* m, unsigned driverinstance) {
	(void)		m;

	// We only support one of these on this board
	if (driverinstance)
		return FALSE;
	return TRUE;
}

static GFXINLINE void aquire_bus(GMouse* m) {
	(void)		m;

}

static GFXINLINE void release_bus(GMouse* m) {
	(void)		m;

}

static void write_reg(GMouse* m, uint8_t reg, uint8_t val) {
	uint8_t txbuf[2];
	(void)		m;

	txbuf[0] = reg;
	txbuf[1] = val;

	i2cAcquireBus(&I2CD2);
	i2cMasterTransmitTimeout(&I2CD2, FT5x06_ADDR, txbuf, 2, 0, 0, MS2ST(FT5x06_TIMEOUT));
	i2cReleaseBus(&I2CD2);
}

static uint8_t read_byte(GMouse* m, uint8_t reg) {
	uint8_t		rxbuf[1];
	(void)		m;

	rxbuf[0] = 0;

	i2cAcquireBus(&I2CD2);
	i2cMasterTransmitTimeout(&I2CD2, FT5x06_ADDR, &reg, 1, rxbuf, 1, MS2ST(FT5x06_TIMEOUT));
	i2cReleaseBus(&I2CD2);

	return rxbuf[0];
}

static uint16_t read_word(GMouse* m, uint8_t reg) {
	uint8_t		rxbuf[2];
	(void)		m;

	rxbuf[0] = 0;
	rxbuf[1] = 0;

	i2cAcquireBus(&I2CD2);
	i2cMasterTransmitTimeout(&I2CD2, FT5x06_ADDR, &reg, 1, rxbuf, 2, MS2ST(FT5x06_TIMEOUT));
	i2cReleaseBus(&I2CD2);

	return (((uint16_t)rxbuf[0]) << 8) | rxbuf[1];
}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
