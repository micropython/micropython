/*
 * @brief LPC18xx/43xx GPIO group driver
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

#ifndef __GPIOGROUP_18XX_43XX_H_
#define __GPIOGROUP_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup GPIOGP_18XX_43XX CHIP: LPC18xx/43xx GPIO group driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief GPIO grouped interrupt register block structure
 */
typedef struct {					/*!< GPIO_GROUP_INTn Structure */
	__IO uint32_t  CTRL;			/*!< GPIO grouped interrupt control register */
	__I  uint32_t  RESERVED0[7];
	__IO uint32_t  PORT_POL[8];		/*!< GPIO grouped interrupt port polarity register */
	__IO uint32_t  PORT_ENA[8];		/*!< GPIO grouped interrupt port m enable register */
	uint32_t       RESERVED1[1000];
} LPC_GPIOGROUPINT_T;

/**
 * LPC18xx/43xx GPIO group bit definitions
 */
#define GPIOGR_INT      (1 << 0)	/*!< GPIO interrupt pending/clear bit */
#define GPIOGR_COMB     (1 << 1)	/*!< GPIO interrupt OR(0)/AND(1) mode bit */
#define GPIOGR_TRIG     (1 << 2)	/*!< GPIO interrupt edge(0)/level(1) mode bit */

/**
 * @brief	Clear interrupt pending status for the selected group
 * @param	pGPIOGPINT	: Pointer to GPIO group register block
 * @param	group		: GPIO group number
 * @return	None
 */
STATIC INLINE void Chip_GPIOGP_ClearIntStatus(LPC_GPIOGROUPINT_T *pGPIOGPINT, uint8_t group)
{
	uint32_t temp;

	temp = pGPIOGPINT[group].CTRL;
	pGPIOGPINT[group].CTRL = temp | GPIOGR_INT;
}

/**
 * @brief	Returns current GPIO group inetrrupt pending status
 * @param	pGPIOGPINT	: Pointer to GPIO group register block
 * @param	group		: GPIO group number
 * @return	true if the group interrupt is pending, otherwise false.
 */
STATIC INLINE bool Chip_GPIOGP_GetIntStatus(LPC_GPIOGROUPINT_T *pGPIOGPINT, uint8_t group)
{
	return (bool) ((pGPIOGPINT[group].CTRL & GPIOGR_INT) != 0);
}

/**
 * @brief	Selected GPIO group functionality for trigger on any pin in group (OR mode)
 * @param	pGPIOGPINT	: Pointer to GPIO group register block
 * @param	group		: GPIO group number
 * @return	None
 */
STATIC INLINE void Chip_GPIOGP_SelectOrMode(LPC_GPIOGROUPINT_T *pGPIOGPINT, uint8_t group)
{
	pGPIOGPINT[group].CTRL &= ~GPIOGR_COMB;
}

/**
 * @brief	Selected GPIO group functionality for trigger on all matching pins in group (AND mode)
 * @param	pGPIOGPINT	: Pointer to GPIO group register block
 * @param	group		: GPIO group number
 * @return	None
 */
STATIC INLINE void Chip_GPIOGP_SelectAndMode(LPC_GPIOGROUPINT_T *pGPIOGPINT, uint8_t group)
{
	pGPIOGPINT[group].CTRL |= GPIOGR_COMB;
}

/**
 * @brief	Selected GPIO group functionality edge trigger mode
 * @param	pGPIOGPINT	: Pointer to GPIO group register block
 * @param	group		: GPIO group number
 * @return	None
 */
STATIC INLINE void Chip_GPIOGP_SelectEdgeMode(LPC_GPIOGROUPINT_T *pGPIOGPINT, uint8_t group)
{
	pGPIOGPINT[group].CTRL &= ~GPIOGR_TRIG;
}

/**
 * @brief	Selected GPIO group functionality level trigger mode
 * @param	pGPIOGPINT	: Pointer to GPIO group register block
 * @param	group		: GPIO group number
 * @return	None
 */
STATIC INLINE void Chip_GPIOGP_SelectLevelMode(LPC_GPIOGROUPINT_T *pGPIOGPINT, uint8_t group)
{
	pGPIOGPINT[group].CTRL |= GPIOGR_TRIG;
}

/**
 * @brief	Set selected pins for the group and port to low level trigger
 * @param	pGPIOGPINT	: Pointer to GPIO group register block
 * @param	group		: GPIO group number
 * @param	port		: GPIO port number
 * @param	pinMask		: Or'ed value of pins to select for low level (bit 0 = pin 0, 1 = pin1, etc.)
 * @return	None
 */
STATIC INLINE void Chip_GPIOGP_SelectLowLevel(LPC_GPIOGROUPINT_T *pGPIOGPINT,
											  uint8_t group,
											  uint8_t port,
											  uint32_t pinMask)
{
	pGPIOGPINT[group].PORT_POL[port] &= ~pinMask;
}

/**
 * @brief	Set selected pins for the group and port to high level trigger
 * @param	pGPIOGPINT	: Pointer to GPIO group register block
 * @param	group		: GPIO group number
 * @param	port		: GPIO port number
 * @param	pinMask		: Or'ed value of pins to select for high level (bit 0 = pin 0, 1 = pin1, etc.)
 * @return	None
 */
STATIC INLINE void Chip_GPIOGP_SelectHighLevel(LPC_GPIOGROUPINT_T *pGPIOGPINT,
											   uint8_t group,
											   uint8_t port,
											   uint32_t pinMask)
{
	pGPIOGPINT[group].PORT_POL[port] |= pinMask;
}

/**
 * @brief	Disabled selected pins for the group interrupt
 * @param	pGPIOGPINT	: Pointer to GPIO group register block
 * @param	group		: GPIO group number
 * @param	port		: GPIO port number
 * @param	pinMask		: Or'ed value of pins to disable interrupt for (bit 0 = pin 0, 1 = pin1, etc.)
 * @return	None
 * @note	Disabled pins do not contrinute to the group interrupt.
 */
STATIC INLINE void Chip_GPIOGP_DisableGroupPins(LPC_GPIOGROUPINT_T *pGPIOGPINT,
												uint8_t group,
												uint8_t port,
												uint32_t pinMask)
{
	pGPIOGPINT[group].PORT_ENA[port] &= ~pinMask;
}

/**
 * @brief	Enable selected pins for the group interrupt
 * @param	pGPIOGPINT	: Pointer to GPIO group register block
 * @param	group		: GPIO group number
 * @param	port		: GPIO port number
 * @param	pinMask		: Or'ed value of pins to enable interrupt for (bit 0 = pin 0, 1 = pin1, etc.)
 * @return	None
 * @note	Enabled pins contribute to the group interrupt.
 */
STATIC INLINE void Chip_GPIOGP_EnableGroupPins(LPC_GPIOGROUPINT_T *pGPIOGPINT,
											   uint8_t group,
											   uint8_t port,
											   uint32_t pinMask)
{
	pGPIOGPINT[group].PORT_ENA[port] |= pinMask;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __GPIOGROUP_18XX_43XX_H_ */
