/**
  ******************************************************************************
  * @file    stm32l4xx_hal_tim_ex.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   TIM HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Timer Extended peripheral:
  *           + Time Hall Sensor Interface Initialization
  *           + Time Hall Sensor Interface Start
  *           + Time Complementary signal break and dead time configuration
  *           + Time Master and Slave synchronization configuration
  *           + Time Output Compare/PWM Channel Configuration (for channels 5 and 6)
  *           + Time OCRef clear configuration
  *           + Timer remapping capabilities configuration
  @verbatim
  ==============================================================================
                      ##### TIMER Extended features #####
  ==============================================================================
  [..]
    The Timer Extended features include:
    (#) Complementary outputs with programmable dead-time for :
        (++) Output Compare
        (++) PWM generation (Edge and Center-aligned Mode)
        (++) One-pulse mode output
    (#) Synchronization circuit to control the timer with external signals and to
        interconnect several timers together.
    (#) Break input to put the timer output signals in reset state or in a known state.
    (#) Supports incremental (quadrature) encoder and hall-sensor circuitry for
        positioning purposes

            ##### How to use this driver #####
  ==============================================================================
    [..]
     (#) Initialize the TIM low level resources by implementing the following functions
         depending on the selected feature:
           (++) Hall Sensor output : HAL_TIMEx_HallSensor_MspInit()

     (#) Initialize the TIM low level resources :
        (##) Enable the TIM interface clock using __HAL_RCC_TIMx_CLK_ENABLE();
        (##) TIM pins configuration
            (+++) Enable the clock for the TIM GPIOs using the following function:
              __HAL_RCC_GPIOx_CLK_ENABLE();
            (+++) Configure these TIM pins in Alternate function mode using HAL_GPIO_Init();

     (#) The external Clock can be configured, if needed (the default clock is the
         internal clock from the APBx), using the following function:
         HAL_TIM_ConfigClockSource, the clock configuration should be done before
         any start function.

     (#) Configure the TIM in the desired functioning mode using one of the
         initialization function of this driver:
          (++) HAL_TIMEx_HallSensor_Init() and HAL_TIMEx_ConfigCommutationEvent(): to use the
              Timer Hall Sensor Interface and the commutation event with the corresponding
              Interrupt and DMA request if needed (Note that One Timer is used to interface
             with the Hall sensor Interface and another Timer should be used to use
             the commutation event).

     (#) Activate the TIM peripheral using one of the start functions:
           (++) Complementary Output Compare : HAL_TIMEx_OCN_Start(), HAL_TIMEx_OCN_Start_DMA(), HAL_TIMEx_OC_Start_IT()
           (++) Complementary PWM generation : HAL_TIMEx_PWMN_Start(), HAL_TIMEx_PWMN_Start_DMA(), HAL_TIMEx_PWMN_Start_IT()
           (++) Complementary One-pulse mode output : HAL_TIMEx_OnePulseN_Start(), HAL_TIMEx_OnePulseN_Start_IT()
           (++) Hall Sensor output : HAL_TIMEx_HallSensor_Start(), HAL_TIMEx_HallSensor_Start_DMA(), HAL_TIMEx_HallSensor_Start_IT().


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
#include "stm32l4xx_hal.h"

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @defgroup TIMEx TIMEx
  * @brief TIM Extended HAL module driver
  * @{
  */

#ifdef HAL_TIM_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BDTR_BKF_SHIFT (16)
#define BDTR_BK2F_SHIFT (20)
#define TIMx_ETRSEL_MASK ((uint32_t)0x0001C000) 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void TIM_OC5_SetConfig(TIM_TypeDef *TIMx,
                              TIM_OC_InitTypeDef *OC_Config);

static void TIM_OC6_SetConfig(TIM_TypeDef *TIMx,
                              TIM_OC_InitTypeDef *OC_Config);

static void TIM_CCxNChannelCmd(TIM_TypeDef* TIMx, uint32_t Channel, uint32_t ChannelNState);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Timer Ouput Compare 5 configuration
  * @param  TIMx to select the TIM peripheral
  * @param  OC_Config: The ouput configuration structure
  * @retval None
  */
