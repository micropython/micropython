/*
 * @brief LPC5410X System & Control driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
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

/* Set source for non-maskable interrupt (NMI) */
void Chip_SYSCON_SetNMISource(uint32_t intsrc)
{
	uint32_t reg;

	reg = LPC_SYSCON->NMISRC;
#if defined(CORE_M4)
	reg &= ~SYSCON_NMISRC_M4_ENABLE;
#else
	reg &= ~SYSCON_NMISRC_M0_ENABLE;
	intsrc = (intsrc << 8);
#endif

	/* First write without NMI bit, and then write source */
	LPC_SYSCON->NMISRC = reg;
	LPC_SYSCON->NMISRC = reg | intsrc;
}

/* Enable interrupt used for NMI source */
void Chip_SYSCON_EnableNMISource(void)
{
#if defined(CORE_M4)
	LPC_SYSCON->NMISRC |= SYSCON_NMISRC_M4_ENABLE;
#else
	LPC_SYSCON->NMISRC |= SYSCON_NMISRC_M0_ENABLE;
#endif
}

/* Disable interrupt used for NMI source */
void Chip_SYSCON_DisableNMISource(void)
{
#if defined(CORE_M4)
	LPC_SYSCON->NMISRC &= ~SYSCON_NMISRC_M4_ENABLE;
#else
	LPC_SYSCON->NMISRC &= ~SYSCON_NMISRC_M0_ENABLE;
#endif
}

/* Enable or disable asynchronous APB bridge and subsystem */
void Chip_SYSCON_Enable_ASYNC_Syscon(bool enable)
{
	if (enable) {
		LPC_SYSCON->ASYNCAPBCTRL = 0x01;
	}
	else {
		LPC_SYSCON->ASYNCAPBCTRL = 0x00;
	}
}

/* Resets a peripheral */
void Chip_SYSCON_PeriphReset(CHIP_SYSCON_PERIPH_RESET_T periph)
{
	uint32_t pid = (uint32_t) periph;

	if (pid >= 128) {
		/* Async resets mapped to 128 and above, offset for peripheral bit index */
		pid = 1 << (((uint32_t) periph) - 128);
		LPC_ASYNC_SYSCON->ASYNCPRESETCTRLSET = pid;
		LPC_ASYNC_SYSCON->ASYNCPRESETCTRLCLR = pid;
	}
	else if (periph >= 32) {
		pid = 1 << (((uint32_t) periph) - 32);
		LPC_SYSCON->PRESETCTRLSET[1] = pid;
		LPC_SYSCON->PRESETCTRLCLR[1] = pid;
	}
	else {
		pid = 1 << ((uint32_t) periph);
		LPC_SYSCON->PRESETCTRLSET[0] = pid;
		LPC_SYSCON->PRESETCTRLCLR[0] = pid;
	}
}

/* Returns the computed value for a frequency measurement cycle */
uint32_t Chip_SYSCON_GetCompFreqMeas(uint32_t refClockRate)
{
	uint32_t capval;
	uint64_t clkrate = 0;

	/* Get raw capture value */
	capval = Chip_SYSCON_GetRawFreqMeasCapval();

	/* Limit CAPVAL check */
	if (capval > 2) {
		clkrate = (((uint64_t) capval - 2) * (uint64_t) refClockRate) / 0x4000;
	}

	return (uint32_t) clkrate;
}

uint32_t Chip_SYSCON_PLLDelay(void)
{
	uint32_t ifreq = Chip_Clock_GetSystemPLLInClockRate();
	uint32_t coreclk = Chip_Clock_GetMainClockRate(), div = 0;
	if (coreclk) {
		div = 12000000 / coreclk;
	}
	if (!div) {
		div = 1;
	}
	if (ifreq <= 100000) {
		/* Dealy for 600 uSecs */
		return 1430 / div;
	}
	else if (ifreq <= 1000000) {
		/* Delay for 300 uSecs */
		return 1210 / div;
	}
	else if (ifreq <= 10000000) {
		return 657 / div;
	}
	else {
		return 172 / div;	/* 72 uSecs */
	}
}

void Chip_SYSCON_PowerUp(uint32_t powerupmask)
{
	/* If turning the PLL back on, perform the following sequence to accelerate PLL lock */
	if (powerupmask & SYSCON_PDRUNCFG_PD_SYS_PLL) {
		volatile uint32_t delayX;
		uint32_t maxCCO = (1 << 18) | 0x5dd2;	/* CCO = 1.6Ghz + MDEC enabled*/
		uint32_t curSSCTRL = LPC_SYSCON->SYSPLLSSCTRL[0] & ~(1 << 17);	/* current value with mreq cleared */

		/* Initialize  and power up PLL */
		LPC_SYSCON->SYSPLLSSCTRL[0] = maxCCO;
		LPC_SYSCON->PDRUNCFGCLR = SYSCON_PDRUNCFG_PD_SYS_PLL;

		/* Set mreq to activate */
		LPC_SYSCON->SYSPLLSSCTRL[0] = maxCCO | (1 << 17);

		/* Delay for 72 uSec @ 12Mhz */
		for (delayX = Chip_SYSCON_PLLDelay(); delayX; --delayX) {}

		/* clear mreq to prepare for restoring mreq */
		LPC_SYSCON->SYSPLLSSCTRL[0] = curSSCTRL;

		/* set original value back and activate */
		LPC_SYSCON->SYSPLLSSCTRL[0] = curSSCTRL | (1 << 17);
	}

	/* Enable peripheral states by setting low */
	LPC_SYSCON->PDRUNCFGCLR = powerupmask;
}
