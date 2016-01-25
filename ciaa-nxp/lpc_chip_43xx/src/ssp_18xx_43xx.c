/*
 * @brief LPC18xx/43xx SSP driver
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

STATIC void SSP_Write2BFifo(LPC_SSP_T *pSSP, Chip_SSP_DATA_SETUP_T *xf_setup)
{
	if (xf_setup->tx_data) {
		Chip_SSP_SendFrame(pSSP, (*(uint16_t *) ((uint32_t) xf_setup->tx_data +
												 xf_setup->tx_cnt)));
	}
	else {
		Chip_SSP_SendFrame(pSSP, 0xFFFF);
	}

	xf_setup->tx_cnt += 2;
}

/** SSP macro: write 1 bytes to FIFO buffer */
STATIC void SSP_Write1BFifo(LPC_SSP_T *pSSP, Chip_SSP_DATA_SETUP_T *xf_setup)
{
	if (xf_setup->tx_data) {
		Chip_SSP_SendFrame(pSSP, (*(uint8_t *) ((uint32_t) xf_setup->tx_data + xf_setup->tx_cnt)));
	}
	else {
		Chip_SSP_SendFrame(pSSP, 0xFF);
	}

	xf_setup->tx_cnt++;
}

/** SSP macro: read 1 bytes from FIFO buffer */
STATIC void SSP_Read2BFifo(LPC_SSP_T *pSSP, Chip_SSP_DATA_SETUP_T *xf_setup)
{
	uint16_t rDat;

	while ((Chip_SSP_GetStatus(pSSP, SSP_STAT_RNE) == SET) &&
		   (xf_setup->rx_cnt < xf_setup->length)) {
		rDat = Chip_SSP_ReceiveFrame(pSSP);
		if (xf_setup->rx_data) {
			*(uint16_t *) ((uint32_t) xf_setup->rx_data + xf_setup->rx_cnt) = rDat;
		}

		xf_setup->rx_cnt += 2;
	}
}

/** SSP macro: read 2 bytes from FIFO buffer */
STATIC void SSP_Read1BFifo(LPC_SSP_T *pSSP, Chip_SSP_DATA_SETUP_T *xf_setup)
{
	uint16_t rDat;

	while ((Chip_SSP_GetStatus(pSSP, SSP_STAT_RNE) == SET) &&
		   (xf_setup->rx_cnt < xf_setup->length)) {
		rDat = Chip_SSP_ReceiveFrame(pSSP);
		if (xf_setup->rx_data) {
			*(uint8_t *) ((uint32_t) xf_setup->rx_data + xf_setup->rx_cnt) = rDat;
		}

		xf_setup->rx_cnt++;
	}
}

/* Returns clock index for the register interface */
STATIC CHIP_CCU_CLK_T Chip_SSP_GetClockIndex(LPC_SSP_T *pSSP)
{
	CHIP_CCU_CLK_T clkSSP;

	if (pSSP == LPC_SSP1) {
		clkSSP = CLK_MX_SSP1;
	}
	else {
		clkSSP = CLK_MX_SSP0;
	}

	return clkSSP;
}

/* Returns clock index for the peripheral block */
STATIC CHIP_CCU_CLK_T Chip_SSP_GetPeriphClockIndex(LPC_SSP_T *pSSP)
{
	CHIP_CCU_CLK_T clkSSP;

	if (pSSP == LPC_SSP1) {
		clkSSP = CLK_APB2_SSP1;
	}
	else {
		clkSSP = CLK_APB0_SSP0;
	}

	return clkSSP;
}
/*****************************************************************************
 * Public functions
 ****************************************************************************/

/*Set up output clocks per bit for SSP bus*/
void Chip_SSP_SetClockRate(LPC_SSP_T *pSSP, uint32_t clk_rate, uint32_t prescale)
{
	uint32_t temp;
	temp = pSSP->CR0 & (~(SSP_CR0_SCR(0xFF)));
	pSSP->CR0 = temp | (SSP_CR0_SCR(clk_rate));
	pSSP->CPSR = prescale;
}

