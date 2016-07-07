/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gadc/AT91SAM7/gadc_lld_config.h
 * @brief   GADC Driver config file.
 *
 * @addtogroup GADC
 * @{
 */

#ifndef GADC_LLD_CONFIG_H
#define GADC_LLD_CONFIG_H

#if GFX_USE_GADC

/*===========================================================================*/
/* Driver hardware support.                                                  */
/*===========================================================================*/

/**
 * @brief	ChibiOS has a nasty bug in its _adc_isr_full_code() routine (defined in adc.h as a macro).
 * 			Do we have the version of ChibiOS with this bug.
 * @detail	Set to TRUE if ChibiOS has this bug.
 * @note	Fixed in ChibiOS 2.4.4stable and 2.5.2unstable (and the repository from 18th Feb 2013)
 * @note	This bug prevents us re-calling adcStartConversionI() from with the ISR even though
 * 			it is clearly designed to handle it. For some reason (on this micro) the high speed timer
 * 			is not affected only the single sample low speed timer. In that situation we wait until
 * 			we get back to thread land. This is terrible for the accuracy of the high speed timer
 * 			but what can we do (other than fix the bug).
 * @note	For the AT91SAM7 ADC driver, it post-dates the finding of the bug so we safely
 * 			say that the bug doesn't exist for this driver.
 */
#define CHIBIOS_ADC_ISR_FULL_CODE_BUG		FALSE

/**
 * @brief	The maximum sample frequency supported by this CPU
 */
#define GADC_MAX_SAMPLE_FREQUENCY			132000

/**
 * @brief	The sample format
 */
#define GADC_SAMPLE_FORMAT					ARRAY_DATA_10BITUNSIGNED

/* Include the user supplied board definitions */
#include "gadc_lld_board.h"

#endif	/* GFX_USE_GADC */

#endif	/* GADC_LLD_CONFIG_H */
/** @} */

