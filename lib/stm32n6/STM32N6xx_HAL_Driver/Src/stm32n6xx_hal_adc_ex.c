/**
  ******************************************************************************
  * @file    stm32n6xx_hal_adc_ex.c
  * @author  MCD Application Team
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the Analog to Digital Converter (ADC)
  *          peripheral:
  *           + Peripheral Control functions
  *          Other functions (generic functions) are available in file
  *          "stm32n6xx_hal_adc.c".
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
  [..]
  (@) Sections "ADC peripheral features" and "How to use this driver" are
      available in file of generic functions "stm32n6xx_hal_adc.c".
  [..]
  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup ADCEx ADCEx
  * @brief ADC Extended HAL module driver
  * @{
  */

#ifdef HAL_ADC_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/** @defgroup ADCEx_Private_Constants ADC Extended Private Constants
  * @{
  */

#define ADC_JSQR_FIELDS  ((ADC_JSQR_JL | ADC_JSQR_JEXTSEL | ADC_JSQR_JEXTEN |\
                           ADC_JSQR_JSQ1  | ADC_JSQR_JSQ2 |\
                           ADC_JSQR_JSQ3 | ADC_JSQR_JSQ4 ))  /*!< ADC_JSQR fields of parameters that can be updated anytime
                                                                  once the ADC is enabled */

#define ADC_JSQR_LOW_FIELDS  (ADC_JSQR_JL | ADC_JSQR_JEXTSEL | ADC_JSQR_JEXTEN)  /*!< ADC_JSQR fields of parameters that can be updated anytime
                                                                  once the ADC is enabled */

/* Fixed timeout value for ADC calibration.                                   */
/* Values defined to be higher than worst cases: low clock frequency          */
/* Ex of profile low frequency: (refer to device datasheet, parameter "fADC") */
/*     Conversion_cycle = (12.5 + 1499.5) = 1512                              */
/* Calibration_time MAX = Conversion_cycle / fADC_min                         */
/*                      = 1512 / (0.7MHz) = 2.16 ms                           */
/* Used timeout value includes a margin versus theoretical max value          */
#define ADC_CALIBRATION_TIMEOUT         (5UL)        /*!< ADC calibration time-out value (unit: ms) */

#define ADC_CALIBRATION_STEPS           (8UL)        /*!< Number of ADC measurement during calibration procedure */
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef ADC_Calibration_MeasureOffset(ADC_HandleTypeDef *hadc,
                                                       uint32_t SingleDiff,
                                                       uint32_t *pCalibrationFactor);
/* Exported functions --------------------------------------------------------*/

/** @defgroup ADCEx_Exported_Functions ADC Extended Exported Functions
  * @{
  */

/** @defgroup ADCEx_Exported_Functions_Group1 Extended Input and Output operation functions
  * @brief    Extended IO operation functions
  *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:

      (+) Perform the ADC self-calibration for single and differential ending.
      (+) Get calibration factors for single or differential ending.
      (+) Set calibration factors for single or differential ending.

      (+) Start conversion of ADC group injected.
      (+) Stop conversion of ADC group injected.
      (+) Poll for conversion complete on ADC group injected.
      (+) Get result of ADC group injected channel conversion.
      (+) Start conversion of ADC group injected and enable interruptions.
      (+) Stop conversion of ADC group injected and disable interruptions.

      (+) When multimode feature is available, start multimode and enable DMA transfer.
      (+) Stop multimode and disable ADC DMA transfer.
      (+) Get result of multimode conversion.

@endverbatim
  * @{
  */

/**
  * @brief  Perform an ADC automatic self-calibration
  *         Calibration prerequisite: ADC must be disabled (execute this
  *         function before HAL_ADC_Start() or after HAL_ADC_Stop() ).
  * @note   Note: This calibration may reduce ADC full range. It is only
  *         recommended for application that needs precise measurement and not
  *         on full range (Vref+ minus few hundreds of mV, refer to reference
  *         manual).
  *         The calibration procedure removes ADC conversion offset error. After
  *         calibration ADC full range is reduced to
  *         [Vref-; (Vref+)-CALFACT_x].
  * @param  hadc       ADC handle
  * @param  SingleDiff Selection of single-ended or differential input
  *         This parameter can be one of the following values:
  *           @arg @ref ADC_SINGLE_ENDED      Channel in mode input single ended
  *           @arg @ref ADC_DIFFERENTIAL_ENDED Channel in mode input
  *           single ended and differential ended
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_Calibration_Start(ADC_HandleTypeDef *hadc, uint32_t SingleDiff)
{
  HAL_StatusTypeDef tmp_hal_status;
  uint32_t calibration_factor;
  uint32_t offset_required_single_end = 0UL;
  uint32_t backup_trigger_settings;
  uint32_t backup_offset_config[4];

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  assert_param(IS_ADC_SINGLE_DIFFERENTIAL(SingleDiff));

  /* Process locked */
  __HAL_LOCK(hadc);

  /* Enable ADC if not already enabled */
  tmp_hal_status = ADC_Enable(hadc);

  /* Ensure no conversion is ongoing and ADC enabled correctly */
  if ((LL_ADC_REG_IsConversionOngoing(hadc->Instance) == 0UL)
      && (tmp_hal_status == HAL_OK))
  {
    /* Set ADC state */
    ADC_STATE_CLR_SET(hadc->State,
                      HAL_ADC_STATE_REG_BUSY | HAL_ADC_STATE_INJ_BUSY,
                      HAL_ADC_STATE_BUSY_INTERNAL);

    /* Save ADC to current configuration */
    backup_trigger_settings = READ_REG(hadc->Instance->CFGR1);
    backup_offset_config[0] = LL_ADC_GetOffsetLevel(hadc->Instance, LL_ADC_OFFSET_1);
    backup_offset_config[1] = LL_ADC_GetOffsetLevel(hadc->Instance, LL_ADC_OFFSET_2);
    backup_offset_config[2] = LL_ADC_GetOffsetLevel(hadc->Instance, LL_ADC_OFFSET_3);
    backup_offset_config[3] = LL_ADC_GetOffsetLevel(hadc->Instance, LL_ADC_OFFSET_4);

    /* Force ADC configuration for calibration */
    CLEAR_BIT(hadc->Instance->CFGR1, ADC_CFGR1_DMNGT | ADC_CFGR1_EXTEN | ADC_CFGR1_CONT | ADC_CFGR1_RES);
    LL_ADC_SetOffsetLevel(hadc->Instance, LL_ADC_OFFSET_1, 0);
    LL_ADC_SetOffsetLevel(hadc->Instance, LL_ADC_OFFSET_2, 0);
    LL_ADC_SetOffsetLevel(hadc->Instance, LL_ADC_OFFSET_3, 0);
    LL_ADC_SetOffsetLevel(hadc->Instance, LL_ADC_OFFSET_4, 0);

    /* Disable additional offset before calibration start */
    LL_ADC_DisableCalibrationOffset(hadc->Instance);

    /* Start ADC offset measurement in single ended mode */
    tmp_hal_status = ADC_Calibration_MeasureOffset(hadc, ADC_SINGLE_ENDED, &calibration_factor);

    if (tmp_hal_status == HAL_OK)
    {
      /* Store the singled-ended calibration factor in CALFACT_S[8:0]. */
      LL_ADC_SetCalibrationFactor(hadc->Instance, LL_ADC_SINGLE_ENDED, calibration_factor);

      if ((SingleDiff & ADC_DIFFERENTIAL_ENDED) == ADC_DIFFERENTIAL_ENDED)
      {
        /* Store calibration offset state after single-ended calibration.     */
        /* keep the same setting for differential-ended calibration.          */
        offset_required_single_end = LL_ADC_IsCalibrationOffsetEnabled(hadc->Instance);

        /* Start ADC offset measurement in differential ended mode */
        tmp_hal_status = ADC_Calibration_MeasureOffset(hadc, ADC_DIFFERENTIAL_ENDED, &calibration_factor);

        if (tmp_hal_status == HAL_OK)
        {
          /* Store the differential-ended calibration factor in CALFACT_D[8:0]. */
          LL_ADC_SetCalibrationFactor(hadc->Instance, LL_ADC_DIFFERENTIAL_ENDED, calibration_factor);

          /* Additional calibration offset is applied to both single-ended    */
          /* and differential-ended conversion mode.                          */
          /* If calibration offset was enabled by differential-ended          */
          /* calibration, single-ended mode should be recalibrated using with */
          /* calibration offset enabled.                                      */
          if (offset_required_single_end != LL_ADC_IsCalibrationOffsetEnabled(hadc->Instance))
          {
            /* Start ADC offset measurement in single-ended mode */
            tmp_hal_status = ADC_Calibration_MeasureOffset(hadc, ADC_SINGLE_ENDED, &calibration_factor);

            if (tmp_hal_status == HAL_OK)
            {
              /* Store the singled-ended calibration factor in CALFACT_S[8:0]. */
              LL_ADC_SetCalibrationFactor(hadc->Instance, LL_ADC_SINGLE_ENDED, calibration_factor);
            }
            else
            {
              SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL);

              /* Note: No need to update variable "tmp_hal_status" here:      */
              /*       already set to state "HAL_ERROR" by function disabling */
              /*       the ADC.                                               */
            }
          }
          else
          {
            /* nothing to do */
          }
        }
        else
        {
          SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL);

          /* Note: No need to update variable "tmp_hal_status" here: already  */
          /*       set to state "HAL_ERROR" by function disabling the ADC.    */
        }
      }
      else
      {
        /* Prevent unused argument(s) compilation warning if no assert_param  */
        /* check (Only used for differential mode calibration)                */
        UNUSED(offset_required_single_end);
      }
    }
    else
    {
      SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL);

      /* Note: No need to update variable "tmp_hal_status" here: already set  */
      /*       to state "HAL_ERROR" by function disabling the ADC.            */
    }

    /* End of calibration procedure */
    LL_ADC_StopCalibration(hadc->Instance);

    /* Restore ADC configuration to previous state */
    WRITE_REG(hadc->Instance->CFGR1, backup_trigger_settings);
    LL_ADC_SetOffsetLevel(hadc->Instance, LL_ADC_OFFSET_1, backup_offset_config[0]);
    LL_ADC_SetOffsetLevel(hadc->Instance, LL_ADC_OFFSET_2, backup_offset_config[1]);
    LL_ADC_SetOffsetLevel(hadc->Instance, LL_ADC_OFFSET_3, backup_offset_config[2]);
    LL_ADC_SetOffsetLevel(hadc->Instance, LL_ADC_OFFSET_4, backup_offset_config[3]);

    /* Set ADC state */
    ADC_STATE_CLR_SET(hadc->State,
                      HAL_ADC_STATE_BUSY_INTERNAL,
                      HAL_ADC_STATE_READY);
  }
  else
  {
    SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL);

    /* Note: No need to update variable "tmp_hal_status" here: already set    */
    /*       to state "HAL_ERROR" by function disabling the ADC.              */
  }

  __HAL_UNLOCK(hadc);

  return tmp_hal_status;
}

/**
  * @brief  Get the calibration factor.
  * @param hadc ADC handle.
  * @param SingleDiff This parameter can be only:
  *           @arg @ref ADC_SINGLE_ENDED      Channel in mode input single ended
  *           @arg @ref ADC_DIFFERENTIAL_ENDED Channel in mode input
  *           differential ended
  * @retval Calibration value.
  */
uint32_t HAL_ADCEx_Calibration_GetValue(const ADC_HandleTypeDef *hadc, uint32_t SingleDiff)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  assert_param(IS_ADC_SINGLE_DIFFERENTIAL(SingleDiff));
  /* Return the selected ADC calibration value */
  return LL_ADC_GetCalibrationFactor(hadc->Instance, SingleDiff);
}

/**
  * @brief  Set the calibration factor to overwrite automatic conversion result.
  *         ADC must be enabled and no conversion is ongoing.
  * @param hadc ADC handle
  * @param SingleDiff This parameter can be only:
  *           @arg @ref ADC_SINGLE_ENDED      Channel in mode input single ended
  *           @arg @ref ADC_DIFFERENTIAL_ENDED Channel in mode input
  *           differential ended
  * @param CalibrationFactor Calibration factor (coded on 7 bits maximum)
  * @retval HAL state
  */
