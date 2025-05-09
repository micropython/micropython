/**
  ******************************************************************************
  * @file    stm32n6xx_hal_gfxtim.c
  * @author  MCD Application Team
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the Multi-function Digital Filter (GFXTIM)
  *          peripheral:
  *           + Initialization and de-initialization
  *           + Integrated frame and line clock generation
  *           + One absolute frame counter with one compare channel
  *           + Two auto reload relative frame counter
  *           + One line timer with two compare channel
  *           + External Tearing Effect line management & synchronization
  *           + Four programmable event generators with external trigger generation
  *           + One watchdog counter
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
    *** Initialization and de-initialization ***
    ============================================
    [..]
      (#) User has first to initialize GFXTIM.
      (#) As prerequisite, fill in the HAL_GFXTIM_MspInit() :
        (++) Enable GFXTIM with __HAL_RCC_GFXTIM_CLK_ENABLE
        (++) Enable the clocks for the used GPIOS with __HAL_RCC_GPIOx_CLK_ENABLE().
        (++) Configure these pins in alternate mode using HAL_GPIO_Init().
        (++) If interrupt mode is used, enable and configure GFXTIM
             interrupt with HAL_NVIC_SetPriority() and HAL_NVIC_EnableIRQ().

    [..]
      (#) User can de-initialize GFXTIM with HAL_GFXTIM_DeInit() function.

    *** generic functions ***
    =========================
    [..]
      (#) HAL_GFXTIM_IRQHandler will be called when GFXTIM interrupt occurs.
      (#) HAL_GFXTIM_ErrorCallback will be called when GFXTIM or ADF error occurs.
      (#) Use HAL_GFXTIM_GetState() to get the current GFXTIM or ADF instance state.
      (#) Use HAL_GFXTIM_GetErrorCode() to get the current GFXTIM or ADF instance error code.



#if defined(GENERATOR_CALLBACKS_REGISTERING_AVAILABLE)
  *** Callback registration ***
  =============================================

  The compilation define  USE_HAL_GFXTIM_REGISTER_CALLBACKS when set to 1
  allows the user to configure dynamically the driver callbacks.
  Use Function @ref HAL_GFXTIM_RegisterCallback() to register an interrupt callback.

  Function @ref HAL_GFXTIM_RegisterCallback() registers following callbacks:
      (+) HAL_GFXTIM_AbsoluteTimer_AFCC1Callback
      (+) HAL_GFXTIM_AbsoluteTimer_AFCOFCallback
      (+) HAL_GFXTIM_AbsoluteTimer_ALCC1Callback
      (+) HAL_GFXTIM_AbsoluteTimer_ALCC2Callback
      (+) HAL_GFXTIM_AbsoluteTimer_ALCOFCallback
      (+) HAL_GFXTIM_RelativeTimer_RFC1RCallback
      (+) HAL_GFXTIM_RelativeTimer_RFC2RCallback
      (+) HAL_GFXTIM_TECallback
      (+) HAL_GFXTIM_EventGenerator_EV1Callback
      (+) HAL_GFXTIM_EventGenerator_EV2Callback
      (+) HAL_GFXTIM_EventGenerator_EV3Callback
      (+) HAL_GFXTIM_EventGenerator_EV4Callback
      (+) HAL_GFXTIM_WatchdogTimer_AlarmCallback
      (+) HAL_GFXTIM_WatchdogTimer_PreAlarmCallback
      (+) MspInitCallback
      (+) MspDeInitCallback
  This function takes as parameters the HAL peripheral handle, the Callback ID
  and a pointer to the user callback function.

  Use function @ref HAL_GFXTIM_UnRegisterCallback() to reset a callback to the default
  weak function.
  @ref HAL_GFXTIM_UnRegisterCallback takes as parameters the HAL peripheral handle,
  and the Callback ID.
  This function resets following callbacks:
      (+) HAL_GFXTIM_AbsoluteTimer_AFCC1Callback
      (+) HAL_GFXTIM_AbsoluteTimer_AFCOFCallback
      (+) HAL_GFXTIM_AbsoluteTimer_ALCC1Callback
      (+) HAL_GFXTIM_AbsoluteTimer_ALCC2Callback
      (+) HAL_GFXTIM_AbsoluteTimer_ALCOFCallback
      (+) HAL_GFXTIM_RelativeTimer_RFC1RCallback
      (+) HAL_GFXTIM_RelativeTimer_RFC2RCallback
      (+) HAL_GFXTIM_TECallback
      (+) HAL_GFXTIM_EventGenerator_EV1Callback
      (+) HAL_GFXTIM_EventGenerator_EV2Callback
      (+) HAL_GFXTIM_EventGenerator_EV3Callback
      (+) HAL_GFXTIM_EventGenerator_EV4Callback
      (+) HAL_GFXTIM_WatchdogTimer_AlarmCallback
      (+) HAL_GFXTIM_WatchdogTimer_PreAlarmCallback
      (+) MspInitCallback
      (+) MspDeInitCallback

  By default, after the HAL_GFXTIM_Init() and when the state is HAL_GFXTIM_STATE_RESET,
  all callbacks are set to the corresponding weak functions:
  examples @ref HAL_GFXTIM_ErrorCallback(), @ref HAL_GFXTIM_CalculateCpltCallback().
  Exception done for MspInit and MspDeInit functions that are
  reset to the legacy weak function in the HAL_GFXTIM_Init()/ @ref HAL_GFXTIM_DeInit() only when
  these callbacks are null (not registered beforehand).
  if not, MspInit or MspDeInit are not null, the HAL_GFXTIM_Init()/ @ref HAL_GFXTIM_DeInit()
  keep and use the user MspInit/MspDeInit callbacks (registered beforehand)

  Callbacks can be registered/unregistered in HAL_GFXTIM_STATE_READY state only.
  Exception done MspInit/MspDeInit that can be registered/unregistered
  in HAL_GFXTIM_STATE_READY or HAL_GFXTIM_STATE_RESET state,
  thus registered (user) MspInit/DeInit callbacks can be used during the Init/DeInit.
  In that case first register the MspInit/MspDeInit user callbacks
  using @ref HAL_GFXTIM_RegisterCallback() before calling @ref HAL_GFXTIM_DeInit()
  or HAL_GFXTIM_Init() function.

  When The compilation define USE_HAL_GFXTIM_REGISTER_CALLBACKS is set to 0 or
  not defined, the callback registration feature is not available and all callbacks
  are set to the corresponding weak functions.
#endif

    @endverbatim
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */
#ifdef HAL_GFXTIM_MODULE_ENABLED
#if defined(GFXTIM)
/** @defgroup GFXTIM GFXTIM
  * @brief GFXTIM HAL module driver
  * @{
  */

/* Exported functions ---------------------------------------------------------*/
/** @defgroup GFXTIM_Exported_Functions  GFXTIM Exported Functions
  * @{
  */

/** @defgroup GFXTIM_Exported_Functions_Group1  Initialization and de-initialization functions
  * @brief    Initialization and de-initialization functions
  *
@verbatim
  ==============================================================================
            ##### Initialization and de-initialization functions #####
  ==============================================================================
    [..]  This section provides functions allowing to :
      (+) Initialize the GFXTIM instance.
      (+) De-initialize the GFXTIM instance.
      (+) Register and unregister callbacks.
@endverbatim
  * @{
  */

