/**
  ******************************************************************************
  * @file    stm32n6xx_ll_icache.h
  * @author  MCD Application Team
  * @brief   Header file of ICACHE LL module.
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
#ifndef STM32N6xx_LL_ICACHE_H
#define STM32N6xx_LL_ICACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes -----------------------------------------------------------------*/
#include "stm32n6xx.h"

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

#if defined(ICACHE)

/** @defgroup ICACHE_LL ICACHE
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/* Exported constants -------------------------------------------------------*/
/** @defgroup ICACHE_LL_Exported_Constants ICACHE Exported Constants
  * @{
  */

/** @defgroup ICACHE_LL_EC_WaysSelection Ways selection
  * @{
  */
#define LL_ICACHE_1WAY                 0U                /*!< 1-way cache (direct mapped cache) */
#define LL_ICACHE_4WAYS                ICACHE_CR_WAYSEL  /*!< 4-ways set associative cache (default) */
/**
  * @}
  */

/** @defgroup ICACHE_LL_EC_Monitor_Type Monitor type
  * @{
  */
#define LL_ICACHE_MONITOR_HIT          ICACHE_CR_HITMEN                       /*!< Hit monitor counter */
#define LL_ICACHE_MONITOR_MISS         ICACHE_CR_MISSMEN                      /*!< Miss monitor counter */
#define LL_ICACHE_MONITOR_ALL          (ICACHE_CR_HITMEN | ICACHE_CR_MISSMEN) /*!< All monitors counters */
/**
  * @}
  */

/** @defgroup ICACHE_LL_EC_GET_FLAG Get Flags Defines
  * @brief    Flags defines which can be used with LL_ICACHE_ReadReg function
  * @{
  */
#define LL_ICACHE_SR_BUSYF             ICACHE_SR_BUSYF     /*!< Busy flag */
#define LL_ICACHE_SR_BSYENDF           ICACHE_SR_BSYENDF   /*!< Busy end flag */
#define LL_ICACHE_SR_ERRF              ICACHE_SR_ERRF      /*!< Cache error flag */
/**
  * @}
  */

/** @defgroup ICACHE_LL_EC_CLEAR_FLAG Clear Flags Defines
  * @brief    Flags defines which can be used with LL_ICACHE_WriteReg function
  * @{
  */
#define LL_ICACHE_FCR_CBSYENDF         ICACHE_FCR_CBSYENDF /*!< Busy end flag */
#define LL_ICACHE_FCR_CERRF            ICACHE_FCR_CERRF    /*!< Cache error flag */
/**
  * @}
  */

/** @defgroup ICACHE_LL_EC_IT IT Defines
  * @brief    IT defines which can be used with LL_ICACHE_ReadReg and  LL_ICACHE_WriteReg functions
  * @{
  */
#define LL_ICACHE_IER_BSYENDIE         ICACHE_IER_BSYENDIE /*!< Busy end interrupt */
#define LL_ICACHE_IER_ERRIE            ICACHE_IER_ERRIE    /*!< Cache error interrupt */
/**
  * @}
  */


/**
  * @}
  */

/* Exported macros ----------------------------------------------------------*/
/** @defgroup ICACHE_LL_Exported_Macros ICACHE Exported Macros
  * @{
  */

/** @defgroup ICACHE_LL_EM_WRITE_READ Common write and read registers Macros
  * @{
  */

/**
  * @brief  Write a value in ICACHE register
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_ICACHE_WriteReg(__REG__, __VALUE__) WRITE_REG(ICACHE->__REG__, (__VALUE__))

/**
  * @brief  Read a value in ICACHE register
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_ICACHE_ReadReg(__REG__) READ_REG(ICACHE->__REG__)
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup ICACHE_LL_Exported_Functions ICACHE Exported Functions
  * @{
  */

/** @defgroup ICACHE_LL_EF_Configuration Configuration
  * @{
  */

/**
  * @brief  Enable the ICACHE.
  * @rmtoll CR           EN            LL_ICACHE_Enable
  * @retval None
  */
__STATIC_INLINE void LL_ICACHE_Enable(void)
{
  SET_BIT(ICACHE->CR, ICACHE_CR_EN);
}

/**
  * @brief  Disable the ICACHE.
  * @rmtoll CR           EN            LL_ICACHE_Disable
  * @retval None
  */
__STATIC_INLINE void LL_ICACHE_Disable(void)
{
  CLEAR_BIT(ICACHE->CR, ICACHE_CR_EN);
}