HAL_StatusTypeDef HAL_ADCEx_Calibration_SetValue(ADC_HandleTypeDef *hadc, uint32_t SingleDiff,
                                                 uint32_t CalibrationFactor)
{
  HAL_StatusTypeDef tmp_hal_status = HAL_OK;
  uint32_t tmp_adc_is_conversion_on_going_regular;
  uint32_t tmp_adc_is_conversion_on_going_injected;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  assert_param(IS_ADC_SINGLE_DIFFERENTIAL(SingleDiff));
  assert_param(IS_ADC_CALFACT(CalibrationFactor));

  /* Process locked */
  __HAL_LOCK(hadc);

  /* Verification of hardware constraints before modifying the calibration    */
  /* factors register: ADC must be enabled, no conversion on going.           */
  tmp_adc_is_conversion_on_going_regular = LL_ADC_REG_IsConversionOngoing(hadc->Instance);
  tmp_adc_is_conversion_on_going_injected = LL_ADC_INJ_IsConversionOngoing(hadc->Instance);
  if ((LL_ADC_IsEnabled(hadc->Instance) != 0UL)
      && (tmp_adc_is_conversion_on_going_regular == 0UL)
      && (tmp_adc_is_conversion_on_going_injected == 0UL)
     )
  {
    /* Set the selected ADC calibration value */
    LL_ADC_SetCalibrationFactor(hadc->Instance, SingleDiff, CalibrationFactor);
  }
  else
  {
    /* Update ADC state machine */
    SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);
    /* Update ADC error code */
    SET_BIT(hadc->ErrorCode, HAL_ADC_ERROR_INTERNAL);

    /* Update ADC state machine to error */
    tmp_hal_status = HAL_ERROR;
  }

  __HAL_UNLOCK(hadc);

  return tmp_hal_status;
}
/**
  * @brief  Enable ADC, start conversion of injected group.
  * @note   Interruptions enabled in this function: None.
  * @note   Case of multimode enabled when multimode feature is available:
  *         HAL_ADCEx_InjectedStart() API must be called for ADC slave first,
  *         then for ADC master.
  *         For ADC slave, ADC is enabled only (conversion is not started).
  *         For ADC master, ADC is enabled and multimode conversion is started.
  * @param hadc ADC handle.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedStart(ADC_HandleTypeDef *hadc)
{
  HAL_StatusTypeDef tmp_hal_status;
#if defined(ADC_MULTIMODE_SUPPORT)
  uint32_t tmp_multimode_config = LL_ADC_GetMultimode(__LL_ADC_COMMON_INSTANCE(hadc->Instance));
#endif /* ADC_MULTIMODE_SUPPORT */

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));

  if (LL_ADC_INJ_IsConversionOngoing(hadc->Instance) != 0UL)
  {
    return HAL_BUSY;
  }
  else
  {
    /* Process locked */
    __HAL_LOCK(hadc);

    /* Enable the ADC peripheral */
    tmp_hal_status = ADC_Enable(hadc);

    /* Start conversion if ADC is effectively enabled */
    if (tmp_hal_status == HAL_OK)
    {
      /* Check if a regular conversion is ongoing */
      if ((hadc->State & HAL_ADC_STATE_REG_BUSY) == 0UL)
      {
        /* Set ADC error code to none */
        ADC_CLEAR_ERRORCODE(hadc);
      }

      /* Set ADC state                                                        */
      /* - Clear state bitfield related to injected group conversion results  */
      /* - Set state bitfield related to injected operation                   */
      ADC_STATE_CLR_SET(hadc->State,
                        HAL_ADC_STATE_READY | HAL_ADC_STATE_INJ_EOC,
                        HAL_ADC_STATE_INJ_BUSY);

#if defined(ADC_MULTIMODE_SUPPORT)
      /* Reset HAL_ADC_STATE_MULTIMODE_SLAVE bit
        - if ADC instance is master or if multimode feature is not available
        - if multimode setting is disabled (ADC instance slave in independent mode) */
      if ((__LL_ADC_MULTI_INSTANCE_MASTER(hadc->Instance) == hadc->Instance)
          || (tmp_multimode_config == LL_ADC_MULTI_INDEPENDENT)
         )
      {
        CLEAR_BIT(hadc->State, HAL_ADC_STATE_MULTIMODE_SLAVE);
      }
#endif /* ADC_MULTIMODE_SUPPORT */

      /* Clear ADC group injected group conversion flag */
      /* (To ensure of no unknown state from potential previous ADC operations) */
      __HAL_ADC_CLEAR_FLAG(hadc, (ADC_FLAG_JEOC | ADC_FLAG_JEOS));

      /* Unlock before starting ADC conversions: in case of potential         */
      /* interruption, to let the process to ADC IRQ Handler.                 */
      __HAL_UNLOCK(hadc);

      /* Enable conversion of injected group, if automatic injected conversion  */
      /* is disabled.                                                           */
      /* If software start has been selected, conversion starts immediately.    */
      /* If external trigger has been selected, conversion will start at next   */
      /* trigger event.                                                         */
      /* Case of multimode enabled (when multimode feature is available):       */
      /* if ADC is slave,                                                       */
      /*    - ADC is enabled only (conversion is not started),                  */
      /*    - if multimode only concerns regular conversion, ADC is enabled     */
      /*     and conversion is started.                                         */
      /* If ADC is master or independent,                                       */
      /*    - ADC is enabled and conversion is started.                         */
#if defined(ADC_MULTIMODE_SUPPORT)
      if ((__LL_ADC_MULTI_INSTANCE_MASTER(hadc->Instance) == hadc->Instance)
          || (tmp_multimode_config == LL_ADC_MULTI_INDEPENDENT)
          || (tmp_multimode_config == LL_ADC_MULTI_DUAL_REG_SIMULT)
          || (tmp_multimode_config == LL_ADC_MULTI_DUAL_REG_INTERL)
         )
      {
        /* ADC instance is not a multimode slave instance with multimode injected conversions enabled */
        if (LL_ADC_INJ_GetTrigAuto(hadc->Instance) == LL_ADC_INJ_TRIG_INDEPENDENT)
        {
          LL_ADC_INJ_StartConversion(hadc->Instance);
        }
      }
      else
      {
        /* ADC instance is not a multimode slave instance with multimode injected conversions enabled */
        SET_BIT(hadc->State, HAL_ADC_STATE_MULTIMODE_SLAVE);
      }
#else
      if (LL_ADC_INJ_GetTrigAuto(hadc->Instance) == LL_ADC_INJ_TRIG_INDEPENDENT)
      {
        /* Start ADC group injected conversion */
        LL_ADC_INJ_StartConversion(hadc->Instance);
      }
#endif /* ADC_MULTIMODE_SUPPORT */

    }
    else
    {
      __HAL_UNLOCK(hadc);
    }

    return tmp_hal_status;
  }
}

/**
  * @brief  Stop conversion of injected channels. Disable ADC peripheral if
  *         no regular conversion is on going.
  * @note   If ADC must be disabled and if conversion is on going on
  *         regular group, function HAL_ADC_Stop must be used to stop both
  *         injected and regular groups, and disable the ADC.
  * @note   If injected group mode auto-injection is enabled,
  *         function HAL_ADC_Stop must be used.
  * @note   In case of multimode enabled (when multimode feature is available),
  *         HAL_ADCEx_InjectedStop() must be called for ADC master first, then for ADC slave.
  *         For ADC master, conversion is stopped and ADC is disabled.
  *         For ADC slave, ADC is disabled only (conversion stop of ADC master
  *         has already stopped conversion of ADC slave).
  * @param hadc ADC handle.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedStop(ADC_HandleTypeDef *hadc)
{
  HAL_StatusTypeDef tmp_hal_status;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));

  /* Process locked */
  __HAL_LOCK(hadc);

  /* 1. Stop potential conversion on going on injected group only. */
  tmp_hal_status = ADC_ConversionStop(hadc, ADC_INJECTED_GROUP);

  /* Disable ADC peripheral if injected conversions are effectively stopped   */
  /* and if no conversion on regular group is on-going                       */
  if (tmp_hal_status == HAL_OK)
  {
    if (LL_ADC_REG_IsConversionOngoing(hadc->Instance) == 0UL)
    {
      /* 2. Disable the ADC peripheral */
      tmp_hal_status = ADC_Disable(hadc);

      /* Check if ADC is effectively disabled */
      if (tmp_hal_status == HAL_OK)
      {
        /* Set ADC state */
        ADC_STATE_CLR_SET(hadc->State,
                          HAL_ADC_STATE_REG_BUSY | HAL_ADC_STATE_INJ_BUSY,
                          HAL_ADC_STATE_READY);
      }
    }
    /* Conversion on injected group is stopped, but ADC not disabled since    */
    /* conversion on regular group is still running.                          */
    else
    {
      /* Set ADC state */
      CLEAR_BIT(hadc->State, HAL_ADC_STATE_INJ_BUSY);
    }
  }

  __HAL_UNLOCK(hadc);

  return tmp_hal_status;
}

/**
  * @brief  Wait for injected group conversion to be completed.
  * @param hadc ADC handle
  * @param Timeout Timeout value in millisecond.
  * @note   Depending on hadc->Init.EOCSelection, JEOS or JEOC is
  *         checked and cleared depending on AUTDLY bit status.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedPollForConversion(ADC_HandleTypeDef *hadc, uint32_t Timeout)
{
  uint32_t tickstart;
  uint32_t tmp_Flag_End;
  uint32_t tmp_adc_inj_is_trigger_source_sw_start;
  uint32_t tmp_adc_reg_is_trigger_source_sw_start;
  uint32_t tmp_cfgr;
#if defined(ADC_MULTIMODE_SUPPORT)
  const ADC_TypeDef *tmpADC_Master;
  uint32_t tmp_multimode_config = LL_ADC_GetMultimode(__LL_ADC_COMMON_INSTANCE(hadc->Instance));
#endif /* ADC_MULTIMODE_SUPPORT */

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));

  /* If end of sequence selected */
  if (hadc->Init.EOCSelection == ADC_EOC_SEQ_CONV)
  {
    tmp_Flag_End = ADC_FLAG_JEOS;
  }
  else /* end of conversion selected */
  {
    tmp_Flag_End = ADC_FLAG_JEOC;
  }

  /* Get timeout */
  tickstart = HAL_GetTick();

  /* Wait until End of Conversion or Sequence flag is raised */
  while ((hadc->Instance->ISR & tmp_Flag_End) == 0UL)
  {
    /* Check if timeout is disabled (set to infinite wait) */
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0UL))
      {
        /* New check to avoid false timeout detection in case of preemption */
        if ((hadc->Instance->ISR & tmp_Flag_End) == 0UL)
        {
          /* Update ADC state machine to timeout */
          SET_BIT(hadc->State, HAL_ADC_STATE_TIMEOUT);

          __HAL_UNLOCK(hadc);

          return HAL_TIMEOUT;
        }
      }
    }
  }

  /* Retrieve ADC configuration */
  tmp_adc_inj_is_trigger_source_sw_start = LL_ADC_INJ_IsTriggerSourceSWStart(hadc->Instance);
  tmp_adc_reg_is_trigger_source_sw_start = LL_ADC_REG_IsTriggerSourceSWStart(hadc->Instance);
  /* Get relevant register CFGR1 in ADC instance of ADC master or slave  */
  /* in function of multimode state (for devices with multimode         */
  /* available).                                                        */
#if defined(ADC_MULTIMODE_SUPPORT)
  if ((__LL_ADC_MULTI_INSTANCE_MASTER(hadc->Instance) == hadc->Instance)
      || (tmp_multimode_config == LL_ADC_MULTI_INDEPENDENT)
      || (tmp_multimode_config == LL_ADC_MULTI_DUAL_REG_SIMULT)
      || (tmp_multimode_config == LL_ADC_MULTI_DUAL_REG_INTERL)
     )
  {
    tmp_cfgr = READ_REG(hadc->Instance->CFGR1);
  }
  else
  {
    tmpADC_Master = __LL_ADC_MULTI_INSTANCE_MASTER(hadc->Instance);
    tmp_cfgr = READ_REG(tmpADC_Master->CFGR1);
  }
