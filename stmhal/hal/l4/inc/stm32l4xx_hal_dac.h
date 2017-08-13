/**
  ******************************************************************************
  * @file    stm32l4xx_hal_dac.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of DAC HAL module.
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
#ifndef __STM32L4xx_HAL_DAC_H
#define __STM32L4xx_HAL_DAC_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal_def.h"
   
/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @addtogroup DAC
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup DAC_Exported_Types DAC Exported Types
  * @{
  */

/** 
  * @brief  HAL State structures definition  
  */ 
typedef enum
{
  HAL_DAC_STATE_RESET             = 0x00,  /*!< DAC not yet initialized or disabled  */
  HAL_DAC_STATE_READY             = 0x01,  /*!< DAC initialized and ready for use    */
  HAL_DAC_STATE_BUSY              = 0x02,  /*!< DAC internal processing is ongoing   */
  HAL_DAC_STATE_TIMEOUT           = 0x03,  /*!< DAC timeout state                    */
  HAL_DAC_STATE_ERROR             = 0x04   /*!< DAC error state                      */
 
}HAL_DAC_StateTypeDef;
 
/** 
  * @brief  DAC handle Structure definition  
  */ 
typedef struct
{
  DAC_TypeDef                 *Instance;     /*!< Register base address             */
  
  __IO HAL_DAC_StateTypeDef   State;         /*!< DAC communication state           */

  HAL_LockTypeDef             Lock;          /*!< DAC locking object                */
  
  DMA_HandleTypeDef           *DMA_Handle1;  /*!< Pointer DMA handler for channel 1 */
  
  DMA_HandleTypeDef           *DMA_Handle2;  /*!< Pointer DMA handler for channel 2 */ 
  
  __IO uint32_t               ErrorCode;     /*!< DAC Error code                    */
  
}DAC_HandleTypeDef;

/** 
  * @brief   DAC Configuration sample and hold Channel structure definition  
  */ 
typedef struct
{
  uint32_t DAC_SampleTime ;          /*!< Specifies the Sample time for the selected channel.
                                          This parameter applies when DAC_SampleAndHold is DAC_SAMPLEANDHOLD_ENABLE. 
                                          This parameter must be a number between Min_Data = 0 and Max_Data = 1023 */  

  uint32_t DAC_HoldTime ;            /*!< Specifies the hold time for the selected channel
                                          This parameter applies when DAC_SampleAndHold is DAC_SAMPLEANDHOLD_ENABLE. 
                                          This parameter must be a number between Min_Data = 0 and Max_Data = 1023 */    
  
  uint32_t DAC_RefreshTime ;         /*!< Specifies the refresh time for the selected channel
                                          This parameter applies when DAC_SampleAndHold is DAC_SAMPLEANDHOLD_ENABLE. 
                                          This parameter must be a number between Min_Data = 0 and Max_Data = 255 */  
}
DAC_SampleAndHoldConfTypeDef;

/** 
  * @brief   DAC Configuration regular Channel structure definition  
  */ 
