/**
  ******************************************************************************
  * @file    stm32n6xx_ll_rng.h
  * @author  MCD Application Team
  * @brief   Header file of RNG LL module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32N6xx_LL_RNG_H
#define STM32N6xx_LL_RNG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx.h"

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

#if defined (RNG)

/** @defgroup RNG_LL RNG
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
#if defined(USE_FULL_LL_DRIVER)
/** @defgroup RNG_LL_ES_Init_Struct RNG Exported Init structures
  * @{
  */


/**
  * @brief LL RNG Init Structure Definition
  */
typedef struct
{
  uint32_t         ClockErrorDetection; /*!< Clock error detection.
                                      This parameter can be one value of @ref RNG_LL_CED.
                                      This parameter can be modified using unitary
                                      functions @ref LL_RNG_EnableClkErrorDetect(). */
} LL_RNG_InitTypeDef;

/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */
/* Exported constants --------------------------------------------------------*/
/** @defgroup RNG_LL_Exported_Constants RNG Exported Constants
  * @{
  */

/** @defgroup RNG_LL_CED Clock Error Detection
  * @{
  */
#define LL_RNG_CED_ENABLE         0x00000000U              /*!< Clock error detection enabled  */
#define LL_RNG_CED_DISABLE        RNG_CR_CED               /*!< Clock error detection disabled */
/**
  * @}
  */
/** @defgroup RNG_LL_ARDIS Auto reset disable
  * @{
  */
#define LL_RNG_ARDIS_ENABLE         0x00000000U              /*!< ARDIS enabled automatic reset to clear SECS bit*/
#define LL_RNG_ARDIS_DISABLE        RNG_CR_ARDIS             /*!< ARDIS disabled no automatic reset to clear SECS bit*/
/**
  * @}
  */

/** @defgroup RNG_LL_Clock_Divider_Factor  Value used to configure an internal
  *            programmable divider acting on the incoming RNG clock
  * @{
  */
#define LL_RNG_CLKDIV_BY_1       (0x00000000UL)                                                           /*!< No clock division                             */
#define LL_RNG_CLKDIV_BY_2       (RNG_CR_CLKDIV_0)                                                        /*!< 2 RNG clock cycles per internal RNG clock     */
#define LL_RNG_CLKDIV_BY_4       (RNG_CR_CLKDIV_1)                                                        /*!< 4 RNG clock cycles per internal RNG clock     */
#define LL_RNG_CLKDIV_BY_8       (RNG_CR_CLKDIV_1 | RNG_CR_CLKDIV_0)                                      /*!< 8 RNG clock cycles per internal RNG clock     */
#define LL_RNG_CLKDIV_BY_16      (RNG_CR_CLKDIV_2)                                                        /*!< 16 RNG clock cycles per internal RNG clock    */
#define LL_RNG_CLKDIV_BY_32      (RNG_CR_CLKDIV_2 | RNG_CR_CLKDIV_0)                                      /*!< 32 RNG clock cycles per internal RNG clock    */
#define LL_RNG_CLKDIV_BY_64      (RNG_CR_CLKDIV_2 | RNG_CR_CLKDIV_1)                                      /*!< 64 RNG clock cycles per internal RNG clock    */
#define LL_RNG_CLKDIV_BY_128     (RNG_CR_CLKDIV_2 | RNG_CR_CLKDIV_1 | RNG_CR_CLKDIV_0)                    /*!< 128 RNG clock cycles per internal RNG clock   */
#define LL_RNG_CLKDIV_BY_256     (RNG_CR_CLKDIV_3)                                                        /*!< 256 RNG clock cycles per internal RNG clock   */
#define LL_RNG_CLKDIV_BY_512     (RNG_CR_CLKDIV_3 | RNG_CR_CLKDIV_0)                                      /*!< 512 RNG clock cycles per internal RNG clock   */
#define LL_RNG_CLKDIV_BY_1024    (RNG_CR_CLKDIV_3 | RNG_CR_CLKDIV_1)                                      /*!< 1024 RNG clock cycles per internal RNG clock  */
#define LL_RNG_CLKDIV_BY_2048    (RNG_CR_CLKDIV_3 | RNG_CR_CLKDIV_1 | RNG_CR_CLKDIV_0)                    /*!< 2048 RNG clock cycles per internal RNG clock  */
#define LL_RNG_CLKDIV_BY_4096    (RNG_CR_CLKDIV_3 | RNG_CR_CLKDIV_2)                                      /*!< 4096 RNG clock cycles per internal RNG clock  */
#define LL_RNG_CLKDIV_BY_8192    (RNG_CR_CLKDIV_3 | RNG_CR_CLKDIV_2 | RNG_CR_CLKDIV_0)                    /*!< 8192 RNG clock cycles per internal RNG clock  */
#define LL_RNG_CLKDIV_BY_16384   (RNG_CR_CLKDIV_3 | RNG_CR_CLKDIV_2 | RNG_CR_CLKDIV_1)                    /*!< 16384 RNG clock cycles per internal RNG clock */
#define LL_RNG_CLKDIV_BY_32768   (RNG_CR_CLKDIV_3 | RNG_CR_CLKDIV_2 | RNG_CR_CLKDIV_1 | RNG_CR_CLKDIV_0)  /*!< 32768 RNG clock cycles per internal RNG clock */
/**
  * @}
  */