#else
  tmp_cfgr = READ_REG(hadc->Instance->CFGR1);
#endif /* ADC_MULTIMODE_SUPPORT */

  /* Update ADC state machine */
  SET_BIT(hadc->State, HAL_ADC_STATE_INJ_EOC);

  /* Determine whether any further conversion upcoming on group injected      */
  /* by external trigger or by automatic injected conversion                  */
  /* from group regular.                                                      */
  if ((tmp_adc_inj_is_trigger_source_sw_start != 0UL)            ||
      ((READ_BIT(tmp_cfgr, ADC_CFGR1_JAUTO) == 0UL)      &&
       ((tmp_adc_reg_is_trigger_source_sw_start != 0UL)  &&
        (READ_BIT(tmp_cfgr, ADC_CFGR1_CONT) == 0UL))))
  {
    /* Check whether end of sequence is reached */
    if (__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_JEOS))
    {
      /* Set ADC state */
      CLEAR_BIT(hadc->State, HAL_ADC_STATE_INJ_BUSY);

      if ((hadc->State & HAL_ADC_STATE_REG_BUSY) == 0UL)
      {
        SET_BIT(hadc->State, HAL_ADC_STATE_READY);
      }

    }
  }

  /* Clear polled flag */
  if (tmp_Flag_End == ADC_FLAG_JEOS)
  {
    /* Clear end of sequence JEOS flag of injected group if low power feature */
    /* "LowPowerAutoWait " is disabled, to not interfere with this feature.   */
    /* For injected groups, no new conversion will start before JEOS is       */
    /* cleared.                                                               */
    if (READ_BIT(tmp_cfgr, ADC_CFGR1_AUTDLY) == 0UL)
    {
      __HAL_ADC_CLEAR_FLAG(hadc, (ADC_FLAG_JEOC | ADC_FLAG_JEOS));
    }
  }
  else
  {
    __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_JEOC);
  }

  /* Return API HAL status */
  return HAL_OK;
}

/**
  * @brief  Enable ADC, start conversion of injected group with interruption.
  * @note   Interruptions enabled in this function according to initialization
  *         setting : JEOC (end of conversion) or JEOS (end of sequence)
  * @note   Case of multimode enabled (when multimode feature is enabled):
  *         HAL_ADCEx_InjectedStart_IT() API must be called for ADC slave first,
  *         then for ADC master.
  *         For ADC slave, ADC is enabled only (conversion is not started).
  *         For ADC master, ADC is enabled and multimode conversion is started.
  * @param hadc ADC handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedStart_IT(ADC_HandleTypeDef *hadc)
{
  HAL_StatusTypeDef tmp_hal_status;
#if defined(ADC_MULTIMODE_SUPPORT)
  uint32_t tmp_multimode_config = LL_ADC_GetMultimode(__LL_ADC_COMMON_INSTANCE(hadc->Instance));
#endif /* ADC_MULTIMODE_SUPPORT */

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));

  if (LL_ADC_INJ_IsConversionOngoing(hadc->Instance) != 0UL)
  {
    return HAL_BUSY;
  }
  else
  {
    /* Process locked */
    __HAL_LOCK(hadc);

    /* Enable the ADC peripheral */
    tmp_hal_status = ADC_Enable(hadc);

    /* Start conversion if ADC is effectively enabled */
    if (tmp_hal_status == HAL_OK)
    {
      /* Check if a regular conversion is ongoing */
      if ((hadc->State & HAL_ADC_STATE_REG_BUSY) == 0UL)
      {
        /* Set ADC error code to none */
        ADC_CLEAR_ERRORCODE(hadc);
      }

      /* Set ADC state                                                        */
      /* - Clear state bitfield related to injected group conversion results  */
      /* - Set state bitfield related to injected operation                   */
      ADC_STATE_CLR_SET(hadc->State,
                        HAL_ADC_STATE_READY | HAL_ADC_STATE_INJ_EOC,
                        HAL_ADC_STATE_INJ_BUSY);

#if defined(ADC_MULTIMODE_SUPPORT)
      /* Reset HAL_ADC_STATE_MULTIMODE_SLAVE bit
        - if ADC instance is master or if multimode feature is not available
        - if multimode setting is disabled (ADC instance slave in independent mode) */
      if ((__LL_ADC_MULTI_INSTANCE_MASTER(hadc->Instance) == hadc->Instance)
          || (tmp_multimode_config == LL_ADC_MULTI_INDEPENDENT)
         )
      {
        CLEAR_BIT(hadc->State, HAL_ADC_STATE_MULTIMODE_SLAVE);
      }
#endif /* ADC_MULTIMODE_SUPPORT */

      /* Clear ADC group injected group conversion flag */
      /* (To ensure of no unknown state from potential previous ADC operations) */
      __HAL_ADC_CLEAR_FLAG(hadc, (ADC_FLAG_JEOC | ADC_FLAG_JEOS));

      /* Unlock before starting ADC conversions: in case of potential         */
      /* interruption, to let the process to ADC IRQ Handler.                 */
      __HAL_UNLOCK(hadc);

      /* Enable ADC end of conversion interrupt */
      switch (hadc->Init.EOCSelection)
      {
        case ADC_EOC_SEQ_CONV:
          __HAL_ADC_DISABLE_IT(hadc, ADC_IT_JEOC);
          __HAL_ADC_ENABLE_IT(hadc, ADC_IT_JEOS);
          break;
        /* case ADC_EOC_SINGLE_CONV */
        default:
          __HAL_ADC_DISABLE_IT(hadc, ADC_IT_JEOS);
          __HAL_ADC_ENABLE_IT(hadc, ADC_IT_JEOC);
          break;
      }

      /* Enable conversion of injected group, if automatic injected conversion  */
      /* is disabled.                                                           */
      /* If software start has been selected, conversion starts immediately.    */
      /* If external trigger has been selected, conversion will start at next   */
      /* trigger event.                                                         */
      /* Case of multimode enabled (when multimode feature is available):       */
      /* if ADC is slave,                                                       */
      /*    - ADC is enabled only (conversion is not started),                  */
      /*    - if multimode only concerns regular conversion, ADC is enabled     */
      /*     and conversion is started.                                         */
      /* If ADC is master or independent,                                       */
      /*    - ADC is enabled and conversion is started.                         */
#if defined(ADC_MULTIMODE_SUPPORT)
      if ((__LL_ADC_MULTI_INSTANCE_MASTER(hadc->Instance) == hadc->Instance)
          || (tmp_multimode_config == LL_ADC_MULTI_INDEPENDENT)
          || (tmp_multimode_config == LL_ADC_MULTI_DUAL_REG_SIMULT)
          || (tmp_multimode_config == LL_ADC_MULTI_DUAL_REG_INTERL)
         )
      {
        /* ADC instance is not a multimode slave instance with multimode injected conversions enabled */
        if (LL_ADC_INJ_GetTrigAuto(hadc->Instance) == LL_ADC_INJ_TRIG_INDEPENDENT)
        {
          LL_ADC_INJ_StartConversion(hadc->Instance);
        }
      }
      else
      {
        /* ADC instance is not a multimode slave instance with multimode injected conversions enabled */
        SET_BIT(hadc->State, HAL_ADC_STATE_MULTIMODE_SLAVE);
      }
#else
      if (LL_ADC_INJ_GetTrigAuto(hadc->Instance) == LL_ADC_INJ_TRIG_INDEPENDENT)
      {
        /* Start ADC group injected conversion */
        LL_ADC_INJ_StartConversion(hadc->Instance);
      }
#endif /* ADC_MULTIMODE_SUPPORT */

    }
    else
    {
      __HAL_UNLOCK(hadc);
    }

    return tmp_hal_status;
  }
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
  * @note   Case of multimode enabled (when multimode feature is available):
  *         HAL_ADCEx_InjectedStop_IT() API must be called for ADC master first,
  *         then for ADC slave.
  *         For ADC master, conversion is stopped and ADC is disabled.
  *         For ADC slave, ADC is disabled only (conversion stop of ADC master
  *         has already stopped conversion of ADC slave).
  * @note   In case of auto-injection mode, HAL_ADC_Stop() must be used.
  * @param hadc ADC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedStop_IT(ADC_HandleTypeDef *hadc)
{
  HAL_StatusTypeDef tmp_hal_status;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));

  /* Process locked */
  __HAL_LOCK(hadc);

  /* 1. Stop potential conversion on going on injected group only. */
  tmp_hal_status = ADC_ConversionStop(hadc, ADC_INJECTED_GROUP);

  /* Disable ADC peripheral if injected conversions are effectively stopped   */
  /* and if no conversion on the other group (regular group) is intended to   */
  /* continue.                                                                */
  if (tmp_hal_status == HAL_OK)
  {
    /* Disable ADC end of conversion interrupt for injected channels */
    __HAL_ADC_DISABLE_IT(hadc, (ADC_IT_JEOC | ADC_IT_JEOS));

    if (LL_ADC_REG_IsConversionOngoing(hadc->Instance) == 0UL)
    {
      /* 2. Disable the ADC peripheral */
      tmp_hal_status = ADC_Disable(hadc);

      /* Check if ADC is effectively disabled */
      if (tmp_hal_status == HAL_OK)
      {
        /* Set ADC state */
        ADC_STATE_CLR_SET(hadc->State,
                          HAL_ADC_STATE_REG_BUSY | HAL_ADC_STATE_INJ_BUSY,
                          HAL_ADC_STATE_READY);
      }
    }
    /* Conversion on injected group is stopped, but ADC not disabled since    */
    /* conversion on regular group is still running.                          */
    else
    {
      /* Set ADC state */
      CLEAR_BIT(hadc->State, HAL_ADC_STATE_INJ_BUSY);
    }
  }

  __HAL_UNLOCK(hadc);

  return tmp_hal_status;
}

