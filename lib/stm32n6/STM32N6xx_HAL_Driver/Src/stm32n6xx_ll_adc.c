/**
  ******************************************************************************
  * @file    stm32n6xx_ll_adc.c
  * @author  MCD Application Team
  * @brief   ADC LL module driver
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
#include "stm32n6xx_ll_adc.h"
#include "stm32n6xx_ll_bus.h"

#ifdef  USE_FULL_ASSERT
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

#if defined (ADC1) || defined (ADC2)

/** @addtogroup ADC_LL ADC
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @addtogroup ADC_LL_Private_Constants
  * @{
  */

/* Definitions of ADC hardware constraints delays */
/* Note: Only ADC peripheral HW delays are defined in ADC LL driver driver,   */
/*       not timeout values:                                                  */
/*       Timeout values for ADC operations are dependent to device clock      */
/*       configuration (system clock versus ADC clock),                       */
/*       and therefore must be defined in user application.                   */
/*       Refer to @ref ADC_LL_EC_HW_DELAYS for description of ADC timeout     */
/*       values definition.                                                   */
/* Note: ADC timeout values are defined here in CPU cycles to be independent  */
/*       of device clock setting.                                             */
/*       In user application, ADC timeout values should be defined with       */
/*       temporal values, in function of device clock settings.               */
/*       Highest ratio CPU clock frequency vs ADC clock frequency:            */
/*        - ADC clock from synchronous clock with AHB prescaler 512,          */
/*          APB prescaler 16, ADC prescaler 4.                                */
/*        - ADC clock from asynchronous clock (PLL) with prescaler 1,         */
/*          with highest ratio CPU clock frequency vs HSI clock frequency     */
/* Unit: CPU cycles.                                                          */
#define ADC_CLOCK_RATIO_VS_CPU_HIGHEST          (512UL * 16UL * 4UL)
#define ADC_TIMEOUT_DISABLE_CPU_CYCLES          (ADC_CLOCK_RATIO_VS_CPU_HIGHEST * 1UL)
#define ADC_TIMEOUT_STOP_CONVERSION_CPU_CYCLES  (ADC_CLOCK_RATIO_VS_CPU_HIGHEST * 1UL)

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/

/** @addtogroup ADC_LL_Private_Macros
  * @{
  */

/* Check of parameters for configuration of ADC hierarchical scope:           */
/* common to several ADC instances.                                           */
/* (None) */

/* Check of parameters for configuration of ADC hierarchical scope:           */
/* ADC instance.                                                              */
#define IS_LL_ADC_RESOLUTION(__RESOLUTION__)                                   \
  (((__RESOLUTION__) == LL_ADC_RESOLUTION_12B)                                 \
   || ((__RESOLUTION__) == LL_ADC_RESOLUTION_10B)                              \
   || ((__RESOLUTION__) == LL_ADC_RESOLUTION_8B)                               \
   || ((__RESOLUTION__) == LL_ADC_RESOLUTION_6B)                               \
  )

#define IS_LL_ADC_LEFT_BIT_SHIFT(__LEFT_BIT_SHIFT__)                           \
  (   ((__LEFT_BIT_SHIFT__) == LL_ADC_LEFT_BIT_SHIFT_NONE)                     \
      || ((__LEFT_BIT_SHIFT__) == LL_ADC_LEFT_BIT_SHIFT_1)                     \
      || ((__LEFT_BIT_SHIFT__) == LL_ADC_LEFT_BIT_SHIFT_2)                     \
      || ((__LEFT_BIT_SHIFT__) == LL_ADC_LEFT_BIT_SHIFT_3)                     \
      || ((__LEFT_BIT_SHIFT__) == LL_ADC_LEFT_BIT_SHIFT_4)                     \
      || ((__LEFT_BIT_SHIFT__) == LL_ADC_LEFT_BIT_SHIFT_5)                     \
      || ((__LEFT_BIT_SHIFT__) == LL_ADC_LEFT_BIT_SHIFT_6)                     \
      || ((__LEFT_BIT_SHIFT__) == LL_ADC_LEFT_BIT_SHIFT_7)                     \
      || ((__LEFT_BIT_SHIFT__) == LL_ADC_LEFT_BIT_SHIFT_8)                     \
      || ((__LEFT_BIT_SHIFT__) == LL_ADC_LEFT_BIT_SHIFT_9)                     \
      || ((__LEFT_BIT_SHIFT__) == LL_ADC_LEFT_BIT_SHIFT_10)                    \
      || ((__LEFT_BIT_SHIFT__) == LL_ADC_LEFT_BIT_SHIFT_11)                    \
      || ((__LEFT_BIT_SHIFT__) == LL_ADC_LEFT_BIT_SHIFT_12)                    \
      || ((__LEFT_BIT_SHIFT__) == LL_ADC_LEFT_BIT_SHIFT_13)                    \
      || ((__LEFT_BIT_SHIFT__) == LL_ADC_LEFT_BIT_SHIFT_14)                    \
      || ((__LEFT_BIT_SHIFT__) == LL_ADC_LEFT_BIT_SHIFT_15)                    \
  )

#define IS_LL_ADC_LOW_POWER(__LOW_POWER__)                                     \
  (   ((__LOW_POWER__) == LL_ADC_LP_MODE_NONE)                                 \
      || ((__LOW_POWER__) == LL_ADC_LP_AUTOWAIT)                               \
  )

/* Check of parameters for configuration of ADC hierarchical scope:           */
/* ADC group regular                                                          */
#define IS_LL_ADC_REG_TRIG_SOURCE(__ADC_INSTANCE__, __REG_TRIG_SOURCE__)       \
  (((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_SOFTWARE)                         \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_EXTI_LINE11)               \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_CH1)                  \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_CH2)                  \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_CH3)                  \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_TRGO)                 \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_TRGO2)                \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM2_CH2)                  \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM2_TRGO)                 \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM3_CH4)                  \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM3_TRGO)                 \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM4_CH4)                  \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM4_TRGO)                 \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM5_TRGO)                 \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM6_TRGO)                 \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM7_TRGO)                 \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM8_TRGO)                 \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM8_TRGO2)                \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM9_CH1)                  \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM9_TRGO)                 \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM12_TRGO)                \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM15_TRGO)                \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM18_TRGO)                \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_LPTIM1_CH1)                \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_LPTIM2_CH1)                \
   || ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_LPTIM3_CH1)                \
  )

