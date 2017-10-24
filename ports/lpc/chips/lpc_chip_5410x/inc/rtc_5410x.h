/*
 * @brief LPC5410X RTC chip driver
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

#ifndef __RTC_5410X_H_
#define __RTC_5410X_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup RTC_5410X CHIP: LPC5410X Real Time clock
 * @ingroup CHIP_5410X_DRIVERS
 * @{
 */

/**
 * @brief LPC5410X Real Time clock register block structure
 */
typedef struct {			/*!< RTC */
	__IO uint32_t CTRL;		/*!< RTC control register */
	__IO uint32_t MATCH;	/*!< PRTC match (alarm) register */
	__IO uint32_t COUNT;	/*!< RTC counter register */
	__IO uint32_t WAKE;		/*!< RTC high-resolution/wake-up timer control register */
} LPC_RTC_T;

/* CTRL register defniitions */
#define RTC_CTRL_SWRESET        (1 << 0)	/*!< Apply reset to RTC */
#define RTC_CTRL_OFD            (1 << 1)	/*!< Oscillator fail detect status (failed bit) */
#define RTC_CTRL_ALARM1HZ       (1 << 2)	/*!< RTC 1 Hz timer alarm flag status (match) bit */
#define RTC_CTRL_WAKE1KHZ       (1 << 3)	/*!< RTC 1 kHz timer wake-up flag status (timeout) bit */
#define RTC_CTRL_ALARMDPD_EN    (1 << 4)	/*!< RTC 1 Hz timer alarm for Deep power-down enable bit */
#define RTC_CTRL_WAKEDPD_EN     (1 << 5)	/*!< RTC 1 kHz timer wake-up for Deep power-down enable bit */
#define RTC_CTRL_RTC1KHZ_EN     (1 << 6)	/*!< RTC 1 kHz clock enable bit */
#define RTC_CTRL_RTC_EN         (1 << 7)	/*!< RTC enable bit */
#define RTC_CTRL_MASK           ((uint32_t) 0xF1)	/*!< RTC Control register Mask for reserved and status bits */

/**
 * @brief	Initialize the RTC peripheral
 * @param	pRTC	: RTC peripheral selected
 * @return	None
 */
__STATIC_INLINE void Chip_RTC_Init(LPC_RTC_T *pRTC)
{
	Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_RTC);
	Chip_SYSCON_PeriphReset(RESET_RTC);
}

/**
 * @brief	De-initialize the RTC peripheral
 * @param	pRTC	: RTC peripheral selected
 * @return	None
 */
__STATIC_INLINE void Chip_RTC_DeInit(LPC_RTC_T *pRTC)
{
	Chip_Clock_DisablePeriphClock(SYSCON_CLOCK_RTC);
}

/**
 * @brief	Enable RTC options
 * @param	pRTC	: The base address of RTC block
 * @param	flags	: And OR'ed value of RTC_CTRL_* definitions to enable
 * @return	Nothing
 * @note	You can enable multiple RTC options at once using this function
 *			by OR'ing them together. It is recommended to only use the
 *			RTC_CTRL_ALARMDPD_EN, RTC_CTRL_WAKEDPD_EN, RTC_CTRL_RTC1KHZ_EN, and
 *			RTC_CTRL_RTC_EN flags with this function.
 */
__STATIC_INLINE void Chip_RTC_EnableOptions(LPC_RTC_T *pRTC, uint32_t flags)
{
	pRTC->CTRL = (pRTC->CTRL & RTC_CTRL_MASK) | flags;
}

/**
 * @brief	Disable RTC options
 * @param	pRTC	: The base address of RTC block
 * @param	flags	: And OR'ed value of RTC_CTRL_* definitions to disable
 * @return	Nothing
 * @note	You can enable multiple RTC options at once using this function
 *			by OR'ing them together. It is recommended to only use the
 *			RTC_CTRL_ALARMDPD_EN, RTC_CTRL_WAKEDPD_EN, RTC_CTRL_RTC1KHZ_EN, and
 *			RTC_CTRL_RTC_EN flags with this function.
 */
