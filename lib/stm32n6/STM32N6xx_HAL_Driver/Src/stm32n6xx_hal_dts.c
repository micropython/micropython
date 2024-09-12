/**
  **********************************************************************************************************************
  * @file    stm32n6xx_hal_dts.c
  * @author  MCD Application Team
  * @brief   DTS HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the DTS peripheral:
  *           + Initialization and de-initialization functions.
  *           + Temperature measurement functions.
  *           + Alarms functions.
  *           + Sample counter functions.
  *           + Get extreme temperatures and handle state functions.
  *
  **********************************************************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **********************************************************************************************************************
  @verbatim
  ======================================================================================================================
                                      ##### DTS Peripheral features #####
  ======================================================================================================================

  [..]
      The DTS hardware IP is a high-precision low-power junction temperature sensor (TS). It is composed of a
      configurable controller plus two embedded temperature sensors.
      In addition of temperature measurements, we have following features for each temperature sensor:

      (+) Two programmable alarms incorporating hysteresis.
      (+) Recording of the minimum and maximum temperatures measured.

      A sample counter is also available.

  ======================================================================================================================
                                      ##### How to use this driver #####
  ======================================================================================================================
  [..]
    *** Initialization and de-initialization ***
    ============================================
    [..]
      (#) User has first to initialize DTS instance.
      (#) As prerequisite, fill in the HAL_DTS_MspInit():
        (++) Enable DTS clock interface with __HAL_RCC_DTS_CLK_ENABLE().
        (++) If interrupt mode is used, enable and configure DTS interrupt with HAL_NVIC_SetPriority()
             and HAL_NVIC_EnableIRQ().
      (#) Call HAL_DTS_Init() function.

    [..]
      (#) User can de-initialize DTS instance with HAL_DTS_DeInit() function.

    *** Temperature measurement ***
    ===============================
    [..]
      (#) Configure senors using HAL_DTS_ConfigSensor() for each sensor.
      (#) In polling mode:
            (++) Use HAL_DTS_Start() to start temperature measurement.
            (++) Use HAL_DTS_PollForTemperature() to detect the end of measurement.
                 Use HAL_DTS_GetTemperature to get measured temperature.
      (#) In interrupt mode:
            (++) Use HAL_DTS_Start_IT() to start temperature measurement.
            (++) HAL_DTS_TemperatureCallback() will be called at the end of measurement.
                 Use HAL_DTS_GetTemperature to get measured temperature.
      (#) Use HAL_DTS_GetExtremeTemperatures() to get the extreme measured temperatures.
      (#) Stop measurement using HAL_DTS_Stop() or HAL_DTS_Stop_IT().
      (#) Configure sample counter using HAL_DTS_ConfigSampleCounter().
      (#) Get sample counter value using HAL_DTS_GetSampleCounter().
      (#) Activate sample discard using HAL_DTS_ConfigSampleDiscard().

    *** Alarms usage ***
    ====================
    [..]
      (#) Configure alarms using HAL_DTS_ConfigAlarmX() for each sensor (X is A or B).
      (#] HAL_DTS_AlarmXCallback() will be called when alarm X occurs (X is A or B).
      (#) Disable alarms using HAL_DTS_ConfigAlarmX() (X is A or B).

    *** generic functions ***
    =========================
    [..]
      (#) HAL_DTS_IRQHandler will be called when DTS interrupt occurs.
      (#) HAL_DTS_ErrorCallback will be called when DTS error occurs.
      (#) Use HAL_DTS_GetState() to get the current DTS instance state.

    *** Callback registration ***
    =============================
    [..]
      The compilation define USE_HAL_DTS_REGISTER_CALLBACKS when set to 1
      allows the user to configure dynamically the driver callbacks.
      Use functions HAL_DTS_RegisterCallback() to register a user callback.

    [..]
      Functions HAL_DTS_RegisterCallback() and HAL_DTS_UnRegisterCallback allow to register
      and unregister following callbacks:
        (+) MspInitCallback     : MSP init callback.
        (+) MspDeInitCallback   : MSP de-init callback.

    [..]
      For temperature measurement complete callback use dedicated register
      and unregister callback functions:
        (+) HAL_DTS_RegisterTemperatureCallback().
        (+) HAL_DTS_UnRegisterTemperatureCallback().

    [..]
      For alarms callback use dedicated register and unregister callback functions:
        (+) HAL_DTS_RegisterAlarmACallback().
        (+) HAL_DTS_RegisterAlarmBCallback().
        (+) HAL_DTS_UnRegisterAlarmACallback().
        (+) HAL_DTS_UnRegisterAlarmBCallback().

    [..]
      For error callback use dedicated register and unregister callback functions:
        (+) HAL_DTS_RegisterErrorCallback().
        (+) HAL_DTS_UnRegisterErrorCallback().

    [..]
      By default, after the call of init function and if the state is RESET
      all callbacks are reset to the corresponding legacy weak functions.
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
      HAL_DTS_RegisterCallback() before calling init or de-init function.

    [..]
      When the compilation define USE_HAL_DTS_REGISTER_CALLBACKS is set to 0 or
      not defined, the callback registering feature is not available
      and weak callbacks are used.

  @endverbatim
  **********************************************************************************************************************
  */

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

#ifdef HAL_DTS_MODULE_ENABLED

/** @defgroup DTS DTS
  * @brief DTS HAL module driver
  * @{
  */

/* Private typedef ---------------------------------------------------------------------------------------------------*/

/** @defgroup DTS_Private_Types  DTS Private Types
  * @{
  */
/**
  * @brief  DTS SDA registers
  */
typedef enum
{
  DTS_SDATS_CR_REG     = 0x00000000U, /*!< SDA TS control register */
  DTS_SDATS_CFGR_REG   = 0x01000000U, /*!< SDA TS configuration register */
  DTS_SDATS_TIMERR_REG = 0x05000000U, /*!< SDA TS timer register */
} DTS_SdaRegisterTypeDef;
/**
  * @}
  */

/* Private define ----------------------------------------------------------------------------------------------------*/

/** @defgroup DTS_Private_Define  DTS Private Define
  * @{
  */
#define DTS_SENSOR_ALL            2U          /* DTS sensor 0 and sensor 1 */
#define DTS_MAXIMUM_TIMEOUT       1000U       /* DTS maximum timeout of 1 second */
#define DTS_SDA_SENSOR_POWER_DOWN 0x00000001U /* Value to set on SDATS_CR register to power down sensor */
#define DTS_SDA_POWER_UP_DELAY    256U        /* Value to set on SDATS_TIMERR register for power-up typical delay */
#define DTS_CAL5_PARAM            4094.0f     /* Cal5 parameter used for temperature ccomputation */
#define DTS_J_PARAM               -0.1f       /* J parameter used for temperature ccomputation */
#define DTS_H_PARAM               200.0f      /* H parameter used for temperature ccomputation */
#define DTS_G_PARAM               60.0f       /* G parameter used for temperature ccomputation */
/**
  * @}
  */

/* Private macro -----------------------------------------------------------------------------------------------------*/
/* Private variables -------------------------------------------------------------------------------------------------*/
/* Private function prototypes ---------------------------------------------------------------------------------------*/

/** @defgroup DTS_Private_Functions  DTS Private Functions
  * @{
  */
