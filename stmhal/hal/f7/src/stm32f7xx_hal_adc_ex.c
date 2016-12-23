/**
  ******************************************************************************
  * @file    stm32f7xx_hal_adc_ex.c
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    23-September-2016
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the ADC extension peripheral:
  *           + Extended features functions
  *
  @verbatim
  ==============================================================================
                    ##### How to use this driver #####
  ==============================================================================
    [..]
    (#)Initialize the ADC low level resources by implementing the HAL_ADC_MspInit():
       (##) Enable the ADC interface clock using __HAL_RCC_ADC_CLK_ENABLE()
       (##) ADC pins configuration
             (+++) Enable the clock for the ADC GPIOs using the following function:
                   __HAL_RCC_GPIOx_CLK_ENABLE()
             (+++) Configure these ADC pins in analog mode using HAL_GPIO_Init()
       (##) In case of using interrupts (e.g. HAL_ADC_Start_IT())
             (+++) Configure the ADC interrupt priority using HAL_NVIC_SetPriority()
             (+++) Enable the ADC IRQ handler using HAL_NVIC_EnableIRQ()
             (+++) In ADC IRQ handler, call HAL_ADC_IRQHandler()
      (##) In case of using DMA to control data transfer (e.g. HAL_ADC_Start_DMA())
             (+++) Enable the DMAx interface clock using __HAL_RCC_DMAx_CLK_ENABLE()
             (+++) Configure and enable two DMA streams stream for managing data
                 transfer from peripheral to memory (output stream)
             (+++) Associate the initialized DMA handle to the ADC DMA handle
                 using  __HAL_LINKDMA()
             (+++) Configure the priority and enable the NVIC for the transfer complete
                 interrupt on the two DMA Streams. The output stream should have higher
                 priority than the input stream.
     (#) Configure the ADC Prescaler, conversion resolution and data alignment
         using the HAL_ADC_Init() function.

     (#) Configure the ADC Injected channels group features, use HAL_ADC_Init()
         and HAL_ADC_ConfigChannel() functions.

     (#) Three operation modes are available within this driver :

     *** Polling mode IO operation ***
     =================================
     [..]
       (+) Start the ADC peripheral using HAL_ADCEx_InjectedStart()
       (+) Wait for end of conversion using HAL_ADC_PollForConversion(), at this stage
           user can specify the value of timeout according to his end application
       (+) To read the ADC converted values, use the HAL_ADCEx_InjectedGetValue() function.
       (+) Stop the ADC peripheral using HAL_ADCEx_InjectedStop()

     *** Interrupt mode IO operation ***
     ===================================
     [..]
       (+) Start the ADC peripheral using HAL_ADCEx_InjectedStart_IT()
       (+) Use HAL_ADC_IRQHandler() called under ADC_IRQHandler() Interrupt subroutine
       (+) At ADC end of conversion HAL_ADCEx_InjectedConvCpltCallback() function is executed and user can
            add his own code by customization of function pointer HAL_ADCEx_InjectedConvCpltCallback
       (+) In case of ADC Error, HAL_ADCEx_InjectedErrorCallback() function is executed and user can
            add his own code by customization of function pointer HAL_ADCEx_InjectedErrorCallback
       (+) Stop the ADC peripheral using HAL_ADCEx_InjectedStop_IT()


     *** DMA mode IO operation ***
     ==============================
     [..]
       (+) Start the ADC peripheral using HAL_ADCEx_InjectedStart_DMA(), at this stage the user specify the length
           of data to be transferred at each end of conversion
       (+) At The end of data transfer ba HAL_ADCEx_InjectedConvCpltCallback() function is executed and user can
            add his own code by customization of function pointer HAL_ADCEx_InjectedConvCpltCallback
       (+) In case of transfer Error, HAL_ADCEx_InjectedErrorCallback() function is executed and user can
            add his own code by customization of function pointer HAL_ADCEx_InjectedErrorCallback
        (+) Stop the ADC peripheral using HAL_ADCEx_InjectedStop_DMA()

     *** Multi mode ADCs Regular channels configuration ***
     ======================================================
     [..]
       (+) Select the Multi mode ADC regular channels features (dual or triple mode)
          and configure the DMA mode using HAL_ADCEx_MultiModeConfigChannel() functions.
       (+) Start the ADC peripheral using HAL_ADCEx_MultiModeStart_DMA(), at this stage the user specify the length
           of data to be transferred at each end of conversion
       (+) Read the ADCs converted values using the HAL_ADCEx_MultiModeGetValue() function.


    @endverbatim
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @defgroup ADCEx ADCEx
  * @brief ADC Extended driver modules
  * @{
  */

