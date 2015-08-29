/*
 * @brief LPC18xx/43xx D/A conversion driver
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

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize the DAC peripheral */
void Chip_DAC_Init(LPC_DAC_T *pDAC)
{
	Chip_Clock_EnableOpts(CLK_APB3_DAC, true, true, 1);

	/* Set maximum update rate 1MHz */
	Chip_DAC_SetBias(pDAC, DAC_MAX_UPDATE_RATE_1MHz);
}

/* Shutdown DAC peripheral */
void Chip_DAC_DeInit(LPC_DAC_T *pDAC)
{
	Chip_Clock_Disable(CLK_APB3_DAC);
}

/* Update value to DAC buffer*/
void Chip_DAC_UpdateValue(LPC_DAC_T *pDAC, uint32_t dac_value)
{
	uint32_t tmp;

	tmp = pDAC->CR & DAC_BIAS_EN;
	tmp |= DAC_VALUE(dac_value);
	/* Update value */
	pDAC->CR = tmp;
}

/* Set Maximum update rate for DAC */
void Chip_DAC_SetBias(LPC_DAC_T *pDAC, uint32_t bias)
{
	pDAC->CR &= ~DAC_BIAS_EN;

	if (bias  == DAC_MAX_UPDATE_RATE_400kHz) {
		pDAC->CR |= DAC_BIAS_EN;
	}
}