#if defined(ADC_MULTIMODE_SUPPORT)
/**
  * @brief  Enable ADC, start MultiMode conversion and transfer regular results through DMA.
  * @note   Multimode must have been previously configured using
  *         HAL_ADCEx_MultiModeConfigChannel() function.
  *         Interruptions enabled in this function:
  *          overrun, DMA half transfer, DMA transfer complete.
  *         Each of these interruptions has its dedicated callback function.
  * @note   Conversion data of both multimode ADC instances will take each one buffer element, therefore
            buffer total size should be doubled to get similar data size for each ADC instance vs independent mode.
  * @note   State field of Slave ADC handle is not updated in this configuration:
  *          user should not rely on it for information related to Slave regular
  *         conversions.
  * @param hadc ADC handle of ADC master (handle of ADC slave must not be used)
  * @param pData Destination Buffer address.
  * @param Length Length of data to be transferred from ADC peripheral to memory (unit: number of transfers)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_MultiModeStart_DMA(ADC_HandleTypeDef *hadc, uint32_t *pData, uint32_t Length)
{
  HAL_StatusTypeDef tmp_hal_status;
  ADC_HandleTypeDef tmphadcSlave;
  ADC_Common_TypeDef *tmp_adc_common;
  uint32_t LengthInBytes;
  DMA_NodeConfTypeDef node_conf;

  /* Check the parameters */
  assert_param(IS_ADC_MULTIMODE_MASTER_INSTANCE(hadc->Instance));
  assert_param(IS_FUNCTIONAL_STATE(hadc->Init.ContinuousConvMode));
  assert_param(IS_ADC_EXTTRIG_EDGE(hadc->Init.ExternalTrigConvEdge));

  if (LL_ADC_REG_IsConversionOngoing(hadc->Instance) != 0UL)
  {
    return HAL_BUSY;
  }
  else
  {
    /* Process locked */
    __HAL_LOCK(hadc);

    /* Temporary handle minimum initialization */
    __HAL_ADC_RESET_HANDLE_STATE(&tmphadcSlave);
    ADC_CLEAR_ERRORCODE(&tmphadcSlave);

    /* Set a temporary handle of the ADC slave associated to the ADC master   */
    ADC_MULTI_SLAVE(hadc, &tmphadcSlave);

    if (tmphadcSlave.Instance == NULL)
    {
      /* Set ADC state */
      SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);

      __HAL_UNLOCK(hadc);

      return HAL_ERROR;
    }

    /* Enable the ADC peripherals: master and slave (in case if not already   */
    /* enabled previously)                                                    */
    tmp_hal_status = ADC_Enable(hadc);
    if (tmp_hal_status == HAL_OK)
    {
      tmp_hal_status = ADC_Enable(&tmphadcSlave);
    }

    /* Start multimode conversion of ADCs pair */
    if (tmp_hal_status == HAL_OK)
    {
      /* Set ADC state */
      ADC_STATE_CLR_SET(hadc->State,
                        (HAL_ADC_STATE_READY | HAL_ADC_STATE_REG_EOC | HAL_ADC_STATE_REG_OVR | HAL_ADC_STATE_REG_EOSMP),
                        HAL_ADC_STATE_REG_BUSY);

      /* Set ADC error code to none */
      ADC_CLEAR_ERRORCODE(hadc);

      /* Set the DMA transfer complete callback */
      hadc->DMA_Handle->XferCpltCallback = ADC_DMAConvCplt;

      /* Set the DMA half transfer complete callback */
      hadc->DMA_Handle->XferHalfCpltCallback = ADC_DMAHalfConvCplt;

      /* Set the DMA error callback */
      hadc->DMA_Handle->XferErrorCallback = ADC_DMAError ;

      /* Pointer to the common control register  */
      tmp_adc_common = __LL_ADC_COMMON_INSTANCE(hadc->Instance);

      /* Manage ADC and DMA start: ADC overrun interruption, DMA start, ADC     */
      /* start (in case of SW start):                                           */

      /* Clear regular group conversion flag and overrun flag */
      /* (To ensure of no unknown state from potential previous ADC operations) */
      __HAL_ADC_CLEAR_FLAG(hadc, (ADC_FLAG_EOC | ADC_FLAG_EOS | ADC_FLAG_OVR));

      /* Unlock before starting ADC conversions: in case of potential         */
      /* interruption, to let the process to ADC IRQ Handler.                 */
      __HAL_UNLOCK(hadc);

      /* Enable ADC overrun interrupt */
      __HAL_ADC_ENABLE_IT(hadc, ADC_IT_OVR);

      /* Start the DMA channel */
      if ((hadc->DMA_Handle->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
      {
        if ((hadc->DMA_Handle->LinkedListQueue != NULL) && (hadc->DMA_Handle->LinkedListQueue->Head != NULL))
        {
          /* Length should be converted to number of bytes */
          if (HAL_DMAEx_List_GetNodeConfig(&node_conf, hadc->DMA_Handle->LinkedListQueue->Head) != HAL_OK)
          {
            return HAL_ERROR;
          }

          /* Length should be converted to number of bytes */
          if (node_conf.Init.SrcDataWidth == DMA_SRC_DATAWIDTH_WORD)
          {
            /* Word -> Bytes */
            LengthInBytes = Length * 4U;
          }
          else if (node_conf.Init.SrcDataWidth == DMA_SRC_DATAWIDTH_HALFWORD)
          {
            /* Halfword -> Bytes */
            LengthInBytes = Length * 2U;
          }
          else /* Bytes */
          {
            /* Same size already expressed in Bytes */
            LengthInBytes = Length;
          }

          hadc->DMA_Handle->LinkedListQueue->Head->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] = (uint32_t)LengthInBytes;
          hadc->DMA_Handle->LinkedListQueue->Head->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET] =
            (uint32_t)&tmp_adc_common->CDR;
          hadc->DMA_Handle->LinkedListQueue->Head->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] = (uint32_t)pData;

          tmp_hal_status = HAL_DMAEx_List_Start_IT(hadc->DMA_Handle);
        }
        else
        {
          tmp_hal_status = HAL_ERROR;
        }
      }
      else
      {
        /* Length should be converted to number of bytes */
        if (hadc->DMA_Handle->Init.SrcDataWidth == DMA_SRC_DATAWIDTH_WORD)
        {
          /* Word -> Bytes */
          LengthInBytes = Length * 4U;
        }
        else if (hadc->DMA_Handle->Init.SrcDataWidth == DMA_SRC_DATAWIDTH_HALFWORD)
        {
          /* Halfword -> Bytes */
          LengthInBytes = Length * 2U;
        }
        else /* Bytes */
        {
          /* Same size already expressed in Bytes */
          LengthInBytes = Length;
        }

        tmp_hal_status = HAL_DMA_Start_IT(hadc->DMA_Handle, (uint32_t)&tmp_adc_common->CDR, (uint32_t)pData,
                                          LengthInBytes);
      }

      if (tmp_hal_status != HAL_ERROR)
      {
        /* Enable conversion of regular group.                                    */
        /* If software start has been selected, conversion starts immediately.    */
        /* If external trigger has been selected, conversion will start at next   */
        /* trigger event.                                                         */
        /* Start ADC group regular conversion */
        LL_ADC_REG_StartConversion(hadc->Instance);
      }
    }
    else
    {
      __HAL_UNLOCK(hadc);
    }

    return tmp_hal_status;
  }
}

/**
  * @brief  Stop multimode ADC conversion, disable ADC DMA transfer, disable ADC peripheral.
  * @note   Multimode is kept enabled after this function. MultiMode DMA bits
  *         (MDMA and DMACFG bits of common CCR register) are maintained. To disable
  *         Multimode (set with HAL_ADCEx_MultiModeConfigChannel()), ADC must be
  *         reinitialized using HAL_ADC_Init() or HAL_ADC_DeInit(), or the user can
  *         resort to HAL_ADCEx_DisableMultiMode() API.
  * @note   In case of DMA configured in circular mode, function
  *         HAL_ADC_Stop_DMA() must be called after this function with handle of
  *         ADC slave, to properly disable the DMA channel.
  * @param hadc ADC handle of ADC master (handle of ADC slave must not be used)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_MultiModeStop_DMA(ADC_HandleTypeDef *hadc)
{
  HAL_StatusTypeDef tmp_hal_status;
  uint32_t tickstart;
  ADC_HandleTypeDef tmphadcSlave;
  uint32_t tmphadcSlave_conversion_on_going;
  HAL_StatusTypeDef tmphadcSlave_disable_status;

  /* Check the parameters */
  assert_param(IS_ADC_MULTIMODE_MASTER_INSTANCE(hadc->Instance));

  /* Process locked */
  __HAL_LOCK(hadc);


  /* 1. Stop potential multimode conversion on going, on regular and injected groups */
  tmp_hal_status = ADC_ConversionStop(hadc, ADC_REGULAR_INJECTED_GROUP);

  /* Disable ADC peripheral if conversions are effectively stopped */
  if (tmp_hal_status == HAL_OK)
  {
    /* Temporary handle minimum initialization */
    __HAL_ADC_RESET_HANDLE_STATE(&tmphadcSlave);
    ADC_CLEAR_ERRORCODE(&tmphadcSlave);

    /* Set a temporary handle of the ADC slave associated to the ADC master   */
    ADC_MULTI_SLAVE(hadc, &tmphadcSlave);

    if (tmphadcSlave.Instance == NULL)
    {
      /* Update ADC state machine to error */
      SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);

      __HAL_UNLOCK(hadc);

      return HAL_ERROR;
    }

    /* Procedure to disable the ADC peripheral: wait for conversions          */
    /* effectively stopped (ADC master and ADC slave), then disable ADC       */

    /* 1. Wait for ADC conversion completion for ADC master and ADC slave */
    tickstart = HAL_GetTick();

    tmphadcSlave_conversion_on_going = LL_ADC_REG_IsConversionOngoing((&tmphadcSlave)->Instance);
    while ((LL_ADC_REG_IsConversionOngoing(hadc->Instance) == 1UL)
           || (tmphadcSlave_conversion_on_going == 1UL)
          )
    {
      if ((HAL_GetTick() - tickstart) > ADC_STOP_CONVERSION_TIMEOUT)
      {
        /* New check to avoid false timeout detection in case of preemption */
        tmphadcSlave_conversion_on_going = LL_ADC_REG_IsConversionOngoing((&tmphadcSlave)->Instance);
        if ((LL_ADC_REG_IsConversionOngoing(hadc->Instance) == 1UL)
            || (tmphadcSlave_conversion_on_going == 1UL)
           )
        {
          /* Update ADC state machine to error */
          SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL);

          __HAL_UNLOCK(hadc);

          return HAL_ERROR;
        }
      }

      tmphadcSlave_conversion_on_going = LL_ADC_REG_IsConversionOngoing((&tmphadcSlave)->Instance);
    }

    /* Disable the DMA channel (in case of DMA in circular mode or stop       */
    /* while DMA transfer is on going)                                        */
    /* Note: DMA channel of ADC slave should be stopped after this function   */
    /*       with HAL_ADC_Stop_DMA() API.                                     */
    if (hadc->DMA_Handle->State == HAL_DMA_STATE_BUSY)
    {
      tmp_hal_status = HAL_DMA_Abort(hadc->DMA_Handle);

      /* Check if DMA channel effectively disabled */
      if (tmp_hal_status == HAL_ERROR)
      {
        /* Update ADC state machine to error */
        SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_DMA);
      }
    }

    /* Disable ADC overrun interrupt */
    __HAL_ADC_DISABLE_IT(hadc, ADC_IT_OVR);

    /* 2. Disable the ADC peripherals: master and slave */
    /* Update "tmp_hal_status" only if DMA channel disabling passed, to keep in */
    /* memory a potential failing status.                                     */
    if (tmp_hal_status == HAL_OK)
    {
      tmphadcSlave_disable_status = ADC_Disable(&tmphadcSlave);
      if ((ADC_Disable(hadc) == HAL_OK)           &&
          (tmphadcSlave_disable_status == HAL_OK))
      {
        tmp_hal_status = HAL_OK;
      }
    }
    else
    {
      /* In case of error, attempt to disable ADC master and slave without status assert */
      (void) ADC_Disable(hadc);
      (void) ADC_Disable(&tmphadcSlave);
    }

    /* Set ADC state (ADC master) */
    ADC_STATE_CLR_SET(hadc->State,
                      HAL_ADC_STATE_REG_BUSY | HAL_ADC_STATE_INJ_BUSY,
                      HAL_ADC_STATE_READY);
  }

  __HAL_UNLOCK(hadc);

  return tmp_hal_status;
}

/**
  * @brief  Return the last ADC Master and Slave regular conversions results when in multimode configuration.
  * @note   Multimode data contains ADC master and slave conversion data
  *         concatenated. Data is usable under conditions of ADC multimode
  *         data format selected and data width,
  *         refer to @ref ADCEx_Dual_Mode_Data_Format.
  *         The recommended method, without any constraint, is to use
  *         DMA transfer. refer to @ref HAL_ADCEx_MultiModeStart_DMA.
  * @note   Another solution exists to retrieve multimode conversion data
  *         without packing (refer to @ref LL_ADC_REG_ReadMultiConvNoPacking)
  *         but with timing constraints.
  *         The recommended method, without any constraint, is to use
  *         DMA transfer. refer to @ref HAL_ADCEx_MultiModeStart_DMA.
  * @param hadc ADC handle of ADC Master (handle of ADC Slave must not be used)
  * @retval The converted data values.
  */
uint32_t HAL_ADCEx_MultiModeGetValue(const ADC_HandleTypeDef *hadc)
{
  const ADC_Common_TypeDef *tmpADC_Common;

  /* Check the parameters */
  assert_param(IS_ADC_MULTIMODE_MASTER_INSTANCE(hadc->Instance));

  /* Prevent unused argument(s) compilation warning if no assert_param check */
  /* and possible no usage in __LL_ADC_COMMON_INSTANCE() below               */
  UNUSED(hadc);

  /* Pointer to the common control register  */
  tmpADC_Common = __LL_ADC_COMMON_INSTANCE(hadc->Instance);

  /* Return the multi mode conversion value */
  return tmpADC_Common->CDR;
}
#endif /* ADC_MULTIMODE_SUPPORT */

