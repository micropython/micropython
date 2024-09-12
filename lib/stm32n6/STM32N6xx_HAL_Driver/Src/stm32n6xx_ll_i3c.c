/**
  ******************************************************************************
  * @file    stm32n6xx_ll_i3c.c
  * @author  MCD Application Team
  * @brief   I3C LL module driver.
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
  */
#if defined(USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_ll_bus.h"
#include "stm32n6xx_ll_i3c.h"
#ifdef  USE_FULL_ASSERT
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

#if defined (I3C1) || defined (I3C2)

/** @defgroup I3C_LL I3C
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @addtogroup I3C_LL_Private_Macros
  * @{
  */
#define IS_LL_I3C_SDAHOLDTIME_VALUE(VALUE) (((VALUE) == LL_I3C_SDA_HOLD_TIME_0_5) || \
                                            ((VALUE) == LL_I3C_SDA_HOLD_TIME_1_5))

#define IS_LL_I3C_WAITTIME_VALUE(VALUE) (((VALUE) == LL_I3C_OWN_ACTIVITY_STATE_0) || \
                                         ((VALUE) == LL_I3C_OWN_ACTIVITY_STATE_1) || \
                                         ((VALUE) == LL_I3C_OWN_ACTIVITY_STATE_2) || \
                                         ((VALUE) == LL_I3C_OWN_ACTIVITY_STATE_3))
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup I3C_LL_Exported_Functions
  * @{
  */

/** @addtogroup I3C_LL_EF_Init
  * @{
  */

/**
  * @brief  De-initialize the I3C registers to their default reset values.
  * @param  I3Cx I3C Instance.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: I3C registers are de-initialized
  *          - ERROR: I3C registers are not de-initialized
  */
ErrorStatus LL_I3C_DeInit(const I3C_TypeDef *I3Cx)
{
  ErrorStatus status = SUCCESS;

  /* Check the I3C Instance I3Cx */
  assert_param(IS_I3C_ALL_INSTANCE(I3Cx));

  if (I3Cx == I3C1)
  {
    /* Force reset of I3C clock */
    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_I3C1);

    /* Release reset of I3C clock */
    LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I3C1);
  }
  else if (I3Cx == I3C2)
  {
    /* Force reset of I3C clock */
    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_I3C2);

    /* Release reset of I3C clock */
    LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I3C2);

  }
  else
  {
    status = ERROR;
  }

  return status;
}

/**
  * @brief  Initialize the I3C registers according to the specified parameters in I3C_InitStruct.
  * @param  I3Cx I3C Instance.
  * @param  I3C_InitStruct pointer to a @ref LL_I3C_InitTypeDef structure.
  * @param  Mode I3C peripheral mode.
  *              This parameter can be a value of @ref I3C_LL_EC_MODE.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: I3C registers are initialized
  *          - ERROR: Not applicable
  */
ErrorStatus LL_I3C_Init(I3C_TypeDef *I3Cx, LL_I3C_InitTypeDef *I3C_InitStruct, uint32_t Mode)
{
  uint32_t waveform_value;
  uint32_t timing_value;

  /* Check the I3C Instance I3Cx */
  assert_param(IS_I3C_ALL_INSTANCE(I3Cx));

  /* Disable the selected I3C peripheral */
  LL_I3C_Disable(I3Cx);

  /* Check on the I3C mode: initialization depends on the mode */
  if (Mode == LL_I3C_MODE_CONTROLLER)
  {
    /* Check the parameters */
    assert_param(IS_LL_I3C_SDAHOLDTIME_VALUE(I3C_InitStruct->CtrlBusCharacteristic.SDAHoldTime));
    assert_param(IS_LL_I3C_WAITTIME_VALUE(I3C_InitStruct->CtrlBusCharacteristic.WaitTime));

    /* Set Controller mode */
    LL_I3C_SetMode(I3Cx, LL_I3C_MODE_CONTROLLER);

    /*------------------ SCL signal waveform configuration : I3C timing register 0 (I3C_TIMINGR0) ------------------- */
    /* Set the controller SCL waveform */
    waveform_value =
      ((uint32_t)(I3C_InitStruct->CtrlBusCharacteristic.SCLPPLowDuration)                                |
       ((uint32_t)I3C_InitStruct->CtrlBusCharacteristic.SCLI3CHighDuration << I3C_TIMINGR0_SCLH_I3C_Pos) |
       ((uint32_t)I3C_InitStruct->CtrlBusCharacteristic.SCLODLowDuration << I3C_TIMINGR0_SCLL_OD_Pos)    |
       ((uint32_t)I3C_InitStruct->CtrlBusCharacteristic.SCLI2CHighDuration << I3C_TIMINGR0_SCLH_I2C_Pos));

    LL_I3C_ConfigClockWaveForm(I3Cx, waveform_value);

    /*------------------- Timing configuration : I3C timing register 1 (I3C_TIMINGR1) ------------------------------- */
    /* Set SDA hold time, activity state, bus free duration and bus available duration */
    timing_value = ((uint32_t)(I3C_InitStruct->CtrlBusCharacteristic.SDAHoldTime)                               |
                    (uint32_t)(I3C_InitStruct->CtrlBusCharacteristic.WaitTime)                                  |
                    ((uint32_t)I3C_InitStruct->CtrlBusCharacteristic.BusFreeDuration <<  I3C_TIMINGR1_FREE_Pos) |
                    (uint32_t)(I3C_InitStruct->CtrlBusCharacteristic.BusIdleDuration));

    LL_I3C_SetCtrlBusCharacteristic(I3Cx, timing_value);
  }
  else
  {
    /* Set target mode */
    LL_I3C_SetMode(I3Cx, LL_I3C_MODE_TARGET);

    /*------------------- Timing configuration : I3C timing register 1 (I3C_TIMINGR1) ------------------------------- */
    /* Set the number of kernel clocks cycles for the bus available condition time */
    LL_I3C_SetTgtBusCharacteristic(I3Cx, I3C_InitStruct->TgtBusCharacteristic.BusAvailableDuration);
  }

  /* Enable the selected I3C peripheral */
  LL_I3C_Enable(I3Cx);

  return SUCCESS;
}

/**
  * @brief  Set each @ref LL_I3C_InitTypeDef field to default value.
  * @param  I3C_InitStruct Pointer to a @ref LL_I3C_InitTypeDef structure.
  * @retval None
  */
void LL_I3C_StructInit(LL_I3C_InitTypeDef *I3C_InitStruct)
{
  /* Set I3C_InitStruct fields to default values */
  I3C_InitStruct->CtrlBusCharacteristic.SDAHoldTime         = LL_I3C_SDA_HOLD_TIME_0_5;
  I3C_InitStruct->CtrlBusCharacteristic.WaitTime            = LL_I3C_OWN_ACTIVITY_STATE_0;
  I3C_InitStruct->CtrlBusCharacteristic.SCLPPLowDuration    = 0U;
  I3C_InitStruct->CtrlBusCharacteristic.SCLI3CHighDuration  = 0U;
  I3C_InitStruct->CtrlBusCharacteristic.SCLODLowDuration    = 0U;
  I3C_InitStruct->CtrlBusCharacteristic.SCLI2CHighDuration  = 0U;
  I3C_InitStruct->CtrlBusCharacteristic.BusFreeDuration     = 0U;
  I3C_InitStruct->CtrlBusCharacteristic.BusIdleDuration     = 0U;
  I3C_InitStruct->TgtBusCharacteristic.BusAvailableDuration = 0U;
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

#endif /* I3C1 || I3C2 */

/**
  * @}
  */

#endif /* USE_FULL_LL_DRIVER */