typedef struct
{
  uint32_t DAC_SampleAndHold;            /*!< Specifies whether the DAC mode.
                                              This parameter can be a value of @ref DAC_SampleAndHold */
  
  uint32_t DAC_Trigger;                  /*!< Specifies the external trigger for the selected DAC channel.
                                              This parameter can be a value of @ref DAC_trigger_selection */
  
  uint32_t DAC_OutputBuffer;             /*!< Specifies whether the DAC channel output buffer is enabled or disabled.
                                               This parameter can be a value of @ref DAC_output_buffer */

  uint32_t DAC_ConnectOnChipPeripheral ; /*!< Specifies whether the DAC output is connected or not to on chip peripheral .
                                              This parameter can be a value of @ref DAC_ConnectOnChipPeripheral */
  
  uint32_t DAC_UserTrimming;             /*!< Specifies the trimming mode 
                                              This parameter must be a value of @ref DAC_UserTrimming 
                                              DAC_UserTrimming is either factory or user trimming */
                                        
  uint32_t DAC_TrimmingValue;             /*!< Specifies the offset trimming value 
                                               i.e. when DAC_SampleAndHold is DAC_TRIMMING_USER. 
                                               This parameter must be a number between Min_Data = 1 and Max_Data = 31 */

  DAC_SampleAndHoldConfTypeDef  DAC_SampleAndHoldConfig;  /*!< Sample and Hold settings */ 
      
}DAC_ChannelConfTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup DAC_Exported_Constants DAC Exported Constants
  * @{
  */

/** @defgroup DAC_Error_Code DAC Error Code
  * @{
  */
#define  HAL_DAC_ERROR_NONE              0x00    /*!< No error                          */
#define  HAL_DAC_ERROR_DMAUNDERRUNCH1    0x01    /*!< DAC channel1 DMA underrun error   */
#define  HAL_DAC_ERROR_DMAUNDERRUNCH2    0x02    /*!< DAC channel2 DMA underrun error   */
#define  HAL_DAC_ERROR_DMA               0x04    /*!< DMA error                         */
#define  HAL_DAC_ERROR_TIMEOUT           0x08    /*!< Timeout error                     */
/**
  * @}
  */

/** @defgroup DAC_trigger_selection DAC trigger selection
  * @{
  */

#define DAC_TRIGGER_NONE                   ((uint32_t)0x00000000) /*!< Conversion is automatic once the DAC_DHRxxxx register 
                                                                       has been loaded, and not by external trigger */
#define DAC_TRIGGER_T2_TRGO                ((uint32_t)(DAC_CR_TSEL1_2 | DAC_CR_TEN1)) /*!< TIM2 TRGO selected as external conversion trigger for DAC channel */
#define DAC_TRIGGER_T4_TRGO                ((uint32_t)(DAC_CR_TSEL1_2 |DAC_CR_TSEL1_0 | DAC_CR_TEN1)) /*!< TIM4 TRGO selected as external conversion trigger for DAC channel */
#define DAC_TRIGGER_T5_TRGO                ((uint32_t)(DAC_CR_TSEL1_1 | DAC_CR_TSEL1_0 | DAC_CR_TEN1)) /*!< TIM5 TRGO selected as external conversion trigger for DAC channel */
#define DAC_TRIGGER_T6_TRGO                ((uint32_t)DAC_CR_TEN1) /*!< TIM6 TRGO selected as external conversion trigger for DAC channel */
#define DAC_TRIGGER_T7_TRGO                ((uint32_t)(DAC_CR_TSEL1_1 | DAC_CR_TEN1)) /*!< TIM7 TRGO selected as external conversion trigger for DAC channel */
#define DAC_TRIGGER_T8_TRGO                ((uint32_t)(DAC_CR_TSEL1_0 | DAC_CR_TEN1)) /*!< TIM8 TRGO selected as external conversion trigger for DAC channel */
#define DAC_TRIGGER_EXT_IT9                ((uint32_t)(DAC_CR_TSEL1_2 | DAC_CR_TSEL1_1 | DAC_CR_TEN1)) /*!< EXTI Line9 event selected as external conversion trigger for DAC channel */
#define DAC_TRIGGER_SOFTWARE               ((uint32_t)(DAC_CR_TSEL1 | DAC_CR_TEN1)) /*!< Conversion started by software trigger for DAC channel */

/**
  * @}
  */   

/** @defgroup DAC_output_buffer DAC output buffer
  * @{
  */
#define DAC_OUTPUTBUFFER_ENABLE            ((uint32_t)0x00000000)
#define DAC_OUTPUTBUFFER_DISABLE           ((uint32_t)DAC_MCR_MODE1_1)

/**
  * @}
  */
    
/** @defgroup DAC_Channel_selection DAC Channel selection
  * @{
  */
#define DAC_CHANNEL_1                      ((uint32_t)0x00000000)
#define DAC_CHANNEL_2                      ((uint32_t)0x00000010)

/**
  * @}
  */

/** @defgroup DAC_data_alignment DAC data alignment
  * @{
  */
#define DAC_ALIGN_12B_R                    ((uint32_t)0x00000000)
#define DAC_ALIGN_12B_L                    ((uint32_t)0x00000004)
#define DAC_ALIGN_8B_R                     ((uint32_t)0x00000008)

/**
  * @}
  */

/** @defgroup DAC_flags_definition DAC flags definition
  * @{
  */ 
#define DAC_FLAG_DMAUDR1                   ((uint32_t)DAC_SR_DMAUDR1)
#define DAC_FLAG_DMAUDR2                   ((uint32_t)DAC_SR_DMAUDR2)   

/**
  * @}
  */

/** @defgroup DAC_IT_definition  DAC IT definition
  * @{
  */ 
#define DAC_IT_DMAUDR1                   ((uint32_t)DAC_SR_DMAUDR1)
#define DAC_IT_DMAUDR2                   ((uint32_t)DAC_SR_DMAUDR2)   

/**
  * @}
  */
  
/** @defgroup DAC_ConnectOnChipPeripheral DAC ConnectOnChipPeripheral
  * @{
  */
#define DAC_CHIPCONNECT_DISABLE  ((uint32_t)0x00000000)
#define DAC_CHIPCONNECT_ENABLE     ((uint32_t)DAC_MCR_MODE1_0)

/**
  * @}
  */

  /** @defgroup DAC_UserTrimming DAC User Trimming
  * @{
  */

#define DAC_TRIMMING_FACTORY        ((uint32_t)0x00000000)           /*!< Factory trimming */
#define DAC_TRIMMING_USER           ((uint32_t)0x00000001)           /*!< User trimming */
   
/**
  * @}
  */ 

/** @defgroup DAC_SampleAndHold DAC power mode
  * @{
  */
#define DAC_SAMPLEANDHOLD_DISABLE     ((uint32_t)0x00000000)
#define DAC_SAMPLEANDHOLD_ENABLE      ((uint32_t)DAC_MCR_MODE1_2)

/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/** @defgroup DAC_Exported_Macros DAC Exported Macros
  * @{
  */

/** @brief Reset DAC handle state.
  * @param  __HANDLE__: specifies the DAC handle.
  * @retval None
  */
#define __HAL_DAC_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_DAC_STATE_RESET)

/** @brief Enable the DAC channel.
  * @param  __HANDLE__: specifies the DAC handle.
  * @param  __DAC_Channel__: specifies the DAC channel
  * @retval None
  */
#define __HAL_DAC_ENABLE(__HANDLE__, __DAC_Channel__) \
((__HANDLE__)->Instance->CR |=  (DAC_CR_EN1 << (__DAC_Channel__)))

/** @brief Disable the DAC channel.
  * @param  __HANDLE__: specifies the DAC handle
  * @param  __DAC_Channel__: specifies the DAC channel.
  * @retval None
  */
#define __HAL_DAC_DISABLE(__HANDLE__, __DAC_Channel__) \
((__HANDLE__)->Instance->CR &=  ~(DAC_CR_EN1 << (__DAC_Channel__)))
 
/** @brief Set DHR12R1 alignment.
  * @param  __ALIGNMENT__: specifies the DAC alignment
  * @retval None
  */
#define DAC_DHR12R1_ALIGNMENT(__ALIGNMENT__) (((uint32_t)0x00000008) + (__ALIGNMENT__))

/** @brief  Set DHR12R2 alignment.
  * @param  __ALIGNMENT__: specifies the DAC alignment
  * @retval None
  */
#define DAC_DHR12R2_ALIGNMENT(__ALIGNMENT__) (((uint32_t)0x00000014) + (__ALIGNMENT__))

/** @brief  Set DHR12RD alignment.
  * @param  __ALIGNMENT__: specifies the DAC alignment
  * @retval None
  */
#define DAC_DHR12RD_ALIGNMENT(__ALIGNMENT__) (((uint32_t)0x00000020) + (__ALIGNMENT__))

/** @brief Enable the DAC interrupt.
  * @param  __HANDLE__: specifies the DAC handle
  * @param  __INTERRUPT__: specifies the DAC interrupt.
  *          This parameter can be any combination of the following values:
  *            @arg DAC_IT_DMAUDR1: DAC channel 1 DMA underrun interrupt
  *            @arg DAC_IT_DMAUDR2: DAC channel 2 DMA underrun interrupt
  * @retval None
  */
#define __HAL_DAC_ENABLE_IT(__HANDLE__, __INTERRUPT__) (((__HANDLE__)->Instance->CR) |= (__INTERRUPT__))

/** @brief Disable the DAC interrupt.
  * @param  __HANDLE__: specifies the DAC handle
  * @param  __INTERRUPT__: specifies the DAC interrupt.
  *          This parameter can be any combination of the following values:
  *            @arg DAC_IT_DMAUDR1: DAC channel 1 DMA underrun interrupt
  *            @arg DAC_IT_DMAUDR2: DAC channel 2 DMA underrun interrupt
  * @retval None
  */
#define __HAL_DAC_DISABLE_IT(__HANDLE__, __INTERRUPT__) (((__HANDLE__)->Instance->CR) &= ~(__INTERRUPT__))

/** @brief  Check whether the specified DAC interrupt source is enabled or not.
  * @param __HANDLE__: DAC handle
  * @param __INTERRUPT__: DAC interrupt source to check
  *          This parameter can be any combination of the following values:
  *            @arg DAC_IT_DMAUDR1: DAC channel 1 DMA underrun interrupt
  *            @arg DAC_IT_DMAUDR2: DAC channel 2 DMA underrun interrupt
  * @retval State of interruption (SET or RESET)
  */
#define __HAL_DAC_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) (((__HANDLE__)->Instance->CR & (__INTERRUPT__)) == (__INTERRUPT__))

/** @brief  Get the selected DAC's flag status.
  * @param  __HANDLE__: specifies the DAC handle.
  * @param  __FLAG__: specifies the DAC flag to get.
  *          This parameter can be any combination of the following values:
  *            @arg DAC_FLAG_DMAUDR1: DAC channel 1 DMA underrun flag
  *            @arg DAC_FLAG_DMAUDR2: DAC channel 2 DMA underrun flag
  * @retval None
  */
#define __HAL_DAC_GET_FLAG(__HANDLE__, __FLAG__) ((((__HANDLE__)->Instance->SR) & (__FLAG__)) == (__FLAG__))

/** @brief  Clear the DAC's flag.
  * @param  __HANDLE__: specifies the DAC handle.
  * @param  __FLAG__: specifies the DAC flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg DAC_FLAG_DMAUDR1: DAC channel 1 DMA underrun flag
  *            @arg DAC_FLAG_DMAUDR2: DAC channel 2 DMA underrun flag
  * @retval None
  */
#define __HAL_DAC_CLEAR_FLAG(__HANDLE__, __FLAG__) (((__HANDLE__)->Instance->SR) = (__FLAG__))

/**
  * @}
  */ 

/* Private macro -------------------------------------------------------------*/

/** @defgroup DAC_Private_Macros DAC Private Macros
  * @{
  */
#define IS_DAC_OUTPUT_BUFFER_STATE(STATE) (((STATE) == DAC_OUTPUTBUFFER_ENABLE) || \
                                           ((STATE) == DAC_OUTPUTBUFFER_DISABLE))

