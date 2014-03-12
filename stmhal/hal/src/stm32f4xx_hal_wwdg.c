/**
  ******************************************************************************
  * @file    stm32f4xx_hal_wwdg.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   WWDG HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Window Watchdog (WWDG) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral State functions       
  @verbatim
  ==============================================================================
                      ##### WWDG specific features #####
  ==============================================================================
  [..] 
    Once enabled the WWDG generates a system reset on expiry of a programmed
    time period, unless the program refreshes the counter (downcounter) 
    before reaching 0x3F value (i.e. a reset is generated when the counter
    value rolls over from 0x40 to 0x3F). 
       
    (+) An MCU reset is also generated if the counter value is refreshed
        before the counter has reached the refresh window value. This 
        implies that the counter must be refreshed in a limited window.
    (+) Once enabled the WWDG cannot be disabled except by a system reset.
    (+) WWDGRST flag in RCC_CSR register can be used to inform when a WWDG
        reset occurs.               
    (+) The WWDG counter input clock is derived from the APB clock divided 
        by a programmable prescaler.
    (+) WWDG clock (Hz) = PCLK1 / (4096 * Prescaler)
    (+) WWDG timeout (mS) = 1000 * Counter / WWDG clock
    (+) WWDG Counter refresh is allowed between the following limits :
        (++) min time (mS) = 1000 * (Counter – Window) / WWDG clock
        (++) max time (mS) = 1000 * (Counter – 0x40) / WWDG clock
    
    (+) Min-max timeout value at 50 MHz(PCLK1): 81.9 us / 41.9 ms 


                     ##### How to use this driver #####
  ==============================================================================
  [..]
    (+) Enable WWDG APB1 clock using __WWDG_CLK_ENABLE().
    (+) Set the WWDG prescaler, refresh window and counter value 
        using HAL_WWDG_Init() function.
    (+) Start the WWDG using HAL_WWDG_Start() function.
        When the WWDG is enabled the counter value should be configured to 
        a value greater than 0x40 to prevent generating an immediate reset.
    (+) Optionally you can enable the Early Wakeup Interrupt (EWI) which is 
        generated when the counter reaches 0x40, and then start the WWDG using
        HAL_WWDG_Start_IT(). At EWI HAL_WWDG_WakeupCallback is executed and user can 
        add his own code by customization of function pointer HAL_WWDG_WakeupCallback
        Once enabled, EWI interrupt cannot be disabled except by a system reset.          
    (+) Then the application program must refresh the WWDG counter at regular
        intervals during normal operation to prevent an MCU reset, using
        HAL_WWDG_Refresh() function. This operation must occur only when
        the counter is lower than the refresh window value already programmed.
        
     *** WWDG HAL driver macros list ***
     ==================================
     [..]
       Below the list of most used macros in WWDG HAL driver.
       
      (+) __HAL_WWDG_ENABLE: Enable the WWDG peripheral 
      (+) __HAL_IWDG_GET_FLAG: Get the selected WWDG's flag status
      (+) __HAL_IWDG_CLEAR_FLAG: Clear the WWDG's pending flags 
      (+) __HAL_IWDG_RELOAD_COUNTER: Enables the WWDG early wakeup interrupt  

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

/** @defgroup WWDG 
  * @brief WWDG HAL module driver.
  * @{
  */