static HAL_StatusTypeDef DTS_ProgramSdaRegister(DTS_HandleTypeDef *hdts, uint32_t Sensor,
                                                DTS_SdaRegisterTypeDef Reg, uint32_t Value);
static float_t           DTS_ConvertToCelsiusDegree(uint32_t sample);
static uint32_t          DTS_ConvertFromCelsiusDegree(float_t temperature);
/**
  * @}
  */

/* Exported functions ------------------------------------------------------------------------------------------------*/

/** @defgroup DTS_Exported_Functions  DTS Exported Functions
  * @{
  */

/** @defgroup DTS_Exported_Functions_Group1  Initialization and de-initialization functions
  * @brief    Initialization and de-initialization functions
  *
@verbatim
  ==============================================================================
            ##### Initialization and de-initialization functions #####
  ==============================================================================
    [..]  This section provides functions allowing to :
      (+) Initialize the DTS instance.
      (+) De-initialize the DTS instance.
      (+) Register and unregister callbacks.
@endverbatim
  * @{
  */

/**
  * @brief  Initialize the DTS instance and configure DTS system clock.
  * @param  hdts DTS handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_Init(DTS_HandleTypeDef *hdts)
{
  HAL_StatusTypeDef status;

  /* Check DTS handle */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameter */
    assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));

    /* Check that instance has not been already initialized */
    if (hdts->State != HAL_DTS_STATE_RESET)
    {
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
    else
    {
#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
      /* Reset callback pointers to the weak predefined callbacks */
      hdts->TemperatureCallback = HAL_DTS_TemperatureCallback;
      hdts->AlarmACallback      = HAL_DTS_AlarmACallback;
      hdts->AlarmBCallback      = HAL_DTS_AlarmBCallback;
      hdts->ErrorCallback       = HAL_DTS_ErrorCallback;

      /* Call DTS MSP init function */
      if (hdts->MspInitCallback == NULL)
      {
        hdts->MspInitCallback = HAL_DTS_MspInit;
      }
      hdts->MspInitCallback(hdts);
#else /* USE_HAL_DTS_REGISTER_CALLBACKS */
      /* Call DTS MSP init function */
      HAL_DTS_MspInit(hdts);
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */

      /* Configure DTS TS clock to TS operating frequency range (4MHz to 8MHz). */
      /* DTS kernel clock (hsi_div8_ck clock) is divided by (CLK_SYNTH_HI + 1) + (CLK_SYNTH_LO + 1). */
      /* Because HSI frequency is 64MHz, hsi_div8_ck frequency is 8MHz. */
      /* So CLK_SYNTH_HI and CLK_SYNTH_LO have to be set to 0 on TSCCLKSYNTHR register */
      /* in order to have TS clock frequency at 4MHz. */
      hdts->Instance->TSCCLKSYNTHR = (DTS_TSCCLKSYNTHR_CLK_SYNTH_EN |
                                      (1U << DTS_TSCCLKSYNTHR_CLK_SYNTH_HOLD_Pos));

      /* Program typical power-up delay for all sensors */
      status = DTS_ProgramSdaRegister(hdts, DTS_SENSOR_ALL, DTS_SDATS_TIMERR_REG, DTS_SDA_POWER_UP_DELAY);

      if (status == HAL_OK)
      {
        /* Enable TS interrupt */
        hdts->Instance->PVT_IER = DTS_PVT_IER_TS_IRQ_ENABLE;

        /* Reset sensor modes and error code and update state */
        hdts->SensorMode[0U] = DTS_SENSOR_MODE_DISABLE;
        hdts->SensorMode[1U] = DTS_SENSOR_MODE_DISABLE;
        hdts->ErrorCode      = HAL_DTS_ERROR_NONE;
        /* Update state */
        hdts->State = HAL_DTS_STATE_READY;
      }
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  De-initialize the DTS instance.
  * @param  hdts DTS handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_DeInit(DTS_HandleTypeDef *hdts)
{
  HAL_StatusTypeDef status;

  /* Check DTS handle */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameter */
    assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));

    /* Check that instance has not been already deinitialized */
    if (hdts->State == HAL_DTS_STATE_RESET)
    {
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
    else
    {
      /* Power down temperature sensors */
      status = DTS_ProgramSdaRegister(hdts, DTS_SENSOR_ALL, DTS_SDATS_CR_REG, DTS_SDA_SENSOR_POWER_DOWN);

      if (status == HAL_OK)
      {
        /* Disable all interrupts */
        DTS_Sensor0->TS_IER = 0U;
        DTS_Sensor1->TS_IER = 0U;

        /* Clear all pendings flags */
        DTS_Sensor0->TS_ICR = (DTS_TS_ICR_IRQ_CLEAR_FAULT  | DTS_TS_ICR_IRQ_CLEAR_DONE |
                               DTS_TS_ICR_IRQ_CLEAR_ALARMA | DTS_TS_ICR_IRQ_CLEAR_ALARMB);
        DTS_Sensor1->TS_ICR = (DTS_TS_ICR_IRQ_CLEAR_FAULT  | DTS_TS_ICR_IRQ_CLEAR_DONE |
                               DTS_TS_ICR_IRQ_CLEAR_ALARMA | DTS_TS_ICR_IRQ_CLEAR_ALARMB);

        /* Disable TS interrupt */
        hdts->Instance->PVT_IER = 0U;

        /* Disable DTS system clock */
        hdts->Instance->TSCCLKSYNTHR &= ~(DTS_TSCCLKSYNTHR_CLK_SYNTH_EN);

        /* Call DTS MSP deinit function */
#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
        if (hdts->MspDeInitCallback == NULL)
        {
          hdts->MspDeInitCallback = HAL_DTS_MspDeInit;
        }
        hdts->MspDeInitCallback(hdts);
#else /* USE_HAL_DTS_REGISTER_CALLBACKS */
        HAL_DTS_MspDeInit(hdts);
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */

        /*Update state */
        hdts->State = HAL_DTS_STATE_RESET;
      }
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Initialize the DTS instance MSP.
  * @param  hdts DTS handle.
  * @retval None.
  */
__weak void HAL_DTS_MspInit(DTS_HandleTypeDef *hdts)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdts);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_DTS_MspInit could be implemented in the user file */
}

/**
  * @brief  De-initialize the DTS instance MSP.
  * @param  hdts DTS handle.
  * @retval None.
  */
__weak void HAL_DTS_MspDeInit(DTS_HandleTypeDef *hdts)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdts);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_DTS_MspDeInit could be implemented in the user file */
}

