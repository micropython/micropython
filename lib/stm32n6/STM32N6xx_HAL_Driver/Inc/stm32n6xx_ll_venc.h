/**
  ******************************************************************************
  * @file    stm32n6xx_ll_venc.h
  * @author  GPM Application Team
  * @brief   Header file of VENC module.
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
#ifndef STM32N6xx_LL_VENC_H
#define STM32N6xx_LL_VENC_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx.h"

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

/** @defgroup VENC_LL VENC
  * @{
  */

#if defined(VENC)
/* Exported types ------------------------------------------------------------*/
/** @defgroup VENC_Exported_Types VENC Exported Types
  * @{

  */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup VENC_Exported_Constants VENC Exported Constants
  * @{

  */
/**
  * @}
  */


/* Exported variables --------------------------------------------------------*/
/** @defgroup VENC_Global_Variables VENC Global Variables
  * @{
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup VENC_Exported_Macros VENC Exported Macros
  * @{
  */

#define LL_VENC_IS_VENCRAM_SYSTEM_ACCESSIBLE() ((SYSCFG->VENCRAMCR & 0b1)!=0)
/** @defgroup RCC_LL_EM_WRITE_READ Common Write and read registers Macros
  * @{
  */

#define VENC_REG(x) (*(__IO uint32_t *) (VENC_BASE + (4UL*(x))))
/**
  * @brief  Write a value in VENC register
  * @param  __REG__ Number of the register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_VENC_WriteRegister(__REG__, __VALUE__) WRITE_REG(VENC_REG(__REG__), (__VALUE__))

/**
  * @brief  Read a value in VENC register
  * @param  __REG__ Number of the register to be read
  * @retval Register value
  */
#define LL_VENC_ReadRegister(__REG__) READ_REG(VENC_REG(__REG__))
/**
  * @}
  */


/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup VENC_Exported_Functions VENC Exported Functions
  * @{
  */

/** @defgroup VENC_LL_EF_Init Init and De-Init functions
  * @{
  */
void LL_VENC_Init(void);
void LL_VENC_DeInit(void);
/**
  * @}
  */


/**
  * @}
  */




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*VENC */

/**
  * @}
  */

/**
  * @}
  */

#endif /* STM32N6xx_LL_VENC_H */
