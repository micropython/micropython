/**
  ******************************************************************************
  * @file    stm32l4xx_ll_rng.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of RNG LL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L4xx_LL_RNG_H
#define __STM32L4xx_LL_RNG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"

/** @addtogroup STM32L4xx_LL_Driver
  * @{
  */

#if defined(RNG)

/** @defgroup RNG_LL RNG
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup RNG_LL_Exported_Constants RNG Exported Constants
  * @{
  */

/** @defgroup RNG_LL_EC_GET_FLAG Get Flags Defines
  * @brief    Flags defines which can be used with LL_RNG_ReadReg function
  * @{
  */
#define LL_RNG_SR_DRDY RNG_SR_DRDY    /*!< Register contains valid random data */
#define LL_RNG_SR_CECS RNG_SR_CECS    /*!< Clock error current status */
#define LL_RNG_SR_SECS RNG_SR_SECS    /*!< Seed error current status */
#define LL_RNG_SR_CEIS RNG_SR_CEIS    /*!< Clock error interrupt status */
#define LL_RNG_SR_SEIS RNG_SR_SEIS    /*!< Seed error interrupt status */
/**
  * @}
  */

/** @defgroup RNG_LL_EC_IT IT Defines
  * @brief    IT defines which can be used with LL_RNG_ReadReg and  LL_RNG_WriteReg macros
  * @{
  */
