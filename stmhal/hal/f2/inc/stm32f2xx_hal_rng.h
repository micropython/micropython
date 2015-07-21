/**
  ******************************************************************************
  * @file    stm32f2xx_hal_rng.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-March-2014
  * @brief   Header file of RNG HAL module.
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
#ifndef __STM32F2xx_HAL_RNG_H
#define __STM32F2xx_HAL_RNG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal_def.h"

/** @addtogroup STM32F2xx_HAL_Driver
  * @{
  */

/** @addtogroup RNG
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 

/** 
  * @brief  RNG HAL State Structure definition  
  */ 
typedef enum
{
  HAL_RNG_STATE_RESET     = 0x00,  /*!< RNG not yet initialized or disabled */
  HAL_RNG_STATE_READY     = 0x01,  /*!< RNG initialized and ready for use   */
  HAL_RNG_STATE_BUSY      = 0x02,  /*!< RNG internal process is ongoing     */ 
  HAL_RNG_STATE_TIMEOUT   = 0x03,  /*!< RNG timeout state                   */
  HAL_RNG_STATE_ERROR     = 0x04   /*!< RNG error state                     */
    
}HAL_RNG_StateTypeDef;

/** 
  * @brief  RNG Handle Structure definition  
  */ 
typedef struct
{
  RNG_TypeDef                 *Instance;  /*!< Register base address   */ 
  
  HAL_LockTypeDef             Lock;       /*!< RNG locking object      */
  
  __IO HAL_RNG_StateTypeDef   State;      /*!< RNG communication state */
  
}RNG_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup RNG_Exported_Constants
  * @{
  */

/** @defgroup RNG_Interrupt_definition 
  * @{
  */ 
#define RNG_IT_CEI   ((uint32_t)0x20)  /*!< Clock error interrupt */
#define RNG_IT_SEI   ((uint32_t)0x40)  /*!< Seed error interrupt  */

#define IS_RNG_IT(IT) (((IT) == RNG_IT_CEI) || \
                       ((IT) == RNG_IT_SEI))
/**
  * @}
  */


/** @defgroup RNG_Flag_definition 
  * @{
  */ 
#define RNG_FLAG_DRDY   ((uint32_t)0x0001)  /*!< Data ready                 */
#define RNG_FLAG_CECS   ((uint32_t)0x0002)  /*!< Clock error current status */
#define RNG_FLAG_SECS   ((uint32_t)0x0004)  /*!< Seed error current status  */

#define IS_RNG_FLAG(FLAG) (((FLAG) == RNG_FLAG_DRDY) || \
                           ((FLAG) == RNG_FLAG_CECS) || \
                           ((FLAG) == RNG_FLAG_SECS))
/**
  * @}
  */

/**
  * @}
  */ 
  
/* Exported macro ------------------------------------------------------------*/

/**
  * @brief  Enables the RNG peripheral.
  * @param  __HANDLE__: RNG Handle
  * @retval None
  */
#define __HAL_RNG_ENABLE(__HANDLE__) ((__HANDLE__)->Instance->CR |=  RNG_CR_RNGEN)

/**
  * @brief  Disables the RNG peripheral.
  * @param  __HANDLE__: RNG Handle
  * @retval None
  */
#define __HAL_RNG_DISABLE(__HANDLE__) ((__HANDLE__)->Instance->CR &= ~RNG_CR_RNGEN)

/**
  * @brief  Gets the selected RNG's flag status.
  * @param  __HANDLE__: RNG Handle
  * @param  __FLAG__: RNG flag
  * @retval The new state of RNG_FLAG (SET or RESET).
  */
#define __HAL_RNG_GET_FLAG(__HANDLE__, __FLAG__) (((__HANDLE__)->Instance->SR & (__FLAG__)) == (__FLAG__))

/**
  * @brief  Clears the RNG's pending flags.
  * @param  __HANDLE__: RNG Handle
  * @param  __FLAG__: RNG flag
  * @retval None
  */
#define __HAL_RNG_CLEAR_FLAG(__HANDLE__, __FLAG__) (((__HANDLE__)->Instance->SR) &= ~(__FLAG__))
    
/**
  * @brief  Enables the RNG interrupts.
  * @param  __HANDLE__: RNG Handle
  * @retval None
  */
#define __HAL_RNG_ENABLE_IT(__HANDLE__) ((__HANDLE__)->Instance->CR |=  RNG_CR_IE)
    
/**
  * @brief  Disables the RNG interrupts.
  * @param  __HANDLE__: RNG Handle
  * @retval None
  */
#define __HAL_RNG_DISABLE_IT(__HANDLE__) ((__HANDLE__)->Instance->CR &= ~RNG_CR_IE)

/**
  * @brief  Checks whether the specified RNG interrupt has occurred or not.
  * @param  __HANDLE__: RNG Handle
  * @param  __INTERRUPT__: specifies the RNG interrupt source to check.
  *         This parameter can be one of the following values:
  *            @arg RNG_FLAG_DRDY: Data ready interrupt
  *            @arg RNG_FLAG_CECS: Clock error interrupt
  *            @arg RNG_FLAG_SECS: Seed error interrupt
  * @retval The new state of RNG_FLAG (SET or RESET).
  */
#define __HAL_RNG_GET_IT(__HANDLE__, __INTERRUPT__) (((__HANDLE__)->Instance->SR & (__INTERRUPT__)) == (__INTERRUPT__))   

/* Exported functions --------------------------------------------------------*/

/* Initialization/de-initialization functions  **********************************/
HAL_StatusTypeDef HAL_RNG_Init(RNG_HandleTypeDef *hrng);
HAL_StatusTypeDef HAL_RNG_DeInit (RNG_HandleTypeDef *hrng);
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng);
void HAL_RNG_MspDeInit(RNG_HandleTypeDef *hrng);

/* Peripheral Control functions  ************************************************/
uint32_t HAL_RNG_GetRandomNumber(RNG_HandleTypeDef *hrng);
uint32_t HAL_RNG_GetRandomNumber_IT(RNG_HandleTypeDef *hrng);
void HAL_RNG_IRQHandler(RNG_HandleTypeDef *hrng);
void HAL_RNG_ReadyCallback(RNG_HandleTypeDef* hrng);
void HAL_RNG_ErrorCallback(RNG_HandleTypeDef *hrng);

/* Peripheral State functions  **************************************************/
HAL_RNG_StateTypeDef HAL_RNG_GetState(RNG_HandleTypeDef *hrng);

/**
  * @}
  */ 

/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif /* __STM32F2xx_HAL_RNG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
