/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef GAUDIO_RECORD_CONFIG_H
#define GAUDIO_RECORD_CONFIG_H

#if GFX_USE_GAUDIO && GAUDIO_NEED_RECORD

/*===========================================================================*/
/* Driver hardware support.                                                  */
/*===========================================================================*/

#define GAUDIO_RECORD_MAX_SAMPLE_FREQUENCY			GADC_MAX_HIGH_SPEED_SAMPLERATE
#define GAUDIO_RECORD_NUM_FORMATS					1
#define GAUDIO_RECORD_FORMAT1						GADC_SAMPLE_FORMAT

/* Include the user supplied board definitions */
#include "gaudio_record_board.h"

#endif	/* GFX_USE_GAUDIO && GAUDIO_NEED_RECORD */

#endif	/* GAUDIO_RECORD_CONFIG_H */
