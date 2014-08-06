/**
  ******************************************************************************
  * @file    stm32f4xx_hal_adc.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Analog to Digital Convertor (ADC) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + State and errors functions
  *         
  @verbatim
  ==============================================================================
                    ##### ADC Peripheral features #####
  ==============================================================================
  [..] 
  (#) 12-bit, 10-bit, 8-bit or 6-bit configurable resolution.
  (#) Interrupt generation at the end of conversion, end of injected conversion,  
      and in case of analog watchdog or overrun events
  (#) Single and continuous conversion modes.
  (#) Scan mode for automatic conversion of channel 0 to channel x.
  (#) Data alignment with in-built data coherency.
  (#) Channel-wise programmable sampling time.
  (#) External trigger option with configurable polarity for both regular and 
      injected conversion.
  (#) Dual/Triple mode (on devices with 2 ADCs or more).
  (#) Configurable DMA data storage in Dual/Triple ADC mode. 
  (#) Configurable delay between conversions in Dual/Triple interleaved mode.
  (#) ADC conversion type (refer to the datasheets).
  (#) ADC supply requirements: 2.4 V to 3.6 V at full speed and down to 1.8 V at 
      slower speed.
  (#) ADC input range: VREF(minus) = VIN = VREF(plus).
  (#) DMA request generation during regular channel conversion.


                     ##### How to use this driver #####
  ==============================================================================
    [..]
    (#)Initialize the ADC low level resources by implementing the HAL_ADC_MspInit():
       (##) Enable the ADC interface clock using __ADC_CLK_ENABLE()
       (##) ADC pins configuration
             (+++) Enable the clock for the ADC GPIOs using the following function:
                   __GPIOx_CLK_ENABLE()  
             (+++) Configure these ADC pins in analog mode using HAL_GPIO_Init() 
       (##) In case of using interrupts (e.g. HAL_ADC_Start_IT())
             (+++) Configure the ADC interrupt priority using HAL_NVIC_SetPriority()
             (+++) Enable the ADC IRQ handler using HAL_NVIC_EnableIRQ()
             (+++) In ADC IRQ handler, call HAL_ADC_IRQHandler()
      (##) In case of using DMA to control data transfer (e.g. HAL_ADC_Start_DMA())
             (+++) Enable the DMAx interface clock using __DMAx_CLK_ENABLE()
             (+++) Configure and enable two DMA streams stream for managing data
                 transfer from peripheral to memory (output stream)
             (+++) Associate the initilalized DMA handle to the CRYP DMA handle
                 using  __HAL_LINKDMA()
             (+++) Configure the priority and enable the NVIC for the transfer complete
                 interrupt on the two DMA Streams. The output stream should have higher
                 priority than the input stream.
                       
     (#) Configure the ADC Prescaler, conversion resolution and data alignment 
         using the HAL_ADC_Init() function.
         
     (#) Configure the ADC regular channels group features, use HAL_ADC_Init()
         and HAL_ADC_ConfigChannel() functions.
         
     (#) Three operation modes are available within this driver :     
  
     *** Polling mode IO operation ***
     =================================
     [..]    
       (+) Start the ADC peripheral using HAL_ADC_Start() 
       (+) Wait for end of conversion using HAL_ADC_PollForConversion(), at this stage
           user can specify the value of timeout according to his end application      
       (+) To read the ADC converted values, use the HAL_ADC_GetValue() function.
       (+) Stop the ADC peripheral using HAL_ADC_Stop()
       
     *** Interrupt mode IO operation ***    
     ===================================
     [..]    
       (+) Start the ADC peripheral using HAL_ADC_Start_IT() 
       (+) Use HAL_ADC_IRQHandler() called under ADC_IRQHandler() Interrupt subroutine
       (+) At ADC end of conversion HAL_ADC_ConvCpltCallback() function is executed and user can 
            add his own code by customization of function pointer HAL_ADC_ConvCpltCallback 
       (+) In case of ADC Error, HAL_ADC_ErrorCallback() function is executed and user can 
            add his own code by customization of function pointer HAL_ADC_ErrorCallback
        (+) Stop the ADC peripheral using HAL_ADC_Stop_IT()     

     *** DMA mode IO operation ***    
     ==============================
     [..]    
       (+) Start the ADC peripheral using HAL_ADC_Start_DMA(), at this stage the user specify the length 
           of data to be transferred at each end of conversion 
       (+) At The end of data transfer by HAL_ADC_ConvCpltCallback() function is executed and user can 
            add his own code by customization of function pointer HAL_ADC_ConvCpltCallback 
       (+) In case of transfer Error, HAL_ADC_ErrorCallback() function is executed and user can 
            add his own code by customization of function pointer HAL_ADC_ErrorCallback
       (+) Stop the ADC peripheral using HAL_ADC_Stop_DMA()
                    
     *** ADC HAL driver macros list ***
     ============================================= 
     [..]
       Below the list of most used macros in ADC HAL driver.
       
      (+) __HAL_ADC_ENABLE : Enable the ADC peripheral
      (+) __HAL_ADC_DISABLE : Disable the ADC peripheral
      (+) __HAL_ADC_ENABLE_IT: Enable the ADC end of conversion interrupt
      (+) __HAL_ADC_DISABLE_IT: Disable the ADC end of conversion interrupt
      (+) __HAL_ADC_GET_IT_SOURCE: Check if the specified ADC interrupt source is enabled or disabled
      (+) __HAL_ADC_CLEAR_FLAG: Clear the ADC's pending flags
      (+) __HAL_ADC_GET_FLAG: Get the selected ADC's flag status
      (+) __HAL_ADC_GET_RESOLUTION: Return resolution bits in CR1 register 
      
     [..] 
       (@) You can refer to the ADC HAL driver header file for more useful macros          
  
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

/** @defgroup ADC 
  * @brief ADC driver modules
  * @{
  */ 

