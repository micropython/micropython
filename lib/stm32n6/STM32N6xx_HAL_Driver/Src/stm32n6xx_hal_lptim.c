/**
  ******************************************************************************
  * @file    stm32n6xx_hal_lptim.c
  * @author  MCD Application Team
  * @brief   LPTIM HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Low Power Timer (LPTIM) peripheral:
  *           + Initialization and de-initialization functions.
  *           + Start/Stop operation functions in polling mode.
  *           + Start/Stop operation functions in interrupt mode.
  *           + Reading operation functions.
  *           + Peripheral State functions.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
    [..]
      The LPTIM HAL driver can be used as follows:

      (#)Initialize the LPTIM low level resources by implementing the
        HAL_LPTIM_MspInit():
         (++) Enable the LPTIM interface clock using __HAL_RCC_LPTIMx_CLK_ENABLE().
         (++) In case of using interrupts (e.g. HAL_LPTIM_PWM_Start_IT()):
             (+++) Configure the LPTIM interrupt priority using HAL_NVIC_SetPriority().
             (+++) Enable the LPTIM IRQ handler using HAL_NVIC_EnableIRQ().
             (+++) In LPTIM IRQ handler, call HAL_LPTIM_IRQHandler().

      (#)Initialize the LPTIM HAL using HAL_LPTIM_Init(). This function
         configures mainly:
         (++) The instance: LPTIM1, LPTIM2, LPTIM3, LPTIM4 or LPTIM5.
         (++) Clock: the counter clock.
             (+++) Source   : it can be either the ULPTIM input (IN1) or one of
                              the internal clock; (APB, LSE, LSI or MSI).
             (+++) Prescaler: select the clock divider.
         (++)  UltraLowPowerClock : To be used only if the ULPTIM is selected
               as counter clock source.
             (+++) Polarity:   polarity of the active edge for the counter unit
                               if the ULPTIM input is selected.
             (+++) SampleTime: clock sampling time to configure the clock glitch
                               filter.
         (++) Trigger: How the counter start.
             (+++) Source: trigger can be software or one of the hardware triggers.
             (+++) ActiveEdge : only for hardware trigger.
             (+++) SampleTime : trigger sampling time to configure the trigger
                                glitch filter.
         (++) OutputPolarity : 2 opposite polarities are possible.
         (++) UpdateMode: specifies whether the update of the autoreload and
              the compare values is done immediately or after the end of current
              period.
         (++) Input1Source: Source selected for input1 (GPIO or comparator output).
         (++) Input2Source: Source selected for input2 (GPIO or comparator output).
              Input2 is used only for encoder feature so is used only for LPTIM1 instance.

      (#)Six modes are available:

         (++) PWM Mode: To generate a PWM signal with specified period and pulse,
         call HAL_LPTIM_PWM_Start() or HAL_LPTIM_PWM_Start_IT() for interruption
         mode.

         (++) One Pulse Mode: To generate pulse with specified width in response
         to a stimulus, call HAL_LPTIM_OnePulse_Start() or
         HAL_LPTIM_OnePulse_Start_IT() for interruption mode.

         (++) Set once Mode: In this mode, the output changes the level (from
         low level to high level if the output polarity is configured high, else
         the opposite) when a compare match occurs. To start this mode, call
         HAL_LPTIM_SetOnce_Start() or HAL_LPTIM_SetOnce_Start_IT() for
         interruption mode.

         (++) Encoder Mode: To use the encoder interface call
         HAL_LPTIM_Encoder_Start() or HAL_LPTIM_Encoder_Start_IT() for
         interruption mode. Only available for LPTIM1 instance.

         (++) Time out Mode: an active edge on one selected trigger input rests
         the counter. The first trigger event will start the timer, any
         successive trigger event will reset the counter and the timer will
         restart. To start this mode call HAL_LPTIM_TimeOut_Start_IT() or
         HAL_LPTIM_TimeOut_Start_IT() for interruption mode.

         (++) Counter Mode: counter can be used to count external events on
         the LPTIM Input1 or it can be used to count internal clock cycles.
         To start this mode, call HAL_LPTIM_Counter_Start() or
         HAL_LPTIM_Counter_Start_IT() for interruption mode.


      (#) User can stop any process by calling the corresponding API:
          HAL_LPTIM_Xxx_Stop() or HAL_LPTIM_Xxx_Stop_IT() if the process is
          already started in interruption mode.

      (#) De-initialize the LPTIM peripheral using HAL_LPTIM_DeInit().

    *** Callback registration ***
  =============================================
  [..]
  The compilation define  USE_HAL_LPTIM_REGISTER_CALLBACKS when set to 1
  allows the user to configure dynamically the driver callbacks.
  [..]
  Use Function HAL_LPTIM_RegisterCallback() to register a callback.
  HAL_LPTIM_RegisterCallback() takes as parameters the HAL peripheral handle,
  the Callback ID and a pointer to the user callback function.
  [..]
  Use function HAL_LPTIM_UnRegisterCallback() to reset a callback to the
  default weak function.
  HAL_LPTIM_UnRegisterCallback takes as parameters the HAL peripheral handle,
  and the Callback ID.
  [..]
  These functions allow to register/unregister following callbacks:

    (+) MspInitCallback         : LPTIM Base Msp Init Callback.
    (+) MspDeInitCallback       : LPTIM Base Msp DeInit Callback.
    (+) CompareMatchCallback    : Compare match Callback.
    (+) AutoReloadMatchCallback : Auto-reload match Callback.
    (+) TriggerCallback         : External trigger event detection Callback.
    (+) CompareWriteCallback    : Compare register write complete Callback.
    (+) AutoReloadWriteCallback : Auto-reload register write complete Callback.
    (+) DirectionUpCallback     : Up-counting direction change Callback.
    (+) DirectionDownCallback   : Down-counting direction change Callback.
    (+) UpdateEventCallback     : Update event detection Callback.
    (+) RepCounterWriteCallback : Repetition counter register write complete Callback.

  [..]
  By default, after the Init and when the state is HAL_LPTIM_STATE_RESET
  all interrupt callbacks are set to the corresponding weak functions:
  examples HAL_LPTIM_TriggerCallback(), HAL_LPTIM_CompareMatchCallback().

  [..]
  Exception done for MspInit and MspDeInit functions that are reset to the legacy weak
  functionalities in the Init/DeInit only when these callbacks are null
  (not registered beforehand). If not, MspInit or MspDeInit are not null, the Init/DeInit
  keep and use the user MspInit/MspDeInit callbacks (registered beforehand)

  [..]
  Callbacks can be registered/unregistered in HAL_LPTIM_STATE_READY state only.
  Exception done MspInit/MspDeInit that can be registered/unregistered
  in HAL_LPTIM_STATE_READY or HAL_LPTIM_STATE_RESET state,
  thus registered (user) MspInit/DeInit callbacks can be used during the Init/DeInit.
  In that case first register the MspInit/MspDeInit user callbacks
  using HAL_LPTIM_RegisterCallback() before calling DeInit or Init function.

  [..]
  When The compilation define USE_HAL_LPTIM_REGISTER_CALLBACKS is set to 0 or
  not defined, the callback registration feature is not available and all callbacks
  are set to the corresponding weak functions.

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup LPTIM LPTIM
  * @brief LPTIM HAL module driver.
  * @{
  */

#ifdef HAL_LPTIM_MODULE_ENABLED