/**
  * @brief  Initialize the GFXTIM instance according to the specified parameters
  *         in the GFXTIM_InitTypeDef structure and initialize the associated handle.
  * @param  hgfxtim GFXTIM handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_Init(GFXTIM_HandleTypeDef *hgfxtim)
{
  HAL_StatusTypeDef status = HAL_ERROR;

  if (hgfxtim != NULL)
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));
    assert_param(IS_GFXTIM_SYNC_SRC(hgfxtim->Init.SynchroSrc));
    assert_param(IS_GFXTIM_TE_SRC(hgfxtim->Init.TearingEffectSrc));
    assert_param(IS_GFXTIM_TE_POLARITY(hgfxtim->Init.TearingEffectPolarity));
    assert_param(IS_GFXTIM_INTERRUPT(hgfxtim->Init.TearingEffectInterrupt));

    if (hgfxtim->State == HAL_GFXTIM_STATE_RESET)
    {
#if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1)
      /* Reset callback pointers to the weak predefined callbacks */
      hgfxtim->HAL_GFXTIM_AbsoluteTimer_AFCC1Callback = HAL_GFXTIM_AbsoluteTimer_AFCC1Callback;
      hgfxtim->HAL_GFXTIM_AbsoluteTimer_AFCOFCallback = HAL_GFXTIM_AbsoluteTimer_AFCOFCallback;
      hgfxtim->HAL_GFXTIM_AbsoluteTimer_ALCC1Callback = HAL_GFXTIM_AbsoluteTimer_ALCC1Callback;
      hgfxtim->HAL_GFXTIM_AbsoluteTimer_ALCC2Callback = HAL_GFXTIM_AbsoluteTimer_ALCC2Callback;
      hgfxtim->HAL_GFXTIM_AbsoluteTimer_ALCOFCallback = HAL_GFXTIM_AbsoluteTimer_ALCOFCallback;
      hgfxtim->HAL_GFXTIM_RelativeTimer_RFC1RCallback = HAL_GFXTIM_RelativeTimer_RFC1RCallback;
      hgfxtim->HAL_GFXTIM_RelativeTimer_RFC2RCallback = HAL_GFXTIM_RelativeTimer_RFC2RCallback;
      hgfxtim->HAL_GFXTIM_TECallback = HAL_GFXTIM_TECallback;
      hgfxtim->HAL_GFXTIM_EventGenerator_EV1Callback = HAL_GFXTIM_EventGenerator_EV1Callback;
      hgfxtim->HAL_GFXTIM_EventGenerator_EV2Callback = HAL_GFXTIM_EventGenerator_EV2Callback;
      hgfxtim->HAL_GFXTIM_EventGenerator_EV3Callback = HAL_GFXTIM_EventGenerator_EV3Callback;
      hgfxtim->HAL_GFXTIM_EventGenerator_EV4Callback = HAL_GFXTIM_EventGenerator_EV4Callback;
      hgfxtim->HAL_GFXTIM_WatchdogTimer_AlarmCallback = HAL_GFXTIM_WatchdogTimer_AlarmCallback;
      hgfxtim->HAL_GFXTIM_WatchdogTimer_PreAlarmCallback = HAL_GFXTIM_WatchdogTimer_PreAlarmCallback;
      hgfxtim->ErrorCallback = HAL_GFXTIM_ErrorCallback;

      /* Call GFXTIM MSP init function */
      if (hgfxtim->MspInitCallback == NULL)
      {
        hgfxtim->MspInitCallback = HAL_GFXTIM_MspInit;
      }
      hgfxtim->MspInitCallback(hgfxtim);
#else /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */
      /* Call GFXTIM MSP init function */
      HAL_GFXTIM_MspInit(hgfxtim);
#endif /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */

      /* Set Synchronization signals sources (HSYNC and VSYNC), Tearing Effect source and polarity,  */
      MODIFY_REG(hgfxtim->Instance->CR, \
                 GFXTIM_CR_SYNCS | GFXTIM_CR_TES | GFXTIM_CR_TEPOL,
                 hgfxtim->Init.SynchroSrc | hgfxtim->Init.TearingEffectSrc | hgfxtim->Init.TearingEffectPolarity);

      /* Set tearing effect interrupt */
      MODIFY_REG(hgfxtim->Instance->IER, GFXTIM_IER_TEIE,
                 (hgfxtim->Init.TearingEffectInterrupt << GFXTIM_IER_TEIE_Pos));

      /* Update error code and state */
      hgfxtim->ErrorCode = GFXTIM_ERROR_NONE;
      hgfxtim->State = HAL_GFXTIM_STATE_READY;
      status = HAL_OK;
    }
  }

  return status;
}

/**
  * @brief  De-initialize the GFXTIM instance.
  * @param  hgfxtim GFXTIM handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_DeInit(GFXTIM_HandleTypeDef *hgfxtim)
{
  HAL_StatusTypeDef status = HAL_ERROR;

  if (hgfxtim != NULL)
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      /* Call GFXTIM MSP deinit function */
#if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1)
      if (hgfxtim->MspDeInitCallback == NULL)
      {
        hgfxtim->MspDeInitCallback = HAL_GFXTIM_MspDeInit;
      }
      hgfxtim->MspDeInitCallback(hgfxtim);
#else /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */
      HAL_GFXTIM_MspDeInit(hgfxtim);
#endif /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */

      /* Update state */
      hgfxtim->State = HAL_GFXTIM_STATE_RESET;
      status = HAL_OK;
    }
  }

  return status;
}

/**
  * @brief  Initialize the GFXTIM instance MSP.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
__weak void HAL_GFXTIM_MspInit(GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgfxtim);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_GFXTIM_MspInit could be implemented in the user file */
}

/**
  * @brief  De-initialize the GFXTIM instance MSP.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
__weak void HAL_GFXTIM_MspDeInit(GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgfxtim);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_GFXTIM_MspDeInit could be implemented in the user file */
}