static void TIM_OC5_SetConfig(TIM_TypeDef *TIMx,
                              TIM_OC_InitTypeDef *OC_Config)
{
  uint32_t tmpccmrx = 0;
  uint32_t tmpccer = 0;
  uint32_t tmpcr2 = 0;

  /* Disable the output: Reset the CCxE Bit */
  TIMx->CCER &= ~TIM_CCER_CC5E;

  /* Get the TIMx CCER register value */
  tmpccer = TIMx->CCER;
  /* Get the TIMx CR2 register value */
  tmpcr2 =  TIMx->CR2;
  /* Get the TIMx CCMR1 register value */
  tmpccmrx = TIMx->CCMR3;

  /* Reset the Output Compare Mode Bits */
  tmpccmrx &= ~(TIM_CCMR3_OC5M);
  /* Select the Output Compare Mode */
  tmpccmrx |= OC_Config->OCMode;

  /* Reset the Output Polarity level */
  tmpccer &= ~TIM_CCER_CC5P;
  /* Set the Output Compare Polarity */
  tmpccer |= (OC_Config->OCPolarity << 16);

  if(IS_TIM_BREAK_INSTANCE(TIMx))
  {
    /* Reset the Output Compare IDLE State */
    tmpcr2 &= ~TIM_CR2_OIS5;
    /* Set the Output Idle state */
    tmpcr2 |= (OC_Config->OCIdleState << 8);
  }
  /* Write to TIMx CR2 */
  TIMx->CR2 = tmpcr2;

  /* Write to TIMx CCMR3 */
  TIMx->CCMR3 = tmpccmrx;

  /* Set the Capture Compare Register value */
  TIMx->CCR5 = OC_Config->Pulse;

  /* Write to TIMx CCER */
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Timer Ouput Compare 6 configuration
  * @param  TIMx to select the TIM peripheral
  * @param  OC_Config: The ouput configuration structure
  * @retval None
  */
static void TIM_OC6_SetConfig(TIM_TypeDef *TIMx,
                              TIM_OC_InitTypeDef *OC_Config)
{
  uint32_t tmpccmrx = 0;
  uint32_t tmpccer = 0;
  uint32_t tmpcr2 = 0;

  /* Disable the output: Reset the CCxE Bit */
  TIMx->CCER &= ~TIM_CCER_CC6E;

  /* Get the TIMx CCER register value */
  tmpccer = TIMx->CCER;
  /* Get the TIMx CR2 register value */
  tmpcr2 =  TIMx->CR2;
  /* Get the TIMx CCMR1 register value */
  tmpccmrx = TIMx->CCMR3;

  /* Reset the Output Compare Mode Bits */
  tmpccmrx &= ~(TIM_CCMR3_OC6M);
  /* Select the Output Compare Mode */
  tmpccmrx |= (OC_Config->OCMode << 8);

  /* Reset the Output Polarity level */
  tmpccer &= (uint32_t)~TIM_CCER_CC6P;
  /* Set the Output Compare Polarity */
  tmpccer |= (OC_Config->OCPolarity << 20);

  if(IS_TIM_BREAK_INSTANCE(TIMx))
  {
    /* Reset the Output Compare IDLE State */
    tmpcr2 &= ~TIM_CR2_OIS6;
    /* Set the Output Idle state */
    tmpcr2 |= (OC_Config->OCIdleState << 10);
  }

  /* Write to TIMx CR2 */
  TIMx->CR2 = tmpcr2;

  /* Write to TIMx CCMR3 */
  TIMx->CCMR3 = tmpccmrx;

  /* Set the Capture Compare Register value */
  TIMx->CCR6 = OC_Config->Pulse;

  /* Write to TIMx CCER */
  TIMx->CCER = tmpccer;
}

/* Exported functions --------------------------------------------------------*/
/** @defgroup TIMEx_Exported_Functions TIM Extended Exported Functions
  * @{
  */

/** @defgroup TIMEx_Exported_Functions_Group1 Extended Timer Hall Sensor functions
  * @brief    Timer Hall Sensor functions
  *
@verbatim
  ==============================================================================
                      ##### Timer Hall Sensor functions #####
  ==============================================================================
  [..]
    This section provides functions allowing to:
    (+) Initialize and configure TIM HAL Sensor.
    (+) De-initialize TIM HAL Sensor.
    (+) Start the Hall Sensor Interface.
    (+) Stop the Hall Sensor Interface.
    (+) Start the Hall Sensor Interface and enable interrupts.
    (+) Stop the Hall Sensor Interface and disable interrupts.
    (+) Start the Hall Sensor Interface and enable DMA transfers.
    (+) Stop the Hall Sensor Interface and disable DMA transfers.

@endverbatim
  * @{
  */
/**
  * @brief  Initializes the TIM Hall Sensor Interface and initialize the associated handle.
  * @param  htim: TIM Encoder Interface handle
  * @param  sConfig: TIM Hall Sensor configuration structure
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Init(TIM_HandleTypeDef *htim, TIM_HallSensor_InitTypeDef* sConfig)
{
  TIM_OC_InitTypeDef OC_Config;

  /* Check the TIM handle allocation */
  if(htim == NULL)
  {
    return HAL_ERROR;
  }

  assert_param(IS_TIM_XOR_INSTANCE(htim->Instance));
  assert_param(IS_TIM_COUNTER_MODE(htim->Init.CounterMode));
  assert_param(IS_TIM_CLOCKDIVISION_DIV(htim->Init.ClockDivision));
  assert_param(IS_TIM_IC_POLARITY(sConfig->IC1Polarity));
  assert_param(IS_TIM_IC_PRESCALER(sConfig->IC1Prescaler));
  assert_param(IS_TIM_IC_FILTER(sConfig->IC1Filter));

  if(htim->State == HAL_TIM_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    htim->Lock = HAL_UNLOCKED;

    /* Init the low level hardware : GPIO, CLOCK, NVIC and DMA */
    HAL_TIMEx_HallSensor_MspInit(htim);
  }

  /* Set the TIM state */
  htim->State = HAL_TIM_STATE_BUSY;

  /* Configure the Time base in the Encoder Mode */
  TIM_Base_SetConfig(htim->Instance, &htim->Init);

  /* Configure the Channel 1 as Input Channel to interface with the three Outputs of the  Hall sensor */
  TIM_TI1_SetConfig(htim->Instance, sConfig->IC1Polarity, TIM_ICSELECTION_TRC, sConfig->IC1Filter);

  /* Reset the IC1PSC Bits */
  htim->Instance->CCMR1 &= ~TIM_CCMR1_IC1PSC;
  /* Set the IC1PSC value */
  htim->Instance->CCMR1 |= sConfig->IC1Prescaler;

  /* Enable the Hall sensor interface (XOR function of the three inputs) */
  htim->Instance->CR2 |= TIM_CR2_TI1S;

  /* Select the TIM_TS_TI1F_ED signal as Input trigger for the TIM */
  htim->Instance->SMCR &= ~TIM_SMCR_TS;
  htim->Instance->SMCR |= TIM_TS_TI1F_ED;

  /* Use the TIM_TS_TI1F_ED signal to reset the TIM counter each edge detection */
  htim->Instance->SMCR &= ~TIM_SMCR_SMS;
  htim->Instance->SMCR |= TIM_SLAVEMODE_RESET;

  /* Program channel 2 in PWM 2 mode with the desired Commutation_Delay*/
  OC_Config.OCFastMode = TIM_OCFAST_DISABLE;
  OC_Config.OCIdleState = TIM_OCIDLESTATE_RESET;
  OC_Config.OCMode = TIM_OCMODE_PWM2;
  OC_Config.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  OC_Config.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  OC_Config.OCPolarity = TIM_OCPOLARITY_HIGH;
  OC_Config.Pulse = sConfig->Commutation_Delay;

  TIM_OC2_SetConfig(htim->Instance, &OC_Config);

  /* Select OC2REF as trigger output on TRGO: write the MMS bits in the TIMx_CR2
    register to 101 */
  htim->Instance->CR2 &= ~TIM_CR2_MMS;
  htim->Instance->CR2 |= TIM_TRGO_OC2REF;

  /* Initialize the TIM state*/
  htim->State= HAL_TIM_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  DeInitialize the TIM Hall Sensor interface
  * @param  htim: TIM Hall Sensor handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_DeInit(TIM_HandleTypeDef *htim)
{
  /* Check the parameters */
  assert_param(IS_TIM_INSTANCE(htim->Instance));

  htim->State = HAL_TIM_STATE_BUSY;

  /* Disable the TIM Peripheral Clock */
  __HAL_TIM_DISABLE(htim);

  /* DeInit the low level hardware: GPIO, CLOCK, NVIC */
  HAL_TIMEx_HallSensor_MspDeInit(htim);

  /* Change TIM state */
  htim->State = HAL_TIM_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(htim);

  return HAL_OK;
}

/**
  * @brief  Initializes the TIM Hall Sensor MSP.
  * @param  htim: TIM handle
  * @retval None
  */
__weak void HAL_TIMEx_HallSensor_MspInit(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_TIMEx_HallSensor_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitialize TIM Hall Sensor MSP.
  * @param  htim: TIM handle
  * @retval None
  */
__weak void HAL_TIMEx_HallSensor_MspDeInit(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_TIMEx_HallSensor_MspDeInit could be implemented in the user file
   */
}

/**
  * @brief  Starts the TIM Hall Sensor Interface.
  * @param  htim : TIM Hall Sensor handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start(TIM_HandleTypeDef *htim)
{
  /* Check the parameters */
  assert_param(IS_TIM_XOR_INSTANCE(htim->Instance));

  /* Enable the Input Capture channels 1
    (in the Hall Sensor Interface the Three possible channels that can be used are TIM_CHANNEL_1, TIM_CHANNEL_2 and TIM_CHANNEL_3) */
  TIM_CCxChannelCmd(htim->Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);

  /* Enable the Peripheral */
  __HAL_TIM_ENABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stops the TIM Hall sensor Interface.
  * @param  htim : TIM Hall Sensor handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop(TIM_HandleTypeDef *htim)
{
  /* Check the parameters */
  assert_param(IS_TIM_XOR_INSTANCE(htim->Instance));

  /* Disable the Input Capture channels 1, 2 and 3
    (in the Hall Sensor Interface the Three possible channels that can be used are TIM_CHANNEL_1, TIM_CHANNEL_2 and TIM_CHANNEL_3) */
  TIM_CCxChannelCmd(htim->Instance, TIM_CHANNEL_1, TIM_CCx_DISABLE);

  /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Starts the TIM Hall Sensor Interface in interrupt mode.
  * @param  htim : TIM Hall Sensor handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_IT(TIM_HandleTypeDef *htim)
{
  /* Check the parameters */
  assert_param(IS_TIM_XOR_INSTANCE(htim->Instance));

  /* Enable the capture compare Interrupts 1 event */
  __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC1);

  /* Enable the Input Capture channels 1
    (in the Hall Sensor Interface the Three possible channels that can be used are TIM_CHANNEL_1, TIM_CHANNEL_2 and TIM_CHANNEL_3) */
  TIM_CCxChannelCmd(htim->Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);

  /* Enable the Peripheral */
  __HAL_TIM_ENABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stops the TIM Hall Sensor Interface in interrupt mode.
  * @param  htim : TIM handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_IT(TIM_HandleTypeDef *htim)
{
  /* Check the parameters */
  assert_param(IS_TIM_XOR_INSTANCE(htim->Instance));

  /* Disable the Input Capture channels 1
    (in the Hall Sensor Interface the Three possible channels that can be used are TIM_CHANNEL_1, TIM_CHANNEL_2 and TIM_CHANNEL_3) */
  TIM_CCxChannelCmd(htim->Instance, TIM_CHANNEL_1, TIM_CCx_DISABLE);

  /* Disable the capture compare Interrupts event */
  __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);

  /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Starts the TIM Hall Sensor Interface in DMA mode.
  * @param  htim : TIM Hall Sensor handle
  * @param  pData: The destination Buffer address.
  * @param  Length: The length of data to be transferred from TIM peripheral to memory.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_DMA(TIM_HandleTypeDef *htim, uint32_t *pData, uint16_t Length)
{
  /* Check the parameters */
  assert_param(IS_TIM_XOR_INSTANCE(htim->Instance));

   if((htim->State == HAL_TIM_STATE_BUSY))
  {
     return HAL_BUSY;
  }
  else if((htim->State == HAL_TIM_STATE_READY))
  {
    if(((uint32_t)pData == 0 ) && (Length > 0))
    {
      return HAL_ERROR;
    }
    else
    {
      htim->State = HAL_TIM_STATE_BUSY;
    }
  }
  /* Enable the Input Capture channels 1
    (in the Hall Sensor Interface the Three possible channels that can be used are TIM_CHANNEL_1, TIM_CHANNEL_2 and TIM_CHANNEL_3) */
  TIM_CCxChannelCmd(htim->Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);

  /* Set the DMA Input Capture 1 Callback */
  htim->hdma[TIM_DMA_ID_CC1]->XferCpltCallback = TIM_DMACaptureCplt;
  /* Set the DMA error callback */
  htim->hdma[TIM_DMA_ID_CC1]->XferErrorCallback = TIM_DMAError ;

  /* Enable the DMA channel for Capture 1*/
  HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC1], (uint32_t)&htim->Instance->CCR1, (uint32_t)pData, Length);

  /* Enable the capture compare 1 Interrupt */
  __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC1);

  /* Enable the Peripheral */
  __HAL_TIM_ENABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stops the TIM Hall Sensor Interface in DMA mode.
  * @param  htim : TIM handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_DMA(TIM_HandleTypeDef *htim)
{
  /* Check the parameters */
  assert_param(IS_TIM_XOR_INSTANCE(htim->Instance));

  /* Disable the Input Capture channels 1
    (in the Hall Sensor Interface the Three possible channels that can be used are TIM_CHANNEL_1, TIM_CHANNEL_2 and TIM_CHANNEL_3) */
  TIM_CCxChannelCmd(htim->Instance, TIM_CHANNEL_1, TIM_CCx_DISABLE);


  /* Disable the capture compare Interrupts 1 event */
  __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC1);

  /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup TIMEx_Exported_Functions_Group2 Extended Timer Complementary Output Compare functions
  *  @brief   Timer Complementary Output Compare functions
  *
@verbatim
  ==============================================================================
              ##### Timer Complementary Output Compare functions #####
  ==============================================================================
  [..]
    This section provides functions allowing to:
    (+) Start the Complementary Output Compare/PWM.
    (+) Stop the Complementary Output Compare/PWM.
    (+) Start the Complementary Output Compare/PWM and enable interrupts.
    (+) Stop the Complementary Output Compare/PWM and disable interrupts.
    (+) Start the Complementary Output Compare/PWM and enable DMA transfers.
    (+) Stop the Complementary Output Compare/PWM and disable DMA transfers.

@endverbatim
  * @{
  */

/**
  * @brief  Starts the TIM Output Compare signal generation on the complementary
  *         output.
  * @param  htim : TIM Output Compare handle
  * @param  Channel : TIM Channel to be enabled
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_OCN_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  /* Enable the Capture compare channel N */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_ENABLE);

  /* Enable the Main Ouput */
  __HAL_TIM_MOE_ENABLE(htim);

  /* Enable the Peripheral */
  __HAL_TIM_ENABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stops the TIM Output Compare signal generation on the complementary
  *         output.
  * @param  htim : TIM handle
  * @param  Channel : TIM Channel to be disabled
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  /* Disable the Capture compare channel N */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_DISABLE);

  /* Disable the Main Ouput */
  __HAL_TIM_MOE_DISABLE(htim);

  /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Starts the TIM Output Compare signal generation in interrupt mode
  *         on the complementary output.
  * @param  htim : TIM OC handle
  * @param  Channel : TIM Channel to be enabled
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_OCN_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  switch (Channel)
  {
    case TIM_CHANNEL_1:
    {
      /* Enable the TIM Output Compare interrupt */
      __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC1);
    }
    break;

    case TIM_CHANNEL_2:
    {
      /* Enable the TIM Output Compare interrupt */
      __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC2);
    }
    break;

    case TIM_CHANNEL_3:
    {
      /* Enable the TIM Output Compare interrupt */
      __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC3);
    }
    break;

    case TIM_CHANNEL_4:
    {
      /* Enable the TIM Output Compare interrupt */
      __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC4);
    }
    break;

    default:
    break;
  }

  /* Enable the TIM Break interrupt */
  __HAL_TIM_ENABLE_IT(htim, TIM_IT_BREAK);
  
  /* Enable the Capture compare channel N */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_ENABLE);

  /* Enable the Main Ouput */
  __HAL_TIM_MOE_ENABLE(htim);

  /* Enable the Peripheral */
  __HAL_TIM_ENABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stops the TIM Output Compare signal generation in interrupt mode
  *         on the complementary output.
  * @param  htim : TIM Output Compare handle
  * @param  Channel : TIM Channel to be disabled
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel)
{
  uint32_t tmpccer = 0;

  /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  switch (Channel)
  {
    case TIM_CHANNEL_1:
    {
      /* Disable the TIM Output Compare interrupt */
      __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);
    }
    break;

    case TIM_CHANNEL_2:
    {
      /* Disable the TIM Output Compare interrupt */
      __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC2);
    }
    break;

    case TIM_CHANNEL_3:
    {
      /* Disable the TIM Output Compare interrupt */
      __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC3);
    }
    break;

    case TIM_CHANNEL_4:
    {
      /* Disable the TIM Output Compare interrupt */
      __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC4);
    }
    break;

    default:
    break;
  }

  /* Disable the Capture compare channel N */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_DISABLE);

  /* Disable the TIM Break interrupt (only if no more channel is active) */
  tmpccer = htim->Instance->CCER;
  if ((tmpccer & (TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE)) == RESET)
  {
    __HAL_TIM_DISABLE_IT(htim, TIM_IT_BREAK);
  }

  /* Disable the Main Ouput */
  __HAL_TIM_MOE_DISABLE(htim);

  /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Starts the TIM Output Compare signal generation in DMA mode
  *         on the complementary output.
  * @param  htim : TIM Output Compare handle
  * @param  Channel : TIM Channel to be enabled
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @param  pData: The source Buffer address.
  * @param  Length: The length of data to be transferred from memory to TIM peripheral
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_OCN_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length)
{
  /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  if((htim->State == HAL_TIM_STATE_BUSY))
  {
     return HAL_BUSY;
  }
  else if((htim->State == HAL_TIM_STATE_READY))
  {
    if(((uint32_t)pData == 0 ) && (Length > 0))
    {
      return HAL_ERROR;
    }
    else
    {
      htim->State = HAL_TIM_STATE_BUSY;
    }
  }
  switch (Channel)
  {
    case TIM_CHANNEL_1:
    {
      /* Set the DMA Period elapsed callback */
      htim->hdma[TIM_DMA_ID_CC1]->XferCpltCallback = TIM_DMADelayPulseCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC1]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA channel */
      HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC1], (uint32_t)pData, (uint32_t)&htim->Instance->CCR1, Length);

      /* Enable the TIM Output Compare DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC1);
    }
    break;

    case TIM_CHANNEL_2:
    {
      /* Set the DMA Period elapsed callback */
      htim->hdma[TIM_DMA_ID_CC2]->XferCpltCallback = TIM_DMADelayPulseCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC2]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA channel */
      HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC2], (uint32_t)pData, (uint32_t)&htim->Instance->CCR2, Length);

      /* Enable the TIM Output Compare DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC2);
    }
    break;

    case TIM_CHANNEL_3:
{
      /* Set the DMA Period elapsed callback */
      htim->hdma[TIM_DMA_ID_CC3]->XferCpltCallback = TIM_DMADelayPulseCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC3]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA channel */
      HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC3], (uint32_t)pData, (uint32_t)&htim->Instance->CCR3,Length);

      /* Enable the TIM Output Compare DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC3);
    }
    break;

    case TIM_CHANNEL_4:
    {
     /* Set the DMA Period elapsed callback */
      htim->hdma[TIM_DMA_ID_CC4]->XferCpltCallback = TIM_DMADelayPulseCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC4]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA channel */
      HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC4], (uint32_t)pData, (uint32_t)&htim->Instance->CCR4, Length);

      /* Enable the TIM Output Compare DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC4);
    }
    break;

    default:
    break;
  }

  /* Enable the Capture compare channel N */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_ENABLE);

  /* Enable the Main Ouput */
  __HAL_TIM_MOE_ENABLE(htim);

  /* Enable the Peripheral */
  __HAL_TIM_ENABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stops the TIM Output Compare signal generation in DMA mode
  *         on the complementary output.
  * @param  htim : TIM Output Compare handle
  * @param  Channel : TIM Channel to be disabled
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  switch (Channel)
  {
    case TIM_CHANNEL_1:
    {
      /* Disable the TIM Output Compare DMA request */
      __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC1);
    }
    break;

    case TIM_CHANNEL_2:
    {
      /* Disable the TIM Output Compare DMA request */
      __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC2);
    }
    break;

    case TIM_CHANNEL_3:
    {
      /* Disable the TIM Output Compare DMA request */
      __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC3);
    }
    break;

    case TIM_CHANNEL_4:
    {
      /* Disable the TIM Output Compare interrupt */
      __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC4);
    }
    break;

    default:
    break;
  }

  /* Disable the Capture compare channel N */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_DISABLE);

  /* Disable the Main Ouput */
  __HAL_TIM_MOE_DISABLE(htim);

  /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

  /* Change the htim state */
  htim->State = HAL_TIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup TIMEx_Exported_Functions_Group3 Extended Timer Complementary PWM functions
  * @brief    Timer Complementary PWM functions
  *
