/*
 * @brief LPC5410x I2C master driver
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

/* Sets HIGH and LOW duty cycle registers */
void Chip_I2CM_SetDutyCycle(LPC_I2C_T *pI2C, uint16_t sclH, uint16_t sclL)
{
	/* Limit to usable range of timing values */
	if (sclH < 2) {
		sclH = 2;
	}
	else if (sclH > 9) {
		sclH = 9;
	}
	if (sclL < 2) {
		sclL = 2;
	}
	else if (sclL > 9) {
		sclL = 9;
	}

	pI2C->MSTTIME = (((sclH - 2) & 0x07) << 4) | ((sclL - 2) & 0x07);
}

/* Set up bus speed for LPC_I2C interface */
void Chip_I2CM_SetBusSpeed(LPC_I2C_T *pI2C, uint32_t busSpeed)
{
	uint32_t scl;

	scl = Chip_Clock_GetAsyncSyscon_ClockRate() / (Chip_I2C_GetClockDiv(pI2C) * busSpeed);
	Chip_I2CM_SetDutyCycle(pI2C, (scl >> 1), (scl - (scl >> 1)));
}

/* Master transfer state change handler handler */
uint32_t Chip_I2CM_XferHandler(LPC_I2C_T *pI2C, I2CM_XFER_T *xfer)
{
	uint32_t status = Chip_I2CM_GetStatus(pI2C);

	if (status & I2C_STAT_MSTRARBLOSS) {
		/* Master Lost Arbitration */
		/* Set transfer status as Arbitration Lost */
		xfer->status = I2CM_STATUS_ARBLOST;
		/* Clear Status Flags */
		Chip_I2CM_ClearStatus(pI2C, I2C_STAT_MSTRARBLOSS);
		/* Master continue */
		if (status & I2C_STAT_MSTPENDING) {
			pI2C->MSTCTL = I2C_MSTCTL_MSTCONTINUE;
		}
	}
	else if (status & I2C_STAT_MSTSTSTPERR) {
		/* Master Start Stop Error */
		/* Set transfer status as Bus Error */
		xfer->status = I2CM_STATUS_BUS_ERROR;
		/* Clear Status Flags */
		Chip_I2CM_ClearStatus(pI2C, I2C_STAT_MSTSTSTPERR);

		/* Master continue */
		if (status & I2C_STAT_MSTPENDING) {
			pI2C->MSTCTL = I2C_MSTCTL_MSTCONTINUE;
		}
	}
	else if (status & I2C_STAT_MSTPENDING) {
		/* Master is Pending */
		/* Branch based on Master State Code */
		switch (Chip_I2CM_GetMasterState(pI2C)) {
		case I2C_STAT_MSTCODE_IDLE:	/* Master idle */
			/* Can transition to idle between transmit and receive states */
			if (xfer->txSz) {
				/* Start transmit state */
				Chip_I2CM_WriteByte(pI2C, (xfer->slaveAddr << 1));
				pI2C->MSTCTL = I2C_MSTCTL_MSTSTART;
			}
			else if (xfer->rxSz) {
				/* Start receive state with start ot repeat start */
				Chip_I2CM_WriteByte(pI2C, (xfer->slaveAddr << 1) | 0x1);
				pI2C->MSTCTL = I2C_MSTCTL_MSTSTART;
			}
			else {
				/* No data to send, done */
				xfer->status = I2CM_STATUS_OK;
			}
			break;

		case I2C_STAT_MSTCODE_RXREADY:	/* Receive data is available */
			/* Read Data up until the buffer size */
			if (xfer->rxSz) {
				*xfer->rxBuff = pI2C->MSTDAT;
				xfer->rxBuff++;
				xfer->rxSz--;
			}

			if (xfer->rxSz) {
				pI2C->MSTCTL = I2C_MSTCTL_MSTCONTINUE;
			}
			else {
				/* Last byte to receive, send stop after byte received */
				pI2C->MSTCTL = I2C_MSTCTL_MSTCONTINUE | I2C_MSTCTL_MSTSTOP;
			}
			break;

		case I2C_STAT_MSTCODE_TXREADY:	/* Master Transmit available */
			if (xfer->txSz) {
				/* If Tx data available transmit data and continue */
				pI2C->MSTDAT = (uint32_t) *xfer->txBuff;
				pI2C->MSTCTL = I2C_MSTCTL_MSTCONTINUE;
				xfer->txBuff++;
				xfer->txSz--;
			}
			else if (xfer->rxSz == 0) {
				pI2C->MSTCTL = I2C_MSTCTL_MSTSTOP;
			}
			else {
				/* Start receive state with start ot repeat start */
				Chip_I2CM_WriteByte(pI2C, (xfer->slaveAddr << 1) | 0x1);
				pI2C->MSTCTL = I2C_MSTCTL_MSTSTART;
			}
			break;

		case I2C_STAT_MSTCODE_NACKADR:	/* Slave address was NACK'ed */
			/* Set transfer status as NACK on address */
			xfer->status = I2CM_STATUS_NAK_ADR;
			pI2C->MSTCTL = I2C_MSTCTL_MSTSTOP;
			break;

		case I2C_STAT_MSTCODE_NACKDAT:	/* Slave data was NACK'ed */
			/* Set transfer status as NACK on data */
			xfer->status = I2CM_STATUS_NAK_DAT;
			pI2C->MSTCTL = I2C_MSTCTL_MSTSTOP;
			break;

		default:
			/* Illegal I2C master state machine case. This should never happen.
			     Try to advance state machine by continuing. */
			xfer->status = I2CM_STATUS_ERROR;
			pI2C->MSTCTL = I2C_MSTCTL_MSTCONTINUE;
			break;
		}
	}
	else {
		/* Unsupported operation. This may be a call to the master handler
		     for a wrong interrupt type. This handler should only be called when a
		     master arbitration loss, master start/stop error, or master pending status
		     occurs. */
		xfer->status = I2CM_STATUS_ERROR;
	}

	return xfer->status != I2CM_STATUS_BUSY;
}

/* Transmit and Receive data in master mode */
void Chip_I2CM_Xfer(LPC_I2C_T *pI2C, I2CM_XFER_T *xfer)
{
	/* set the transfer status as busy */
	xfer->status = I2CM_STATUS_BUSY;

	/* Reset master state machine */
	Chip_I2CM_Disable(pI2C);
	Chip_I2CM_Enable(pI2C);

	/* Clear controller state. */
	Chip_I2CM_ClearStatus(pI2C, I2C_STAT_MSTRARBLOSS | I2C_STAT_MSTSTSTPERR);

	/* Handle transfer via initial call to handler */
	Chip_I2CM_XferHandler(pI2C, xfer);
}

/* Transmit and Receive data in master mode */
uint32_t Chip_I2CM_XferBlocking(LPC_I2C_T *pI2C, I2CM_XFER_T *xfer)
{
	/* start transfer */
	Chip_I2CM_Xfer(pI2C, xfer);

	while (xfer->status == I2CM_STATUS_BUSY) {
		/* wait for status change interrupt */
		while (!Chip_I2CM_IsMasterPending(pI2C)) {}
		/* call state change handler */
		Chip_I2CM_XferHandler(pI2C, xfer);
	}

	return xfer->status == I2CM_STATUS_OK;
}
