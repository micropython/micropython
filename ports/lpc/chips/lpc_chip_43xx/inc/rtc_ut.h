/*
 * @brief RTC tick to (a more) Universal Time
 * Adds conversion functions to use an RTC that only provides a
 * seconds capability to provide "struct tm" support.
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licenser disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __RTC_UT_H_
#define __RTC_UT_H_

#include "chip.h"
#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup RTC_UT CHIP: RTC tick to (a more) Universal Time conversion functions
 * @ingroup CHIP_Common
 * This driver converts between a RTC 1-second tick value and
 * a Universal time format in a structure of type 'struct tm'.
 * @{
 */

/* Starting year and starting day of week for the driver */
#define TM_YEAR_BASE    (1900)
#define TM_DAYOFWEEK    (1)

/**
 * @brief	Converts a RTC tick time to Universal time
 * @param	rtcTick	: Current RTC time value
 * @param	pTime	: Pointer to time structure to fill
 * @return	Nothing
 * @note	When setting time, the 'tm_wday', 'tm_yday', and 'tm_isdst'
 * fields are not used.
 */
void ConvertRtcTime(uint32_t rtcTick, struct tm *pTime);

/**
 * @brief	Converts a Universal time to RTC tick time
 * @param	pTime	: Pointer to time structure to use
 * @param	rtcTick	: Pointer to RTC time value to fill
 * @return	Nothing
 * @note	When converting time, the 'tm_isdst' field is not
 * populated by the conversion function.
 */
void ConvertTimeRtc(struct tm *pTime, uint32_t *rtcTick);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTC_UT_H_ */