__STATIC_INLINE void Chip_RTC_DisableOptions(LPC_RTC_T *pRTC, uint32_t flags)
{
	pRTC->CTRL = (pRTC->CTRL & RTC_CTRL_MASK) & ~flags;
}

/**
 * @brief	Reset RTC
 * @param	pRTC	: The base address of RTC block
 * @return	Nothing
 * @note	The RTC state will be returned to it's default.
 */
__STATIC_INLINE void Chip_RTC_Reset(LPC_RTC_T *pRTC)
{
	Chip_RTC_EnableOptions(pRTC, RTC_CTRL_SWRESET);
	Chip_RTC_DisableOptions(pRTC, RTC_CTRL_SWRESET);
}

/**
 * @brief	Enables the RTC
 * @param	pRTC	: The base address of RTC block
 * @return	Nothing
 * @note	You can also use Chip_RTC_EnableOptions() with the
 *			RTC_CTRL_RTC_EN flag to enable the RTC.
 */
__STATIC_INLINE void Chip_RTC_Enable(LPC_RTC_T *pRTC)
{
	Chip_RTC_EnableOptions(pRTC, RTC_CTRL_RTC_EN);
}

/**
 * @brief	Disables the RTC
 * @param	pRTC	: The base address of RTC block
 * @return	Nothing
 * @note	You can also use Chip_RTC_DisableOptions() with the
 *			RTC_CTRL_RTC_EN flag to enable the RTC.
 */
__STATIC_INLINE void Chip_RTC_Disable(LPC_RTC_T *pRTC)
{
	Chip_RTC_DisableOptions(pRTC, RTC_CTRL_RTC_EN);
}

/**
 * @brief	Enables the RTC 1KHz high resolution timer
 * @param	pRTC	: The base address of RTC block
 * @return	Nothing
 * @note	You can also use Chip_RTC_EnableOptions() with the
 *			RTC_CTRL_RTC1KHZ_EN flag to enable the high resolution
 *			timer.
 */
__STATIC_INLINE void Chip_RTC_Enable1KHZ(LPC_RTC_T *pRTC)
{
	Chip_RTC_EnableOptions(pRTC, RTC_CTRL_RTC1KHZ_EN);
}

/**
 * @brief	Disables the RTC 1KHz high resolution timer
 * @param	pRTC	: The base address of RTC block
 * @return	Nothing
 * @note	You can also use Chip_RTC_DisableOptions() with the
 *			RTC_CTRL_RTC1KHZ_EN flag to disable the high resolution
 *			timer.
 */
__STATIC_INLINE void Chip_RTC_Disable1KHZ(LPC_RTC_T *pRTC)
{
	Chip_RTC_DisableOptions(pRTC, RTC_CTRL_RTC1KHZ_EN);
}

/**
 * @brief	Enables selected RTC wakeup events
 * @param	pRTC	: The base address of RTC block
 * @param	ints	: Wakeup events to enable
 * @return	Nothing
 * @note	Select either one or both (OR'ed) RTC_CTRL_ALARMDPD_EN
 *			and RTC_CTRL_WAKEDPD_EN values to enabled. You can also
 *			use Chip_RTC_EnableOptions() with the flags to enable
 *			the events.
 */
__STATIC_INLINE void Chip_RTC_EnableWakeup(LPC_RTC_T *pRTC, uint32_t ints)
{
	Chip_RTC_EnableOptions(pRTC, ints);
}

/**
 * @brief	Disables selected RTC wakeup events
 * @param	pRTC	: The base address of RTC block
 * @param	ints	: Wakeup events to disable
 * @return	Nothing
 * @note	Select either one or both (OR'ed) RTC_CTRL_ALARMDPD_EN
 *			and RTC_CTRL_WAKEDPD_EN values to disabled. You can also
 *			use Chip_RTC_DisableOptions() with the flags to disable
 *			the events.
 */