#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register a user DTS callback to be used instead of the weak predefined callback.
  * @param  hdts DTS handle.
  * @param  CallbackID ID of the callback to be registered.
  *         This parameter can be one of the following values:
  *           @arg @ref HAL_DTS_MSPINIT_CB_ID MSP init callback ID.
  *           @arg @ref HAL_DTS_MSPDEINIT_CB_ID MSP de-init callback ID.
  * @param  pCallback pointer to the callback function.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_RegisterCallback(DTS_HandleTypeDef        *hdts,
                                           HAL_DTS_CallbackIDTypeDef CallbackID,
                                           pDTS_CallbackTypeDef      pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameter */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else if (pCallback == NULL)
  {
    hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_PARAM;
    status = HAL_ERROR;
  }
  else
  {
    const HAL_DTS_StateTypeDef temp_state = hdts->State;

    if ((temp_state == HAL_DTS_STATE_READY) || (temp_state == HAL_DTS_STATE_RESET))
    {
      switch (CallbackID)
      {
        case HAL_DTS_MSPINIT_CB_ID :
          hdts->MspInitCallback = pCallback;
          break;
        case HAL_DTS_MSPDEINIT_CB_ID :
          hdts->MspDeInitCallback = pCallback;
          break;
        default :
          hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_CALLBACK;
          status = HAL_ERROR;
          break;
      }
    }
    else
    {
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Unregister a user DTS callback.
  *         DTS callback is redirected to the weak predefined callback.
  * @param  hdts DTS handle.
  * @param  CallbackID ID of the callback to be unregistered.
  *         This parameter can be one of the following values:
  *           @arg @ref HAL_DTS_MSPINIT_CB_ID MSP init callback ID.
  *           @arg @ref HAL_DTS_MSPDEINIT_CB_ID MSP de-init callback ID.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_UnRegisterCallback(DTS_HandleTypeDef        *hdts,
                                             HAL_DTS_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameter */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    const HAL_DTS_StateTypeDef temp_state = hdts->State;

    if ((temp_state == HAL_DTS_STATE_READY) || (temp_state == HAL_DTS_STATE_RESET))
    {
      switch (CallbackID)
      {
        case HAL_DTS_MSPINIT_CB_ID :
          hdts->MspInitCallback = HAL_DTS_MspInit;
          break;
        case HAL_DTS_MSPDEINIT_CB_ID :
          hdts->MspDeInitCallback = HAL_DTS_MspDeInit;
          break;
        default :
          hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_CALLBACK;
          status = HAL_ERROR;
          break;
      }
    }
    else
    {
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Register a user DTS sensor callback to be used instead of the weak predefined callback.
  * @param  hdts DTS handle.
  * @param  CallbackID ID of the sensor callback to be registered.
  *         This parameter can be one of the following values:
  *           @arg @ref HAL_DTS_SENSOR_TEMPERATURE_CB_ID Sensor temperature callback ID.
  *           @arg @ref HAL_DTS_SENSOR_ALARMA_CB_ID Sensor alarm A callback ID.
  *           @arg @ref HAL_DTS_SENSOR_ALARMB_CB_ID Sensor alarm B callback ID.
  *           @arg @ref HAL_DTS_SENSOR_ERROR_CB_ID Sensor error callback ID.
  * @param  pCallback pointer to the sensor callback function.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_RegisterSensorCallback(DTS_HandleTypeDef              *hdts,
                                                 HAL_DTS_SensorCallbackIDTypeDef CallbackID,
                                                 pDTS_SensorCallbackTypeDef      pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameter */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else if (pCallback == NULL)
  {
    hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_PARAM;
    status = HAL_ERROR;
  }
  else
  {
    if (hdts->State == HAL_DTS_STATE_READY)
    {
      switch (CallbackID)
      {
        case HAL_DTS_SENSOR_TEMPERATURE_CB_ID :
          hdts->TemperatureCallback = pCallback;
          break;
        case HAL_DTS_SENSOR_ALARMA_CB_ID :
          hdts->AlarmACallback = pCallback;
          break;
        case HAL_DTS_SENSOR_ALARMB_CB_ID :
          hdts->AlarmBCallback = pCallback;
          break;
        case HAL_DTS_SENSOR_ERROR_CB_ID :
          hdts->ErrorCallback = pCallback;
          break;
        default :
          hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_CALLBACK;
          status = HAL_ERROR;
          break;
      }
    }
    else
    {
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Unregister a user DTS sensor callback.
  *         DTS sensor callback is redirected to the weak predefined callback.
  * @param  hdts DTS handle.
  * @param  CallbackID ID of the sensor callback to be unregistered.
  *         This parameter can be one of the following values:
  *           @arg @ref HAL_DTS_SENSOR_TEMPERATURE_CB_ID Sensor temperature callback ID.
  *           @arg @ref HAL_DTS_SENSOR_ALARMA_CB_ID Sensor alarm A callback ID.
  *           @arg @ref HAL_DTS_SENSOR_ALARMB_CB_ID Sensor alarm B callback ID.
  *           @arg @ref HAL_DTS_SENSOR_ERROR_CB_ID Sensor error callback ID.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_UnRegisterSensorCallback(DTS_HandleTypeDef              *hdts,
                                                   HAL_DTS_SensorCallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameter */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    if (hdts->State == HAL_DTS_STATE_READY)
    {
      switch (CallbackID)
      {
        case HAL_DTS_SENSOR_TEMPERATURE_CB_ID :
          hdts->TemperatureCallback = HAL_DTS_TemperatureCallback;
          break;
        case HAL_DTS_SENSOR_ALARMA_CB_ID :
          hdts->AlarmACallback = HAL_DTS_AlarmACallback;
          break;
        case HAL_DTS_SENSOR_ALARMB_CB_ID :
          hdts->AlarmBCallback = HAL_DTS_AlarmBCallback;
          break;
        case HAL_DTS_SENSOR_ERROR_CB_ID :
          hdts->ErrorCallback = HAL_DTS_ErrorCallback;
          break;
        default :
          hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_CALLBACK;
          status = HAL_ERROR;
          break;
      }
    }
    else
    {
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @defgroup DTS_Exported_Functions_Group2  Temperature measurement functions
  * @brief    Temperature measuremen functions
  *
@verbatim
  ==============================================================================
                    ##### Temperature measurement functions #####
  ==============================================================================
    [..]  This section provides functions allowing to :
      (+) configure sensor.
      (+) Start and stop temperature measurement in polling or interrupt mode.
      (+) Wait and get temperature values.
      (+) Get extreme temperature values.
      (+) Configure and get sample counter.
      (+) Configure sample discard.
@endverbatim
  * @{
  */

/**
  * @brief  Configure a sensor.
  * @param  hdts DTS handle.
  * @param  Sensor Sensor to configure.
  *         This parameter can be one of the following values:
  *           @arg @ref DTS_SENSOR_0 Sensor 0.
  *           @arg @ref DTS_SENSOR_1 Sensor 1.
  * @param  pSensorParams Sensor parameters.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_ConfigSensor(DTS_HandleTypeDef       *hdts,
                                       HAL_DTS_Sensor           Sensor,
                                       DTS_SensorConfigTypeDef *pSensorParams)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else if (pSensorParams == NULL)
  {
    hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_PARAM;
    status = HAL_ERROR;
  }
  else
  {
    const HAL_DTS_StateTypeDef temp_state = hdts->State;

    /* Check parameters */
    assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));
    assert_param(IS_DTS_SENSOR_MODE(pSensorParams->Mode));

    /* Check current state */
    if ((temp_state == HAL_DTS_STATE_READY) ||
        ((temp_state == HAL_DTS_STATE_RUNNING_1) && (Sensor == DTS_SENSOR_0)) ||
        ((temp_state == HAL_DTS_STATE_RUNNING_0) && (Sensor == DTS_SENSOR_1)))
    {
      if (pSensorParams->Mode != DTS_SENSOR_MODE_DISABLE)
      {
        /* Check resolution parameter */
        assert_param(IS_DTS_SENSOR_RESOLUTION(pSensorParams->Resolution));

        /* Program sensor trigger and resolution */
        if (pSensorParams->Mode == DTS_SENSOR_MODE_TRIGGER)
        {
          /* Check sensor trigger parameter */
          assert_param(IS_DTS_SENSOR_TRIGGER(pSensorParams->Trigger));
          status = DTS_ProgramSdaRegister(hdts, (uint32_t) Sensor, DTS_SDATS_CFGR_REG,
                                          (pSensorParams->Trigger | pSensorParams->Resolution));
        }
        else
        {
          status = DTS_ProgramSdaRegister(hdts, (uint32_t) Sensor, DTS_SDATS_CFGR_REG, pSensorParams->Resolution);
        }

        if (status == HAL_OK)
        {
          /* Remove potential sensor disabling in TSCSDIFDISABLER register */
          hdts->Instance->TSCSDIFDISABLER &= (Sensor == DTS_SENSOR_0) ? ~(DTS_TSCSDIFDISABLER_TS0_SDIF_DISABLE) :
                                             ~(DTS_TSCSDIFDISABLER_TS1_SDIF_DISABLE);

          /* Update sensor mode */
          hdts->SensorMode[Sensor] = pSensorParams->Mode;
        }
      }
      else
      {
        /* Disable sensor */
        hdts->Instance->TSCSDIFDISABLER |= (Sensor == DTS_SENSOR_0) ? DTS_TSCSDIFDISABLER_TS0_SDIF_DISABLE :
                                           DTS_TSCSDIFDISABLER_TS1_SDIF_DISABLE;

        /* Update sensor mode */
        hdts->SensorMode[Sensor] = DTS_SENSOR_MODE_DISABLE;
      }
    }
    else
    {
      /* function call in wrong state */
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Start temperature measurement on one sensor in polling mode.
  * @param  hdts DTS handle.
  * @param  Sensor Sensor.
  *         This parameter can be one of the following values:
  *           @arg @ref DTS_SENSOR_0 Sensor 0.
  *           @arg @ref DTS_SENSOR_1 Sensor 1.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_Start(DTS_HandleTypeDef *hdts,
                                HAL_DTS_Sensor     Sensor)
{
  HAL_StatusTypeDef status;

  /* Check parameter */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    const HAL_DTS_StateTypeDef temp_state = hdts->State;

    /* Check parameter */
    assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));

    /* Check current state */
    if ((temp_state == HAL_DTS_STATE_READY) ||
        ((temp_state == HAL_DTS_STATE_RUNNING_1) && (Sensor == DTS_SENSOR_0)) ||
        ((temp_state == HAL_DTS_STATE_RUNNING_0) && (Sensor == DTS_SENSOR_1)))
    {
      /* Check sensor mode */
      if (hdts->SensorMode[Sensor] == DTS_SENSOR_MODE_DISABLE)
      {
        hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_SENSOR_MODE;
        status = HAL_ERROR;
      }
      else
      {
        DTS_SensorTypeDef *psensor;

        psensor = (Sensor == DTS_SENSOR_0) ? DTS_Sensor0 : DTS_Sensor1;

        /* Reset min and max measured values for this sensor */
        psensor->TSHILORESETR = (DTS_TSHILORESETR_SMPL_LO_SET | DTS_TSHILORESETR_SMPL_HI_CLR);

        /* Start sensor with associated mode */
        status = DTS_ProgramSdaRegister(hdts, (uint32_t) Sensor, DTS_SDATS_CR_REG, hdts->SensorMode[Sensor]);

        if (status == HAL_OK)
        {
          /* Update state */
          if (hdts->State == HAL_DTS_STATE_READY)
          {
            hdts->State = (Sensor == DTS_SENSOR_0) ? HAL_DTS_STATE_RUNNING_0 : HAL_DTS_STATE_RUNNING_1;
          }
          else
          {
            hdts->State = HAL_DTS_STATE_RUNNING_BOTH;
          }
        }
      }
    }
    else
    {
      /* function call in wrong state */
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Poll for available temperature measurement on one sensor.
  * @param  hdts DTS handle.
  * @param  Sensor Sensor.
  *         This parameter can be one of the following values:
  *           @arg @ref DTS_SENSOR_0 Sensor 0.
  *           @arg @ref DTS_SENSOR_1 Sensor 1.
  * @param  Timeout Timeout value in milliseconds.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_PollForTemperature(DTS_HandleTypeDef *hdts,
                                             HAL_DTS_Sensor     Sensor,
                                             uint32_t           Timeout)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameter */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    const HAL_DTS_StateTypeDef temp_state = hdts->State;

    /* Check parameter */
    assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));

    /* Check current state */
    if ((temp_state == HAL_DTS_STATE_RUNNING_BOTH) ||
        ((temp_state == HAL_DTS_STATE_RUNNING_0) && (Sensor == DTS_SENSOR_0)) ||
        ((temp_state == HAL_DTS_STATE_RUNNING_1) && (Sensor == DTS_SENSOR_1)))
    {
      uint32_t tickstart = HAL_GetTick();
      const DTS_SensorTypeDef *psensor;

      psensor = (Sensor == DTS_SENSOR_0) ? DTS_Sensor0 : DTS_Sensor1;

      /* Wait for available temperature measurement */
      while (((psensor->TSSDIFDONER & DTS_TSSDIFDONER_SDIF_SMPL_DONE) != DTS_TSSDIFDONER_SDIF_SMPL_DONE) &&
             (status == HAL_OK))
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
        /* Update state */
        if (hdts->SensorMode[Sensor] == DTS_SENSOR_MODE_SINGLE)
        {
          if (hdts->State == HAL_DTS_STATE_RUNNING_BOTH)
          {
            hdts->State = (Sensor == DTS_SENSOR_0) ? HAL_DTS_STATE_RUNNING_1 : HAL_DTS_STATE_RUNNING_0;
          }
          else
          {
            hdts->State = HAL_DTS_STATE_READY;
          }
        }
      }
    }
    else
    {
      /* function call in wrong state */
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Get temperature measurement on one sensor.
  * @param  hdts DTS handle.
  * @param  Sensor Sensor.
  *         This parameter can be one of the following values:
  *           @arg @ref DTS_SENSOR_0 Sensor 0.
  *           @arg @ref DTS_SENSOR_1 Sensor 1.
  * @param  pTemperature Pointer to temperature in celsius degree.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_GetTemperature(DTS_HandleTypeDef *hdts,
                                         HAL_DTS_Sensor     Sensor,
                                         float_t           *pTemperature)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else if (pTemperature == NULL)
  {
    hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_PARAM;
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameter */
    assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));

    /* Check current state */
    if (hdts->State != HAL_DTS_STATE_RESET)
    {
      const DTS_SensorTypeDef *psensor;
      uint32_t sample;

      psensor = (Sensor == DTS_SENSOR_0) ? DTS_Sensor0 : DTS_Sensor1;

      /* Get sample */
      sample = psensor->TSSDIFDATAR;

      /* Check if sample is valid */
      if ((sample & (DTS_TSSDIFDATAR_SAMPLE_TYPE | DTS_TSSDIFDATAR_SAMPLE_FAULT)) == 0U)
      {
        /* Convert sample in celsius degree */
        *pTemperature = DTS_ConvertToCelsiusDegree((sample & DTS_TSSDIFDATAR_SAMPLE_DATA_Msk));
      }
      else
      {
        /* Invalid sample or fault*/
        if ((sample & DTS_TSSDIFDATAR_SAMPLE_TYPE) != 0U)
        {
          hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_SAMPLE;
        }
        else
        {
          hdts->ErrorCode |= HAL_DTS_ERROR_FAULT;
        }
        status = HAL_ERROR;
      }
    }
    else
    {
      /* function call in wrong state */
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Stop temperature measurement on one sensor in polling mode.
  * @param  hdts DTS handle.
  * @param  Sensor Sensor.
  *         This parameter can be one of the following values:
  *           @arg @ref DTS_SENSOR_0 Sensor 0.
  *           @arg @ref DTS_SENSOR_1 Sensor 1.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_Stop(DTS_HandleTypeDef *hdts,
                               HAL_DTS_Sensor     Sensor)
{
  HAL_StatusTypeDef status;

  /* Check parameter */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    const HAL_DTS_StateTypeDef temp_state = hdts->State;

    /* Check parameter */
    assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));

    /* Check current state */
    if ((temp_state == HAL_DTS_STATE_RUNNING_BOTH) ||
        ((temp_state == HAL_DTS_STATE_RUNNING_0) && (Sensor == DTS_SENSOR_0)) ||
        ((temp_state == HAL_DTS_STATE_RUNNING_1) && (Sensor == DTS_SENSOR_1)))
    {
      /* Power down sensor */
      status = DTS_ProgramSdaRegister(hdts, (uint32_t) Sensor, DTS_SDATS_CR_REG, DTS_SDA_SENSOR_POWER_DOWN);

      if (status == HAL_OK)
      {
        /* Update state */
        if (hdts->State == HAL_DTS_STATE_RUNNING_BOTH)
        {
          hdts->State = (Sensor == DTS_SENSOR_0) ? HAL_DTS_STATE_RUNNING_1 : HAL_DTS_STATE_RUNNING_0;
        }
        else
        {
          hdts->State = HAL_DTS_STATE_READY;
        }
      }
    }
    else
    {
      /* function call in wrong state */
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Start temperature measurement on one sensor in interrupt mode.
  * @param  hdts DTS handle.
  * @param  Sensor Sensor.
  *         This parameter can be one of the following values:
  *           @arg @ref DTS_SENSOR_0 Sensor 0.
  *           @arg @ref DTS_SENSOR_1 Sensor 1.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_Start_IT(DTS_HandleTypeDef *hdts,
                                   HAL_DTS_Sensor     Sensor)
{
  HAL_StatusTypeDef status;

  /* Check parameter */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    const HAL_DTS_StateTypeDef temp_state = hdts->State;

    /* Check parameter */
    assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));

    /* Check current state */
    if ((temp_state == HAL_DTS_STATE_READY) ||
        ((temp_state == HAL_DTS_STATE_RUNNING_1) && (Sensor == DTS_SENSOR_0)) ||
        ((temp_state == HAL_DTS_STATE_RUNNING_0) && (Sensor == DTS_SENSOR_1)))
    {
      /* Check sensor mode */
      if (hdts->SensorMode[Sensor] == DTS_SENSOR_MODE_DISABLE)
      {
        hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_SENSOR_MODE;
        status = HAL_ERROR;
      }
      else
      {
        DTS_SensorTypeDef *psensor;

        psensor = (Sensor == DTS_SENSOR_0) ? DTS_Sensor0 : DTS_Sensor1;

        /* Enable sample done and fault interrupts */
        psensor->TS_IER |= (DTS_TS_IER_IRQ_EN_FAULT | DTS_TS_IER_IRQ_EN_DONE);

        /* Reset min and max measured values for this sensor */
        psensor->TSHILORESETR = (DTS_TSHILORESETR_SMPL_LO_SET | DTS_TSHILORESETR_SMPL_HI_CLR);

        /* Start sensor with associated mode */
        status = DTS_ProgramSdaRegister(hdts, (uint32_t) Sensor, DTS_SDATS_CR_REG, hdts->SensorMode[Sensor]);

        if (status == HAL_OK)
        {
          /* Update state */
          if (hdts->State == HAL_DTS_STATE_READY)
          {
            hdts->State = (Sensor == DTS_SENSOR_0) ? HAL_DTS_STATE_RUNNING_0 : HAL_DTS_STATE_RUNNING_1;
          }
          else
          {
            hdts->State = HAL_DTS_STATE_RUNNING_BOTH;
          }
        }
      }
    }
    else
    {
      /* function call in wrong state */
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Stop temperature measurement on one sensor in interrupt mode.
  * @param  hdts DTS handle.
  * @param  Sensor Sensor.
  *         This parameter can be one of the following values:
  *           @arg @ref DTS_SENSOR_0 Sensor 0.
  *           @arg @ref DTS_SENSOR_1 Sensor 1.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_Stop_IT(DTS_HandleTypeDef *hdts,
                                  HAL_DTS_Sensor     Sensor)
{
  HAL_StatusTypeDef status;

  /* Check parameter */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    const HAL_DTS_StateTypeDef temp_state = hdts->State;

    /* Check parameter */
    assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));

    /* Check current state */
    if ((temp_state == HAL_DTS_STATE_RUNNING_BOTH) ||
        ((temp_state == HAL_DTS_STATE_RUNNING_0) && (Sensor == DTS_SENSOR_0)) ||
        ((temp_state == HAL_DTS_STATE_RUNNING_1) && (Sensor == DTS_SENSOR_1)))
    {
      DTS_SensorTypeDef *psensor;

      psensor = (Sensor == DTS_SENSOR_0) ? DTS_Sensor0 : DTS_Sensor1;

      /* Power down sensor */
      status = DTS_ProgramSdaRegister(hdts, (uint32_t) Sensor, DTS_SDATS_CR_REG, DTS_SDA_SENSOR_POWER_DOWN);

      if (status == HAL_OK)
      {
        /* Disable sample done and fault interrupts */
        psensor->TS_IER &= ~(DTS_TS_IER_IRQ_EN_FAULT | DTS_TS_IER_IRQ_EN_DONE);

        /* Clear potential sample done and fault interrupt flags */
        psensor->TS_ICR |= (DTS_TS_ICR_IRQ_CLEAR_FAULT | DTS_TS_ICR_IRQ_CLEAR_DONE);

        /* Update state */
        if (hdts->State == HAL_DTS_STATE_RUNNING_BOTH)
        {
          hdts->State = (Sensor == DTS_SENSOR_0) ? HAL_DTS_STATE_RUNNING_1 : HAL_DTS_STATE_RUNNING_0;
        }
        else
        {
          hdts->State = HAL_DTS_STATE_READY;
        }
      }
    }
    else
    {
      /* function call in wrong state */
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  DTS temperature callback.
  * @param  hdts DTS handle.
  * @param  Sensor Sensor.
  *         This parameter can be one of the following values:
  *           @arg @ref DTS_SENSOR_0 Sensor 0.
  *           @arg @ref DTS_SENSOR_1 Sensor 1.
  * @retval None.
  */
__weak void HAL_DTS_TemperatureCallback(DTS_HandleTypeDef *hdts,
                                        HAL_DTS_Sensor     Sensor)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdts);
  UNUSED(Sensor);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_DTS_TemperatureCallback could be implemented in the user file */
}

/**
  * @brief  Get extreme temperatures on one sensor.
  * @param  hdts DTS handle.
  * @param  Sensor Sensor.
  *         This parameter can be one of the following values:
  *           @arg @ref DTS_SENSOR_0 Sensor 0.
  *           @arg @ref DTS_SENSOR_1 Sensor 1.
  * @param  pMin Pointer to minimum temperature in celsius degree.
  * @param  pMax Pointer to maximum temperature in celsius degree.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_GetExtremeTemperatures(DTS_HandleTypeDef *hdts,
                                                 HAL_DTS_Sensor     Sensor,
                                                 float_t           *pMin,
                                                 float_t           *pMax)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else if ((pMin == NULL) || (pMax == NULL))
  {
    hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_PARAM;
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameter */
    assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));

    /* Check current state */
    if (hdts->State != HAL_DTS_STATE_RESET)
    {
      const DTS_SensorTypeDef *psensor;
      uint32_t sample;

      psensor = (Sensor == DTS_SENSOR_0) ? DTS_Sensor0 : DTS_Sensor1;

      /* Get min and max samples then convert it */
      sample = psensor->TSHLSAMPLER;
      *pMin = DTS_ConvertToCelsiusDegree((sample & DTS_TSHLSAMPLER_SMPL_LO_Msk));
      *pMax = DTS_ConvertToCelsiusDegree((sample & DTS_TSHLSAMPLER_SMPL_HI_Msk) >> DTS_TSHLSAMPLER_SMPL_HI_Pos);
    }
    else
    {
      /* function call in wrong state */
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Configure sample counter.
  * @param  hdts DTS handle.
  * @param  Enable Enable sample counter.
  * @param  Clear Clear sample counter.
  * @param  Hold Set sample counter on hold.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_ConfigSampleCounter(DTS_HandleTypeDef *hdts,
                                              FunctionalState    Enable,
                                              FunctionalState    Clear,
                                              FunctionalState    Hold)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameter */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameter */
    assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));

    /* Check current state */
    if (hdts->State != HAL_DTS_STATE_RESET)
    {
      /* Enable or disable sample counter */
      if (Enable == ENABLE)
      {
        hdts->Instance->TSCSMPL_CR &= ~(DTS_TSCSMPL_CR_SMPL_CTR_DISABLE);
      }
      else
      {
        hdts->Instance->TSCSMPL_CR |= DTS_TSCSMPL_CR_SMPL_CTR_DISABLE;
      }

      /* Clear sample counter */
      if (Clear == ENABLE)
      {
        hdts->Instance->TSCSDIFSMPLCLRR |= DTS_TSCSDIFSMPLCLRR_SMPL_CNTER_CLEAR;
      }

      /* Configure hold status for sample counter */
      if (Hold == ENABLE)
      {
        hdts->Instance->TSCSMPL_CR |= DTS_TSCSMPL_CR_SMPL_CTR_HOLD;
      }
      else
      {
        hdts->Instance->TSCSMPL_CR &= ~(DTS_TSCSMPL_CR_SMPL_CTR_HOLD);
      }
    }
    else
    {
      /* function call in wrong state */
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Get sample counter value.
  * @param  hdts DTS handle.
  * @param  pNumber Pointer to sample counter value.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_GetSampleCounterValue(DTS_HandleTypeDef *hdts,
                                                uint32_t          *pNumber)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else if (pNumber == NULL)
  {
    hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_PARAM;
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameter */
    assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));

    /* Check current state */
    if (hdts->State != HAL_DTS_STATE_RESET)
    {
      /* Get sample counter value */
      *pNumber = hdts->Instance->TSCSMPLCNTR;
    }
    else
    {
      /* function call in wrong state */
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Configure sample discard.
  * @param  hdts DTS handle.
  * @param  Status Enable or disable.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_ConfigSampleDiscard(DTS_HandleTypeDef *hdts,
                                              FunctionalState    Status)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check parameter */
  if (hdts == NULL)
  {
    retVal = HAL_ERROR;
  }
  else
  {
    /* Check current state */
    if (hdts->State != HAL_DTS_STATE_RESET)
    {
      /* Configure sample discard */
      if (Status == ENABLE)
      {
        hdts->Instance->TSCSMPL_CR |= DTS_TSCSMPL_CR_SMPL_DISCARD;
      }
      else
      {
        hdts->Instance->TSCSMPL_CR &= ~(DTS_TSCSMPL_CR_SMPL_DISCARD);
      }
    }
    else
    {
      /* function call in wrong state */
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      retVal = HAL_ERROR;
    }
  }

  /* Return function status */
  return retVal;
}

/**
  * @}
  */

/** @defgroup DTS_Exported_Functions_Group3  Alarm functions
  * @brief    Alarm functions
  *
@verbatim
  ==============================================================================
                       ##### Alarms functions #####
  ==============================================================================
    [..]  This section provides functions allowing to :
      (+) Configure alarms.
      (+) Manage alarm callbacks.
@endverbatim
  * @{
  */

/**
  * @brief  Configure alarmA for one sensor.
  * @param  hdts DTS handle.
  * @param  Sensor Sensor.
  *         This parameter can be one of the following values:
  *           @arg @ref DTS_SENSOR_0 Sensor 0.
  *           @arg @ref DTS_SENSOR_1 Sensor 1.
  * @param  pAlarmParams Pointer to alarm parameters.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_ConfigAlarmA(DTS_HandleTypeDef            *hdts,
                                       HAL_DTS_Sensor                Sensor,
                                       const DTS_AlarmConfigTypeDef *pAlarmParams)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else if (pAlarmParams == NULL)
  {
    hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_PARAM;
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameter */
    assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));
    assert_param(IS_FUNCTIONAL_STATE(pAlarmParams->Enable));

    /* Check current state */
    if (hdts->State == HAL_DTS_STATE_READY)
    {
      uint32_t alarmThreshold;
      uint32_t alarmHysteresis;
      DTS_SensorTypeDef *psensor;

      psensor = (Sensor == DTS_SENSOR_0) ? DTS_Sensor0 : DTS_Sensor1;

      /* Configure alarm A */
      if (pAlarmParams->Enable == ENABLE)
      {
        /* Check threshold and hysteresis parameters */
        assert_param(IS_DTS_ALARM_PARAM(pAlarmParams->Threshold));
        assert_param(IS_DTS_ALARM_PARAM(pAlarmParams->Hysteresis));

        /* Convert threshold and hysteresis from celsius degree  and store it on DTS register */
        alarmThreshold = DTS_ConvertFromCelsiusDegree(pAlarmParams->Threshold);
        alarmHysteresis = DTS_ConvertFromCelsiusDegree(pAlarmParams->Hysteresis);
        psensor->TSALARMA_CFGR = (alarmThreshold << DTS_TSALARMA_CFGR_ALARMA_THRESH_Pos) |
                                 (alarmHysteresis << DTS_TSALARMA_CFGR_HYSTA_THRESH_Pos);

        /* Enable alarm A interrupt */
        psensor->TS_IER |= DTS_TS_IER_IRQ_EN_ALARMA;
      }
      else
      {
        /* Disable alarm A interrupt */
        psensor->TS_IER &= ~(DTS_TS_IER_IRQ_EN_ALARMA);

        /* Clear potential alarm A pending flag */
        psensor->TS_ICR |= DTS_TS_ICR_IRQ_CLEAR_ALARMA;

        /* Disable alarm A */
        psensor->TSALARMA_CFGR = 0U;
      }
    }
    else
    {
      /* function call in wrong state */
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Configure alarmB for one sensor.
  * @param  hdts DTS handle.
  * @param  Sensor Sensor.
  *         This parameter can be one of the following values:
  *           @arg @ref DTS_SENSOR_0 Sensor 0.
  *           @arg @ref DTS_SENSOR_1 Sensor 1.
  * @param  pAlarmParams Pointer to alarm parameters.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DTS_ConfigAlarmB(DTS_HandleTypeDef            *hdts,
                                       HAL_DTS_Sensor                Sensor,
                                       const DTS_AlarmConfigTypeDef *pAlarmParams)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check parameters */
  if (hdts == NULL)
  {
    status = HAL_ERROR;
  }
  else if (pAlarmParams == NULL)
  {
    hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_PARAM;
    status = HAL_ERROR;
  }
  else
  {
    /* Check parameter */
    assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));
    assert_param(IS_FUNCTIONAL_STATE(pAlarmParams->Enable));

    /* Check current state */
    if (hdts->State == HAL_DTS_STATE_READY)
    {
      uint32_t alarmThreshold;
      uint32_t alarmHysteresis;
      DTS_SensorTypeDef *psensor;

      psensor = (Sensor == DTS_SENSOR_0) ? DTS_Sensor0 : DTS_Sensor1;

      /* Configure alarm B */
      if (pAlarmParams->Enable == ENABLE)
      {
        /* Check threshold and hysteresis parameters */
        assert_param(IS_DTS_ALARM_PARAM(pAlarmParams->Threshold));
        assert_param(IS_DTS_ALARM_PARAM(pAlarmParams->Hysteresis));

        /* Convert threshold and hysteresis from celsius degree and store it on DTS register */
        alarmThreshold = DTS_ConvertFromCelsiusDegree(pAlarmParams->Threshold);
        alarmHysteresis = DTS_ConvertFromCelsiusDegree(pAlarmParams->Hysteresis);
        psensor->TSALARMB_CFGR = (alarmThreshold << DTS_TSALARMB_CFGR_ALARMB_THRESH_Pos) |
                                 (alarmHysteresis << DTS_TSALARMB_CFGR_HYSTB_THRESH_Pos);

        /* Enable alarm B interrupt */
        psensor->TS_IER |= DTS_TS_IER_IRQ_EN_ALARMB;
      }
      else
      {
        /* Disable alarm B interrupt */
        psensor->TS_IER &= ~(DTS_TS_IER_IRQ_EN_ALARMB);

        /* Clear potential alarm B pending flag */
        psensor->TS_ICR |= DTS_TS_ICR_IRQ_CLEAR_ALARMB;

        /* Disable alarm A */
        psensor->TSALARMB_CFGR = 0U;
      }
    }
    else
    {
      /* function call in wrong state */
      hdts->ErrorCode |= HAL_DTS_ERROR_INVALID_STATE;
      status = HAL_ERROR;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  DTS alarm A callback.
  * @param  hdts DTS handle.
  * @param  Sensor Sensor.
  *         This parameter can be one of the following values:
  *           @arg @ref DTS_SENSOR_0 Sensor 0.
  *           @arg @ref DTS_SENSOR_1 Sensor 1.
  * @retval None.
  */
__weak void HAL_DTS_AlarmACallback(DTS_HandleTypeDef *hdts,
                                   HAL_DTS_Sensor     Sensor)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdts);
  UNUSED(Sensor);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_DTS_AlarmACallback could be implemented in the user file */
}