/** @defgroup RNG_LL_NIST_Compliance  NIST Compliance configuration
  * @{
  */
#define LL_RNG_NIST_COMPLIANT     (0x00000000UL) /*!< Default NIST compliant configuration*/
#define LL_RNG_CUSTOM_NIST        (RNG_CR_NISTC) /*!< Custom NIST configuration */

/**
  * @}
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
__STATIC_INLINE uint32_t LL_RNG_IsEnabled(const RNG_TypeDef *RNGx)
{
  return ((READ_BIT(RNGx->CR, RNG_CR_RNGEN) == (RNG_CR_RNGEN)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Clock Error Detection
  * @rmtoll CR           CED           LL_RNG_EnableClkErrorDetect
  * @param  RNGx RNG Instance
  * @retval None
  */
__STATIC_INLINE void LL_RNG_EnableClkErrorDetect(RNG_TypeDef *RNGx)
{
  MODIFY_REG(RNGx->CR, RNG_CR_CED | RNG_CR_CONDRST, LL_RNG_CED_ENABLE | RNG_CR_CONDRST);
  CLEAR_BIT(RNGx->CR, RNG_CR_CONDRST);
}

/**
  * @brief  Disable RNG Clock Error Detection
  * @rmtoll CR           CED         LL_RNG_DisableClkErrorDetect
  * @param  RNGx RNG Instance
  * @retval None
  */
__STATIC_INLINE void LL_RNG_DisableClkErrorDetect(RNG_TypeDef *RNGx)
{
  MODIFY_REG(RNGx->CR, RNG_CR_CED | RNG_CR_CONDRST, LL_RNG_CED_DISABLE | RNG_CR_CONDRST);
  CLEAR_BIT(RNGx->CR, RNG_CR_CONDRST);
}

/**
  * @brief  Check if RNG Clock Error Detection is enabled
  * @rmtoll CR           CED         LL_RNG_IsEnabledClkErrorDetect
  * @param  RNGx RNG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RNG_IsEnabledClkErrorDetect(const RNG_TypeDef *RNGx)
{
  return ((READ_BIT(RNGx->CR, RNG_CR_CED) != (RNG_CR_CED)) ? 1UL : 0UL);
}

/**
  * @brief  Set RNG Conditioning Soft Reset bit
  * @rmtoll CR           CONDRST          LL_RNG_EnableCondReset
  * @param  RNGx RNG Instance
  * @retval None
  */
__STATIC_INLINE void LL_RNG_EnableCondReset(RNG_TypeDef *RNGx)
{
  SET_BIT(RNGx->CR, RNG_CR_CONDRST);
}

/**
  * @brief  Reset RNG  Conditioning Soft Reset bit
  * @rmtoll CR           CONDRST         LL_RNG_DisableCondReset
  * @param  RNGx RNG Instance
  * @retval None
  */
