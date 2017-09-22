/*
 * @brief Common stopwatch support
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
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

#ifndef __STOPWATCH_H_
#define __STOPWATCH_H_

#include "cmsis.h"

/** @defgroup Stop_Watch CHIP: Stopwatch primitives.
 * @ingroup CHIP_Common
 * @{
 */

/**
 * @brief	Initialize stopwatch
 * @return	Nothing
 */
void StopWatch_Init(void);

/**
 * @brief	Start a stopwatch
 * @return	Current cycle count
 */
uint32_t StopWatch_Start(void);

/**
 * @brief      Returns number of ticks elapsed since stopwatch was started
 * @param      startTime	: Time returned by StopWatch_Start().
 * @return     Number of ticks elapsed since stopwatch was started
 */
STATIC INLINE uint32_t StopWatch_Elapsed(uint32_t startTime)
{
	return StopWatch_Start() - startTime;
}

/**
 * @brief	Returns number of ticks per second of the stopwatch timer
 * @return	Number of ticks per second of the stopwatch timer
 */
uint32_t StopWatch_TicksPerSecond(void);

/**
 * @brief	Converts from stopwatch ticks to mS.
 * @param	ticks	: Duration in ticks to convert to mS.
 * @return	Number of mS in given number of ticks
 */
uint32_t StopWatch_TicksToMs(uint32_t ticks);

/**
 * @brief	Converts from stopwatch ticks to uS.
 * @param	ticks	: Duration in ticks to convert to uS.
 * @return	Number of uS in given number of ticks
 */
uint32_t StopWatch_TicksToUs(uint32_t ticks);

/**
 * @brief	Converts from mS to stopwatch ticks.
 * @param	mS	: Duration in mS to convert to ticks.
 * @return	Number of ticks in given number of mS
 */
uint32_t StopWatch_MsToTicks(uint32_t mS);

/**
 * @brief	Converts from uS to stopwatch ticks.
 * @param	uS	: Duration in uS to convert to ticks.
 * @return	Number of ticks in given number of uS
 */
uint32_t StopWatch_UsToTicks(uint32_t uS);

/**
 * @brief	Delays the given number of ticks using stopwatch primitives
 * @param	ticks	: Number of ticks to delay
 * @return	Nothing
 */
STATIC INLINE void StopWatch_DelayTicks(uint32_t ticks)
{
	uint32_t startTime = StopWatch_Start();
	while (StopWatch_Elapsed(startTime) < ticks) {}
}

/**
 * @brief	Delays the given number of mS using stopwatch primitives
 * @param	mS	: Number of mS to delay
 * @return	Nothing
 */
STATIC INLINE void StopWatch_DelayMs(uint32_t mS)
{
	uint32_t ticks = StopWatch_MsToTicks(mS);
	uint32_t startTime = StopWatch_Start();
	while (StopWatch_Elapsed(startTime) < ticks) {}
}

/**
 * @brief	Delays the given number of uS using stopwatch primitives
 * @param	uS	: Number of uS to delay
 * @return	Nothing
 */
STATIC INLINE void StopWatch_DelayUs(uint32_t uS)
{
	uint32_t ticks = StopWatch_UsToTicks(uS);
	uint32_t startTime = StopWatch_Start();
	while (StopWatch_Elapsed(startTime) < ticks) {}
}

/**
 * @}
 */

#endif /* __STOPWATCH_H_ */