/**
  * @brief  Get ADC injected group conversion result.
  * @note   Reading register JDRx automatically clears ADC flag JEOC
  *         (ADC group injected end of unitary conversion).
  * @note   This function returns an unsigned value. Using the ADC offset
  *         feature can result in negative conversion data.
  *         To read conversion data with ADC offset enabled
  *         use function @ref HAL_ADCEx_InjectedGetSignedValue.
  * @note   This function does not clear ADC flag JEOS
  *         (ADC group injected end of sequence conversion)
  *         Occurrence of flag JEOS rising:
  *          - If sequencer is composed of 1 rank, flag JEOS is equivalent
  *            to flag JEOC.
  *          - If sequencer is composed of several ranks, during the scan
  *            sequence flag JEOC only is raised, at the end of the scan sequence
  *            both flags JEOC and EOS are raised.
  *         Flag JEOS must not be cleared by this function because
  *         it would not be compliant with low power features
  *         (feature low power auto-wait, not available on all STM32 families).
  *         To clear this flag, either use function:
  *         in programming model IT: @ref HAL_ADC_IRQHandler(), in programming
  *         model polling: @ref HAL_ADCEx_InjectedPollForConversion()
  *         or @ref __HAL_ADC_CLEAR_FLAG(&hadc, ADC_FLAG_JEOS).
  * @param hadc ADC handle
  * @param InjectedRank the converted ADC injected rank.
  *          This parameter can be one of the following values:
  *            @arg @ref ADC_INJECTED_RANK_1 ADC group injected rank 1
  *            @arg @ref ADC_INJECTED_RANK_2 ADC group injected rank 2
  *            @arg @ref ADC_INJECTED_RANK_3 ADC group injected rank 3
  *            @arg @ref ADC_INJECTED_RANK_4 ADC group injected rank 4
  * @retval ADC group injected conversion data
  */
uint32_t HAL_ADCEx_InjectedGetValue(const ADC_HandleTypeDef *hadc, uint32_t InjectedRank)
{
  uint32_t tmp_jdr;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  assert_param(IS_ADC_INJECTED_RANK(InjectedRank));

  /* Get ADC converted value */
  switch (InjectedRank)
  {
    case ADC_INJECTED_RANK_4:
      tmp_jdr = hadc->Instance->JDR4;
      break;
    case ADC_INJECTED_RANK_3:
      tmp_jdr = hadc->Instance->JDR3;
      break;
    case ADC_INJECTED_RANK_2:
      tmp_jdr = hadc->Instance->JDR2;
      break;
    case ADC_INJECTED_RANK_1:
    default:
      tmp_jdr = hadc->Instance->JDR1;
      break;
  }

  /* Return ADC converted value */
  return tmp_jdr;
}

/**
  * @brief  Get ADC injected group conversion result.
  * @note   Reading register JDRx automatically clears ADC flag JEOC
  *         (ADC group injected end of unitary conversion).
  * @note   This function does not clear ADC flag JEOS
  *         (ADC group injected end of sequence conversion)
  *         Occurrence of flag JEOS rising:
  *          - If sequencer is composed of 1 rank, flag JEOS is equivalent
  *            to flag JEOC.
  *          - If sequencer is composed of several ranks, during the scan
  *            sequence flag JEOC only is raised, at the end of the scan sequence
  *            both flags JEOC and EOS are raised.
  *         Flag JEOS must not be cleared by this function because
  *         it would not be compliant with low power features
  *         (feature low power auto-wait, not available on all STM32 families).
  *         To clear this flag, either use function:
  *         in programming model IT: @ref HAL_ADC_IRQHandler(), in programming
  *         model polling: @ref HAL_ADCEx_InjectedPollForConversion()
  *         or @ref __HAL_ADC_CLEAR_FLAG(&hadc, ADC_FLAG_JEOS).
  * @param hadc ADC handle
  * @param InjectedRank the converted ADC injected rank.
  *          This parameter can be one of the following values:
  *            @arg @ref ADC_INJECTED_RANK_1 ADC group injected rank 1
  *            @arg @ref ADC_INJECTED_RANK_2 ADC group injected rank 2
  *            @arg @ref ADC_INJECTED_RANK_3 ADC group injected rank 3
  *            @arg @ref ADC_INJECTED_RANK_4 ADC group injected rank 4
  * @retval ADC group injected conversion data
  */
int32_t HAL_ADCEx_InjectedGetSignedValue(const ADC_HandleTypeDef *hadc, uint32_t InjectedRank)
{
  int32_t tmp_jdr;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  assert_param(IS_ADC_INJECTED_RANK(InjectedRank));

  /* Get ADC converted value */
  switch (InjectedRank)
  {
    case ADC_INJECTED_RANK_4:
      tmp_jdr = (int32_t)(hadc->Instance->JDR4);
      break;
    case ADC_INJECTED_RANK_3:
      tmp_jdr = (int32_t)(hadc->Instance->JDR3);
      break;
    case ADC_INJECTED_RANK_2:
      tmp_jdr = (int32_t)(hadc->Instance->JDR2);
      break;
    case ADC_INJECTED_RANK_1:
    default:
      tmp_jdr = (int32_t)(hadc->Instance->JDR1);
      break;
  }

  /* Return ADC converted value */
  return tmp_jdr;
}

/**
  * @brief  Injected conversion complete callback in non-blocking mode.
  * @param hadc ADC handle
  * @retval None
  */
__weak void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);

  /* NOTE : This function should not be modified. When the callback is needed,
            function HAL_ADCEx_InjectedConvCpltCallback must be implemented in the user file.
  */
}

/**
  * @brief  Analog watchdog 2 callback in non-blocking mode.
  * @param hadc ADC handle
  * @retval None
  */
__weak void HAL_ADCEx_LevelOutOfWindow2Callback(ADC_HandleTypeDef *hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);

  /* NOTE : This function should not be modified. When the callback is needed,
            function HAL_ADCEx_LevelOutOfWindow2Callback must be implemented in the user file.
  */
}

/**
  * @brief  Analog watchdog 3 callback in non-blocking mode.
  * @param hadc ADC handle
  * @retval None
  */
__weak void HAL_ADCEx_LevelOutOfWindow3Callback(ADC_HandleTypeDef *hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);

  /* NOTE : This function should not be modified. When the callback is needed,
            function HAL_ADCEx_LevelOutOfWindow3Callback must be implemented in the user file.
  */
}


/**
  * @brief  End Of Sampling callback in non-blocking mode.
  * @param hadc ADC handle
  * @retval None
  */
__weak void HAL_ADCEx_EndOfSamplingCallback(ADC_HandleTypeDef *hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);

  /* NOTE : This function should not be modified. When the callback is needed,
            function HAL_ADCEx_EndOfSamplingCallback must be implemented in the user file.
  */
}

/**
  * @brief  Stop ADC conversion of regular group (and injected channels in
  *         case of auto_injection mode), disable ADC peripheral if no
  *         conversion is on going on injected group.
  * @param hadc ADC handle
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADCEx_RegularStop(ADC_HandleTypeDef *hadc)
{
  HAL_StatusTypeDef tmp_hal_status;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));

  /* Process locked */
  __HAL_LOCK(hadc);

  /* 1. Stop potential regular conversion on going */
  tmp_hal_status = ADC_ConversionStop(hadc, ADC_REGULAR_GROUP);

  /* Disable ADC peripheral if regular conversions are effectively stopped
     and if no injected conversions are on-going */
  if (tmp_hal_status == HAL_OK)
  {
    /* Clear HAL_ADC_STATE_REG_BUSY bit */
    CLEAR_BIT(hadc->State, HAL_ADC_STATE_REG_BUSY);

    if (LL_ADC_INJ_IsConversionOngoing(hadc->Instance) == 0UL)
    {
      /* 2. Disable the ADC peripheral */
      tmp_hal_status = ADC_Disable(hadc);

      /* Check if ADC is effectively disabled */
      if (tmp_hal_status == HAL_OK)
      {
        /* Set ADC state */
        ADC_STATE_CLR_SET(hadc->State,
                          HAL_ADC_STATE_INJ_BUSY,
                          HAL_ADC_STATE_READY);
      }
    }
    /* Conversion on injected group is stopped, but ADC not disabled since    */
    /* conversion on regular group is still running.                          */
    else
    {
      SET_BIT(hadc->State, HAL_ADC_STATE_INJ_BUSY);
    }
  }

  __HAL_UNLOCK(hadc);

  return tmp_hal_status;
}


/**
  * @brief  Stop ADC conversion of ADC groups regular and injected,
  *         disable interrution of end-of-conversion,
  *         disable ADC peripheral if no conversion is on going
  *         on injected group.
  * @param hadc ADC handle
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADCEx_RegularStop_IT(ADC_HandleTypeDef *hadc)
{
  HAL_StatusTypeDef tmp_hal_status;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));

  /* Process locked */
  __HAL_LOCK(hadc);

  /* 1. Stop potential regular conversion on going */
  tmp_hal_status = ADC_ConversionStop(hadc, ADC_REGULAR_GROUP);

  /* Disable ADC peripheral if conversions are effectively stopped
    and if no injected conversion is on-going */
  if (tmp_hal_status == HAL_OK)
  {
    /* Clear HAL_ADC_STATE_REG_BUSY bit */
    CLEAR_BIT(hadc->State, HAL_ADC_STATE_REG_BUSY);

    /* Disable all regular-related interrupts */
    __HAL_ADC_DISABLE_IT(hadc, (ADC_IT_EOC | ADC_IT_EOS | ADC_IT_OVR));

    /* 2. Disable ADC peripheral if no injected conversions are on-going */
    if (LL_ADC_INJ_IsConversionOngoing(hadc->Instance) == 0UL)
    {
      tmp_hal_status = ADC_Disable(hadc);
      /* if no issue reported */
      if (tmp_hal_status == HAL_OK)
      {
        /* Set ADC state */
        ADC_STATE_CLR_SET(hadc->State,
                          HAL_ADC_STATE_INJ_BUSY,
                          HAL_ADC_STATE_READY);
      }
    }
    else
    {
      SET_BIT(hadc->State, HAL_ADC_STATE_INJ_BUSY);
    }
  }

  __HAL_UNLOCK(hadc);

  return tmp_hal_status;
}

/**
  * @brief  Stop ADC conversion of regular group (and injected group in
  *         case of auto_injection mode), disable ADC DMA transfer, disable
  *         ADC peripheral if no conversion is on going
  *         on injected group.
  * @note   HAL_ADCEx_RegularStop_DMA() function is dedicated to single-ADC mode only.
  *         For multimode (when multimode feature is available),
  *         HAL_ADCEx_RegularMultiModeStop_DMA() API must be used.
  * @param hadc ADC handle
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADCEx_RegularStop_DMA(ADC_HandleTypeDef *hadc)
{
  HAL_StatusTypeDef tmp_hal_status;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));

  /* Process locked */
  __HAL_LOCK(hadc);

  /* 1. Stop potential regular conversion on going */
  tmp_hal_status = ADC_ConversionStop(hadc, ADC_REGULAR_GROUP);

  /* Disable ADC peripheral if conversions are effectively stopped
     and if no injected conversion is on-going */
  if (tmp_hal_status == HAL_OK)
  {
    /* Clear HAL_ADC_STATE_REG_BUSY bit */
    CLEAR_BIT(hadc->State, HAL_ADC_STATE_REG_BUSY);

    /* Disable ADC DMA (ADC DMA configuration ADC_CFGR1_DMACFG is kept) */
    MODIFY_REG(hadc->Instance->CFGR1, ADC_CFGR1_DMNGT_0 | ADC_CFGR1_DMNGT_1, 0UL);

    /* Disable the DMA channel (in case of DMA in circular mode or stop while */
    /* while DMA transfer is on going)                                        */
    tmp_hal_status = HAL_DMA_Abort(hadc->DMA_Handle);

    /* Check if DMA channel effectively disabled */
    if (tmp_hal_status != HAL_OK)
    {
      /* Update ADC state machine to error */
      SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_DMA);
    }

    /* Disable ADC overrun interrupt */
    __HAL_ADC_DISABLE_IT(hadc, ADC_IT_OVR);

    /* 2. Disable the ADC peripheral */
    /* Update "tmp_hal_status" only if DMA channel disabling passed,          */
    /* to keep in memory a potential failing status.                          */
    if (LL_ADC_INJ_IsConversionOngoing(hadc->Instance) == 0UL)
    {
      if (tmp_hal_status == HAL_OK)
      {
        tmp_hal_status = ADC_Disable(hadc);
      }
      else
      {
        (void)ADC_Disable(hadc);
      }

      /* Check if ADC is effectively disabled */
      if (tmp_hal_status == HAL_OK)
      {
        /* Set ADC state */
        ADC_STATE_CLR_SET(hadc->State,
                          HAL_ADC_STATE_INJ_BUSY,
                          HAL_ADC_STATE_READY);
      }
    }
    else
    {
      SET_BIT(hadc->State, HAL_ADC_STATE_INJ_BUSY);
    }
  }

  __HAL_UNLOCK(hadc);

  return tmp_hal_status;
}