#define IS_DAC_CHANNEL(CHANNEL) (((CHANNEL) == DAC_CHANNEL_1) || \
                                ((CHANNEL) == DAC_CHANNEL_2))

#define IS_DAC_ALIGN(ALIGN) (((ALIGN) == DAC_ALIGN_12B_R) || \
                             ((ALIGN) == DAC_ALIGN_12B_L) || \
                             ((ALIGN) == DAC_ALIGN_8B_R))

#define IS_DAC_DATA(DATA) ((DATA) <= 0xFFF0)

#define IS_DAC_REFRESHTIME(TIME)   ((TIME) <= 0x0000000FF)

/**
  * @}
  */
  
/* Include DAC HAL Extended module */
#include "stm32l4xx_hal_dac_ex.h"   

/* Exported functions --------------------------------------------------------*/  

/** @addtogroup DAC_Exported_Functions
  * @{
  */

/** @addtogroup DAC_Exported_Functions_Group1
  * @{
  */  
/* Initialization and de-initialization functions *****************************/ 
HAL_StatusTypeDef HAL_DAC_Init(DAC_HandleTypeDef* hdac);
HAL_StatusTypeDef HAL_DAC_DeInit(DAC_HandleTypeDef* hdac);
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac);
void HAL_DAC_MspDeInit(DAC_HandleTypeDef* hdac);