#if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register a user GFXTIM callback to be used instead of the weak predefined callback.
  * @param  hgfxtim GFXTIM handle.
  * @param  CallbackID ID of the callback to be registered.
  *         This parameter can be one of the following values:
  *           @arg @ref HAL_GFXTIM_AFC_COMPARE1_CB_ID   Absolute frame counter compare 1 callback ID
  *           @arg @ref HAL_GFXTIM_AFC_OVERFLOW_CB_ID   Absolute frame counter overflow callback ID
  *           @arg @ref HAL_GFXTIM_ALC_COMPARE1_CB_ID   Absolute line counter compare 1 callback ID
  *           @arg @ref HAL_GFXTIM_ALC_COMPARE2_CB_ID   Absolute line counter compare 2 callback ID
  *           @arg @ref HAL_GFXTIM_ALC_OVERFLOW_CB_ID   Absolute line counter overflow callback ID
  *           @arg @ref HAL_GFXTIM_RFC1_RELOAD_CB_ID    Relative frame counter 1 reload callback ID
  *           @arg @ref HAL_GFXTIM_RFC2_RELOAD_CB_ID    Relative frame counter 2 reload callback ID
  *           @arg @ref HAL_GFXTIM_TE_CB_ID             External tearing effect callback ID
  *           @arg @ref HAL_GFXTIM_EVENT1_CB_ID         Event events 1 callback ID
  *           @arg @ref HAL_GFXTIM_EVENT2_CB_ID         Event events 2 callback ID
  *           @arg @ref HAL_GFXTIM_EVENT3_CB_ID         Event events 3 callback ID
  *           @arg @ref HAL_GFXTIM_EVENT4_CB_ID         Event events 4 callback ID
  *           @arg @ref HAL_GFXTIM_WDG_ALARM_CB_ID      Watchdog alarm callback ID
  *           @arg @ref HAL_GFXTIM_WDG_PREALARM_CB_ID   Watchdog pre alarm callback ID
  *           @arg @ref HAL_GFXTIM_ERROR_CB_ID          error callback ID
  *           @arg @ref HAL_GFXTIM_MSP_INIT_CB_ID       MSP initialization user callback ID
  *           @arg @ref HAL_GFXTIM_MSP_DEINIT_CB_ID     MSP de-initialization user callback ID
  * @param  pCallback pointer to the callback function.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_RegisterCallback(GFXTIM_HandleTypeDef *hgfxtim,
                                              HAL_GFXTIM_CallbackIDTypeDef CallbackID,
                                              pGFXTIM_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hgfxtim == NULL)
  {
    status = HAL_ERROR;
  }
  else if (pCallback == NULL)
  {
    /* Update error code and status */
    hgfxtim->ErrorCode |= GFXTIM_ERROR_INVALID_CALLBACK;
    status = HAL_ERROR;
  }
  else
  {
    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      switch (CallbackID)
      {
        case HAL_GFXTIM_AFC_COMPARE1_CB_ID :
          hgfxtim->HAL_GFXTIM_AbsoluteTimer_AFCC1Callback = pCallback;
          break;
        case HAL_GFXTIM_AFC_OVERFLOW_CB_ID :
          hgfxtim->HAL_GFXTIM_AbsoluteTimer_AFCOFCallback = pCallback;
          break;
        case HAL_GFXTIM_ALC_COMPARE1_CB_ID :
          hgfxtim->HAL_GFXTIM_AbsoluteTimer_ALCC1Callback = pCallback;
          break;
        case HAL_GFXTIM_ALC_COMPARE2_CB_ID :
          hgfxtim->HAL_GFXTIM_AbsoluteTimer_ALCC2Callback = pCallback;
          break;
        case HAL_GFXTIM_ALC_OVERFLOW_CB_ID :
          hgfxtim->HAL_GFXTIM_AbsoluteTimer_ALCOFCallback = pCallback;
          break;
        case HAL_GFXTIM_RFC1_RELOAD_CB_ID :
          hgfxtim->HAL_GFXTIM_RelativeTimer_RFC1RCallback = pCallback;
          break;
        case HAL_GFXTIM_RFC2_RELOAD_CB_ID :
          hgfxtim->HAL_GFXTIM_RelativeTimer_RFC2RCallback = pCallback;
          break;
        case HAL_GFXTIM_TE_CB_ID :
          hgfxtim->HAL_GFXTIM_TECallback = pCallback;
          break;
        case HAL_GFXTIM_EVENT1_CB_ID :
          hgfxtim->HAL_GFXTIM_EventGenerator_EV1Callback = pCallback;
          break;
        case HAL_GFXTIM_EVENT2_CB_ID :
          hgfxtim->HAL_GFXTIM_EventGenerator_EV2Callback = pCallback;
          break;
        case HAL_GFXTIM_EVENT3_CB_ID :
          hgfxtim->HAL_GFXTIM_EventGenerator_EV3Callback = pCallback;
          break;
        case HAL_GFXTIM_EVENT4_CB_ID :
          hgfxtim->HAL_GFXTIM_EventGenerator_EV4Callback = pCallback;
          break;
        case HAL_GFXTIM_WDG_ALARM_CB_ID :
          hgfxtim->HAL_GFXTIM_WatchdogTimer_AlarmCallback = pCallback;
          break;
        case HAL_GFXTIM_WDG_PREALARM_CB_ID :
          hgfxtim->HAL_GFXTIM_WatchdogTimer_PreAlarmCallback = pCallback;
          break;
        case HAL_GFXTIM_ERROR_CB_ID :
          hgfxtim->ErrorCallback = pCallback;
          break;
        case HAL_GFXTIM_MSP_INIT_CB_ID :
          hgfxtim->MspInitCallback = pCallback;
          break;
        case HAL_GFXTIM_MSP_DEINIT_CB_ID :
          hgfxtim->MspDeInitCallback = pCallback;
          break;
        default :
          /* Update error code and status */
          hgfxtim->ErrorCode |= GFXTIM_ERROR_INVALID_CALLBACK;
          status = HAL_ERROR;
          break;
      }
    }
    else if (hgfxtim->State == HAL_GFXTIM_STATE_RESET)
    {
      switch (CallbackID)
      {
        case HAL_GFXTIM_MSP_INIT_CB_ID :
          hgfxtim->MspInitCallback = pCallback;
          break;
        case HAL_GFXTIM_MSP_DEINIT_CB_ID :
          hgfxtim->MspDeInitCallback = pCallback;
          break;
        default :
          /* Update error code and status */
          hgfxtim->ErrorCode |= GFXTIM_ERROR_INVALID_CALLBACK;
          status = HAL_ERROR;
          break;
      }
    }
    else
    {
      /* Update error code and status */
      hgfxtim->ErrorCode |= GFXTIM_ERROR_INVALID_CALLBACK;
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  Unregister a user GFXTIM callback.
  *         GFXTIM callback is redirected to the weak predefined callback.
  * @param  hgfxtim GFXTIM handle.
  * @param  CallbackID ID of the callback to be unregistered.
  *         This parameter can be one of the following values:
  *           @arg @ref HAL_GFXTIM_AFC_COMPARE1_CB_ID   Absolute frame counter compare 1 callback ID
  *           @arg @ref HAL_GFXTIM_AFC_OVERFLOW_CB_ID   Absolute frame counter overflow callback ID
  *           @arg @ref HAL_GFXTIM_ALC_COMPARE1_CB_ID   Absolute line counter compare 1 callback ID
  *           @arg @ref HAL_GFXTIM_ALC_COMPARE2_CB_ID   Absolute line counter compare 2 callback ID
  *           @arg @ref HAL_GFXTIM_ALC_OVERFLOW_CB_ID   Absolute line counter overflow callback ID
  *           @arg @ref HAL_GFXTIM_RFC1_RELOAD_CB_ID    Relative frame counter 1 reload callback ID
  *           @arg @ref HAL_GFXTIM_RFC2_RELOAD_CB_ID    Relative frame counter 2 reload callback ID
  *           @arg @ref HAL_GFXTIM_TE_CB_ID             External tearing effect callback ID
  *           @arg @ref HAL_GFXTIM_EVENT1_CB_ID         Event events 1 callback ID
  *           @arg @ref HAL_GFXTIM_EVENT2_CB_ID         Event events 2 callback ID
  *           @arg @ref HAL_GFXTIM_EVENT3_CB_ID         Event events 3 callback ID
  *           @arg @ref HAL_GFXTIM_EVENT4_CB_ID         Event events 4 callback ID
  *           @arg @ref HAL_GFXTIM_WDG_ALARM_CB_ID      Watchdog alarm callback ID
  *           @arg @ref HAL_GFXTIM_WDG_PREALARM_CB_ID   Watchdog pre alarm callback ID
  *           @arg @ref HAL_GFXTIM_ERROR_CB_ID          error callback ID
  *           @arg @ref HAL_GFXTIM_MSP_INIT_CB_ID       MSP initialization user callback ID
  *           @arg @ref HAL_GFXTIM_MSP_DEINIT_CB_ID     MSP de-initialization user callback ID
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_UnRegisterCallback(GFXTIM_HandleTypeDef *hgfxtim,
                                                HAL_GFXTIM_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hgfxtim == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      switch (CallbackID)
      {
        case HAL_GFXTIM_AFC_COMPARE1_CB_ID :
          hgfxtim->HAL_GFXTIM_AbsoluteTimer_AFCC1Callback = HAL_GFXTIM_AbsoluteTimer_AFCC1Callback;
          break;
        case HAL_GFXTIM_AFC_OVERFLOW_CB_ID :
          hgfxtim->HAL_GFXTIM_AbsoluteTimer_AFCOFCallback = HAL_GFXTIM_AbsoluteTimer_AFCOFCallback;
          break;
        case HAL_GFXTIM_ALC_COMPARE1_CB_ID :
          hgfxtim->HAL_GFXTIM_AbsoluteTimer_ALCC1Callback = HAL_GFXTIM_AbsoluteTimer_ALCC1Callback;
          break;
        case HAL_GFXTIM_ALC_COMPARE2_CB_ID :
          hgfxtim->HAL_GFXTIM_AbsoluteTimer_ALCC2Callback = HAL_GFXTIM_AbsoluteTimer_ALCC2Callback;
          break;
        case HAL_GFXTIM_ALC_OVERFLOW_CB_ID :
          hgfxtim->HAL_GFXTIM_AbsoluteTimer_ALCOFCallback = HAL_GFXTIM_AbsoluteTimer_ALCOFCallback;
          break;
        case HAL_GFXTIM_RFC1_RELOAD_CB_ID :
          hgfxtim->HAL_GFXTIM_RelativeTimer_RFC1RCallback = HAL_GFXTIM_RelativeTimer_RFC1RCallback;
          break;
        case HAL_GFXTIM_RFC2_RELOAD_CB_ID :
          hgfxtim->HAL_GFXTIM_RelativeTimer_RFC2RCallback = HAL_GFXTIM_RelativeTimer_RFC2RCallback;
          break;
        case HAL_GFXTIM_TE_CB_ID :
          hgfxtim->HAL_GFXTIM_TECallback = HAL_GFXTIM_TECallback;
          break;
        case HAL_GFXTIM_EVENT1_CB_ID :
          hgfxtim->HAL_GFXTIM_EventGenerator_EV1Callback = HAL_GFXTIM_EventGenerator_EV1Callback;
          break;
        case HAL_GFXTIM_EVENT2_CB_ID :
          hgfxtim->HAL_GFXTIM_EventGenerator_EV2Callback = HAL_GFXTIM_EventGenerator_EV2Callback;
          break;
        case HAL_GFXTIM_EVENT3_CB_ID :
          hgfxtim->HAL_GFXTIM_EventGenerator_EV3Callback = HAL_GFXTIM_EventGenerator_EV3Callback;
          break;
        case HAL_GFXTIM_EVENT4_CB_ID :
          hgfxtim->HAL_GFXTIM_EventGenerator_EV4Callback = HAL_GFXTIM_EventGenerator_EV4Callback;
          break;
        case HAL_GFXTIM_WDG_ALARM_CB_ID :
          hgfxtim->HAL_GFXTIM_WatchdogTimer_AlarmCallback = HAL_GFXTIM_WatchdogTimer_AlarmCallback;
          break;
        case HAL_GFXTIM_WDG_PREALARM_CB_ID :
          hgfxtim->HAL_GFXTIM_WatchdogTimer_PreAlarmCallback = HAL_GFXTIM_WatchdogTimer_PreAlarmCallback;
          break;
        case HAL_GFXTIM_ERROR_CB_ID :
          hgfxtim->ErrorCallback = HAL_GFXTIM_ErrorCallback;
          break;
        case HAL_GFXTIM_MSP_INIT_CB_ID :
          hgfxtim->MspInitCallback = HAL_GFXTIM_MspInit;
          break;
        case HAL_GFXTIM_MSP_DEINIT_CB_ID :
          hgfxtim->MspDeInitCallback = HAL_GFXTIM_MspDeInit;
          break;
        default :
          /* Update error code and status */
          hgfxtim->ErrorCode |= GFXTIM_ERROR_INVALID_CALLBACK;
          status = HAL_ERROR;
          break;
      }
    }
    else if (hgfxtim->State == HAL_GFXTIM_STATE_RESET)
    {
      switch (CallbackID)
      {
        case HAL_GFXTIM_MSP_INIT_CB_ID :
          hgfxtim->MspInitCallback = HAL_GFXTIM_MspInit;
          break;
        case HAL_GFXTIM_MSP_DEINIT_CB_ID :
          hgfxtim->MspDeInitCallback = HAL_GFXTIM_MspDeInit;
          break;
        default :
          /* Update error code and status */
          hgfxtim->ErrorCode |= GFXTIM_ERROR_INVALID_CALLBACK;
          status = HAL_ERROR;
          break;
      }
    }
    else
    {
      /* Update error code and status */
      hgfxtim->ErrorCode |= GFXTIM_ERROR_INVALID_CALLBACK;
      status = HAL_ERROR;
    }
  }

  return status;
}
#endif /* #if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1) */