#if defined (LPTIM1) || defined (LPTIM2) || defined (LPTIM3) || defined (LPTIM4) || defined (LPTIM5)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup LPTIM_Private_Constants
  * @{
  */
#define TIMEOUT                                     1000UL /* Timeout is 1s */
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/** @addtogroup LPTIM_Private_Macros
  * @{
  */
#define __HAL_LPTIM_WAKEUPTIMER_EXTI_ENABLE_IT(__INSTANCE__) \
  (((__INSTANCE__) == LPTIM1) ? __HAL_LPTIM_LPTIM1_EXTI_ENABLE_IT() :\
   ((__INSTANCE__) == LPTIM2) ? __HAL_LPTIM_LPTIM2_EXTI_ENABLE_IT() :\
   ((__INSTANCE__) == LPTIM3) ? __HAL_LPTIM_LPTIM3_EXTI_ENABLE_IT() :\
   ((__INSTANCE__) == LPTIM4) ? __HAL_LPTIM_LPTIM4_EXTI_ENABLE_IT() : __HAL_LPTIM_LPTIM5_EXTI_ENABLE_IT())

#define __HAL_LPTIM_WAKEUPTIMER_EXTI_DISABLE_IT(__INSTANCE__) \
  (((__INSTANCE__) == LPTIM1) ? __HAL_LPTIM_LPTIM1_EXTI_DISABLE_IT() :\
   ((__INSTANCE__) == LPTIM2) ? __HAL_LPTIM_LPTIM2_EXTI_DISABLE_IT() :\
   ((__INSTANCE__) == LPTIM3) ? __HAL_LPTIM_LPTIM3_EXTI_DISABLE_IT() :\
   ((__INSTANCE__) == LPTIM4) ? __HAL_LPTIM_LPTIM4_EXTI_DISABLE_IT() : __HAL_LPTIM_LPTIM5_EXTI_DISABLE_IT())
/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef LPTIM_OC1_SetConfig(LPTIM_HandleTypeDef *hlptim, const LPTIM_OC_ConfigTypeDef *sConfig);
static HAL_StatusTypeDef LPTIM_OC2_SetConfig(LPTIM_HandleTypeDef *hlptim, const LPTIM_OC_ConfigTypeDef *sConfig);
static void LPTIM_IC1_SetConfig(LPTIM_HandleTypeDef *hlptim, const LPTIM_IC_ConfigTypeDef *sConfig);
static void LPTIM_IC2_SetConfig(LPTIM_HandleTypeDef *hlptim, const LPTIM_IC_ConfigTypeDef *sConfig);
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
static void LPTIM_ResetCallback(LPTIM_HandleTypeDef *lptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
static HAL_StatusTypeDef LPTIM_WaitForFlag(const LPTIM_HandleTypeDef *hlptim, uint32_t flag);
void LPTIM_DMAError(DMA_HandleTypeDef *hdma);
void LPTIM_DMACaptureCplt(DMA_HandleTypeDef *hdma);
void LPTIM_DMACaptureHalfCplt(DMA_HandleTypeDef *hdma);
void LPTIM_DMAUpdateEventCplt(DMA_HandleTypeDef *hdma);
void LPTIM_DMAUpdateEventHalfCplt(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef LPTIM_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t src, uint32_t dst,
                                     uint32_t length);

/* Exported functions --------------------------------------------------------*/

/** @defgroup LPTIM_Exported_Functions LPTIM Exported Functions
  * @{
  */

/** @defgroup LPTIM_Exported_Functions_Group1 Initialization/de-initialization functions
  *  @brief    Initialization and Configuration functions.
  *
@verbatim
  ==============================================================================
              ##### Initialization and de-initialization functions #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize the LPTIM according to the specified parameters in the
          LPTIM_InitTypeDef and initialize the associated handle.
      (+) DeInitialize the LPTIM peripheral.
      (+) Initialize the LPTIM MSP.
      (+) DeInitialize the LPTIM MSP.

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the LPTIM according to the specified parameters in the
  *         LPTIM_InitTypeDef and initialize the associated handle.
  * @param  hlptim LPTIM handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_Init(LPTIM_HandleTypeDef *hlptim)
{
  uint32_t tmpcfgr;

  /* Check the LPTIM handle allocation */
  if (hlptim == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));
  assert_param(IS_LPTIM_PERIOD(hlptim->Init.Period));

  assert_param(IS_LPTIM_CLOCK_SOURCE(hlptim->Init.Clock.Source));
  assert_param(IS_LPTIM_CLOCK_PRESCALER(hlptim->Init.Clock.Prescaler));
  if ((hlptim->Init.Clock.Source == LPTIM_CLOCKSOURCE_ULPTIM)
      || (hlptim->Init.CounterSource == LPTIM_COUNTERSOURCE_EXTERNAL))
  {
    assert_param(IS_LPTIM_CLOCK_POLARITY(hlptim->Init.UltraLowPowerClock.Polarity));
    assert_param(IS_LPTIM_CLOCK_SAMPLE_TIME(hlptim->Init.UltraLowPowerClock.SampleTime));
  }
  assert_param(IS_LPTIM_TRG_SOURCE(hlptim->Init.Trigger.Source));
  if (hlptim->Init.Trigger.Source != LPTIM_TRIGSOURCE_SOFTWARE)
  {
    assert_param(IS_LPTIM_EXT_TRG_POLARITY(hlptim->Init.Trigger.ActiveEdge));
    assert_param(IS_LPTIM_TRIG_SAMPLE_TIME(hlptim->Init.Trigger.SampleTime));
  }
  assert_param(IS_LPTIM_UPDATE_MODE(hlptim->Init.UpdateMode));
  assert_param(IS_LPTIM_COUNTER_SOURCE(hlptim->Init.CounterSource));
  assert_param(IS_LPTIM_REPETITION(hlptim->Init.RepetitionCounter));

  if (hlptim->State == HAL_LPTIM_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hlptim->Lock = HAL_UNLOCKED;

#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
    /* Reset interrupt callbacks to legacy weak callbacks */
    LPTIM_ResetCallback(hlptim);

    if (hlptim->MspInitCallback == NULL)
    {
      hlptim->MspInitCallback = HAL_LPTIM_MspInit;
    }

    /* Init the low level hardware : GPIO, CLOCK, NVIC */
    hlptim->MspInitCallback(hlptim);
#else
    /* Init the low level hardware : GPIO, CLOCK, NVIC */
    HAL_LPTIM_MspInit(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
  }

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_REPOK);

  /* Set the repetition counter */
  __HAL_LPTIM_REPETITIONCOUNTER_SET(hlptim, hlptim->Init.RepetitionCounter);

  /* Wait for the completion of the write operation to the LPTIM_RCR register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_REPOK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }


  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_ARROK);

  /* Set LPTIM Period */
  __HAL_LPTIM_AUTORELOAD_SET(hlptim, hlptim->Init.Period);

  /* Wait for the completion of the write operation to the LPTIM_ARR register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_ARROK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Get the LPTIMx CFGR value */
  tmpcfgr = hlptim->Instance->CFGR;

  if ((hlptim->Init.Clock.Source == LPTIM_CLOCKSOURCE_ULPTIM)
      || (hlptim->Init.CounterSource == LPTIM_COUNTERSOURCE_EXTERNAL))
  {
    tmpcfgr &= (uint32_t)(~(LPTIM_CFGR_CKPOL | LPTIM_CFGR_CKFLT));
  }
  if (hlptim->Init.Trigger.Source != LPTIM_TRIGSOURCE_SOFTWARE)
  {
    tmpcfgr &= (uint32_t)(~(LPTIM_CFGR_TRGFLT | LPTIM_CFGR_TRIGSEL));
  }

  /* Clear CKSEL, PRESC, TRIGEN, TRGFLT, WAVPOL, PRELOAD & COUNTMODE bits */
  tmpcfgr &= (uint32_t)(~(LPTIM_CFGR_CKSEL | LPTIM_CFGR_TRIGEN | LPTIM_CFGR_PRELOAD |
                          LPTIM_CFGR_PRESC | LPTIM_CFGR_COUNTMODE));

  /* Set initialization parameters */
  tmpcfgr |= (hlptim->Init.Clock.Source    |
              hlptim->Init.Clock.Prescaler |
              hlptim->Init.UpdateMode      |
              hlptim->Init.CounterSource);

  /* Glitch filters for internal triggers and  external inputs are configured
   * only if an internal clock source is provided to the LPTIM
   */
  if (hlptim->Init.Clock.Source == LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC)
  {
    tmpcfgr |= (hlptim->Init.Trigger.SampleTime |
                hlptim->Init.UltraLowPowerClock.SampleTime);
  }

  /* Configure LPTIM external clock polarity and digital filter */
  if ((hlptim->Init.Clock.Source == LPTIM_CLOCKSOURCE_ULPTIM)
      || (hlptim->Init.CounterSource == LPTIM_COUNTERSOURCE_EXTERNAL))
  {
    tmpcfgr |= (hlptim->Init.UltraLowPowerClock.Polarity |
                hlptim->Init.UltraLowPowerClock.SampleTime);
  }

  /* Configure LPTIM external trigger */
  if (hlptim->Init.Trigger.Source != LPTIM_TRIGSOURCE_SOFTWARE)
  {
    /* Enable External trigger and set the trigger source */
    tmpcfgr |= (hlptim->Init.Trigger.Source     |
                hlptim->Init.Trigger.ActiveEdge |
                hlptim->Init.Trigger.SampleTime);
  }

  /* Write to LPTIMx CFGR */
  hlptim->Instance->CFGR = tmpcfgr;

  /* Configure LPTIM input sources */
  if ((hlptim->Instance == LPTIM1) || (hlptim->Instance == LPTIM2))
  {
    /* Check LPTIM Input1 and Input2 sources */
    assert_param(IS_LPTIM_INPUT1_SOURCE(hlptim->Instance, hlptim->Init.Input1Source));
    assert_param(IS_LPTIM_INPUT2_SOURCE(hlptim->Instance, hlptim->Init.Input2Source));

    /* Configure LPTIM Input1 and Input2 sources */
    hlptim->Instance->CFGR2 = (hlptim->Init.Input1Source | hlptim->Init.Input2Source);
  }
  else
  {
    if (hlptim->Instance == LPTIM3)
    {
      /* Check LPTIM3 Input1 source */
      assert_param(IS_LPTIM_INPUT1_SOURCE(hlptim->Instance, hlptim->Init.Input1Source));

      /* Configure LPTIM3 Input1 source */
      hlptim->Instance->CFGR2 = hlptim->Init.Input1Source;
    }
  }

  /* Initialize the LPTIM channels state */
  LPTIM_CHANNEL_STATE_SET_ALL(hlptim, HAL_LPTIM_CHANNEL_STATE_READY);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  DeInitialize the LPTIM peripheral.
  * @param  hlptim LPTIM handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_DeInit(LPTIM_HandleTypeDef *hlptim)
{
  /* Check the LPTIM handle allocation */
  if (hlptim == NULL)
  {
    return HAL_ERROR;
  }

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  __HAL_LPTIM_ENABLE(hlptim);
  if (IS_LPTIM_CC2_INSTANCE(hlptim->Instance))
  {
    hlptim->Instance->CCMR1 = 0;
  }

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_CMP1OK);

  __HAL_LPTIM_COMPARE_SET(hlptim, LPTIM_CHANNEL_1, 0);
  /* Wait for the completion of the write operation to the LPTIM_CCR1 register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_CMP1OK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  if (IS_LPTIM_CC2_INSTANCE(hlptim->Instance))
  {
    /* Clear flag */
    __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_CMP2OK);

    __HAL_LPTIM_COMPARE_SET(hlptim, LPTIM_CHANNEL_2, 0);
    /* Wait for the completion of the write operation to the LPTIM_CCR2 register */
    if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_CMP2OK) == HAL_TIMEOUT)
    {
      return HAL_TIMEOUT;
    }
  }

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_ARROK);

  __HAL_LPTIM_AUTORELOAD_SET(hlptim, 0);

  /* Wait for the completion of the write operation to the LPTIM_ARR register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_ARROK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* Disable the LPTIM Peripheral Clock */
  __HAL_LPTIM_DISABLE(hlptim);

  hlptim->Instance->CFGR = 0;
  hlptim->Instance->CFGR2 = 0;

#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
  if (hlptim->MspDeInitCallback == NULL)
  {
    hlptim->MspDeInitCallback = HAL_LPTIM_MspDeInit;
  }

  /* DeInit the low level hardware: CLOCK, NVIC.*/
  hlptim->MspDeInitCallback(hlptim);
#else
  /* DeInit the low level hardware: CLOCK, NVIC.*/
  HAL_LPTIM_MspDeInit(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */

  /* Change the LPTIM channels state */
  LPTIM_CHANNEL_STATE_SET_ALL(hlptim, HAL_LPTIM_CHANNEL_STATE_RESET);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hlptim);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initialize the LPTIM MSP.
  * @param  hlptim LPTIM handle
  * @retval None
  */
