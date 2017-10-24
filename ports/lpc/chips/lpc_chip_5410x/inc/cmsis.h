/*
 * @brief LPC5410x selective CMSIS inclusion file
 *
 * Copyright(C) NXP Semiconductors, 2014
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

#ifdef __cplusplus
extern "C" {
#endif

/* Select correct CMSIS include file based on CORE_* definition */
#if defined(CORE_M4)
#include "cmsis_5410x.h"
typedef LPC5410X_IRQn_Type IRQn_Type;
#include "core_cm4.h"					/*!< Cortex-M4 processor and core peripherals      */
#elif defined(CORE_M0PLUS)
#include "cmsis_5410x_m0.h"
typedef LPC5410X_M0_IRQn_Type IRQn_Type;
#include "core_cm0plus.h"				/*!< Cortex-M0 Plus processor and core peripherals  */
#else
#error "No CORE_* definition is defined"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __CMSIS_H_ */
