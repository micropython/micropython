/*
 * @brief LPC5410x I2C slave driver
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

/* Slave transfer state change handler */
uint32_t Chip_I2CS_XferHandler(LPC_I2C_T *pI2C, const I2CS_XFER_T *xfers)
{
	uint32_t done = 0, xferDone = 0;

	uint8_t data;
	uint32_t state;

	/* Transfer complete? */
	if ((Chip_I2C_GetPendingInt(pI2C) & I2C_INTENSET_SLVDESEL) != 0) {
		Chip_I2CS_ClearStatus(pI2C, I2C_STAT_SLVDESEL);
		xfers->slaveDone();
		xferDone = 1;
	}
	else {
		/* Determine the current I2C slave state */
		state = Chip_I2CS_GetSlaveState(pI2C);

		switch (state) {
		case I2C_STAT_SLVCODE_ADDR:		/* Slave address received */
			/* Get slave address that needs servicing */
			data = Chip_I2CS_GetSlaveAddr(pI2C, Chip_I2CS_GetSlaveMatchIndex(pI2C));

			/* Call address callback */
			xfers->slaveStart(data);
			break;

		case I2C_STAT_SLVCODE_RX:		/* Data byte received, not used with DMA */
			/* Get received data */
			data = Chip_I2CS_ReadByte(pI2C);
			done = xfers->slaveRecv(data);
			break;

		case I2C_STAT_SLVCODE_TX:		/* Get byte that needs to be sent, or start DMA */
			/* Get data to send */
			done = xfers->slaveSend(&data);
			if (!((done == I2C_SLVCTL_SLVNACK) || (done == I2C_SLVCTL_SLVDMA))) {
				Chip_I2CS_WriteByte(pI2C, data);
			}
			break;
		}

		if (done == I2C_SLVCTL_SLVNACK) {
			Chip_I2CS_SlaveNACK(pI2C);
		}
		else if (done == I2C_SLVCTL_SLVDMA) {
			Chip_I2CS_SlaveEnableDMA(pI2C);
		}
		else {
			Chip_I2CS_SlaveContinue(pI2C);
		}
	}

	return xferDone;
}