/**
  * @brief  DTS alarm B callback.
  * @param  hdts DTS handle.
  * @param  Sensor Sensor.
  *         This parameter can be one of the following values:
  *           @arg @ref DTS_SENSOR_0 Sensor 0.
  *           @arg @ref DTS_SENSOR_1 Sensor 1.
  * @retval None.
  */
__weak void HAL_DTS_AlarmBCallback(DTS_HandleTypeDef *hdts,
                                   HAL_DTS_Sensor     Sensor)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdts);
  UNUSED(Sensor);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_DTS_AlarmBCallback could be implemented in the user file */
}

/**
  * @}
  */

/** @defgroup DTS_Exported_Functions_Group4  Generic functions
  * @brief    Generic functions
  *
@verbatim
  ==============================================================================
                       ##### Generic functions #####
  ==============================================================================
    [..]  This section provides functions allowing to :
      (+) Handle DTS interrupt.
      (+) Inform user that error occurs.
      (+) Get the current DTS instance state.
@endverbatim
  * @{
  */

/**
  * @brief  Handle DTS interrupts.
  * @param  hdts DTS handle.
  * @retval None.
  */
void HAL_DTS_IRQHandler(DTS_HandleTypeDef *hdts)
{
  DTS_SensorTypeDef *psensor;
  HAL_DTS_Sensor Sensor;
  uint32_t tmp_reg1;
  uint32_t tmp_reg2;
  uint32_t interrupts;

  /* Check on which sensor interrupt occurs */
  if ((hdts->Instance->TS_ISR & DTS_TS_ISR_TS0_IRQ_STATUS) == DTS_TS_ISR_TS0_IRQ_STATUS)
  {
    psensor = DTS_Sensor0;
    Sensor = DTS_SENSOR_0;
  }
  else
  {
    psensor = DTS_Sensor1;
    Sensor = DTS_SENSOR_1;
  }

  /* Read current flags and interrupts on sensor */
  tmp_reg1 = psensor->TS_IER;
  tmp_reg2 = psensor->TS_ISR;
  interrupts = (tmp_reg1 & tmp_reg2);

  /* Check if sample done occurs */
  if ((interrupts & DTS_TS_ISR_IRQ_STATUS_DONE) == DTS_TS_ISR_IRQ_STATUS_DONE)
  {
    /* Clear corresponding flag */
    psensor->TS_ICR |= DTS_TS_ICR_IRQ_CLEAR_DONE;

    /* Call temperature callback */
#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
    hdts->TemperatureCallback(hdts, Sensor);
#else /* USE_HAL_DTS_REGISTER_CALLBACKS */
    HAL_DTS_TemperatureCallback(hdts, Sensor);
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */

    /* Update state */
    if (hdts->SensorMode[Sensor] == DTS_SENSOR_MODE_SINGLE)
    {
      /* Disable sample done interrupt */
      psensor->TS_IER &= ~(DTS_TS_IER_IRQ_EN_DONE);

      if (hdts->State == HAL_DTS_STATE_RUNNING_BOTH)
      {
        hdts->State = (Sensor == DTS_SENSOR_0) ? HAL_DTS_STATE_RUNNING_1 : HAL_DTS_STATE_RUNNING_0;
      }
      else
      {
        hdts->State = HAL_DTS_STATE_READY;
      }
    }
  }
  /* Check if alarm A occurs */
  if ((interrupts & DTS_TS_ISR_IRQ_STATUS_ALARMA) == DTS_TS_ISR_IRQ_STATUS_ALARMA)
  {
    /* Clear corresponding flag */
    psensor->TS_ICR |= DTS_TS_ICR_IRQ_CLEAR_ALARMA;

    /* Call alarm A callback */
#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
    hdts->AlarmACallback(hdts, Sensor);
#else /* USE_HAL_DTS_REGISTER_CALLBACKS */
    HAL_DTS_AlarmACallback(hdts, Sensor);
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
  }
  /* Check if alarm B occurs */
  if ((interrupts & DTS_TS_ISR_IRQ_STATUS_ALARMB) == DTS_TS_ISR_IRQ_STATUS_ALARMB)
  {
    /* Clear corresponding flag */
    psensor->TS_ICR |= DTS_TS_ICR_IRQ_CLEAR_ALARMB;

    /* Call alarm B callback */
#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
    hdts->AlarmBCallback(hdts, Sensor);
#else /* USE_HAL_DTS_REGISTER_CALLBACKS */
    HAL_DTS_AlarmBCallback(hdts, Sensor);
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
  }
  /* Check if fault occurs */
  if ((interrupts & DTS_TS_ISR_IRQ_STATUS_FAULT) == DTS_TS_ISR_IRQ_STATUS_FAULT)
  {
    /* Clear corresponding flag */
    psensor->TS_ICR |= DTS_TS_ICR_IRQ_CLEAR_FAULT;

    /* Update error code */
    hdts->ErrorCode |= HAL_DTS_ERROR_FAULT;

    /* Call error callback */
#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
    hdts->ErrorCallback(hdts, Sensor);
#else /* USE_HAL_DTS_REGISTER_CALLBACKS */
    HAL_DTS_ErrorCallback(hdts, Sensor);
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
  }
}

