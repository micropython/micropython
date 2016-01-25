/*
 * @brief LPC18xx/43xx High speed ADC driver
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

/* CRS and DGEC values mapped to maximum HSADC sample rates */
typedef struct {
	uint32_t minRate;
	uint8_t crs;
	uint8_t dgec;
} CRSDCEG_T;
static const CRSDCEG_T powerSets[] = {
	{20000000,   0, 0x0},	/* Use 0/0 for less than 20MHz */
	{30000000,   1, 0x0},	/* Use 1/0 for less than 30MHz */
	{50000000,   2, 0x0},	/* Use 2/0 for less than 50MHz */
	{65000000,   3, 0xF},	/* Use 3/F for less than 65MHz */
	{0xFFFFFFFF, 4, 0xE},	/* Use 4/E for everything else */
};

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize the High speed ADC */
void Chip_HSADC_Init(LPC_HSADC_T *pHSADC)
{
	/* Enable HSADC register clock */
	Chip_Clock_EnableOpts(CLK_MX_ADCHS, true, true, 1);

	/* Enable HSADC sample clock */
	Chip_Clock_Enable(CLK_ADCHS);

	/* Reset HSADC, will auto-clear */
	Chip_RGU_TriggerReset(RGU_ADCHS_RST);
}

/* Shutdown ADC */
void Chip_HSADC_DeInit(LPC_HSADC_T *pHSADC)
{
	/* Power down */
	Chip_HSADC_DisablePower(pHSADC);

	/* Reset HSADC and wait for clear, will auto-clear */
	Chip_RGU_TriggerReset(RGU_ADCHS_RST);
	while (Chip_RGU_InReset(RGU_ADCHS_RST)) {}

	/* SHutdown HSADC clock after reset is complete */
	Chip_Clock_Disable(CLK_MX_ADCHS);
}

/* Sets up HSADC FIFO trip level and packing */
void Chip_HSADC_SetupFIFO(LPC_HSADC_T *pHSADC, uint8_t trip, bool packed)
{
	uint32_t val = (uint32_t) trip << 1;
	if (packed) {
		val |= 1;
	}

	pHSADC->FIFO_CFG = val;
}

/* Set HSADC Threshold low value */
void Chip_HSADC_SetThrLowValue(LPC_HSADC_T *pHSADC, uint8_t thrnum, uint16_t value)
{
	uint32_t reg;

	reg = pHSADC->THR[thrnum] & ~0xFFF;
	pHSADC->THR[thrnum] = reg | (uint32_t) value;
}

/* Set HSADC Threshold high value */
void Chip_HSADC_SetThrHighValue(LPC_HSADC_T *pHSADC, uint8_t thrnum, uint16_t value)
{
	uint32_t reg;

	reg = pHSADC->THR[thrnum] & ~0xFFF0000;
	pHSADC->THR[thrnum] = reg | (((uint32_t) value) << 16);
}

/* Setup speed for a input channel */
void Chip_HSADC_SetSpeed(LPC_HSADC_T *pHSADC, uint8_t channel, uint8_t speed)
{
	uint32_t reg, shift = channel * 4;

	reg = pHSADC->ADC_SPEED & ~(0xF << shift);
	pHSADC->ADC_SPEED = reg | (speed << shift);
}

/* Setup (common) HSADC power and speed settings */
void Chip_HSADC_SetPowerSpeed(LPC_HSADC_T *pHSADC, bool comp2)
{
	uint32_t rate, val, orBits;
	int i, idx;

	/* Get current clock rate for HSADC */
	rate = Chip_HSADC_GetBaseClockRate(pHSADC);

	/* Determine optimal CRS and DCEG settings based on clock rate */
	idx = 0;
	while (rate > powerSets[idx].minRate) {
		idx++;
	}

	/* Add CRS selection based on clock speed */
	orBits = powerSets[idx].crs;

	/* Enable 2's complement data format? */
	if (comp2) {
		orBits |= (1 << 16);
	}

	/* Update DCEG settings for all channels based on current CRS */
	for (i = 0; i < 6; i++) {
		Chip_HSADC_SetSpeed(pHSADC, i, powerSets[idx].dgec);
	}

	/* Get current power control register value and mask off bits that
	   may change */
	val = pHSADC->POWER_CONTROL & ~((1 << 16) | 0xF);

	/* Update with new power and data format settings */
	pHSADC->POWER_CONTROL = val | orBits;
}

/* Setup AC-DC coupling selection for a channel */
void Chip_HSADC_SetACDCBias(LPC_HSADC_T *pHSADC, uint8_t channel,
							HSADC_DCBIAS_T dcInNeg, HSADC_DCBIAS_T dcInPos)
{
	uint32_t reg, mask, orBits;

	/* Build mask and enable words for selected DCINNEG abd DCINPOS
	   fields for the selected channel */
	mask = ((1 << 4) | (1 << 10)) << channel;
	orBits = (((uint32_t) dcInNeg << 4) | ((uint32_t) dcInPos << 10)) << channel;

	reg = pHSADC->POWER_CONTROL & ~mask;
	pHSADC->POWER_CONTROL = reg | orBits;
}
