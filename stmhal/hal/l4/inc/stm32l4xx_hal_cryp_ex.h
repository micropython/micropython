/**
  ******************************************************************************
  * @file    stm32l4xx_hal_cryp_ex.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of CRYPEx HAL module.
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
#ifndef __STM32L4xx_HAL_CRYP_EX_H
#define __STM32L4xx_HAL_CRYP_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(STM32L485xx) || defined(STM32L486xx)

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal_def.h"

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @addtogroup CRYPEx
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @addtogroup CRYPEx_Exported_Functions
  * @{
  */

/** @addtogroup CRYPEx_Exported_Functions_Group1
  * @{
  */

/* CallBack functions  ********************************************************/
void HAL_CRYPEx_ComputationCpltCallback(CRYP_HandleTypeDef *hcryp);

/**
  * @}
  */ 

/** @addtogroup CRYPEx_Exported_Functions_Group2
  * @{
  */

/* AES encryption/decryption processing functions  ****************************/
HAL_StatusTypeDef HAL_CRYPEx_AES(CRYP_HandleTypeDef *hcryp, uint8_t *pInputData, uint16_t Size, uint8_t *pOutputData, uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYPEx_AES_IT(CRYP_HandleTypeDef *hcryp,  uint8_t *pInputData, uint16_t Size, uint8_t *pOutputData);
HAL_StatusTypeDef HAL_CRYPEx_AES_DMA(CRYP_HandleTypeDef *hcryp,  uint8_t *pInputData, uint16_t Size, uint8_t *pOutputData);

/* AES encryption/decryption/authentication processing functions  *************/
HAL_StatusTypeDef HAL_CRYPEx_AES_Auth(CRYP_HandleTypeDef *hcryp, uint8_t *pInputData, uint64_t Size, uint8_t *pOutputData, uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYPEx_AES_Auth_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pInputData, uint64_t Size, uint8_t *pOutputData);
HAL_StatusTypeDef HAL_CRYPEx_AES_Auth_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pInputData, uint64_t Size, uint8_t *pOutputData);

/**
  * @}
  */ 

/** @addtogroup CRYPEx_Exported_Functions_Group3
  * @{
  */

/* AES suspension/resumption functions  ***************************************/
void HAL_CRYPEx_Read_IVRegisters(CRYP_HandleTypeDef *hcryp, uint8_t* Output);
void HAL_CRYPEx_Write_IVRegisters(CRYP_HandleTypeDef *hcryp, uint8_t* Input);
void HAL_CRYPEx_Read_SuspendRegisters(CRYP_HandleTypeDef *hcryp, uint8_t* Output);
void HAL_CRYPEx_Write_SuspendRegisters(CRYP_HandleTypeDef *hcryp, uint8_t* Input);
void HAL_CRYPEx_Read_KeyRegisters(CRYP_HandleTypeDef *hcryp, uint8_t* Output, uint32_t KeySize);
void HAL_CRYPEx_Write_KeyRegisters(CRYP_HandleTypeDef *hcryp, uint8_t* Input, uint32_t KeySize);
void HAL_CRYPEx_Read_ControlRegister(CRYP_HandleTypeDef *hcryp, uint8_t* Output);
void HAL_CRYPEx_Write_ControlRegister(CRYP_HandleTypeDef *hcryp, uint8_t* Input);
void HAL_CRYPEx_ProcessSuspend(CRYP_HandleTypeDef *hcryp);

/**
  * @}
  */  
  

/**
  * @}
  */ 
  
/* Private functions -----------------------------------------------------------*/
/** @addtogroup CRYPEx_Private_Functions CRYPEx Private Functions
  * @{
  */
HAL_StatusTypeDef CRYP_AES_Auth_IT(CRYP_HandleTypeDef *hcryp);

/**
  * @}
  */
  

/**
  * @}
  */ 

/**
  * @}
  */ 
  
#endif /* defined(STM32L485xx) || defined(STM32L486xx) */
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32L4xx_HAL_CRYP_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