__weak void HAL_LPTIM_MspInit(LPTIM_HandleTypeDef *hlptim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LPTIM_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitialize LPTIM MSP.
  * @param  hlptim LPTIM handle
  * @retval None
  */
__weak void HAL_LPTIM_MspDeInit(LPTIM_HandleTypeDef *hlptim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LPTIM_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup LPTIM_Exported_Functions_Group2 LPTIM Start-Stop operation functions
  *  @brief   Start-Stop operation functions.
  *
@verbatim
  ==============================================================================
                ##### LPTIM Start Stop operation functions #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) Start the PWM mode.
      (+) Stop the PWM mode.
      (+) Start the One pulse mode.
      (+) Stop the One pulse mode.
      (+) Start the Set once mode.
      (+) Stop the Set once mode.
      (+) Start the Encoder mode.
      (+) Stop the Encoder mode.
      (+) Start the Timeout mode.
      (+) Stop the Timeout mode.
      (+) Start the Counter mode.
      (+) Stop the Counter mode.


@endverbatim
  * @{
  */

/**
  * @brief  Start the LPTIM PWM generation.
  * @param  hlptim LPTIM handle
  * @param  Channel LPTIM Channel to be enabled
  *         This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_PWM_Start(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Check LPTIM channel state */
  if (LPTIM_CHANNEL_STATE_GET(hlptim, Channel) != HAL_LPTIM_CHANNEL_STATE_READY)
  {
    return HAL_ERROR;
  }

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_BUSY);

  /* Reset WAVE bit to set PWM mode */
  hlptim->Instance->CFGR &= ~LPTIM_CFGR_WAVE;

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Enable LPTIM signal on the corresponding output pin */
  __HAL_LPTIM_CAPTURE_COMPARE_ENABLE(hlptim, Channel);

  /* Start timer in continuous mode */
  __HAL_LPTIM_START_CONTINUOUS(hlptim);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stop the LPTIM PWM generation.
  * @param  hlptim LPTIM handle
  * @param  Channel LPTIM Channel to be disabled
  *         This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_PWM_Stop(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Disable LPTIM signal from the corresponding output pin */
  __HAL_LPTIM_CAPTURE_COMPARE_DISABLE(hlptim, Channel);

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_READY);

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Start the LPTIM PWM generation in interrupt mode.
  * @param  hlptim LPTIM handle
  * @param  Channel LPTIM Channel to be enabled
  *         This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_PWM_Start_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Check LPTIM channel state */
  if (LPTIM_CHANNEL_STATE_GET(hlptim, Channel) != HAL_LPTIM_CHANNEL_STATE_READY)
  {
    return HAL_ERROR;
  }

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_BUSY);

  /* Reset WAVE bit to set PWM mode */
  hlptim->Instance->CFGR &= ~LPTIM_CFGR_WAVE;

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);
  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);

  switch (Channel)
  {
    case LPTIM_CHANNEL_1:
      /* Enable interrupt */
      __HAL_LPTIM_ENABLE_IT(hlptim, LPTIM_IT_CMP1OK | LPTIM_IT_CC1 | LPTIM_IT_ARROK | LPTIM_IT_ARRM | LPTIM_IT_REPOK |
                            LPTIM_IT_UPDATE);
      break;
    case LPTIM_CHANNEL_2:
      /* Enable interrupt */
      __HAL_LPTIM_ENABLE_IT(hlptim, LPTIM_IT_CMP2OK | LPTIM_IT_CC2 | LPTIM_IT_ARROK | LPTIM_IT_ARRM | LPTIM_IT_REPOK |
                            LPTIM_IT_UPDATE);
      break;
    default:
      break;
  }

  /* Wait for the completion of the write operation to the LPTIM_DIER register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* If external trigger source is used, then enable external trigger interrupt */
  if ((hlptim->Init.Trigger.Source) != LPTIM_TRIGSOURCE_SOFTWARE)
  {
    /* Clear flag */
    __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);

    /* Enable external trigger interrupt */
    __HAL_LPTIM_ENABLE_IT(hlptim, LPTIM_IT_EXTTRIG);

    /* Wait for the completion of the write operation to the LPTIM_DIER register */
    if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
    {
      return HAL_TIMEOUT;
    }
  }

  __HAL_LPTIM_CAPTURE_COMPARE_ENABLE(hlptim, Channel);

  /* Start timer in continuous mode */
  __HAL_LPTIM_START_CONTINUOUS(hlptim);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stop the LPTIM PWM generation in interrupt mode.
  * @param  hlptim LPTIM handle
  * @param  Channel LPTIM Channel to be disabled
  *         This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_PWM_Stop_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Disable LPTIM signal from the corresponding output pin */
  __HAL_LPTIM_CAPTURE_COMPARE_DISABLE(hlptim, Channel);

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);

  switch (Channel)
  {
    case LPTIM_CHANNEL_1:
      /* Disable interrupt */
      __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_CMP1OK | LPTIM_IT_CC1 | LPTIM_IT_ARROK | LPTIM_IT_ARRM | LPTIM_IT_REPOK |
                             LPTIM_IT_UPDATE);
      break;
    case LPTIM_CHANNEL_2:
      /* Disable interrupt */
      __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_CMP2OK | LPTIM_IT_CC2 | LPTIM_IT_ARROK | LPTIM_IT_ARRM | LPTIM_IT_REPOK |
                             LPTIM_IT_UPDATE);
      break;
    default:
      break;
  }

  /* Wait for the completion of the write operation to the LPTIM_DIER register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* If external trigger source is used, then enable external trigger interrupt */
  if ((hlptim->Init.Trigger.Source) != LPTIM_TRIGSOURCE_SOFTWARE)
  {
    /* Clear flag */
    __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);

    /* Enable external trigger interrupt */
    __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_EXTTRIG);

    /* Wait for the completion of the write operation to the LPTIM_DIER register */
    if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
    {
      return HAL_TIMEOUT;
    }
  }

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_READY);

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Start the LPTIM PWM generation in DMA mode.
  * @param  hlptim LPTIM handle
  * @param  Channel LPTIM Channel to be enabled
  *         This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @param  pData The destination Buffer address
  * @param  Length The length of data to be transferred from LPTIM peripheral to memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_PWM_Start_DMA(LPTIM_HandleTypeDef *hlptim, uint32_t Channel, const uint32_t *pData,
                                          uint32_t Length)
{
  DMA_HandleTypeDef *hdma;

  /* Check the parameters */
  assert_param(IS_LPTIM_DMA_INSTANCE(hlptim->Instance));
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  if ((pData == NULL) || (Length == 0U))
  {
    return HAL_ERROR;
  }

  /* Check LPTIM channel state */
  if (LPTIM_CHANNEL_STATE_GET(hlptim, Channel) != HAL_LPTIM_CHANNEL_STATE_READY)
  {
    return HAL_ERROR;
  }

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_BUSY);

  /* Reset WAVE bit to set PWM mode */
  hlptim->Instance->CFGR &= ~LPTIM_CFGR_WAVE;

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Enable update event DMA request */
  __HAL_LPTIM_ENABLE_DMA(hlptim, LPTIM_DMA_UPDATE);

  /* Wait for the completion of the write operation to the LPTIM_DIER register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  switch (Channel)
  {
    case LPTIM_CHANNEL_1:
      /* Set the DMA update event callbacks */
      hlptim->hdma[LPTIM_DMA_ID_CC1]->XferCpltCallback = LPTIM_DMAUpdateEventCplt;
      hlptim->hdma[LPTIM_DMA_ID_CC1]->XferHalfCpltCallback = LPTIM_DMAUpdateEventHalfCplt;

      /* Set the DMA error callback */
      hlptim->hdma[LPTIM_DMA_ID_CC1]->XferErrorCallback = LPTIM_DMAError;

      /* Enable the DMA Channel */
      hdma = hlptim->hdma[LPTIM_DMA_ID_CC1];
      if (LPTIM_DMA_Start_IT(hdma, (uint32_t)pData, (uint32_t)&hlptim->Instance->CCR1, Length) != HAL_OK)
      {
        /* Return error status */
        return HAL_ERROR;
      }
      break;
    case LPTIM_CHANNEL_2:
      /* Set the DMA update event callbacks */
      hlptim->hdma[LPTIM_DMA_ID_CC2]->XferCpltCallback = LPTIM_DMAUpdateEventCplt;
      hlptim->hdma[LPTIM_DMA_ID_CC2]->XferHalfCpltCallback = LPTIM_DMAUpdateEventHalfCplt;

      /* Set the DMA error callback */
      hlptim->hdma[LPTIM_DMA_ID_CC2]->XferErrorCallback = LPTIM_DMAError;

      /* Enable the DMA Channel */
      hdma = hlptim->hdma[LPTIM_DMA_ID_CC2];
      if (LPTIM_DMA_Start_IT(hdma, (uint32_t)pData, (uint32_t)&hlptim->Instance->CCR2, Length) != HAL_OK)
      {
        /* Return error status */
        return HAL_ERROR;
      }
      break;
    default:
      break;
  }

  /* Enable LPTIM signal on the corresponding output pin */
  __HAL_LPTIM_CAPTURE_COMPARE_ENABLE(hlptim, Channel);

  /* Start timer in continuous mode */
  __HAL_LPTIM_START_CONTINUOUS(hlptim);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stop the LPTIM PWM generation in DMA mode.
  * @param  hlptim LPTIM handle
  * @param  Channel LPTIM Channel to be disabled
  *         This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_PWM_Stop_DMA(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_DMA_INSTANCE(hlptim->Instance));
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Disable update event DMA request */
  __HAL_LPTIM_DISABLE_DMA(hlptim, LPTIM_DMA_UPDATE);

  switch (Channel)
  {
    case LPTIM_CHANNEL_1:
      /* Disable update event DMA request */
      (void)HAL_DMA_Abort_IT(hlptim->hdma[LPTIM_DMA_ID_CC1]);
      break;
    case LPTIM_CHANNEL_2:
      /* Disable update event DMA request */
      (void)HAL_DMA_Abort_IT(hlptim->hdma[LPTIM_DMA_ID_CC2]);
      break;
    default:
      break;
  }

  /* Disable LPTIM signal from the corresponding output pin */
  __HAL_LPTIM_CAPTURE_COMPARE_DISABLE(hlptim, Channel);

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_READY);

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Start the LPTIM One pulse generation.
  * @param  hlptim LPTIM handle
  * @param  Channel LPTIM Channel to be enabled
  *         This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_OnePulse_Start(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Check LPTIM channel state */
  if (LPTIM_CHANNEL_STATE_GET(hlptim, Channel) != HAL_LPTIM_CHANNEL_STATE_READY)
  {
    return HAL_ERROR;
  }

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_BUSY);

  /* Reset WAVE bit to set one pulse mode */
  hlptim->Instance->CFGR &= ~LPTIM_CFGR_WAVE;

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Enable LPTIM signal on the corresponding output pin */
  __HAL_LPTIM_CAPTURE_COMPARE_ENABLE(hlptim, Channel);

  /* Start timer in single (one shot) mode */
  __HAL_LPTIM_START_SINGLE(hlptim);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stop the LPTIM One pulse generation.
  * @param  hlptim LPTIM handle
  * @param  Channel LPTIM Channel to be disabled
  *         This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_OnePulse_Stop(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Disable LPTIM signal on the corresponding output pin */
  __HAL_LPTIM_CAPTURE_COMPARE_DISABLE(hlptim, Channel);

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_READY);

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Start the LPTIM One pulse generation in interrupt mode.
  * @param  hlptim LPTIM handle
  * @param  Channel LPTIM Channel to be enabled
  *         This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_OnePulse_Start_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Check LPTIM channel state */
  if (LPTIM_CHANNEL_STATE_GET(hlptim, Channel) != HAL_LPTIM_CHANNEL_STATE_READY)
  {
    return HAL_ERROR;
  }

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_BUSY);

  /* Reset WAVE bit to set one pulse mode */
  hlptim->Instance->CFGR &= ~LPTIM_CFGR_WAVE;

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);

  switch (Channel)
  {
    case LPTIM_CHANNEL_1:
      /* Enable  interrupt */
      __HAL_LPTIM_ENABLE_IT(hlptim, LPTIM_IT_CMP1OK | LPTIM_IT_CC1 | LPTIM_IT_ARROK | LPTIM_IT_ARRM | LPTIM_IT_REPOK |
                            LPTIM_IT_UPDATE);
      break;
    case LPTIM_CHANNEL_2:
      /* Enable interrupt */
      __HAL_LPTIM_ENABLE_IT(hlptim, LPTIM_IT_CMP2OK | LPTIM_IT_CC2 | LPTIM_IT_ARROK | LPTIM_IT_ARRM | LPTIM_IT_REPOK |
                            LPTIM_IT_UPDATE);
      break;
    default:
      break;
  }

  /* Wait for the completion of the write operation to the LPTIM_DIER register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* If external trigger source is used, then enable external trigger interrupt */
  if ((hlptim->Init.Trigger.Source) != LPTIM_TRIGSOURCE_SOFTWARE)
  {
    /* Clear flag */
    __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);
    /* Enable external trigger interrupt */
    __HAL_LPTIM_ENABLE_IT(hlptim, LPTIM_IT_EXTTRIG);
    /* Wait for the completion of the write operation to the LPTIM_DIER register */
    if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
    {
      return HAL_TIMEOUT;
    }
  }

  /* Enable LPTIM signal on the corresponding output pin */
  __HAL_LPTIM_CAPTURE_COMPARE_ENABLE(hlptim, Channel);

  /* Start timer in single (one shot) mode */
  __HAL_LPTIM_START_SINGLE(hlptim);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stop the LPTIM One pulse generation in interrupt mode.
  * @param  hlptim LPTIM handle
  * @param  Channel LPTIM Channel to be disabled
  *         This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_OnePulse_Stop_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Disable LPTIM signal on the corresponding output pin */
  __HAL_LPTIM_CAPTURE_COMPARE_DISABLE(hlptim, Channel);

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);


  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);

  switch (Channel)
  {
    case LPTIM_CHANNEL_1:
      /* Disable interrupt */
      __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_CMP1OK | LPTIM_IT_CC1 | LPTIM_IT_ARROK | LPTIM_IT_ARRM | LPTIM_IT_REPOK |
                             LPTIM_IT_UPDATE);
      break;
    case LPTIM_CHANNEL_2:
      /* Disable interrupt */
      __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_CMP2OK | LPTIM_IT_CC2 | LPTIM_IT_ARROK | LPTIM_IT_ARRM | LPTIM_IT_REPOK |
                             LPTIM_IT_UPDATE);
      break;
    default:
      break;
  }

  /* Wait for the completion of the write operation to the LPTIM_DIER register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* If external trigger source is used, then enable external trigger interrupt */
  if ((hlptim->Init.Trigger.Source) != LPTIM_TRIGSOURCE_SOFTWARE)
  {
    /* Clear flag */
    __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);
    /* Enable external trigger interrupt */
    __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_EXTTRIG);
    /* Wait for the completion of the write operation to the LPTIM_DIER register */
    if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
    {
      return HAL_TIMEOUT;
    }
  }

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_READY);

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Start the LPTIM in Set once mode.
  * @param  hlptim LPTIM handle
  * @param  Channel LPTIM Channel to be enabled
  *         This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_SetOnce_Start(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Check LPTIM channel state */
  if (LPTIM_CHANNEL_STATE_GET(hlptim, Channel) != HAL_LPTIM_CHANNEL_STATE_READY)
  {
    return HAL_ERROR;
  }

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_BUSY);

  /* Set WAVE bit to enable the set once mode */
  hlptim->Instance->CFGR |= LPTIM_CFGR_WAVE;

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Enable LPTIM signal on the corresponding output pin */
  __HAL_LPTIM_CAPTURE_COMPARE_ENABLE(hlptim, Channel);

  /* Start timer in single (one shot) mode */
  __HAL_LPTIM_START_SINGLE(hlptim);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stop the LPTIM Set once mode.
  * @param  hlptim LPTIM handle
  * @param  Channel LPTIM Channel to be disabled
  *         This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_SetOnce_Stop(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Disable LPTIM signal on the corresponding output pin */
  __HAL_LPTIM_CAPTURE_COMPARE_DISABLE(hlptim, Channel);

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_READY);

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Start the LPTIM Set once mode in interrupt mode.
  * @param  hlptim LPTIM handle
  * @param  Channel LPTIM Channel to be enabled
  *         This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_SetOnce_Start_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Check LPTIM channel state */
  if (LPTIM_CHANNEL_STATE_GET(hlptim, Channel) != HAL_LPTIM_CHANNEL_STATE_READY)
  {
    return HAL_ERROR;
  }

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_BUSY);

  /* Set WAVE bit to enable the set once mode */
  hlptim->Instance->CFGR |= LPTIM_CFGR_WAVE;

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);

  switch (Channel)
  {
    case LPTIM_CHANNEL_1:
      /* Enable interrupt */
      __HAL_LPTIM_ENABLE_IT(hlptim, LPTIM_IT_CMP1OK | LPTIM_IT_CC1 | LPTIM_IT_ARROK | LPTIM_IT_ARRM | LPTIM_IT_UPDATE);
      break;
    case LPTIM_CHANNEL_2:
      /* Enable interrupt */
      __HAL_LPTIM_ENABLE_IT(hlptim, LPTIM_IT_CMP2OK | LPTIM_IT_CC2 | LPTIM_IT_ARROK | LPTIM_IT_ARRM | LPTIM_IT_UPDATE);
      break;
    default:
      break;
  }

  /* Wait for the completion of the write operation to the LPTIM_DIER register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* If external trigger source is used, then enable external trigger interrupt */
  if ((hlptim->Init.Trigger.Source) != LPTIM_TRIGSOURCE_SOFTWARE)
  {
    /* Clear flag */
    __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);
    /* Enable external trigger interrupt */
    __HAL_LPTIM_ENABLE_IT(hlptim, LPTIM_IT_EXTTRIG);
    /* Wait for the completion of the write operation to the LPTIM_DIER register */
    if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
    {
      return HAL_TIMEOUT;
    }
  }

  /* Enable LPTIM signal on the corresponding output pin */
  __HAL_LPTIM_CAPTURE_COMPARE_ENABLE(hlptim, Channel);

  /* Start timer in single (one shot) mode */
  __HAL_LPTIM_START_SINGLE(hlptim);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stop the LPTIM Set once mode in interrupt mode.
  * @param  hlptim LPTIM handle
  * @param  Channel LPTIM Channel to be disabled
  *         This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_SetOnce_Stop_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Disable LPTIM signal on the corresponding output pin */
  __HAL_LPTIM_CAPTURE_COMPARE_DISABLE(hlptim, Channel);

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);

  switch (Channel)
  {
    case LPTIM_CHANNEL_1:
      /* Disable interrupt */
      __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_CMP1OK | LPTIM_IT_CC1 | LPTIM_IT_ARROK | LPTIM_IT_ARRM);
      break;
    case LPTIM_CHANNEL_2:
      /* Disable interrupt */
      __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_CMP2OK | LPTIM_IT_CC2 | LPTIM_IT_ARROK | LPTIM_IT_ARRM);
      break;
    default:
      break;
  }

  /* Wait for the completion of the write operation to the LPTIM_DIER register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* If external trigger source is used, then enable external trigger interrupt */
  if ((hlptim->Init.Trigger.Source) != LPTIM_TRIGSOURCE_SOFTWARE)
  {
    /* Clear flag */
    __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);
    /* Enable external trigger interrupt */
    __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_EXTTRIG);

    /* Wait for the completion of the write operation to the LPTIM_DIER register */
    if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
    {
      return HAL_TIMEOUT;
    }
  }

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_READY);

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Start the Encoder interface.
  * @param  hlptim LPTIM handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_Encoder_Start(LPTIM_HandleTypeDef *hlptim)
{
  uint32_t          tmpcfgr;

  /* Check the parameters */
  assert_param(IS_LPTIM_ENCODER_INTERFACE_INSTANCE(hlptim->Instance));
  assert_param(hlptim->Init.Clock.Source == LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC);
  assert_param(hlptim->Init.Clock.Prescaler == LPTIM_PRESCALER_DIV1);
  assert_param(IS_LPTIM_CLOCK_POLARITY(hlptim->Init.UltraLowPowerClock.Polarity));

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Get the LPTIMx CFGR value */
  tmpcfgr = hlptim->Instance->CFGR;

  /* Clear CKPOL bits */
  tmpcfgr &= (uint32_t)(~LPTIM_CFGR_CKPOL);

  /* Set Input polarity */
  tmpcfgr |=  hlptim->Init.UltraLowPowerClock.Polarity;

  /* Write to LPTIMx CFGR */
  hlptim->Instance->CFGR = tmpcfgr;

  /* Set ENC bit to enable the encoder interface */
  hlptim->Instance->CFGR |= LPTIM_CFGR_ENC;

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Start timer in continuous mode */
  __HAL_LPTIM_START_CONTINUOUS(hlptim);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stop the Encoder interface.
  * @param  hlptim LPTIM handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_Encoder_Stop(LPTIM_HandleTypeDef *hlptim)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_ENCODER_INTERFACE_INSTANCE(hlptim->Instance));

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Reset ENC bit to disable the encoder interface */
  hlptim->Instance->CFGR &= ~LPTIM_CFGR_ENC;

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Start the Encoder interface in interrupt mode.
  * @param  hlptim LPTIM handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_Encoder_Start_IT(LPTIM_HandleTypeDef *hlptim)
{
  uint32_t          tmpcfgr;

  /* Check the parameters */
  assert_param(IS_LPTIM_ENCODER_INTERFACE_INSTANCE(hlptim->Instance));
  assert_param(hlptim->Init.Clock.Source == LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC);
  assert_param(hlptim->Init.Clock.Prescaler == LPTIM_PRESCALER_DIV1);
  assert_param(IS_LPTIM_CLOCK_POLARITY(hlptim->Init.UltraLowPowerClock.Polarity));

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Configure edge sensitivity for encoder mode */
  /* Get the LPTIMx CFGR value */
  tmpcfgr = hlptim->Instance->CFGR;

  /* Clear CKPOL bits */
  tmpcfgr &= (uint32_t)(~LPTIM_CFGR_CKPOL);

  /* Set Input polarity */
  tmpcfgr |=  hlptim->Init.UltraLowPowerClock.Polarity;

  /* Write to LPTIMx CFGR */
  hlptim->Instance->CFGR = tmpcfgr;

  /* Set ENC bit to enable the encoder interface */
  hlptim->Instance->CFGR |= LPTIM_CFGR_ENC;

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);

  /* Enable "switch to up/down direction" interrupt */
  __HAL_LPTIM_ENABLE_IT(hlptim, LPTIM_IT_UP | LPTIM_IT_DOWN);

  /* Wait for the completion of the write operation to the LPTIM_DIER register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }


  /* Start timer in continuous mode */
  __HAL_LPTIM_START_CONTINUOUS(hlptim);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stop the Encoder interface in interrupt mode.
  * @param  hlptim LPTIM handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_Encoder_Stop_IT(LPTIM_HandleTypeDef *hlptim)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_ENCODER_INTERFACE_INSTANCE(hlptim->Instance));

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Reset ENC bit to disable the encoder interface */
  hlptim->Instance->CFGR &= ~LPTIM_CFGR_ENC;
  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);

  /* Disable "switch to down/up direction" interrupt */
  __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_UP | LPTIM_IT_DOWN);

  /* Wait for the completion of the write operation to the LPTIM_DIER register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Start the Timeout function.
  * @note   The first trigger event will start the timer, any successive
  *         trigger event will reset the counter and the timer restarts.
  * @param  hlptim LPTIM handle
  * @param  Timeout Specifies the TimeOut value to reset the counter.
  *         This parameter must be a value between 0x0000 and 0xFFFF.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_TimeOut_Start(LPTIM_HandleTypeDef *hlptim, uint32_t Timeout)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));
  assert_param(IS_LPTIM_PULSE(Timeout));

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Set TIMOUT bit to enable the timeout function */
  hlptim->Instance->CFGR |= LPTIM_CFGR_TIMOUT;

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_CMP1OK);

  /* Load the Timeout value in the compare register */
  __HAL_LPTIM_COMPARE_SET(hlptim, LPTIM_CHANNEL_1, Timeout);

  /* Wait for the completion of the write operation to the LPTIM_CCR1 register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_CMP1OK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* Start timer in continuous mode */
  __HAL_LPTIM_START_CONTINUOUS(hlptim);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stop the Timeout function.
  * @param  hlptim LPTIM handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_TimeOut_Stop(LPTIM_HandleTypeDef *hlptim)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Reset TIMOUT bit to enable the timeout function */
  hlptim->Instance->CFGR &= ~LPTIM_CFGR_TIMOUT;

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Start the Timeout function in interrupt mode.
  * @note   The first trigger event will start the timer, any successive
  *         trigger event will reset the counter and the timer restarts.
  * @param  hlptim LPTIM handle
  * @param  Timeout Specifies the TimeOut value to reset the counter.
  *         This parameter must be a value between 0x0000 and 0xFFFF.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_TimeOut_Start_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Timeout)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));
  assert_param(IS_LPTIM_PULSE(Timeout));

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Enable EXTI Line interrupt on the LPTIM Wake-up Timer */
  __HAL_LPTIM_WAKEUPTIMER_EXTI_ENABLE_IT(hlptim->Instance);

  /* Set TIMOUT bit to enable the timeout function */
  hlptim->Instance->CFGR |= LPTIM_CFGR_TIMOUT;

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);

  /* Enable Compare match CH1 interrupt */
  __HAL_LPTIM_ENABLE_IT(hlptim, LPTIM_IT_CC1);

  /* Wait for the completion of the write operation to the LPTIM_DIER register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_CMP1OK);

  /* Load the Timeout value in the compare register */
  __HAL_LPTIM_COMPARE_SET(hlptim, LPTIM_CHANNEL_1, Timeout);

  /* Wait for the completion of the write operation to the LPTIM_CCR1 register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_CMP1OK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* Start timer in continuous mode */
  __HAL_LPTIM_START_CONTINUOUS(hlptim);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stop the Timeout function in interrupt mode.
  * @param  hlptim LPTIM handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_TimeOut_Stop_IT(LPTIM_HandleTypeDef *hlptim)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));


  /* Disable EXTI Line interrupt on the LPTIM Wake-up Timer */
  __HAL_LPTIM_WAKEUPTIMER_EXTI_DISABLE_IT(hlptim->Instance);

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Reset TIMOUT bit to enable the timeout function */
  hlptim->Instance->CFGR &= ~LPTIM_CFGR_TIMOUT;

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);

  /* Disable Compare match CH1 interrupt */
  __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_CC1);

  /* Wait for the completion of the write operation to the LPTIM_DIER register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Start the Counter mode.
  * @param  hlptim LPTIM handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_Counter_Start(LPTIM_HandleTypeDef *hlptim)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* If clock source is not ULPTIM clock and counter source is external, then it must not be prescaled */
  if ((hlptim->Init.Clock.Source != LPTIM_CLOCKSOURCE_ULPTIM)
      && (hlptim->Init.CounterSource == LPTIM_COUNTERSOURCE_EXTERNAL))
  {
    /* Check if clock is prescaled */
    assert_param(IS_LPTIM_CLOCK_PRESCALERDIV1(hlptim->Init.Clock.Prescaler));
    /* Set clock prescaler to 0 */
    hlptim->Instance->CFGR &= ~LPTIM_CFGR_PRESC;
  }

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Start timer in continuous mode */
  __HAL_LPTIM_START_CONTINUOUS(hlptim);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stop the Counter mode.
  * @param  hlptim LPTIM handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_Counter_Stop(LPTIM_HandleTypeDef *hlptim)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Start the Counter mode in interrupt mode.
  * @param  hlptim LPTIM handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_Counter_Start_IT(LPTIM_HandleTypeDef *hlptim)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Enable EXTI Line interrupt on the LPTIM Wake-up Timer */
  __HAL_LPTIM_WAKEUPTIMER_EXTI_ENABLE_IT(hlptim->Instance);

  /* If clock source is not ULPTIM clock and counter source is external, then it must not be prescaled */
  if ((hlptim->Init.Clock.Source != LPTIM_CLOCKSOURCE_ULPTIM)
      && (hlptim->Init.CounterSource == LPTIM_COUNTERSOURCE_EXTERNAL))
  {
    /* Check if clock is prescaled */
    assert_param(IS_LPTIM_CLOCK_PRESCALERDIV1(hlptim->Init.Clock.Prescaler));
    /* Set clock prescaler to 0 */
    hlptim->Instance->CFGR &= ~LPTIM_CFGR_PRESC;
  }

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);

  /* Enable interrupt */
  __HAL_LPTIM_ENABLE_IT(hlptim, LPTIM_IT_ARROK | LPTIM_IT_ARRM | LPTIM_IT_REPOK | LPTIM_IT_UPDATE);

  /* Wait for the completion of the write operation to the LPTIM_DIER register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* Start timer in continuous mode */
  __HAL_LPTIM_START_CONTINUOUS(hlptim);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stop the Counter mode in interrupt mode.
  * @param  hlptim LPTIM handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_Counter_Stop_IT(LPTIM_HandleTypeDef *hlptim)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));


  /* Disable EXTI Line interrupt on the LPTIM Wake-up Timer */
  __HAL_LPTIM_WAKEUPTIMER_EXTI_DISABLE_IT(hlptim->Instance);

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);


  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DIEROK);

  /* Disable interrupt */
  __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_ARROK | LPTIM_IT_ARRM | LPTIM_IT_REPOK | LPTIM_IT_UPDATE);

  /* Wait for the completion of the write operation to the LPTIM_DIER register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Starts the LPTIM Input Capture measurement.
  * @param  hlptim LPTIM Input Capture handle
  * @param  Channel LPTIM Channels to be enabled
  *          This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: TIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_IC_Start(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_INPUT_CAPTURE_INSTANCE(hlptim->Instance));
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Check LPTIM channel state */
  if (LPTIM_CHANNEL_STATE_GET(hlptim, Channel) != HAL_LPTIM_CHANNEL_STATE_READY)
  {
    return HAL_ERROR;
  }

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_BUSY);

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Start timer in continuous mode */
  __HAL_LPTIM_START_CONTINUOUS(hlptim);

  /* Enable capture */
  __HAL_LPTIM_CAPTURE_COMPARE_ENABLE(hlptim, Channel);

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stops the LPTIM Input Capture measurement.
  * @param  hlptim LPTIM Input Capture handle
  * @param  Channel LPTIM Channels to be disabled
  *          This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: TIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_IC_Stop(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_INPUT_CAPTURE_INSTANCE(hlptim->Instance));
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Disable capture */
  __HAL_LPTIM_CAPTURE_COMPARE_DISABLE(hlptim, Channel);

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_READY);

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Starts the LPTIM Input Capture measurement in interrupt mode.
  * @param  hlptim LPTIM Input Capture handle
  * @param  Channel LPTIM Channels to be enabled
  *          This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: TIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_IC_Start_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_INPUT_CAPTURE_INSTANCE(hlptim->Instance));
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Check LPTIM channel state */
  if (LPTIM_CHANNEL_STATE_GET(hlptim, Channel) != HAL_LPTIM_CHANNEL_STATE_READY)
  {
    return HAL_ERROR;
  }

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_BUSY);

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  switch (Channel)
  {
    case LPTIM_CHANNEL_1:
      /* Enable Capture/Compare 1 interrupt */
      __HAL_LPTIM_ENABLE_IT(hlptim, LPTIM_IT_CC1);
      break;
    case LPTIM_CHANNEL_2:
      /* Disable Capture/Compare 2 interrupt */
      __HAL_LPTIM_ENABLE_IT(hlptim, LPTIM_IT_CC2);
      break;
    default:
      break;
  }

  /* Wait for the completion of the write operation to the LPTIM_DIER register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* Start timer in continuous mode */
  __HAL_LPTIM_START_CONTINUOUS(hlptim);

  /* Enable capture */
  __HAL_LPTIM_CAPTURE_COMPARE_ENABLE(hlptim, Channel);

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stops the LPTIM Input Capture measurement in interrupt mode.
  * @param  hlptim LPTIM Input Capture handle
  * @param  Channel LPTIM Channels to be disabled
  *          This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: TIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_IC_Stop_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_INPUT_CAPTURE_INSTANCE(hlptim->Instance));
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  switch (Channel)
  {
    case LPTIM_CHANNEL_1:
      /* Disable Capture/Compare 1 interrupt */
      __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_CC1);
      break;
    case LPTIM_CHANNEL_2:
      /* Disable Capture/Compare 2 interrupt */
      __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_CC2);
      break;
    default:
      return HAL_ERROR;
      break;
  }
  /* Disable capture */
  __HAL_LPTIM_CAPTURE_COMPARE_DISABLE(hlptim, Channel);

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_READY);

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Starts the LPTIM Input Capture measurement in DMA mode.
  * @param  hlptim LPTIM Input Capture handle
  * @param  Channel LPTIM Channels to be enabled
  *          This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: TIM Channel 2 selected
  * @param  pData The destination Buffer address
  * @param  Length The length of data to be transferred from LPTIM peripheral to memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_IC_Start_DMA(LPTIM_HandleTypeDef *hlptim, uint32_t Channel, uint32_t *pData,
                                         uint32_t Length)
{
  DMA_HandleTypeDef *hdma;

  /* Check the parameters */
  assert_param(IS_LPTIM_DMA_INSTANCE(hlptim->Instance));
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  if ((pData == NULL) || (Length == 0U))
  {
    return HAL_ERROR;
  }

  /* Check LPTIM channel state */
  if (LPTIM_CHANNEL_STATE_GET(hlptim, Channel) != HAL_LPTIM_CHANNEL_STATE_READY)
  {
    return HAL_ERROR;
  }

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_BUSY);

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  switch (Channel)
  {
    case LPTIM_CHANNEL_1:
      /* Set the DMA capture callbacks */
      hlptim->hdma[LPTIM_DMA_ID_CC1]->XferCpltCallback = LPTIM_DMACaptureCplt;
      hlptim->hdma[LPTIM_DMA_ID_CC1]->XferHalfCpltCallback = LPTIM_DMACaptureHalfCplt;

      /* Set the DMA error callback */
      hlptim->hdma[LPTIM_DMA_ID_CC1]->XferErrorCallback = LPTIM_DMAError;

      /* Enable the DMA Channel */
      hdma = hlptim->hdma[LPTIM_DMA_ID_CC1];
      if (LPTIM_DMA_Start_IT(hdma, (uint32_t)&hlptim->Instance->CCR1, (uint32_t)pData, Length) != HAL_OK)
      {
        /* Return error status */
        return HAL_ERROR;
      }

      /* Enable Capture/Compare 1 DMA request */
      __HAL_LPTIM_ENABLE_DMA(hlptim, LPTIM_DMA_CC1);
      break;

    case LPTIM_CHANNEL_2:
      /* Set the DMA capture callbacks */
      hlptim->hdma[LPTIM_DMA_ID_CC2]->XferCpltCallback = LPTIM_DMACaptureCplt;
      hlptim->hdma[LPTIM_DMA_ID_CC2]->XferHalfCpltCallback = LPTIM_DMACaptureHalfCplt;

      /* Set the DMA error callback */
      hlptim->hdma[LPTIM_DMA_ID_CC2]->XferErrorCallback = LPTIM_DMAError;

      /* Enable the DMA Channel */
      hdma = hlptim->hdma[LPTIM_DMA_ID_CC2];
      if (LPTIM_DMA_Start_IT(hdma, (uint32_t)&hlptim->Instance->CCR2, (uint32_t)pData, Length) != HAL_OK)
      {
        /* Return error status */
        return HAL_ERROR;
      }

      /* Enable Capture/Compare 2 DMA request */
      __HAL_LPTIM_ENABLE_DMA(hlptim, LPTIM_DMA_CC2);
      break;

    default:
      break;
  }

  /* Wait for the completion of the write operation to the LPTIM_DIER register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_DIEROK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* Start timer in continuous mode */
  __HAL_LPTIM_START_CONTINUOUS(hlptim);

  /* Enable capture */
  __HAL_LPTIM_CAPTURE_COMPARE_ENABLE(hlptim, Channel);

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Stops the LPTIM Input Capture measurement in DMA mode.
  * @param  hlptim LPTIM Input Capture handle
  * @param  Channel LPTIM Channels to be disabled
  *          This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: TIM Channel 2 selected
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_IC_Stop_DMA(LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_DMA_INSTANCE(hlptim->Instance));
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

  switch (Channel)
  {
    case LPTIM_CHANNEL_1:
      /* Disable Capture/Compare 1 DMA request */
      __HAL_LPTIM_DISABLE_DMA(hlptim, LPTIM_DMA_CC1);
      (void)HAL_DMA_Abort_IT(hlptim->hdma[LPTIM_DMA_ID_CC1]);
      break;

    case LPTIM_CHANNEL_2:
      /* Disable Capture/Compare 2 DMA request */
      __HAL_LPTIM_DISABLE_DMA(hlptim, LPTIM_DMA_CC2);
      (void)HAL_DMA_Abort_IT(hlptim->hdma[LPTIM_DMA_ID_CC2]);
      break;
    default:
      return HAL_ERROR;
      break;
  }

  /* Disable capture */
  __HAL_LPTIM_CAPTURE_COMPARE_DISABLE(hlptim, Channel);

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Set the LPTIM channel state */
  LPTIM_CHANNEL_STATE_SET(hlptim, Channel, HAL_LPTIM_CHANNEL_STATE_READY);

  /* Set the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}
/**
  * @}
  */

