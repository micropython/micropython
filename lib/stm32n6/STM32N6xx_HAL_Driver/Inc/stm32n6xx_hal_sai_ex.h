/**
  ******************************************************************************
  * @file    stm32n6xx_hal_sai_ex.h
  * @author  MCD Application Team
  * @brief   Header file of SAI HAL extended module.
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
#ifndef STM32N6xx_HAL_SAI_EX_H
#define STM32N6xx_HAL_SAI_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup SAIEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup SAIEx_Exported_Types SAIEx Exported Types
  * @{
  */

/**
  * @brief  PDM microphone delay structure definition
  */
typedef struct
{
  uint32_t MicPair;     /*!< Specifies which pair of microphones is selected.
                             This parameter must be a number between Min_Data = 1 and Max_Data = 3. */

  uint32_t LeftDelay;   /*!< Specifies the delay in PDM clock unit to apply on left microphone.
                             This parameter must be a number between Min_Data = 0 and Max_Data = 7. */

  uint32_t RightDelay;  /*!< Specifies the delay in PDM clock unit to apply on right microphone.
                             This parameter must be a number between Min_Data = 0 and Max_Data = 7. */
} SAIEx_PdmMicDelayParamTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @addtogroup SAIEx_Exported_Functions SAIEx Extended Exported Functions
  * @{
  */

/** @addtogroup SAIEx_Exported_Functions_Group1 Peripheral Control functions
  * @{
  */
HAL_StatusTypeDef HAL_SAIEx_ConfigPdmMicDelay(const SAI_HandleTypeDef *hsai,
                                              const SAIEx_PdmMicDelayParamTypeDef *pdmMicDelay);
/**
  * @}
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @addtogroup SAIEx_Private_Macros SAIEx Extended Private Macros
  * @{
  */
#define IS_SAI_PDM_MIC_DELAY(VALUE)   ((VALUE) <= 7U)
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_SAI_EX_H */
