/*
 * @brief LPC5410X ADC driver
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

/* Initialize the ADC peripheral */
void Chip_ADC_Init(LPC_ADC_T *pADC, uint32_t flags)
{
	/* Power up ADC and enable ADC base clock */
	Chip_SYSCON_PowerUp(SYSCON_PDRUNCFG_PD_ADC0 | SYSCON_PDRUNCFG_PD_VDDA_ENA | SYSCON_PDRUNCFG_PD_VREFP);
	Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_ADC0);

	/* Disable ADC interrupts */
	pADC->INTEN = 0;

	/* Set ADC control options */
	pADC->CTRL = flags;
}

/* Shutdown ADC */
void Chip_ADC_DeInit(LPC_ADC_T *pADC)
{
	pADC->INTEN = 0;
	pADC->CTRL = 0;

	/* Stop ADC clock and then power down ADC */
	Chip_Clock_DisablePeriphClock(SYSCON_CLOCK_ADC0);
	Chip_SYSCON_PowerDown(SYSCON_PDRUNCFG_PD_ADC0 | SYSCON_PDRUNCFG_PD_VDDA_ENA | SYSCON_PDRUNCFG_PD_VREFP);
}

/* Calibrate ADC for system clock frequency */
uint32_t Chip_ADC_Calibration(LPC_ADC_T *pADC)
{
	volatile uint32_t i;
	uint32_t sysclk_freq = Chip_Clock_GetSystemClockRate();

	pADC->STARTUP = ADC_STARTUP_ENABLE;
	for ( i = 0; i < 0x10; i++ ) {}
	if ( !(pADC->STARTUP & ADC_STARTUP_ENABLE) ) {
		return ERR_ADC_NO_POWER;
	}

	/* If not in by-pass mode do the calibration */
	if ( (pADC->CALIBR & ADC_CALREQD) && !(pADC->CTRL & ADC_CR_BYPASS) ) {
		uint32_t ctrl = pADC->CTRL & 0x7FFF;
		uint32_t tmp = ctrl;

		/* Set ADC to SYNC mode */
		tmp &= ~ADC_CR_ASYNC_MODE;

		/* To be safe run calibration at 1MHz UM permits upto 30MHz */
		if (sysclk_freq > 1000000UL) {
			pADC->CTRL = tmp | (((sysclk_freq / 1000000UL) - 1) & 0xFF);
		}

		/* Calibration is needed, do it now. */
		pADC->CALIBR = ADC_CALIB;
		i = 0xF0000;
		while ( (pADC->CALIBR & ADC_CALIB) && --i ) {}
		pADC->CTRL = ctrl;
		return i ? LPC_OK : ERR_TIME_OUT;
	}

	/* A dummy conversion cycle will be performed. */
	pADC->STARTUP = (pADC->STARTUP | ADC_STARTUP_INIT) & 0x03;
	i = 0x7FFFF;
	while ( (pADC->STARTUP & ADC_STARTUP_INIT) && --i ) {}
	return i ? LPC_OK : ERR_TIME_OUT;
}

/* Set ADC clock rate */
void Chip_ADC_SetClockRate(LPC_ADC_T *pADC, uint32_t rate)
{
	uint32_t div;

	/* Get ADC clock source to determine base ADC rate. IN sychronous mode,
	   the ADC base clock comes from the system clock. In ASYNC mode, it
	   comes from the ASYNC ADC clock and this function doesn't work. */
	div = Chip_Clock_GetSystemClockRate() / rate;
	if (div == 0) {
		div = 1;
	}

	Chip_ADC_SetDivider(pADC, (uint8_t) div - 1);
}