#if defined(ADC_MULTIMODE_SUPPORT)
/**
  * @brief  Stop DMA-based multimode ADC conversion, disable ADC DMA transfer, disable ADC peripheral if no injected
  *         conversion is on-going.
  * @note   Multimode is kept enabled after this function. Multimode DMA bits
  *         (MDMA and DMACFG bits of common CCR register) are maintained. To disable
  *         multimode (set with HAL_ADCEx_MultiModeConfigChannel()), ADC must be
  *         reinitialized using HAL_ADC_Init() or HAL_ADC_DeInit(), or the user can
  *         resort to HAL_ADCEx_DisableMultiMode() API.
  * @note   In case of DMA configured in circular mode, function
  *         HAL_ADCEx_RegularStop_DMA() must be called after this function with handle of
  *         ADC slave, to properly disable the DMA channel.
  * @param hadc ADC handle of ADC master (handle of ADC slave must not be used)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_RegularMultiModeStop_DMA(ADC_HandleTypeDef *hadc)
{
  HAL_StatusTypeDef tmp_hal_status;
  uint32_t tickstart;
  ADC_HandleTypeDef tmphadcSlave;
  uint32_t tmphadcSlave_conversion_on_going;

  /* Check the parameters */
  assert_param(IS_ADC_MULTIMODE_MASTER_INSTANCE(hadc->Instance));

  /* Process locked */
  __HAL_LOCK(hadc);


  /* 1. Stop potential multimode conversion on going, on regular groups */
  tmp_hal_status = ADC_ConversionStop(hadc, ADC_REGULAR_GROUP);

  /* Disable ADC peripheral if conversions are effectively stopped */
  if (tmp_hal_status == HAL_OK)
  {
    /* Clear HAL_ADC_STATE_REG_BUSY bit */
    CLEAR_BIT(hadc->State, HAL_ADC_STATE_REG_BUSY);

    /* Temporary handle minimum initialization */
    __HAL_ADC_RESET_HANDLE_STATE(&tmphadcSlave);
    ADC_CLEAR_ERRORCODE(&tmphadcSlave);

    /* Set a temporary handle of the ADC slave associated to the ADC master   */
    ADC_MULTI_SLAVE(hadc, &tmphadcSlave);

    if (tmphadcSlave.Instance == NULL)
    {
      /* Update ADC state machine to error */
      SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);

      __HAL_UNLOCK(hadc);

      return HAL_ERROR;
    }

    /* Procedure to disable the ADC peripheral: wait for conversions          */
    /* effectively stopped (ADC master and ADC slave), then disable ADC       */

    /* 1. Wait for ADC conversion completion for ADC master and ADC slave */
    tickstart = HAL_GetTick();

    tmphadcSlave_conversion_on_going = LL_ADC_REG_IsConversionOngoing((&tmphadcSlave)->Instance);
    while ((LL_ADC_REG_IsConversionOngoing(hadc->Instance) == 1UL)
           || (tmphadcSlave_conversion_on_going == 1UL)
          )
    {
      if ((HAL_GetTick() - tickstart) > ADC_STOP_CONVERSION_TIMEOUT)
      {
        /* New check to avoid false timeout detection in case of preemption */
        tmphadcSlave_conversion_on_going = LL_ADC_REG_IsConversionOngoing((&tmphadcSlave)->Instance);
        if ((LL_ADC_REG_IsConversionOngoing(hadc->Instance) == 1UL)
            || (tmphadcSlave_conversion_on_going == 1UL)
           )
        {
          /* Update ADC state machine to error */
          SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL);

          __HAL_UNLOCK(hadc);

          return HAL_ERROR;
        }
      }

      tmphadcSlave_conversion_on_going = LL_ADC_REG_IsConversionOngoing((&tmphadcSlave)->Instance);
    }

    /* Disable the DMA channel (in case of DMA in circular mode or stop       */
    /* while DMA transfer is on going)                                        */
    /* Note: DMA channel of ADC slave should be stopped after this function   */
    /* with HAL_ADCEx_RegularStop_DMA() API.                                  */
    tmp_hal_status = HAL_DMA_Abort(hadc->DMA_Handle);

    /* Check if DMA channel effectively disabled */
    if (tmp_hal_status != HAL_OK)
    {
      /* Update ADC state machine to error */
      SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_DMA);
    }

    /* Disable ADC overrun interrupt */
    __HAL_ADC_DISABLE_IT(hadc, ADC_IT_OVR);

    /* 2. Disable the ADC peripherals: master and slave if no injected        */
    /*   conversion is on-going.                                              */
    /* Update "tmp_hal_status" only if DMA channel disabling passed, to keep in */
    /* memory a potential failing status.                                     */
    if (tmp_hal_status == HAL_OK)
    {
      if (LL_ADC_INJ_IsConversionOngoing(hadc->Instance) == 0UL)
      {
        tmp_hal_status =  ADC_Disable(hadc);
        if (tmp_hal_status == HAL_OK)
        {
          if (LL_ADC_INJ_IsConversionOngoing((&tmphadcSlave)->Instance) == 0UL)
          {
            tmp_hal_status =  ADC_Disable(&tmphadcSlave);
          }
        }
      }

      if (tmp_hal_status == HAL_OK)
      {
        /* Both Master and Slave ADC's could be disabled. Update Master State */
        /* Clear HAL_ADC_STATE_INJ_BUSY bit, set HAL_ADC_STATE_READY bit */
        ADC_STATE_CLR_SET(hadc->State, HAL_ADC_STATE_INJ_BUSY, HAL_ADC_STATE_READY);
      }
      else
      {
        /* injected (Master or Slave) conversions are still on-going,
           no Master State change */
      }
    }
  }

  __HAL_UNLOCK(hadc);

  return tmp_hal_status;
}
#endif /* ADC_MULTIMODE_SUPPORT */

/**
  * @}
  */

/** @defgroup ADCEx_Exported_Functions_Group2 ADC Extended Peripheral Control functions
  * @brief    ADC Extended Peripheral Control functions
  *
@verbatim
 ===============================================================================
             ##### Peripheral Control functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Configure channels on injected group
      (+) Configure multimode (when multimode feature is available)
      (+) Disable ADC voltage regulator
      (+) Enter ADC deep-power-down mode

@endverbatim
  * @{
  */