__STATIC_INLINE void LL_RNG_DisableCondReset(RNG_TypeDef *RNGx)
{
  CLEAR_BIT(RNGx->CR, RNG_CR_CONDRST);
}

/**
  * @brief  Check if RNG Conditioning Soft Reset bit is set
  * @rmtoll CR           CONDRST         LL_RNG_IsEnabledCondReset
  * @param  RNGx RNG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RNG_IsEnabledCondReset(const RNG_TypeDef *RNGx)
{
  return ((READ_BIT(RNGx->CR, RNG_CR_CONDRST) == (RNG_CR_CONDRST)) ? 1UL : 0UL);
}

/**
  * @brief  Enable RNG Config Lock
  * @rmtoll CR           CONFIGLOCK          LL_RNG_ConfigLock
  * @param  RNGx RNG Instance
  * @retval None
  */
__STATIC_INLINE void LL_RNG_ConfigLock(RNG_TypeDef *RNGx)
{
  SET_BIT(RNGx->CR, RNG_CR_CONFIGLOCK);
}

/**
  * @brief  Check if RNG Config Lock is enabled
  * @rmtoll CR           CONFIGLOCK         LL_RNG_IsConfigLocked
  * @param  RNGx RNG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RNG_IsConfigLocked(const RNG_TypeDef *RNGx)
{
  return ((READ_BIT(RNGx->CR, RNG_CR_CONFIGLOCK) == (RNG_CR_CONFIGLOCK)) ? 1UL : 0UL);
}

/**
  * @brief  Enable NIST Compliance
  * @rmtoll CR           NISTC         LL_RNG_EnableNistCompliance
  * @param  RNGx RNG Instance
  * @retval None
  */
__STATIC_INLINE void LL_RNG_EnableNistCompliance(RNG_TypeDef *RNGx)
{
  MODIFY_REG(RNGx->CR, RNG_CR_NISTC | RNG_CR_CONDRST, LL_RNG_NIST_COMPLIANT | RNG_CR_CONDRST);
  CLEAR_BIT(RNGx->CR, RNG_CR_CONDRST);
}

/**
  * @brief  Disable NIST Compliance
  * @rmtoll CR           NISTC         LL_RNG_DisableNistCompliance
  * @param  RNGx RNG Instance
  * @retval None
  */
__STATIC_INLINE void LL_RNG_DisableNistCompliance(RNG_TypeDef *RNGx)
{
  MODIFY_REG(RNGx->CR, RNG_CR_NISTC | RNG_CR_CONDRST, LL_RNG_CUSTOM_NIST | RNG_CR_CONDRST);
  CLEAR_BIT(RNGx->CR, RNG_CR_CONDRST);
}

/**
  * @brief  Check if NIST Compliance is enabled
  * @rmtoll CR           NISTC         LL_RNG_IsEnabledNistCompliance
  * @param  RNGx RNG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RNG_IsEnabledNistCompliance(const RNG_TypeDef *RNGx)
{
  return ((READ_BIT(RNGx->CR, RNG_CR_NISTC) != (RNG_CR_NISTC)) ? 1UL : 0UL);
}

/**
  * @brief  Set RNG  Config1 Configuration field value
  * @rmtoll CR           RNG_CONFIG1         LL_RNG_SetConfig1
  * @param  RNGx RNG Instance
  * @param  Config1 Value between 0 and 0x3F
  * @retval None
  */
__STATIC_INLINE void LL_RNG_SetConfig1(RNG_TypeDef *RNGx, uint32_t Config1)
{
  MODIFY_REG(RNGx->CR, RNG_CR_RNG_CONFIG1 | RNG_CR_CONDRST, (Config1 << RNG_CR_RNG_CONFIG1_Pos) | RNG_CR_CONDRST);
  CLEAR_BIT(RNGx->CR, RNG_CR_CONDRST);
}