@verbatim
  ==============================================================================
                 ##### Timer Complementary PWM functions #####
  ==============================================================================
  [..]
    This section provides functions allowing to:
    (+) Start the Complementary PWM.
    (+) Stop the Complementary PWM.
    (+) Start the Complementary PWM and enable interrupts.
    (+) Stop the Complementary PWM and disable interrupts.
    (+) Start the Complementary PWM and enable DMA transfers.
    (+) Stop the Complementary PWM and disable DMA transfers.
    (+) Start the Complementary Input Capture measurement.
    (+) Stop the Complementary Input Capture.
    (+) Start the Complementary Input Capture and enable interrupts.
    (+) Stop the Complementary Input Capture and disable interrupts.
    (+) Start the Complementary Input Capture and enable DMA transfers.
    (+) Stop the Complementary Input Capture and disable DMA transfers.
    (+) Start the Complementary One Pulse generation.
    (+) Stop the Complementary One Pulse.
    (+) Start the Complementary One Pulse and enable interrupts.
    (+) Stop the Complementary One Pulse and disable interrupts.

@endverbatim
  * @{
  */

/**
  * @brief  Starts the PWM signal generation on the complementary output.
  * @param  htim : TIM handle
  * @param  Channel : TIM Channel to be enabled
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  /* Enable the complementary PWM output  */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_ENABLE);

  /* Enable the Main Ouput */
  __HAL_TIM_MOE_ENABLE(htim);

  /* Enable the Peripheral */
  __HAL_TIM_ENABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stops the PWM signal generation on the complementary output.
  * @param  htim : TIM handle
  * @param  Channel : TIM Channel to be disabled
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  /* Disable the complementary PWM output  */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_DISABLE);

  /* Disable the Main Ouput */
  __HAL_TIM_MOE_DISABLE(htim);

  /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Starts the PWM signal generation in interrupt mode on the
  *         complementary output.
  * @param  htim : TIM handle
  * @param  Channel : TIM Channel to be disabled
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  switch (Channel)
  {
    case TIM_CHANNEL_1:
    {
      /* Enable the TIM Capture/Compare 1 interrupt */
      __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC1);
    }
    break;

    case TIM_CHANNEL_2:
    {
      /* Enable the TIM Capture/Compare 2 interrupt */
      __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC2);
    }
    break;

    case TIM_CHANNEL_3:
    {
      /* Enable the TIM Capture/Compare 3 interrupt */
      __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC3);
    }
    break;

    case TIM_CHANNEL_4:
    {
      /* Enable the TIM Capture/Compare 4 interrupt */
      __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC4);
    }
    break;

    default:
    break;
  }

  /* Enable the TIM Break interrupt */
  __HAL_TIM_ENABLE_IT(htim, TIM_IT_BREAK);

  /* Enable the complementary PWM output  */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_ENABLE);

  /* Enable the Main Ouput */
  __HAL_TIM_MOE_ENABLE(htim);

  /* Enable the Peripheral */
  __HAL_TIM_ENABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stops the PWM signal generation in interrupt mode on the
  *         complementary output.
  * @param  htim : TIM handle
  * @param  Channel : TIM Channel to be disabled
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_IT (TIM_HandleTypeDef *htim, uint32_t Channel)
{
  uint32_t tmpccer = 0;

  /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  switch (Channel)
  {
    case TIM_CHANNEL_1:
    {
      /* Disable the TIM Capture/Compare 1 interrupt */
      __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);
    }
    break;

    case TIM_CHANNEL_2:
    {
      /* Disable the TIM Capture/Compare 2 interrupt */
      __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC2);
    }
    break;

    case TIM_CHANNEL_3:
    {
      /* Disable the TIM Capture/Compare 3 interrupt */
      __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC3);
    }
    break;

    case TIM_CHANNEL_4:
    {
      /* Disable the TIM Capture/Compare 3 interrupt */
      __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC4);
    }
    break;

    default:
    break;
  }

  /* Disable the complementary PWM output  */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_DISABLE);

  
  /* Disable the TIM Break interrupt (only if no more channel is active) */
  tmpccer = htim->Instance->CCER;
  if ((tmpccer & (TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE)) == RESET)
  {
    __HAL_TIM_DISABLE_IT(htim, TIM_IT_BREAK);
  }

  /* Disable the Main Ouput */
  __HAL_TIM_MOE_DISABLE(htim);

  /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Starts the TIM PWM signal generation in DMA mode on the
  *         complementary output
  * @param  htim : TIM handle
  * @param  Channel : TIM Channel to be enabled
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @param  pData: The source Buffer address.
  * @param  Length: The length of data to be transferred from memory to TIM peripheral
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length)
{
  /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  if((htim->State == HAL_TIM_STATE_BUSY))
  {
     return HAL_BUSY;
  }
  else if((htim->State == HAL_TIM_STATE_READY))
  {
    if(((uint32_t)pData == 0 ) && (Length > 0))
    {
      return HAL_ERROR;
    }
    else
    {
      htim->State = HAL_TIM_STATE_BUSY;
    }
  }
  switch (Channel)
  {
    case TIM_CHANNEL_1:
    {
      /* Set the DMA Period elapsed callback */
      htim->hdma[TIM_DMA_ID_CC1]->XferCpltCallback = TIM_DMADelayPulseCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC1]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA channel */
      HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC1], (uint32_t)pData, (uint32_t)&htim->Instance->CCR1, Length);

      /* Enable the TIM Capture/Compare 1 DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC1);
    }
    break;

    case TIM_CHANNEL_2:
    {
      /* Set the DMA Period elapsed callback */
      htim->hdma[TIM_DMA_ID_CC2]->XferCpltCallback = TIM_DMADelayPulseCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC2]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA channel */
      HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC2], (uint32_t)pData, (uint32_t)&htim->Instance->CCR2, Length);

      /* Enable the TIM Capture/Compare 2 DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC2);
    }
    break;

    case TIM_CHANNEL_3:
    {
      /* Set the DMA Period elapsed callback */
      htim->hdma[TIM_DMA_ID_CC3]->XferCpltCallback = TIM_DMADelayPulseCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC3]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA channel */
      HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC3], (uint32_t)pData, (uint32_t)&htim->Instance->CCR3,Length);

      /* Enable the TIM Capture/Compare 3 DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC3);
    }
    break;

    case TIM_CHANNEL_4:
    {
     /* Set the DMA Period elapsed callback */
      htim->hdma[TIM_DMA_ID_CC4]->XferCpltCallback = TIM_DMADelayPulseCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC4]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA channel */
      HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC4], (uint32_t)pData, (uint32_t)&htim->Instance->CCR4, Length);

      /* Enable the TIM Capture/Compare 4 DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC4);
    }
    break;

    default:
    break;
  }

  /* Enable the complementary PWM output  */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_ENABLE);

  /* Enable the Main Ouput */
  __HAL_TIM_MOE_ENABLE(htim);

  /* Enable the Peripheral */
  __HAL_TIM_ENABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stops the TIM PWM signal generation in DMA mode on the complementary
  *         output
  * @param  htim : TIM handle
  * @param  Channel : TIM Channel to be disabled
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  switch (Channel)
  {
    case TIM_CHANNEL_1:
    {
      /* Disable the TIM Capture/Compare 1 DMA request */
      __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC1);
    }
    break;

    case TIM_CHANNEL_2:
    {
      /* Disable the TIM Capture/Compare 2 DMA request */
      __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC2);
    }
    break;

    case TIM_CHANNEL_3:
    {
      /* Disable the TIM Capture/Compare 3 DMA request */
      __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC3);
    }
    break;

    case TIM_CHANNEL_4:
    {
      /* Disable the TIM Capture/Compare 4 DMA request */
      __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC4);
    }
    break;

    default:
    break;
  }

  /* Disable the complementary PWM output */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_DISABLE);

  /* Disable the Main Ouput */
  __HAL_TIM_MOE_DISABLE(htim);

  /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

  /* Change the htim state */
  htim->State = HAL_TIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup TIMEx_Exported_Functions_Group4 Extended Timer Complementary One Pulse functions
  * @brief    Timer Complementary One Pulse functions
  *