/**
  * @}
  */

/** @defgroup GFXTIM_Exported_Functions_Group2  Clock Generator functions
  * @brief    Clock Generator functions
  *
@verbatim
  ==============================================================================
                        ##### Clock Generator functions #####
  ==============================================================================
    [..]  This section provides functions allowing to :
      (+) Configure the clock generator.
      (+) Force reload of FCC and LCC.

@endverbatim
  * @{
  */

/**
  * @brief  This function configures the clock generator.
  * @param  hgfxtim GFXTIM handle.
  * @param  pClockGeneratorConfig Clock Generator configuration parameters.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_ClockGenerator_Config(GFXTIM_HandleTypeDef *hgfxtim,
                                                   const GFXTIM_ClockGeneratorConfigTypeDef *pClockGeneratorConfig)
{
  HAL_StatusTypeDef status = HAL_OK;

  if ((hgfxtim == NULL) || (pClockGeneratorConfig == NULL))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));
    assert_param(IS_GFXTIM_LCC_HW_RELOAD_SRC(pClockGeneratorConfig->LCCHwReloadSrc));
    assert_param(IS_GFXTIM_LCC_RELOAD_VALUE(pClockGeneratorConfig->LCCReloadValue));
    assert_param(IS_GFXTIM_LCC_CLK_SRC(pClockGeneratorConfig->LCCClockSrc));
    assert_param(IS_GFXTIM_LINE_CLK_SRC(pClockGeneratorConfig->LineClockSrc));
    assert_param(IS_GFXTIM_FCC_HW_RELOAD_SRC(pClockGeneratorConfig->FCCHwReloadSrc));
    assert_param(IS_GFXTIM_FCC_RELOAD_VALUE(pClockGeneratorConfig->FCCReloadValue));
    assert_param(IS_GFXTIM_FCC_CLK_SRC(pClockGeneratorConfig->FCCClockSrc));
    assert_param(IS_GFXTIM_FRAME_CLK_SRC(pClockGeneratorConfig->FrameClockSrc));
    assert_param(IS_GFXTIM_LINE_CLK_CALIB(pClockGeneratorConfig->LineClockCalib));
    assert_param(IS_GFXTIM_FRAME_CLK_CALIB(pClockGeneratorConfig->FrameClockCalib));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      /* Disable FCC and LCC */
      CLEAR_BIT(hgfxtim->Instance->CGCR,
                GFXTIM_CGCR_LCCCS | GFXTIM_CGCR_FCCCS);

      /* Set Line Clock Counter (LCC) reload value (22 bits) */
      MODIFY_REG(hgfxtim->Instance->LCCRR, GFXTIM_LCCRR_RELOAD,
                 (pClockGeneratorConfig->LCCReloadValue << GFXTIM_LCCRR_RELOAD_Pos));

      /* Set Frame Clock Counter (FCC) reload value (12 bits) */
      MODIFY_REG(hgfxtim->Instance->FCCRR, GFXTIM_FCCRR_RELOAD,
                 (pClockGeneratorConfig->FCCReloadValue << GFXTIM_FCCRR_RELOAD_Pos));

      /* Set line and frame config */
      MODIFY_REG(hgfxtim->Instance->CGCR,
                 GFXTIM_CGCR_LCCHRS | GFXTIM_CGCR_LCCCS | GFXTIM_CGCR_LCS |
                 GFXTIM_CGCR_FCCHRS | GFXTIM_CGCR_FCCCS | GFXTIM_CGCR_FCS,
                 pClockGeneratorConfig->LCCHwReloadSrc | pClockGeneratorConfig->LCCClockSrc |
                 pClockGeneratorConfig->LineClockSrc | pClockGeneratorConfig->FCCHwReloadSrc |
                 pClockGeneratorConfig->FCCClockSrc | pClockGeneratorConfig->FrameClockSrc);

      /* Set debug output config for Line and frame clocks */
      MODIFY_REG(hgfxtim->Instance->CR,
                 GFXTIM_CR_LCCOE | GFXTIM_CR_FCCOE,
                 pClockGeneratorConfig->LineClockCalib | pClockGeneratorConfig->FrameClockCalib);
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  This function force clock generator counter(s) reload
  * @param  hgfxtim GFXTIM handle.
  * @param  ClockGeneratorCounter Clock Generator counter
  *          This parameter can be a value of @ref GFXTIM_ClockGeneratorCounter.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_ClockGenerator_Reload(GFXTIM_HandleTypeDef *hgfxtim, uint32_t ClockGeneratorCounter)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hgfxtim == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));
    assert_param(IS_GFXTIM_CLOCK_GENERATOR_COUNTER(ClockGeneratorCounter));


    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      SET_BIT(hgfxtim->Instance->CGCR, ClockGeneratorCounter);
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}


/**
  * @brief  GFXTIM Tearing effect callback.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
__weak void HAL_GFXTIM_TECallback(GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgfxtim);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_GFXTIM_TECallback could be implemented in the user file */
}

/**
  * @}
  */

/** @defgroup GFXTIM_Exported_Functions_Group3 Absolute Timer functions
  * @brief    Absolute Timer functions
  *
@verbatim
  ==============================================================================
                        ##### Absolute Timers functions #####
  ==============================================================================
    [..]  This section provides functions allowing to :
      (+) Configure the absolute timer.
      (+) Start the absolute timer.
      (+) Stop the absolute timer.
      (+) Reset the absolute timer counters.
      (+) Get the absolute time value.
      (+) Set the absolute frame compare value.
      (+) Set the absolute line compare value.
@endverbatim
  * @{
  */