#define IS_LL_ADC_REG_CONTINUOUS_MODE(__REG_CONTINUOUS_MODE__)                 \
  (((__REG_CONTINUOUS_MODE__) == LL_ADC_REG_CONV_SINGLE)                       \
   || ((__REG_CONTINUOUS_MODE__) == LL_ADC_REG_CONV_CONTINUOUS)                \
  )

#define IS_LL_ADC_REG_DATA_TRANSFER_MODE(__REG_DATA_TRANSFER_MODE__)           \
  (((__REG_DATA_TRANSFER_MODE__) == LL_ADC_REG_DR_TRANSFER)                    \
   || ((__REG_DATA_TRANSFER_MODE__) == LL_ADC_REG_DMA_TRANSFER_LIMITED)        \
   || ((__REG_DATA_TRANSFER_MODE__) == LL_ADC_REG_DMA_TRANSFER_UNLIMITED)      \
   || ((__REG_DATA_TRANSFER_MODE__) == LL_ADC_REG_MDF_TRANSFER)                \
  )

#define IS_LL_ADC_REG_OVR_DATA_BEHAVIOR(__REG_OVR_DATA_BEHAVIOR__)             \
  (((__REG_OVR_DATA_BEHAVIOR__) == LL_ADC_REG_OVR_DATA_PRESERVED)              \
   || ((__REG_OVR_DATA_BEHAVIOR__) == LL_ADC_REG_OVR_DATA_OVERWRITTEN)         \
  )

#define IS_LL_ADC_REG_SEQ_SCAN_LENGTH(__REG_SEQ_SCAN_LENGTH__)                 \
  (((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_DISABLE)                  \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_9RANKS)         \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_10RANKS)        \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_11RANKS)        \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_12RANKS)        \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_13RANKS)        \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_14RANKS)        \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_15RANKS)        \
   || ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_16RANKS)        \
  )

#define IS_LL_ADC_REG_SEQ_SCAN_DISCONT_MODE(__REG_SEQ_DISCONT_MODE__)          \
  (   ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_DISABLE)           \
      || ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_1RANK)          \
      || ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_2RANKS)         \
      || ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_3RANKS)         \
      || ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_4RANKS)         \
      || ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_5RANKS)         \
      || ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_6RANKS)         \
      || ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_7RANKS)         \
      || ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_8RANKS)         \
  )

/* Check of parameters for configuration of ADC hierarchical scope:           */
/* ADC group injected                                                         */
#define IS_LL_ADC_INJ_TRIG_SOURCE(__ADC_INSTANCE__, __INJ_TRIG_SOURCE__)       \
  (((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_SOFTWARE)                         \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_EXTI_LINE15)               \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM1_CH4)                  \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM1_TRGO)                 \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM1_TRGO2)                \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM2_CH1)                  \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM2_TRGO)                 \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM3_CH1)                  \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM3_CH3)                  \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM3_CH4)                  \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM3_TRGO)                 \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM4_TRGO)                 \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM5_TRGO)                 \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM6_TRGO)                 \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM7_TRGO)                 \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM8_TRGO)                 \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM8_TRGO2)                \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM9_CH2)                  \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM9_TRGO)                 \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM12_TRGO)                \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM15_TRGO)                \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM18_TRGO)                \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_LPTIM1_CH2)                \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_LPTIM2_CH2)                \
   || ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_LPTIM3_CH2)                \
  )

#define IS_LL_ADC_INJ_TRIG_EXT_EDGE(__INJ_TRIG_EXT_EDGE__)                     \
  (((__INJ_TRIG_EXT_EDGE__) == LL_ADC_INJ_TRIG_EXT_RISING)                     \
   || ((__INJ_TRIG_EXT_EDGE__) == LL_ADC_INJ_TRIG_EXT_FALLING)                 \
   || ((__INJ_TRIG_EXT_EDGE__) == LL_ADC_INJ_TRIG_EXT_RISINGFALLING)           \
  )

#define IS_LL_ADC_INJ_TRIG_AUTO(__INJ_TRIG_AUTO__)                             \
  (((__INJ_TRIG_AUTO__) == LL_ADC_INJ_TRIG_INDEPENDENT)                        \
   || ((__INJ_TRIG_AUTO__) == LL_ADC_INJ_TRIG_FROM_GRP_REGULAR)                \
  )

#define IS_LL_ADC_INJ_SEQ_SCAN_LENGTH(__INJ_SEQ_SCAN_LENGTH__)                 \
  (((__INJ_SEQ_SCAN_LENGTH__) == LL_ADC_INJ_SEQ_SCAN_DISABLE)                  \
   || ((__INJ_SEQ_SCAN_LENGTH__) == LL_ADC_INJ_SEQ_SCAN_ENABLE_2RANKS)         \
   || ((__INJ_SEQ_SCAN_LENGTH__) == LL_ADC_INJ_SEQ_SCAN_ENABLE_3RANKS)         \
   || ((__INJ_SEQ_SCAN_LENGTH__) == LL_ADC_INJ_SEQ_SCAN_ENABLE_4RANKS)         \
  )

#define IS_LL_ADC_INJ_SEQ_SCAN_DISCONT_MODE(__INJ_SEQ_DISCONT_MODE__)          \
  (((__INJ_SEQ_DISCONT_MODE__) == LL_ADC_INJ_SEQ_DISCONT_DISABLE)              \
   || ((__INJ_SEQ_DISCONT_MODE__) == LL_ADC_INJ_SEQ_DISCONT_1RANK)             \
  )

#if defined(ADC_MULTIMODE_SUPPORT)
/* Check of parameters for configuration of ADC hierarchical scope:           */
/* multimode.                                                                 */
#define IS_LL_ADC_MULTI_MODE(__MULTI_MODE__)                                   \
  (((__MULTI_MODE__) == LL_ADC_MULTI_INDEPENDENT)                              \
   || ((__MULTI_MODE__) == LL_ADC_MULTI_DUAL_REG_SIMULT)                       \
   || ((__MULTI_MODE__) == LL_ADC_MULTI_DUAL_REG_INTERL)                       \
   || ((__MULTI_MODE__) == LL_ADC_MULTI_DUAL_INJ_SIMULT)                       \
   || ((__MULTI_MODE__) == LL_ADC_MULTI_DUAL_INJ_ALTERN)                       \
   || ((__MULTI_MODE__) == LL_ADC_MULTI_DUAL_REG_SIM_INJ_SIM)                  \
   || ((__MULTI_MODE__) == LL_ADC_MULTI_DUAL_REG_SIM_INJ_ALT)                  \
   || ((__MULTI_MODE__) == LL_ADC_MULTI_DUAL_REG_INT_INJ_SIM)                  \
  )

