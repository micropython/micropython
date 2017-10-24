/*
 * @brief LPC5410X Power Management declarations and functions
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

#ifndef __PMU_5410X_H_
#define __PMU_5410X_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup PMU_5410X CHIP: LPC5410X Power Management declarations and functions
 * @ingroup CHIP_5410X_DRIVERS
 * @{
 */

/**
 * @brief PMU register block structure
 * @note Most of the PMU support is handled by the PMU library.
 */
typedef struct {
	__I  uint32_t RESERVED0[4];
	__I  uint32_t RESERVED1[4];
	__I  uint32_t RESERVED2[4];
	__I  uint32_t RESERVED3[4];
	__I  uint32_t RESERVED4;
	__IO uint32_t BODCTRL;
	__I  uint32_t RESERVED5;
	__I  uint32_t RESERVED6;
	__IO uint32_t DPDWAKESRC;
} LPC_PMU_T;

/**
 * Brown-out detector reset level
 */
typedef enum {
	PMU_BODRSTLVL_0,	/*!< Brown-out reset at ~1.5v */
	PMU_BODRSTLVL_1_50V = PMU_BODRSTLVL_0,
	PMU_BODRSTLVL_1,	/*!< Brown-out reset at ~1.85v */
	PMU_BODRSTLVL_1_85V = PMU_BODRSTLVL_1,
	PMU_BODRSTLVL_2,	/*!< Brown-out reset at ~2.0v */
	PMU_BODRSTLVL_2_00V = PMU_BODRSTLVL_2,
	PMU_BODRSTLVL_3,	/*!< Brown-out reset at ~2.3v */
	PMU_BODRSTLVL_2_30V = PMU_BODRSTLVL_3
} CHIP_PMU_BODRSTLVL_T;

/**
 * Brown-out detector interrupt level
 */
typedef enum CHIP_PMU_BODRINTVAL {
	PMU_BODINTVAL_LVL0,	/*!< Brown-out interrupt at ~2.05v */
	PMU_BODINTVAL_2_05v = PMU_BODINTVAL_LVL0,
	PMU_BODINTVAL_LVL1,	/*!< Brown-out interrupt at ~2.45v */
	PMU_BODINTVAL_2_45v = PMU_BODINTVAL_LVL1,
	PMU_BODINTVAL_LVL2,	/*!< Brown-out interrupt at ~2.75v */
	PMU_BODINTVAL_2_75v = PMU_BODINTVAL_LVL2,
	PMU_BODINTVAL_LVL3,	/*!< Brown-out interrupt at ~3.05v */
	PMU_BODINTVAL_3_05v = PMU_BODINTVAL_LVL3
} CHIP_PMU_BODRINTVAL_T;

/**
 * brown-out detection reset status (in BODCTRL register)
 */
#define PMU_BOD_RST     (1 << 6)
/**
 * brown-out detection interrupt status (in BODCTRL register)
 */
#define PMU_BOD_INT     (1 << 7)

/**
 * @brief	Set brown-out detection interrupt and reset levels
 * @param	rstlvl	: Brown-out detector reset level
 * @param	intlvl	: Brown-out interrupt level
 * @return	Nothing
 * @note	Brown-out detection reset will be disabled upon exiting this function.
 * Use Chip_PMU_EnableBODReset() to re-enable.
 */
__STATIC_INLINE void Chip_PMU_SetBODLevels(CHIP_PMU_BODRSTLVL_T rstlvl,
										   CHIP_PMU_BODRINTVAL_T intlvl)
{
	LPC_PMU->BODCTRL = ((uint32_t) rstlvl) | (((uint32_t) intlvl) << 2);
}

/**
 * @brief	Enable brown-out detection reset
 * @return	Nothing
 */
__STATIC_INLINE void Chip_PMU_EnableBODReset(void)
{
	LPC_PMU->BODCTRL |= (1 << 4);
}

/**
 * @brief	Disable brown-out detection reset
 * @return	Nothing
 */
__STATIC_INLINE void Chip_PMU_DisableBODReset(void)
{
	LPC_PMU->BODCTRL &= ~(1 << 4);
}

/**
 * @brief	Enable brown-out detection interrupt
 * @return	Nothing
 */
__STATIC_INLINE void Chip_PMU_EnableBODInt(void)
{
	LPC_PMU->BODCTRL |= (1 << 5);
}

/**
 * @brief	Disable brown-out detection interrupt
 * @return	Nothing
 */
__STATIC_INLINE void Chip_PMU_DisableBODInt(void)
{
	LPC_PMU->BODCTRL &= ~(1 << 5);
}

/**
 * Deep power down reset sources
 */
#define PMU_DPDWU_RESET     (1 << 0)	/*!< Deep powerdown wakeup by reset pin */
#define PMU_DPDWU_RTC       (1 << 1)	/*!< Deep powerdown wakeup by RTC */
#define PMU_DPDWU_BODRESET  (1 << 2)	/*!< Deep powerdown wakeup by brown out reset*/
#define PMU_DPDWU_BODINTR   (1 << 3)	/*!< Deep powerdown wakeup by brown out interrupt */

/**
 * @brief	Return wakeup sources from deep power down mode
 * @return	Deep power down mode wakeup sources
 * @note	Mask the return value with a PMU_DPDWU_* value to determine
 * the wakeup source from deep power down.
 */
__STATIC_INLINE uint32_t Chip_PMU_GetDPDWUSource(void)
{
	return LPC_PMU->DPDWAKESRC;
}

/**
 * @brief	Clear a deep power down mode wakeup source
 * @param	mask	: Or'ed PMU_DPDWU_* values to clear
 * @return	Nothing
 */
__STATIC_INLINE void Chip_PMU_ClearDPDWUSource(uint32_t mask)
{
	LPC_PMU->DPDWAKESRC = mask;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __PMU_5410X_H_ */