/** @defgroup LPTIM_Exported_Functions_Group3 LPTIM Read operation functions
  *  @brief  Read operation functions.
  *
@verbatim
  ==============================================================================
                  ##### LPTIM Read operation functions #####
  ==============================================================================
[..]  This section provides LPTIM Reading functions.
      (+) Read the counter value.
      (+) Read the period (Auto-reload) value.
      (+) Read the pulse (Compare)value.
@endverbatim
  * @{
  */

/**
  * @brief  Return the current counter value.
  * @param  hlptim LPTIM handle
  * @retval Counter value.
  */
uint32_t HAL_LPTIM_ReadCounter(const LPTIM_HandleTypeDef *hlptim)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));

  return (hlptim->Instance->CNT);
}

/**
  * @brief  Return the current Autoreload (Period) value.
  * @param  hlptim LPTIM handle
  * @retval Autoreload value.
  */
uint32_t HAL_LPTIM_ReadAutoReload(const LPTIM_HandleTypeDef *hlptim)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));

  return (hlptim->Instance->ARR);
}

/**
  * @brief  Return the current Compare (Pulse) value.
  * @param  hlptim LPTIM handle
  * @param  Channel LPTIM Channel to be selected
  *         This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @retval Compare value.
  */
uint32_t HAL_LPTIM_ReadCapturedValue(const LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{
  uint32_t tmpccr;

  /* Check the parameters */
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));

  switch (Channel)
  {
    case LPTIM_CHANNEL_1:
      tmpccr = hlptim->Instance->CCR1;
      break;
    case LPTIM_CHANNEL_2:
      tmpccr = hlptim->Instance->CCR2;
      break;
    default:
      tmpccr = 0;
      break;
  }
  return tmpccr;
}