#define IS_LL_ADC_MULTI_DATA_FORMAT(__MULTI_DATA_FORMAT__)                     \
  (((__MULTI_DATA_FORMAT__) == LL_ADC_MULTI_REG_DATA_EACH_ADC)                 \
   || ((__MULTI_DATA_FORMAT__) == LL_ADC_MULTI_REG_DATA_COMMON_32B)            \
   || ((__MULTI_DATA_FORMAT__) == LL_ADC_MULTI_REG_DATA_COMMON_16B)            \
  )

#define IS_LL_ADC_MULTI_TWOSMP_DELAY(__MULTI_TWOSMP_DELAY__)                   \
  (((__MULTI_TWOSMP_DELAY__) == LL_ADC_MULTI_TWOSMP_DELAY_1CYCLE)              \
   || ((__MULTI_TWOSMP_DELAY__) == LL_ADC_MULTI_TWOSMP_DELAY_2CYCLES)          \
   || ((__MULTI_TWOSMP_DELAY__) == LL_ADC_MULTI_TWOSMP_DELAY_3CYCLES)          \
   || ((__MULTI_TWOSMP_DELAY__) == LL_ADC_MULTI_TWOSMP_DELAY_4CYCLES)          \
   || ((__MULTI_TWOSMP_DELAY__) == LL_ADC_MULTI_TWOSMP_DELAY_5CYCLES)          \
   || ((__MULTI_TWOSMP_DELAY__) == LL_ADC_MULTI_TWOSMP_DELAY_6CYCLES)          \
   || ((__MULTI_TWOSMP_DELAY__) == LL_ADC_MULTI_TWOSMP_DELAY_7CYCLES)          \
   || ((__MULTI_TWOSMP_DELAY__) == LL_ADC_MULTI_TWOSMP_DELAY_8CYCLES)          \
   || ((__MULTI_TWOSMP_DELAY__) == LL_ADC_MULTI_TWOSMP_DELAY_9CYCLES)          \
   || ((__MULTI_TWOSMP_DELAY__) == LL_ADC_MULTI_TWOSMP_DELAY_10CYCLES)         \
   || ((__MULTI_TWOSMP_DELAY__) == LL_ADC_MULTI_TWOSMP_DELAY_11CYCLES)         \
   || ((__MULTI_TWOSMP_DELAY__) == LL_ADC_MULTI_TWOSMP_DELAY_12CYCLES)         \
   || ((__MULTI_TWOSMP_DELAY__) == LL_ADC_MULTI_TWOSMP_DELAY_13CYCLES)         \
  )

#define IS_LL_ADC_MULTI_MASTER_SLAVE(__MULTI_MASTER_SLAVE__)                   \
  (((__MULTI_MASTER_SLAVE__) == LL_ADC_MULTI_MASTER)                           \
   || ((__MULTI_MASTER_SLAVE__) == LL_ADC_MULTI_SLAVE)                         \
   || ((__MULTI_MASTER_SLAVE__) == LL_ADC_MULTI_MASTER_SLAVE)                  \
  )

#endif /* ADC_MULTIMODE_SUPPORT */

/**
  * @}
  */