#ifdef HAL_ADC_MODULE_ENABLED
    
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void ADC_Init(ADC_HandleTypeDef* hadc);
static void ADC_DMAConvCplt(DMA_HandleTypeDef *hdma);
static void ADC_DMAError(DMA_HandleTypeDef *hdma);
static void ADC_DMAHalfConvCplt(DMA_HandleTypeDef *hdma); 
/* Private functions ---------------------------------------------------------*/

/** @defgroup ADC_Private_Functions
  * @{
  */ 

/** @defgroup ADC_Group1 Initialization and de-initialization functions 
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize and configure the ADC. 
      (+) De-initialize the ADC. 
         
@endverbatim
  * @{
  */

/**
  * @brief  Initializes the ADCx peripheral according to the specified parameters 
  *         in the ADC_InitStruct and initializes the ADC MSP.
  *           
  * @note   This function is used to configure the global features of the ADC ( 
  *         ClockPrescaler, Resolution, Data Alignment and number of conversion), however,
  *         the rest of the configuration parameters are specific to the regular
  *         channels group (scan mode activation, continuous mode activation,
  *         External trigger source and edge, DMA continuous request after the  
  *         last transfer and End of conversion selection).
  *             
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef* hadc)
{
  /* Check ADC handle */
  if(hadc == NULL)
  {
     return HAL_ERROR;
  }
  
  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  assert_param(IS_ADC_CLOCKPRESCALER(hadc->Init.ClockPrescaler));
  assert_param(IS_ADC_RESOLUTION(hadc->Init.Resolution));
  assert_param(IS_FUNCTIONAL_STATE(hadc->Init.ScanConvMode));
  assert_param(IS_FUNCTIONAL_STATE(hadc->Init.ContinuousConvMode));
  assert_param(IS_ADC_EXT_TRIG_EDGE(hadc->Init.ExternalTrigConvEdge)); 
  assert_param(IS_ADC_EXT_TRIG(hadc->Init.ExternalTrigConv));
  assert_param(IS_ADC_DATA_ALIGN(hadc->Init.DataAlign));
  assert_param(IS_ADC_REGULAR_LENGTH(hadc->Init.NbrOfConversion));
  assert_param(IS_FUNCTIONAL_STATE(hadc->Init.DMAContinuousRequests));
  assert_param(IS_ADC_EOCSelection(hadc->Init.EOCSelection));
  assert_param(IS_FUNCTIONAL_STATE(hadc->Init.DiscontinuousConvMode));
  
  if(hadc->State == HAL_ADC_STATE_RESET)
  {
    /* Init the low level hardware */
    HAL_ADC_MspInit(hadc);
  }
  
  /* Initialize the ADC state */
  hadc->State = HAL_ADC_STATE_BUSY;
  
  /* Set ADC parameters */
  ADC_Init(hadc);
  
  /* Set ADC error code to none */
  hadc->ErrorCode = HAL_ADC_ERROR_NONE;
  
  /* Initialize the ADC state */
  hadc->State = HAL_ADC_STATE_READY;

  /* Release Lock */
  __HAL_UNLOCK(hadc);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Deinitializes the ADCx peripheral registers to their default reset values. 
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADC_DeInit(ADC_HandleTypeDef* hadc)
{
  /* Check ADC handle */
  if(hadc == NULL)
  {
     return HAL_ERROR;
  } 
  
  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  
  /* Change ADC state */
  hadc->State = HAL_ADC_STATE_BUSY;
  
  /* DeInit the low level hardware */
  HAL_ADC_MspDeInit(hadc);
  
  /* Set ADC error code to none */
  hadc->ErrorCode = HAL_ADC_ERROR_NONE;
  
  /* Change ADC state */
  hadc->State = HAL_ADC_STATE_RESET;
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the ADC MSP.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.  
  * @retval None
  */
__weak void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_ADC_MspInit could be implemented in the user file
   */ 
}

/**
  * @brief  DeInitializes the ADC MSP.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.  
  * @retval None
  */
__weak void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_ADC_MspDeInit could be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup ADC_Group2 IO operation functions
 *  @brief    IO operation functions 
 *
