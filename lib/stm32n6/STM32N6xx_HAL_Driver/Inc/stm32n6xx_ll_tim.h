/**
  ******************************************************************************
  * @file    stm32n6xx_ll_tim.h
  * @author  MCD Application Team
  * @brief   Header file of TIM LL module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32N6xx_LL_TIM_H
#define __STM32N6xx_LL_TIM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx.h"

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

#if defined (TIM1)  \
 || defined (TIM2)  \
 || defined (TIM3)  \
 || defined (TIM4)  \
 || defined (TIM5)  \
 || defined (TIM6)  \
 || defined (TIM7)  \
 || defined (TIM8)  \
 || defined (TIM9)  \
 || defined (TIM10) \
 || defined (TIM11) \
 || defined (TIM12) \
 || defined (TIM13) \
 || defined (TIM14) \
 || defined (TIM15) \
 || defined (TIM16) \
 || defined (TIM17) \
 || defined (TIM18)

/** @defgroup TIM_LL TIM
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup TIM_LL_Private_Variables TIM Private Variables
  * @{
  */
static const uint8_t OFFSET_TAB_CCMRx[] =
{
  0x00U,   /* 0: TIMx_CH1  */
  0x00U,   /* 1: TIMx_CH1N */
  0x00U,   /* 2: TIMx_CH2  */
  0x00U,   /* 3: TIMx_CH2N */
  0x04U,   /* 4: TIMx_CH3  */
  0x04U,   /* 5: TIMx_CH3N */
  0x04U,   /* 6: TIMx_CH4  */
  0x04U,   /* 7: TIMx_CH4N */
  0x38U,   /* 8: TIMx_CH5  */
  0x38U    /* 9: TIMx_CH6  */

};

static const uint8_t SHIFT_TAB_OCxx[] =
{
  0U,            /* 0: OC1M, OC1FE, OC1PE */
  0U,            /* 1: - NA */
  8U,            /* 2: OC2M, OC2FE, OC2PE */
  0U,            /* 3: - NA */
  0U,            /* 4: OC3M, OC3FE, OC3PE */
  0U,            /* 5: - NA */
  8U,            /* 6: OC4M, OC4FE, OC4PE */
  0U,            /* 7: - NA */
  0U,            /* 8: OC5M, OC5FE, OC5PE */
  8U             /* 9: OC6M, OC6FE, OC6PE */
};

static const uint8_t SHIFT_TAB_ICxx[] =
{
  0U,            /* 0: CC1S, IC1PSC, IC1F */
  0U,            /* 1: - NA */
  8U,            /* 2: CC2S, IC2PSC, IC2F */
  0U,            /* 3: - NA */
  0U,            /* 4: CC3S, IC3PSC, IC3F */
  0U,            /* 5: - NA */
  8U,            /* 6: CC4S, IC4PSC, IC4F */
  0U,            /* 7: - NA */
  0U,            /* 8: - NA */
  0U             /* 9: - NA */
};

static const uint8_t SHIFT_TAB_CCxP[] =
{
  0U,            /* 0: CC1P */
  2U,            /* 1: CC1NP */
  4U,            /* 2: CC2P */
  6U,            /* 3: CC2NP */
  8U,            /* 4: CC3P */
  10U,           /* 5: CC3NP */
  12U,           /* 6: CC4P */
  14U,           /* 7: CC4NP */
  16U,           /* 8: CC5P */
  20U            /* 9: CC6P */
};

