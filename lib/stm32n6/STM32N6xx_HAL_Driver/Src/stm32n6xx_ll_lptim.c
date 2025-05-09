/**
  ******************************************************************************
  * @file    stm32n6xx_ll_lptim.c
  * @author  MCD Application Team
  * @brief   LPTIM LL module driver.
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
#include "stm32n6xx_ll_lptim.h"
#include "stm32n6xx_ll_bus.h"
#include "stm32n6xx_ll_rcc.h"


#ifdef  USE_FULL_ASSERT
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

#if defined (LPTIM1) || defined (LPTIM2) || defined (LPTIM3) || defined (LPTIM4) || defined (LPTIM5)

/** @addtogroup LPTIM_LL
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @addtogroup LPTIM_LL_Private_Macros
  * @{
  */
#define IS_LL_LPTIM_CLOCK_SOURCE(__VALUE__) (((__VALUE__) == LL_LPTIM_CLK_SOURCE_INTERNAL) \
                                             || ((__VALUE__) == LL_LPTIM_CLK_SOURCE_EXTERNAL))

#define IS_LL_LPTIM_CLOCK_PRESCALER(__VALUE__) (((__VALUE__) == LL_LPTIM_PRESCALER_DIV1)   \
                                                || ((__VALUE__) == LL_LPTIM_PRESCALER_DIV2)   \
                                                || ((__VALUE__) == LL_LPTIM_PRESCALER_DIV4)   \
                                                || ((__VALUE__) == LL_LPTIM_PRESCALER_DIV8)   \
                                                || ((__VALUE__) == LL_LPTIM_PRESCALER_DIV16)  \
                                                || ((__VALUE__) == LL_LPTIM_PRESCALER_DIV32)  \
                                                || ((__VALUE__) == LL_LPTIM_PRESCALER_DIV64)  \
                                                || ((__VALUE__) == LL_LPTIM_PRESCALER_DIV128))

#define IS_LL_LPTIM_WAVEFORM(__VALUE__) (((__VALUE__) == LL_LPTIM_OUTPUT_WAVEFORM_PWM) \
                                         || ((__VALUE__) == LL_LPTIM_OUTPUT_WAVEFORM_SETONCE))

/**
  * @}
  */


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/** @defgroup LPTIM_Private_Functions LPTIM Private Functions
  * @{
  */
/**
  * @}
  */
/* Exported functions --------------------------------------------------------*/
/** @addtogroup LPTIM_LL_Exported_Functions
  * @{
  */

/** @addtogroup LPTIM_LL_EF_Init
  * @{
  */

/**
  * @brief  Set LPTIMx registers to their reset values.
  * @param  LPTIMx LP Timer instance
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: LPTIMx registers are de-initialized
  *          - ERROR: invalid LPTIMx instance
  */
ErrorStatus LL_LPTIM_DeInit(const LPTIM_TypeDef *LPTIMx)
{
  ErrorStatus result = SUCCESS;

  /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(LPTIMx));

  if (LPTIMx == LPTIM1)
  {
    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_LPTIM1);
    LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_LPTIM1);
  }
  else if (LPTIMx == LPTIM2)
  {
    LL_APB4_GRP1_ForceReset(LL_APB4_GRP1_PERIPH_LPTIM2);
    LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_LPTIM2);
  }
  else if (LPTIMx == LPTIM3)
  {
    LL_APB4_GRP1_ForceReset(LL_APB4_GRP1_PERIPH_LPTIM3);
    LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_LPTIM3);
  }
  else if (LPTIMx == LPTIM4)
  {
    LL_APB4_GRP1_ForceReset(LL_APB4_GRP1_PERIPH_LPTIM4);
    LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_LPTIM4);
  }
  else if (LPTIMx == LPTIM5)
  {
    LL_APB4_GRP1_ForceReset(LL_APB4_GRP1_PERIPH_LPTIM5);
    LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_LPTIM5);
  }
  else
  {
    result = ERROR;
  }

  return result;
}

/**
  * @brief  Set each fields of the LPTIM_InitStruct structure to its default
  *         value.
  * @param  LPTIM_InitStruct pointer to a @ref LL_LPTIM_InitTypeDef structure
  * @retval None
  */
void LL_LPTIM_StructInit(LL_LPTIM_InitTypeDef *LPTIM_InitStruct)
{
  /* Set the default configuration */
  LPTIM_InitStruct->ClockSource = LL_LPTIM_CLK_SOURCE_INTERNAL;
  LPTIM_InitStruct->Prescaler   = LL_LPTIM_PRESCALER_DIV1;
  LPTIM_InitStruct->Waveform    = LL_LPTIM_OUTPUT_WAVEFORM_PWM;
}

/**
  * @brief  Configure the LPTIMx peripheral according to the specified parameters.
  * @note LL_LPTIM_Init can only be called when the LPTIM instance is disabled.
  * @note LPTIMx can be disabled using unitary function @ref LL_LPTIM_Disable().
  * @param  LPTIMx LP Timer Instance
  * @param  LPTIM_InitStruct pointer to a @ref LL_LPTIM_InitTypeDef structure
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: LPTIMx instance has been initialized
  *          - ERROR: LPTIMx instance hasn't been initialized
  */
ErrorStatus LL_LPTIM_Init(LPTIM_TypeDef *LPTIMx, const LL_LPTIM_InitTypeDef *LPTIM_InitStruct)
{
  ErrorStatus result = SUCCESS;
  /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(LPTIMx));
  assert_param(IS_LL_LPTIM_CLOCK_SOURCE(LPTIM_InitStruct->ClockSource));
  assert_param(IS_LL_LPTIM_CLOCK_PRESCALER(LPTIM_InitStruct->Prescaler));
  assert_param(IS_LL_LPTIM_WAVEFORM(LPTIM_InitStruct->Waveform));

  /* The LPTIMx_CFGR register must only be modified when the LPTIM is disabled
     (ENABLE bit is reset to 0).
  */
  if (LL_LPTIM_IsEnabled(LPTIMx) == 1UL)
  {
    result = ERROR;
  }
  else
  {
    /* Set CKSEL bitfield according to ClockSource value */
    /* Set PRESC bitfield according to Prescaler value */
    /* Set WAVE bitfield according to Waveform value */
    MODIFY_REG(LPTIMx->CFGR,
               (LPTIM_CFGR_CKSEL | LPTIM_CFGR_PRESC | LPTIM_CFGR_WAVE),
               LPTIM_InitStruct->ClockSource | \
               LPTIM_InitStruct->Prescaler | \
               LPTIM_InitStruct->Waveform);
  }

  return result;
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

#endif /* LPTIM1 || LPTIM2 || LPTIM3 || LPTIM4 || LPTIM5 */

/**
  * @}
  */

#endif /* USE_FULL_LL_DRIVER */