/**
  * @brief  LPTimer Input Capture Get Offset(in counter step unit)
  * @note   The real capture value corresponding to the input capture trigger can be calculated using
  *         the formula hereafter : Real capture value = captured(LPTIM_CCRx) - offset
  *         The Offset value is depending on the glitch filter value for the channel
  *         and the value of the prescaler for the kernel clock.
  *         Please check Errata Sheet V1_8 for more details under "variable latency
  *         on input capture channel" section.
  * @param  hlptim pointer to a LPTIM_HandleTypeDef structure that contains
  *         the configuration information for LPTIM module.
  * @param  Channel This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @retval The offset value
  */
uint8_t HAL_LPTIM_IC_GetOffset(const LPTIM_HandleTypeDef *hlptim, uint32_t Channel)
{

  uint8_t offset ;
  uint32_t prescaler;
  uint32_t filter ;

  /* Get prescaler value */
  prescaler = LL_LPTIM_GetPrescaler(hlptim->Instance);

  /* Get filter value */
  filter = LL_LPTIM_IC_GetFilter(hlptim->Instance, Channel);

  /* Get offset value */
  offset = LL_LPTIM_IC_GET_OFFSET(prescaler, filter);

  /* return offset value */
  return offset;
}

/**
  * @}
  */
/** @defgroup LPTIM_Exported_Functions_Group5 LPTIM Config function
  *  @brief  Config channel
  *
@verbatim
  ==============================================================================
                  ##### LPTIM Config function #####
  ==============================================================================
[..]  This section provides LPTIM Config function.
      (+) Configure channel: Output Compare mode, Period, Polarity.
@endverbatim
  * @{
  */

/**
  * @brief
  * @param  hlptim LPTIM handle
  * @param  sConfig The output configuration structure
  * @param  Channel LPTIM Channel to be configured
  *         This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @note   Successive calls to HAL_LPTIM_OC_ConfigChannel can only be performed
  *         after a delay that must be greater or equal than the value of
  *        (PRESC x 3) kernel clock cycles, PRESC[2:0] being the clock decimal
  *         division factor (1, 2, 4, ..., 128). Any successive call violating
  *         this delay, leads to unpredictable results.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_OC_ConfigChannel(LPTIM_HandleTypeDef *hlptim, const LPTIM_OC_ConfigTypeDef *sConfig,
                                             uint32_t Channel)
{
  HAL_StatusTypeDef status;
  /* Check the parameters */
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));
  assert_param(IS_LPTIM_OC_POLARITY(sConfig->OCPolarity));
  assert_param(IS_LPTIM_PULSE(sConfig->Pulse));

  hlptim->State = HAL_LPTIM_STATE_BUSY;

  switch (Channel)
  {
    case LPTIM_CHANNEL_1:
    {
      /* Check the parameters */
      assert_param(IS_LPTIM_CC1_INSTANCE(hlptim->Instance));

      /* Configure the LPTIM Channel 1 in Output Compare */
      status = LPTIM_OC1_SetConfig(hlptim, sConfig);
      if (status != HAL_OK)
      {
        return status;
      }
      break;
    }
    case LPTIM_CHANNEL_2:
    {
      /* Check the parameters */
      assert_param(IS_LPTIM_CC2_INSTANCE(hlptim->Instance));

      /* Configure the LPTIM Channel 2 in Output Compare */
      status = LPTIM_OC2_SetConfig(hlptim, sConfig);
      if (status != HAL_OK)
      {
        return status;
      }
      break;
    }
    default:
      break;
  }

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief
  * @param  hlptim LPTIM handle
  * @param  sConfig The input configuration structure
  * @param  Channel LPTIM Channel to be configured
  *         This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @note   Successive calls to HAL_LPTIM_IC_ConfigChannel can only be performed
  *         after a delay that must be greater or equal than the value of
  *        (PRESC x 3) kernel clock cycles, PRESC[2:0] being the clock decimal
  *         division factor (1, 2, 4, ..., 128). Any successive call violating
  *         this delay, leads to unpredictable results.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LPTIM_IC_ConfigChannel(LPTIM_HandleTypeDef *hlptim, const LPTIM_IC_ConfigTypeDef *sConfig,
                                             uint32_t Channel)
{
  /* Check the parameters */
  assert_param(IS_LPTIM_CCX_INSTANCE(hlptim->Instance, Channel));
  assert_param(IS_LPTIM_IC_PRESCALER(sConfig->ICPrescaler));
  assert_param(IS_LPTIM_IC_POLARITY(sConfig->ICPolarity));
  assert_param(IS_LPTIM_IC_FILTER(sConfig->ICFilter));

  hlptim->State = HAL_LPTIM_STATE_BUSY;

  switch (Channel)
  {
    case LPTIM_CHANNEL_1:
    {
      /* Check the parameters */
      assert_param(IS_LPTIM_CC1_INSTANCE(hlptim->Instance));
      assert_param(IS_LPTIM_IC1_SOURCE(hlptim->Instance, sConfig->ICInputSource));

      /* Configure the LPTIM Channel 1 in Input Capture */
      LPTIM_IC1_SetConfig(hlptim, sConfig);
      break;
    }
    case LPTIM_CHANNEL_2:
    {
      /* Check the parameters */
      assert_param(IS_LPTIM_CC2_INSTANCE(hlptim->Instance));
      assert_param(IS_LPTIM_IC2_SOURCE(hlptim->Instance, sConfig->ICInputSource));

      /* Configure the LPTIM Channel 2 in Input Capture */
      LPTIM_IC2_SetConfig(hlptim, sConfig);
      break;
    }
    default:
      break;
  }

  /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;
  /* Return function status */
  return HAL_OK;
}
/**
  * @}
  */

