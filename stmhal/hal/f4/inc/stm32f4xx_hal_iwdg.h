/**
  ******************************************************************************
  * @file    stm32f4xx_hal_iwdg.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of IWDG HAL module.
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
#ifndef __STM32F4xx_HAL_IWDG_H
#define __STM32F4xx_HAL_IWDG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup IWDG
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/

/** 
  * @brief  IWDG HAL State Structure definition  
  */ 
typedef enum
{
  HAL_IWDG_STATE_RESET     = 0x00,  /*!< IWDG not yet initialized or disabled */
  HAL_IWDG_STATE_READY     = 0x01,  /*!< IWDG initialized and ready for use   */
  HAL_IWDG_STATE_BUSY      = 0x02,  /*!< IWDG internal process is ongoing     */
  HAL_IWDG_STATE_TIMEOUT   = 0x03,  /*!< IWDG timeout state                   */
  HAL_IWDG_STATE_ERROR     = 0x04   /*!< IWDG error state                     */
    
}HAL_IWDG_StateTypeDef;

/** 
  * @brief  IWDG Init structure definition  
  */ 
typedef struct
{
  uint32_t Prescaler;  /*!< Select the prescaler of the IWDG.  
                            This parameter can be a value of @ref IWDG_Prescaler */
  
  uint32_t Reload;     /*!< Specifies the IWDG down-counter reload value. 
                            This parameter must be a number between Min_Data = 0 and Max_Data = 0x0FFF */

}IWDG_InitTypeDef;

/** 
  * @brief  IWDG handle Structure definition  
  */ 
typedef struct
{
  IWDG_TypeDef                 *Instance;  /*!< Register base address    */ 
  
  IWDG_InitTypeDef             Init;       /*!< IWDG required parameters */
  
  HAL_LockTypeDef              Lock;       /*!< IWDG locking object      */
  
  __IO HAL_IWDG_StateTypeDef   State;      /*!< IWDG communication state */
  
}IWDG_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/
/** @defgroup IWDG_Exported_Constants
  * @{
  */

/** @defgroup IWDG_Registers_BitMask
  * @{
  */
/* --- KR Register ---*/
/* KR register bit mask */
#define KR_KEY_RELOAD   ((uint32_t)0xAAAA)  /*!< IWDG reload counter enable   */
#define KR_KEY_ENABLE   ((uint32_t)0xCCCC)  /*!< IWDG peripheral enable       */
#define KR_KEY_EWA      ((uint32_t)0x5555)  /*!< IWDG KR write Access enable  */
#define KR_KEY_DWA      ((uint32_t)0x0000)  /*!< IWDG KR write Access disable */

#define IS_IWDG_KR(__KR__) (((__KR__) == KR_KEY_RELOAD) || \
                            ((__KR__) == KR_KEY_ENABLE))|| \
                            ((__KR__) == KR_KEY_EWA))   || \
                            ((__KR__) == KR_KEY_DWA))
/**
  * @}
  */

/** @defgroup IWDG_Flag_definition 
  * @{
  */ 
#define IWDG_FLAG_PVU   ((uint32_t)0x0001)  /*!< Watchdog counter prescaler value update flag */
#define IWDG_FLAG_RVU   ((uint32_t)0x0002)  /*!< Watchdog counter reload value update flag    */
  
#define IS_IWDG_FLAG(FLAG) (((FLAG) == IWDG_FLAG_PVU) || \
                            ((FLAG) == IWDG_FLAG_RVU))

/**
  * @}
  */

/** @defgroup IWDG_Prescaler 
  * @{
  */ 
#define IWDG_PRESCALER_4     ((uint8_t)0x00)  /*!< IWDG prescaler set to 4   */
#define IWDG_PRESCALER_8     ((uint8_t)(IWDG_PR_PR_0))                  /*!< IWDG prescaler set to 8   */
#define IWDG_PRESCALER_16    ((uint8_t)(IWDG_PR_PR_1))                  /*!< IWDG prescaler set to 16  */
#define IWDG_PRESCALER_32    ((uint8_t)(IWDG_PR_PR_1 | IWDG_PR_PR_0))   /*!< IWDG prescaler set to 32  */
#define IWDG_PRESCALER_64    ((uint8_t)(IWDG_PR_PR_2))                  /*!< IWDG prescaler set to 64  */
#define IWDG_PRESCALER_128   ((uint8_t)(IWDG_PR_PR_2 | IWDG_PR_PR_0))   /*!< IWDG prescaler set to 128 */
#define IWDG_PRESCALER_256   ((uint8_t)(IWDG_PR_PR_2 | IWDG_PR_PR_1))   /*!< IWDG prescaler set to 256 */