@verbatim   
 ===============================================================================
             ##### IO operation functions #####
 ===============================================================================  
    [..]  This section provides functions allowing to:
      (+) Start conversion of regular channel.
      (+) Stop conversion of regular channel.
      (+) Start conversion of regular channel and enable interrupt.
      (+) Stop conversion of regular channel and disable interrupt.
      (+) Start conversion of regular channel and enable DMA transfer.
      (+) Stop conversion of regular channel and disable DMA transfer.
      (+) Handle ADC interrupt request. 
               
@endverbatim
  * @{
  */

/**
  * @brief  Enables ADC and starts conversion of the regular channels.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef* hadc)
{
  uint16_t i = 0;
  
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(hadc->Init.ContinuousConvMode));
  assert_param(IS_ADC_EXT_TRIG_EDGE(hadc->Init.ExternalTrigConvEdge)); 
  
  /* Process locked */
  __HAL_LOCK(hadc);
  
  /* Check if an injected conversion is ongoing */
  if(hadc->State == HAL_ADC_STATE_BUSY_INJ)
  {
    /* Change ADC state */
    hadc->State = HAL_ADC_STATE_BUSY_INJ_REG;  
  }
  else
  {
    /* Change ADC state */
    hadc->State = HAL_ADC_STATE_BUSY_REG;
  } 
    
  /* Check if ADC peripheral is disabled in order to enable it and wait during 
     Tstab time the ADC's stabilization */
  if((hadc->Instance->CR2 & ADC_CR2_ADON) != ADC_CR2_ADON)
  {  
    /* Enable the Peripheral */
    __HAL_ADC_ENABLE(hadc);
    
    /* Delay inserted to wait during Tstab time the ADC's stabilazation */
    for(; i <= 540; i++)
    {
      __NOP();
    }
  }

  /* Check if Multimode enabled */
  if(HAL_IS_BIT_CLR(ADC->CCR, ADC_CCR_MULTI))
  {
    /* if no external trigger present enable software conversion of regular channels */
    if(hadc->Init.ExternalTrigConvEdge == ADC_EXTERNALTRIGCONVEDGE_NONE)
    {
      /* Enable the selected ADC software conversion for regular group */
      hadc->Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
    }
  }
  else
  {
    /* if instance of handle correspond to ADC1 and  no external trigger present enable software conversion of regular channels */
    if((hadc->Instance == ADC1) && (hadc->Init.ExternalTrigConvEdge == ADC_EXTERNALTRIGCONVEDGE_NONE))
    {
      /* Enable the selected ADC software conversion for regular group */
        hadc->Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
    }
  }
  
  /* Process unlocked */
  __HAL_UNLOCK(hadc);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Disables ADC and stop conversion of regular channels.
  * 
  * @note   Caution: This function will stop also injected channels.  
  *
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  *
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADC_Stop(ADC_HandleTypeDef* hadc)
{
  /* Disable the Peripheral */
  __HAL_ADC_DISABLE(hadc);
  
  /* Change ADC state */
  hadc->State = HAL_ADC_STATE_READY;
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Poll for regular conversion complete
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @param  Timeout: Timeout value in millisecond.  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef* hadc, uint32_t Timeout)
{
  uint32_t tickstart = 0;
 
  /* Get tick */ 
  tickstart = HAL_GetTick();

  /* Check End of conversion flag */
  while(!(__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_EOC)))
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
      {
        hadc->State= HAL_ADC_STATE_TIMEOUT;
        /* Process unlocked */
        __HAL_UNLOCK(hadc);
        return HAL_TIMEOUT;
      }
    }
  }
  
  /* Check if an injected conversion is ready */
  if(hadc->State == HAL_ADC_STATE_EOC_INJ)
  {
    /* Change ADC state */
    hadc->State = HAL_ADC_STATE_EOC_INJ_REG;  
  }
  else
  {
    /* Change ADC state */
    hadc->State = HAL_ADC_STATE_EOC_REG;
  }
  
  /* Return ADC state */
  return HAL_OK;
}

/**
  * @brief  Poll for conversion event
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @param  EventType: the ADC event type.
  *          This parameter can be one of the following values:
  *            @arg AWD_EVENT: ADC Analog watch Dog event.
  *            @arg OVR_EVENT: ADC Overrun event.
  * @param  Timeout: Timeout value in millisecond.   
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADC_PollForEvent(ADC_HandleTypeDef* hadc, uint32_t EventType, uint32_t Timeout)
{
  uint32_t tickstart = 0;
  
  /* Check the parameters */
  assert_param(IS_ADC_EVENT_TYPE(EventType));

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Check selected event flag */
  while(!(__HAL_ADC_GET_FLAG(hadc,EventType)))
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
      {
        hadc->State= HAL_ADC_STATE_TIMEOUT;
        /* Process unlocked */
        __HAL_UNLOCK(hadc);
        return HAL_TIMEOUT;
      }
    }
  }
  
  /* Check analog watchdog flag */
  if(EventType == AWD_EVENT)
  {
     /* Change ADC state */
     hadc->State = HAL_ADC_STATE_AWD;
      
     /* Clear the ADCx's analog watchdog flag */
     __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_AWD);
  }
  else
  {
     /* Change ADC state */
     hadc->State = HAL_ADC_STATE_ERROR;
     
     /* Clear the ADCx's Overrun flag */
     __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_OVR);
  }
  
  /* Return ADC state */
  return HAL_OK;
}