#define LL_RNG_CR_IE   RNG_CR_IE      /*!< RNG Interrupt enable */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup RNG_LL_Exported_Macros RNG Exported Macros
  * @{
  */

/** @defgroup RNG_LL_EM_WRITE_READ Common Write and read registers Macros
  * @{
  */

/**
  * @brief  Write a value in RNG register
  * @param  __INSTANCE__ RNG Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_RNG_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/**
  * @brief  Read a value in RNG register
  * @param  __INSTANCE__ RNG Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_RNG_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/**
  * @}
  */

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @defgroup RNG_LL_Exported_Functions RNG Exported Functions
  * @{
  */
/** @defgroup RNG_LL_EF_Configuration RNG Configuration functions
  * @{
  */

/**
  * @brief  Enable Random Number Generation
  * @rmtoll CR           RNGEN         LL_RNG_Enable
  * @param  RNGx RNG Instance
  * @retval None
  */
__STATIC_INLINE void LL_RNG_Enable(RNG_TypeDef *RNGx)
{
  SET_BIT(RNGx->CR, RNG_CR_RNGEN);
}

/**
  * @brief  Disable Random Number Generation
  * @rmtoll CR           RNGEN         LL_RNG_Disable
  * @param  RNGx RNG Instance
  * @retval None
  */
__STATIC_INLINE void LL_RNG_Disable(RNG_TypeDef *RNGx)
{
  CLEAR_BIT(RNGx->CR, RNG_CR_RNGEN);
}

/**
  * @brief  Check if Random Number Generator is enabled
  * @rmtoll CR           RNGEN         LL_RNG_IsEnabled
  * @param  RNGx RNG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RNG_IsEnabled(RNG_TypeDef *RNGx)
{
  return (READ_BIT(RNGx->CR, RNG_CR_RNGEN) == (RNG_CR_RNGEN));
}

/**
  * @}
  */

/** @defgroup RNG_LL_EF_FLAG_Management FLAG Management
  * @{
  */

/**
  * @brief  Indicate if the RNG Data ready Flag is set or not
  * @rmtoll SR           DRDY          LL_RNG_IsActiveFlag_DRDY
  * @param  RNGx RNG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RNG_IsActiveFlag_DRDY(RNG_TypeDef *RNGx)
{
  return (READ_BIT(RNGx->SR, RNG_SR_DRDY) == (RNG_SR_DRDY));
}

/**
  * @brief  Indicate if the Clock Error Current Status Flag is set or not
  * @rmtoll SR           CECS          LL_RNG_IsActiveFlag_CECS
  * @param  RNGx RNG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RNG_IsActiveFlag_CECS(RNG_TypeDef *RNGx)
{
  return (READ_BIT(RNGx->SR, RNG_SR_CECS) == (RNG_SR_CECS));
}

/**
  * @brief  Indicate if the Seed Error Current Status Flag is set or not
  * @rmtoll SR           SECS          LL_RNG_IsActiveFlag_SECS
  * @param  RNGx RNG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RNG_IsActiveFlag_SECS(RNG_TypeDef *RNGx)
{
  return (READ_BIT(RNGx->SR, RNG_SR_SECS) == (RNG_SR_SECS));
}

/**
  * @brief  Indicate if the Clock Error Interrupt Status Flag is set or not
  * @rmtoll SR           CEIS          LL_RNG_IsActiveFlag_CEIS
  * @param  RNGx RNG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RNG_IsActiveFlag_CEIS(RNG_TypeDef *RNGx)
{
  return (READ_BIT(RNGx->SR, RNG_SR_CEIS) == (RNG_SR_CEIS));
}

/**
  * @brief  Indicate if the Seed Error Interrupt Status Flag is set or not
  * @rmtoll SR           SEIS          LL_RNG_IsActiveFlag_SEIS
  * @param  RNGx RNG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RNG_IsActiveFlag_SEIS(RNG_TypeDef *RNGx)
{
  return (READ_BIT(RNGx->SR, RNG_SR_SEIS) == (RNG_SR_SEIS));
}

/**
  * @brief  Clear Clock Error interrupt Status (CEIS) Flag
  * @rmtoll SR           CEIS          LL_RNG_ClearFlag_CEIS
  * @param  RNGx RNG Instance
  * @retval None
  */
__STATIC_INLINE void LL_RNG_ClearFlag_CEIS(RNG_TypeDef *RNGx)
{
  WRITE_REG(RNGx->SR, ~RNG_SR_CEIS);
}

/**
  * @brief  Clear Seed Error interrupt Status (SEIS) Flag
  * @rmtoll SR           SEIS          LL_RNG_ClearFlag_SEIS
  * @param  RNGx RNG Instance
  * @retval None
  */
__STATIC_INLINE void LL_RNG_ClearFlag_SEIS(RNG_TypeDef *RNGx)
{
  WRITE_REG(RNGx->SR, ~RNG_SR_SEIS);
}

/**
  * @}
  */

/** @defgroup RNG_LL_EF_IT_Management IT Management
  * @{
  */

/**
  * @brief  Enable Random Number Generator Interrupt
  *         (applies for either Seed error, Clock Error or Data ready interrupts)
  * @rmtoll CR           IE            LL_RNG_EnableIT
  * @param  RNGx RNG Instance
  * @retval None
  */
__STATIC_INLINE void LL_RNG_EnableIT(RNG_TypeDef *RNGx)
{
  SET_BIT(RNGx->CR, RNG_CR_IE);
}

/**
  * @brief  Disable Random Number Generator Interrupt
  *         (applies for either Seed error, Clock Error or Data ready interrupts)
  * @rmtoll CR           IE            LL_RNG_DisableIT
  * @param  RNGx RNG Instance
  * @retval None
  */
__STATIC_INLINE void LL_RNG_DisableIT(RNG_TypeDef *RNGx)
{
  CLEAR_BIT(RNGx->CR, RNG_CR_IE);
}

/**
  * @brief  Check if Random Number Generator Interrupt is enabled
  *         (applies for either Seed error, Clock Error or Data ready interrupts)
  * @rmtoll CR           IE            LL_RNG_IsEnabledIT
  * @param  RNGx RNG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RNG_IsEnabledIT(RNG_TypeDef *RNGx)
{
  return (READ_BIT(RNGx->CR, RNG_CR_IE) == (RNG_CR_IE));
}

/**
  * @}
  */

/** @defgroup RNG_LL_EF_Data_Management Data Management
  * @{
  */

/**
  * @brief  Return32-bit Random Number value
  * @rmtoll DR           RNDATA        LL_RNG_ReadRandData32
  * @param  RNGx RNG Instance
  * @retval Generated 32-bit random value
  */
__STATIC_INLINE uint32_t LL_RNG_ReadRandData32(RNG_TypeDef *RNGx)
{
  return (uint32_t)(READ_REG(RNGx->DR));
}

/**
  * @}
  */

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup RNG_LL_EF_Init Initialization and de-initialization functions
  * @{
  */

ErrorStatus LL_RNG_DeInit(RNG_TypeDef *RNGx);

/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/**
  * @}
  */

/**
  * @}
  */

#endif /* defined(RNG) */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32L4xx_LL_RNG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
