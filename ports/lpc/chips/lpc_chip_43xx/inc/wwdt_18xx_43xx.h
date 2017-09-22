/*
 * @brief LPC18xx/43xx WWDT driver
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

#ifndef __WWDT_18XX_43XX_H_
#define __WWDT_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup WWDT_18XX_43XX CHIP: LPC18xx/43xx Windowed Watchdog driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

#define WATCHDOG_WINDOW_SUPPORT

/** WDT oscillator frequency value */
#define WDT_OSC     (CGU_IRC_FREQ)

/**
 * @brief Windowed Watchdog register block structure
 */
typedef struct {				/*!< WWDT Structure         */
	__IO uint32_t  MOD;			/*!< Watchdog mode register. This register contains the basic mode and status of the Watchdog Timer. */
	__IO uint32_t  TC;			/*!< Watchdog timer constant register. This register determines the time-out value. */
	__O  uint32_t  FEED;		/*!< Watchdog feed sequence register. Writing 0xAA followed by 0x55 to this register reloads the Watchdog timer with the value contained in WDTC. */
	__I  uint32_t  TV;			/*!< Watchdog timer value register. This register reads out the current value of the Watchdog timer. */
	__I  uint32_t  RESERVED0;
#ifdef WATCHDOG_WINDOW_SUPPORT
	__IO uint32_t  WARNINT;		/*!< Watchdog warning interrupt register. This register contains the Watchdog warning interrupt compare value. */
	__IO uint32_t  WINDOW;		/*!< Watchdog timer window register. This register contains the Watchdog window value. */
#endif
} LPC_WWDT_T;

/**
 * @brief Watchdog Mode register definitions
 */
/** Watchdog Mode Bitmask */
#define WWDT_WDMOD_BITMASK          ((uint32_t) 0x1F)
/** WWDT interrupt enable bit */
#define WWDT_WDMOD_WDEN             ((uint32_t) (1 << 0))
/** WWDT interrupt enable bit */
#define WWDT_WDMOD_WDRESET          ((uint32_t) (1 << 1))
/** WWDT time out flag bit */
#define WWDT_WDMOD_WDTOF            ((uint32_t) (1 << 2))
/** WDT Time Out flag bit */
#define WWDT_WDMOD_WDINT            ((uint32_t) (1 << 3))
/** WWDT Protect flag bit */
#define WWDT_WDMOD_WDPROTECT        ((uint32_t) (1 << 4))

/**
 * @brief	Initialize the Watchdog timer
 * @param	pWWDT	: The base of WatchDog Timer peripheral on the chip
 * @return	None
 */
void Chip_WWDT_Init(LPC_WWDT_T *pWWDT);

/**
 * @brief	Shutdown the Watchdog timer
 * @param	pWWDT	: The base of WatchDog Timer peripheral on the chip
 * @return	None
 */
void Chip_WWDT_DeInit(LPC_WWDT_T *pWWDT);

/**
 * @brief	Set WDT timeout constant value used for feed
 * @param	pWWDT	: The base of WatchDog Timer peripheral on the chip
 * @param	timeout	: WDT timeout in ticks, between WWDT_TICKS_MIN and WWDT_TICKS_MAX
 * @return	none
 */
STATIC INLINE void Chip_WWDT_SetTimeOut(LPC_WWDT_T *pWWDT, uint32_t timeout)
{
	pWWDT->TC = timeout;
}

/**
 * @brief	Feed watchdog timer
 * @param	pWWDT	: The base of WatchDog Timer peripheral on the chip
 * @return	None
 * @note	If this function isn't called, a watchdog timer warning will occur.
 * After the warning, a timeout will occur if a feed has happened.
 */
STATIC INLINE void Chip_WWDT_Feed(LPC_WWDT_T *pWWDT)
{
	pWWDT->FEED = 0xAA;
	pWWDT->FEED = 0x55;
}

#if defined(WATCHDOG_WINDOW_SUPPORT)
/**
 * @brief	Set WWDT warning interrupt
 * @param	pWWDT	: The base of WatchDog Timer peripheral on the chip
 * @param	timeout	: WDT warning in ticks, between 0 and 1023
 * @return	None
 * @note	This is the number of ticks after the watchdog interrupt that the
 * warning interrupt will be generated.
 */
