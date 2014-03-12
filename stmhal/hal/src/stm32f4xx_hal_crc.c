/**
  ******************************************************************************
  * @file    stm32f4xx_hal_crc.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   CRC HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Cyclic Redundancy Check (CRC) peripheral:
  *           + Initialization and de-initialization functions
  *           + Peripheral Control functions 
  *           + Peripheral State functions
  *
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
    [..]
      The CRC HAL driver can be used as follows:

      (#) Enable CRC AHB clock using __CRC_CLK_ENABLE();

      (#) Use HAL_CRC_Accumulate() function to compute the CRC value of 
          a 32-bit data buffer using combination of the previous CRC value
          and the new one.

      (#) Use HAL_CRC_Calculate() function to compute the CRC Value of 
          a new 32-bit data buffer. This function resets the CRC computation  
          unit before starting the computation to avoid getting wrong CRC values.

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

/** @defgroup CRC 
  * @brief CRC HAL module driver.
  * @{
  */

#ifdef HAL_CRC_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup CRC_Private_Functions
  * @{
  */

/** @defgroup CRC_Group1 Initialization and de-initialization functions 
 *  @brief    Initialization and Configuration functions. 
 *
@verbatim    
  ==============================================================================
            ##### Initialization and de-initialization functions #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize the CRC according to the specified parameters 
          in the CRC_InitTypeDef and create the associated handle
      (+) DeInitialize the CRC peripheral
      (+) Initialize the CRC MSP
      (+) DeInitialize CRC MSP 
 
@endverbatim
  * @{
  */

/**
  * @brief  Initializes the CRC according to the specified
  *         parameters in the CRC_InitTypeDef and creates the associated handle.
  * @param  hcrc: CRC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRC_Init(CRC_HandleTypeDef *hcrc)
{
  /* Check the CRC handle allocation */
  if(hcrc == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CRC_ALL_INSTANCE(hcrc->Instance));

  if(hcrc->State == HAL_CRC_STATE_RESET)
  {
    /* Init the low level hardware */
    HAL_CRC_MspInit(hcrc);
  }
  
  /* Change CRC peripheral state */
  hcrc->State = HAL_CRC_STATE_BUSY;
   
  /* Change CRC peripheral state */
  hcrc->State = HAL_CRC_STATE_READY;
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  DeInitializes the CRC peripheral.
  * @param  hcrc: CRC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRC_DeInit(CRC_HandleTypeDef *hcrc)
{
  /* Check the CRC handle allocation */
  if(hcrc == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CRC_ALL_INSTANCE(hcrc->Instance));

  /* Change CRC peripheral state */
  hcrc->State = HAL_CRC_STATE_BUSY;

  /* DeInit the low level hardware */
  HAL_CRC_MspDeInit(hcrc);

  /* Change CRC peripheral state */
  hcrc->State = HAL_CRC_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hcrc);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRC MSP.
  * @param  hcrc: CRC handle
  * @retval None
  */
__weak void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CRC_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitializes the CRC MSP.
  * @param  hcrc: CRC handle
  * @retval None
  */
__weak void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CRC_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup CRC_Group2 Peripheral Control functions 
 *  @brief    management functions. 
 *
@verbatim   
  ==============================================================================
                      ##### Peripheral Control functions #####
  ==============================================================================  
    [..]  This section provides functions allowing to:
      (+) Compute the 32-bit CRC value of 32-bit data buffer,
          using combination of the previous CRC value and the new one.
      (+) Compute the 32-bit CRC value of 32-bit data buffer,
          independently of the previous CRC value.

@endverbatim
  * @{
  */

/**
  * @brief  Computes the 32-bit CRC of 32-bit data buffer using combination
  *         of the previous CRC value and the new one.
  * @param  hcrc: CRC handle
  * @param  pBuffer: pointer to the buffer containing the data to be computed
  * @param  BufferLength: length of the buffer to be computed
  * @retval 32-bit CRC
  */
uint32_t HAL_CRC_Accumulate(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[], uint32_t BufferLength)
{
  uint32_t index = 0;

  /* Process Locked */
  __HAL_LOCK(hcrc);

  /* Change CRC peripheral state */
  hcrc->State = HAL_CRC_STATE_BUSY;

  /* Enter Data to the CRC calculator */
  for(index = 0; index < BufferLength; index++)
  {
    hcrc->Instance->DR = pBuffer[index];
  }

  /* Change CRC peripheral state */
  hcrc->State = HAL_CRC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hcrc);

  /* Return the CRC computed value */
  return hcrc->Instance->DR;
}

/**
  * @brief  Computes the 32-bit CRC of 32-bit data buffer independently
  *         of the previous CRC value.
  * @param  hcrc: CRC handle
  * @param  pBuffer: Pointer to the buffer containing the data to be computed
  * @param  BufferLength: Length of the buffer to be computed
  * @retval 32-bit CRC
  */
uint32_t HAL_CRC_Calculate(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[], uint32_t BufferLength)
{
  uint32_t index = 0;

  /* Process Locked */
  __HAL_LOCK(hcrc); 

  /* Change CRC peripheral state */
  hcrc->State = HAL_CRC_STATE_BUSY;

  /* Reset CRC Calculation Unit */
  __HAL_CRC_DR_RESET(hcrc);

  /* Enter Data to the CRC calculator */
  for(index = 0; index < BufferLength; index++)
  {
    hcrc->Instance->DR = pBuffer[index];
  }

  /* Change CRC peripheral state */
  hcrc->State = HAL_CRC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hcrc);

  /* Return the CRC computed value */
  return hcrc->Instance->DR;
}

/**
  * @}
  */

/** @defgroup CRC_Group3 Peripheral State functions 
 *  @brief    Peripheral State functions. 
 *
@verbatim   
  ==============================================================================
                      ##### Peripheral State functions #####
  ==============================================================================  
    [..]
    This subsection permits to get in run-time the status of the peripheral 
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Returns the CRC state.
  * @param  hcrc: CRC handle
  * @retval HAL state
  */
HAL_CRC_StateTypeDef HAL_CRC_GetState(CRC_HandleTypeDef *hcrc)
{
  return hcrc->State;
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_CRC_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
