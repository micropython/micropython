/**
  ******************************************************************************
  * @file    stm32f4xx_hal_i2c_ex.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of I2C HAL Extension module.
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
#ifndef __STM32F4xx_HAL_I2C_EX_H
#define __STM32F4xx_HAL_I2C_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx)|| defined(STM32F439xx) ||\
    defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F411xE)
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"  

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup I2CEx
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 
/* Exported constants --------------------------------------------------------*/

/** @defgroup I2CEx_Exported_Constants
  * @{
  */

/** @defgroup I2CEx_Analog_Filter
  * @{
  */
#define I2C_ANALOGFILTER_ENABLED        ((uint32_t)0x00000000)
#define I2C_ANALOGFILTER_DISABLED       I2C_FLTR_ANOFF

#define IS_I2C_ANALOG_FILTER(FILTER) (((FILTER) == I2C_ANALOGFILTER_ENABLED) || \
                                      ((FILTER) == I2C_ANALOGFILTER_DISABLED))
/**
  * @}
  */

/** @defgroup I2CEx_Digital_Filter
  * @{
  */
#define IS_I2C_DIGITAL_FILTER(FILTER)   ((FILTER) <= 0x0000000F)
/**
  * @}
  */
  
/**
  * @}
  */ 
  
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/* Peripheral Control functions  ************************************************/
HAL_StatusTypeDef HAL_I2CEx_AnalogFilter_Config(I2C_HandleTypeDef *hi2c, uint32_t AnalogFilter);
HAL_StatusTypeDef HAL_I2CEx_DigitalFilter_Config(I2C_HandleTypeDef *hi2c, uint32_t DigitalFilter);

/**
  * @}
  */ 
#endif /* STM32F427xx || STM32F429xx || STM32F437xx || STM32F439xx || STM32F401xC || STM32F401xE */
/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_I2C_EX_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