@verbatim
  ==============================================================================
                ##### Timer Complementary One Pulse functions #####
  ==============================================================================
  [..]
    This section provides functions allowing to:
    (+) Start the Complementary One Pulse generation.
    (+) Stop the Complementary One Pulse.
    (+) Start the Complementary One Pulse and enable interrupts.
    (+) Stop the Complementary One Pulse and disable interrupts.

@endverbatim
  * @{
  */

/**
  * @brief  Starts the TIM One Pulse signal generation on the complementary
  *         output.
  * @param  htim : TIM One Pulse handle
  * @param  OutputChannel : TIM Channel to be enabled
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start(TIM_HandleTypeDef *htim, uint32_t OutputChannel)
  {
  /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, OutputChannel));

  /* Enable the complementary One Pulse output */
  TIM_CCxNChannelCmd(htim->Instance, OutputChannel, TIM_CCxN_ENABLE);

  /* Enable the Main Ouput */
  __HAL_TIM_MOE_ENABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stops the TIM One Pulse signal generation on the complementary
  *         output.
  * @param  htim : TIM One Pulse handle
  * @param  OutputChannel : TIM Channel to be disabled
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop(TIM_HandleTypeDef *htim, uint32_t OutputChannel)
{

  /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, OutputChannel));

  /* Disable the complementary One Pulse output */
  TIM_CCxNChannelCmd(htim->Instance, OutputChannel, TIM_CCxN_DISABLE);

  /* Disable the Main Ouput */
  __HAL_TIM_MOE_DISABLE(htim);

  /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Starts the TIM One Pulse signal generation in interrupt mode on the
  *         complementary channel.
  * @param  htim : TIM One Pulse handle
  * @param  OutputChannel : TIM Channel to be enabled
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel)
{
  /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, OutputChannel));

  /* Enable the TIM Capture/Compare 1 interrupt */
  __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC1);

  /* Enable the TIM Capture/Compare 2 interrupt */
  __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC2);

  /* Enable the complementary One Pulse output */
  TIM_CCxNChannelCmd(htim->Instance, OutputChannel, TIM_CCxN_ENABLE);

  /* Enable the Main Ouput */
  __HAL_TIM_MOE_ENABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stops the TIM One Pulse signal generation in interrupt mode on the
  *         complementary channel.
  * @param  htim : TIM One Pulse handle
  * @param  OutputChannel : TIM Channel to be disabled
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel)
{
  /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, OutputChannel));

  /* Disable the TIM Capture/Compare 1 interrupt */
  __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);

  /* Disable the TIM Capture/Compare 2 interrupt */
  __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC2);

  /* Disable the complementary One Pulse output */
  TIM_CCxNChannelCmd(htim->Instance, OutputChannel, TIM_CCxN_DISABLE);

  /* Disable the Main Ouput */
  __HAL_TIM_MOE_DISABLE(htim);

  /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup TIMEx_Exported_Functions_Group5 Extended Peripheral Control functions
  * @brief    Peripheral Control functions
  *
@verbatim
  ==============================================================================
                    ##### Peripheral Control functions #####
  ==============================================================================
  [..]
    This section provides functions allowing to:
    (+) Configure the commutation event in case of use of the Hall sensor interface.
      (+) Configure Output channels for OC and PWM mode.

      (+) Configure Complementary channels, break features and dead time.
      (+) Configure Master synchronization.
      (+) Configure timer remapping capabilities.
      (+) Enable or disable channel grouping

@endverbatim
  * @{
  */

/**
  * @brief  Configure the TIM commutation event sequence.
  * @note  This function is mandatory to use the commutation event in order to
  *        update the configuration at each commutation detection on the TRGI input of the Timer,
  *        the typical use of this feature is with the use of another Timer(interface Timer)
  *        configured in Hall sensor interface, this interface Timer will generate the
  *        commutation at its TRGO output (connected to Timer used in this function) each time
  *        the TI1 of the Interface Timer detect a commutation at its input TI1.
  * @param  htim: TIM handle
  * @param  InputTrigger : the Internal trigger corresponding to the Timer Interfacing with the Hall sensor
  *          This parameter can be one of the following values:
  *            @arg TIM_TS_ITR0: Internal trigger 0 selected
  *            @arg TIM_TS_ITR1: Internal trigger 1 selected
  *            @arg TIM_TS_ITR2: Internal trigger 2 selected
  *            @arg TIM_TS_ITR3: Internal trigger 3 selected
  *            @arg TIM_TS_NONE: No trigger is needed
  * @param  CommutationSource : the Commutation Event source
  *          This parameter can be one of the following values:
  *            @arg TIM_COMMUTATION_TRGI: Commutation source is the TRGI of the Interface Timer
  *            @arg TIM_COMMUTATION_SOFTWARE:  Commutation source is set by software using the COMG bit
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutationEvent(TIM_HandleTypeDef *htim, uint32_t  InputTrigger, uint32_t  CommutationSource)
{
  /* Check the parameters */
  assert_param(IS_TIM_COMMUTATION_EVENT_INSTANCE(htim->Instance));
  assert_param(IS_TIM_INTERNAL_TRIGGEREVENT_SELECTION(InputTrigger));

  __HAL_LOCK(htim);

  if ((InputTrigger == TIM_TS_ITR0) || (InputTrigger == TIM_TS_ITR1) ||
      (InputTrigger == TIM_TS_ITR2) || (InputTrigger == TIM_TS_ITR3))
  {
    /* Select the Input trigger */
    htim->Instance->SMCR &= ~TIM_SMCR_TS;
    htim->Instance->SMCR |= InputTrigger;
  }

  /* Select the Capture Compare preload feature */
  htim->Instance->CR2 |= TIM_CR2_CCPC;
  /* Select the Commutation event source */
  htim->Instance->CR2 &= ~TIM_CR2_CCUS;
  htim->Instance->CR2 |= CommutationSource;

  __HAL_UNLOCK(htim);

  return HAL_OK;
}

/**
  * @brief  Configure the TIM commutation event sequence with interrupt.
  * @note  This function is mandatory to use the commutation event in order to
  *        update the configuration at each commutation detection on the TRGI input of the Timer,
  *        the typical use of this feature is with the use of another Timer(interface Timer)
  *        configured in Hall sensor interface, this interface Timer will generate the
  *        commutation at its TRGO output (connected to Timer used in this function) each time
  *        the TI1 of the Interface Timer detect a commutation at its input TI1.
  * @param  htim: TIM handle
  * @param  InputTrigger : the Internal trigger corresponding to the Timer Interfacing with the Hall sensor
  *          This parameter can be one of the following values:
  *            @arg TIM_TS_ITR0: Internal trigger 0 selected
  *            @arg TIM_TS_ITR1: Internal trigger 1 selected
  *            @arg TIM_TS_ITR2: Internal trigger 2 selected
  *            @arg TIM_TS_ITR3: Internal trigger 3 selected
  *            @arg TIM_TS_NONE: No trigger is needed
  * @param  CommutationSource : the Commutation Event source
  *          This parameter can be one of the following values:
  *            @arg TIM_COMMUTATION_TRGI: Commutation source is the TRGI of the Interface Timer
  *            @arg TIM_COMMUTATION_SOFTWARE:  Commutation source is set by software using the COMG bit
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutationEvent_IT(TIM_HandleTypeDef *htim, uint32_t  InputTrigger, uint32_t  CommutationSource)
{
  /* Check the parameters */
  assert_param(IS_TIM_COMMUTATION_EVENT_INSTANCE(htim->Instance));
  assert_param(IS_TIM_INTERNAL_TRIGGEREVENT_SELECTION(InputTrigger));

  __HAL_LOCK(htim);

  if ((InputTrigger == TIM_TS_ITR0) || (InputTrigger == TIM_TS_ITR1) ||
      (InputTrigger == TIM_TS_ITR2) || (InputTrigger == TIM_TS_ITR3))
  {
    /* Select the Input trigger */
    htim->Instance->SMCR &= ~TIM_SMCR_TS;
    htim->Instance->SMCR |= InputTrigger;
  }

  /* Select the Capture Compare preload feature */
  htim->Instance->CR2 |= TIM_CR2_CCPC;
  /* Select the Commutation event source */
  htim->Instance->CR2 &= ~TIM_CR2_CCUS;
  htim->Instance->CR2 |= CommutationSource;

  /* Enable the Commutation Interrupt Request */
  __HAL_TIM_ENABLE_IT(htim, TIM_IT_COM);

  __HAL_UNLOCK(htim);

  return HAL_OK;
}

