/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef GAUDIO_PLAY_BOARD_H
#define GAUDIO_PLAY_BOARD_H

/* Our timer callback */
static void gptcallback(GPTDriver *gptp) {
	(void) gptp;
	gaudio_play_pwm_timer_callbackI();
}

/* PWM configuration structure. The speaker is on PWM0/PB19 ie PWM1/PIN1 in ChibiOS speak */
static PWMConfig pwmcfg = {
  1000000,		/* 1 MHz PWM clock frequency. Ignored as we are using PWM_MCK_DIV_n */
  1024,			/* PWM period is 1024 cycles (10 bits). */
  0,
  {
   {PWM_MCK_DIV_1 | PWM_OUTPUT_CENTER | PWM_OUTPUT_ACTIVE_HIGH | PWM_OUTPUT_PIN1 | PWM_DISABLEPULLUP_PIN1, 0},
  },
};

/* Timer configuration structure. We use Timer 2 (TC1) */
static GPTConfig gptcfg = {
  8192,							// frequency
  gptcallback,					// callback
  GPT_CLOCK_FREQUENCY,			// clocksource
  GPT_GATE_NONE,				// clockgate
  GPT_TRIGGER_NONE,				// trigger
};

static uint16_t		lastvalue;

static bool gaudio_play_pwm_setup(uint32_t frequency, ArrayDataFormat format) {
	if (format == ARRAY_DATA_10BITUNSIGNED)
		pwmcfg.period = 1024;
	else if (format == ARRAY_DATA_8BITUNSIGNED)
		pwmcfg.period = 256;
	else
		return FALSE;
	gptcfg.frequency = frequency;
	return TRUE;
}

static void gaudio_play_pwm_start(void) {
	/* Start the PWM */
	pwmStart(&PWMD1, &pwmcfg);
	lastvalue = pwmcfg.period>>1;
	pwmEnableChannelI(&PWMD1, 0, lastvalue);

	/* Start the timer interrupt */
	gptStart(&GPTD2, &gptcfg);
	gptStartContinuous(&GPTD2, 0);
}

static void gaudio_play_pwm_stop(void) {
	/* Stop the timer interrupt */
	gptStopTimer(&GPTD2);

	/* Stop the PWM */
	pwmStop(&PWMD1);
}

static void gaudio_play_pwm_setI(uint16_t value) {
	if (value != lastvalue) {
		lastvalue = value;
		pwmEnableChannelI(&PWMD1, 0, value);
	}
}

#endif	/* GAUDIO_PLAY_BOARD_H */