/**
  * @brief  This function configures an absolute Timer.
  * @param  hgfxtim GFXTIM handle.
  * @param  pAbsoluteTimerConfig pointer to a GFXTIM_AbsoluteTimerConfigTypeDef structure that
  *         contains absoluite timer comparators and counters values.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_AbsoluteTimer_Config(GFXTIM_HandleTypeDef *hgfxtim,
                                                  const GFXTIM_AbsoluteTimerConfigTypeDef *pAbsoluteTimerConfig)
{
  HAL_StatusTypeDef status = HAL_OK;

  if ((hgfxtim == NULL) || (pAbsoluteTimerConfig == NULL))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));
    assert_param(IS_GFXTIM_ABSOLUTE_FRAME_VALUE(pAbsoluteTimerConfig->FrameCompare1Value));
    assert_param(IS_GFXTIM_ABSOLUTE_FRAME_VALUE(pAbsoluteTimerConfig->FrameCounterValue));
    assert_param(IS_GFXTIM_INTERRUPT(pAbsoluteTimerConfig->FrameOverflowInterrupt));
    assert_param(IS_GFXTIM_INTERRUPT(pAbsoluteTimerConfig->FrameCompare1Interrupt));
    assert_param(IS_GFXTIM_ABSOLUTE_LINE_VALUE(pAbsoluteTimerConfig->LineCompare1Value));
    assert_param(IS_GFXTIM_ABSOLUTE_LINE_VALUE(pAbsoluteTimerConfig->LineCompare2Value));
    assert_param(IS_GFXTIM_ABSOLUTE_LINE_VALUE(pAbsoluteTimerConfig->LineCounterValue));
    assert_param(IS_GFXTIM_INTERRUPT(pAbsoluteTimerConfig->LineOverflowInterrupt));
    assert_param(IS_GFXTIM_INTERRUPT(pAbsoluteTimerConfig->LineCompare1Interrupt));
    assert_param(IS_GFXTIM_INTERRUPT(pAbsoluteTimerConfig->LineCompare2Interrupt));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {

      /* Set AFC compare 1 value */
      MODIFY_REG(hgfxtim->Instance->AFCC1R, GFXTIM_AFCC1R_FRAME,
                 (pAbsoluteTimerConfig->FrameCompare1Value << GFXTIM_AFCC1R_FRAME_Pos));

      /* Set AFC counter value */
      MODIFY_REG(hgfxtim->Instance->AFCR, GFXTIM_AFCR_FRAME,
                 (pAbsoluteTimerConfig->FrameCounterValue << GFXTIM_AFCR_FRAME_Pos));

      /* Set ALC compare 1 value */
      MODIFY_REG(hgfxtim->Instance->ALCC1R, GFXTIM_ALCC1R_LINE,
                 (pAbsoluteTimerConfig->LineCompare1Value << GFXTIM_ALCC1R_LINE_Pos));

      /* Set ALC compare 2 value */
      MODIFY_REG(hgfxtim->Instance->ALCC2R, GFXTIM_ALCC2R_LINE,
                 (pAbsoluteTimerConfig->LineCompare2Value << GFXTIM_ALCC2R_LINE_Pos));

      /* Set ALC counter value */
      MODIFY_REG(hgfxtim->Instance->ALCR, GFXTIM_ALCR_LINE,
                 (pAbsoluteTimerConfig->LineCounterValue << GFXTIM_ALCR_LINE_Pos));

      /* Set ALC compare 1, compare 2, overflow interrupts, AFC compare 1 and overflow interrupts */
      MODIFY_REG(hgfxtim->Instance->IER,
                 GFXTIM_IER_ALCC1IE | GFXTIM_IER_ALCC2IE | GFXTIM_IER_ALCOIE | GFXTIM_IER_AFCC1IE | GFXTIM_IER_AFCOIE,
                 (pAbsoluteTimerConfig->FrameOverflowInterrupt << GFXTIM_IER_AFCOIE_Pos) |
                 (pAbsoluteTimerConfig->FrameCompare1Interrupt << GFXTIM_IER_AFCC1IE_Pos) |
                 (pAbsoluteTimerConfig->LineOverflowInterrupt << GFXTIM_IER_ALCOIE_Pos) |
                 (pAbsoluteTimerConfig->LineCompare1Interrupt << GFXTIM_IER_ALCC1IE_Pos) |
                 (pAbsoluteTimerConfig->LineCompare2Interrupt << GFXTIM_IER_ALCC2IE_Pos));
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function starts absolute timer.
  * @param  hgfxtim GFXTIM handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_AbsoluteTimer_Start(GFXTIM_HandleTypeDef *hgfxtim)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hgfxtim == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      /* Enable absolute Timer */
      SET_BIT(hgfxtim->Instance->TCR, (GFXTIM_TCR_AFCEN | GFXTIM_TCR_ALCEN));
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function stops absolute timer counter(s).
  * @param  hgfxtim GFXTIM handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_AbsoluteTimer_Stop(GFXTIM_HandleTypeDef *hgfxtim)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hgfxtim == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      /* Disable absolute counter(s) */
      SET_BIT(hgfxtim->Instance->TDR, (GFXTIM_TDR_ALCDIS | GFXTIM_TDR_AFCDIS));
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function resets absolute timer counters.
  * @param  hgfxtim GFXTIM handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_AbsoluteTimer_Reset(GFXTIM_HandleTypeDef *hgfxtim)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hgfxtim == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      /* Disable absolute counter(s) */
      SET_BIT(hgfxtim->Instance->TCR, (GFXTIM_TCR_FAFCR | GFXTIM_TCR_FALCR));
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function gets absolute timer value.
  * @param  hgfxtim GFXTIM handle.
  * @param  AbsoluteTime absolute time
  *         This parameter can be a value of @ref GFXTIM_AbsoluteTime.
  * @param  pValue Absolute time value
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_AbsoluteTimer_GetCounter(const GFXTIM_HandleTypeDef *hgfxtim, uint32_t AbsoluteTime,
                                                      uint32_t *pValue)
{
  HAL_StatusTypeDef status = HAL_OK;

  if ((hgfxtim == NULL) || (pValue == NULL))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));
    assert_param(IS_GFXTIM_ABSOLUTE_TIME(AbsoluteTime));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      switch (AbsoluteTime)
      {
        case GFXTIM_ABSOLUTE_GLOBAL_TIME:
          *pValue = READ_REG(hgfxtim->Instance->ATR);
          break;
        case GFXTIM_ABSOLUTE_FRAME_TIME:
          *pValue = READ_REG(hgfxtim->Instance->AFCR);
          break;
        default:
          /* GFXTIM_ABSOLUTE_LINE_TIME */
          *pValue = READ_REG(hgfxtim->Instance->ALCR);
          break;
      }
    }
    else
    {
      status = HAL_ERROR;
    }
  }
  return status;
}

/**
  * @brief  This function sets absolute frame compare value.
  * @param  hgfxtim GFXTIM handle.
  * @param  Value Absolute frame compare 1 value
  *         This parameter can be a number between Min_Data = 0x00000 and Max_Data = 0xFFFFF
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_AbsoluteTimer_SetFrameCompare(GFXTIM_HandleTypeDef *hgfxtim, uint32_t Value)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hgfxtim == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      /* Set absolute frame counter compare 1 register value */
      MODIFY_REG(hgfxtim->Instance->AFCC1R, GFXTIM_AFCC1R_FRAME,
                 (Value << GFXTIM_AFCC1R_FRAME_Pos));
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  This function sets line compare value.
  * @param  hgfxtim GFXTIM handle.
  * @param  AbsoluteLineComparator Absolute line compare value
  *         This parameter can be a value of @ref GFXTIM_AbsoluteLineComparator.
  * @param  Value Absolute line compare value
  *         This parameter can be a number between Min_Data = 0x000 and Max_Data = 0xFFF
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_AbsoluteTimer_SetLineCompare(GFXTIM_HandleTypeDef *hgfxtim,
                                                          uint32_t AbsoluteLineComparator, uint32_t Value)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hgfxtim == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));
    assert_param(IS_GFXTIM_ABSOLUTE_LINE_COMPARATOR(AbsoluteLineComparator));
    assert_param(IS_GFXTIM_ABSOLUTE_LINE_VALUE(Value));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      switch (AbsoluteLineComparator)
      {
        case GFXTIM_ABSOLUTE_LINE_COMPARE1:
          WRITE_REG(hgfxtim->Instance->ALCC1R, Value);
          break;
        default:
          /* GFXTIM_ABSOLUTE_LINE_COMPARE2 */
          WRITE_REG(hgfxtim->Instance->ALCC2R, Value);
          break;
      }
    }
    else
    {
      status = HAL_ERROR;
    }
  }
  return status;
}

/**
  * @brief  GFXTIM Absolute frame counter overflow callback.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
__weak void HAL_GFXTIM_AbsoluteTimer_AFCOFCallback(GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgfxtim);
  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_GFXTIM_AbsoluteTimer_AFCOFCallback could be implemented in the user file */
}

/**
  * @brief  GFXTIM Absolute frame counter compare 1 callback.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
__weak void HAL_GFXTIM_AbsoluteTimer_AFCC1Callback(GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgfxtim);
  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_GFXTIM_AbsoluteTimer_AFCC1Callback could be implemented in the user file */
}

/**
  * @brief  GFXTIM Absolute line counter compare 1 callback.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
__weak void HAL_GFXTIM_AbsoluteTimer_ALCC1Callback(GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgfxtim);
  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_GFXTIM_AbsoluteTimer_ALCC1Callback could be implemented in the user file */
}