#ifdef HAL_ADC_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @addtogroup ADCEx_Private_Functions
  * @{
  */
/* Private function prototypes -----------------------------------------------*/
static void ADC_MultiModeDMAConvCplt(DMA_HandleTypeDef *hdma);
static void ADC_MultiModeDMAError(DMA_HandleTypeDef *hdma);
static void ADC_MultiModeDMAHalfConvCplt(DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup ADCEx_Exported_Functions ADC Exported Functions
  * @{
  */

/** @defgroup ADCEx_Exported_Functions_Group1  Extended features functions
  *  @brief    Extended features functions
  *
@verbatim
 ===============================================================================
                 ##### Extended features functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Start conversion of injected channel.
      (+) Stop conversion of injected channel.
      (+) Start multimode and enable DMA transfer.
      (+) Stop multimode and disable DMA transfer.
      (+) Get result of injected channel conversion.
      (+) Get result of multimode conversion.
      (+) Configure injected channels.
      (+) Configure multimode.

@endverbatim
  * @{
  */

/**
  * @brief  Enables the selected ADC software start conversion of the injected channels.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedStart(ADC_HandleTypeDef* hadc)
{
  __IO uint32_t counter = 0;
  uint32_t tmp1 = 0, tmp2 = 0;

  /* Process locked */
  __HAL_LOCK(hadc);

  /* Enable the ADC peripheral */

  /* Check if ADC peripheral is disabled in order to enable it and wait during
     Tstab time the ADC's stabilization */
  if((hadc->Instance->CR2 & ADC_CR2_ADON) != ADC_CR2_ADON)
  {
    /* Enable the Peripheral */
    __HAL_ADC_ENABLE(hadc);

    /* Delay for ADC stabilization time */
    /* Compute number of CPU cycles to wait for */
    counter = (ADC_STAB_DELAY_US * (SystemCoreClock / 1000000));
    while(counter != 0)
    {
      counter--;
    }
  }

  /* Start conversion if ADC is effectively enabled */
  if(HAL_IS_BIT_SET(hadc->Instance->CR2, ADC_CR2_ADON))
  {
    /* Set ADC state                                                          */
    /* - Clear state bitfield related to injected group conversion results    */
    /* - Set state bitfield related to injected operation                     */
    ADC_STATE_CLR_SET(hadc->State,
                      HAL_ADC_STATE_READY | HAL_ADC_STATE_INJ_EOC,
                      HAL_ADC_STATE_INJ_BUSY);

    /* Check if a regular conversion is ongoing */
    /* Note: On this device, there is no ADC error code fields related to     */
    /*       conversions on group injected only. In case of conversion on     */
    /*       going on group regular, no error code is reset.                  */
    if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_REG_BUSY))
    {
      /* Reset ADC all error code fields */
      ADC_CLEAR_ERRORCODE(hadc);
    }

    /* Process unlocked */
    /* Unlock before starting ADC conversions: in case of potential           */
    /* interruption, to let the process to ADC IRQ Handler.                   */
    __HAL_UNLOCK(hadc);

    /* Clear injected group conversion flag */
    /* (To ensure of no unknown state from potential previous ADC operations) */
    __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_JEOC);

    /* Check if Multimode enabled */
    if(HAL_IS_BIT_CLR(ADC->CCR, ADC_CCR_MULTI))
    {
      tmp1 = HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_JEXTEN);
      tmp2 = HAL_IS_BIT_CLR(hadc->Instance->CR1, ADC_CR1_JAUTO);
      if(tmp1 && tmp2)
      {
        /* Enable the selected ADC software conversion for injected group */
        hadc->Instance->CR2 |= ADC_CR2_JSWSTART;
      }
    }
    else
    {
      tmp1 = HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_JEXTEN);
      tmp2 = HAL_IS_BIT_CLR(hadc->Instance->CR1, ADC_CR1_JAUTO);
      if((hadc->Instance == ADC1) && tmp1 && tmp2)
      {
        /* Enable the selected ADC software conversion for injected group */
        hadc->Instance->CR2 |= ADC_CR2_JSWSTART;
      }
    }
  }

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Enables the interrupt and starts ADC conversion of injected channels.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  *
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedStart_IT(ADC_HandleTypeDef* hadc)
{
  __IO uint32_t counter = 0;
  uint32_t tmp1 = 0, tmp2 = 0;

  /* Process locked */
  __HAL_LOCK(hadc);

  /* Enable the ADC peripheral */

  /* Check if ADC peripheral is disabled in order to enable it and wait during
     Tstab time the ADC's stabilization */
  if((hadc->Instance->CR2 & ADC_CR2_ADON) != ADC_CR2_ADON)
  {
    /* Enable the Peripheral */
    __HAL_ADC_ENABLE(hadc);

    /* Delay for ADC stabilization time */
    /* Compute number of CPU cycles to wait for */
    counter = (ADC_STAB_DELAY_US * (SystemCoreClock / 1000000));
    while(counter != 0)
    {
      counter--;
    }
  }

  /* Start conversion if ADC is effectively enabled */
  if(HAL_IS_BIT_SET(hadc->Instance->CR2, ADC_CR2_ADON))
  {
    /* Set ADC state                                                          */
    /* - Clear state bitfield related to injected group conversion results    */
    /* - Set state bitfield related to injected operation                     */
    ADC_STATE_CLR_SET(hadc->State,
                      HAL_ADC_STATE_READY | HAL_ADC_STATE_INJ_EOC,
                      HAL_ADC_STATE_INJ_BUSY);

    /* Check if a regular conversion is ongoing */
    /* Note: On this device, there is no ADC error code fields related to     */
    /*       conversions on group injected only. In case of conversion on     */
    /*       going on group regular, no error code is reset.                  */
    if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_REG_BUSY))
    {
      /* Reset ADC all error code fields */
      ADC_CLEAR_ERRORCODE(hadc);
    }

    /* Process unlocked */
    /* Unlock before starting ADC conversions: in case of potential           */
    /* interruption, to let the process to ADC IRQ Handler.                   */
    __HAL_UNLOCK(hadc);

    /* Clear injected group conversion flag */
    /* (To ensure of no unknown state from potential previous ADC operations) */
    __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_JEOC);

    /* Enable end of conversion interrupt for injected channels */
    __HAL_ADC_ENABLE_IT(hadc, ADC_IT_JEOC);

    /* Check if Multimode enabled */
    if(HAL_IS_BIT_CLR(ADC->CCR, ADC_CCR_MULTI))
    {
      tmp1 = HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_JEXTEN);
      tmp2 = HAL_IS_BIT_CLR(hadc->Instance->CR1, ADC_CR1_JAUTO);
      if(tmp1 && tmp2)
      {
        /* Enable the selected ADC software conversion for injected group */
        hadc->Instance->CR2 |= ADC_CR2_JSWSTART;
      }
    }
    else
    {
      tmp1 = HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_JEXTEN);
      tmp2 = HAL_IS_BIT_CLR(hadc->Instance->CR1, ADC_CR1_JAUTO);
      if((hadc->Instance == ADC1) && tmp1 && tmp2)
      {
        /* Enable the selected ADC software conversion for injected group */
        hadc->Instance->CR2 |= ADC_CR2_JSWSTART;
      }
    }
  }

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stop conversion of injected channels. Disable ADC peripheral if
  *         no regular conversion is on going.
  * @note   If ADC must be disabled and if conversion is on going on
  *         regular group, function HAL_ADC_Stop must be used to stop both
  *         injected and regular groups, and disable the ADC.
  * @note   If injected group mode auto-injection is enabled,
  *         function HAL_ADC_Stop must be used.
  * @note   In case of auto-injection mode, HAL_ADC_Stop must be used.
  * @param  hadc: ADC handle
  * @retval None
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedStop(ADC_HandleTypeDef* hadc)
{
  HAL_StatusTypeDef tmp_hal_status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));

  /* Process locked */
  __HAL_LOCK(hadc);

  /* Stop potential conversion and disable ADC peripheral                     */
  /* Conditioned to:                                                          */
  /* - No conversion on the other group (regular group) is intended to        */
  /*   continue (injected and regular groups stop conversion and ADC disable  */
  /*   are common)                                                            */
  /* - In case of auto-injection mode, HAL_ADC_Stop must be used.             */
  if(((hadc->State & HAL_ADC_STATE_REG_BUSY) == RESET)  &&
     HAL_IS_BIT_CLR(hadc->Instance->CR1, ADC_CR1_JAUTO)   )
  {
    /* Stop potential conversion on going, on regular and injected groups */
    /* Disable ADC peripheral */
    __HAL_ADC_DISABLE(hadc);

    /* Check if ADC is effectively disabled */
    if(HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_ADON))
    {
      /* Set ADC state */
      ADC_STATE_CLR_SET(hadc->State,
                        HAL_ADC_STATE_REG_BUSY | HAL_ADC_STATE_INJ_BUSY,
                        HAL_ADC_STATE_READY);
    }
  }
  else
  {
    /* Update ADC state machine to error */
    SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);

    tmp_hal_status = HAL_ERROR;
  }

  /* Process unlocked */
  __HAL_UNLOCK(hadc);

  /* Return function status */
  return tmp_hal_status;
}

