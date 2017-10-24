/*
 * @brief LPC5410x basic chip inclusion file
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

#ifndef __CHIP_H_
#define __CHIP_H_

#include "lpc_types.h"
#include "cmsis.h"

#ifdef __cplusplus
extern "C" {
#endif

/* LPCXpresso macro LPCOpen macro defines */
#ifdef __LPC5410X__
#define CHIP_LPC5410X
#endif

#ifndef CORE_M4
#ifndef CORE_M0PLUS
#error "CORE_M4 or CORE_M0PLUS is not defined for the LPC5410x architecture"
#error "CORE_M4 or CORE_M0PLUS should be defined as part of your compiler define list"
#endif
#endif

#ifndef CHIP_LPC5410X
#error "The LPC5410X Chip include path is used for this build, but"
#error "CHIP_LPC5410X is not defined!"
#endif

/** @defgroup PERIPH_5410X_BASE CHIP: LPC5410X Peripheral addresses and register set declarations
 * @ingroup CHIP_5410X_DRIVERS
 * @{
 */

/* Main memory addresses */
#define LPC_FLASHMEM_BASE          0x00000000UL
#define LPC_SRAM0_BASE             0x02000000UL
#define LPC_SRAM1_BASE             0x02010000UL
#define LPC_ROM_BASE               0x03000000UL
#define LPC_SRAM2_BASE             0x03400000UL
#define LPC_GPIO_PORT_BASE         0x1C000000UL
#define LPC_DMA_BASE               0x1C004000UL
#define LPC_CRC_BASE               0x1C010000UL
#define LPC_SCT_BASE               0x1C018000UL
#define LPC_MBOX_BASE              0x1C02C000UL
#define LPC_ADC_BASE               0x1C034000UL
#define LPC_FIFO_BASE              0x1C038000UL

/* APB0 peripheral group addresses */
#define LPC_SYSCON_BASE            0x40000000UL
#define LPC_TIMER2_BASE            0x40004000UL
#define LPC_TIMER3_BASE            0x40008000UL
#define LPC_TIMER4_BASE            0x4000C000UL
#define LPC_GPIO_GROUPINT0_BASE    0x40010000UL
#define LPC_GPIO_GROUPINT1_BASE    0x40014000UL
#define LPC_PIN_INT_BASE           0x40018000UL
#define LPC_IOCON_BASE             0x4001C000UL
#define LPC_UTICK_BASE             0x40020000UL
#define LPC_FMC_BASE               0x40024000UL
#define LPC_PMU_BASE               0x4002C000UL
#define LPC_WWDT_BASE              0x40038000UL
#define LPC_RTC_BASE               0x4003C000UL

/* APB1 peripheral group addresses */
#define LPC_ASYNC_SYSCON_BASE      0x40080000UL
#define LPC_USART0_BASE            0x40084000UL
#define LPC_USART1_BASE            0x40088000UL
#define LPC_USART2_BASE            0x4008C000UL
#define LPC_USART3_BASE            0x40090000UL
#define LPC_I2C0_BASE              0x40094000UL
#define LPC_I2C1_BASE              0x40098000UL
#define LPC_I2C2_BASE              0x4009C000UL
#define LPC_SPI0_BASE              0x400A4000UL
#define LPC_SPI1_BASE              0x400A8000UL
#define LPC_TIMER0_BASE            0x400B4000UL
#define LPC_TIMER1_BASE            0x400B8000UL
#define LPC_INMUX_BASE             0x40050000UL
#define LPC_RITIMER_BASE           0x40070000UL
#define LPC_MRT_BASE               0x40074000UL

/* Main memory register access */
#define LPC_GPIO           ((LPC_GPIO_T            *) LPC_GPIO_PORT_BASE)
#define LPC_DMA            ((LPC_DMA_T             *) LPC_DMA_BASE)
#define LPC_CRC            ((LPC_CRC_T             *) LPC_CRC_BASE)
#define LPC_SCT            ((LPC_SCT_T             *) LPC_SCT_BASE)
#define LPC_MBOX           ((LPC_MBOX_T            *) LPC_MBOX_BASE)
#define LPC_ADC            ((LPC_ADC_T             *) LPC_ADC_BASE)
#define LPC_FIFO           ((LPC_FIFO_T            *) LPC_FIFO_BASE)

/* APB0 peripheral group register access */
#define LPC_SYSCON         ((LPC_SYSCON_T          *) LPC_SYSCON_BASE)
#define LPC_TIMER2         ((LPC_TIMER_T           *) LPC_TIMER2_BASE)
#define LPC_TIMER3         ((LPC_TIMER_T           *) LPC_TIMER3_BASE)
#define LPC_TIMER4         ((LPC_TIMER_T           *) LPC_TIMER4_BASE)
#define LPC_GINT           ((LPC_GPIOGROUPINT_T    *) LPC_GPIO_GROUPINT0_BASE)
#define LPC_PININT         ((LPC_PIN_INT_T         *) LPC_PIN_INT_BASE)
#define LPC_IOCON          ((LPC_IOCON_T           *) LPC_IOCON_BASE)
#define LPC_UTICK          ((LPC_UTICK_T           *) LPC_UTICK_BASE)
#define LPC_WWDT           ((LPC_WWDT_T            *) LPC_WWDT_BASE)
#define LPC_RTC            ((LPC_RTC_T             *) LPC_RTC_BASE)

