/*
 * @brief Basic CMSIS include file for LPC5410x M0+ core
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

#ifndef __CMSIS_5410X_M0_H_
#define __CMSIS_5410X_M0_H_

#include "lpc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup CMSIS_5410X_M0 CHIP: LPC5410X M0 core CMSIS include file
 * @ingroup CHIP_5410X_CMSIS_DRIVERS
 * @{
 */

#if defined(__ARMCC_VERSION)
// Kill warning "#pragma push with no matching #pragma pop"
  #pragma diag_suppress 2525
  #pragma push
  #pragma anon_unions
#elif defined(__CWCC__)
  #pragma push
  #pragma cpp_extensions on
#elif defined(__GNUC__)
/* anonymous unions are enabled by default */
#elif defined(__IAR_SYSTEMS_ICC__)
//  #pragma push // FIXME not usable for IAR
  #pragma language=extended
#else
  #error Not supported compiler type
#endif

/*
 * ==========================================================================
 * ---------- Interrupt Number Definition -----------------------------------
 * ==========================================================================
 */

#if !defined(CORE_M0PLUS)
#error "CORE_M0PLUS is not defined"
#endif

/** @defgroup CMSIS_5410X_M0_IRQ CHIP_5410X: LPC5410X M0 core peripheral interrupt numbers
 * @{
 */

typedef enum {
	/******  Cortex-M0 Processor Exceptions Numbers ***************************************************/
	Reset_IRQn                    = -15,	/*!< 1 Reset Vector, invoked on Power up and warm reset */
	NonMaskableInt_IRQn           = -14,	/*!< 2 Non Maskable Interrupt                           */
	HardFault_IRQn                = -13,	/*!< 3 Cortex-M0 Hard Fault Interrupt                   */
	SVCall_IRQn                   = -5,		/*!< 11 Cortex-M0 SV Call Interrupt                     */
	PendSV_IRQn                   = -2,		/*!< 14 Cortex-M0 Pend SV Interrupt                     */
	SysTick_IRQn                  = -1,		/*!< 15 Cortex-M0 System Tick Interrupt                 */

	/******  LPC5410X Specific Interrupt Numbers ********************************************************/
	WDT_IRQn                      = 0,		/*!< WWDT                                             */
	BOD_IRQn                      = 1,		/*!< BOD                                              */
	Reserved2_IRQn                = 2,		/*!< Reserved Interrupt                               */
	DMA_IRQn                      = 3,		/*!< DMA                                              */
	GINT0_IRQn                    = 4,		/*!< GINT0                                            */
	PIN_INT0_IRQn                 = 5,		/*!< PININT0                                          */
	PIN_INT1_IRQn                 = 6,		/*!< PININT1                                          */
	PIN_INT2_IRQn                 = 7,		/*!< PININT2                                          */
	PIN_INT3_IRQn                 = 8,		/*!< PININT3                                          */
	UTICK_IRQn                    = 9,		/*!< Micro-tick Timer interrupt                       */
	MRT_IRQn                      = 10,		/*!< Multi-rate timer interrupt                       */
	CT32B0_IRQn                   = 11,		/*!< CTMR0                                            */
	CT32B1_IRQn                   = 12,		/*!< CTMR1                                            */
	CT32B2_IRQn                   = 13,		/*!< CTMR2                                            */
	CT32B3_IRQn                   = 14,		/*!< CTMR3                                            */
	CT32B4_IRQn                   = 15,		/*!< CTMR4                                            */
	SCT0_IRQn                     = 16,		/*!< SCT                                              */
	UART0_IRQn                    = 17,		/*!< UART0                                            */
	UART1_IRQn                    = 18,		/*!< UART1                                            */
	UART2_IRQn                    = 19,		/*!< UART2                                            */
	UART3_IRQn                    = 20,		/*!< UART3                                            */
	I2C0_IRQn                     = 21,		/*!< I2C0                                             */
	I2C1_IRQn                     = 22,		/*!< I2C1                                             */
	I2C2_IRQn                     = 23,		/*!< I2C2                                             */
	SPI0_IRQn                     = 24,		/*!< SPI0                                             */
	SPI1_IRQn                     = 25,		/*!< SPI1                                             */
	ADC_SEQA_IRQn                 = 26,		/*!< ADC0 sequence A completion                       */
	ADC_SEQB_IRQn                 = 27,		/*!< ADC0 sequence B completion                       */
	ADC_THCMP_IRQn                = 28,		/*!< ADC0 threshold compare and error                 */
	RTC_IRQn                      = 29,		/*!< RTC alarm and wake-up interrupts                 */
	Reserved30_IRQn               = 30,		/*!< Reserved Interrupt                               */
	MAILBOX_IRQn                  = 31,		/*!< Mailbox                                          */
} LPC5410X_M0_IRQn_Type;

/**
 * @}
 */

/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/** @defgroup CMSIS_5410X_M0_COMMON CHIP: LPC5410X M0 core Cortex CMSIS definitions
 * @{
 */

/* Configuration of the Cortex-M0+ Processor and Core Peripherals */
#define __CM0PLUS_REV             0x0001	/*!< Cortex-M0PLUS Core Revision                      */
#define __MPU_PRESENT             0			/*!< MPU present or not                               */
#define __NVIC_PRIO_BITS          2			/*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0			/*!< Set to 1 if different SysTick Config is used     */
#define __VTOR_PRESENT            1

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __CMSIS_5410X_M0_H_ */