/**
  * @brief  Enables the interrupt and starts ADC conversion of regular channels.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADC_Start_IT(ADC_HandleTypeDef* hadc)
{
  uint16_t i = 0;
  
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(hadc->Init.ContinuousConvMode));
  assert_param(IS_ADC_EXT_TRIG_EDGE(hadc->Init.ExternalTrigConvEdge));
  
  /* Process locked */
  __HAL_LOCK(hadc);
  
  /* Check if an injected conversion is ongoing */
  if(hadc->State == HAL_ADC_STATE_BUSY_INJ)
  {
    /* Change ADC state */
    hadc->State = HAL_ADC_STATE_BUSY_INJ_REG;  
  }
  else
  {
    /* Change ADC state */
    hadc->State = HAL_ADC_STATE_BUSY_REG;
  } 
  
  /* Set ADC error code to none */
  hadc->ErrorCode = HAL_ADC_ERROR_NONE;
  
  /* Check if ADC peripheral is disabled in order to enable it and wait during 
     Tstab time the ADC's stabilization */
  if((hadc->Instance->CR2 & ADC_CR2_ADON) != ADC_CR2_ADON)
  {  
    /* Enable the Peripheral */
    __HAL_ADC_ENABLE(hadc);
    
    /* Delay inserted to wait during Tstab time the ADC's stabilazation */
    for(; i <= 540; i++)
    {
      __NOP();
    }
  }
  
  /* Enable the ADC overrun interrupt */
  __HAL_ADC_ENABLE_IT(hadc, ADC_IT_OVR);
  
  /* Enable the ADC end of conversion interrupt for regular group */
  __HAL_ADC_ENABLE_IT(hadc, ADC_IT_EOC);
  
  /* Check if Multimode enabled */
  if(HAL_IS_BIT_CLR(ADC->CCR, ADC_CCR_MULTI))
  {
    /* if no externel trigger present enable software conversion of regular channels */
    if (hadc->Init.ExternalTrigConvEdge == ADC_EXTERNALTRIGCONVEDGE_NONE)
    {
      /* Enable the selected ADC software conversion for regular group */
      hadc->Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
    }
  }
  else
  {
    /* if instance of handle correspond to ADC1 and  no external trigger present enable software conversion of regular channels */
    if ((hadc->Instance == (ADC_TypeDef*)0x40012000) && (hadc->Init.ExternalTrigConvEdge == ADC_EXTERNALTRIGCONVEDGE_NONE))
    {
      /* Enable the selected ADC software conversion for regular group */
        hadc->Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
    }
  }
  
  /* Process unlocked */
  __HAL_UNLOCK(hadc);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Disables the interrupt and stop ADC conversion of regular channels.
  * 
  * @note   Caution: This function will stop also injected channels.  
  *
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADC_Stop_IT(ADC_HandleTypeDef* hadc)
{
  /* Disable the ADC end of conversion interrupt for regular group */
  __HAL_ADC_DISABLE_IT(hadc, ADC_IT_EOC);
  
  /* Disable the ADC end of conversion interrupt for injected group */
  __HAL_ADC_DISABLE_IT(hadc, ADC_CR1_JEOCIE);
  
  /* Enable the Periphral */
  __HAL_ADC_DISABLE(hadc);
  
  /* Change ADC state */
  hadc->State = HAL_ADC_STATE_READY;
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Handles ADC interrupt request  
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval None
  */
void HAL_ADC_IRQHandler(ADC_HandleTypeDef* hadc)
{
  uint32_t tmp1 = 0, tmp2 = 0;
  
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(hadc->Init.ContinuousConvMode));
  assert_param(IS_ADC_REGULAR_LENGTH(hadc->Init.NbrOfConversion));
  assert_param(IS_ADC_EOCSelection(hadc->Init.EOCSelection));
  
  tmp1 = __HAL_ADC_GET_FLAG(hadc, ADC_FLAG_EOC);
  tmp2 = __HAL_ADC_GET_IT_SOURCE(hadc, ADC_IT_EOC);
  /* Check End of conversion flag for regular channels */
  if(tmp1 && tmp2)
  {
    /* Check if an injected conversion is ready */
    if(hadc->State == HAL_ADC_STATE_EOC_INJ)
    {
      /* Change ADC state */
      hadc->State = HAL_ADC_STATE_EOC_INJ_REG;  
    }
    else
    {
      /* Change ADC state */
      hadc->State = HAL_ADC_STATE_EOC_REG;
    }
  
    if((hadc->Init.ContinuousConvMode == DISABLE) && (hadc->Init.ExternalTrigConvEdge == ADC_EXTERNALTRIGCONVEDGE_NONE))
    {
      if(hadc->Init.EOCSelection == EOC_SEQ_CONV)
      {   
        /* DISABLE the ADC end of conversion interrupt for regular group */
        __HAL_ADC_DISABLE_IT(hadc, ADC_IT_EOC);
        
        /* DISABLE the ADC overrun interrupt */
        __HAL_ADC_DISABLE_IT(hadc, ADC_IT_OVR);
      }
      else
      {
        if (hadc->NbrOfCurrentConversionRank == 0)
        {
          hadc->NbrOfCurrentConversionRank = hadc->Init.NbrOfConversion;
        }
        
        /* Decrement the number of conversion when an interrupt occurs */
        hadc->NbrOfCurrentConversionRank--;
        
        /* Check if all conversions are finished */
        if(hadc->NbrOfCurrentConversionRank == 0)
        {
          /* DISABLE the ADC end of conversion interrupt for regular group */
          __HAL_ADC_DISABLE_IT(hadc, ADC_IT_EOC);
          
          /* DISABLE the ADC overrun interrupt */
          __HAL_ADC_DISABLE_IT(hadc, ADC_IT_OVR);
        }
      }
    }
    
    /* Conversion complete callback */ 
    HAL_ADC_ConvCpltCallback(hadc);
    
   /* Clear the ADCx flag for regular end of conversion */
    __HAL_ADC_CLEAR_FLAG(hadc,ADC_FLAG_EOC);
  }
  
  tmp1 = __HAL_ADC_GET_FLAG(hadc, ADC_FLAG_JEOC);
  tmp2 = __HAL_ADC_GET_IT_SOURCE(hadc, ADC_IT_JEOC);                               
  /* Check End of conversion flag for injected channels */
  if(tmp1 && tmp2)
  {
    /* Check if a regular conversion is ready */
    if(hadc->State == HAL_ADC_STATE_EOC_REG)
    {
      /* Change ADC state */
      hadc->State = HAL_ADC_STATE_EOC_INJ_REG;  
    }
    else
    {
      /* Change ADC state */
      hadc->State = HAL_ADC_STATE_EOC_INJ;
    }
    
    tmp1 = HAL_IS_BIT_CLR(hadc->Instance->CR1, ADC_CR1_JAUTO);
    tmp2 = HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_JEXTEN);
    if(((hadc->Init.ContinuousConvMode == DISABLE) || tmp1) && tmp2)
    {
      /* DISABLE the ADC end of conversion interrupt for injected group */
      __HAL_ADC_DISABLE_IT(hadc, ADC_IT_JEOC);
    }
    
    /* Conversion complete callback */ 
    HAL_ADCEx_InjectedConvCpltCallback(hadc);
    
   /* Clear the ADCx flag for injected end of conversion */
    __HAL_ADC_CLEAR_FLAG(hadc,ADC_FLAG_JEOC);
  }
  
  tmp1 = __HAL_ADC_GET_FLAG(hadc, ADC_FLAG_AWD);
  tmp2 = __HAL_ADC_GET_IT_SOURCE(hadc, ADC_IT_AWD);                          
  /* Check Analog watchdog flag */
  if(tmp1 && tmp2)
  {
    /* Change ADC state */
    hadc->State = HAL_ADC_STATE_AWD;
      
    /* Clear the ADCx's Analog watchdog flag */
    __HAL_ADC_CLEAR_FLAG(hadc,ADC_FLAG_AWD);
    
    /* Level out of window callback */ 
    HAL_ADC_LevelOutOfWindowCallback(hadc);
  }
  
  tmp1 = __HAL_ADC_GET_FLAG(hadc, ADC_FLAG_OVR);
  tmp2 = __HAL_ADC_GET_IT_SOURCE(hadc, ADC_IT_OVR);
  /* Check Overrun flag */
  if(tmp1 && tmp2)
  {
    /* Change ADC state to overrun state */
    hadc->State = HAL_ADC_STATE_ERROR;
    
    /* Set ADC error code to overrun */
    hadc->ErrorCode |= HAL_ADC_ERROR_OVR;
    
    /* Clear the Overrun flag */
    __HAL_ADC_CLEAR_FLAG(hadc,ADC_FLAG_OVR);
    
    /* Error callback */ 
    HAL_ADC_ErrorCallback(hadc);
  }
}

