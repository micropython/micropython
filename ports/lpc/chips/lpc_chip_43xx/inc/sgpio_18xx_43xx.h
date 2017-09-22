/*
 * @brief LPC43xx Serial GPIO driver
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

#ifndef __SGPIO_43XX_H_
#define __SGPIO_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup SGPIO_43XX CHIP: LPC43xx Serial GPIO driver
 * @ingroup LPC_CHIP_18XX_43XX_Drivers
 * This module is present in LPC43xx MCUs only.
 * @{
 */

#if defined(CHIP_LPC43XX)

/**
 * @brief Serial GPIO register block structure
 */
typedef struct {						/*!< SGPIO Structure        */
	__IO uint32_t  OUT_MUX_CFG[16];		/*!< Pin multiplexer configurationregisters. */
	__IO uint32_t  SGPIO_MUX_CFG[16];	/*!< SGPIO multiplexer configuration registers. */
	__IO uint32_t  SLICE_MUX_CFG[16];	/*!< Slice multiplexer configuration registers. */
	__IO uint32_t  REG[16];				/*!< Slice data registers. Eachtime COUNT0 reaches 0x0 the register shifts loading bit 31 withdata captured from DIN(n). DOUT(n) is set to REG(0) */
	__IO uint32_t  REG_SS[16];			/*!< Slice data shadow registers. Each time POSreaches 0x0 the contents of REG_SS is exchanged with the contentof REG */
	__IO uint32_t  PRESET[16];			/*!< Reload valueof COUNT0, loaded when COUNT0 reaches 0x0 */
	__IO uint32_t  COUNT[16];			/*!< Down counter, counts down each clock cycle. */
	__IO uint32_t  POS[16];				/*!< Each time COUNT0 reaches 0x0 */
	__IO uint32_t  MASK_A;				/*!< Mask for pattern match function of slice A */
	__IO uint32_t  MASK_H;				/*!< Mask for pattern match function of slice H */
	__IO uint32_t  MASK_I;				/*!< Mask for pattern match function of slice I */
	__IO uint32_t  MASK_P;				/*!< Mask for pattern match function of slice P */
	__I  uint32_t  GPIO_INREG;			/*!< GPIO input status register */
	__IO uint32_t  GPIO_OUTREG;			/*!< GPIO output control register */
	__IO uint32_t  GPIO_OENREG;			/*!< GPIO OE control register */
	__IO uint32_t  CTRL_ENABLED;		/*!< Enables the slice COUNT counter */
	__IO uint32_t  CTRL_DISABLED;		/*!< Disables the slice COUNT counter */
	__I  uint32_t  RESERVED0[823];
	__O  uint32_t  CLR_EN_0;			/*!< Shift clock interrupt clear mask */
	__O  uint32_t  SET_EN_0;			/*!< Shift clock interrupt set mask */
	__I  uint32_t  ENABLE_0;			/*!< Shift clock interrupt enable */
	__I  uint32_t  STATUS_0;			/*!< Shift clock interrupt status */
	__O  uint32_t  CTR_STATUS_0;		/*!< Shift clock interrupt clear status */
	__O  uint32_t  SET_STATUS_0;		/*!< Shift clock interrupt set status */
	__I  uint32_t  RESERVED1[2];
	__O  uint32_t  CLR_EN_1;			/*!< Capture clock interrupt clear mask */
	__O  uint32_t  SET_EN_1;			/*!< Capture clock interrupt set mask */
	__I  uint32_t  ENABLE_1;			/*!< Capture clock interrupt enable */
	__I  uint32_t  STATUS_1;			/*!< Capture clock interrupt status */
	__O  uint32_t  CTR_STATUS_1;		/*!< Capture clock interrupt clear status */
	__O  uint32_t  SET_STATUS_1;		/*!< Capture clock interrupt set status */
	__I  uint32_t  RESERVED2[2];
	__O  uint32_t  CLR_EN_2;			/*!< Pattern match interrupt clear mask */
	__O  uint32_t  SET_EN_2;			/*!< Pattern match interrupt set mask */
	__I  uint32_t  ENABLE_2;			/*!< Pattern match interrupt enable */
	__I  uint32_t  STATUS_2;			/*!< Pattern match interrupt status */
	__O  uint32_t  CTR_STATUS_2;		/*!< Pattern match interrupt clear status */
	__O  uint32_t  SET_STATUS_2;		/*!< Pattern match interrupt set status */
	__I  uint32_t  RESERVED3[2];
	__O  uint32_t  CLR_EN_3;			/*!< Input interrupt clear mask */
	__O  uint32_t  SET_EN_3;			/*!< Input bit match interrupt set mask */
	__I  uint32_t  ENABLE_3;			/*!< Input bit match interrupt enable */
	__I  uint32_t  STATUS_3;			/*!< Input bit match interrupt status */
	__O  uint32_t  CTR_STATUS_3;		/*!< Input bit match interrupt clear status */
	__O  uint32_t  SET_STATUS_3;		/*!< Shift clock interrupt set status */
} LPC_SGPIO_T;

#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __SGPIO_43XX_H_ */






