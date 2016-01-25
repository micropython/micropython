/*
 * @brief LPC18xx/43xx SD/SDIO driver
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
#include "string.h"

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

/* Initializes the SD/MMC controller */
void Chip_SDIF_Init(LPC_SDMMC_T *pSDMMC)
{
    /* Enable SDIO module clock */
	Chip_Clock_EnableOpts(CLK_MX_SDIO, true, true, 1);
	
    /* Software reset */
	pSDMMC->BMOD = MCI_BMOD_SWR;

	/* reset all blocks */
	pSDMMC->CTRL = MCI_CTRL_RESET | MCI_CTRL_FIFO_RESET | MCI_CTRL_DMA_RESET;
	while (pSDMMC->CTRL & (MCI_CTRL_RESET | MCI_CTRL_FIFO_RESET | MCI_CTRL_DMA_RESET)) {}

	/* Internal DMA setup for control register */
	pSDMMC->CTRL = MCI_CTRL_USE_INT_DMAC | MCI_CTRL_INT_ENABLE;
	pSDMMC->INTMASK = 0;

	/* Clear the interrupts for the host controller */
	pSDMMC->RINTSTS = 0xFFFFFFFF;

	/* Put in max timeout */
	pSDMMC->TMOUT = 0xFFFFFFFF;

	/* FIFO threshold settings for DMA, DMA burst of 4,   FIFO watermark at 16 */
	pSDMMC->FIFOTH = MCI_FIFOTH_DMA_MTS_4 | MCI_FIFOTH_RX_WM((SD_FIFO_SZ / 2) - 1) | MCI_FIFOTH_TX_WM(SD_FIFO_SZ / 2);

	/* Enable internal DMA, burst size of 4, fixed burst */
	pSDMMC->BMOD = MCI_BMOD_DE | MCI_BMOD_PBL4 | MCI_BMOD_DSL(4);

	/* disable clock to CIU (needs latch) */
	pSDMMC->CLKENA = 0;
	pSDMMC->CLKSRC = 0;
}

/* Shutdown the SD/MMC controller */
void Chip_SDIF_DeInit(LPC_SDMMC_T *pSDMMC)
{
    /* Disable the clock */
	Chip_Clock_Disable(CLK_MX_SDIO);
}

/* Function to send command to Card interface unit (CIU) */
int32_t Chip_SDIF_SendCmd(LPC_SDMMC_T *pSDMMC, uint32_t cmd, uint32_t arg)
{
	volatile int32_t tmo = 50;
	volatile int delay;

	/* set command arg reg*/
	pSDMMC->CMDARG = arg;
	pSDMMC->CMD = MCI_CMD_START | cmd;

	/* poll untill command is accepted by the CIU */
	while (--tmo && (pSDMMC->CMD & MCI_CMD_START)) {
		if (tmo & 1) {
			delay = 50;
		}
		else {
			delay = 18000;
		}

		while (--delay > 1) {}
	}

	return (tmo < 1) ? 1 : 0;
}

/* Read the response from the last command */
void Chip_SDIF_GetResponse(LPC_SDMMC_T *pSDMMC, uint32_t *resp)
{
	/* on this chip response is not a fifo so read all 4 regs */
	resp[0] = pSDMMC->RESP0;
	resp[1] = pSDMMC->RESP1;
	resp[2] = pSDMMC->RESP2;
	resp[3] = pSDMMC->RESP3;
}

/* Sets the SD bus clock speed */
void Chip_SDIF_SetClock(LPC_SDMMC_T *pSDMMC, uint32_t clk_rate, uint32_t speed)
{
	/* compute SD/MMC clock dividers */
	uint32_t div;

	div = ((clk_rate / speed) + 2) >> 1;

	if ((div == pSDMMC->CLKDIV) && pSDMMC->CLKENA) {
		return;	/* Closest speed is already set */

	}
	/* disable clock */
	pSDMMC->CLKENA = 0;

	/* User divider 0 */
	pSDMMC->CLKSRC = MCI_CLKSRC_CLKDIV0;

	/* inform CIU */
	Chip_SDIF_SendCmd(pSDMMC, MCI_CMD_UPD_CLK | MCI_CMD_PRV_DAT_WAIT, 0);

	/* set divider 0 to desired value */
	pSDMMC->CLKDIV = MCI_CLOCK_DIVIDER(0, div);

	/* inform CIU */
	Chip_SDIF_SendCmd(pSDMMC, MCI_CMD_UPD_CLK | MCI_CMD_PRV_DAT_WAIT, 0);

	/* enable clock */
	pSDMMC->CLKENA = MCI_CLKEN_ENABLE;

	/* inform CIU */
	Chip_SDIF_SendCmd(pSDMMC, MCI_CMD_UPD_CLK | MCI_CMD_PRV_DAT_WAIT, 0);
}

/* Function to clear interrupt & FIFOs */
void Chip_SDIF_SetClearIntFifo(LPC_SDMMC_T *pSDMMC)
{
	/* reset all blocks */
	pSDMMC->CTRL |= MCI_CTRL_FIFO_RESET;

	/* wait till resets clear */
	while (pSDMMC->CTRL & MCI_CTRL_FIFO_RESET) {}

	/* Clear interrupt status */
	pSDMMC->RINTSTS = 0xFFFFFFFF;
}

/* Setup DMA descriptors */
void Chip_SDIF_DmaSetup(LPC_SDMMC_T *pSDMMC, sdif_device *psdif_dev, uint32_t addr, uint32_t size)
{
	int i = 0;
	uint32_t ctrl, maxs;

	/* Reset DMA */
	pSDMMC->CTRL |= MCI_CTRL_DMA_RESET | MCI_CTRL_FIFO_RESET;
	while (pSDMMC->CTRL & MCI_CTRL_DMA_RESET) {}

	/* Build a descriptor list using the chained DMA method */
	while (size > 0) {
		/* Limit size of the transfer to maximum buffer size */
		maxs = size;
		if (maxs > MCI_DMADES1_MAXTR) {
			maxs = MCI_DMADES1_MAXTR;
		}
		size -= maxs;

		/* Set buffer size */
		psdif_dev->mci_dma_dd[i].des1 = MCI_DMADES1_BS1(maxs);

		/* Setup buffer address (chained) */
		psdif_dev->mci_dma_dd[i].des2 = addr + (i * MCI_DMADES1_MAXTR);

		/* Setup basic control */
		ctrl = MCI_DMADES0_OWN | MCI_DMADES0_CH;
		if (i == 0) {
			ctrl |= MCI_DMADES0_FS;	/* First DMA buffer */

		}
		/* No more data? Then this is the last descriptor */
		if (!size) {
			ctrl |= MCI_DMADES0_LD;
		}
		else {
			ctrl |= MCI_DMADES0_DIC;
		}

		/* Another descriptor is needed */
		psdif_dev->mci_dma_dd[i].des3 = (uint32_t) &psdif_dev->mci_dma_dd[i + 1];
		psdif_dev->mci_dma_dd[i].des0 = ctrl;

		i++;
	}

	/* Set DMA derscriptor base address */
	pSDMMC->DBADDR = (uint32_t) &psdif_dev->mci_dma_dd[0];
}