/**
  * @brief  Return if ICACHE is enabled or not.
  * @rmtoll CR           EN            LL_ICACHE_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ICACHE_IsEnabled(void)
{
  return ((READ_BIT(ICACHE->CR, ICACHE_CR_EN) == (ICACHE_CR_EN)) ? 1UL : 0UL);
}

/**
  * @brief  Select the ICACHE operating mode.
  * @rmtoll CR           WAYSEL        LL_ICACHE_SetMode
  * @param  Mode This parameter can be one of the following values:
  *         @arg @ref LL_ICACHE_1WAY
  *         @arg @ref LL_ICACHE_4WAYS
  * @retval None
  */
__STATIC_INLINE void LL_ICACHE_SetMode(uint32_t Mode)
{
  MODIFY_REG(ICACHE->CR, ICACHE_CR_WAYSEL, Mode);
}

/**
  * @brief  Get the selected ICACHE operating mode.
  * @rmtoll CR           WAYSEL        LL_ICACHE_GetMode
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ICACHE_1WAY
  *         @arg @ref LL_ICACHE_4WAYS
  */
__STATIC_INLINE uint32_t LL_ICACHE_GetMode(void)
{
  return (READ_BIT(ICACHE->CR, ICACHE_CR_WAYSEL));
}

/**
  * @brief  Invalidate the ICACHE.
  * @note   Until the BSYEND flag is set, the cache is bypassed.
  * @rmtoll CR           CACHEINV      LL_ICACHE_Invalidate
  * @retval None
  */
__STATIC_INLINE void LL_ICACHE_Invalidate(void)
{
  SET_BIT(ICACHE->CR, ICACHE_CR_CACHEINV);
}

/**
  * @}
  */

/** @defgroup ICACHE_LL_EF_Monitors Monitors
  * @{
  */

/**
  * @brief  Enable the hit/miss monitor(s).
  * @rmtoll CR           HITMEN        LL_ICACHE_EnableMonitors
  * @rmtoll CR           MISSMEN       LL_ICACHE_EnableMonitors
  * @param  Monitors This parameter can be one or a combination of the following values:
  *         @arg @ref LL_ICACHE_MONITOR_HIT
  *         @arg @ref LL_ICACHE_MONITOR_MISS
  *         @arg @ref LL_ICACHE_MONITOR_ALL
  * @retval None
  */
__STATIC_INLINE void LL_ICACHE_EnableMonitors(uint32_t Monitors)
{
  SET_BIT(ICACHE->CR, Monitors);
}

/**
  * @brief  Disable the hit/miss monitor(s).
  * @rmtoll CR           HITMEN        LL_ICACHE_DisableMonitors
  * @rmtoll CR           MISSMEN       LL_ICACHE_DisableMonitors
  * @param  Monitors This parameter can be one or a combination of the following values:
  *         @arg @ref LL_ICACHE_MONITOR_HIT
  *         @arg @ref LL_ICACHE_MONITOR_MISS
  *         @arg @ref LL_ICACHE_MONITOR_ALL
  * @retval None
  */
__STATIC_INLINE void LL_ICACHE_DisableMonitors(uint32_t Monitors)
{
  CLEAR_BIT(ICACHE->CR, Monitors);
}

/**
  * @brief  Check if the monitor(s) is(are) enabled or disabled.
  * @rmtoll CR           HITMEN        LL_ICACHE_IsEnabledMonitors
  * @rmtoll CR           MISSMEN       LL_ICACHE_IsEnabledMonitors
  * @param  Monitors This parameter can be one or a combination of the following values:
  *         @arg @ref LL_ICACHE_MONITOR_HIT
  *         @arg @ref LL_ICACHE_MONITOR_MISS
  *         @arg @ref LL_ICACHE_MONITOR_ALL
  * @retval State of parameter value (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ICACHE_IsEnabledMonitors(uint32_t Monitors)
{
  return ((READ_BIT(ICACHE->CR, Monitors) == (Monitors)) ? 1UL : 0UL);
}

/**
  * @brief  Reset the hit/miss monitor(s).
  * @rmtoll CR           HITMRST       LL_ICACHE_ResetMonitors
  * @rmtoll CR           MISSMRST      LL_ICACHE_ResetMonitors
  * @param  Monitors This parameter can be one or a combination of the following values:
  *         @arg @ref LL_ICACHE_MONITOR_HIT
  *         @arg @ref LL_ICACHE_MONITOR_MISS
  *         @arg @ref LL_ICACHE_MONITOR_ALL
  * @retval None
  */
__STATIC_INLINE void LL_ICACHE_ResetMonitors(uint32_t Monitors)
{
  /* Reset */
  SET_BIT(ICACHE->CR, (Monitors << 2U));
  /* Release reset */
  CLEAR_BIT(ICACHE->CR, (Monitors << 2U));
}

/**
  * @brief  Get the Hit monitor.
  * @note   Upon reaching the 32-bit maximum value, hit monitor does not wrap.
  * @rmtoll HMONR        HITMON        LL_ICACHE_GetHitMonitor
  * @retval Value between Min_Data=0 and Max_Data=0xFFFFFFFF
  */