/**
  * @brief  Poll for injected conversion complete
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @param  Timeout: Timeout value in millisecond.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedPollForConversion(ADC_HandleTypeDef* hadc, uint32_t Timeout)
{
  uint32_t tickstart = 0;

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Check End of conversion flag */
  while(!(__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_JEOC)))
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

  /* Clear injected group conversion flag */
  __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_JSTRT | ADC_FLAG_JEOC);

  /* Update ADC state machine */
  SET_BIT(hadc->State, HAL_ADC_STATE_INJ_EOC);

  /* Determine whether any further conversion upcoming on group injected      */
  /* by external trigger, continuous mode or scan sequence on going.          */
  /* Note: On STM32F7, there is no independent flag of end of sequence.       */
  /*       The test of scan sequence on going is done either with scan        */
  /*       sequence disabled or with end of conversion flag set to            */
  /*       of end of sequence.                                                */
  if(ADC_IS_SOFTWARE_START_INJECTED(hadc)                    &&
     (HAL_IS_BIT_CLR(hadc->Instance->JSQR, ADC_JSQR_JL)  ||
      HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_EOCS)    ) &&
     (HAL_IS_BIT_CLR(hadc->Instance->CR1, ADC_CR1_JAUTO) &&
      (ADC_IS_SOFTWARE_START_REGULAR(hadc)       &&
      (hadc->Init.ContinuousConvMode == DISABLE)   )       )   )
  {
    /* Set ADC state */
    CLEAR_BIT(hadc->State, HAL_ADC_STATE_INJ_BUSY);

    if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_REG_BUSY))
    {
      SET_BIT(hadc->State, HAL_ADC_STATE_READY);
    }
  }

  /* Return ADC state */
  return HAL_OK;
}

