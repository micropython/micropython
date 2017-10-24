/*
 * @brief LPC18xx/43xx Alarm Timer driver
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

#ifndef __ATIMER_18XX_43XX_H_
#define __ATIMER_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup ATIMER_18XX_43XX CHIP: LPC18xx/43xx Alarm Timer driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief Alarm Timer register block structure
 */
typedef struct {					/*!< ATIMER Structure       */
	__IO uint32_t DOWNCOUNTER;		/*!< Downcounter register   */
	__IO uint32_t PRESET;			/*!< Preset value register  */
	__I  uint32_t RESERVED0[1012];
	__O  uint32_t CLR_EN;			/*!< Interrupt clear enable register */
	__O  uint32_t SET_EN;			/*!< Interrupt set enable register */
	__I  uint32_t STATUS;			/*!< Status register        */
	__I  uint32_t ENABLE;			/*!< Enable register        */
	__O  uint32_t CLR_STAT;			/*!< Clear register         */
	__O  uint32_t SET_STAT;			/*!< Set register           */
} LPC_ATIMER_T;

/**
 * @brief	Initialize Alarm Timer
 * @param	pATIMER		: The base of ATIMER peripheral on the chip
 * @param	PresetValue	: Count of 1 to 1024s for Alarm
 * @return	None
 */
void Chip_ATIMER_Init(LPC_ATIMER_T *pATIMER, uint32_t PresetValue);

/**
 * @brief	Close ATIMER device
 * @param	pATIMER	: The base of ATIMER peripheral on the chip
 * @return	None
 */
void Chip_ATIMER_DeInit(LPC_ATIMER_T *pATIMER);

/**
 * @brief	Enable ATIMER Interrupt
 * @param	pATIMER	: The base of ATIMER peripheral on the chip
 * @return	None
 */
STATIC INLINE void Chip_ATIMER_IntEnable(LPC_ATIMER_T *pATIMER)
{
	pATIMER->SET_EN = 1;
}

/**
 * @brief	Disable ATIMER Interrupt
 * @param	pATIMER	: The base of ATIMER peripheral on the chip
 * @return	None
 */
STATIC INLINE void Chip_ATIMER_IntDisable(LPC_ATIMER_T *pATIMER)
{
	pATIMER->CLR_EN = 1;
}

/**
 * @brief	Clear ATIMER Interrupt Status
 * @param	pATIMER	: The base of ATIMER peripheral on the chip
 * @return	None
 */
STATIC INLINE void Chip_ATIMER_ClearIntStatus(LPC_ATIMER_T *pATIMER)
{
	pATIMER->CLR_STAT = 1;
}

/**
 * @brief	Set ATIMER Interrupt Status
 * @param	pATIMER	: The base of ATIMER peripheral on the chip
 * @return	None
 */
STATIC INLINE void Chip_ATIMER_SetIntStatus(LPC_ATIMER_T *pATIMER)
{
	pATIMER->SET_STAT = 1;
}

/**
 * @brief	Update Preset value
 * @param	pATIMER		: The base of ATIMER peripheral on the chip
 * @param	PresetValue	: updated preset value
 * @return	Nothing
 */
STATIC INLINE void Chip_ATIMER_UpdatePresetValue(LPC_ATIMER_T *pATIMER, uint32_t PresetValue)
{
	pATIMER->PRESET = PresetValue;
}

/**
 * @brief	Read value of preset register
 * @param	pATIMER	: The base of ATIMER peripheral on the chip
 * @return	Value of capture register
 */
STATIC INLINE uint32_t Chip_ATIMER_GetPresetValue(LPC_ATIMER_T *pATIMER)
{
	return pATIMER->PRESET;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __ATIMER_18XX_43XX_H_ */






