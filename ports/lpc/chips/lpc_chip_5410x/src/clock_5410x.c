/*
 * @brief LPC5410X clock driver
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

/* Return asynchronous APB clock rate (no regard for divider) */
static uint32_t Chip_Clock_GetAsyncSyscon_ClockRate_NoDiv(void)
{
	CHIP_ASYNC_SYSCON_SRC_T src;
	uint32_t clkRate;

	src = Chip_Clock_GetAsyncSysconClockSource();
	switch (src) {
	case SYSCON_ASYNC_IRC:
		clkRate = Chip_Clock_GetIntOscRate();
		break;

	case SYSCON_ASYNC_WDTOSC:
		clkRate = Chip_Clock_GetWDTOSCRate();
		break;

	case SYSCON_ASYNC_MAINCLK:
		clkRate = Chip_Clock_GetMainClockRate();
		break;

	case SYSCON_ASYNC_CLKIN:
		clkRate = Chip_Clock_GetSystemPLLInClockRate();
		break;

	case SYSCON_ASYNC_SYSPLLOUT:
		clkRate = Chip_Clock_GetSystemPLLOutClockRate(false);
		break;

	default:
		clkRate = 0;
		break;
	}

	return clkRate;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Return main A clock rate */
uint32_t Chip_Clock_GetMain_A_ClockRate(void)
{
	uint32_t clkRate = 0;

	switch (Chip_Clock_GetMain_A_ClockSource()) {
	case SYSCON_MAIN_A_CLKSRC_IRC:
		clkRate = Chip_Clock_GetIntOscRate();
		break;

	case SYSCON_MAIN_A_CLKSRCA_CLKIN:
		clkRate = Chip_Clock_GetExtClockInRate();
		break;

	case SYSCON_MAIN_A_CLKSRCA_WDTOSC:
		clkRate = Chip_Clock_GetWDTOSCRate();
		break;

	default:
		clkRate = 0;
		break;
	}

	return clkRate;
}

/* Return main B clock rate */
uint32_t Chip_Clock_GetMain_B_ClockRate(void)
{
	uint32_t clkRate = 0;

	switch (Chip_Clock_GetMain_B_ClockSource()) {
	case SYSCON_MAIN_B_CLKSRC_MAINCLKSELA:
		clkRate = Chip_Clock_GetMain_A_ClockRate();
		break;

	case SYSCON_MAIN_B_CLKSRC_SYSPLLIN:
		clkRate = Chip_Clock_GetSystemPLLInClockRate();
		break;

	case SYSCON_MAIN_B_CLKSRC_SYSPLLOUT:
		clkRate = Chip_Clock_GetSystemPLLOutClockRate(false);
		break;

	case SYSCON_MAIN_B_CLKSRC_RTC:
		clkRate = Chip_Clock_GetRTCOscRate();
		break;
	}

	return clkRate;
}

/* Set CLKOUT clock source and divider */
void Chip_Clock_SetCLKOUTSource(CHIP_SYSCON_CLKOUTSRC_T src, uint32_t div)
{
	uint32_t srcClk = (uint32_t) src;

	/* Use a clock A source? */
	if (src >= SYSCON_CLKOUTSRCA_OUTPUT) {
		/* Not using a CLKOUT A source */
		LPC_SYSCON->CLKOUTSELB = srcClk - SYSCON_CLKOUTSRCA_OUTPUT;
	}
	else {
		/* Using a clock A source, select A and then switch B to A */
		LPC_SYSCON->CLKOUTSELA = srcClk;
		LPC_SYSCON->CLKOUTSELB = 0;
	}

	LPC_SYSCON->CLKOUTDIV = div;
}

/* Enable a system or peripheral clock */
void Chip_Clock_EnablePeriphClock(CHIP_SYSCON_CLOCK_T clk)
{
	uint32_t clkEnab = (uint32_t) clk;

	if (clkEnab >= 128) {
		clkEnab = clkEnab - 128;

		LPC_ASYNC_SYSCON->ASYNCAPBCLKCTRLSET = (1 << clkEnab);
	}
	else if (clkEnab >= 32) {
		LPC_SYSCON->AHBCLKCTRLSET[1] = (1 << (clkEnab - 32));
	}
	else {
		LPC_SYSCON->AHBCLKCTRLSET[0] = (1 << clkEnab);
	}
}

/* Disable a system or peripheral clock */
void Chip_Clock_DisablePeriphClock(CHIP_SYSCON_CLOCK_T clk)
{
	uint32_t clkEnab = (uint32_t) clk;

	if (clkEnab >= 128) {
		clkEnab = clkEnab - 128;

		LPC_ASYNC_SYSCON->ASYNCAPBCLKCTRLCLR = (1 << clkEnab);
	}
	else if (clkEnab >= 32) {
		LPC_SYSCON->AHBCLKCTRLCLR[1] = (1 << (clkEnab - 32));
	}
	else {
		LPC_SYSCON->AHBCLKCTRLCLR[0] = (1 << clkEnab);
	}
}

/* Returns the system tick rate as used with the system tick divider */
uint32_t Chip_Clock_GetSysTickClockRate(void)
{
	uint32_t sysRate, div;

	div = LPC_SYSCON->SYSTICKCLKDIV;

	/* If divider is 0, the system tick clock is disabled */
	if (div == 0) {
		sysRate = 0;
	}
	else {
		sysRate = Chip_Clock_GetSystemClockRate() / LPC_SYSCON->SYSTICKCLKDIV;
	}

	return sysRate;
}

/* Return ADC clock rate */
uint32_t Chip_Clock_GetADCClockRate(void)
{
	uint32_t div, clkRate = 0;

	div = Chip_Clock_GetADCClockDiv();

	/* ADC clock only enabled if div>0 */
	if (div > 0) {
		switch (Chip_Clock_GetADCClockSource()) {
		case SYSCON_ADCCLKSELSRC_MAINCLK:
			clkRate = Chip_Clock_GetMainClockRate();
			break;

		case SYSCON_ADCCLKSELSRC_SYSPLLOUT:
			clkRate = Chip_Clock_GetSystemPLLOutClockRate(false);
			break;

		case SYSCON_ADCCLKSELSRC_IRC:
			clkRate = Chip_Clock_GetIntOscRate();
			break;
		}

		clkRate = clkRate / div;
	}

	return clkRate;
}

/* Set asynchronous APB clock source */
void Chip_Clock_SetAsyncSysconClockSource(CHIP_ASYNC_SYSCON_SRC_T src)
{
	uint32_t clkSrc = (uint32_t) src;

	if (src >= SYSCON_ASYNC_MAINCLK) {
		LPC_ASYNC_SYSCON->ASYNCAPBCLKSELB = (clkSrc - 4);
	}
	else {
		LPC_ASYNC_SYSCON->ASYNCAPBCLKSELA = clkSrc;
		LPC_ASYNC_SYSCON->ASYNCAPBCLKSELB = 3;
	}
}

/* Get asynchronous APB clock source */
CHIP_ASYNC_SYSCON_SRC_T Chip_Clock_GetAsyncSysconClockSource(void)
{
	uint32_t clkSrc;

	if (LPC_ASYNC_SYSCON->ASYNCAPBCLKSELB == 3) {
		clkSrc = LPC_ASYNC_SYSCON->ASYNCAPBCLKSELA;
	}
	else {
		clkSrc = 4 + LPC_ASYNC_SYSCON->ASYNCAPBCLKSELB;
	}

	return (CHIP_ASYNC_SYSCON_SRC_T) clkSrc;
}

/* Return asynchronous APB clock rate */
uint32_t Chip_Clock_GetAsyncSyscon_ClockRate(void)
{
	uint32_t clkRate, div;

	clkRate = Chip_Clock_GetAsyncSyscon_ClockRate_NoDiv();
	div = LPC_ASYNC_SYSCON->ASYNCCLKDIV;
	if (div == 0) {
		/* Clock is disabled */
		return 0;
	}

	return clkRate / div;
}

/* Set main system clock source */
void Chip_Clock_SetMainClockSource(CHIP_SYSCON_MAINCLKSRC_T src)
{
	uint32_t clkSrc = (uint32_t) src;

	if (clkSrc >= 4) {
		/* Main B source only, not using main A */
		Chip_Clock_SetMain_B_ClockSource((CHIP_SYSCON_MAIN_B_CLKSRC_T) (clkSrc - 4));
	}
	else {
		/* Select main A clock source and set main B source to use main A */
		Chip_Clock_SetMain_A_ClockSource((CHIP_SYSCON_MAIN_A_CLKSRC_T) clkSrc);
		Chip_Clock_SetMain_B_ClockSource(SYSCON_MAIN_B_CLKSRC_MAINCLKSELA);
	}
}

/* Returns the main clock source */
CHIP_SYSCON_MAINCLKSRC_T Chip_Clock_GetMainClockSource(void)
{
	CHIP_SYSCON_MAIN_B_CLKSRC_T srcB;
	uint32_t clkSrc;

	/* Get main B clock source */
	srcB = Chip_Clock_GetMain_B_ClockSource();
	if (srcB == SYSCON_MAIN_B_CLKSRC_MAINCLKSELA) {
		/* Using source A, so return source A */
		clkSrc = (uint32_t) Chip_Clock_GetMain_A_ClockSource();
	}
	else {
		/* Using source B */
		clkSrc = 4 + (uint32_t) srcB;
	}

	return (CHIP_SYSCON_MAINCLKSRC_T) clkSrc;
}

/* Return main clock rate */
uint32_t Chip_Clock_GetMainClockRate(void)
{
	uint32_t clkRate;

	if (Chip_Clock_GetMain_B_ClockSource() == SYSCON_MAIN_B_CLKSRC_MAINCLKSELA) {
		/* Return main A clock rate */
		clkRate = Chip_Clock_GetMain_A_ClockRate();
	}
	else {
		/* Return main B clock rate */
		clkRate = Chip_Clock_GetMain_B_ClockRate();
	}

	return clkRate;
}

/* Return system clock rate */
uint32_t Chip_Clock_GetSystemClockRate(void)
{
	/* No point in checking for divide by 0 */
	return Chip_Clock_GetMainClockRate() / LPC_SYSCON->AHBCLKDIV;
}

/* Get UART base rate */
uint32_t Chip_Clock_GetUARTBaseClockRate(void)
{
	uint64_t inclk;

	/* Get clock rate into FRG */
	inclk = (uint64_t) Chip_Clock_GetAsyncSyscon_ClockRate();

	if (inclk != 0) {
		uint32_t mult, divmult;

		divmult = LPC_ASYNC_SYSCON->FRGCTRL & 0xFF;
		if ((divmult & 0xFF) == 0xFF) {
			/* Fractional part is enabled, get multiplier */
			mult = (divmult >> 8) & 0xFF;

			/* Get fractional error */
			inclk = (inclk * 256) / (uint64_t) (256 + mult);
		}
	}

	return (uint32_t) inclk;
}

/* Set UART base rate */
uint32_t Chip_Clock_SetUARTBaseClockRate(uint32_t rate)
{
	uint32_t div, inclk, err;
	uint64_t uart_fra_multiplier;

	/* Input clock into FRG block is the main system cloock */
	inclk = Chip_Clock_GetAsyncSyscon_ClockRate();

	/* Get integer divider for coarse rate */
	div = inclk / rate;
	if (div == 0) {
		div = 1;
	}

	/* Enable FRG clock */
	Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_FRG);

	err = inclk - (rate * div);
	uart_fra_multiplier = (((uint64_t) err + (uint64_t) rate) * 256) / (uint64_t) (rate * div);

	/* Enable fractional divider and set multiplier */
	LPC_ASYNC_SYSCON->FRGCTRL = 0xFF | (uart_fra_multiplier << 8);

	return Chip_Clock_GetUARTBaseClockRate();
}