/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup ADC_LL_Exported_Functions
  * @{
  */

/** @addtogroup ADC_LL_EF_Init
  * @{
  */

/**
  * @brief  De-initialize registers of all ADC instances belonging to
  *         the same ADC common instance to their default reset values.
  * @note   This function is performing a hard reset, using high level
  *         clock source RCC ADC reset.
  *         Caution: On this STM32 series, if several ADC instances are available
  *         on the selected device, RCC ADC reset will reset
  *         all ADC instances belonging to the common ADC instance.
  *         To de-initialize only 1 ADC instance, use
  *         function @ref LL_ADC_DeInit().
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: ADC common registers are de-initialized
  *          - ERROR: not applicable
  */
ErrorStatus LL_ADC_CommonDeInit(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  /* Check the parameters */
  assert_param(IS_ADC_COMMON_INSTANCE(ADCxy_COMMON));

  /* Force reset of ADC clock (bus clock) */
  LL_AHB1_GRP1_ForceReset(LL_AHB1_GRP1_PERIPH_ADC12);

  /* Release reset of ADC clock (bus clock) */
  LL_AHB1_GRP1_ReleaseReset(LL_AHB1_GRP1_PERIPH_ADC12);

  return SUCCESS;
}

/**
  * @brief  Initialize some features of ADC common parameters
  *         (all ADC instances belonging to the same ADC common instance)
  *         and multimode (for devices with several ADC instances available).
  * @note   The setting of ADC common parameters is conditioned to
  *         ADC instances state:
  *         All ADC instances belonging to the same ADC common instance
  *         must be disabled.
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @param  pADC_CommonInitStruct Pointer to a @ref LL_ADC_CommonInitTypeDef structure
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: ADC common registers are initialized
  *          - ERROR: ADC common registers are not initialized
  */
ErrorStatus LL_ADC_CommonInit(ADC_Common_TypeDef *ADCxy_COMMON, const LL_ADC_CommonInitTypeDef *pADC_CommonInitStruct)
{
  ErrorStatus status = SUCCESS;

  /* Check the parameters */
  assert_param(IS_ADC_COMMON_INSTANCE(ADCxy_COMMON));
#if defined(ADC_MULTIMODE_SUPPORT)
  assert_param(IS_LL_ADC_MULTI_MODE(pADC_CommonInitStruct->Multimode));
  if (pADC_CommonInitStruct->Multimode != LL_ADC_MULTI_INDEPENDENT)
  {
    assert_param(IS_LL_ADC_MULTI_DATA_FORMAT(pADC_CommonInitStruct->MultiDataFormat));
    assert_param(IS_LL_ADC_MULTI_TWOSMP_DELAY(pADC_CommonInitStruct->MultiTwoSamplingDelay));
  }
#endif /* ADC_MULTIMODE_SUPPORT */

  /* Note: Hardware constraint (refer to description of functions             */
  /*       "LL_ADC_SetCommonXXX()" and "LL_ADC_SetMultiXXX()"):               */
  /*       On this STM32 series, setting of these features is conditioned to  */
  /*       ADC state:                                                         */
  /*       All ADC instances of the ADC common group must be disabled.        */
  if (__LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE(ADCxy_COMMON) == 0UL)
  {
    /* Configuration of ADC hierarchical scope:                               */
    /*  - common to several ADC                                               */
    /*    (all ADC instances belonging to the same ADC common instance)       */
    /*  - multimode (if several ADC instances available on the                */
    /*    selected device)                                                    */
#if defined(ADC_MULTIMODE_SUPPORT)
    if (pADC_CommonInitStruct->Multimode != LL_ADC_MULTI_INDEPENDENT)
    {
      MODIFY_REG(ADCxy_COMMON->CCR,
                 ADC_CCR_DUAL
                 | ADC_CCR_DAMDF
                 | ADC_CCR_DELAY
                 ,
                 pADC_CommonInitStruct->Multimode
                 | pADC_CommonInitStruct->MultiDataFormat
                 | pADC_CommonInitStruct->MultiTwoSamplingDelay
                );
    }
    else
    {
      MODIFY_REG(ADCxy_COMMON->CCR,
                 ADC_CCR_DUAL
                 | ADC_CCR_DAMDF
                 | ADC_CCR_DELAY
                 ,
                 LL_ADC_MULTI_INDEPENDENT
                );
    }
#else
    /* No configuration to perform */
#endif /* ADC_MULTIMODE_SUPPORT */
  }
  else
  {
    /* Initialization error: One or several ADC instances belonging to        */
    /* the same ADC common instance are not disabled.                         */
    status = ERROR;
  }

  return status;
}

/**
  * @brief  Set each @ref LL_ADC_CommonInitTypeDef field to default value.
  * @param  pADC_CommonInitStruct Pointer to a @ref LL_ADC_CommonInitTypeDef structure
  *                              whose fields will be set to default values.
  * @retval None
  */
void LL_ADC_CommonStructInit(LL_ADC_CommonInitTypeDef *pADC_CommonInitStruct)
{
  /* Set ADC_CommonInitStruct fields to default values */
  /* Set fields of ADC common */
  /* (all ADC instances belonging to the same ADC common instance) */
#if defined(ADC_MULTIMODE_SUPPORT)
  /* Set fields of ADC multimode */
  pADC_CommonInitStruct->Multimode             = LL_ADC_MULTI_INDEPENDENT;
  pADC_CommonInitStruct->MultiDataFormat       = LL_ADC_MULTI_REG_DATA_EACH_ADC;
  pADC_CommonInitStruct->MultiTwoSamplingDelay = LL_ADC_MULTI_TWOSMP_DELAY_1CYCLE;
#endif /* ADC_MULTIMODE_SUPPORT */
}

/**
  * @brief  De-initialize registers of the selected ADC instance
  *         to their default reset values.
  * @note   To reset all ADC instances quickly (perform a hard reset),
  *         use function @ref LL_ADC_CommonDeInit().
  * @note   If this functions returns error status, it means that ADC instance
  *         is in an unknown state.
  *         In this case, perform a hard reset using high level
  *         clock source RCC ADC reset.
  *         Caution: On this STM32 series, if several ADC instances are available
  *         on the selected device, RCC ADC reset will reset
  *         all ADC instances belonging to the common ADC instance.
  *         Refer to function @ref LL_ADC_CommonDeInit().
  * @param  ADCx ADC instance
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: ADC registers are de-initialized
  *          - ERROR: ADC registers are not de-initialized
  */
ErrorStatus LL_ADC_DeInit(ADC_TypeDef *ADCx)
{
  ErrorStatus status = SUCCESS;

  __IO uint32_t timeout_cpu_cycles = 0UL;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(ADCx));

  /* Disable ADC instance if not already disabled.                            */
  if (LL_ADC_IsEnabled(ADCx) == 1UL)
  {
    /* Stop potential ADC conversion on going on ADC group regular.           */
    if (LL_ADC_REG_IsConversionOngoing(ADCx) != 0UL)
    {
      if (LL_ADC_REG_IsStopConversionOngoing(ADCx) == 0UL)
      {
        LL_ADC_REG_StopConversion(ADCx);
      }
    }

    /* Stop potential ADC conversion on going on ADC group injected.          */
    if (LL_ADC_INJ_IsConversionOngoing(ADCx) != 0UL)
    {
      if (LL_ADC_INJ_IsStopConversionOngoing(ADCx) == 0UL)
      {
        LL_ADC_INJ_StopConversion(ADCx);
      }
    }

    /* Wait for ADC conversions are effectively stopped                       */
    timeout_cpu_cycles = ADC_TIMEOUT_STOP_CONVERSION_CPU_CYCLES;
    while ((LL_ADC_REG_IsStopConversionOngoing(ADCx)
            | LL_ADC_INJ_IsStopConversionOngoing(ADCx)) == 1UL)
    {
      timeout_cpu_cycles--;
      if (timeout_cpu_cycles == 0UL)
      {
        /* Time-out error */
        status = ERROR;
        break;
      }
    }

    /* Disable the ADC instance */
    LL_ADC_Disable(ADCx);

    /* Wait for ADC instance is effectively disabled */
    timeout_cpu_cycles = ADC_TIMEOUT_DISABLE_CPU_CYCLES;
    while (LL_ADC_IsDisableOngoing(ADCx) == 1UL)
    {
      timeout_cpu_cycles--;
      if (timeout_cpu_cycles == 0UL)
      {
        /* Time-out error */
        status = ERROR;
        break;
      }
    }
  }

  /* Check whether ADC state is compliant with expected state */
  if (READ_BIT(ADCx->CR,
               (ADC_CR_JADSTP | ADC_CR_ADSTP | ADC_CR_JADSTART | ADC_CR_ADSTART
                | ADC_CR_ADDIS | ADC_CR_ADEN)
              )
      == 0UL)
  {
    /* ========== Reset ADC registers ========== */
    /* Reset register IER */
    CLEAR_BIT(ADCx->IER,
              (LL_ADC_IT_ADRDY
               | LL_ADC_IT_EOC
               | LL_ADC_IT_EOS
               | LL_ADC_IT_OVR
               | LL_ADC_IT_EOSMP
               | LL_ADC_IT_JEOC
               | LL_ADC_IT_JEOS
               | LL_ADC_IT_AWD1
               | LL_ADC_IT_AWD2
               | LL_ADC_IT_AWD3
              )
             );

    /* Reset register ISR */
    SET_BIT(ADCx->ISR,
            (LL_ADC_FLAG_ADRDY
             | LL_ADC_FLAG_EOC
             | LL_ADC_FLAG_EOS
             | LL_ADC_FLAG_OVR
             | LL_ADC_FLAG_EOSMP
             | LL_ADC_FLAG_JEOC
             | LL_ADC_FLAG_JEOS
             | LL_ADC_FLAG_AWD1
             | LL_ADC_FLAG_AWD2
             | LL_ADC_FLAG_AWD3
            )
           );

    /* Reset register CR */
    /*  - Bits ADC_CR_JADSTP, ADC_CR_ADSTP, ADC_CR_JADSTART, ADC_CR_ADSTART,  */
    /*    ADC_CR_ADCAL, ADC_CR_ADDIS, ADC_CR_ADEN are in                      */
    /*    access mode "read-set": no direct reset applicable.                 */
    /*  - Reset Calibration mode to default setting (single ended).           */
    /*  - Enable ADC deep power down.                                         */
    /*    Note: ADC internal voltage regulator disable and ADC deep power     */
    /*          down enable are conditioned to ADC state disabled:            */
    /*          already done above.                                           */
    CLEAR_BIT(ADCx->CR, ADC_CR_ADCALDIF);
    SET_BIT(ADCx->CR, ADC_CR_DEEPPWD);

    /* Reset register CFGR */
    CLEAR_BIT(ADCx->CFGR1,
              ADC_CFGR1_AWD1CH  | ADC_CFGR1_JAUTO   | ADC_CFGR1_JAWD1EN |
              ADC_CFGR1_AWD1EN  | ADC_CFGR1_AWD1SGL | ADC_CFGR1_JDISCEN |
              ADC_CFGR1_DISCNUM | ADC_CFGR1_DISCEN  | ADC_CFGR1_AUTDLY  |
              ADC_CFGR1_CONT    | ADC_CFGR1_OVRMOD  |
              ADC_CFGR1_EXTEN   | ADC_CFGR1_EXTSEL  |
              ADC_CFGR1_RES     | ADC_CFGR1_DMNGT);


    /* Reset register CFGR2 */
    CLEAR_BIT(ADCx->CFGR2,
              (ADC_CFGR2_ROVSM  | ADC_CFGR2_TROVS   | ADC_CFGR2_OVSS
               | ADC_CFGR2_SWTRIG | ADC_CFGR2_BULB | ADC_CFGR2_SMPTRIG
               | ADC_CFGR2_OVSR  | ADC_CFGR2_JOVSE | ADC_CFGR2_ROVSE)
             );

    /* Reset register SMPR1 */
    CLEAR_BIT(ADCx->SMPR1,
              (ADC_SMPR1_SMP9 | ADC_SMPR1_SMP8 | ADC_SMPR1_SMP7
               | ADC_SMPR1_SMP6 | ADC_SMPR1_SMP5 | ADC_SMPR1_SMP4
               | ADC_SMPR1_SMP3 | ADC_SMPR1_SMP2 | ADC_SMPR1_SMP1)
             );

    /* Reset register SMPR2 */
    CLEAR_BIT(ADCx->SMPR2,
              (ADC_SMPR2_SMP18 | ADC_SMPR2_SMP17 | ADC_SMPR2_SMP16
               | ADC_SMPR2_SMP15 | ADC_SMPR2_SMP14 | ADC_SMPR2_SMP13
               | ADC_SMPR2_SMP12 | ADC_SMPR2_SMP11 | ADC_SMPR2_SMP10)
             );

    /* Reset registers of AWD1 thresholds */
    CLEAR_BIT(ADCx->AWD1LTR, ADC_AWD1LTR_LTR);
    SET_BIT(ADCx->AWD1HTR, ADC_AWD1HTR_HTR & (~ADC_AWD1HTR_HTR_22));

    /* Reset registers of AWD2 thresholds */
    CLEAR_BIT(ADCx->AWD2LTR, ADC_AWD2LTR_LTR);
    SET_BIT(ADCx->AWD2HTR, ADC_AWD2HTR_HTR & (~ADC_AWD2HTR_HTR_22));

    /* Reset registers of AWD3 thresholds */
    CLEAR_BIT(ADCx->AWD3LTR, ADC_AWD3LTR_LTR);
    SET_BIT(ADCx->AWD3HTR, ADC_AWD3HTR_HTR & (~ADC_AWD3HTR_HTR_22));

    /* Reset register SQR1 */
    CLEAR_BIT(ADCx->SQR1,
              (ADC_SQR1_SQ4 | ADC_SQR1_SQ3 | ADC_SQR1_SQ2
               | ADC_SQR1_SQ1 | ADC_SQR1_L)
             );

    /* Reset register SQR2 */
    CLEAR_BIT(ADCx->SQR2,
              (ADC_SQR2_SQ9 | ADC_SQR2_SQ8 | ADC_SQR2_SQ7
               | ADC_SQR2_SQ6 | ADC_SQR2_SQ5)
             );

    /* Reset register SQR3 */
    CLEAR_BIT(ADCx->SQR3,
              (ADC_SQR3_SQ14 | ADC_SQR3_SQ13 | ADC_SQR3_SQ12
               | ADC_SQR3_SQ11 | ADC_SQR3_SQ10)
             );

    /* Reset register SQR4 */
    CLEAR_BIT(ADCx->SQR4, ADC_SQR4_SQ16 | ADC_SQR4_SQ15);

    /* Reset register JSQR */
    CLEAR_BIT(ADCx->JSQR,
              (ADC_JSQR_JL
               | ADC_JSQR_JEXTSEL | ADC_JSQR_JEXTEN
               | ADC_JSQR_JSQ4    | ADC_JSQR_JSQ3
               | ADC_JSQR_JSQ2    | ADC_JSQR_JSQ1)
             );

    /* Reset register DR */
    /* Note: bits in access mode read only, no direct reset applicable */

    /* Reset register OFR1 */
    CLEAR_BIT(ADCx->OFR1, ADC_OFR1_OFFSET);
    /* Reset register OFCFGR1 */
    CLEAR_BIT(ADCx->OFCFGR1, ADC_OFCFGR1_SSAT | ADC_OFCFGR1_USAT | ADC_OFCFGR1_POSOFF | ADC_OFCFGR1_OFFSET_CH);
    /* Reset register OFR2 */
    CLEAR_BIT(ADCx->OFR2, ADC_OFR2_OFFSET);
    /* Reset register OFCFGR2 */
    CLEAR_BIT(ADCx->OFCFGR2, ADC_OFCFGR2_SSAT | ADC_OFCFGR2_USAT | ADC_OFCFGR2_POSOFF | ADC_OFCFGR2_OFFSET_CH);
    /* Reset register OFR3 */
    CLEAR_BIT(ADCx->OFR3, ADC_OFR3_OFFSET);
    /* Reset register OFCFGR3 */
    CLEAR_BIT(ADCx->OFCFGR3, ADC_OFCFGR3_SSAT | ADC_OFCFGR3_USAT | ADC_OFCFGR3_POSOFF | ADC_OFCFGR3_OFFSET_CH);
    /* Reset register OFR4 */
    CLEAR_BIT(ADCx->OFR4, ADC_OFR4_OFFSET);
    /* Reset register OFCFGR4 */
    CLEAR_BIT(ADCx->OFCFGR4, ADC_OFCFGR4_SSAT | ADC_OFCFGR4_USAT | ADC_OFCFGR4_POSOFF | ADC_OFCFGR4_OFFSET_CH);

    /* Reset register GCOMP */
    /* Note: Bitfield ADC_GCOMP_GCOMPCOEFF reset value is 0x1000 */
    MODIFY_REG(ADCx->GCOMP, ADC_GCOMP_GCOMP | ADC_GCOMP_GCOMPCOEFF, 0x1000UL);

    /* Reset registers JDR1, JDR2, JDR3, JDR4 */
    /* Note: bits in access mode read only, no direct reset applicable */

    /* Reset register AWD2CR */
    CLEAR_BIT(ADCx->AWD2CR, ADC_AWD2CR_AWD2CH);

    /* Reset register AWD3CR */
    CLEAR_BIT(ADCx->AWD3CR, ADC_AWD3CR_AWD3CH);

    /* Reset register DIFSEL */
    CLEAR_BIT(ADCx->DIFSEL, ADC_DIFSEL_DIFSEL);

    /* Reset register PCSEL */
    CLEAR_BIT(ADCx->PCSEL, ADC_PCSEL_PCSEL);

    /* Reset register CALFACT */
    CLEAR_BIT(ADCx->CALFACT, ADC_CALFACT_CALFACT_D | ADC_CALFACT_CALFACT_S);
  }
  else
  {
    /* ADC instance is in an unknown state */
    /* Need to performing a hard reset of ADC instance, using high level      */
    /* clock source RCC ADC reset.                                            */
    /* Caution: On this STM32 series, if several ADC instances are available  */
    /*          on the selected device, RCC ADC reset will reset              */
    /*          all ADC instances belonging to the common ADC instance.       */
    /* Caution: On this STM32 series, if several ADC instances are available  */
    /*          on the selected device, RCC ADC reset will reset              */
    /*          all ADC instances belonging to the common ADC instance.       */
    status = ERROR;
  }

  return status;
}

