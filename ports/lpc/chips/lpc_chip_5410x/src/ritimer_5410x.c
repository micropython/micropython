/*
 * @brief LPC5410X RITimer chip driver
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

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize the RIT */
void Chip_RIT_Init(LPC_RITIMER_T *pRITimer)
{
	Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_RIT);
	Chip_SYSCON_PeriphReset(RESET_RIT);

	/* Default is timer disabled */
	pRITimer->CTRL = 0x0;
}

/* DeInitialize the RIT */
void Chip_RIT_DeInit(LPC_RITIMER_T *pRITimer)
{
	pRITimer->CTRL = 0x0;
	Chip_Clock_DisablePeriphClock(SYSCON_CLOCK_RIT);
}

/* Set timer interval value */
void Chip_RIT_SetTimerInterval(LPC_RITIMER_T *pRITimer, uint32_t time_interval)
{
	uint32_t cmp_value;

	/* Determine aapproximate compare value based on clock rate and passed interval */
	cmp_value = (Chip_Clock_GetMainClockRate() / 1000) * time_interval;

	/* Set timer compare value */
	Chip_RIT_SetCOMPVAL(pRITimer, cmp_value);
}

/* Set timer interval value (48-bit) */
void Chip_RIT_SetTimerInterval64(LPC_RITIMER_T *pRITimer, uint64_t time_interval)
{
	uint64_t cmp_value;

	/* Determine aapproximate compare value based on clock rate and passed interval */
	cmp_value = (uint64_t) Chip_Clock_GetMainClockRate() / 1000;
	cmp_value = cmp_value * time_interval;

	/* Set timer compare value */
	Chip_RIT_SetCOMPVAL64(pRITimer, cmp_value);
}

/* Check whether interrupt is pending */
IntStatus Chip_RIT_GetIntStatus(LPC_RITIMER_T *pRITimer)
{
	uint8_t result;

	if ((pRITimer->CTRL & RIT_CTRL_INT) == 1) {
		result = SET;
	}
	else {
		return RESET;
	}

	return (IntStatus) result;
}
