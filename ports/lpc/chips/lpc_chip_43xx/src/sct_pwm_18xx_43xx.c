/*
 * @brief LPC18xx_43xx State Configurable Timer PWM driver
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

/* Setup the OUTPUT pin corresponding to the PWM index */
void Chip_SCTPWM_SetOutPin(LPC_SCT_T *pSCT, uint8_t index, uint8_t pin)
{
	int ix = (int) index;
	pSCT->EVENT[ix].CTRL = index | (1 << 12);
	pSCT->EVENT[ix].STATE = 1;
	pSCT->OUT[pin].SET = 1;
	pSCT->OUT[pin].CLR = 1 << ix;

	/* Clear the output in-case of conflict */
	pSCT->RES = (pSCT->RES & ~(3 << (pin << 1))) | (0x01 << (pin << 1));

	/* Set and Clear do not depend on direction */
	pSCT->OUTPUTDIRCTRL = (pSCT->OUTPUTDIRCTRL & ~(3 << (pin << 1)));
}

/* Set the PWM frequency */
void Chip_SCTPWM_SetRate(LPC_SCT_T *pSCT, uint32_t freq)
{
	uint32_t rate;

	rate = Chip_Clock_GetRate(CLK_MX_SCT) / freq;;

	/* Stop the SCT before configuration */
	Chip_SCTPWM_Stop(pSCT);

	/* Set MATCH0 for max limit */
	pSCT->REGMODE_L = 0;
	pSCT->REGMODE_H = 0;
	Chip_SCT_SetMatchCount(pSCT, SCT_MATCH_0, 0);
	Chip_SCT_SetMatchReload(pSCT, SCT_MATCH_0, rate);
	pSCT->EVENT[0].CTRL = 1 << 12;
	pSCT->EVENT[0].STATE = 1;
	pSCT->LIMIT_L = 1;

	/* Set SCT Counter to count 32-bits and reset to 0 after reaching MATCH0 */
	Chip_SCT_Config(pSCT, SCT_CONFIG_32BIT_COUNTER | SCT_CONFIG_AUTOLIMIT_L);
}