/**
  * @brief  Configure a channel to be assigned to ADC group injected.
  * @note   Possibility to update parameters on the fly:
  *         This function initializes injected group, following calls to this
  *         function can be used to reconfigure some parameters of structure
  *         "ADC_InjectionConfTypeDef" on the fly, without resetting the ADC.
  *         The setting of these parameters is conditioned to ADC state:
  *         Refer to comments of structure "ADC_InjectionConfTypeDef".
  * @note   In case of usage of internal measurement channels (VrefInt, ...):
  *         These internal paths can be disabled using function
  *         HAL_ADC_DeInit().
  * @param hadc ADC handle
  * @param pConfigInjected Structure of ADC injected group and ADC channel for
  *         injected group.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedConfigChannel(ADC_HandleTypeDef *hadc,
                                                  const ADC_InjectionConfTypeDef *pConfigInjected)
{
  HAL_StatusTypeDef tmp_hal_status = HAL_OK;
  uint32_t tmpOffsetShifted;
  uint32_t tmp_config_common_path_internal_channel;
  uint32_t tmp_config_path_internal_channel;
  uint32_t tmp_adc_is_conversion_on_going_regular;
  uint32_t tmp_adc_is_conversion_on_going_injected;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  assert_param(IS_ADC_SAMPLING_TIME(pConfigInjected->InjectedSamplingTime));
  assert_param(IS_ADC_SINGLE_DIFFERENTIAL(pConfigInjected->InjectedSingleDiff));
  assert_param(IS_FUNCTIONAL_STATE(pConfigInjected->AutoInjectedConv));
  assert_param(IS_ADC_EXTTRIGINJEC_EDGE(pConfigInjected->ExternalTrigInjecConvEdge));
  assert_param(IS_ADC_EXTTRIGINJEC(hadc->Instance, pConfigInjected->ExternalTrigInjecConv));
  assert_param(IS_ADC_OFFSET_NUMBER(pConfigInjected->InjectedOffsetNumber));
  assert_param(IS_ADC_RANGE(ADC_GET_RESOLUTION(hadc), pConfigInjected->InjectedOffset));
  assert_param(IS_ADC_OFFSET_SIGN(pConfigInjected->InjectedOffsetSign));
  assert_param(IS_FUNCTIONAL_STATE(pConfigInjected->InjecOversamplingMode));

  if (hadc->Init.ScanConvMode != ADC_SCAN_DISABLE)
  {
    assert_param(IS_ADC_INJECTED_RANK(pConfigInjected->InjectedRank));
    assert_param(IS_ADC_INJECTED_NB_CONV(pConfigInjected->InjectedNbrOfConversion));
    assert_param(IS_FUNCTIONAL_STATE(pConfigInjected->InjectedDiscontinuousConvMode));
  }

  /* Check offset range according to oversampling setting */
  if (hadc->Init.OversamplingMode == ENABLE)
  {
    assert_param(IS_ADC_RANGE(ADC_GET_RESOLUTION(hadc),
                              pConfigInjected->InjectedOffset / (hadc->Init.Oversampling.Ratio + 1U)));
  }
  else
  {
    assert_param(IS_ADC_RANGE(ADC_GET_RESOLUTION(hadc), pConfigInjected->InjectedOffset));
  }

  /* JDISCEN and JAUTO bits can't be set at the same time  */
  assert_param(!((pConfigInjected->InjectedDiscontinuousConvMode == ENABLE)                                 \
                 && (pConfigInjected->AutoInjectedConv == ENABLE)));

  /*  DISCEN and JAUTO bits can't be set at the same time */
  assert_param(!((hadc->Init.DiscontinuousConvMode == ENABLE) && (pConfigInjected->AutoInjectedConv == ENABLE)));

  /* Verification of channel number */
  if (pConfigInjected->InjectedSingleDiff != ADC_DIFFERENTIAL_ENDED)
  {
    assert_param(IS_ADC_CHANNEL(hadc, pConfigInjected->InjectedChannel));
  }
  else
  {
    assert_param(IS_ADC_DIFF_CHANNEL(hadc, pConfigInjected->InjectedChannel));
  }

  /* ADC must be disabled to set configuration bits                           */
  if (LL_ADC_IsEnabled(hadc->Instance) != 0UL)
  {
    return HAL_ERROR;
  }

  /* Process locked */
  __HAL_LOCK(hadc);

  /* Configuration of injected group sequencer:                               */
  /* - if scan mode is disabled:                                              */
  /*    * Injected channels sequence length is set to 0x00: 1 channel         */
  /*      converted (channel on injected rank 1)                              */
  /*      Parameter "InjectedNbrOfConversion" is discarded.                   */
  /* - if scan mode is enabled:                                               */
  /*    * Injected channels sequence length is set to parameter               */
  /*      "InjectedNbrOfConversion".                                          */
  /*   Note: Scan mode is not present by hardware on this device, but used    */
  /*   by software for alignment over all STM32 devices.                      */

  if ((hadc->Init.ScanConvMode == ADC_SCAN_DISABLE)  ||
      (pConfigInjected->InjectedNbrOfConversion == 1U))
  {
    /* Configuration of context register JSQR:                                */
    /*  - number of ranks in injected group sequencer: fixed to 1st rank      */
    /*    (scan mode disabled, only rank 1 used)                              */
    /*  - external trigger to start conversion                                */
    /*  - external trigger polarity                                           */
    /*  - channel set to rank 1 (scan mode disabled, only rank 1 can be used) */

    if (pConfigInjected->InjectedRank == ADC_INJECTED_RANK_1)
    {
      /* Enable external trigger if trigger selection is different of         */
      /* software start.                                                      */
      /* Note: This configuration keeps the hardware feature of parameter     */
      /*       ExternalTrigInjecConvEdge "trigger edge none" equivalent to    */
      /*       software start.                                                */
      if (pConfigInjected->ExternalTrigInjecConv != ADC_INJECTED_SOFTWARE_START)
      {
        MODIFY_REG(hadc->Instance->JSQR,
                   ADC_JSQR_FIELDS,
                   ADC_JSQR_RK(pConfigInjected->InjectedChannel, ADC_INJECTED_RANK_1)
                   | (pConfigInjected->ExternalTrigInjecConv & ADC_JSQR_JEXTSEL)
                   | pConfigInjected->ExternalTrigInjecConvEdge);
      }
      else
      {
        MODIFY_REG(hadc->Instance->JSQR,
                   ADC_JSQR_FIELDS,
                   ADC_JSQR_RK(pConfigInjected->InjectedChannel, ADC_INJECTED_RANK_1));
      }
    }
  }
  else
  {
    /* Case of scan mode enabled, several channels to set into injected group */
    /* sequencer.                                                             */

    MODIFY_REG(hadc->Instance->JSQR,
               ADC_JSQR_LOW_FIELDS,
               ADC_JSQR_RK(pConfigInjected->InjectedChannel, pConfigInjected->InjectedRank)
               | (pConfigInjected->ExternalTrigInjecConv & ADC_JSQR_JEXTSEL)
               | pConfigInjected->ExternalTrigInjecConvEdge
               | ((pConfigInjected->InjectedNbrOfConversion - 1U) & ADC_JSQR_JL));
  }

  /* Parameters update conditioned to ADC state:                              */
  /* Parameters that can be updated when ADC is disabled or enabled without   */
  /* conversion on going on injected group:                                   */
  /*  - Injected discontinuous mode: can be enabled only if auto-injected     */
  /*    mode is disabled.                                                     */
  if (LL_ADC_INJ_IsConversionOngoing(hadc->Instance) == 0UL)
  {
    /* ADC channels preselection */
    LL_ADC_SetChannelPreselection(hadc->Instance, pConfigInjected->InjectedChannel);

    /* If auto-injected mode is disabled: no constraint                       */
    if (pConfigInjected->AutoInjectedConv == DISABLE)
    {
      MODIFY_REG(hadc->Instance->CFGR1,
                 ADC_CFGR1_JDISCEN,
                 ADC_CFGR1_INJECT_DISCCONTINUOUS((uint32_t)pConfigInjected->InjectedDiscontinuousConvMode));
    }
    /* If auto-injected mode is enabled: Injected discontinuous setting is    */
    /* discarded.                                                             */
    else
    {
      MODIFY_REG(hadc->Instance->CFGR1,
                 ADC_CFGR1_JDISCEN,
                 ADC_CFGR1_INJECT_DISCCONTINUOUS((uint32_t)pConfigInjected->InjectedDiscontinuousConvMode));
    }

  }

  /* Parameters update conditioned to ADC state:                              */
  /* Parameters that can be updated when ADC is disabled or enabled without   */
  /* conversion on going on regular and injected groups:                      */
  /*  - Automatic injected conversion: can be enabled if injected group       */
  /*    external triggers are disabled.                                       */
  /*  - Channel sampling time                                                 */
  /*  - Channel offset                                                        */
  tmp_adc_is_conversion_on_going_regular = LL_ADC_REG_IsConversionOngoing(hadc->Instance);
  tmp_adc_is_conversion_on_going_injected = LL_ADC_INJ_IsConversionOngoing(hadc->Instance);

  if ((tmp_adc_is_conversion_on_going_regular == 0UL)
      && (tmp_adc_is_conversion_on_going_injected == 0UL)
     )
  {
    /* If injected group external triggers are disabled (set to injected      */
    /* software start): no constraint                                         */
    if ((pConfigInjected->ExternalTrigInjecConv == ADC_INJECTED_SOFTWARE_START)
        || (pConfigInjected->ExternalTrigInjecConvEdge == ADC_EXTERNALTRIGINJECCONV_EDGE_NONE))
    {
      if (pConfigInjected->AutoInjectedConv == ENABLE)
      {
        SET_BIT(hadc->Instance->CFGR1, ADC_CFGR1_JAUTO);
      }
      else
      {
        CLEAR_BIT(hadc->Instance->CFGR1, ADC_CFGR1_JAUTO);
      }
    }
    /* If Automatic injected conversion was intended to be set and could not  */
    /* due to injected group external triggers enabled, error is reported.    */
    else
    {
      if (pConfigInjected->AutoInjectedConv == ENABLE)
      {
        /* Update ADC state machine to error */
        SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);

        tmp_hal_status = HAL_ERROR;
      }
      else
      {
        CLEAR_BIT(hadc->Instance->CFGR1, ADC_CFGR1_JAUTO);
      }
    }

    if (pConfigInjected->InjecOversamplingMode == ENABLE)
    {
      assert_param(IS_ADC_OVERSAMPLING_RATIO(pConfigInjected->InjecOversampling.Ratio));
      assert_param(IS_ADC_RIGHT_BIT_SHIFT(pConfigInjected->InjecOversampling.RightBitShift));

      /*  JOVSE must be reset in case of triggered regular mode  */
      assert_param(!(READ_BIT(hadc->Instance->CFGR2, ADC_CFGR2_ROVSE | ADC_CFGR2_TROVS) ==
                     (ADC_CFGR2_ROVSE | ADC_CFGR2_TROVS)));

      /* Configuration of Injected Oversampler:                                 */
      /*  - Oversampling Ratio                                                  */
      /*  - Right bit shift                                                     */

      /* Enable OverSampling mode */
      MODIFY_REG(hadc->Instance->CFGR2,
                 ADC_CFGR2_JOVSE |
                 ADC_CFGR2_OVSR  |
                 ADC_CFGR2_OVSS,
                 ADC_CFGR2_JOVSE                                  |
                 ((pConfigInjected->InjecOversampling.Ratio - 1UL)  << ADC_CFGR2_OVSR_Pos) |
                 pConfigInjected->InjecOversampling.RightBitShift
                );
    }
    else
    {
      /* Disable Regular OverSampling */
      CLEAR_BIT(hadc->Instance->CFGR2, ADC_CFGR2_JOVSE);
    }

    /* Set sampling time of the selected ADC channel */
    LL_ADC_SetChannelSamplingTime(hadc->Instance, pConfigInjected->InjectedChannel,
                                  pConfigInjected->InjectedSamplingTime);

    /* Configure the offset: offset enable/disable, channel, offset value */

    /* Shift the offset with respect to the selected ADC resolution. */
    /* Offset has to be left-aligned on bit 11, the LSB (right bits) are set to 0 */
    tmpOffsetShifted = ADC_OFFSET_SHIFT_RESOLUTION(hadc, pConfigInjected->InjectedOffset);

    if (pConfigInjected->InjectedOffsetNumber != ADC_OFFSET_NONE)
    {
      /* Set ADC selected offset number */
      LL_ADC_SetOffsetChannel(hadc->Instance, pConfigInjected->InjectedOffsetNumber, pConfigInjected->InjectedChannel);
      LL_ADC_SetOffsetLevel(hadc->Instance, pConfigInjected->InjectedOffsetNumber, tmpOffsetShifted);

      assert_param(IS_ADC_OFFSET_SIGN(pConfigInjected->InjectedOffsetSign));
      assert_param(IS_FUNCTIONAL_STATE(pConfigInjected->InjectedOffsetSignedSaturation));
      assert_param(IS_FUNCTIONAL_STATE(pConfigInjected->InjectedOffsetSaturation));
      /* Signed and unsigned saturation cannot be set at the same time */
      assert_param(!((pConfigInjected->InjectedOffsetSignedSaturation == ENABLE)
                     && (pConfigInjected->InjectedOffsetSaturation == ENABLE)));

      /* Set ADC selected offset sign  */
      LL_ADC_SetOffsetSign(hadc->Instance, pConfigInjected->InjectedOffsetNumber, pConfigInjected->InjectedOffsetSign);
      /* Set ADC selected offset signed saturation */
      LL_ADC_SetOffsetSignedSaturation(hadc->Instance, pConfigInjected->InjectedOffsetNumber,
                                       (pConfigInjected->InjectedOffsetSignedSaturation == ENABLE)
                                       ? LL_ADC_OFFSET_SIGNED_SAT_ENABLE                           \
                                       : LL_ADC_OFFSET_SIGNED_SAT_DISABLE);
      /* Set ADC offset unsigned saturation */
      LL_ADC_SetOffsetUnsignedSaturation(hadc->Instance, pConfigInjected->InjectedOffsetNumber,    \
                                         (pConfigInjected->InjectedOffsetSaturation == ENABLE)     \
                                         ? LL_ADC_OFFSET_UNSIGNED_SAT_ENABLE                       \
                                         : LL_ADC_OFFSET_UNSIGNED_SAT_DISABLE);
    }
    else
    {
      /* Scan each offset register to check if the selected channel is targeted. */
      /* If this is the case, the corresponding offset number is disabled.       */
      /* Scan each offset register to check if the selected channel is targeted.
         If this is the case, the corresponding offset number is disabled.       */
      if (__LL_ADC_CHANNEL_TO_DECIMAL_NB(LL_ADC_GetOffsetChannel(hadc->Instance, LL_ADC_OFFSET_1))
          == __HAL_ADC_CHANNEL_TO_DECIMAL_NB(pConfigInjected->InjectedChannel))
      {
        LL_ADC_SetOffsetLevel(hadc->Instance, LL_ADC_OFFSET_1, 0UL);
      }
      if (__LL_ADC_CHANNEL_TO_DECIMAL_NB(LL_ADC_GetOffsetChannel(hadc->Instance, LL_ADC_OFFSET_2))
          == __HAL_ADC_CHANNEL_TO_DECIMAL_NB(pConfigInjected->InjectedChannel))
      {
        LL_ADC_SetOffsetLevel(hadc->Instance, LL_ADC_OFFSET_2, 0UL);
      }
      if (__LL_ADC_CHANNEL_TO_DECIMAL_NB(LL_ADC_GetOffsetChannel(hadc->Instance, LL_ADC_OFFSET_3))
          == __HAL_ADC_CHANNEL_TO_DECIMAL_NB(pConfigInjected->InjectedChannel))
      {
        LL_ADC_SetOffsetLevel(hadc->Instance, LL_ADC_OFFSET_3, 0UL);
      }
      if (__LL_ADC_CHANNEL_TO_DECIMAL_NB(LL_ADC_GetOffsetChannel(hadc->Instance, LL_ADC_OFFSET_4))
          == __HAL_ADC_CHANNEL_TO_DECIMAL_NB(pConfigInjected->InjectedChannel))
      {
        LL_ADC_SetOffsetLevel(hadc->Instance, LL_ADC_OFFSET_4, 0UL);
      }
    }

  }
  /* Parameters update conditioned to ADC state:                              */
  /* Parameters that can be updated only when ADC is disabled:                */
  /*  - Single or differential mode                                           */
  /*  - Internal measurement channels: Vbat/VrefInt/TempSensor                */
  if (LL_ADC_IsEnabled(hadc->Instance) == 0UL)
  {
    /* Set mode single-ended or differential input of the selected ADC channel */
    LL_ADC_SetChannelSingleDiff(hadc->Instance, pConfigInjected->InjectedChannel, pConfigInjected->InjectedSingleDiff);

    /* Configuration of differential mode */
    if (pConfigInjected->InjectedSingleDiff == ADC_DIFFERENTIAL_ENDED)
    {
      /* Set ADC channel preselection of corresponding negative channel */
      LL_ADC_SetChannelPreselection(hadc->Instance,
                                    __HAL_ADC_CHANNEL_DIFF_NEG_INPUT(hadc, pConfigInjected->InjectedChannel));
    }

  }

  /* Management of internal measurement channels: Vbat/VrefInt/TempSensor   */
  /* internal measurement paths enable: If internal channel selected,       */
  /* enable dedicated internal buffers and path.                            */
  /* Note: these internal measurement paths can be disabled using           */
  /* HAL_ADC_DeInit().                                                      */

  if (__LL_ADC_IS_CHANNEL_INTERNAL(pConfigInjected->InjectedChannel))
  {
    tmp_config_common_path_internal_channel = LL_ADC_GetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(hadc->Instance));
    tmp_config_path_internal_channel = LL_ADC_GetPathInternalCh(hadc->Instance);

    /* If the requested internal measurement path has already been enabled,   */
    /* bypass the configuration processing.                                   */
    if ((pConfigInjected->InjectedChannel == ADC_CHANNEL_VREFINT)
        && ((tmp_config_common_path_internal_channel & LL_ADC_PATH_INTERNAL_VREFINT) == 0UL))
    {
      if (ADC_VREFINT_INSTANCE(hadc))
      {
        LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(hadc->Instance),
                                       LL_ADC_PATH_INTERNAL_VREFINT | tmp_config_common_path_internal_channel);
      }
    }
    else if ((pConfigInjected->InjectedChannel == ADC_CHANNEL_VBAT)
             && ((tmp_config_common_path_internal_channel & LL_ADC_PATH_INTERNAL_VBAT) == 0UL))
    {
      if (ADC_BATTERY_VOLTAGE_INSTANCE(hadc))
      {
        LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(hadc->Instance),
                                       LL_ADC_PATH_INTERNAL_VBAT | tmp_config_common_path_internal_channel);
      }
    }
    else if (((pConfigInjected->InjectedChannel == ADC_CHANNEL_VDDCORE)
              && ((tmp_config_path_internal_channel & LL_ADC_PATH_INTERNAL_VDDCORE) == 0UL)))
    {
      if (ADC_VDDCORE_INSTANCE(hadc))
      {
        LL_ADC_SetPathInternalCh(hadc->Instance, LL_ADC_PATH_INTERNAL_VDDCORE | tmp_config_path_internal_channel);
      }
    }
    else
    {
      /* nothing to do */
    }
  }

  __HAL_UNLOCK(hadc);

  return tmp_hal_status;
}

