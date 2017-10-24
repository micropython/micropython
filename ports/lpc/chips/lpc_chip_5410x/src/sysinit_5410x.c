/*
 * @brief LPC5410X Chip specific SystemInit
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

/* Sets the best FLASH clock arte for the passed frequency */
static void setupFlashClocks(uint32_t freq)
{
	/* v17.0 ROM support only - coarse FLASH clocking timing.
	   FLASH access is setup based on voltage for v17.1 and later ROMs
	   as part of the power library. */
	if (Chip_POWER_GetROMVersion() == LPC5410X_ROMVER_0) {
		if (freq < 20000000) {
			Chip_SYSCON_SetFLASHAccess(SYSCON_FLASH_1CYCLE);
		}
		else if (freq < 48000000) {
			Chip_SYSCON_SetFLASHAccess(SYSCON_FLASH_2CYCLE);
		}
		else if (freq < 72000000) {
			Chip_SYSCON_SetFLASHAccess(SYSCON_FLASH_3CYCLE);
		}
		else {
			Chip_SYSCON_SetFLASHAccess(SYSCON_FLASH_4CYCLE);
		}
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Clock and PLL initialization based on the internal oscillator */
void Chip_SetupIrcClocking(uint32_t iFreq)
{
	PLL_CONFIG_T pllConfig;
	PLL_SETUP_T pllSetup;
	PLL_ERROR_T pllError;
	bool usePLL = false;

	/* Turn on the IRC by clearing the power down bit */
	Chip_SYSCON_PowerUp(SYSCON_PDRUNCFG_PD_IRC_OSC | SYSCON_PDRUNCFG_PD_IRC);

	/* Select the PLL input to the IRC */
	Chip_Clock_SetSystemPLLSource(SYSCON_PLLCLKSRC_IRC);

	/* Setup FLASH access */
	setupFlashClocks(iFreq);

	if (iFreq > 12000000) {
		usePLL = true;
	}

	if (usePLL) {
		/* Select the PLL input to the IRC */

		/* Power down PLL to change the PLL divider ratio */
		Chip_SYSCON_PowerDown(SYSCON_PDRUNCFG_PD_SYS_PLL);

		/* Setup PLL configuration */
		pllConfig.desiredRate = iFreq;
		pllConfig.InputRate = 0;
		pllConfig.flags = PLL_CONFIGFLAG_FORCENOFRACT;
		pllError = Chip_Clock_SetupPLLData(&pllConfig, &pllSetup);
		if (pllError == PLL_ERROR_SUCCESS) {
			pllSetup.flags = PLL_SETUPFLAG_WAITLOCK | PLL_SETUPFLAG_ADGVOLT;
			pllError = Chip_Clock_SetupSystemPLLPrec(&pllSetup);
		}
	}

	/* Set system clock divider to 1 */
	Chip_Clock_SetSysClockDiv(1);

	/* Set main clock source to the system PLL. This will drive 24MHz
	   for the main clock and 24MHz for the system clock */
	if (usePLL)
		Chip_Clock_SetMainClockSource(SYSCON_MAINCLKSRC_PLLOUT);
	else
		Chip_Clock_SetMainClockSource(SYSCON_MAINCLKSRC_IRC);

	/* ASYSNC SYSCON needs to be on or all serial peripheral won't work.
	   Be careful if PLL is used or not, ASYNC_SYSCON source needs to be
	   selected carefully. */
	Chip_SYSCON_Enable_ASYNC_Syscon(true);
	Chip_Clock_SetAsyncSysconClockDiv(1);
	Chip_Clock_SetAsyncSysconClockSource(SYSCON_ASYNC_IRC);
}

/* Clock and PLL initialization based on the external clock input */
void Chip_SetupExtInClocking(uint32_t iFreq)
{
	PLL_CONFIG_T pllConfig;
	PLL_SETUP_T pllSetup;
	PLL_ERROR_T pllError;

	/* IOCON clock left on, this is needed is CLKIN is used. */
	Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_IOCON);

	/* Select external clock input pin */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 22, (IOCON_MODE_PULLUP |
											IOCON_FUNC1 | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF));

	/* Select the PLL input to the EXT clock input */
	Chip_Clock_SetSystemPLLSource(SYSCON_PLLCLKSRC_CLKIN);

	/* Setup FLASH access */
	setupFlashClocks(iFreq);

	/* Power down PLL to change the PLL divider ratio */
	Chip_SYSCON_PowerDown(SYSCON_PDRUNCFG_PD_SYS_PLL);

	/* Setup PLL configuration */
	pllConfig.desiredRate = iFreq;
	pllConfig.InputRate = 0;
	pllConfig.flags = PLL_CONFIGFLAG_FORCENOFRACT;
	pllError = Chip_Clock_SetupPLLData(&pllConfig, &pllSetup);
	if (pllError == PLL_ERROR_SUCCESS) {
		pllSetup.flags = PLL_SETUPFLAG_WAITLOCK | PLL_SETUPFLAG_ADGVOLT;
		pllError = Chip_Clock_SetupSystemPLLPrec(&pllSetup);
	}

	/* Set system clock divider to 1 */
	Chip_Clock_SetSysClockDiv(1);

	/* Set main clock source to the system PLL. This will drive 24MHz
	   for the main clock and 24MHz for the system clock */
	Chip_Clock_SetMainClockSource(SYSCON_MAINCLKSRC_PLLOUT);

	/* ASYSNC SYSCON needs to be on or all serial peripheral won't work.
	   Be careful if PLL is used or not, ASYNC_SYSCON source needs to be
	   selected carefully. */
	Chip_SYSCON_Enable_ASYNC_Syscon(true);
	Chip_Clock_SetAsyncSysconClockDiv(1);
	Chip_Clock_SetAsyncSysconClockSource(SYSCON_ASYNC_IRC);
}

/* Set up and initialize hardware prior to call to main */
void Chip_SystemInit(void)
{
	/* Initial internal clocking @100MHz */
	Chip_SetupIrcClocking(100000000);
}
