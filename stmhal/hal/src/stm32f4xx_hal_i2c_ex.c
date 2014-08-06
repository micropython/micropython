/**
  ******************************************************************************
  * @file    stm32f4xx_hal_i2c_ex.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   I2C Extension HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of I2C extension peripheral:
  *           + Extension features functions
  *    
  @verbatim
  ==============================================================================
               ##### I2C peripheral extension features  #####
  ==============================================================================
           
  [..] Comparing to other previous devices, the I2C interface for STM32F427xx/437xx/ 
       429xx/439xx devices contains the following additional features :
       
       (+) Possibility to disable or enable Analog Noise Filter
       (+) Use of a configured Digital Noise Filter
   
                     ##### How to use this driver #####
  ==============================================================================
  [..] This driver provides functions to configure Noise Filter
    (#) Configure I2C Analog noise filter using the function HAL_I2C_AnalogFilter_Config()
    (#) Configure I2C Digital noise filter using the function HAL_I2C_DigitalFilter_Config()
  
  @endverbatim
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup I2CEx 
  * @brief I2C HAL module driver
  * @{
  */

#ifdef HAL_I2C_MODULE_ENABLED

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx)|| defined(STM32F439xx) ||\
    defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F411xE)
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup I2CEx_Private_Functions
  * @{
  */


/** @defgroup I2CEx_Group1 Extension features functions 
 *  @brief   Extension features functions 
 *
@verbatim   
 ===============================================================================
                      ##### Extension features functions #####
 ===============================================================================  
    [..] This section provides functions allowing to:
      (+) Configure Noise Filters 

@endverbatim
  * @{
  */
  
/**
  * @brief  Configures I2C Analog noise filter. 
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2Cx peripheral.
  * @param  AnalogFilter: new state of the Analog filter.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2CEx_AnalogFilter_Config(I2C_HandleTypeDef *hi2c, uint32_t AnalogFilter)
{
  uint32_t tmp = 0;
  
  /* Check the parameters */
  assert_param(IS_I2C_ALL_INSTANCE(hi2c->Instance));
  assert_param(IS_I2C_ANALOG_FILTER(AnalogFilter));
  
  tmp = hi2c->State;
  if((tmp == HAL_I2C_STATE_BUSY) || (tmp == HAL_I2C_STATE_BUSY_TX) || (tmp == HAL_I2C_STATE_BUSY_RX))
  {
    return HAL_BUSY;
  }
  
  hi2c->State = HAL_I2C_STATE_BUSY;
  
  /* Disable the selected I2C peripheral */
  __HAL_I2C_DISABLE(hi2c);    
  
  /* Reset I2Cx ANOFF bit */
  hi2c->Instance->FLTR &= ~(I2C_FLTR_ANOFF);    
  
  /* Disable the analog filter */
  hi2c->Instance->FLTR |= AnalogFilter;
  
  __HAL_I2C_ENABLE(hi2c); 
  
  hi2c->State = HAL_I2C_STATE_READY;
  
  return HAL_OK; 
}

/**
  * @brief  Configures I2C Digital noise filter. 
  * @param  hi2c: pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2Cx peripheral.
  * @param  DigitalFilter: Coefficient of digital noise filter between 0x00 and 0x0F.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2CEx_DigitalFilter_Config(I2C_HandleTypeDef *hi2c, uint32_t DigitalFilter)
{
  uint16_t tmpreg = 0;
  uint32_t tmp = 0;  
  
  /* Check the parameters */
  assert_param(IS_I2C_ALL_INSTANCE(hi2c->Instance));
  assert_param(IS_I2C_DIGITAL_FILTER(DigitalFilter));
  
  tmp = hi2c->State;
  if((tmp == HAL_I2C_STATE_BUSY) || (tmp == HAL_I2C_STATE_BUSY_TX) || (tmp == HAL_I2C_STATE_BUSY_RX))
  {
    return HAL_BUSY;
  }
  
  hi2c->State = HAL_I2C_STATE_BUSY;
  
  /* Disable the selected I2C peripheral */
  __HAL_I2C_DISABLE(hi2c);  
  
  /* Get the old register value */
  tmpreg = hi2c->Instance->FLTR;
  
  /* Reset I2Cx DNF bit [3:0] */
  tmpreg &= ~(I2C_FLTR_DNF);
  
  /* Set I2Cx DNF coefficient */
  tmpreg |= DigitalFilter;
  
  /* Store the new register value */
  hi2c->Instance->FLTR = tmpreg;
  
  __HAL_I2C_ENABLE(hi2c); 
  
  hi2c->State = HAL_I2C_STATE_READY;
  
  return HAL_OK; 
}  

/**
  * @}
  */

/**
  * @}
  */  
#endif /* STM32F427xx || STM32F429xx || STM32F437xx || STM32F439xx || STM32F401xC || STM32F401xE */

#endif /* HAL_I2C_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