/**
  * @brief  DTS error callback.
  * @param  hdts DTS handle.
  * @param  Sensor Sensor.
  *         This parameter can be one of the following values:
  *           @arg @ref DTS_SENSOR_0 Sensor 0.
  *           @arg @ref DTS_SENSOR_1 Sensor 1.
  * @retval None.
  */
__weak void HAL_DTS_ErrorCallback(DTS_HandleTypeDef *hdts,
                                  HAL_DTS_Sensor     Sensor)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdts);
  UNUSED(Sensor);

  /* NOTE : This function should not be modified, when the function is needed,
            the HAL_DTS_ErrorCallback could be implemented in the user file */
}

/**
  * @brief  Get current DTS handle state.
  * @param  hdts DTS handle.
  * @retval DTS state.
  */
HAL_DTS_StateTypeDef HAL_DTS_GetState(const DTS_HandleTypeDef *hdts)
{
  /* Return DTS state */
  return hdts->State;
}

/**
  * @brief  Get DTS error code.
  * @param  hdts DTS handle.
  * @retval DTS error code.
  */
uint32_t HAL_DTS_GetError(const DTS_HandleTypeDef *hdts)
{
  return hdts->ErrorCode;
}

/**
  * @}
  */

/**
  * @}
  */

/* Private functions -------------------------------------------------------------------------------------------------*/