/**
  * @brief  Initialize some features of ADC instance.
  * @note   These parameters have an impact on ADC scope: ADC instance.
  *         Affects both group regular and group injected (availability
  *         of ADC group injected depends on STM32 families).
  *         Refer to corresponding unitary functions into
  *         @ref ADC_LL_EF_Configuration_ADC_Instance .
  * @note   The setting of these parameters by function @ref LL_ADC_Init()
  *         is conditioned to ADC state:
  *         ADC instance must be disabled.
  *         This condition is applied to all ADC features, for efficiency
  *         and compatibility over all STM32 families. However, the different
  *         features can be set under different ADC state conditions
  *         (setting possible with ADC enabled without conversion on going,
  *         ADC enabled with conversion on going, ...)
  *         Each feature can be updated afterwards with a unitary function
  *         and potentially with ADC in a different state than disabled,
  *         refer to description of each function for setting
  *         conditioned to ADC state.
  * @note   After using this function, some other features must be configured
  *         using LL unitary functions.
  *         The minimum configuration remaining to be done is:
  *          - Set ADC group regular or group injected sequencer:
  *            map channel on the selected sequencer rank.
  *            Refer to function @ref LL_ADC_REG_SetSequencerRanks().
  *          - Set ADC channel sampling time
  *            Refer to function LL_ADC_SetChannelSamplingTime();
  * @param  ADCx ADC instance
  * @param  pADC_InitStruct Pointer to a @ref LL_ADC_REG_InitTypeDef structure
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: ADC registers are initialized
  *          - ERROR: ADC registers are not initialized
  */