/**
  * @brief  Configure the TIM commutation event sequence with DMA.
  * @note  This function is mandatory to use the commutation event in order to
  *        update the configuration at each commutation detection on the TRGI input of the Timer,
  *        the typical use of this feature is with the use of another Timer(interface Timer)
  *        configured in Hall sensor interface, this interface Timer will generate the
  *        commutation at its TRGO output (connected to Timer used in this function) each time
  *        the TI1 of the Interface Timer detect a commutation at its input TI1.
  * @note  The user should configure the DMA in his own software, in This function only the COMDE bit is set
  * @param  htim: TIM handle
  * @param  InputTrigger : the Internal trigger corresponding to the Timer Interfacing with the Hall sensor
  *          This parameter can be one of the following values:
  *            @arg TIM_TS_ITR0: Internal trigger 0 selected
  *            @arg TIM_TS_ITR1: Internal trigger 1 selected
  *            @arg TIM_TS_ITR2: Internal trigger 2 selected
  *            @arg TIM_TS_ITR3: Internal trigger 3 selected
  *            @arg TIM_TS_NONE: No trigger is needed
  * @param  CommutationSource : the Commutation Event source
  *          This parameter can be one of the following values:
  *            @arg TIM_COMMUTATION_TRGI: Commutation source is the TRGI of the Interface Timer
  *            @arg TIM_COMMUTATION_SOFTWARE:  Commutation source is set by software using the COMG bit
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutationEvent_DMA(TIM_HandleTypeDef *htim, uint32_t  InputTrigger, uint32_t  CommutationSource)
{
  /* Check the parameters */
  assert_param(IS_TIM_COMMUTATION_EVENT_INSTANCE(htim->Instance));
  assert_param(IS_TIM_INTERNAL_TRIGGEREVENT_SELECTION(InputTrigger));

  __HAL_LOCK(htim);

  if ((InputTrigger == TIM_TS_ITR0) || (InputTrigger == TIM_TS_ITR1) ||
      (InputTrigger == TIM_TS_ITR2) || (InputTrigger == TIM_TS_ITR3))
  {
    /* Select the Input trigger */
    htim->Instance->SMCR &= ~TIM_SMCR_TS;
    htim->Instance->SMCR |= InputTrigger;
  }

  /* Select the Capture Compare preload feature */
  htim->Instance->CR2 |= TIM_CR2_CCPC;
  /* Select the Commutation event source */
  htim->Instance->CR2 &= ~TIM_CR2_CCUS;
  htim->Instance->CR2 |= CommutationSource;

  /* Enable the Commutation DMA Request */
  /* Set the DMA Commutation Callback */
  htim->hdma[TIM_DMA_ID_COMMUTATION]->XferCpltCallback = TIMEx_DMACommutationCplt;
  /* Set the DMA error callback */
  htim->hdma[TIM_DMA_ID_COMMUTATION]->XferErrorCallback = TIM_DMAError;

  /* Enable the Commutation DMA Request */
  __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_COM);

  __HAL_UNLOCK(htim);

  return HAL_OK;
}

/**
  * @brief  Initializes the TIM Output Compare Channels according to the specified
  *         parameters in the TIM_OC_InitTypeDef.
  * @param  htim: TIM Output Compare handle
  * @param  sConfig: TIM Output Compare configuration structure
  * @param  Channel : TIM Channels to configure
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  *            @arg TIM_CHANNEL_5: TIM Channel 5 selected
  *            @arg TIM_CHANNEL_6: TIM Channel 6 selected
  *            @arg TIM_CHANNEL_ALL: all output channels supported by the timer instance selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIM_OC_ConfigChannel(TIM_HandleTypeDef *htim,
                                           TIM_OC_InitTypeDef* sConfig,
                                           uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_TIM_CHANNELS(Channel));
  assert_param(IS_TIM_OC_MODE(sConfig->OCMode));
  assert_param(IS_TIM_OC_POLARITY(sConfig->OCPolarity));

  /* Check input state */
  __HAL_LOCK(htim);

  htim->State = HAL_TIM_STATE_BUSY;

  switch (Channel)
  {
    case TIM_CHANNEL_1:
    {
      /* Check the parameters */
      assert_param(IS_TIM_CC1_INSTANCE(htim->Instance));

     /* Configure the TIM Channel 1 in Output Compare */
      TIM_OC1_SetConfig(htim->Instance, sConfig);
    }
    break;

    case TIM_CHANNEL_2:
    {
      /* Check the parameters */
      assert_param(IS_TIM_CC2_INSTANCE(htim->Instance));

      /* Configure the TIM Channel 2 in Output Compare */
      TIM_OC2_SetConfig(htim->Instance, sConfig);
    }
    break;

    case TIM_CHANNEL_3:
    {
      /* Check the parameters */
      assert_param(IS_TIM_CC3_INSTANCE(htim->Instance));

      /* Configure the TIM Channel 3 in Output Compare */
      TIM_OC3_SetConfig(htim->Instance, sConfig);
    }
    break;

    case TIM_CHANNEL_4:
    {
      /* Check the parameters */
      assert_param(IS_TIM_CC4_INSTANCE(htim->Instance));

       /* Configure the TIM Channel 4 in Output Compare */
       TIM_OC4_SetConfig(htim->Instance, sConfig);
    }
    break;

    case TIM_CHANNEL_5:
    {
      /* Check the parameters */
      assert_param(IS_TIM_CC5_INSTANCE(htim->Instance));

       /* Configure the TIM Channel 5 in Output Compare */
       TIM_OC5_SetConfig(htim->Instance, sConfig);
    }
    break;

    case TIM_CHANNEL_6:
    {
      /* Check the parameters */
      assert_param(IS_TIM_CC6_INSTANCE(htim->Instance));

       /* Configure the TIM Channel 6 in Output Compare */
       TIM_OC6_SetConfig(htim->Instance, sConfig);
    }
    break;

    default:
    break;
  }

  htim->State = HAL_TIM_STATE_READY;

  __HAL_UNLOCK(htim);

  return HAL_OK;
}

/**
  * @brief  Initializes the TIM PWM  channels according to the specified
  *         parameters in the TIM_OC_InitTypeDef.
  * @param  htim: TIM PWM handle
  * @param  sConfig: TIM PWM configuration structure
  * @param  Channel : TIM Channels to be configured
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  *            @arg TIM_CHANNEL_5: TIM Channel 5 selected
  *            @arg TIM_CHANNEL_6: TIM Channel 6 selected
  *            @arg TIM_CHANNEL_ALL: all PWM channels supported by the timer instance selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIM_PWM_ConfigChannel(TIM_HandleTypeDef *htim,
                                            TIM_OC_InitTypeDef* sConfig,
                                            uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_TIM_CHANNELS(Channel));
  assert_param(IS_TIM_PWM_MODE(sConfig->OCMode));
  assert_param(IS_TIM_OC_POLARITY(sConfig->OCPolarity));
  assert_param(IS_TIM_FAST_STATE(sConfig->OCFastMode));

  /* Check input state */
  __HAL_LOCK(htim);

  htim->State = HAL_TIM_STATE_BUSY;

  switch (Channel)
  {
    case TIM_CHANNEL_1:
    {
      /* Check the parameters */
      assert_param(IS_TIM_CC1_INSTANCE(htim->Instance));

      /* Configure the Channel 1 in PWM mode */
      TIM_OC1_SetConfig(htim->Instance, sConfig);

      /* Set the Preload enable bit for channel1 */
      htim->Instance->CCMR1 |= TIM_CCMR1_OC1PE;

      /* Configure the Output Fast mode */
      htim->Instance->CCMR1 &= ~TIM_CCMR1_OC1FE;
      htim->Instance->CCMR1 |= sConfig->OCFastMode;
    }
    break;

    case TIM_CHANNEL_2:
    {
      /* Check the parameters */
      assert_param(IS_TIM_CC2_INSTANCE(htim->Instance));

      /* Configure the Channel 2 in PWM mode */
      TIM_OC2_SetConfig(htim->Instance, sConfig);

      /* Set the Preload enable bit for channel2 */
      htim->Instance->CCMR1 |= TIM_CCMR1_OC2PE;

      /* Configure the Output Fast mode */
      htim->Instance->CCMR1 &= ~TIM_CCMR1_OC2FE;
      htim->Instance->CCMR1 |= sConfig->OCFastMode << 8;
    }
    break;

    case TIM_CHANNEL_3:
    {
      /* Check the parameters */
      assert_param(IS_TIM_CC3_INSTANCE(htim->Instance));

      /* Configure the Channel 3 in PWM mode */
      TIM_OC3_SetConfig(htim->Instance, sConfig);

      /* Set the Preload enable bit for channel3 */
      htim->Instance->CCMR2 |= TIM_CCMR2_OC3PE;

     /* Configure the Output Fast mode */
      htim->Instance->CCMR2 &= ~TIM_CCMR2_OC3FE;
      htim->Instance->CCMR2 |= sConfig->OCFastMode;
    }
    break;

    case TIM_CHANNEL_4:
    {
      /* Check the parameters */
      assert_param(IS_TIM_CC4_INSTANCE(htim->Instance));

      /* Configure the Channel 4 in PWM mode */
      TIM_OC4_SetConfig(htim->Instance, sConfig);

      /* Set the Preload enable bit for channel4 */
      htim->Instance->CCMR2 |= TIM_CCMR2_OC4PE;

     /* Configure the Output Fast mode */
      htim->Instance->CCMR2 &= ~TIM_CCMR2_OC4FE;
      htim->Instance->CCMR2 |= sConfig->OCFastMode << 8;
    }
    break;

    case TIM_CHANNEL_5:
    {
       /* Check the parameters */
      assert_param(IS_TIM_CC5_INSTANCE(htim->Instance));

     /* Configure the Channel 5 in PWM mode */
      TIM_OC5_SetConfig(htim->Instance, sConfig);

      /* Set the Preload enable bit for channel5*/
      htim->Instance->CCMR3 |= TIM_CCMR3_OC5PE;

     /* Configure the Output Fast mode */
      htim->Instance->CCMR3 &= ~TIM_CCMR3_OC5FE;
      htim->Instance->CCMR3 |= sConfig->OCFastMode;
    }
    break;

    case TIM_CHANNEL_6:
    {
       /* Check the parameters */
      assert_param(IS_TIM_CC6_INSTANCE(htim->Instance));

     /* Configure the Channel 5 in PWM mode */
      TIM_OC6_SetConfig(htim->Instance, sConfig);

      /* Set the Preload enable bit for channel6 */
      htim->Instance->CCMR3 |= TIM_CCMR3_OC6PE;

     /* Configure the Output Fast mode */
      htim->Instance->CCMR3 &= ~TIM_CCMR3_OC6FE;
      htim->Instance->CCMR3 |= sConfig->OCFastMode << 8;
    }
    break;

    default:
    break;
  }

  htim->State = HAL_TIM_STATE_READY;

  __HAL_UNLOCK(htim);

  return HAL_OK;
}

