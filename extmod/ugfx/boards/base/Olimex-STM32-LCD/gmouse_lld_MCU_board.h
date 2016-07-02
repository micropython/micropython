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
#define GMOUSE_MCU_Z_MAX					1
#define GMOUSE_MCU_Z_TOUCHON				1
#define GMOUSE_MCU_Z_TOUCHOFF				0

// How much extra data to allocate at the end of the GMouse structure for the board's use
#define GMOUSE_MCU_BOARD_DATA_SIZE		0

#define ADC_NUM_CHANNELS   2
#define ADC_BUF_DEPTH      1

static const ADCConversionGroup adc_y_config = {
    FALSE,
    ADC_NUM_CHANNELS,
    0,
    0,
    0, 0,
    0, 0,
    ADC_SQR1_NUM_CH(ADC_NUM_CHANNELS),
    0,
    ADC_SQR3_SQ2_N(ADC_CHANNEL_IN12) | ADC_SQR3_SQ1_N(ADC_CHANNEL_IN13)
};

static const ADCConversionGroup adc_x_config = {
    FALSE,
    ADC_NUM_CHANNELS,
    0,
    0,
    0, 0,
    0, 0,
    ADC_SQR1_NUM_CH(ADC_NUM_CHANNELS),
    0,
    ADC_SQR3_SQ2_N(ADC_CHANNEL_IN10) | ADC_SQR3_SQ1_N(ADC_CHANNEL_IN11)
};

static GFXINLINE void setup_z(void) {
	palSetPadMode(GPIOC, 0, PAL_MODE_INPUT_PULLDOWN);
	palSetPadMode(GPIOC, 1, PAL_MODE_INPUT);
	palSetPadMode(GPIOC, 2, PAL_MODE_INPUT);
	palSetPadMode(GPIOC, 3, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPad(GPIOC, 3);
}

static bool_t init_board(GMouse *m, unsigned driverinstance) {
	(void)	m;

	// Only one touch interface on this board
	if (driverinstance)
		return FALSE;

	adcStart(&ADCD1, 0);

	// Set up for reading Z
	setup_z();
    chThdSleepMilliseconds(1);				// Settling time
	return TRUE;
}

static bool_t read_xyz(GMouse *m, GMouseReading *prd) {
	adcsample_t samples[ADC_NUM_CHANNELS * ADC_BUF_DEPTH];
	uint16_t val1, val2;
	(void)		m;

	// No buttons and assume touch off
	prd->buttons = 0;
	prd->z = 0;

	// Get the z reading (assumes we are ready to read z)
    // Take a shortcut and don't read x, y if we know we are definitely not touched.
	if (palReadPad(GPIOC, 0)) {
		prd->z = 1;

		// Get the x reading - Weird but it works. Optimize later.
		palSetPadMode(GPIOC, 0, PAL_MODE_INPUT_ANALOG);
		palSetPadMode(GPIOC, 1, PAL_MODE_INPUT_ANALOG);
		palSetPadMode(GPIOC, 2, PAL_MODE_OUTPUT_PUSHPULL);
		palSetPadMode(GPIOC, 3, PAL_MODE_OUTPUT_PUSHPULL);

		palSetPad(GPIOC, 2);
	    palClearPad(GPIOC, 3);
	    gfxSleepMilliseconds(1);
	    adcConvert(&ADCD1, &adc_x_config, samples, ADC_BUF_DEPTH);
	    val1 = ((samples[0] + samples[1])/2);

		palClearPad(GPIOC, 2);
	    palSetPad(GPIOC, 3);
	    gfxSleepMilliseconds(1);
	    adcConvert(&ADCD1, &adc_x_config, samples, ADC_BUF_DEPTH);
	    val2 = ((samples[0] + samples[1])/2);

	    prd->x = ((val1+((1<<12)-val2))/4);

		// Get the y reading - Weird but it works. Optimize later.
		palSetPadMode(GPIOC, 2, PAL_MODE_INPUT_ANALOG);
		palSetPadMode(GPIOC, 3, PAL_MODE_INPUT_ANALOG);
		palSetPadMode(GPIOC, 0, PAL_MODE_OUTPUT_PUSHPULL);
		palSetPadMode(GPIOC, 1, PAL_MODE_OUTPUT_PUSHPULL);

	    palSetPad(GPIOC, 1);
	    palClearPad(GPIOC, 0);
	    gfxSleepMilliseconds(1);
	    adcConvert(&ADCD1, &adc_y_config, samples, ADC_BUF_DEPTH);
	    val1 = ((samples[0] + samples[1])/2);

	    palClearPad(GPIOC, 1);
	    palSetPad(GPIOC, 0);
	    gfxSleepMilliseconds(1);
	    adcConvert(&ADCD1, &adc_y_config, samples, ADC_BUF_DEPTH);
	    val2 = ((samples[0] + samples[1])/2);

	    prd->y = ((val1+((1<<12)-val2))/4);

		// Set up for reading z again. We know it will be 20ms before we get called again so don't worry about settling time
	    setup_z();
    }
	return TRUE;
}

#endif /* _LLD_GMOUSE_MCU_BOARD_H */
