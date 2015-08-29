/*
 * @brief LPC18xx/43xx A/D conversion driver
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

/* Get the number of clock for a full conversion */
STATIC INLINE uint8_t getFullConvClk(void)
{
	return 11;
}

/* Returns clock index for the peripheral block */
STATIC CHIP_CCU_CLK_T Chip_ADC_GetClockIndex(LPC_ADC_T *pADC)
{
	CHIP_CCU_CLK_T clkADC;

	if (pADC == LPC_ADC1) {
		clkADC = CLK_APB3_ADC1;
	}
	else {
		clkADC = CLK_APB3_ADC0;
	}

	return clkADC;
}

/* Get divider value */
STATIC uint8_t getClkDiv(LPC_ADC_T *pADC, bool burstMode, uint32_t adcRate, uint8_t clks)
{
	uint32_t adcBlockFreq;
	uint32_t fullAdcRate;
	uint8_t div;

	/* The APB clock (PCLK_ADC0) is divided by (CLKDIV+1) to produce the clock for
	   A/D converter, which should be less than or equal to 4.5MHz.
	   A fully conversion requires (bits_accuracy+1) of these clocks.
	   ADC Clock = PCLK_ADC0 / (CLKDIV + 1);
	   ADC rate = ADC clock / (the number of clocks required for each conversion);
	 */
	adcBlockFreq = Chip_Clock_GetRate(Chip_ADC_GetClockIndex(pADC));
	if (burstMode) {
		fullAdcRate = adcRate * clks;
	}
	else {
		fullAdcRate = adcRate * getFullConvClk();
	}

	/* Get the round value by fomular: (2*A + B)/(2*B) */
	div = ((adcBlockFreq * 2 + fullAdcRate) / (fullAdcRate * 2)) - 1;
	return div;
}

/* Set start mode for ADC */
void setStartMode(LPC_ADC_T *pADC, uint8_t start_mode)
{
	uint32_t temp;
	temp = pADC->CR & (~ADC_CR_START_MASK);
	pADC->CR = temp | (ADC_CR_START_MODE_SEL((uint32_t) start_mode));
}

