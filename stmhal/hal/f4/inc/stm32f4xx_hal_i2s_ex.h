/**
  ******************************************************************************
  * @file    stm32f4xx_hal_i2s_ex.h
  * @author  MCD Application Team
  * @version V1.5.1
  * @date    01-July-2016
  * @brief   Header file of I2S HAL module.
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
#ifndef __STM32F4xx_HAL_I2S_EX_H
#define __STM32F4xx_HAL_I2S_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"  

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup I2SEx
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/** @defgroup I2SEx_Exported_Types I2S Exported Types
  * @{
  */ 
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup I2SEx_Exported_Constants I2S Exported Constants
  * @{
  */

/** @defgroup I2S_Clock_Source I2S Clock Source 
  * @{
  */

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || \
    defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || \
    defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F411xE) || defined(STM32F469xx) || \
    defined(STM32F479xx)
#define I2S_CLOCK_PLL                     ((uint32_t)0x00000000U)
#define I2S_CLOCK_EXTERNAL                ((uint32_t)0x00000001U)      
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx ||
          STM32F401xC || STM32F401xE || STM32F411xE || STM32F469xx || STM32F479xx */    
   
#if defined(STM32F446xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx)  
#define I2S_CLOCK_PLL                     ((uint32_t)0x00000000U)
#define I2S_CLOCK_EXTERNAL                ((uint32_t)0x00000001U) 
#define I2S_CLOCK_PLLR                    ((uint32_t)0x00000002U)
#define I2S_CLOCK_PLLSRC                  ((uint32_t)0x00000003U)
#endif /* STM32F446xx || STM32F412Zx || STM32F412Vx || STM32F412Rx || STM32F412Cx */

#if defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx)
#define I2S_CLOCK_PLLSRC                  ((uint32_t)0x00000000U)      
#define I2S_CLOCK_EXTERNAL                ((uint32_t)0x00000001U) 
#define I2S_CLOCK_PLLR                    ((uint32_t)0x00000002U)
#endif /* STM32F410Tx || STM32F410Cx || STM32F410Rx */
/**
  * @}
  */

/**
  * @}
  */ 

/* Exported macro ------------------------------------------------------------*/
/** @defgroup I2SEx_Exported_Macros I2S Exported Macros
  * @{
  */

/**
  * @}
  */ 

/* Exported functions --------------------------------------------------------*/
/** @addtogroup I2SEx_Exported_Functions
  * @{
  */

/** @addtogroup I2SEx_Exported_Functions_Group1
  * @{
  */

/* Extended features functions **************************************************/
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_I2SEx_TransmitReceive(I2S_HandleTypeDef *hi2s, uint16_t *pTxData, uint16_t *pRxData, uint16_t Size, uint32_t Timeout);
/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_I2SEx_TransmitReceive_IT(I2S_HandleTypeDef *hi2s, uint16_t *pTxData, uint16_t *pRxData, uint16_t Size);
/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_I2SEx_TransmitReceive_DMA(I2S_HandleTypeDef *hi2s, uint16_t *pTxData, uint16_t *pRxData, uint16_t Size);
/**
  * @}
  */ 

/**
  * @}
  */
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup I2SEx_Private_Constants I2S Private Constants
  * @{
  */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup I2SEx_Private_Macros I2S Private Macros
  * @{
  */
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || \
    defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || \
    defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F411xE) || defined(STM32F469xx) || \
    defined(STM32F479xx)
#define IS_I2S_CLOCKSOURCE(CLOCK) (((CLOCK) == I2S_CLOCK_EXTERNAL) ||\
                                   ((CLOCK) == I2S_CLOCK_PLL))
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx ||
          STM32F401xC || STM32F401xE || STM32F411xE || STM32F469xx || STM32F479xx */

#if defined(STM32F446xx) || defined(STM32F412Zx) || defined(STM32F412Vx) ||\
    defined(STM32F412Rx) || defined(STM32F412Cx)  
#define IS_I2S_CLOCKSOURCE(CLOCK) (((CLOCK) == I2S_CLOCK_EXTERNAL) ||\
                                   ((CLOCK) == I2S_CLOCK_PLL)      ||\
                                   ((CLOCK) == I2S_CLOCK_PLLSRC)   ||\
                                   ((CLOCK) == I2S_CLOCK_PLLR))
#endif /* STM32F446xx || STM32F412Zx || STM32F412Vx || STM32F412Rx || STM32F412Cx */

#if defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx)
#define IS_I2S_CLOCKSOURCE(CLOCK) (((CLOCK) == I2S_CLOCK_EXTERNAL) ||\
                                   ((CLOCK) == I2S_CLOCK_PLLSRC)     ||\
                                   ((CLOCK) == I2S_CLOCK_PLLR))
#endif /* STM32F410Tx || STM32F410Cx || STM32F410Rx */

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || \
    defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || \
    defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F410Cx) || defined(STM32F410Rx) || \
    defined(STM32F411xE) || defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx) || \
    defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx)  
#define I2SxEXT(__INSTANCE__) ((__INSTANCE__) == (SPI2)? (SPI_TypeDef *)(I2S2ext_BASE): (SPI_TypeDef *)(I2S3ext_BASE))
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx ||
          STM32F401xC || STM32F401xE || STM32F410Cx || STM32F410Rx || STM32F411xE || STM32F446xx || STM32F469xx || STM32F479xx || 
          STM32F412Zx || STM32F412Vx || STM32F412Rx || STM32F412Cx */      

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup I2SEx_Private_Functions I2S Private Functions
  * @{
  */
HAL_StatusTypeDef I2SEx_TransmitReceive_IT(I2S_HandleTypeDef *hi2s);
uint32_t I2S_GetInputClock(I2S_HandleTypeDef *hi2s); 
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


#endif /* __STM32F4xx_HAL_I2S_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