/**
  * @brief  GFXTIM Absolute line counter compare 2 callback.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
__weak void HAL_GFXTIM_AbsoluteTimer_ALCC2Callback(GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgfxtim);
  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_GFXTIM_AbsoluteTimer_ALCC2Callback could be implemented in the user file */
}

/**
  * @brief  GFXTIM Absolute line counter overflow callback.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
__weak void HAL_GFXTIM_AbsoluteTimer_ALCOFCallback(GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgfxtim);
  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_GFXTIM_AbsoluteTimer_ALCOFCallback could be implemented in the user file */
}

/**
  * @}
  */

/** @defgroup GFXTIM_Exported_Functions_Group4 Relative Timer functions
  * @brief    Clock Generator functions
  *
@verbatim
  ==============================================================================
                        ##### Relative Timer functions #####
  ==============================================================================
    [..]  This section provides functions allowing to :
      (+) Configure a relative timer.
      (+) Start a relative timer counter.
      (+) Stop a relative timer counter.
      (+) Force a relative timer counter reload.
      (+) Set a relative timer reload value.
      (+) Get a relative timer counter value.

@endverbatim
  * @{
  */

/**
  * @brief  This function configures a Relative Timer.
  * @param  hgfxtim GFXTIM handle.
  * @param  pRelativeTimerConfig pointer to a GFXTIM_RelativeTimerConfigTypeDef structure that
  *         contains relative timer comparators and counters values.
  * @param  RelativeTimer Relative Timer identifier
  *         This parameter can be a value of @ref GFXTIM_RelativeTimer.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_RelativeTimer_Config(GFXTIM_HandleTypeDef *hgfxtim,
                                                  const GFXTIM_RelativeTimerConfigTypeDef *pRelativeTimerConfig,
                                                  uint32_t RelativeTimer)
{
  HAL_StatusTypeDef status = HAL_OK;

  if ((hgfxtim == NULL) || (pRelativeTimerConfig == NULL))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));
    assert_param(IS_GFXTIM_RELATIVE_FRAME_VALUE(pRelativeTimerConfig->AutoReloadValue));
    assert_param(IS_GFXTIM_RELATIVE_FRAME_VALUE(pRelativeTimerConfig->CounterMode));
    assert_param(IS_GFXTIM_INTERRUPT(pRelativeTimerConfig->ReloadInterrupt));
    assert_param(IS_GFXTIM_RELATIVE_TIMER(RelativeTimer));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      if (RelativeTimer == GFXTIM_RELATIVE_TIMER1)
      {
        /* Set RFC1 auto reload */
        MODIFY_REG(hgfxtim->Instance->RFC1RR, GFXTIM_RFC1RR_FRAME,
                   pRelativeTimerConfig->AutoReloadValue << GFXTIM_RFC1RR_FRAME_Pos);

        /* Set relative timer mode */
        MODIFY_REG(hgfxtim->Instance->TCR, GFXTIM_TCR_RFC1CM,
                   (pRelativeTimerConfig->CounterMode << GFXTIM_TCR_RFC1CM_Pos));

        /* Set relative timer 1 interrupt */
        MODIFY_REG(hgfxtim->Instance->IER, GFXTIM_IER_RFC1RIE_Msk,
                   (pRelativeTimerConfig->ReloadInterrupt << GFXTIM_IER_RFC1RIE_Pos));
      }
      else
      {
        /* Set RFC2 auto reload */
        MODIFY_REG(hgfxtim->Instance->RFC2RR, GFXTIM_RFC2RR_FRAME,
                   pRelativeTimerConfig->AutoReloadValue << GFXTIM_RFC2RR_FRAME_Pos);

        /* Set relative timer mode */
        MODIFY_REG(hgfxtim->Instance->TCR, GFXTIM_TCR_RFC2CM,
                   (pRelativeTimerConfig->CounterMode << GFXTIM_TCR_RFC2CM_Pos));

        /* Set relative timer 2 interrupt */
        MODIFY_REG(hgfxtim->Instance->IER, GFXTIM_IER_RFC2RIE_Msk,
                   (pRelativeTimerConfig->ReloadInterrupt << GFXTIM_IER_RFC2RIE_Pos));
      }
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  This function starts a relative Timer.
  * @param  hgfxtim GFXTIM handle.
  * @param  RelativeTimer Relative timer counter to Enable
  *         This parameter can be a value of @ref GFXTIM_RelativeTimer.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_RelativeTimer_Start(GFXTIM_HandleTypeDef *hgfxtim, uint32_t RelativeTimer)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hgfxtim == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));
    assert_param(IS_GFXTIM_RELATIVE_TIMER(RelativeTimer));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      if (RelativeTimer == GFXTIM_RELATIVE_TIMER1)
      {
        /* Enable relative timer 1 */
        SET_BIT(hgfxtim->Instance->TCR, GFXTIM_TCR_RFC1EN);
      }
      else
      {
        /* Enable relative timer 2 */
        SET_BIT(hgfxtim->Instance->TCR, GFXTIM_TCR_RFC2EN);
      }
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  This function stops a relative Timer counter.
  * @param  hgfxtim GFXTIM handle.
  * @param  RelativeTimer Relative timer counter to Disable
  *         This parameter can be a value of @ref GFXTIM_RelativeTimer.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_RelativeTimer_Stop(GFXTIM_HandleTypeDef *hgfxtim, uint32_t RelativeTimer)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hgfxtim == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));
    assert_param(IS_GFXTIM_RELATIVE_TIMER(RelativeTimer));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      if (RelativeTimer == GFXTIM_RELATIVE_TIMER1)
      {
        /* Disable relative timer 1 */
        SET_BIT(hgfxtim->Instance->TDR, GFXTIM_TDR_RFC1DIS);
      }
      else
      {
        /* Disable relative timer 2 */
        SET_BIT(hgfxtim->Instance->TDR, GFXTIM_TDR_RFC2DIS);
      }
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}


/**
  * @brief  This function force a relative Timer reload.
  * @param  hgfxtim GFXTIM handle.
  * @param  RelativeTimer Relative timer to Foce Reload
  *           This parameter can be a value of @ref GFXTIM_RelativeTimer.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_RelativeTimer_ForceReload(GFXTIM_HandleTypeDef *hgfxtim, uint32_t RelativeTimer)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hgfxtim == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));
    assert_param(IS_GFXTIM_RELATIVE_TIMER(RelativeTimer));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      if (RelativeTimer == GFXTIM_RELATIVE_TIMER1)
      {
        /* Force relative timer 1 reload */
        SET_BIT(hgfxtim->Instance->TCR, GFXTIM_TCR_FRFC1R);
      }
      else
      {
        /* Force relative timer 2 reload*/
        SET_BIT(hgfxtim->Instance->TCR, GFXTIM_TCR_FRFC2R);
      }
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}


/**
  * @brief  This function sets Relative frame timer reload value.
  * @param  hgfxtim GFXTIM handle.
  * @param  RelativeTimer specifies the Auto-reload register to be modified.
  *         This parameter can be a value of @ref GFXTIM_RelativeTimer.
  * @param  Value Reload value
  *         This parameter can be a number between Min_Data = 0x000 and Max_Data = 0xFFF
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_RelativeTimer_SetReload(GFXTIM_HandleTypeDef *hgfxtim, uint32_t RelativeTimer,
                                                     uint32_t Value)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hgfxtim == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));
    assert_param(IS_GFXTIM_RELATIVE_FRAME_VALUE(Value));
    assert_param(IS_GFXTIM_RELATIVE_TIMER(RelativeTimer));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      if (RelativeTimer == GFXTIM_RELATIVE_TIMER1)
      {
        /* Set RFC1 auto reload */
        WRITE_REG(hgfxtim->Instance->RFC1RR, Value);
      }
      else
      {
        /* Set RFC2 auto reload */
        WRITE_REG(hgfxtim->Instance->RFC2RR, Value);
      }
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  This function sets Relative frame timer compare value.
  * @param  hgfxtim GFXTIM handle.
  * @param  RelativeTimer Relative frame counter reload
  *         This parameter can be a value of @ref GFXTIM_RelativeTimer.
  * @param  pValue pointer to a relative frame counter value
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_RelativeTimer_GetCounter(const GFXTIM_HandleTypeDef *hgfxtim, uint32_t RelativeTimer,
                                                      uint32_t *pValue)
{
  HAL_StatusTypeDef status = HAL_OK;

  if ((hgfxtim == NULL) || (pValue == NULL))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));
    assert_param(IS_GFXTIM_RELATIVE_TIMER(RelativeTimer));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      if (RelativeTimer == GFXTIM_RELATIVE_TIMER1)
      {
        /* Set RFC1 auto reload */
        *pValue = READ_REG(hgfxtim->Instance->RFC1R);
      }
      else
      {
        /* Set RFC2 auto reload */
        *pValue = READ_REG(hgfxtim->Instance->RFC2R);
      }
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}