/** @defgroup LPTIM_Exported_Functions_Group4 LPTIM IRQ handler and callbacks
  *  @brief  LPTIM  IRQ handler.
  *
@verbatim
  ==============================================================================
                      ##### LPTIM IRQ handler and callbacks  #####
  ==============================================================================
[..]  This section provides LPTIM IRQ handler and callback functions called within
      the IRQ handler:
   (+) LPTIM interrupt request handler
   (+) Compare match Callback
   (+) Auto-reload match Callback
   (+) External trigger event detection Callback
   (+) Compare register write complete Callback
   (+) Auto-reload register write complete Callback
   (+) Up-counting direction change Callback
   (+) Down-counting direction change Callback

@endverbatim
  * @{
  */

/**
  * @brief  Handle LPTIM interrupt request.
  * @param  hlptim LPTIM handle
  * @retval None
  */
void HAL_LPTIM_IRQHandler(LPTIM_HandleTypeDef *hlptim)
{
  /* Capture Compare 1 interrupt */
  if (__HAL_LPTIM_GET_FLAG(hlptim, LPTIM_FLAG_CC1) != RESET)
  {
    if (__HAL_LPTIM_GET_IT_SOURCE(hlptim, LPTIM_FLAG_CC1) != RESET)
    {
      __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_CC1);
      hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_1;

      /* Input capture event */
      if ((hlptim->Instance->CCMR1 & LPTIM_CCMR1_CC1SEL) != 0x00U)
      {
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
        hlptim->IC_CaptureCallback(hlptim);
#else
        HAL_LPTIM_IC_CaptureCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
      }
      /* Output compare event */
      else
      {
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
        hlptim->CompareMatchCallback(hlptim);
#else
        HAL_LPTIM_CompareMatchCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
      }
      hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_CLEARED;
    }
  }

  /* Capture Compare 2 interrupt */
  if (__HAL_LPTIM_GET_FLAG(hlptim, LPTIM_FLAG_CC2) != RESET)
  {
    if (__HAL_LPTIM_GET_IT_SOURCE(hlptim, LPTIM_FLAG_CC2) != RESET)
    {
      __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_CC2);
      hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_2;

      /* Input capture event */
      if ((hlptim->Instance->CCMR1 & LPTIM_CCMR1_CC2SEL) != 0x00U)
      {
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
        hlptim->IC_CaptureCallback(hlptim);
#else
        HAL_LPTIM_IC_CaptureCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
      }
      /* Output compare event */
      else
      {
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
        hlptim->CompareMatchCallback(hlptim);
#else
        HAL_LPTIM_CompareMatchCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
      }
      hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_CLEARED;
    }
  }

  /* Over Capture 1 interrupt */
  if (__HAL_LPTIM_GET_FLAG(hlptim, LPTIM_FLAG_CC1O) != RESET)
  {
    if (__HAL_LPTIM_GET_IT_SOURCE(hlptim, LPTIM_FLAG_CC1O) != RESET)
    {
      __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_CC1O);
      hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_1;

      /* Over capture event */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
      hlptim->IC_OverCaptureCallback(hlptim);
#else
      HAL_LPTIM_IC_OverCaptureCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
      hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_CLEARED;
    }
  }

  /* Over Capture 2 interrupt */
  if (__HAL_LPTIM_GET_FLAG(hlptim, LPTIM_FLAG_CC2O) != RESET)
  {
    if (__HAL_LPTIM_GET_IT_SOURCE(hlptim, LPTIM_FLAG_CC2O) != RESET)
    {
      __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_CC2O);
      hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_2;

      /* Over capture event */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
      hlptim->IC_OverCaptureCallback(hlptim);
#else
      HAL_LPTIM_IC_OverCaptureCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
      hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_CLEARED;
    }
  }

  /* Autoreload match interrupt */
  if (__HAL_LPTIM_GET_FLAG(hlptim, LPTIM_FLAG_ARRM) != RESET)
  {
    if (__HAL_LPTIM_GET_IT_SOURCE(hlptim, LPTIM_IT_ARRM) != RESET)
    {
      /* Clear Autoreload match flag */
      __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_ARRM);

      /* Autoreload match Callback */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
      hlptim->AutoReloadMatchCallback(hlptim);
#else
      HAL_LPTIM_AutoReloadMatchCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
    }
  }

  /* Trigger detected interrupt */
  if (__HAL_LPTIM_GET_FLAG(hlptim, LPTIM_FLAG_EXTTRIG) != RESET)
  {
    if (__HAL_LPTIM_GET_IT_SOURCE(hlptim, LPTIM_IT_EXTTRIG) != RESET)
    {
      /* Clear Trigger detected flag */
      __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_EXTTRIG);

      /* Trigger detected callback */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
      hlptim->TriggerCallback(hlptim);
#else
      HAL_LPTIM_TriggerCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
    }
  }

  /* Compare write interrupt */
  if (__HAL_LPTIM_GET_FLAG(hlptim, LPTIM_FLAG_CMP1OK) != RESET)
  {
    if (__HAL_LPTIM_GET_IT_SOURCE(hlptim, LPTIM_IT_CMP1OK) != RESET)
    {
      /* Clear Compare write flag */
      __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_CMP1OK);
      hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_1;
      /* Compare write Callback */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
      hlptim->CompareWriteCallback(hlptim);
#else
      HAL_LPTIM_CompareWriteCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
    }
  }

  /* Compare write interrupt */
  if (__HAL_LPTIM_GET_FLAG(hlptim, LPTIM_FLAG_CMP2OK) != RESET)
  {
    if (__HAL_LPTIM_GET_IT_SOURCE(hlptim, LPTIM_IT_CMP2OK) != RESET)
    {
      /* Clear Compare write flag */
      __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_CMP2OK);
      hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_2;
      /* Compare write Callback */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
      hlptim->CompareWriteCallback(hlptim);
#else
      HAL_LPTIM_CompareWriteCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
    }
  }

  /* Autoreload write interrupt */
  if (__HAL_LPTIM_GET_FLAG(hlptim, LPTIM_FLAG_ARROK) != RESET)
  {
    if (__HAL_LPTIM_GET_IT_SOURCE(hlptim, LPTIM_IT_ARROK) != RESET)
    {
      /* Clear Autoreload write flag */
      __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_ARROK);

      /* Autoreload write Callback */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
      hlptim->AutoReloadWriteCallback(hlptim);
#else
      HAL_LPTIM_AutoReloadWriteCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
    }
  }

  /* Direction counter changed from Down to Up interrupt */
  if (__HAL_LPTIM_GET_FLAG(hlptim, LPTIM_FLAG_UP) != RESET)
  {
    if (__HAL_LPTIM_GET_IT_SOURCE(hlptim, LPTIM_IT_UP) != RESET)
    {
      /* Clear Direction counter changed from Down to Up flag */
      __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_UP);

      /* Direction counter changed from Down to Up Callback */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
      hlptim->DirectionUpCallback(hlptim);
#else
      HAL_LPTIM_DirectionUpCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
    }
  }

  /* Direction counter changed from Up to Down interrupt */
  if (__HAL_LPTIM_GET_FLAG(hlptim, LPTIM_FLAG_DOWN) != RESET)
  {
    if (__HAL_LPTIM_GET_IT_SOURCE(hlptim, LPTIM_IT_DOWN) != RESET)
    {
      /* Clear Direction counter changed from Up to Down flag */
      __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_DOWN);

      /* Direction counter changed from Up to Down Callback */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
      hlptim->DirectionDownCallback(hlptim);
#else
      HAL_LPTIM_DirectionDownCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
    }
  }

  /* Repetition counter underflowed (or contains zero) and the LPTIM counter
     overflowed */
  if (__HAL_LPTIM_GET_FLAG(hlptim, LPTIM_FLAG_UPDATE) != RESET)
  {
    if (__HAL_LPTIM_GET_IT_SOURCE(hlptim, LPTIM_IT_UPDATE) != RESET)
    {
      /* Clear update event flag */
      __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_UPDATE);

      /* Update event Callback */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
      hlptim->UpdateEventCallback(hlptim);
#else
      HAL_LPTIM_UpdateEventCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
    }
  }

  /* Successful APB bus write to repetition counter register */
  if (__HAL_LPTIM_GET_FLAG(hlptim, LPTIM_FLAG_REPOK) != RESET)
  {
    if (__HAL_LPTIM_GET_IT_SOURCE(hlptim, LPTIM_IT_REPOK) != RESET)
    {
      /* Clear successful APB bus write to repetition counter flag */
      __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_REPOK);

      /* Successful APB bus write to repetition counter Callback */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
      hlptim->RepCounterWriteCallback(hlptim);
#else
      HAL_LPTIM_RepCounterWriteCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
    }
  }
}

/**
  * @brief  Compare match callback in non-blocking mode.
  * @param  hlptim LPTIM handle
  * @retval None
  */
__weak void HAL_LPTIM_CompareMatchCallback(LPTIM_HandleTypeDef *hlptim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LPTIM_CompareMatchCallback could be implemented in the user file
   */
}

/**
  * @brief  Autoreload match callback in non-blocking mode.
  * @param  hlptim LPTIM handle
  * @retval None
  */
__weak void HAL_LPTIM_AutoReloadMatchCallback(LPTIM_HandleTypeDef *hlptim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LPTIM_AutoReloadMatchCallback could be implemented in the user file
   */
}

/**
  * @brief  Trigger detected callback in non-blocking mode.
  * @param  hlptim LPTIM handle
  * @retval None
  */
__weak void HAL_LPTIM_TriggerCallback(LPTIM_HandleTypeDef *hlptim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LPTIM_TriggerCallback could be implemented in the user file
   */
}

/**
  * @brief  Compare write callback in non-blocking mode.
  * @param  hlptim LPTIM handle
  * @retval None
  */
__weak void HAL_LPTIM_CompareWriteCallback(LPTIM_HandleTypeDef *hlptim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LPTIM_CompareWriteCallback could be implemented in the user file
   */
}

/**
  * @brief  Autoreload write callback in non-blocking mode.
  * @param  hlptim LPTIM handle
  * @retval None
  */
__weak void HAL_LPTIM_AutoReloadWriteCallback(LPTIM_HandleTypeDef *hlptim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LPTIM_AutoReloadWriteCallback could be implemented in the user file
   */
}

/**
  * @brief  Direction counter changed from Down to Up callback in non-blocking mode.
  * @param  hlptim LPTIM handle
  * @retval None
  */
__weak void HAL_LPTIM_DirectionUpCallback(LPTIM_HandleTypeDef *hlptim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LPTIM_DirectionUpCallback could be implemented in the user file
   */
}

/**
  * @brief  Direction counter changed from Up to Down callback in non-blocking mode.
  * @param  hlptim LPTIM handle
  * @retval None
  */
__weak void HAL_LPTIM_DirectionDownCallback(LPTIM_HandleTypeDef *hlptim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LPTIM_DirectionDownCallback could be implemented in the user file
   */
}

/**
  * @brief Repetition counter underflowed (or contains zero) and LPTIM counter overflowed callback in non-blocking mode.
  * @param  hlptim LPTIM handle
  * @retval None
  */
__weak void HAL_LPTIM_UpdateEventCallback(LPTIM_HandleTypeDef *hlptim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LPTIM_UpdateEventCallback could be implemented in the user file
   */
}

/**
  * @brief  Successful APB bus write to repetition counter register callback in non-blocking mode.
  * @param  hlptim LPTIM handle
  * @retval None
  */
__weak void HAL_LPTIM_RepCounterWriteCallback(LPTIM_HandleTypeDef *hlptim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LPTIM_RepCounterWriteCallback could be implemented in the user file
   */
}

/**
  * @brief  Input Capture callback in non-blocking mode
  * @param  hlptim LPTIM handle
  * @retval None
  */
__weak void HAL_LPTIM_IC_CaptureCallback(LPTIM_HandleTypeDef *hlptim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LPTIM_IC_CaptureCallback could be implemented in the user file
   */
}

/**
  * @brief  Over Capture callback in non-blocking mode
  * @param  hlptim LPTIM handle
  * @retval None
  */
__weak void HAL_LPTIM_IC_OverCaptureCallback(LPTIM_HandleTypeDef *hlptim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LPTIM_IC_OverCaptureCallback could be implemented in the user file
   */
}

/**
  * @brief  Input Capture half complete callback in non-blocking mode
  * @param  hlptim LPTIM IC handle
  * @retval None
  */