/**
  * @brief  Stop conversion of injected channels, disable interruption of
  *         end-of-conversion. Disable ADC peripheral if no regular conversion
  *         is on going.
  * @note   If ADC must be disabled and if conversion is on going on
  *         regular group, function HAL_ADC_Stop must be used to stop both
  *         injected and regular groups, and disable the ADC.
  * @note   If injected group mode auto-injection is enabled,
  *         function HAL_ADC_Stop must be used.
  * @param  hadc: ADC handle
  * @retval None
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedStop_IT(ADC_HandleTypeDef* hadc)
{
  HAL_StatusTypeDef tmp_hal_status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));

  /* Process locked */
  __HAL_LOCK(hadc);

  /* Stop potential conversion and disable ADC peripheral                     */
  /* Conditioned to:                                                          */
  /* - No conversion on the other group (regular group) is intended to        */
  /*   continue (injected and regular groups stop conversion and ADC disable  */
  /*   are common)                                                            */
  /* - In case of auto-injection mode, HAL_ADC_Stop must be used.             */
  if(((hadc->State & HAL_ADC_STATE_REG_BUSY) == RESET)  &&
     HAL_IS_BIT_CLR(hadc->Instance->CR1, ADC_CR1_JAUTO)   )
  {
    /* Stop potential conversion on going, on regular and injected groups */
    /* Disable ADC peripheral */
    __HAL_ADC_DISABLE(hadc);

    /* Check if ADC is effectively disabled */
    if(HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_ADON))
    {
      /* Disable ADC end of conversion interrupt for injected channels */
      __HAL_ADC_DISABLE_IT(hadc, ADC_IT_JEOC);

      /* Set ADC state */
      ADC_STATE_CLR_SET(hadc->State,
                        HAL_ADC_STATE_REG_BUSY | HAL_ADC_STATE_INJ_BUSY,
                        HAL_ADC_STATE_READY);
    }
  }
  else
  {
    /* Update ADC state machine to error */
    SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);

    tmp_hal_status = HAL_ERROR;
  }

  /* Process unlocked */
  __HAL_UNLOCK(hadc);

  /* Return function status */
  return tmp_hal_status;
}

/**
  * @brief  Gets the converted value from data register of injected channel.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @param  InjectedRank: the ADC injected rank.
  *          This parameter can be one of the following values:
  *            @arg ADC_INJECTED_RANK_1: Injected Channel1 selected
  *            @arg ADC_INJECTED_RANK_2: Injected Channel2 selected
  *            @arg ADC_INJECTED_RANK_3: Injected Channel3 selected
  *            @arg ADC_INJECTED_RANK_4: Injected Channel4 selected
  * @retval None
  */
