/*
 * @brief LPC18xx/43xx event router driver
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

#ifndef __EVRT_18XX_43XX_H_
#define __EVRT_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup EVRT_18XX_43XX CHIP: LPC18xx/43xx Event router driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief Event Router register structure
 */
typedef struct {						/*!< EVENTROUTER Structure  */
	__IO uint32_t HILO;					/*!< Level configuration register */
	__IO uint32_t EDGE;					/*!< Edge configuration     */
	__I  uint32_t RESERVED0[1012];
	__O  uint32_t CLR_EN;				/*!< Event clear enable register */
	__O  uint32_t SET_EN;				/*!< Event set enable register */
	__I  uint32_t STATUS;				/*!< Status register        */
	__I  uint32_t ENABLE;				/*!< Enable register        */
	__O  uint32_t CLR_STAT;				/*!< Clear register         */
	__O  uint32_t SET_STAT;				/*!< Set register           */
} LPC_EVRT_T;

/**
 * @brief EVRT input sources
 */
typedef enum CHIP_EVRT_SRC {
	EVRT_SRC_WAKEUP0,			/*!< WAKEUP0 event router source		*/
	EVRT_SRC_WAKEUP1,			/*!< WAKEUP1 event router source		*/
	EVRT_SRC_WAKEUP2,			/*!< WAKEUP2 event router source		*/
	EVRT_SRC_WAKEUP3,			/*!< WAKEUP3 event router source		*/
	EVRT_SRC_ATIMER,			/*!< Alarm timer event router source	*/
	EVRT_SRC_RTC,				/*!< RTC event router source			*/
	EVRT_SRC_BOD1,				/*!< BOD event router source			*/
	EVRT_SRC_WWDT,				/*!< WWDT event router source			*/
	EVRT_SRC_ETHERNET,			/*!< Ethernet event router source		*/
	EVRT_SRC_USB0,				/*!< USB0 event router source			*/
	EVRT_SRC_USB1,				/*!< USB1 event router source			*/
	EVRT_SRC_SDIO,				/*!< Reserved							*/
	EVRT_SRC_CCAN,				/*!< C_CAN event router source			*/
	EVRT_SRC_COMBINE_TIMER2,	/*!< Combined timer 2 event router source	*/
	EVRT_SRC_COMBINE_TIMER6,	/*!< Combined timer 6 event router source	*/
	EVRT_SRC_QEI,				/*!< QEI event router source			*/
	EVRT_SRC_COMBINE_TIMER14,	/*!< Combined timer 14 event router source	*/
	EVRT_SRC_RESERVED1,			/*!< Reserved                           */
	EVRT_SRC_RESERVED2,			/*!< Reserved							*/
	EVRT_SRC_RESET,				/*!< Reset event router source			*/
    EVRT_SRC_BOD_RESET,			/*!< Reset event router source			*/
    EVRT_SRC_DEEP_POWER_DOWN    /*!< Reset event router source			*/
} CHIP_EVRT_SRC_T;

/**
 * @brief Macro for checking for a valid EVRT source
 */
#define PARAM_EVRT_SOURCE(n)    ((n == EVRT_SRC_WAKEUP0) || (n == EVRT_SRC_WAKEUP1)	\
								 || (n == EVRT_SRC_WAKEUP2) || (n == EVRT_SRC_WAKEUP3) \
								 || (n == EVRT_SRC_ATIMER) || (n == EVRT_SRC_RTC) \
								 || (n == EVRT_SRC_BOD1) || (n == EVRT_SRC_WWDT) \
								 || (n == EVRT_SRC_ETHERNET) || (n == EVRT_SRC_USB0) \
								 || (n == EVRT_SRC_USB1) || (n == EVRT_SRC_CCAN) || (n == EVRT_SRC_SDIO) \
								 || (n == EVRT_SRC_COMBINE_TIMER2) || (n == EVRT_SRC_COMBINE_TIMER6) \
								 || (n == EVRT_SRC_QEI) || (n == EVRT_SRC_COMBINE_TIMER14) \
								 || (n == EVRT_SRC_RESET) || (n == EVRT_SRC_BOD_RESET) || (n == EVRT_SRC_DEEP_POWER_DOWN)) \

/**
 * @brief EVRT input state detecting type
 */
typedef enum CHIP_EVRT_SRC_ACTIVE {
	EVRT_SRC_ACTIVE_LOW_LEVEL,		/*!< Active low level       */
	EVRT_SRC_ACTIVE_HIGH_LEVEL,		/*!< Active high level		*/
	EVRT_SRC_ACTIVE_FALLING_EDGE,	/*!< Active falling edge	*/
	EVRT_SRC_ACTIVE_RISING_EDGE		/*!< Active rising edge		*/
} CHIP_EVRT_SRC_ACTIVE_T;

/**
 * @brief Macro for checking for a valid EVRT state type
 */
#define PARAM_EVRT_SOURCE_ACTIVE_TYPE(n) ((n == EVRT_SRC_ACTIVE_LOW_LEVEL) || (n == EVRT_SRC_ACTIVE_HIGH_LEVEL)	\
										  || (n == EVRT_SRC_ACTIVE_FALLING_EDGE) || (n == EVRT_SRC_ACTIVE_RISING_EDGE))

/**
 * @brief	Initialize the EVRT
 * @return	Nothing
 */
void Chip_EVRT_Init (void);

/**
 * @brief	Set up the type of interrupt type for a source to EVRT
 * @param	EVRT_Src	: EVRT source, should be one of CHIP_EVRT_SRC_T type
 * @param	type		: EVRT type, should be one of CHIP_EVRT_SRC_ACTIVE_T type
 * @return	Nothing
 */
void Chip_EVRT_ConfigIntSrcActiveType(CHIP_EVRT_SRC_T EVRT_Src, CHIP_EVRT_SRC_ACTIVE_T type);

/**
 * @brief	Check if a source is sending interrupt to EVRT
 * @param	EVRT_Src	: EVRT source, should be one of CHIP_EVRT_SRC_T type
 * @return	true if the interrupt from the source is pending, otherwise false
 */
IntStatus Chip_EVRT_IsSourceInterrupting(CHIP_EVRT_SRC_T EVRT_Src);

/**
 * @brief	Enable or disable interrupt sources to EVRT
 * @param	EVRT_Src	: EVRT source, should be one of CHIP_EVRT_SRC_T type
 * @param	state		: ENABLE or DISABLE to enable or disable event router source
 * @return	Nothing
 */
void Chip_EVRT_SetUpIntSrc(CHIP_EVRT_SRC_T EVRT_Src, FunctionalState state);

/**
 * @brief	De-initializes the EVRT peripheral
 * @return	Nothing
 */
STATIC INLINE void Chip_EVRT_DeInit(void)
{
	LPC_EVRT->CLR_EN    = 0xFFFF;
	LPC_EVRT->CLR_STAT  = 0xFFFF;
}

/**
 * @brief	Clear pending interrupt EVRT source
 * @param	EVRT_Src	: EVRT source, should be one of CHIP_EVRT_SRC_T type
 * @return	Nothing
 */
STATIC INLINE void Chip_EVRT_ClrPendIntSrc(CHIP_EVRT_SRC_T EVRT_Src)
{
	LPC_EVRT->CLR_STAT = (1 << (uint8_t) EVRT_Src);
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __EVRT_18XX_43XX_H_ */