#define IS_IWDG_PRESCALER(PRESCALER) (((PRESCALER) == IWDG_PRESCALER_4)  || \
                                      ((PRESCALER) == IWDG_PRESCALER_8)  || \
                                      ((PRESCALER) == IWDG_PRESCALER_16) || \
                                      ((PRESCALER) == IWDG_PRESCALER_32) || \
                                      ((PRESCALER) == IWDG_PRESCALER_64) || \
                                      ((PRESCALER) == IWDG_PRESCALER_128)|| \
                                      ((PRESCALER) == IWDG_PRESCALER_256))

/**
  * @}
  */ 

/** @defgroup IWDG_Reload_Value 
  * @{
  */ 
#define IS_IWDG_RELOAD(RELOAD) ((RELOAD) <= 0xFFF)

/**
  * @}
  */ 

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/** @brief Reset IWDG handle state
  * @param  __HANDLE__: IWDG handle
  * @retval None
  */
#define __HAL_IWDG_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_IWDG_STATE_RESET)

/**
  * @brief  Enables the IWDG peripheral.
  * @param  __HANDLE__: IWDG handle
  * @retval None
  */
#define __HAL_IWDG_START(__HANDLE__) WRITE_REG((__HANDLE__)->Instance->KR, KR_KEY_ENABLE)

/**
  * @brief  Reloads IWDG counter with value defined in the reload register
  *         (write access to IWDG_PR and IWDG_RLR registers disabled).
  * @param  __HANDLE__: IWDG handle
  * @retval None
  */
#define __HAL_IWDG_RELOAD_COUNTER(__HANDLE__) WRITE_REG((__HANDLE__)->Instance->KR, KR_KEY_RELOAD)

/**
  * @brief  Enables write access to IWDG_PR and IWDG_RLR registers.
  * @param  __HANDLE__: IWDG handle
  * @retval None
  */
#define __HAL_IWDG_ENABLE_WRITE_ACCESS(__HANDLE__) WRITE_REG((__HANDLE__)->Instance->KR, KR_KEY_EWA)

/**
  * @brief  Disables write access to IWDG_PR and IWDG_RLR registers.
  * @param  __HANDLE__: IWDG handle
  * @retval None
  */
#define __HAL_IWDG_DISABLE_WRITE_ACCESS(__HANDLE__) WRITE_REG((__HANDLE__)->Instance->KR, KR_KEY_DWA)

/**
  * @brief  Gets the selected IWDG's flag status.
  * @param  __HANDLE__: IWDG handle
  * @param  __FLAG__: specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg IWDG_FLAG_PVU: Watchdog counter reload value update flag
  *            @arg IWDG_FLAG_RVU: Watchdog counter prescaler value flag
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_IWDG_GET_FLAG(__HANDLE__, __FLAG__) (((__HANDLE__)->Instance->SR & (__FLAG__)) == (__FLAG__))

/* Exported functions --------------------------------------------------------*/

/* Initialization/de-initialization functions  ********************************/
HAL_StatusTypeDef HAL_IWDG_Init(IWDG_HandleTypeDef *hiwdg);
void HAL_IWDG_MspInit(IWDG_HandleTypeDef *hiwdg);

/* I/O operation functions ****************************************************/
HAL_StatusTypeDef HAL_IWDG_Start(IWDG_HandleTypeDef *hiwdg);
HAL_StatusTypeDef HAL_IWDG_Refresh(IWDG_HandleTypeDef *hiwdg);

/* Peripheral State functions  ************************************************/
HAL_IWDG_StateTypeDef HAL_IWDG_GetState(IWDG_HandleTypeDef *hiwdg);

/**
  * @}
  */ 

/**
  * @}
  */ 
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_IWDG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
