/*
 * @brief LPC5410X Pin Interrupt and Pattern Match Registers and driver
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

/* Set source for pattern match engine */
void Chip_PININT_SetPatternMatchSrc(LPC_PIN_INT_T *pPININT,
									Chip_PININT_SELECT_T channelNum,
									Chip_PININT_BITSLICE_T sliceNum)
{
	uint32_t pmsrc_reg;

	/* Source source for pattern matching */
	pmsrc_reg = pPININT->PMSRC & ~(PININT_SRC_BITSOURCE_MASK << (PININT_SRC_BITSOURCE_START + (sliceNum * 3)));
	pPININT->PMSRC = pmsrc_reg | (channelNum << (PININT_SRC_BITSOURCE_START + (sliceNum * 3)));
}

/* Configure Pattern match engine */
void Chip_PININT_SetPatternMatchConfig(LPC_PIN_INT_T *pPININT, Chip_PININT_BITSLICE_T sliceNum,
									   Chip_PININT_BITSLICE_CFG_T slice_cfg, bool end_point)
{	
	uint32_t pmcfg_reg, idx = PININT_SRC_BITCFG_START + (sliceNum * 3);

	/* Configure bit slice configuration */
	pmcfg_reg = pPININT->PMCFG;
	pmcfg_reg = (pmcfg_reg & ~(PININT_SRC_BITCFG_MASK << idx)) | (slice_cfg << idx);

	/* If end point is true, enable the bits */
	if (sliceNum != PININTBITSLICE7) {
		if (end_point) {
			pmcfg_reg |= (0x1 << sliceNum);
		}
		else {
			pmcfg_reg &= ~(0x1 << sliceNum);
		}
	}

	pPININT->PMCFG = pmcfg_reg;
}