/** @addtogroup DTS_Private_Functions
  * @brief      Private functions
  * @{
  */

/**
  * @brief  Program SDA register.
  * @param  hdts DTS handle.
  * @param  Sensor Sensor.
  *         This parameter can be one of the following values:
  *           @arg @ref DTS_SENSOR_0 Sensor 0.
  *           @arg @ref DTS_SENSOR_1 Sensor 1.
  * @param  Reg SDA register to program.
  * @param  Value Value to program on SDA register.
  * @retval HAL status.
  */
static HAL_StatusTypeDef DTS_ProgramSdaRegister(DTS_HandleTypeDef *hdts, uint32_t Sensor,
                                                DTS_SdaRegisterTypeDef Reg, uint32_t Value)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t tickstart = HAL_GetTick();

  /* Check that serial data interface is not busy */
  while (((hdts->Instance->TSCSDIF_SR & DTS_TSCSDIF_SR_SDIF_BUSY) != 0U) && (status == HAL_OK))
  {
    if ((HAL_GetTick() - tickstart) > DTS_MAXIMUM_TIMEOUT)
    {
      status = HAL_TIMEOUT;
    }
  }

  if (status == HAL_OK)
  {
    if (Sensor == DTS_SENSOR_ALL)
    {
      hdts->Instance->TSCSDIF_CFGR = 0U;
    }
    else
    {
      /* Inhibit serial programming of other sensor */
      hdts->Instance->TSCSDIF_CFGR = (Sensor == (uint32_t) DTS_SENSOR_0) ? DTS_TSCSDIF_CFGR_SDIF_INHIBIT_1 :
                                     DTS_TSCSDIF_CFGR_SDIF_INHIBIT_0;
    }

    /* program value on SDA register using DTS_TSCSDIF_CR */
    hdts->Instance->TSCSDIF_CR = (DTS_TSCSDIF_CR_SDIF_PROG | DTS_TSCSDIF_CR_SDIF_WRN | (uint32_t) Reg | Value);
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Convert DTS sample to celsius degree.
  * @param  sample DTS sample.
  * @retval Temperature in celsius degree.
  */
static float_t DTS_ConvertToCelsiusDegree(uint32_t sample)
{
  float_t value;

  /* The temperature computation is Temp (celsius degree) = G + (H x Eqbs) + (J x Fclk_ts) */
  /* where Eqbs is equal to (sample/Cal5) - 0.5 */
  /* and Fclk_ts is 4MHz */

  /* Computation of Eqbs */
  value = ((((float_t) sample) / DTS_CAL5_PARAM) - 0.5f);

  /* Computation of temperature */
  value = DTS_G_PARAM + (DTS_H_PARAM * value) + (DTS_J_PARAM * 4.0f);

  /* Return temperature value */
  return value;
}

/**
  * @brief  Convert DTS sample from celsius degree.
  * @param  temperature Temperature in celsius degree.
  * @retval DTS sample.
  */
static uint32_t DTS_ConvertFromCelsiusDegree(float_t temperature)
{
  float_t value;

  /* The temperature computation is Temp (celsius degree) = G + (H x Eqbs) + (J x Fclk_ts) */
  /* where Eqbs = (sample/Cal5) - 0.5 */
  /* and Fclk_ts is 4MHz. */
  /* So we have sample = (Eqbs + 0.5) x Cal5 */
  /* and Eqbs = (Temp - G - (J x Fclk_ts)) / H. */

  /* Computation of Eqbs */
  value = (temperature - DTS_G_PARAM - (DTS_J_PARAM * 4.0f)) / DTS_H_PARAM;

  /* Computation of sample */
  value = (value + 0.5f) * DTS_CAL5_PARAM;

  /* Return sample value */
  return (uint32_t) value;
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_DTS_MODULE_ENABLED */

/**
  * @}
  */
