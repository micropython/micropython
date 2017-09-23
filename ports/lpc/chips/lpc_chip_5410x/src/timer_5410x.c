/*
 * @brief LPC5410X 32-bit Timer/PWM driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
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

#include "chip.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

struct TBASE_TO_TMRBITS {
	uint32_t base;
	uint8_t  clockID;
	uint8_t  resetID;
};

#define LAST_TIMER      (4)
static const struct TBASE_TO_TMRBITS tbaseToTimerIDs[LAST_TIMER + 1] = {
	{LPC_TIMER0_BASE, (uint8_t) SYSCON_CLOCK_TIMER0, (uint8_t) RESET_TIMER0},
	{LPC_TIMER1_BASE, (uint8_t) SYSCON_CLOCK_TIMER1, (uint8_t) RESET_TIMER1},
	{LPC_TIMER2_BASE, (uint8_t) SYSCON_CLOCK_TIMER2, (uint8_t) RESET_TIMER2},
	{LPC_TIMER3_BASE, (uint8_t) SYSCON_CLOCK_TIMER3, (uint8_t) RESET_TIMER3},
	{LPC_TIMER4_BASE, (uint8_t) SYSCON_CLOCK_TIMER4, (uint8_t) RESET_TIMER4}
};

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Return index into tbaseToTimerIDs for timers 0-4 */
static int GetClockID(LPC_TIMER_T *pTMR)
{
	int timerId = LAST_TIMER;

	while (timerId >= 0) {
		if (pTMR == (LPC_TIMER_T *) tbaseToTimerIDs[timerId].base) {
			return timerId;
		}

		timerId--;
	}

	/* Waill return timer 0 if no timer match */
	return 0;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize a timer */
void Chip_TIMER_Init(LPC_TIMER_T *pTMR)
{
	int clockId = GetClockID(pTMR);

	Chip_Clock_EnablePeriphClock((CHIP_SYSCON_CLOCK_T) tbaseToTimerIDs[clockId].clockID);
	Chip_SYSCON_PeriphReset((CHIP_SYSCON_PERIPH_RESET_T) tbaseToTimerIDs[clockId].resetID);
}

/*	Shutdown a timer */
void Chip_TIMER_DeInit(LPC_TIMER_T *pTMR)
{
	int clockId = GetClockID(pTMR);

	Chip_Clock_DisablePeriphClock((CHIP_SYSCON_CLOCK_T) tbaseToTimerIDs[clockId].clockID);
}

/* Resets the timer counter and prescale counts to 0 */
void Chip_TIMER_Reset(LPC_TIMER_T *pTMR)
{
	uint32_t reg;

	/* Disable timer, set terminal count to non-0 */
	reg = pTMR->TCR;
	pTMR->TCR = 0;
	pTMR->TC = 1;

	/* Reset timer counter */
	pTMR->TCR = TIMER_RESET;

	/* Wait for terminal count to clear */
	while (pTMR->TC != 0) {}

	/* Restore timer state */
	pTMR->TCR = reg;
}

/* Sets external match control (MATn.matchnum) pin control */
void Chip_TIMER_ExtMatchControlSet(LPC_TIMER_T *pTMR, int8_t initial_state,
								   TIMER_PIN_MATCH_STATE_T matchState, int8_t matchnum)
{
	uint32_t mask, reg;

	/* Clear bits corresponding to selected match register */
	mask = (1 << matchnum) | (0x03 << (4 + (matchnum * 2)));
	/* Also mask reserved bits */
	reg = (pTMR->EMR & TIMER_EMR_MASK) & ~mask;

	/* Set new configuration for selected match register */
	pTMR->EMR = reg | (((uint32_t) initial_state) << matchnum) |
				(((uint32_t) matchState) << (4 + (matchnum * 2)));
}
