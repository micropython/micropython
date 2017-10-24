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

#ifndef __CLOCK_5410X_H_
#define __CLOCK_5410X_H_

#include "pll_5410x.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup CLOCK_5410X CHIP: LPC5410X Clock Driver
 * @ingroup CHIP_5410X_DRIVERS
 * @{
 */

/* Internal oscillator frequency */
#define SYSCON_IRC_FREQ     (12000000)
#define SYSCON_WDTOSC_FREQ  (500000)
#define SYSCON_RTC_FREQ     (32768)

/**
 * @brief	Returns the internal oscillator (IRC) clock rate
 * @return	internal oscillator (IRC) clock rate
 */
__STATIC_INLINE uint32_t Chip_Clock_GetIntOscRate(void)
{
	return SYSCON_IRC_FREQ;
}

/**
 * @brief	Returns the external clock input rate
 * @return	External clock input rate
 */
__STATIC_INLINE uint32_t Chip_Clock_GetExtClockInRate(void)
{
	return ExtClockIn;
}

/**
 * @brief	Returns the RTC clock rate
 * @return	RTC oscillator clock rate in Hz
 */
__STATIC_INLINE uint32_t Chip_Clock_GetRTCOscRate(void)
{
	return SYSCON_RTC_FREQ;
}

/**
 * @brief	Return estimated watchdog oscillator rate
 * @return	Estimated watchdog oscillator rate
 * @note	This rate is accurate to plus or minus 40%.
 */
__STATIC_INLINE uint32_t Chip_Clock_GetWDTOSCRate(void)
{
	return SYSCON_WDTOSC_FREQ;
}

/**
 * Clock source selections for only the main A system clock. The main A system
 * clock is used as an input into the main B system clock selector. Main clock A
 * only needs to be setup if the main clock A input is used in the main clock
 * system selector.
 */
typedef enum {
	SYSCON_MAIN_A_CLKSRC_IRC = 0,		/*!< Internal oscillator */
	SYSCON_MAIN_A_CLKSRCA_CLKIN,		/*!< Crystal (main) oscillator in */
	SYSCON_MAIN_A_CLKSRCA_WDTOSC,		/*!< Watchdog oscillator rate */
} CHIP_SYSCON_MAIN_A_CLKSRC_T;

/**
 * @brief	Set main A system clock source
 * @param	src	: Clock source for main A
 * @return	Nothing
 * @note	This function only needs to be setup if main clock A will be
 * selected in the Chip_Clock_GetMain_B_ClockRate() function.
 */
__STATIC_INLINE void Chip_Clock_SetMain_A_ClockSource(CHIP_SYSCON_MAIN_A_CLKSRC_T src)
{
	LPC_SYSCON->MAINCLKSELA = (uint32_t) src;
}

/**
 * @brief   Returns the main A clock source
 * @return	Returns which clock is used for the main A
 */
__STATIC_INLINE CHIP_SYSCON_MAIN_A_CLKSRC_T Chip_Clock_GetMain_A_ClockSource(void)
{
	return (CHIP_SYSCON_MAIN_A_CLKSRC_T) (LPC_SYSCON->MAINCLKSELA);
}

/**
 * @brief	Return main A clock rate
 * @return	main A clock rate in Hz
 */
uint32_t Chip_Clock_GetMain_A_ClockRate(void);

/**
 * Clock sources for only main B system clock
 */
typedef enum {
	SYSCON_MAIN_B_CLKSRC_MAINCLKSELA = 0,	/*!< main clock A */
	SYSCON_MAIN_B_CLKSRC_SYSPLLIN,			/*!< System PLL input */
	SYSCON_MAIN_B_CLKSRC_SYSPLLOUT,			/*!< System PLL output */
	SYSCON_MAIN_B_CLKSRC_RTC,				/*!< RTC oscillator 32KHz output */
} CHIP_SYSCON_MAIN_B_CLKSRC_T;

/**
 * @brief	Set main B system clock source
 * @param	src	: Clock source for main B
 * @return	Nothing
 */
__STATIC_INLINE void Chip_Clock_SetMain_B_ClockSource(CHIP_SYSCON_MAIN_B_CLKSRC_T src)
{
	LPC_SYSCON->MAINCLKSELB = (uint32_t) src;
}

/**
 * @brief   Returns the main B clock source
 * @return	Returns which clock is used for the main B
 */
__STATIC_INLINE CHIP_SYSCON_MAIN_B_CLKSRC_T Chip_Clock_GetMain_B_ClockSource(void)
{
	return (CHIP_SYSCON_MAIN_B_CLKSRC_T) (LPC_SYSCON->MAINCLKSELB);
}

/**
 * @brief	Return main B clock rate
 * @return	main B clock rate
 */
uint32_t Chip_Clock_GetMain_B_ClockRate(void);

/**
 * Clock sources for CLKOUT
 */
typedef enum {
	SYSCON_CLKOUTSRC_MAINCLK = 0,		/*!< Main system clock for CLKOUT */
	SYSCON_CLKOUTSRC_CLKIN,				/*!< CLKIN for CLKOUT */
	SYSCON_CLKOUTSRC_WDTOSC,			/*!< Watchdog oscillator for CLKOUT */
	SYSCON_CLKOUTSRC_IRC,				/*!< Internal oscillator for CLKOUT */
	SYSCON_CLKOUTSRCA_OUTPUT,			/*!< clkoutA output route to input of clkoutB */
	SYSCON_CLKOUTSRC_RTC = 7			/*!< RTC oscillator 32KHz for CLKOUT */
} CHIP_SYSCON_CLKOUTSRC_T;

/**
 * @brief	Set CLKOUT clock source and divider
 * @param	src	: Clock source for CLKOUT
 * @param	div	: divider for CLKOUT clock
 * @return	Nothing
 * @note	Use 0 to disable, or a divider value of 1 to 255. The CLKOUT clock
 * rate is the clock source divided by the divider. This function will
 * also toggle the clock source update register to update the clock
 * source.
 */
void Chip_Clock_SetCLKOUTSource(CHIP_SYSCON_CLKOUTSRC_T src, uint32_t div);

/**
 * System and peripheral clocks enum
 */
typedef enum CHIP_SYSCON_CLOCK {
	/* Peripheral clock enables for SYSAHBCLKCTRL0 */
	SYSCON_CLOCK_ROM = 1,				/*!< ROM clock */
	SYSCON_CLOCK_SRAM1 = 3,				/*!< SRAM1 clock */
	SYSCON_CLOCK_SRAM2,					/*!< SRAM2 clock */
	SYSCON_CLOCK_FLASH = 7,				/*!< FLASH controller clock */
	SYSCON_CLOCK_FMC,					/*!< FMC clock */
	SYSCON_CLOCK_INPUTMUX = 11,			/*!< Input mux clock */
	SYSCON_CLOCK_IOCON = 13,			/*!< IOCON clock */
	SYSCON_CLOCK_GPIO0,					/*!< GPIO0 clock */
	SYSCON_CLOCK_GPIO1,					/*!< GPIO1 clock */
	SYSCON_CLOCK_PINT = 18,				/*!< PININT clock */
	SYSCON_CLOCK_GINT,					/*!< grouped pin interrupt block clock */
	SYSCON_CLOCK_DMA,					/*!< DMA clock */
	SYSCON_CLOCK_CRC,					/*!< CRC clock */
	SYSCON_CLOCK_WWDT,					/*!< WDT clock */
	SYSCON_CLOCK_RTC,					/*!< RTC clock */
	SYSCON_CLOCK_MAILBOX = 26,			/*!< Mailbox clock */
	SYSCON_CLOCK_ADC0,					/*!< ADC0 clock */

	/* Peripheral clock enables for SYSAHBCLKCTRL1 */
	SYSCON_CLOCK_MRT = 32,				/*!< multi-rate timer clock */
	SYSCON_CLOCK_RIT,					/*!< Repetitive interval timer clock */
	SYSCON_CLOCK_SCT0,					/*!< SCT0 clock */
	SYSCON_CLOCK_FIFO = 32 + 9,			/*!< System FIFO clock */
	SYSCON_CLOCK_UTICK,					/*!< UTICK clock */
	SYSCON_CLOCK_TIMER2 = 32 + 22,		/*!< TIMER2 clock */
	SYSCON_CLOCK_TIMER3 = 32 + 26,		/*!< TIMER3 clock */
	SYSCON_CLOCK_TIMER4,				/*!< TIMER4 clock */

	/* Peripheral clock enables for ASYNCAPBCLKCTRLCLR */
	SYSCON_CLOCK_USART0 = 128 + 1,		/*!< USART0 clock */
	SYSCON_CLOCK_USART1,				/*!< USART1 clock */
	SYSCON_CLOCK_USART2,				/*!< USART2 clock */
	SYSCON_CLOCK_USART3,				/*!< USART3 clock */
	SYSCON_CLOCK_I2C0,					/*!< I2C0  clock */
	SYSCON_CLOCK_I2C1,					/*!< I2C1  clock */
	SYSCON_CLOCK_I2C2,					/*!< I2C2  clock */
	SYSCON_CLOCK_SPI0 = 128 + 9,		/*!< SPI0  clock */
	SYSCON_CLOCK_SPI1,					/*!< SPI1  clock */
	SYSCON_CLOCK_TIMER0 = 128 + 13,		/*!< TIMER0 clock */
	SYSCON_CLOCK_TIMER1,				/*!< TIMER1 clock */
	SYSCON_CLOCK_FRG					/*!< FRG clock */
} CHIP_SYSCON_CLOCK_T;

/**
 * @brief	Enable a system or peripheral clock
 * @param	clk	: Clock to enable
 * @return	Nothing
 */
void Chip_Clock_EnablePeriphClock(CHIP_SYSCON_CLOCK_T clk);

/**
 * @brief	Disable a system or peripheral clock
 * @param	clk	: Clock to disable
 * @return	Nothing
 */
void Chip_Clock_DisablePeriphClock(CHIP_SYSCON_CLOCK_T clk);

/**
 * @brief	Set system tick clock divider (external CLKIN as SYSTICK reference only)
 * @param	div	: divider for system clock
 * @return	Nothing
 * @note	Use 0 to disable, or a divider value of 1 to 255. The system tick
 * rate is the external CLKIN rate divided by this value. The extern CLKIN pin
 * signal, divided by the SYSTICKCLKDIV divider, is selected by clearing
 * CLKSOURCE bit 2 in the System Tick CSR register. The core clock must be at least
 * 2.5 times faster than the reference system tick clock otherwise the count
 * values are unpredictable.
 */
__STATIC_INLINE void Chip_Clock_SetSysTickClockDiv(uint32_t div)
{
	LPC_SYSCON->SYSTICKCLKDIV = div;
}

/**
 * @brief	Returns system tick clock divider
 * @return	system tick clock divider
 */
__STATIC_INLINE uint32_t Chip_Clock_GetSysTickClockDiv(void)
{
	return LPC_SYSCON->SYSTICKCLKDIV;
}

/**
 * @brief	Returns the system tick rate as used with the system tick divider
 * @return	the system tick rate
 */
uint32_t Chip_Clock_GetSysTickClockRate(void);

/**
 * @brief	Set system clock divider
 * @param	div	: divider for system clock
 * @return	Nothing
 * @note	Use 0 to disable, or a divider value of 1 to 255. The system clock
 * rate is the main system clock divided by this value.
 */
__STATIC_INLINE void Chip_Clock_SetSysClockDiv(uint32_t div)
{
	LPC_SYSCON->AHBCLKDIV = div;
}

/**
 * @brief	Set system tick clock divider
 * @param	div	: divider for system clock
 * @return	Nothing
 * @note	Use 0 to disable, or a divider value of 1 to 255. The system tick
 * rate is the main system clock divided by this value. Use caution when using
 * the CMSIS SysTick_Config() functions as they typically use SystemCoreClock
 * for setup.
 */
__STATIC_INLINE void Chip_Clock_SetADCClockDiv(uint32_t div)
{
	LPC_SYSCON->ADCCLKDIV = div;
}

/**
 * @brief	Returns ADC clock divider
 * @return	ADC clock divider, 0 = disabled
 */
__STATIC_INLINE uint32_t Chip_Clock_GetADCClockDiv(void)
{
	return LPC_SYSCON->ADCCLKDIV;
}

/**
 * Clock sources for ADC clock source select
 */
typedef enum {
	SYSCON_ADCCLKSELSRC_MAINCLK = 0,		/*!< Main clock */
	SYSCON_ADCCLKSELSRC_SYSPLLOUT,			/*!< PLL output */
	SYSCON_ADCCLKSELSRC_IRC					/*!< Internal oscillator */
} CHIP_SYSCON_ADCCLKSELSRC_T;

/**
 * @brief	Set the ADC clock source
 * @param	src	: ADC clock source
 * @return	Nothing
 */
__STATIC_INLINE void Chip_Clock_SetADCClockSource(CHIP_SYSCON_ADCCLKSELSRC_T src)
{
	LPC_SYSCON->ADCCLKSEL = (uint32_t) src;
}

/**
 * @brief   Returns the ADC clock source
 * @return	Returns which clock is used for the ADC clock source
 */
__STATIC_INLINE CHIP_SYSCON_ADCCLKSELSRC_T Chip_Clock_GetADCClockSource(void)
{
	return (CHIP_SYSCON_ADCCLKSELSRC_T) (LPC_SYSCON->ADCCLKSEL);
}

/**
 * @brief	Return ADC clock rate
 * @return	ADC clock rate
 */
uint32_t Chip_Clock_GetADCClockRate(void);

/**
 * @brief	Enable the RTC 32KHz output
 * @return	Nothing
 * @note	This clock can be used for the main clock directly, but
 *			do not use this clock with the system PLL.
 */
__STATIC_INLINE void Chip_Clock_EnableRTCOsc(void)
{
	LPC_SYSCON->RTCOSCCTRL  = 1;
}

/**
 * @brief	Disable the RTC 32KHz output
 * @return	Nothing
 */
__STATIC_INLINE void Chip_Clock_DisableRTCOsc(void)
{
	LPC_SYSCON->RTCOSCCTRL  = 0;
}

/**
 * Clock source selections for the asynchronous APB clock
 */
typedef enum {
	SYSCON_ASYNC_IRC = 0,			/*!< IRC input */
	SYSCON_ASYNC_WDTOSC,			/*!< Watchdog oscillator */
	SYSCON_ASYNC_MAINCLK = 4,		/*!< Main clock */
	SYSCON_ASYNC_CLKIN,				/*!< external CLK input */
	SYSCON_ASYNC_SYSPLLOUT			/*!< System PLL output */
} CHIP_ASYNC_SYSCON_SRC_T;

/**
 * @brief	Set asynchronous APB clock source
 * @param	src	: Clock source for asynchronous APB clock
 * @return	Nothing
 */
void Chip_Clock_SetAsyncSysconClockSource(CHIP_ASYNC_SYSCON_SRC_T src);

/**
 * @brief	Get asynchronous APB clock source
 * @return	Clock source for asynchronous APB clock
 */
CHIP_ASYNC_SYSCON_SRC_T Chip_Clock_GetAsyncSysconClockSource(void);

/**
 * @brief	Return asynchronous APB clock rate
 * @return	Asynchronous APB clock rate
 * @note	Includes adjustments by Async clock divider (ASYNCCLKDIV).
 */
uint32_t Chip_Clock_GetAsyncSyscon_ClockRate(void);

/**
 * @brief	Set UART divider clock
 * @param	div	: divider for UART clock
 * @return	Nothing
 * @note	Use 0 to disable, or a divider value of 1 to 255. The UART clock
 * rate is the main system clock divided by this value.
 */
__STATIC_INLINE void Chip_Clock_SetAsyncSysconClockDiv(uint32_t div)
{
	LPC_ASYNC_SYSCON->ASYNCCLKDIV = div;
}

/**
 * Clock sources for main system clock. This is a mix of both main clock A
 * and B selections.
 */
typedef enum {
	SYSCON_MAINCLKSRC_IRC = 0,				/*!< Internal oscillator */
	SYSCON_MAINCLKSRC_CLKIN,				/*!< Crystal (main) oscillator in */
	SYSCON_MAINCLKSRC_WDTOSC,				/*!< Watchdog oscillator rate */
	SYSCON_MAINCLKSRC_PLLIN = 5,			/*!< System PLL input */
	SYSCON_MAINCLKSRC_PLLOUT,				/*!< System PLL output */
	SYSCON_MAINCLKSRC_RTC					/*!< RTC oscillator 32KHz output */
} CHIP_SYSCON_MAINCLKSRC_T;

/**
 * @brief	Set main system clock source
 * @param	src	: Clock source for main system
 * @return	Nothing
 */
void Chip_Clock_SetMainClockSource(CHIP_SYSCON_MAINCLKSRC_T src);

/**
 * @brief	Get main system clock source
 * @return	Clock source for main system
 * @note
 */
CHIP_SYSCON_MAINCLKSRC_T Chip_Clock_GetMainClockSource(void);

/**
 * @brief	Return main clock rate
 * @return	main clock rate
 */
uint32_t Chip_Clock_GetMainClockRate(void);

/**
 * @brief	Return system clock rate
 * @return	system clock rate
 * @note	This is the main clock rate divided by AHBCLKDIV.
 */
uint32_t Chip_Clock_GetSystemClockRate(void);

/**
 * @brief	Get UART base clock rate
 * @return	UART base clock rate
 */
uint32_t Chip_Clock_GetUARTBaseClockRate(void);

/**
 * @brief	Get UART base clock rate using FRG
 * @return	Actual UART base clock rate
 * @note	It's recommended to set a base rate at least 16x the
 * expected maximum UART transfer bit rate.
 */
uint32_t Chip_Clock_SetUARTBaseClockRate(uint32_t rate);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __CLOCK_5410X_H_ */