/* APB1 peripheral group register access */
#define LPC_ASYNC_SYSCON   ((LPC_ASYNC_SYSCON_T    *) LPC_ASYNC_SYSCON_BASE)
#define LPC_USART0         ((LPC_USART_T           *) LPC_USART0_BASE)
#define LPC_USART1         ((LPC_USART_T           *) LPC_USART1_BASE)
#define LPC_USART2         ((LPC_USART_T           *) LPC_USART2_BASE)
#define LPC_USART3         ((LPC_USART_T           *) LPC_USART3_BASE)
#define LPC_I2C0           ((LPC_I2C_T             *) LPC_I2C0_BASE)
#define LPC_I2C1           ((LPC_I2C_T             *) LPC_I2C1_BASE)
#define LPC_I2C2           ((LPC_I2C_T             *) LPC_I2C2_BASE)
#define LPC_SCT0           LPC_SCT
#define LPC_SPI0           ((LPC_SPI_T             *) LPC_SPI0_BASE)
#define LPC_SPI1           ((LPC_SPI_T             *) LPC_SPI1_BASE)
#define LPC_TIMER0         ((LPC_TIMER_T           *) LPC_TIMER0_BASE)
#define LPC_TIMER1         ((LPC_TIMER_T           *) LPC_TIMER1_BASE)
#define LPC_INMUX          ((LPC_INMUX_T           *) LPC_INMUX_BASE)
#define LPC_RITIMER        ((LPC_RITIMER_T         *) LPC_RITIMER_BASE)
#define LPC_MRT            ((LPC_MRT_T             *) LPC_MRT_BASE)
#define LPC_PMU            ((LPC_PMU_T             *) LPC_PMU_BASE)

/**
 * @}
 */

/** @ingroup CHIP_5410X_DRIVER_OPTIONS
 * @{
 */

/**
 * @brief	Clock rate on the CLKIN pin
 * This value is defined externally to the chip layer and contains
 * the value in Hz for the CLKIN pin for the board. If this pin isn't used,
 * this rate can be 0.
 */
extern const uint32_t ExtClockIn;

/**
 * @}
 */

/* Include order is important! */
#include "romapi_5410x.h"
#include "syscon_5410x.h"
#include "cpuctrl_5410x.h"
#include "clock_5410x.h"
#include "pmu_5410x.h"
#include "iocon_5410x.h"
#include "pinint_5410x.h"
#include "inmux_5410x.h"
#include "crc_5410x.h"
#include "gpio_5410x.h"
#include "fifo_5410x.h"
#include "mrt_5410x.h"
#include "wwdt_5410x.h"
#include "sct_5410x.h"
#include "sct_pwm_5410x.h"
#include "rtc_5410x.h"
#include "timer_5410x.h"
#include "ritimer_5410x.h"
#include "utick_5410x.h"
#include "gpiogroup_5410x.h"
#include "mailbox_5410x.h"
#include "fpu_init.h"
#include "power_lib_5410x.h"
#include "adc_5410x.h"
#include "dma_5410x.h"
#include "uart_5410x.h"
#include "spi_common_5410x.h"
#include "spim_5410x.h"
#include "spis_5410x.h"
#include "i2c_common_5410x.h"
#include "i2cm_5410x.h"
#include "i2cs_5410x.h"

/** @defgroup SUPPORT_5410X_FUNC CHIP: LPC5410X support functions
 * @ingroup CHIP_5410X_DRIVERS
 * @{
 */

/**
 * @brief	Current system clock rate, mainly used for peripherals in SYSCON
 */
extern uint32_t SystemCoreClock;

/**
 * @brief	Update system core and ASYNC syscon clock rate, should be called if the
 *			system has a clock rate change
 * @return	None
 */
void SystemCoreClockUpdate(void);

/**
 * @brief	Set up and initialize hardware prior to call to main()
 * @return	None
 * @note	Chip_SystemInit() is called prior to the application and sets up
 * system clocking prior to the application starting.
 */
void Chip_SystemInit(void);

/**
 * @brief	Clock and PLL initialization based on the internal oscillator
 * @param	iFreq	: Rate (in Hz) to set the main system clock to
 * @return	None
 */
void Chip_SetupIrcClocking(uint32_t iFreq);

/**
 * @brief	Clock and PLL initialization based on the external clock input
 * @param	iFreq	: Rate (in Hz) to set the main system clock to
 * @return	None
 */
void Chip_SetupExtInClocking(uint32_t iFreq);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __CHIP_H_ */