STATIC INLINE void Chip_WWDT_SetWarning(LPC_WWDT_T *pWWDT, uint32_t timeout)
{
	pWWDT->WARNINT = timeout;
}

/**
 * @brief	Set WWDT window time
 * @param	pWWDT	: The base of WatchDog Timer peripheral on the chip
 * @param	timeout	: WDT timeout in ticks, between WWDT_TICKS_MIN and WWDT_TICKS_MAX
 * @return	None
 * @note	The watchdog timer must be fed between the timeout from the Chip_WWDT_SetTimeOut()
 * function and this function, with this function defining the last tick before the
 * watchdog window interrupt occurs.
 */
STATIC INLINE void Chip_WWDT_SetWindow(LPC_WWDT_T *pWWDT, uint32_t timeout)
{
	pWWDT->WINDOW = timeout;
}

#endif

/**
 * @brief	Enable watchdog timer options
 * @param	pWWDT	: The base of WatchDog Timer peripheral on the chip
 * @param	options	: An or'ed set of options of values
 *						WWDT_WDMOD_WDEN, WWDT_WDMOD_WDRESET, and WWDT_WDMOD_WDPROTECT
 * @return	None
 * @note	You can enable more than one option at once (ie, WWDT_WDMOD_WDRESET |
 * WWDT_WDMOD_WDPROTECT), but use the WWDT_WDMOD_WDEN after all other options
 * are set (or unset) with no other options. If WWDT_WDMOD_LOCK is used, it cannot
 * be unset.
 */
STATIC INLINE void Chip_WWDT_SetOption(LPC_WWDT_T *pWWDT, uint32_t options)
{
	pWWDT->MOD |= options;
}

/**
 * @brief	Disable/clear watchdog timer options
 * @param	pWWDT	: The base of WatchDog Timer peripheral on the chip
 * @param	options	: An or'ed set of options of values
 *						WWDT_WDMOD_WDEN, WWDT_WDMOD_WDRESET, and WWDT_WDMOD_WDPROTECT
 * @return	None
 * @note	You can disable more than one option at once (ie, WWDT_WDMOD_WDRESET |
 * WWDT_WDMOD_WDTOF).
 */
STATIC INLINE void Chip_WWDT_UnsetOption(LPC_WWDT_T *pWWDT, uint32_t options)
{
	pWWDT->MOD &= (~options) & WWDT_WDMOD_BITMASK;
}

/**
 * @brief	Enable WWDT activity
 * @param	pWWDT	: The base of WatchDog Timer peripheral on the chip
 * @return	None
 */
STATIC INLINE void Chip_WWDT_Start(LPC_WWDT_T *pWWDT)
{
	Chip_WWDT_SetOption(pWWDT, WWDT_WDMOD_WDEN);
	Chip_WWDT_Feed(pWWDT);
}

/**
 * @brief	Read WWDT status flag
 * @param	pWWDT	: The base of WatchDog Timer peripheral on the chip
 * @return	Watchdog status, an Or'ed value of WWDT_WDMOD_*
 */
STATIC INLINE uint32_t Chip_WWDT_GetStatus(LPC_WWDT_T *pWWDT)
{
	return pWWDT->MOD;
}

/**
 * @brief	Clear WWDT interrupt status flags
 * @param	pWWDT	: The base of WatchDog Timer peripheral on the chip
 * @param	status	: Or'ed value of status flag(s) that you want to clear, should be:
 *              - WWDT_WDMOD_WDTOF: Clear watchdog timeout flag
 *              - WWDT_WDMOD_WDINT: Clear watchdog warning flag
 * @return	None
 */
void Chip_WWDT_ClearStatusFlag(LPC_WWDT_T *pWWDT, uint32_t status);

/**
 * @brief	Get the current value of WDT
 * @param	pWWDT	: The base of WatchDog Timer peripheral on the chip
 * @return	current value of WDT
 */
STATIC INLINE uint32_t Chip_WWDT_GetCurrentCount(LPC_WWDT_T *pWWDT)
{
	return pWWDT->TV;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WWDT_18XX_43XX_H_ */






