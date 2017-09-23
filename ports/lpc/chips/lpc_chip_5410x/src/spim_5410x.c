/*
 * @brief LPC5410X SPI master driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2015
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licenser disclaim any and
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

/* Get SPI master bit rate */
uint32_t Chip_SPIM_GetClockRate(LPC_SPI_T *pSPI)
{
	return Chip_Clock_GetAsyncSyscon_ClockRate() / (pSPI->DIV + 1);
}

/* Set SPI master bit rate */
uint32_t Chip_SPIM_SetClockRate(LPC_SPI_T *pSPI, uint32_t rate)
{
	uint32_t baseClock, div;

	/* Get peripheral base clock rate */
	baseClock = Chip_Clock_GetAsyncSyscon_ClockRate();

	/* Compute divider */
	div = baseClock / rate;

	/* Limit values */
	if (div == 0) {
		div = 1;
	}
	else if (div > 0x10000) {
		div = 0x10000;
	}
	pSPI->DIV = div - 1;

	return Chip_SPIM_GetClockRate(pSPI);
}

/* Handler SPI transfer RX */
__STATIC_INLINE int Chip_SPIM_HandlerRx(LPC_SPI_T *pSPI, SPIM_XFER_T *xfer)
{
	int ret = 0;
	int flen = (xfer->options >> 8) & 0xF;
	uint32_t data = Chip_SPI_ReadRawRXFifo(pSPI);

	if (xfer->rxCount > xfer->rxDoneCount) {
		if (flen > 8) {
			((uint16_t *) xfer->rxBuff)[xfer->rxDoneCount] = data;
		}
		else {
			((uint8_t *) xfer->rxBuff)[xfer->rxDoneCount] = data;
		}
		ret = 1;
	}
	xfer->rxDoneCount++;
	if ((xfer->rxCount == xfer->rxDoneCount) && xfer->cbFunc) {
		xfer->cbFunc(SPIM_EVT_RXDONE, xfer);
	}
	return ret;
}

/* SPI master transfer state change handler */
void Chip_SPIM_XferHandler(LPC_SPI_T *pSPI, SPIM_XFER_T *xfer)
{
	uint32_t data;
	uint8_t flen;

	/* Get length of a receive value */
	flen = (pSPI->TXCTRL >> 24) & 0xF;

	/* Master asserts slave */
	if ((Chip_SPI_GetStatus(pSPI) & SPI_STAT_SSA) != 0) {
		Chip_SPI_ClearStatus(pSPI, SPI_STAT_SSA);

		xfer->state = SPIM_XFER_STATE_BUSY;
		/* SSEL assertion callback */
		if (xfer->cbFunc) {
			xfer->cbFunc(SPIM_EVT_SSELASSERT, xfer);
		}
	}

	/* Slave de-assertion */
	if ((Chip_SPI_GetStatus(pSPI) & SPI_STAT_SSD) != 0) {
		Chip_SPI_ClearStatus(pSPI, SPI_STAT_SSD);

		xfer->state = SPIM_XFER_STATE_DONE;
		/* SSEL assertion callback */
		if (xfer->cbFunc) {
			xfer->cbFunc(SPIM_EVT_SSELDEASSERT, xfer);
		}
	}

	/* Data received? */
	while ((Chip_SPI_GetStatus(pSPI) & SPI_STAT_RXRDY) != 0 && Chip_SPIM_HandlerRx(pSPI, xfer)) {}

	/* Transmit data? */
	while (((Chip_SPI_GetStatus(pSPI) & SPI_STAT_TXRDY) != 0)) {
		int32_t len = xfer->rxCount > xfer->txCount ? xfer->rxCount : xfer->txCount;
		if ((xfer->rxCount <= xfer->rxDoneCount) && (xfer->txCount <= xfer->txDoneCount)) {
			xfer->state = SPIM_XFER_STATE_DONE;
			/* Transfer is done, this will be last data */
			Chip_SPIM_ForceEndOfTransfer(pSPI);
			return;
		}
		data = 0;
		if (xfer->txCount > xfer->txDoneCount) {
			if (flen > 8) {
				data = ((uint16_t *) xfer->txBuff)[xfer->txDoneCount];
			}
			else {
				data = ((uint8_t *) xfer->txBuff)[xfer->txDoneCount];
			}
		}

		/* Check for end of transfer and end the transfer if needed */
		if ((len == (xfer->txDoneCount + 1)) && (xfer->options & SPIM_XFER_OPTION_EOT)) {
			pSPI->TXDATCTL = pSPI->TXCTRL | SPI_TXDATCTL_EOT | data;
		}
		else {
			Chip_SPI_WriteTXData(pSPI, data);
		}

		xfer->txDoneCount++;
		if ((xfer->txCount == xfer->txDoneCount) && xfer->cbFunc) {
			xfer->cbFunc(SPIM_EVT_TXDONE, xfer);
		}

		/* Check if we have a data ready to receive */
		if (Chip_SPI_GetStatus(pSPI) & SPI_STAT_RXRDY) {
			Chip_SPIM_HandlerRx(pSPI, xfer);
		}
	}
}

/* Start non-blocking SPI master transfer */
void Chip_SPIM_Xfer(LPC_SPI_T *pSPI, SPIM_XFER_T *xfer)
{
	/* Setup SPI master select, data length, EOT/EOF timing, and RX data ignore */
	pSPI->TXCTRL =
		((xfer->options <<
		  16) | SPI_TXDATCTL_DEASSERT_ALL | (xfer->rxBuff ? 0 : SPI_TXDATCTL_RXIGNORE)) & ~SPI_TXDATCTL_EOT;
	Chip_SPIM_AssertSSEL(pSPI, xfer->sselNum);

	/* Clear initial transfer states */
	xfer->txDoneCount = xfer->rxDoneCount = 0;

	/* Call main handler to start transfer */
	Chip_SPIM_XferHandler(pSPI, xfer);
}

/* Perform blocking SPI master transfer */
void Chip_SPIM_XferBlocking(LPC_SPI_T *pSPI, SPIM_XFER_T *xfer)
{
	/* Start trasnfer */
	Chip_SPIM_Xfer(pSPI, xfer);

	/* Wait for termination */
	while (xfer->state != SPIM_XFER_STATE_DONE) {
		Chip_SPIM_XferHandler(pSPI, xfer);
	}
}