/**
  * @}
  */

/** @addtogroup DAC_Exported_Functions_Group2
 * @{
 */    
/* IO operation functions *****************************************************/
HAL_StatusTypeDef HAL_DAC_Start(DAC_HandleTypeDef* hdac, uint32_t Channel);
HAL_StatusTypeDef HAL_DAC_Stop(DAC_HandleTypeDef* hdac, uint32_t Channel);
HAL_StatusTypeDef HAL_DAC_Start_DMA(DAC_HandleTypeDef* hdac, uint32_t Channel, uint32_t* pData, uint32_t Length, uint32_t Alignment);
HAL_StatusTypeDef HAL_DAC_Stop_DMA(DAC_HandleTypeDef* hdac, uint32_t Channel);

void HAL_DAC_IRQHandler(DAC_HandleTypeDef* hdac);

HAL_StatusTypeDef HAL_DAC_SetValue(DAC_HandleTypeDef* hdac, uint32_t Channel, uint32_t Alignment, uint32_t Data);

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac);
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef* hdac);
void HAL_DAC_ErrorCallbackCh1(DAC_HandleTypeDef *hdac);
void HAL_DAC_DMAUnderrunCallbackCh1(DAC_HandleTypeDef *hdac);
/**
  * @}
  */

/** @addtogroup DAC_Exported_Functions_Group3
  * @{
  */ 
/* Peripheral Control functions ***********************************************/
uint32_t HAL_DAC_GetValue(DAC_HandleTypeDef* hdac, uint32_t Channel);

HAL_StatusTypeDef HAL_DAC_ConfigChannel(DAC_HandleTypeDef* hdac, DAC_ChannelConfTypeDef* sConfig, uint32_t Channel);
/**
  * @}
  */

/** @addtogroup DAC_Exported_Functions_Group4
  * @{
  */     
/* Peripheral State and Error functions ***************************************/
HAL_DAC_StateTypeDef HAL_DAC_GetState(DAC_HandleTypeDef* hdac);
uint32_t HAL_DAC_GetError(DAC_HandleTypeDef *hdac);

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
  
#ifdef __cplusplus
}
#endif 
  

#endif /*__STM32L4xx_HAL_DAC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

