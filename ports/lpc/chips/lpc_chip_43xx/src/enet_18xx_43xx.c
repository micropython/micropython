/*
 * @brief LPC18xx/43xx Ethernet driver
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

/* Saved address for PHY and clock divider */
STATIC uint32_t phyCfg;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

STATIC INLINE void reset(LPC_ENET_T *pENET)
{
    Chip_RGU_TriggerReset(RGU_ETHERNET_RST);
	while (Chip_RGU_InReset(RGU_ETHERNET_RST))
    {}

	/* Reset ethernet peripheral */
	Chip_ENET_Reset(pENET);
}

STATIC uint32_t Chip_ENET_CalcMDCClock(void)
{
	uint32_t val = SystemCoreClock / 1000000UL;

	if (val >= 20 && val < 35)
		return 2;
	if (val >= 35 && val < 60)
		return 3;
	if (val >= 60 && val < 100)
		return 0;
	if (val >= 100 && val < 150)
		return 1;
	if (val >= 150 && val < 250)
		return 4;
	if (val >= 250 && val < 300)
		return 5;

	/* Code should never reach here
	   unless there is BUG in frequency settings
	*/
	return 0;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Basic Ethernet interface initialization */
void Chip_ENET_Init(LPC_ENET_T *pENET, uint32_t phyAddr)
{
	Chip_Clock_EnableOpts(CLK_MX_ETHERNET, true, true, 1);

	reset(pENET);

	/* Setup MII link divider to /102 and PHY address 1 */
	Chip_ENET_SetupMII(pENET, Chip_ENET_CalcMDCClock(), phyAddr);

	/* Enhanced descriptors, burst length = 1 */
	pENET->DMA_BUS_MODE = DMA_BM_ATDS | DMA_BM_PBL(1) | DMA_BM_RPBL(1);

	/* Initial MAC configuration for checksum offload, full duplex,
	   100Mbps, disable receive own in half duplex, inter-frame gap
	   of 64-bits */
	pENET->MAC_CONFIG = MAC_CFG_BL(0) | MAC_CFG_IPC | MAC_CFG_DM |
						MAC_CFG_DO | MAC_CFG_FES | MAC_CFG_PS | MAC_CFG_IFG(3);

	/* Setup default filter */
	pENET->MAC_FRAME_FILTER = MAC_FF_PR | MAC_FF_RA;

	/* Flush transmit FIFO */
	pENET->DMA_OP_MODE = DMA_OM_FTF;

	/* Setup DMA to flush receive FIFOs at 32 bytes, service TX FIFOs at
	   64 bytes */
	pENET->DMA_OP_MODE |= DMA_OM_RTC(1) | DMA_OM_TTC(0);

	/* Clear all MAC interrupts */
	pENET->DMA_STAT = DMA_ST_ALL;

	/* Enable MAC interrupts */
	pENET->DMA_INT_EN = 0;
}

/* Ethernet interface shutdown */
void Chip_ENET_DeInit(LPC_ENET_T *pENET)
{
	/* Disable packet reception */
	pENET->MAC_CONFIG = 0;

	/* Flush transmit FIFO */
	pENET->DMA_OP_MODE = DMA_OM_FTF;

	/* Disable receive and transmit DMA processes */
	pENET->DMA_OP_MODE = 0;

	Chip_Clock_Disable(CLK_MX_ETHERNET);
}

/* Sets up the PHY link clock divider and PHY address */
void Chip_ENET_SetupMII(LPC_ENET_T *pENET, uint32_t div, uint8_t addr)
{
	/* Save clock divider and PHY address in MII address register */
	phyCfg = MAC_MIIA_PA(addr) | MAC_MIIA_CR(div);
}

/* Starts a PHY write via the MII */
void Chip_ENET_StartMIIWrite(LPC_ENET_T *pENET, uint8_t reg, uint16_t data)
{
	/* Write value at PHY address and register */
	pENET->MAC_MII_ADDR = phyCfg | MAC_MIIA_GR(reg) | MAC_MIIA_W;
	pENET->MAC_MII_DATA = (uint32_t) data;
	pENET->MAC_MII_ADDR |= MAC_MIIA_GB;
}

/*Starts a PHY read via the MII */
void Chip_ENET_StartMIIRead(LPC_ENET_T *pENET, uint8_t reg)
{
	/* Read value at PHY address and register */
	pENET->MAC_MII_ADDR = phyCfg | MAC_MIIA_GR(reg);
	pENET->MAC_MII_ADDR |= MAC_MIIA_GB;
}

/* Sets full or half duplex for the interface */
void Chip_ENET_SetDuplex(LPC_ENET_T *pENET, bool full)
{
	if (full) {
		pENET->MAC_CONFIG |= MAC_CFG_DM;
	}
	else {
		pENET->MAC_CONFIG &= ~MAC_CFG_DM;
	}
}

/* Sets speed for the interface */
void Chip_ENET_SetSpeed(LPC_ENET_T *pENET, bool speed100)
{
	if (speed100) {
		pENET->MAC_CONFIG |= MAC_CFG_FES;
	}
	else {
		pENET->MAC_CONFIG &= ~MAC_CFG_FES;
	}
}







