/*
 * @brief LPC5410X System FIFO chip driver
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

/** SPI FIFO read FIFO statuses */
#define LPC_SPIRXFIFO_STAT_SSEL0N       (1 << 16)		/*!< Slave select for receive on SSEL0 (active low) */
#define LPC_SPIRXFIFO_STAT_SSEL1N       (1 << 17)		/*!< Slave select for receive on SSEL1 (active low) */
#define LPC_SPIRXFIFO_STAT_SSEL2N       (1 << 18)		/*!< Slave select for receive on SSEL2 (active low) */
#define LPC_SPIRXFIFO_STAT_SSEL3N       (1 << 19)		/*!< Slave select for receive on SSEL3 (active low) */
#define LPC_SPIRXFIFO_STAT_SOT          (1 << 20)		/*!< This flag will be 1 if this is the first data after the SSELs went from deasserted to asserted */

/** SPI FIFO write FIFO control */
#define LPC_SPITXFIFO_CTRL_SSEL0N       (1 << 16)		/*!< Master assert for receive on SSEL0 (active low) */
#define LPC_SPITXFIFO_CTRL_SSEL1N       (1 << 17)		/*!< Master assert for receive on SSEL1 (active low) */
#define LPC_SPITXFIFO_CTRL_SSEL2N       (1 << 18)		/*!< Master assert for receive on SSEL2 (active low) */
#define LPC_SPITXFIFO_CTRL_SSEL3N       (1 << 19)		/*!< Master assert for receive on SSEL3 (active low) */
#define LPC_SPITXFIFO_CTRL_EOT          (1 << 20)		/*!< End of Transfer. The asserted SSEL will be deasserted at the end of a transfer */
#define LPC_SPITXFIFO_CTRL_EOF          (1 << 21)		/*!< End of Frame. Between frames, a delay may be inserted, as defined by the FRAME_DELAY value in the DLY register */
#define LPC_SPITXFIFO_CTRL_RXIGNORE     (1 << 22)		/*!< Receive Ignore. This allows data to be transmitted using the SPI without the need to read unneeded data from the receiver */
#define LPC_SPITXFIFO_CTRL_LEN(n)       ((n) << 24)		/*!< Data Length. Specifies the data length from 1 to 16 bits ((n-1) encoded) */

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initializes the system FIFO */
void Chip_FIFO_Init(LPC_FIFO_T *pFIFO)
{
	Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_FIFO);
	Chip_SYSCON_PeriphReset(RESET_FIFO);
}

/* Deinitializes the system FIFO */
void Chip_FIFO_Deinit(LPC_FIFO_T *pFIFO)
{
	Chip_Clock_DisablePeriphClock(SYSCON_CLOCK_FIFO);
}

/* Get the FIFO space available for the USART/SPI direction */
uint32_t Chip_FIFO_GetFifoSpace(LPC_FIFO_T *pFIFO, LPC_FIFO_PERIPHID_T periphId, LPC_FIFO_DIR_T dir)
{
	uint32_t pcfg;

	if (periphId == FIFO_USART) {
		pcfg = pFIFO->common.FIFOCTLUSART;
	}
	else {
		pcfg = pFIFO->common.FIFOCTLSPI;
	}

	if (dir == FIFO_RX) {
		pcfg = pcfg >> 16;
	}
	else {
		pcfg = pcfg >> 24;
	}

	return pcfg & 0xFF;
}

/* Pause a peripheral FIFO */
void Chip_FIFO_PauseFifo(LPC_FIFO_T *pFIFO, LPC_FIFO_PERIPHID_T periphId, LPC_FIFO_DIR_T dir)
{
	if (periphId == FIFO_USART) {
		if (dir == FIFO_RX) {
			pFIFO->common.FIFOCTLUSART |= (1 << 0);
		}
		else {
			pFIFO->common.FIFOCTLUSART |= (1 << 8);
		}
	}
	else {
		if (dir == FIFO_RX) {
			pFIFO->common.FIFOCTLSPI |= (1 << 0);
		}
		else {
			pFIFO->common.FIFOCTLSPI |= (1 << 8);
		}
	}
}

/* Unpause a peripheral FIFO */
void Chip_FIFO_UnpauseFifo(LPC_FIFO_T *pFIFO, LPC_FIFO_PERIPHID_T periphId, LPC_FIFO_DIR_T dir)
{
	if (periphId == FIFO_USART) {
		if (dir == FIFO_RX) {
			pFIFO->common.FIFOCTLUSART &= ~(1 << 0);
		}
		else {
			pFIFO->common.FIFOCTLUSART &= ~(1 << 8);
		}
	}
	else {
		if (dir == FIFO_RX) {
			pFIFO->common.FIFOCTLSPI &= ~(1 << 0);
		}
		else {
			pFIFO->common.FIFOCTLSPI &= ~(1 << 8);
		}
	}
}

