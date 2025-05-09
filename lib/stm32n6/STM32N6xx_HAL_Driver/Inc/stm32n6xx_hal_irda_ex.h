/**
  ******************************************************************************
  * @file    stm32n6xx_hal_irda_ex.h
  * @author  MCD Application Team
  * @brief   Header file of IRDA HAL Extended module.
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
#ifndef STM32N6xx_HAL_IRDA_EX_H
#define STM32N6xx_HAL_IRDA_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup IRDAEx IRDAEx
  * @brief IRDA Extended HAL module driver
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup IRDAEx_Extended_Exported_Constants IRDAEx Extended Exported Constants
  * @{
  */

/** @defgroup IRDAEx_Word_Length IRDAEx Word Length
  * @{
  */
#define IRDA_WORDLENGTH_7B                  USART_CR1_M1   /*!< 7-bit long frame */
#define IRDA_WORDLENGTH_8B                  0x00000000U    /*!< 8-bit long frame */
#define IRDA_WORDLENGTH_9B                  USART_CR1_M0   /*!< 9-bit long frame */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/** @defgroup IRDAEx_Private_Macros IRDAEx Private Macros
  * @{
  */

/** @brief  Report the IRDA clock source.
  * @param  __HANDLE__ specifies the IRDA Handle.
  * @param  __CLOCKSOURCE__ output variable.
  * @retval IRDA clocking source, written in __CLOCKSOURCE__.
  */
/** @brief  Return clock source used for USART instance used for IRDA.
  * @param  __HANDLE__ specifies the UART Handle.
  * @param  __CLOCKSOURCE__ output variable.
  * @retval USART clocking source, written in __CLOCKSOURCE__.
  */
#define IRDA_GETCLOCKSOURCE(__HANDLE__,__CLOCKSOURCE__)           \
  do {                                                            \
    if((__HANDLE__)->Instance == USART1)                          \
    {                                                             \
      (__CLOCKSOURCE__) = (uint64_t)RCC_PERIPHCLK_USART1;         \
    }                                                             \
    else if((__HANDLE__)->Instance == USART2)                     \
    {                                                             \
      (__CLOCKSOURCE__) = (uint64_t)RCC_PERIPHCLK_USART2;         \
    }                                                             \
    else if((__HANDLE__)->Instance == USART3)                     \
    {                                                             \
      (__CLOCKSOURCE__) = (uint64_t)RCC_PERIPHCLK_USART3;         \
    }                                                             \
    else if((__HANDLE__)->Instance == UART4)                      \
    {                                                             \
      (__CLOCKSOURCE__) = (uint64_t)RCC_PERIPHCLK_UART4;          \
    }                                                             \
    else if((__HANDLE__)->Instance == UART5)                      \
    {                                                             \
      (__CLOCKSOURCE__) = (uint64_t)RCC_PERIPHCLK_UART5;          \
    }                                                             \
    else if((__HANDLE__)->Instance == USART6)                     \
    {                                                             \
      (__CLOCKSOURCE__) = (uint64_t)RCC_PERIPHCLK_USART6;         \
    }                                                             \
    else if((__HANDLE__)->Instance == UART7)                      \
    {                                                             \
      (__CLOCKSOURCE__) = (uint64_t)RCC_PERIPHCLK_UART7;          \
    }                                                             \
    else if((__HANDLE__)->Instance == UART8)                      \
    {                                                             \
      (__CLOCKSOURCE__) = (uint64_t)RCC_PERIPHCLK_UART8;          \
    }                                                             \
    else if((__HANDLE__)->Instance == UART9)                      \
    {                                                             \
      (__CLOCKSOURCE__) = (uint64_t)RCC_PERIPHCLK_UART9;          \
    }                                                             \
    else if((__HANDLE__)->Instance == USART10)                    \
    {                                                             \
      (__CLOCKSOURCE__) = (uint64_t)RCC_PERIPHCLK_USART10;        \
    }                                                             \
    else                                                          \
    {                                                             \
      (__CLOCKSOURCE__) = 0U;                                     \
    }                                                             \
  } while(0U)


/** @brief  Compute the mask to apply to retrieve the received data
  *         according to the word length and to the parity bits activation.
  * @param  __HANDLE__ specifies the IRDA Handle.
  * @retval None, the mask to apply to the associated UART RDR register is stored in (__HANDLE__)->Mask field.
  */
#define IRDA_MASK_COMPUTATION(__HANDLE__)                             \
  do {                                                                \
    if ((__HANDLE__)->Init.WordLength == IRDA_WORDLENGTH_9B)          \
    {                                                                 \
      if ((__HANDLE__)->Init.Parity == IRDA_PARITY_NONE)              \
      {                                                               \
        (__HANDLE__)->Mask = 0x01FFU ;                                \
      }                                                               \
      else                                                            \
      {                                                               \
        (__HANDLE__)->Mask = 0x00FFU ;                                \
      }                                                               \
    }                                                                 \
    else if ((__HANDLE__)->Init.WordLength == IRDA_WORDLENGTH_8B)     \
    {                                                                 \
      if ((__HANDLE__)->Init.Parity == IRDA_PARITY_NONE)              \
      {                                                               \
        (__HANDLE__)->Mask = 0x00FFU ;                                \
      }                                                               \
      else                                                            \
      {                                                               \
        (__HANDLE__)->Mask = 0x007FU ;                                \
      }                                                               \
    }                                                                 \
    else if ((__HANDLE__)->Init.WordLength == IRDA_WORDLENGTH_7B)     \
    {                                                                 \
      if ((__HANDLE__)->Init.Parity == IRDA_PARITY_NONE)              \
      {                                                               \
        (__HANDLE__)->Mask = 0x007FU ;                                \
      }                                                               \
      else                                                            \
      {                                                               \
        (__HANDLE__)->Mask = 0x003FU ;                                \
      }                                                               \
    }                                                                 \
    else                                                              \
    {                                                                 \
      (__HANDLE__)->Mask = 0x0000U;                                   \
    }                                                                 \
  } while(0U)

/** @brief Ensure that IRDA frame length is valid.
  * @param __LENGTH__ IRDA frame length.
  * @retval SET (__LENGTH__ is valid) or RESET (__LENGTH__ is invalid)
  */
#define IS_IRDA_WORD_LENGTH(__LENGTH__) (((__LENGTH__) == IRDA_WORDLENGTH_7B) || \
                                         ((__LENGTH__) == IRDA_WORDLENGTH_8B) || \
                                         ((__LENGTH__) == IRDA_WORDLENGTH_9B))
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_IRDA_EX_H */

