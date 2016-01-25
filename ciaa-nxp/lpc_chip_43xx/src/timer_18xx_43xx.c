/*
 * @brief LPC18xx/43xx 16/32-bit Timer/PWM driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
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

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Returns clock index for the peripheral block */
STATIC CHIP_CCU_CLK_T Chip_TIMER_GetClockIndex(LPC_TIMER_T *pTMR)
{
	CHIP_CCU_CLK_T clkTMR;

	if (pTMR == LPC_TIMER3) {
		clkTMR = CLK_MX_TIMER3;
	}
    else if (pTMR == LPC_TIMER2) {
		clkTMR = CLK_MX_TIMER2;
	}
    else if (pTMR == LPC_TIMER1) {
		clkTMR = CLK_MX_TIMER1;
	}
	else {
		clkTMR = CLK_MX_TIMER0;
	}

	return clkTMR;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize a timer */
void Chip_TIMER_Init(LPC_TIMER_T *pTMR)
{
	Chip_Clock_Enable(Chip_TIMER_GetClockIndex(pTMR));
}

/*	Shutdown a timer */
void Chip_TIMER_DeInit(LPC_TIMER_T *pTMR)
{
	Chip_Clock_Disable(Chip_TIMER_GetClockIndex(pTMR));
}

/* Resets the timer terminal and prescale counts to 0 */
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
	reg = pTMR->EMR &= ~mask;

	/* Set new configuration for selected match register */
	pTMR->EMR = reg | (((uint32_t) initial_state) << matchnum) |
				(((uint32_t) matchState) << (4 + (matchnum * 2)));
}