/**
  * @brief  Configures the OCRef clear feature
  * @param  htim: TIM handle
  * @param  sClearInputConfig: pointer to a TIM_ClearInputConfigTypeDef structure that
  *         contains the OCREF clear feature and parameters for the TIM peripheral.
  * @param  Channel: specifies the TIM Channel
  *          This parameter can be one of the following values:
  *            @arg TIM_Channel_1: TIM Channel 1
  *            @arg TIM_Channel_2: TIM Channel 2
  *            @arg TIM_Channel_3: TIM Channel 3
  *            @arg TIM_Channel_4: TIM Channel 4
  *            @arg TIM_Channel_5: TIM Channel 5
  *            @arg TIM_Channel_6: TIM Channel 6
  * @retval None
  */
HAL_StatusTypeDef HAL_TIM_ConfigOCrefClear(TIM_HandleTypeDef *htim,
                                           TIM_ClearInputConfigTypeDef *sClearInputConfig,
                                           uint32_t Channel)
{
  uint32_t tmpsmcr = 0;

  /* Check the parameters */
  assert_param(IS_TIM_OCXREF_CLEAR_INSTANCE(htim->Instance));
  assert_param(IS_TIM_CLEARINPUT_SOURCE(sClearInputConfig->ClearInputSource));

  /* Check input state */
  __HAL_LOCK(htim);

  switch (sClearInputConfig->ClearInputSource)
  {
    case TIM_CLEARINPUTSOURCE_NONE:
    {
      /* Get the TIMx SMCR register value */
      tmpsmcr = htim->Instance->SMCR;
      
      /* Clear the OCREF clear selection bit */
      tmpsmcr &= ~TIM_SMCR_OCCS;

      /* Clear the ETR Bits */
      tmpsmcr &= ~(TIM_SMCR_ETF | TIM_SMCR_ETPS | TIM_SMCR_ECE | TIM_SMCR_ETP);

      /* Set TIMx_SMCR */
      htim->Instance->SMCR = tmpsmcr;
   }
    break;

    case TIM_CLEARINPUTSOURCE_OCREFCLR:
    {
      /* Clear the OCREF clear selection bit */
      htim->Instance->SMCR &= ~TIM_SMCR_OCCS;
    }
    break;

    case TIM_CLEARINPUTSOURCE_ETR:
    {
      /* Check the parameters */
      assert_param(IS_TIM_CLEARINPUT_POLARITY(sClearInputConfig->ClearInputPolarity));
      assert_param(IS_TIM_CLEARINPUT_PRESCALER(sClearInputConfig->ClearInputPrescaler));
      assert_param(IS_TIM_CLEARINPUT_FILTER(sClearInputConfig->ClearInputFilter));

      TIM_ETR_SetConfig(htim->Instance,
                        sClearInputConfig->ClearInputPrescaler,
                        sClearInputConfig->ClearInputPolarity,
                        sClearInputConfig->ClearInputFilter);

      /* Set the OCREF clear selection bit */
      htim->Instance->SMCR |= TIM_SMCR_OCCS;
    }
    break;
    
  default:
    break;
  }

  switch (Channel)
  {
    case TIM_CHANNEL_1:
      {
        if(sClearInputConfig->ClearInputState != RESET)
        {
          /* Enable the OCREF clear feature for Channel 1 */
          htim->Instance->CCMR1 |= TIM_CCMR1_OC1CE;
        }
        else
        {
          /* Disable the OCREF clear feature for Channel 1 */
          htim->Instance->CCMR1 &= ~TIM_CCMR1_OC1CE;
        }
      }
      break;
    case TIM_CHANNEL_2:
      {
        if(sClearInputConfig->ClearInputState != RESET)
        {
          /* Enable the OCREF clear feature for Channel 2 */
          htim->Instance->CCMR1 |= TIM_CCMR1_OC2CE;
        }
        else
        {
          /* Disable the OCREF clear feature for Channel 2 */
          htim->Instance->CCMR1 &= ~TIM_CCMR1_OC2CE;
        }
      }
    break;
    case TIM_CHANNEL_3:
      {
        if(sClearInputConfig->ClearInputState != RESET)
        {
          /* Enable the OCREF clear feature for Channel 3 */
          htim->Instance->CCMR2 |= TIM_CCMR2_OC3CE;
        }
        else
        {
          /* Disable the OCREF clear feature for Channel 3 */
          htim->Instance->CCMR2 &= ~TIM_CCMR2_OC3CE;
        }
      }
    break;
    case TIM_CHANNEL_4:
      {
        if(sClearInputConfig->ClearInputState != RESET)
        {
          /* Enable the OCREF clear feature for Channel 4 */
          htim->Instance->CCMR2 |= TIM_CCMR2_OC4CE;
        }
        else
        {
          /* Disable the OCREF clear feature for Channel 4 */
          htim->Instance->CCMR2 &= ~TIM_CCMR2_OC4CE;
        }
      }
    break;
    case TIM_CHANNEL_5:
      {
        if(sClearInputConfig->ClearInputState != RESET)
        {
          /* Enable the OCREF clear feature for Channel 1 */
          htim->Instance->CCMR3 |= TIM_CCMR3_OC5CE;
        }
        else
        {
          /* Disable the OCREF clear feature for Channel 1 */
          htim->Instance->CCMR3 &= ~TIM_CCMR3_OC5CE;
        }
      }
    break;
    case TIM_CHANNEL_6:
      {
        if(sClearInputConfig->ClearInputState != RESET)
        {
          /* Enable the OCREF clear feature for Channel 1 */
          htim->Instance->CCMR3 |= TIM_CCMR3_OC6CE;
        }
        else
        {
          /* Disable the OCREF clear feature for Channel 1 */
          htim->Instance->CCMR3 &= ~TIM_CCMR3_OC6CE;
        }
      }
    break;
    default:
    break;
  }

  __HAL_UNLOCK(htim);

  return HAL_OK;
}

/**
  * @brief  Configures the TIM in master mode.
  * @param  htim: TIM handle.
  * @param  sMasterConfig: pointer to a TIM_MasterConfigTypeDef structure that
  *         contains the selected trigger output (TRGO) and the Master/Slave
  *         mode.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_MasterConfigSynchronization(TIM_HandleTypeDef *htim,
                                                      TIM_MasterConfigTypeDef * sMasterConfig)
{
  uint32_t tmpcr2;
  uint32_t tmpsmcr;

  /* Check the parameters */
  assert_param(IS_TIM_SYNCHRO_INSTANCE(htim->Instance));
  assert_param(IS_TIM_TRGO_SOURCE(sMasterConfig->MasterOutputTrigger));
  assert_param(IS_TIM_MSM_STATE(sMasterConfig->MasterSlaveMode));

  /* Check input state */
  __HAL_LOCK(htim);

 /* Get the TIMx CR2 register value */
  tmpcr2 = htim->Instance->CR2;

  /* Get the TIMx SMCR register value */
  tmpsmcr = htim->Instance->SMCR;

  /* If the timer supports ADC synchronization through TRGO2, set the master mode selection 2 */
  if (IS_TIM_TRGO2_INSTANCE(htim->Instance))
  {
    /* Check the parameters */
    assert_param(IS_TIM_TRGO2_SOURCE(sMasterConfig->MasterOutputTrigger2));

    /* Clear the MMS2 bits */
    tmpcr2 &= ~TIM_CR2_MMS2;
    /* Select the TRGO2 source*/
    tmpcr2 |= sMasterConfig->MasterOutputTrigger2;
  }

  /* Reset the MMS Bits */
  tmpcr2 &= ~TIM_CR2_MMS;
  /* Select the TRGO source */
  tmpcr2 |=  sMasterConfig->MasterOutputTrigger;

  /* Reset the MSM Bit */
  tmpsmcr &= ~TIM_SMCR_MSM;
  /* Set master mode */
  tmpsmcr |= sMasterConfig->MasterSlaveMode;

  /* Update TIMx CR2 */
  htim->Instance->CR2 = tmpcr2;

  /* Update TIMx SMCR */
  htim->Instance->SMCR = tmpsmcr;

  __HAL_UNLOCK(htim);

  return HAL_OK;
}

