/*
 * @brief LPC18xx/43xx Motor Control PWM driver
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

#ifndef __MCPWM_18XX_43XX_H_
#define __MCPWM_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MCPWM_18XX_43XX CHIP: LPC18xx/43xx Motor Control PWM driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief Motor Control PWM register block structure
 */
typedef struct {					/*!< MCPWM Structure        */
	__I  uint32_t  CON;				/*!< PWM Control read address */
	__O  uint32_t  CON_SET;			/*!< PWM Control set address */
	__O  uint32_t  CON_CLR;			/*!< PWM Control clear address */
	__I  uint32_t  CAPCON;			/*!< Capture Control read address */
	__O  uint32_t  CAPCON_SET;		/*!< Capture Control set address */
	__O  uint32_t  CAPCON_CLR;		/*!< Event Control clear address */
	__IO uint32_t TC[3];			/*!< Timer Counter register */
	__IO uint32_t LIM[3];			/*!< Limit register         */
	__IO uint32_t MAT[3];			/*!< Match register         */
	__IO uint32_t  DT;				/*!< Dead time register     */
	__IO uint32_t  CCP;				/*!< Communication Pattern register */
	__I  uint32_t CAP[3];			/*!< Capture register       */
	__I  uint32_t  INTEN;			/*!< Interrupt Enable read address */
	__O  uint32_t  INTEN_SET;		/*!< Interrupt Enable set address */
	__O  uint32_t  INTEN_CLR;		/*!< Interrupt Enable clear address */
	__I  uint32_t  CNTCON;			/*!< Count Control read address */
	__O  uint32_t  CNTCON_SET;		/*!< Count Control set address */
	__O  uint32_t  CNTCON_CLR;		/*!< Count Control clear address */
	__I  uint32_t  INTF;			/*!< Interrupt flags read address */
	__O  uint32_t  INTF_SET;		/*!< Interrupt flags set address */
	__O  uint32_t  INTF_CLR;		/*!< Interrupt flags clear address */
	__O  uint32_t  CAP_CLR;			/*!< Capture clear address  */
} LPC_MCPWM_T;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __MCPWM_18XX_43XX_H_ */






