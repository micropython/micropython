/*
 * @brief LPC18xx/43xx Quadrature Encoder Interface driver
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

#ifndef __QEI_18XX_43XX_H_
#define __QEI_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup QEI_18XX_43XX CHIP: LPC18xx/43xx Quadrature Encoder Interface driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief Quadrature Encoder Interface register block structure
 */
typedef struct {				/*!< QEI Structure          */
	__O  uint32_t  CON;			/*!< Control register       */
	__I  uint32_t  STAT;		/*!< Encoder status register */
	__IO uint32_t  CONF;		/*!< Configuration register */
	__I  uint32_t  POS;			/*!< Position register      */
	__IO uint32_t  MAXPOS;		/*!< Maximum position register */
	__IO uint32_t  CMPOS0;		/*!< position compare register 0 */
	__IO uint32_t  CMPOS1;		/*!< position compare register 1 */
	__IO uint32_t  CMPOS2;		/*!< position compare register 2 */
	__I  uint32_t  INXCNT;		/*!< Index count register   */
	__IO uint32_t  INXCMP0;		/*!< Index compare register 0 */
	__IO uint32_t  LOAD;		/*!< Velocity timer reload register */
	__I  uint32_t  TIME;		/*!< Velocity timer register */
	__I  uint32_t  VEL;			/*!< Velocity counter register */
	__I  uint32_t  CAP;			/*!< Velocity capture register */
	__IO uint32_t  VELCOMP;		/*!< Velocity compare register */
	__IO uint32_t  FILTERPHA;	/*!< Digital filter register on input phase A (QEI_A) */
	__IO uint32_t  FILTERPHB;	/*!< Digital filter register on input phase B (QEI_B) */
	__IO uint32_t  FILTERINX;	/*!< Digital filter register on input index (QEI_IDX) */
	__IO uint32_t  WINDOW;		/*!< Index acceptance window register */
	__IO uint32_t  INXCMP1;		/*!< Index compare register 1 */
	__IO uint32_t  INXCMP2;		/*!< Index compare register 2 */
	__I  uint32_t  RESERVED0[993];
	__O  uint32_t  IEC;			/*!< Interrupt enable clear register */
	__O  uint32_t  IES;			/*!< Interrupt enable set register */
	__I  uint32_t  INTSTAT;		/*!< Interrupt status register */
	__I  uint32_t  IE;			/*!< Interrupt enable register */
	__O  uint32_t  CLR;			/*!< Interrupt status clear register */
	__O  uint32_t  SET;			/*!< Interrupt status set register */
} LPC_QEI_T;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __QEI_18XX_43XX_H_ */