/**
  * @brief   Configures the Break feature, dead time, Lock level, OSSI/OSSR State
  *         and the AOE(automatic output enable).
  * @param  htim: TIM handle
  * @param  sBreakDeadTimeConfig: pointer to a TIM_ConfigBreakDeadConfigTypeDef structure that
  *         contains the BDTR Register configuration  information for the TIM peripheral.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_ConfigBreakDeadTime(TIM_HandleTypeDef *htim,
                                                TIM_BreakDeadTimeConfigTypeDef * sBreakDeadTimeConfig)
{
  uint32_t tmpbdtr = 0;

  /* Check the parameters */
  assert_param(IS_TIM_BREAK_INSTANCE(htim->Instance));
  assert_param(IS_TIM_OSSR_STATE(sBreakDeadTimeConfig->OffStateRunMode));
  assert_param(IS_TIM_OSSI_STATE(sBreakDeadTimeConfig->OffStateIDLEMode));
  assert_param(IS_TIM_LOCK_LEVEL(sBreakDeadTimeConfig->LockLevel));
  assert_param(IS_TIM_DEADTIME(sBreakDeadTimeConfig->DeadTime));
  assert_param(IS_TIM_BREAK_STATE(sBreakDeadTimeConfig->BreakState));
  assert_param(IS_TIM_BREAK_POLARITY(sBreakDeadTimeConfig->BreakPolarity));
  assert_param(IS_TIM_BREAK_FILTER(sBreakDeadTimeConfig->BreakFilter));
  assert_param(IS_TIM_AUTOMATIC_OUTPUT_STATE(sBreakDeadTimeConfig->AutomaticOutput));

  /* Check input state */
  __HAL_LOCK(htim);

  /* Set the Lock level, the Break enable Bit and the Polarity, the OSSR State,
     the OSSI State, the dead time value and the Automatic Output Enable Bit */
  if (IS_TIM_BKIN2_INSTANCE(htim->Instance))
  {
    assert_param(IS_TIM_BREAK2_STATE(sBreakDeadTimeConfig->Break2State));
    assert_param(IS_TIM_BREAK2_POLARITY(sBreakDeadTimeConfig->Break2Polarity));
    assert_param(IS_TIM_BREAK_FILTER(sBreakDeadTimeConfig->Break2Filter));

    /* Clear the BDTR bits */
    tmpbdtr &= ~(TIM_BDTR_DTG | TIM_BDTR_LOCK |  TIM_BDTR_OSSI |
                 TIM_BDTR_OSSR | TIM_BDTR_BKE | TIM_BDTR_BKP |
                 TIM_BDTR_AOE | TIM_BDTR_MOE | TIM_BDTR_BKF |
                 TIM_BDTR_BK2F | TIM_BDTR_BK2E | TIM_BDTR_BK2P);

    /* Set the BDTR bits */
    tmpbdtr |= sBreakDeadTimeConfig->DeadTime;
    tmpbdtr |= sBreakDeadTimeConfig->LockLevel;
    tmpbdtr |= sBreakDeadTimeConfig->OffStateIDLEMode;
    tmpbdtr |= sBreakDeadTimeConfig->OffStateRunMode;
    tmpbdtr |= sBreakDeadTimeConfig->BreakState;
    tmpbdtr |= sBreakDeadTimeConfig->BreakPolarity;
    tmpbdtr |= sBreakDeadTimeConfig->AutomaticOutput;
    tmpbdtr |= (sBreakDeadTimeConfig->BreakFilter << BDTR_BKF_SHIFT);
    tmpbdtr |= (sBreakDeadTimeConfig->Break2Filter << BDTR_BK2F_SHIFT);
    tmpbdtr |= sBreakDeadTimeConfig->Break2State;
    tmpbdtr |= sBreakDeadTimeConfig->Break2Polarity;
  }
  else
  {
    /* Clear the BDTR bits */
    tmpbdtr &= ~(TIM_BDTR_DTG | TIM_BDTR_LOCK |  TIM_BDTR_OSSI |
                 TIM_BDTR_OSSR | TIM_BDTR_BKE | TIM_BDTR_BKP |
                 TIM_BDTR_AOE | TIM_BDTR_MOE | TIM_BDTR_BKF);

    /* Set the BDTR bits */
    tmpbdtr |= sBreakDeadTimeConfig->DeadTime;
    tmpbdtr |= sBreakDeadTimeConfig->LockLevel;
    tmpbdtr |= sBreakDeadTimeConfig->OffStateIDLEMode;
    tmpbdtr |= sBreakDeadTimeConfig->OffStateRunMode;
    tmpbdtr |= sBreakDeadTimeConfig->BreakState;
    tmpbdtr |= sBreakDeadTimeConfig->BreakPolarity;
    tmpbdtr |= sBreakDeadTimeConfig->AutomaticOutput;
    tmpbdtr |= (sBreakDeadTimeConfig->BreakFilter << BDTR_BKF_SHIFT);
  }

  /* Set TIMx_BDTR */
  htim->Instance->BDTR = tmpbdtr;

  __HAL_UNLOCK(htim);

  return HAL_OK;
}

/**
  * @brief  Configures the break input source.
  * @param  htim: TIM handle.
  * @param  BreakInput: Break input to configure
  *          This parameter can be one of the following values:
  *            @arg TIM_BREAKINPUT_BRK: Timer break input
  *            @arg TIM_BREAKINPUT_BRK2: Timer break 2 input
  * @param  sBreakInputConfig: Break input source configuration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_ConfigBreakInput(TIM_HandleTypeDef *htim,
                                             uint32_t BreakInput,
                                             TIMEx_BreakInputConfigTypeDef *sBreakInputConfig)

{
  uint32_t tmporx = 0;
  uint32_t bkin_enable_mask = 0;
  uint32_t bkin_polarity_mask = 0;
  uint32_t bkin_enable_bitpos = 0;
  uint32_t bkin_polarity_bitpos = 0;

  /* Check the parameters */
  assert_param(IS_TIM_BREAK_INSTANCE(htim->Instance));
  assert_param(IS_TIM_BREAKINPUT(BreakInput));
  assert_param(IS_TIM_BREAKINPUTSOURCE(sBreakInputConfig->Source));
  assert_param(IS_TIM_BREAKINPUTSOURCE_STATE(sBreakInputConfig->Enable));

  if (sBreakInputConfig->Source != TIM_BREAKINPUTSOURCE_DFSDM)
  {
    assert_param(IS_TIM_BREAKINPUTSOURCE_POLARITY(sBreakInputConfig->Polarity));
  }
  
  /* Check input state */
  __HAL_LOCK(htim);
  
  switch(sBreakInputConfig->Source)
  {
  case TIM_BREAKINPUTSOURCE_BKIN:
    {
      bkin_enable_mask = TIM1_OR2_BKINE;
      bkin_enable_bitpos = 0;
      bkin_polarity_mask = TIM1_OR2_BKINP;
      bkin_polarity_bitpos = 9;
    }
    break;
  case TIM_BREAKINPUTSOURCE_COMP1:
    {
      bkin_enable_mask = TIM1_OR2_BKCMP1E;
      bkin_enable_bitpos = 1;
      bkin_polarity_mask = TIM1_OR2_BKCMP1P;
      bkin_polarity_bitpos = 10;
    }
    break;
  case TIM_BREAKINPUTSOURCE_COMP2:
    {
      bkin_enable_mask = TIM1_OR2_BKCMP2E;
      bkin_enable_bitpos = 2;
      bkin_polarity_mask = TIM1_OR2_BKCMP2P;
      bkin_polarity_bitpos = 11;
    }
    break;
  case TIM_BREAKINPUTSOURCE_DFSDM:
    {
      bkin_enable_mask = TIM1_OR2_BKDFBK0E;
      bkin_enable_bitpos = 8;
    }
    break;    
  default:
    break;
  }
  
  switch(BreakInput)
  {
    case TIM_BREAKINPUT_BRK:
      {
        /* Get the TIMx_OR2 register value */
        tmporx = htim->Instance->OR2;
        
        /* Enable the break input */
        tmporx &= ~bkin_enable_mask;
        tmporx |= (sBreakInputConfig->Enable << bkin_enable_bitpos) & bkin_enable_mask;
        
        /* Set the break input polarity */
        if (sBreakInputConfig->Source != TIM_BREAKINPUTSOURCE_DFSDM)
        {
          tmporx &= ~bkin_polarity_mask;
          tmporx |= (sBreakInputConfig->Polarity << bkin_polarity_bitpos) & bkin_polarity_mask;
        }
        
        /* Set TIMx_OR2 */
        htim->Instance->OR2 = tmporx;        
      }
        break;
    case TIM_BREAKINPUT_BRK2:
      {
        /* Get the TIMx_OR3 register value */
        tmporx = htim->Instance->OR3;
        
        /* Enable the break input */
        tmporx &= ~bkin_enable_mask;
        tmporx |= (sBreakInputConfig->Enable << bkin_enable_bitpos) & bkin_enable_mask;
        
        /* Set the break input polarity */
        if (sBreakInputConfig->Source != TIM_BREAKINPUTSOURCE_DFSDM)
        {
          tmporx &= ~bkin_polarity_mask;
          tmporx |= (sBreakInputConfig->Polarity << bkin_polarity_bitpos) & bkin_polarity_mask;
        }
        
        /* Set TIMx_OR3 */
        htim->Instance->OR3 = tmporx;        
      }
      break;    
  default:
    break;
  }
  
  __HAL_UNLOCK(htim);

  return HAL_OK;
}

