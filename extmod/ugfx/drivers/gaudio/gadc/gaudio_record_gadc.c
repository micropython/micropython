/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#define GAUDIO_RECORD_IMPLEMENTATION
#include "gfx.h"

#if GFX_USE_GAUDIO && GAUDIO_NEED_RECORD

/* Double check the GADC system is turned on */
#if !GFX_USE_GADC
	#error "GAUDIO - The GADC driver for GAUDIO requires GFX_USE_GADC to be TRUE"
#endif

/* Include the driver defines */
#include "../../../src/gaudio/gaudio_driver_record.h"

static void gadcCallbackI(void) {
	GDataBuffer *pd;

	pd = gadcHighSpeedGetDataI();
	if (pd)
		gaudioRecordSaveDataBlockI(pd);
}

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

bool_t gaudio_record_lld_init(uint16_t channel, uint32_t frequency, ArrayDataFormat format) {
	/* Check the parameters */
	if (channel >= GAUDIO_RECORD_NUM_CHANNELS || frequency > GAUDIO_RECORD_MAX_SAMPLE_FREQUENCY || format != GAUDIO_RECORD_FORMAT1)
		return FALSE;

	/* Setup the high speed GADC */
	gadcHighSpeedInit(gaudio_gadc_physdevs[channel], frequency);

	/* Register ourselves for ISR callbacks */
	gadcHighSpeedSetISRCallback(gadcCallbackI);

	return TRUE;
}

void gaudio_record_lld_start(void) {
	gadcHighSpeedStart();
}

void gaudio_record_lld_stop(void) {
	gadcHighSpeedStop();
}

#endif /* GFX_USE_GAUDIO && GAUDIO_NEED_RECORD */
