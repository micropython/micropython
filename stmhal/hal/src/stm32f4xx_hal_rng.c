/**
  ******************************************************************************
  * @file    stm32f4xx_hal_rng.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   RNG HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Random Number Generator (RNG) peripheral:
  *           + Initialization/de-initialization functions
  *           + Peripheral Control functions 
  *           + Peripheral State functions
  *         
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
    [..]
      The RNG HAL driver can be used as follows:

         (#) Enable the RNG controller clock using __RNG_CLK_ENABLE() macro.
         (#) Activate the RNG peripheral using __HAL_RNG_ENABLE() macro.
         (#) Wait until the 32 bit Random Number Generator contains a valid 
             random data using (polling/interrupt) mode.   
         (#) Get the 32 bit random number using HAL_RNG_GetRandomNumber() function.
  
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

/** @defgroup RNG 
  * @brief RNG HAL module driver.
  * @{
  */

#ifdef HAL_RNG_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define RNG_TIMEOUT_VALUE     1000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup RNG_Private_Functions
  * @{
  */

/** @defgroup RNG_Group1 Initialization and de-initialization functions 
 *  @brief    Initialization and Configuration functions. 
 *
@verbatim    
 ===============================================================================
          ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize the RNG according to the specified parameters 
          in the RNG_InitTypeDef and create the associated handle
      (+) DeInitialize the RNG peripheral
      (+) Initialize the RNG MSP
      (+) DeInitialize RNG MSP 
 
@endverbatim
  * @{
  */

/**
  * @brief  Initializes the RNG according to the specified
  *         parameters in the RNG_InitTypeDef and creates the associated handle.
  * @param  hrng: RNG handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RNG_Init(RNG_HandleTypeDef *hrng)
{ 
  /* Check the RNG handle allocation */
  if(hrng == NULL)
  {
    return HAL_ERROR;
  }

  if(hrng->State == HAL_RNG_STATE_RESET)
  {  
    /* Init the low level hardware */
    HAL_RNG_MspInit(hrng);
  }

  /* Enable the RNG Peripheral */
  __HAL_RNG_ENABLE(hrng);
  
  /* Initialize the RNG state */
  hrng->State = HAL_RNG_STATE_READY;
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  DeInitializes the RNG peripheral. 
  * @param  hrng: RNG handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RNG_DeInit(RNG_HandleTypeDef *hrng)
{ 
  /* Check the RNG peripheral state */
  if(hrng->State == HAL_RNG_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  
  /* Update the RNG state */  
  hrng->State = HAL_RNG_STATE_BUSY;
  
  /* Disable the RNG Peripheral */
  __HAL_RNG_DISABLE(hrng);
  
  /* Set the RNG registers to their reset values */
  hrng->Instance->CR &= 0xFFFFFFF3;
  hrng->Instance->SR &= 0xFFFFFF98;
  hrng->Instance->DR &= 0x0;
  
  /* DeInit the low level hardware */
  HAL_RNG_MspDeInit(hrng);
  
  /* Update the RNG state */
  hrng->State = HAL_RNG_STATE_RESET; 

  /* Release Lock */
  __HAL_UNLOCK(hrng);
  
  /* Return the function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the RNG MSP.
  * @param  hrng: RNG handle
  * @retval None
  */
__weak void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_RNG_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitializes the RNG MSP.
  * @param  hrng: RNG handle
  * @retval None
  */
__weak void HAL_RNG_MspDeInit(RNG_HandleTypeDef *hrng)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_RNG_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup RNG_Group2 Peripheral Control functions 
 *  @brief    management functions. 
 *
@verbatim   
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================  
    [..]  This section provides functions allowing to:
      (+) Get the 32 bit Random number
      (+) Get the 32 bit Random number with interrupt enabled
      (+) Handle RNG interrupt request 


@endverbatim
  * @{
  */

/**
  * @brief  Returns a 32-bit random number.
  * @note   Each time the random number data is read the RNG_FLAG_DRDY flag 
  *         is automatically cleared.
  * @param  hrng: RNG handle
  * @retval 32-bit random number
  */
uint32_t HAL_RNG_GetRandomNumber(RNG_HandleTypeDef *hrng)
{
  uint32_t random32bit = 0;
  uint32_t timeout = 0;   
  
  /* Process Locked */
  __HAL_LOCK(hrng); 
  
  timeout = HAL_GetTick() + RNG_TIMEOUT_VALUE;
  
  /* Check if data register contains valid random data */
  while(__HAL_RNG_GET_FLAG(hrng, RNG_FLAG_DRDY) == RESET)
  {
    if(HAL_GetTick() >= timeout)
    {    
      return HAL_TIMEOUT;
    } 
  }
  
  /* Get a 32bit Random number */ 
  random32bit = hrng->Instance->DR;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hrng);
  
  /* Return the 32 bit random number */   
  return random32bit;
}

/**
  * @brief  Returns a 32-bit random number with interrupt enabled.
  * @param  hrng: RNG handle
  * @retval 32-bit random number
  */
uint32_t HAL_RNG_GetRandomNumber_IT(RNG_HandleTypeDef *hrng)
{
  uint32_t random32bit = 0;
  
  /* Process Locked */
  __HAL_LOCK(hrng);
  
  /* Change RNG peripheral state */  
  hrng->State = HAL_RNG_STATE_BUSY;  
  
  /* Get a 32bit Random number */ 
  random32bit = hrng->Instance->DR;
  
  /* Enable the RNG Interrupts: Data Ready, Clock error, Seed error */ 
  __HAL_RNG_ENABLE_IT(hrng); 
  
  /* Return the 32 bit random number */   
  return random32bit;
}

/**
  * @brief  Handles RNG interrupt request.
  * @note   In the case of a clock error, the RNG is no more able to generate 
  *         random numbers because the PLL48CLK clock is not correct. User has 
  *         to check that the clock controller is correctly configured to provide
  *         the RNG clock and clear the CEIS bit using __HAL_RNG_CLEAR_FLAG(). 
  *         The clock error has no impact on the previously generated 
  *         random numbers, and the RNG_DR register contents can be used.
  * @note   In the case of a seed error, the generation of random numbers is 
  *         interrupted as long as the SECS bit is '1'. If a number is 
  *         available in the RNG_DR register, it must not be used because it may 
  *         not have enough entropy. In this case, it is recommended to clear the 
  *         SEIS bit using __HAL_RNG_CLEAR_FLAG(), then disable and enable 
  *         the RNG peripheral to reinitialize and restart the RNG.
  * @param  hrng: RNG handle
  * @retval None

  */
void HAL_RNG_IRQHandler(RNG_HandleTypeDef *hrng)
{
  /* RNG clock error interrupt occured */
  if(__HAL_RNG_GET_FLAG(hrng, RNG_IT_CEI) != RESET)
  { 
    HAL_RNG_ErrorCallback(hrng);
    
    /* Clear the clock error flag */
    __HAL_RNG_CLEAR_FLAG(hrng, RNG_IT_CEI);
    
    /* Change RNG peripheral state */
    hrng->State = HAL_RNG_STATE_ERROR;
    
    /* Process Unlocked */
    __HAL_UNLOCK(hrng);
  }
  
  /* RNG seed error interrupt occured */
  if(__HAL_RNG_GET_FLAG(hrng, RNG_IT_SEI) != RESET)
  { 
    HAL_RNG_ErrorCallback(hrng);
    
    /* Clear the seed error flag */
    __HAL_RNG_CLEAR_FLAG(hrng, RNG_IT_SEI);
    
    /* Change RNG peripheral state */
    hrng->State = HAL_RNG_STATE_ERROR;

    /* Process Unlocked */
    __HAL_UNLOCK(hrng);    
  }
  
  /* Check RNG data ready flag */    
  if(__HAL_RNG_GET_FLAG(hrng, RNG_FLAG_DRDY) != RESET)
  {
    /* Data Ready callback */ 
    HAL_RNG_ReadyCallback(hrng);
    
    /* Change RNG peripheral state */
    hrng->State = HAL_RNG_STATE_READY; 
      
    /* Clear the RNG Data Ready flag */
    __HAL_RNG_CLEAR_FLAG(hrng, RNG_FLAG_DRDY);
    
    /* Process Unlocked */
    __HAL_UNLOCK(hrng);
  }
} 

/**
  * @brief  Data Ready callback in non-blocking mode. 
  * @param  hrng: RNG handle
  * @retval None
  */

__weak void HAL_RNG_ReadyCallback(RNG_HandleTypeDef* hrng)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_RNG_ReadyCallback could be implemented in the user file
   */
}

/**
  * @brief  RNG error callbacks.
  * @param  hrng: RNG handle
  * @retval None
  */
__weak void HAL_RNG_ErrorCallback(RNG_HandleTypeDef *hrng)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_RNG_ErrorCallback could be implemented in the user file
   */ 
}
 
/**
  * @}
  */

/** @defgroup RNG_Group3 Peripheral State functions 
 *  @brief    Peripheral State functions. 
 *
@verbatim   
 ===============================================================================
                      ##### Peripheral State functions #####
 ===============================================================================  
    [..]
    This subsection permits to get in run-time the status of the peripheral 
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Returns the RNG state.
  * @param  hrng: RNG handle
  * @retval HAL state
  */
HAL_RNG_StateTypeDef HAL_RNG_GetState(RNG_HandleTypeDef *hrng)
{
  return hrng->State;
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_RNG_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
                  
