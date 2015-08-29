/*
 * @brief FPU init code
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

#if defined(CORE_M4)

#include "sys_config.h"
#include "cmsis.h"
#include "stdint.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define LPC_CPACR           0xE000ED88

#define SCB_MVFR0           0xE000EF40
#define SCB_MVFR0_RESET     0x10110021

#define SCB_MVFR1           0xE000EF44
#define SCB_MVFR1_RESET     0x11000011

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Early initialization of the FPU */
void fpuInit(void)
{
#if __FPU_PRESENT != 0
	// from arm trm manual:
	//                ; CPACR is located at address 0xE000ED88
	//                LDR.W R0, =0xE000ED88
	//                ; Read CPACR
	//                LDR R1, [R0]
	//                ; Set bits 20-23 to enable CP10 and CP11 coprocessors
	//                ORR R1, R1, #(0xF << 20)
	//                ; Write back the modified value to the CPACR
	//                STR R1, [R0]

	volatile uint32_t *regCpacr = (uint32_t *) LPC_CPACR;
	volatile uint32_t *regMvfr0 = (uint32_t *) SCB_MVFR0;
	volatile uint32_t *regMvfr1 = (uint32_t *) SCB_MVFR1;
	volatile uint32_t Cpacr;
	volatile uint32_t Mvfr0;
	volatile uint32_t Mvfr1;
	char vfpPresent = 0;

	Mvfr0 = *regMvfr0;
	Mvfr1 = *regMvfr1;

	vfpPresent = ((SCB_MVFR0_RESET == Mvfr0) && (SCB_MVFR1_RESET == Mvfr1));

	if (vfpPresent) {
		Cpacr = *regCpacr;
		Cpacr |= (0xF << 20);
		*regCpacr = Cpacr;	// enable CP10 and CP11 for full access
	}
#endif /* __FPU_PRESENT != 0 */
}

#endif /* defined(CORE_M4 */
