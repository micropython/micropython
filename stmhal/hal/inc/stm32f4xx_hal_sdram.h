/**
  ******************************************************************************
  * @file    stm32f4xx_hal_sdram.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of SDRAM HAL module.
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
#ifndef __STM32F4xx_HAL_SDRAM_H
#define __STM32F4xx_HAL_SDRAM_H

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_ll_fmc.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup SDRAM
  * @{
  */ 

/* Exported typedef ----------------------------------------------------------*/   

/** 
  * @brief  HAL SDRAM State structure definition  
  */ 
typedef enum
{
  HAL_SDRAM_STATE_RESET             = 0x00,  /*!< SDRAM not yet initialized or disabled */
  HAL_SDRAM_STATE_READY             = 0x01,  /*!< SDRAM initialized and ready for use   */
  HAL_SDRAM_STATE_BUSY              = 0x02,  /*!< SDRAM internal process is ongoing     */
  HAL_SDRAM_STATE_ERROR             = 0x03,  /*!< SDRAM error state                     */
  HAL_SDRAM_STATE_WRITE_PROTECTED   = 0x04,  /*!< SDRAM device write protected          */
  HAL_SDRAM_STATE_PRECHARGED        = 0x05   /*!< SDRAM device precharged               */
  
}HAL_SDRAM_StateTypeDef;

/** 
  * @brief  SDRAM handle Structure definition  
  */ 
typedef struct
{
  FMC_SDRAM_TypeDef             *Instance;  /*!< Register base address                 */
  
  FMC_SDRAM_InitTypeDef         Init;       /*!< SDRAM device configuration parameters */
  
  __IO HAL_SDRAM_StateTypeDef   State;      /*!< SDRAM access state                    */
  
  HAL_LockTypeDef               Lock;       /*!< SDRAM locking object                  */ 

  DMA_HandleTypeDef             *hdma;      /*!< Pointer DMA handler                   */
  
}SDRAM_HandleTypeDef;
         
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/** @brief Reset SDRAM handle state
  * @param  __HANDLE__: specifies the SDRAM handle.
  * @retval None
  */
#define __HAL_SDRAM_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_SDRAM_STATE_RESET)

/* Exported functions --------------------------------------------------------*/

/* Initialization/de-initialization functions  **********************************/
HAL_StatusTypeDef HAL_SDRAM_Init(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_TimingTypeDef *Timing);
HAL_StatusTypeDef HAL_SDRAM_DeInit(SDRAM_HandleTypeDef *hsdram);
void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram);
void HAL_SDRAM_MspDeInit(SDRAM_HandleTypeDef *hsdram);

void HAL_SDRAM_IRQHandler(SDRAM_HandleTypeDef *hsdram);
void HAL_SDRAM_RefreshErrorCallback(SDRAM_HandleTypeDef *hsdram);
void HAL_SDRAM_DMA_XferCpltCallback(DMA_HandleTypeDef *hdma);
void HAL_SDRAM_DMA_XferErrorCallback(DMA_HandleTypeDef *hdma);

/* I/O operation functions  *****************************************************/
HAL_StatusTypeDef HAL_SDRAM_Read_8b(SDRAM_HandleTypeDef *hsdram, uint32_t *pAddress, uint8_t *pDstBuffer, uint32_t BufferSize);
HAL_StatusTypeDef HAL_SDRAM_Write_8b(SDRAM_HandleTypeDef *hsdram, uint32_t *pAddress, uint8_t *pSrcBuffer, uint32_t BufferSize);
HAL_StatusTypeDef HAL_SDRAM_Read_16b(SDRAM_HandleTypeDef *hsdram, uint32_t *pAddress, uint16_t *pDstBuffer, uint32_t BufferSize);
HAL_StatusTypeDef HAL_SDRAM_Write_16b(SDRAM_HandleTypeDef *hsdram, uint32_t *pAddress, uint16_t *pSrcBuffer, uint32_t BufferSize);
HAL_StatusTypeDef HAL_SDRAM_Read_32b(SDRAM_HandleTypeDef *hsdram, uint32_t *pAddress, uint32_t *pDstBuffer, uint32_t BufferSize);
HAL_StatusTypeDef HAL_SDRAM_Write_32b(SDRAM_HandleTypeDef *hsdram, uint32_t *pAddress, uint32_t *pSrcBuffer, uint32_t BufferSize);

HAL_StatusTypeDef HAL_SDRAM_Read_DMA(SDRAM_HandleTypeDef *hsdram, uint32_t * pAddress, uint32_t *pDstBuffer, uint32_t BufferSize);
HAL_StatusTypeDef HAL_SDRAM_Write_DMA(SDRAM_HandleTypeDef *hsdram, uint32_t *pAddress, uint32_t *pSrcBuffer, uint32_t BufferSize);

/* SDRAM Control functions  *****************************************************/
HAL_StatusTypeDef HAL_SDRAM_WriteProtection_Enable(SDRAM_HandleTypeDef *hsdram);
HAL_StatusTypeDef HAL_SDRAM_WriteProtection_Disable(SDRAM_HandleTypeDef *hsdram);
HAL_StatusTypeDef HAL_SDRAM_SendCommand(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command, uint32_t Timeout);
HAL_StatusTypeDef HAL_SDRAM_ProgramRefreshRate(SDRAM_HandleTypeDef *hsdram, uint32_t RefreshRate);
HAL_StatusTypeDef HAL_SDRAM_SetAutoRefreshNumber(SDRAM_HandleTypeDef *hsdram, uint32_t AutoRefreshNumber);
uint32_t          HAL_SDRAM_GetModeStatus(SDRAM_HandleTypeDef *hsdram);

/* SDRAM State functions ********************************************************/
HAL_SDRAM_StateTypeDef  HAL_SDRAM_GetState(SDRAM_HandleTypeDef *hsdram);

#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */
/**
  * @}
  */ 

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_SDRAM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