#if defined(ADC_MULTIMODE_SUPPORT)
/**
  * @brief  Enable ADC multimode and configure multimode parameters
  * @note   Possibility to update parameters on the fly:
  *         This function initializes multimode parameters, following
  *         calls to this function can be used to reconfigure some parameters
  *         of structure "ADC_MultiModeTypeDef" on the fly, without resetting
  *         the ADCs.
  *         The setting of these parameters is conditioned to ADC state.
  *         For parameters constraints, see comments of structure
  *         "ADC_MultiModeTypeDef".
  * @note   To move back configuration from multimode to single mode, ADC must
  *         be reset (using function HAL_ADC_Init() ).
  * @param hadc Master ADC handle
  * @param pMultimode Structure of ADC multimode configuration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_MultiModeConfigChannel(ADC_HandleTypeDef *hadc,
                                                   const ADC_MultiModeTypeDef *pMultimode)
{
  HAL_StatusTypeDef tmp_hal_status = HAL_OK;
  ADC_Common_TypeDef *tmpADC_Common;
  ADC_HandleTypeDef tmphadcSlave;
  uint32_t tmphadcSlave_conversion_on_going;

  /* Check the parameters */
  assert_param(IS_ADC_MULTIMODE_MASTER_INSTANCE(hadc->Instance));
  assert_param(IS_ADC_MULTIMODE(pMultimode->Mode));
  if (pMultimode->Mode != ADC_MODE_INDEPENDENT)
  {
    assert_param(IS_ADC_DUAL_DATA_MODE(pMultimode->DualModeData));
    assert_param(IS_ADC_SAMPLING_DELAY(pMultimode->TwoSamplingDelay));
  }

  /* Process locked */
  __HAL_LOCK(hadc);

  /* Temporary handle minimum initialization */
  __HAL_ADC_RESET_HANDLE_STATE(&tmphadcSlave);
  ADC_CLEAR_ERRORCODE(&tmphadcSlave);

  ADC_MULTI_SLAVE(hadc, &tmphadcSlave);

  if (tmphadcSlave.Instance == NULL)
  {
    /* Update ADC state machine to error */
    SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);

    __HAL_UNLOCK(hadc);

    return HAL_ERROR;
  }

  /* ADC must be disabled to set configuration bits */
  if (LL_ADC_IsEnabled(hadc->Instance) != 0UL)
  {
    if (LL_ADC_IsEnabled(tmphadcSlave.Instance) != 0UL)
    {
      return HAL_ERROR;
    }
  }

  /* Parameters update conditioned to ADC state:                              */
  /* Parameters that can be updated when ADC is disabled or enabled without   */
  /* conversion on going on regular group:                                    */
  /*  - Multimode DMA configuration                                           */
  /*  - Multimode DMA mode                                                    */
  tmphadcSlave_conversion_on_going = LL_ADC_REG_IsConversionOngoing((&tmphadcSlave)->Instance);
  if ((LL_ADC_REG_IsConversionOngoing(hadc->Instance) == 0UL)
      && (tmphadcSlave_conversion_on_going == 0UL))
  {
    /* Pointer to the common control register */
    tmpADC_Common = __LL_ADC_COMMON_INSTANCE(hadc->Instance);

    /* If multimode is selected, configure all multimode parameters.          */
    /* Otherwise, reset multimode parameters (can be used in case of          */
    /* transition from multimode to independent mode).                        */
    if (pMultimode->Mode != ADC_MODE_INDEPENDENT)
    {
      MODIFY_REG(tmpADC_Common->CCR, ADC_CCR_DAMDF, pMultimode->DualModeData);

      /* Parameters that can be updated only when ADC is disabled:                */
      /*  - Multimode mode selection                                              */
      /*  - Multimode delay                                                       */
      /*    Note: Delay range depends on selected resolution:                     */
      /*      from 1 to 12 clock cycles for 12 bits                               */
      /*      from 1 to 10 clock cycles for 10 bits,                              */
      /*      from 1 to 8 clock cycles for 8 bits                                 */
      /*      from 1 to 6 clock cycles for 6 bits                                 */
      /*    If a higher delay is selected, it will be clipped to maximum delay    */
      /*    range                                                                 */
      if (__LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE(__LL_ADC_COMMON_INSTANCE(hadc->Instance)) == 0UL)
      {
        MODIFY_REG(tmpADC_Common->CCR,
                   ADC_CCR_DUAL |
                   ADC_CCR_DELAY,
                   pMultimode->Mode |
                   pMultimode->TwoSamplingDelay
                  );
      }
    }
    else /* ADC_MODE_INDEPENDENT */
    {
      CLEAR_BIT(tmpADC_Common->CCR, ADC_CCR_DAMDF);

      /* Parameters that can be updated only when ADC is disabled:                */
      /*  - Multimode mode selection                                              */
      /*  - Multimode delay                                                       */
      if (__LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE(__LL_ADC_COMMON_INSTANCE(hadc->Instance)) == 0UL)
      {
        CLEAR_BIT(tmpADC_Common->CCR, ADC_CCR_DUAL | ADC_CCR_DELAY);
      }
    }
  }
  /* If one of the ADC sharing the same common group is enabled, no update    */
  /* could be done on neither of the multimode structure parameters.          */
  else
  {
    /* Update ADC state machine to error */
    SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);

    tmp_hal_status = HAL_ERROR;
  }

  __HAL_UNLOCK(hadc);

  return tmp_hal_status;
}
#endif /* ADC_MULTIMODE_SUPPORT */

/**
  * @brief  Enter ADC deep-power-down mode
  * @note   This mode is achieved in setting DEEPPWD bit and allows to save power
  *         in reducing leakage currents. It is particularly interesting before
  *         entering stop modes.
  * @note   Setting DEEPPWD automatically clears ADVREGEN bit and disables the
  *         ADC voltage regulator. This means that this API encompasses
  *         HAL_ADCEx_DisableVoltageRegulator(). Additionally, the internal
  *         calibration is lost.
  * @note   To exit the ADC deep-power-down mode, the user is expected to
  *         resort to HAL_ADC_Init() API as well as to relaunch a calibration
  *         with HAL_ADCEx_Calibration_Start() API or to re-apply a previously
  *         saved calibration factor.
  * @param hadc ADC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_EnterADCDeepPowerDownMode(ADC_HandleTypeDef *hadc)
{
  HAL_StatusTypeDef tmp_hal_status;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));

  /* Setting of this feature is conditioned to ADC state: ADC must be ADC disabled */
  if (LL_ADC_IsEnabled(hadc->Instance) == 0UL)
  {
    LL_ADC_EnableDeepPowerDown(hadc->Instance);
    tmp_hal_status = HAL_OK;
  }
  else
  {
    tmp_hal_status = HAL_ERROR;
  }

  return tmp_hal_status;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @brief Measure ADC offset during calibration
  * @note  To measure ADC calibration offset ADC must be enabled and calibration
  *         mode should be enabled. This function is intended to be used inside
  *         function @ref HAL_ADCEx_Calibration_Start
  * @param hadc ADC handle
  * @param SingleDiff
  * @param pCalibrationFactor ADC measurement offset
  * @retval HAL status
  */
HAL_StatusTypeDef ADC_Calibration_MeasureOffset(ADC_HandleTypeDef *hadc,
                                                uint32_t SingleDiff,
                                                uint32_t *pCalibrationFactor)
{
  int32_t  calib_factor_avg = 0;
  uint32_t calibration_step;
  uint32_t tickstart;

  HAL_StatusTypeDef tmp_hal_status = HAL_OK;

  if (LL_ADC_IsEnabled(hadc->Instance) != 0UL)
  {
    /* Start ADC calibration */
    LL_ADC_StartCalibration(hadc->Instance, SingleDiff);

    do
    {
      /* Measure current ADC offset */

      /* With calibration mode enabled, start multiple conversion and           */
      /* accumulate data to compute ADC conversion offset.                      */
      /* The calibration factor will be the averaged converted value            */
      for (calibration_step = 0; calibration_step < ADC_CALIBRATION_STEPS; calibration_step++)
      {
        LL_ADC_REG_StartConversion(hadc->Instance);

        /* Wait for ADC conversion to end */
        /* Get tick count */
        tickstart = HAL_GetTick();
        while (LL_ADC_REG_IsConversionOngoing(hadc->Instance) != 0UL)
        {
          if ((HAL_GetTick() - tickstart) > ADC_CALIBRATION_TIMEOUT)
          {
            /* New check to avoid false timeout detection in case of preemption */
            if (LL_ADC_REG_IsConversionOngoing(hadc->Instance) != 0UL)
            {
              /* Update ADC state machine to error */
              SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL);

              /* Set ADC error code to ADC peripheral internal error */
              SET_BIT(hadc->ErrorCode, HAL_ADC_ERROR_INTERNAL);

              return HAL_ERROR;
            }
          }
        }

        calib_factor_avg += (int32_t)(LL_ADC_REG_ReadConversionData32(hadc->Instance));
      } /* end of calibration steps */

      /* Compute the average data */
      calib_factor_avg = calib_factor_avg / (int32_t)(calibration_step);

      if (SingleDiff == ADC_DIFFERENTIAL_ENDED)
      {
        /* In differential mode, subtract averaged data by 0x7FF (middle      */
        /* value for differential ended corresponding to a null offset)      */
        calib_factor_avg = calib_factor_avg - 0x7FF;
      }
      else
      {
        /* nothing to do */
      }
      if (calib_factor_avg <= 0)
      {
        if (LL_ADC_IsCalibrationOffsetEnabled(hadc->Instance) == 0UL)
        {
          LL_ADC_EnableCalibrationOffset(hadc->Instance);
        }
        else
        {
          /* If calibration additional offset is enabled, measured calibration */
          /* factor should be different than 0.                                */
          tmp_hal_status = HAL_ERROR;

          SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL);
        }
      }
      else
      {
        *pCalibrationFactor = (uint32_t)(calib_factor_avg);
      }
    } while ((calib_factor_avg <= 0) && (tmp_hal_status == HAL_OK));
  }
  else
  {
    /* Update ADC state machine to error */
    SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL);

    tmp_hal_status = HAL_ERROR;
  }

  return tmp_hal_status;
}


#endif /* HAL_ADC_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */
