/*
 * @brief LPC43xx SPI driver
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

#if defined(CHIP_LPC43XX)

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Execute callback function */
STATIC void executeCallback(LPC_SPI_T *pSPI, SPI_CALLBACK_T pfunc)
{
	if (pfunc) {
		(pfunc) ();
	}
}

/* Write byte(s) to FIFO buffer */
STATIC void writeData(LPC_SPI_T *pSPI, SPI_DATA_SETUP_T *pXfSetup, uint32_t num_bytes)
{
	uint16_t data2write = 0xFFFF;

	if ( pXfSetup->pTxData) {
		data2write =  pXfSetup->pTxData[pXfSetup->cnt];
		if (num_bytes == 2) {
			data2write |= pXfSetup->pTxData[pXfSetup->cnt + 1] << 8;
		}
	}

	Chip_SPI_SendFrame(pSPI, data2write);

}

/* Read byte(s) from FIFO buffer */
STATIC void readData(LPC_SPI_T *pSPI, SPI_DATA_SETUP_T *pXfSetup, uint16_t rDat, uint32_t num_bytes)
{
	rDat = Chip_SPI_ReceiveFrame(pSPI);
	if (pXfSetup->pRxData) {
		pXfSetup->pRxData[pXfSetup->cnt] = rDat;
		if (num_bytes == 2) {
			pXfSetup->pRxData[pXfSetup->cnt + 1] = rDat >> 8;
		}
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* SPI Polling Read/Write in blocking mode */
uint32_t Chip_SPI_RWFrames_Blocking(LPC_SPI_T *pSPI, SPI_DATA_SETUP_T *pXfSetup)
{
	uint32_t status;
	uint16_t rDat = 0x0000;
	uint8_t bytes = 1;

	/* Clear status */
	Chip_SPI_Int_FlushData(pSPI);

	if (Chip_SPI_GetDataSize(pSPI) != SPI_BITS_8) {
		bytes = 2;
	}

	executeCallback(pSPI, pXfSetup->fnBefTransfer);

	while (pXfSetup->cnt < pXfSetup->length) {

		executeCallback(pSPI, pXfSetup->fnBefFrame);

		/* write data to buffer */
		writeData(pSPI, pXfSetup, bytes);

		/* Wait for transfer completes */
		while (1) {
			status = Chip_SPI_GetStatus(pSPI);
			/* Check error */
			if (status & SPI_SR_ERROR) {
				goto rw_end;
			}
			if (status & SPI_SR_SPIF) {
				break;
			}
		}

		executeCallback(pSPI, pXfSetup->fnAftFrame);

		/* Read data*/
		readData(pSPI, pXfSetup, rDat, bytes);
		pXfSetup->cnt += bytes;
	}

rw_end:
	executeCallback(pSPI, pXfSetup->fnAftTransfer);
	return pXfSetup->cnt;
}

/* Clean all data in RX FIFO of SPI */
void Chip_SPI_Int_FlushData(LPC_SPI_T *pSPI)
{
	volatile uint32_t tmp;
	Chip_SPI_GetStatus(pSPI);
	tmp = Chip_SPI_ReceiveFrame(pSPI);
	Chip_SPI_Int_ClearStatus(pSPI, SPI_INT_SPIF);
}

/* SPI Interrupt Read/Write with 8-bit frame width */
Status Chip_SPI_Int_RWFrames(LPC_SPI_T *pSPI, SPI_DATA_SETUP_T *pXfSetup, uint8_t bytes)
{
	uint32_t status;
	uint16_t rDat = 0x0000;

	status = Chip_SPI_GetStatus(pSPI);
	/* Check error status */
	if (status & SPI_SR_ERROR) {
		return ERROR;
	}

	Chip_SPI_Int_ClearStatus(pSPI, SPI_INT_SPIF);
	if (status & SPI_SR_SPIF) {
		executeCallback(pSPI, pXfSetup->fnAftFrame);
		if (pXfSetup->cnt < pXfSetup->length) {
			/* read data */
			readData(pSPI, pXfSetup, rDat, bytes);
			pXfSetup->cnt += bytes;
		}
	}

	if (pXfSetup->cnt < pXfSetup->length) {

		executeCallback(pSPI, pXfSetup->fnBefFrame);

		/* Write data  */
		writeData(pSPI, pXfSetup, bytes);
	}
	else {
		executeCallback(pSPI, pXfSetup->fnAftTransfer);
	}
	return SUCCESS;
}

/* SPI Interrupt Read/Write with 8-bit frame width */
Status Chip_SPI_Int_RWFrames8Bits(LPC_SPI_T *pSPI, SPI_DATA_SETUP_T *pXfSetup)
{
	return Chip_SPI_Int_RWFrames(pSPI, pXfSetup, 1);
}

/* SPI Interrupt Read/Write with 16-bit frame width */
Status Chip_SPI_Int_RWFrames16Bits(LPC_SPI_T *pSPI, SPI_DATA_SETUP_T *pXfSetup)
{
	return Chip_SPI_Int_RWFrames(pSPI, pXfSetup, 2);
}

/* Set the clock frequency for SPI interface */
void Chip_SPI_SetBitRate(LPC_SPI_T *pSPI, uint32_t bitRate)
{
	uint32_t spiClk, counter;
	/* Get SPI clock rate */
	spiClk = Chip_Clock_GetRate(CLK_SPI);

	counter = spiClk / bitRate;
	if (counter < 8) {
		counter = 8;
	}
	counter = ((counter + 1) / 2) * 2;

	if (counter > 254) {
		counter = 254;
	}

	Chip_SPI_SetClockCounter(pSPI, counter);
}

/* Initialize the SPI */
void Chip_SPI_Init(LPC_SPI_T *pSPI)
{
	Chip_Clock_Enable(CLK_SPI);

	Chip_SPI_SetMode(pSPI, SPI_MODE_MASTER);
	pSPI->CR = (pSPI->CR & (~0xF1C)) | SPI_CR_BIT_EN | SPI_BITS_8 | SPI_CLOCK_CPHA0_CPOL0 | SPI_DATA_MSB_FIRST;
	Chip_SPI_SetBitRate(pSPI, 400000);
}

/* De-initializes the SPI peripheral */
void Chip_SPI_DeInit(LPC_SPI_T *pSPI)
{
	Chip_Clock_Disable(CLK_SPI);
}

#endif