__STATIC_INLINE uint32_t LL_ICACHE_GetHitMonitor(void)
{
  return (ICACHE->HMONR);
}

/**
  * @brief  Get the Miss monitor.
  * @note   Upon reaching the 16-bit maximum value, miss monitor does not wrap.
  * @rmtoll MMONR        MISSMON       LL_ICACHE_GetMissMonitor
  * @retval Value between Min_Data=0 and Max_Data=0xFFFF
  */
__STATIC_INLINE uint32_t LL_ICACHE_GetMissMonitor(void)
{
  return (ICACHE->MMONR);
}

/**
  * @}
  */

/** @defgroup ICACHE_LL_EF_IT_Management IT_Management
  * @{
  */

/**
  * @brief  Enable BSYEND interrupt.
  * @rmtoll IER          BSYENDIE      LL_ICACHE_EnableIT_BSYEND
  * @retval None
  */
__STATIC_INLINE void LL_ICACHE_EnableIT_BSYEND(void)
{
  SET_BIT(ICACHE->IER, ICACHE_IER_BSYENDIE);
}

/**
  * @brief  Disable BSYEND interrupt.
  * @rmtoll IER          BSYENDIE      LL_ICACHE_DisableIT_BSYEND
  * @retval None
  */
__STATIC_INLINE void LL_ICACHE_DisableIT_BSYEND(void)
{
  CLEAR_BIT(ICACHE->IER, ICACHE_IER_BSYENDIE);
}

/**
  * @brief  Check if the BSYEND Interrupt is enabled or disabled.
  * @rmtoll IER          BSYENDIE      LL_ICACHE_IsEnabledIT_BSYEND
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ICACHE_IsEnabledIT_BSYEND(void)
{
  return ((READ_BIT(ICACHE->IER, ICACHE_IER_BSYENDIE) == (ICACHE_IER_BSYENDIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable ERR interrupt.
  * @rmtoll IER          ERRIE         LL_ICACHE_EnableIT_ERR
  * @retval None
  */
__STATIC_INLINE void LL_ICACHE_EnableIT_ERR(void)
{
  SET_BIT(ICACHE->IER, ICACHE_IER_ERRIE);
}

/**
  * @brief  Disable ERR interrupt.
  * @rmtoll IER          ERRIE        LL_ICACHE_DisableIT_ERR
  * @retval None
  */
__STATIC_INLINE void LL_ICACHE_DisableIT_ERR(void)
{
  CLEAR_BIT(ICACHE->IER, ICACHE_IER_ERRIE);
}

/**
  * @brief  Check if the ERR Interrupt is enabled or disabled.
  * @rmtoll IER          ERRIE         LL_ICACHE_IsEnabledIT_ERR
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ICACHE_IsEnabledIT_ERR(void)
{
  return ((READ_BIT(ICACHE->IER, ICACHE_IER_ERRIE) == (ICACHE_IER_ERRIE)) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup ICACHE_LL_EF_FLAG_Management FLAG_Management
  * @{
  */

/**
  * @brief  Indicate the status of an ongoing operation flag.
  * @rmtoll SR           BUSYF         LL_ICACHE_IsActiveFlag_BUSY
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ICACHE_IsActiveFlag_BUSY(void)
{
  return ((READ_BIT(ICACHE->SR, ICACHE_SR_BUSYF) == (ICACHE_SR_BUSYF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicate the status of an operation end flag.
  * @rmtoll SR           BSYEND       LL_ICACHE_IsActiveFlag_BSYEND
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ICACHE_IsActiveFlag_BSYEND(void)
{
  return ((READ_BIT(ICACHE->SR, ICACHE_SR_BSYENDF) == (ICACHE_SR_BSYENDF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicate the status of an error flag.
  * @rmtoll SR           ERRF          LL_ICACHE_IsActiveFlag_ERR
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ICACHE_IsActiveFlag_ERR(void)
{
  return ((READ_BIT(ICACHE->SR, ICACHE_SR_ERRF) == (ICACHE_SR_ERRF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear busy end of operation flag.
  * @rmtoll FCR          CBSYENDF      LL_ICACHE_ClearFlag_BSYEND
  * @retval None
  */
__STATIC_INLINE void LL_ICACHE_ClearFlag_BSYEND(void)
{
  WRITE_REG(ICACHE->FCR, ICACHE_FCR_CBSYENDF);
}

/**
  * @brief  Clear error flag.
  * @rmtoll FCR          ERRF          LL_ICACHE_ClearFlag_ERR
  * @retval None
  */
__STATIC_INLINE void LL_ICACHE_ClearFlag_ERR(void)
{
  WRITE_REG(ICACHE->FCR, ICACHE_FCR_CERRF);
}

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

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_LL_ICACHE_H */