/* Configure a peripheral's FIFO sizes */
void Chip_FIFO_ConfigFifoSize(LPC_FIFO_T *pFIFO, LPC_FIFO_PERIPHID_T periphId, LPC_FIFO_CFGSIZE_T *pSizes)
{
	int maxP, i;
	uint32_t upDateMask;
	volatile uint32_t *updateReg, *pFifoSizes, *pFifoPause;

	/* Pause FIFOs */
	Chip_FIFO_PauseFifo(LPC_FIFO, periphId, FIFO_RX);
	Chip_FIFO_PauseFifo(LPC_FIFO, periphId, FIFO_TX);

	/* Maximum peripheral FIFOs supported */
	if (periphId == FIFO_USART) {
		maxP = LPC_FIFO_USART_MAX;
		updateReg = &pFIFO->common.FIFOUPDATEUSART;
		upDateMask = 0xF | (0xF << 16);
		pFifoSizes = &pFIFO->common.FIFOCFGUSART[0];
		pFifoPause = &pFIFO->common.FIFOCTLUSART;
	}
	else {
		maxP = LPC_FIFO_SPI_MAX;
		updateReg = &pFIFO->common.FIFOUPDATESPI;
		upDateMask = 0x3 | (0x3 << 16);
		pFifoSizes = &pFIFO->common.FIFOCFGSPI[0];
		pFifoPause = &pFIFO->common.FIFOCTLSPI;
	}

	/* Wait for FIFO pause */
	while ((*pFifoPause & ((1 << 0) | (1 << 8))) != ((1 << 0) | (1 << 8))) {}

	/* Update FIFO sizes */
	for (i = 0; i < maxP; i++) {
		pFifoSizes[i] = ((uint32_t) (pSizes->fifoRXSize[i]) << 0) |
						((uint32_t) (pSizes->fifoTXSize[i]) << 8);
	}

	/* Update all peripheral FIFO sizes */
	*updateReg = upDateMask;
}

/* Configure the USART system FIFO */
void Chip_FIFOUSART_Configure(LPC_FIFO_T *pFIFO, int usartIndex, LPC_FIFO_CFG_T *pUSARTCfg)
{
	pFIFO->usart[usartIndex].CFG =
		(pUSARTCfg->noTimeoutContWrite << 4) |
		(pUSARTCfg->noTimeoutContEmpty << 5) |
		(pUSARTCfg->timeoutBase << 8) |
		(pUSARTCfg->timeoutValue << 12) |
		(pUSARTCfg->rxThreshold << 16) |
		(pUSARTCfg->txThreshold << 24);
}

/* Write data to a system FIFO (non-blocking) */
int Chip_FIFOUSART_WriteTX(LPC_FIFO_T *pFIFO, int usartIndex, bool sz8, void *buff, int numData)
{
	int datumWritten, sz16;
	uint8_t *p8 = (uint8_t *) buff;
	uint16_t *p16 = (uint16_t *) buff;

	/* Get configured FIFO size to determine write size, limit to buffer size */
	sz16 = (pFIFO->usart[usartIndex].STAT >> 24) & 0xFF;
	if (sz16 > numData) {
		sz16 = numData;
	}
	datumWritten = sz16;

	/* Write from buffer */
	while (sz16 > 0) {
		if (sz8) {
			pFIFO->usart[usartIndex].TXDAT = (uint32_t) *p8;
			p8++;
		}
		else {
			pFIFO->usart[usartIndex].TXDAT = (uint32_t) *p16;
			p16++;
		}

		sz16--;
	}

	return datumWritten;
}

/* Read data from a system FIFO (non-blocking) */
int Chip_FIFOUSART_ReadRX(LPC_FIFO_T *pFIFO, int usartIndex, bool sz8, void *buff, int numData)
{
	int datumRead, sz16;
	uint8_t *p8 = (uint8_t *) buff;
	uint16_t *p16 = (uint16_t *) buff;

	/* Get configured FIFO size to determine read size, limit to buffer size */
	sz16 = (pFIFO->usart[usartIndex].STAT >> 16) & 0xFF;
	if (sz16 > numData) {
		sz16 = numData;
	}
	datumRead = sz16;

	/* Read into buffer */
	while (sz16 > 0) {
		if (sz8) {
			*p8 = (uint8_t) (pFIFO->usart[usartIndex].RXDAT & 0xFF);
			p8++;
		}
		else {
			*p16 = (uint16_t) (pFIFO->usart[usartIndex].RXDAT & 0x1FF);
			p16++;
		}

		sz16--;
	}

	return datumRead;
}

/* Read data from a system FIFO with status (non-blocking) */
int Chip_FIFOUSART_ReadRXStatus(LPC_FIFO_T *pFIFO, int usartIndex, uint16_t *buff, int numData)
{
	int datumRead, sz16;
	uint16_t *p16 = (uint16_t *) buff;

	/* Get configured FIFO size to determine read size, limit to buffer size */
	sz16 = (pFIFO->usart[usartIndex].STAT >> 16) & 0xFF;
	if (sz16 > numData) {
		sz16 = numData;
	}
	datumRead = sz16;

	/* Read into buffer */
	while (sz16 > 0) {
		*p16 = (uint16_t) (pFIFO->usart[usartIndex].RXDATSTAT & 0xFFFF);
		p16++;
		sz16--;
	}

	return datumRead;
}

#if 0	/* Sorry, not yet support */
/* Configure the USART system FIFO */
void Chip_FIFOSPI_Configure(LPC_FIFO_T *pFIFO, int spiIndex, LPC_FIFO_CFG_T *pSPICfg)
{
	pFIFO->spi[spiIndex].CFG =
		(pSPICfg->noTimeoutContWrite << 4) |
		(pSPICfg->noTimeoutContEmpty << 5) |
		(pSPICfg->timeoutBase << 6) |
		(pSPICfg->timeoutValue << 12) |
		(pSPICfg->rxThreshold << 16) |
		(pSPICfg->txThreshold << 24);
}

/* Start a data transfer (non-blocking) */
void Chip_FIFOSPI_StartTransfer(LPC_FIFO_T *pFIFO, LPC_FIFO_SPICTL_T *pSetupData)
{
	pSetupData->start = 1;
	Chip_FIFOSPI_Transfer(pFIFO, pSetupData);
}

/* Feed a SPI data transfer (non-blocking) */
void Chip_FIFOSPI_Transfer(LPC_FIFO_T *pFIFO, LPC_FIFO_SPICTL_T *pSetupData)
{
	// FIXME - not yet ready
}

#endif