uint32_t HAL_ADCEx_InjectedGetValue(ADC_HandleTypeDef* hadc, uint32_t InjectedRank)
{
  __IO uint32_t tmp = 0;

  /* Check the parameters */
  assert_param(IS_ADC_INJECTED_RANK(InjectedRank));

  /* Clear injected group conversion flag to have similar behaviour as        */
  /* regular group: reading data register also clears end of conversion flag. */
  __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_JEOC);

  /* Return the selected ADC converted value */
  switch(InjectedRank)
  {
    case ADC_INJECTED_RANK_4:
    {
      tmp =  hadc->Instance->JDR4;
    }
    break;
    case ADC_INJECTED_RANK_3:
    {
      tmp =  hadc->Instance->JDR3;
    }
    break;
    case ADC_INJECTED_RANK_2:
    {
      tmp =  hadc->Instance->JDR2;
    }
    break;
    case ADC_INJECTED_RANK_1:
    {
      tmp =  hadc->Instance->JDR1;
    }
    break;
    default:
    break;
  }
  return tmp;
}

/**
  * @brief  Enables ADC DMA request after last transfer (Multi-ADC mode) and enables ADC peripheral
  *
  * @note   Caution: This function must be used only with the ADC master.
  *
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @param  pData:   Pointer to buffer in which transferred from ADC peripheral to memory will be stored.
  * @param  Length:  The length of data to be transferred from ADC peripheral to memory.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_MultiModeStart_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length)
{
  __IO uint32_t counter = 0;

  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(hadc->Init.ContinuousConvMode));
  assert_param(IS_ADC_EXT_TRIG_EDGE(hadc->Init.ExternalTrigConvEdge));
  assert_param(IS_FUNCTIONAL_STATE(hadc->Init.DMAContinuousRequests));

  /* Process locked */
  __HAL_LOCK(hadc);

  /* Check if ADC peripheral is disabled in order to enable it and wait during
     Tstab time the ADC's stabilization */
  if((hadc->Instance->CR2 & ADC_CR2_ADON) != ADC_CR2_ADON)
  {
    /* Enable the Peripheral */
    __HAL_ADC_ENABLE(hadc);

    /* Delay for temperature sensor stabilization time */
    /* Compute number of CPU cycles to wait for */
    counter = (ADC_STAB_DELAY_US * (SystemCoreClock / 1000000));
    while(counter != 0)
    {
      counter--;
    }
  }

  /* Start conversion if ADC is effectively enabled */
  if(HAL_IS_BIT_SET(hadc->Instance->CR2, ADC_CR2_ADON))
  {
    /* Set ADC state                                                          */
    /* - Clear state bitfield related to regular group conversion results     */
    /* - Set state bitfield related to regular group operation                */
    ADC_STATE_CLR_SET(hadc->State,
                      HAL_ADC_STATE_READY | HAL_ADC_STATE_REG_EOC | HAL_ADC_STATE_REG_OVR,
                      HAL_ADC_STATE_REG_BUSY);

    /* If conversions on group regular are also triggering group injected,    */
    /* update ADC state.                                                      */
    if (READ_BIT(hadc->Instance->CR1, ADC_CR1_JAUTO) != RESET)
    {
      ADC_STATE_CLR_SET(hadc->State, HAL_ADC_STATE_INJ_EOC, HAL_ADC_STATE_INJ_BUSY);
    }

    /* State machine update: Check if an injected conversion is ongoing */
    if (HAL_IS_BIT_SET(hadc->State, HAL_ADC_STATE_INJ_BUSY))
    {
      /* Reset ADC error code fields related to conversions on group regular */
      CLEAR_BIT(hadc->ErrorCode, (HAL_ADC_ERROR_OVR | HAL_ADC_ERROR_DMA));
    }
    else
    {
      /* Reset ADC all error code fields */
      ADC_CLEAR_ERRORCODE(hadc);
    }

    /* Process unlocked */
    /* Unlock before starting ADC conversions: in case of potential           */
    /* interruption, to let the process to ADC IRQ Handler.                   */
    __HAL_UNLOCK(hadc);

    /* Set the DMA transfer complete callback */
    hadc->DMA_Handle->XferCpltCallback = ADC_MultiModeDMAConvCplt;

    /* Set the DMA half transfer complete callback */
    hadc->DMA_Handle->XferHalfCpltCallback = ADC_MultiModeDMAHalfConvCplt;

    /* Set the DMA error callback */
    hadc->DMA_Handle->XferErrorCallback = ADC_MultiModeDMAError ;

    /* Manage ADC and DMA start: ADC overrun interruption, DMA start, ADC     */
    /* start (in case of SW start):                                           */

    /* Clear regular group conversion flag and overrun flag */
    /* (To ensure of no unknown state from potential previous ADC operations) */
    __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_EOC);

    /* Enable ADC overrun interrupt */
    __HAL_ADC_ENABLE_IT(hadc, ADC_IT_OVR);

    if (hadc->Init.DMAContinuousRequests != DISABLE)
    {
      /* Enable the selected ADC DMA request after last transfer */
      ADC->CCR |= ADC_CCR_DDS;
    }
    else
    {
      /* Disable the selected ADC EOC rising on each regular channel conversion */
      ADC->CCR &= ~ADC_CCR_DDS;
    }

    /* Enable the DMA Stream */
    HAL_DMA_Start_IT(hadc->DMA_Handle, (uint32_t)&ADC->CDR, (uint32_t)pData, Length);

    /* if no external trigger present enable software conversion of regular channels */
    if((hadc->Instance->CR2 & ADC_CR2_EXTEN) == RESET)
    {
      /* Enable the selected ADC software conversion for regular group */
      hadc->Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
    }
  }

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Disables ADC DMA (multi-ADC mode) and disables ADC peripheral
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_MultiModeStop_DMA(ADC_HandleTypeDef* hadc)
{
  HAL_StatusTypeDef tmp_hal_status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));

  /* Process locked */
  __HAL_LOCK(hadc);

  /* Stop potential conversion on going, on regular and injected groups */
  /* Disable ADC peripheral */
  __HAL_ADC_DISABLE(hadc);

  /* Check if ADC is effectively disabled */
  if(HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_ADON))
  {
    /* Disable the selected ADC DMA mode for multimode */
    ADC->CCR &= ~ADC_CCR_DDS;

    /* Disable the DMA channel (in case of DMA in circular mode or stop while */
    /* DMA transfer is on going)                                              */
    tmp_hal_status = HAL_DMA_Abort(hadc->DMA_Handle);

    /* Disable ADC overrun interrupt */
    __HAL_ADC_DISABLE_IT(hadc, ADC_IT_OVR);

    /* Set ADC state */
    ADC_STATE_CLR_SET(hadc->State,
                      HAL_ADC_STATE_REG_BUSY | HAL_ADC_STATE_INJ_BUSY,
                      HAL_ADC_STATE_READY);
  }

  /* Process unlocked */
  __HAL_UNLOCK(hadc);

  /* Return function status */
  return tmp_hal_status;
}

