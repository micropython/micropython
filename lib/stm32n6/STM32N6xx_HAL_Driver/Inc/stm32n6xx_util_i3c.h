/**
  **********************************************************************************************************************
  * @file    stm32n6xx_util_i3c.h
  * @author  MCD Application Team
  * @brief   Header of stm32n6xx_util_i3c.c
  **********************************************************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **********************************************************************************************************************
  */

/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/
#ifndef STM32N6xx_UTIL_I3C_H
#define STM32N6xx_UTIL_I3C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------------------------------------------------*/
#if defined (USE_HAL_DRIVER)
#include "stm32n6xx_hal.h"
#endif /* USE_HAL_DRIVER */

#if defined (USE_FULL_LL_DRIVER)
#include "stm32n6xx_ll_i3c.h"
#endif /* USE_FULL_LL_DRIVER */

#if (defined(USE_HAL_DRIVER) && defined(HAL_I3C_MODULE_ENABLED)) || defined(USE_FULL_LL_DRIVER)
/** @addtogroup STM32N6xx_UTIL_Driver
  * @{
  */

/** @defgroup UTILITY_I3C I3C Utility
  * @{
  */
/* Exported types ----------------------------------------------------------------------------------------------------*/
/** @defgroup I3C_UTIL_Exported_Types I3C Utility Exported Types
  * @{
  */

/** @defgroup I3C_Controller_Timing_Structure_definition I3C Controller Timing Structure definition
  * @brief    I3C Controller Timing Structure definition
  * @{
  */
typedef struct
{
  uint32_t clockSrcFreq; /*!< Specifies the I3C clock source (in Hz).                                                 */

  uint32_t i3cPPFreq;   /*!< Specifies the I3C required bus clock for Push-Pull phase (in Hz).                        */

  uint32_t i2cODFreq;   /*!< Specifies I2C required bus clock for Open-Drain phase (in Hz).                           */

  uint32_t dutyCycle;   /*!< Specifies the I3C duty cycle for Pure I3C bus or I2C duty cycle for Mixed bus in percent
                             This parameter must be a value less than or equal to 50 percent.                         */

  uint32_t busType;     /*!< Specifies the Bus configuration type.
                             This parameter must be a value of @ref I3C_UTIL_EC_BUS_TYPE                              */
} I3C_CtrlTimingTypeDef;
/**
  * @}
  */

/** @defgroup I3C_Target_Timing_Structure_definition I3C Target Timing Structure definition
  * @brief    I3C Target Timing Structure definition
  * @{
  */
typedef struct
{
  uint32_t clockSrcFreq; /*!< Specifies the I3C clock source (in Hz).                                                 */
} I3C_TgtTimingTypeDef;
/**
  * @}
  */

/**
  * @}
  */

/* Exported define ---------------------------------------------------------------------------------------------------*/
/** @defgroup I3C_UTIL_Exported_Define I3C Utility Exported Define
  * @{
  */

/** @defgroup I3C_UTIL_EC_BUS_TYPE I3C Utility Bus Type
  * @brief    Bus type defines which can be used with I3C_CtrlTimingComputation function
  * @{
  */
#define I3C_PURE_I3C_BUS        0U      /*!< Pure I3C bus, no I2C    */
#define I3C_MIXED_BUS           1U      /*!< Mixed bus I3C and I2C   */
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions ------------------------------------------------------------------------------------------------*/
/** @addtogroup I3C_UTIL_Exported_Functions
  * @{
  */
/** @addtogroup I3C_UTIL_EF_Computation
  * @{
  */
ErrorStatus I3C_CtrlTimingComputation(const I3C_CtrlTimingTypeDef *pInputTiming,
                                      LL_I3C_CtrlBusConfTypeDef *pOutputConfig);
ErrorStatus I3C_TgtTimingComputation(const I3C_TgtTimingTypeDef *pInputTiming,
                                     LL_I3C_TgtBusConfTypeDef *pOutputConfig);
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

#endif /* (defined(USE_HAL_DRIVER) && defined(HAL_I3C_MODULE_ENABLED)) || defined(USE_FULL_LL_DRIVER) */
#ifdef __cplusplus
}
#endif

#endif /* __STM32N6xx_UTIL_I3C_H */