/* SSP Polling Read/Write in blocking mode */
uint32_t Chip_SSP_RWFrames_Blocking(LPC_SSP_T *pSSP, Chip_SSP_DATA_SETUP_T *xf_setup)
{
	/* Clear all remaining frames in RX FIFO */
	while (Chip_SSP_GetStatus(pSSP, SSP_STAT_RNE)) {
		Chip_SSP_ReceiveFrame(pSSP);
	}

	/* Clear status */
	Chip_SSP_ClearIntPending(pSSP, SSP_INT_CLEAR_BITMASK);

	if (Chip_SSP_GetDataSize(pSSP) > SSP_BITS_8) {
		while (xf_setup->rx_cnt < xf_setup->length || xf_setup->tx_cnt < xf_setup->length) {
			/* write data to buffer */
			if (( Chip_SSP_GetStatus(pSSP, SSP_STAT_TNF) == SET) && ( xf_setup->tx_cnt < xf_setup->length) ) {
				SSP_Write2BFifo(pSSP, xf_setup);
			}

			/* Check overrun error */
			if (Chip_SSP_GetRawIntStatus(pSSP, SSP_RORRIS) == SET) {
				return ERROR;
			}

			/* Check for any data available in RX FIFO */
			SSP_Read2BFifo(pSSP, xf_setup);
		}
	}
	else {
		while (xf_setup->rx_cnt < xf_setup->length || xf_setup->tx_cnt < xf_setup->length) {
			/* write data to buffer */
			if (( Chip_SSP_GetStatus(pSSP, SSP_STAT_TNF) == SET) && ( xf_setup->tx_cnt < xf_setup->length) ) {
				SSP_Write1BFifo(pSSP, xf_setup);
			}

			/* Check overrun error */
			if (Chip_SSP_GetRawIntStatus(pSSP, SSP_RORRIS) == SET) {
				return ERROR;
			}

			/* Check for any data available in RX FIFO */
			SSP_Read1BFifo(pSSP, xf_setup);
		}
	}
	if (xf_setup->tx_data) {
		return xf_setup->tx_cnt;
	}
	else if (xf_setup->rx_data) {
		return xf_setup->rx_cnt;
	}

	return 0;
}

/* SSP Polling Write in blocking mode */
uint32_t Chip_SSP_WriteFrames_Blocking(LPC_SSP_T *pSSP, const uint8_t *buffer, uint32_t buffer_len)
{
	uint32_t tx_cnt = 0, rx_cnt = 0;

	/* Clear all remaining frames in RX FIFO */
	while (Chip_SSP_GetStatus(pSSP, SSP_STAT_RNE)) {
		Chip_SSP_ReceiveFrame(pSSP);
	}

	/* Clear status */
	Chip_SSP_ClearIntPending(pSSP, SSP_INT_CLEAR_BITMASK);

	if (Chip_SSP_GetDataSize(pSSP) > SSP_BITS_8) {
		uint16_t *wdata16;

		wdata16 = (uint16_t *) buffer;

		while (tx_cnt < buffer_len || rx_cnt < buffer_len) {
			/* write data to buffer */
			if ((Chip_SSP_GetStatus(pSSP, SSP_STAT_TNF) == SET) && (tx_cnt < buffer_len)) {
				Chip_SSP_SendFrame(pSSP, *wdata16);
				wdata16++;
				tx_cnt += 2;
			}

			/* Check overrun error */
			if (Chip_SSP_GetRawIntStatus(pSSP, SSP_RORRIS) == SET) {
				return ERROR;
			}

			/* Check for any data available in RX FIFO */
			while (Chip_SSP_GetStatus(pSSP, SSP_STAT_RNE) == SET) {
				Chip_SSP_ReceiveFrame(pSSP);	/* read dummy data */
				rx_cnt += 2;
			}
		}
	}
	else {
		const uint8_t *wdata8;

		wdata8 = buffer;

		while (tx_cnt < buffer_len || rx_cnt < buffer_len) {
			/* write data to buffer */
			if ((Chip_SSP_GetStatus(pSSP, SSP_STAT_TNF) == SET) && (tx_cnt < buffer_len)) {
				Chip_SSP_SendFrame(pSSP, *wdata8);
				wdata8++;
				tx_cnt++;
			}

			/* Check overrun error */
			if (Chip_SSP_GetRawIntStatus(pSSP, SSP_RORRIS) == SET) {
				return ERROR;
			}

			/* Check for any data available in RX FIFO */
			while (Chip_SSP_GetStatus(pSSP, SSP_STAT_RNE) == SET && rx_cnt < buffer_len) {
				Chip_SSP_ReceiveFrame(pSSP);	/* read dummy data */
				rx_cnt++;
			}
		}
	}

	return tx_cnt;

}

