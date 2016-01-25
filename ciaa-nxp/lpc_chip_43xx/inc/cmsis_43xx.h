/*
 * @brief Basic CMSIS include file for LPC43XX
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

#ifndef __CMSIS_43XX_M0_H_
#define __CMSIS_43XX_M0_H_

#ifndef __CMSIS_H_
#error "cmsis_43xx.h should not be included directly use cmsis.h instead"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup CMSIS_43XX CHIP: LPC43xx CMSIS include file
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

#if defined(__ARMCC_VERSION)
  #pragma diag_suppress 2525
  #pragma push
  #pragma anon_unions
#elif defined(__CWCC__)
  #pragma push
  #pragma cpp_extensions on
#elif defined(__GNUC__)
/* anonymous unions are enabled by default */
#elif defined(__IAR_SYSTEMS_ICC__)
  #pragma language=extended
#else
  #error Not supported compiler type
#endif

/** @defgroup CMSIS_43XX_COMMON CHIP: LPC43xx Cortex CMSIS definitions
 * @{
 */

#define __CM4_REV              0x0001		/*!< Cortex-M4 Core Revision               */
#define __MPU_PRESENT             1			/*!< MPU present or not                    */
#define __NVIC_PRIO_BITS          3			/*!< Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0			/*!< Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT             1			/*!< FPU present or not                    */

/**
 * @}
 */

/** @defgroup CMSIS_43XX_IRQ CHIP: LPC43xx peripheral interrupt numbers
 * @{
 */

typedef enum {
	/* -------------------------  Cortex-M4 Processor Exceptions Numbers  ----------------------------- */
	Reset_IRQn                        = -15,/*!<   1  Reset Vector, invoked on Power up and warm reset */
	NonMaskableInt_IRQn               = -14,/*!<   2  Non maskable Interrupt, cannot be stopped or preempted */
	HardFault_IRQn                    = -13,/*!<   3  Hard Fault, all classes of Fault */
	MemoryManagement_IRQn             = -12,/*!<   4  Memory Management, MPU mismatch, including Access Violation and No Match */
	BusFault_IRQn                     = -11,/*!<   5  Bus Fault, Pre-Fetch-, Memory Access Fault, other address/memory related Fault */
	UsageFault_IRQn                   = -10,/*!<   6  Usage Fault, i.e. Undef Instruction, Illegal State Transition */
	SVCall_IRQn                       =  -5,/*!<  11  System Service Call via SVC instruction */
	DebugMonitor_IRQn                 =  -4,/*!<  12  Debug Monitor                    */
	PendSV_IRQn                       =  -2,/*!<  14  Pendable request for system service */
	SysTick_IRQn                      =  -1,/*!<  15  System Tick Timer                */

	/* ---------------------------  LPC18xx/43xx Specific Interrupt Numbers  ------------------------------- */
	DAC_IRQn                          =   0,/*!<   0  DAC                              */
	M0APP_IRQn                        =   1,/*!<   1  M0APP Core interrupt             */
	DMA_IRQn                          =   2,/*!<   2  DMA                              */
	RESERVED1_IRQn                    =   3,/*!<   3  EZH/EDM                          */
	RESERVED2_IRQn                    =   4,
	ETHERNET_IRQn                     =   5,/*!<   5  ETHERNET                         */
	SDIO_IRQn                         =   6,/*!<   6  SDIO                             */
	LCD_IRQn                          =   7,/*!<   7  LCD                              */
	USB0_IRQn                         =   8,/*!<   8  USB0                             */
	USB1_IRQn                         =   9,/*!<   9  USB1                             */
	SCT_IRQn                          =  10,/*!<  10  SCT                              */
	RITIMER_IRQn                      =  11,/*!<  11  RITIMER                          */
	TIMER0_IRQn                       =  12,/*!<  12  TIMER0                           */
	TIMER1_IRQn                       =  13,/*!<  13  TIMER1                           */
	TIMER2_IRQn                       =  14,/*!<  14  TIMER2                           */
	TIMER3_IRQn                       =  15,/*!<  15  TIMER3                           */
	MCPWM_IRQn                        =  16,/*!<  16  MCPWM                            */
	ADC0_IRQn                         =  17,/*!<  17  ADC0                             */
	I2C0_IRQn                         =  18,/*!<  18  I2C0                             */
	I2C1_IRQn                         =  19,/*!<  19  I2C1                             */
	SPI_INT_IRQn                      =  20,/*!<  20  SPI_INT                          */
	ADC1_IRQn                         =  21,/*!<  21  ADC1                             */
	SSP0_IRQn                         =  22,/*!<  22  SSP0                             */
	SSP1_IRQn                         =  23,/*!<  23  SSP1                             */
	USART0_IRQn                       =  24,/*!<  24  USART0                           */
	UART1_IRQn                        =  25,/*!<  25  UART1                            */
	USART2_IRQn                       =  26,/*!<  26  USART2                           */
	USART3_IRQn                       =  27,/*!<  27  USART3                           */
	I2S0_IRQn                         =  28,/*!<  28  I2S0                             */
	I2S1_IRQn                         =  29,/*!<  29  I2S1                             */
	RESERVED4_IRQn                    =  30,
	SGPIO_INT_IRQn                    =  31,/*!<  31  SGPIO_IINT                       */
	PIN_INT0_IRQn                     =  32,/*!<  32  PIN_INT0                         */
	PIN_INT1_IRQn                     =  33,/*!<  33  PIN_INT1                         */
	PIN_INT2_IRQn                     =  34,/*!<  34  PIN_INT2                         */
	PIN_INT3_IRQn                     =  35,/*!<  35  PIN_INT3                         */
	PIN_INT4_IRQn                     =  36,/*!<  36  PIN_INT4                         */
	PIN_INT5_IRQn                     =  37,/*!<  37  PIN_INT5                         */
	PIN_INT6_IRQn                     =  38,/*!<  38  PIN_INT6                         */
	PIN_INT7_IRQn                     =  39,/*!<  39  PIN_INT7                         */
	GINT0_IRQn                        =  40,/*!<  40  GINT0                            */
	GINT1_IRQn                        =  41,/*!<  41  GINT1                            */
	EVENTROUTER_IRQn                  =  42,/*!<  42  EVENTROUTER                      */
	C_CAN1_IRQn                       =  43,/*!<  43  C_CAN1                           */
	RESERVED6_IRQn                    =  44,
	ADCHS_IRQn                        =  45,/*!<  45  ADCHS interrupt                  */
	ATIMER_IRQn                       =  46,/*!<  46  ATIMER                           */
	RTC_IRQn                          =  47,/*!<  47  RTC                              */
	RESERVED8_IRQn                    =  48,
	WWDT_IRQn                         =  49,/*!<  49  WWDT                             */
	M0SUB_IRQn                        =  50,/*!<  50  M0SUB core interrupt             */
	C_CAN0_IRQn                       =  51,/*!<  51  C_CAN0                           */
	QEI_IRQn                          =  52,/*!<  52  QEI                              */
} LPC43XX_IRQn_Type;

/**
 * @}
 */

typedef LPC43XX_IRQn_Type IRQn_Type;

/* Cortex-M4 processor and core peripherals */
#include "core_cm4.h"

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* ifndef __CMSIS_43XX_M0_H_ */