/**
  * @brief  Enables ADC DMA request after last transfer (Single-ADC mode) and enables ADC peripheral  
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @param  pData: The destination Buffer address.
  * @param  Length: The length of data to be transferred from ADC peripheral to memory.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length)
{
  uint16_t i = 0;
  
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(hadc->Init.ContinuousConvMode));
  assert_param(IS_ADC_EXT_TRIG_EDGE(hadc->Init.ExternalTrigConvEdge));
  
  /* Process locked */
  __HAL_LOCK(hadc);
  
  /* Enable ADC overrun interrupt */
  __HAL_ADC_ENABLE_IT(hadc, ADC_IT_OVR);
  
  /* Enable ADC DMA mode */
  hadc->Instance->CR2 |= ADC_CR2_DMA;
  
  /* Set the DMA transfer complete callback */
  hadc->DMA_Handle->XferCpltCallback = ADC_DMAConvCplt;
  
  /* Set the DMA half transfer complete callback */
  hadc->DMA_Handle->XferHalfCpltCallback = ADC_DMAHalfConvCplt;
     
  /* Set the DMA error callback */
  hadc->DMA_Handle->XferErrorCallback = ADC_DMAError ;
  
  /* Enable the DMA Stream */
  HAL_DMA_Start_IT(hadc->DMA_Handle, (uint32_t)&hadc->Instance->DR, (uint32_t)pData, Length);
  
  /* Change ADC state */
  hadc->State = HAL_ADC_STATE_BUSY_REG;
   
  /* Check if ADC peripheral is disabled in order to enable it and wait during 
     Tstab time the ADC's stabilization */
  if((hadc->Instance->CR2 & ADC_CR2_ADON) != ADC_CR2_ADON)
  {  
    /* Enable the Peripheral */
    __HAL_ADC_ENABLE(hadc);
    
    /* Delay inserted to wait during Tstab time the ADC's stabilazation */
    for(; i <= 540; i++)
    {
      __NOP();
    }
  }
  
  /* if no external trigger present enable software conversion of regular channels */
  if (hadc->Init.ExternalTrigConvEdge == ADC_EXTERNALTRIGCONVEDGE_NONE)
  {
    /* Enable the selected ADC software conversion for regular group */
    hadc->Instance->CR2 |= ADC_CR2_SWSTART;
  }
  
  /* Process unlocked */
  __HAL_UNLOCK(hadc);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Disables ADC DMA (Single-ADC mode) and disables ADC peripheral    
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADC_Stop_DMA(ADC_HandleTypeDef* hadc)
{
  /* Disable the Periphral */
  __HAL_ADC_DISABLE(hadc);
  
  /* Disable ADC overrun interrupt */
  __HAL_ADC_DISABLE_IT(hadc, ADC_IT_OVR);
  
  /* Disable the selected ADC DMA mode */
  hadc->Instance->CR2 &= ~ADC_CR2_DMA;
  
  /* Disable the ADC DMA Stream */
  HAL_DMA_Abort(hadc->DMA_Handle);
  
  /* Change ADC state */
  hadc->State = HAL_ADC_STATE_READY;
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Gets the converted value from data register of regular channel.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval Converted value
  */
uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef* hadc)
{       
  /* Return the selected ADC converted value */ 
  return hadc->Instance->DR;
}

