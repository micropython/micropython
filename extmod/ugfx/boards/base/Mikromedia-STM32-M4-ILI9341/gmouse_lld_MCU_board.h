/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _LLD_GMOUSE_MCU_BOARD_H
#define _LLD_GMOUSE_MCU_BOARD_H

// Resolution and Accuracy Settings
#define GMOUSE_MCU_PEN_CALIBRATE_ERROR		8
#define GMOUSE_MCU_PEN_CLICK_ERROR			6
#define GMOUSE_MCU_PEN_MOVE_ERROR			4
#define GMOUSE_MCU_FINGER_CALIBRATE_ERROR	14
#define GMOUSE_MCU_FINGER_CLICK_ERROR		18
#define GMOUSE_MCU_FINGER_MOVE_ERROR		14
#define GMOUSE_MCU_Z_MIN					0
#define GMOUSE_MCU_Z_MAX					4095
#define GMOUSE_MCU_Z_TOUCHON				3090
#define GMOUSE_MCU_Z_TOUCHOFF				400

// How much extra data to allocate at the end of the GMouse structure for the board's use
#define GMOUSE_MCU_BOARD_DATA_SIZE		0

#define ADC_NUM_CHANNELS   2
#define ADC_BUF_DEPTH      1

static const ADCConversionGroup adcgrpcfg = {
  FALSE,
  ADC_NUM_CHANNELS,
  0,
  0,
  /* HW dependent part.*/
  0,
  ADC_CR2_SWSTART,
  0,
  0,
  ADC_SQR1_NUM_CH(ADC_NUM_CHANNELS),
  0,
  ADC_SQR3_SQ2_N(ADC_CHANNEL_IN8) | ADC_SQR3_SQ1_N(ADC_CHANNEL_IN9)
};

static bool_t init_board(GMouse *m, unsigned driverinstance) {
	(void)	m;

	// Only one touch interface on this board
	if (driverinstance)
		return FALSE;

	adcStart(&ADCD1, 0);

	// Set up for reading Z
	palClearPad(GPIOB, GPIOB_DRIVEA);
	palClearPad(GPIOB, GPIOB_DRIVEB);
    chThdSleepMilliseconds(1);				// Settling time
	return TRUE;
}

static bool_t read_xyz(GMouse *m, GMouseReading *prd) {
	adcsample_t samples[ADC_NUM_CHANNELS * ADC_BUF_DEPTH];
	(void)		m;

	// No buttons
	prd->buttons = 0;

	// Get the z reading (assumes we are ready to read z)
    adcConvert(&ADCD1, &adcgrpcfg, samples, ADC_BUF_DEPTH);
    prd->z = samples[0];

    // Take a shortcut and don't read x, y if we know we are definitely not touched.
    if (prd->z >= GMOUSE_MCU_Z_TOUCHOFF) {

		// Get the x reading
		palSetPad(GPIOB, GPIOB_DRIVEA);
		palClearPad(GPIOB, GPIOB_DRIVEB);
		chThdSleepMilliseconds(2);
		adcConvert(&ADCD1, &adcgrpcfg, samples, ADC_BUF_DEPTH);
		prd->x = samples[1];

		// Get the y reading
		palClearPad(GPIOB, GPIOB_DRIVEA);
		palSetPad(GPIOB, GPIOB_DRIVEB);
		chThdSleepMilliseconds(2);
		adcConvert(&ADCD1, &adcgrpcfg, samples, ADC_BUF_DEPTH);
		prd->y = samples[0];

		// Set up for reading z again. We know it will be 20ms before we get called again so don't worry about settling time
		palClearPad(GPIOB, GPIOB_DRIVEA);
		palClearPad(GPIOB, GPIOB_DRIVEB);
    }
    return TRUE;
}

#endif /* _LLD_GMOUSE_MCU_BOARD_H */