/* Get the ADC value */
Status readAdcVal(LPC_ADC_T *pADC, uint8_t channel, uint16_t *data)
{
	uint32_t temp;
	temp = pADC->DR[channel];
	if (!ADC_DR_DONE(temp)) {
		return ERROR;
	}
	/*	if(ADC_DR_OVERRUN(temp) && (pADC->CR & ADC_CR_BURST)) */
	/*	return ERROR; */
	*data = (uint16_t) ADC_DR_RESULT(temp);
	return SUCCESS;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize the ADC peripheral and the ADC setup structure to default value */
void Chip_ADC_Init(LPC_ADC_T *pADC, ADC_CLOCK_SETUP_T *ADCSetup)
{
	uint8_t div;
	uint32_t cr = 0;
	uint32_t clk;

	Chip_Clock_EnableOpts(Chip_ADC_GetClockIndex(pADC), true, true, 1);

	pADC->INTEN = 0;		/* Disable all interrupts */

	cr |= ADC_CR_PDN;
	ADCSetup->adcRate = ADC_MAX_SAMPLE_RATE;
	ADCSetup->bitsAccuracy = ADC_10BITS;
	clk = 11;
	ADCSetup->burstMode = false;
	div = getClkDiv(pADC, false, ADCSetup->adcRate, clk);
	cr |= ADC_CR_CLKDIV(div);
	cr |= ADC_CR_BITACC(ADCSetup->bitsAccuracy);
	pADC->CR = cr;
}

/* Shutdown ADC */
void Chip_ADC_DeInit(LPC_ADC_T *pADC)
{
	pADC->INTEN = 0x00000100;
	pADC->CR = 0;
	Chip_Clock_Disable(Chip_ADC_GetClockIndex(pADC));
}

/* Get the ADC value */
Status Chip_ADC_ReadValue(LPC_ADC_T *pADC, uint8_t channel, uint16_t *data)
{
	return readAdcVal(pADC, channel, data);
}

/* Get ADC Channel status from ADC data register */
FlagStatus Chip_ADC_ReadStatus(LPC_ADC_T *pADC, uint8_t channel, uint32_t StatusType)
{
	switch (StatusType) {
	case ADC_DR_DONE_STAT:
		return (pADC->STAT & (1UL << channel)) ? SET : RESET;

	case ADC_DR_OVERRUN_STAT:
		channel += 8;
		return (pADC->STAT & (1UL << channel)) ? SET : RESET;

	case ADC_DR_ADINT_STAT:
		return pADC->STAT >> 16 ? SET : RESET;

	default:
		break;
	}
	return RESET;
}

/* Enable/Disable interrupt for ADC channel */
void Chip_ADC_Int_SetChannelCmd(LPC_ADC_T *pADC, uint8_t channel, FunctionalState NewState)
{
	if (NewState == ENABLE) {
		pADC->INTEN |= (1UL << channel);
	}
	else {
		pADC->INTEN &= (~(1UL << channel));
	}
}

/* Select the mode starting the AD conversion */
void Chip_ADC_SetStartMode(LPC_ADC_T *pADC, ADC_START_MODE_T mode, ADC_EDGE_CFG_T EdgeOption)
{
	if ((mode != ADC_START_NOW) && (mode != ADC_NO_START)) {
		if (EdgeOption) {
			pADC->CR |= ADC_CR_EDGE;
		}
		else {
			pADC->CR &= ~ADC_CR_EDGE;
		}
	}
	setStartMode(pADC, (uint8_t) mode);
}

/* Set the ADC Sample rate */
void Chip_ADC_SetSampleRate(LPC_ADC_T *pADC, ADC_CLOCK_SETUP_T *ADCSetup, uint32_t rate)
{
	uint8_t div;
	uint32_t cr;

	cr = pADC->CR & (~ADC_SAMPLE_RATE_CONFIG_MASK);
	ADCSetup->adcRate = rate;
	div = getClkDiv(pADC, ADCSetup->burstMode, rate, (11 - ADCSetup->bitsAccuracy));
	cr |= ADC_CR_CLKDIV(div);
	cr |= ADC_CR_BITACC(ADCSetup->bitsAccuracy);
	pADC->CR = cr;
}

/* Set the ADC accuracy bits */
void Chip_ADC_SetResolution(LPC_ADC_T *pADC, ADC_CLOCK_SETUP_T *ADCSetup, ADC_RESOLUTION_T resolution)
{
	ADCSetup->bitsAccuracy = resolution;
	Chip_ADC_SetSampleRate(pADC, ADCSetup, ADCSetup->adcRate);
}

/* Enable or disable the ADC channel on ADC peripheral */
void Chip_ADC_EnableChannel(LPC_ADC_T *pADC, ADC_CHANNEL_T channel, FunctionalState NewState)
{
	if (NewState == ENABLE) {
		pADC->CR |= ADC_CR_CH_SEL(channel);
	}
	else {
		pADC->CR &= ~ADC_CR_START_MASK;
		pADC->CR &= ~ADC_CR_CH_SEL(channel);
	}
}

/* Enable burst mode */
void Chip_ADC_SetBurstCmd(LPC_ADC_T *pADC, FunctionalState NewState)
{
	setStartMode(pADC, ADC_NO_START);
	
    if (NewState == DISABLE) {
		pADC->CR &= ~ADC_CR_BURST;
	}
	else {
		pADC->CR |= ADC_CR_BURST;
	}
}

/* Read the ADC value and convert it to 8bits value */
Status Chip_ADC_ReadByte(LPC_ADC_T *pADC, ADC_CHANNEL_T channel, uint8_t *data)
{
	uint16_t temp;
	Status rt;

	rt = readAdcVal(pADC, channel, &temp);
	*data = (uint8_t) temp;

	return rt;
}

