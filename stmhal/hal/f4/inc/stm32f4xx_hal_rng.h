/**
  ******************************************************************************
  * @file    stm32f4xx_hal_rng.h
  * @author  MCD Application Team
  * @version V1.5.2
  * @date    22-September-2016
  * @brief   Header file of RNG HAL module.
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
#ifndef __STM32F4xx_HAL_RNG_H
#define __STM32F4xx_HAL_RNG_H

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) ||\
    defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) ||\
    defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx) || defined(STM32F469xx) ||\
    defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) ||\
    defined(STM32F412Cx)
      
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup RNG RNG
  * @brief RNG HAL module driver
  * @{
  */

/* Exported types ------------------------------------------------------------*/ 

/** @defgroup RNG_Exported_Types RNG Exported Types
  * @{
  */

/** @defgroup RNG_Exported_Types_Group1 RNG State Structure definition 
  * @{
  */
typedef enum
{
  HAL_RNG_STATE_RESET     = 0x00U,  /*!< RNG not yet initialized or disabled */
  HAL_RNG_STATE_READY     = 0x01U,  /*!< RNG initialized and ready for use   */
  HAL_RNG_STATE_BUSY      = 0x02U,  /*!< RNG internal process is ongoing     */ 
  HAL_RNG_STATE_TIMEOUT   = 0x03U,  /*!< RNG timeout state                   */
  HAL_RNG_STATE_ERROR     = 0x04U   /*!< RNG error state                     */
    
}HAL_RNG_StateTypeDef;

/** 
  * @}
  */

/** @defgroup RNG_Exported_Types_Group2 RNG Handle Structure definition   
  * @{
  */ 
typedef struct
{
  RNG_TypeDef                 *Instance;    /*!< Register base address   */ 
  
  HAL_LockTypeDef             Lock;         /*!< RNG locking object      */
  
  __IO HAL_RNG_StateTypeDef   State;        /*!< RNG communication state */
  
  uint32_t                    RandomNumber; /*!< Last Generated RNG Data */
  
}RNG_HandleTypeDef;

/** 
  * @}
  */

/**
  * @}
  */ 
   
/* Exported constants --------------------------------------------------------*/

/** @defgroup RNG_Exported_Constants RNG Exported Constants
  * @{
  */

/** @defgroup RNG_Exported_Constants_Group1 RNG Interrupt definition
  * @{
  */
#define RNG_IT_DRDY  RNG_SR_DRDY  /*!< Data Ready interrupt  */
#define RNG_IT_CEI   RNG_SR_CEIS  /*!< Clock error interrupt */
#define RNG_IT_SEI   RNG_SR_SEIS  /*!< Seed error interrupt  */
/**
  * @}
  */

/** @defgroup RNG_Exported_Constants_Group2 RNG Flag definition
  * @{
  */
#define RNG_FLAG_DRDY   RNG_SR_DRDY  /*!< Data ready                 */
#define RNG_FLAG_CECS   RNG_SR_CECS  /*!< Clock error current status */
#define RNG_FLAG_SECS   RNG_SR_SECS  /*!< Seed error current status  */

/**
  * @}
  */

/**
  * @}
  */ 
  
/* Exported macros -----------------------------------------------------------*/

/** @defgroup RNG_Exported_Macros RNG Exported Macros
  * @{
  */

/** @brief Reset RNG handle state
  * @param  __HANDLE__: RNG Handle
  * @retval None
  */
#define __HAL_RNG_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_RNG_STATE_RESET)

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
  * @brief  Check the selected RNG flag status.
  * @param  __HANDLE__: RNG Handle
  * @param  __FLAG__: RNG flag
  *          This parameter can be one of the following values:
  *            @arg RNG_FLAG_DRDY: Data ready                
  *            @arg RNG_FLAG_CECS: Clock error current status
  *            @arg RNG_FLAG_SECS: Seed error current status 
  * @retval The new state of __FLAG__ (SET or RESET).
  */
#define __HAL_RNG_GET_FLAG(__HANDLE__, __FLAG__) (((__HANDLE__)->Instance->SR & (__FLAG__)) == (__FLAG__))

/**
  * @brief  Clears the selected RNG flag status.
  * @param  __HANDLE__: RNG handle
  * @param  __FLAG__: RNG flag to clear  
  * @note   WARNING: This is a dummy macro for HAL code alignment,
  *         flags RNG_FLAG_DRDY, RNG_FLAG_CECS and RNG_FLAG_SECS are read-only.
  * @retval None
  */
#define __HAL_RNG_CLEAR_FLAG(__HANDLE__, __FLAG__)                      /* dummy  macro */



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
  * @param  __INTERRUPT__: specifies the RNG interrupt status flag to check.
  *         This parameter can be one of the following values:
  *            @arg RNG_IT_DRDY: Data ready interrupt              
  *            @arg RNG_IT_CEI: Clock error interrupt
  *            @arg RNG_IT_SEI: Seed error interrupt
  * @retval The new state of __INTERRUPT__ (SET or RESET).
  */