ErrorStatus LL_ADC_Init(ADC_TypeDef *ADCx, const LL_ADC_InitTypeDef *pADC_InitStruct)
{
  ErrorStatus status = SUCCESS;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(ADCx));

  assert_param(IS_LL_ADC_RESOLUTION(pADC_InitStruct->Resolution));
  assert_param(IS_LL_ADC_LOW_POWER(pADC_InitStruct->LowPowerMode));
  assert_param(IS_LL_ADC_LEFT_BIT_SHIFT(pADC_InitStruct->LeftBitShift));

  /* Note: Hardware constraint (refer to description of this function):       */
  /*       ADC instance must be disabled.                                     */
  if (LL_ADC_IsEnabled(ADCx) == 0UL)
  {
    /* Configuration of ADC hierarchical scope:                               */
    /*  - ADC instance                                                        */
    /*    - Set ADC data resolution                                           */
    /*    - Set ADC conversion data alignment                                 */
    /*    - Set ADC low power mode                                            */
    MODIFY_REG(ADCx->CFGR1,
               ADC_CFGR1_RES
               | ADC_CFGR1_AUTDLY
               ,
               pADC_InitStruct->Resolution
               | pADC_InitStruct->LowPowerMode
              );

    MODIFY_REG(ADCx->CFGR2, ADC_CFGR2_LSHIFT, pADC_InitStruct->LeftBitShift);
  }
  else
  {
    /* Initialization error: ADC instance is not disabled. */
    status = ERROR;
  }

  return status;
}

/**
  * @brief  Set each @ref LL_ADC_InitTypeDef field to default value.
  * @param  pADC_InitStruct Pointer to a @ref LL_ADC_InitTypeDef structure
  *                        whose fields will be set to default values.
  * @retval None
  */
void LL_ADC_StructInit(LL_ADC_InitTypeDef *pADC_InitStruct)
{
  /* Set ADC_InitStruct fields to default values */
  /* Set fields of ADC instance */
  pADC_InitStruct->Resolution    = LL_ADC_RESOLUTION_12B;
  pADC_InitStruct->LowPowerMode  = LL_ADC_LP_MODE_NONE;
  pADC_InitStruct->LeftBitShift  = LL_ADC_LEFT_BIT_SHIFT_NONE;
}

