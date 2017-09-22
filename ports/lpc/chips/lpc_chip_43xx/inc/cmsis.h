/*
 * @brief LPC11xx selective CMSIS inclusion file
 *
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
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
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __CMSIS_H_
#define __CMSIS_H_

#include "lpc_types.h"
#include "sys_config.h"

/* Select correct CMSIS include file based on CHIP_* definition */
#if defined(CHIP_LPC43XX)

#ifdef CORE_M4
#include "cmsis_43xx.h"

#elif defined(CORE_M0)
#if defined(LPC43XX_CORE_M0APP)
#include "cmsis_43xx_m0app.h"

#elif (defined(LPC43XX_CORE_M0SUB))
#include "cmsis_43xx_m0sub.h"

#else
#error "LPC43XX_CORE_M0APP or LPC43XX_CORE_M0SUB must be defined"
#endif

#else
#error "CORE_M0 or CORE_M4 must be defined for CHIP_LPC43XX"
#endif

#elif defined(CHIP_LPC18XX)
#include "cmsis_18xx.h"

#else
#error "No CHIP_* definition is defined"
#endif

#endif /* __CMSIS_H_ */






