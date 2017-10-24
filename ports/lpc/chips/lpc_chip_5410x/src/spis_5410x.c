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

/* Determine SSEL associated with the current data value */
static uint8_t Chip_SPIS_FindSSEL(LPC_SPI_T *pSPI, uint32_t data)
{
	int i;
	uint8_t ssel = 0;

	for (i = 0; i <= 3; i++) {
		if ((data & SPI_RXDAT_RXSSELNUM_INACTIVE(i)) == 0) {
			/* Signal is active on low */
			ssel = (uint8_t) i;
		}
	}

	return ssel;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* SPI slave transfer state change handler */
uint32_t Chip_SPIS_XferHandler(LPC_SPI_T *pSPI, SPIS_XFER_T *xfer)
{
	uint32_t staterr, data;
	uint8_t flen;

	/* Get length of a receive value */
	flen = (pSPI->TXCTRL >> 24) & 0xF;

	/* Data received? */
	while ((Chip_SPI_GetStatus(pSPI) & SPI_STAT_RXRDY) != 0) {
		/* Get raw data and status */
		data = Chip_SPI_ReadRXData(pSPI);
		if (xfer->rxCount > xfer->rxDoneCount) {
			if (flen > 8) {
				((uint16_t *) xfer->rxBuff)[xfer->rxDoneCount] = data;
			}
			else {
				((uint8_t *) xfer->rxBuff)[xfer->rxDoneCount] = data;
			}
		}
		xfer->rxDoneCount++;
		if (xfer->cbFunc && (xfer->rxCount == xfer->rxDoneCount)) {
			xfer->cbFunc(SPIS_EVT_RXDONE, xfer);
		}
	}

	/* Get errors for later, we'll continue even if errors occur, but we notify
	   caller on return */
	staterr = Chip_SPI_GetStatus(pSPI) & (SPI_STAT_RXOV | SPI_STAT_TXUR);
	if (staterr != 0) {
		Chip_SPI_ClearStatus(pSPI, staterr);
	}

	/* Slave assertion */
	if ((Chip_SPI_GetStatus(pSPI) & SPI_STAT_SSA) != 0) {
		Chip_SPI_ClearStatus(pSPI, SPI_STAT_SSA);

		/* Determine SPI select. Read the data FIFO to get the slave number. Data
		   should not be in the receive FIFO yet, only the statuses */
		xfer->sselNum = Chip_SPIS_FindSSEL(pSPI, Chip_SPI_ReadRawRXFifo(pSPI));

		xfer->state = SPIS_XFER_STATE_BUSY;
		/* SSEL assertion callback */
		if (xfer->cbFunc) {
			xfer->cbFunc(SPIS_EVT_SSELASSERT, xfer);
		}
	}

	/* Transmit data? */
	while ((Chip_SPI_GetStatus(pSPI) & SPI_STAT_TXRDY) != 0) {
		data = 0;
		if (xfer->txCount > xfer->txDoneCount) {
			data = flen > 8 ?
				   (uint32_t) ((uint16_t *) xfer->txBuff)[xfer->txDoneCount] :
				   (uint32_t) ((uint8_t *) xfer->txBuff)[xfer->txDoneCount];
		}
		Chip_SPI_WriteTXData(pSPI, data);
		xfer->txDoneCount++;
		if (xfer->cbFunc && (xfer->txCount == xfer->txDoneCount)) {
			xfer->cbFunc(SPIS_EVT_TXDONE, xfer);
		}
	}

	/* Slave de-assertion */
	if (((Chip_SPI_GetStatus(pSPI) & SPI_STAT_SSD) != 0) && ((Chip_SPI_GetStatus(pSPI) & SPI_STAT_RXRDY) == 0)) {
		Chip_SPI_ClearStatus(pSPI, SPI_STAT_SSD);
		xfer->state = SPIS_XFER_STATE_DONE;
		/* SSEL assertion callback */
		if (xfer->cbFunc) {
			xfer->cbFunc(SPIS_EVT_SSELDEASSERT, xfer);
		}
	}

	return staterr;
}

/* SPI slave transfer blocking function */
uint32_t Chip_SPIS_XferBlocking(LPC_SPI_T *pSPI, SPIS_XFER_T *xfer)
{
	uint32_t status = 0;

	/* Wait forever until deassertion event */
	while (xfer->state != SPIS_XFER_STATE_DONE) {
		status = Chip_SPIS_XferHandler(pSPI, xfer);
	}

	return status;
}
