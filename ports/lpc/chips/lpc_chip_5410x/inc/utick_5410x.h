/*
 * @brief LPC5410X Micro Tick chip driver
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

#ifndef __UTICK_5410X_H_
#define __UTICK_5410X_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup UTICK_5410X CHIP: LPC5410X Micro Tick driver
 * @ingroup CHIP_5410X_DRIVERS
 * @{
 */

/**
 * @brief Micro Tick register block structure
 */
typedef struct {
	__IO uint32_t CTRL;				/*!< UTick Control register */
	__IO uint32_t STATUS;			/*!< UTick Status register */
} LPC_UTICK_T;

/**
 * @brief UTick register definitions
 */
/** UTick repeat delay bit */
#define UTICK_CTRL_REPEAT           ((uint32_t) 1UL << 31)
/** UTick Delay Value Mask */
#define UTICK_CTRL_DELAY_MASK       ((uint32_t) 0x7FFFFFFF)
/** UTick Interrupt Status bit */
#define UTICK_STATUS_INTR           ((uint32_t) 1 << 0)
/** UTick Active Status bit */
#define UTICK_STATUS_ACTIVE         ((uint32_t) 1 << 1)
/** UTick Status Register Mask */
#define UTICK_STATUS_MASK           ((uint32_t) 0x03)

/**
 * @brief	Initialize the UTICK peripheral
 * @param	pUTICK	: UTICK peripheral selected
 * @return	Nothing
 */
__STATIC_INLINE void Chip_UTICK_Init(LPC_UTICK_T *pUTICK)
{
	Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_UTICK);
	Chip_SYSCON_PeriphReset(RESET_UTICK);
}

/**
 * @brief	De-initialize the UTICK peripheral
 * @param	pUTICK	: UTICK peripheral selected
 * @return	Nothing
 */
__STATIC_INLINE void Chip_UTICK_DeInit(LPC_UTICK_T *pUTICK)
{
	Chip_Clock_DisablePeriphClock(SYSCON_CLOCK_UTICK);
}

/**
 * @brief	Setup UTICK
 * @param	pUTICK		: The base address of UTICK block
 * @param	tick_value	: Tick value, should not exceed UTICK_CTRL_DELAY_MASK
 * @param	repeat		: If true then delay repeats continuously else it is one time
 * @return	Nothing
 */
__STATIC_INLINE void Chip_UTICK_SetTick(LPC_UTICK_T *pUTICK, uint32_t tick_value, bool repeat)
{
	if (repeat) {
		tick_value |= UTICK_CTRL_REPEAT;
	}

	pUTICK->CTRL = tick_value;
}

/**
 * @brief	Setup UTICK for the passed delay (in mS)
 * @param	pUTICK	: The base address of UTICK block
 * @param	delayMs	: Delay value in mS (Maximum is 1000mS)
 * @param	repeat	: If true then delay repeats continuously else it is one time
 * @return	Nothing
 * @note	The WDT oscillator runs at about 500KHz, so delays in uS won't be
 * too accurate.
 */
__STATIC_INLINE void Chip_UTICK_SetDelayMs(LPC_UTICK_T *pUTICK, uint32_t delayMs, bool repeat)
{
	uint32_t tick_value = (delayMs * Chip_Clock_GetWDTOSCRate()) / 1000;

	if (repeat) {
		tick_value |= UTICK_CTRL_REPEAT;
	}
	else {
		tick_value &= ~UTICK_CTRL_REPEAT;
	}

	pUTICK->CTRL = tick_value;
}

/**
 * @brief	Read UTICK Value
 * @param	pUTICK	: The base address of UTICK block
 * @return	Current tick value
 */
__STATIC_INLINE uint32_t Chip_UTICK_GetTick(LPC_UTICK_T *pUTICK)
{
	return pUTICK->CTRL & UTICK_CTRL_DELAY_MASK;
}

/**
 * @brief	Halt UTICK timer
 * @param	pUTICK	: The base address of UTICK block
 * @return	Nothing
 */
__STATIC_INLINE void Chip_UTICK_Halt(LPC_UTICK_T *pUTICK)
{
	pUTICK->CTRL = 0;
}

/**
 * @brief	Returns the status of UTICK
 * @param	pUTICK	: The base address of UTICK block
 * @return Micro tick timer status register value
 */
__STATIC_INLINE uint32_t Chip_UTICK_GetStatus(LPC_UTICK_T *pUTICK)
{
	return pUTICK->STATUS;
}

/**
 * @brief	Clears UTICK Interrupt flag
 * @param	pUTICK	: The base address of UTICK block
 * @return	Nothing
 */
__STATIC_INLINE void Chip_UTICK_ClearInterrupt(LPC_UTICK_T *pUTICK)
{
	pUTICK->STATUS = UTICK_STATUS_INTR;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __UTICK_5410X_H_ */
