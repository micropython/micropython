/**
  ******************************************************************************
  * @file    stm32n6xx_hal_mdf.c
  * @author  MCD Application Team
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the Multi-function Digital Filter (MDF)
  *          peripheral:
  *           + Initialization and de-initialization
  *           + Acquisition
  *           + Clock absence detection
  *           + Short circuit detection
  *           + Out-off limit detection
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
    *** Initialization and de-initialization ***
    ============================================
    [..]
      (#) User has first to initialize MDF or ADF instance.
      (#) As prerequisite, fill in the HAL_MDF_MspInit() :
        (++) Enable MDFz or ADFz clock interface with __HAL_RCC_MDFz_CLK_ENABLE()
             or __HAL_RCC_ADFz_CLK_ENABLE().
        (++) Enable the clocks for the used GPIOS with __HAL_RCC_GPIOx_CLK_ENABLE().
        (++) Configure these pins in alternate mode using HAL_GPIO_Init().
        (++) If interrupt mode is used, enable and configure MDFz_FLTx or ADFz
             interrupt with HAL_NVIC_SetPriority() and HAL_NVIC_EnableIRQ().
        (++) If DMA mode is used, initialize and configure DMA.
      (#) Configure the common parameters (only for first MDF or ADF instance init),
          serial interface parameters and filter bitstream selection by calling
          HAL_MDF_Init() function.

    [..]
      (#) User can de-initialize MDF or ADF instance with HAL_MDF_DeInit() function.

    *** Acquisition ***
    ===================
    [..]
      (#) Configure filter parameters and start acquisition using HAL_MDF_AcqStart(),
          HAL_MDF_AcqStart_IT() or HAL_MDF_AcqStart_DMA().
      (#) In polling mode :
            (++) Use HAL_MDF_PollForAcq() to detect the end of acquisition.
                 Use HAL_MDF_GetAcqValue to get acquisition value.
            (++) Only for MDF instance, use HAL_MDF_PollForSnapshotAcq() to detect
                 the end of snapshot acquisition.
                 Use HAL_MDF_GetSnapshotAcqValue to get snapshot acquisition value.
            (++) Only for ADF instance, use HAL_MDF_PollForSndLvl() to detect and get
                 new sound level value and ambient noise value.
            (++) Only for ADF instance, use HAL_MDF_PollForSad() to detect sound activity.
      (#) In interrupt mode :
            (++) HAL_MDF_AcqCpltCallback() will be called at the end of acquisition.
                 Use HAL_MDF_GetAcqValue to get acquisition value or use
                 HAL_MDF_GetSnapshotAcqValue to get snapshot acquisition value.
            (++) Only for ADF instance, HAL_MDF_SndLvlCallback() will be called when new
                 sound level and ambient noise values are available.
            (++) Only for ADF instance, HAL_MDF_SadCallback() will be called when
                 sound activity detection occurs.
            (++) HAL_MDF_ErrorCallback() will be called if overflow, filter overrun or
                 saturation occurs.
                 Use HAL_MDF_GetErrorCode() to get the corresponding error.
      (#) In DMA mode :
            (++) HAL_MDF_AcqHalfCpltCallback() and HAL_MDF_AcqCpltCallback() will be called
                 respectively at the half acquisition and at the acquisition complete.
            (++) Only for ADF instance, HAL_MDF_SndLvlCallback() will be called when new
                 sound level and ambient noise values are available.
            (++) Only for ADF instance, HAL_MDF_SadCallback() will be called when
                 sound activity detection occurs.
            (++) HAL_MDF_ErrorCallback() will be called if overflow, filter overrun,
                 saturation or DMA error occurs.
                 Use HAL_MDF_GetErrorCode() to get the corresponding error.
      (#) Use HAL_MDF_GenerateTrgo() to generate pulse on TRGO signal.
      (#) During acquisition, use HAL_MDF_SetDelay() and HAL_MDF_GetDelay() to respectively
          set and get the delay on data source.
      (#) During acquisition, use HAL_MDF_SetGain() and HAL_MDF_GetGain() to respectively
          set and get the filter gain.
      (#) During acquisition, use HAL_MDF_SetOffset() and HAL_MDF_GetOffset() to respectively
          set and get the filter offset error compensation.
      (#) During acquisition, only for MDF instance, use HAL_MDF_SetOffset() and HAL_MDF_GetOffset()
          to respectively set and get the filter offset error compensation.
      (#) Stop acquisition using HAL_MDF_AcqStop(), HAL_MDF_AcqStop_IT() or HAL_MDF_AcqStop_DMA().

    *** Clock absence detection ***
    ===============================
    [..]
      (#) Clock absence detection is always enabled so no need to start clock absence detection
          in polling mode.
          Use HAL_MDF_CkabStart_IT() to start clock absence detection in interrupt mode.
      (#) In polling mode, use HAL_MDF_PollForCkab() to detect the clock absence.
      (#) In interrupt mode, HAL_MDF_ErrorCallback() will be called if clock absence detection
          occurs.
          Use HAL_MDF_GetErrorCode() to get the corresponding error.
      (#) Stop clock absence detection in interrupt mode using HAL_MDF_CkabStop_IT().

    *** Short circuit detection ***
    ===============================
    [..]
      (#) Only for MDF instance, start short circuit detection using HAL_MDF_ScdStart()
          or HAL_MDF_ScdStart_IT().
      (#) In polling mode, use HAL_MDF_PollForScd() to detect short circuit.
      (#) In interrupt mode, HAL_MDF_ErrorCallback() will be called if short circuit detection
          occurs.
          Use HAL_MDF_GetErrorCode() to get the corresponding error.
      (#) Stop short circuit detection using HAL_MDF_ScdStop() or HAL_MDF_ScdStop_IT().

    *** Out-off limit detection ***
    ===============================
    [..]
      (#) Only for MDF instance, start out-off limit detection using HAL_MDF_OldStart()
          or HAL_MDF_OldStart_IT().
      (#) In polling mode, use HAL_MDF_PollForOld() to detect out-off limit and to get threshold
          information.
      (#) In interrupt mode, HAL_MDF_OldCallback() will be called if out-off limit detection occurs.
      (#) Stop out-off limit detection using HAL_MDF_OldStop() or HAL_MDF_OldStop_IT().

    *** generic functions ***
    =========================
    [..]
      (#) HAL_MDF_IRQHandler will be called when MDF or ADF interrupt occurs.
      (#) HAL_MDF_ErrorCallback will be called when MDF or ADF error occurs.
      (#) Use HAL_MDF_GetState() to get the current MDF or ADF instance state.
      (#) Use HAL_MDF_GetErrorCode() to get the current MDF or ADF instance error code.

    *** Callback registration ***
    =============================
    [..]
    The compilation define USE_HAL_MDF_REGISTER_CALLBACKS when set to 1
    allows the user to configure dynamically the driver callbacks.
    Use functions HAL_MDF_RegisterCallback(), HAL_MDF_RegisterOldCallback()
    or HAL_MDF_RegisterSndLvlCallback() to register a user callback.

    [..]
    Function HAL_MDF_RegisterCallback() allows to register following callbacks :
      (+) AcqCpltCallback     : Acquisition complete callback.
      (+) AcqHalfCpltCallback : Acquisition half complete callback.
      (+) SadCallback         : Sound activity detection callback (only for ADF instance).
      (+) ErrorCallback       : Error callback.
      (+) MspInitCallback     : MSP init callback.
      (+) MspDeInitCallback   : MSP de-init callback.
    [..]
    This function takes as parameters the HAL peripheral handle, the Callback ID
    and a pointer to the user callback function.

    [..]
    For MDF instance and for specific out-off limit detection callback use dedicated
    register callback :
    HAL_MDF_RegisterOldCallback().

    [..]
    For ADF instance and for specific sound level callback use dedicated register callback :
    HAL_MDF_RegisterSndLvlCallback().

    [..]
    Use function HAL_MDF_UnRegisterCallback() to reset a callback to the default weak function.

    [..]
    HAL_MDF_UnRegisterCallback() takes as parameters the HAL peripheral handle and the Callback ID.
    [..]
    This function allows to reset following callbacks :
      (+) AcqCpltCallback     : Acquisition complete callback.
      (+) AcqHalfCpltCallback : Acquisition half complete callback.
      (+) SadCallback         : Sound activity detection callback (only for ADF instance).
      (+) ErrorCallback       : Error callback.
      (+) MspInitCallback     : MSP init callback.
      (+) MspDeInitCallback   : MSP de-init callback.

    [..]
    For MDF instance and for specific out-off limit detection callback use dedicated
    unregister callback :
    HAL_MDF_UnRegisterOldCallback().

    [..]
    For ADF instance and for specific sound level callback use dedicated unregister callback :
    HAL_MDF_UnRegisterSndLvlCallback().

    [..]
    By default, after the call of init function and if the state is RESET
    all callbacks are reset to the corresponding legacy weak functions :
    examples HAL_MDF_AcqCpltCallback(), HAL_MDF_ErrorCallback().
    Exception done for MspInit and MspDeInit callbacks that are respectively
    reset to the legacy weak functions in the init and de-init only when these
    callbacks are null (not registered beforehand).
    If not, MspInit or MspDeInit are not null, the init and de-init keep and use
    the user MspInit/MspDeInit callbacks (registered beforehand).

    [..]
    Callbacks can be registered/unregistered in READY state only.
    Exception done for MspInit/MspDeInit callbacks that can be registered/unregistered
    in READY or RESET state, thus registered (user) MspInit/DeInit callbacks can be used
    during the init/de-init.
    In that case first register the MspInit/MspDeInit user callbacks using
    HAL_MDF_RegisterCallback() before calling init or de-init function.

    [..]
    When the compilation define USE_HAL_MDF_REGISTER_CALLBACKS is set to 0 or
    not defined, the callback registering feature is not available
    and weak callbacks are used.

    @endverbatim
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup MDF MDF
  * @brief MDF HAL module driver
  * @{
  */