/* SSP Polling Read in blocking mode */
uint32_t Chip_SSP_ReadFrames_Blocking(LPC_SSP_T *pSSP, uint8_t *buffer, uint32_t buffer_len)
{
	uint32_t rx_cnt = 0, tx_cnt = 0;

	/* Clear all remaining frames in RX FIFO */
	while (Chip_SSP_GetStatus(pSSP, SSP_STAT_RNE)) {
		Chip_SSP_ReceiveFrame(pSSP);
	}

	/* Clear status */
	Chip_SSP_ClearIntPending(pSSP, SSP_INT_CLEAR_BITMASK);

	if (Chip_SSP_GetDataSize(pSSP) > SSP_BITS_8) {
		uint16_t *rdata16;

		rdata16 = (uint16_t *) buffer;

		while (tx_cnt < buffer_len || rx_cnt < buffer_len) {
			/* write data to buffer */
			if ((Chip_SSP_GetStatus(pSSP, SSP_STAT_TNF) == SET) && (tx_cnt < buffer_len)) {
				Chip_SSP_SendFrame(pSSP, 0xFFFF);	/* just send dummy data */
				tx_cnt += 2;
			}

			/* Check overrun error */
			if (Chip_SSP_GetRawIntStatus(pSSP, SSP_RORRIS) == SET) {
				return ERROR;
			}

			/* Check for any data available in RX FIFO */
			while (Chip_SSP_GetStatus(pSSP, SSP_STAT_RNE) == SET && rx_cnt < buffer_len) {
				*rdata16 = Chip_SSP_ReceiveFrame(pSSP);
				rdata16++;
				rx_cnt += 2;
			}
		}
	}
	else {
		uint8_t *rdata8;

		rdata8 = buffer;

		while (tx_cnt < buffer_len || rx_cnt < buffer_len) {
			/* write data to buffer */
			if ((Chip_SSP_GetStatus(pSSP, SSP_STAT_TNF) == SET) && (tx_cnt < buffer_len)) {
				Chip_SSP_SendFrame(pSSP, 0xFF);	/* just send dummy data		 */
				tx_cnt++;
			}

			/* Check overrun error */
			if (Chip_SSP_GetRawIntStatus(pSSP, SSP_RORRIS) == SET) {
				return ERROR;
			}

			/* Check for any data available in RX FIFO */
			while (Chip_SSP_GetStatus(pSSP, SSP_STAT_RNE) == SET && rx_cnt < buffer_len) {
				*rdata8 = Chip_SSP_ReceiveFrame(pSSP);
				rdata8++;
				rx_cnt++;
			}
		}
	}

	return rx_cnt;

}

/* Clean all data in RX FIFO of SSP */
void Chip_SSP_Int_FlushData(LPC_SSP_T *pSSP)
{
	if (Chip_SSP_GetStatus(pSSP, SSP_STAT_BSY)) {
		while (Chip_SSP_GetStatus(pSSP, SSP_STAT_BSY)) {}
	}

	/* Clear all remaining frames in RX FIFO */
	while (Chip_SSP_GetStatus(pSSP, SSP_STAT_RNE)) {
		Chip_SSP_ReceiveFrame(pSSP);
	}

	/* Clear status */
	Chip_SSP_ClearIntPending(pSSP, SSP_INT_CLEAR_BITMASK);
}