/**
  * @brief  Initialize some features of ADC group regular.
  * @note   These parameters have an impact on ADC scope: ADC group regular.
  *         Refer to corresponding unitary functions into
  *         @ref ADC_LL_EF_Configuration_ADC_Group_Regular
  *         (functions with prefix "REG").
  * @note   The setting of these parameters by function @ref LL_ADC_Init()
  *         is conditioned to ADC state:
  *         ADC instance must be disabled.
  *         This condition is applied to all ADC features, for efficiency
  *         and compatibility over all STM32 families. However, the different
  *         features can be set under different ADC state conditions
  *         (setting possible with ADC enabled without conversion on going,
  *         ADC enabled with conversion on going, ...)
  *         Each feature can be updated afterwards with a unitary function
  *         and potentially with ADC in a different state than disabled,
  *         refer to description of each function for setting
  *         conditioned to ADC state.
  * @note   After using this function, other features must be configured
  *         using LL unitary functions.
  *         The minimum configuration remaining to be done is:
  *          - Set ADC group regular or group injected sequencer:
  *            map channel on the selected sequencer rank.
  *            Refer to function @ref LL_ADC_REG_SetSequencerRanks().
  *          - Set ADC channel sampling time
  *            Refer to function LL_ADC_SetChannelSamplingTime();
  * @param  ADCx ADC instance
  * @param  pADC_RegInitStruct Pointer to a @ref LL_ADC_REG_InitTypeDef structure
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: ADC registers are initialized
  *          - ERROR: ADC registers are not initialized
  */
ErrorStatus LL_ADC_REG_Init(ADC_TypeDef *ADCx, const LL_ADC_REG_InitTypeDef *pADC_RegInitStruct)
{
  ErrorStatus status = SUCCESS;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(ADCx));
  assert_param(IS_LL_ADC_REG_TRIG_SOURCE(ADCx, pADC_RegInitStruct ->TriggerSource));
  assert_param(IS_LL_ADC_REG_SEQ_SCAN_LENGTH(pADC_RegInitStruct ->SequencerLength));
  assert_param(IS_LL_ADC_REG_DATA_TRANSFER_MODE(pADC_RegInitStruct ->DataTransferMode));

  if (pADC_RegInitStruct ->SequencerLength != LL_ADC_REG_SEQ_SCAN_DISABLE)
  {
    assert_param(IS_LL_ADC_REG_SEQ_SCAN_DISCONT_MODE(pADC_RegInitStruct ->SequencerDiscont));

    /* ADC group regular continuous mode and discontinuous mode                 */
    /* can not be enabled simultaneously                                       */
    assert_param((pADC_RegInitStruct ->ContinuousMode == LL_ADC_REG_CONV_SINGLE)
                 || (pADC_RegInitStruct ->SequencerDiscont == LL_ADC_REG_SEQ_DISCONT_DISABLE));
  }
  assert_param(IS_LL_ADC_REG_CONTINUOUS_MODE(pADC_RegInitStruct ->ContinuousMode));
  assert_param(IS_LL_ADC_REG_OVR_DATA_BEHAVIOR(pADC_RegInitStruct ->Overrun));

  /* Note: Hardware constraint (refer to description of this function):       */
  /*       ADC instance must be disabled.                                     */
  if (LL_ADC_IsEnabled(ADCx) == 0UL)
  {
    /* Configuration of ADC hierarchical scope:                               */
    /*  - ADC group regular                                                   */
    /*    - Set ADC group regular trigger source                              */
    /*    - Set ADC group regular sequencer length                            */
    /*    - Set ADC group regular sequencer discontinuous mode                */
    /*    - Set ADC group regular continuous mode                             */
    /*    - Set ADC group regular conversion data transfer: no transfer or    */
    /*      transfer by DMA, and DMA requests mode                            */
    /*    - Set ADC group regular overrun behavior                            */
    /* Note: On this STM32 series, ADC trigger edge is set when starting      */
    /*       ADC conversion.                                                  */
    /*       Refer to function @ref LL_ADC_REG_StartConversionExtTrig().      */
    if (pADC_RegInitStruct ->SequencerLength != LL_ADC_REG_SEQ_SCAN_DISABLE)
    {
      MODIFY_REG(ADCx->CFGR1,
                 ADC_CFGR1_EXTSEL
                 | ADC_CFGR1_EXTEN
                 | ADC_CFGR1_DISCEN
                 | ADC_CFGR1_DISCNUM
                 | ADC_CFGR1_CONT
                 | ADC_CFGR1_DMNGT
                 | ADC_CFGR1_OVRMOD
                 ,
                 pADC_RegInitStruct ->TriggerSource
                 | pADC_RegInitStruct ->SequencerDiscont
                 | pADC_RegInitStruct ->ContinuousMode
                 | pADC_RegInitStruct ->DataTransferMode
                 | pADC_RegInitStruct ->Overrun
                );
    }
    else
    {
      MODIFY_REG(ADCx->CFGR1,
                 ADC_CFGR1_EXTSEL
                 | ADC_CFGR1_EXTEN
                 | ADC_CFGR1_DISCEN
                 | ADC_CFGR1_DISCNUM
                 | ADC_CFGR1_CONT
                 | ADC_CFGR1_DMNGT
                 | ADC_CFGR1_OVRMOD
                 ,
                 pADC_RegInitStruct ->TriggerSource
                 | LL_ADC_REG_SEQ_DISCONT_DISABLE
                 | pADC_RegInitStruct ->ContinuousMode
                 | pADC_RegInitStruct ->DataTransferMode
                 | pADC_RegInitStruct ->Overrun
                );
    }

    /* Set ADC group regular sequencer length and scan direction */
    LL_ADC_REG_SetSequencerLength(ADCx, pADC_RegInitStruct ->SequencerLength);
  }
  else
  {
    /* Initialization error: ADC instance is not disabled. */
    status = ERROR;
  }
  return status;
}

/**
  * @brief  Set each @ref LL_ADC_REG_InitTypeDef field to default value.
  * @param  pADC_RegInitStruct  Pointer to a @ref LL_ADC_REG_InitTypeDef structure
  *                            whose fields will be set to default values.
  * @retval None
  */
