/**
  ******************************************************************************
  * @file    stm32f4xx_hal_crc.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   Header file of CRC HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
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
#ifndef __STM32F4xx_HAL_CRC_H
#define __STM32F4xx_HAL_CRC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup CRC
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/

/** 
  * @brief  CRC HAL State Structure definition  
  */ 
typedef enum
{
  HAL_CRC_STATE_RESET     = 0x00,  /*!< CRC not yet initialized or disabled */
  HAL_CRC_STATE_READY     = 0x01,  /*!< CRC initialized and ready for use   */
  HAL_CRC_STATE_BUSY      = 0x02,  /*!< CRC internal process is ongoing     */
  HAL_CRC_STATE_TIMEOUT   = 0x03,  /*!< CRC timeout state                   */
  HAL_CRC_STATE_ERROR     = 0x04   /*!< CRC error state                     */

}HAL_CRC_StateTypeDef;

/** 
  * @brief  CRC handle Structure definition
  */ 
typedef struct
{
  CRC_TypeDef                 *Instance;  /*!< Register base address   */

  HAL_LockTypeDef             Lock;       /*!< CRC locking object      */

  __IO HAL_CRC_StateTypeDef   State;      /*!< CRC communication state */

}CRC_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/**
  * @brief  Resets CRC Data Register.
  * @param  __HANDLE__: CRC handle
  * @retval None
  */
#define __HAL_CRC_DR_RESET(__HANDLE__) ((__HANDLE__)->Instance->CR |= CRC_CR_RESET)

/* Exported functions --------------------------------------------------------*/

/* Initialization/de-initialization functions  **********************************/
HAL_StatusTypeDef HAL_CRC_Init(CRC_HandleTypeDef *hcrc);
HAL_StatusTypeDef HAL_CRC_DeInit (CRC_HandleTypeDef *hcrc);
void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc);
void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc);

/* Peripheral Control functions  ************************************************/
uint32_t HAL_CRC_Accumulate(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[], uint32_t BufferLength);
uint32_t HAL_CRC_Calculate(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[], uint32_t BufferLength);

/* Peripheral State functions  **************************************************/
HAL_CRC_StateTypeDef HAL_CRC_GetState(CRC_HandleTypeDef *hcrc);

/**
  * @}
  */ 

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_CRC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