/* SSP Interrupt Read/Write with 8-bit frame width */
Status Chip_SSP_Int_RWFrames8Bits(LPC_SSP_T *pSSP, Chip_SSP_DATA_SETUP_T *xf_setup)
{
	/* Check overrun error in RIS register */
	if (Chip_SSP_GetRawIntStatus(pSSP, SSP_RORRIS) == SET) {
		return ERROR;
	}

	if ((xf_setup->tx_cnt != xf_setup->length) || (xf_setup->rx_cnt != xf_setup->length)) {
		/* check if RX FIFO contains data */
		SSP_Read1BFifo(pSSP, xf_setup);

		while ((Chip_SSP_GetStatus(pSSP, SSP_STAT_TNF)) && (xf_setup->tx_cnt != xf_setup->length)) {
			/* Write data to buffer */
			SSP_Write1BFifo(pSSP, xf_setup);

			/* Check overrun error in RIS register */
			if (Chip_SSP_GetRawIntStatus(pSSP, SSP_RORRIS) == SET) {
				return ERROR;
			}

			/*  Check for any data available in RX FIFO */
			SSP_Read1BFifo(pSSP, xf_setup);
		}

		return SUCCESS;
	}

	return ERROR;
}

/* SSP Interrupt Read/Write with 16-bit frame width */
Status Chip_SSP_Int_RWFrames16Bits(LPC_SSP_T *pSSP, Chip_SSP_DATA_SETUP_T *xf_setup)
{
	/* Check overrun error in RIS register */
	if (Chip_SSP_GetRawIntStatus(pSSP, SSP_RORRIS) == SET) {
		return ERROR;
	}

	if ((xf_setup->tx_cnt != xf_setup->length) || (xf_setup->rx_cnt != xf_setup->length)) {
		/* check if RX FIFO contains data */
		SSP_Read2BFifo(pSSP, xf_setup);

		while ((Chip_SSP_GetStatus(pSSP, SSP_STAT_TNF)) && (xf_setup->tx_cnt != xf_setup->length)) {
			/* Write data to buffer */
			SSP_Write2BFifo(pSSP, xf_setup);

			/* Check overrun error in RIS register */
			if (Chip_SSP_GetRawIntStatus(pSSP, SSP_RORRIS) == SET) {
				return ERROR;
			}

			/*  Check for any data available in RX FIFO			 */
			SSP_Read2BFifo(pSSP, xf_setup);
		}

		return SUCCESS;
	}

	return ERROR;
}

/* Set the SSP operating modes, master or slave */
void Chip_SSP_SetMaster(LPC_SSP_T *pSSP, bool master)
{
	if (master) {
		Chip_SSP_Set_Mode(pSSP, SSP_MODE_MASTER);
	}
	else {
		Chip_SSP_Set_Mode(pSSP, SSP_MODE_SLAVE);
	}
}

/* Set the clock frequency for SSP interface */
void Chip_SSP_SetBitRate(LPC_SSP_T *pSSP, uint32_t bitRate)
{
	uint32_t ssp_clk, cr0_div, cmp_clk, prescale;

	ssp_clk = Chip_Clock_GetRate(Chip_SSP_GetPeriphClockIndex(pSSP));

	cr0_div = 0;
	cmp_clk = 0xFFFFFFFF;
	prescale = 2;

	while (cmp_clk > bitRate) {
		cmp_clk = ssp_clk / ((cr0_div + 1) * prescale);
		if (cmp_clk > bitRate) {
			cr0_div++;
			if (cr0_div > 0xFF) {
				cr0_div = 0;
				prescale += 2;
			}
		}
	}

	Chip_SSP_SetClockRate(pSSP, cr0_div, prescale);
}

/* Initialize the SSP */
void Chip_SSP_Init(LPC_SSP_T *pSSP)
{
	Chip_Clock_Enable(Chip_SSP_GetClockIndex(pSSP));
	Chip_Clock_Enable(Chip_SSP_GetPeriphClockIndex(pSSP));

	Chip_SSP_Set_Mode(pSSP, SSP_MODE_MASTER);
	Chip_SSP_SetFormat(pSSP, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_CPHA0_CPOL0);
	Chip_SSP_SetBitRate(pSSP, 100000);
}

/* De-initializes the SSP peripheral */
void Chip_SSP_DeInit(LPC_SSP_T *pSSP)
{
	Chip_SSP_Disable(pSSP);

	Chip_Clock_Disable(Chip_SSP_GetPeriphClockIndex(pSSP));
	Chip_Clock_Disable(Chip_SSP_GetClockIndex(pSSP));
	
}