/**
  * @brief  Get RNG  Config1 Configuration field value
  * @rmtoll CR           RNG_CONFIG1         LL_RNG_GetConfig1
  * @param  RNGx RNG Instance
  * @retval Returned Value expressed on 6 bits : Value between 0 and 0x3F
  */
__STATIC_INLINE uint32_t LL_RNG_GetConfig1(const RNG_TypeDef *RNGx)
{
  return (uint32_t)(READ_BIT(RNGx->CR, RNG_CR_RNG_CONFIG1) >> RNG_CR_RNG_CONFIG1_Pos);
}

/**
  * @brief  Set RNG  Config2 Configuration field value
  * @rmtoll CR           RNG_CONFIG2         LL_RNG_SetConfig2
  * @param  RNGx RNG Instance
  * @param  Config2 Value between 0 and 0x7
  * @retval None
  */
__STATIC_INLINE void LL_RNG_SetConfig2(RNG_TypeDef *RNGx, uint32_t Config2)
{
  MODIFY_REG(RNGx->CR, RNG_CR_RNG_CONFIG2 | RNG_CR_CONDRST, (Config2 << RNG_CR_RNG_CONFIG2_Pos) | RNG_CR_CONDRST);
  CLEAR_BIT(RNGx->CR, RNG_CR_CONDRST);
}

/**
  * @brief  Get RNG  Config2 Configuration field value
  * @rmtoll CR           RNG_CONFIG2         LL_RNG_GetConfig2
  * @param  RNGx RNG Instance
  * @retval Returned Value expressed on 3 bits : Value between 0 and 0x7
  */
__STATIC_INLINE uint32_t LL_RNG_GetConfig2(const RNG_TypeDef *RNGx)
{
  return (uint32_t)(READ_BIT(RNGx->CR, RNG_CR_RNG_CONFIG2) >> RNG_CR_RNG_CONFIG2_Pos);
}

/**
  * @brief  Set RNG  Config3 Configuration field value
  * @rmtoll CR           RNG_CONFIG3         LL_RNG_SetConfig3
  * @param  RNGx RNG Instance
  * @param  Config3 Value between 0 and 0xF
  * @retval None
  */
__STATIC_INLINE void LL_RNG_SetConfig3(RNG_TypeDef *RNGx, uint32_t Config3)
{
  MODIFY_REG(RNGx->CR, RNG_CR_RNG_CONFIG3 | RNG_CR_CONDRST, (Config3 << RNG_CR_RNG_CONFIG3_Pos) | RNG_CR_CONDRST);
  CLEAR_BIT(RNGx->CR, RNG_CR_CONDRST);
}

/**
  * @brief  Get RNG  Config3 Configuration field value
  * @rmtoll CR           RNG_CONFIG3         LL_RNG_GetConfig3
  * @param  RNGx RNG Instance
  * @retval Returned Value expressed on 4 bits : Value between 0 and 0xF
  */
__STATIC_INLINE uint32_t LL_RNG_GetConfig3(const RNG_TypeDef *RNGx)
{
  return (uint32_t)(READ_BIT(RNGx->CR, RNG_CR_RNG_CONFIG3) >> RNG_CR_RNG_CONFIG3_Pos);
}

/**
  * @brief  Set RNG  Clock divider factor
  * @rmtoll CR           CLKDIV         LL_RNG_SetClockDivider
  * @param  RNGx RNG Instance
  * @param  Divider can be one of the following values:
  *         @arg @ref LL_RNG_CLKDIV_BY_1
  *         @arg @ref LL_RNG_CLKDIV_BY_2
  *         @arg @ref LL_RNG_CLKDIV_BY_4
  *         @arg @ref LL_RNG_CLKDIV_BY_8
  *         @arg @ref LL_RNG_CLKDIV_BY_16
  *         @arg @ref LL_RNG_CLKDIV_BY_32
  *         @arg @ref LL_RNG_CLKDIV_BY_64
  *         @arg @ref LL_RNG_CLKDIV_BY_128
  *         @arg @ref LL_RNG_CLKDIV_BY_256
  *         @arg @ref LL_RNG_CLKDIV_BY_512
  *         @arg @ref LL_RNG_CLKDIV_BY_1024
  *         @arg @ref LL_RNG_CLKDIV_BY_2048
  *         @arg @ref LL_RNG_CLKDIV_BY_4096
  *         @arg @ref LL_RNG_CLKDIV_BY_8192
  *         @arg @ref LL_RNG_CLKDIV_BY_16384
  *         @arg @ref LL_RNG_CLKDIV_BY_32768
  * @retval None
  */