void LL_ADC_REG_StructInit(LL_ADC_REG_InitTypeDef *pADC_RegInitStruct)
{
  /* Set ADC_REG_InitStruct fields to default values */
  /* Set fields of ADC group regular */
  /* Note: On this STM32 series, ADC trigger edge is set to value 0x0 by      */
  /*       setting of trigger source to SW start.                             */
  pADC_RegInitStruct ->TriggerSource    = LL_ADC_REG_TRIG_SOFTWARE;
  pADC_RegInitStruct ->SequencerLength  = LL_ADC_REG_SEQ_SCAN_DISABLE;
  pADC_RegInitStruct ->SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
  pADC_RegInitStruct ->ContinuousMode   = LL_ADC_REG_CONV_SINGLE;
  pADC_RegInitStruct ->DataTransferMode = LL_ADC_REG_DR_TRANSFER;
  pADC_RegInitStruct ->Overrun          = LL_ADC_REG_OVR_DATA_OVERWRITTEN;
}

/**
  * @brief  Initialize some features of ADC group injected.
  * @note   These parameters have an impact on ADC scope: ADC group injected.
  *         Refer to corresponding unitary functions into
  *         @ref ADC_LL_EF_Configuration_ADC_Group_Regular
  *         (functions with prefix "INJ").
  * @note   The setting of these parameters by function @ref LL_ADC_Init()
  *         is conditioned to ADC state:
  *         ADC instance must be disabled.
  *         This condition is applied to all ADC features, for efficiency
  *         and compatibility over all STM32 families. However, the different
  *         features can be set under different ADC state conditions
  *         (setting possible with ADC enabled without conversion on going,
  *         ADC enabled with conversion on going, ...)
  *         Each feature can be updated afterwards with a unitary function
  *         and potentially with ADC in a different state than disabled,
  *         refer to description of each function for setting
  *         conditioned to ADC state.
  * @note   After using this function, other features must be configured
  *         using LL unitary functions.
  *         The minimum configuration remaining to be done is:
  *          - Set ADC group injected sequencer:
  *            map channel on the selected sequencer rank.
  *            Refer to function @ref LL_ADC_INJ_SetSequencerRanks().
  *          - Set ADC channel sampling time
  *            Refer to function LL_ADC_SetChannelSamplingTime();
  * @param  ADCx ADC instance
  * @param  pADC_InjInitStruct Pointer to a @ref LL_ADC_INJ_InitTypeDef structure
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: ADC registers are initialized
  *          - ERROR: ADC registers are not initialized
  */
ErrorStatus LL_ADC_INJ_Init(ADC_TypeDef *ADCx, const LL_ADC_INJ_InitTypeDef *pADC_InjInitStruct)
{
  ErrorStatus status = SUCCESS;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(ADCx));
  assert_param(IS_LL_ADC_INJ_TRIG_SOURCE(ADCx, pADC_InjInitStruct->TriggerSource));
  assert_param(IS_LL_ADC_INJ_SEQ_SCAN_LENGTH(pADC_InjInitStruct->SequencerLength));
  if (pADC_InjInitStruct->SequencerLength != LL_ADC_INJ_SEQ_SCAN_DISABLE)
  {
    assert_param(IS_LL_ADC_INJ_SEQ_SCAN_DISCONT_MODE(pADC_InjInitStruct->SequencerDiscont));
  }
  assert_param(IS_LL_ADC_INJ_TRIG_AUTO(pADC_InjInitStruct->TrigAuto));

  /* Note: Hardware constraint (refer to description of this function):       */
  /*       ADC instance must be disabled.                                     */
  if (LL_ADC_IsEnabled(ADCx) == 0UL)
  {
    /* Configuration of ADC hierarchical scope:                               */
    /*  - ADC group injected                                                  */
    /*    - Set ADC group injected trigger source                             */
    /*    - Set ADC group injected sequencer length                           */
    /*    - Set ADC group injected sequencer discontinuous mode               */
    /*    - Set ADC group injected conversion trigger: independent or         */
    /*      from ADC group regular                                            */
    /* Note: On this STM32 series, ADC trigger edge is set when starting       */
    /*       ADC conversion.                                                  */
    /*       Refer to function @ref LL_ADC_INJ_StartConversionExtTrig().      */
    if (pADC_InjInitStruct->SequencerLength != LL_ADC_INJ_SEQ_SCAN_DISABLE)
    {
      MODIFY_REG(ADCx->CFGR1,
                 ADC_CFGR1_JDISCEN
                 | ADC_CFGR1_JAUTO
                 ,
                 pADC_InjInitStruct->SequencerDiscont
                 | pADC_InjInitStruct->TrigAuto
                );
    }
    else
    {
      MODIFY_REG(ADCx->CFGR1,
                 ADC_CFGR1_JDISCEN
                 | ADC_CFGR1_JAUTO
                 ,
                 LL_ADC_REG_SEQ_DISCONT_DISABLE
                 | pADC_InjInitStruct->TrigAuto
                );
    }

    MODIFY_REG(ADCx->JSQR,
               ADC_JSQR_JEXTSEL
               | ADC_JSQR_JEXTEN
               | ADC_JSQR_JL
               ,
               pADC_InjInitStruct->TriggerSource
               | pADC_InjInitStruct->SequencerLength
              );
  }
  else
  {
    /* Initialization error: ADC instance is not disabled. */
    status = ERROR;
  }
  return status;
}

/**
  * @brief  Set each @ref LL_ADC_INJ_InitTypeDef field to default value.
  * @param  pADC_InjInitStruct Pointer to a @ref LL_ADC_INJ_InitTypeDef structure
  *                            whose fields will be set to default values.
  * @retval None
  */
void LL_ADC_INJ_StructInit(LL_ADC_INJ_InitTypeDef *pADC_InjInitStruct)
{
  /* Set ADC_INJ_InitStruct fields to default values */
  /* Set fields of ADC group injected */
  pADC_InjInitStruct->TriggerSource    = LL_ADC_INJ_TRIG_SOFTWARE;
  pADC_InjInitStruct->SequencerLength  = LL_ADC_INJ_SEQ_SCAN_DISABLE;
  pADC_InjInitStruct->SequencerDiscont = LL_ADC_INJ_SEQ_DISCONT_DISABLE;
  pADC_InjInitStruct->TrigAuto         = LL_ADC_INJ_TRIG_INDEPENDENT;
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

#endif /* ADC1 || ADC2 */

/**
  * @}
  */

#endif /* USE_FULL_LL_DRIVER */
