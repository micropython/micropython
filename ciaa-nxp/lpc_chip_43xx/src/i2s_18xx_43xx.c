/*
 * @brief LPC18xx/43xx I2S driver
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

/* Get divider value */
STATIC Status getClkDiv(LPC_I2S_T *pI2S, I2S_AUDIO_FORMAT_T *format, uint16_t *pxDiv, uint16_t *pyDiv, uint32_t *pN)
{
	uint32_t pClk;
	uint32_t x, y;
	uint64_t divider;
	uint16_t dif;
	uint16_t xDiv = 0, yDiv = 0;
	uint32_t N;
	uint16_t err, ErrorOptimal = 0xFFFF;

	pClk = Chip_Clock_GetRate(CLK_APB1_I2S);

	/* divider is a fixed point number with 16 fractional bits */
	divider = (((uint64_t) (format->SampleRate) * 2 * (format->WordWidth) * 2) << 16) / pClk;
	/* find N that make x/y <= 1 -> divider <= 2^16 */
	for (N = 64; N > 0; N--) {
		if ((divider * N) < (1 << 16)) {
			break;
		}
	}
	if (N == 0) {
		return ERROR;
	}
	divider *= N;
	for (y = 255; y > 0; y--) {
		x = y * divider;
		if (x & (0xFF000000)) {
			continue;
		}
		dif = x & 0xFFFF;
		if (dif > 0x8000) {
			err = 0x10000 - dif;
		}
		else {
			err = dif;
		}
		if (err == 0) {
			yDiv = y;
			break;
		}
		else if (err < ErrorOptimal) {
			ErrorOptimal = err;
			yDiv = y;
		}
	}
	xDiv = ((uint64_t) yDiv * (format->SampleRate) * 2 * (format->WordWidth) * N * 2) / pClk;
	if (xDiv >= 256) {
		xDiv = 0xFF;
	}
	if (xDiv == 0) {
		xDiv = 1;
	}

	*pxDiv = xDiv;
	*pyDiv = yDiv;
	*pN = N;
	return SUCCESS;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize the I2S interface */
void Chip_I2S_Init(LPC_I2S_T *pI2S)
{
	Chip_Clock_Enable(CLK_APB1_I2S);
}

/* Shutdown I2S */
void Chip_I2S_DeInit(LPC_I2S_T *pI2S)
{
	pI2S->DAI = 0x07E1;
	pI2S->DAO = 0x87E1;
	pI2S->IRQ = 0;
	pI2S->TXMODE = 0;
	pI2S->RXMODE = 0;
	pI2S->DMA[I2S_DMA_REQUEST_CHANNEL_1] = 0;
	pI2S->DMA[I2S_DMA_REQUEST_CHANNEL_2] = 0;
	Chip_Clock_Disable(CLK_APB1_I2S);
}

/* Configure I2S for Audio Format input */
Status Chip_I2S_TxConfig(LPC_I2S_T *pI2S, I2S_AUDIO_FORMAT_T *format)
{
	uint32_t temp;
	uint16_t xDiv, yDiv;
	uint32_t N;

	if (getClkDiv(pI2S, format, &xDiv, &yDiv, &N) == ERROR) {
		return ERROR;
	}

	temp = pI2S->DAO & (~(I2S_DAO_WORDWIDTH_MASK | I2S_DAO_MONO | I2S_DAO_SLAVE | I2S_DAO_WS_HALFPERIOD_MASK));
	if (format->WordWidth <= 8) {
		temp |= I2S_WORDWIDTH_8;
	}
	else if (format->WordWidth <= 16) {
		temp |= I2S_WORDWIDTH_16;
	}
	else {
		temp |= I2S_WORDWIDTH_32;
	}

	temp |= (format->ChannelNumber) == 1 ? I2S_MONO : I2S_STEREO;
	temp |= I2S_MASTER_MODE;
	temp |= I2S_DAO_WS_HALFPERIOD(format->WordWidth - 1);
	pI2S->DAO = temp;
	pI2S->TXMODE = I2S_TXMODE_CLKSEL(0);
	pI2S->TXBITRATE = N - 1;
	pI2S->TXRATE = yDiv | (xDiv << 8);
	return SUCCESS;
}

/* Configure I2S for Audio Format input */
Status Chip_I2S_RxConfig(LPC_I2S_T *pI2S, I2S_AUDIO_FORMAT_T *format)
{
	uint32_t temp;
	uint16_t xDiv, yDiv;
	uint32_t N;

	if (getClkDiv(pI2S, format, &xDiv, &yDiv, &N) == ERROR) {
		return ERROR;
	}
	temp = pI2S->DAI & (~(I2S_DAI_WORDWIDTH_MASK | I2S_DAI_MONO | I2S_DAI_SLAVE | I2S_DAI_WS_HALFPERIOD_MASK));
	if (format->WordWidth <= 8) {
		temp |= I2S_WORDWIDTH_8;
	}
	else if (format->WordWidth <= 16) {
		temp |= I2S_WORDWIDTH_16;
	}
	else {
		temp |= I2S_WORDWIDTH_32;
	}

	temp |= (format->ChannelNumber) == 1 ? I2S_MONO : I2S_STEREO;
	temp |= I2S_MASTER_MODE;
	temp |= I2S_DAI_WS_HALFPERIOD(format->WordWidth - 1);
	pI2S->DAI = temp;
	pI2S->RXMODE = I2S_RXMODE_CLKSEL(0);
	pI2S->RXBITRATE = N - 1;
	pI2S->RXRATE = yDiv | (xDiv << 8);
	return SUCCESS;
}

/* Enable/Disable Interrupt with a specific FIFO depth */
void Chip_I2S_Int_TxCmd(LPC_I2S_T *pI2S, FunctionalState newState, uint8_t depth)
{
	uint32_t temp;
	depth &= 0x0F;
	if (newState == ENABLE) {
	    temp = pI2S->IRQ & (~I2S_IRQ_TX_DEPTH_MASK);
	    pI2S->IRQ = temp | (I2S_IRQ_TX_DEPTH(depth));
		pI2S->IRQ |= 0x02;
	}
	else {
		pI2S->IRQ &= (~0x02);
	}
}

/* Enable/Disable Interrupt with a specific FIFO depth */
void Chip_I2S_Int_RxCmd(LPC_I2S_T *pI2S, FunctionalState newState, uint8_t depth)
{
	uint32_t temp;
	depth &= 0x0F;
	if (newState == ENABLE) {
	    temp = pI2S->IRQ & (~I2S_IRQ_RX_DEPTH_MASK);
	    pI2S->IRQ = temp | (I2S_IRQ_RX_DEPTH(depth));
		pI2S->IRQ |= 0x01;
	}
	else {
		pI2S->IRQ &= (~0x01);
	}
}

/* Enable/Disable DMA with a specific FIFO depth */
void Chip_I2S_DMA_TxCmd(LPC_I2S_T *pI2S,
						I2S_DMA_CHANNEL_T dmaNum,
						FunctionalState newState,
						uint8_t depth)
{
	/* Enable/Disable I2S transmit*/
	if (newState == ENABLE) {
	    /* Set FIFO Level */
	    pI2S->DMA[I2S_DMA_REQUEST_CHANNEL_1] &= ~(0x0F << 16);
	    pI2S->DMA[I2S_DMA_REQUEST_CHANNEL_1] |= depth << 16;
		pI2S->DMA[I2S_DMA_REQUEST_CHANNEL_1] |= 0x02;
	}
	else {
		pI2S->DMA[I2S_DMA_REQUEST_CHANNEL_1] &= ~0x02;
	}
}

/* Enable/Disable DMA with a specific FIFO depth */
void Chip_I2S_DMA_RxCmd(LPC_I2S_T *pI2S,
						I2S_DMA_CHANNEL_T dmaNum,
						FunctionalState newState,
						uint8_t depth)
{
	
	/* Enable/Disable I2S Receive */
	if (newState == ENABLE) {
	    /* Set FIFO Level */
	    pI2S->DMA[I2S_DMA_REQUEST_CHANNEL_2] &= ~(0x0F << 8);
	    pI2S->DMA[I2S_DMA_REQUEST_CHANNEL_2] |= depth << 8;
		pI2S->DMA[I2S_DMA_REQUEST_CHANNEL_2] |= 0x01;
	}
	else {
		pI2S->DMA[I2S_DMA_REQUEST_CHANNEL_2] &= ~0x01;
	}
}