static const uint8_t SHIFT_TAB_OISx[] =
{
  0U,            /* 0: OIS1 */
  1U,            /* 1: OIS1N */
  2U,            /* 2: OIS2 */
  3U,            /* 3: OIS2N */
  4U,            /* 4: OIS3 */
  5U,            /* 5: OIS3N */
  6U,            /* 6: OIS4 */
  7U,            /* 7: OIS4N */
  8U,            /* 8: OIS5 */
  10U            /* 9: OIS6 */
};
/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup TIM_LL_Private_Constants TIM Private Constants
  * @{
  */

/* Defines used for the bit position in the register and perform offsets */
#define TIM_POSITION_BRK_SOURCE            (POSITION_VAL(Source) & 0x1FUL)

/* Generic bit definitions for TIMx_AF1 register */
#define TIMx_AF1_BKINP     TIM_AF1_BKINP     /*!< BRK BKIN input polarity */
#define TIMx_AF1_ETRSEL    TIM_AF1_ETRSEL    /*!< TIMx ETR source selection */


/* Mask used to set the TDG[x:0] of the DTG bits of the TIMx_BDTR register */
#define DT_DELAY_1 ((uint8_t)0x7F)
#define DT_DELAY_2 ((uint8_t)0x3F)
#define DT_DELAY_3 ((uint8_t)0x1F)
#define DT_DELAY_4 ((uint8_t)0x1F)

/* Mask used to set the DTG[7:5] bits of the DTG bits of the TIMx_BDTR register */
#define DT_RANGE_1 ((uint8_t)0x00)
#define DT_RANGE_2 ((uint8_t)0x80)
#define DT_RANGE_3 ((uint8_t)0xC0)
#define DT_RANGE_4 ((uint8_t)0xE0)

/** Legacy definitions for compatibility purpose
@cond 0
  */
/**
@endcond
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup TIM_LL_Private_Macros TIM Private Macros
  * @{
  */
/** @brief  Convert channel id into channel index.
  * @param  __CHANNEL__ This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH1N
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH2N
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH3N
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH4N
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @retval none
  */
#define TIM_GET_CHANNEL_INDEX( __CHANNEL__) \
  (((__CHANNEL__) == LL_TIM_CHANNEL_CH1) ? 0U :\
   ((__CHANNEL__) == LL_TIM_CHANNEL_CH1N) ? 1U :\
   ((__CHANNEL__) == LL_TIM_CHANNEL_CH2) ? 2U :\
   ((__CHANNEL__) == LL_TIM_CHANNEL_CH2N) ? 3U :\
   ((__CHANNEL__) == LL_TIM_CHANNEL_CH3) ? 4U :\
   ((__CHANNEL__) == LL_TIM_CHANNEL_CH3N) ? 5U :\
   ((__CHANNEL__) == LL_TIM_CHANNEL_CH4) ? 6U :\
   ((__CHANNEL__) == LL_TIM_CHANNEL_CH4N) ? 7U :\
   ((__CHANNEL__) == LL_TIM_CHANNEL_CH5) ? 8U : 9U)

/** @brief  Calculate the deadtime sampling period(in ps).
  * @param  __TIMCLK__ timer input clock frequency (in Hz).
  * @param  __CKD__ This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CLOCKDIVISION_DIV1
  *         @arg @ref LL_TIM_CLOCKDIVISION_DIV2
  *         @arg @ref LL_TIM_CLOCKDIVISION_DIV4
  * @retval none
  */
#define TIM_CALC_DTS(__TIMCLK__, __CKD__)                                                        \
  (((__CKD__) == LL_TIM_CLOCKDIVISION_DIV1) ? ((uint64_t)1000000000000U/(__TIMCLK__))         : \
   ((__CKD__) == LL_TIM_CLOCKDIVISION_DIV2) ? ((uint64_t)1000000000000U/((__TIMCLK__) >> 1U)) : \
   ((uint64_t)1000000000000U/((__TIMCLK__) >> 2U)))
/**
  * @}
  */


/* Exported types ------------------------------------------------------------*/
#if defined(USE_FULL_LL_DRIVER)
/** @defgroup TIM_LL_ES_INIT TIM Exported Init structure
  * @{
  */

/**
  * @brief  TIM Time Base configuration structure definition.
  */
typedef struct
{
  uint16_t Prescaler;         /*!< Specifies the prescaler value used to divide the TIM clock.
                                   This parameter can be a number between Min_Data=0x0000 and Max_Data=0xFFFF.

                                   This feature can be modified afterwards using unitary function
                                   @ref LL_TIM_SetPrescaler().*/

  uint32_t CounterMode;       /*!< Specifies the counter mode.
                                   This parameter can be a value of @ref TIM_LL_EC_COUNTERMODE.

                                   This feature can be modified afterwards using unitary function
                                   @ref LL_TIM_SetCounterMode().*/

  uint32_t Autoreload;        /*!< Specifies the auto reload value to be loaded into the active
                                   Auto-Reload Register at the next update event.
                                   This parameter must be a number between Min_Data=0x0000 and Max_Data=0xFFFF.
                                   Some timer instances may support 32 bits counters. In that case this parameter must
                                   be a number between 0x0000 and 0xFFFFFFFF.

                                   This feature can be modified afterwards using unitary function
                                   @ref LL_TIM_SetAutoReload().*/

  uint32_t ClockDivision;     /*!< Specifies the clock division.
                                   This parameter can be a value of @ref TIM_LL_EC_CLOCKDIVISION.

                                   This feature can be modified afterwards using unitary function
                                   @ref LL_TIM_SetClockDivision().*/

  uint32_t RepetitionCounter;  /*!< Specifies the repetition counter value. Each time the RCR downcounter
                                   reaches zero, an update event is generated and counting restarts
                                   from the RCR value (N).
                                   This means in PWM mode that (N+1) corresponds to:
                                      - the number of PWM periods in edge-aligned mode
                                      - the number of half PWM period in center-aligned mode
                                   GP timers: this parameter must be a number between Min_Data = 0x00 and
                                   Max_Data = 0xFF.
                                   Advanced timers: this parameter must be a number between Min_Data = 0x0000 and
                                   Max_Data = 0xFFFF.

                                   This feature can be modified afterwards using unitary function
                                   @ref LL_TIM_SetRepetitionCounter().*/
} LL_TIM_InitTypeDef;

/**
  * @brief  TIM Output Compare configuration structure definition.
  */
typedef struct
{
  uint32_t OCMode;        /*!< Specifies the output mode.
                               This parameter can be a value of @ref TIM_LL_EC_OCMODE.

                               This feature can be modified afterwards using unitary function
                               @ref LL_TIM_OC_SetMode().*/

  uint32_t OCState;       /*!< Specifies the TIM Output Compare state.
                               This parameter can be a value of @ref TIM_LL_EC_OCSTATE.

                               This feature can be modified afterwards using unitary functions
                               @ref LL_TIM_CC_EnableChannel() or @ref LL_TIM_CC_DisableChannel().*/

  uint32_t OCNState;      /*!< Specifies the TIM complementary Output Compare state.
                               This parameter can be a value of @ref TIM_LL_EC_OCSTATE.

                               This feature can be modified afterwards using unitary functions
                               @ref LL_TIM_CC_EnableChannel() or @ref LL_TIM_CC_DisableChannel().*/

  uint32_t CompareValue;  /*!< Specifies the Compare value to be loaded into the Capture Compare Register.
                               This parameter can be a number between Min_Data=0x0000 and Max_Data=0xFFFF.

                               This feature can be modified afterwards using unitary function
                               LL_TIM_OC_SetCompareCHx (x=1..6).*/

  uint32_t OCPolarity;    /*!< Specifies the output polarity.
                               This parameter can be a value of @ref TIM_LL_EC_OCPOLARITY.

                               This feature can be modified afterwards using unitary function
                               @ref LL_TIM_OC_SetPolarity().*/

  uint32_t OCNPolarity;   /*!< Specifies the complementary output polarity.
                               This parameter can be a value of @ref TIM_LL_EC_OCPOLARITY.

                               This feature can be modified afterwards using unitary function
                               @ref LL_TIM_OC_SetPolarity().*/


  uint32_t OCIdleState;   /*!< Specifies the TIM Output Compare pin state during Idle state.
                               This parameter can be a value of @ref TIM_LL_EC_OCIDLESTATE.

                               This feature can be modified afterwards using unitary function
                               @ref LL_TIM_OC_SetIdleState().*/

  uint32_t OCNIdleState;  /*!< Specifies the TIM Output Compare pin state during Idle state.
                               This parameter can be a value of @ref TIM_LL_EC_OCIDLESTATE.

                               This feature can be modified afterwards using unitary function
                               @ref LL_TIM_OC_SetIdleState().*/
} LL_TIM_OC_InitTypeDef;

/**
  * @brief  TIM Input Capture configuration structure definition.
  */

typedef struct
{

  uint32_t ICPolarity;    /*!< Specifies the active edge of the input signal.
                               This parameter can be a value of @ref TIM_LL_EC_IC_POLARITY.

                               This feature can be modified afterwards using unitary function
                               @ref LL_TIM_IC_SetPolarity().*/

  uint32_t ICActiveInput; /*!< Specifies the input.
                               This parameter can be a value of @ref TIM_LL_EC_ACTIVEINPUT.

                               This feature can be modified afterwards using unitary function
                               @ref LL_TIM_IC_SetActiveInput().*/

  uint32_t ICPrescaler;   /*!< Specifies the Input Capture Prescaler.
                               This parameter can be a value of @ref TIM_LL_EC_ICPSC.

                               This feature can be modified afterwards using unitary function
                               @ref LL_TIM_IC_SetPrescaler().*/

  uint32_t ICFilter;      /*!< Specifies the input capture filter.
                               This parameter can be a value of @ref TIM_LL_EC_IC_FILTER.

                               This feature can be modified afterwards using unitary function
                               @ref LL_TIM_IC_SetFilter().*/
} LL_TIM_IC_InitTypeDef;


/**
  * @brief  TIM Encoder interface configuration structure definition.
  */
typedef struct
{
  uint32_t EncoderMode;     /*!< Specifies the encoder resolution (x2 or x4).
                                 This parameter can be a value of @ref TIM_LL_EC_ENCODERMODE.

                                 This feature can be modified afterwards using unitary function
                                 @ref LL_TIM_SetEncoderMode().*/

  uint32_t IC1Polarity;     /*!< Specifies the active edge of TI1 input.
                                 This parameter can be a value of @ref TIM_LL_EC_IC_POLARITY.

                                 This feature can be modified afterwards using unitary function
                                 @ref LL_TIM_IC_SetPolarity().*/

  uint32_t IC1ActiveInput;  /*!< Specifies the TI1 input source
                                 This parameter can be a value of @ref TIM_LL_EC_ACTIVEINPUT.

                                 This feature can be modified afterwards using unitary function
                                 @ref LL_TIM_IC_SetActiveInput().*/

  uint32_t IC1Prescaler;    /*!< Specifies the TI1 input prescaler value.
                                 This parameter can be a value of @ref TIM_LL_EC_ICPSC.

                                 This feature can be modified afterwards using unitary function
                                 @ref LL_TIM_IC_SetPrescaler().*/

  uint32_t IC1Filter;       /*!< Specifies the TI1 input filter.
                                 This parameter can be a value of @ref TIM_LL_EC_IC_FILTER.

                                 This feature can be modified afterwards using unitary function
                                 @ref LL_TIM_IC_SetFilter().*/

  uint32_t IC2Polarity;      /*!< Specifies the active edge of TI2 input.
                                 This parameter can be a value of @ref TIM_LL_EC_IC_POLARITY.

                                 This feature can be modified afterwards using unitary function
                                 @ref LL_TIM_IC_SetPolarity().*/

  uint32_t IC2ActiveInput;  /*!< Specifies the TI2 input source
                                 This parameter can be a value of @ref TIM_LL_EC_ACTIVEINPUT.

                                 This feature can be modified afterwards using unitary function
                                 @ref LL_TIM_IC_SetActiveInput().*/

  uint32_t IC2Prescaler;    /*!< Specifies the TI2 input prescaler value.
                                 This parameter can be a value of @ref TIM_LL_EC_ICPSC.

                                 This feature can be modified afterwards using unitary function
                                 @ref LL_TIM_IC_SetPrescaler().*/

  uint32_t IC2Filter;       /*!< Specifies the TI2 input filter.
                                 This parameter can be a value of @ref TIM_LL_EC_IC_FILTER.

                                 This feature can be modified afterwards using unitary function
                                 @ref LL_TIM_IC_SetFilter().*/

} LL_TIM_ENCODER_InitTypeDef;

/**
  * @brief  TIM Hall sensor interface configuration structure definition.
  */
typedef struct
{

  uint32_t IC1Polarity;        /*!< Specifies the active edge of TI1 input.
                                    This parameter can be a value of @ref TIM_LL_EC_IC_POLARITY.

                                    This feature can be modified afterwards using unitary function
                                    @ref LL_TIM_IC_SetPolarity().*/

  uint32_t IC1Prescaler;       /*!< Specifies the TI1 input prescaler value.
                                    Prescaler must be set to get a maximum counter period longer than the
                                    time interval between 2 consecutive changes on the Hall inputs.
                                    This parameter can be a value of @ref TIM_LL_EC_ICPSC.

                                    This feature can be modified afterwards using unitary function
                                    @ref LL_TIM_IC_SetPrescaler().*/

  uint32_t IC1Filter;          /*!< Specifies the TI1 input filter.
                                    This parameter can be a value of
                                    @ref TIM_LL_EC_IC_FILTER.

                                    This feature can be modified afterwards using unitary function
                                    @ref LL_TIM_IC_SetFilter().*/

  uint32_t CommutationDelay;   /*!< Specifies the compare value to be loaded into the Capture Compare Register.
                                    A positive pulse (TRGO event) is generated with a programmable delay every time
                                    a change occurs on the Hall inputs.
                                    This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF.

                                    This feature can be modified afterwards using unitary function
                                    @ref LL_TIM_OC_SetCompareCH2().*/
} LL_TIM_HALLSENSOR_InitTypeDef;

/**
  * @brief  BDTR (Break and Dead Time) structure definition
  */
typedef struct
{
  uint32_t OSSRState;            /*!< Specifies the Off-State selection used in Run mode.
                                      This parameter can be a value of @ref TIM_LL_EC_OSSR

                                      This feature can be modified afterwards using unitary function
                                      @ref LL_TIM_SetOffStates()

                                      @note This bit-field cannot be modified as long as LOCK level 2 has been
                                       programmed. */

  uint32_t OSSIState;            /*!< Specifies the Off-State used in Idle state.
                                      This parameter can be a value of @ref TIM_LL_EC_OSSI

                                      This feature can be modified afterwards using unitary function
                                      @ref LL_TIM_SetOffStates()

                                      @note This bit-field cannot be modified as long as LOCK level 2 has been
                                      programmed. */

  uint32_t LockLevel;            /*!< Specifies the LOCK level parameters.
                                      This parameter can be a value of @ref TIM_LL_EC_LOCKLEVEL

                                      @note The LOCK bits can be written only once after the reset. Once the TIMx_BDTR
                                      register has been written, their content is frozen until the next reset.*/

  uint8_t DeadTime;              /*!< Specifies the delay time between the switching-off and the
                                      switching-on of the outputs.
                                      This parameter can be a number between Min_Data = 0x00 and Max_Data = 0xFF.

                                      This feature can be modified afterwards using unitary function
                                      @ref LL_TIM_OC_SetDeadTime()

                                      @note This bit-field can not be modified as long as LOCK level 1, 2 or 3 has been
                                       programmed. */

  uint16_t BreakState;           /*!< Specifies whether the TIM Break input is enabled or not.
                                      This parameter can be a value of @ref TIM_LL_EC_BREAK_ENABLE

                                      This feature can be modified afterwards using unitary functions
                                      @ref LL_TIM_EnableBRK() or @ref LL_TIM_DisableBRK()

                                      @note This bit-field can not be modified as long as LOCK level 1 has been
                                      programmed. */

  uint32_t BreakPolarity;        /*!< Specifies the TIM Break Input pin polarity.
                                      This parameter can be a value of @ref TIM_LL_EC_BREAK_POLARITY

                                      This feature can be modified afterwards using unitary function
                                      @ref LL_TIM_ConfigBRK()

                                      @note This bit-field can not be modified as long as LOCK level 1 has been
                                      programmed. */

  uint32_t BreakFilter;          /*!< Specifies the TIM Break Filter.
                                      This parameter can be a value of @ref TIM_LL_EC_BREAK_FILTER

                                      This feature can be modified afterwards using unitary function
                                      @ref LL_TIM_ConfigBRK()

                                      @note This bit-field can not be modified as long as LOCK level 1 has been
                                      programmed. */

  uint32_t BreakAFMode;           /*!< Specifies the alternate function mode of the break input.
                                      This parameter can be a value of @ref TIM_LL_EC_BREAK_AFMODE

                                      This feature can be modified afterwards using unitary functions
                                      @ref LL_TIM_ConfigBRK()

                                      @note Bidirectional break input is only supported by advanced timers instances.

                                      @note This bit-field can not be modified as long as LOCK level 1 has been
                                      programmed. */

  uint32_t Break2State;          /*!< Specifies whether the TIM Break2 input is enabled or not.
                                      This parameter can be a value of @ref TIM_LL_EC_BREAK2_ENABLE

                                      This feature can be modified afterwards using unitary functions
                                      @ref LL_TIM_EnableBRK2() or @ref LL_TIM_DisableBRK2()

                                      @note This bit-field can not be modified as long as LOCK level 1 has been
                                      programmed. */

  uint32_t Break2Polarity;        /*!< Specifies the TIM Break2 Input pin polarity.
                                      This parameter can be a value of @ref TIM_LL_EC_BREAK2_POLARITY

                                      This feature can be modified afterwards using unitary function
                                      @ref LL_TIM_ConfigBRK2()

                                      @note This bit-field can not be modified as long as LOCK level 1 has been
                                      programmed. */

  uint32_t Break2Filter;          /*!< Specifies the TIM Break2 Filter.
                                      This parameter can be a value of @ref TIM_LL_EC_BREAK2_FILTER

                                      This feature can be modified afterwards using unitary function
                                      @ref LL_TIM_ConfigBRK2()

                                      @note This bit-field can not be modified as long as LOCK level 1 has been
                                      programmed. */

  uint32_t Break2AFMode;          /*!< Specifies the alternate function mode of the break2 input.
                                      This parameter can be a value of @ref TIM_LL_EC_BREAK2_AFMODE

                                      This feature can be modified afterwards using unitary functions
                                      @ref LL_TIM_ConfigBRK2()

                                      @note Bidirectional break input is only supported by advanced timers instances.

                                      @note This bit-field can not be modified as long as LOCK level 1 has been
                                      programmed. */

  uint32_t AutomaticOutput;      /*!< Specifies whether the TIM Automatic Output feature is enabled or not.
                                      This parameter can be a value of @ref TIM_LL_EC_AUTOMATICOUTPUT_ENABLE

                                      This feature can be modified afterwards using unitary functions
                                      @ref LL_TIM_EnableAutomaticOutput() or @ref LL_TIM_DisableAutomaticOutput()

                                      @note This bit-field can not be modified as long as LOCK level 1 has been
                                      programmed. */
} LL_TIM_BDTR_InitTypeDef;

/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/* Exported constants --------------------------------------------------------*/
/** @defgroup TIM_LL_Exported_Constants TIM Exported Constants
  * @{
  */

/** @defgroup TIM_LL_EC_GET_FLAG Get Flags Defines
  * @brief    Flags defines which can be used with LL_TIM_ReadReg function.
  * @{
  */
#define LL_TIM_SR_UIF                          TIM_SR_UIF           /*!< Update interrupt flag */
#define LL_TIM_SR_CC1IF                        TIM_SR_CC1IF         /*!< Capture/compare 1 interrupt flag */
#define LL_TIM_SR_CC2IF                        TIM_SR_CC2IF         /*!< Capture/compare 2 interrupt flag */
#define LL_TIM_SR_CC3IF                        TIM_SR_CC3IF         /*!< Capture/compare 3 interrupt flag */
#define LL_TIM_SR_CC4IF                        TIM_SR_CC4IF         /*!< Capture/compare 4 interrupt flag */
#define LL_TIM_SR_CC5IF                        TIM_SR_CC5IF         /*!< Capture/compare 5 interrupt flag */
#define LL_TIM_SR_CC6IF                        TIM_SR_CC6IF         /*!< Capture/compare 6 interrupt flag */
#define LL_TIM_SR_COMIF                        TIM_SR_COMIF         /*!< COM interrupt flag */
#define LL_TIM_SR_TIF                          TIM_SR_TIF           /*!< Trigger interrupt flag */
#define LL_TIM_SR_BIF                          TIM_SR_BIF           /*!< Break interrupt flag */
#define LL_TIM_SR_B2IF                         TIM_SR_B2IF          /*!< Second break interrupt flag */
#define LL_TIM_SR_CC1OF                        TIM_SR_CC1OF         /*!< Capture/Compare 1 overcapture flag */
#define LL_TIM_SR_CC2OF                        TIM_SR_CC2OF         /*!< Capture/Compare 2 overcapture flag */
#define LL_TIM_SR_CC3OF                        TIM_SR_CC3OF         /*!< Capture/Compare 3 overcapture flag */
#define LL_TIM_SR_CC4OF                        TIM_SR_CC4OF         /*!< Capture/Compare 4 overcapture flag */
#define LL_TIM_SR_SBIF                         TIM_SR_SBIF          /*!< System Break interrupt flag  */
#define LL_TIM_SR_IDXF                         TIM_SR_IDXF          /*!< Index interrupt flag  */
#define LL_TIM_SR_DIRF                         TIM_SR_DIRF          /*!< Direction Change interrupt flag  */
#define LL_TIM_SR_IERRF                        TIM_SR_IERRF         /*!< Index Error flag  */
#define LL_TIM_SR_TERRF                        TIM_SR_TERRF         /*!< Transition Error flag  */
/**
  * @}
  */

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup TIM_LL_EC_BREAK_ENABLE Break Enable
  * @{
  */
#define LL_TIM_BREAK_DISABLE            0x00000000U             /*!< Break function disabled */
#define LL_TIM_BREAK_ENABLE             TIM_BDTR_BKE            /*!< Break function enabled */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_BREAK2_ENABLE Break2 Enable
  * @{
  */
#define LL_TIM_BREAK2_DISABLE            0x00000000U              /*!< Break2 function disabled */
#define LL_TIM_BREAK2_ENABLE             TIM_BDTR_BK2E            /*!< Break2 function enabled */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_AUTOMATICOUTPUT_ENABLE Automatic output enable
  * @{
  */
#define LL_TIM_AUTOMATICOUTPUT_DISABLE         0x00000000U             /*!< MOE can be set only by software */
#define LL_TIM_AUTOMATICOUTPUT_ENABLE          TIM_BDTR_AOE            /*!< MOE can be set by software or automatically at the next update event */
/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/** @defgroup TIM_LL_EC_IT IT Defines
  * @brief    IT defines which can be used with LL_TIM_ReadReg and  LL_TIM_WriteReg functions.
  * @{
  */
#define LL_TIM_DIER_UIE                        TIM_DIER_UIE         /*!< Update interrupt enable */
#define LL_TIM_DIER_CC1IE                      TIM_DIER_CC1IE       /*!< Capture/compare 1 interrupt enable */
#define LL_TIM_DIER_CC2IE                      TIM_DIER_CC2IE       /*!< Capture/compare 2 interrupt enable */
#define LL_TIM_DIER_CC3IE                      TIM_DIER_CC3IE       /*!< Capture/compare 3 interrupt enable */
#define LL_TIM_DIER_CC4IE                      TIM_DIER_CC4IE       /*!< Capture/compare 4 interrupt enable */
#define LL_TIM_DIER_COMIE                      TIM_DIER_COMIE       /*!< COM interrupt enable */
#define LL_TIM_DIER_TIE                        TIM_DIER_TIE         /*!< Trigger interrupt enable */
#define LL_TIM_DIER_BIE                        TIM_DIER_BIE         /*!< Break interrupt enable */
#define LL_TIM_DIER_IDXIE                      TIM_DIER_IDXIE       /*!< Index interrupt enable */
#define LL_TIM_DIER_DIRIE                      TIM_DIER_DIRIE       /*!< Direction Change interrupt enable */
#define LL_TIM_DIER_IERRIE                     TIM_DIER_IERRIE      /*!< Index Error interrupt enable */
#define LL_TIM_DIER_TERRIE                     TIM_DIER_TERRIE      /*!< Transition Error interrupt enable */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_UPDATESOURCE Update Source
  * @{
  */
#define LL_TIM_UPDATESOURCE_REGULAR            0x00000000U          /*!< Counter overflow/underflow, Setting the UG bit or Update generation through the slave mode controller generates an update request */
#define LL_TIM_UPDATESOURCE_COUNTER            TIM_CR1_URS          /*!< Only counter overflow/underflow generates an update request */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_ONEPULSEMODE One Pulse Mode
  * @{
  */
#define LL_TIM_ONEPULSEMODE_SINGLE             TIM_CR1_OPM          /*!< Counter stops counting at the next update event */
#define LL_TIM_ONEPULSEMODE_REPETITIVE         0x00000000U          /*!< Counter is not stopped at update event */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_COUNTERMODE Counter Mode
  * @{
  */
#define LL_TIM_COUNTERMODE_UP                  0x00000000U          /*!< Counter used as upcounter */
#define LL_TIM_COUNTERMODE_DOWN                TIM_CR1_DIR          /*!< Counter used as downcounter */
#define LL_TIM_COUNTERMODE_CENTER_DOWN         TIM_CR1_CMS_0        /*!< The counter counts up and down alternatively. Output compare interrupt flags of output channels  are set only when the counter is counting down. */
#define LL_TIM_COUNTERMODE_CENTER_UP           TIM_CR1_CMS_1        /*!< The counter counts up and down alternatively. Output compare interrupt flags of output channels  are set only when the counter is counting up */
#define LL_TIM_COUNTERMODE_CENTER_UP_DOWN      TIM_CR1_CMS          /*!< The counter counts up and down alternatively. Output compare interrupt flags of output channels  are set only when the counter is counting up or down. */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_CLOCKDIVISION Clock Division
  * @{
  */
#define LL_TIM_CLOCKDIVISION_DIV1              0x00000000U          /*!< tDTS=tCK_INT */
#define LL_TIM_CLOCKDIVISION_DIV2              TIM_CR1_CKD_0        /*!< tDTS=2*tCK_INT */
#define LL_TIM_CLOCKDIVISION_DIV4              TIM_CR1_CKD_1        /*!< tDTS=4*tCK_INT */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_COUNTERDIRECTION Counter Direction
  * @{
  */
#define LL_TIM_COUNTERDIRECTION_UP             0x00000000U          /*!< Timer counter counts up */
#define LL_TIM_COUNTERDIRECTION_DOWN           TIM_CR1_DIR          /*!< Timer counter counts down */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_CCUPDATESOURCE Capture Compare  Update Source
  * @{
  */
#define LL_TIM_CCUPDATESOURCE_COMG_ONLY        0x00000000U          /*!< Capture/compare control bits are updated by setting the COMG bit only */
#define LL_TIM_CCUPDATESOURCE_COMG_AND_TRGI    TIM_CR2_CCUS         /*!< Capture/compare control bits are updated by setting the COMG bit or when a rising edge occurs on trigger input (TRGI) */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_CCDMAREQUEST Capture Compare DMA Request
  * @{
  */
#define LL_TIM_CCDMAREQUEST_CC                 0x00000000U          /*!< CCx DMA request sent when CCx event occurs */
#define LL_TIM_CCDMAREQUEST_UPDATE             TIM_CR2_CCDS         /*!< CCx DMA requests sent when update event occurs */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_LOCKLEVEL Lock Level
  * @{
  */
#define LL_TIM_LOCKLEVEL_OFF                   0x00000000U          /*!< LOCK OFF - No bit is write protected */
#define LL_TIM_LOCKLEVEL_1                     TIM_BDTR_LOCK_0      /*!< LOCK Level 1 */
#define LL_TIM_LOCKLEVEL_2                     TIM_BDTR_LOCK_1      /*!< LOCK Level 2 */
#define LL_TIM_LOCKLEVEL_3                     TIM_BDTR_LOCK        /*!< LOCK Level 3 */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_CHANNEL Channel
  * @{
  */
#define LL_TIM_CHANNEL_CH1                     TIM_CCER_CC1E     /*!< Timer input/output channel 1 */
#define LL_TIM_CHANNEL_CH1N                    TIM_CCER_CC1NE    /*!< Timer complementary output channel 1 */
#define LL_TIM_CHANNEL_CH2                     TIM_CCER_CC2E     /*!< Timer input/output channel 2 */
#define LL_TIM_CHANNEL_CH2N                    TIM_CCER_CC2NE    /*!< Timer complementary output channel 2 */
#define LL_TIM_CHANNEL_CH3                     TIM_CCER_CC3E     /*!< Timer input/output channel 3 */
#define LL_TIM_CHANNEL_CH3N                    TIM_CCER_CC3NE    /*!< Timer complementary output channel 3 */
#define LL_TIM_CHANNEL_CH4                     TIM_CCER_CC4E     /*!< Timer input/output channel 4 */
#define LL_TIM_CHANNEL_CH4N                    TIM_CCER_CC4NE     /*!< Timer complementary output channel 4 */
#define LL_TIM_CHANNEL_CH5                     TIM_CCER_CC5E     /*!< Timer output channel 5 */
#define LL_TIM_CHANNEL_CH6                     TIM_CCER_CC6E     /*!< Timer output channel 6 */
/**
  * @}
  */

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup TIM_LL_EC_OCSTATE Output Configuration State
  * @{
  */
#define LL_TIM_OCSTATE_DISABLE                 0x00000000U             /*!< OCx is not active */
#define LL_TIM_OCSTATE_ENABLE                  TIM_CCER_CC1E           /*!< OCx signal is output on the corresponding output pin */
/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/** Legacy definitions for compatibility purpose
@cond 0
  */
#define LL_TIM_OCMODE_ASSYMETRIC_PWM1 LL_TIM_OCMODE_ASYMMETRIC_PWM1
#define LL_TIM_OCMODE_ASSYMETRIC_PWM2 LL_TIM_OCMODE_ASYMMETRIC_PWM2
/**
@endcond
  */

/** @defgroup TIM_LL_EC_OCMODE Output Configuration Mode
  * @{
  */
#define LL_TIM_OCMODE_FROZEN                   0x00000000U                                              /*!<The comparison between the output compare register TIMx_CCRy and the counter TIMx_CNT has no effect on the output channel level */
#define LL_TIM_OCMODE_ACTIVE                   TIM_CCMR1_OC1M_0                                         /*!<OCyREF is forced high on compare match*/
#define LL_TIM_OCMODE_INACTIVE                 TIM_CCMR1_OC1M_1                                         /*!<OCyREF is forced low on compare match*/
#define LL_TIM_OCMODE_TOGGLE                   (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0)                    /*!<OCyREF toggles on compare match*/
#define LL_TIM_OCMODE_FORCED_INACTIVE          TIM_CCMR1_OC1M_2                                         /*!<OCyREF is forced low*/
#define LL_TIM_OCMODE_FORCED_ACTIVE            (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0)                    /*!<OCyREF is forced high*/
#define LL_TIM_OCMODE_PWM1                     (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1)                    /*!<In upcounting, channel y is active as long as TIMx_CNT<TIMx_CCRy else inactive.  In downcounting, channel y is inactive as long as TIMx_CNT>TIMx_CCRy else active.*/
#define LL_TIM_OCMODE_PWM2                     (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0) /*!<In upcounting, channel y is inactive as long as TIMx_CNT<TIMx_CCRy else active.  In downcounting, channel y is active as long as TIMx_CNT>TIMx_CCRy else inactive*/
#define LL_TIM_OCMODE_RETRIG_OPM1              TIM_CCMR1_OC1M_3                                         /*!<Retrigerrable OPM mode 1*/
#define LL_TIM_OCMODE_RETRIG_OPM2              (TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M_0)                    /*!<Retrigerrable OPM mode 2*/
#define LL_TIM_OCMODE_COMBINED_PWM1            (TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M_2)                    /*!<Combined PWM mode 1*/
#define LL_TIM_OCMODE_COMBINED_PWM2            (TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_2) /*!<Combined PWM mode 2*/
#define LL_TIM_OCMODE_ASYMMETRIC_PWM1          (TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2) /*!<Asymmetric PWM mode 1*/
#define LL_TIM_OCMODE_ASYMMETRIC_PWM2          (TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M)                      /*!<Asymmetric PWM mode 2*/
#define LL_TIM_OCMODE_PULSE_ON_COMPARE         (TIM_CCMR2_OC3M_3 | TIM_CCMR2_OC3M_1)                    /*!<Pulse on Compare mode */
#define LL_TIM_OCMODE_DIRECTION_OUTPUT         (TIM_CCMR2_OC3M_3 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0) /*!<Direction output mode */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_OCPOLARITY Output Configuration Polarity
  * @{
  */
#define LL_TIM_OCPOLARITY_HIGH                 0x00000000U                 /*!< OCxactive high*/
#define LL_TIM_OCPOLARITY_LOW                  TIM_CCER_CC1P               /*!< OCxactive low*/
/**
  * @}
  */

/** @defgroup TIM_LL_EC_OCIDLESTATE Output Configuration Idle State
  * @{
  */
#define LL_TIM_OCIDLESTATE_LOW                 0x00000000U             /*!<OCx=0 (after a dead-time if OC is implemented) when MOE=0*/
#define LL_TIM_OCIDLESTATE_HIGH                TIM_CR2_OIS1            /*!<OCx=1 (after a dead-time if OC is implemented) when MOE=0*/
/**
  * @}
  */

/** @defgroup TIM_LL_EC_GROUPCH5 GROUPCH5
  * @{
  */
#define LL_TIM_GROUPCH5_NONE                   0x00000000U           /*!< No effect of OC5REF on OC1REFC, OC2REFC and OC3REFC */
#define LL_TIM_GROUPCH5_OC1REFC                TIM_CCR5_GC5C1        /*!< OC1REFC is the logical AND of OC1REFC and OC5REF */
#define LL_TIM_GROUPCH5_OC2REFC                TIM_CCR5_GC5C2        /*!< OC2REFC is the logical AND of OC2REFC and OC5REF */
#define LL_TIM_GROUPCH5_OC3REFC                TIM_CCR5_GC5C3        /*!< OC3REFC is the logical AND of OC3REFC and OC5REF */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_ACTIVEINPUT Active Input Selection
  * @{
  */
#define LL_TIM_ACTIVEINPUT_DIRECTTI            (TIM_CCMR1_CC1S_0 << 16U) /*!< ICx is mapped on TIx */
#define LL_TIM_ACTIVEINPUT_INDIRECTTI          (TIM_CCMR1_CC1S_1 << 16U) /*!< ICx is mapped on TIy */
#define LL_TIM_ACTIVEINPUT_TRC                 (TIM_CCMR1_CC1S << 16U)   /*!< ICx is mapped on TRC */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_ICPSC Input Configuration Prescaler
  * @{
  */
#define LL_TIM_ICPSC_DIV1                      0x00000000U                    /*!< No prescaler, capture is done each time an edge is detected on the capture input */
#define LL_TIM_ICPSC_DIV2                      (TIM_CCMR1_IC1PSC_0 << 16U)    /*!< Capture is done once every 2 events */
#define LL_TIM_ICPSC_DIV4                      (TIM_CCMR1_IC1PSC_1 << 16U)    /*!< Capture is done once every 4 events */
#define LL_TIM_ICPSC_DIV8                      (TIM_CCMR1_IC1PSC << 16U)      /*!< Capture is done once every 8 events */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_IC_FILTER Input Configuration Filter
  * @{
  */
#define LL_TIM_IC_FILTER_FDIV1                 0x00000000U                                                        /*!< No filter, sampling is done at fDTS */
#define LL_TIM_IC_FILTER_FDIV1_N2              (TIM_CCMR1_IC1F_0 << 16U)                                          /*!< fSAMPLING=fCK_INT, N=2 */
#define LL_TIM_IC_FILTER_FDIV1_N4              (TIM_CCMR1_IC1F_1 << 16U)                                          /*!< fSAMPLING=fCK_INT, N=4 */
#define LL_TIM_IC_FILTER_FDIV1_N8              ((TIM_CCMR1_IC1F_1 | TIM_CCMR1_IC1F_0) << 16U)                     /*!< fSAMPLING=fCK_INT, N=8 */
#define LL_TIM_IC_FILTER_FDIV2_N6              (TIM_CCMR1_IC1F_2 << 16U)                                          /*!< fSAMPLING=fDTS/2, N=6 */
#define LL_TIM_IC_FILTER_FDIV2_N8              ((TIM_CCMR1_IC1F_2 | TIM_CCMR1_IC1F_0) << 16U)                     /*!< fSAMPLING=fDTS/2, N=8 */
#define LL_TIM_IC_FILTER_FDIV4_N6              ((TIM_CCMR1_IC1F_2 | TIM_CCMR1_IC1F_1) << 16U)                     /*!< fSAMPLING=fDTS/4, N=6 */
#define LL_TIM_IC_FILTER_FDIV4_N8              ((TIM_CCMR1_IC1F_2 | TIM_CCMR1_IC1F_1 | TIM_CCMR1_IC1F_0) << 16U)  /*!< fSAMPLING=fDTS/4, N=8 */
#define LL_TIM_IC_FILTER_FDIV8_N6              (TIM_CCMR1_IC1F_3 << 16U)                                          /*!< fSAMPLING=fDTS/8, N=6 */
#define LL_TIM_IC_FILTER_FDIV8_N8              ((TIM_CCMR1_IC1F_3 | TIM_CCMR1_IC1F_0) << 16U)                     /*!< fSAMPLING=fDTS/8, N=8 */
#define LL_TIM_IC_FILTER_FDIV16_N5             ((TIM_CCMR1_IC1F_3 | TIM_CCMR1_IC1F_1) << 16U)                     /*!< fSAMPLING=fDTS/16, N=5 */
#define LL_TIM_IC_FILTER_FDIV16_N6             ((TIM_CCMR1_IC1F_3 | TIM_CCMR1_IC1F_1 | TIM_CCMR1_IC1F_0) << 16U)  /*!< fSAMPLING=fDTS/16, N=6 */
#define LL_TIM_IC_FILTER_FDIV16_N8             ((TIM_CCMR1_IC1F_3 | TIM_CCMR1_IC1F_2) << 16U)                     /*!< fSAMPLING=fDTS/16, N=8 */
#define LL_TIM_IC_FILTER_FDIV32_N5             ((TIM_CCMR1_IC1F_3 | TIM_CCMR1_IC1F_2 | TIM_CCMR1_IC1F_0) << 16U)  /*!< fSAMPLING=fDTS/32, N=5 */
#define LL_TIM_IC_FILTER_FDIV32_N6             ((TIM_CCMR1_IC1F_3 | TIM_CCMR1_IC1F_2 | TIM_CCMR1_IC1F_1) << 16U)  /*!< fSAMPLING=fDTS/32, N=6 */
#define LL_TIM_IC_FILTER_FDIV32_N8             (TIM_CCMR1_IC1F << 16U)                                            /*!< fSAMPLING=fDTS/32, N=8 */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_IC_POLARITY Input Configuration Polarity
  * @{
  */
#define LL_TIM_IC_POLARITY_RISING              0x00000000U                      /*!< The circuit is sensitive to TIxFP1 rising edge, TIxFP1 is not inverted */
#define LL_TIM_IC_POLARITY_FALLING             TIM_CCER_CC1P                    /*!< The circuit is sensitive to TIxFP1 falling edge, TIxFP1 is inverted */
#define LL_TIM_IC_POLARITY_BOTHEDGE            (TIM_CCER_CC1P | TIM_CCER_CC1NP) /*!< The circuit is sensitive to both TIxFP1 rising and falling edges, TIxFP1 is not inverted */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_CLOCKSOURCE Clock Source
  * @{
  */
#define LL_TIM_CLOCKSOURCE_INTERNAL            0x00000000U                                          /*!< The timer is clocked by the internal clock provided from the RCC */
#define LL_TIM_CLOCKSOURCE_EXT_MODE1           (TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0)   /*!< Counter counts at each rising or falling edge on a selected input*/
#define LL_TIM_CLOCKSOURCE_EXT_MODE2           TIM_SMCR_ECE                                         /*!< Counter counts at each rising or falling edge on the external trigger input ETR */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_ENCODERMODE Encoder Mode
  * @{
  */
#define LL_TIM_ENCODERMODE_X2_TI1                     TIM_SMCR_SMS_0                                                     /*!< Quadrature encoder mode 1, x2 mode - Counter counts up/down on TI1FP1 edge depending on TI2FP2 level */
#define LL_TIM_ENCODERMODE_X2_TI2                     TIM_SMCR_SMS_1                                                     /*!< Quadrature encoder mode 2, x2 mode - Counter counts up/down on TI2FP2 edge depending on TI1FP1 level */
#define LL_TIM_ENCODERMODE_X4_TI12                   (TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0)                                   /*!< Quadrature encoder mode 3, x4 mode - Counter counts up/down on both TI1FP1 and TI2FP2 edges depending on the level of the other input */
#define LL_TIM_ENCODERMODE_CLOCKPLUSDIRECTION_X2     (TIM_SMCR_SMS_3 | TIM_SMCR_SMS_1)                                   /*!< Encoder mode: Clock plus direction - x2 mode */
#define LL_TIM_ENCODERMODE_CLOCKPLUSDIRECTION_X1     (TIM_SMCR_SMS_3 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0)                  /*!< Encoder mode: Clock plus direction, x1 mode, TI2FP2 edge sensitivity is set by CC2P */
#define LL_TIM_ENCODERMODE_DIRECTIONALCLOCK_X2       (TIM_SMCR_SMS_3 | TIM_SMCR_SMS_2)                                   /*!< Encoder mode: Directional Clock, x2 mode */
#define LL_TIM_ENCODERMODE_DIRECTIONALCLOCK_X1_TI12  (TIM_SMCR_SMS_3 | TIM_SMCR_SMS_2 | TIM_SMCR_SMS_0)                  /*!< Encoder mode: Directional Clock, x1 mode, TI1FP1 and TI2FP2 edge sensitivity is set by CC1P and CC2P */
#define LL_TIM_ENCODERMODE_X1_TI1                    (TIM_SMCR_SMS_3 | TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1)                  /*!< Quadrature encoder mode: x1 mode, counting on TI1FP1 edges only, edge sensitivity is set by CC1P */
#define LL_TIM_ENCODERMODE_X1_TI2                    (TIM_SMCR_SMS_3 | TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0) /*!< Quadrature encoder mode: x1 mode, counting on TI2FP2 edges only, edge sensitivity is set by CC1P */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TRGO Trigger Output
  * @{
  */
#define LL_TIM_TRGO_RESET                      0x00000000U                                     /*!< UG bit from the TIMx_EGR register is used as trigger output */
#define LL_TIM_TRGO_ENABLE                     TIM_CR2_MMS_0                                   /*!< Counter Enable signal (CNT_EN) is used as trigger output */
#define LL_TIM_TRGO_UPDATE                     TIM_CR2_MMS_1                                   /*!< Update event is used as trigger output */
#define LL_TIM_TRGO_CC1IF                      (TIM_CR2_MMS_1 | TIM_CR2_MMS_0)                 /*!< CC1 capture or a compare match is used as trigger output */
#define LL_TIM_TRGO_OC1REF                     TIM_CR2_MMS_2                                   /*!< OC1REF signal is used as trigger output */
#define LL_TIM_TRGO_OC2REF                     (TIM_CR2_MMS_2 | TIM_CR2_MMS_0)                 /*!< OC2REF signal is used as trigger output */
#define LL_TIM_TRGO_OC3REF                     (TIM_CR2_MMS_2 | TIM_CR2_MMS_1)                 /*!< OC3REF signal is used as trigger output */
#define LL_TIM_TRGO_OC4REF                     (TIM_CR2_MMS_2 | TIM_CR2_MMS_1 | TIM_CR2_MMS_0) /*!< OC4REF signal is used as trigger output */
#define LL_TIM_TRGO_ENCODERCLK                 TIM_CR2_MMS_3                                   /*!< Encoder clock signal is used as trigger output */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TRGO2 Trigger Output 2
  * @{
  */
#define LL_TIM_TRGO2_RESET                     0x00000000U                                                         /*!< UG bit from the TIMx_EGR register is used as trigger output 2 */
#define LL_TIM_TRGO2_ENABLE                    TIM_CR2_MMS2_0                                                      /*!< Counter Enable signal (CNT_EN) is used as trigger output 2 */
#define LL_TIM_TRGO2_UPDATE                    TIM_CR2_MMS2_1                                                      /*!< Update event is used as trigger output 2 */
#define LL_TIM_TRGO2_CC1F                      (TIM_CR2_MMS2_1 | TIM_CR2_MMS2_0)                                   /*!< CC1 capture or a compare match is used as trigger output 2 */
#define LL_TIM_TRGO2_OC1                       TIM_CR2_MMS2_2                                                      /*!< OC1REF signal is used as trigger output 2 */
#define LL_TIM_TRGO2_OC2                       (TIM_CR2_MMS2_2 | TIM_CR2_MMS2_0)                                   /*!< OC2REF signal is used as trigger output 2 */
#define LL_TIM_TRGO2_OC3                       (TIM_CR2_MMS2_2 | TIM_CR2_MMS2_1)                                   /*!< OC3REF signal is used as trigger output 2 */
#define LL_TIM_TRGO2_OC4                       (TIM_CR2_MMS2_2 | TIM_CR2_MMS2_1 | TIM_CR2_MMS2_0)                  /*!< OC4REF signal is used as trigger output 2 */
#define LL_TIM_TRGO2_OC5                       TIM_CR2_MMS2_3                                                      /*!< OC5REF signal is used as trigger output 2 */
#define LL_TIM_TRGO2_OC6                       (TIM_CR2_MMS2_3 | TIM_CR2_MMS2_0)                                   /*!< OC6REF signal is used as trigger output 2 */
#define LL_TIM_TRGO2_OC4_RISINGFALLING         (TIM_CR2_MMS2_3 | TIM_CR2_MMS2_1)                                   /*!< OC4REF rising or falling edges are used as trigger output 2 */
#define LL_TIM_TRGO2_OC6_RISINGFALLING         (TIM_CR2_MMS2_3 | TIM_CR2_MMS2_1 | TIM_CR2_MMS2_0)                  /*!< OC6REF rising or falling edges are used as trigger output 2 */
#define LL_TIM_TRGO2_OC4_RISING_OC6_RISING     (TIM_CR2_MMS2_3 | TIM_CR2_MMS2_2)                                   /*!< OC4REF or OC6REF rising edges are used as trigger output 2 */
#define LL_TIM_TRGO2_OC4_RISING_OC6_FALLING    (TIM_CR2_MMS2_3 | TIM_CR2_MMS2_2 | TIM_CR2_MMS2_0)                  /*!< OC4REF rising or OC6REF falling edges are used as trigger output 2 */
#define LL_TIM_TRGO2_OC5_RISING_OC6_RISING     (TIM_CR2_MMS2_3 | TIM_CR2_MMS2_2 |TIM_CR2_MMS2_1)                   /*!< OC5REF or OC6REF rising edges are used as trigger output 2 */
#define LL_TIM_TRGO2_OC5_RISING_OC6_FALLING    (TIM_CR2_MMS2_3 | TIM_CR2_MMS2_2 | TIM_CR2_MMS2_1 | TIM_CR2_MMS2_0) /*!< OC5REF rising or OC6REF falling edges are used as trigger output 2 */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_SLAVEMODE Slave Mode
  * @{
  */
#define LL_TIM_SLAVEMODE_DISABLED              0x00000000U                         /*!< Slave mode disabled */
#define LL_TIM_SLAVEMODE_RESET                 TIM_SMCR_SMS_2                      /*!< Reset Mode - Rising edge of the selected trigger input (TRGI) reinitializes the counter */
#define LL_TIM_SLAVEMODE_GATED                 (TIM_SMCR_SMS_2 | TIM_SMCR_SMS_0)   /*!< Gated Mode - The counter clock is enabled when the trigger input (TRGI) is high */
#define LL_TIM_SLAVEMODE_TRIGGER               (TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1)   /*!< Trigger Mode - The counter starts at a rising edge of the trigger TRGI */
#define LL_TIM_SLAVEMODE_COMBINED_RESETTRIGGER TIM_SMCR_SMS_3                      /*!< Combined reset + trigger mode - Rising edge of the selected trigger input (TRGI)  reinitializes the counter, generates an update of the registers and starts the counter */
#define LL_TIM_SLAVEMODE_COMBINED_GATEDRESET   (TIM_SMCR_SMS_3 | TIM_SMCR_SMS_0)   /*!< Combined gated + reset mode - The counter clock is enabled when the trigger input (TRGI) is high. The counter stops and is reset) as soon as the trigger becomes low.Both startand stop of
                                                                                        the counter are controlled. */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_SMS_PRELOAD_SOURCE SMS Preload Source
  * @{
  */
#define LL_TIM_SMSPS_TIMUPDATE                 0x00000000U                         /*!< The SMS preload transfer is triggered by the Timer's Update event */
#define LL_TIM_SMSPS_INDEX                     TIM_SMCR_SMSPS                      /*!< The SMS preload transfer is triggered by the Index event */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TS Trigger Selection
  * @{
  */
#define LL_TIM_TS_ITR0                         0x00000000U                                                     /*!< Internal Trigger 0 (ITR0) is used as trigger input */
#define LL_TIM_TS_ITR1                         TIM_SMCR_TS_0                                                   /*!< Internal Trigger 1 (ITR1) is used as trigger input */
#define LL_TIM_TS_ITR2                         TIM_SMCR_TS_1                                                   /*!< Internal Trigger 2 (ITR2) is used as trigger input */
#define LL_TIM_TS_ITR3                         (TIM_SMCR_TS_0 | TIM_SMCR_TS_1)                                 /*!< Internal Trigger 3 (ITR3) is used as trigger input */
#define LL_TIM_TS_ITR4                         (TIM_SMCR_TS_3)                                                 /*!< Internal Trigger 4 (ITR4) is used as trigger input */
#define LL_TIM_TS_ITR5                         (TIM_SMCR_TS_3 | TIM_SMCR_TS_0)                                 /*!< Internal Trigger 5 (ITR5) is used as trigger input */
#define LL_TIM_TS_ITR6                         (TIM_SMCR_TS_3 | TIM_SMCR_TS_1)                                 /*!< Internal Trigger 6 (ITR6) is used as trigger input */
#define LL_TIM_TS_ITR7                         (TIM_SMCR_TS_3 | TIM_SMCR_TS_1 | TIM_SMCR_TS_0)                 /*!< Internal Trigger 7 (ITR7) is used as trigger input */
#define LL_TIM_TS_ITR8                         (TIM_SMCR_TS_3 | TIM_SMCR_TS_2)                                 /*!< Internal Trigger 8 (ITR8) is used as trigger input */
#define LL_TIM_TS_ITR9                         (TIM_SMCR_TS_3 | TIM_SMCR_TS_2 | TIM_SMCR_TS_0)                 /*!< Internal Trigger 9 (ITR9) is used as trigger input */
#define LL_TIM_TS_ITR10                        (TIM_SMCR_TS_3 | TIM_SMCR_TS_2 | TIM_SMCR_TS_1)                 /*!< Internal Trigger 10 (ITR10) is used as trigger input */
#define LL_TIM_TS_ITR11                        (TIM_SMCR_TS_3 | TIM_SMCR_TS_2 | TIM_SMCR_TS_1 | TIM_SMCR_TS_0) /*!< Internal Trigger 11 (ITR11) is used as trigger input */
#define LL_TIM_TS_ITR12                        (TIM_SMCR_TS_4)                                                 /*!< Internal Trigger 12 (ITR12) is used as trigger input */
#define LL_TIM_TS_ITR13                        (TIM_SMCR_TS_4 | TIM_SMCR_TS_0)                                 /*!< Internal Trigger 13 (ITR13) is used as trigger input */
#define LL_TIM_TS_TI1F_ED                      TIM_SMCR_TS_2                                                   /*!< TI1 Edge Detector (TI1F_ED) is used as trigger input */
#define LL_TIM_TS_TI1FP1                       (TIM_SMCR_TS_2 | TIM_SMCR_TS_0)                                 /*!< Filtered Timer Input 1 (TI1FP1) is used as trigger input */
#define LL_TIM_TS_TI2FP2                       (TIM_SMCR_TS_2 | TIM_SMCR_TS_1)                                 /*!< Filtered Timer Input 2 (TI12P2) is used as trigger input */
#define LL_TIM_TS_ETRF                         (TIM_SMCR_TS_2 | TIM_SMCR_TS_1 | TIM_SMCR_TS_0)                 /*!< Filtered external Trigger (ETRF) is used as trigger input */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_ETR_POLARITY External Trigger Polarity
  * @{
  */
#define LL_TIM_ETR_POLARITY_NONINVERTED        0x00000000U             /*!< ETR is non-inverted, active at high level or rising edge */
#define LL_TIM_ETR_POLARITY_INVERTED           TIM_SMCR_ETP            /*!< ETR is inverted, active at low level or falling edge */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_ETR_PRESCALER External Trigger Prescaler
  * @{
  */
#define LL_TIM_ETR_PRESCALER_DIV1              0x00000000U             /*!< ETR prescaler OFF */
#define LL_TIM_ETR_PRESCALER_DIV2              TIM_SMCR_ETPS_0         /*!< ETR frequency is divided by 2 */
#define LL_TIM_ETR_PRESCALER_DIV4              TIM_SMCR_ETPS_1         /*!< ETR frequency is divided by 4 */
#define LL_TIM_ETR_PRESCALER_DIV8              TIM_SMCR_ETPS           /*!< ETR frequency is divided by 8 */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_ETR_FILTER External Trigger Filter
  * @{
  */
#define LL_TIM_ETR_FILTER_FDIV1                0x00000000U                                          /*!< No filter, sampling is done at fDTS */
#define LL_TIM_ETR_FILTER_FDIV1_N2             TIM_SMCR_ETF_0                                       /*!< fSAMPLING=fCK_INT, N=2 */
#define LL_TIM_ETR_FILTER_FDIV1_N4             TIM_SMCR_ETF_1                                       /*!< fSAMPLING=fCK_INT, N=4 */
#define LL_TIM_ETR_FILTER_FDIV1_N8             (TIM_SMCR_ETF_1 | TIM_SMCR_ETF_0)                    /*!< fSAMPLING=fCK_INT, N=8 */
#define LL_TIM_ETR_FILTER_FDIV2_N6             TIM_SMCR_ETF_2                                       /*!< fSAMPLING=fDTS/2, N=6 */
#define LL_TIM_ETR_FILTER_FDIV2_N8             (TIM_SMCR_ETF_2 | TIM_SMCR_ETF_0)                    /*!< fSAMPLING=fDTS/2, N=8 */
#define LL_TIM_ETR_FILTER_FDIV4_N6             (TIM_SMCR_ETF_2 | TIM_SMCR_ETF_1)                    /*!< fSAMPLING=fDTS/4, N=6 */
#define LL_TIM_ETR_FILTER_FDIV4_N8             (TIM_SMCR_ETF_2 | TIM_SMCR_ETF_1 | TIM_SMCR_ETF_0)   /*!< fSAMPLING=fDTS/4, N=8 */
#define LL_TIM_ETR_FILTER_FDIV8_N6             TIM_SMCR_ETF_3                                       /*!< fSAMPLING=fDTS/8, N=6 */
#define LL_TIM_ETR_FILTER_FDIV8_N8             (TIM_SMCR_ETF_3 | TIM_SMCR_ETF_0)                    /*!< fSAMPLING=fDTS/16, N=8 */
#define LL_TIM_ETR_FILTER_FDIV16_N5            (TIM_SMCR_ETF_3 | TIM_SMCR_ETF_1)                    /*!< fSAMPLING=fDTS/16, N=5 */
#define LL_TIM_ETR_FILTER_FDIV16_N6            (TIM_SMCR_ETF_3 | TIM_SMCR_ETF_1 | TIM_SMCR_ETF_0)   /*!< fSAMPLING=fDTS/16, N=6 */
#define LL_TIM_ETR_FILTER_FDIV16_N8            (TIM_SMCR_ETF_3 | TIM_SMCR_ETF_2)                    /*!< fSAMPLING=fDTS/16, N=8 */
#define LL_TIM_ETR_FILTER_FDIV32_N5            (TIM_SMCR_ETF_3 | TIM_SMCR_ETF_2 | TIM_SMCR_ETF_0)   /*!< fSAMPLING=fDTS/32, N=5 */
#define LL_TIM_ETR_FILTER_FDIV32_N6            (TIM_SMCR_ETF_3 | TIM_SMCR_ETF_2 | TIM_SMCR_ETF_1)   /*!< fSAMPLING=fDTS/32, N=6 */
#define LL_TIM_ETR_FILTER_FDIV32_N8            TIM_SMCR_ETF                                         /*!< fSAMPLING=fDTS/32, N=8 */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM1_ETRSOURCE External Trigger Source TIM1
  * @{
  */
#define LL_TIM_TIM1_ETRSOURCE_GPIO           0x00000000UL                                                /*!< TIM1_ETR is connected to I/O */
#define LL_TIM_TIM1_ETRSOURCE_ADC1_AWD1      (TIM_AF1_ETRSEL_1 | TIM_AF1_ETRSEL_0)                       /*!< TIM1_ETR is connected to ADC1 AWD1 */
#define LL_TIM_TIM1_ETRSOURCE_ADC1_AWD2      TIM_AF1_ETRSEL_2                                            /*!< TIM1_ETR is connected to ADC1 AWD2 */
#define LL_TIM_TIM1_ETRSOURCE_ADC1_AWD3      (TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_0)                       /*!< TIM1_ETR is connected to ADC1 AWD3 */
#define LL_TIM_TIM1_ETRSOURCE_ADC2_AWD1      (TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_1)                       /*!< TIM1_ETR is connected to ADC2 AWD1 */
#define LL_TIM_TIM1_ETRSOURCE_ADC2_AWD2      (TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_1 | TIM_AF1_ETRSEL_0)    /*!< TIM1_ETR is connected to ADC2 AWD2 */
#define LL_TIM_TIM1_ETRSOURCE_ADC2_AWD3      TIM_AF1_ETRSEL_3                                            /*!< TIM1_ETR is connected to ADC2 AWD3 */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM2_ETRSOURCE External Trigger Source TIM2
  * @{
  */
#define LL_TIM_TIM2_ETRSOURCE_GPIO          0x00000000UL                                                                 /*!< TIM2_ETR is connected to I/O */
#define LL_TIM_TIM2_ETRSOURCE_DCMIPP_HSYNC  TIM_AF1_ETRSEL_0                                                             /*!< TIM2_ETR is connected to DCMIPP HSYNC */
#define LL_TIM_TIM2_ETRSOURCE_LCD_HSYNC     TIM_AF1_ETRSEL_1                                                             /*!< TIM2_ETR is connected to LCD HSYNC      */
#define LL_TIM_TIM2_ETRSOURCE_SAI1_FSA      TIM_AF1_ETRSEL_2                                                             /*!< TIM2_ETR is connected to SAI1 FS_A      */
#define LL_TIM_TIM2_ETRSOURCE_SAI1_FSB      (TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_0)                                        /*!< TIM2_ETR is connected to SAI1 FS_B */
#define LL_TIM_TIM2_ETRSOURCE_GFXTIM_TE     (TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_1)                                        /*!< TIM2_ETR is connected to GFXTIM TE */
#define LL_TIM_TIM2_ETRSOURCE_DCMIPP_VSYNC  (TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_1 | TIM_AF1_ETRSEL_0)                     /*!< TIM2_ETR is connected to DCMIPP VSYNC */
#define LL_TIM_TIM2_ETRSOURCE_LCD_VSYNC     TIM_AF1_ETRSEL_3                                                             /*!< TIM2_ETR is connected to LCD VSYNC */
#define LL_TIM_TIM2_ETRSOURCE_TIM3_ETR      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_1)                                        /*!< TIM2_ETR is connected to TIM3 ETR */
#define LL_TIM_TIM2_ETRSOURCE_TIM4_ETR      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_1 | TIM_AF1_ETRSEL_0)                     /*!< TIM2_ETR is connected to TIM4 ETR */
#define LL_TIM_TIM2_ETRSOURCE_TIM5_ETR      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_2)                                        /*!< TIM2_ETR is connected to TIM5 ETR */
#define LL_TIM_TIM2_ETRSOURCE_ETH1_PPS      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_0)                     /*!< TIM2_ETR is connected to ETH1 PPS */
#define LL_TIM_TIM2_ETRSOURCE_USB1_SOF      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_1)                     /*!< TIM2_ETR is connected to USB1 OTG SOF */
#define LL_TIM_TIM2_ETRSOURCE_USB2_SOF      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_1 | TIM_AF1_ETRSEL_0)  /*!< TIM2_ETR is connected to USB2 OTG SOF */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM3_ETRSOURCE External Trigger Source TIM3
  * @{
  */
#define LL_TIM_TIM3_ETRSOURCE_GPIO          0x00000000UL                                                /*!< TIM3_ETR is connected to I/O */
#define LL_TIM_TIM3_ETRSOURCE_DCMIPP_HSYNC  TIM_AF1_ETRSEL_0                                            /*!< TIM3_ETR is connected to DCMIPP HSYNC */
#define LL_TIM_TIM3_ETRSOURCE_LCD_HSYNC     TIM_AF1_ETRSEL_1                                            /*!< TIM3_ETR is connected to LCD HSYNC */
#define LL_TIM_TIM3_ETRSOURCE_GFXTIM_TE     (TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_1)                       /*!< TIM3_ETR is connected to GFXTIM TE */
#define LL_TIM_TIM3_ETRSOURCE_DCMIPP_VSYNC  (TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_1 | TIM_AF1_ETRSEL_0)    /*!< TIM3_ETR is connected to DCMIPP VSYNC */
#define LL_TIM_TIM3_ETRSOURCE_LCD_VSYNC     TIM_AF1_ETRSEL_3                                            /*!< TIM3_ETR is connected to LCD VSYNC */
#define LL_TIM_TIM3_ETRSOURCE_TIM2_ETR      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_0)                       /*!< TIM3_ETR is connected to TIM2 ETR */
#define LL_TIM_TIM3_ETRSOURCE_TIM4_ETR      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_1 | TIM_AF1_ETRSEL_0)    /*!< TIM3_ETR is connected to TIM4 ETR */
#define LL_TIM_TIM3_ETRSOURCE_TIM5_ETR      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_2)                       /*!< TIM3_ETR is connected to TIM5 ETR */
#define LL_TIM_TIM3_ETRSOURCE_ETH1_PPS      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_0)    /*!< TIM3_ETR is connected to ETH1 PPS */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM4_ETRSOURCE External Trigger Source TIM4
  * @{
  */
#define LL_TIM_TIM4_ETRSOURCE_GPIO          0x00000000UL                                                /*!< TIM4_ETR is connected to I/O */
#define LL_TIM_TIM4_ETRSOURCE_DCMIPP_HSYNC  TIM_AF1_ETRSEL_0                                            /*!< TIM4_ETR is connected to DCMIPP HSYNC */
#define LL_TIM_TIM4_ETRSOURCE_LCD_HSYNC     TIM_AF1_ETRSEL_1                                            /*!< TIM4_ETR is connected to LCD HSYNC */
#define LL_TIM_TIM4_ETRSOURCE_GFXTIM_TE     (TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_1)                       /*!< TIM4_ETR is connected to GFXTIM TE */
#define LL_TIM_TIM4_ETRSOURCE_DCMIPP_VSYNC  (TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_1 | TIM_AF1_ETRSEL_0)    /*!< TIM4_ETR is connected to DCMIPP VSYNC */
#define LL_TIM_TIM4_ETRSOURCE_LCD_VSYNC     TIM_AF1_ETRSEL_3                                            /*!< TIM4_ETR is connected to LCD VSYNC */
#define LL_TIM_TIM4_ETRSOURCE_TIM2_ETR      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_0)                       /*!< TIM4_ETR is connected to TIM2 ETR */
#define LL_TIM_TIM4_ETRSOURCE_TIM3_ETR      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_1)                       /*!< TIM4_ETR is connected to TIM3 ETR */
#define LL_TIM_TIM4_ETRSOURCE_TIM5_ETR      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_2)                       /*!< TIM4_ETR is connected to TIM5 ETR */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM5_ETRSOURCE External Trigger Source TIM5
  * @{
  */
#define LL_TIM_TIM5_ETRSOURCE_GPIO          0x00000000UL                                                                 /*!< TIM5_ETR is connected to I/O */
#define LL_TIM_TIM5_ETRSOURCE_SAI2_FSA      TIM_AF1_ETRSEL_0                                                             /*!< TIM5_ETR is connected to SAI2 FS_A */
#define LL_TIM_TIM5_ETRSOURCE_SAI2_FSB      TIM_AF1_ETRSEL_1                                                             /*!< TIM5_ETR is connected to SAI2 FS_B */
#define LL_TIM_TIM5_ETRSOURCE_DCMIPP_HSYNC  (TIM_AF1_ETRSEL_1 | TIM_AF1_ETRSEL_0)                                        /*!< TIM5_ETR is connected to DCMIPP HSYNC */
#define LL_TIM_TIM5_ETRSOURCE_LCD_HSYNC     TIM_AF1_ETRSEL_2                                                             /*!< TIM5_ETR is connected to LCD HSYNC */
#define LL_TIM_TIM5_ETRSOURCE_GFXTIM_TE     (TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_1)                                        /*!< TIM5_ETR is connected to GFXTIM TE */
#define LL_TIM_TIM5_ETRSOURCE_DCMIPP_VSYNC  (TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_1 | TIM_AF1_ETRSEL_0)                     /*!< TIM5_ETR is connected to DCMIPP VSYNC */
#define LL_TIM_TIM5_ETRSOURCE_LCD_VSYNC     TIM_AF1_ETRSEL_3                                                             /*!< TIM5_ETR is connected to LCD VSYNC */
#define LL_TIM_TIM5_ETRSOURCE_TIM2_ETR      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_0)                                        /*!< TIM5_ETR is connected to TIM2 ETR */
#define LL_TIM_TIM5_ETRSOURCE_TIM3_ETR      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_1)                                        /*!< TIM5_ETR is connected to TIM3 ETR */
#define LL_TIM_TIM5_ETRSOURCE_TIM4_ETR      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_1 | TIM_AF1_ETRSEL_0)                     /*!< TIM5_ETR is connected to TIM5 ETR */
#define LL_TIM_TIM5_ETRSOURCE_USB1_SOF      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_1)                     /*!< TIM5_ETR is connected to USB1 OTG SOF */
#define LL_TIM_TIM5_ETRSOURCE_USB2_SOF      (TIM_AF1_ETRSEL_3 | TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_1 | TIM_AF1_ETRSEL_0)  /*!< TIM5_ETR is connected to USB2 OTG SOF */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM8_ETRSOURCE External Trigger Source TIM8
  * @{
  */
#define LL_TIM_TIM8_ETRSOURCE_GPIO           0x00000000UL                                                /*!< TIM8_ETR is connected to I/O */
#define LL_TIM_TIM8_ETRSOURCE_ADC2_AWD1      (TIM_AF1_ETRSEL_1 | TIM_AF1_ETRSEL_0)                       /*!< TIM8_ETR is connected to ADC2 AWD1 */
#define LL_TIM_TIM8_ETRSOURCE_ADC2_AWD2      TIM_AF1_ETRSEL_2                                            /*!< TIM8_ETR is connected to ADC2 AWD2 */
#define LL_TIM_TIM8_ETRSOURCE_ADC2_AWD3      (TIM_AF1_ETRSEL_2 | TIM_AF1_ETRSEL_0)                       /*!< TIM8_ETR is connected to ADC2 AWD3 */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_BREAK_POLARITY break polarity
  * @{
  */
#define LL_TIM_BREAK_POLARITY_LOW              0x00000000U               /*!< Break input BRK is active low */
#define LL_TIM_BREAK_POLARITY_HIGH             TIM_BDTR_BKP              /*!< Break input BRK is active high */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_BREAK_FILTER break filter
  * @{
  */
#define LL_TIM_BREAK_FILTER_FDIV1              0x00000000U   /*!< No filter, BRK acts asynchronously */
#define LL_TIM_BREAK_FILTER_FDIV1_N2           0x00010000U   /*!< fSAMPLING=fCK_INT, N=2 */
#define LL_TIM_BREAK_FILTER_FDIV1_N4           0x00020000U   /*!< fSAMPLING=fCK_INT, N=4 */
#define LL_TIM_BREAK_FILTER_FDIV1_N8           0x00030000U   /*!< fSAMPLING=fCK_INT, N=8 */
#define LL_TIM_BREAK_FILTER_FDIV2_N6           0x00040000U   /*!< fSAMPLING=fDTS/2, N=6 */
#define LL_TIM_BREAK_FILTER_FDIV2_N8           0x00050000U   /*!< fSAMPLING=fDTS/2, N=8 */
#define LL_TIM_BREAK_FILTER_FDIV4_N6           0x00060000U   /*!< fSAMPLING=fDTS/4, N=6 */
#define LL_TIM_BREAK_FILTER_FDIV4_N8           0x00070000U   /*!< fSAMPLING=fDTS/4, N=8 */
#define LL_TIM_BREAK_FILTER_FDIV8_N6           0x00080000U   /*!< fSAMPLING=fDTS/8, N=6 */
#define LL_TIM_BREAK_FILTER_FDIV8_N8           0x00090000U   /*!< fSAMPLING=fDTS/8, N=8 */
#define LL_TIM_BREAK_FILTER_FDIV16_N5          0x000A0000U   /*!< fSAMPLING=fDTS/16, N=5 */
#define LL_TIM_BREAK_FILTER_FDIV16_N6          0x000B0000U   /*!< fSAMPLING=fDTS/16, N=6 */
#define LL_TIM_BREAK_FILTER_FDIV16_N8          0x000C0000U   /*!< fSAMPLING=fDTS/16, N=8 */
#define LL_TIM_BREAK_FILTER_FDIV32_N5          0x000D0000U   /*!< fSAMPLING=fDTS/32, N=5 */
#define LL_TIM_BREAK_FILTER_FDIV32_N6          0x000E0000U   /*!< fSAMPLING=fDTS/32, N=6 */
#define LL_TIM_BREAK_FILTER_FDIV32_N8          0x000F0000U   /*!< fSAMPLING=fDTS/32, N=8 */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_BREAK2_POLARITY BREAK2 POLARITY
  * @{
  */
#define LL_TIM_BREAK2_POLARITY_LOW             0x00000000U             /*!< Break input BRK2 is active low */
#define LL_TIM_BREAK2_POLARITY_HIGH            TIM_BDTR_BK2P           /*!< Break input BRK2 is active high */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_BREAK2_FILTER BREAK2 FILTER
  * @{
  */
#define LL_TIM_BREAK2_FILTER_FDIV1             0x00000000U   /*!< No filter, BRK acts asynchronously */
#define LL_TIM_BREAK2_FILTER_FDIV1_N2          0x00100000U   /*!< fSAMPLING=fCK_INT, N=2 */
#define LL_TIM_BREAK2_FILTER_FDIV1_N4          0x00200000U   /*!< fSAMPLING=fCK_INT, N=4 */
#define LL_TIM_BREAK2_FILTER_FDIV1_N8          0x00300000U   /*!< fSAMPLING=fCK_INT, N=8 */
#define LL_TIM_BREAK2_FILTER_FDIV2_N6          0x00400000U   /*!< fSAMPLING=fDTS/2, N=6 */
#define LL_TIM_BREAK2_FILTER_FDIV2_N8          0x00500000U   /*!< fSAMPLING=fDTS/2, N=8 */
#define LL_TIM_BREAK2_FILTER_FDIV4_N6          0x00600000U   /*!< fSAMPLING=fDTS/4, N=6 */
#define LL_TIM_BREAK2_FILTER_FDIV4_N8          0x00700000U   /*!< fSAMPLING=fDTS/4, N=8 */
#define LL_TIM_BREAK2_FILTER_FDIV8_N6          0x00800000U   /*!< fSAMPLING=fDTS/8, N=6 */
#define LL_TIM_BREAK2_FILTER_FDIV8_N8          0x00900000U   /*!< fSAMPLING=fDTS/8, N=8 */
#define LL_TIM_BREAK2_FILTER_FDIV16_N5         0x00A00000U   /*!< fSAMPLING=fDTS/16, N=5 */
#define LL_TIM_BREAK2_FILTER_FDIV16_N6         0x00B00000U   /*!< fSAMPLING=fDTS/16, N=6 */
#define LL_TIM_BREAK2_FILTER_FDIV16_N8         0x00C00000U   /*!< fSAMPLING=fDTS/16, N=8 */
#define LL_TIM_BREAK2_FILTER_FDIV32_N5         0x00D00000U   /*!< fSAMPLING=fDTS/32, N=5 */
#define LL_TIM_BREAK2_FILTER_FDIV32_N6         0x00E00000U   /*!< fSAMPLING=fDTS/32, N=6 */
#define LL_TIM_BREAK2_FILTER_FDIV32_N8         0x00F00000U   /*!< fSAMPLING=fDTS/32, N=8 */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_OSSI OSSI
  * @{
  */
#define LL_TIM_OSSI_DISABLE                    0x00000000U             /*!< When inactive, OCx/OCxN outputs are disabled */
#define LL_TIM_OSSI_ENABLE                     TIM_BDTR_OSSI           /*!< When inactive, OxC/OCxN outputs are first forced with their inactive level then forced to their idle level after the deadtime */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_OSSR OSSR
  * @{
  */
#define LL_TIM_OSSR_DISABLE                    0x00000000U             /*!< When inactive, OCx/OCxN outputs are disabled */
#define LL_TIM_OSSR_ENABLE                     TIM_BDTR_OSSR           /*!< When inactive, OC/OCN outputs are enabled with their inactive level as soon as CCxE=1 or CCxNE=1 */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_BREAK_INPUT BREAK INPUT
  * @{
  */
#define LL_TIM_BREAK_INPUT_BKIN                0x00000000U  /*!< TIMx_BKIN input */
#define LL_TIM_BREAK_INPUT_BKIN2               0x00000004U  /*!< TIMx_BKIN2 input */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_BKIN_SOURCE BKIN SOURCE
  * @{
  */
#define LL_TIM_BKIN_SOURCE_BKIN                TIM_AF1_BKINE      /*!< BKIN input from AF controller */
#define LL_TIM_BKIN_SOURCE_MDF1                TIM_AF1_BKCMP8E    /*!< internal signal: MDF1 output */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_BKIN_POLARITY BKIN POLARITY
  * @{
  */
#define LL_TIM_BKIN_POLARITY_LOW               TIM_AF1_BKINP           /*!< BRK BKIN input is active low */
#define LL_TIM_BKIN_POLARITY_HIGH              0x00000000U              /*!< BRK BKIN input is active high */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_BREAK_AFMODE BREAK AF MODE
  * @{
  */
#define LL_TIM_BREAK_AFMODE_INPUT              0x00000000U              /*!< Break input BRK in input mode */
#define LL_TIM_BREAK_AFMODE_BIDIRECTIONAL      TIM_BDTR_BKBID           /*!< Break input BRK in bidirectional mode */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_BREAK2_AFMODE BREAK2 AF MODE
  * @{
  */
#define LL_TIM_BREAK2_AFMODE_INPUT             0x00000000U             /*!< Break2 input BRK2 in input mode */
#define LL_TIM_BREAK2_AFMODE_BIDIRECTIONAL     TIM_BDTR_BK2BID         /*!< Break2 input BRK2 in bidirectional mode */
/**
  * @}
  */

/** Legacy definitions for compatibility purpose
@cond 0
  */
#define LL_TIM_ReArmBRK(_PARAM_)
#define LL_TIM_ReArmBRK2(_PARAM_)
/**
@endcond
  */

/** @defgroup TIM_LL_EC_DMABURST_BASEADDR DMA Burst Base Address
  * @{
  */
#define LL_TIM_DMABURST_BASEADDR_CR1           0x00000000U                                                      /*!< TIMx_CR1 register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_CR2           TIM_DCR_DBA_0                                                    /*!< TIMx_CR2 register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_SMCR          TIM_DCR_DBA_1                                                    /*!< TIMx_SMCR register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_DIER          (TIM_DCR_DBA_1 |  TIM_DCR_DBA_0)                                 /*!< TIMx_DIER register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_SR            TIM_DCR_DBA_2                                                    /*!< TIMx_SR register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_EGR           (TIM_DCR_DBA_2 | TIM_DCR_DBA_0)                                  /*!< TIMx_EGR register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_CCMR1         (TIM_DCR_DBA_2 | TIM_DCR_DBA_1)                                  /*!< TIMx_CCMR1 register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_CCMR2         (TIM_DCR_DBA_2 | TIM_DCR_DBA_1 | TIM_DCR_DBA_0)                  /*!< TIMx_CCMR2 register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_CCER          TIM_DCR_DBA_3                                                    /*!< TIMx_CCER register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_CNT           (TIM_DCR_DBA_3 | TIM_DCR_DBA_0)                                  /*!< TIMx_CNT register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_PSC           (TIM_DCR_DBA_3 | TIM_DCR_DBA_1)                                  /*!< TIMx_PSC register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_ARR           (TIM_DCR_DBA_3 | TIM_DCR_DBA_1 | TIM_DCR_DBA_0)                  /*!< TIMx_ARR register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_RCR           (TIM_DCR_DBA_3 | TIM_DCR_DBA_2)                                  /*!< TIMx_RCR register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_CCR1          (TIM_DCR_DBA_3 | TIM_DCR_DBA_2 | TIM_DCR_DBA_0)                  /*!< TIMx_CCR1 register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_CCR2          (TIM_DCR_DBA_3 | TIM_DCR_DBA_2 | TIM_DCR_DBA_1)                  /*!< TIMx_CCR2 register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_CCR3          (TIM_DCR_DBA_3 | TIM_DCR_DBA_2 | TIM_DCR_DBA_1 | TIM_DCR_DBA_0)  /*!< TIMx_CCR3 register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_CCR4          TIM_DCR_DBA_4                                                    /*!< TIMx_CCR4 register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_BDTR          (TIM_DCR_DBA_4 | TIM_DCR_DBA_0)                                  /*!< TIMx_BDTR register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_CCR5          (TIM_DCR_DBA_4 | TIM_DCR_DBA_1)                                  /*!< TIMx_CCR5 register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_CCR6          (TIM_DCR_DBA_4 | TIM_DCR_DBA_1 | TIM_DCR_DBA_0)                  /*!< TIMx_CCR6 register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_CCMR3         (TIM_DCR_DBA_4 | TIM_DCR_DBA_2)                                  /*!< TIMx_CCMR3 register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_DTR2          (TIM_DCR_DBA_4 | TIM_DCR_DBA_2 | TIM_DCR_DBA_0)                  /*!< TIMx_DTR2 register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_ECR           (TIM_DCR_DBA_4 | TIM_DCR_DBA_2 | TIM_DCR_DBA_1)                  /*!< TIMx_ECR register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_TISEL         (TIM_DCR_DBA_4 | TIM_DCR_DBA_2 | TIM_DCR_DBA_1 | TIM_DCR_DBA_0)  /*!< TIMx_TISEL register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_AF1           (TIM_DCR_DBA_4 | TIM_DCR_DBA_3)                                  /*!< TIMx_AF1 register is the DMA base address for DMA burst */
#define LL_TIM_DMABURST_BASEADDR_AF2           (TIM_DCR_DBA_4 | TIM_DCR_DBA_3 | TIM_DCR_DBA_0)                  /*!< TIMx_AF2 register is the DMA base address for DMA burst */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_DMABURST_LENGTH DMA Burst Length
  * @{
  */
#define LL_TIM_DMABURST_LENGTH_1TRANSFER       0x00000000U                                                     /*!< Transfer is done to 1 register starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_2TRANSFERS      TIM_DCR_DBL_0                                                   /*!< Transfer is done to 2 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_3TRANSFERS      TIM_DCR_DBL_1                                                   /*!< Transfer is done to 3 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_4TRANSFERS      (TIM_DCR_DBL_1 |  TIM_DCR_DBL_0)                                /*!< Transfer is done to 4 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_5TRANSFERS      TIM_DCR_DBL_2                                                   /*!< Transfer is done to 5 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_6TRANSFERS      (TIM_DCR_DBL_2 | TIM_DCR_DBL_0)                                 /*!< Transfer is done to 6 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_7TRANSFERS      (TIM_DCR_DBL_2 | TIM_DCR_DBL_1)                                 /*!< Transfer is done to 7 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_8TRANSFERS      (TIM_DCR_DBL_2 | TIM_DCR_DBL_1 | TIM_DCR_DBL_0)                 /*!< Transfer is done to 1 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_9TRANSFERS      TIM_DCR_DBL_3                                                   /*!< Transfer is done to 9 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_10TRANSFERS     (TIM_DCR_DBL_3 | TIM_DCR_DBL_0)                                 /*!< Transfer is done to 10 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_11TRANSFERS     (TIM_DCR_DBL_3 | TIM_DCR_DBL_1)                                 /*!< Transfer is done to 11 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_12TRANSFERS     (TIM_DCR_DBL_3 | TIM_DCR_DBL_1 | TIM_DCR_DBL_0)                 /*!< Transfer is done to 12 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_13TRANSFERS     (TIM_DCR_DBL_3 | TIM_DCR_DBL_2)                                 /*!< Transfer is done to 13 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_14TRANSFERS     (TIM_DCR_DBL_3 | TIM_DCR_DBL_2 | TIM_DCR_DBL_0)                 /*!< Transfer is done to 14 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_15TRANSFERS     (TIM_DCR_DBL_3 | TIM_DCR_DBL_2 | TIM_DCR_DBL_1)                 /*!< Transfer is done to 15 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_16TRANSFERS     (TIM_DCR_DBL_3 | TIM_DCR_DBL_2 | TIM_DCR_DBL_1 | TIM_DCR_DBL_0) /*!< Transfer is done to 16 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_17TRANSFERS     TIM_DCR_DBL_4                                                   /*!< Transfer is done to 17 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_18TRANSFERS     (TIM_DCR_DBL_4 |  TIM_DCR_DBL_0)                                /*!< Transfer is done to 18 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_19TRANSFERS     (TIM_DCR_DBL_4 | TIM_DCR_DBL_1)                                 /*!< Transfer is done to 19 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_20TRANSFERS     (TIM_DCR_DBL_4 | TIM_DCR_DBL_1 | TIM_DCR_DBL_0)                 /*!< Transfer is done to 20 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_21TRANSFERS     (TIM_DCR_DBL_4 | TIM_DCR_DBL_2)                                 /*!< Transfer is done to 21 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_22TRANSFERS     (TIM_DCR_DBL_4 | TIM_DCR_DBL_2 | TIM_DCR_DBL_0)                 /*!< Transfer is done to 22 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_23TRANSFERS     (TIM_DCR_DBL_4 | TIM_DCR_DBL_2 | TIM_DCR_DBL_1)                 /*!< Transfer is done to 23 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_24TRANSFERS     (TIM_DCR_DBL_4 | TIM_DCR_DBL_2 | TIM_DCR_DBL_1 | TIM_DCR_DBL_0) /*!< Transfer is done to 24 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_25TRANSFERS     (TIM_DCR_DBL_4 | TIM_DCR_DBL_3)                                 /*!< Transfer is done to 25 registers starting from the DMA burst base address */
#define LL_TIM_DMABURST_LENGTH_26TRANSFERS     (TIM_DCR_DBL_4 | TIM_DCR_DBL_3 | TIM_DCR_DBL_0)                 /*!< Transfer is done to 26 registers starting from the DMA burst base address */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_DMABURST_SOURCE DMA Burst Source
  * @{
  */
#define LL_TIM_DMA_UPDATE                      TIM_DCR_DBSS_0                                                  /*!< Transfer source is update event */
#define LL_TIM_DMA_CC1                         TIM_DCR_DBSS_1                                                  /*!< Transfer source is CC1 event */
#define LL_TIM_DMA_CC2                         (TIM_DCR_DBSS_1 |  TIM_DCR_DBSS_0)                              /*!< Transfer source is CC2 event */
#define LL_TIM_DMA_CC3                         TIM_DCR_DBSS_2                                                  /*!< Transfer source is CC3 event */
#define LL_TIM_DMA_CC4                         (TIM_DCR_DBSS_2 | TIM_DCR_DBSS_0)                               /*!< Transfer source is CC4 event */
#define LL_TIM_DMA_COM                         (TIM_DCR_DBSS_2 | TIM_DCR_DBSS_1)                               /*!< Transfer source is COM event */
#define LL_TIM_DMA_TRIGGER                     (TIM_DCR_DBSS_2 | TIM_DCR_DBSS_1 | TIM_DCR_DBSS_0)              /*!< Transfer source is trigger event */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM2_TI1_RMP  TIM2 External Input Ch1 Remap
  * @{
  */
#define LL_TIM_TIM2_TI1_RMP_GPIO                0x00000000UL                                               /*!< TIM2_TI1 is connected to GPIO */
#define LL_TIM_TIM2_TI1_RMP_ETH1_PPS            TIM_TISEL_TI1SEL_0                                         /*!< TIM2_TI1 is connected to ETH1 PPS */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM3_TI1_RMP  TIM3 External Input Ch1 Remap
  * @{
  */
#define LL_TIM_TIM3_TI1_RMP_GPIO                0x00000000UL                                               /*!< TIM3_TI1 is connected to GPIO */
#define LL_TIM_TIM3_TI1_RMP_ETH1_PPS            TIM_TISEL_TI1SEL_0                                         /*!< TIM3_TI1 is connected to ETH1 PPS */
#define LL_TIM_TIM3_TI1_RMP_FDCAN_RTP           TIM_TISEL_TI1SEL_1                                         /*!< TIM3_TI1 is connected to FDCAN RTP */
#define LL_TIM_TIM3_TI1_RMP_FDCAN_TMP           (TIM_TISEL_TI1SEL_1 | TIM_TISEL_TI1SEL_0)                  /*!< TIM3_TI1 is connected to FDCAN TMP */
#define LL_TIM_TIM3_TI1_RMP_FDCAN_SOC           TIM_TISEL_TI1SEL_2                                         /*!< TIM3_TI1 is connected to FDCAN SOC */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM5_TI1_RMP  TIM5 External Input Ch1 Remap
  * @{
  */
#define LL_TIM_TIM5_TI1_RMP_GPIO                0x00000000UL                                               /*!< TIM5_TI1 is connected to GPIO */
#define LL_TIM_TIM5_TI1_RMP_FDCAN_RTP           TIM_TISEL_TI1SEL_1                                         /*!< TIM5_TI1 is connected to FDCAN RTP */
#define LL_TIM_TIM5_TI1_RMP_FDCAN_TMP           (TIM_TISEL_TI1SEL_1 | TIM_TISEL_TI1SEL_0)                  /*!< TIM5_TI1 is connected to FDCAN TMP */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM9_TI1_RMP  TIM9 External Input Ch1 Remap
  * @{
  */
#define LL_TIM_TIM9_TI1_RMP_GPIO            0x00000000UL                                                   /*!< TIM9_TI1 is connected to GPIO */
#define LL_TIM_TIM9_TI1_RMP_MCO1            TIM_TISEL_TI1SEL_2                                             /*!< TIM9_TI1 is connected to MCO1 */
#define LL_TIM_TIM9_TI1_RMP_MCO2            (TIM_TISEL_TI1SEL_2 | TIM_TISEL_TI1SEL_0)                      /*!< TIM9_TI1 is connected to MCO2 */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM10_TI1_RMP  TIM10 External Input Ch1 Remap
  * @{
  */
#define LL_TIM_TIM10_TI1_RMP_GPIO           0x00000000UL                                                   /*!< TIM10_TI1 is connected to GPIO */
#define LL_TIM_TIM10_TI1_RMP_I3C1_IBIACK    TIM_TISEL_TI1SEL_0                                             /*!< TIM10_TI1 is connected to I3C1 IBI ACK */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM11_TI1_RMP  TIM11 External Input Ch1 Remap
  * @{
  */
#define LL_TIM_TIM11_TI1_RMP_GPIO           0x00000000UL                                                   /*!< TIM11_TI1 is connected to GPIO */
#define LL_TIM_TIM11_TI1_RMP_I3C2_IBIACK    TIM_TISEL_TI1SEL_0                                             /*!< TIM11_TI1 is connected to I3C2 IBI ACK */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM12_TI1_RMP  TIM12 External Input Ch1 Remap
  * @{
  */
#define LL_TIM_TIM12_TI1_RMP_GPIO           0x00000000UL                                                   /*!< TIM12_TI1 is connected to GPIO */
#define LL_TIM_TIM12_TI1_RMP_SPDIF_FS       TIM_TISEL_TI1SEL_0                                             /*!< TIM12 TI1 is connected to SPDIF FS */
#define LL_TIM_TIM12_TI1_RMP_HSI_1024       TIM_TISEL_TI1SEL_1                                             /*!< TIM12_TI1 is connected to HSI/1024 */
#define LL_TIM_TIM12_TI1_RMP_MSI_128        (TIM_TISEL_TI1SEL_1 | TIM_TISEL_TI1SEL_0)                      /*!< TIM12_TI1 is connected to MSI/128 */
#define LL_TIM_TIM12_TI1_RMP_MCO1           TIM_TISEL_TI1SEL_2                                             /*!< TIM12_TI1 is connected to MCO1 */
#define LL_TIM_TIM12_TI1_RMP_MCO2           (TIM_TISEL_TI1SEL_2 | TIM_TISEL_TI1SEL_0)                      /*!< TIM12_TI1 is connected to MCO2 */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM13_TI1_RMP  TIM13 External Input Ch1 Remap
  * @{
  */
#define LL_TIM_TIM13_TI1_RMP_GPIO           0x00000000UL                                                   /*!< TIM13_TI1 is connected to GPIO */
#define LL_TIM_TIM13_TI1_RMP_I3C1_IBIACK    TIM_TISEL_TI1SEL_0                                             /*!< TIM13_TI1 is connected to I3C1 IBI ACK */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM14_TI1_RMP  TIM14 External Input Ch1 Remap
  * @{
  */
#define LL_TIM_TIM14_TI1_RMP_GPIO           0x00000000UL                                                   /*!< TIM14_TI1 is connected to GPIO */
#define LL_TIM_TIM14_TI1_RMP_I3C2_IBIACK    TIM_TISEL_TI1SEL_0                                             /*!< TIM14_TI1 is connected to I3C2 IBI ACK */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM15_TI1_RMP  TIM15 External Input Ch1 Remap
  * @{
  */
#define LL_TIM_TIM15_TI1_RMP_GPIO           0x00000000UL                                                   /*!< TIM15_TI1 is connected to GPIO */
#define LL_TIM_TIM15_TI1_RMP_TIM2_CH1       TIM_TISEL_TI1SEL_0                                             /*!< TIM15_TI1 is connected to TIM2 CH1 GPIO */
#define LL_TIM_TIM15_TI1_RMP_TIM3_CH1       TIM_TISEL_TI1SEL_1                                             /*!< TIM15_TI1 is connected to TIM3 CH1 GPIO */
#define LL_TIM_TIM15_TI1_RMP_TIM4_CH1       (TIM_TISEL_TI1SEL_1 | TIM_TISEL_TI1SEL_0)                      /*!< TIM15_TI1 is connected to TIM4 CH1 GPIO */
#define LL_TIM_TIM15_TI1_RMP_MCO1           TIM_TISEL_TI1SEL_2                                             /*!< TIM15_TI1 is connected to MCO1 */
#define LL_TIM_TIM15_TI1_RMP_MCO2           (TIM_TISEL_TI1SEL_2 | TIM_TISEL_TI1SEL_0)                      /*!< TIM15_TI1 is connected to MCO2 */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM15_TI2_RMP  TIM15 External Input Ch2 Remap
  * @{
  */
#define LL_TIM_TIM15_TI2_RMP_GPIO           0x00000000UL                                                   /*!< TIM15_TI2 is connected to GPIO */
#define LL_TIM_TIM15_TI2_RMP_TIM2_CH2       TIM_TISEL_TI2SEL_0                                             /*!< TIM15_TI2 is connected to TIM2 CH2 GPIO */
#define LL_TIM_TIM15_TI2_RMP_TIM3_CH2       TIM_TISEL_TI2SEL_1                                             /*!< TIM15_TI2 is connected to TIM3 CH2 GPIO */
#define LL_TIM_TIM15_TI2_RMP_TIM4_CH2       (TIM_TISEL_TI2SEL_1 | TIM_TISEL_TI2SEL_0)                      /*!< TIM15_TI2 is connected to TIM4 CH2 GPIO */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM16_TI1_RMP  TIM16 External Input Ch1 Remap
  * @{
  */
#define LL_TIM_TIM16_TI1_RMP_GPIO           0x00000000UL                                                   /*!< TIM16_TI1 is connected to GPIO */
#define LL_TIM_TIM16_TI1_RMP_LSI            TIM_TISEL_TI1SEL_0                                             /*!< TIM16 TI1 is connected to LSI */
#define LL_TIM_TIM16_TI1_RMP_LSE            TIM_TISEL_TI1SEL_1                                             /*!< TIM16 TI1 is connected to LSE */
#define LL_TIM_TIM16_TI1_RMP_RTC_WKUP       (TIM_TISEL_TI1SEL_1 | TIM_TISEL_TI1SEL_0)                      /*!< TIM16_TI1 is connected to RTC Wakeup */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_TIM17_TI1_RMP  TIM17 External Input Ch1 Remap
  * @{
  */
#define LL_TIM_TIM17_TI1_RMP_GPIO           0x00000000UL                                                   /*!< TIM17_TI1 is connected to GPIO */
#define LL_TIM_TIM17_TI1_RMP_SPDIF_FS       TIM_TISEL_TI1SEL_0                                             /*!< TIM17_TI1 is connected to SPDIFRX FS */
#define LL_TIM_TIM17_TI1_RMP_HSE_1024       TIM_TISEL_TI1SEL_1                                             /*!< TIM17 TI1 is connected to HSE/1024 */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_INDEX_DIR index direction selection
  * @{
  */
#define LL_TIM_INDEX_UP_DOWN     0x00000000U         /*!< Index resets the counter whatever the direction */
#define LL_TIM_INDEX_UP          TIM_ECR_IDIR_0      /*!< Index resets the counter when up-counting only */
#define LL_TIM_INDEX_DOWN        TIM_ECR_IDIR_1      /*!< Index resets the counter when down-counting only */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_INDEX_BLANK index blanking selection
  * @{
  */
#define LL_TIM_INDEX_BLANK_ALWAYS     0x00000000U         /*!< Index always active */
#define LL_TIM_INDEX_BLANK_TI3        TIM_ECR_IBLK_0      /*!< Index disabled when TI3 input is active, as per CC3P bitfield */
#define LL_TIM_INDEX_BLANK_TI4        TIM_ECR_IBLK_1      /*!< Index disabled when TI4 input is active, as per CC4P bitfield */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_INDEX_POSITION index positioning selection
  * @{
  */
#define LL_TIM_INDEX_POSITION_DOWN_DOWN    0x00000000U                           /*!< Index resets the counter when AB = 00 */
#define LL_TIM_INDEX_POSITION_DOWN_UP      TIM_ECR_IPOS_0                        /*!< Index resets the counter when AB = 01 */
#define LL_TIM_INDEX_POSITION_UP_DOWN      TIM_ECR_IPOS_1                        /*!< Index resets the counter when AB = 10 */
#define LL_TIM_INDEX_POSITION_UP_UP        (TIM_ECR_IPOS_1 | TIM_ECR_IPOS_0)     /*!< Index resets the counter when AB = 11 */
#define LL_TIM_INDEX_POSITION_DOWN         0x00000000U                           /*!< Index resets the counter when clock is 0 */
#define LL_TIM_INDEX_POSITION_UP           TIM_ECR_IPOS_0                        /*!< Index resets the counter when clock is 1 */
/**
  * @}
  */

/** @defgroup TIM_LL_EC_FIRST_INDEX first index selection
  * @{
  */
#define LL_TIM_INDEX_ALL           0x00000000U                           /*!< Index is always active */
#define LL_TIM_INDEX_FIRST_ONLY    TIM_ECR_FIDX                          /*!< The first Index only resets the counter */
/**
  * @}
  */
/** @defgroup TIM_LL_EC_PWPRSC Pulse on compare pulse width prescaler
  * @{
  */
#define LL_TIM_PWPRSC_X1     0x00000000U                                              /*!< Pulse on compare pulse width prescaler 1 */
#define LL_TIM_PWPRSC_X2     TIM_ECR_PWPRSC_0                                         /*!< Pulse on compare pulse width prescaler 2 */
#define LL_TIM_PWPRSC_X4     TIM_ECR_PWPRSC_1                                         /*!< Pulse on compare pulse width prescaler 4 */
#define LL_TIM_PWPRSC_X8     (TIM_ECR_PWPRSC_1 | TIM_ECR_PWPRSC_0)                    /*!< Pulse on compare pulse width prescaler 8 */
#define LL_TIM_PWPRSC_X16    TIM_ECR_PWPRSC_2                                         /*!< Pulse on compare pulse width prescaler 16 */
#define LL_TIM_PWPRSC_X32    (TIM_ECR_PWPRSC_2 | TIM_ECR_PWPRSC_0)                    /*!< Pulse on compare pulse width prescaler 32 */
#define LL_TIM_PWPRSC_X64    (TIM_ECR_PWPRSC_2 | TIM_ECR_PWPRSC_1)                    /*!< Pulse on compare pulse width prescaler 64 */
#define LL_TIM_PWPRSC_X128   (TIM_ECR_PWPRSC_2 | TIM_ECR_PWPRSC_1 | TIM_ECR_PWPRSC_0) /*!< Pulse on compare pulse width prescaler 128 */
/**
  * @}
  */

/** Legacy definitions for compatibility purpose
@cond 0
  */
#define LL_TIM_BKIN_SOURCE_DFBK  LL_TIM_BKIN_SOURCE_DF1BK
/**
@endcond
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup TIM_LL_Exported_Macros TIM Exported Macros
  * @{
  */

/** @defgroup TIM_LL_EM_WRITE_READ Common Write and read registers Macros
  * @{
  */
/**
  * @brief  Write a value in TIM register.
  * @param  __INSTANCE__ TIM Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_TIM_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG((__INSTANCE__)->__REG__, (__VALUE__))

/**
  * @brief  Read a value in TIM register.
  * @param  __INSTANCE__ TIM Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_TIM_ReadReg(__INSTANCE__, __REG__) READ_REG((__INSTANCE__)->__REG__)
/**
  * @}
  */

/**
  * @brief  HELPER macro retrieving the UIFCPY flag from the counter value.
  * @note ex: @ref __LL_TIM_GETFLAG_UIFCPY (@ref LL_TIM_GetCounter ());
  * @note  Relevant only if UIF flag remapping has been enabled  (UIF status bit is copied
  *        to TIMx_CNT register bit 31)
  * @param  __CNT__ Counter value
  * @retval UIF status bit
  */
#define __LL_TIM_GETFLAG_UIFCPY(__CNT__)  \
  (READ_BIT((__CNT__), TIM_CNT_UIFCPY) >> TIM_CNT_UIFCPY_Pos)

/**
  * @brief  HELPER macro calculating DTG[0:7] in the TIMx_BDTR register to achieve the requested dead time duration.
  * @note ex: @ref __LL_TIM_CALC_DEADTIME (80000000, @ref LL_TIM_GetClockDivision (), 120);
  * @param  __TIMCLK__ timer input clock frequency (in Hz)
  * @param  __CKD__ This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CLOCKDIVISION_DIV1
  *         @arg @ref LL_TIM_CLOCKDIVISION_DIV2
  *         @arg @ref LL_TIM_CLOCKDIVISION_DIV4
  * @param  __DT__ deadtime duration (in ns)
  * @retval DTG[0:7]
  */
#define __LL_TIM_CALC_DEADTIME(__TIMCLK__, __CKD__, __DT__)  \
  ( (((uint64_t)((__DT__)*1000U)) < ((DT_DELAY_1+1U) * TIM_CALC_DTS((__TIMCLK__), (__CKD__))))    ?  \
    (uint8_t)(((uint64_t)((__DT__)*1000U) / TIM_CALC_DTS((__TIMCLK__), (__CKD__)))  & DT_DELAY_1) :      \
    (((uint64_t)((__DT__)*1000U)) < ((64U + (DT_DELAY_2+1U)) * 2U * TIM_CALC_DTS((__TIMCLK__), (__CKD__))))  ?  \
    (uint8_t)(DT_RANGE_2 | ((uint8_t)((uint8_t)((((uint64_t)((__DT__)*1000U))/ TIM_CALC_DTS((__TIMCLK__),   \
                                                 (__CKD__))) >> 1U) - (uint8_t) 64) & DT_DELAY_2)) :\
    (((uint64_t)((__DT__)*1000U)) < ((32U + (DT_DELAY_3+1U)) * 8U * TIM_CALC_DTS((__TIMCLK__), (__CKD__))))  ?  \
    (uint8_t)(DT_RANGE_3 | ((uint8_t)((uint8_t)(((((uint64_t)(__DT__)*1000U))/ TIM_CALC_DTS((__TIMCLK__),  \
                                                 (__CKD__))) >> 3U) - (uint8_t) 32) & DT_DELAY_3)) :\
    (((uint64_t)((__DT__)*1000U)) < ((32U + (DT_DELAY_4+1U)) * 16U * TIM_CALC_DTS((__TIMCLK__), (__CKD__)))) ?  \
    (uint8_t)(DT_RANGE_4 | ((uint8_t)((uint8_t)(((((uint64_t)(__DT__)*1000U))/ TIM_CALC_DTS((__TIMCLK__),  \
                                                 (__CKD__))) >> 4U) - (uint8_t) 32) & DT_DELAY_4)) :\
    0U)

/**
  * @brief  HELPER macro calculating the prescaler value to achieve the required counter clock frequency.
  * @note ex: @ref __LL_TIM_CALC_PSC (80000000, 1000000);
  * @param  __TIMCLK__ timer input clock frequency (in Hz)
  * @param  __CNTCLK__ counter clock frequency (in Hz)
  * @retval Prescaler value  (between Min_Data=0 and Max_Data=65535)
  */
#define __LL_TIM_CALC_PSC(__TIMCLK__, __CNTCLK__)   \
  (((__TIMCLK__) >= (__CNTCLK__)) ? (uint32_t)((((__TIMCLK__) + (__CNTCLK__)/2U)/(__CNTCLK__)) - 1U) : 0U)

/**
  * @brief  HELPER macro calculating the auto-reload value to achieve the required output signal frequency.
  * @note ex: @ref __LL_TIM_CALC_ARR (1000000, @ref LL_TIM_GetPrescaler (), 10000);
  * @param  __TIMCLK__ timer input clock frequency (in Hz)
  * @param  __PSC__ prescaler
  * @param  __FREQ__ output signal frequency (in Hz)
  * @retval  Auto-reload value  (between Min_Data=0 and Max_Data=65535)
  */
#define __LL_TIM_CALC_ARR(__TIMCLK__, __PSC__, __FREQ__) \
  ((((__TIMCLK__)/((__PSC__) + 1U)) >= (__FREQ__)) ? (((__TIMCLK__)/((__FREQ__) * ((__PSC__) + 1U))) - 1U) : 0U)

/**
  * @brief  HELPER macro calculating the auto-reload value, with dithering feature enabled, to achieve the required
  *         output signal frequency.
  * @note ex: @ref __LL_TIM_CALC_ARR_DITHER (1000000, @ref LL_TIM_GetPrescaler (), 10000);
  * @param  __TIMCLK__ timer input clock frequency (in Hz)
  * @param  __PSC__ prescaler
  * @param  __FREQ__ output signal frequency (in Hz)
  * @retval  Auto-reload value  (between Min_Data=0 and Max_Data=65535)
  */
#define __LL_TIM_CALC_ARR_DITHER(__TIMCLK__, __PSC__, __FREQ__) \
  ((((__TIMCLK__)/((__PSC__) + 1U)) >= (__FREQ__)) ? \
   (uint32_t)((((uint64_t)(__TIMCLK__) * 16U/((__FREQ__) * ((__PSC__) + 1U))) - 16U)) : 0U)

/**
  * @brief  HELPER macro calculating the compare value required to achieve the required timer output compare
  *         active/inactive delay.
  * @note ex: @ref __LL_TIM_CALC_DELAY (1000000, @ref LL_TIM_GetPrescaler (), 10);
  * @param  __TIMCLK__ timer input clock frequency (in Hz)
  * @param  __PSC__ prescaler
  * @param  __DELAY__ timer output compare active/inactive delay (in us)
  * @retval Compare value  (between Min_Data=0 and Max_Data=65535)
  */
#define __LL_TIM_CALC_DELAY(__TIMCLK__, __PSC__, __DELAY__)  \
  ((uint32_t)(((uint64_t)(__TIMCLK__) * (uint64_t)(__DELAY__)) \
              / ((uint64_t)1000000U * (uint64_t)((__PSC__) + 1U))))

/**
  * @brief  HELPER macro calculating the compare value, with dithering feature enabled, to achieve the required timer
  *         output compare active/inactive delay.
  * @note ex: @ref __LL_TIM_CALC_DELAY_DITHER (1000000, @ref LL_TIM_GetPrescaler (), 10);
  * @param  __TIMCLK__ timer input clock frequency (in Hz)
  * @param  __PSC__ prescaler
  * @param  __DELAY__ timer output compare active/inactive delay (in us)
  * @retval Compare value  (between Min_Data=0 and Max_Data=65535)
  */
#define __LL_TIM_CALC_DELAY_DITHER(__TIMCLK__, __PSC__, __DELAY__)  \
  ((uint32_t)(((uint64_t)(__TIMCLK__) * (uint64_t)(__DELAY__) * 16U) \
              / ((uint64_t)1000000U * (uint64_t)((__PSC__) + 1U))))

/**
  * @brief  HELPER macro calculating the auto-reload value to achieve the required pulse duration
  *         (when the timer operates in one pulse mode).
  * @note ex: @ref __LL_TIM_CALC_PULSE (1000000, @ref LL_TIM_GetPrescaler (), 10, 20);
  * @param  __TIMCLK__ timer input clock frequency (in Hz)
  * @param  __PSC__ prescaler
  * @param  __DELAY__ timer output compare active/inactive delay (in us)
  * @param  __PULSE__ pulse duration (in us)
  * @retval Auto-reload value  (between Min_Data=0 and Max_Data=65535)
  */
#define __LL_TIM_CALC_PULSE(__TIMCLK__, __PSC__, __DELAY__, __PULSE__)  \
  ((uint32_t)(__LL_TIM_CALC_DELAY((__TIMCLK__), (__PSC__), (__PULSE__)) \
              + __LL_TIM_CALC_DELAY((__TIMCLK__), (__PSC__), (__DELAY__))))

/**
  * @brief  HELPER macro calculating the auto-reload value, with dithering feature enabled, to achieve the required
  *         pulse duration (when the timer operates in one pulse mode).
  * @note ex: @ref __LL_TIM_CALC_PULSE_DITHER (1000000, @ref LL_TIM_GetPrescaler (), 10, 20);
  * @param  __TIMCLK__ timer input clock frequency (in Hz)
  * @param  __PSC__ prescaler
  * @param  __DELAY__ timer output compare active/inactive delay (in us)
  * @param  __PULSE__ pulse duration (in us)
  * @retval Auto-reload value  (between Min_Data=0 and Max_Data=65535)
  */
#define __LL_TIM_CALC_PULSE_DITHER(__TIMCLK__, __PSC__, __DELAY__, __PULSE__)  \
  ((uint32_t)(__LL_TIM_CALC_DELAY_DITHER((__TIMCLK__), (__PSC__), (__PULSE__)) \
              + __LL_TIM_CALC_DELAY_DITHER((__TIMCLK__), (__PSC__), (__DELAY__))))

/**
  * @brief  HELPER macro retrieving the ratio of the input capture prescaler
  * @note ex: @ref __LL_TIM_GET_ICPSC_RATIO (@ref LL_TIM_IC_GetPrescaler ());
  * @param  __ICPSC__ This parameter can be one of the following values:
  *         @arg @ref LL_TIM_ICPSC_DIV1
  *         @arg @ref LL_TIM_ICPSC_DIV2
  *         @arg @ref LL_TIM_ICPSC_DIV4
  *         @arg @ref LL_TIM_ICPSC_DIV8
  * @retval Input capture prescaler ratio (1, 2, 4 or 8)
  */
#define __LL_TIM_GET_ICPSC_RATIO(__ICPSC__)  \
  ((uint32_t)(0x01U << (((__ICPSC__) >> 16U) >> TIM_CCMR1_IC1PSC_Pos)))


/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup TIM_LL_Exported_Functions TIM Exported Functions
  * @{
  */

/** @defgroup TIM_LL_EF_Time_Base Time Base configuration
  * @{
  */
/**
  * @brief  Enable timer counter.
  * @rmtoll CR1          CEN           LL_TIM_EnableCounter
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableCounter(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->CR1, TIM_CR1_CEN);
}

/**
  * @brief  Disable timer counter.
  * @rmtoll CR1          CEN           LL_TIM_DisableCounter
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableCounter(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->CR1, TIM_CR1_CEN);
}

/**
  * @brief  Indicates whether the timer counter is enabled.
  * @rmtoll CR1          CEN           LL_TIM_IsEnabledCounter
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledCounter(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->CR1, TIM_CR1_CEN) == (TIM_CR1_CEN)) ? 1UL : 0UL);
}

/**
  * @brief  Enable update event generation.
  * @rmtoll CR1          UDIS          LL_TIM_EnableUpdateEvent
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableUpdateEvent(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->CR1, TIM_CR1_UDIS);
}

/**
  * @brief  Disable update event generation.
  * @rmtoll CR1          UDIS          LL_TIM_DisableUpdateEvent
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableUpdateEvent(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->CR1, TIM_CR1_UDIS);
}

/**
  * @brief  Indicates whether update event generation is enabled.
  * @rmtoll CR1          UDIS          LL_TIM_IsEnabledUpdateEvent
  * @param  TIMx Timer instance
  * @retval Inverted state of bit (0 or 1).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledUpdateEvent(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->CR1, TIM_CR1_UDIS) == (uint32_t)RESET) ? 1UL : 0UL);
}

/**
  * @brief  Set update event source
  * @note Update event source set to LL_TIM_UPDATESOURCE_REGULAR: any of the following events
  *       generate an update interrupt or DMA request if enabled:
  *        - Counter overflow/underflow
  *        - Setting the UG bit
  *        - Update generation through the slave mode controller
  * @note Update event source set to LL_TIM_UPDATESOURCE_COUNTER: only counter
  *       overflow/underflow generates an update interrupt or DMA request if enabled.
  * @rmtoll CR1          URS           LL_TIM_SetUpdateSource
  * @param  TIMx Timer instance
  * @param  UpdateSource This parameter can be one of the following values:
  *         @arg @ref LL_TIM_UPDATESOURCE_REGULAR
  *         @arg @ref LL_TIM_UPDATESOURCE_COUNTER
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetUpdateSource(TIM_TypeDef *TIMx, uint32_t UpdateSource)
{
  MODIFY_REG(TIMx->CR1, TIM_CR1_URS, UpdateSource);
}

/**
  * @brief  Get actual event update source
  * @rmtoll CR1          URS           LL_TIM_GetUpdateSource
  * @param  TIMx Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_UPDATESOURCE_REGULAR
  *         @arg @ref LL_TIM_UPDATESOURCE_COUNTER
  */
__STATIC_INLINE uint32_t LL_TIM_GetUpdateSource(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_BIT(TIMx->CR1, TIM_CR1_URS));
}

/**
  * @brief  Set one pulse mode (one shot v.s. repetitive).
  * @rmtoll CR1          OPM           LL_TIM_SetOnePulseMode
  * @param  TIMx Timer instance
  * @param  OnePulseMode This parameter can be one of the following values:
  *         @arg @ref LL_TIM_ONEPULSEMODE_SINGLE
  *         @arg @ref LL_TIM_ONEPULSEMODE_REPETITIVE
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetOnePulseMode(TIM_TypeDef *TIMx, uint32_t OnePulseMode)
{
  MODIFY_REG(TIMx->CR1, TIM_CR1_OPM, OnePulseMode);
}

/**
  * @brief  Get actual one pulse mode.
  * @rmtoll CR1          OPM           LL_TIM_GetOnePulseMode
  * @param  TIMx Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_ONEPULSEMODE_SINGLE
  *         @arg @ref LL_TIM_ONEPULSEMODE_REPETITIVE
  */
__STATIC_INLINE uint32_t LL_TIM_GetOnePulseMode(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_BIT(TIMx->CR1, TIM_CR1_OPM));
}

/**
  * @brief  Set the timer counter counting mode.
  * @note Macro IS_TIM_COUNTER_MODE_SELECT_INSTANCE(TIMx) can be used to
  *       check whether or not the counter mode selection feature is supported
  *       by a timer instance.
  * @note Switching from Center Aligned counter mode to Edge counter mode (or reverse)
  *       requires a timer reset to avoid unexpected direction
  *       due to DIR bit readonly in center aligned mode.
  * @rmtoll CR1          DIR           LL_TIM_SetCounterMode\n
  *         CR1          CMS           LL_TIM_SetCounterMode
  * @param  TIMx Timer instance
  * @param  CounterMode This parameter can be one of the following values:
  *         @arg @ref LL_TIM_COUNTERMODE_UP
  *         @arg @ref LL_TIM_COUNTERMODE_DOWN
  *         @arg @ref LL_TIM_COUNTERMODE_CENTER_UP
  *         @arg @ref LL_TIM_COUNTERMODE_CENTER_DOWN
  *         @arg @ref LL_TIM_COUNTERMODE_CENTER_UP_DOWN
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetCounterMode(TIM_TypeDef *TIMx, uint32_t CounterMode)
{
  MODIFY_REG(TIMx->CR1, (TIM_CR1_DIR | TIM_CR1_CMS), CounterMode);
}

/**
  * @brief  Get actual counter mode.
  * @note Macro IS_TIM_COUNTER_MODE_SELECT_INSTANCE(TIMx) can be used to
  *       check whether or not the counter mode selection feature is supported
  *       by a timer instance.
  * @rmtoll CR1          DIR           LL_TIM_GetCounterMode\n
  *         CR1          CMS           LL_TIM_GetCounterMode
  * @param  TIMx Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_COUNTERMODE_UP
  *         @arg @ref LL_TIM_COUNTERMODE_DOWN
  *         @arg @ref LL_TIM_COUNTERMODE_CENTER_UP
  *         @arg @ref LL_TIM_COUNTERMODE_CENTER_DOWN
  *         @arg @ref LL_TIM_COUNTERMODE_CENTER_UP_DOWN
  */
__STATIC_INLINE uint32_t LL_TIM_GetCounterMode(const TIM_TypeDef *TIMx)
{
  uint32_t counter_mode;

  counter_mode = (uint32_t)(READ_BIT(TIMx->CR1, TIM_CR1_CMS));

  if (counter_mode == 0U)
  {
    counter_mode = (uint32_t)(READ_BIT(TIMx->CR1, TIM_CR1_DIR));
  }

  return counter_mode;
}

/**
  * @brief  Enable auto-reload (ARR) preload.
  * @rmtoll CR1          ARPE          LL_TIM_EnableARRPreload
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableARRPreload(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->CR1, TIM_CR1_ARPE);
}

/**
  * @brief  Disable auto-reload (ARR) preload.
  * @rmtoll CR1          ARPE          LL_TIM_DisableARRPreload
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableARRPreload(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->CR1, TIM_CR1_ARPE);
}

/**
  * @brief  Indicates whether auto-reload (ARR) preload is enabled.
  * @rmtoll CR1          ARPE          LL_TIM_IsEnabledARRPreload
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledARRPreload(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->CR1, TIM_CR1_ARPE) == (TIM_CR1_ARPE)) ? 1UL : 0UL);
}

/**
  * @brief  Set the division ratio between the timer clock  and the sampling clock used by the dead-time generators
  *         (when supported) and the digital filters.
  * @note Macro IS_TIM_CLOCK_DIVISION_INSTANCE(TIMx) can be used to check
  *       whether or not the clock division feature is supported by the timer
  *       instance.
  * @rmtoll CR1          CKD           LL_TIM_SetClockDivision
  * @param  TIMx Timer instance
  * @param  ClockDivision This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CLOCKDIVISION_DIV1
  *         @arg @ref LL_TIM_CLOCKDIVISION_DIV2
  *         @arg @ref LL_TIM_CLOCKDIVISION_DIV4
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetClockDivision(TIM_TypeDef *TIMx, uint32_t ClockDivision)
{
  MODIFY_REG(TIMx->CR1, TIM_CR1_CKD, ClockDivision);
}

/**
  * @brief  Get the actual division ratio between the timer clock  and the sampling clock used by the dead-time
  *         generators (when supported) and the digital filters.
  * @note Macro IS_TIM_CLOCK_DIVISION_INSTANCE(TIMx) can be used to check
  *       whether or not the clock division feature is supported by the timer
  *       instance.
  * @rmtoll CR1          CKD           LL_TIM_GetClockDivision
  * @param  TIMx Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_CLOCKDIVISION_DIV1
  *         @arg @ref LL_TIM_CLOCKDIVISION_DIV2
  *         @arg @ref LL_TIM_CLOCKDIVISION_DIV4
  */
__STATIC_INLINE uint32_t LL_TIM_GetClockDivision(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_BIT(TIMx->CR1, TIM_CR1_CKD));
}

/**
  * @brief  Set the counter value.
  * @note Macro IS_TIM_32B_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a 32 bits counter.
  * @note If dithering is activated, pay attention to the Counter value interpretation
  * @rmtoll CNT          CNT           LL_TIM_SetCounter
  * @param  TIMx Timer instance
  * @param  Counter Counter value (between Min_Data=0 and Max_Data=0xFFFF or 0xFFFFFFFF)
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetCounter(TIM_TypeDef *TIMx, uint32_t Counter)
{
  WRITE_REG(TIMx->CNT, Counter);
}

/**
  * @brief  Get the counter value.
  * @note Macro IS_TIM_32B_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a 32 bits counter.
  * @note If dithering is activated, pay attention to the Counter value interpretation
  * @rmtoll CNT          CNT           LL_TIM_GetCounter
  * @param  TIMx Timer instance
  * @retval Counter value (between Min_Data=0 and Max_Data=0xFFFF or 0xFFFFFFFF)
  */
__STATIC_INLINE uint32_t LL_TIM_GetCounter(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_REG(TIMx->CNT));
}

/**
  * @brief  Get the current direction of the counter
  * @rmtoll CR1          DIR           LL_TIM_GetDirection
  * @param  TIMx Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_COUNTERDIRECTION_UP
  *         @arg @ref LL_TIM_COUNTERDIRECTION_DOWN
  */
__STATIC_INLINE uint32_t LL_TIM_GetDirection(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_BIT(TIMx->CR1, TIM_CR1_DIR));
}

/**
  * @brief  Set the prescaler value.
  * @note The counter clock frequency CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1).
  * @note The prescaler can be changed on the fly as this control register is buffered. The new
  *       prescaler ratio is taken into account at the next update event.
  * @note Helper macro @ref __LL_TIM_CALC_PSC can be used to calculate the Prescaler parameter
  * @rmtoll PSC          PSC           LL_TIM_SetPrescaler
  * @param  TIMx Timer instance
  * @param  Prescaler between Min_Data=0 and Max_Data=65535
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetPrescaler(TIM_TypeDef *TIMx, uint32_t Prescaler)
{
  WRITE_REG(TIMx->PSC, Prescaler);
}

/**
  * @brief  Get the prescaler value.
  * @rmtoll PSC          PSC           LL_TIM_GetPrescaler
  * @param  TIMx Timer instance
  * @retval  Prescaler value between Min_Data=0 and Max_Data=65535
  */
__STATIC_INLINE uint32_t LL_TIM_GetPrescaler(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_REG(TIMx->PSC));
}

/**
  * @brief  Set the auto-reload value.
  * @note The counter is blocked while the auto-reload value is null.
  * @note Macro IS_TIM_32B_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a 32 bits counter.
  * @note Helper macro @ref __LL_TIM_CALC_ARR can be used to calculate the AutoReload parameter
  *       In case dithering is activated,macro __LL_TIM_CALC_ARR_DITHER can be used instead, to calculate the AutoReload
  *       parameter.
  * @rmtoll ARR          ARR           LL_TIM_SetAutoReload
  * @param  TIMx Timer instance
  * @param  AutoReload between Min_Data=0 and Max_Data=65535
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetAutoReload(TIM_TypeDef *TIMx, uint32_t AutoReload)
{
  WRITE_REG(TIMx->ARR, AutoReload);
}

/**
  * @brief  Get the auto-reload value.
  * @rmtoll ARR          ARR           LL_TIM_GetAutoReload
  * @note Macro IS_TIM_32B_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a 32 bits counter.
  * @note If dithering is activated, pay attention to the returned value interpretation
  * @param  TIMx Timer instance
  * @retval Auto-reload value
  */
__STATIC_INLINE uint32_t LL_TIM_GetAutoReload(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_REG(TIMx->ARR));
}

/**
  * @brief  Set the repetition counter value.
  * @note For advanced timer instances RepetitionCounter can be up to 65535.
  * @note Macro IS_TIM_REPETITION_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a repetition counter.
  * @rmtoll RCR          REP           LL_TIM_SetRepetitionCounter
  * @param  TIMx Timer instance
  * @param  RepetitionCounter between Min_Data=0 and Max_Data=255 or 65535 for advanced timer.
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetRepetitionCounter(TIM_TypeDef *TIMx, uint32_t RepetitionCounter)
{
  WRITE_REG(TIMx->RCR, RepetitionCounter);
}

/**
  * @brief  Get the repetition counter value.
  * @note Macro IS_TIM_REPETITION_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a repetition counter.
  * @rmtoll RCR          REP           LL_TIM_GetRepetitionCounter
  * @param  TIMx Timer instance
  * @retval Repetition counter value
  */
__STATIC_INLINE uint32_t LL_TIM_GetRepetitionCounter(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_REG(TIMx->RCR));
}

/**
  * @brief  Force a continuous copy of the update interrupt flag (UIF) into the timer counter register (bit 31).
  * @note This allows both the counter value and a potential roll-over condition signalled by the UIFCPY flag to be read
  *       in an atomic way.
  * @rmtoll CR1          UIFREMAP      LL_TIM_EnableUIFRemap
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableUIFRemap(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->CR1, TIM_CR1_UIFREMAP);
}

/**
  * @brief  Disable update interrupt flag (UIF) remapping.
  * @rmtoll CR1          UIFREMAP      LL_TIM_DisableUIFRemap
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableUIFRemap(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->CR1, TIM_CR1_UIFREMAP);
}

/**
  * @brief  Indicate whether update interrupt flag (UIF) copy is set.
  * @param  Counter Counter value
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveUIFCPY(const uint32_t Counter)
{
  return (((Counter & TIM_CNT_UIFCPY) == (TIM_CNT_UIFCPY)) ? 1UL : 0UL);
}

/**
  * @brief  Enable dithering.
  * @note Macro IS_TIM_DITHERING_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides dithering.
  * @rmtoll CR1          DITHEN          LL_TIM_EnableDithering
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableDithering(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->CR1, TIM_CR1_DITHEN);
}

/**
  * @brief  Disable dithering.
  * @note Macro IS_TIM_DITHERING_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides dithering.
  * @rmtoll CR1          DITHEN          LL_TIM_DisableDithering
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableDithering(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->CR1, TIM_CR1_DITHEN);
}

/**
  * @brief  Indicates whether dithering is activated.
  * @note Macro IS_TIM_DITHERING_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides dithering.
  * @rmtoll CR1          DITHEN          LL_TIM_IsEnabledDithering
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledDithering(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->CR1, TIM_CR1_DITHEN) == (TIM_CR1_DITHEN)) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup TIM_LL_EF_Capture_Compare Capture Compare configuration
  * @{
  */
/**
  * @brief  Enable  the capture/compare control bits (CCxE, CCxNE and OCxM) preload.
  * @note CCxE, CCxNE and OCxM bits are preloaded, after having been written,
  *       they are updated only when a commutation event (COM) occurs.
  * @note Only on channels that have a complementary output.
  * @note Macro IS_TIM_COMMUTATION_EVENT_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance is able to generate a commutation event.
  * @rmtoll CR2          CCPC          LL_TIM_CC_EnablePreload
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_CC_EnablePreload(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->CR2, TIM_CR2_CCPC);
}

/**
  * @brief  Disable  the capture/compare control bits (CCxE, CCxNE and OCxM) preload.
  * @note Macro IS_TIM_COMMUTATION_EVENT_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance is able to generate a commutation event.
  * @rmtoll CR2          CCPC          LL_TIM_CC_DisablePreload
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_CC_DisablePreload(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->CR2, TIM_CR2_CCPC);
}

/**
  * @brief  Indicates whether the capture/compare control bits (CCxE, CCxNE and OCxM) preload is enabled.
  * @rmtoll CR2          CCPC          LL_TIM_CC_IsEnabledPreload
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_CC_IsEnabledPreload(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->CR2, TIM_CR2_CCPC) == (TIM_CR2_CCPC)) ? 1UL : 0UL);
}

/**
  * @brief  Set the updated source of the capture/compare control bits (CCxE, CCxNE and OCxM).
  * @note Macro IS_TIM_COMMUTATION_EVENT_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance is able to generate a commutation event.
  * @rmtoll CR2          CCUS          LL_TIM_CC_SetUpdate
  * @param  TIMx Timer instance
  * @param  CCUpdateSource This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CCUPDATESOURCE_COMG_ONLY
  *         @arg @ref LL_TIM_CCUPDATESOURCE_COMG_AND_TRGI
  * @retval None
  */
__STATIC_INLINE void LL_TIM_CC_SetUpdate(TIM_TypeDef *TIMx, uint32_t CCUpdateSource)
{
  MODIFY_REG(TIMx->CR2, TIM_CR2_CCUS, CCUpdateSource);
}

/**
  * @brief  Set the trigger of the capture/compare DMA request.
  * @rmtoll CR2          CCDS          LL_TIM_CC_SetDMAReqTrigger
  * @param  TIMx Timer instance
  * @param  DMAReqTrigger This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CCDMAREQUEST_CC
  *         @arg @ref LL_TIM_CCDMAREQUEST_UPDATE
  * @retval None
  */
__STATIC_INLINE void LL_TIM_CC_SetDMAReqTrigger(TIM_TypeDef *TIMx, uint32_t DMAReqTrigger)
{
  MODIFY_REG(TIMx->CR2, TIM_CR2_CCDS, DMAReqTrigger);
}

/**
  * @brief  Get actual trigger of the capture/compare DMA request.
  * @rmtoll CR2          CCDS          LL_TIM_CC_GetDMAReqTrigger
  * @param  TIMx Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_CCDMAREQUEST_CC
  *         @arg @ref LL_TIM_CCDMAREQUEST_UPDATE
  */
__STATIC_INLINE uint32_t LL_TIM_CC_GetDMAReqTrigger(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_BIT(TIMx->CR2, TIM_CR2_CCDS));
}

/**
  * @brief  Set the lock level to freeze the
  *         configuration of several capture/compare parameters.
  * @note Macro IS_TIM_BREAK_INSTANCE(TIMx) can be used to check whether or not
  *       the lock mechanism is supported by a timer instance.
  * @rmtoll BDTR         LOCK          LL_TIM_CC_SetLockLevel
  * @param  TIMx Timer instance
  * @param  LockLevel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_LOCKLEVEL_OFF
  *         @arg @ref LL_TIM_LOCKLEVEL_1
  *         @arg @ref LL_TIM_LOCKLEVEL_2
  *         @arg @ref LL_TIM_LOCKLEVEL_3
  * @retval None
  */
__STATIC_INLINE void LL_TIM_CC_SetLockLevel(TIM_TypeDef *TIMx, uint32_t LockLevel)
{
  MODIFY_REG(TIMx->BDTR, TIM_BDTR_LOCK, LockLevel);
}

/**
  * @brief  Enable capture/compare channels.
  * @rmtoll CCER         CC1E          LL_TIM_CC_EnableChannel\n
  *         CCER         CC1NE         LL_TIM_CC_EnableChannel\n
  *         CCER         CC2E          LL_TIM_CC_EnableChannel\n
  *         CCER         CC2NE         LL_TIM_CC_EnableChannel\n
  *         CCER         CC3E          LL_TIM_CC_EnableChannel\n
  *         CCER         CC3NE         LL_TIM_CC_EnableChannel\n
  *         CCER         CC4E          LL_TIM_CC_EnableChannel\n
  *         CCER         CC4NE         LL_TIM_CC_EnableChannel\n
  *         CCER         CC5E          LL_TIM_CC_EnableChannel\n
  *         CCER         CC6E          LL_TIM_CC_EnableChannel
  * @param  TIMx Timer instance
  * @param  Channels This parameter can be a combination of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH1N
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH2N
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH3N
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH4N
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @retval None
  */
__STATIC_INLINE void LL_TIM_CC_EnableChannel(TIM_TypeDef *TIMx, uint32_t Channels)
{
  SET_BIT(TIMx->CCER, Channels);
}

/**
  * @brief  Disable capture/compare channels.
  * @rmtoll CCER         CC1E          LL_TIM_CC_DisableChannel\n
  *         CCER         CC1NE         LL_TIM_CC_DisableChannel\n
  *         CCER         CC2E          LL_TIM_CC_DisableChannel\n
  *         CCER         CC2NE         LL_TIM_CC_DisableChannel\n
  *         CCER         CC3E          LL_TIM_CC_DisableChannel\n
  *         CCER         CC3NE         LL_TIM_CC_DisableChannel\n
  *         CCER         CC4E          LL_TIM_CC_DisableChannel\n
  *         CCER         CC4NE         LL_TIM_CC_DisableChannel\n
  *         CCER         CC5E          LL_TIM_CC_DisableChannel\n
  *         CCER         CC6E          LL_TIM_CC_DisableChannel
  * @param  TIMx Timer instance
  * @param  Channels This parameter can be a combination of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH1N
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH2N
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH3N
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH4N
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @retval None
  */
__STATIC_INLINE void LL_TIM_CC_DisableChannel(TIM_TypeDef *TIMx, uint32_t Channels)
{
  CLEAR_BIT(TIMx->CCER, Channels);
}

/**
  * @brief  Indicate whether channel(s) is(are) enabled.
  * @rmtoll CCER         CC1E          LL_TIM_CC_IsEnabledChannel\n
  *         CCER         CC1NE         LL_TIM_CC_IsEnabledChannel\n
  *         CCER         CC2E          LL_TIM_CC_IsEnabledChannel\n
  *         CCER         CC2NE         LL_TIM_CC_IsEnabledChannel\n
  *         CCER         CC3E          LL_TIM_CC_IsEnabledChannel\n
  *         CCER         CC3NE         LL_TIM_CC_IsEnabledChannel\n
  *         CCER         CC4E          LL_TIM_CC_IsEnabledChannel\n
  *         CCER         CC4NE         LL_TIM_CC_IsEnabledChannel\n
  *         CCER         CC5E          LL_TIM_CC_IsEnabledChannel\n
  *         CCER         CC6E          LL_TIM_CC_IsEnabledChannel
  * @param  TIMx Timer instance
  * @param  Channels This parameter can be a combination of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH1N
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH2N
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH3N
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH4N
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_CC_IsEnabledChannel(const TIM_TypeDef *TIMx, uint32_t Channels)
{
  return ((READ_BIT(TIMx->CCER, Channels) == (Channels)) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup TIM_LL_EF_Output_Channel Output channel configuration
  * @{
  */
/**
  * @brief  Configure an output channel.
  * @rmtoll CCMR1        CC1S          LL_TIM_OC_ConfigOutput\n
  *         CCMR1        CC2S          LL_TIM_OC_ConfigOutput\n
  *         CCMR2        CC3S          LL_TIM_OC_ConfigOutput\n
  *         CCMR2        CC4S          LL_TIM_OC_ConfigOutput\n
  *         CCMR3        CC5S          LL_TIM_OC_ConfigOutput\n
  *         CCMR3        CC6S          LL_TIM_OC_ConfigOutput\n
  *         CCER         CC1P          LL_TIM_OC_ConfigOutput\n
  *         CCER         CC2P          LL_TIM_OC_ConfigOutput\n
  *         CCER         CC3P          LL_TIM_OC_ConfigOutput\n
  *         CCER         CC4P          LL_TIM_OC_ConfigOutput\n
  *         CCER         CC5P          LL_TIM_OC_ConfigOutput\n
  *         CCER         CC6P          LL_TIM_OC_ConfigOutput\n
  *         CR2          OIS1          LL_TIM_OC_ConfigOutput\n
  *         CR2          OIS2          LL_TIM_OC_ConfigOutput\n
  *         CR2          OIS3          LL_TIM_OC_ConfigOutput\n
  *         CR2          OIS4          LL_TIM_OC_ConfigOutput\n
  *         CR2          OIS5          LL_TIM_OC_ConfigOutput\n
  *         CR2          OIS6          LL_TIM_OC_ConfigOutput
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @param  Configuration This parameter must be a combination of all the following values:
  *         @arg @ref LL_TIM_OCPOLARITY_HIGH or @ref LL_TIM_OCPOLARITY_LOW
  *         @arg @ref LL_TIM_OCIDLESTATE_LOW or @ref LL_TIM_OCIDLESTATE_HIGH
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_ConfigOutput(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t Configuration)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  CLEAR_BIT(*pReg, (TIM_CCMR1_CC1S << SHIFT_TAB_OCxx[iChannel]));
  MODIFY_REG(TIMx->CCER, (TIM_CCER_CC1P << SHIFT_TAB_CCxP[iChannel]),
             (Configuration & TIM_CCER_CC1P) << SHIFT_TAB_CCxP[iChannel]);
  MODIFY_REG(TIMx->CR2, (TIM_CR2_OIS1 << SHIFT_TAB_OISx[iChannel]),
             (Configuration & TIM_CR2_OIS1) << SHIFT_TAB_OISx[iChannel]);
}

/**
  * @brief  Define the behavior of the output reference signal OCxREF from which
  *         OCx and OCxN (when relevant) are derived.
  * @rmtoll CCMR1        OC1M          LL_TIM_OC_SetMode\n
  *         CCMR1        OC2M          LL_TIM_OC_SetMode\n
  *         CCMR2        OC3M          LL_TIM_OC_SetMode\n
  *         CCMR2        OC4M          LL_TIM_OC_SetMode\n
  *         CCMR3        OC5M          LL_TIM_OC_SetMode\n
  *         CCMR3        OC6M          LL_TIM_OC_SetMode
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @param  Mode This parameter can be one of the following values:
  *         @arg @ref LL_TIM_OCMODE_FROZEN
  *         @arg @ref LL_TIM_OCMODE_ACTIVE
  *         @arg @ref LL_TIM_OCMODE_INACTIVE
  *         @arg @ref LL_TIM_OCMODE_TOGGLE
  *         @arg @ref LL_TIM_OCMODE_FORCED_INACTIVE
  *         @arg @ref LL_TIM_OCMODE_FORCED_ACTIVE
  *         @arg @ref LL_TIM_OCMODE_PWM1
  *         @arg @ref LL_TIM_OCMODE_PWM2
  *         @arg @ref LL_TIM_OCMODE_RETRIG_OPM1
  *         @arg @ref LL_TIM_OCMODE_RETRIG_OPM2
  *         @arg @ref LL_TIM_OCMODE_COMBINED_PWM1
  *         @arg @ref LL_TIM_OCMODE_COMBINED_PWM2
  *         @arg @ref LL_TIM_OCMODE_ASYMMETRIC_PWM1
  *         @arg @ref LL_TIM_OCMODE_ASYMMETRIC_PWM2
  *         @arg @ref LL_TIM_OCMODE_PULSE_ON_COMPARE   (for channel 3 or channel 4 only)
  *         @arg @ref LL_TIM_OCMODE_DIRECTION_OUTPUT   (for channel 3 or channel 4 only)
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_SetMode(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t Mode)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  MODIFY_REG(*pReg, ((TIM_CCMR1_OC1M  | TIM_CCMR1_CC1S) << SHIFT_TAB_OCxx[iChannel]), Mode << SHIFT_TAB_OCxx[iChannel]);
}

/**
  * @brief  Get the output compare mode of an output channel.
  * @rmtoll CCMR1        OC1M          LL_TIM_OC_GetMode\n
  *         CCMR1        OC2M          LL_TIM_OC_GetMode\n
  *         CCMR2        OC3M          LL_TIM_OC_GetMode\n
  *         CCMR2        OC4M          LL_TIM_OC_GetMode\n
  *         CCMR3        OC5M          LL_TIM_OC_GetMode\n
  *         CCMR3        OC6M          LL_TIM_OC_GetMode
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_OCMODE_FROZEN
  *         @arg @ref LL_TIM_OCMODE_ACTIVE
  *         @arg @ref LL_TIM_OCMODE_INACTIVE
  *         @arg @ref LL_TIM_OCMODE_TOGGLE
  *         @arg @ref LL_TIM_OCMODE_FORCED_INACTIVE
  *         @arg @ref LL_TIM_OCMODE_FORCED_ACTIVE
  *         @arg @ref LL_TIM_OCMODE_PWM1
  *         @arg @ref LL_TIM_OCMODE_PWM2
  *         @arg @ref LL_TIM_OCMODE_RETRIG_OPM1
  *         @arg @ref LL_TIM_OCMODE_RETRIG_OPM2
  *         @arg @ref LL_TIM_OCMODE_COMBINED_PWM1
  *         @arg @ref LL_TIM_OCMODE_COMBINED_PWM2
  *         @arg @ref LL_TIM_OCMODE_ASYMMETRIC_PWM1
  *         @arg @ref LL_TIM_OCMODE_ASYMMETRIC_PWM2
  *         @arg @ref LL_TIM_OCMODE_PULSE_ON_COMPARE   (for channel 3 or channel 4 only)
  *         @arg @ref LL_TIM_OCMODE_DIRECTION_OUTPUT   (for channel 3 or channel 4 only)
  */
__STATIC_INLINE uint32_t LL_TIM_OC_GetMode(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  const __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  return (READ_BIT(*pReg, ((TIM_CCMR1_OC1M | TIM_CCMR1_CC1S) << SHIFT_TAB_OCxx[iChannel])) >> SHIFT_TAB_OCxx[iChannel]);
}

/**
  * @brief  Set the polarity of an output channel.
  * @rmtoll CCER         CC1P          LL_TIM_OC_SetPolarity\n
  *         CCER         CC1NP         LL_TIM_OC_SetPolarity\n
  *         CCER         CC2P          LL_TIM_OC_SetPolarity\n
  *         CCER         CC2NP         LL_TIM_OC_SetPolarity\n
  *         CCER         CC3P          LL_TIM_OC_SetPolarity\n
  *         CCER         CC3NP         LL_TIM_OC_SetPolarity\n
  *         CCER         CC4P          LL_TIM_OC_SetPolarity\n
  *         CCER         CC4NP         LL_TIM_OC_SetPolarity\n
  *         CCER         CC5P          LL_TIM_OC_SetPolarity\n
  *         CCER         CC6P          LL_TIM_OC_SetPolarity
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH1N
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH2N
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH3N
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH4N
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @param  Polarity This parameter can be one of the following values:
  *         @arg @ref LL_TIM_OCPOLARITY_HIGH
  *         @arg @ref LL_TIM_OCPOLARITY_LOW
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_SetPolarity(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t Polarity)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  MODIFY_REG(TIMx->CCER, (TIM_CCER_CC1P << SHIFT_TAB_CCxP[iChannel]),  Polarity << SHIFT_TAB_CCxP[iChannel]);
}

/**
  * @brief  Get the polarity of an output channel.
  * @rmtoll CCER         CC1P          LL_TIM_OC_GetPolarity\n
  *         CCER         CC1NP         LL_TIM_OC_GetPolarity\n
  *         CCER         CC2P          LL_TIM_OC_GetPolarity\n
  *         CCER         CC2NP         LL_TIM_OC_GetPolarity\n
  *         CCER         CC3P          LL_TIM_OC_GetPolarity\n
  *         CCER         CC3NP         LL_TIM_OC_GetPolarity\n
  *         CCER         CC4P          LL_TIM_OC_GetPolarity\n
  *         CCER         CC4NP         LL_TIM_OC_GetPolarity\n
  *         CCER         CC5P          LL_TIM_OC_GetPolarity\n
  *         CCER         CC6P          LL_TIM_OC_GetPolarity
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH1N
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH2N
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH3N
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH4N
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_OCPOLARITY_HIGH
  *         @arg @ref LL_TIM_OCPOLARITY_LOW
  */
__STATIC_INLINE uint32_t LL_TIM_OC_GetPolarity(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  return (READ_BIT(TIMx->CCER, (TIM_CCER_CC1P << SHIFT_TAB_CCxP[iChannel])) >> SHIFT_TAB_CCxP[iChannel]);
}

/**
  * @brief  Set the IDLE state of an output channel
  * @note This function is significant only for the timer instances
  *       supporting the break feature. Macro IS_TIM_BREAK_INSTANCE(TIMx)
  *       can be used to check whether or not a timer instance provides
  *       a break input.
  * @rmtoll CR2         OIS1          LL_TIM_OC_SetIdleState\n
  *         CR2         OIS2N         LL_TIM_OC_SetIdleState\n
  *         CR2         OIS2          LL_TIM_OC_SetIdleState\n
  *         CR2         OIS2N         LL_TIM_OC_SetIdleState\n
  *         CR2         OIS3          LL_TIM_OC_SetIdleState\n
  *         CR2         OIS3N         LL_TIM_OC_SetIdleState\n
  *         CR2         OIS4          LL_TIM_OC_SetIdleState\n
  *         CR2         OIS4N         LL_TIM_OC_SetIdleState\n
  *         CR2         OIS5          LL_TIM_OC_SetIdleState\n
  *         CR2         OIS6          LL_TIM_OC_SetIdleState
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH1N
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH2N
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH3N
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH4N
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @param  IdleState This parameter can be one of the following values:
  *         @arg @ref LL_TIM_OCIDLESTATE_LOW
  *         @arg @ref LL_TIM_OCIDLESTATE_HIGH
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_SetIdleState(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t IdleState)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  MODIFY_REG(TIMx->CR2, (TIM_CR2_OIS1 << SHIFT_TAB_OISx[iChannel]),  IdleState << SHIFT_TAB_OISx[iChannel]);
}

/**
  * @brief  Get the IDLE state of an output channel
  * @rmtoll CR2         OIS1          LL_TIM_OC_GetIdleState\n
  *         CR2         OIS2N         LL_TIM_OC_GetIdleState\n
  *         CR2         OIS2          LL_TIM_OC_GetIdleState\n
  *         CR2         OIS2N         LL_TIM_OC_GetIdleState\n
  *         CR2         OIS3          LL_TIM_OC_GetIdleState\n
  *         CR2         OIS3N         LL_TIM_OC_GetIdleState\n
  *         CR2         OIS4          LL_TIM_OC_GetIdleState\n
  *         CR2         OIS4N         LL_TIM_OC_GetIdleState\n
  *         CR2         OIS5          LL_TIM_OC_GetIdleState\n
  *         CR2         OIS6          LL_TIM_OC_GetIdleState
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH1N
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH2N
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH3N
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH4N
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_OCIDLESTATE_LOW
  *         @arg @ref LL_TIM_OCIDLESTATE_HIGH
  */
__STATIC_INLINE uint32_t LL_TIM_OC_GetIdleState(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  return (READ_BIT(TIMx->CR2, (TIM_CR2_OIS1 << SHIFT_TAB_OISx[iChannel])) >> SHIFT_TAB_OISx[iChannel]);
}

/**
  * @brief  Enable fast mode for the output channel.
  * @note Acts only if the channel is configured in PWM1 or PWM2 mode.
  * @rmtoll CCMR1        OC1FE          LL_TIM_OC_EnableFast\n
  *         CCMR1        OC2FE          LL_TIM_OC_EnableFast\n
  *         CCMR2        OC3FE          LL_TIM_OC_EnableFast\n
  *         CCMR2        OC4FE          LL_TIM_OC_EnableFast\n
  *         CCMR3        OC5FE          LL_TIM_OC_EnableFast\n
  *         CCMR3        OC6FE          LL_TIM_OC_EnableFast
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_EnableFast(TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  SET_BIT(*pReg, (TIM_CCMR1_OC1FE << SHIFT_TAB_OCxx[iChannel]));

}

/**
  * @brief  Disable fast mode for the output channel.
  * @rmtoll CCMR1        OC1FE          LL_TIM_OC_DisableFast\n
  *         CCMR1        OC2FE          LL_TIM_OC_DisableFast\n
  *         CCMR2        OC3FE          LL_TIM_OC_DisableFast\n
  *         CCMR2        OC4FE          LL_TIM_OC_DisableFast\n
  *         CCMR3        OC5FE          LL_TIM_OC_DisableFast\n
  *         CCMR3        OC6FE          LL_TIM_OC_DisableFast
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_DisableFast(TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  CLEAR_BIT(*pReg, (TIM_CCMR1_OC1FE << SHIFT_TAB_OCxx[iChannel]));

}

/**
  * @brief  Indicates whether fast mode is enabled for the output channel.
  * @rmtoll CCMR1        OC1FE          LL_TIM_OC_IsEnabledFast\n
  *         CCMR1        OC2FE          LL_TIM_OC_IsEnabledFast\n
  *         CCMR2        OC3FE          LL_TIM_OC_IsEnabledFast\n
  *         CCMR2        OC4FE          LL_TIM_OC_IsEnabledFast\n
  *         CCMR3        OC5FE          LL_TIM_OC_IsEnabledFast\n
  *         CCMR3        OC6FE          LL_TIM_OC_IsEnabledFast
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_OC_IsEnabledFast(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  const __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  uint32_t bitfield = TIM_CCMR1_OC1FE << SHIFT_TAB_OCxx[iChannel];
  return ((READ_BIT(*pReg, bitfield) == bitfield) ? 1UL : 0UL);
}

/**
  * @brief  Enable compare register (TIMx_CCRx) preload for the output channel.
  * @rmtoll CCMR1        OC1PE          LL_TIM_OC_EnablePreload\n
  *         CCMR1        OC2PE          LL_TIM_OC_EnablePreload\n
  *         CCMR2        OC3PE          LL_TIM_OC_EnablePreload\n
  *         CCMR2        OC4PE          LL_TIM_OC_EnablePreload\n
  *         CCMR3        OC5PE          LL_TIM_OC_EnablePreload\n
  *         CCMR3        OC6PE          LL_TIM_OC_EnablePreload
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_EnablePreload(TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  SET_BIT(*pReg, (TIM_CCMR1_OC1PE << SHIFT_TAB_OCxx[iChannel]));
}

/**
  * @brief  Disable compare register (TIMx_CCRx) preload for the output channel.
  * @rmtoll CCMR1        OC1PE          LL_TIM_OC_DisablePreload\n
  *         CCMR1        OC2PE          LL_TIM_OC_DisablePreload\n
  *         CCMR2        OC3PE          LL_TIM_OC_DisablePreload\n
  *         CCMR2        OC4PE          LL_TIM_OC_DisablePreload\n
  *         CCMR3        OC5PE          LL_TIM_OC_DisablePreload\n
  *         CCMR3        OC6PE          LL_TIM_OC_DisablePreload
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_DisablePreload(TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  CLEAR_BIT(*pReg, (TIM_CCMR1_OC1PE << SHIFT_TAB_OCxx[iChannel]));
}

/**
  * @brief  Indicates whether compare register (TIMx_CCRx) preload is enabled for the output channel.
  * @rmtoll CCMR1        OC1PE          LL_TIM_OC_IsEnabledPreload\n
  *         CCMR1        OC2PE          LL_TIM_OC_IsEnabledPreload\n
  *         CCMR2        OC3PE          LL_TIM_OC_IsEnabledPreload\n
  *         CCMR2        OC4PE          LL_TIM_OC_IsEnabledPreload\n
  *         CCMR3        OC5PE          LL_TIM_OC_IsEnabledPreload\n
  *         CCMR3        OC6PE          LL_TIM_OC_IsEnabledPreload
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_OC_IsEnabledPreload(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  const __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  uint32_t bitfield = TIM_CCMR1_OC1PE << SHIFT_TAB_OCxx[iChannel];
  return ((READ_BIT(*pReg, bitfield) == bitfield) ? 1UL : 0UL);
}

/**
  * @brief  Enable clearing the output channel on an external event.
  * @note This function can only be used in Output compare and PWM modes. It does not work in Forced mode.
  * @note Macro IS_TIM_OCXREF_CLEAR_INSTANCE(TIMx) can be used to check whether
  *       or not a timer instance can clear the OCxREF signal on an external event.
  * @rmtoll CCMR1        OC1CE          LL_TIM_OC_EnableClear\n
  *         CCMR1        OC2CE          LL_TIM_OC_EnableClear\n
  *         CCMR2        OC3CE          LL_TIM_OC_EnableClear\n
  *         CCMR2        OC4CE          LL_TIM_OC_EnableClear\n
  *         CCMR3        OC5CE          LL_TIM_OC_EnableClear\n
  *         CCMR3        OC6CE          LL_TIM_OC_EnableClear
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_EnableClear(TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  SET_BIT(*pReg, (TIM_CCMR1_OC1CE << SHIFT_TAB_OCxx[iChannel]));
}

/**
  * @brief  Disable clearing the output channel on an external event.
  * @note Macro IS_TIM_OCXREF_CLEAR_INSTANCE(TIMx) can be used to check whether
  *       or not a timer instance can clear the OCxREF signal on an external event.
  * @rmtoll CCMR1        OC1CE          LL_TIM_OC_DisableClear\n
  *         CCMR1        OC2CE          LL_TIM_OC_DisableClear\n
  *         CCMR2        OC3CE          LL_TIM_OC_DisableClear\n
  *         CCMR2        OC4CE          LL_TIM_OC_DisableClear\n
  *         CCMR3        OC5CE          LL_TIM_OC_DisableClear\n
  *         CCMR3        OC6CE          LL_TIM_OC_DisableClear
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_DisableClear(TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  CLEAR_BIT(*pReg, (TIM_CCMR1_OC1CE << SHIFT_TAB_OCxx[iChannel]));
}

/**
  * @brief  Indicates clearing the output channel on an external event is enabled for the output channel.
  * @note This function enables clearing the output channel on an external event.
  * @note This function can only be used in Output compare and PWM modes. It does not work in Forced mode.
  * @note Macro IS_TIM_OCXREF_CLEAR_INSTANCE(TIMx) can be used to check whether
  *       or not a timer instance can clear the OCxREF signal on an external event.
  * @rmtoll CCMR1        OC1CE          LL_TIM_OC_IsEnabledClear\n
  *         CCMR1        OC2CE          LL_TIM_OC_IsEnabledClear\n
  *         CCMR2        OC3CE          LL_TIM_OC_IsEnabledClear\n
  *         CCMR2        OC4CE          LL_TIM_OC_IsEnabledClear\n
  *         CCMR3        OC5CE          LL_TIM_OC_IsEnabledClear\n
  *         CCMR3        OC6CE          LL_TIM_OC_IsEnabledClear
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  *         @arg @ref LL_TIM_CHANNEL_CH5
  *         @arg @ref LL_TIM_CHANNEL_CH6
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_OC_IsEnabledClear(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  const __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  uint32_t bitfield = TIM_CCMR1_OC1CE << SHIFT_TAB_OCxx[iChannel];
  return ((READ_BIT(*pReg, bitfield) == bitfield) ? 1UL : 0UL);
}

/**
  * @brief  Set the dead-time delay (delay inserted between the rising edge of the OCxREF signal and the rising edge of
  *         the Ocx and OCxN signals).
  * @note Macro IS_TIM_BREAK_INSTANCE(TIMx) can be used to check whether or not
  *       dead-time insertion feature is supported by a timer instance.
  * @note Helper macro @ref __LL_TIM_CALC_DEADTIME can be used to calculate the DeadTime parameter
  * @rmtoll BDTR         DTG           LL_TIM_OC_SetDeadTime
  * @param  TIMx Timer instance
  * @param  DeadTime between Min_Data=0 and Max_Data=255
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_SetDeadTime(TIM_TypeDef *TIMx, uint32_t DeadTime)
{
  MODIFY_REG(TIMx->BDTR, TIM_BDTR_DTG, DeadTime);
}

/**
  * @brief  Set compare value for output channel 1 (TIMx_CCR1).
  * @note In 32-bit timer implementations compare value can be between 0x00000000 and 0xFFFFFFFF.
  * @note Macro IS_TIM_32B_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a 32 bits counter.
  * @note Macro IS_TIM_CC1_INSTANCE(TIMx) can be used to check whether or not
  *       output channel 1 is supported by a timer instance.
  * @note If dithering is activated, CompareValue can be calculated with macro @ref __LL_TIM_CALC_DELAY_DITHER .
  * @rmtoll CCR1         CCR1          LL_TIM_OC_SetCompareCH1
  * @param  TIMx Timer instance
  * @param  CompareValue between Min_Data=0 and Max_Data=65535
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_SetCompareCH1(TIM_TypeDef *TIMx, uint32_t CompareValue)
{
  WRITE_REG(TIMx->CCR1, CompareValue);
}

/**
  * @brief  Set compare value for output channel 2 (TIMx_CCR2).
  * @note In 32-bit timer implementations compare value can be between 0x00000000 and 0xFFFFFFFF.
  * @note Macro IS_TIM_32B_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a 32 bits counter.
  * @note Macro IS_TIM_CC2_INSTANCE(TIMx) can be used to check whether or not
  *       output channel 2 is supported by a timer instance.
  * @note If dithering is activated, CompareValue can be calculated with macro @ref __LL_TIM_CALC_DELAY_DITHER .
  * @rmtoll CCR2         CCR2          LL_TIM_OC_SetCompareCH2
  * @param  TIMx Timer instance
  * @param  CompareValue between Min_Data=0 and Max_Data=65535
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_SetCompareCH2(TIM_TypeDef *TIMx, uint32_t CompareValue)
{
  WRITE_REG(TIMx->CCR2, CompareValue);
}

/**
  * @brief  Set compare value for output channel 3 (TIMx_CCR3).
  * @note In 32-bit timer implementations compare value can be between 0x00000000 and 0xFFFFFFFF.
  * @note Macro IS_TIM_32B_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a 32 bits counter.
  * @note Macro IS_TIM_CC3_INSTANCE(TIMx) can be used to check whether or not
  *       output channel is supported by a timer instance.
  * @note If dithering is activated, CompareValue can be calculated with macro @ref __LL_TIM_CALC_DELAY_DITHER .
  * @rmtoll CCR3         CCR3          LL_TIM_OC_SetCompareCH3
  * @param  TIMx Timer instance
  * @param  CompareValue between Min_Data=0 and Max_Data=65535
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_SetCompareCH3(TIM_TypeDef *TIMx, uint32_t CompareValue)
{
  WRITE_REG(TIMx->CCR3, CompareValue);
}

/**
  * @brief  Set compare value for output channel 4 (TIMx_CCR4).
  * @note In 32-bit timer implementations compare value can be between 0x00000000 and 0xFFFFFFFF.
  * @note Macro IS_TIM_32B_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a 32 bits counter.
  * @note Macro IS_TIM_CC4_INSTANCE(TIMx) can be used to check whether or not
  *       output channel 4 is supported by a timer instance.
  * @note If dithering is activated, CompareValue can be calculated with macro @ref __LL_TIM_CALC_DELAY_DITHER .
  * @rmtoll CCR4         CCR4          LL_TIM_OC_SetCompareCH4
  * @param  TIMx Timer instance
  * @param  CompareValue between Min_Data=0 and Max_Data=65535
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_SetCompareCH4(TIM_TypeDef *TIMx, uint32_t CompareValue)
{
  WRITE_REG(TIMx->CCR4, CompareValue);
}

/**
  * @brief  Set compare value for output channel 5 (TIMx_CCR5).
  * @note Macro IS_TIM_CC5_INSTANCE(TIMx) can be used to check whether or not
  *       output channel 5 is supported by a timer instance.
  * @note If dithering is activated, CompareValue can be calculated with macro @ref __LL_TIM_CALC_DELAY_DITHER .
  * @rmtoll CCR5         CCR5          LL_TIM_OC_SetCompareCH5
  * @param  TIMx Timer instance
  * @param  CompareValue between Min_Data=0 and Max_Data=65535
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_SetCompareCH5(TIM_TypeDef *TIMx, uint32_t CompareValue)
{
  MODIFY_REG(TIMx->CCR5, TIM_CCR5_CCR5, CompareValue);
}

/**
  * @brief  Set compare value for output channel 6 (TIMx_CCR6).
  * @note Macro IS_TIM_CC6_INSTANCE(TIMx) can be used to check whether or not
  *       output channel 6 is supported by a timer instance.
  * @note If dithering is activated, CompareValue can be calculated with macro @ref __LL_TIM_CALC_DELAY_DITHER .
  * @rmtoll CCR6         CCR6          LL_TIM_OC_SetCompareCH6
  * @param  TIMx Timer instance
  * @param  CompareValue between Min_Data=0 and Max_Data=65535
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_SetCompareCH6(TIM_TypeDef *TIMx, uint32_t CompareValue)
{
  WRITE_REG(TIMx->CCR6, CompareValue);
}

/**
  * @brief  Get compare value (TIMx_CCR1) set for  output channel 1.
  * @note In 32-bit timer implementations returned compare value can be between 0x00000000 and 0xFFFFFFFF.
  * @note Macro IS_TIM_32B_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a 32 bits counter.
  * @note Macro IS_TIM_CC1_INSTANCE(TIMx) can be used to check whether or not
  *       output channel 1 is supported by a timer instance.
  * @note If dithering is activated, pay attention to the returned value interpretation.
  * @rmtoll CCR1         CCR1          LL_TIM_OC_GetCompareCH1
  * @param  TIMx Timer instance
  * @retval CompareValue (between Min_Data=0 and Max_Data=65535)
  */
__STATIC_INLINE uint32_t LL_TIM_OC_GetCompareCH1(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_REG(TIMx->CCR1));
}

/**
  * @brief  Get compare value (TIMx_CCR2) set for  output channel 2.
  * @note In 32-bit timer implementations returned compare value can be between 0x00000000 and 0xFFFFFFFF.
  * @note Macro IS_TIM_32B_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a 32 bits counter.
  * @note Macro IS_TIM_CC2_INSTANCE(TIMx) can be used to check whether or not
  *       output channel 2 is supported by a timer instance.
  * @note If dithering is activated, pay attention to the returned value interpretation.
  * @rmtoll CCR2         CCR2          LL_TIM_OC_GetCompareCH2
  * @param  TIMx Timer instance
  * @retval CompareValue (between Min_Data=0 and Max_Data=65535)
  */
__STATIC_INLINE uint32_t LL_TIM_OC_GetCompareCH2(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_REG(TIMx->CCR2));
}

/**
  * @brief  Get compare value (TIMx_CCR3) set for  output channel 3.
  * @note In 32-bit timer implementations returned compare value can be between 0x00000000 and 0xFFFFFFFF.
  * @note Macro IS_TIM_32B_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a 32 bits counter.
  * @note Macro IS_TIM_CC3_INSTANCE(TIMx) can be used to check whether or not
  *       output channel 3 is supported by a timer instance.
  * @note If dithering is activated, pay attention to the returned value interpretation.
  * @rmtoll CCR3         CCR3          LL_TIM_OC_GetCompareCH3
  * @param  TIMx Timer instance
  * @retval CompareValue (between Min_Data=0 and Max_Data=65535)
  */
__STATIC_INLINE uint32_t LL_TIM_OC_GetCompareCH3(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_REG(TIMx->CCR3));
}

/**
  * @brief  Get compare value (TIMx_CCR4) set for  output channel 4.
  * @note In 32-bit timer implementations returned compare value can be between 0x00000000 and 0xFFFFFFFF.
  * @note Macro IS_TIM_32B_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a 32 bits counter.
  * @note Macro IS_TIM_CC4_INSTANCE(TIMx) can be used to check whether or not
  *       output channel 4 is supported by a timer instance.
  * @note If dithering is activated, pay attention to the returned value interpretation.
  * @rmtoll CCR4         CCR4          LL_TIM_OC_GetCompareCH4
  * @param  TIMx Timer instance
  * @retval CompareValue (between Min_Data=0 and Max_Data=65535)
  */
__STATIC_INLINE uint32_t LL_TIM_OC_GetCompareCH4(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_REG(TIMx->CCR4));
}

/**
  * @brief  Get compare value (TIMx_CCR5) set for  output channel 5.
  * @note Macro IS_TIM_CC5_INSTANCE(TIMx) can be used to check whether or not
  *       output channel 5 is supported by a timer instance.
  * @note If dithering is activated, pay attention to the returned value interpretation.
  * @rmtoll CCR5         CCR5          LL_TIM_OC_GetCompareCH5
  * @param  TIMx Timer instance
  * @retval CompareValue (between Min_Data=0 and Max_Data=65535)
  */
__STATIC_INLINE uint32_t LL_TIM_OC_GetCompareCH5(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_BIT(TIMx->CCR5, TIM_CCR5_CCR5));
}

/**
  * @brief  Get compare value (TIMx_CCR6) set for  output channel 6.
  * @note Macro IS_TIM_CC6_INSTANCE(TIMx) can be used to check whether or not
  *       output channel 6 is supported by a timer instance.
  * @note If dithering is activated, pay attention to the returned value interpretation.
  * @rmtoll CCR6         CCR6          LL_TIM_OC_GetCompareCH6
  * @param  TIMx Timer instance
  * @retval CompareValue (between Min_Data=0 and Max_Data=65535)
  */
__STATIC_INLINE uint32_t LL_TIM_OC_GetCompareCH6(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_REG(TIMx->CCR6));
}

/**
  * @brief  Select on which reference signal the OC5REF is combined to.
  * @note Macro IS_TIM_COMBINED3PHASEPWM_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports the combined 3-phase PWM mode.
  * @rmtoll CCR5         GC5C3          LL_TIM_SetCH5CombinedChannels\n
  *         CCR5         GC5C2          LL_TIM_SetCH5CombinedChannels\n
  *         CCR5         GC5C1          LL_TIM_SetCH5CombinedChannels
  * @param  TIMx Timer instance
  * @param  GroupCH5 This parameter can be a combination of the following values:
  *         @arg @ref LL_TIM_GROUPCH5_NONE
  *         @arg @ref LL_TIM_GROUPCH5_OC1REFC
  *         @arg @ref LL_TIM_GROUPCH5_OC2REFC
  *         @arg @ref LL_TIM_GROUPCH5_OC3REFC
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetCH5CombinedChannels(TIM_TypeDef *TIMx, uint32_t GroupCH5)
{
  MODIFY_REG(TIMx->CCR5, (TIM_CCR5_GC5C3 | TIM_CCR5_GC5C2 | TIM_CCR5_GC5C1), GroupCH5);
}

/**
  * @brief  Set the pulse on compare pulse width prescaler.
  * @note Macro IS_TIM_PULSEONCOMPARE_INSTANCE(TIMx) can be used to check
  *       whether or not the pulse on compare feature is supported by the timer
  *       instance.
  * @rmtoll ECR          PWPRSC           LL_TIM_OC_SetPulseWidthPrescaler
  * @param  TIMx Timer instance
  * @param  PulseWidthPrescaler This parameter can be one of the following values:
  *         @arg @ref LL_TIM_PWPRSC_X1
  *         @arg @ref LL_TIM_PWPRSC_X2
  *         @arg @ref LL_TIM_PWPRSC_X4
  *         @arg @ref LL_TIM_PWPRSC_X8
  *         @arg @ref LL_TIM_PWPRSC_X16
  *         @arg @ref LL_TIM_PWPRSC_X32
  *         @arg @ref LL_TIM_PWPRSC_X64
  *         @arg @ref LL_TIM_PWPRSC_X128
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_SetPulseWidthPrescaler(TIM_TypeDef *TIMx, uint32_t PulseWidthPrescaler)
{
  MODIFY_REG(TIMx->ECR, TIM_ECR_PWPRSC, PulseWidthPrescaler);
}

/**
  * @brief  Get the pulse on compare pulse width prescaler.
  * @note Macro IS_TIM_PULSEONCOMPARE_INSTANCE(TIMx) can be used to check
  *       whether or not the pulse on compare feature is supported by the timer
  *       instance.
  * @rmtoll ECR          PWPRSC           LL_TIM_OC_GetPulseWidthPrescaler
  * @param  TIMx Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_PWPRSC_X1
  *         @arg @ref LL_TIM_PWPRSC_X2
  *         @arg @ref LL_TIM_PWPRSC_X4
  *         @arg @ref LL_TIM_PWPRSC_X8
  *         @arg @ref LL_TIM_PWPRSC_X16
  *         @arg @ref LL_TIM_PWPRSC_X32
  *         @arg @ref LL_TIM_PWPRSC_X64
  *         @arg @ref LL_TIM_PWPRSC_X128
  */
__STATIC_INLINE uint32_t LL_TIM_OC_GetPulseWidthPrescaler(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_BIT(TIMx->ECR, TIM_ECR_PWPRSC));
}

/**
  * @brief  Set the pulse on compare pulse width duration.
  * @note Macro IS_TIM_PULSEONCOMPARE_INSTANCE(TIMx) can be used to check
  *       whether or not the pulse on compare feature is supported by the timer
  *       instance.
  * @rmtoll ECR          PW           LL_TIM_OC_SetPulseWidth
  * @param  TIMx Timer instance
  * @param  PulseWidth This parameter can be between Min_Data=0 and Max_Data=255
  * @retval None
  */
__STATIC_INLINE void LL_TIM_OC_SetPulseWidth(TIM_TypeDef *TIMx, uint32_t PulseWidth)
{
  MODIFY_REG(TIMx->ECR, TIM_ECR_PW, PulseWidth << TIM_ECR_PW_Pos);
}

/**
  * @brief  Get the pulse on compare pulse width duration.
  * @note Macro IS_TIM_PULSEONCOMPARE_INSTANCE(TIMx) can be used to check
  *       whether or not the pulse on compare feature is supported by the timer
  *       instance.
  * @rmtoll ECR          PW           LL_TIM_OC_GetPulseWidth
  * @param  TIMx Timer instance
  * @retval Returned value can be between Min_Data=0 and Max_Data=255:
  */
__STATIC_INLINE uint32_t LL_TIM_OC_GetPulseWidth(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_BIT(TIMx->ECR, TIM_ECR_PW));
}

/**
  * @}
  */

/** @defgroup TIM_LL_EF_Input_Channel Input channel configuration
  * @{
  */
/**
  * @brief  Configure input channel.
  * @rmtoll CCMR1        CC1S          LL_TIM_IC_Config\n
  *         CCMR1        IC1PSC        LL_TIM_IC_Config\n
  *         CCMR1        IC1F          LL_TIM_IC_Config\n
  *         CCMR1        CC2S          LL_TIM_IC_Config\n
  *         CCMR1        IC2PSC        LL_TIM_IC_Config\n
  *         CCMR1        IC2F          LL_TIM_IC_Config\n
  *         CCMR2        CC3S          LL_TIM_IC_Config\n
  *         CCMR2        IC3PSC        LL_TIM_IC_Config\n
  *         CCMR2        IC3F          LL_TIM_IC_Config\n
  *         CCMR2        CC4S          LL_TIM_IC_Config\n
  *         CCMR2        IC4PSC        LL_TIM_IC_Config\n
  *         CCMR2        IC4F          LL_TIM_IC_Config\n
  *         CCER         CC1P          LL_TIM_IC_Config\n
  *         CCER         CC1NP         LL_TIM_IC_Config\n
  *         CCER         CC2P          LL_TIM_IC_Config\n
  *         CCER         CC2NP         LL_TIM_IC_Config\n
  *         CCER         CC3P          LL_TIM_IC_Config\n
  *         CCER         CC3NP         LL_TIM_IC_Config\n
  *         CCER         CC4P          LL_TIM_IC_Config\n
  *         CCER         CC4NP         LL_TIM_IC_Config
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  * @param  Configuration This parameter must be a combination of all the following values:
  *         @arg @ref LL_TIM_ACTIVEINPUT_DIRECTTI or @ref LL_TIM_ACTIVEINPUT_INDIRECTTI or @ref LL_TIM_ACTIVEINPUT_TRC
  *         @arg @ref LL_TIM_ICPSC_DIV1 or ... or @ref LL_TIM_ICPSC_DIV8
  *         @arg @ref LL_TIM_IC_FILTER_FDIV1 or ... or @ref LL_TIM_IC_FILTER_FDIV32_N8
  *         @arg @ref LL_TIM_IC_POLARITY_RISING or @ref LL_TIM_IC_POLARITY_FALLING or @ref LL_TIM_IC_POLARITY_BOTHEDGE
  * @retval None
  */
__STATIC_INLINE void LL_TIM_IC_Config(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t Configuration)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  MODIFY_REG(*pReg, ((TIM_CCMR1_IC1F | TIM_CCMR1_IC1PSC | TIM_CCMR1_CC1S) << SHIFT_TAB_ICxx[iChannel]),
             ((Configuration >> 16U) & (TIM_CCMR1_IC1F | TIM_CCMR1_IC1PSC | TIM_CCMR1_CC1S))                \
             << SHIFT_TAB_ICxx[iChannel]);
  MODIFY_REG(TIMx->CCER, ((TIM_CCER_CC1NP | TIM_CCER_CC1P) << SHIFT_TAB_CCxP[iChannel]),
             (Configuration & (TIM_CCER_CC1NP | TIM_CCER_CC1P)) << SHIFT_TAB_CCxP[iChannel]);
}

/**
  * @brief  Set the active input.
  * @rmtoll CCMR1        CC1S          LL_TIM_IC_SetActiveInput\n
  *         CCMR1        CC2S          LL_TIM_IC_SetActiveInput\n
  *         CCMR2        CC3S          LL_TIM_IC_SetActiveInput\n
  *         CCMR2        CC4S          LL_TIM_IC_SetActiveInput
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  * @param  ICActiveInput This parameter can be one of the following values:
  *         @arg @ref LL_TIM_ACTIVEINPUT_DIRECTTI
  *         @arg @ref LL_TIM_ACTIVEINPUT_INDIRECTTI
  *         @arg @ref LL_TIM_ACTIVEINPUT_TRC
  * @retval None
  */
__STATIC_INLINE void LL_TIM_IC_SetActiveInput(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t ICActiveInput)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  MODIFY_REG(*pReg, ((TIM_CCMR1_CC1S) << SHIFT_TAB_ICxx[iChannel]), (ICActiveInput >> 16U) << SHIFT_TAB_ICxx[iChannel]);
}

/**
  * @brief  Get the current active input.
  * @rmtoll CCMR1        CC1S          LL_TIM_IC_GetActiveInput\n
  *         CCMR1        CC2S          LL_TIM_IC_GetActiveInput\n
  *         CCMR2        CC3S          LL_TIM_IC_GetActiveInput\n
  *         CCMR2        CC4S          LL_TIM_IC_GetActiveInput
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_ACTIVEINPUT_DIRECTTI
  *         @arg @ref LL_TIM_ACTIVEINPUT_INDIRECTTI
  *         @arg @ref LL_TIM_ACTIVEINPUT_TRC
  */
__STATIC_INLINE uint32_t LL_TIM_IC_GetActiveInput(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  const __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  return ((READ_BIT(*pReg, ((TIM_CCMR1_CC1S) << SHIFT_TAB_ICxx[iChannel])) >> SHIFT_TAB_ICxx[iChannel]) << 16U);
}

/**
  * @brief  Set the prescaler of input channel.
  * @rmtoll CCMR1        IC1PSC        LL_TIM_IC_SetPrescaler\n
  *         CCMR1        IC2PSC        LL_TIM_IC_SetPrescaler\n
  *         CCMR2        IC3PSC        LL_TIM_IC_SetPrescaler\n
  *         CCMR2        IC4PSC        LL_TIM_IC_SetPrescaler
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  * @param  ICPrescaler This parameter can be one of the following values:
  *         @arg @ref LL_TIM_ICPSC_DIV1
  *         @arg @ref LL_TIM_ICPSC_DIV2
  *         @arg @ref LL_TIM_ICPSC_DIV4
  *         @arg @ref LL_TIM_ICPSC_DIV8
  * @retval None
  */
__STATIC_INLINE void LL_TIM_IC_SetPrescaler(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t ICPrescaler)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  MODIFY_REG(*pReg, ((TIM_CCMR1_IC1PSC) << SHIFT_TAB_ICxx[iChannel]), (ICPrescaler >> 16U) << SHIFT_TAB_ICxx[iChannel]);
}

/**
  * @brief  Get the current prescaler value acting on an  input channel.
  * @rmtoll CCMR1        IC1PSC        LL_TIM_IC_GetPrescaler\n
  *         CCMR1        IC2PSC        LL_TIM_IC_GetPrescaler\n
  *         CCMR2        IC3PSC        LL_TIM_IC_GetPrescaler\n
  *         CCMR2        IC4PSC        LL_TIM_IC_GetPrescaler
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_ICPSC_DIV1
  *         @arg @ref LL_TIM_ICPSC_DIV2
  *         @arg @ref LL_TIM_ICPSC_DIV4
  *         @arg @ref LL_TIM_ICPSC_DIV8
  */
__STATIC_INLINE uint32_t LL_TIM_IC_GetPrescaler(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  const __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  return ((READ_BIT(*pReg, ((TIM_CCMR1_IC1PSC) << SHIFT_TAB_ICxx[iChannel])) >> SHIFT_TAB_ICxx[iChannel]) << 16U);
}

/**
  * @brief  Set the input filter duration.
  * @rmtoll CCMR1        IC1F          LL_TIM_IC_SetFilter\n
  *         CCMR1        IC2F          LL_TIM_IC_SetFilter\n
  *         CCMR2        IC3F          LL_TIM_IC_SetFilter\n
  *         CCMR2        IC4F          LL_TIM_IC_SetFilter
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  * @param  ICFilter This parameter can be one of the following values:
  *         @arg @ref LL_TIM_IC_FILTER_FDIV1
  *         @arg @ref LL_TIM_IC_FILTER_FDIV1_N2
  *         @arg @ref LL_TIM_IC_FILTER_FDIV1_N4
  *         @arg @ref LL_TIM_IC_FILTER_FDIV1_N8
  *         @arg @ref LL_TIM_IC_FILTER_FDIV2_N6
  *         @arg @ref LL_TIM_IC_FILTER_FDIV2_N8
  *         @arg @ref LL_TIM_IC_FILTER_FDIV4_N6
  *         @arg @ref LL_TIM_IC_FILTER_FDIV4_N8
  *         @arg @ref LL_TIM_IC_FILTER_FDIV8_N6
  *         @arg @ref LL_TIM_IC_FILTER_FDIV8_N8
  *         @arg @ref LL_TIM_IC_FILTER_FDIV16_N5
  *         @arg @ref LL_TIM_IC_FILTER_FDIV16_N6
  *         @arg @ref LL_TIM_IC_FILTER_FDIV16_N8
  *         @arg @ref LL_TIM_IC_FILTER_FDIV32_N5
  *         @arg @ref LL_TIM_IC_FILTER_FDIV32_N6
  *         @arg @ref LL_TIM_IC_FILTER_FDIV32_N8
  * @retval None
  */
__STATIC_INLINE void LL_TIM_IC_SetFilter(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t ICFilter)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  MODIFY_REG(*pReg, ((TIM_CCMR1_IC1F) << SHIFT_TAB_ICxx[iChannel]), (ICFilter >> 16U) << SHIFT_TAB_ICxx[iChannel]);
}

/**
  * @brief  Get the input filter duration.
  * @rmtoll CCMR1        IC1F          LL_TIM_IC_GetFilter\n
  *         CCMR1        IC2F          LL_TIM_IC_GetFilter\n
  *         CCMR2        IC3F          LL_TIM_IC_GetFilter\n
  *         CCMR2        IC4F          LL_TIM_IC_GetFilter
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_IC_FILTER_FDIV1
  *         @arg @ref LL_TIM_IC_FILTER_FDIV1_N2
  *         @arg @ref LL_TIM_IC_FILTER_FDIV1_N4
  *         @arg @ref LL_TIM_IC_FILTER_FDIV1_N8
  *         @arg @ref LL_TIM_IC_FILTER_FDIV2_N6
  *         @arg @ref LL_TIM_IC_FILTER_FDIV2_N8
  *         @arg @ref LL_TIM_IC_FILTER_FDIV4_N6
  *         @arg @ref LL_TIM_IC_FILTER_FDIV4_N8
  *         @arg @ref LL_TIM_IC_FILTER_FDIV8_N6
  *         @arg @ref LL_TIM_IC_FILTER_FDIV8_N8
  *         @arg @ref LL_TIM_IC_FILTER_FDIV16_N5
  *         @arg @ref LL_TIM_IC_FILTER_FDIV16_N6
  *         @arg @ref LL_TIM_IC_FILTER_FDIV16_N8
  *         @arg @ref LL_TIM_IC_FILTER_FDIV32_N5
  *         @arg @ref LL_TIM_IC_FILTER_FDIV32_N6
  *         @arg @ref LL_TIM_IC_FILTER_FDIV32_N8
  */
__STATIC_INLINE uint32_t LL_TIM_IC_GetFilter(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  const __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  return ((READ_BIT(*pReg, ((TIM_CCMR1_IC1F) << SHIFT_TAB_ICxx[iChannel])) >> SHIFT_TAB_ICxx[iChannel]) << 16U);
}

/**
  * @brief  Set the input channel polarity.
  * @rmtoll CCER         CC1P          LL_TIM_IC_SetPolarity\n
  *         CCER         CC1NP         LL_TIM_IC_SetPolarity\n
  *         CCER         CC2P          LL_TIM_IC_SetPolarity\n
  *         CCER         CC2NP         LL_TIM_IC_SetPolarity\n
  *         CCER         CC3P          LL_TIM_IC_SetPolarity\n
  *         CCER         CC3NP         LL_TIM_IC_SetPolarity\n
  *         CCER         CC4P          LL_TIM_IC_SetPolarity\n
  *         CCER         CC4NP         LL_TIM_IC_SetPolarity
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  * @param  ICPolarity This parameter can be one of the following values:
  *         @arg @ref LL_TIM_IC_POLARITY_RISING
  *         @arg @ref LL_TIM_IC_POLARITY_FALLING
  *         @arg @ref LL_TIM_IC_POLARITY_BOTHEDGE
  * @retval None
  */
__STATIC_INLINE void LL_TIM_IC_SetPolarity(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t ICPolarity)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  MODIFY_REG(TIMx->CCER, ((TIM_CCER_CC1NP | TIM_CCER_CC1P) << SHIFT_TAB_CCxP[iChannel]),
             ICPolarity << SHIFT_TAB_CCxP[iChannel]);
}

/**
  * @brief  Get the current input channel polarity.
  * @rmtoll CCER         CC1P          LL_TIM_IC_GetPolarity\n
  *         CCER         CC1NP         LL_TIM_IC_GetPolarity\n
  *         CCER         CC2P          LL_TIM_IC_GetPolarity\n
  *         CCER         CC2NP         LL_TIM_IC_GetPolarity\n
  *         CCER         CC3P          LL_TIM_IC_GetPolarity\n
  *         CCER         CC3NP         LL_TIM_IC_GetPolarity\n
  *         CCER         CC4P          LL_TIM_IC_GetPolarity\n
  *         CCER         CC4NP         LL_TIM_IC_GetPolarity
  * @param  TIMx Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CHANNEL_CH1
  *         @arg @ref LL_TIM_CHANNEL_CH2
  *         @arg @ref LL_TIM_CHANNEL_CH3
  *         @arg @ref LL_TIM_CHANNEL_CH4
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_IC_POLARITY_RISING
  *         @arg @ref LL_TIM_IC_POLARITY_FALLING
  *         @arg @ref LL_TIM_IC_POLARITY_BOTHEDGE
  */
__STATIC_INLINE uint32_t LL_TIM_IC_GetPolarity(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = TIM_GET_CHANNEL_INDEX(Channel);
  return (READ_BIT(TIMx->CCER, ((TIM_CCER_CC1NP | TIM_CCER_CC1P) << SHIFT_TAB_CCxP[iChannel])) >>
          SHIFT_TAB_CCxP[iChannel]);
}

/**
  * @brief  Connect the TIMx_CH1, CH2 and CH3 pins  to the TI1 input (XOR combination).
  * @note Macro IS_TIM_XOR_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides an XOR input.
  * @rmtoll CR2          TI1S          LL_TIM_IC_EnableXORCombination
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_IC_EnableXORCombination(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->CR2, TIM_CR2_TI1S);
}

/**
  * @brief  Disconnect the TIMx_CH1, CH2 and CH3 pins  from the TI1 input.
  * @note Macro IS_TIM_XOR_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides an XOR input.
  * @rmtoll CR2          TI1S          LL_TIM_IC_DisableXORCombination
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_IC_DisableXORCombination(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->CR2, TIM_CR2_TI1S);
}

/**
  * @brief  Indicates whether the TIMx_CH1, CH2 and CH3 pins are connectected to the TI1 input.
  * @note Macro IS_TIM_XOR_INSTANCE(TIMx) can be used to check whether or not
  * a timer instance provides an XOR input.
  * @rmtoll CR2          TI1S          LL_TIM_IC_IsEnabledXORCombination
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IC_IsEnabledXORCombination(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->CR2, TIM_CR2_TI1S) == (TIM_CR2_TI1S)) ? 1UL : 0UL);
}

/**
  * @brief  Get captured value for input channel 1.
  * @note In 32-bit timer implementations returned captured value can be between 0x00000000 and 0xFFFFFFFF.
  * @note Macro IS_TIM_32B_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a 32 bits counter.
  * @note Macro IS_TIM_CC1_INSTANCE(TIMx) can be used to check whether or not
  *       input channel 1 is supported by a timer instance.
  * @note If dithering is activated, pay attention to the returned value interpretation.
  * @rmtoll CCR1         CCR1          LL_TIM_IC_GetCaptureCH1
  * @param  TIMx Timer instance
  * @retval CapturedValue (between Min_Data=0 and Max_Data=65535)
  */
__STATIC_INLINE uint32_t LL_TIM_IC_GetCaptureCH1(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_REG(TIMx->CCR1));
}

/**
  * @brief  Get captured value for input channel 2.
  * @note In 32-bit timer implementations returned captured value can be between 0x00000000 and 0xFFFFFFFF.
  * @note Macro IS_TIM_32B_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a 32 bits counter.
  * @note Macro IS_TIM_CC2_INSTANCE(TIMx) can be used to check whether or not
  *       input channel 2 is supported by a timer instance.
  * @note If dithering is activated, pay attention to the returned value interpretation.
  * @rmtoll CCR2         CCR2          LL_TIM_IC_GetCaptureCH2
  * @param  TIMx Timer instance
  * @retval CapturedValue (between Min_Data=0 and Max_Data=65535)
  */
__STATIC_INLINE uint32_t LL_TIM_IC_GetCaptureCH2(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_REG(TIMx->CCR2));
}

/**
  * @brief  Get captured value for input channel 3.
  * @note In 32-bit timer implementations returned captured value can be between 0x00000000 and 0xFFFFFFFF.
  * @note Macro IS_TIM_32B_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a 32 bits counter.
  * @note Macro IS_TIM_CC3_INSTANCE(TIMx) can be used to check whether or not
  *       input channel 3 is supported by a timer instance.
  * @note If dithering is activated, pay attention to the returned value interpretation.
  * @rmtoll CCR3         CCR3          LL_TIM_IC_GetCaptureCH3
  * @param  TIMx Timer instance
  * @retval CapturedValue (between Min_Data=0 and Max_Data=65535)
  */
__STATIC_INLINE uint32_t LL_TIM_IC_GetCaptureCH3(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_REG(TIMx->CCR3));
}

/**
  * @brief  Get captured value for input channel 4.
  * @note In 32-bit timer implementations returned captured value can be between 0x00000000 and 0xFFFFFFFF.
  * @note Macro IS_TIM_32B_COUNTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports a 32 bits counter.
  * @note Macro IS_TIM_CC4_INSTANCE(TIMx) can be used to check whether or not
  *       input channel 4 is supported by a timer instance.
  * @note If dithering is activated, pay attention to the returned value interpretation.
  * @rmtoll CCR4         CCR4          LL_TIM_IC_GetCaptureCH4
  * @param  TIMx Timer instance
  * @retval CapturedValue (between Min_Data=0 and Max_Data=65535)
  */
__STATIC_INLINE uint32_t LL_TIM_IC_GetCaptureCH4(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_REG(TIMx->CCR4));
}

/**
  * @}
  */

/** @defgroup TIM_LL_EF_Clock_Selection Counter clock selection
  * @{
  */
/**
  * @brief  Enable external clock mode 2.
  * @note When external clock mode 2 is enabled the counter is clocked by any active edge on the ETRF signal.
  * @note Macro IS_TIM_CLOCKSOURCE_ETRMODE2_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports external clock mode2.
  * @rmtoll SMCR         ECE           LL_TIM_EnableExternalClock
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableExternalClock(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->SMCR, TIM_SMCR_ECE);
}

/**
  * @brief  Disable external clock mode 2.
  * @note Macro IS_TIM_CLOCKSOURCE_ETRMODE2_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports external clock mode2.
  * @rmtoll SMCR         ECE           LL_TIM_DisableExternalClock
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableExternalClock(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->SMCR, TIM_SMCR_ECE);
}

/**
  * @brief  Indicate whether external clock mode 2 is enabled.
  * @note Macro IS_TIM_CLOCKSOURCE_ETRMODE2_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports external clock mode2.
  * @rmtoll SMCR         ECE           LL_TIM_IsEnabledExternalClock
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledExternalClock(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SMCR, TIM_SMCR_ECE) == (TIM_SMCR_ECE)) ? 1UL : 0UL);
}

/**
  * @brief  Set the clock source of the counter clock.
  * @note when selected clock source is external clock mode 1, the timer input
  *       the external clock is applied is selected by calling the @ref LL_TIM_SetTriggerInput()
  *       function. This timer input must be configured by calling
  *       the @ref LL_TIM_IC_Config() function.
  * @note Macro IS_TIM_CLOCKSOURCE_ETRMODE1_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports external clock mode1.
  * @note Macro IS_TIM_CLOCKSOURCE_ETRMODE2_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports external clock mode2.
  * @rmtoll SMCR         SMS           LL_TIM_SetClockSource\n
  *         SMCR         ECE           LL_TIM_SetClockSource
  * @param  TIMx Timer instance
  * @param  ClockSource This parameter can be one of the following values:
  *         @arg @ref LL_TIM_CLOCKSOURCE_INTERNAL
  *         @arg @ref LL_TIM_CLOCKSOURCE_EXT_MODE1
  *         @arg @ref LL_TIM_CLOCKSOURCE_EXT_MODE2
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetClockSource(TIM_TypeDef *TIMx, uint32_t ClockSource)
{
  MODIFY_REG(TIMx->SMCR, TIM_SMCR_SMS | TIM_SMCR_ECE, ClockSource);
}

/**
  * @brief  Set the encoder interface mode.
  * @note Macro IS_TIM_ENCODER_INTERFACE_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports the encoder mode.
  * @rmtoll SMCR         SMS           LL_TIM_SetEncoderMode
  * @param  TIMx Timer instance
  * @param  EncoderMode This parameter can be one of the following values:
  *         @arg @ref LL_TIM_ENCODERMODE_X2_TI1
  *         @arg @ref LL_TIM_ENCODERMODE_X2_TI2
  *         @arg @ref LL_TIM_ENCODERMODE_X4_TI12
  *         @arg @ref LL_TIM_ENCODERMODE_CLOCKPLUSDIRECTION_X2
  *         @arg @ref LL_TIM_ENCODERMODE_CLOCKPLUSDIRECTION_X1
  *         @arg @ref LL_TIM_ENCODERMODE_DIRECTIONALCLOCK_X2
  *         @arg @ref LL_TIM_ENCODERMODE_DIRECTIONALCLOCK_X1_TI12
  *         @arg @ref LL_TIM_ENCODERMODE_X1_TI1
  *         @arg @ref LL_TIM_ENCODERMODE_X1_TI2
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetEncoderMode(TIM_TypeDef *TIMx, uint32_t EncoderMode)
{
  MODIFY_REG(TIMx->SMCR, TIM_SMCR_SMS, EncoderMode);
}

/**
  * @}
  */

/** @defgroup TIM_LL_EF_Timer_Synchronization Timer synchronisation configuration
  * @{
  */
/**
  * @brief  Set the trigger output (TRGO) used for timer synchronization .
  * @note Macro IS_TIM_MASTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance can operate as a master timer.
  * @rmtoll CR2          MMS           LL_TIM_SetTriggerOutput
  * @param  TIMx Timer instance
  * @param  TimerSynchronization This parameter can be one of the following values:
  *         @arg @ref LL_TIM_TRGO_RESET
  *         @arg @ref LL_TIM_TRGO_ENABLE
  *         @arg @ref LL_TIM_TRGO_UPDATE
  *         @arg @ref LL_TIM_TRGO_CC1IF
  *         @arg @ref LL_TIM_TRGO_OC1REF
  *         @arg @ref LL_TIM_TRGO_OC2REF
  *         @arg @ref LL_TIM_TRGO_OC3REF
  *         @arg @ref LL_TIM_TRGO_OC4REF
  *         @arg @ref LL_TIM_TRGO_ENCODERCLK
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetTriggerOutput(TIM_TypeDef *TIMx, uint32_t TimerSynchronization)
{
  MODIFY_REG(TIMx->CR2, TIM_CR2_MMS, TimerSynchronization);
}

/**
  * @brief  Set the trigger output 2 (TRGO2) used for ADC synchronization .
  * @note Macro IS_TIM_TRGO2_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance can be used for ADC synchronization.
  * @rmtoll CR2          MMS2          LL_TIM_SetTriggerOutput2
  * @param  TIMx Timer Instance
  * @param  ADCSynchronization This parameter can be one of the following values:
  *         @arg @ref LL_TIM_TRGO2_RESET
  *         @arg @ref LL_TIM_TRGO2_ENABLE
  *         @arg @ref LL_TIM_TRGO2_UPDATE
  *         @arg @ref LL_TIM_TRGO2_CC1F
  *         @arg @ref LL_TIM_TRGO2_OC1
  *         @arg @ref LL_TIM_TRGO2_OC2
  *         @arg @ref LL_TIM_TRGO2_OC3
  *         @arg @ref LL_TIM_TRGO2_OC4
  *         @arg @ref LL_TIM_TRGO2_OC5
  *         @arg @ref LL_TIM_TRGO2_OC6
  *         @arg @ref LL_TIM_TRGO2_OC4_RISINGFALLING
  *         @arg @ref LL_TIM_TRGO2_OC6_RISINGFALLING
  *         @arg @ref LL_TIM_TRGO2_OC4_RISING_OC6_RISING
  *         @arg @ref LL_TIM_TRGO2_OC4_RISING_OC6_FALLING
  *         @arg @ref LL_TIM_TRGO2_OC5_RISING_OC6_RISING
  *         @arg @ref LL_TIM_TRGO2_OC5_RISING_OC6_FALLING
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetTriggerOutput2(TIM_TypeDef *TIMx, uint32_t ADCSynchronization)
{
  MODIFY_REG(TIMx->CR2, TIM_CR2_MMS2, ADCSynchronization);
}

/**
  * @brief  Set the synchronization mode of a slave timer.
  * @note Macro IS_TIM_SLAVE_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance can operate as a slave timer.
  * @rmtoll SMCR         SMS           LL_TIM_SetSlaveMode
  * @param  TIMx Timer instance
  * @param  SlaveMode This parameter can be one of the following values:
  *         @arg @ref LL_TIM_SLAVEMODE_DISABLED
  *         @arg @ref LL_TIM_SLAVEMODE_RESET
  *         @arg @ref LL_TIM_SLAVEMODE_GATED
  *         @arg @ref LL_TIM_SLAVEMODE_TRIGGER
  *         @arg @ref LL_TIM_SLAVEMODE_COMBINED_RESETTRIGGER
  *         @arg @ref LL_TIM_SLAVEMODE_COMBINED_GATEDRESET
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetSlaveMode(TIM_TypeDef *TIMx, uint32_t SlaveMode)
{
  MODIFY_REG(TIMx->SMCR, TIM_SMCR_SMS, SlaveMode);
}

/**
  * @brief  Set the selects the trigger input to be used to synchronize the counter.
  * @note Macro IS_TIM_SLAVE_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance can operate as a slave timer.
  * @rmtoll SMCR         TS            LL_TIM_SetTriggerInput
  * @param  TIMx Timer instance
  * @param  TriggerInput This parameter can be one of the following values:
  *         @arg @ref LL_TIM_TS_ITR0
  *         @arg @ref LL_TIM_TS_ITR1
  *         @arg @ref LL_TIM_TS_ITR2
  *         @arg @ref LL_TIM_TS_ITR3
  *         @arg @ref LL_TIM_TS_ITR4
  *         @arg @ref LL_TIM_TS_ITR5
  *         @arg @ref LL_TIM_TS_ITR6
  *         @arg @ref LL_TIM_TS_ITR7
  *         @arg @ref LL_TIM_TS_ITR8
  *         @arg @ref LL_TIM_TS_ITR9
  *         @arg @ref LL_TIM_TS_ITR10
  *         @arg @ref LL_TIM_TS_ITR11
  *         @arg @ref LL_TIM_TS_ITR12
  *         @arg @ref LL_TIM_TS_ITR13
  *         @arg @ref LL_TIM_TS_TI1F_ED
  *         @arg @ref LL_TIM_TS_TI1FP1
  *         @arg @ref LL_TIM_TS_TI2FP2
  *         @arg @ref LL_TIM_TS_ETRF
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetTriggerInput(TIM_TypeDef *TIMx, uint32_t TriggerInput)
{
  MODIFY_REG(TIMx->SMCR, TIM_SMCR_TS, TriggerInput);
}

/**
  * @brief  Enable the Master/Slave mode.
  * @note Macro IS_TIM_SLAVE_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance can operate as a slave timer.
  * @rmtoll SMCR         MSM           LL_TIM_EnableMasterSlaveMode
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableMasterSlaveMode(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->SMCR, TIM_SMCR_MSM);
}

/**
  * @brief  Disable the Master/Slave mode.
  * @note Macro IS_TIM_SLAVE_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance can operate as a slave timer.
  * @rmtoll SMCR         MSM           LL_TIM_DisableMasterSlaveMode
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableMasterSlaveMode(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->SMCR, TIM_SMCR_MSM);
}

/**
  * @brief Indicates whether the Master/Slave mode is enabled.
  * @note Macro IS_TIM_SLAVE_INSTANCE(TIMx) can be used to check whether or not
  * a timer instance can operate as a slave timer.
  * @rmtoll SMCR         MSM           LL_TIM_IsEnabledMasterSlaveMode
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledMasterSlaveMode(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SMCR, TIM_SMCR_MSM) == (TIM_SMCR_MSM)) ? 1UL : 0UL);
}

/**
  * @brief  Configure the external trigger (ETR) input.
  * @note Macro IS_TIM_ETR_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides an external trigger input.
  * @rmtoll SMCR         ETP           LL_TIM_ConfigETR\n
  *         SMCR         ETPS          LL_TIM_ConfigETR\n
  *         SMCR         ETF           LL_TIM_ConfigETR
  * @param  TIMx Timer instance
  * @param  ETRPolarity This parameter can be one of the following values:
  *         @arg @ref LL_TIM_ETR_POLARITY_NONINVERTED
  *         @arg @ref LL_TIM_ETR_POLARITY_INVERTED
  * @param  ETRPrescaler This parameter can be one of the following values:
  *         @arg @ref LL_TIM_ETR_PRESCALER_DIV1
  *         @arg @ref LL_TIM_ETR_PRESCALER_DIV2
  *         @arg @ref LL_TIM_ETR_PRESCALER_DIV4
  *         @arg @ref LL_TIM_ETR_PRESCALER_DIV8
  * @param  ETRFilter This parameter can be one of the following values:
  *         @arg @ref LL_TIM_ETR_FILTER_FDIV1
  *         @arg @ref LL_TIM_ETR_FILTER_FDIV1_N2
  *         @arg @ref LL_TIM_ETR_FILTER_FDIV1_N4
  *         @arg @ref LL_TIM_ETR_FILTER_FDIV1_N8
  *         @arg @ref LL_TIM_ETR_FILTER_FDIV2_N6
  *         @arg @ref LL_TIM_ETR_FILTER_FDIV2_N8
  *         @arg @ref LL_TIM_ETR_FILTER_FDIV4_N6
  *         @arg @ref LL_TIM_ETR_FILTER_FDIV4_N8
  *         @arg @ref LL_TIM_ETR_FILTER_FDIV8_N6
  *         @arg @ref LL_TIM_ETR_FILTER_FDIV8_N8
  *         @arg @ref LL_TIM_ETR_FILTER_FDIV16_N5
  *         @arg @ref LL_TIM_ETR_FILTER_FDIV16_N6
  *         @arg @ref LL_TIM_ETR_FILTER_FDIV16_N8
  *         @arg @ref LL_TIM_ETR_FILTER_FDIV32_N5
  *         @arg @ref LL_TIM_ETR_FILTER_FDIV32_N6
  *         @arg @ref LL_TIM_ETR_FILTER_FDIV32_N8
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ConfigETR(TIM_TypeDef *TIMx, uint32_t ETRPolarity, uint32_t ETRPrescaler,
                                      uint32_t ETRFilter)
{
  MODIFY_REG(TIMx->SMCR, TIM_SMCR_ETP | TIM_SMCR_ETPS | TIM_SMCR_ETF, ETRPolarity | ETRPrescaler | ETRFilter);
}

/**
  * @brief  Select the external trigger (ETR) input source.
  * @note Macro IS_TIM_ETRSEL_INSTANCE(TIMx) can be used to check whether or
  *       not a timer instance supports ETR source selection.
  * @rmtoll AF1          ETRSEL        LL_TIM_SetETRSource
  * @param  TIMx Timer instance
  * @param  ETRSource This parameter can be one of the following values:
  *
  *         For TIM1, the parameter is one of the following values:
  *
  *         @arg @ref LL_TIM_TIM1_ETRSOURCE_GPIO
  *         @arg @ref LL_TIM_TIM1_ETRSOURCE_ADC1_AWD1
  *         @arg @ref LL_TIM_TIM1_ETRSOURCE_ADC1_AWD2
  *         @arg @ref LL_TIM_TIM1_ETRSOURCE_ADC1_AWD3
  *         @arg @ref LL_TIM_TIM1_ETRSOURCE_ADC2_AWD1
  *         @arg @ref LL_TIM_TIM1_ETRSOURCE_ADC2_AWD2
  *         @arg @ref LL_TIM_TIM1_ETRSOURCE_ADC2_AWD3
  *
  *         For TIM2, the parameter is one of the following values:
  *
  *         @arg @ref LL_TIM_TIM2_ETRSOURCE_GPIO
  *         @arg @ref LL_TIM_TIM2_ETRSOURCE_DCMIPP_HSYNC
  *         @arg @ref LL_TIM_TIM2_ETRSOURCE_LCD_HSYNC
  *         @arg @ref LL_TIM_TIM2_ETRSOURCE_SAI1_FSA
  *         @arg @ref LL_TIM_TIM2_ETRSOURCE_SAI1_FSB
  *         @arg @ref LL_TIM_TIM2_ETRSOURCE_GFXTIM_TE
  *         @arg @ref LL_TIM_TIM2_ETRSOURCE_DCMIPP_VSYNC
  *         @arg @ref LL_TIM_TIM2_ETRSOURCE_LCD_VSYNC
  *         @arg @ref LL_TIM_TIM2_ETRSOURCE_TIM3_ETR
  *         @arg @ref LL_TIM_TIM2_ETRSOURCE_TIM4_ETR
  *         @arg @ref LL_TIM_TIM2_ETRSOURCE_TIM5_ETR
  *         @arg @ref LL_TIM_TIM2_ETRSOURCE_USB1_SOF
  *         @arg @ref LL_TIM_TIM2_ETRSOURCE_USB2_SOF
  *
  *         For TIM3, the parameter is one of the following values:
  *
  *         @arg @ref LL_TIM_TIM3_ETRSOURCE_GPIO
  *         @arg @ref LL_TIM_TIM3_ETRSOURCE_DCMIPP_HSYNC
  *         @arg @ref LL_TIM_TIM3_ETRSOURCE_LCD_HSYNC
  *         @arg @ref LL_TIM_TIM3_ETRSOURCE_GFXTIM_TE
  *         @arg @ref LL_TIM_TIM3_ETRSOURCE_DCMIPP_VSYNC
  *         @arg @ref LL_TIM_TIM3_ETRSOURCE_LCD_VSYNC
  *         @arg @ref LL_TIM_TIM3_ETRSOURCE_TIM2_ETR
  *         @arg @ref LL_TIM_TIM3_ETRSOURCE_TIM4_ETR
  *         @arg @ref LL_TIM_TIM3_ETRSOURCE_TIM5_ETR
  *
  *         For TIM4, the parameter is one of the following values:
  *
  *         @arg @ref LL_TIM_TIM4_ETRSOURCE_GPIO
  *         @arg @ref LL_TIM_TIM4_ETRSOURCE_DCMIPP_HSYNC
  *         @arg @ref LL_TIM_TIM4_ETRSOURCE_LCD_HSYNC
  *         @arg @ref LL_TIM_TIM4_ETRSOURCE_GFXTIM_TE
  *         @arg @ref LL_TIM_TIM4_ETRSOURCE_DCMIPP_VSYNC
  *         @arg @ref LL_TIM_TIM4_ETRSOURCE_LCD_VSYNC
  *         @arg @ref LL_TIM_TIM4_ETRSOURCE_TIM2_ETR
  *         @arg @ref LL_TIM_TIM4_ETRSOURCE_TIM3_ETR
  *         @arg @ref LL_TIM_TIM4_ETRSOURCE_TIM5_ETR
  *
  *         For TIM5, the parameter is one of the following values:
  *
  *         @arg @ref LL_TIM_TIM5_ETRSOURCE_GPIO
  *         @arg @ref LL_TIM_TIM5_ETRSOURCE_SAI2_FSA
  *         @arg @ref LL_TIM_TIM5_ETRSOURCE_SAI2_FSB
  *         @arg @ref LL_TIM_TIM5_ETRSOURCE_DCMIPP_HSYNC
  *         @arg @ref LL_TIM_TIM5_ETRSOURCE_LCD_HSYNC
  *         @arg @ref LL_TIM_TIM5_ETRSOURCE_GFXTIM_TE
  *         @arg @ref LL_TIM_TIM5_ETRSOURCE_DCMIPP_VSYNC
  *         @arg @ref LL_TIM_TIM5_ETRSOURCE_LCD_VSYNC
  *         @arg @ref LL_TIM_TIM5_ETRSOURCE_TIM2_ETR
  *         @arg @ref LL_TIM_TIM5_ETRSOURCE_TIM3_ETR
  *         @arg @ref LL_TIM_TIM5_ETRSOURCE_TIM4_ETR
  *         @arg @ref LL_TIM_TIM5_ETRSOURCE_USB1_SOF
  *         @arg @ref LL_TIM_TIM5_ETRSOURCE_USB2_SOF
  *
  *         For TIM8, the parameter is one of the following values:
  *
  *         @arg @ref LL_TIM_TIM8_ETRSOURCE_GPIO
  *         @arg @ref LL_TIM_TIM8_ETRSOURCE_ADC2_AWD1
  *         @arg @ref LL_TIM_TIM8_ETRSOURCE_ADC2_AWD2
  *         @arg @ref LL_TIM_TIM8_ETRSOURCE_ADC2_AWD3
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetETRSource(TIM_TypeDef *TIMx, uint32_t ETRSource)
{
  MODIFY_REG(TIMx->AF1, TIMx_AF1_ETRSEL, ETRSource);
}

/**
  * @brief  Enable SMS preload.
  * @note Macro IS_TIM_SMS_PRELOAD_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports the preload of SMS field in SMCR register.
  * @rmtoll SMCR         SMSPE           LL_TIM_EnableSMSPreload
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableSMSPreload(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->SMCR, TIM_SMCR_SMSPE);
}

/**
  * @brief  Disable SMS preload.
  * @note Macro IS_TIM_SMS_PRELOAD_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports the preload of SMS field in SMCR register.
  * @rmtoll SMCR         SMSPE           LL_TIM_DisableSMSPreload
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableSMSPreload(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->SMCR, TIM_SMCR_SMSPE);
}

/**
  * @brief  Indicate whether  SMS preload is enabled.
  * @note Macro IS_TIM_SMS_PRELOAD_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports the preload of SMS field in SMCR register.
  * @rmtoll SMCR         SMSPE           LL_TIM_IsEnabledSMSPreload
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledSMSPreload(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SMCR, TIM_SMCR_SMSPE) == (TIM_SMCR_SMSPE)) ? 1UL : 0UL);
}

/**
  * @brief  Set the preload source of SMS.
  * @note Macro IS_TIM_SMS_PRELOAD_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports the preload of SMS field in SMCR register.
  * @rmtoll SMCR         SMSPS        LL_TIM_SetSMSPreloadSource\n
  * @param  TIMx Timer instance
  * @param  PreloadSource This parameter can be one of the following values:
  *         @arg @ref LL_TIM_SMSPS_TIMUPDATE
  *         @arg @ref LL_TIM_SMSPS_INDEX
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetSMSPreloadSource(TIM_TypeDef *TIMx, uint32_t PreloadSource)
{
  MODIFY_REG(TIMx->SMCR, TIM_SMCR_SMSPS, PreloadSource);
}

/**
  * @brief  Get the preload source of SMS.
  * @note Macro IS_TIM_SMS_PRELOAD_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports the preload of SMS field in SMCR register.
  * @rmtoll SMCR         SMSPS        LL_TIM_GetSMSPreloadSource\n
  * @param  TIMx Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_SMSPS_TIMUPDATE
  *         @arg @ref LL_TIM_SMSPS_INDEX
  */
__STATIC_INLINE uint32_t LL_TIM_GetSMSPreloadSource(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_BIT(TIMx->SMCR, TIM_SMCR_SMSPS));
}


/**
  * @brief  Enable ADC synchronization.
  * @note Macro IS_TIM_MASTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports ADC synchronization.
  * @rmtoll CR2         ADSYNC           LL_TIM_EnableADCSynchronization
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableADCSynchronization(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->CR2, TIM_CR2_ADSYNC);
}

/**
  * @brief  Enable ADC synchronization.
  * @note Macro IS_TIM_MASTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports ADC synchronization.
  * @rmtoll CR2         ADSYNC           LL_TIM_DisableADCSynchronization
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableADCSynchronization(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->CR2, TIM_CR2_ADSYNC);
}

/**
  * @brief  Indicate whether ADC sycnhronization is enabled.
  * @note Macro IS_TIM_MASTER_INSTANCE(TIMx) can be used to check
  *       whether or not a timer instance supports ADC synchronization.
  * @rmtoll CR2         ADSYNC           LL_TIM_IsEnabledADCSynchronization
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledADCSynchronization(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->CR2, TIM_CR2_ADSYNC) == (TIM_CR2_ADSYNC)) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup TIM_LL_EF_Break_Function Break function configuration
  * @{
  */
/**
  * @brief  Enable the break function.
  * @note Macro IS_TIM_BREAK_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides a break input.
  * @rmtoll BDTR         BKE           LL_TIM_EnableBRK
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableBRK(TIM_TypeDef *TIMx)
{
  __IO uint32_t tmpreg;
  SET_BIT(TIMx->BDTR, TIM_BDTR_BKE);
  /* Note: Any write operation to this bit takes a delay of 1 APB clock cycle to become effective. */
  tmpreg = READ_REG(TIMx->BDTR);
  (void)(tmpreg);
}

/**
  * @brief  Disable the break function.
  * @rmtoll BDTR         BKE           LL_TIM_DisableBRK
  * @param  TIMx Timer instance
  * @note Macro IS_TIM_BREAK_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides a break input.
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableBRK(TIM_TypeDef *TIMx)
{
  __IO uint32_t tmpreg;
  CLEAR_BIT(TIMx->BDTR, TIM_BDTR_BKE);
  /* Note: Any write operation to this bit takes a delay of 1 APB clock cycle to become effective. */
  tmpreg = READ_REG(TIMx->BDTR);
  (void)(tmpreg);
}

/**
  * @brief  Configure the break input.
  * @note Macro IS_TIM_BREAK_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides a break input.
  * @note Bidirectional mode is only supported by advanced timer instances.
  *       Macro IS_TIM_ADVANCED_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance is an advanced-control timer.
  * @note In bidirectional mode (BKBID bit set), the Break input is configured both
  *        in input mode and in open drain output mode. Any active Break event will
  *        assert a low logic level on the Break input to indicate an internal break
  *        event to external devices.
  * @note When bidirectional mode isn't supported, BreakAFMode must be set to
  *       LL_TIM_BREAK_AFMODE_INPUT.
  * @rmtoll BDTR         BKP           LL_TIM_ConfigBRK\n
  *         BDTR         BKF           LL_TIM_ConfigBRK\n
  *         BDTR         BKBID         LL_TIM_ConfigBRK
  * @param  TIMx Timer instance
  * @param  BreakPolarity This parameter can be one of the following values:
  *         @arg @ref LL_TIM_BREAK_POLARITY_LOW
  *         @arg @ref LL_TIM_BREAK_POLARITY_HIGH
  * @param  BreakFilter This parameter can be one of the following values:
  *         @arg @ref LL_TIM_BREAK_FILTER_FDIV1
  *         @arg @ref LL_TIM_BREAK_FILTER_FDIV1_N2
  *         @arg @ref LL_TIM_BREAK_FILTER_FDIV1_N4
  *         @arg @ref LL_TIM_BREAK_FILTER_FDIV1_N8
  *         @arg @ref LL_TIM_BREAK_FILTER_FDIV2_N6
  *         @arg @ref LL_TIM_BREAK_FILTER_FDIV2_N8
  *         @arg @ref LL_TIM_BREAK_FILTER_FDIV4_N6
  *         @arg @ref LL_TIM_BREAK_FILTER_FDIV4_N8
  *         @arg @ref LL_TIM_BREAK_FILTER_FDIV8_N6
  *         @arg @ref LL_TIM_BREAK_FILTER_FDIV8_N8
  *         @arg @ref LL_TIM_BREAK_FILTER_FDIV16_N5
  *         @arg @ref LL_TIM_BREAK_FILTER_FDIV16_N6
  *         @arg @ref LL_TIM_BREAK_FILTER_FDIV16_N8
  *         @arg @ref LL_TIM_BREAK_FILTER_FDIV32_N5
  *         @arg @ref LL_TIM_BREAK_FILTER_FDIV32_N6
  *         @arg @ref LL_TIM_BREAK_FILTER_FDIV32_N8
  * @param  BreakAFMode This parameter can be one of the following values:
  *         @arg @ref LL_TIM_BREAK_AFMODE_INPUT
  *         @arg @ref LL_TIM_BREAK_AFMODE_BIDIRECTIONAL
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ConfigBRK(TIM_TypeDef *TIMx, uint32_t BreakPolarity, uint32_t BreakFilter,
                                      uint32_t BreakAFMode)
{
  __IO uint32_t tmpreg;
  MODIFY_REG(TIMx->BDTR, TIM_BDTR_BKP | TIM_BDTR_BKF | TIM_BDTR_BKBID, BreakPolarity | BreakFilter | BreakAFMode);
  /* Note: Any write operation to BKP bit takes a delay of 1 APB clock cycle to become effective. */
  tmpreg = READ_REG(TIMx->BDTR);
  (void)(tmpreg);
}

/**
  * @brief  Disarm the break input (when it operates in bidirectional mode).
  * @note  The break input can be disarmed only when it is configured in
  *        bidirectional mode and when when MOE is reset.
  * @note  Purpose is to be able to have the input voltage back to high-state,
  *        whatever the time constant on the output .
  * @rmtoll BDTR         BKDSRM        LL_TIM_DisarmBRK
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisarmBRK(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->BDTR, TIM_BDTR_BKDSRM);
}

/**
  * @brief  Enable the break 2 function.
  * @note Macro IS_TIM_BKIN2_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides a second break input.
  * @rmtoll BDTR         BK2E          LL_TIM_EnableBRK2
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableBRK2(TIM_TypeDef *TIMx)
{
  __IO uint32_t tmpreg;
  SET_BIT(TIMx->BDTR, TIM_BDTR_BK2E);
  /* Note: Any write operation to this bit takes a delay of 1 APB clock cycle to become effective. */
  tmpreg = READ_REG(TIMx->BDTR);
  (void)(tmpreg);
}

/**
  * @brief  Disable the break  2 function.
  * @note Macro IS_TIM_BKIN2_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides a second break input.
  * @rmtoll BDTR         BK2E          LL_TIM_DisableBRK2
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableBRK2(TIM_TypeDef *TIMx)
{
  __IO uint32_t tmpreg;
  CLEAR_BIT(TIMx->BDTR, TIM_BDTR_BK2E);
  /* Note: Any write operation to this bit takes a delay of 1 APB clock cycle to become effective. */
  tmpreg = READ_REG(TIMx->BDTR);
  (void)(tmpreg);
}

/**
  * @brief  Configure the break 2 input.
  * @note Macro IS_TIM_BKIN2_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides a second break input.
  * @note Bidirectional mode is only supported by advanced timer instances.
  *       Macro IS_TIM_ADVANCED_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance is an advanced-control timer.
  * @note In bidirectional mode (BK2BID bit set), the Break 2 input is configured both
  *        in input mode and in open drain output mode. Any active Break event will
  *        assert a low logic level on the Break 2 input to indicate an internal break
  *        event to external devices.
  * @note When bidirectional mode isn't supported, Break2AFMode must be set to
  *       LL_TIM_BREAK2_AFMODE_INPUT.
  * @rmtoll BDTR         BK2P          LL_TIM_ConfigBRK2\n
  *         BDTR         BK2F          LL_TIM_ConfigBRK2\n
  *         BDTR         BK2BID        LL_TIM_ConfigBRK2
  * @param  TIMx Timer instance
  * @param  Break2Polarity This parameter can be one of the following values:
  *         @arg @ref LL_TIM_BREAK2_POLARITY_LOW
  *         @arg @ref LL_TIM_BREAK2_POLARITY_HIGH
  * @param  Break2Filter This parameter can be one of the following values:
  *         @arg @ref LL_TIM_BREAK2_FILTER_FDIV1
  *         @arg @ref LL_TIM_BREAK2_FILTER_FDIV1_N2
  *         @arg @ref LL_TIM_BREAK2_FILTER_FDIV1_N4
  *         @arg @ref LL_TIM_BREAK2_FILTER_FDIV1_N8
  *         @arg @ref LL_TIM_BREAK2_FILTER_FDIV2_N6
  *         @arg @ref LL_TIM_BREAK2_FILTER_FDIV2_N8
  *         @arg @ref LL_TIM_BREAK2_FILTER_FDIV4_N6
  *         @arg @ref LL_TIM_BREAK2_FILTER_FDIV4_N8
  *         @arg @ref LL_TIM_BREAK2_FILTER_FDIV8_N6
  *         @arg @ref LL_TIM_BREAK2_FILTER_FDIV8_N8
  *         @arg @ref LL_TIM_BREAK2_FILTER_FDIV16_N5
  *         @arg @ref LL_TIM_BREAK2_FILTER_FDIV16_N6
  *         @arg @ref LL_TIM_BREAK2_FILTER_FDIV16_N8
  *         @arg @ref LL_TIM_BREAK2_FILTER_FDIV32_N5
  *         @arg @ref LL_TIM_BREAK2_FILTER_FDIV32_N6
  *         @arg @ref LL_TIM_BREAK2_FILTER_FDIV32_N8
  * @param  Break2AFMode This parameter can be one of the following values:
  *         @arg @ref LL_TIM_BREAK2_AFMODE_INPUT
  *         @arg @ref LL_TIM_BREAK2_AFMODE_BIDIRECTIONAL
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ConfigBRK2(TIM_TypeDef *TIMx, uint32_t Break2Polarity, uint32_t Break2Filter,
                                       uint32_t Break2AFMode)
{
  __IO uint32_t tmpreg;
  MODIFY_REG(TIMx->BDTR, TIM_BDTR_BK2P | TIM_BDTR_BK2F | TIM_BDTR_BK2BID, Break2Polarity | Break2Filter | Break2AFMode);
  /* Note: Any write operation to BK2P bit takes a delay of 1 APB clock cycle to become effective. */
  tmpreg = READ_REG(TIMx->BDTR);
  (void)(tmpreg);
}

/**
  * @brief  Disarm the break 2 input (when it operates in bidirectional mode).
  * @note  The break 2 input can be disarmed only when it is configured in
  *        bidirectional mode and when when MOE is reset.
  * @note  Purpose is to be able to have the input voltage back to high-state,
  *        whatever the time constant on the output.
  * @rmtoll BDTR         BK2DSRM       LL_TIM_DisarmBRK2
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisarmBRK2(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->BDTR, TIM_BDTR_BK2DSRM);
}

/**
  * @brief  Select the outputs off state (enabled v.s. disabled) in Idle and Run modes.
  * @note Macro IS_TIM_BREAK_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides a break input.
  * @rmtoll BDTR         OSSI          LL_TIM_SetOffStates\n
  *         BDTR         OSSR          LL_TIM_SetOffStates
  * @param  TIMx Timer instance
  * @param  OffStateIdle This parameter can be one of the following values:
  *         @arg @ref LL_TIM_OSSI_DISABLE
  *         @arg @ref LL_TIM_OSSI_ENABLE
  * @param  OffStateRun This parameter can be one of the following values:
  *         @arg @ref LL_TIM_OSSR_DISABLE
  *         @arg @ref LL_TIM_OSSR_ENABLE
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetOffStates(TIM_TypeDef *TIMx, uint32_t OffStateIdle, uint32_t OffStateRun)
{
  MODIFY_REG(TIMx->BDTR, TIM_BDTR_OSSI | TIM_BDTR_OSSR, OffStateIdle | OffStateRun);
}

/**
  * @brief  Enable automatic output (MOE can be set by software or automatically when a break input is active).
  * @note Macro IS_TIM_BREAK_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides a break input.
  * @rmtoll BDTR         AOE           LL_TIM_EnableAutomaticOutput
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableAutomaticOutput(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->BDTR, TIM_BDTR_AOE);
}

/**
  * @brief  Disable automatic output (MOE can be set only by software).
  * @note Macro IS_TIM_BREAK_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides a break input.
  * @rmtoll BDTR         AOE           LL_TIM_DisableAutomaticOutput
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableAutomaticOutput(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->BDTR, TIM_BDTR_AOE);
}

/**
  * @brief  Indicate whether automatic output is enabled.
  * @note Macro IS_TIM_BREAK_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides a break input.
  * @rmtoll BDTR         AOE           LL_TIM_IsEnabledAutomaticOutput
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledAutomaticOutput(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->BDTR, TIM_BDTR_AOE) == (TIM_BDTR_AOE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable the outputs (set the MOE bit in TIMx_BDTR register).
  * @note The MOE bit in TIMx_BDTR register allows to enable /disable the outputs by
  *       software and is reset in case of break or break2 event
  * @note Macro IS_TIM_BREAK_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides a break input.
  * @rmtoll BDTR         MOE           LL_TIM_EnableAllOutputs
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableAllOutputs(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->BDTR, TIM_BDTR_MOE);
}

/**
  * @brief  Disable the outputs (reset the MOE bit in TIMx_BDTR register).
  * @note The MOE bit in TIMx_BDTR register allows to enable /disable the outputs by
  *       software and is reset in case of break or break2 event.
  * @note Macro IS_TIM_BREAK_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides a break input.
  * @rmtoll BDTR         MOE           LL_TIM_DisableAllOutputs
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableAllOutputs(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->BDTR, TIM_BDTR_MOE);
}

/**
  * @brief  Indicates whether outputs are enabled.
  * @note Macro IS_TIM_BREAK_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides a break input.
  * @rmtoll BDTR         MOE           LL_TIM_IsEnabledAllOutputs
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledAllOutputs(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->BDTR, TIM_BDTR_MOE) == (TIM_BDTR_MOE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable the signals connected to the designated timer break input.
  * @note Macro IS_TIM_BREAKSOURCE_INSTANCE(TIMx) can be used to check whether
  *       or not a timer instance allows for break input selection.
  * @rmtoll AF1          BKINE         LL_TIM_EnableBreakInputSource\n
  *         AF1          BKCMP8E       LL_TIM_EnableBreakInputSource\n
  *         AF2          BK2INE        LL_TIM_EnableBreakInputSource\n
  *         AF2          BK2CMP8E      LL_TIM_EnableBreakInputSource\n
  * @param  TIMx Timer instance
  * @param  BreakInput This parameter can be one of the following values:
  *         @arg @ref LL_TIM_BREAK_INPUT_BKIN
  *         @arg @ref LL_TIM_BREAK_INPUT_BKIN2
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_TIM_BKIN_SOURCE_BKIN
  *         @arg @ref LL_TIM_BKIN_SOURCE_MDF1
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableBreakInputSource(TIM_TypeDef *TIMx, uint32_t BreakInput, uint32_t Source)
{
  __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->AF1) + BreakInput));
  SET_BIT(*pReg, Source);
}

/**
  * @brief  Disable the signals connected to the designated timer break input.
  * @note Macro IS_TIM_BREAKSOURCE_INSTANCE(TIMx) can be used to check whether
  *       or not a timer instance allows for break input selection.
  * @rmtoll AF1          BKINE         LL_TIM_DisableBreakInputSource\n
  *         AF1          BKCMP8E       LL_TIM_DisableBreakInputSource\n
  *         AF2          BK2INE        LL_TIM_DisableBreakInputSource\n
  *         AF2          BK2CMP8E      LL_TIM_DisableBreakInputSource\n
  * @param  TIMx Timer instance
  * @param  BreakInput This parameter can be one of the following values:
  *         @arg @ref LL_TIM_BREAK_INPUT_BKIN
  *         @arg @ref LL_TIM_BREAK_INPUT_BKIN2
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_TIM_BKIN_SOURCE_BKIN
  *         @arg @ref LL_TIM_BKIN_SOURCE_MDF1
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableBreakInputSource(TIM_TypeDef *TIMx, uint32_t BreakInput, uint32_t Source)
{
  __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->AF1) + BreakInput));
  CLEAR_BIT(*pReg, Source);
}

/**
  * @brief  Set the polarity of the break signal for the timer break input.
  * @note Macro IS_TIM_BREAKSOURCE_INSTANCE(TIMx) can be used to check whether
  *       or not a timer instance allows for break input selection.
  * @rmtoll AF1          BKINP         LL_TIM_SetBreakInputSourcePolarity\n
  *         AF2          BK2INP        LL_TIM_SetBreakInputSourcePolarity
  * @param  TIMx Timer instance
  * @param  BreakInput This parameter can be one of the following values:
  *         @arg @ref LL_TIM_BREAK_INPUT_BKIN
  *         @arg @ref LL_TIM_BREAK_INPUT_BKIN2
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_TIM_BKIN_SOURCE_BKIN
  * @param  Polarity This parameter can be one of the following values:
  *         @arg @ref LL_TIM_BKIN_POLARITY_LOW
  *         @arg @ref LL_TIM_BKIN_POLARITY_HIGH
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetBreakInputSourcePolarity(TIM_TypeDef *TIMx, uint32_t BreakInput, uint32_t Source,
                                                        uint32_t Polarity)
{
  __IO uint32_t *pReg = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMx->AF1) + BreakInput));
  MODIFY_REG(*pReg, (TIMx_AF1_BKINP << TIM_POSITION_BRK_SOURCE), (Polarity << TIM_POSITION_BRK_SOURCE));
}
/**
  * @brief  Enable asymmetrical deadtime.
  * @note Macro IS_TIM_DEADTIME_ASYMMETRICAL_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides asymmetrical deadtime.
  * @rmtoll DTR2          DTAE          LL_TIM_EnableAsymmetricalDeadTime
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableAsymmetricalDeadTime(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DTR2, TIM_DTR2_DTAE);
}

/**
  * @brief  Disable asymmetrical dead-time.
  * @note Macro IS_TIM_DEADTIME_ASYMMETRICAL_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides asymmetrical deadtime.
  * @rmtoll DTR2          DTAE          LL_TIM_DisableAsymmetricalDeadTime
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableAsymmetricalDeadTime(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DTR2, TIM_DTR2_DTAE);
}

/**
  * @brief  Indicates whether asymmetrical deadtime is activated.
  * @note Macro IS_TIM_DEADTIME_ASYMMETRICAL_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides asymmetrical deadtime.
  * @rmtoll DTR2          DTAE          LL_TIM_IsEnabledAsymmetricalDeadTime
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledAsymmetricalDeadTime(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DTR2, TIM_DTR2_DTAE) == (TIM_DTR2_DTAE)) ? 1UL : 0UL);
}

/**
  * @brief  Set the falling edge dead-time delay (delay inserted between the falling edge of the OCxREF signal and the
  *         rising edge of OCxN signals).
  * @note Macro IS_TIM_DEADTIME_ASYMMETRICAL_INSTANCE(TIMx) can be used to check whether or not
  *       asymmetrical dead-time insertion feature is supported by a timer instance.
  * @note Helper macro @ref __LL_TIM_CALC_DEADTIME can be used to calculate the DeadTime parameter
  * @note This bit-field can not be modified as long as LOCK level 1, 2 or 3 has been programmed
  *       (LOCK bits in TIMx_BDTR register).
  * @rmtoll DTR2         DTGF           LL_TIM_SetFallingDeadTime
  * @param  TIMx Timer instance
  * @param  DeadTime between Min_Data=0 and Max_Data=255
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetFallingDeadTime(TIM_TypeDef *TIMx, uint32_t DeadTime)
{
  MODIFY_REG(TIMx->DTR2, TIM_DTR2_DTGF, DeadTime);
}

/**
  * @brief  Get the falling edge dead-time delay (delay inserted between the falling edge of the OCxREF signal and
  *         the rising edge of OCxN signals).
  * @note Macro IS_TIM_DEADTIME_ASYMMETRICAL_INSTANCE(TIMx) can be used to check whether or not
  *       asymmetrical dead-time insertion feature is supported by a timer instance.
  * @note This bit-field can not be modified as long as LOCK level 1, 2 or 3 has been programmed
  *       (LOCK bits in TIMx_BDTR register).
  * @rmtoll DTR2          DTGF           LL_TIM_GetFallingDeadTime
  * @param  TIMx Timer instance
  * @retval Returned value can be between Min_Data=0 and Max_Data=255:
  */
__STATIC_INLINE uint32_t LL_TIM_GetFallingDeadTime(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_BIT(TIMx->DTR2, TIM_DTR2_DTGF));
}

/**
  * @brief  Enable deadtime preload.
  * @note Macro IS_TIM_BREAK_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides deadtime preload.
  * @rmtoll DTR2          DTPE          LL_TIM_EnableDeadTimePreload
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableDeadTimePreload(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DTR2, TIM_DTR2_DTPE);
}

/**
  * @brief  Disable dead-time preload.
  * @note Macro IS_TIM_BREAK_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides deadtime preload.
  * @rmtoll DTR2          DTPE          LL_TIM_DisableDeadTimePreload
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableDeadTimePreload(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DTR2, TIM_DTR2_DTPE);
}

/**
  * @brief  Indicates whether deadtime preload is activated.
  * @note Macro IS_TIM_BREAK_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides deadtime preload.
  * @rmtoll DTR2          DTPE          LL_TIM_IsEnabledDeadTimePreload
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledDeadTimePreload(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DTR2, TIM_DTR2_DTPE) == (TIM_DTR2_DTPE)) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup TIM_LL_EF_DMA_Burst_Mode DMA burst mode configuration
  * @{
  */
/**
  * @brief  Configures the timer DMA burst feature.
  * @note Macro IS_TIM_DMABURST_INSTANCE(TIMx) can be used to check whether or
  *       not a timer instance supports the DMA burst mode.
  * @rmtoll DCR          DBL           LL_TIM_ConfigDMABurst\n
  *         DCR          DBA           LL_TIM_ConfigDMABurst
  * @param  TIMx Timer instance
  * @param  DMABurstBaseAddress This parameter can be one of the following values:
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_CR1
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_CR2
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_SMCR
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_DIER
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_SR
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_EGR
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_CCMR1
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_CCMR2
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_CCER
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_CNT
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_PSC
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_ARR
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_RCR
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_CCR1
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_CCR2
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_CCR3
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_CCR4
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_BDTR
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_CCR5
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_CCR6
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_CCMR3
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_DTR2
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_ECR
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_TISEL
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_AF1
  *         @arg @ref LL_TIM_DMABURST_BASEADDR_AF2
  * @param  DMABurstLength This parameter can be one of the following values:
  *         @arg @ref LL_TIM_DMABURST_LENGTH_1TRANSFER
  *         @arg @ref LL_TIM_DMABURST_LENGTH_2TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_3TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_4TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_5TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_6TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_7TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_8TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_9TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_10TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_11TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_12TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_13TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_14TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_15TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_16TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_17TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_18TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_19TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_20TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_21TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_22TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_23TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_24TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_25TRANSFERS
  *         @arg @ref LL_TIM_DMABURST_LENGTH_26TRANSFERS
  * @param  DMABurstSource This parameter can be one of the following values:
  *         @arg @ref LL_TIM_DMA_UPDATE
  *         @arg @ref LL_TIM_DMA_CC1
  *         @arg @ref LL_TIM_DMA_CC2
  *         @arg @ref LL_TIM_DMA_CC3
  *         @arg @ref LL_TIM_DMA_CC4
  *         @arg @ref LL_TIM_DMA_COM
  *         @arg @ref LL_TIM_DMA_TRIGGER
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ConfigDMABurst(TIM_TypeDef *TIMx, uint32_t DMABurstBaseAddress, uint32_t DMABurstLength,
                                           uint32_t DMABurstSource)
{
  MODIFY_REG(TIMx->DCR, (TIM_DCR_DBL | TIM_DCR_DBA | TIM_DCR_DBSS),
             (DMABurstBaseAddress | DMABurstLength | DMABurstSource));
}

/**
  * @}
  */

/** @defgroup TIM_LL_EF_Encoder Encoder configuration
  * @{
  */

/**
  * @brief  Enable encoder index.
  * @note Macro IS_TIM_INDEX_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides an index input.
  * @rmtoll ECR         IE           LL_TIM_EnableEncoderIndex
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableEncoderIndex(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->ECR, TIM_ECR_IE);
}

/**
  * @brief  Disable encoder index.
  * @note Macro IS_TIM_INDEX_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides an index input.
  * @rmtoll ECR         IE           LL_TIM_DisableEncoderIndex
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableEncoderIndex(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->ECR, TIM_ECR_IE);
}

/**
  * @brief  Indicate whether encoder index is enabled.
  * @note Macro IS_TIM_INDEX_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides an index input.
  * @rmtoll ECR         IE           LL_TIM_IsEnabledEncoderIndex
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledEncoderIndex(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->ECR, TIM_ECR_IE) == (TIM_ECR_IE)) ? 1U : 0U);
}

/**
  * @brief  Set index direction
  * @note Macro IS_TIM_INDEX_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides an index input.
  * @rmtoll ECR          IDIR           LL_TIM_SetIndexDirection
  * @param  TIMx Timer instance
  * @param  IndexDirection This parameter can be one of the following values:
  *         @arg @ref LL_TIM_INDEX_UP_DOWN
  *         @arg @ref LL_TIM_INDEX_UP
  *         @arg @ref LL_TIM_INDEX_DOWN
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetIndexDirection(TIM_TypeDef *TIMx, uint32_t IndexDirection)
{
  MODIFY_REG(TIMx->ECR, TIM_ECR_IDIR, IndexDirection);
}

/**
  * @brief  Get actual index direction
  * @note Macro IS_TIM_INDEX_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides an index input.
  * @rmtoll ECR          IDIR           LL_TIM_GetIndexDirection
  * @param  TIMx Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_INDEX_UP_DOWN
  *         @arg @ref LL_TIM_INDEX_UP
  *         @arg @ref LL_TIM_INDEX_DOWN
  */
__STATIC_INLINE uint32_t LL_TIM_GetIndexDirection(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_BIT(TIMx->ECR, TIM_ECR_IDIR));
}

/**
  * @brief  Set index blanking
  * @note Macro IS_TIM_INDEX_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides an index input.
  * @rmtoll ECR          IBLK           LL_TIM_SetIndexblanking
  * @param  TIMx Timer instance
  * @param  Indexblanking This parameter can be one of the following values:
  *         @arg @ref LL_TIM_INDEX_BLANK_ALWAYS
  *         @arg @ref LL_TIM_INDEX_BLANK_TI3
  *         @arg @ref LL_TIM_INDEX_BLANK_TI4
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetIndexblanking(TIM_TypeDef *TIMx, uint32_t Indexblanking)
{
  MODIFY_REG(TIMx->ECR, TIM_ECR_IBLK, Indexblanking);
}

/**
  * @brief  Get actual index blanking
  * @note Macro IS_TIM_INDEX_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides an index input.
  * @rmtoll ECR          IBLK           LL_TIM_GetIndexblanking
  * @param  TIMx Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_INDEX_BLANK_ALWAYS
  *         @arg @ref LL_TIM_INDEX_BLANK_TI3
  *         @arg @ref LL_TIM_INDEX_BLANK_TI4
  */
__STATIC_INLINE uint32_t LL_TIM_GetIndexblanking(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_BIT(TIMx->ECR, TIM_ECR_IBLK));
}


/**
  * @brief  Enable first index.
  * @note Macro IS_TIM_INDEX_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides an index input.
  * @rmtoll ECR          FIDX          LL_TIM_EnableFirstIndex
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableFirstIndex(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->ECR, TIM_ECR_FIDX);
}

/**
  * @brief  Disable first index.
  * @note Macro IS_TIM_INDEX_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides an index input.
  * @rmtoll ECR          FIDX          LL_TIM_DisableFirstIndex
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableFirstIndex(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->ECR, TIM_ECR_FIDX);
}

/**
  * @brief  Indicates whether first index is enabled.
  * @note Macro IS_TIM_INDEX_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides an index input.
  * @rmtoll ECR          FIDX          LL_TIM_IsEnabledFirstIndex
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledFirstIndex(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->ECR, TIM_ECR_FIDX) == (TIM_ECR_FIDX)) ? 1UL : 0UL);
}

/**
  * @brief  Set index positioning
  * @note Macro IS_TIM_INDEX_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides an index input.
  * @rmtoll ECR          IPOS           LL_TIM_SetIndexPositionning
  * @param  TIMx Timer instance
  * @param  IndexPositionning This parameter can be one of the following values:
  *         @arg @ref LL_TIM_INDEX_POSITION_DOWN_DOWN
  *         @arg @ref LL_TIM_INDEX_POSITION_DOWN_UP
  *         @arg @ref LL_TIM_INDEX_POSITION_UP_DOWN
  *         @arg @ref LL_TIM_INDEX_POSITION_UP_UP
  *         @arg @ref LL_TIM_INDEX_POSITION_DOWN
  *         @arg @ref LL_TIM_INDEX_POSITION_UP
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetIndexPositionning(TIM_TypeDef *TIMx, uint32_t IndexPositionning)
{
  MODIFY_REG(TIMx->ECR, TIM_ECR_IPOS, IndexPositionning);
}

/**
  * @brief  Get actual index positioning
  * @note Macro IS_TIM_INDEX_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides an index input.
  * @rmtoll ECR          IPOS           LL_TIM_GetIndexPositionning
  * @param  TIMx Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_TIM_INDEX_POSITION_DOWN_DOWN
  *         @arg @ref LL_TIM_INDEX_POSITION_DOWN_UP
  *         @arg @ref LL_TIM_INDEX_POSITION_UP_DOWN
  *         @arg @ref LL_TIM_INDEX_POSITION_UP_UP
  *         @arg @ref LL_TIM_INDEX_POSITION_DOWN
  *         @arg @ref LL_TIM_INDEX_POSITION_UP
  */
__STATIC_INLINE uint32_t LL_TIM_GetIndexPositionning(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_BIT(TIMx->ECR, TIM_ECR_IPOS));
}

/**
  * @brief  Configure encoder index.
  * @note Macro IS_TIM_INDEX_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides an index input.
  * @rmtoll ECR          IDIR          LL_TIM_ConfigIDX\n
  *         ECR          IBLK          LL_TIM_ConfigIDX\n
  *         ECR          FIDX          LL_TIM_ConfigIDX\n
  *         ECR          IPOS          LL_TIM_ConfigIDX
  * @param  TIMx Timer instance
  * @param  Configuration This parameter must be a combination of all the following values:
  *         @arg @ref LL_TIM_INDEX_UP or @ref LL_TIM_INDEX_DOWN or @ref LL_TIM_INDEX_UP_DOWN
  *         @arg @ref LL_TIM_INDEX_BLANK_ALWAYS or @ref LL_TIM_INDEX_BLANK_TI3 or @ref LL_TIM_INDEX_BLANK_TI4
  *         @arg @ref LL_TIM_INDEX_ALL or @ref LL_TIM_INDEX_FIRST_ONLY
  *         @arg @ref LL_TIM_INDEX_POSITION_DOWN_DOWN or ... or @ref LL_TIM_INDEX_POSITION_UP
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ConfigIDX(TIM_TypeDef *TIMx, uint32_t Configuration)
{
  MODIFY_REG(TIMx->ECR, TIM_ECR_IDIR | TIM_ECR_IBLK | TIM_ECR_FIDX | TIM_ECR_IPOS, Configuration);
}

/**
  * @}
  */

/** @defgroup TIM_LL_EF_Timer_Inputs_Remapping Timer input remapping
  * @{
  */
/**
  * @brief  Remap TIM inputs (input channel, internal/external triggers).
  * @note Macro IS_TIM_REMAP_INSTANCE(TIMx) can be used to check whether or not
  *       a some timer inputs can be remapped.
  * @rmtoll TIM3_TISEL    TI1SEL      LL_TIM_SetRemap\n
  *         TIM5_TISEL    TI1SEL      LL_TIM_SetRemap\n
  *         TIM9_TISEL    TI1SEL      LL_TIM_SetRemap\n
  *         TIM10_TISEL   TI1SEL      LL_TIM_SetRemap\n
  *         TIM11_TISEL   TI1SEL      LL_TIM_SetRemap\n
  *         TIM12_TISEL   TI1SEL      LL_TIM_SetRemap\n
  *         TIM13_TISEL   TI1SEL      LL_TIM_SetRemap\n
  *         TIM14_TISEL   TI1SEL      LL_TIM_SetRemap\n
  *         TIM15_TISEL   TI1SEL      LL_TIM_SetRemap\n
  *         TIM16_TISEL   TI1SEL      LL_TIM_SetRemap\n
  *         TIM17_TISEL   TI1SEL      LL_TIM_SetRemap\n
  *
  * @param  TIMx Timer instance
  * @param  Remap Remap param depends on the TIMx. Description available only
  *         in CHM version of the User Manual (not in .pdf).
  *         Otherwise see Reference Manual description of TISEL registers.
  *
  *         Below description summarizes "Timer Instance" and "Remap" param combinations:
  *
  *         TIM3: one of the following values:
  *            @arg @ref LL_TIM_TIM3_TI1_RMP_GPIO
  *            @arg @ref LL_TIM_TIM3_TI1_RMP_ETH1_PPS
  *            @arg @ref LL_TIM_TIM3_TI1_RMP_FDCAN_RTP
  *            @arg @ref LL_TIM_TIM3_TI1_RMP_FDCAN_TMP
  *            @arg @ref LL_TIM_TIM3_TI1_RMP_FDCAN_SOC
  *
  *         TIM5: one of the following values:
  *            @arg @ref LL_TIM_TIM5_TI1_RMP_GPIO
  *            @arg @ref LL_TIM_TIM5_TI1_RMP_FDCAN_RTP
  *            @arg @ref LL_TIM_TIM5_TI1_RMP_FDCAN_TMP
  *
  *         TIM9: one of the following values:
  *            @arg @ref LL_TIM_TIM9_TI1_RMP_GPIO
  *            @arg @ref LL_TIM_TIM9_TI1_RMP_MCO1
  *            @arg @ref LL_TIM_TIM9_TI1_RMP_MCO2
  *
  *         TIM10: one of the following values:
  *            @arg @ref LL_TIM_TIM10_TI1_RMP_GPIO
  *            @arg @ref LL_TIM_TIM10_TI1_RMP_I3C1_IBIACK
  *
  *         TIM11: one of the following values:
  *            @arg @ref LL_TIM_TIM11_TI1_RMP_GPIO
  *            @arg @ref LL_TIM_TIM11_TI1_RMP_I3C2_IBIACK
  *
  *         TIM12: one of the following values:
  *            @arg @ref LL_TIM_TIM12_TI1_RMP_GPIO
  *            @arg @ref LL_TIM_TIM12_TI1_RMP_SPDIF_FS
  *            @arg @ref LL_TIM_TIM12_TI1_RMP_HSI_1024
  *            @arg @ref LL_TIM_TIM12_TI1_RMP_MSI_128
  *            @arg @ref LL_TIM_TIM12_TI1_RMP_MCO1
  *            @arg @ref LL_TIM_TIM12_TI1_RMP_MCO2
  *
  *         TIM13: one of the following values:
  *            @arg @ref LL_TIM_TIM13_TI1_RMP_GPIO
  *            @arg @ref LL_TIM_TIM13_TI1_RMP_I3C1_IBIACK
  *
  *         TIM14: one of the following values:
  *            @arg @ref LL_TIM_TIM14_TI1_RMP_GPIO
  *            @arg @ref LL_TIM_TIM14_TI1_RMP_I3C2_IBIACK
  *
  *         TIM15: any combination of TI1_RMP and TI2_RMP where
  *
  *            . . TI1_RMP can be one of the following values
  *            @arg @ref LL_TIM_TIM15_TI1_RMP_GPIO
  *            @arg @ref LL_TIM_TIM15_TI1_RMP_TIM2_CH1
  *            @arg @ref LL_TIM_TIM15_TI1_RMP_TIM3_CH1
  *            @arg @ref LL_TIM_TIM15_TI1_RMP_TIM4_CH1
  *            @arg @ref LL_TIM_TIM15_TI1_RMP_MCO1
  *            @arg @ref LL_TIM_TIM15_TI1_RMP_MCO2
  *
  *            . . TI2_RMP can be one of the following values
  *            @arg @ref LL_TIM_TIM15_TI2_RMP_GPIO
  *            @arg @ref LL_TIM_TIM15_TI2_RMP_TIM2_CH2
  *            @arg @ref LL_TIM_TIM15_TI2_RMP_TIM3_CH2
  *            @arg @ref LL_TIM_TIM15_TI2_RMP_TIM4_CH2
  *
  *         TIM16: one of the following values:
  *            @arg @ref LL_TIM_TIM16_TI1_RMP_GPIO
  *            @arg @ref LL_TIM_TIM16_TI1_RMP_LSI
  *            @arg @ref LL_TIM_TIM16_TI1_RMP_LSE
  *            @arg @ref LL_TIM_TIM16_TI1_RMP_RTC_WKUP
  *
  *         TIM17: one of the following values:
  *            @arg @ref LL_TIM_TIM17_TI1_RMP_GPIO
  *            @arg @ref LL_TIM_TIM17_TI1_RMP_SPDIF_FS
  *            @arg @ref LL_TIM_TIM17_TI1_RMP_HSE_1024
  *
  * @retval None
  */
__STATIC_INLINE void LL_TIM_SetRemap(TIM_TypeDef *TIMx, uint32_t Remap)
{
  MODIFY_REG(TIMx->TISEL, (TIM_TISEL_TI1SEL | TIM_TISEL_TI2SEL | TIM_TISEL_TI3SEL | TIM_TISEL_TI4SEL), Remap);
}

/**
  * @}
  */

/** @defgroup TIM_LL_EF_FLAG_Management FLAG-Management
  * @{
  */
/**
  * @brief  Clear the update interrupt flag (UIF).
  * @rmtoll SR           UIF           LL_TIM_ClearFlag_UPDATE
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_UPDATE(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_UIF));
}

/**
  * @brief  Indicate whether update interrupt flag (UIF) is set (update interrupt is pending).
  * @rmtoll SR           UIF           LL_TIM_IsActiveFlag_UPDATE
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_UPDATE(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_UIF) == (TIM_SR_UIF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the Capture/Compare 1 interrupt flag (CC1F).
  * @rmtoll SR           CC1IF         LL_TIM_ClearFlag_CC1
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_CC1(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_CC1IF));
}

/**
  * @brief  Indicate whether Capture/Compare 1 interrupt flag (CC1F) is set (Capture/Compare 1 interrupt is pending).
  * @rmtoll SR           CC1IF         LL_TIM_IsActiveFlag_CC1
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_CC1(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_CC1IF) == (TIM_SR_CC1IF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the Capture/Compare 2 interrupt flag (CC2F).
  * @rmtoll SR           CC2IF         LL_TIM_ClearFlag_CC2
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_CC2(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_CC2IF));
}

/**
  * @brief  Indicate whether Capture/Compare 2 interrupt flag (CC2F) is set (Capture/Compare 2 interrupt is pending).
  * @rmtoll SR           CC2IF         LL_TIM_IsActiveFlag_CC2
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_CC2(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_CC2IF) == (TIM_SR_CC2IF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the Capture/Compare 3 interrupt flag (CC3F).
  * @rmtoll SR           CC3IF         LL_TIM_ClearFlag_CC3
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_CC3(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_CC3IF));
}

/**
  * @brief  Indicate whether Capture/Compare 3 interrupt flag (CC3F) is set (Capture/Compare 3 interrupt is pending).
  * @rmtoll SR           CC3IF         LL_TIM_IsActiveFlag_CC3
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_CC3(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_CC3IF) == (TIM_SR_CC3IF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the Capture/Compare 4 interrupt flag (CC4F).
  * @rmtoll SR           CC4IF         LL_TIM_ClearFlag_CC4
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_CC4(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_CC4IF));
}

/**
  * @brief  Indicate whether Capture/Compare 4 interrupt flag (CC4F) is set (Capture/Compare 4 interrupt is pending).
  * @rmtoll SR           CC4IF         LL_TIM_IsActiveFlag_CC4
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_CC4(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_CC4IF) == (TIM_SR_CC4IF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the Capture/Compare 5 interrupt flag (CC5F).
  * @rmtoll SR           CC5IF         LL_TIM_ClearFlag_CC5
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_CC5(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_CC5IF));
}

/**
  * @brief  Indicate whether Capture/Compare 5 interrupt flag (CC5F) is set (Capture/Compare 5 interrupt is pending).
  * @rmtoll SR           CC5IF         LL_TIM_IsActiveFlag_CC5
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_CC5(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_CC5IF) == (TIM_SR_CC5IF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the Capture/Compare 6 interrupt flag (CC6F).
  * @rmtoll SR           CC6IF         LL_TIM_ClearFlag_CC6
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_CC6(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_CC6IF));
}

/**
  * @brief  Indicate whether Capture/Compare 6 interrupt flag (CC6F) is set (Capture/Compare 6 interrupt is pending).
  * @rmtoll SR           CC6IF         LL_TIM_IsActiveFlag_CC6
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_CC6(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_CC6IF) == (TIM_SR_CC6IF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the commutation interrupt flag (COMIF).
  * @rmtoll SR           COMIF         LL_TIM_ClearFlag_COM
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_COM(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_COMIF));
}

/**
  * @brief  Indicate whether commutation interrupt flag (COMIF) is set (commutation interrupt is pending).
  * @rmtoll SR           COMIF         LL_TIM_IsActiveFlag_COM
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_COM(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_COMIF) == (TIM_SR_COMIF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the trigger interrupt flag (TIF).
  * @rmtoll SR           TIF           LL_TIM_ClearFlag_TRIG
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_TRIG(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_TIF));
}

/**
  * @brief  Indicate whether trigger interrupt flag (TIF) is set (trigger interrupt is pending).
  * @rmtoll SR           TIF           LL_TIM_IsActiveFlag_TRIG
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_TRIG(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_TIF) == (TIM_SR_TIF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the break interrupt flag (BIF).
  * @rmtoll SR           BIF           LL_TIM_ClearFlag_BRK
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_BRK(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_BIF));
}

/**
  * @brief  Indicate whether break interrupt flag (BIF) is set (break interrupt is pending).
  * @rmtoll SR           BIF           LL_TIM_IsActiveFlag_BRK
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_BRK(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_BIF) == (TIM_SR_BIF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the break 2 interrupt flag (B2IF).
  * @rmtoll SR           B2IF          LL_TIM_ClearFlag_BRK2
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_BRK2(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_B2IF));
}

/**
  * @brief  Indicate whether break 2 interrupt flag (B2IF) is set (break 2 interrupt is pending).
  * @rmtoll SR           B2IF          LL_TIM_IsActiveFlag_BRK2
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_BRK2(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_B2IF) == (TIM_SR_B2IF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the Capture/Compare 1 over-capture interrupt flag (CC1OF).
  * @rmtoll SR           CC1OF         LL_TIM_ClearFlag_CC1OVR
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_CC1OVR(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_CC1OF));
}

/**
  * @brief  Indicate whether Capture/Compare 1 over-capture interrupt flag (CC1OF) is set
  *         (Capture/Compare 1 interrupt is pending).
  * @rmtoll SR           CC1OF         LL_TIM_IsActiveFlag_CC1OVR
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_CC1OVR(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_CC1OF) == (TIM_SR_CC1OF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the Capture/Compare 2 over-capture interrupt flag (CC2OF).
  * @rmtoll SR           CC2OF         LL_TIM_ClearFlag_CC2OVR
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_CC2OVR(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_CC2OF));
}

/**
  * @brief  Indicate whether Capture/Compare 2 over-capture interrupt flag (CC2OF) is set
  *         (Capture/Compare 2 over-capture interrupt is pending).
  * @rmtoll SR           CC2OF         LL_TIM_IsActiveFlag_CC2OVR
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_CC2OVR(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_CC2OF) == (TIM_SR_CC2OF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the Capture/Compare 3 over-capture interrupt flag (CC3OF).
  * @rmtoll SR           CC3OF         LL_TIM_ClearFlag_CC3OVR
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_CC3OVR(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_CC3OF));
}

/**
  * @brief  Indicate whether Capture/Compare 3 over-capture interrupt flag (CC3OF) is set
  *         (Capture/Compare 3 over-capture interrupt is pending).
  * @rmtoll SR           CC3OF         LL_TIM_IsActiveFlag_CC3OVR
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_CC3OVR(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_CC3OF) == (TIM_SR_CC3OF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the Capture/Compare 4 over-capture interrupt flag (CC4OF).
  * @rmtoll SR           CC4OF         LL_TIM_ClearFlag_CC4OVR
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_CC4OVR(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_CC4OF));
}

/**
  * @brief  Indicate whether Capture/Compare 4 over-capture interrupt flag (CC4OF) is set
  *         (Capture/Compare 4 over-capture interrupt is pending).
  * @rmtoll SR           CC4OF         LL_TIM_IsActiveFlag_CC4OVR
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_CC4OVR(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_CC4OF) == (TIM_SR_CC4OF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the system break interrupt flag (SBIF).
  * @rmtoll SR           SBIF          LL_TIM_ClearFlag_SYSBRK
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_SYSBRK(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_SBIF));
}

/**
  * @brief  Indicate whether system break interrupt flag (SBIF) is set (system break interrupt is pending).
  * @rmtoll SR           SBIF          LL_TIM_IsActiveFlag_SYSBRK
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_SYSBRK(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_SBIF) == (TIM_SR_SBIF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the transition error interrupt flag (TERRF).
  * @note Macro IS_TIM_ENCODER_ERROR_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder error management.
  * @rmtoll SR           TERRF           LL_TIM_ClearFlag_TERR
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_TERR(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_TERRF));
}

/**
  * @brief  Indicate whether transition error interrupt flag (TERRF) is set (transition error interrupt is pending).
  * @note Macro IS_TIM_ENCODER_ERROR_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder error management.
  * @rmtoll SR           TERRF           LL_TIM_IsActiveFlag_TERR
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_TERR(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_TERRF) == (TIM_SR_TERRF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the index error interrupt flag (IERRF).
  * @note Macro IS_TIM_ENCODER_ERROR_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder error management.
  * @rmtoll SR           IERRF           LL_TIM_ClearFlag_IERR
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_IERR(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_IERRF));
}

/**
  * @brief  Indicate whether index error interrupt flag (IERRF) is set (index error interrupt is pending).
  * @note Macro IS_TIM_ENCODER_ERROR_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder error management.
  * @rmtoll SR           IERRF           LL_TIM_IsActiveFlag_IERR
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_IERR(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_IERRF) == (TIM_SR_IERRF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the direction change interrupt flag (DIRF).
  * @note Macro IS_TIM_FUNCTINONAL_ENCODER_INTERRUPT_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder interrupt management.
  * @rmtoll SR           DIRF           LL_TIM_ClearFlag_DIR
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_DIR(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_DIRF));
}

/**
  * @brief  Indicate whether direction change interrupt flag (DIRF) is set (direction change interrupt is pending).
  * @note Macro IS_TIM_FUNCTINONAL_ENCODER_INTERRUPT_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder interrupt management.
  * @rmtoll SR           DIRF           LL_TIM_IsActiveFlag_DIR
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_DIR(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_DIRF) == (TIM_SR_DIRF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the index interrupt flag (IDXF).
  * @note Macro IS_TIM_FUNCTINONAL_ENCODER_INTERRUPT_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder interrupt management.
  * @rmtoll SR           IDXF           LL_TIM_ClearFlag_IDX
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_ClearFlag_IDX(TIM_TypeDef *TIMx)
{
  WRITE_REG(TIMx->SR, ~(TIM_SR_IDXF));
}

/**
  * @brief  Indicate whether index interrupt flag (IDXF) is set (index interrupt is pending).
  * @note Macro IS_TIM_FUNCTINONAL_ENCODER_INTERRUPT_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder interrupt management.
  * @rmtoll SR           IDXF           LL_TIM_IsActiveFlag_IDX
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsActiveFlag_IDX(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->SR, TIM_SR_IDXF) == (TIM_SR_IDXF)) ? 1UL : 0UL);
}
/**
  * @}
  */

/** @defgroup TIM_LL_EF_IT_Management IT-Management
  * @{
  */
/**
  * @brief  Enable update interrupt (UIE).
  * @rmtoll DIER         UIE           LL_TIM_EnableIT_UPDATE
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableIT_UPDATE(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_UIE);
}

/**
  * @brief  Disable update interrupt (UIE).
  * @rmtoll DIER         UIE           LL_TIM_DisableIT_UPDATE
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableIT_UPDATE(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_UIE);
}

/**
  * @brief  Indicates whether the update interrupt (UIE) is enabled.
  * @rmtoll DIER         UIE           LL_TIM_IsEnabledIT_UPDATE
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledIT_UPDATE(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_UIE) == (TIM_DIER_UIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable capture/compare 1 interrupt (CC1IE).
  * @rmtoll DIER         CC1IE         LL_TIM_EnableIT_CC1
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableIT_CC1(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_CC1IE);
}

/**
  * @brief  Disable capture/compare 1  interrupt (CC1IE).
  * @rmtoll DIER         CC1IE         LL_TIM_DisableIT_CC1
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableIT_CC1(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_CC1IE);
}

/**
  * @brief  Indicates whether the capture/compare 1 interrupt (CC1IE) is enabled.
  * @rmtoll DIER         CC1IE         LL_TIM_IsEnabledIT_CC1
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledIT_CC1(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_CC1IE) == (TIM_DIER_CC1IE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable capture/compare 2 interrupt (CC2IE).
  * @rmtoll DIER         CC2IE         LL_TIM_EnableIT_CC2
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableIT_CC2(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_CC2IE);
}

/**
  * @brief  Disable capture/compare 2  interrupt (CC2IE).
  * @rmtoll DIER         CC2IE         LL_TIM_DisableIT_CC2
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableIT_CC2(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_CC2IE);
}

/**
  * @brief  Indicates whether the capture/compare 2 interrupt (CC2IE) is enabled.
  * @rmtoll DIER         CC2IE         LL_TIM_IsEnabledIT_CC2
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledIT_CC2(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_CC2IE) == (TIM_DIER_CC2IE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable capture/compare 3 interrupt (CC3IE).
  * @rmtoll DIER         CC3IE         LL_TIM_EnableIT_CC3
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableIT_CC3(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_CC3IE);
}

/**
  * @brief  Disable capture/compare 3  interrupt (CC3IE).
  * @rmtoll DIER         CC3IE         LL_TIM_DisableIT_CC3
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableIT_CC3(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_CC3IE);
}

/**
  * @brief  Indicates whether the capture/compare 3 interrupt (CC3IE) is enabled.
  * @rmtoll DIER         CC3IE         LL_TIM_IsEnabledIT_CC3
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledIT_CC3(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_CC3IE) == (TIM_DIER_CC3IE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable capture/compare 4 interrupt (CC4IE).
  * @rmtoll DIER         CC4IE         LL_TIM_EnableIT_CC4
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableIT_CC4(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_CC4IE);
}

/**
  * @brief  Disable capture/compare 4  interrupt (CC4IE).
  * @rmtoll DIER         CC4IE         LL_TIM_DisableIT_CC4
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableIT_CC4(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_CC4IE);
}

/**
  * @brief  Indicates whether the capture/compare 4 interrupt (CC4IE) is enabled.
  * @rmtoll DIER         CC4IE         LL_TIM_IsEnabledIT_CC4
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledIT_CC4(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_CC4IE) == (TIM_DIER_CC4IE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable commutation interrupt (COMIE).
  * @rmtoll DIER         COMIE         LL_TIM_EnableIT_COM
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableIT_COM(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_COMIE);
}

/**
  * @brief  Disable commutation interrupt (COMIE).
  * @rmtoll DIER         COMIE         LL_TIM_DisableIT_COM
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableIT_COM(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_COMIE);
}

/**
  * @brief  Indicates whether the commutation interrupt (COMIE) is enabled.
  * @rmtoll DIER         COMIE         LL_TIM_IsEnabledIT_COM
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledIT_COM(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_COMIE) == (TIM_DIER_COMIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable trigger interrupt (TIE).
  * @rmtoll DIER         TIE           LL_TIM_EnableIT_TRIG
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableIT_TRIG(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_TIE);
}

/**
  * @brief  Disable trigger interrupt (TIE).
  * @rmtoll DIER         TIE           LL_TIM_DisableIT_TRIG
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableIT_TRIG(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_TIE);
}

/**
  * @brief  Indicates whether the trigger interrupt (TIE) is enabled.
  * @rmtoll DIER         TIE           LL_TIM_IsEnabledIT_TRIG
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledIT_TRIG(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_TIE) == (TIM_DIER_TIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable break interrupt (BIE).
  * @rmtoll DIER         BIE           LL_TIM_EnableIT_BRK
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableIT_BRK(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_BIE);
}

/**
  * @brief  Disable break interrupt (BIE).
  * @rmtoll DIER         BIE           LL_TIM_DisableIT_BRK
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableIT_BRK(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_BIE);
}

/**
  * @brief  Indicates whether the break interrupt (BIE) is enabled.
  * @rmtoll DIER         BIE           LL_TIM_IsEnabledIT_BRK
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledIT_BRK(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_BIE) == (TIM_DIER_BIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable transition error interrupt (TERRIE).
  * @note Macro IS_TIM_ENCODER_ERROR_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder error management.
  * @rmtoll DIER         TERRIE           LL_TIM_EnableIT_TERR
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableIT_TERR(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_TERRIE);
}

/**
  * @brief  Disable transition error interrupt (TERRIE).
  * @note Macro IS_TIM_ENCODER_ERROR_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder error management.
  * @rmtoll DIER         TERRIE           LL_TIM_DisableIT_TERR
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableIT_TERR(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_TERRIE);
}

/**
  * @brief  Indicates whether the transition error interrupt (TERRIE) is enabled.
  * @note Macro IS_TIM_ENCODER_ERROR_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder error management.
  * @rmtoll DIER         TERRIE           LL_TIM_IsEnabledIT_TERR
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledIT_TERR(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_TERRIE) == (TIM_DIER_TERRIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable index error interrupt (IERRIE).
  * @note Macro IS_TIM_ENCODER_ERROR_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder error management.
  * @rmtoll DIER         IERRIE           LL_TIM_EnableIT_IERR
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableIT_IERR(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_IERRIE);
}

/**
  * @brief  Disable index error interrupt (IERRIE).
  * @note Macro IS_TIM_ENCODER_ERROR_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder error management.
  * @rmtoll DIER         IERRIE           LL_TIM_DisableIT_IERR
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableIT_IERR(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_IERRIE);
}

/**
  * @brief  Indicates whether the index error interrupt (IERRIE) is enabled.
  * @note Macro IS_TIM_ENCODER_ERROR_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder error management.
  * @rmtoll DIER         IERRIE           LL_TIM_IsEnabledIT_IERR
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledIT_IERR(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_IERRIE) == (TIM_DIER_IERRIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable direction change interrupt (DIRIE).
  * @note Macro IS_TIM_FUNCTINONAL_ENCODER_INTERRUPT_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder interrupt management.
  * @rmtoll DIER         DIRIE           LL_TIM_EnableIT_DIR
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableIT_DIR(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_DIRIE);
}

/**
  * @brief  Disable direction change interrupt (DIRIE).
  * @note Macro IS_TIM_FUNCTINONAL_ENCODER_INTERRUPT_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder interrupt management.
  * @rmtoll DIER         DIRIE           LL_TIM_DisableIT_DIR
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableIT_DIR(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_DIRIE);
}

/**
  * @brief  Indicates whether the direction change interrupt (DIRIE) is enabled.
  * @note Macro IS_TIM_FUNCTINONAL_ENCODER_INTERRUPT_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder interrupt management.
  * @rmtoll DIER         DIRIE           LL_TIM_IsEnabledIT_DIR
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledIT_DIR(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_DIRIE) == (TIM_DIER_DIRIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable index interrupt (IDXIE).
  * @note Macro IS_TIM_FUNCTINONAL_ENCODER_INTERRUPT_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder interrupt management.
  * @rmtoll DIER         IDXIE           LL_TIM_EnableIT_IDX
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableIT_IDX(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_IDXIE);
}

/**
  * @brief  Disable index interrupt (IDXIE).
  * @note Macro IS_TIM_FUNCTINONAL_ENCODER_INTERRUPT_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder interrupt management.
  * @rmtoll DIER         IDXIE           LL_TIM_DisableIT_IDX
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableIT_IDX(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_IDXIE);
}

/**
  * @brief  Indicates whether the index interrupt (IDXIE) is enabled.
  * @note Macro IS_TIM_FUNCTINONAL_ENCODER_INTERRUPT_INSTANCE(TIMx) can be used to check whether or not
  *       a timer instance provides encoder interrupt management.
  * @rmtoll DIER         IDXIE           LL_TIM_IsEnabledIT_IDX
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledIT_IDX(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_IDXIE) == (TIM_DIER_IDXIE)) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup TIM_LL_EF_DMA_Management DMA Management
  * @{
  */
/**
  * @brief  Enable update DMA request (UDE).
  * @rmtoll DIER         UDE           LL_TIM_EnableDMAReq_UPDATE
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableDMAReq_UPDATE(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_UDE);
}

/**
  * @brief  Disable update DMA request (UDE).
  * @rmtoll DIER         UDE           LL_TIM_DisableDMAReq_UPDATE
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableDMAReq_UPDATE(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_UDE);
}

/**
  * @brief  Indicates whether the update DMA request  (UDE) is enabled.
  * @rmtoll DIER         UDE           LL_TIM_IsEnabledDMAReq_UPDATE
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledDMAReq_UPDATE(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_UDE) == (TIM_DIER_UDE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable capture/compare 1 DMA request (CC1DE).
  * @rmtoll DIER         CC1DE         LL_TIM_EnableDMAReq_CC1
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableDMAReq_CC1(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_CC1DE);
}

/**
  * @brief  Disable capture/compare 1  DMA request (CC1DE).
  * @rmtoll DIER         CC1DE         LL_TIM_DisableDMAReq_CC1
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableDMAReq_CC1(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_CC1DE);
}

/**
  * @brief  Indicates whether the capture/compare 1 DMA request (CC1DE) is enabled.
  * @rmtoll DIER         CC1DE         LL_TIM_IsEnabledDMAReq_CC1
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledDMAReq_CC1(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_CC1DE) == (TIM_DIER_CC1DE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable capture/compare 2 DMA request (CC2DE).
  * @rmtoll DIER         CC2DE         LL_TIM_EnableDMAReq_CC2
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableDMAReq_CC2(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_CC2DE);
}

/**
  * @brief  Disable capture/compare 2  DMA request (CC2DE).
  * @rmtoll DIER         CC2DE         LL_TIM_DisableDMAReq_CC2
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableDMAReq_CC2(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_CC2DE);
}

/**
  * @brief  Indicates whether the capture/compare 2 DMA request (CC2DE) is enabled.
  * @rmtoll DIER         CC2DE         LL_TIM_IsEnabledDMAReq_CC2
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledDMAReq_CC2(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_CC2DE) == (TIM_DIER_CC2DE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable capture/compare 3 DMA request (CC3DE).
  * @rmtoll DIER         CC3DE         LL_TIM_EnableDMAReq_CC3
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableDMAReq_CC3(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_CC3DE);
}

/**
  * @brief  Disable capture/compare 3  DMA request (CC3DE).
  * @rmtoll DIER         CC3DE         LL_TIM_DisableDMAReq_CC3
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableDMAReq_CC3(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_CC3DE);
}

/**
  * @brief  Indicates whether the capture/compare 3 DMA request (CC3DE) is enabled.
  * @rmtoll DIER         CC3DE         LL_TIM_IsEnabledDMAReq_CC3
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledDMAReq_CC3(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_CC3DE) == (TIM_DIER_CC3DE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable capture/compare 4 DMA request (CC4DE).
  * @rmtoll DIER         CC4DE         LL_TIM_EnableDMAReq_CC4
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableDMAReq_CC4(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_CC4DE);
}

/**
  * @brief  Disable capture/compare 4  DMA request (CC4DE).
  * @rmtoll DIER         CC4DE         LL_TIM_DisableDMAReq_CC4
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableDMAReq_CC4(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_CC4DE);
}

/**
  * @brief  Indicates whether the capture/compare 4 DMA request (CC4DE) is enabled.
  * @rmtoll DIER         CC4DE         LL_TIM_IsEnabledDMAReq_CC4
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledDMAReq_CC4(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_CC4DE) == (TIM_DIER_CC4DE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable commutation DMA request (COMDE).
  * @rmtoll DIER         COMDE         LL_TIM_EnableDMAReq_COM
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableDMAReq_COM(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_COMDE);
}

/**
  * @brief  Disable commutation DMA request (COMDE).
  * @rmtoll DIER         COMDE         LL_TIM_DisableDMAReq_COM
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableDMAReq_COM(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_COMDE);
}

/**
  * @brief  Indicates whether the commutation DMA request (COMDE) is enabled.
  * @rmtoll DIER         COMDE         LL_TIM_IsEnabledDMAReq_COM
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledDMAReq_COM(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_COMDE) == (TIM_DIER_COMDE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable trigger interrupt (TDE).
  * @rmtoll DIER         TDE           LL_TIM_EnableDMAReq_TRIG
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_EnableDMAReq_TRIG(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->DIER, TIM_DIER_TDE);
}

/**
  * @brief  Disable trigger interrupt (TDE).
  * @rmtoll DIER         TDE           LL_TIM_DisableDMAReq_TRIG
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_DisableDMAReq_TRIG(TIM_TypeDef *TIMx)
{
  CLEAR_BIT(TIMx->DIER, TIM_DIER_TDE);
}

/**
  * @brief  Indicates whether the trigger interrupt (TDE) is enabled.
  * @rmtoll DIER         TDE           LL_TIM_IsEnabledDMAReq_TRIG
  * @param  TIMx Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_TIM_IsEnabledDMAReq_TRIG(const TIM_TypeDef *TIMx)
{
  return ((READ_BIT(TIMx->DIER, TIM_DIER_TDE) == (TIM_DIER_TDE)) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup TIM_LL_EF_EVENT_Management EVENT-Management
  * @{
  */
/**
  * @brief  Generate an update event.
  * @rmtoll EGR          UG            LL_TIM_GenerateEvent_UPDATE
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_GenerateEvent_UPDATE(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->EGR, TIM_EGR_UG);
}

/**
  * @brief  Generate Capture/Compare 1 event.
  * @rmtoll EGR          CC1G          LL_TIM_GenerateEvent_CC1
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_GenerateEvent_CC1(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->EGR, TIM_EGR_CC1G);
}

/**
  * @brief  Generate Capture/Compare 2 event.
  * @rmtoll EGR          CC2G          LL_TIM_GenerateEvent_CC2
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_GenerateEvent_CC2(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->EGR, TIM_EGR_CC2G);
}

/**
  * @brief  Generate Capture/Compare 3 event.
  * @rmtoll EGR          CC3G          LL_TIM_GenerateEvent_CC3
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_GenerateEvent_CC3(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->EGR, TIM_EGR_CC3G);
}

/**
  * @brief  Generate Capture/Compare 4 event.
  * @rmtoll EGR          CC4G          LL_TIM_GenerateEvent_CC4
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_GenerateEvent_CC4(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->EGR, TIM_EGR_CC4G);
}

/**
  * @brief  Generate commutation event.
  * @rmtoll EGR          COMG          LL_TIM_GenerateEvent_COM
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_GenerateEvent_COM(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->EGR, TIM_EGR_COMG);
}

/**
  * @brief  Generate trigger event.
  * @rmtoll EGR          TG            LL_TIM_GenerateEvent_TRIG
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_GenerateEvent_TRIG(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->EGR, TIM_EGR_TG);
}

/**
  * @brief  Generate break event.
  * @rmtoll EGR          BG            LL_TIM_GenerateEvent_BRK
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_GenerateEvent_BRK(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->EGR, TIM_EGR_BG);
}

/**
  * @brief  Generate break 2 event.
  * @rmtoll EGR          B2G           LL_TIM_GenerateEvent_BRK2
  * @param  TIMx Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_TIM_GenerateEvent_BRK2(TIM_TypeDef *TIMx)
{
  SET_BIT(TIMx->EGR, TIM_EGR_B2G);
}

/**
  * @}
  */

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup TIM_LL_EF_Init Initialisation and deinitialisation functions
  * @{
  */

ErrorStatus LL_TIM_DeInit(const TIM_TypeDef *TIMx);
void LL_TIM_StructInit(LL_TIM_InitTypeDef *TIM_InitStruct);
ErrorStatus LL_TIM_Init(TIM_TypeDef *TIMx, const LL_TIM_InitTypeDef *TIM_InitStruct);
void LL_TIM_OC_StructInit(LL_TIM_OC_InitTypeDef *TIM_OC_InitStruct);
ErrorStatus LL_TIM_OC_Init(TIM_TypeDef *TIMx, uint32_t Channel, const LL_TIM_OC_InitTypeDef *TIM_OC_InitStruct);
void LL_TIM_IC_StructInit(LL_TIM_IC_InitTypeDef *TIM_ICInitStruct);
ErrorStatus LL_TIM_IC_Init(TIM_TypeDef *TIMx, uint32_t Channel, const LL_TIM_IC_InitTypeDef *TIM_IC_InitStruct);
void LL_TIM_ENCODER_StructInit(LL_TIM_ENCODER_InitTypeDef *TIM_EncoderInitStruct);
ErrorStatus LL_TIM_ENCODER_Init(TIM_TypeDef *TIMx, const LL_TIM_ENCODER_InitTypeDef *TIM_EncoderInitStruct);
void LL_TIM_HALLSENSOR_StructInit(LL_TIM_HALLSENSOR_InitTypeDef *TIM_HallSensorInitStruct);
ErrorStatus LL_TIM_HALLSENSOR_Init(TIM_TypeDef *TIMx, const LL_TIM_HALLSENSOR_InitTypeDef *TIM_HallSensorInitStruct);
void LL_TIM_BDTR_StructInit(LL_TIM_BDTR_InitTypeDef *TIM_BDTRInitStruct);
ErrorStatus LL_TIM_BDTR_Init(TIM_TypeDef *TIMx, const LL_TIM_BDTR_InitTypeDef *TIM_BDTRInitStruct);
/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/**
  * @}
  */

/**
  * @}
  */

#endif /* TIM1 || TIM2 || TIM3 || TIM4 || TIM5 || TIM6 || TIM7 || TIM8 || TIM9 || TIM10 || TIM11 || TIM12 || TIM13 || TIM14 || TIM15 || TIM16 || TIM17 || TIM18 */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32N6xx_LL_TIM_H */
