/*
 * @brief LPC5410X State Configurable Timer driver
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

/* Initialize SCT */
void Chip_SCT_Init(LPC_SCT_T *pSCT)
{
	Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_SCT0);
	Chip_SYSCON_PeriphReset(RESET_SCT0);
}

/* Shutdown SCT */
void Chip_SCT_DeInit(LPC_SCT_T *pSCT)
{
	Chip_Clock_DisablePeriphClock(SYSCON_CLOCK_SCT0);
}

/* Set/Clear SCT control register */
void Chip_SCT_SetClrControl(LPC_SCT_T *pSCT, uint32_t value, FunctionalState ena)
{
	if (ena == ENABLE) {
		Chip_SCT_SetControl(pSCT, value);
	}
	else {
		Chip_SCT_ClearControl(pSCT, value);
	}
}

/* Set Conflict resolution */
void Chip_SCT_SetConflictResolution(LPC_SCT_T *pSCT, uint8_t outnum, uint8_t value)
{
	uint32_t tem;

	tem = pSCT->RES & (~(0x03 << (2 * outnum)));
	pSCT->RES = tem | (value << (2 * outnum));
}