__weak void HAL_LPTIM_IC_CaptureHalfCpltCallback(LPTIM_HandleTypeDef *hlptim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LPTIM_IC_CaptureHalfCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Update event half complete callback in non-blocking mode
  * @param  hlptim LPTIM handle
  * @retval None
  */
__weak void HAL_LPTIM_UpdateEventHalfCpltCallback(LPTIM_HandleTypeDef *hlptim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LPTIM_UpdateEventHalfCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Error callback in non-blocking mode
  * @param  hlptim LPTIM handle
  * @retval None
  */
__weak void HAL_LPTIM_ErrorCallback(LPTIM_HandleTypeDef *hlptim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LPTIM_ErrorCallback could be implemented in the user file
   */
}


#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register a User LPTIM callback to be used instead of the weak predefined callback
  * @param hlptim LPTIM handle
  * @param CallbackID ID of the callback to be registered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_LPTIM_MSPINIT_CB_ID          LPTIM Base Msp Init Callback ID
  *          @arg @ref HAL_LPTIM_MSPDEINIT_CB_ID        LPTIM Base Msp DeInit Callback ID
  *          @arg @ref HAL_LPTIM_COMPARE_MATCH_CB_ID    Compare match Callback ID
  *          @arg @ref HAL_LPTIM_AUTORELOAD_MATCH_CB_ID Auto-reload match Callback ID
  *          @arg @ref HAL_LPTIM_TRIGGER_CB_ID          External trigger event detection Callback ID
  *          @arg @ref HAL_LPTIM_COMPARE_WRITE_CB_ID    Compare register write complete Callback ID
  *          @arg @ref HAL_LPTIM_AUTORELOAD_WRITE_CB_ID Auto-reload register write complete Callback ID
  *          @arg @ref HAL_LPTIM_DIRECTION_UP_CB_ID     Up-counting direction change Callback ID
  *          @arg @ref HAL_LPTIM_DIRECTION_DOWN_CB_ID   Down-counting direction change Callback ID
  *          @arg @ref HAL_LPTIM_UPDATE_EVENT_CB_ID      Update event detection Callback ID
  *          @arg @ref HAL_LPTIM_REP_COUNTER_WRITE_CB_ID Repetition counter register write complete Callback ID
  *          @arg @ref HAL_LPTIM_UPDATE_EVENT_HALF_CB_ID Update event Half detection Callback ID
  *          @arg @ref HAL_LPTIM_ERROR_CB_ID             Error  Callback ID
  *          @arg @ref HAL_LPTIM_IC_CAPTURE_CB_ID        Input Capture Callback ID
  *          @arg @ref HAL_LPTIM_IC_CAPTURE_HALF_CB_ID   Input Capture half complete Callback ID
  *          @arg @ref HAL_LPTIM_OVER_CAPTURE_CB_ID      Over Capture Callback ID
  * @param pCallback pointer to the callback function
  * @retval status
  */
HAL_StatusTypeDef HAL_LPTIM_RegisterCallback(LPTIM_HandleTypeDef        *hlptim,
                                             HAL_LPTIM_CallbackIDTypeDef CallbackID,
                                             pLPTIM_CallbackTypeDef      pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    return HAL_ERROR;
  }

  if (hlptim->State == HAL_LPTIM_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_LPTIM_MSPINIT_CB_ID :
        hlptim->MspInitCallback = pCallback;
        break;

      case HAL_LPTIM_MSPDEINIT_CB_ID :
        hlptim->MspDeInitCallback = pCallback;
        break;

      case HAL_LPTIM_COMPARE_MATCH_CB_ID :
        hlptim->CompareMatchCallback = pCallback;
        break;

      case HAL_LPTIM_AUTORELOAD_MATCH_CB_ID :
        hlptim->AutoReloadMatchCallback = pCallback;
        break;

      case HAL_LPTIM_TRIGGER_CB_ID :
        hlptim->TriggerCallback = pCallback;
        break;

      case HAL_LPTIM_COMPARE_WRITE_CB_ID :
        hlptim->CompareWriteCallback = pCallback;
        break;

      case HAL_LPTIM_AUTORELOAD_WRITE_CB_ID :
        hlptim->AutoReloadWriteCallback = pCallback;
        break;

      case HAL_LPTIM_DIRECTION_UP_CB_ID :
        hlptim->DirectionUpCallback = pCallback;
        break;

      case HAL_LPTIM_DIRECTION_DOWN_CB_ID :
        hlptim->DirectionDownCallback = pCallback;
        break;

      case HAL_LPTIM_UPDATE_EVENT_CB_ID :
        hlptim->UpdateEventCallback = pCallback;
        break;

      case HAL_LPTIM_REP_COUNTER_WRITE_CB_ID :
        hlptim->RepCounterWriteCallback = pCallback;
        break;

      case HAL_LPTIM_UPDATE_EVENT_HALF_CB_ID :
        hlptim->UpdateEventHalfCpltCallback = pCallback;
        break;

      case HAL_LPTIM_ERROR_CB_ID :
        hlptim->ErrorCallback = pCallback;
        break;

      case HAL_LPTIM_IC_CAPTURE_CB_ID :
        hlptim->IC_CaptureCallback = pCallback;
        break;

      case HAL_LPTIM_IC_CAPTURE_HALF_CB_ID :
        hlptim->IC_CaptureHalfCpltCallback = pCallback;
        break;

      case HAL_LPTIM_OVER_CAPTURE_CB_ID :
        hlptim->IC_OverCaptureCallback = pCallback;
        break;

      default :
        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hlptim->State == HAL_LPTIM_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_LPTIM_MSPINIT_CB_ID :
        hlptim->MspInitCallback = pCallback;
        break;

      case HAL_LPTIM_MSPDEINIT_CB_ID :
        hlptim->MspDeInitCallback = pCallback;
        break;

      default :
        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Unregister a LPTIM callback
  *         LLPTIM callback is redirected to the weak predefined callback
  * @param hlptim LPTIM handle
  * @param CallbackID ID of the callback to be unregistered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_LPTIM_MSPINIT_CB_ID          LPTIM Base Msp Init Callback ID
  *          @arg @ref HAL_LPTIM_MSPDEINIT_CB_ID        LPTIM Base Msp DeInit Callback ID
  *          @arg @ref HAL_LPTIM_COMPARE_MATCH_CB_ID    Compare match Callback ID
  *          @arg @ref HAL_LPTIM_AUTORELOAD_MATCH_CB_ID Auto-reload match Callback ID
  *          @arg @ref HAL_LPTIM_TRIGGER_CB_ID          External trigger event detection Callback ID
  *          @arg @ref HAL_LPTIM_COMPARE_WRITE_CB_ID    Compare register write complete Callback ID
  *          @arg @ref HAL_LPTIM_AUTORELOAD_WRITE_CB_ID Auto-reload register write complete Callback ID
  *          @arg @ref HAL_LPTIM_DIRECTION_UP_CB_ID     Up-counting direction change Callback ID
  *          @arg @ref HAL_LPTIM_DIRECTION_DOWN_CB_ID   Down-counting direction change Callback ID
  *          @arg @ref HAL_LPTIM_UPDATE_EVENT_CB_ID      Update event detection Callback ID
  *          @arg @ref HAL_LPTIM_REP_COUNTER_WRITE_CB_ID Repetition counter register write complete Callback ID
  *          @arg @ref HAL_LPTIM_UPDATE_EVENT_HALF_CB_ID Update event Half detection Callback ID
  *          @arg @ref HAL_LPTIM_ERROR_CB_ID             Error  Callback ID
  *          @arg @ref HAL_LPTIM_IC_CAPTURE_CB_ID        Input Capture Callback ID
  *          @arg @ref HAL_LPTIM_IC_CAPTURE_HALF_CB_ID   Input Capture half complete Callback ID
  *          @arg @ref HAL_LPTIM_OVER_CAPTURE_CB_ID      Over Capture Callback ID
  * @retval status
  */
HAL_StatusTypeDef HAL_LPTIM_UnRegisterCallback(LPTIM_HandleTypeDef        *hlptim,
                                               HAL_LPTIM_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hlptim->State == HAL_LPTIM_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_LPTIM_MSPINIT_CB_ID :
        /* Legacy weak MspInit Callback */
        hlptim->MspInitCallback = HAL_LPTIM_MspInit;
        break;

      case HAL_LPTIM_MSPDEINIT_CB_ID :
        /* Legacy weak Msp DeInit Callback */
        hlptim->MspDeInitCallback = HAL_LPTIM_MspDeInit;
        break;

      case HAL_LPTIM_COMPARE_MATCH_CB_ID :
        /* Legacy weak Compare match Callback */
        hlptim->CompareMatchCallback = HAL_LPTIM_CompareMatchCallback;
        break;

      case HAL_LPTIM_AUTORELOAD_MATCH_CB_ID :
        /* Legacy weak Auto-reload match Callback */
        hlptim->AutoReloadMatchCallback = HAL_LPTIM_AutoReloadMatchCallback;
        break;

      case HAL_LPTIM_TRIGGER_CB_ID :
        /* Legacy weak External trigger event detection Callback */
        hlptim->TriggerCallback = HAL_LPTIM_TriggerCallback;
        break;

      case HAL_LPTIM_COMPARE_WRITE_CB_ID :
        /* Legacy weak Compare register write complete Callback */
        hlptim->CompareWriteCallback = HAL_LPTIM_CompareWriteCallback;
        break;

      case HAL_LPTIM_AUTORELOAD_WRITE_CB_ID :
        /* Legacy weak Auto-reload register write complete Callback */
        hlptim->AutoReloadWriteCallback = HAL_LPTIM_AutoReloadWriteCallback;
        break;

      case HAL_LPTIM_DIRECTION_UP_CB_ID :
        /* Legacy weak Up-counting direction change Callback */
        hlptim->DirectionUpCallback = HAL_LPTIM_DirectionUpCallback;
        break;

      case HAL_LPTIM_DIRECTION_DOWN_CB_ID :
        /* Legacy weak Down-counting direction change Callback */
        hlptim->DirectionDownCallback = HAL_LPTIM_DirectionDownCallback;
        break;

      case HAL_LPTIM_UPDATE_EVENT_CB_ID :
        /* Legacy weak Update event detection Callback */
        hlptim->UpdateEventCallback = HAL_LPTIM_UpdateEventCallback;
        break;

      case HAL_LPTIM_REP_COUNTER_WRITE_CB_ID :
        /* Legacy weak Repetition counter register write complete Callback */
        hlptim->RepCounterWriteCallback = HAL_LPTIM_RepCounterWriteCallback;
        break;

      case HAL_LPTIM_UPDATE_EVENT_HALF_CB_ID :
        /* Legacy weak Update event half complete detection Callback */
        hlptim->UpdateEventHalfCpltCallback = HAL_LPTIM_UpdateEventHalfCpltCallback;
        break;

      case HAL_LPTIM_ERROR_CB_ID :
        /* Legacy weak error Callback */
        hlptim->ErrorCallback = HAL_LPTIM_ErrorCallback;
        break;

      case HAL_LPTIM_IC_CAPTURE_CB_ID :
        /* Legacy weak IC Capture Callback */
        hlptim->IC_CaptureCallback = HAL_LPTIM_IC_CaptureCallback;
        break;

      case HAL_LPTIM_IC_CAPTURE_HALF_CB_ID :
        /* Legacy weak IC Capture half complete Callback */
        hlptim->IC_CaptureHalfCpltCallback = HAL_LPTIM_IC_CaptureHalfCpltCallback;
        break;

      case HAL_LPTIM_OVER_CAPTURE_CB_ID :
        /* Legacy weak IC over capture Callback */
        hlptim->IC_OverCaptureCallback = HAL_LPTIM_IC_OverCaptureCallback;
        break;

      default :
        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hlptim->State == HAL_LPTIM_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_LPTIM_MSPINIT_CB_ID :
        /* Legacy weak MspInit Callback */
        hlptim->MspInitCallback = HAL_LPTIM_MspInit;
        break;

      case HAL_LPTIM_MSPDEINIT_CB_ID :
        /* Legacy weak Msp DeInit Callback */
        hlptim->MspDeInitCallback = HAL_LPTIM_MspDeInit;
        break;

      default :
        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @defgroup LPTIM_Group5 Peripheral State functions
  *  @brief   Peripheral State functions.
  *
@verbatim
  ==============================================================================
                      ##### Peripheral State functions #####
  ==============================================================================
    [..]
    This subsection permits to get in run-time the status of the peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Return the LPTIM handle state.
  * @param  hlptim LPTIM handle
  * @retval HAL state
  */
HAL_LPTIM_StateTypeDef HAL_LPTIM_GetState(const LPTIM_HandleTypeDef *hlptim)
{
  /* Return LPTIM handle state */
  return hlptim->State;
}

/**
  * @}
  */


/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/

/** @defgroup LPTIM_Private_Functions LPTIM Private Functions
  * @{
  */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
/**
  * @brief  Reset interrupt callbacks to the legacy weak callbacks.
  * @param  lptim pointer to a LPTIM_HandleTypeDef structure that contains
  *                the configuration information for LPTIM module.
  * @retval None
  */
static void LPTIM_ResetCallback(LPTIM_HandleTypeDef *lptim)
{
  /* Reset the LPTIM callback to the legacy weak callbacks */
  lptim->CompareMatchCallback    = HAL_LPTIM_CompareMatchCallback;
  lptim->AutoReloadMatchCallback = HAL_LPTIM_AutoReloadMatchCallback;
  lptim->TriggerCallback         = HAL_LPTIM_TriggerCallback;
  lptim->CompareWriteCallback    = HAL_LPTIM_CompareWriteCallback;
  lptim->AutoReloadWriteCallback = HAL_LPTIM_AutoReloadWriteCallback;
  lptim->DirectionUpCallback     = HAL_LPTIM_DirectionUpCallback;
  lptim->DirectionDownCallback   = HAL_LPTIM_DirectionDownCallback;
  lptim->UpdateEventCallback = HAL_LPTIM_UpdateEventCallback;
  lptim->RepCounterWriteCallback = HAL_LPTIM_RepCounterWriteCallback;
  lptim->UpdateEventHalfCpltCallback = HAL_LPTIM_UpdateEventHalfCpltCallback;
  lptim->IC_CaptureCallback      = HAL_LPTIM_IC_CaptureCallback;
  lptim->IC_CaptureHalfCpltCallback = HAL_LPTIM_IC_CaptureHalfCpltCallback;
  lptim->IC_OverCaptureCallback  = HAL_LPTIM_IC_OverCaptureCallback;
  lptim->ErrorCallback           = HAL_LPTIM_ErrorCallback;
}
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */

/**
  * @brief  LPTimer Wait for flag set
  * @param  hlptim pointer to a LPTIM_HandleTypeDef structure that contains
  *                the configuration information for LPTIM module.
  * @param  flag   The lptim flag
  * @retval HAL status
  */
static HAL_StatusTypeDef LPTIM_WaitForFlag(const LPTIM_HandleTypeDef *hlptim, uint32_t flag)
{
  HAL_StatusTypeDef result = HAL_OK;
  uint32_t count = TIMEOUT * (SystemCoreClock / 20UL / 1000UL);
  do
  {
    count--;
    if (count == 0UL)
    {
      result = HAL_TIMEOUT;
    }
  } while ((!(__HAL_LPTIM_GET_FLAG((hlptim), (flag)))) && (count != 0UL));

  return result;
}

/**
  * @brief  LPTIM DMA error callback
  * @param  hdma pointer to DMA handle.
  * @retval None
  */
void LPTIM_DMAError(DMA_HandleTypeDef *hdma)
{
  LPTIM_HandleTypeDef *hlptim = (LPTIM_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  hlptim->State = HAL_LPTIM_STATE_READY;

#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
  hlptim->ErrorCallback(hlptim);
#else
  HAL_LPTIM_ErrorCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
}

/**
  * @brief  LPTIM DMA Capture complete callback.
  * @param  hdma pointer to DMA handle.
  * @retval None
  */
void LPTIM_DMACaptureCplt(DMA_HandleTypeDef *hdma)
{
  LPTIM_HandleTypeDef *hlptim = (LPTIM_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  hlptim->State = HAL_LPTIM_STATE_READY;

  if (hdma == hlptim->hdma[LPTIM_DMA_ID_CC1])
  {
    hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_1;
  }
  else if (hdma == hlptim->hdma[LPTIM_DMA_ID_CC2])
  {
    hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_2;
  }
  else
  {
    /* nothing to do */
  }

#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
  hlptim->IC_CaptureCallback(hlptim);
#else
  HAL_LPTIM_IC_CaptureCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */

  hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_CLEARED;
}

/**
  * @brief  LPTIM DMA Capture half complete callback.
  * @param  hdma pointer to DMA handle.
  * @retval None
  */
void LPTIM_DMACaptureHalfCplt(DMA_HandleTypeDef *hdma)
{
  LPTIM_HandleTypeDef *hlptim = (LPTIM_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  hlptim->State = HAL_LPTIM_STATE_READY;

  if (hdma == hlptim->hdma[LPTIM_DMA_ID_CC1])
  {
    hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_1;
  }
  else if (hdma == hlptim->hdma[LPTIM_DMA_ID_CC2])
  {
    hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_2;
  }
  else
  {
    /* nothing to do */
  }

#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
  hlptim->IC_CaptureHalfCpltCallback(hlptim);
#else
  HAL_LPTIM_IC_CaptureHalfCpltCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */

  hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_CLEARED;
}

/**
  * @brief  LPTIM DMA Update event complete callback.
  * @param  hdma pointer to DMA handle.
  * @retval None
  */
void LPTIM_DMAUpdateEventCplt(DMA_HandleTypeDef *hdma)
{
  LPTIM_HandleTypeDef *hlptim = (LPTIM_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  hlptim->State = HAL_LPTIM_STATE_READY;

  if (hdma == hlptim->hdma[LPTIM_DMA_ID_CC1])
  {
    hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_1;
  }
  else if (hdma == hlptim->hdma[LPTIM_DMA_ID_CC2])
  {
    hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_2;
  }
  else
  {
    /* nothing to do */
  }

#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
  hlptim->UpdateEventCallback(hlptim);
#else
  HAL_LPTIM_UpdateEventCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */

  hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_CLEARED;
}

/**
  * @brief  LPTIM DMA Capture half complete callback.
  * @param  hdma pointer to DMA handle.
  * @retval None
  */
void LPTIM_DMAUpdateEventHalfCplt(DMA_HandleTypeDef *hdma)
{
  LPTIM_HandleTypeDef *hlptim = (LPTIM_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  hlptim->State = HAL_LPTIM_STATE_READY;

  if (hdma == hlptim->hdma[LPTIM_DMA_ID_CC1])
  {
    hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_1;
  }
  else if (hdma == hlptim->hdma[LPTIM_DMA_ID_CC2])
  {
    hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_2;
  }
  else
  {
    /* nothing to do */
  }

#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
  hlptim->UpdateEventHalfCpltCallback(hlptim);
#else
  HAL_LPTIM_UpdateEventHalfCpltCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */

  hlptim->Channel = HAL_LPTIM_ACTIVE_CHANNEL_CLEARED;
}
/**
  * @brief  LPTimer Output Compare 1 configuration
  * @param  hlptim pointer to a LPTIM_HandleTypeDef structure that contains
  *                the configuration information for LPTIM module.
  * @param  sConfig The output configuration structure
  * @retval None
  */
static HAL_StatusTypeDef LPTIM_OC1_SetConfig(LPTIM_HandleTypeDef *hlptim, const LPTIM_OC_ConfigTypeDef *sConfig)
{
  uint32_t tmpccmr1;
  uint32_t tmpcfgr;

  tmpccmr1 = hlptim->Instance->CCMR1;
  tmpccmr1 &= ~(LPTIM_CCMR1_CC1P_Msk | LPTIM_CCMR1_CC1SEL_Msk);

  if ((hlptim->Instance == LPTIM4) || (hlptim->Instance == LPTIM5))
  {
    tmpcfgr = hlptim->Instance->CFGR;
    tmpcfgr &= ~LPTIM_CFGR_WAVPOL_Msk;
    tmpcfgr |= sConfig->OCPolarity << LPTIM_CFGR_WAVPOL_Pos;

    /* Write to CFGR register */
    hlptim->Instance->CFGR = tmpcfgr;
  }
  else
  {
    tmpccmr1 |= sConfig->OCPolarity << LPTIM_CCMR1_CC1P_Pos;
  }

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_CMP1OK);

  /* Write to CCR1 register */
  __HAL_LPTIM_COMPARE_SET(hlptim, LPTIM_CHANNEL_1, sConfig->Pulse);

  /* Wait for the completion of the write operation to the LPTIM_CCR1 register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_CMP1OK) == HAL_TIMEOUT)
  {
    return HAL_TIMEOUT;
  }

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Write to CCMR1 register */
  hlptim->Instance->CCMR1 = tmpccmr1;

  return HAL_OK;
}

/**
  * @brief  LPTimer Output Compare 2 configuration
  * @param  hlptim pointer to a LPTIM_HandleTypeDef structure that contains
  *                the configuration information for LPTIM module.
  * @param  sConfig The output configuration structure
  * @retval None
  */
static HAL_StatusTypeDef LPTIM_OC2_SetConfig(LPTIM_HandleTypeDef *hlptim, const LPTIM_OC_ConfigTypeDef *sConfig)
{
  uint32_t tmpccmr1;

  tmpccmr1 = hlptim->Instance->CCMR1;
  tmpccmr1 &= ~(LPTIM_CCMR1_CC2P_Msk | LPTIM_CCMR1_CC2SEL_Msk);
  tmpccmr1 |= sConfig->OCPolarity << LPTIM_CCMR1_CC2P_Pos;

  /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

  /* Clear flag */
  __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_CMP2OK);

  /* Write to CCR2 register */
  __HAL_LPTIM_COMPARE_SET(hlptim, LPTIM_CHANNEL_2, sConfig->Pulse);

  /* Wait for the completion of the write operation to the LPTIM_CCR2 register */
  if (LPTIM_WaitForFlag(hlptim, LPTIM_FLAG_CMP2OK) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }

  /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

  /* Write to CCMR1 register */
  hlptim->Instance->CCMR1 = tmpccmr1;

  return HAL_OK;
}

/**
  * @brief  LPTimer Input Capture 1 configuration
  * @param  hlptim pointer to a LPTIM_HandleTypeDef structure that contains
  *                the configuration information for LPTIM module.
  * @param  sConfig The input configuration structure
  * @retval None
  */
static void LPTIM_IC1_SetConfig(LPTIM_HandleTypeDef *hlptim, const LPTIM_IC_ConfigTypeDef *sConfig)
{
  uint32_t tmpccmr1;
  uint32_t tmpcfgr2;

  tmpccmr1 = hlptim->Instance->CCMR1;
  tmpccmr1 &= ~(LPTIM_CCMR1_IC1PSC_Msk | LPTIM_CCMR1_CC1P_Msk | LPTIM_CCMR1_IC1F_Msk);
  tmpccmr1 |= sConfig->ICPrescaler |
              sConfig->ICPolarity |
              sConfig->ICFilter |
              LPTIM_CCMR1_CC1SEL;

  tmpcfgr2 = hlptim->Instance->CFGR2;
  tmpcfgr2 &= ~(LPTIM_CFGR2_IC1SEL_Msk);
  tmpcfgr2 |= sConfig->ICInputSource;

  /* Write to CCMR1 register */
  hlptim->Instance->CCMR1 = tmpccmr1;

  /* Write to CFGR2 register */
  hlptim->Instance->CFGR2 = tmpcfgr2;
}

/**
  * @brief  LPTimer Input Capture 2 configuration
  * @param  hlptim pointer to a LPTIM_HandleTypeDef structure that contains
  *                the configuration information for LPTIM module.
  * @param  sConfig The input configuration structure
  * @retval None
  */
static void LPTIM_IC2_SetConfig(LPTIM_HandleTypeDef *hlptim, const LPTIM_IC_ConfigTypeDef *sConfig)
{
  uint32_t tmpccmr1;
  uint32_t tmpcfgr2;

  tmpccmr1 = hlptim->Instance->CCMR1;
  tmpccmr1 &= ~(LPTIM_CCMR1_IC2PSC_Msk | LPTIM_CCMR1_CC2P_Msk | LPTIM_CCMR1_IC2F_Msk);
  tmpccmr1 |= (sConfig->ICPrescaler << (LPTIM_CCMR1_IC2PSC_Pos - LPTIM_CCMR1_IC1PSC_Pos)) |
              (sConfig->ICPolarity << (LPTIM_CCMR1_CC2P_Pos - LPTIM_CCMR1_CC1P_Pos)) |
              (sConfig->ICFilter << (LPTIM_CCMR1_IC2F_Pos - LPTIM_CCMR1_IC1F_Pos)) |
              LPTIM_CCMR1_CC2SEL;

  tmpcfgr2 = hlptim->Instance->CFGR2;
  tmpcfgr2 &= ~(LPTIM_CFGR2_IC2SEL_Msk);
  tmpcfgr2 |= sConfig->ICInputSource;

  /* Write to CCMR1 register */
  hlptim->Instance->CCMR1 = tmpccmr1;

  /* Write to CFGR2 register */
  hlptim->Instance->CFGR2 = tmpcfgr2;
}

/**
  * @brief  Start the DMA data transfer.
  * @param  hdma DMA handle
  * @param  src The source memory Buffer address.
  * @param  dst The destination memory Buffer address.
  * @param  length The size of a source block transfer in byte.
  * @retval HAL status
  */
HAL_StatusTypeDef LPTIM_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t src, uint32_t dst,
                                     uint32_t length)
{
  HAL_StatusTypeDef status;

  /* Enable the DMA channel */
  if ((hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
  {
    if ((hdma->LinkedListQueue != 0U) && (hdma->LinkedListQueue->Head != 0U))
    {
      /* Enable the DMA channel */
      hdma->LinkedListQueue->Head->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] = length;
      hdma->LinkedListQueue->Head->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET] = (uint32_t)src;
      hdma->LinkedListQueue->Head->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] = (uint32_t)dst;

      status = HAL_DMAEx_List_Start_IT(hdma);
    }
    else
    {
      status = HAL_ERROR;
    }
  }
  else
  {
    status =  HAL_DMA_Start_IT(hdma, src, dst, length);
  }

  return status;
}
/**
  * @}
  */
#endif /* LPTIM1 || LPTIM2 || LPTIM3 || LPTIM4 || LPTIM5 */

#endif /* HAL_LPTIM_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */
