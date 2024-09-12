/**
  ******************************************************************************
  * @file    stm32n6xx_hal_icache.h
  * @author  MCD Application Team
  * @brief   Header file of ICACHE HAL module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef STM32N6xx_HAL_ICACHE_H
#define STM32N6xx_HAL_ICACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes -----------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

#if defined(ICACHE)
/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup ICACHE
  * @{
  */

/* Exported types -----------------------------------------------------------*/

/* Exported constants -------------------------------------------------------*/
/** @defgroup ICACHE_Exported_Constants ICACHE Exported Constants
  * @{
  */

/** @defgroup ICACHE_WaysSelection Ways selection
  * @{
  */
#define ICACHE_1WAY                    0U                /*!< 1-way cache (direct mapped cache) */
#define ICACHE_4WAYS                   ICACHE_CR_WAYSEL  /*!< 4-ways set associative cache (default) */
/**
  * @}
  */

/** @defgroup ICACHE_Monitor_Type Monitor type
  * @{
  */
#define ICACHE_MONITOR_HIT_MISS        (ICACHE_CR_HITMEN | ICACHE_CR_MISSMEN) /*!< Hit & Miss monitoring */
#define ICACHE_MONITOR_HIT             ICACHE_CR_HITMEN                       /*!< Hit monitoring */
#define ICACHE_MONITOR_MISS            ICACHE_CR_MISSMEN                      /*!< Miss monitoring */
/**
  * @}
  */


/** @defgroup ICACHE_Interrupts Interrupts
  * @{
  */
#define ICACHE_IT_BUSYEND              ICACHE_IER_BSYENDIE /*!< Busy end interrupt */
#define ICACHE_IT_ERROR                ICACHE_IER_ERRIE    /*!< Cache error interrupt */
/**
  * @}
  */

/** @defgroup ICACHE_Flags Flags
  * @{
  */
#define ICACHE_FLAG_BUSY               ICACHE_SR_BUSYF     /*!< Busy flag */
#define ICACHE_FLAG_BUSYEND            ICACHE_SR_BSYENDF   /*!< Busy end flag */
#define ICACHE_FLAG_ERROR              ICACHE_SR_ERRF      /*!< Cache error flag */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros ----------------------------------------------------------*/
/** @defgroup ICACHE_Exported_Macros ICACHE Exported Macros
  * @{
  */

/** @defgroup ICACHE_Flags_Interrupts_Management Flags and Interrupts Management
  * @brief macros to manage the specified ICACHE flags and interrupts.
  * @{
  */

/** @brief  Enable ICACHE interrupts.
  * @param  __INTERRUPT__ specifies the ICACHE interrupt sources to be enabled.
  *         This parameter can be any combination of the following values:
  *            @arg @ref ICACHE_IT_BUSYEND  Busy end interrupt
  *            @arg @ref ICACHE_IT_ERROR  Cache error interrupt
  */
#define __HAL_ICACHE_ENABLE_IT(__INTERRUPT__) SET_BIT(ICACHE->IER, (__INTERRUPT__))

/** @brief  Disable ICACHE interrupts.
  * @param  __INTERRUPT__ specifies the ICACHE interrupt sources to be disabled.
  *         This parameter can be any combination of the following values:
  *            @arg @ref ICACHE_IT_BUSYEND  Busy end interrupt
  *            @arg @ref ICACHE_IT_ERROR  Cache error interrupt
  */
#define __HAL_ICACHE_DISABLE_IT(__INTERRUPT__) CLEAR_BIT(ICACHE->IER, (__INTERRUPT__))

/** @brief  Check whether the specified ICACHE interrupt source is enabled or not.
  * @param  __INTERRUPT__ specifies the ICACHE interrupt source to check.
  *         This parameter can be any combination of the following values:
  *            @arg @ref ICACHE_IT_BUSYEND  Busy end interrupt
  *            @arg @ref ICACHE_IT_ERROR  Cache error interrupt
  * @retval The state of __INTERRUPT__ (0 or 1).
  */
#define __HAL_ICACHE_GET_IT_SOURCE(__INTERRUPT__)  \
  ((READ_BIT(ICACHE->IER, (__INTERRUPT__)) == (__INTERRUPT__)) ? 1U : 0U)

/** @brief  Check whether the selected ICACHE flag is set or not.
  * @param  __FLAG__ specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg @ref ICACHE_FLAG_BUSY  Busy flag
  *            @arg @ref ICACHE_FLAG_BUSYEND  Busy end flag
  *            @arg @ref ICACHE_FLAG_ERROR  Cache error flag
  * @retval The state of __FLAG__ (0 or 1).
  */
#define __HAL_ICACHE_GET_FLAG(__FLAG__) ((READ_BIT(ICACHE->SR, (__FLAG__)) != 0U) ? 1U : 0U)

/** @brief  Clear the selected ICACHE flags.
  * @param  __FLAG__ specifies the ICACHE flags to clear.
  *         This parameter can be any combination of the following values:
  *            @arg @ref ICACHE_FLAG_BUSYEND  Busy end flag
  *            @arg @ref ICACHE_FLAG_ERROR  Cache error flag
  */
#define __HAL_ICACHE_CLEAR_FLAG(__FLAG__) WRITE_REG(ICACHE->FCR, (__FLAG__))

/**
  * @}
  */

/**
  * @}
  */

/* Exported functions -------------------------------------------------------*/
/** @addtogroup ICACHE_Exported_Functions
  * @{
  */

/** @addtogroup ICACHE_Exported_Functions_Group1
  * @brief    Initialization and control functions
  * @{
  */
/* Peripheral Control functions **********************************************/
HAL_StatusTypeDef HAL_ICACHE_Enable(void);
HAL_StatusTypeDef HAL_ICACHE_Disable(void);
uint32_t HAL_ICACHE_IsEnabled(void);
HAL_StatusTypeDef HAL_ICACHE_ConfigAssociativityMode(uint32_t AssociativityMode);
HAL_StatusTypeDef HAL_ICACHE_DeInit(void);

/******* Invalidate in blocking mode (Polling) */
HAL_StatusTypeDef HAL_ICACHE_Invalidate(void);
/******* Invalidate in non-blocking mode (Interrupt) */
HAL_StatusTypeDef HAL_ICACHE_Invalidate_IT(void);
/******* Wait for Invalidate complete in blocking mode (Polling) */
HAL_StatusTypeDef HAL_ICACHE_WaitForInvalidateComplete(void);

/******* Performance instruction cache monitoring functions */
HAL_StatusTypeDef HAL_ICACHE_Monitor_Start(uint32_t MonitorType);
HAL_StatusTypeDef HAL_ICACHE_Monitor_Stop(uint32_t MonitorType);
HAL_StatusTypeDef HAL_ICACHE_Monitor_Reset(uint32_t MonitorType);
uint32_t HAL_ICACHE_Monitor_GetHitValue(void);
uint32_t HAL_ICACHE_Monitor_GetMissValue(void);

/**
  * @}
  */

/** @addtogroup ICACHE_Exported_Functions_Group2
  * @brief    IRQ and callback functions
  * @{
  */
/******* IRQHandler and Callbacks used in non-blocking mode (Interrupt) */
void HAL_ICACHE_IRQHandler(void);
void HAL_ICACHE_InvalidateCompleteCallback(void);
void HAL_ICACHE_ErrorCallback(void);

/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#endif /* ICACHE */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_ICACHE_H */