/**
  * @brief  Configures the TIMx Remapping input capabilities.
  * @param  htim: TIM handle.
  * @param  Remap: specifies the TIM remapping source.
  *
  *         For TIM1, the parameter is a combination of 4 fields (field1 | field2 | field3 | field4):
  *
  *                   field1 can have the following values:
  *            @arg TIM_TIM1_ETR_ADC1_NONE:           TIM1_ETR is not connected to any ADC1 AWD (analog watchdog)
  *            @arg TIM_TIM1_ETR_ADC1_AWD1:           TIM1_ETR is connected to ADC1 AWD1
  *            @arg TIM_TIM1_ETR_ADC1_AWD2:           TIM1_ETR is connected to ADC1 AWD2
  *            @arg TIM_TIM1_ETR_ADC1_AWD3:           TIM1_ETR is connected to ADC1 AWD3
  *
  *                   field2 can have the following values:
  *            @arg TIM_TIM1_ETR_ADC3_NONE:           TIM1_ETR is not connected to any ADC3 AWD (analog watchdog)
  *            @arg TIM_TIM1_ETR_ADC3_AWD1:           TIM1_ETR is connected to ADC3 AWD1
  *            @arg TIM_TIM1_ETR_ADC3_AWD2:           TIM1_ETR is connected to ADC3 AWD2
  *            @arg TIM_TIM1_ETR_ADC3_AWD3:           TIM1_ETR is connected to ADC3 AWD3
  *
  *                   field3 can have the following values:
  *            @arg TIM_TIM1_TI1_GPIO:                TIM1 TI1 is connected to GPIO
  *            @arg TIM_TIM1_TI1_COMP1:               TIM1 TI1 is connected to COMP1 output
  *
  *                   field4 can have the following values:
  *            @arg TIM_TIM1_ETR_COMP1:               TIM1_ETR is connected to COMP1 output
  *            @arg TIM_TIM1_ETR_COMP2:               TIM1_ETR is connected to COMP2 output
  *            @note  When field4 is set to TIM_TIM1_ETR_COMP1 or TIM_TIM1_ETR_COMP2 field1 and field2 values are not significant 
  *
  *         For TIM2, the parameter is a combination of 3 fields (field1 | field2 | field3):
  *
  *                   field1 can have the following values:
  *            @arg TIM_TIM2_ITR1_TIM8_TRGO:          TIM2_ITR1 is connected to TIM8_TRGO
  *            @arg TIM_TIM2_ITR1_OTG_FS_SOF:         TIM2_ITR1 is connected to OTG_FS SOF
  *
  *                   field2 can have the following values:
  *            @arg TIM_TIM2_ETR_GPIO:                TIM2_ETR is connected to GPIO
  *            @arg TIM_TIM2_ETR_LSE:                 TIM2_ETR is connected to LSE
  *            @arg TIM_TIM2_ETR_COMP1:               TIM2_ETR is connected to COMP1 output
  *            @arg TIM_TIM2_ETR_COMP2:               TIM2_ETR is connected to COMP2 output
  *
  *                   field3 can have the following values:
  *            @arg TIM_TIM2_TI4_GPIO:                TIM2 TI4 is connected to GPIO
  *            @arg TIM_TIM2_TI4_COMP1:               TIM2 TI4 is connected to COMP1 output
  *            @arg TIM_TIM2_TI4_COMP2:               TIM2 TI4 is connected to COMP2 output
  *            @arg TIM_TIM2_TI4_COMP1_COMP2:         TIM2 TI4 is connected to logical OR between COMP1 and COMP2 output
  *
  *         For TIM3, the parameter is a combination 2 fields(field1 | field2):
  *
  *                   field1 can have the following values:
  *            @arg TIM_TIM3_TI1_GPIO:                TIM3 TI1 is connected to GPIO
  *            @arg TIM_TIM3_TI1_COMP1:               TIM3 TI1 is connected to COMP1 output
  *            @arg TIM_TIM3_TI1_COMP2:               TIM3 TI1 is connected to COMP2 output
  *            @arg TIM_TIM3_TI1_COMP1_COMP2:         TIM3 TI1 is connected to logical OR between COMP1 and COMP2 output
  *
  *                   field2 can have the following values:
  *            @arg TIM_TIM3_ETR_GPIO:                TIM3_ETR is connected to GPIO
  *            @arg TIM_TIM3_ETR_COMP1:               TIM3_ETR is connected to COMP1 output
  *
  *         For TIM8, the parameter is a combination of 3 fields (field1 | field2 | field3):
  *
  *                   field1 can have the following values:
  *            @arg TIM_TIM8_ETR_ADC2_NONE:          TIM8_ETR is not connected to any ADC2 AWD (analog watchdog)
  *            @arg TIM_TIM8_ETR_ADC2_AWD1:          TIM8_ETR is connected to ADC2 AWD1
  *            @arg TIM_TIM8_ETR_ADC2_AWD2:          TIM8_ETR is connected to ADC2 AWD2
  *            @arg TIM_TIM8_ETR_ADC2_AWD3:          TIM8_ETR is connected to ADC2 AWD3
  *
  *                   field2 can have the following values:
  *            @arg TIM_TIM8_ETR_ADC3_NONE:          TIM8_ETR is not connected to any ADC3 AWD (analog watchdog)
  *            @arg TIM_TIM8_ETR_ADC3_AWD1:          TIM8_ETR is connected to ADC3 AWD1
  *            @arg TIM_TIM8_ETR_ADC3_AWD2:          TIM8_ETR is connected to ADC3 AWD2
  *            @arg TIM_TIM8_ETR_ADC3_AWD3:          TIM8_ETR is connected to ADC3 AWD3
  *
  *                   field3 can have the following values:
  *            @arg TIM_TIM8_TI1_GPIO:               TIM8 TI1 is connected to GPIO
  *            @arg TIM_TIM8_TI1_COMP2:              TIM8 TI1 is connected to COMP2 output
  *
  *                   field4 can have the following values:
  *            @arg TIM_TIM8_ETR_COMP1:               TIM8_ETR is connected to COMP1 output
  *            @arg TIM_TIM8_ETR_COMP2:               TIM8_ETR is connected to COMP2 output
  *            @note  When field4 is set to TIM_TIM8_ETR_COMP1 or TIM_TIM8_ETR_COMP2 field1 and field2 values are not significant 
  *
  *         For TIM15, the parameter is a combination of 3 fields (field1 | field2):
  *
  *                   field1 can have the following values:
  *            @arg TIM_TIM15_TI1_GPIO:              TIM15 TI1 is connected to GPIO
  *            @arg TIM_TIM15_TI1_LSE:               TIM15 TI1 is connected to LSE
  *
  *                   field2 can have the following values:
  *            @arg TIM_TIM15_ENCODERMODE_NONE:      No redirection 
  *            @arg TIM_TIM15_ENCODERMODE_TIM2:      TIM2 IC1 and TIM2 IC2 are connected to TIM15 IC1 and TIM15 IC2 respectively
  *            @arg TIM_TIM15_ENCODERMODE_TIM3:      TIM3 IC1 and TIM3 IC2 are connected to TIM15 IC1 and TIM15 IC2 respectively
  *            @arg TIM_TIM15_ENCODERMODE_TIM4:      TIM4 IC1 and TIM4 IC2 are connected to TIM15 IC1 and TIM15 IC2 respectively
  *
  *         For TIM16, the parameter can have the following values:
  *            @arg TIM_TIM16_TI1_GPIO:              TIM16 TI1 is connected to GPIO
  *            @arg TIM_TIM16_TI1_LSI:               TIM16 TI1 is connected to LSI
  *            @arg TIM_TIM16_TI1_LSE:               TIM16 TI1 is connected to LSE
  *            @arg TIM_TIM16_TI1_RTC:               TIM16 TI1 is connected to RTC wakeup interrupt
  *
  *         For TIM17, the parameter can have the following values:
  *            @arg TIM_TIM17_TI1_GPIO:              TIM17 TI1 is connected to GPIO
  *            @arg TIM_TIM17_TI1_MSI:               TIM17 TI1 is connected to MSI  (contraints: MSI clock < 1/4 TIM APB clock)
  *            @arg TIM_TIM17_TI1_HSE_32:            TIM17 TI1 is connected to HSE div 32
  *            @arg TIM_TIM17_TI1_MCO:               TIM17 TI1 is connected to MCO
  *
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_RemapConfig(TIM_HandleTypeDef *htim, uint32_t Remap)
{
  uint32_t tmpor1 = 0;
  uint32_t tmpor2 = 0;
  
  __HAL_LOCK(htim);

  /* Check parameters */
  assert_param(IS_TIM_REMAP_INSTANCE(htim->Instance));
  assert_param(IS_TIM_REMAP(Remap));
  
  /* Set ETR_SEL bit field (if required) */
  if (IS_TIM_ETRSEL_INSTANCE(htim->Instance))
  {
    tmpor2 = htim->Instance->OR2;
    tmpor2 &= ~TIMx_ETRSEL_MASK;
    tmpor2 |= (Remap & TIMx_ETRSEL_MASK);
    
    /* Set TIMx_OR2 */
    htim->Instance->OR2 = tmpor2;        
  }
  
  /* Set other remapping capabilities */
  tmpor1 = Remap;
  tmpor1 &= ~TIMx_ETRSEL_MASK;

  /* Set TIMx_OR1 */
  htim->Instance->OR1 = Remap;
  
  /* Set TIMx_OR1 */
  htim->Instance->OR1 = tmpor1;        
  
  htim->State = HAL_TIM_STATE_READY;

  __HAL_UNLOCK(htim);

  return HAL_OK;
}

/**
  * @brief  Group channel 5 and channel 1, 2 or 3
  * @param  htim: TIM handle.
  * @param  Channels: specifies the reference signal(s) the OC5REF is combined with.
  *         This parameter can be any combination of the following values:
  *         TIM_GROUPCH5_NONE: No effect of OC5REF on OC1REFC, OC2REFC and OC3REFC
  *         TIM_GROUPCH5_OC1REFC: OC1REFC is the logical AND of OC1REFC and OC5REF
  *         TIM_GROUPCH5_OC2REFC: OC2REFC is the logical AND of OC2REFC and OC5REF
  *         TIM_GROUPCH5_OC3REFC: OC3REFC is the logical AND of OC3REFC and OC5REF
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_TIMEx_GroupChannel5(TIM_HandleTypeDef *htim, uint32_t Channels)
{
  /* Check parameters */
  assert_param(IS_TIM_COMBINED3PHASEPWM_INSTANCE(htim->Instance));
  assert_param(IS_TIM_GROUPCH5(Channels));

  /* Process Locked */
  __HAL_LOCK(htim);

  htim->State = HAL_TIM_STATE_BUSY;

  /* Clear GC5Cx bit fields */
  htim->Instance->CCR5 &= ~(TIM_CCR5_GC5C3|TIM_CCR5_GC5C2|TIM_CCR5_GC5C1);

  /* Set GC5Cx bit fields */
  htim->Instance->CCR5 |= Channels;

  htim->State = HAL_TIM_STATE_READY;

  __HAL_UNLOCK(htim);

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup TIMEx_Exported_Functions_Group6 Extended Callbacks functions
  * @brief    Extended Callbacks functions
  *
@verbatim
  ==============================================================================
                    ##### Extended Callbacks functions #####
  ==============================================================================
  [..]
    This section provides Extended TIM callback functions:
    (+) Timer Commutation callback
    (+) Timer Break callback

@endverbatim
  * @{
  */

/**
  * @brief  Hall commutation changed callback in non-blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
__weak void HAL_TIMEx_CommutationCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_TIMEx_CommutationCallback could be implemented in the user file
   */
}

/**
  * @brief  Hall Break detection callback in non-blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
__weak void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_TIMEx_BreakCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup TIMEx_Exported_Functions_Group7 Extended Peripheral State functions
  * @brief    Extended Peripheral State functions
  *
@verbatim
  ==============================================================================
                ##### Extended Peripheral State functions #####
  ==============================================================================
  [..]
    This subsection permits to get in run-time the status of the peripheral
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Return the TIM Hall Sensor interface handle state.
  * @param  htim: TIM Hall Sensor handle
  * @retval HAL state
  */
HAL_TIM_StateTypeDef HAL_TIMEx_HallSensor_GetState(TIM_HandleTypeDef *htim)
{
  return htim->State;
}

/**
  * @}
  */

/**
  * @brief  TIM DMA Commutation callback.
  * @param  hdma : pointer to DMA handle.
  * @retval None
  */
void TIMEx_DMACommutationCplt(DMA_HandleTypeDef *hdma)
{
  TIM_HandleTypeDef* htim = ( TIM_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  htim->State= HAL_TIM_STATE_READY;

  HAL_TIMEx_CommutationCallback(htim);
}

/**
  * @brief  Enables or disables the TIM Capture Compare Channel xN.
  * @param  TIMx to select the TIM peripheral
  * @param  Channel: specifies the TIM Channel
  *          This parameter can be one of the following values:
  *            @arg TIM_Channel_1: TIM Channel 1
  *            @arg TIM_Channel_2: TIM Channel 2
  *            @arg TIM_Channel_3: TIM Channel 3
  * @param  ChannelNState: specifies the TIM Channel CCxNE bit new state.
  *          This parameter can be: TIM_CCxN_ENABLE or TIM_CCxN_Disable.
  * @retval None
  */
static void TIM_CCxNChannelCmd(TIM_TypeDef* TIMx, uint32_t Channel, uint32_t ChannelNState)
{
  uint32_t tmp = 0;

  tmp = TIM_CCER_CC1NE << Channel;

  /* Reset the CCxNE Bit */
  TIMx->CCER &=  ~tmp;

  /* Set or reset the CCxNE Bit */
  TIMx->CCER |=  (uint32_t)(ChannelNState << Channel);
}

/**
  * @}
  */

#endif /* HAL_TIM_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