/**
  * @brief  GFXTIM Relative frame counter 1 reload callback.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
__weak void HAL_GFXTIM_RelativeTimer_RFC1RCallback(GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgfxtim);
  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_GFXTIM_RelativeTimer_RFC1RCallback could be implemented in the user file */
}

/**
  * @brief  GFXTIM Relative frame counter 2 reload callback.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
__weak void HAL_GFXTIM_RelativeTimer_RFC2RCallback(GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgfxtim);
  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_GFXTIM_RelativeTimer_RFC2RCallback could be implemented in the user file */
}

/**
  * @}
  */

/** @defgroup GFXTIM_Exported_Functions_Group5 Event Generator functions
  * @brief    Event Generator functions
  *
@verbatim
  ==============================================================================
                        ##### Event Generator functions #####
  ==============================================================================
    [..]  This section provides functions allowing to :
      (+) Configure an Event Generator.
      (+) Enable an Event Generator.
      (+) Disable an Event Generator.
@endverbatim
  * @{
  */

/**
  * @brief  This function configures an Event Generator.
  * @param  hgfxtim GFXTIM handle.
  * @param  EventGenerator Event Generator
  *           This parameter can be a value of @ref GFXTIM_EventGenerator.
  * @param  pEventGeneratorConfig pointer to a GFXTIM_EventGeneratorConfigTypeDef structure that
  *         contains Event Generator configuration parameters.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_EventGenerator_Config(GFXTIM_HandleTypeDef *hgfxtim, uint32_t EventGenerator,
                                                   const GFXTIM_EventGeneratorConfigTypeDef *pEventGeneratorConfig)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t shift;

  if ((hgfxtim == NULL) || (pEventGeneratorConfig == NULL))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_EVENT_GENERATOR(EventGenerator));
    assert_param(IS_GFXTIM_EVENT_LINE(pEventGeneratorConfig->LineEvent));
    assert_param(IS_GFXTIM_EVENT_FRAME(pEventGeneratorConfig->FrameEvent));
    assert_param(IS_GFXTIM_INTERRUPT(pEventGeneratorConfig->EventInterrupt));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      /* Select frame and line events */
      shift = (EventGenerator) * 8U;
      MODIFY_REG(hgfxtim->Instance->EVSR, \
                 ((GFXTIM_EVSR_LES1 | GFXTIM_EVSR_FES1) << shift),
                 ((pEventGeneratorConfig->LineEvent | pEventGeneratorConfig->FrameEvent) << shift));

      /* Event interrupt */
      MODIFY_REG(hgfxtim->Instance->IER, \
                 (GFXTIM_IER_EV1IE << (EventGenerator)), \
                 (pEventGeneratorConfig->EventInterrupt << (EventGenerator + GFXTIM_IER_EV1IE_Pos)));
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  This function enables an Event Generator.
  * @param  hgfxtim GFXTIM handle.
  * @param  EventGenerator Event Generator
  *           This parameter can be a value of @ref GFXTIM_EventGenerator.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_EventGenerator_Enable(GFXTIM_HandleTypeDef *hgfxtim, uint32_t EventGenerator)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hgfxtim == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_EVENT_GENERATOR(EventGenerator));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      /* Enable event generator */
      SET_BIT(hgfxtim->Instance->EVCR, GFXTIM_EVCR_EV1EN << EventGenerator);
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  This function disables an Event Generator.
  * @param  hgfxtim GFXTIM handle.
  * @param  EventGenerator Event Generator
  *           This parameter can be a value of @ref GFXTIM_EventGenerator.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_EventGenerator_Disable(GFXTIM_HandleTypeDef *hgfxtim, uint32_t EventGenerator)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hgfxtim == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_EVENT_GENERATOR(EventGenerator));

    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      /* Disable event generator */
      CLEAR_BIT(hgfxtim->Instance->EVCR, GFXTIM_EVCR_EV1EN << EventGenerator);
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}



/**
  * @brief  GFXTIM Combined events 1 callback.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
__weak void HAL_GFXTIM_EventGenerator_EV1Callback(GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgfxtim);
  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_GFXTIM_EventGenerator_EV1Callback could be implemented in the user file */
}

/**
  * @brief  GFXTIM Combined events 2 callback.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
__weak void HAL_GFXTIM_EventGenerator_EV2Callback(GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgfxtim);
  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_GFXTIM_EventGenerator_EV2Callback could be implemented in the user file */
}

/**
  * @brief  GFXTIM Combined events 3 callback.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
__weak void HAL_GFXTIM_EventGenerator_EV3Callback(GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgfxtim);
  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_GFXTIM_EventGenerator_EV3Callback could be implemented in the user file */
}

/**
  * @brief  GFXTIM Combined events 4 callback.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
__weak void HAL_GFXTIM_EventGenerator_EV4Callback(GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgfxtim);
  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_GFXTIM_EventGenerator_EV4Callback could be implemented in the user file */
}

/**
  * @}
  */

/** @defgroup GFXTIM_Exported_Functions_Group6 Watchdog functions
  * @brief    Event Generator functions
  *
@verbatim
  ==============================================================================
                        ##### Watchdog functions #####
  ==============================================================================
    [..]  This section provides functions allowing to :
      (+) Configure the Watchdog.
      (+) Enable the Watchdog
      (+) Disable the Watchdog.
      (+) Refresh the Watchdog.
@endverbatim
  * @{
  */

/**
  * @brief  This function configures the Watchdog.
  * @param  hgfxtim GFXTIM handle.
  * @param  pWatchdogConfig Watchdog configuration parameters.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_WatchdogTimer_Config(GFXTIM_HandleTypeDef *hgfxtim,
                                                  const GFXTIM_WatchdogConfigTypeDef *pWatchdogConfig)
{
  HAL_StatusTypeDef status = HAL_OK;

  if ((hgfxtim == NULL) || (pWatchdogConfig == NULL))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));
    assert_param(IS_GFXTIM_WATCHDOG_CLOCK_SRC(pWatchdogConfig->ClockSrc));
    assert_param(IS_GFXTIM_WATCHDOG_HW_RELOAD_CONFIG(pWatchdogConfig->HwReloadConfig));

    assert_param(IS_GFXTIM_WATCHDOG_VALUE(pWatchdogConfig->AutoReloadValue));
    assert_param(IS_GFXTIM_WATCHDOG_VALUE(pWatchdogConfig->PreAlarmValue));

    assert_param(IS_GFXTIM_INTERRUPT(pWatchdogConfig->AlarmInterrupt));
    assert_param(IS_GFXTIM_INTERRUPT(pWatchdogConfig->PreAlarmInterrupt));


    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      /* Set watchdog auto reload value */
      MODIFY_REG(hgfxtim->Instance->WDGRR, GFXTIM_WDGRR_RELOAD,
                 (pWatchdogConfig->AutoReloadValue << GFXTIM_WDGRR_RELOAD_Pos));

      /* Set watchdog pre alarm value */
      MODIFY_REG(hgfxtim->Instance->WDGPAR, GFXTIM_WDGPAR_PREALARM,
                 pWatchdogConfig->PreAlarmValue << GFXTIM_WDGPAR_PREALARM_Pos);

      /* Set watchdog clock source and hardware reload */
      MODIFY_REG(hgfxtim->Instance->WDGTCR, (GFXTIM_WDGTCR_WDGCS | GFXTIM_WDGTCR_WDGHRC),
                 (pWatchdogConfig->ClockSrc | pWatchdogConfig->HwReloadConfig));

      /* Set watchdog interrupts */
      MODIFY_REG(hgfxtim->Instance->IER, \
                 (GFXTIM_IER_WDGAIE | GFXTIM_IER_WDGPIE), \
                 ((pWatchdogConfig->AlarmInterrupt << GFXTIM_IER_WDGAIE_Pos) | \
                  (pWatchdogConfig->PreAlarmInterrupt << GFXTIM_IER_WDGPIE_Pos)));
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  This function Enable the Watchdog Counter.
  * @param  hgfxtim GFXTIM handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_WatchdogTimer_Enable(GFXTIM_HandleTypeDef *hgfxtim)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hgfxtim == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));


    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      /* Set watchdog enable bit */
      SET_BIT(hgfxtim->Instance->WDGTCR, GFXTIM_WDGTCR_WDGEN);
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  This function Disable the Watchdog Counter.
  * @param  hgfxtim GFXTIM handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_WatchdogTimer_Disable(GFXTIM_HandleTypeDef *hgfxtim)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hgfxtim == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));


    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      /* Set watchdog disable bit */
      SET_BIT(hgfxtim->Instance->WDGTCR, GFXTIM_WDGTCR_WDGDIS);
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  This function refresh the Watchdog counter.
  * @param  hgfxtim GFXTIM handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_GFXTIM_WatchdogTimer_Refresh(GFXTIM_HandleTypeDef *hgfxtim)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hgfxtim == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_GFXTIM_ALL_INSTANCE(hgfxtim->Instance));


    if (hgfxtim->State == HAL_GFXTIM_STATE_READY)
    {
      /* Set watchdog SW relaod */
      SET_BIT(hgfxtim->Instance->WDGTCR, GFXTIM_WDGTCR_FWDGR);
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  GFXTIM Watchdog alarm callback.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
__weak void HAL_GFXTIM_WatchdogTimer_AlarmCallback(GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgfxtim);
  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_GFXTIM_WatchdogTimer_AlarmCallback could be implemented in the user file */
}

