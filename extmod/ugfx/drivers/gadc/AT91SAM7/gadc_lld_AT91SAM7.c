/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gadc/AT91SAM7/gadc_lld.c
 * @brief   GADC - Periodic ADC driver source file for the AT91SAM7 cpu.
 */

#include "gfx.h"

#if GFX_USE_GADC

#include "../../../src/gadc/gadc_driver.h"

static uint32_t		nextfreq;

// Forward references to ISR routines
static void ISR_CompleteI(ADCDriver *adcp, adcsample_t *buffer, size_t n);
static void ISR_ErrorI(ADCDriver *adcp, adcerror_t err);

static ADCConversionGroup acg = {
		FALSE,					// circular
		1,						// num_channels
		ISR_CompleteI,			// end_cb
		ISR_ErrorI,				// error_cb
		0,						// channelselects
		0,						// trigger
		0,						// frequency
		};

static void ISR_CompleteI(ADCDriver *adcp, adcsample_t *buffer, size_t n) {
	(void)	adcp;
	(void)	buffer;

	gadcGotDataI(n);
}

static void ISR_ErrorI(ADCDriver *adcp, adcerror_t err) {
	(void)	adcp;
	(void)	err;

	gadcGotDataI(0);
}

void gadc_lld_init(void) {
	adcStart(&ADCD1, 0);
}

size_t gadc_lld_samplesperconversion(uint32_t physdev) {
	size_t	samples;

	for(samples = 0; physdev; physdev >>= 1)
		if (physdev & 0x01)
			samples++;
	return samples;
}

void gadc_lld_start_timerI(uint32_t frequency) {
	// Nothing to do here - the AT91SAM7 adc driver uses an internal timer
	//	which is set up when the job is started. We save this here just to
	//	indicate the timer should be re-initialised on the next timer job
	nextfreq = frequency;
}

void gadc_lld_stop_timerI(void) {
	// Nothing to do here. The AT91SAM7 adc driver automatically turns off timer interrupts
	//	on completion of the job
}

void gadc_lld_timerjobI(GadcTimerJob *pj) {
	acg.channelselects = pj->physdev;
	acg.trigger = ADC_TRIGGER_TIMER;
	acg.frequency = nextfreq;
	nextfreq = 0;						// Next job use the same timer
	adcStartConversionI(&ADCD1, &acg, pj->buffer, pj->todo);
}

void gadc_lld_nontimerjobI(GadcNonTimerJob *pj) {
	acg.channelselects = pj->physdev;
	acg.trigger = ADC_TRIGGER_SOFTWARE;
	adcStartConversionI(&ADCD1, &acg, pj->buffer, 1);
}

#endif /* GFX_USE_GADC */