__STATIC_INLINE void LL_RNG_SetClockDivider(RNG_TypeDef *RNGx, uint32_t Divider)
{
  MODIFY_REG(RNGx->CR, RNG_CR_CLKDIV | RNG_CR_CONDRST, Divider | RNG_CR_CONDRST);
  CLEAR_BIT(RNGx->CR, RNG_CR_CONDRST);
}

/**
  * @brief  Get RNG  Clock divider factor
  * @rmtoll CR           CLKDIV         LL_RNG_GetClockDivider
  * @param  RNGx RNG Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_RNG_CLKDIV_BY_1
  *         @arg @ref LL_RNG_CLKDIV_BY_2
  *         @arg @ref LL_RNG_CLKDIV_BY_4
  *         @arg @ref LL_RNG_CLKDIV_BY_8
  *         @arg @ref LL_RNG_CLKDIV_BY_16
  *         @arg @ref LL_RNG_CLKDIV_BY_32
  *         @arg @ref LL_RNG_CLKDIV_BY_64
  *         @arg @ref LL_RNG_CLKDIV_BY_128
  *         @arg @ref LL_RNG_CLKDIV_BY_256
  *         @arg @ref LL_RNG_CLKDIV_BY_512
  *         @arg @ref LL_RNG_CLKDIV_BY_1024
  *         @arg @ref LL_RNG_CLKDIV_BY_2048
  *         @arg @ref LL_RNG_CLKDIV_BY_4096
  *         @arg @ref LL_RNG_CLKDIV_BY_8192
  *         @arg @ref LL_RNG_CLKDIV_BY_16384
  *         @arg @ref LL_RNG_CLKDIV_BY_32768
  */