#ifdef HAL_WWDG_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup WWDG_Private_Functions
  * @{
  */

/** @defgroup WWDG_Group1 Initialization and de-initialization functions 
 *  @brief    Initialization and Configuration functions. 
 *
@verbatim    
  ==============================================================================
          ##### Initialization and de-initialization functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to:
    (+) Initialize the WWDG according to the specified parameters 
        in the WWDG_InitTypeDef and create the associated handle
    (+) DeInitialize the WWDG peripheral
    (+) Initialize the WWDG MSP
    (+) DeInitialize the WWDG MSP 
 
@endverbatim
  * @{
  */

/**
  * @brief  Initializes the WWDG according to the specified
  *         parameters in the WWDG_InitTypeDef and creates the associated handle.
  * @param  hwwdg: WWDG handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_WWDG_Init(WWDG_HandleTypeDef *hwwdg)
{
  uint32_t tmp = 0;
  
  /* Check the WWDG handle allocation */
  if(hwwdg == NULL)
  {
    return HAL_ERROR;
  }
  
  /* Check the parameters */
  assert_param(IS_WWDG_PRESCALER(hwwdg->Init.Prescaler));
  assert_param(IS_WWDG_WINDOW(hwwdg->Init.Window)); 
  assert_param(IS_WWDG_COUNTER(hwwdg->Init.Counter)); 
  
  if(hwwdg->State == HAL_WWDG_STATE_RESET)
  {
    /* Init the low level hardware */
    HAL_WWDG_MspInit(hwwdg);
  }
  
  /* Change WWDG peripheral state */
  hwwdg->State = HAL_WWDG_STATE_BUSY;

  /* Set WWDG Prescaler and Window */
  /* Get the CFR register value */
  tmp = hwwdg->Instance->CFR;
  
  /* Clear WDGTB[1:0] and W[6:0] bits */
  tmp &= ((uint32_t)~(WWDG_CFR_WDGTB | WWDG_CFR_W));
  
  /* Prepare the WWDG Prescaler and Window parameters */
  tmp |= hwwdg->Init.Prescaler | hwwdg->Init.Window;
  
  /* Write to WWDG CFR */
  hwwdg->Instance->CFR = tmp;   
 
  /* Set WWDG Counter */
  /* Get the CR register value */
  tmp = hwwdg->Instance->CR;
  
  /* Clear T[6:0] bits */
  tmp &= ((uint32_t)~(WWDG_CR_T));
  
  /* Prepare the WWDG Counter parameter */
  tmp |= (hwwdg->Init.Counter);  
  
  /* Write to WWDG CR */
  hwwdg->Instance->CR = tmp;  
   
  /* Change WWDG peripheral state */
  hwwdg->State = HAL_WWDG_STATE_READY;
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  DeInitializes the WWDG peripheral. 
  * @param  hwwdg: WWDG handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_WWDG_DeInit(WWDG_HandleTypeDef *hwwdg)
{   
  /* Change WWDG peripheral state */  
  hwwdg->State = HAL_WWDG_STATE_BUSY;
  
  /* DeInit the low level hardware */
  HAL_WWDG_MspDeInit(hwwdg);
  
  /* Reset WWDG Control register */
  hwwdg->Instance->CR  = (uint32_t)0x0000007F;
  
  /* Reset WWDG Configuration register */
  hwwdg->Instance->CFR = (uint32_t)0x0000007F;
  
  /* Reset WWDG Status register */
  hwwdg->Instance->SR  = 0; 
  
  /* Change WWDG peripheral state */    
  hwwdg->State = HAL_WWDG_STATE_RESET; 

  /* Release Lock */
  __HAL_UNLOCK(hwwdg);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the WWDG MSP.
  * @param  hwwdg: WWDG handle
  * @retval None
  */
__weak void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_WWDG_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitializes the WWDG MSP.
  * @param  hwwdg: WWDG handle
  * @retval None
  */
__weak void HAL_WWDG_MspDeInit(WWDG_HandleTypeDef *hwwdg)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_WWDG_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup WWDG_Group2 IO operation functions 
 *  @brief    IO operation functions 
 *
@verbatim   
  ==============================================================================
                       ##### IO operation functions #####
  ==============================================================================  
  [..]  
    This section provides functions allowing to:
    (+) Start the WWDG.
    (+) Refresh the WWDG.
    (+) Handle WWDG interrupt request. 

@endverbatim
  * @{
  */

/**
  * @brief  Starts the WWDG.
  * @param  hwwdg: WWDG handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_WWDG_Start(WWDG_HandleTypeDef *hwwdg)
{
  /* Process Locked */
  __HAL_LOCK(hwwdg); 
  
  /* Change WWDG peripheral state */  
  hwwdg->State = HAL_WWDG_STATE_BUSY;

  /* Enable the peripheral */
  __HAL_WWDG_ENABLE(hwwdg);  
  
  /* Change WWDG peripheral state */    
  hwwdg->State = HAL_WWDG_STATE_READY; 
  
  /* Process Unlocked */
  __HAL_UNLOCK(hwwdg);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Starts the WWDG with interrupt enabled.
  * @param  hwwdg: WWDG handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_WWDG_Start_IT(WWDG_HandleTypeDef *hwwdg)
{   
  /* Process Locked */
  __HAL_LOCK(hwwdg); 
  
  /* Change WWDG peripheral state */  
  hwwdg->State = HAL_WWDG_STATE_BUSY;
  
  /* Enable the Early Wakeup Interrupt */ 
  __HAL_WWDG_ENABLE_IT(WWDG_IT_EWI);

  /* Enable the peripheral */
  __HAL_WWDG_ENABLE(hwwdg);  
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Refreshes the WWDG.
  * @param  hwwdg: WWDG handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_WWDG_Refresh(WWDG_HandleTypeDef *hwwdg, uint32_t Counter)
{
  /* Process Locked */
  __HAL_LOCK(hwwdg); 
  
  /* Change WWDG peripheral state */  
  hwwdg->State = HAL_WWDG_STATE_BUSY;
  
  /* Check the parameters */
  assert_param(IS_WWDG_COUNTER(Counter));
  
  /* Write to WWDG CR the WWDG Counter value to refresh with */
  MODIFY_REG(hwwdg->Instance->CR, WWDG_CR_T, Counter);
  
  /* Change WWDG peripheral state */    
  hwwdg->State = HAL_WWDG_STATE_READY; 
  
  /* Process Unlocked */
  __HAL_UNLOCK(hwwdg);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Handles WWDG interrupt request.
  * @note   The Early Wakeup Interrupt (EWI) can be used if specific safety operations 
  *         or data logging must be performed before the actual reset is generated. 
  *         The EWI interrupt is enabled using __HAL_WWDG_ENABLE_IT() macro.
  *         When the downcounter reaches the value 0x40, and EWI interrupt is 
  *         generated and the corresponding Interrupt Service Routine (ISR) can 
  *         be used to trigger specific actions (such as communications or data 
  *         logging), before resetting the device. 
  * @param  hwwdg: WWDG handle
  * @retval None
  */
void HAL_WWDG_IRQHandler(WWDG_HandleTypeDef *hwwdg)
{ 
  /* WWDG Early Wakeup Interrupt occurred */   
  if(__HAL_WWDG_GET_FLAG(hwwdg, WWDG_FLAG_EWIF) != RESET)
  {
    /* Early Wakeup callback */ 
    HAL_WWDG_WakeupCallback(hwwdg);
    
    /* Change WWDG peripheral state */
    hwwdg->State = HAL_WWDG_STATE_READY; 
    
    /* Clear the WWDG Data Ready flag */
    __HAL_WWDG_CLEAR_FLAG(hwwdg, WWDG_FLAG_EWIF);
    
    /* Process Unlocked */
    __HAL_UNLOCK(hwwdg);
  }
} 

/**
  * @brief  Early Wakeup WWDG callback.
  * @param  hwwdg: WWDG handle
  * @retval None
  */
__weak void HAL_WWDG_WakeupCallback(WWDG_HandleTypeDef* hwwdg)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_WWDG_WakeupCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup WWDG_Group3 Peripheral State functions 
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
  * @brief  Returns the WWDG state.
  * @param  hwwdg: WWDG handle
  * @retval HAL state
  */
HAL_WWDG_StateTypeDef HAL_WWDG_GetState(WWDG_HandleTypeDef *hwwdg)
{
  return hwwdg->State;
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_WWDG_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