__STATIC_INLINE void Chip_RTC_DisableWakeup(LPC_RTC_T *pRTC, uint32_t ints)
{
	Chip_RTC_DisableOptions(pRTC, ints);
}

/**
 * @brief	Clears latched RTC statuses
 * @param	pRTC	: The base address of RTC block
 * @param	stsMask	: OR'ed status bits to clear
 * @return	Nothing
 * @note	Use and OR'ed stsMask value of RTC_CTRL_OFD, RTC_CTRL_ALARM1HZ,
 *			and RTC_CTRL_WAKE1KHZ to clear specific RTC states.
 */
__STATIC_INLINE void Chip_RTC_ClearStatus(LPC_RTC_T *pRTC, uint32_t stsMask)
{
	pRTC->CTRL = (pRTC->CTRL & RTC_CTRL_MASK) | stsMask;
}

/**
 * @brief	Return RTC control/status register
 * @param	pRTC	: The base address of RTC block
 * @return	The current RTC control/status register
 * @note	Mask the return value with a RTC_CTRL_* definitions to determine
 *			which bits are set. For example, mask the return value with
 *			RTC_CTRL_ALARM1HZ to determine if the alarm interrupt is pending.
 */
__STATIC_INLINE uint32_t Chip_RTC_GetStatus(LPC_RTC_T *pRTC)
{
	return pRTC->CTRL;
}

/**
 * @brief	Set RTC match value for alarm status/interrupt
 * @param	pRTC	: The base address of RTC block
 * @param	count	: Alarm event time
 * @return	Nothing
 */
__STATIC_INLINE void Chip_RTC_SetAlarm(LPC_RTC_T *pRTC, uint32_t count)
{
	pRTC->MATCH = count;
}

/**
 * @brief	Return the RTC match value used for alarm status/interrupt
 * @param	pRTC	: The base address of RTC block
 * @return	Alarm event time
 */
__STATIC_INLINE uint32_t Chip_RTC_GetAlarm(LPC_RTC_T *pRTC)
{
	return pRTC->MATCH;
}

/**
 * @brief	Set RTC match count for 1 second timer count
 * @param	pRTC	: The base address of RTC block
 * @param	count	: Initial count to set
 * @return	Nothing
 * @note	Only write to this register when the RTC_CTRL_RTC_EN bit in
 *			the CTRL Register is 0. The counter increments one second
 *			after the RTC_CTRL_RTC_EN bit is set.
 */
__STATIC_INLINE void Chip_RTC_SetCount(LPC_RTC_T *pRTC, uint32_t count)
{
	pRTC->COUNT = count;
}

/**
 * @brief	Get current RTC 1 second timer count
 * @param	pRTC	: The base address of RTC block
 * @return	current RTC 1 second timer count
 */
__STATIC_INLINE uint32_t Chip_RTC_GetCount(LPC_RTC_T *pRTC)
{
	return pRTC->COUNT;
}

/**
 * @brief	Set RTC wake count countdown value (in mS ticks)
 * @param	pRTC	: The base address of RTC block
 * @param	count	: wakeup time in milliSeconds
 * @return	Nothing
 * @note	A write pre-loads a start count value into the wake-up
 *			timer and initializes a count-down sequence.
 */
__STATIC_INLINE void Chip_RTC_SetWake(LPC_RTC_T *pRTC, uint16_t count)
{
	pRTC->WAKE = count;
}

/**
 * @brief	Get RTC wake count countdown value
 * @param	pRTC	: The base address of RTC block
 * @return	current RTC wake count countdown value (in mS)
 */
__STATIC_INLINE uint16_t Chip_RTC_GetWake(LPC_RTC_T *pRTC)
{
	return pRTC->WAKE;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTC_5410X_H_ */