/**
  * @brief  GFXTIM Watchdog pre alarm callback.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
__weak void HAL_GFXTIM_WatchdogTimer_PreAlarmCallback(GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgfxtim);
  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_GFXTIM_WatchdogTimer_PreAlarmCallback could be implemented in the user file */
}

/**
  * @}
  */

/** @defgroup GFXTIM_Exported_Functions_Group7  Generic functions
  * @brief    Generic functions
  *
@verbatim
  ==============================================================================
                          ##### Generic functions #####
  ==============================================================================
    [..]  This section provides functions allowing to :
      (+) Handle GFXTIM interrupt.
      (+) Inform user that error occurs.
      (+) Get the current GFXTIM instance state
      (+) Get the current GFXTIM instance error code.
@endverbatim
  * @{
  */

/**
  * @brief  This function handles the GFXTIM interrupts.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
void HAL_GFXTIM_IRQHandler(GFXTIM_HandleTypeDef *hgfxtim)
{
  uint32_t tmp_reg1;
  uint32_t tmp_reg2;
  uint32_t interrupts;

  /* Read all pending interrupts */
  tmp_reg1 = READ_REG(hgfxtim->Instance->ISR);
  tmp_reg2 = READ_REG(hgfxtim->Instance->IER);
  interrupts = tmp_reg1 & tmp_reg2;

  if ((interrupts & GFXTIM_ISR_AFCC1F) != 0U)
  {
#if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1U)
    hgfxtim->HAL_GFXTIM_AbsoluteTimer_AFCC1Callback(hgfxtim);
#else
    HAL_GFXTIM_AbsoluteTimer_AFCC1Callback(hgfxtim);
#endif /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */
  }

  if ((interrupts & GFXTIM_ISR_AFCOF) != 0U)
  {
#if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1U)
    hgfxtim->HAL_GFXTIM_AbsoluteTimer_AFCOFCallback(hgfxtim);
#else
    HAL_GFXTIM_AbsoluteTimer_AFCOFCallback(hgfxtim);
#endif /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */
  }

  if ((interrupts & GFXTIM_ISR_ALCC1F) != 0U)
  {
#if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1U)
    hgfxtim->HAL_GFXTIM_AbsoluteTimer_ALCC1Callback(hgfxtim);
#else
    HAL_GFXTIM_AbsoluteTimer_ALCC1Callback(hgfxtim);
#endif /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */
  }

  if ((interrupts & GFXTIM_ISR_ALCC2F) != 0U)
  {
#if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1U)
    hgfxtim->HAL_GFXTIM_AbsoluteTimer_ALCC2Callback(hgfxtim);
#else
    HAL_GFXTIM_AbsoluteTimer_ALCC2Callback(hgfxtim);
#endif /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */
  }

  if ((interrupts & GFXTIM_ISR_ALCOF) != 0U)
  {
#if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1U)
    hgfxtim->HAL_GFXTIM_AbsoluteTimer_ALCOFCallback(hgfxtim);
#else
    HAL_GFXTIM_AbsoluteTimer_ALCOFCallback(hgfxtim);
#endif /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */
  }

  if ((interrupts & GFXTIM_ISR_TEF) != 0U)
  {
#if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1U)
    hgfxtim->HAL_GFXTIM_TECallback(hgfxtim);
#else
    HAL_GFXTIM_TECallback(hgfxtim);
#endif /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */
  }

  if ((interrupts & GFXTIM_ISR_RFC1RF) != 0U)
  {
#if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1U)
    hgfxtim->HAL_GFXTIM_RelativeTimer_RFC1RCallback(hgfxtim);
#else
    HAL_GFXTIM_RelativeTimer_RFC1RCallback(hgfxtim);
#endif /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */
  }

  if ((interrupts & GFXTIM_ISR_RFC2RF) != 0U)
  {
#if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1U)
    hgfxtim->HAL_GFXTIM_RelativeTimer_RFC2RCallback(hgfxtim);
#else
    HAL_GFXTIM_RelativeTimer_RFC2RCallback(hgfxtim);
#endif /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */
  }

  if ((interrupts & GFXTIM_ISR_EV1F) != 0U)
  {
#if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1U)
    hgfxtim->HAL_GFXTIM_EventGenerator_EV1Callback(hgfxtim);
#else
    HAL_GFXTIM_EventGenerator_EV1Callback(hgfxtim);
#endif /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */
  }

  if ((interrupts & GFXTIM_ISR_EV2F) != 0U)
  {
#if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1U)
    hgfxtim->HAL_GFXTIM_EventGenerator_EV2Callback(hgfxtim);
#else
    HAL_GFXTIM_EventGenerator_EV2Callback(hgfxtim);
#endif /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */
  }

  if ((interrupts & GFXTIM_ISR_EV3F) != 0U)
  {
#if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1U)
    hgfxtim->HAL_GFXTIM_EventGenerator_EV3Callback(hgfxtim);
#else
    HAL_GFXTIM_EventGenerator_EV3Callback(hgfxtim);
#endif /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */
  }

  if ((interrupts & GFXTIM_ISR_EV4F) != 0U)
  {
#if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1U)
    hgfxtim->HAL_GFXTIM_EventGenerator_EV4Callback(hgfxtim);
#else
    HAL_GFXTIM_EventGenerator_EV4Callback(hgfxtim);
#endif /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */
  }

  if ((interrupts & GFXTIM_ISR_WDGAF) != 0U)
  {
#if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1U)
    hgfxtim->HAL_GFXTIM_WatchdogTimer_AlarmCallback(hgfxtim);
#else
    HAL_GFXTIM_WatchdogTimer_AlarmCallback(hgfxtim);
#endif /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */
  }

  if ((interrupts & GFXTIM_ISR_WDGPF) != 0U)
  {
#if (USE_HAL_GFXTIM_REGISTER_CALLBACKS == 1U)
    hgfxtim->HAL_GFXTIM_WatchdogTimer_PreAlarmCallback(hgfxtim);
#else
    HAL_GFXTIM_WatchdogTimer_PreAlarmCallback(hgfxtim);
#endif /* USE_HAL_GFXTIM_REGISTER_CALLBACKS */
  }
  /* Clear all pending interrupts */
  WRITE_REG(hgfxtim->Instance->ICR, interrupts);
}

/**
  * @brief  GFXTIM error callback.
  * @param  hgfxtim GFXTIM handle.
  * @retval None.
  */
__weak void HAL_GFXTIM_ErrorCallback(GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgfxtim);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_GFXTIM_ErrorCallback could be implemented in the user file. */
}

/**
  * @brief  This function get the current GFXTIM state.
  * @param  hgfxtim GFXTIM handle.
  * @retval GFXTIM state.
  */
HAL_GFXTIM_StateTypeDef HAL_GFXTIM_GetState(const GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Return GFXTIM state */
  return hgfxtim->State;
}

/**
  * @brief  This function get the current GFXTIM error.
  * @param  hgfxtim GFXTIM handle.
  * @retval GFXTIM error code.
  */
uint32_t HAL_GFXTIM_GetError(const GFXTIM_HandleTypeDef *hgfxtim)
{
  /* Return GFXTIM error code */
  return hgfxtim->ErrorCode;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#endif /* GFXTIM */
#endif /* HAL_GFXTIM_MODULE_ENABLED */
/**
  * @}
  */