/**
  * @brief  Regular conversion complete callback in non blocking mode 
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval None
  */
__weak void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_ADC_ConvCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Regular conversion half DMA transfer callback in non blocking mode 
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval None
  */
__weak void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_ADC_ConvHalfCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Analog watchdog callback in non blocking mode 
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval None
  */
__weak void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_ADC_LevelOoutOfWindowCallback could be implemented in the user file
   */
}

/**
  * @brief  Error ADC callback.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval None
  */
__weak void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_ADC_ErrorCallback could be implemented in the user file
   */
}

/**
  * @}
  */
  
/** @defgroup ADC_Group3 Peripheral Control functions
 *  @brief   	Peripheral Control functions 
 *
@verbatim   
 ===============================================================================
             ##### Peripheral Control functions #####
 ===============================================================================  
    [..]  This section provides functions allowing to:
      (+) Configure regular channels. 
      (+) Configure injected channels.
      (+) Configure multimode.
      (+) Configure the analog watch dog.
      
@endverbatim
  * @{
  */

  /**
  * @brief  Configures for the selected ADC regular channel its corresponding
  *         rank in the sequencer and its sample time.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @param  sConfig: ADC configuration structure. 
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef* hadc, ADC_ChannelConfTypeDef* sConfig)
{
  /* Check the parameters */
  assert_param(IS_ADC_CHANNEL(sConfig->Channel));
  assert_param(IS_ADC_REGULAR_RANK(sConfig->Rank));
  assert_param(IS_ADC_SAMPLE_TIME(sConfig->SamplingTime));
  
  /* Process locked */
  __HAL_LOCK(hadc);
    
  /* if ADC_Channel_10 ... ADC_Channel_18 is selected */
  if (sConfig->Channel > ADC_CHANNEL_9)
  {
    /* Clear the old sample time */
    hadc->Instance->SMPR1 &= ~__HAL_ADC_SMPR1(ADC_SMPR1_SMP10, sConfig->Channel);
    
    /* Set the new sample time */
    hadc->Instance->SMPR1 |= __HAL_ADC_SMPR1(sConfig->SamplingTime, sConfig->Channel);
  }
  else /* ADC_Channel include in ADC_Channel_[0..9] */
  {
    /* Clear the old sample time */
    hadc->Instance->SMPR2 &= ~__HAL_ADC_SMPR2(ADC_SMPR2_SMP0, sConfig->Channel);
    
    /* Set the new sample time */
    hadc->Instance->SMPR2 |= __HAL_ADC_SMPR2(sConfig->SamplingTime, sConfig->Channel);
  }
  
  /* For Rank 1 to 6 */
  if (sConfig->Rank < 7)
  {
    /* Clear the old SQx bits for the selected rank */
    hadc->Instance->SQR3 &= ~__HAL_ADC_SQR3_RK(ADC_SQR3_SQ1, sConfig->Rank);
    
    /* Set the SQx bits for the selected rank */
    hadc->Instance->SQR3 |= __HAL_ADC_SQR3_RK(sConfig->Channel, sConfig->Rank);
  }
  /* For Rank 7 to 12 */
  else if (sConfig->Rank < 13)
  {
    /* Clear the old SQx bits for the selected rank */
    hadc->Instance->SQR2 &= ~__HAL_ADC_SQR2_RK(ADC_SQR2_SQ7, sConfig->Rank);
    
    /* Set the SQx bits for the selected rank */
    hadc->Instance->SQR2 |= __HAL_ADC_SQR2_RK(sConfig->Channel, sConfig->Rank);
  }
  /* For Rank 13 to 16 */
  else
  {
    /* Clear the old SQx bits for the selected rank */
    hadc->Instance->SQR1 &= ~__HAL_ADC_SQR1_RK(ADC_SQR1_SQ13, sConfig->Rank);
    
    /* Set the SQx bits for the selected rank */
    hadc->Instance->SQR1 |= __HAL_ADC_SQR1_RK(sConfig->Channel, sConfig->Rank);
  }
  
  /* if ADC1 Channel_18 is selected enable VBAT Channel */
  if ((hadc->Instance == ADC1) && (sConfig->Channel == ADC_CHANNEL_VBAT))
  {
    /* Enable the VBAT channel*/
    ADC->CCR |= ADC_CCR_VBATE;
  }
  
  /* if ADC1 Channel_16 or Channel_17 is selected enable TSVREFE Channel(Temperature sensor and VREFINT) */
  if ((hadc->Instance == ADC1) && ((sConfig->Channel == ADC_CHANNEL_TEMPSENSOR) || (sConfig->Channel == ADC_CHANNEL_VREFINT)))
  {
    /* Enable the TSVREFE channel*/
    ADC->CCR |= ADC_CCR_TSVREFE;
  }
  
  /* Process unlocked */
  __HAL_UNLOCK(hadc);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Configures the analog watchdog.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @param  AnalogWDGConfig : pointer to an ADC_AnalogWDGConfTypeDef structure 
  *         that contains the configuration information of ADC analog watchdog.
  * @retval HAL status	  
  */
