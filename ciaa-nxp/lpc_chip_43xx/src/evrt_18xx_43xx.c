/*
 * @brief LPC18xx/43xx event router driver
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

/* Initialize the EVRT */
void Chip_EVRT_Init(void)
{
	uint8_t i = 0;
	// Clear all register to be default
	LPC_EVRT->HILO      = 0x0000;
	LPC_EVRT->EDGE      = 0x0000;
	LPC_EVRT->CLR_EN    = 0xFFFF;
	do {
		i++;
		LPC_EVRT->CLR_STAT  = 0xFFFFF;
	} while ((LPC_EVRT->STATUS != 0) && (i < 10));
}

/* Set up the type of interrupt type for a source to EVRT */
void Chip_EVRT_ConfigIntSrcActiveType(CHIP_EVRT_SRC_T EVRT_Src, CHIP_EVRT_SRC_ACTIVE_T type)
{
	switch (type) {
	case EVRT_SRC_ACTIVE_LOW_LEVEL:
		LPC_EVRT->HILO &= ~(1 << (uint8_t) EVRT_Src);
		LPC_EVRT->EDGE &= ~(1 << (uint8_t) EVRT_Src);
		break;

	case EVRT_SRC_ACTIVE_HIGH_LEVEL:
		LPC_EVRT->HILO |= (1 << (uint8_t) EVRT_Src);
		LPC_EVRT->EDGE &= ~(1 << (uint8_t) EVRT_Src);
		break;

	case EVRT_SRC_ACTIVE_FALLING_EDGE:
		LPC_EVRT->HILO &= ~(1 << (uint8_t) EVRT_Src);
		LPC_EVRT->EDGE |= (1 << (uint8_t) EVRT_Src);
		break;

	case EVRT_SRC_ACTIVE_RISING_EDGE:
		LPC_EVRT->HILO |= (1 << (uint8_t) EVRT_Src);
		LPC_EVRT->EDGE |= (1 << (uint8_t) EVRT_Src);
		break;

	default:
		break;
	}
}

/* Enable or disable interrupt sources to EVRT */
void Chip_EVRT_SetUpIntSrc(CHIP_EVRT_SRC_T EVRT_Src, FunctionalState state)
{
	if (state == ENABLE) {
		LPC_EVRT->SET_EN = (1 << (uint8_t) EVRT_Src);
	}
	else {
		LPC_EVRT->CLR_EN = (1 << (uint8_t) EVRT_Src);
	}
}

/* Check if a source is sending interrupt to EVRT */
IntStatus Chip_EVRT_IsSourceInterrupting(CHIP_EVRT_SRC_T EVRT_Src)
{
	if (LPC_EVRT->STATUS & (1 << (uint8_t) EVRT_Src)) {
		return SET;
	}
	else {return RESET; }
}