/**
  * @brief  Returns the last ADC1, ADC2 and ADC3 regular conversions results
  *         data in the selected multi mode.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval The converted data value.
  */
uint32_t HAL_ADCEx_MultiModeGetValue(ADC_HandleTypeDef* hadc)
{
  /* Return the multi mode conversion value */
  return ADC->CDR;
}

/**
  * @brief  Injected conversion complete callback in non blocking mode
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval None
  */
__weak void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_ADC_InjectedConvCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Configures for the selected ADC injected channel its corresponding
  *         rank in the sequencer and its sample time.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @param  sConfigInjected: ADC configuration structure for injected channel.
  * @retval None
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedConfigChannel(ADC_HandleTypeDef* hadc, ADC_InjectionConfTypeDef* sConfigInjected)
{

#ifdef USE_FULL_ASSERT
  uint32_t tmp = 0;
#endif /* USE_FULL_ASSERT  */

  /* Check the parameters */
  assert_param(IS_ADC_CHANNEL(sConfigInjected->InjectedChannel));
  assert_param(IS_ADC_INJECTED_RANK(sConfigInjected->InjectedRank));
  assert_param(IS_ADC_SAMPLE_TIME(sConfigInjected->InjectedSamplingTime));
  assert_param(IS_ADC_EXT_INJEC_TRIG(sConfigInjected->ExternalTrigInjecConv));
  assert_param(IS_ADC_INJECTED_LENGTH(sConfigInjected->InjectedNbrOfConversion));
  assert_param(IS_FUNCTIONAL_STATE(sConfigInjected->AutoInjectedConv));
  assert_param(IS_FUNCTIONAL_STATE(sConfigInjected->InjectedDiscontinuousConvMode));

#ifdef USE_FULL_ASSERT
  tmp = ADC_GET_RESOLUTION(hadc);
  assert_param(IS_ADC_RANGE(tmp, sConfigInjected->InjectedOffset));
#endif /* USE_FULL_ASSERT  */

  if(sConfigInjected->ExternalTrigInjecConv != ADC_INJECTED_SOFTWARE_START)
  {
    assert_param(IS_ADC_EXT_INJEC_TRIG_EDGE(sConfigInjected->ExternalTrigInjecConvEdge));
  }

  /* Process locked */
  __HAL_LOCK(hadc);

  /* if ADC_Channel_10 ... ADC_Channel_18 is selected */
  if (sConfigInjected->InjectedChannel > ADC_CHANNEL_9)
  {
    /* Clear the old sample time */
    hadc->Instance->SMPR1 &= ~ADC_SMPR1(ADC_SMPR1_SMP10, sConfigInjected->InjectedChannel);

    /* Set the new sample time */
    hadc->Instance->SMPR1 |= ADC_SMPR1(sConfigInjected->InjectedSamplingTime, sConfigInjected->InjectedChannel);
  }
  else /* ADC_Channel include in ADC_Channel_[0..9] */
  {
    /* Clear the old sample time */
    hadc->Instance->SMPR2 &= ~ADC_SMPR2(ADC_SMPR2_SMP0, sConfigInjected->InjectedChannel);

    /* Set the new sample time */
    hadc->Instance->SMPR2 |= ADC_SMPR2(sConfigInjected->InjectedSamplingTime, sConfigInjected->InjectedChannel);
  }

  /*---------------------------- ADCx JSQR Configuration -----------------*/
  hadc->Instance->JSQR &= ~(ADC_JSQR_JL);
  hadc->Instance->JSQR |=  ADC_SQR1(sConfigInjected->InjectedNbrOfConversion);

  /* Rank configuration */

  /* Clear the old SQx bits for the selected rank */
  hadc->Instance->JSQR &= ~ADC_JSQR(ADC_JSQR_JSQ1, sConfigInjected->InjectedRank,sConfigInjected->InjectedNbrOfConversion);

  /* Set the SQx bits for the selected rank */
  hadc->Instance->JSQR |= ADC_JSQR(sConfigInjected->InjectedChannel, sConfigInjected->InjectedRank,sConfigInjected->InjectedNbrOfConversion);

  /* Enable external trigger if trigger selection is different of software  */
  /* start.                                                                 */
  /* Note: This configuration keeps the hardware feature of parameter       */
  /*       ExternalTrigConvEdge "trigger edge none" equivalent to           */
  /*       software start.                                                  */
  if(sConfigInjected->ExternalTrigInjecConv != ADC_INJECTED_SOFTWARE_START)
  {
    /* Select external trigger to start conversion */
    hadc->Instance->CR2 &= ~(ADC_CR2_JEXTSEL);
    hadc->Instance->CR2 |=  sConfigInjected->ExternalTrigInjecConv;

    /* Select external trigger polarity */
    hadc->Instance->CR2 &= ~(ADC_CR2_JEXTEN);
    hadc->Instance->CR2 |= sConfigInjected->ExternalTrigInjecConvEdge;
  }
  else
  {
    /* Reset the external trigger */
    hadc->Instance->CR2 &= ~(ADC_CR2_JEXTSEL);
    hadc->Instance->CR2 &= ~(ADC_CR2_JEXTEN);
  }

  if (sConfigInjected->AutoInjectedConv != DISABLE)
  {
    /* Enable the selected ADC automatic injected group conversion */
    hadc->Instance->CR1 |= ADC_CR1_JAUTO;
  }
  else
  {
    /* Disable the selected ADC automatic injected group conversion */
    hadc->Instance->CR1 &= ~(ADC_CR1_JAUTO);
  }

  if (sConfigInjected->InjectedDiscontinuousConvMode != DISABLE)
  {
    /* Enable the selected ADC injected discontinuous mode */
    hadc->Instance->CR1 |= ADC_CR1_JDISCEN;
  }
  else
  {
    /* Disable the selected ADC injected discontinuous mode */
    hadc->Instance->CR1 &= ~(ADC_CR1_JDISCEN);
  }

  switch(sConfigInjected->InjectedRank)
  {
    case 1:
      /* Set injected channel 1 offset */
      hadc->Instance->JOFR1 &= ~(ADC_JOFR1_JOFFSET1);
      hadc->Instance->JOFR1 |= sConfigInjected->InjectedOffset;
      break;
    case 2:
      /* Set injected channel 2 offset */
      hadc->Instance->JOFR2 &= ~(ADC_JOFR2_JOFFSET2);
      hadc->Instance->JOFR2 |= sConfigInjected->InjectedOffset;
      break;
    case 3:
      /* Set injected channel 3 offset */
      hadc->Instance->JOFR3 &= ~(ADC_JOFR3_JOFFSET3);
      hadc->Instance->JOFR3 |= sConfigInjected->InjectedOffset;
      break;
    default:
      /* Set injected channel 4 offset */
      hadc->Instance->JOFR4 &= ~(ADC_JOFR4_JOFFSET4);
      hadc->Instance->JOFR4 |= sConfigInjected->InjectedOffset;
      break;
  }

  /* if ADC1 Channel_18 is selected enable VBAT Channel */
  if ((hadc->Instance == ADC1) && (sConfigInjected->InjectedChannel == ADC_CHANNEL_VBAT))
  {
    /* Enable the VBAT channel*/
    ADC->CCR |= ADC_CCR_VBATE;
  }

  /* if ADC1 Channel_16 or Channel_17 is selected enable TSVREFE Channel(Temperature sensor and VREFINT) */
  if ((hadc->Instance == ADC1) && ((sConfigInjected->InjectedChannel == ADC_CHANNEL_TEMPSENSOR) || (sConfigInjected->InjectedChannel == ADC_CHANNEL_VREFINT)))
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
  * @brief  Configures the ADC multi-mode
  * @param  hadc      : pointer to a ADC_HandleTypeDef structure that contains
  *                     the configuration information for the specified ADC.
  * @param  multimode : pointer to an ADC_MultiModeTypeDef structure that contains
  *                     the configuration information for  multimode.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_MultiModeConfigChannel(ADC_HandleTypeDef* hadc, ADC_MultiModeTypeDef* multimode)
{
  /* Check the parameters */
  assert_param(IS_ADC_MODE(multimode->Mode));
  assert_param(IS_ADC_DMA_ACCESS_MODE(multimode->DMAAccessMode));
  assert_param(IS_ADC_SAMPLING_DELAY(multimode->TwoSamplingDelay));

  /* Process locked */
  __HAL_LOCK(hadc);

  /* Set ADC mode */
  ADC->CCR &= ~(ADC_CCR_MULTI);
  ADC->CCR |= multimode->Mode;

  /* Set the ADC DMA access mode */
  ADC->CCR &= ~(ADC_CCR_DMA);
  ADC->CCR |= multimode->DMAAccessMode;

  /* Set delay between two sampling phases */
  ADC->CCR &= ~(ADC_CCR_DELAY);
  ADC->CCR |= multimode->TwoSamplingDelay;

  /* Process unlocked */
  __HAL_UNLOCK(hadc);

  /* Return function status */
  return HAL_OK;
}

/**
  * @}
  */

  /**
  * @brief  DMA transfer complete callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void ADC_MultiModeDMAConvCplt(DMA_HandleTypeDef *hdma)
{
  /* Retrieve ADC handle corresponding to current DMA handle */
  ADC_HandleTypeDef* hadc = ( ADC_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  /* Update state machine on conversion status if not in error state */
  if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL | HAL_ADC_STATE_ERROR_DMA))
  {
    /* Update ADC state machine */
    SET_BIT(hadc->State, HAL_ADC_STATE_REG_EOC);

    /* Determine whether any further conversion upcoming on group regular   */
    /* by external trigger, continuous mode or scan sequence on going.      */
    /* Note: On STM32F7, there is no independent flag of end of sequence.   */
    /*       The test of scan sequence on going is done either with scan    */
    /*       sequence disabled or with end of conversion flag set to        */
    /*       of end of sequence.                                            */
    if(ADC_IS_SOFTWARE_START_REGULAR(hadc)                   &&
       (hadc->Init.ContinuousConvMode == DISABLE)            &&
       (HAL_IS_BIT_CLR(hadc->Instance->SQR1, ADC_SQR1_L) ||
        HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_EOCS)  )   )
    {
      /* Disable ADC end of single conversion interrupt on group regular */
      /* Note: Overrun interrupt was enabled with EOC interrupt in          */
      /* HAL_ADC_Start_IT(), but is not disabled here because can be used   */
      /* by overrun IRQ process below.                                      */
      __HAL_ADC_DISABLE_IT(hadc, ADC_IT_EOC);

      /* Set ADC state */
      CLEAR_BIT(hadc->State, HAL_ADC_STATE_REG_BUSY);

      if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_INJ_BUSY))
      {
        SET_BIT(hadc->State, HAL_ADC_STATE_READY);
      }
    }

    /* Conversion complete callback */
    HAL_ADC_ConvCpltCallback(hadc);
  }
  else
  {
    /* Call DMA error callback */
    hadc->DMA_Handle->XferErrorCallback(hdma);
  }
}

/**
  * @brief  DMA half transfer complete callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void ADC_MultiModeDMAHalfConvCplt(DMA_HandleTypeDef *hdma)
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
static void ADC_MultiModeDMAError(DMA_HandleTypeDef *hdma)
{
    ADC_HandleTypeDef* hadc = ( ADC_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
    hadc->State= HAL_ADC_STATE_ERROR_DMA;
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