HAL_StatusTypeDef HAL_ADC_AnalogWDGConfig(ADC_HandleTypeDef* hadc, ADC_AnalogWDGConfTypeDef* AnalogWDGConfig)
{
#ifdef USE_FULL_ASSERT  
  uint32_t tmp = 0;
#endif /* USE_FULL_ASSERT  */  
  
  /* Check the parameters */
  assert_param(IS_ADC_ANALOG_WATCHDOG(AnalogWDGConfig->WatchdogMode));
  assert_param(IS_ADC_CHANNEL(AnalogWDGConfig->Channel));
  assert_param(IS_FUNCTIONAL_STATE(AnalogWDGConfig->ITMode));

#ifdef USE_FULL_ASSERT  
  tmp = __HAL_ADC_GET_RESOLUTION(hadc);
  assert_param(IS_ADC_RANGE(tmp, AnalogWDGConfig->HighThreshold));
  assert_param(IS_ADC_RANGE(tmp, AnalogWDGConfig->LowThreshold));
#endif /* USE_FULL_ASSERT  */
  
  /* Process locked */
  __HAL_LOCK(hadc);
  
  if(AnalogWDGConfig->ITMode == ENABLE)
  {
    /* Enable the ADC Analog watchdog interrupt */
    __HAL_ADC_ENABLE_IT(hadc, ADC_IT_AWD);
  }
  else
  {
    /* Disable the ADC Analog watchdog interrupt */
    __HAL_ADC_DISABLE_IT(hadc, ADC_IT_AWD);
  }
  
  /* Clear AWDEN, JAWDEN and AWDSGL bits */
  hadc->Instance->CR1 &=  ~(ADC_CR1_AWDSGL | ADC_CR1_JAWDEN | ADC_CR1_AWDEN);
  
  /* Set the analog watchdog enable mode */
  hadc->Instance->CR1 |= AnalogWDGConfig->WatchdogMode;
  
  /* Set the high threshold */
  hadc->Instance->HTR = AnalogWDGConfig->HighThreshold;
  
  /* Set the low threshold */
  hadc->Instance->LTR = AnalogWDGConfig->LowThreshold;
  
  /* Clear the Analog watchdog channel select bits */
  hadc->Instance->CR1 &= ~ADC_CR1_AWDCH;
  
  /* Set the Analog watchdog channel */
  hadc->Instance->CR1 |= AnalogWDGConfig->Channel;
  
  /* Process unlocked */
  __HAL_UNLOCK(hadc);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup ADC_Group4 ADC Peripheral State functions
 *  @brief   ADC Peripheral State functions 
 *
@verbatim   
 ===============================================================================
            ##### Peripheral State and errors functions #####
 ===============================================================================  
    [..]
    This subsection provides functions allowing to
      (+) Check the ADC state
      (+) Check the ADC Error
         
@endverbatim
  * @{
  */
  
/**
  * @brief  return the ADC state
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval HAL state
  */
HAL_ADC_StateTypeDef HAL_ADC_GetState(ADC_HandleTypeDef* hadc)
{
  /* Return ADC state */
  return hadc->State;
}

/**
  * @brief  Return the ADC error code
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval ADC Error Code
  */
uint32_t HAL_ADC_GetError(ADC_HandleTypeDef *hadc)
{
  return hadc->ErrorCode;
}

/**
  * @}
  */

/**
  * @brief  Initializes the ADCx peripheral according to the specified parameters 
  *         in the ADC_InitStruct without initializing the ADC MSP.       
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.  
  * @retval None
  */
static void ADC_Init(ADC_HandleTypeDef* hadc)
{
  
  /* Set ADC parameters */
  /* Set the ADC clock prescaler */
  ADC->CCR &= ~(ADC_CCR_ADCPRE);
  ADC->CCR |=  hadc->Init.ClockPrescaler;
  
  /* Set ADC scan mode */
  hadc->Instance->CR1 &= ~(ADC_CR1_SCAN);
  hadc->Instance->CR1 |=  __HAL_ADC_CR1_SCANCONV(hadc->Init.ScanConvMode);
  
  /* Set ADC resolution */
  hadc->Instance->CR1 &= ~(ADC_CR1_RES);
  hadc->Instance->CR1 |=  hadc->Init.Resolution;
  
  /* Set ADC data alignment */
  hadc->Instance->CR2 &= ~(ADC_CR2_ALIGN);
  hadc->Instance->CR2 |= hadc->Init.DataAlign;
  
  /* Select external trigger to start conversion */
  hadc->Instance->CR2 &= ~(ADC_CR2_EXTSEL);
  hadc->Instance->CR2 |= hadc->Init.ExternalTrigConv;

  /* Select external trigger polarity */
  hadc->Instance->CR2 &= ~(ADC_CR2_EXTEN);
  hadc->Instance->CR2 |= hadc->Init.ExternalTrigConvEdge;
  
  /* Enable or disable ADC continuous conversion mode */
  hadc->Instance->CR2 &= ~(ADC_CR2_CONT);
  hadc->Instance->CR2 |= __HAL_ADC_CR2_CONTINUOUS(hadc->Init.ContinuousConvMode);
  
  if (hadc->Init.DiscontinuousConvMode != DISABLE)
  {
    assert_param(IS_ADC_REGULAR_DISC_NUMBER(hadc->Init.NbrOfDiscConversion));
  
    /* Enable the selected ADC regular discontinuous mode */
    hadc->Instance->CR1 |= (uint32_t)ADC_CR1_DISCEN;
    
    /* Set the number of channels to be converted in discontinuous mode */
    hadc->Instance->CR1 &= ~(ADC_CR1_DISCNUM);
    hadc->Instance->CR1 |=  __HAL_ADC_CR1_DISCONTINUOUS(hadc->Init.NbrOfDiscConversion);
  }
  else
  {
    /* Disable the selected ADC regular discontinuous mode */
    hadc->Instance->CR1 &= ~(ADC_CR1_DISCEN);
  }
  
  /* Set ADC number of conversion */
  hadc->Instance->SQR1 &= ~(ADC_SQR1_L);
  hadc->Instance->SQR1 |=  __HAL_ADC_SQR1(hadc->Init.NbrOfConversion);
  
  /* Enable or disable ADC DMA continuous request */
  hadc->Instance->CR2 &= ~(ADC_CR2_DDS);
  hadc->Instance->CR2 |= __HAL_ADC_CR2_DMAContReq(hadc->Init.DMAContinuousRequests);
  
  /* Enable or disable ADC end of conversion selection */
  hadc->Instance->CR2 &= ~(ADC_CR2_EOCS);
  hadc->Instance->CR2 |= __HAL_ADC_CR2_EOCSelection(hadc->Init.EOCSelection);
}

/**
  * @brief  DMA transfer complete callback. 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void ADC_DMAConvCplt(DMA_HandleTypeDef *hdma)   
{
  ADC_HandleTypeDef* hadc = ( ADC_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
    
  /* Check if an injected conversion is ready */
  if(hadc->State == HAL_ADC_STATE_EOC_INJ)
  {
    /* Change ADC state */
    hadc->State = HAL_ADC_STATE_EOC_INJ_REG;  
  }
  else
  {
    /* Change ADC state */
    hadc->State = HAL_ADC_STATE_EOC_REG;
  }
    
    HAL_ADC_ConvCpltCallback(hadc); 
}

/**
  * @brief  DMA half transfer complete callback. 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void ADC_DMAHalfConvCplt(DMA_HandleTypeDef *hdma)   
{
    ADC_HandleTypeDef* hadc = ( ADC_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
    /* Conversion complete callback */
    HAL_ADC_ConvHalfCpltCallback(hadc); 
}

/**
  * @brief  DMA error callback 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void ADC_DMAError(DMA_HandleTypeDef *hdma)   
{
    ADC_HandleTypeDef* hadc = ( ADC_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
    hadc->State= HAL_ADC_STATE_ERROR;
    /* Set ADC error code to DMA error */
    hadc->ErrorCode |= HAL_ADC_ERROR_DMA;
    HAL_ADC_ErrorCallback(hadc); 
}


/**
  * @}
  */

#endif /* HAL_ADC_MODULE_ENABLED */
/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