#define __HAL_RNG_GET_IT(__HANDLE__, __INTERRUPT__) (((__HANDLE__)->Instance->SR & (__INTERRUPT__)) == (__INTERRUPT__))   

/**
  * @brief  Clear the RNG interrupt status flags.
  * @param  __HANDLE__: RNG Handle
  * @param  __INTERRUPT__: specifies the RNG interrupt status flag to clear.
  *          This parameter can be one of the following values:            
  *            @arg RNG_IT_CEI: Clock error interrupt
  *            @arg RNG_IT_SEI: Seed error interrupt
  * @note   RNG_IT_DRDY flag is read-only, reading RNG_DR register automatically clears RNG_IT_DRDY.          
  * @retval None
  */
#define __HAL_RNG_CLEAR_IT(__HANDLE__, __INTERRUPT__) (((__HANDLE__)->Instance->SR) = ~(__INTERRUPT__))

/**
  * @}
  */ 

/* Exported functions --------------------------------------------------------*/
/** @defgroup RNG_Exported_Functions RNG Exported Functions
  * @{
  */

/** @defgroup RNG_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */  
HAL_StatusTypeDef HAL_RNG_Init(RNG_HandleTypeDef *hrng);
HAL_StatusTypeDef HAL_RNG_DeInit (RNG_HandleTypeDef *hrng);
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng);
void HAL_RNG_MspDeInit(RNG_HandleTypeDef *hrng);

/**
  * @}
  */ 

/** @defgroup RNG_Exported_Functions_Group2 Peripheral Control functions
  * @{
  */
uint32_t HAL_RNG_GetRandomNumber(RNG_HandleTypeDef *hrng);    /* Obsolete, use HAL_RNG_GenerateRandomNumber() instead    */
uint32_t HAL_RNG_GetRandomNumber_IT(RNG_HandleTypeDef *hrng); /* Obsolete, use HAL_RNG_GenerateRandomNumber_IT() instead */

HAL_StatusTypeDef HAL_RNG_GenerateRandomNumber(RNG_HandleTypeDef *hrng, uint32_t *random32bit);
HAL_StatusTypeDef HAL_RNG_GenerateRandomNumber_IT(RNG_HandleTypeDef *hrng);
uint32_t HAL_RNG_ReadLastRandomNumber(RNG_HandleTypeDef *hrng);

void HAL_RNG_IRQHandler(RNG_HandleTypeDef *hrng);
void HAL_RNG_ErrorCallback(RNG_HandleTypeDef *hrng);
void HAL_RNG_ReadyDataCallback(RNG_HandleTypeDef* hrng, uint32_t random32bit);

/**
  * @}
  */ 

/** @defgroup RNG_Exported_Functions_Group3 Peripheral State functions
  * @{
  */
HAL_RNG_StateTypeDef HAL_RNG_GetState(RNG_HandleTypeDef *hrng);

/**
  * @}
  */
  
/**
  * @}
  */ 

/* Private types -------------------------------------------------------------*/
/** @defgroup RNG_Private_Types RNG Private Types
  * @{
  */

/**
  * @}
  */ 

/* Private defines -----------------------------------------------------------*/
/** @defgroup RNG_Private_Defines RNG Private Defines
  * @{
  */

/**
  * @}
  */ 
          
/* Private variables ---------------------------------------------------------*/
/** @defgroup RNG_Private_Variables RNG Private Variables
  * @{
  */

/**
  * @}
  */ 

/* Private constants ---------------------------------------------------------*/
/** @defgroup RNG_Private_Constants RNG Private Constants
  * @{
  */

/**
  * @}
  */ 

/* Private macros ------------------------------------------------------------*/
/** @defgroup RNG_Private_Macros RNG Private Macros
  * @{
  */
#define IS_RNG_IT(IT) (((IT) == RNG_IT_CEI) || \
                       ((IT) == RNG_IT_SEI))

#define IS_RNG_FLAG(FLAG) (((FLAG) == RNG_FLAG_DRDY) || \
                           ((FLAG) == RNG_FLAG_CECS) || \
                           ((FLAG) == RNG_FLAG_SECS))

/**
  * @}
  */ 

/* Private functions prototypes ----------------------------------------------*/
/** @defgroup RNG_Private_Functions_Prototypes RNG Private Functions Prototypes
  * @{
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup RNG_Private_Functions RNG Private Functions
  * @{
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

#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx ||\
          STM32F429xx || STM32F439xx || STM32F410xx || STM32F469xx || STM32F479xx || STM32F412Zx ||\
          STM32F412Vx || STM32F412Rx || STM32F412Cx */

#ifdef __cplusplus
}
#endif


#endif /* __STM32F4xx_HAL_RNG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
