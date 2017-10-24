/*
 * @brief LPC18xx/LPC43xx Chip specific SystemInit
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
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

/* Structure for initial base clock states */
struct CLK_BASE_STATES {
	CHIP_CGU_BASE_CLK_T clk;	/* Base clock */
	CHIP_CGU_CLKIN_T clkin;	/* Base clock source, see UM for allowable souorces per base clock */
	bool autoblock_enab;	/* Set to true to enable autoblocking on frequency change */
	bool powerdn;			/* Set to true if the base clock is initially powered down */
};

static const struct CLK_BASE_STATES InitClkStates[] = {
	{CLK_BASE_SAFE, CLKIN_IRC, true, false},
	{CLK_BASE_APB1, CLKIN_MAINPLL, true, false},
	{CLK_BASE_APB3, CLKIN_MAINPLL, true, false},
	{CLK_BASE_USB0, CLKIN_USBPLL, true, true},
#if defined(CHIP_LPC43XX)
	{CLK_BASE_PERIPH, CLKIN_MAINPLL, true, false},
	{CLK_BASE_SPI, CLKIN_MAINPLL, true, false},
	{CLK_BASE_ADCHS, CLKIN_MAINPLL, true, true},
#endif
	{CLK_BASE_SDIO, CLKIN_MAINPLL, true, false},
	{CLK_BASE_SSP0, CLKIN_MAINPLL, true, false},
	{CLK_BASE_SSP1, CLKIN_MAINPLL, true, false},
	{CLK_BASE_UART0, CLKIN_MAINPLL, true, false},
	{CLK_BASE_UART1, CLKIN_MAINPLL, true, false},
	{CLK_BASE_UART2, CLKIN_MAINPLL, true, false},
	{CLK_BASE_UART3, CLKIN_MAINPLL, true, false},
	{CLK_BASE_OUT, CLKINPUT_PD, true, false},
	{CLK_BASE_APLL, CLKINPUT_PD, true, false},
	{CLK_BASE_CGU_OUT0, CLKINPUT_PD, true, false},
	{CLK_BASE_CGU_OUT1, CLKINPUT_PD, true, false},
};

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/
/* Setup Chip Core clock */
void Chip_SetupCoreClock(CHIP_CGU_CLKIN_T clkin, uint32_t core_freq, bool setbase)
{
	int i;
	volatile uint32_t delay = 500;
	uint32_t direct = 0, pdivide = 0;
	PLL_PARAM_T ppll;

	if (clkin == CLKIN_CRYSTAL) {
		/* Switch main system clocking to crystal */
		Chip_Clock_EnableCrystal();
	}
	Chip_Clock_SetBaseClock(CLK_BASE_MX, clkin, true, false);
	Chip_Clock_DisableMainPLL(); /* Disable PLL */

	/* Calculate the PLL Parameters */
	ppll.srcin = clkin;
	Chip_Clock_CalcMainPLLValue(core_freq, &ppll);

	if (core_freq > 110000000UL) {
		if (ppll.ctrl & (1 << 6)) {
			while(1);		// to run in integer mode above 110 MHz, you need to use IDIV clock to boot strap CPU to that freq
		} else if (ppll.ctrl & (1 << 7)){
			direct = 1;
			ppll.ctrl &= ~(1 << 7);
		} else {
			pdivide = 1;
			ppll.psel++;
		}
	}

	/* Setup and start the PLL */
	Chip_Clock_SetupMainPLL(&ppll);

	/* Wait for the PLL to lock */
	while(!Chip_Clock_MainPLLLocked()) {}

	/* Set core clock base as PLL1 */
	Chip_Clock_SetBaseClock(CLK_BASE_MX, CLKIN_MAINPLL, true, false);

	if (direct) {
		delay = 1000;
		while(delay --){} /* Wait for approx 50 uSec -- for power supply to stabilize*/
		ppll.ctrl |= 1 << 7;
		Chip_Clock_SetupMainPLL(&ppll); /* Set DIRECT to operate at full frequency */
	} else if (pdivide) {
		delay = 1000;
		while(delay --){} /* Wait for approx 50 uSec -- for power supply to stabilize */
		ppll.psel--;
		Chip_Clock_SetupMainPLL(&ppll); /* Set PDIV to operate at full frequency */
	}

	if (setbase) {
		/* Setup system base clocks and initial states. This won't enable and
		   disable individual clocks, but sets up the base clock sources for
		   each individual peripheral clock. */
		for (i = 0; i < (sizeof(InitClkStates) / sizeof(InitClkStates[0])); i++) {
			Chip_Clock_SetBaseClock(InitClkStates[i].clk, InitClkStates[i].clkin,
									InitClkStates[i].autoblock_enab, InitClkStates[i].powerdn);
		}
	}
}

/* Setup system clocking */
void Chip_SetupXtalClocking(void)
{
	Chip_SetupCoreClock(CLKIN_CRYSTAL, MAX_CLOCK_FREQ, true);
}

/* Set up and initialize hardware prior to call to main */
void Chip_SetupIrcClocking(void)
{
	Chip_SetupCoreClock(CLKIN_IRC, MAX_CLOCK_FREQ, true);
}

/* Set up and initialize hardware prior to call to main */
void Chip_SystemInit(void)
{
	/* Initial internal clocking */
	Chip_SetupIrcClocking();
}






