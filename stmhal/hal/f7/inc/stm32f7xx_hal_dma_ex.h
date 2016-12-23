/**
  ******************************************************************************
  * @file    stm32f7xx_hal_dma_ex.h
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    23-September-2016
  * @brief   Header file of DMA HAL extension module.
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
#ifndef __STM32F7xx_HAL_DMA_EX_H
#define __STM32F7xx_HAL_DMA_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal_def.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @addtogroup DMAEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup DMAEx_Exported_Types DMAEx Exported Types
  * @brief DMAEx Exported types
  * @{
  */

/**
  * @brief  HAL DMA Memory definition
  */
typedef enum
{
  MEMORY0      = 0x00U,    /*!< Memory 0     */
  MEMORY1      = 0x01U,    /*!< Memory 1     */

}HAL_DMA_MemoryTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup DMA_Exported_Constants DMA Exported Constants
  * @brief    DMA Exported constants
  * @{
  */

/** @defgroup DMAEx_Channel_selection DMA Channel selection
  * @brief    DMAEx channel selection
  * @{
  */
#define DMA_CHANNEL_0        ((uint32_t)0x00000000U)  /*!< DMA Channel 0 */
#define DMA_CHANNEL_1        ((uint32_t)0x02000000U)  /*!< DMA Channel 1 */
#define DMA_CHANNEL_2        ((uint32_t)0x04000000U)  /*!< DMA Channel 2 */
#define DMA_CHANNEL_3        ((uint32_t)0x06000000U)  /*!< DMA Channel 3 */
#define DMA_CHANNEL_4        ((uint32_t)0x08000000U)  /*!< DMA Channel 4 */
#define DMA_CHANNEL_5        ((uint32_t)0x0A000000U)  /*!< DMA Channel 5 */
#define DMA_CHANNEL_6        ((uint32_t)0x0C000000U)  /*!< DMA Channel 6 */
#define DMA_CHANNEL_7        ((uint32_t)0x0E000000U)  /*!< DMA Channel 7 */
#if defined (STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx)
#define DMA_CHANNEL_8        ((uint32_t)0x10000000U)  /*!< DMA Channel 8 */
#define DMA_CHANNEL_9        ((uint32_t)0x12000000U)  /*!< DMA Channel 9 */
#define DMA_CHANNEL_10       ((uint32_t)0x14000000U)  /*!< DMA Channel 10*/
#define DMA_CHANNEL_11       ((uint32_t)0x16000000U)  /*!< DMA Channel 11*/
#define DMA_CHANNEL_12       ((uint32_t)0x18000000U)  /*!< DMA Channel 12*/
#define DMA_CHANNEL_13       ((uint32_t)0x1A000000U)  /*!< DMA Channel 13*/
#define DMA_CHANNEL_14       ((uint32_t)0x1C000000U)  /*!< DMA Channel 14*/
#define DMA_CHANNEL_15       ((uint32_t)0x1E000000U)  /*!< DMA Channel 15*/
#endif /* STM32F765xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup DMAEx_Exported_Functions DMAEx Exported Functions
  * @brief   DMAEx Exported functions
  * @{
  */

/** @defgroup DMAEx_Exported_Functions_Group1 Extended features functions
  * @brief   Extended features functions
  * @{
  */

/* IO operation functions *******************************************************/
HAL_StatusTypeDef HAL_DMAEx_MultiBufferStart(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SecondMemAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMAEx_MultiBufferStart_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SecondMemAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMAEx_ChangeMemory(DMA_HandleTypeDef *hdma, uint32_t Address, HAL_DMA_MemoryTypeDef memory);

/**
  * @}
  */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup DMAEx_Private_Macros DMA Private Macros
  * @brief    DMAEx private macros
  * @{
  */
#if defined (STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx)
#define IS_DMA_CHANNEL(CHANNEL) (((CHANNEL) == DMA_CHANNEL_0)  || \
                                 ((CHANNEL) == DMA_CHANNEL_1)  || \
                                 ((CHANNEL) == DMA_CHANNEL_2)  || \
                                 ((CHANNEL) == DMA_CHANNEL_3)  || \
                                 ((CHANNEL) == DMA_CHANNEL_4)  || \
                                 ((CHANNEL) == DMA_CHANNEL_5)  || \
                                 ((CHANNEL) == DMA_CHANNEL_6)  || \
                                 ((CHANNEL) == DMA_CHANNEL_7)  || \
                                 ((CHANNEL) == DMA_CHANNEL_8)  || \
                                 ((CHANNEL) == DMA_CHANNEL_9)  || \
                                 ((CHANNEL) == DMA_CHANNEL_10) || \
                                 ((CHANNEL) == DMA_CHANNEL_11) || \
                                 ((CHANNEL) == DMA_CHANNEL_12) || \
                                 ((CHANNEL) == DMA_CHANNEL_13) || \
                                 ((CHANNEL) == DMA_CHANNEL_14) || \
                                 ((CHANNEL) == DMA_CHANNEL_15))
#else
#define IS_DMA_CHANNEL(CHANNEL) (((CHANNEL) == DMA_CHANNEL_0) || \
                                 ((CHANNEL) == DMA_CHANNEL_1) || \
                                 ((CHANNEL) == DMA_CHANNEL_2) || \
                                 ((CHANNEL) == DMA_CHANNEL_3) || \
                                 ((CHANNEL) == DMA_CHANNEL_4) || \
                                 ((CHANNEL) == DMA_CHANNEL_5) || \
                                 ((CHANNEL) == DMA_CHANNEL_6) || \
                                 ((CHANNEL) == DMA_CHANNEL_7))
#endif /* STM32F765xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup DMAEx_Private_Functions DMAEx Private Functions
  * @brief DMAEx Private functions
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

#ifdef __cplusplus
}
#endif

#endif /* __STM32F7xx_HAL_DMA_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
