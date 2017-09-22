/*
 * @brief LPC18xx/LPC43xx chip driver source
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
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
/* USB PLL pre-initialized setup values for 480MHz output rate */
static const CGU_USBAUDIO_PLL_SETUP_T usbPLLSetup = {
	0x0000601D,	/* Default control with main osc input, PLL disabled */
	0x06167FFA,	/* M-divider value for 480MHz output from 12MHz input */
	0x00000000,	/* N-divider value */
	0x00000000,	/* Not applicable for USB PLL */
	480000000   /* PLL output frequency */
};

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
/* System Clock Frequency (Core Clock) */
uint32_t SystemCoreClock;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

static void Chip_USB_PllSetup(void)
{
	/* No need to setup anything if PLL is already setup for the frequency */
	if (Chip_Clock_GetClockInputHz(CLKIN_USBPLL) == usbPLLSetup.freq)
		return ;

	/* Setup default USB PLL state for a 480MHz output and attach */
	Chip_Clock_SetupPLL(CLKIN_CRYSTAL, CGU_USB_PLL, &usbPLLSetup);

	/* enable USB PLL */
	Chip_Clock_EnablePLL(CGU_USB_PLL);

	/* Wait for PLL lock */
	while (!(Chip_Clock_GetPLLStatus(CGU_USB_PLL) & CGU_PLL_LOCKED)) {}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

void Chip_USB0_Init(void)
{
	/* Set up USB PLL */
	Chip_USB_PllSetup();

	/* Setup USB0 base clock as clock out from USB PLL */
	Chip_Clock_SetBaseClock( CLK_BASE_USB0, CLKIN_USBPLL, true, true);

	/* enable USB main clock */
	Chip_Clock_EnableBaseClock(CLK_BASE_USB0);
	Chip_Clock_EnableOpts(CLK_MX_USB0, true, true, 1);
	/* enable USB0 phy */
	Chip_CREG_EnableUSB0Phy();
}

void Chip_USB1_Init(void)
{
	/* Setup and enable the PLL */
	Chip_USB_PllSetup();

	/* USB1 needs a 60MHz clock. To get it, a divider of 4 and then 2 are
	   chained to make a divide by 8 function. Connect the output of
	   divider D to the USB1 base clock. */
	Chip_Clock_SetDivider(CLK_IDIV_A, CLKIN_USBPLL, 4);
	Chip_Clock_SetDivider(CLK_IDIV_D, CLKIN_IDIVA, 2);
	Chip_Clock_SetBaseClock(CLK_BASE_USB1, CLKIN_IDIVD, true, true);

	/* enable USB main clock */
	Chip_Clock_EnableBaseClock(CLK_BASE_USB1);
	Chip_Clock_EnableOpts(CLK_MX_USB1, true, true, 1);
	/* enable USB1_DP and USB1_DN on chip FS phy.*/
	LPC_SCU->SFSUSB = 0x12;
}


/* Update system core clock rate, should be called if the system has
   a clock rate change */
void SystemCoreClockUpdate(void)
{
	/* CPU core speed */
	SystemCoreClock = Chip_Clock_GetRate(CLK_MX_MXCORE);
}