__STATIC_INLINE uint32_t LL_RNG_GetClockDivider(const RNG_TypeDef *RNGx)
{
  return (uint32_t)READ_BIT(RNGx->CR, RNG_CR_CLKDIV);
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
__STATIC_INLINE uint32_t LL_RNG_IsActiveFlag_DRDY(const RNG_TypeDef *RNGx)
{
  return ((READ_BIT(RNGx->SR, RNG_SR_DRDY) == (RNG_SR_DRDY)) ? 1UL : 0UL);
}

/**
  * @brief  Indicate if the Clock Error Current Status Flag is set or not
  * @rmtoll SR           CECS          LL_RNG_IsActiveFlag_CECS
  * @param  RNGx RNG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RNG_IsActiveFlag_CECS(const RNG_TypeDef *RNGx)
{
  return ((READ_BIT(RNGx->SR, RNG_SR_CECS) == (RNG_SR_CECS)) ? 1UL : 0UL);
}

/**
  * @brief  Indicate if the Seed Error Current Status Flag is set or not
  * @rmtoll SR           SECS          LL_RNG_IsActiveFlag_SECS
  * @param  RNGx RNG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RNG_IsActiveFlag_SECS(const RNG_TypeDef *RNGx)
{
  return ((READ_BIT(RNGx->SR, RNG_SR_SECS) == (RNG_SR_SECS)) ? 1UL : 0UL);
}

/**
  * @brief  Indicate if the Clock Error Interrupt Status Flag is set or not
  * @rmtoll SR           CEIS          LL_RNG_IsActiveFlag_CEIS
  * @param  RNGx RNG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RNG_IsActiveFlag_CEIS(const RNG_TypeDef *RNGx)
{
  return ((READ_BIT(RNGx->SR, RNG_SR_CEIS) == (RNG_SR_CEIS)) ? 1UL : 0UL);
}

/**
  * @brief  Indicate if the Seed Error Interrupt Status Flag is set or not
  * @rmtoll SR           SEIS          LL_RNG_IsActiveFlag_SEIS
  * @param  RNGx RNG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RNG_IsActiveFlag_SEIS(const RNG_TypeDef *RNGx)
{
  return ((READ_BIT(RNGx->SR, RNG_SR_SEIS) == (RNG_SR_SEIS)) ? 1UL : 0UL);
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
__STATIC_INLINE uint32_t LL_RNG_IsEnabledIT(const RNG_TypeDef *RNGx)
{
  return ((READ_BIT(RNGx->CR, RNG_CR_IE) == (RNG_CR_IE)) ? 1UL : 0UL);
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
__STATIC_INLINE uint32_t LL_RNG_ReadRandData32(const RNG_TypeDef *RNGx)
{
  return (uint32_t)(READ_REG(RNGx->DR));
}

/**
  * @}
  */

/**
  * @brief  Enable Auto reset
  * @rmtoll CR           ARDIS           LL_RNG_EnableArdis
  * @param  RNGx RNG Instance
  * @retval None
  */
__STATIC_INLINE void LL_RNG_EnableArdis(RNG_TypeDef *RNGx)
{
  MODIFY_REG(RNGx->CR, RNG_CR_ARDIS | RNG_CR_CONDRST, LL_RNG_ARDIS_ENABLE | RNG_CR_CONDRST);
  CLEAR_BIT(RNGx->CR, RNG_CR_CONDRST);
}

/**
  * @brief  Disable Auto reset
  * @rmtoll CR           ARDIS         LL_RNG_DisableArdis
  * @param  RNGx RNG Instance
  * @retval None
  */
__STATIC_INLINE void LL_RNG_DisableArdis(RNG_TypeDef *RNGx)
{
  MODIFY_REG(RNGx->CR, RNG_CR_ARDIS | RNG_CR_CONDRST, LL_RNG_ARDIS_DISABLE | RNG_CR_CONDRST);
  CLEAR_BIT(RNGx->CR, RNG_CR_CONDRST);
}

/**
  * @brief  Check if RNG Auto reset is enabled
  * @rmtoll CR           ARDIS         LL_RNG_IsEnabledArdis
  * @param  RNGx RNG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RNG_IsEnabledArdis(const RNG_TypeDef *RNGx)
{
  return ((READ_BIT(RNGx->CR, RNG_CR_ARDIS) != (RNG_CR_ARDIS)) ? 1UL : 0UL);
}

/** @defgroup RNG_LL_EF_Health_Test_Control Health Test Control
  * @{
  */

/**
  * @brief  Set RNG Health Test Control
  * @rmtoll HTCR       HTCFG       LL_RNG_SetHealthConfig
  * @param  RNGx RNG Instance
  * @param  HTCFG can be values of 32 bits
  * @retval None
  */
__STATIC_INLINE void LL_RNG_SetHealthConfig(RNG_TypeDef *RNGx, uint32_t HTCFG)
{
  WRITE_REG(RNGx->HTCR, HTCFG);
}

/**
  * @brief  Get RNG Health Test Control
  * @rmtoll HTCR         HTCFG        LL_RNG_GetHealthConfig
  * @param  RNGx RNG Instance
  * @retval Return 32-bit RNG Health Test configuration
  */
__STATIC_INLINE uint32_t LL_RNG_GetHealthConfig(const RNG_TypeDef *RNGx)
{
  return (uint32_t)READ_REG(RNGx->HTCR);
}

/**
  * @}
  */
#if defined(USE_FULL_LL_DRIVER)
/** @defgroup RNG_LL_EF_Init Initialization and de-initialization functions
  * @{
  */
ErrorStatus LL_RNG_Init(RNG_TypeDef *RNGx, const LL_RNG_InitTypeDef *RNG_InitStruct);
void LL_RNG_StructInit(LL_RNG_InitTypeDef *RNG_InitStruct);
ErrorStatus LL_RNG_DeInit(const RNG_TypeDef *RNGx);

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

#endif /* RNG */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32N6xx_LL_RNG_H */

