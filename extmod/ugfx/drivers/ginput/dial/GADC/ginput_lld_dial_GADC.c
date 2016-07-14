/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GINPUT && GINPUT_NEED_DIAL

#include "../../../../src/ginput/ginput_driver_dial.h"

#if GINPUT_DIAL_NUM_PORTS >= 5
	#error "GINPUT: Dial - GADC driver currently only supports 4 devices"
#endif

static adcsample_t samplebuf[GINPUT_DIAL_NUM_PORTS];

#if GINPUT_DIAL_CYCLE_POLL
	static uint16_t		cycle;
#endif

static void GADCCallback(adcsample_t *buffer, void *param) {
	((DialCallbackFn)param)(buffer-samplebuf, buffer[0]);
}

void ginput_lld_dial_init(void) {
	#if GINPUT_DIAL_CYCLE_POLL
		cycle = 0;
	#endif
}

void ginput_lld_dial_poll(DialCallbackFn fn) {
	#if GINPUT_DIAL_CYCLE_POLL
		switch(cycle++) {
		#if GINPUT_DIAL_NUM_PORTS >= 1
			case 0:
				gadcLowSpeedStart(GINPUT_DIAL_DEVICE0, samplebuf+0, GADCCallback, (void *)fn);
				break;
		#endif
		#if GINPUT_DIAL_NUM_PORTS >= 2
			case 1:
				gadcLowSpeedStart(GINPUT_DIAL_DEVICE1, samplebuf+1, GADCCallback, (void *)fn);
				break;
		#endif
		#if GINPUT_DIAL_NUM_PORTS >= 3
			case 2:
				gadcLowSpeedStart(GINPUT_DIAL_DEVICE2, samplebuf+2, GADCCallback, (void *)fn);
				break;
		#endif
		#if GINPUT_DIAL_NUM_PORTS >= 4
			case 3:
				gadcLowSpeedStart(GINPUT_DIAL_DEVICE3, samplebuf+3, GADCCallback, (void *)fn);
				break;
		#endif
		}
		if (cycle >= GINPUT_DIAL_NUM_PORTS)
			cycle = 0;
	#else
		#if GINPUT_DIAL_NUM_PORTS >= 1
			gadcLowSpeedStart(GINPUT_DIAL_DEVICE0, samplebuf+0, GADCCallback, (void *)fn);
		#endif
		#if GINPUT_DIAL_NUM_PORTS >= 2
			gadcLowSpeedStart(GINPUT_DIAL_DEVICE1, samplebuf+1, GADCCallback, (void *)fn);
		#endif
		#if GINPUT_DIAL_NUM_PORTS >= 3
			gadcLowSpeedStart(GINPUT_DIAL_DEVICE2, samplebuf+2, GADCCallback, (void *)fn);
		#endif
		#if GINPUT_DIAL_NUM_PORTS >= 4
			gadcLowSpeedStart(GINPUT_DIAL_DEVICE3, samplebuf+3, GADCCallback, (void *)fn);
		#endif
	#endif
}

#endif /* GFX_USE_GINPUT && GINPUT_NEED_DIAL */
