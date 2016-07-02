/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    boards/addons/ginput/touch/MCU/ginput_lld_mouse_board_olimex_stm32_lcd.h
 * @brief   GINPUT Touch low level driver source for the MCU on the example board.
 *
 * @note	This file contains a mix of hardware specific and operating system specific
 *			code. You will need to change it for your CPU and/or operating system.
 */

#ifndef _GINPUT_LLD_MOUSE_BOARD_H
#define _GINPUT_LLD_MOUSE_BOARD_H

#define ADC_MAX 1023

// Resolution and Accuracy Settings
#define GMOUSE_MCU_PEN_CALIBRATE_ERROR		8
#define GMOUSE_MCU_PEN_CLICK_ERROR			6
#define GMOUSE_MCU_PEN_MOVE_ERROR			4
#define GMOUSE_MCU_FINGER_CALIBRATE_ERROR	14
#define GMOUSE_MCU_FINGER_CLICK_ERROR		18
#define GMOUSE_MCU_FINGER_MOVE_ERROR		14

#define GMOUSE_MCU_Z_MIN					0			// The minimum Z reading
#define GMOUSE_MCU_Z_MAX					ADC_MAX		// The maximum Z reading
#define GMOUSE_MCU_Z_TOUCHON				60			// Values between this and Z_MAX are definitely pressed
#define GMOUSE_MCU_Z_TOUCHOFF				30			// Values between this and Z_MIN are definitely not pressed

// How much extra data to allocate at the end of the GMouse structure for the board's use
#define GMOUSE_MCU_BOARD_DATA_SIZE			0

static const ADCConfig ADCC = {
  .vref = ADC_VREF_CFG_AVDD_AVSS,
  .stime = 15,
  .irq = EIC_IRQ_ADC,
  .base = _ADC10_BASE_ADDRESS,
};
static struct ADCDriver ADCD;

#define YNEG 13 // U
#define XNEG 15 // R
#define XPOS 12 // L
#define YPOS 11 // D

static const ADCConversionGroup ADC_X_CG = {
  .circular = FALSE,
  .num_channels = 1,
  .channels = 1 << XNEG,
};

static const ADCConversionGroup ADC_Y_CG = {
  .circular = FALSE,
  .num_channels = 1,
  .channels = 1 << YPOS,
};

static bool_t init_board(GMouse *m, unsigned driverinstance) {
	(void)	m;

	if (driverinstance)
		return FALSE;

	adcObjectInit(&ADCD);
	adcStart(&ADCD, &ADCC);
	return TRUE;
}

static bool_t read_xyz(GMouse *m, GMouseReading *prd) {
	adcsample_t samples[2];

	prd->buttons = 0;

	// Read the z value first.
	// Set X+ to ground and Y- to VCC
	palSetPadMode(IOPORTB, XPOS, PAL_MODE_OUTPUT);
	palClearPad(IOPORTB, XPOS);
	palSetPadMode(IOPORTB, YNEG, PAL_MODE_OUTPUT);
	palSetPad(IOPORTB, YNEG);
	palSetPadMode(IOPORTB, XNEG, PAL_MODE_INPUT_ANALOG);
	palSetPadMode(IOPORTB, YPOS, PAL_MODE_INPUT_ANALOG);
	adcConvert(&ADCD, &ADC_X_CG, &samples[0], 1);
	adcConvert(&ADCD, &ADC_Y_CG, &samples[1], 1);
	pdr->z = ADC_MAX - (samples[1] - samples[0]);

	// Shortcut - no need to read X or Y if the touch is off.
	if (pdr->z < GMOUSE_MCU_Z_TOUCHON)
		return TRUE;

	// Read X
	palSetPadMode(IOPORTB, XPOS, PAL_MODE_OUTPUT);
	palSetPad(IOPORTB, XPOS);
	palSetPadMode(IOPORTB, XNEG, PAL_MODE_OUTPUT);
	palClearPad(IOPORTB, XNEG);
	palSetPadMode(IOPORTB, YNEG, PAL_MODE_INPUT);
	palSetPadMode(IOPORTB, YPOS, PAL_MODE_INPUT_ANALOG);
	adcConvert(&ADCD, &ADC_Y_CG, &samples[0], 1);
	pdr->x = ADC_MAX - samples[0];

	// Read Y
	palSetPadMode(IOPORTB, YNEG, PAL_MODE_OUTPUT);
	palClearPad(IOPORTB, YNEG);
	palSetPadMode(IOPORTB, YPOS, PAL_MODE_OUTPUT);
	palSetPad(IOPORTB, YPOS);
	palSetPadMode(IOPORTB, XPOS, PAL_MODE_INPUT);
	palSetPadMode(IOPORTB, XNEG, PAL_MODE_INPUT_ANALOG);
	adcConvert(&ADCD, &ADC_X_CG, &samples[0], 1);
	pdr->y = ADC_MAX - samples[0];

	return TRUE;
}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
/** @} */