#ifdef HAL_MDF_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/** @defgroup MDF_Private_Typedefs  MDF Private Typedefs
  * @{
  */
/**
  * @}
  */

/* Private define ------------------------------------------------------------*/
/** @defgroup MDF_Private_Constants  MDF Private Constants
  * @{
  */
#define MDF_INSTANCE_NUMBER 7U /* 6 instances for MDF1 and 1 instance for ADF1 */
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup MDF_Private_Variables  MDF Private Variables
  * @{
  */
static uint32_t           v_mdf1InstanceCounter = 0U;
static uint32_t           v_adf1InstanceCounter = 0U;
static MDF_HandleTypeDef *a_mdfHandle[MDF_INSTANCE_NUMBER] = {NULL};
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup MDF_Private_Functions  MDF Private Functions
  * @{
  */
static uint32_t MDF_GetHandleNumberFromInstance(const MDF_Filter_TypeDef *const pInstance);
static void     MDF_AcqStart(MDF_HandleTypeDef *const hmdf, const MDF_FilterConfigTypeDef *const pFilterConfig);
static void     MDF_DmaXferCpltCallback(DMA_HandleTypeDef *hdma);
static void     MDF_DmaXferHalfCpltCallback(DMA_HandleTypeDef *hdma);
static void     MDF_DmaErrorCallback(DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/
/** @defgroup MDF_Exported_Functions  MDF Exported Functions
  * @{
  */

/** @defgroup MDF_Exported_Functions_Group1  Initialization and de-initialization functions
  * @brief    Initialization and de-initialization functions
  *
@verbatim
  ==============================================================================
            ##### Initialization and de-initialization functions #####
  ==============================================================================
    [..]  This section provides functions allowing to :
      (+) Initialize the MDF or ADF instance.
      (+) De-initialize the MDF or ADF instance.
      (+) Register and unregister callbacks.
@endverbatim
  * @{
  */

/**
  * @brief  Initialize the MDF instance according to the specified parameters
  *         in the MDF_InitTypeDef structure and initialize the associated handle.
  * @param  hmdf MDF handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_Init(MDF_HandleTypeDef *hmdf)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check MDF handle */
  if (hmdf == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_MDF_ALL_INSTANCE(hmdf->Instance));
    assert_param(IS_MDF_FILTER_BITSTREAM(hmdf->Init.FilterBistream));
    assert_param(IS_FUNCTIONAL_STATE(hmdf->Init.SerialInterface.Activation));

    /* Check that instance has not been already initialized */
    if (a_mdfHandle[MDF_GetHandleNumberFromInstance(hmdf->Instance)] != NULL)
    {
      status = HAL_ERROR;
    }
    else
    {
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
      /* Reset callback pointers to the weak predefined callbacks */
      if (IS_ADF_INSTANCE(hmdf->Instance))
      {
        hmdf->OldCallback   = NULL;
        hmdf->SndLvCallback = HAL_MDF_SndLvlCallback;
        hmdf->SadCallback   = HAL_MDF_SadCallback;
      }
      else /* MDF instance */
      {
        hmdf->OldCallback   = HAL_MDF_OldCallback;
        hmdf->SndLvCallback = NULL;
        hmdf->SadCallback   = NULL;
      }
      hmdf->AcqCpltCallback     = HAL_MDF_AcqCpltCallback;
      hmdf->AcqHalfCpltCallback = HAL_MDF_AcqHalfCpltCallback;
      hmdf->ErrorCallback       = HAL_MDF_ErrorCallback;

      /* Call MDF MSP init function */
      if (hmdf->MspInitCallback == NULL)
      {
        hmdf->MspInitCallback = HAL_MDF_MspInit;
      }
      hmdf->MspInitCallback(hmdf);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
      /* Call MDF MSP init function */
      HAL_MDF_MspInit(hmdf);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */

      /* Configure common parameters only for first MDF or ADF instance */
      if (((v_mdf1InstanceCounter == 0U) && IS_MDF_INSTANCE(hmdf->Instance)) ||
          ((v_adf1InstanceCounter == 0U) && IS_ADF_INSTANCE(hmdf->Instance)))
      {
        MDF_TypeDef *mdfBase;
        /* Get MDF base according instance */
        mdfBase = (IS_ADF_INSTANCE(hmdf->Instance)) ? ADF1 : MDF1;

        /* Check clock generator status */
        if ((mdfBase->CKGCR & MDF_CKGCR_CCKACTIVE) != 0U)
        {
          status = HAL_ERROR;
        }
        else
        {
          /* Configure number of interleaved filters for MDF instance */
          if (IS_MDF_INSTANCE(hmdf->Instance))
          {
            assert_param(IS_MDF_INTERLEAVED_FILTERS(hmdf->Init.CommonParam.InterleavedFilters));
            mdfBase->GCR &= ~(MDF_GCR_ILVNB);
            mdfBase->GCR |= (hmdf->Init.CommonParam.InterleavedFilters << MDF_GCR_ILVNB_Pos);
          }

          /* Configure processing clock divider, output clock divider,
             output clock pins and output clock generation trigger */
          assert_param(IS_MDF_PROC_CLOCK_DIVIDER(hmdf->Init.CommonParam.ProcClockDivider));
          assert_param(IS_FUNCTIONAL_STATE(hmdf->Init.CommonParam.OutputClock.Activation));
          mdfBase->CKGCR = 0U;
          mdfBase->CKGCR |= ((hmdf->Init.CommonParam.ProcClockDivider - 1U) << MDF_CKGCR_PROCDIV_Pos);
          if (hmdf->Init.CommonParam.OutputClock.Activation == ENABLE)
          {
            assert_param(IS_MDF_OUTPUT_CLOCK_PINS(hmdf->Init.CommonParam.OutputClock.Pins));
            assert_param(IS_MDF_OUTPUT_CLOCK_DIVIDER(hmdf->Init.CommonParam.OutputClock.Divider));
            assert_param(IS_FUNCTIONAL_STATE(hmdf->Init.CommonParam.OutputClock.Trigger.Activation));
            mdfBase->CKGCR |= (((hmdf->Init.CommonParam.OutputClock.Divider - 1U) << MDF_CKGCR_CCKDIV_Pos) |
                               hmdf->Init.CommonParam.OutputClock.Pins |
                               (hmdf->Init.CommonParam.OutputClock.Pins >> 4U));
            if (hmdf->Init.CommonParam.OutputClock.Trigger.Activation == ENABLE)
            {
              if (IS_MDF_INSTANCE(hmdf->Instance))
              {
                assert_param(IS_MDF_OUTPUT_CLOCK_TRIGGER_SOURCE(hmdf->Init.CommonParam.OutputClock.Trigger.Source));
              }
              else /* ADF instance */
              {
                assert_param(IS_ADF_OUTPUT_CLOCK_TRIGGER_SOURCE(hmdf->Init.CommonParam.OutputClock.Trigger.Source));
              }
              assert_param(IS_MDF_OUTPUT_CLOCK_TRIGGER_EDGE(hmdf->Init.CommonParam.OutputClock.Trigger.Edge));
              mdfBase->CKGCR |= (hmdf->Init.CommonParam.OutputClock.Trigger.Source |
                                 hmdf->Init.CommonParam.OutputClock.Trigger.Edge |
                                 MDF_CKGCR_CKGMOD);
            }
          }

          /* Activate clock generator */
          mdfBase->CKGCR |= MDF_CKGCR_CKDEN;
        }
      }

      /* Configure serial interface */
      if ((status == HAL_OK) && (hmdf->Init.SerialInterface.Activation == ENABLE))
      {
        /* Check serial interface status */
        if ((hmdf->Instance->SITFCR & MDF_SITFCR_SITFACTIVE) != 0U)
        {
          status = HAL_ERROR;
        }
        else
        {
          /* Configure mode, clock source and threshold */
          assert_param(IS_MDF_SITF_MODE(hmdf->Init.SerialInterface.Mode));
          assert_param(IS_MDF_SITF_CLOCK_SOURCE(hmdf->Init.SerialInterface.ClockSource));
          assert_param(IS_MDF_SITF_THRESHOLD(hmdf->Init.SerialInterface.Threshold));
          hmdf->Instance->SITFCR = 0U;
          hmdf->Instance->SITFCR |= ((hmdf->Init.SerialInterface.Threshold << MDF_SITFCR_STH_Pos) |
                                     hmdf->Init.SerialInterface.Mode | hmdf->Init.SerialInterface.ClockSource);

          /* Activate serial interface */
          hmdf->Instance->SITFCR |= MDF_SITFCR_SITFEN;
        }
      }

      if (status == HAL_OK)
      {
        /* Configure filter bitstream */
        hmdf->Instance->BSMXCR &= ~(MDF_BSMXCR_BSSEL);
        hmdf->Instance->BSMXCR |= hmdf->Init.FilterBistream;

        /* Update instance counter and table */
        if (IS_ADF_INSTANCE(hmdf->Instance))
        {
          v_adf1InstanceCounter++;
        }
        else /* MDF instance */
        {
          v_mdf1InstanceCounter++;
        }
        a_mdfHandle[MDF_GetHandleNumberFromInstance(hmdf->Instance)] = hmdf;

        /* Update error code and state */
        hmdf->ErrorCode = MDF_ERROR_NONE;
        hmdf->State     = HAL_MDF_STATE_READY;
      }
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  De-initialize the MDF instance.
  * @param  hmdf MDF handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_DeInit(MDF_HandleTypeDef *hmdf)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check MDF handle */
  if (hmdf == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameters */
    assert_param(IS_MDF_ALL_INSTANCE(hmdf->Instance));

    /* Check that instance has not been already deinitialized */
    if (a_mdfHandle[MDF_GetHandleNumberFromInstance(hmdf->Instance)] == NULL)
    {
      status = HAL_ERROR;
    }
    else
    {
      if (IS_MDF_INSTANCE(hmdf->Instance))
      {
        /* Disable short circuit detector if needed */
        if ((hmdf->Instance->SCDCR & MDF_SCDCR_SCDACTIVE) != 0U)
        {
          hmdf->Instance->SCDCR &= ~(MDF_SCDCR_SCDEN);
        }

        /* Disable out-off limit detector if needed */
        if ((hmdf->Instance->OLDCR & MDF_OLDCR_OLDACTIVE) != 0U)
        {
          hmdf->Instance->OLDCR &= ~(MDF_OLDCR_OLDEN);
        }
      }

      /* Disable sound activity detector if needed */
      if (IS_ADF_INSTANCE(hmdf->Instance))
      {
        if ((hmdf->Instance->SADCR & MDF_SADCR_SADACTIVE) != 0U)
        {
          hmdf->Instance->SADCR &= ~(MDF_SADCR_SADEN);
        }
      }

      /* Disable filter if needed */
      if ((hmdf->Instance->DFLTCR & MDF_DFLTCR_DFLTACTIVE) != 0U)
      {
        hmdf->Instance->DFLTCR &= ~(MDF_DFLTCR_DFLTEN);
      }

      /* Disable serial interface if needed */
      if ((hmdf->Instance->SITFCR & MDF_SITFCR_SITFACTIVE) != 0U)
      {
        hmdf->Instance->SITFCR &= ~(MDF_SITFCR_SITFEN);
      }

      /* Disable all interrupts and clear all pending flags */
      hmdf->Instance->DFLTIER = 0U;
      hmdf->Instance->DFLTISR = 0xFFFFFFFFU;

      /* Disable clock generator only for last MDF or ADF instance deinitialization */
      if (((v_mdf1InstanceCounter == 1U) && IS_MDF_INSTANCE(hmdf->Instance)) ||
          ((v_adf1InstanceCounter == 1U) && IS_ADF_INSTANCE(hmdf->Instance)))
      {
        MDF_TypeDef *p_mdf_base;
        /* Get MDF base according instance */
        p_mdf_base = (IS_ADF_INSTANCE(hmdf->Instance)) ? ADF1 : MDF1;

        /* Disable clock generator */
        p_mdf_base->CKGCR &= ~(MDF_CKGCR_CKDEN);
      }

      /* Call MDF MSP deinit function */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
      if (hmdf->MspDeInitCallback == NULL)
      {
        hmdf->MspDeInitCallback = HAL_MDF_MspDeInit;
      }
      hmdf->MspDeInitCallback(hmdf);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
      HAL_MDF_MspDeInit(hmdf);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */

      /* Update instance counter and table */
      if (IS_ADF_INSTANCE(hmdf->Instance))
      {
        v_adf1InstanceCounter--;
      }
      else /* MDF instance */
      {
        v_mdf1InstanceCounter--;
      }
      a_mdfHandle[MDF_GetHandleNumberFromInstance(hmdf->Instance)] = (MDF_HandleTypeDef *) NULL;

      /* Update state */
      hmdf->State = HAL_MDF_STATE_RESET;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Initialize the MDF instance MSP.
  * @param  hmdf MDF handle.
  * @retval None.
  */
__weak void HAL_MDF_MspInit(MDF_HandleTypeDef *hmdf)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdf);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_MDF_MspInit could be implemented in the user file */
}

/**
  * @brief  De-initialize the MDF instance MSP.
  * @param  hmdf MDF handle.
  * @retval None.
  */
__weak void HAL_MDF_MspDeInit(MDF_HandleTypeDef *hmdf)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdf);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_MDF_MspDeInit could be implemented in the user file */
}

#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register a user MDF callback to be used instead of the weak predefined callback.
  * @param  hmdf MDF handle.
  * @param  CallbackID ID of the callback to be registered.
  *         This parameter can be one of the following values:
  *           @arg @ref HAL_MDF_ACQ_COMPLETE_CB_ID acquisition complete callback ID.
  *           @arg @ref HAL_MDF_ACQ_HALFCOMPLETE_CB_ID acquisition half complete callback ID.
  *           @arg @ref HAL_MDF_SAD_CB_ID sound activity detector callback ID (only for ADF instance).
  *           @arg @ref HAL_MDF_ERROR_CB_ID error callback ID.
  *           @arg @ref HAL_MDF_MSPINIT_CB_ID MSP init callback ID.
  *           @arg @ref HAL_MDF_MSPDEINIT_CB_ID MSP de-init callback ID.
  * @param  pCallback pointer to the callback function.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_RegisterCallback(MDF_HandleTypeDef        *hmdf,
                                           HAL_MDF_CallbackIDTypeDef CallbackID,
                                           pMDF_CallbackTypeDef      pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (pCallback == NULL)
  {
    /* Update error code and status */
    hmdf->ErrorCode |= MDF_ERROR_INVALID_CALLBACK;
    status = HAL_ERROR;
  }
  else
  {
    if (hmdf->State == HAL_MDF_STATE_READY)
    {
      switch (CallbackID)
      {
        case HAL_MDF_ACQ_COMPLETE_CB_ID :
          hmdf->AcqCpltCallback = pCallback;
          break;
        case HAL_MDF_ACQ_HALFCOMPLETE_CB_ID :
          hmdf->AcqHalfCpltCallback = pCallback;
          break;
        case HAL_MDF_SAD_CB_ID :
          hmdf->SadCallback = pCallback;
          break;
        case HAL_MDF_ERROR_CB_ID :
          hmdf->ErrorCallback = pCallback;
          break;
        case HAL_MDF_MSPINIT_CB_ID :
          hmdf->MspInitCallback = pCallback;
          break;
        case HAL_MDF_MSPDEINIT_CB_ID :
          hmdf->MspDeInitCallback = pCallback;
          break;
        default :
          /* Update error code and status */
          hmdf->ErrorCode |= MDF_ERROR_INVALID_CALLBACK;
          status = HAL_ERROR;
          break;
      }
    }
    else if (hmdf->State == HAL_MDF_STATE_RESET)
    {
      switch (CallbackID)
      {
        case HAL_MDF_MSPINIT_CB_ID :
          hmdf->MspInitCallback = pCallback;
          break;
        case HAL_MDF_MSPDEINIT_CB_ID :
          hmdf->MspDeInitCallback = pCallback;
          break;
        default :
          /* Update error code and status */
          hmdf->ErrorCode |= MDF_ERROR_INVALID_CALLBACK;
          status = HAL_ERROR;
          break;
      }
    }
    else
    {
      /* Update error code and status */
      hmdf->ErrorCode |= MDF_ERROR_INVALID_CALLBACK;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Unregister a user MDF callback.
  *         MDF callback is redirected to the weak predefined callback.
  * @param  hmdf MDF handle.
  * @param  CallbackID ID of the callback to be unregistered.
  *         This parameter can be one of the following values:
  *           @arg @ref HAL_MDF_ACQ_COMPLETE_CB_ID acquisition complete callback ID.
  *           @arg @ref HAL_MDF_ACQ_HALFCOMPLETE_CB_ID acquisition half complete callback ID.
  *           @arg @ref HAL_MDF_SAD_CB_ID sound activity detector callback ID (only for ADF instance).
  *           @arg @ref HAL_MDF_ERROR_CB_ID error callback ID.
  *           @arg @ref HAL_MDF_MSPINIT_CB_ID MSP init callback ID.
  *           @arg @ref HAL_MDF_MSPDEINIT_CB_ID MSP de-init callback ID.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_UnRegisterCallback(MDF_HandleTypeDef        *hmdf,
                                             HAL_MDF_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hmdf->State == HAL_MDF_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_MDF_ACQ_COMPLETE_CB_ID :
        hmdf->AcqCpltCallback = HAL_MDF_AcqCpltCallback;
        break;
      case HAL_MDF_ACQ_HALFCOMPLETE_CB_ID :
        hmdf->AcqHalfCpltCallback = HAL_MDF_AcqHalfCpltCallback;
        break;
      case HAL_MDF_SAD_CB_ID :
        hmdf->SadCallback = HAL_MDF_SadCallback;
        break;
      case HAL_MDF_ERROR_CB_ID :
        hmdf->ErrorCallback = HAL_MDF_ErrorCallback;
        break;
      case HAL_MDF_MSPINIT_CB_ID :
        hmdf->MspInitCallback = HAL_MDF_MspInit;
        break;
      case HAL_MDF_MSPDEINIT_CB_ID :
        hmdf->MspDeInitCallback = HAL_MDF_MspDeInit;
        break;
      default :
        /* Update error code and status */
        hmdf->ErrorCode |= MDF_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
        break;
    }
  }
  else if (hmdf->State == HAL_MDF_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_MDF_MSPINIT_CB_ID :
        hmdf->MspInitCallback = HAL_MDF_MspInit;
        break;
      case HAL_MDF_MSPDEINIT_CB_ID :
        hmdf->MspDeInitCallback = HAL_MDF_MspDeInit;
        break;
      default :
        /* Update error code and status */
        hmdf->ErrorCode |= MDF_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update error code and status */
    hmdf->ErrorCode |= MDF_ERROR_INVALID_CALLBACK;
    status = HAL_ERROR;
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Register specific MDF out-off limit detector callback
  *         to be used instead of the weak predefined callback.
  * @param  hmdf MDF handle.
  * @param  pCallback pointer to the out-off limit detector callback function.
  * @retval HAL status.
  * @note   This function must not be used with ADF instance.
  */
HAL_StatusTypeDef HAL_MDF_RegisterOldCallback(MDF_HandleTypeDef      *hmdf,
                                              pMDF_OldCallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (pCallback == NULL)
  {
    /* Update error code and status */
    hmdf->ErrorCode |= MDF_ERROR_INVALID_CALLBACK;
    status = HAL_ERROR;
  }
  else
  {
    if (hmdf->State == HAL_MDF_STATE_READY)
    {
      hmdf->OldCallback = pCallback;
    }
    else
    {
      /* Update error code and status */
      hmdf->ErrorCode |= MDF_ERROR_INVALID_CALLBACK;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Unregister the specific MDF out-off limit detector callback.
  *         MDF out-off limit detector callback is redirected to the weak predefined callback.
  * @param  hmdf MDF handle.
  * @retval HAL status.
  * @note   This function must not be used with ADF instance.
  */
HAL_StatusTypeDef HAL_MDF_UnRegisterOldCallback(MDF_HandleTypeDef *hmdf)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hmdf->State == HAL_MDF_STATE_READY)
  {
    hmdf->OldCallback = HAL_MDF_OldCallback;
  }
  else
  {
    /* Update error code and status */
    hmdf->ErrorCode |= MDF_ERROR_INVALID_CALLBACK;
    status = HAL_ERROR;
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Register specific MDF sound level callback
  *         to be used instead of the weak predefined callback.
  * @param  hmdf MDF handle.
  * @param  pCallback pointer to the sound level callback function.
  * @retval HAL status.
  * @note   This function must not be used with MDF instance.
  */
HAL_StatusTypeDef HAL_MDF_RegisterSndLvlCallback(MDF_HandleTypeDef         *hmdf,
                                                 pMDF_SndLvlCallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (pCallback == NULL)
  {
    /* Update error code and status */
    hmdf->ErrorCode |= MDF_ERROR_INVALID_CALLBACK;
    status = HAL_ERROR;
  }
  else
  {
    if (hmdf->State == HAL_MDF_STATE_READY)
    {
      hmdf->SndLvCallback = pCallback;
    }
    else
    {
      /* Update error code and status */
      hmdf->ErrorCode |= MDF_ERROR_INVALID_CALLBACK;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Unregister the specific MDF sound level callback.
  *         MDF sound level callback is redirected to the weak predefined callback.
  * @param  hmdf MDF handle.
  * @retval HAL status.
  * @note   This function must not be used with MDF instance.
  */
HAL_StatusTypeDef HAL_MDF_UnRegisterSndLvlCallback(MDF_HandleTypeDef *hmdf)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hmdf->State == HAL_MDF_STATE_READY)
  {
    hmdf->SndLvCallback = HAL_MDF_SndLvlCallback;
  }
  else
  {
    /* Update error code and status */
    hmdf->ErrorCode |= MDF_ERROR_INVALID_CALLBACK;
    status = HAL_ERROR;
  }

  /* Return function status */
  return status;
}
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @defgroup MDF_Exported_Functions_Group2  Acquisition functions
  * @brief    Acquisition functions
  *
@verbatim
  ==============================================================================
                        ##### Acquisition functions #####
  ==============================================================================
    [..]  This section provides functions allowing to :
      (+) Start and stop acquisition in polling, interrupt or DMA mode.
      (+) Wait and get acquisition values.
      (+) Generate pulse on TRGO signal.
      (+) Modify and get some filter parameters during acquisition.
      (+) Wait and get sound level values for ADF instance.
      (+) Detect sound activity for ADF instance.
@endverbatim
  * @{
  */

/**
  * @brief  This function allows to start acquisition in polling mode.
  * @param  hmdf MDF handle.
  * @param  pFilterConfig Filter configuration parameters.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_AcqStart(MDF_HandleTypeDef *hmdf, const MDF_FilterConfigTypeDef *pFilterConfig)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (pFilterConfig == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    if (IS_ADF_INSTANCE(hmdf->Instance))
    {
      assert_param(IS_ADF_ACQUISITION_MODE(pFilterConfig->AcquisitionMode));
      assert_param(IS_FUNCTIONAL_STATE(pFilterConfig->SoundActivity.Activation));
    }
    else
    {
      assert_param(IS_MDF_ACQUISITION_MODE(pFilterConfig->AcquisitionMode));
    }
    if ((IS_ADF_INSTANCE(hmdf->Instance)) && (pFilterConfig->SoundActivity.Activation == ENABLE) &&
        ((pFilterConfig->AcquisitionMode == MDF_MODE_ASYNC_SINGLE) ||
         (pFilterConfig->AcquisitionMode == MDF_MODE_SYNC_SINGLE) ||
         (pFilterConfig->AcquisitionMode == MDF_MODE_WINDOW_CONT)))
    {
      status = HAL_ERROR;
    }
    /* Check state */
    else if (hmdf->State != HAL_MDF_STATE_READY)
    {
      status = HAL_ERROR;
    }
    /* Check filter status */
    else if ((hmdf->Instance->DFLTCR & MDF_DFLTCR_DFLTACTIVE) != 0U)
    {
      status = HAL_ERROR;
    }
    else
    {
      /* For ADF instance, check SAD status */
      if (IS_ADF_INSTANCE(hmdf->Instance))
      {
        if ((hmdf->Instance->SADCR & MDF_SADCR_SADACTIVE) != 0U)
        {
          status = HAL_ERROR;
        }
      }
    }

    if (status == HAL_OK)
    {
      /* For MDF instance, check OLD status and main filter order */
      assert_param(IS_MDF_CIC_MODE(pFilterConfig->CicMode));
      if (IS_MDF_INSTANCE(hmdf->Instance))
      {
        if (((hmdf->Instance->OLDCR & MDF_OLDCR_OLDACTIVE) != 0U) && (pFilterConfig->CicMode >= MDF_ONE_FILTER_SINC4))
        {
          status = HAL_ERROR;
        }
      }

      if (status == HAL_OK)
      {
        /* Configure filter and start acquisition */
        hmdf->Instance->DFLTCR = 0U;
        MDF_AcqStart(hmdf, pFilterConfig);
      }
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to poll for available acquisition value.
  * @param  hmdf MDF handle.
  * @param  Timeout Timeout value in milliseconds.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_PollForAcq(MDF_HandleTypeDef *hmdf, uint32_t Timeout)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check state */
  if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    status = HAL_ERROR;
  }
  else
  {
    uint32_t tickstart = HAL_GetTick();

    /* Wait for available acquisition value */
    while (((hmdf->Instance->DFLTISR & MDF_DFLTISR_RXNEF) != MDF_DFLTISR_RXNEF) && (status == HAL_OK))
    {
      /* Check the timeout */
      if (Timeout != HAL_MAX_DELAY)
      {
        if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
        {
          status = HAL_TIMEOUT;
        }
      }
    }

    /* Check if data overflow, saturation or reshape filter occurs */
    uint32_t error_flags = (hmdf->Instance->DFLTISR & (MDF_DFLTISR_DOVRF | MDF_DFLTISR_SATF | MDF_DFLTISR_RFOVRF));
    if (error_flags != 0U)
    {
      /* Update error code */
      if ((error_flags & MDF_DFLTISR_DOVRF) == MDF_DFLTISR_DOVRF)
      {
        hmdf->ErrorCode |= MDF_ERROR_ACQUISITION_OVERFLOW;
      }
      if ((error_flags & MDF_DFLTISR_SATF) == MDF_DFLTISR_SATF)
      {
        hmdf->ErrorCode |= MDF_ERROR_SATURATION;
      }
      if ((error_flags & MDF_DFLTISR_RFOVRF) == MDF_DFLTISR_RFOVRF)
      {
        hmdf->ErrorCode |= MDF_ERROR_RSF_OVERRUN;
      }

      /* Clear corresponding flags */
      hmdf->Instance->DFLTISR |= error_flags;

      /* Call error callback */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
      hmdf->ErrorCallback(hmdf);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
      HAL_MDF_ErrorCallback(hmdf);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
    }

    if (status == HAL_OK)
    {
      /* Update state only in asynchronous single shot mode */
      if ((hmdf->Instance->DFLTCR & MDF_DFLTCR_ACQMOD) == MDF_MODE_ASYNC_SINGLE)
      {
        hmdf->State = HAL_MDF_STATE_READY;
      }
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to poll for available snapshot acquisition value.
  * @param  hmdf MDF handle.
  * @param  Timeout Timeout value in milliseconds.
  * @retval HAL status.
  * @note   This function must not be used with ADF instance.
  */
HAL_StatusTypeDef HAL_MDF_PollForSnapshotAcq(MDF_HandleTypeDef *hmdf, uint32_t Timeout)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check state */
  if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    status = HAL_ERROR;
  }
  else
  {
    uint32_t tickstart = HAL_GetTick();

    /* Wait for available snapshot acquisition value */
    while (((hmdf->Instance->DFLTISR & MDF_DFLTISR_SSDRF) != MDF_DFLTISR_SSDRF) && (status == HAL_OK))
    {
      /* Check the timeout */
      if (Timeout != HAL_MAX_DELAY)
      {
        if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
        {
          status = HAL_TIMEOUT;
        }
      }
    }

    /* Check if snapshot overrun, saturation or reshape filter occurs */
    uint32_t error_flags = (hmdf->Instance->DFLTISR & (MDF_DFLTISR_SSOVRF | MDF_DFLTISR_SATF | MDF_DFLTISR_RFOVRF));
    if (error_flags != 0U)
    {
      /* Update error code */
      if ((error_flags & MDF_DFLTISR_SSOVRF) == MDF_DFLTISR_SSOVRF)
      {
        hmdf->ErrorCode |= MDF_ERROR_ACQUISITION_OVERFLOW;
      }
      if ((error_flags & MDF_DFLTISR_SATF) == MDF_DFLTISR_SATF)
      {
        hmdf->ErrorCode |= MDF_ERROR_SATURATION;
      }
      if ((error_flags & MDF_DFLTISR_RFOVRF) == MDF_DFLTISR_RFOVRF)
      {
        hmdf->ErrorCode |= MDF_ERROR_RSF_OVERRUN;
      }

      /* Clear corresponding flags */
      hmdf->Instance->DFLTISR |= error_flags;

      /* Call error callback */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
      hmdf->ErrorCallback(hmdf);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
      HAL_MDF_ErrorCallback(hmdf);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to get acquisition value.
  * @param  hmdf MDF handle.
  * @param  pValue Acquisition value on 24 MSB.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_GetAcqValue(const MDF_HandleTypeDef *hmdf, int32_t *pValue)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (pValue == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check state */
    if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
    {
      if (hmdf->State != HAL_MDF_STATE_READY)
      {
        status = HAL_ERROR;
      }
    }
  }

  if (status == HAL_OK)
  {
    /* Get acquisition value */
    *pValue = (int32_t) hmdf->Instance->DFLTDR;
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to get snapshot acquisition value.
  * @param  hmdf MDF handle.
  * @param  pSnapshotParam Snapshot parameters.
  * @retval HAL status.
  * @note   This function must not be used with ADF instance.
  */
HAL_StatusTypeDef HAL_MDF_GetSnapshotAcqValue(MDF_HandleTypeDef *hmdf, MDF_SnapshotParamTypeDef *pSnapshotParam)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (pSnapshotParam == NULL)
  {
    status = HAL_ERROR;
  }
  /* Check state */
  else if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    status = HAL_ERROR;
  }
  else
  {
    uint32_t snpsdr_value;

    /* Read value of snapshot data register */
    snpsdr_value = hmdf->Instance->SNPSDR;

    /* Clear snapshot data ready flag */
    hmdf->Instance->DFLTISR |= MDF_DFLTISR_SSDRF;

    /* Store value of decimation counter in snapshot parameter structure */
    pSnapshotParam->DecimationCounter = (snpsdr_value & MDF_SNPSDR_MCICDC);

    /* Check snapshot format */
    if ((hmdf->Instance->DFLTCR & MDF_SNAPSHOT_16BITS) == MDF_SNAPSHOT_16BITS)
    {
      /* Store value of integrator counter in snapshot parameter structure */
      pSnapshotParam->IntegratorCounter = ((snpsdr_value & MDF_SNPSDR_EXTSDR) >> MDF_SNPSDR_EXTSDR_Pos);

      /* Store snapshot acquisition value (16MSB) in snapshot parameter structure */
      snpsdr_value &= 0xFFFF0000U;
      pSnapshotParam->Value = (int32_t) snpsdr_value;
    }
    else
    {
      /* Store snapshot acquisition value (23MSB) in snapshot parameter structure */
      snpsdr_value &= 0xFFFFFE00U;
      pSnapshotParam->Value = (int32_t) snpsdr_value;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to stop acquisition in polling mode.
  * @param  hmdf MDF handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_AcqStop(MDF_HandleTypeDef *hmdf)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check state */
  if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    /* Check if state is ready and filter active */
    if (hmdf->State == HAL_MDF_STATE_READY)
    {
      if ((hmdf->Instance->DFLTCR & MDF_DFLTCR_DFLTACTIVE) != MDF_DFLTCR_DFLTACTIVE)
      {
        status = HAL_ERROR;
      }
    }
    else
    {
      status = HAL_ERROR;
    }
  }
  else
  {
    /* Disable sound activity detector if needed for ADF instance */
    if (IS_ADF_INSTANCE(hmdf->Instance))
    {
      if ((hmdf->Instance->SADCR & MDF_SADCR_SADACTIVE) != 0U)
      {
        hmdf->Instance->SADCR &= ~(MDF_SADCR_SADEN);
      }
    }
  }

  if (status == HAL_OK)
  {
    /* Disable filter */
    hmdf->Instance->DFLTCR &= ~(MDF_DFLTCR_DFLTEN);

    /* Clear all potential pending flags */
    if (IS_ADF_INSTANCE(hmdf->Instance))
    {
      hmdf->Instance->DFLTISR |= (MDF_DFLTISR_DOVRF | MDF_DFLTISR_SATF | MDF_DFLTISR_RFOVRF |
                                  MDF_DFLTISR_SDDETF | MDF_DFLTISR_SDLVLF);
    }
    else
    {
      hmdf->Instance->DFLTISR |= (MDF_DFLTISR_DOVRF | MDF_DFLTISR_SSDRF | MDF_DFLTISR_SSOVRF |
                                  MDF_DFLTISR_SATF | MDF_DFLTISR_RFOVRF);
    }

    /* Update state */
    hmdf->State = HAL_MDF_STATE_READY;
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to start acquisition in interrupt mode.
  * @param  hmdf MDF handle.
  * @param  pFilterConfig Filter configuration parameters.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_AcqStart_IT(MDF_HandleTypeDef *hmdf, const MDF_FilterConfigTypeDef *pFilterConfig)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (pFilterConfig == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    if (IS_ADF_INSTANCE(hmdf->Instance))
    {
      assert_param(IS_ADF_ACQUISITION_MODE(pFilterConfig->AcquisitionMode));
      assert_param(IS_FUNCTIONAL_STATE(pFilterConfig->SoundActivity.Activation));
    }
    else
    {
      assert_param(IS_MDF_ACQUISITION_MODE(pFilterConfig->AcquisitionMode));
    }
    if ((IS_ADF_INSTANCE(hmdf->Instance)) && (pFilterConfig->SoundActivity.Activation == ENABLE) &&
        ((pFilterConfig->AcquisitionMode == MDF_MODE_ASYNC_SINGLE) ||
         (pFilterConfig->AcquisitionMode == MDF_MODE_SYNC_SINGLE) ||
         (pFilterConfig->AcquisitionMode == MDF_MODE_WINDOW_CONT)))
    {
      status = HAL_ERROR;
    }
    /* Check state */
    else if (hmdf->State != HAL_MDF_STATE_READY)
    {
      status = HAL_ERROR;
    }
    /* Check filter status */
    else if ((hmdf->Instance->DFLTCR & MDF_DFLTCR_DFLTACTIVE) != 0U)
    {
      status = HAL_ERROR;
    }
    else
    {
      /* For ADF instance, check SAD status */
      if (IS_ADF_INSTANCE(hmdf->Instance))
      {
        if ((hmdf->Instance->SADCR & MDF_SADCR_SADACTIVE) != 0U)
        {
          status = HAL_ERROR;
        }
      }
    }

    if (status == HAL_OK)
    {
      /* For MDF instance, check OLD status and main filter order */
      assert_param(IS_MDF_CIC_MODE(pFilterConfig->CicMode));
      if (IS_MDF_INSTANCE(hmdf->Instance))
      {
        if (((hmdf->Instance->OLDCR & MDF_OLDCR_OLDACTIVE) != 0U) && (pFilterConfig->CicMode >= MDF_ONE_FILTER_SINC4))
        {
          status = HAL_ERROR;
        }
      }

      if (status == HAL_OK)
      {
        if (pFilterConfig->AcquisitionMode == MDF_MODE_SYNC_SNAPSHOT)
        {
          /* Enable snapshot overrun and data ready interrupts */
          hmdf->Instance->DFLTIER |= (MDF_DFLTIER_SSOVRIE | MDF_DFLTIER_SSDRIE);
        }
        else
        {
          if ((IS_MDF_INSTANCE(hmdf->Instance)) || (pFilterConfig->SoundActivity.Activation == DISABLE) ||
              (pFilterConfig->SoundActivity.DataMemoryTransfer != MDF_SAD_NO_MEMORY_TRANSFER))
          {
            /* Enable data overflow and fifo threshold interrupts */
            hmdf->Instance->DFLTIER |= (MDF_DFLTIER_DOVRIE | MDF_DFLTIER_FTHIE);
          }
        }

        if (pFilterConfig->ReshapeFilter.Activation == ENABLE)
        {
          /* Enable reshape filter overrun interrupt */
          hmdf->Instance->DFLTIER |= MDF_DFLTIER_RFOVRIE;
        }

        /* Enable saturation interrupt */
        hmdf->Instance->DFLTIER |= MDF_DFLTIER_SATIE;

        if ((IS_ADF_INSTANCE(hmdf->Instance)) && (pFilterConfig->SoundActivity.Activation == ENABLE))
        {
          /* Enable sound level value ready and sound activity detection interrupts */
          assert_param(IS_FUNCTIONAL_STATE(pFilterConfig->SoundActivity.SoundLevelInterrupt));
          hmdf->Instance->DFLTIER |= (pFilterConfig->SoundActivity.SoundLevelInterrupt == ENABLE) ?
                                     (MDF_DFLTIER_SDLVLIE | MDF_DFLTIER_SDDETIE) :
                                     MDF_DFLTIER_SDDETIE;
        }

        /* Configure filter and start acquisition */
        hmdf->Instance->DFLTCR = 0U;
        MDF_AcqStart(hmdf, pFilterConfig);
      }
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to stop acquisition in interrupt mode.
  * @param  hmdf MDF handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_AcqStop_IT(MDF_HandleTypeDef *hmdf)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check state */
  if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    /* Check if state is ready and filter active */
    if (hmdf->State == HAL_MDF_STATE_READY)
    {
      if ((hmdf->Instance->DFLTCR & MDF_DFLTCR_DFLTACTIVE) != MDF_DFLTCR_DFLTACTIVE)
      {
        status = HAL_ERROR;
      }
    }
    else
    {
      status = HAL_ERROR;
    }
  }
  else
  {
    /* Disable sound activity detector if needed for ADF instance */
    if (IS_ADF_INSTANCE(hmdf->Instance))
    {
      if ((hmdf->Instance->SADCR & MDF_SADCR_SADACTIVE) != 0U)
      {
        hmdf->Instance->SADCR &= ~(MDF_SADCR_SADEN);
      }
    }
  }

  if (status == HAL_OK)
  {
    /* Disable filter */
    hmdf->Instance->DFLTCR &= ~(MDF_DFLTCR_DFLTEN);

    /* Disable interrupts and clear all potential pending flags */
    if (IS_ADF_INSTANCE(hmdf->Instance))
    {
      hmdf->Instance->DFLTIER &= ~(MDF_DFLTIER_FTHIE | MDF_DFLTIER_DOVRIE | MDF_DFLTIER_SATIE |
                                   MDF_DFLTIER_RFOVRIE | MDF_DFLTIER_SDDETIE | MDF_DFLTIER_SDLVLIE);
      hmdf->Instance->DFLTISR |= (MDF_DFLTISR_DOVRF | MDF_DFLTISR_SATF | MDF_DFLTISR_RFOVRF |
                                  MDF_DFLTISR_SDDETF | MDF_DFLTISR_SDLVLF);
    }
    else
    {
      hmdf->Instance->DFLTIER &= ~(MDF_DFLTIER_FTHIE | MDF_DFLTIER_DOVRIE | MDF_DFLTIER_SSDRIE |
                                   MDF_DFLTIER_SSOVRIE | MDF_DFLTIER_SATIE | MDF_DFLTIER_RFOVRIE);
      hmdf->Instance->DFLTISR |= (MDF_DFLTISR_DOVRF | MDF_DFLTISR_SSDRF | MDF_DFLTISR_SSOVRF |
                                  MDF_DFLTISR_SATF | MDF_DFLTISR_RFOVRF);
    }

    /* Update state */
    hmdf->State = HAL_MDF_STATE_READY;
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to start acquisition in DMA mode.
  * @param  hmdf MDF handle.
  * @param  pFilterConfig Filter configuration parameters.
  * @param  pDmaConfig DMA configuration parameters.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_AcqStart_DMA(MDF_HandleTypeDef *hmdf, const MDF_FilterConfigTypeDef *pFilterConfig,
                                       const MDF_DmaConfigTypeDef *pDmaConfig)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if ((pFilterConfig == NULL) || (pDmaConfig == NULL))
  {
    status = HAL_ERROR;
  }
  else
  {
    assert_param(IS_FUNCTIONAL_STATE(pDmaConfig->MsbOnly));
    if (IS_ADF_INSTANCE(hmdf->Instance))
    {
      assert_param(IS_ADF_ACQUISITION_MODE(pFilterConfig->AcquisitionMode));
      assert_param(IS_FUNCTIONAL_STATE(pFilterConfig->SoundActivity.Activation));
    }
    else
    {
      assert_param(IS_MDF_ACQUISITION_MODE(pFilterConfig->AcquisitionMode));
    }
    if ((IS_ADF_INSTANCE(hmdf->Instance)) && (pFilterConfig->SoundActivity.Activation == ENABLE) &&
        ((pFilterConfig->AcquisitionMode == MDF_MODE_ASYNC_SINGLE) ||
         (pFilterConfig->AcquisitionMode == MDF_MODE_SYNC_SINGLE) ||
         (pFilterConfig->AcquisitionMode == MDF_MODE_WINDOW_CONT)))
    {
      status = HAL_ERROR;
    }
    else if (pFilterConfig->AcquisitionMode == MDF_MODE_SYNC_SNAPSHOT)
    {
      status = HAL_ERROR;
    }
    /* Check state */
    else if (hmdf->State != HAL_MDF_STATE_READY)
    {
      status = HAL_ERROR;
    }
    /* Check filter status */
    else if ((hmdf->Instance->DFLTCR & MDF_DFLTCR_DFLTACTIVE) != 0U)
    {
      status = HAL_ERROR;
    }
    else
    {
      /* For ADF instance, check SAD status */
      if (IS_ADF_INSTANCE(hmdf->Instance))
      {
        if ((hmdf->Instance->SADCR & MDF_SADCR_SADACTIVE) != 0U)
        {
          status = HAL_ERROR;
        }
      }
    }

    if (status == HAL_OK)
    {
      /* For MDF instance, check OLD status and main filter order */
      assert_param(IS_MDF_CIC_MODE(pFilterConfig->CicMode));
      if (IS_MDF_INSTANCE(hmdf->Instance))
      {
        if (((hmdf->Instance->OLDCR & MDF_OLDCR_OLDACTIVE) != 0U) && (pFilterConfig->CicMode >= MDF_ONE_FILTER_SINC4))
        {
          status = HAL_ERROR;
        }
      }

      if (status == HAL_OK)
      {
        uint32_t SrcAddress;

        if (pFilterConfig->ReshapeFilter.Activation == ENABLE)
        {
          /* Enable reshape filter overrun interrupt */
          hmdf->Instance->DFLTIER |= MDF_DFLTIER_RFOVRIE;
        }

        /* Enable saturation interrupt */
        hmdf->Instance->DFLTIER |= MDF_DFLTIER_SATIE;

        if ((IS_ADF_INSTANCE(hmdf->Instance)) && (pFilterConfig->SoundActivity.Activation == ENABLE))
        {
          /* Enable sound level value ready and sound activity detection interrupts */
          assert_param(IS_FUNCTIONAL_STATE(pFilterConfig->SoundActivity.SoundLevelInterrupt));
          hmdf->Instance->DFLTIER |= (pFilterConfig->SoundActivity.SoundLevelInterrupt == ENABLE) ?
                                     (MDF_DFLTIER_SDLVLIE | MDF_DFLTIER_SDDETIE) :
                                     MDF_DFLTIER_SDDETIE;
        }

        /* Enable MDF DMA requests */
        hmdf->Instance->DFLTCR = MDF_DFLTCR_DMAEN;

        /* Start DMA transfer */
        hmdf->hdma->XferCpltCallback     = MDF_DmaXferCpltCallback;
        hmdf->hdma->XferHalfCpltCallback = MDF_DmaXferHalfCpltCallback;
        hmdf->hdma->XferErrorCallback    = MDF_DmaErrorCallback;
        SrcAddress = (pDmaConfig->MsbOnly == ENABLE) ? (((uint32_t) &hmdf->Instance->DFLTDR) + 2U) :
                     (uint32_t) &hmdf->Instance->DFLTDR;
        if ((hmdf->hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
        {
          if (hmdf->hdma->LinkedListQueue != NULL)
          {
            hmdf->hdma->LinkedListQueue->Head->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] = pDmaConfig->DataLength;
            hmdf->hdma->LinkedListQueue->Head->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET] = SrcAddress;
            hmdf->hdma->LinkedListQueue->Head->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] = pDmaConfig->Address;

            status = HAL_DMAEx_List_Start_IT(hmdf->hdma);
          }
          else
          {
            status = HAL_ERROR;
          }
        }
        else
        {
          status = HAL_DMA_Start_IT(hmdf->hdma, SrcAddress, pDmaConfig->Address, pDmaConfig->DataLength);
        }
        if (status != HAL_OK)
        {
          /* Update state */
          hmdf->State = HAL_MDF_STATE_ERROR;
          status = HAL_ERROR;
        }
        else
        {
          /* Configure filter and start acquisition */
          MDF_AcqStart(hmdf, pFilterConfig);
        }
      }
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to stop acquisition in DMA mode.
  * @param  hmdf MDF handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_AcqStop_DMA(MDF_HandleTypeDef *hmdf)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check if state is ready and filter active */
  if (hmdf->State == HAL_MDF_STATE_READY)
  {
    if ((hmdf->Instance->DFLTCR & MDF_DFLTCR_DFLTACTIVE) != MDF_DFLTCR_DFLTACTIVE)
    {
      status = HAL_ERROR;
    }
  }
  else
  {
    /* Check state */
    if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
    {
      status = HAL_ERROR;
    }
    else
    {
      /* Stop the DMA transfer */
      if (HAL_DMA_Abort(hmdf->hdma) != HAL_OK)
      {
        /* Update state */
        hmdf->State = HAL_MDF_STATE_ERROR;
        status = HAL_ERROR;
      }
    }
  }

  if (status == HAL_OK)
  {
    /* Disable sound activity detector if needed for ADF instance */
    if (IS_ADF_INSTANCE(hmdf->Instance))
    {
      if ((hmdf->Instance->SADCR & MDF_SADCR_SADACTIVE) != 0U)
      {
        hmdf->Instance->SADCR &= ~(MDF_SADCR_SADEN);
      }
    }

    /* Disable filter */
    hmdf->Instance->DFLTCR &= ~(MDF_DFLTCR_DFLTEN);

    /* Disable interrupts and clear all potential pending flags */
    if (IS_ADF_INSTANCE(hmdf->Instance))
    {
      hmdf->Instance->DFLTIER &= ~(MDF_DFLTIER_SATIE | MDF_DFLTIER_RFOVRIE | MDF_DFLTIER_SDDETIE |
                                   MDF_DFLTIER_SDLVLIE);
      hmdf->Instance->DFLTISR |= (MDF_DFLTISR_SATF | MDF_DFLTISR_RFOVRF | MDF_DFLTISR_SDDETF |
                                  MDF_DFLTISR_SDLVLF);
    }
    else
    {
      hmdf->Instance->DFLTIER &= ~(MDF_DFLTIER_SATIE | MDF_DFLTIER_RFOVRIE);
      hmdf->Instance->DFLTISR |= (MDF_DFLTISR_SATF | MDF_DFLTISR_RFOVRF);
    }

    /* Disable MDF DMA requests */
    hmdf->Instance->DFLTCR &= ~(MDF_DFLTCR_DMAEN);

    /* Update state */
    hmdf->State = HAL_MDF_STATE_READY;
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to generate pulse on TRGO signal.
  * @param  hmdf MDF handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_GenerateTrgo(const MDF_HandleTypeDef *hmdf)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check state */
  if (hmdf->State != HAL_MDF_STATE_READY)
  {
    if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
    {
      status = HAL_ERROR;
    }
  }

  if (status == HAL_OK)
  {
    MDF_TypeDef *p_mdf_base;

    /* Get MDF base according instance */
    p_mdf_base = (IS_ADF_INSTANCE(hmdf->Instance)) ? ADF1 : MDF1;

    /* Check if trigger output control is already active */
    if ((p_mdf_base->GCR & MDF_GCR_TRGO) == MDF_GCR_TRGO)
    {
      status = HAL_ERROR;
    }
    else
    {
      /* Generate pulse on trigger output control signal */
      p_mdf_base->GCR |= MDF_GCR_TRGO;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to set delay to apply on data source in number of samples.
  * @param  hmdf MDF handle.
  * @param  Delay Delay to apply on data source in number of samples.
  *         This parameter must be a number between Min_Data = 0 and Max_Data = 127.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_SetDelay(MDF_HandleTypeDef *hmdf, uint32_t Delay)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  assert_param(IS_MDF_DELAY(Delay));

  /* Check state */
  if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check if bitstream delay is already active */
    if ((hmdf->Instance->DLYCR & MDF_DLYCR_SKPBF) == MDF_DLYCR_SKPBF)
    {
      status = HAL_ERROR;
    }
    else
    {
      /* Configure bitstream delay */
      hmdf->Instance->DLYCR |= Delay;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to get current delay applied on data source in number of samples.
  * @param  hmdf MDF handle.
  * @param  pDelay Current delay applied on data source in number of samples.
  *         This value is between Min_Data = 0 and Max_Data = 127.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_GetDelay(const MDF_HandleTypeDef *hmdf, uint32_t *pDelay)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (pDelay == NULL)
  {
    status = HAL_ERROR;
  }
  /* Check state */
  else if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Get current bitstream delay */
    *pDelay = (hmdf->Instance->DLYCR & MDF_DLYCR_SKPDLY);
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to set filter gain.
  * @param  hmdf MDF handle.
  * @param  Gain Filter gain in step of around 3db (from -48db to 72dB).
  *         This parameter must be a number between Min_Data = -16 and Max_Data = 24.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_SetGain(MDF_HandleTypeDef *hmdf, int32_t Gain)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  assert_param(IS_MDF_GAIN(Gain));

  /* Check state */
  if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    status = HAL_ERROR;
  }
  else
  {
    uint32_t register_gain_value;
    uint32_t tmp_register;

    if (Gain < 0)
    {
      int32_t adjust_gain;

      /* adjust gain value to set on register for negative value (offset of -16) */
      adjust_gain = Gain - 16;
      register_gain_value = ((uint32_t) adjust_gain & 0x3FU);
    }
    else
    {
      /* for positive value, no offset to apply */
      register_gain_value = (uint32_t) Gain;
    }
    /* Set gain */
    tmp_register = (hmdf->Instance->DFLTCICR & ~(MDF_DFLTCICR_SCALE));
    hmdf->Instance->DFLTCICR = (tmp_register | (register_gain_value << MDF_DFLTCICR_SCALE_Pos));
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to get filter gain.
  * @param  hmdf MDF handle.
  * @param  pGain Filter gain in step of around 3db (from -48db to 72dB).
  *         This parameter is between Min_Data = -16 and Max_Data = 24.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_GetGain(const MDF_HandleTypeDef *hmdf, int32_t *pGain)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (pGain == NULL)
  {
    status = HAL_ERROR;
  }
  /* Check state */
  else if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    status = HAL_ERROR;
  }
  else
  {
    uint32_t register_gain_value;

    /* Get current gain */
    register_gain_value = ((hmdf->Instance->DFLTCICR & MDF_DFLTCICR_SCALE) >> MDF_DFLTCICR_SCALE_Pos);
    if (register_gain_value > 31U)
    {
      /* adjust gain value to set on register for negative value (offset of +16) */
      register_gain_value |= 0xFFFFFFC0U;
      *pGain = (int32_t) register_gain_value + 16;
    }
    else
    {
      /* for positive value, no offset to apply */
      *pGain = (int32_t) register_gain_value;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to set filter offset error compensation.
  * @param  hmdf MDF handle.
  * @param  Offset Filter offset error compensation.
  *         This parameter must be a number between Min_Data = -33554432 and Max_Data = 33554431.
  * @retval HAL status.
  * @note   This function must not be used with ADF instance.
  */
HAL_StatusTypeDef HAL_MDF_SetOffset(MDF_HandleTypeDef *hmdf, int32_t Offset)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  assert_param(IS_MDF_INSTANCE(hmdf->Instance));
  assert_param(IS_MDF_OFFSET(Offset));

  /* Check state */
  if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Set offset */
    hmdf->Instance->OECCR = (uint32_t) Offset;
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to get filter offset error compensation.
  * @param  hmdf MDF handle.
  * @param  pOffset Filter offset error compensation.
  *         This value is between Min_Data = -33554432 and Max_Data = 33554431.
  * @retval HAL status.
  * @note   This function must not be used with ADF instance.
  */
HAL_StatusTypeDef HAL_MDF_GetOffset(const MDF_HandleTypeDef *hmdf, int32_t *pOffset)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  assert_param(IS_MDF_INSTANCE(hmdf->Instance));
  if (pOffset == NULL)
  {
    status = HAL_ERROR;
  }
  /* Check state */
  else if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    status = HAL_ERROR;
  }
  else
  {
    uint32_t register_offset_value;

    /* Get current offset */
    register_offset_value = hmdf->Instance->OECCR;
    if (register_offset_value > 33554431U)
    {
      /* Negative value */
      register_offset_value |= 0xFC000000U;
      *pOffset = (int32_t) register_offset_value;
    }
    else
    {
      /* Positive value */
      *pOffset = (int32_t) register_offset_value;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to poll for sound level data.
  * @param  hmdf MDF handle.
  * @param  Timeout Timeout value in milliseconds.
  * @param  pSoundLevel Sound level.
            This parameter can be a value between Min_Data = 0 and Max_Data = 32767.
  * @param  pAmbientNoise Ambient noise.
            This parameter can be a value between Min_Data = 0 and Max_Data = 32767.
  * @retval HAL status.
  * @note   This function must not be used with MDF instance.
  */
HAL_StatusTypeDef HAL_MDF_PollForSndLvl(MDF_HandleTypeDef *hmdf, uint32_t Timeout, uint32_t *pSoundLevel,
                                        uint32_t *pAmbientNoise)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  assert_param(IS_ADF_INSTANCE(hmdf->Instance));
  if ((pSoundLevel == NULL) || (pAmbientNoise == NULL))
  {
    status = HAL_ERROR;
  }
  /* Check state */
  else if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    status = HAL_ERROR;
  }
  /* Check SAD status */
  else if ((hmdf->Instance->SADCR & MDF_SADCR_SADACTIVE) == 0U)
  {
    status = HAL_ERROR;
  }
  else
  {
    uint32_t tickstart = HAL_GetTick();

    /* Wait for available sound level data */
    while (((hmdf->Instance->DFLTISR & MDF_DFLTISR_SDLVLF) != MDF_DFLTISR_SDLVLF) && (status == HAL_OK))
    {
      /* Check the timeout */
      if (Timeout != HAL_MAX_DELAY)
      {
        if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
        {
          status = HAL_TIMEOUT;
        }
      }
    }

    if (status == HAL_OK)
    {
      /* Get sound level */
      *pSoundLevel = hmdf->Instance->SADSDLVR;

      /* Get ambient noise */
      *pAmbientNoise = hmdf->Instance->SADANLVR;

      /* Clear sound level ready flag */
      hmdf->Instance->DFLTISR |= MDF_DFLTISR_SDLVLF;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to poll for sound activity detection.
  * @param  hmdf MDF handle.
  * @param  Timeout Timeout value in milliseconds.
  * @retval HAL status.
  * @note   This function must not be used with MDF instance.
  */
HAL_StatusTypeDef HAL_MDF_PollForSad(MDF_HandleTypeDef *hmdf, uint32_t Timeout)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  assert_param(IS_ADF_INSTANCE(hmdf->Instance));

  /* Check state */
  if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    status = HAL_ERROR;
  }
  /* Check SAD status */
  else if ((hmdf->Instance->SADCR & MDF_SADCR_SADACTIVE) == 0U)
  {
    status = HAL_ERROR;
  }
  else
  {
    uint32_t tickstart = HAL_GetTick();

    /* Wait for sound activity detection */
    while (((hmdf->Instance->DFLTISR & MDF_DFLTISR_SDDETF) != MDF_DFLTISR_SDDETF) && (status == HAL_OK))
    {
      /* Check the timeout */
      if (Timeout != HAL_MAX_DELAY)
      {
        if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
        {
          status = HAL_TIMEOUT;
        }
      }
    }

    if (status == HAL_OK)
    {
      /* Clear sound activity detection flag */
      hmdf->Instance->DFLTISR |= MDF_DFLTISR_SDDETF;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  MDF acquisition complete callback.
  * @param  hmdf MDF handle.
  * @retval None.
  */
__weak void HAL_MDF_AcqCpltCallback(MDF_HandleTypeDef *hmdf)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdf);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_MDF_AcqCpltCallback could be implemented in the user file */
}

/**
  * @brief  MDF acquisition half complete callback.
  * @param  hmdf MDF handle.
  * @retval None.
  */
__weak void HAL_MDF_AcqHalfCpltCallback(MDF_HandleTypeDef *hmdf)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdf);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_MDF_AcqHalfCpltCallback could be implemented in the user file */
}

/**
  * @brief  MDF sound level callback.
  * @param  hmdf MDF handle.
  * @param  SoundLevel Sound level value computed by sound activity detector.
  *         This parameter can be a value between Min_Data = 0 and Max_Data = 32767.
  * @param  AmbientNoise Ambient noise value computed by sound activity detector.
  *         This parameter can be a value between Min_Data = 0 and Max_Data = 32767.
  * @retval None.
  */
__weak void HAL_MDF_SndLvlCallback(MDF_HandleTypeDef *hmdf, uint32_t SoundLevel, uint32_t AmbientNoise)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdf);
  UNUSED(SoundLevel);
  UNUSED(AmbientNoise);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_MDF_SndLvlCallback could be implemented in the user file */
}

/**
  * @brief  MDF sound activity detector callback.
  * @param  hmdf MDF handle.
  * @retval None.
  */
__weak void HAL_MDF_SadCallback(MDF_HandleTypeDef *hmdf)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdf);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_MDF_SadCallback could be implemented in the user file */
}

/**
  * @}
  */

/** @defgroup MDF_Exported_Functions_Group3  Clock absence detection functions
  * @brief    Clock absence detection functions
  *
@verbatim
  ==============================================================================
                  ##### Clock absence detection functions #####
  ==============================================================================
    [..]  This section provides functions allowing to :
      (+) Start and stop clock absence detection in interrupt mode.
      (+) Detect clock absence.
@endverbatim
  * @{
  */

/**
  * @brief  This function allows to poll for the clock absence detection.
  * @param  hmdf MDF handle.
  * @param  Timeout Timeout value in milliseconds.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_PollForCkab(MDF_HandleTypeDef *hmdf, uint32_t Timeout)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check state */
  if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    if (hmdf->State != HAL_MDF_STATE_READY)
    {
      status = HAL_ERROR;
    }
  }

  if (status == HAL_OK)
  {
    /* Check serial interface status and mode */
    if ((hmdf->Instance->SITFCR & MDF_SITFCR_SITFACTIVE) == 0U)
    {
      status = HAL_ERROR;
    }
    else
    {
      if ((hmdf->Instance->SITFCR & MDF_SITFCR_SITFMOD) != MDF_SITF_NORMAL_SPI_MODE)
      {
        status = HAL_ERROR;
      }
    }
  }

  if (status == HAL_OK)
  {
    uint32_t tickstart = HAL_GetTick();

    /* Wait for clock absence detection */
    while (((hmdf->Instance->DFLTISR & MDF_DFLTISR_CKABF) != MDF_DFLTISR_CKABF) && (status == HAL_OK))
    {
      /* Check the timeout */
      if (Timeout != HAL_MAX_DELAY)
      {
        if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
        {
          status = HAL_TIMEOUT;
        }
      }
    }

    if (status == HAL_OK)
    {
      /* Clear clock absence detection flag */
      hmdf->Instance->DFLTISR |= MDF_DFLTISR_CKABF;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to start clock absence detection in interrupt mode.
  * @param  hmdf MDF handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_CkabStart_IT(MDF_HandleTypeDef *hmdf)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check state */
  if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    if (hmdf->State != HAL_MDF_STATE_READY)
    {
      status = HAL_ERROR;
    }
  }

  if (status == HAL_OK)
  {
    /* Check serial interface status and mode */
    if ((hmdf->Instance->SITFCR & MDF_SITFCR_SITFACTIVE) == 0U)
    {
      status = HAL_ERROR;
    }
    else
    {
      if ((hmdf->Instance->SITFCR & MDF_SITFCR_SITFMOD) != MDF_SITF_NORMAL_SPI_MODE)
      {
        status = HAL_ERROR;
      }
    }
  }

  if (status == HAL_OK)
  {
    /* Clear clock absence detection flag */
    hmdf->Instance->DFLTISR |= MDF_DFLTISR_CKABF;

    /* Check clock absence detection flag */
    if ((hmdf->Instance->DFLTISR & MDF_DFLTISR_CKABF) == MDF_DFLTISR_CKABF)
    {
      status = HAL_ERROR;
    }
    else
    {
      /* Enable clock absence detection interrupt */
      hmdf->Instance->DFLTIER |= MDF_DFLTIER_CKABIE;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to stop clock absence detection in interrupt mode.
  * @param  hmdf MDF handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_MDF_CkabStop_IT(MDF_HandleTypeDef *hmdf)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check state */
  if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    if (hmdf->State != HAL_MDF_STATE_READY)
    {
      status = HAL_ERROR;
    }
  }

  if (status == HAL_OK)
  {
    /* Check serial interface status and mode */
    if ((hmdf->Instance->SITFCR & MDF_SITFCR_SITFACTIVE) == 0U)
    {
      status = HAL_ERROR;
    }
    else
    {
      if ((hmdf->Instance->SITFCR & MDF_SITFCR_SITFMOD) != MDF_SITF_NORMAL_SPI_MODE)
      {
        status = HAL_ERROR;
      }
    }
  }

  if (status == HAL_OK)
  {
    /* Disable clock absence detection interrupt */
    hmdf->Instance->DFLTIER &= ~(MDF_DFLTIER_CKABIE);

    /* Clear potential pending clock absence detection flag */
    hmdf->Instance->DFLTISR |= MDF_DFLTISR_CKABF;
  }

  /* Return function status */
  return status;
}

/**
  * @}
  */

/** @defgroup MDF_Exported_Functions_Group4  Short circuit detection functions
  * @brief    Short circuit detection functions
  *
@verbatim
  ==============================================================================
                  ##### Short circuit detection functions #####
  ==============================================================================
    [..]  This section provides functions available only for MDF instance
          allowing to :
      (+) Start and stop short circuit detection in polling and interrupt mode.
      (+) Detect short circuit.
@endverbatim
  * @{
  */

/**
  * @brief  This function allows to start short-circuit detection in polling mode.
  * @param  hmdf MDF handle.
  * @param  pScdConfig Short-circuit detector configuration parameters.
  * @retval HAL status.
  * @note   This function must not be used with ADF instance.
  */
HAL_StatusTypeDef HAL_MDF_ScdStart(MDF_HandleTypeDef *hmdf, const MDF_ScdConfigTypeDef *pScdConfig)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (pScdConfig == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    assert_param(IS_MDF_INSTANCE(hmdf->Instance));
    assert_param(IS_MDF_SCD_THRESHOLD(pScdConfig->Threshold));
    assert_param(IS_MDF_BREAK_SIGNAL(pScdConfig->BreakSignal));

    /* Check state */
    if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
    {
      if (hmdf->State != HAL_MDF_STATE_READY)
      {
        status = HAL_ERROR;
      }
    }

    if (status == HAL_OK)
    {
      /* Check short-circuit detector status */
      if ((hmdf->Instance->SCDCR & MDF_SCDCR_SCDACTIVE) == MDF_SCDCR_SCDACTIVE)
      {
        status = HAL_ERROR;
      }
      else
      {
        /* Configure threshold and break signal */
        hmdf->Instance->SCDCR = (((pScdConfig->Threshold - 1U) << MDF_SCDCR_SCDT_Pos) |
                                 (pScdConfig->BreakSignal << MDF_SCDCR_BKSCD_Pos));

        /* Enable short-circuit detector */
        hmdf->Instance->SCDCR |= MDF_SCDCR_SCDEN;
      }
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to poll for the short-circuit detection.
  * @param  hmdf MDF handle.
  * @param  Timeout Timeout value in milliseconds.
  * @retval HAL status.
  * @note   This function must not be used with ADF instance.
  */
HAL_StatusTypeDef HAL_MDF_PollForScd(MDF_HandleTypeDef *hmdf, uint32_t Timeout)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  assert_param(IS_MDF_INSTANCE(hmdf->Instance));

  /* Check state */
  if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    if (hmdf->State != HAL_MDF_STATE_READY)
    {
      status = HAL_ERROR;
    }
  }

  if (status == HAL_OK)
  {
    /* Check short-circuit detector status */
    if ((hmdf->Instance->SCDCR & MDF_SCDCR_SCDACTIVE) != MDF_SCDCR_SCDACTIVE)
    {
      status = HAL_ERROR;
    }
    else
    {
      uint32_t tickstart = HAL_GetTick();

      /* Wait for short-circuit detection */
      while (((hmdf->Instance->DFLTISR & MDF_DFLTISR_SCDF) != MDF_DFLTISR_SCDF) && (status == HAL_OK))
      {
        /* Check the timeout */
        if (Timeout != HAL_MAX_DELAY)
        {
          if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
          {
            status = HAL_TIMEOUT;
          }
        }
      }

      if (status == HAL_OK)
      {
        /* Clear short-circuit detection flag */
        hmdf->Instance->DFLTISR |= MDF_DFLTISR_SCDF;
      }
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to stop short-circuit detection in polling mode.
  * @param  hmdf MDF handle.
  * @retval HAL status.
  * @note   This function must not be used with ADF instance.
  */
HAL_StatusTypeDef HAL_MDF_ScdStop(MDF_HandleTypeDef *hmdf)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  assert_param(IS_MDF_INSTANCE(hmdf->Instance));

  /* Check state */
  if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    if (hmdf->State != HAL_MDF_STATE_READY)
    {
      status = HAL_ERROR;
    }
  }

  if (status == HAL_OK)
  {
    /* Check short-circuit detector status */
    if ((hmdf->Instance->SCDCR & MDF_SCDCR_SCDACTIVE) != MDF_SCDCR_SCDACTIVE)
    {
      status = HAL_ERROR;
    }
    else
    {
      /* Disable short-circuit detection */
      hmdf->Instance->SCDCR &= ~(MDF_SCDCR_SCDEN);

      /* Clear potential pending short-circuit detection flag */
      hmdf->Instance->DFLTISR |= MDF_DFLTISR_SCDF;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to start short-circuit detection in interrupt mode.
  * @param  hmdf MDF handle.
  * @param  pScdConfig Short-circuit detector configuration parameters.
  * @retval HAL status.
  * @note   This function must not be used with ADF instance.
  */
HAL_StatusTypeDef HAL_MDF_ScdStart_IT(MDF_HandleTypeDef *hmdf, const MDF_ScdConfigTypeDef *pScdConfig)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (pScdConfig == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    assert_param(IS_MDF_INSTANCE(hmdf->Instance));
    assert_param(IS_MDF_SCD_THRESHOLD(pScdConfig->Threshold));
    assert_param(IS_MDF_BREAK_SIGNAL(pScdConfig->BreakSignal));

    /* Check state */
    if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
    {
      if (hmdf->State != HAL_MDF_STATE_READY)
      {
        status = HAL_ERROR;
      }
    }

    if (status == HAL_OK)
    {
      /* Check short-circuit detector status */
      if ((hmdf->Instance->SCDCR & MDF_SCDCR_SCDACTIVE) == MDF_SCDCR_SCDACTIVE)
      {
        status = HAL_ERROR;
      }
      else
      {
        /* Configure threshold and break signal */
        hmdf->Instance->SCDCR = (((pScdConfig->Threshold - 1U) << MDF_SCDCR_SCDT_Pos) |
                                 (pScdConfig->BreakSignal << MDF_SCDCR_BKSCD_Pos));

        /* Enable short-circuit detector interrupt */
        hmdf->Instance->DFLTIER |= MDF_DFLTIER_SCDIE;

        /* Enable short-circuit detector */
        hmdf->Instance->SCDCR |= MDF_SCDCR_SCDEN;
      }
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to stop short-circuit detection in interrupt mode.
  * @param  hmdf MDF handle.
  * @retval HAL status.
  * @note   This function must not be used with ADF instance.
  */
HAL_StatusTypeDef HAL_MDF_ScdStop_IT(MDF_HandleTypeDef *hmdf)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  assert_param(IS_MDF_INSTANCE(hmdf->Instance));

  /* Check state */
  if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    if (hmdf->State != HAL_MDF_STATE_READY)
    {
      status = HAL_ERROR;
    }
  }

  if (status == HAL_OK)
  {
    /* Check short-circuit detector status */
    if ((hmdf->Instance->SCDCR & MDF_SCDCR_SCDACTIVE) != MDF_SCDCR_SCDACTIVE)
    {
      status = HAL_ERROR;
    }
    else
    {
      /* Disable short-circuit detection */
      hmdf->Instance->SCDCR &= ~(MDF_SCDCR_SCDEN);

      /* Disable short-circuit detection interrupt */
      hmdf->Instance->DFLTIER &= ~(MDF_DFLTIER_SCDIE);

      /* Clear potential pending short-circuit detection flag */
      hmdf->Instance->DFLTISR |= MDF_DFLTISR_SCDF;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @}
  */

/** @defgroup MDF_Exported_Functions_Group5  Out-off limit detection functions
  * @brief    Out-off limit detection functions
  *
@verbatim
  ==============================================================================
                  ##### Out-off limit detection functions #####
  ==============================================================================
    [..]  This section provides functions available only for MDF instance
          allowing to :
      (+) Start and stop out-off limit detection in polling and interrupt mode.
      (+) Detect short circuit and get threshold information.
@endverbatim
  * @{
  */

/**
  * @brief  This function allows to start out-off limit detection in polling mode.
  * @param  hmdf MDF handle.
  * @param  pOldConfig Out-off limit detector configuration parameters.
  * @retval HAL status.
  * @note   This function must not be used with ADF instance.
  */
HAL_StatusTypeDef HAL_MDF_OldStart(MDF_HandleTypeDef *hmdf, const MDF_OldConfigTypeDef *pOldConfig)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (pOldConfig == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    assert_param(IS_MDF_INSTANCE(hmdf->Instance));
    assert_param(IS_MDF_OLD_CIC_MODE(pOldConfig->OldCicMode));
    assert_param(IS_MDF_OLD_DECIMATION_RATIO(pOldConfig->OldDecimationRatio));
    assert_param(IS_MDF_OLD_THRESHOLD(pOldConfig->HighThreshold));
    assert_param(IS_MDF_OLD_THRESHOLD(pOldConfig->LowThreshold));
    assert_param(IS_MDF_OLD_EVENT_CONFIG(pOldConfig->OldEventConfig));
    assert_param(IS_MDF_BREAK_SIGNAL(pOldConfig->BreakSignal));
    if (pOldConfig->LowThreshold >= pOldConfig->HighThreshold)
    {
      status = HAL_ERROR;
    }
    else
    {
      /* Check state */
      if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
      {
        if (hmdf->State != HAL_MDF_STATE_READY)
        {
          status = HAL_ERROR;
        }
      }
    }

    if (status == HAL_OK)
    {
      /* Check out-off limit detector status */
      if ((hmdf->Instance->OLDCR & MDF_OLDCR_OLDACTIVE) == MDF_OLDCR_OLDACTIVE)
      {
        status = HAL_ERROR;
      }
      else
      {
        /* Check filter status */
        if ((hmdf->Instance->DFLTCR & MDF_DFLTCR_DFLTACTIVE) == MDF_DFLTCR_DFLTACTIVE)
        {
          /* Check main filter order */
          if ((hmdf->Instance->DFLTCICR & MDF_DFLTCICR_CICMOD) >= MDF_ONE_FILTER_SINC4)
          {
            status = HAL_ERROR;
          }
        }
        else
        {
          /* Reset main filter order */
          hmdf->Instance->DFLTCICR &= ~(MDF_DFLTCICR_CICMOD);
        }

        if (status == HAL_OK)
        {
          /* Configure OLD CIC mode, decimation ratio, event and break signal */
          hmdf->Instance->OLDCR = (pOldConfig->OldCicMode | pOldConfig->OldEventConfig |
                                   ((pOldConfig->OldDecimationRatio - 1U) << MDF_OLDCR_ACICD_Pos) |
                                   (pOldConfig->BreakSignal << MDF_OLDCR_BKOLD_Pos));

          /* Configure low and high thresholds */
          hmdf->Instance->OLDTHLR = (uint32_t) pOldConfig->LowThreshold;
          hmdf->Instance->OLDTHHR = (uint32_t) pOldConfig->HighThreshold;

          /* Enable out-off limit detector */
          hmdf->Instance->OLDCR |= MDF_OLDCR_OLDEN;
        }
      }
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to poll for the out-off limit detection.
  * @param  hmdf MDF handle.
  * @param  Timeout Timeout value in milliseconds.
  * @param  pThresholdInfo Threshold information of out-off limit detection.
  *         This parameter can be a value of @ref MDF_OldThresholdInfo.
  * @retval HAL status.
  * @note   This function must not be used with ADF instance.
  */
HAL_StatusTypeDef HAL_MDF_PollForOld(MDF_HandleTypeDef *hmdf, uint32_t Timeout, uint32_t *pThresholdInfo)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  assert_param(IS_MDF_INSTANCE(hmdf->Instance));
  if (pThresholdInfo == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check state */
    if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
    {
      if (hmdf->State != HAL_MDF_STATE_READY)
      {
        status = HAL_ERROR;
      }
    }
  }

  if (status == HAL_OK)
  {
    /* Check out-off limit detector status */
    if ((hmdf->Instance->OLDCR & MDF_OLDCR_OLDACTIVE) != MDF_OLDCR_OLDACTIVE)
    {
      status = HAL_ERROR;
    }
    else
    {
      uint32_t tickstart = HAL_GetTick();

      /* Wait for out-off limit detection */
      while (((hmdf->Instance->DFLTISR & MDF_DFLTISR_OLDF) != MDF_DFLTISR_OLDF) && (status == HAL_OK))
      {
        /* Check the timeout */
        if (Timeout != HAL_MAX_DELAY)
        {
          if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
          {
            status = HAL_TIMEOUT;
          }
        }
      }

      if (status == HAL_OK)
      {
        /* Get threshold information */
        if ((hmdf->Instance->DFLTISR & (MDF_DFLTISR_THLF | MDF_DFLTISR_THHF)) == 0U)
        {
          *pThresholdInfo = MDF_OLD_IN_THRESHOLDS;
        }
        else if ((hmdf->Instance->DFLTISR & MDF_DFLTISR_THLF) == MDF_DFLTISR_THLF)
        {
          *pThresholdInfo = MDF_OLD_LOW_THRESHOLD;
        }
        else
        {
          *pThresholdInfo = MDF_OLD_HIGH_THRESHOLD;
        }

        /* Clear out-off limit detection flags */
        hmdf->Instance->DFLTISR |= MDF_DFLTISR_OLDF;
      }
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to stop out-off limit detection in polling mode.
  * @param  hmdf MDF handle.
  * @retval HAL status.
  * @note   This function must not be used with ADF instance.
  */
HAL_StatusTypeDef HAL_MDF_OldStop(MDF_HandleTypeDef *hmdf)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  assert_param(IS_MDF_INSTANCE(hmdf->Instance));

  /* Check state */
  if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    if (hmdf->State != HAL_MDF_STATE_READY)
    {
      status = HAL_ERROR;
    }
  }

  if (status == HAL_OK)
  {
    /* Check out-off limit detector status */
    if ((hmdf->Instance->OLDCR & MDF_OLDCR_OLDACTIVE) != MDF_OLDCR_OLDACTIVE)
    {
      status = HAL_ERROR;
    }
    else
    {
      /* Disable out-off limit detection */
      hmdf->Instance->OLDCR &= ~(MDF_OLDCR_OLDEN);

      /* Clear potential pending out-off limit detection flags */
      hmdf->Instance->DFLTISR |= MDF_DFLTISR_OLDF;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to start out-off limit detection in interrupt mode.
  * @param  hmdf MDF handle.
  * @param  pOldConfig Out-off limit detector configuration parameters.
  * @retval HAL status.
  * @note   This function must not be used with ADF instance.
  */
HAL_StatusTypeDef HAL_MDF_OldStart_IT(MDF_HandleTypeDef *hmdf, const MDF_OldConfigTypeDef *pOldConfig)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (pOldConfig == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    assert_param(IS_MDF_INSTANCE(hmdf->Instance));
    assert_param(IS_MDF_OLD_CIC_MODE(pOldConfig->OldCicMode));
    assert_param(IS_MDF_OLD_DECIMATION_RATIO(pOldConfig->OldDecimationRatio));
    assert_param(IS_MDF_OLD_THRESHOLD(pOldConfig->HighThreshold));
    assert_param(IS_MDF_OLD_THRESHOLD(pOldConfig->LowThreshold));
    assert_param(IS_MDF_OLD_EVENT_CONFIG(pOldConfig->OldEventConfig));
    assert_param(IS_MDF_BREAK_SIGNAL(pOldConfig->BreakSignal));
    if (pOldConfig->LowThreshold >= pOldConfig->HighThreshold)
    {
      status = HAL_ERROR;
    }
    else
    {
      /* Check state */
      if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
      {
        if (hmdf->State != HAL_MDF_STATE_READY)
        {
          status = HAL_ERROR;
        }
      }
    }

    if (status == HAL_OK)
    {
      /* Check out-off limit detector status */
      if ((hmdf->Instance->OLDCR & MDF_OLDCR_OLDACTIVE) == MDF_OLDCR_OLDACTIVE)
      {
        status = HAL_ERROR;
      }
      else
      {
        /* Check filter status */
        if ((hmdf->Instance->DFLTCR & MDF_DFLTCR_DFLTACTIVE) == MDF_DFLTCR_DFLTACTIVE)
        {
          /* Check main filter order */
          if ((hmdf->Instance->DFLTCICR & MDF_DFLTCICR_CICMOD) >= MDF_ONE_FILTER_SINC4)
          {
            status = HAL_ERROR;
          }
        }
        else
        {
          /* Reset main filter order */
          hmdf->Instance->DFLTCICR &= ~(MDF_DFLTCICR_CICMOD);
        }

        if (status == HAL_OK)
        {
          /* Configure OLD CIC mode, decimation ratio, event and break signal */
          hmdf->Instance->OLDCR = (pOldConfig->OldCicMode | pOldConfig->OldEventConfig |
                                   ((pOldConfig->OldDecimationRatio - 1U) << MDF_OLDCR_ACICD_Pos) |
                                   (pOldConfig->BreakSignal << MDF_OLDCR_BKOLD_Pos));

          /* Configure low and high thresholds */
          hmdf->Instance->OLDTHLR = (uint32_t) pOldConfig->LowThreshold;
          hmdf->Instance->OLDTHHR = (uint32_t) pOldConfig->HighThreshold;

          /* Enable out-off limit detector interrupt */
          hmdf->Instance->DFLTIER |= MDF_DFLTIER_OLDIE;

          /* Enable out-off limit detector */
          hmdf->Instance->OLDCR |= MDF_OLDCR_OLDEN;
        }
      }
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  This function allows to stop out-off limit detection in interrupt mode.
  * @param  hmdf MDF handle.
  * @retval HAL status.
  * @note   This function must not be used with ADF instance.
  */
HAL_StatusTypeDef HAL_MDF_OldStop_IT(MDF_HandleTypeDef *hmdf)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  assert_param(IS_MDF_INSTANCE(hmdf->Instance));

  /* Check state */
  if (hmdf->State != HAL_MDF_STATE_ACQUISITION)
  {
    if (hmdf->State != HAL_MDF_STATE_READY)
    {
      status = HAL_ERROR;
    }
  }

  if (status == HAL_OK)
  {
    /* Check out-off limit detector status */
    if ((hmdf->Instance->OLDCR & MDF_OLDCR_OLDACTIVE) != MDF_OLDCR_OLDACTIVE)
    {
      status = HAL_ERROR;
    }
    else
    {
      /* Disable out-off limit detection */
      hmdf->Instance->OLDCR &= ~(MDF_OLDCR_OLDEN);

      /* Disable out-off limit detector interrupt */
      hmdf->Instance->DFLTIER &= ~(MDF_DFLTIER_OLDIE);

      /* Clear potential pending out-off limit detection flags */
      hmdf->Instance->DFLTISR |= MDF_DFLTISR_OLDF;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  MDF out-off limit detector callback.
  * @param  hmdf MDF handle.
  * @param  ThresholdInfo Threshold information of out-off limit detection.
  *         This parameter can be a value of @ref MDF_OldThresholdInfo.
  * @retval None.
  */
__weak void HAL_MDF_OldCallback(MDF_HandleTypeDef *hmdf, uint32_t ThresholdInfo)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdf);
  UNUSED(ThresholdInfo);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_MDF_OldCallback could be implemented in the user file */
}

/**
  * @}
  */

/** @defgroup MDF_Exported_Functions_Group6  Generic functions
  * @brief    Generic functions
  *
@verbatim
  ==============================================================================
                          ##### Generic functions #####
  ==============================================================================
    [..]  This section provides functions allowing to :
      (+) Handle MDF interrupt.
      (+) Inform user that error occurs.
      (+) Get the current MDF instance state.
      (+) Get the current MDF instance error code.
@endverbatim
  * @{
  */

/**
  * @brief  This function handles the MDF interrupts.
  * @param  hmdf MDF handle.
  * @retval None.
  */
void HAL_MDF_IRQHandler(MDF_HandleTypeDef *hmdf)
{
  uint32_t tmp_reg1;
  uint32_t tmp_reg2;
  uint32_t interrupts;

  /* Read current flags and interrupts and determine which ones occur */
  tmp_reg1 = hmdf->Instance->DFLTIER;
  tmp_reg2 = hmdf->Instance->DFLTISR;
  interrupts = (tmp_reg1 & tmp_reg2);

  /* Check if data overflow occurs */
  if ((interrupts & MDF_DFLTISR_DOVRF) == MDF_DFLTISR_DOVRF)
  {
    /* Clear data overflow flag */
    hmdf->Instance->DFLTISR |= MDF_DFLTISR_DOVRF;

    /* Update error code */
    hmdf->ErrorCode |= MDF_ERROR_ACQUISITION_OVERFLOW;

    /* Call error callback */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
    hmdf->ErrorCallback(hmdf);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
    HAL_MDF_ErrorCallback(hmdf);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
  }
  /* Check if snapshot overrun occurs */
  else if ((interrupts & MDF_DFLTISR_SSOVRF) == MDF_DFLTISR_SSOVRF)
  {
    /* Clear snapshot overrun flag */
    hmdf->Instance->DFLTISR |= MDF_DFLTISR_SSOVRF;

    /* Update error code */
    hmdf->ErrorCode |= MDF_ERROR_ACQUISITION_OVERFLOW;

    /* Call error callback */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
    hmdf->ErrorCallback(hmdf);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
    HAL_MDF_ErrorCallback(hmdf);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
  }
  /* Check if RXFIFO threshold occurs */
  else if ((interrupts & MDF_DFLTISR_FTHF) == MDF_DFLTISR_FTHF)
  {
    /* Call acquisition complete callback */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
    hmdf->AcqCpltCallback(hmdf);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
    HAL_MDF_AcqCpltCallback(hmdf);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */

    /* Update state only in asynchronous single shot mode */
    if ((hmdf->Instance->DFLTCR & MDF_DFLTCR_ACQMOD) == MDF_MODE_ASYNC_SINGLE)
    {
      hmdf->State = HAL_MDF_STATE_READY;
    }
  }
  /* Check if snapshot data ready occurs */
  else if ((interrupts & MDF_DFLTISR_SSDRF) == MDF_DFLTISR_SSDRF)
  {
    /* Clear snapshot data ready flag */
    hmdf->Instance->DFLTISR |= MDF_DFLTISR_SSDRF;

    /* Call acquisition complete callback */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
    hmdf->AcqCpltCallback(hmdf);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
    HAL_MDF_AcqCpltCallback(hmdf);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
  }
  /* Check if reshape filter overrun occurs */
  else if ((interrupts & MDF_DFLTISR_RFOVRF) == MDF_DFLTISR_RFOVRF)
  {
    /* Clear reshape filter overrun flag */
    hmdf->Instance->DFLTISR |= MDF_DFLTISR_RFOVRF;

    /* Update error code */
    hmdf->ErrorCode |= MDF_ERROR_RSF_OVERRUN;

    /* Call error callback */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
    hmdf->ErrorCallback(hmdf);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
    HAL_MDF_ErrorCallback(hmdf);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
  }
  /* Check if clock absence detection occurs */
  else if ((interrupts & MDF_DFLTISR_CKABF) == MDF_DFLTISR_CKABF)
  {
    /* Clear clock absence detection flag */
    hmdf->Instance->DFLTISR |= MDF_DFLTISR_CKABF;

    /* Update error code */
    hmdf->ErrorCode |= MDF_ERROR_CLOCK_ABSENCE;

    /* Call error callback */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
    hmdf->ErrorCallback(hmdf);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
    HAL_MDF_ErrorCallback(hmdf);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
  }
  /* Check if saturation occurs */
  else if ((interrupts & MDF_DFLTISR_SATF) == MDF_DFLTISR_SATF)
  {
    /* Clear saturation flag */
    hmdf->Instance->DFLTISR |= MDF_DFLTISR_SATF;

    /* Update error code */
    hmdf->ErrorCode |= MDF_ERROR_SATURATION;

    /* Call error callback */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
    hmdf->ErrorCallback(hmdf);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
    HAL_MDF_ErrorCallback(hmdf);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
  }
  /* Check if short-circuit detection occurs */
  else if ((interrupts & MDF_DFLTISR_SCDF) == MDF_DFLTISR_SCDF)
  {
    /* Clear short-circuit detection flag */
    hmdf->Instance->DFLTISR |= MDF_DFLTISR_SCDF;

    /* Update error code */
    hmdf->ErrorCode |= MDF_ERROR_SHORT_CIRCUIT;

    /* Call error callback */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
    hmdf->ErrorCallback(hmdf);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
    HAL_MDF_ErrorCallback(hmdf);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
  }
  /* Check if out-off limit detection occurs */
  else if ((interrupts & MDF_DFLTISR_OLDF) == MDF_DFLTISR_OLDF)
  {
    uint32_t threshold_info;

    /* Get threshold information */
    if ((hmdf->Instance->DFLTISR & (MDF_DFLTISR_THLF | MDF_DFLTISR_THHF)) == 0U)
    {
      threshold_info = MDF_OLD_IN_THRESHOLDS;
    }
    else if ((hmdf->Instance->DFLTISR & MDF_DFLTISR_THLF) == MDF_DFLTISR_THLF)
    {
      threshold_info = MDF_OLD_LOW_THRESHOLD;
    }
    else
    {
      threshold_info = MDF_OLD_HIGH_THRESHOLD;
    }

    /* Clear out-off limit detection flag */
    hmdf->Instance->DFLTISR |= MDF_DFLTISR_OLDF;

    /* Update error code */
    hmdf->ErrorCode |= MDF_ERROR_OUT_OFF_LIMIT;

    /* Call out-off limit detection callback */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
    hmdf->OldCallback(hmdf, threshold_info);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
    HAL_MDF_OldCallback(hmdf, threshold_info);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
  }
  /* Check if sound activity detection occurs */
  else if ((interrupts & MDF_DFLTISR_SDDETF) == MDF_DFLTISR_SDDETF)
  {
    /* Clear sound activity detection flag */
    hmdf->Instance->DFLTISR |= MDF_DFLTISR_SDDETF;

    /* Call sound activity detection callback */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
    hmdf->SadCallback(hmdf);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
    HAL_MDF_SadCallback(hmdf);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
  }
  else
  {
    /* Check if sound level ready occurs */
    if ((interrupts & MDF_DFLTISR_SDLVLF) == MDF_DFLTISR_SDLVLF)
    {
      uint32_t sound_level;
      uint32_t ambient_noise;

      /* Get sound level */
      sound_level = hmdf->Instance->SADSDLVR;

      /* Get ambient noise */
      ambient_noise = hmdf->Instance->SADANLVR;

      /* Clear sound level ready flag */
      hmdf->Instance->DFLTISR |= MDF_DFLTISR_SDLVLF;

      /* Call sound level callback */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
      hmdf->SndLvCallback(hmdf, sound_level, ambient_noise);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
      HAL_MDF_SndLvlCallback(hmdf, sound_level, ambient_noise);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
    }
  }
}

/**
  * @brief  MDF error callback.
  * @param  hmdf MDF handle.
  * @retval None.
  */
__weak void HAL_MDF_ErrorCallback(MDF_HandleTypeDef *hmdf)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdf);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_MDF_ErrorCallback could be implemented in the user file */
}

/**
  * @brief  This function allows to get the current MDF state.
  * @param  hmdf MDF handle.
  * @retval MDF state.
  */
HAL_MDF_StateTypeDef HAL_MDF_GetState(const MDF_HandleTypeDef *hmdf)
{
  /* Return MDF state */
  return hmdf->State;
}

/**
  * @brief  This function allows to get the current MDF error.
  * @param  hmdf MDF handle.
  * @retval MDF error code.
  */
uint32_t HAL_MDF_GetError(const MDF_HandleTypeDef *hmdf)
{
  /* Return MDF error code */
  return hmdf->ErrorCode;
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup MDF_Private_Functions
  * @brief      Private functions
  * @{
  */

/**
  * @brief  This function allows to get the handle number from instance.
  * @param  pInstance MDF instance.
  * @retval Instance number.
  */
static uint32_t MDF_GetHandleNumberFromInstance(const MDF_Filter_TypeDef *const pInstance)
{
  uint32_t handle_number;

  /* Get handle number from instance */
  if (pInstance == MDF1_Filter0)
  {
    handle_number = 0U;
  }
  else if (pInstance == MDF1_Filter1)
  {
    handle_number = 1U;
  }
  else if (pInstance == MDF1_Filter2)
  {
    handle_number = 2U;
  }
  else if (pInstance == MDF1_Filter3)
  {
    handle_number = 3U;
  }
  else if (pInstance == MDF1_Filter4)
  {
    handle_number = 4U;
  }
  else if (pInstance == MDF1_Filter5)
  {
    handle_number = 5U;
  }
  else /* ADF1_Filter0 */
  {
    handle_number = 6U;
  }

  return handle_number;
}

/**
  * @brief  This function allows to configure filter and start acquisition.
  * @param  hmdf MDF handle.
  * @param  pFilterConfig Filter configuration parameters.
  * @retval None.
  */
static void MDF_AcqStart(MDF_HandleTypeDef *const hmdf, const MDF_FilterConfigTypeDef *const pFilterConfig)
{
  uint32_t register_gain_value;

  /* Configure acquisition mode, discard samples, trigger and fifo threshold */
  assert_param(IS_MDF_DISCARD_SAMPLES(pFilterConfig->DiscardSamples));
  assert_param(IS_MDF_FIFO_THRESHOLD(pFilterConfig->FifoThreshold));
  if ((pFilterConfig->AcquisitionMode == MDF_MODE_ASYNC_CONT) ||
      (pFilterConfig->AcquisitionMode == MDF_MODE_ASYNC_SINGLE))
  {
    /* Trigger parameters are not used */
    hmdf->Instance->DFLTCR |= (pFilterConfig->AcquisitionMode | pFilterConfig->FifoThreshold |
                               (pFilterConfig->DiscardSamples << MDF_DFLTCR_NBDIS_Pos));
  }
  else
  {
    /* Trigger parameters are used */
    if (IS_ADF_INSTANCE(hmdf->Instance))
    {
      assert_param(IS_ADF_TRIGGER_SOURCE(pFilterConfig->Trigger.Source));
    }
    else
    {
      assert_param(IS_MDF_TRIGGER_SOURCE(pFilterConfig->Trigger.Source));
    }
    assert_param(IS_MDF_TRIGGER_EDGE(pFilterConfig->Trigger.Edge));
    hmdf->Instance->DFLTCR |= (pFilterConfig->AcquisitionMode | pFilterConfig->FifoThreshold |
                               pFilterConfig->Trigger.Source | pFilterConfig->Trigger.Edge |
                               (pFilterConfig->DiscardSamples << MDF_DFLTCR_NBDIS_Pos));
  }

  /* Configure if needed snapshot format only for MDF instance */
  if (IS_MDF_INSTANCE(hmdf->Instance) && (pFilterConfig->AcquisitionMode == MDF_MODE_SYNC_SNAPSHOT))
  {
    assert_param(IS_MDF_SNAPSHOT_FORMAT(pFilterConfig->SnapshotFormat));
    hmdf->Instance->DFLTCR |= pFilterConfig->SnapshotFormat;
  }

  /* Configure data source, CIC mode, decimation ratio and gain */
  if (IS_ADF_INSTANCE(hmdf->Instance))
  {
    assert_param(IS_ADF_DATA_SOURCE(pFilterConfig->DataSource));
    assert_param(IS_ADF_CIC_MODE(pFilterConfig->CicMode));
  }
  else
  {
    assert_param(IS_MDF_DATA_SOURCE(pFilterConfig->DataSource));
  }
  assert_param(IS_MDF_DECIMATION_RATIO(pFilterConfig->DecimationRatio));
  assert_param(IS_MDF_GAIN(pFilterConfig->Gain));
  if (pFilterConfig->Gain < 0)
  {
    int32_t adjust_gain;

    /* adjust gain value to set on register for negative value (offset of -16) */
    adjust_gain = pFilterConfig->Gain - 16;
    register_gain_value = ((uint32_t) adjust_gain & 0x3FU);
  }
  else
  {
    /* for positive value, no offset to apply */
    register_gain_value = (uint32_t) pFilterConfig->Gain;
  }
  hmdf->Instance->DFLTCICR = (pFilterConfig->DataSource | pFilterConfig->CicMode |
                              ((pFilterConfig->DecimationRatio - 1U) << MDF_DFLTCICR_MCICD_Pos) |
                              (register_gain_value << MDF_DFLTCICR_SCALE_Pos));

  /* Configure bitstream delay */
  assert_param(IS_MDF_DELAY(pFilterConfig->Delay));
  hmdf->Instance->DLYCR = pFilterConfig->Delay;

  /* Configure offset compensation only for MDF instance */
  if (IS_MDF_INSTANCE(hmdf->Instance))
  {
    assert_param(IS_MDF_OFFSET(pFilterConfig->Offset));
    hmdf->Instance->OECCR = (uint32_t) pFilterConfig->Offset;
  }

  /* Configure reshape filter */
  assert_param(IS_FUNCTIONAL_STATE(pFilterConfig->ReshapeFilter.Activation));
  hmdf->Instance->DFLTRSFR = 0U;
  if (pFilterConfig->ReshapeFilter.Activation == ENABLE)
  {
    /* Configure reshape filter decimation ratio */
    assert_param(IS_MDF_RSF_DECIMATION_RATIO(pFilterConfig->ReshapeFilter.DecimationRatio));
    hmdf->Instance->DFLTRSFR |= pFilterConfig->ReshapeFilter.DecimationRatio;
  }
  else
  {
    /* Bypass reshape filter */
    hmdf->Instance->DFLTRSFR |= MDF_DFLTRSFR_RSFLTBYP;
  }

  /* Configure high-pass filter */
  assert_param(IS_FUNCTIONAL_STATE(pFilterConfig->HighPassFilter.Activation));
  if (pFilterConfig->HighPassFilter.Activation == ENABLE)
  {
    /* Configure high-pass filter cut-off frequency */
    assert_param(IS_MDF_HPF_CUTOFF_FREQ(pFilterConfig->HighPassFilter.CutOffFrequency));
    hmdf->Instance->DFLTRSFR |= pFilterConfig->HighPassFilter.CutOffFrequency;
  }
  else
  {
    /* Bypass high-pass filter */
    hmdf->Instance->DFLTRSFR |= MDF_DFLTRSFR_HPFBYP;
  }

  /* Configure integrator only for MDF instance */
  if (IS_MDF_INSTANCE(hmdf->Instance))
  {
    assert_param(IS_FUNCTIONAL_STATE(pFilterConfig->Integrator.Activation));
    if (pFilterConfig->Integrator.Activation == ENABLE)
    {
      /* Configure integrator value and output division */
      assert_param(IS_MDF_INTEGRATOR_VALUE(pFilterConfig->Integrator.Value));
      assert_param(IS_MDF_INTEGRATOR_OUTPUT_DIV(pFilterConfig->Integrator.OutputDivision));
      hmdf->Instance->DFLTINTR = (((pFilterConfig->Integrator.Value - 1U) << MDF_DFLTINTR_INTVAL_Pos) |
                                  pFilterConfig->Integrator.OutputDivision);
    }
    else
    {
      /* Bypass integrator */
      hmdf->Instance->DFLTINTR = 0U;
    }
  }

  if (IS_ADF_INSTANCE(hmdf->Instance))
  {
    assert_param(IS_FUNCTIONAL_STATE(pFilterConfig->SoundActivity.Activation));
    if (pFilterConfig->SoundActivity.Activation == ENABLE)
    {
      /* Configure SAD mode, frame size, hysteresis, sound trigger event
         and data memory transfer only for ADF instance */
      assert_param(IS_MDF_SAD_MODE(pFilterConfig->SoundActivity.Mode));
      assert_param(IS_MDF_SAD_FRAME_SIZE(pFilterConfig->SoundActivity.FrameSize));
      if (pFilterConfig->SoundActivity.Mode != MDF_SAD_AMBIENT_NOISE_DETECTOR)
      {
        assert_param(IS_FUNCTIONAL_STATE(pFilterConfig->SoundActivity.Hysteresis));
      }
      assert_param(IS_MDF_SAD_SOUND_TRIGGER(pFilterConfig->SoundActivity.SoundTriggerEvent));
      assert_param(IS_MDF_SAD_DATA_MEMORY_TRANSFER(pFilterConfig->SoundActivity.DataMemoryTransfer));
      if ((pFilterConfig->SoundActivity.Mode != MDF_SAD_AMBIENT_NOISE_DETECTOR) &&
          (pFilterConfig->SoundActivity.Hysteresis == ENABLE))
      {
        hmdf->Instance->SADCR = (pFilterConfig->SoundActivity.Mode | pFilterConfig->SoundActivity.FrameSize |
                                 MDF_SADCR_HYSTEN | pFilterConfig->SoundActivity.SoundTriggerEvent |
                                 pFilterConfig->SoundActivity.DataMemoryTransfer);
      }
      else
      {
        hmdf->Instance->SADCR = (pFilterConfig->SoundActivity.Mode | pFilterConfig->SoundActivity.FrameSize |
                                 pFilterConfig->SoundActivity.SoundTriggerEvent |
                                 pFilterConfig->SoundActivity.DataMemoryTransfer);
      }

      /* Configure SAD minimum noise level, hangover window, learning frames,
         ambient noise slope control and signal noise threshold only for ADF instance */
      assert_param(IS_MDF_SAD_MIN_NOISE_LEVEL(pFilterConfig->SoundActivity.MinNoiseLevel));
      assert_param(IS_MDF_SAD_HANGOVER_WINDOW(pFilterConfig->SoundActivity.HangoverWindow));
      assert_param(IS_MDF_SAD_LEARNING_FRAMES(pFilterConfig->SoundActivity.LearningFrames));
      assert_param(IS_MDF_SAD_SIGNAL_NOISE_THRESHOLD(pFilterConfig->SoundActivity.SignalNoiseThreshold));
      if (pFilterConfig->SoundActivity.Mode != MDF_SAD_SOUND_DETECTOR)
      {
        assert_param(IS_MDF_SAD_AMBIENT_NOISE_SLOPE(pFilterConfig->SoundActivity.AmbientNoiseSlope));
        hmdf->Instance->SADCFGR = ((pFilterConfig->SoundActivity.MinNoiseLevel << MDF_SADCFGR_ANMIN_Pos) |
                                   pFilterConfig->SoundActivity.HangoverWindow |
                                   pFilterConfig->SoundActivity.LearningFrames |
                                   (pFilterConfig->SoundActivity.AmbientNoiseSlope << MDF_SADCFGR_ANSLP_Pos) |
                                   pFilterConfig->SoundActivity.SignalNoiseThreshold);
      }
      else
      {
        hmdf->Instance->SADCFGR = ((pFilterConfig->SoundActivity.MinNoiseLevel << MDF_SADCFGR_ANMIN_Pos) |
                                   pFilterConfig->SoundActivity.HangoverWindow |
                                   pFilterConfig->SoundActivity.LearningFrames |
                                   pFilterConfig->SoundActivity.SignalNoiseThreshold);
      }
    }
    else
    {
      /* SAD is not used */
      hmdf->Instance->SADCR = 0U;
      hmdf->Instance->SADCFGR = 0U;
    }
  }

  /* Update instance state */
  hmdf->State = HAL_MDF_STATE_ACQUISITION;

  /* Enable sound activity detector if needed only for ADF instance */
  if ((IS_ADF_INSTANCE(hmdf->Instance)) && (pFilterConfig->SoundActivity.Activation == ENABLE))
  {
    hmdf->Instance->SADCR |= MDF_SADCR_SADEN;
  }

  /* Enable filter */
  hmdf->Instance->DFLTCR |= MDF_DFLTCR_DFLTEN;
}

/**
  * @brief  This function handles DMA transfer complete callback.
  * @param  hdma DMA handle.
  * @retval None.
  */
static void MDF_DmaXferCpltCallback(DMA_HandleTypeDef *hdma)
{
  MDF_HandleTypeDef *hmdf = (MDF_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  /* Check if DMA in circular mode */
  if (hdma->Mode != DMA_LINKEDLIST_CIRCULAR)
  {
    hmdf->State = HAL_MDF_STATE_READY;
  }

#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
  hmdf->AcqCpltCallback(hmdf);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
  HAL_MDF_AcqCpltCallback(hmdf);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
}

/**
  * @brief  This function handles DMA half transfer complete callback.
  * @param  hdma DMA handle.
  * @retval None.
  */
static void MDF_DmaXferHalfCpltCallback(DMA_HandleTypeDef *hdma)
{
  MDF_HandleTypeDef *hmdf = (MDF_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
  hmdf->AcqHalfCpltCallback(hmdf);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
  HAL_MDF_AcqHalfCpltCallback(hmdf);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
}

/**
  * @brief  This function handles DMA error callback.
  * @param  hdma DMA handle.
  * @retval None.
  */
static void MDF_DmaErrorCallback(DMA_HandleTypeDef *hdma)
{
  MDF_HandleTypeDef *hmdf = (MDF_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  /* Update error code */
  hmdf->ErrorCode |= MDF_ERROR_DMA;

#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
  hmdf->ErrorCallback(hmdf);
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
  HAL_MDF_ErrorCallback(hmdf);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
}

/**
  * @}
  */

#endif /* HAL_MDF_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */
