/**
  ******************************************************************************
  * @file    stm32f4xx_hal_tim.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of TIM HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_HAL_TIM_H
#define __STM32F4xx_HAL_TIM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"

/** @addtogroup STM32F4xx_HAL
  * @{
  */

/** @addtogroup TIM
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** 
  * @brief  TIM Time base Configuration Structure definition  
  */
typedef struct
{
  uint32_t Prescaler;         /*!< Specifies the prescaler value used to divide the TIM clock.
                                   This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF */

  uint32_t CounterMode;       /*!< Specifies the counter mode.
                                   This parameter can be a value of @ref TIM_Counter_Mode */

  uint32_t Period;            /*!< Specifies the period value to be loaded into the active
                                   Auto-Reload Register at the next update event.
                                   This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF.  */

  uint32_t ClockDivision;     /*!< Specifies the clock division.
                                   This parameter can be a value of @ref TIM_ClockDivision */

  uint32_t RepetitionCounter;  /*!< Specifies the repetition counter value. Each time the RCR downcounter
                                    reaches zero, an update event is generated and counting restarts
                                    from the RCR value (N).
                                    This means in PWM mode that (N+1) corresponds to:
                                        - the number of PWM periods in edge-aligned mode
                                        - the number of half PWM period in center-aligned mode
                                     This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. 
                                     @note This parameter is valid only for TIM1 and TIM8. */
} TIM_Base_InitTypeDef;

/** 
  * @brief  TIM Output Compare Configuration Structure definition  
  */

typedef struct
{
  uint32_t OCMode;        /*!< Specifies the TIM mode.
                               This parameter can be a value of @ref TIM_Output_Compare_and_PWM_modes */

  uint32_t Pulse;         /*!< Specifies the pulse value to be loaded into the Capture Compare Register. 
                               This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF */

  uint32_t OCPolarity;    /*!< Specifies the output polarity.
                               This parameter can be a value of @ref TIM_Output_Compare_Polarity */

  uint32_t OCNPolarity;   /*!< Specifies the complementary output polarity.
                               This parameter can be a value of @ref TIM_Output_Compare_N_Polarity
                               @note This parameter is valid only for TIM1 and TIM8. */
  
  uint32_t OCFastMode;   /*!< Specifies the Fast mode state.
                               This parameter can be a value of @ref TIM_Output_Fast_State
                               @note This parameter is valid only in PWM1 and PWM2 mode. */


  uint32_t OCIdleState;   /*!< Specifies the TIM Output Compare pin state during Idle state.
                               This parameter can be a value of @ref TIM_Output_Compare_Idle_State
                               @note This parameter is valid only for TIM1 and TIM8. */

  uint32_t OCNIdleState;  /*!< Specifies the TIM Output Compare pin state during Idle state.
                               This parameter can be a value of @ref TIM_Output_Compare_N_Idle_State
                               @note This parameter is valid only for TIM1 and TIM8. */
} TIM_OC_InitTypeDef;  

/** 
  * @brief  TIM One Pulse Mode Configuration Structure definition  
  */
typedef struct
{
  uint32_t OCMode;        /*!< Specifies the TIM mode.
                               This parameter can be a value of @ref TIM_Output_Compare_and_PWM_modes */

  uint32_t Pulse;         /*!< Specifies the pulse value to be loaded into the Capture Compare Register. 
                               This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF */

  uint32_t OCPolarity;    /*!< Specifies the output polarity.
                               This parameter can be a value of @ref TIM_Output_Compare_Polarity */

  uint32_t OCNPolarity;   /*!< Specifies the complementary output polarity.
                               This parameter can be a value of @ref TIM_Output_Compare_N_Polarity
                               @note This parameter is valid only for TIM1 and TIM8. */

  uint32_t OCIdleState;   /*!< Specifies the TIM Output Compare pin state during Idle state.
                               This parameter can be a value of @ref TIM_Output_Compare_Idle_State
                               @note This parameter is valid only for TIM1 and TIM8. */

  uint32_t OCNIdleState;  /*!< Specifies the TIM Output Compare pin state during Idle state.
                               This parameter can be a value of @ref TIM_Output_Compare_N_Idle_State
                               @note This parameter is valid only for TIM1 and TIM8. */

  uint32_t ICPolarity;    /*!< Specifies the active edge of the input signal.
                               This parameter can be a value of @ref TIM_Input_Capture_Polarity */

  uint32_t ICSelection;   /*!< Specifies the input.
                              This parameter can be a value of @ref TIM_Input_Capture_Selection */

  uint32_t ICFilter;      /*!< Specifies the input capture filter.
                              This parameter can be a number between Min_Data = 0x0 and Max_Data = 0xF */  
} TIM_OnePulse_InitTypeDef;  


/** 
  * @brief  TIM Input Capture Configuration Structure definition  
  */

typedef struct
{
  uint32_t  ICPolarity;   /*!< Specifies the active edge of the input signal.
                               This parameter can be a value of @ref TIM_Input_Capture_Polarity */

  uint32_t ICSelection;  /*!< Specifies the input.
                              This parameter can be a value of @ref TIM_Input_Capture_Selection */

  uint32_t ICPrescaler;  /*!< Specifies the Input Capture Prescaler.
                              This parameter can be a value of @ref TIM_Input_Capture_Prescaler */

  uint32_t ICFilter;     /*!< Specifies the input capture filter.
                              This parameter can be a number between Min_Data = 0x0 and Max_Data = 0xF */
} TIM_IC_InitTypeDef;

/** 
  * @brief  TIM Encoder Configuration Structure definition  
  */

typedef struct
{
  uint32_t EncoderMode;   /*!< Specifies the active edge of the input signal.
                               This parameter can be a value of @ref TIM_Encoder_Mode */
                                  
  uint32_t IC1Polarity;   /*!< Specifies the active edge of the input signal.
                               This parameter can be a value of @ref TIM_Input_Capture_Polarity */

  uint32_t IC1Selection;  /*!< Specifies the input.
                               This parameter can be a value of @ref TIM_Input_Capture_Selection */

  uint32_t IC1Prescaler;  /*!< Specifies the Input Capture Prescaler.
                               This parameter can be a value of @ref TIM_Input_Capture_Prescaler */

  uint32_t IC1Filter;     /*!< Specifies the input capture filter.
                               This parameter can be a number between Min_Data = 0x0 and Max_Data = 0xF */
                                  
  uint32_t IC2Polarity;   /*!< Specifies the active edge of the input signal.
                               This parameter can be a value of @ref TIM_Input_Capture_Polarity */

  uint32_t IC2Selection;  /*!< Specifies the input.
                              This parameter can be a value of @ref TIM_Input_Capture_Selection */

  uint32_t IC2Prescaler;  /*!< Specifies the Input Capture Prescaler.
                               This parameter can be a value of @ref TIM_Input_Capture_Prescaler */

  uint32_t IC2Filter;     /*!< Specifies the input capture filter.
                               This parameter can be a number between Min_Data = 0x0 and Max_Data = 0xF */
} TIM_Encoder_InitTypeDef;

/** 
  * @brief  Clock Configuration Handle Structure definition  
  */ 
typedef struct
{
  uint32_t ClockSource;     /*!< TIM clock sources. 
                                 This parameter can be a value of @ref TIM_Clock_Source */ 
  uint32_t ClockPolarity;   /*!< TIM clock polarity. 
                                 This parameter can be a value of @ref TIM_Clock_Polarity */
  uint32_t ClockPrescaler;  /*!< TIM clock prescaler. 
                                 This parameter can be a value of @ref TIM_Clock_Prescaler */
  uint32_t ClockFilter;    /*!< TIM clock filter. 
                                This parameter can be a number between Min_Data = 0x0 and Max_Data = 0xF */
}TIM_ClockConfigTypeDef;

/** 
  * @brief  Clear Input Configuration Handle Structure definition  
  */ 
typedef struct
{ 
  uint32_t ClearInputState;      /*!< TIM clear Input state. 
                                      This parameter can be ENABLE or DISABLE */  
  uint32_t ClearInputSource;     /*!< TIM clear Input sources. 
                                      This parameter can be a value of @ref TIM_ClearInput_Source */ 
  uint32_t ClearInputPolarity;   /*!< TIM Clear Input polarity. 
                                      This parameter can be a value of @ref TIM_ClearInput_Polarity */
  uint32_t ClearInputPrescaler;  /*!< TIM Clear Input prescaler. 
                                      This parameter can be a value of @ref TIM_ClearInput_Prescaler */
  uint32_t ClearInputFilter;    /*!< TIM Clear Input filter. 
                                     This parameter can be a number between Min_Data = 0x0 and Max_Data = 0xF */
}TIM_ClearInputConfigTypeDef;

/** 
  * @brief  TIM Slave configuration Structure definition  
  */ 
typedef struct {
  uint32_t  SlaveMode;         /*!< Slave mode selection 
                                  This parameter can be a value of @ref TIM_Slave_Mode */ 
  uint32_t  InputTrigger;      /*!< Input Trigger source 
                                  This parameter can be a value of @ref TIM_Trigger_Selection */
  uint32_t  TriggerPolarity;   /*!< Input Trigger polarity 
                                  This parameter can be a value of @ref TIM_Trigger_Polarity */
  uint32_t  TriggerPrescaler;  /*!< Input trigger prescaler 
                                  This parameter can be a value of @ref TIM_Trigger_Prescaler */
  uint32_t  TriggerFilter;     /*!< Input trigger filter 
                                  This parameter can be a number between Min_Data = 0x0 and Max_Data = 0xF */  

}TIM_SlaveConfigTypeDef;

/** 
  * @brief  HAL State structures definition  
  */ 
typedef enum
{
  HAL_TIM_STATE_RESET             = 0x00,    /*!< Peripheral not yet initialized or disabled  */
  HAL_TIM_STATE_READY             = 0x01,    /*!< Peripheral Initialized and ready for use    */
  HAL_TIM_STATE_BUSY              = 0x02,    /*!< An internal process is ongoing              */
  HAL_TIM_STATE_TIMEOUT           = 0x03,    /*!< Timeout state                               */
  HAL_TIM_STATE_ERROR             = 0x04     /*!< Reception process is ongoing                */
}HAL_TIM_StateTypeDef;

/** 
  * @brief  HAL Active channel structures definition  
  */ 
typedef enum
{
  HAL_TIM_ACTIVE_CHANNEL_1        = 0x01,    /*!< The active channel is 1     */
  HAL_TIM_ACTIVE_CHANNEL_2        = 0x02,    /*!< The active channel is 2     */
  HAL_TIM_ACTIVE_CHANNEL_3        = 0x04,    /*!< The active channel is 3     */
  HAL_TIM_ACTIVE_CHANNEL_4        = 0x08,    /*!< The active channel is 4     */
  HAL_TIM_ACTIVE_CHANNEL_CLEARED  = 0x00     /*!< All active channels cleared */
}HAL_TIM_ActiveChannel;

/** 
  * @brief  TIM Time Base Handle Structure definition  
  */ 
typedef struct
{
  TIM_TypeDef                 *Instance;     /*!< Register base address             */
  TIM_Base_InitTypeDef        Init;          /*!< TIM Time Base required parameters */
  HAL_TIM_ActiveChannel       Channel;       /*!< Active channel                    */
  DMA_HandleTypeDef           *hdma[7];      /*!< DMA Handlers array
                                             This array is accessed by a @ref DMA_Handle_index */
  HAL_LockTypeDef             Lock;          /*!< Locking object                    */
  __IO HAL_TIM_StateTypeDef   State;         /*!< TIM operation state               */
}TIM_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/
/** @defgroup TIM_Exported_Constants
  * @{
  */

/** @defgroup TIM_Input_Channel_Polarity 
  * @{
  */
#define  TIM_INPUTCHANNELPOLARITY_RISING      ((uint32_t)0x00000000)            /*!< Polarity for TIx source */
#define  TIM_INPUTCHANNELPOLARITY_FALLING     (TIM_CCER_CC1P)                   /*!< Polarity for TIx source */
#define  TIM_INPUTCHANNELPOLARITY_BOTHEDGE    (TIM_CCER_CC1P | TIM_CCER_CC1NP)  /*!< Polarity for TIx source */
/**
  * @}
  */

/** @defgroup TIM_ETR_Polarity 
  * @{
  */
#define TIM_ETRPOLARITY_INVERTED              (TIM_SMCR_ETP)                    /*!< Polarity for ETR source */
#define TIM_ETRPOLARITY_NONINVERTED           ((uint32_t)0x0000)                /*!< Polarity for ETR source */
/**
  * @}
  */

/** @defgroup TIM_ETR_Prescaler 
  * @{
  */
#define TIM_ETRPRESCALER_DIV1                 ((uint32_t)0x0000)                /*!< No prescaler is used */
#define TIM_ETRPRESCALER_DIV2                 (TIM_SMCR_ETPS_0)                 /*!< ETR input source is divided by 2 */
#define TIM_ETRPRESCALER_DIV4                 (TIM_SMCR_ETPS_1)                 /*!< ETR input source is divided by 4 */
#define TIM_ETRPRESCALER_DIV8                 (TIM_SMCR_ETPS)                   /*!< ETR input source is divided by 8 */
/**
  * @}
  */

/** @defgroup TIM_Counter_Mode 
  * @{
  */
#define TIM_COUNTERMODE_UP                 ((uint32_t)0x0000)
#define TIM_COUNTERMODE_DOWN               TIM_CR1_DIR
#define TIM_COUNTERMODE_CENTERALIGNED1     TIM_CR1_CMS_0
#define TIM_COUNTERMODE_CENTERALIGNED2     TIM_CR1_CMS_1
#define TIM_COUNTERMODE_CENTERALIGNED3     TIM_CR1_CMS

#define IS_TIM_COUNTER_MODE(MODE) (((MODE) == TIM_COUNTERMODE_UP)              || \
                                   ((MODE) == TIM_COUNTERMODE_DOWN)            || \
                                   ((MODE) == TIM_COUNTERMODE_CENTERALIGNED1)  || \
                                   ((MODE) == TIM_COUNTERMODE_CENTERALIGNED2)  || \
                                   ((MODE) == TIM_COUNTERMODE_CENTERALIGNED3))
/**
  * @}
  */

/** @defgroup TIM_ClockDivision 
  * @{
  */
#define TIM_CLOCKDIVISION_DIV1                       ((uint32_t)0x0000)
#define TIM_CLOCKDIVISION_DIV2                       (TIM_CR1_CKD_0)
#define TIM_CLOCKDIVISION_DIV4                       (TIM_CR1_CKD_1)

#define IS_TIM_CLOCKDIVISION_DIV(DIV) (((DIV) == TIM_CLOCKDIVISION_DIV1) || \
                                       ((DIV) == TIM_CLOCKDIVISION_DIV2) || \
                                       ((DIV) == TIM_CLOCKDIVISION_DIV4))
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_and_PWM_modes 
  * @{
  */
#define TIM_OCMODE_TIMING                   ((uint32_t)0x0000)
#define TIM_OCMODE_ACTIVE                   (TIM_CCMR1_OC1M_0)
#define TIM_OCMODE_INACTIVE                 (TIM_CCMR1_OC1M_1)
#define TIM_OCMODE_TOGGLE                   (TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1)
#define TIM_OCMODE_PWM1                     (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2)
#define TIM_OCMODE_PWM2                     (TIM_CCMR1_OC1M)
#define TIM_OCMODE_FORCED_ACTIVE            (TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_2)
#define TIM_OCMODE_FORCED_INACTIVE          (TIM_CCMR1_OC1M_2)

#define IS_TIM_PWM_MODE(MODE) (((MODE) == TIM_OCMODE_PWM1) || \
                               ((MODE) == TIM_OCMODE_PWM2))
                              
#define IS_TIM_OC_MODE(MODE) (((MODE) == TIM_OCMODE_TIMING)       || \
                          ((MODE) == TIM_OCMODE_ACTIVE)           || \
                          ((MODE) == TIM_OCMODE_INACTIVE)         || \
                          ((MODE) == TIM_OCMODE_TOGGLE)           || \
                          ((MODE) == TIM_OCMODE_FORCED_ACTIVE)    || \
                          ((MODE) == TIM_OCMODE_FORCED_INACTIVE))
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_State 
  * @{
  */
#define TIM_OUTPUTSTATE_DISABLE            ((uint32_t)0x0000)
#define TIM_OUTPUTSTATE_ENABLE             (TIM_CCER_CC1E)

#define IS_TIM_OUTPUT_STATE(STATE) (((STATE) == TIM_OUTPUTSTATE_DISABLE) || \
                                    ((STATE) == TIM_OUTPUTSTATE_ENABLE))
/**
  * @}
  */

/** @defgroup TIM_Output_Fast_State 
  * @{
  */
#define TIM_OCFAST_DISABLE                ((uint32_t)0x0000)
#define TIM_OCFAST_ENABLE                 (TIM_CCMR1_OC1FE)

#define IS_TIM_FAST_STATE(STATE) (((STATE) == TIM_OCFAST_DISABLE) || \
                                  ((STATE) == TIM_OCFAST_ENABLE))
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_N_State
  * @{
  */
#define TIM_OUTPUTNSTATE_DISABLE            ((uint32_t)0x0000)
#define TIM_OUTPUTNSTATE_ENABLE             (TIM_CCER_CC1NE)

#define IS_TIM_OUTPUTN_STATE(STATE) (((STATE) == TIM_OUTPUTNSTATE_DISABLE) || \
                                     ((STATE) == TIM_OUTPUTNSTATE_ENABLE))
/**
  * @}
  */ 

/** @defgroup TIM_Output_Compare_Polarity 
  * @{
  */
#define TIM_OCPOLARITY_HIGH                ((uint32_t)0x0000)
#define TIM_OCPOLARITY_LOW                 (TIM_CCER_CC1P)

#define IS_TIM_OC_POLARITY(POLARITY) (((POLARITY) == TIM_OCPOLARITY_HIGH) || \
                                      ((POLARITY) == TIM_OCPOLARITY_LOW))
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_N_Polarity 
  * @{
  */
#define TIM_OCNPOLARITY_HIGH               ((uint32_t)0x0000)
#define TIM_OCNPOLARITY_LOW                (TIM_CCER_CC1NP)

#define IS_TIM_OCN_POLARITY(POLARITY) (((POLARITY) == TIM_OCNPOLARITY_HIGH) || \
                                       ((POLARITY) == TIM_OCNPOLARITY_LOW))
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_Idle_State 
  * @{
  */
#define TIM_OCIDLESTATE_SET                (TIM_CR2_OIS1)
#define TIM_OCIDLESTATE_RESET              ((uint32_t)0x0000)
#define IS_TIM_OCIDLE_STATE(STATE) (((STATE) == TIM_OCIDLESTATE_SET) || \
                                    ((STATE) == TIM_OCIDLESTATE_RESET))
/**
  * @}
  */ 

/** @defgroup TIM_Output_Compare_N_Idle_State 
  * @{
  */
#define TIM_OCNIDLESTATE_SET               (TIM_CR2_OIS1N)
#define TIM_OCNIDLESTATE_RESET             ((uint32_t)0x0000)
#define IS_TIM_OCNIDLE_STATE(STATE) (((STATE) == TIM_OCNIDLESTATE_SET) || \
                                     ((STATE) == TIM_OCNIDLESTATE_RESET))
/**
  * @}
  */ 

/** @defgroup TIM_Channel 
  * @{
  */
#define TIM_CHANNEL_1                      ((uint32_t)0x0000)
#define TIM_CHANNEL_2                      ((uint32_t)0x0004)
#define TIM_CHANNEL_3                      ((uint32_t)0x0008)
#define TIM_CHANNEL_4                      ((uint32_t)0x000C)
#define TIM_CHANNEL_ALL                    ((uint32_t)0x0018)
                                 
#define IS_TIM_CHANNELS(CHANNEL) (((CHANNEL) == TIM_CHANNEL_1) || \
                                  ((CHANNEL) == TIM_CHANNEL_2) || \
                                  ((CHANNEL) == TIM_CHANNEL_3) || \
                                  ((CHANNEL) == TIM_CHANNEL_4) || \
                                  ((CHANNEL) == TIM_CHANNEL_ALL))
                                 
#define IS_TIM_PWMI_CHANNELS(CHANNEL) (((CHANNEL) == TIM_CHANNEL_1) || \
                                       ((CHANNEL) == TIM_CHANNEL_2))

#define IS_TIM_OPM_CHANNELS(CHANNEL) (((CHANNEL) == TIM_CHANNEL_1) || \
                                      ((CHANNEL) == TIM_CHANNEL_2))
                                      
#define IS_TIM_COMPLEMENTARY_CHANNELS(CHANNEL) (((CHANNEL) == TIM_CHANNEL_1) || \
                                                ((CHANNEL) == TIM_CHANNEL_2) || \
                                                ((CHANNEL) == TIM_CHANNEL_3))
/**
  * @}
  */

/** @defgroup TIM_Input_Capture_Polarity 
  * @{
  */
#define  TIM_ICPOLARITY_RISING             TIM_INPUTCHANNELPOLARITY_RISING
#define  TIM_ICPOLARITY_FALLING            TIM_INPUTCHANNELPOLARITY_FALLING
#define  TIM_ICPOLARITY_BOTHEDGE           TIM_INPUTCHANNELPOLARITY_BOTHEDGE

#define IS_TIM_IC_POLARITY(POLARITY) (((POLARITY) == TIM_ICPOLARITY_RISING)   || \
                                      ((POLARITY) == TIM_ICPOLARITY_FALLING)  || \
                                      ((POLARITY) == TIM_ICPOLARITY_BOTHEDGE))
/**
  * @}
  */

/** @defgroup TIM_Input_Capture_Selection 
  * @{
  */
#define TIM_ICSELECTION_DIRECTTI           (TIM_CCMR1_CC1S_0)   /*!< TIM Input 1, 2, 3 or 4 is selected to be 
                                                                     connected to IC1, IC2, IC3 or IC4, respectively */
#define TIM_ICSELECTION_INDIRECTTI         (TIM_CCMR1_CC1S_1)   /*!< TIM Input 1, 2, 3 or 4 is selected to be
                                                                     connected to IC2, IC1, IC4 or IC3, respectively */
#define TIM_ICSELECTION_TRC                (TIM_CCMR1_CC1S)     /*!< TIM Input 1, 2, 3 or 4 is selected to be connected to TRC */

#define IS_TIM_IC_SELECTION(SELECTION) (((SELECTION) == TIM_ICSELECTION_DIRECTTI) || \
                                        ((SELECTION) == TIM_ICSELECTION_INDIRECTTI) || \
                                        ((SELECTION) == TIM_ICSELECTION_TRC))
/**
  * @}
  */

/** @defgroup TIM_Input_Capture_Prescaler 
  * @{
  */
#define TIM_ICPSC_DIV1                     ((uint32_t)0x0000)       /*!< Capture performed each time an edge is detected on the capture input */
#define TIM_ICPSC_DIV2                     (TIM_CCMR1_IC1PSC_0)     /*!< Capture performed once every 2 events */
#define TIM_ICPSC_DIV4                     (TIM_CCMR1_IC1PSC_1)     /*!< Capture performed once every 4 events */
#define TIM_ICPSC_DIV8                     (TIM_CCMR1_IC1PSC)       /*!< Capture performed once every 8 events */

#define IS_TIM_IC_PRESCALER(PRESCALER) (((PRESCALER) == TIM_ICPSC_DIV1) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV2) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV4) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV8))
/**
  * @}
  */ 

/** @defgroup TIM_One_Pulse_Mode 
  * @{
  */
#define TIM_OPMODE_SINGLE                  (TIM_CR1_OPM)
#define TIM_OPMODE_REPETITIVE              ((uint32_t)0x0000)
#define IS_TIM_OPM_MODE(MODE) (((MODE) == TIM_OPMODE_SINGLE) || \
                               ((MODE) == TIM_OPMODE_REPETITIVE))
/**
  * @}
  */

/** @defgroup TIM_Encoder_Mode 
  * @{
  */
#define TIM_ENCODERMODE_TI1                (TIM_SMCR_SMS_0)
#define TIM_ENCODERMODE_TI2                (TIM_SMCR_SMS_1)
#define TIM_ENCODERMODE_TI12               (TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0)
#define IS_TIM_ENCODER_MODE(MODE) (((MODE) == TIM_ENCODERMODE_TI1) || \
                                   ((MODE) == TIM_ENCODERMODE_TI2) || \
                                   ((MODE) == TIM_ENCODERMODE_TI12))   
/**
  * @}
  */

/** @defgroup TIM_Interrupt_definition 
  * @{
  */ 
#define TIM_IT_UPDATE           (TIM_DIER_UIE)
#define TIM_IT_CC1              (TIM_DIER_CC1IE)
#define TIM_IT_CC2              (TIM_DIER_CC2IE)
#define TIM_IT_CC3              (TIM_DIER_CC3IE)
#define TIM_IT_CC4              (TIM_DIER_CC4IE)
#define TIM_IT_COM              (TIM_DIER_COMIE)
#define TIM_IT_TRIGGER          (TIM_DIER_TIE)
#define TIM_IT_BREAK            (TIM_DIER_BIE)

#define IS_TIM_IT(IT) ((((IT) & 0xFFFFFF00) == 0x00000000) && ((IT) != 0x00000000))

#define IS_TIM_GET_IT(IT) (((IT) == TIM_IT_UPDATE)  || \
                           ((IT) == TIM_IT_CC1)     || \
                           ((IT) == TIM_IT_CC2)     || \
                           ((IT) == TIM_IT_CC3)     || \
                           ((IT) == TIM_IT_CC4)     || \
                           ((IT) == TIM_IT_COM)     || \
                           ((IT) == TIM_IT_TRIGGER) || \
                           ((IT) == TIM_IT_BREAK))
/**
  * @}
  */
#define TIM_COMMUTATION_TRGI              (TIM_CR2_CCUS)
#define TIM_COMMUTATION_SOFTWARE          ((uint32_t)0x0000)

/** @defgroup TIM_DMA_sources 
  * @{
  */
#define TIM_DMA_UPDATE                     (TIM_DIER_UDE)
#define TIM_DMA_CC1                        (TIM_DIER_CC1DE)
#define TIM_DMA_CC2                        (TIM_DIER_CC2DE)
#define TIM_DMA_CC3                        (TIM_DIER_CC3DE)
#define TIM_DMA_CC4                        (TIM_DIER_CC4DE)
#define TIM_DMA_COM                        (TIM_DIER_COMDE)
#define TIM_DMA_TRIGGER                    (TIM_DIER_TDE)
#define IS_TIM_DMA_SOURCE(SOURCE) ((((SOURCE) & 0xFFFF80FF) == 0x00000000) && ((SOURCE) != 0x00000000))
/**
  * @}
  */

/** @defgroup TIM_Event_Source 
  * @{
  */
#define TIM_EventSource_Update              TIM_EGR_UG  
#define TIM_EventSource_CC1                 TIM_EGR_CC1G
#define TIM_EventSource_CC2                 TIM_EGR_CC2G
#define TIM_EventSource_CC3                 TIM_EGR_CC3G
#define TIM_EventSource_CC4                 TIM_EGR_CC4G
#define TIM_EventSource_COM                 TIM_EGR_COMG
#define TIM_EventSource_Trigger             TIM_EGR_TG  
#define TIM_EventSource_Break               TIM_EGR_BG  
#define IS_TIM_EVENT_SOURCE(SOURCE) ((((SOURCE) & 0xFFFFFF00) == 0x00000000) && ((SOURCE) != 0x00000000))
/**
  * @}
  */

/** @defgroup TIM_Flag_definition 
  * @{
  */
#define TIM_FLAG_UPDATE                    (TIM_SR_UIF)
#define TIM_FLAG_CC1                       (TIM_SR_CC1IF)
#define TIM_FLAG_CC2                       (TIM_SR_CC2IF)
#define TIM_FLAG_CC3                       (TIM_SR_CC3IF)
#define TIM_FLAG_CC4                       (TIM_SR_CC4IF)
#define TIM_FLAG_COM                       (TIM_SR_COMIF)
#define TIM_FLAG_TRIGGER                   (TIM_SR_TIF)
#define TIM_FLAG_BREAK                     (TIM_SR_BIF)
#define TIM_FLAG_CC1OF                     (TIM_SR_CC1OF)
#define TIM_FLAG_CC2OF                     (TIM_SR_CC2OF)
#define TIM_FLAG_CC3OF                     (TIM_SR_CC3OF)
#define TIM_FLAG_CC4OF                     (TIM_SR_CC4OF)

#define IS_TIM_FLAG(FLAG) (((FLAG) == TIM_FLAG_UPDATE) || \
                           ((FLAG) == TIM_FLAG_CC1)     || \
                           ((FLAG) == TIM_FLAG_CC2)     || \
                           ((FLAG) == TIM_FLAG_CC3)     || \
                           ((FLAG) == TIM_FLAG_CC4)     || \
                           ((FLAG) == TIM_FLAG_COM)     || \
                           ((FLAG) == TIM_FLAG_TRIGGER) || \
                           ((FLAG) == TIM_FLAG_BREAK)   || \
                           ((FLAG) == TIM_FLAG_CC1OF)   || \
                           ((FLAG) == TIM_FLAG_CC2OF)   || \
                           ((FLAG) == TIM_FLAG_CC3OF)   || \
                           ((FLAG) == TIM_FLAG_CC4OF))
/**
  * @}
  */

/** @defgroup TIM_Clock_Source 
  * @{
  */
#define	TIM_CLOCKSOURCE_ETRMODE2    (TIM_SMCR_ETPS_1) 
#define	TIM_CLOCKSOURCE_INTERNAL    (TIM_SMCR_ETPS_0) 
#define	TIM_CLOCKSOURCE_ITR0        ((uint32_t)0x0000)
#define	TIM_CLOCKSOURCE_ITR1        (TIM_SMCR_TS_0)
#define	TIM_CLOCKSOURCE_ITR2        (TIM_SMCR_TS_1)
#define	TIM_CLOCKSOURCE_ITR3        (TIM_SMCR_TS_0 | TIM_SMCR_TS_1)
#define	TIM_CLOCKSOURCE_TI1ED       (TIM_SMCR_TS_2)
#define	TIM_CLOCKSOURCE_TI1         (TIM_SMCR_TS_0 | TIM_SMCR_TS_2)
#define	TIM_CLOCKSOURCE_TI2         (TIM_SMCR_TS_1 | TIM_SMCR_TS_2)
#define	TIM_CLOCKSOURCE_ETRMODE1    (TIM_SMCR_TS)

#define IS_TIM_CLOCKSOURCE(CLOCK) (((CLOCK) == TIM_CLOCKSOURCE_INTERNAL) || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_ETRMODE2) || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_ITR0)     || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_ITR1)     || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_ITR2)     || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_ITR3)     || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_TI1ED)    || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_TI1)      || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_TI2)      || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_ETRMODE1))
/**
  * @}
  */

/** @defgroup TIM_Clock_Polarity 
  * @{
  */
#define TIM_CLOCKPOLARITY_INVERTED           TIM_ETRPOLARITY_INVERTED          /*!< Polarity for ETRx clock sources */ 
#define TIM_CLOCKPOLARITY_NONINVERTED        TIM_ETRPOLARITY_NONINVERTED       /*!< Polarity for ETRx clock sources */ 
#define TIM_CLOCKPOLARITY_RISING             TIM_INPUTCHANNELPOLARITY_RISING   /*!< Polarity for TIx clock sources */ 
#define TIM_CLOCKPOLARITY_FALLING            TIM_INPUTCHANNELPOLARITY_FALLING   /*!< Polarity for TIx clock sources */ 
#define TIM_CLOCKPOLARITY_BOTHEDGE           TIM_INPUTCHANNELPOLARITY_BOTHEDGE  /*!< Polarity for TIx clock sources */ 

#define IS_TIM_CLOCKPOLARITY(POLARITY) (((POLARITY) == TIM_CLOCKPOLARITY_INVERTED)    || \
                                        ((POLARITY) == TIM_CLOCKPOLARITY_NONINVERTED) || \
                                        ((POLARITY) == TIM_CLOCKPOLARITY_RISING)      || \
                                        ((POLARITY) == TIM_CLOCKPOLARITY_FALLING)     || \
                                        ((POLARITY) == TIM_CLOCKPOLARITY_BOTHEDGE))
/**
  * @}
  */

/** @defgroup TIM_Clock_Prescaler 
  * @{
  */
#define TIM_CLOCKPRESCALER_DIV1                 TIM_ETRPRESCALER_DIV1     /*!< No prescaler is used */
#define TIM_CLOCKPRESCALER_DIV2                 TIM_ETRPRESCALER_DIV2     /*!< Prescaler for External ETR Clock: Capture performed once every 2 events. */
#define TIM_CLOCKPRESCALER_DIV4                 TIM_ETRPRESCALER_DIV4     /*!< Prescaler for External ETR Clock: Capture performed once every 4 events. */
#define TIM_CLOCKPRESCALER_DIV8                 TIM_ETRPRESCALER_DIV8     /*!< Prescaler for External ETR Clock: Capture performed once every 8 events. */

#define IS_TIM_CLOCKPRESCALER(PRESCALER) (((PRESCALER) == TIM_CLOCKPRESCALER_DIV1) || \
                                          ((PRESCALER) == TIM_CLOCKPRESCALER_DIV2) || \
                                          ((PRESCALER) == TIM_CLOCKPRESCALER_DIV4) || \
                                          ((PRESCALER) == TIM_CLOCKPRESCALER_DIV8)) 
/**
  * @}
  */

/** @defgroup TIM_Clock_Filter
  * @{
  */
#define IS_TIM_CLOCKFILTER(ICFILTER)      ((ICFILTER) <= 0xF) 
/**
  * @}
  */

/** @defgroup TIM_ClearInput_Source
  * @{
  */
#define TIM_CLEARINPUTSOURCE_ETR           ((uint32_t)0x0001) 
#define TIM_CLEARINPUTSOURCE_NONE          ((uint32_t)0x0000)

#define IS_TIM_CLEARINPUT_SOURCE(SOURCE)  (((SOURCE) == TIM_CLEARINPUTSOURCE_NONE) || \
                                         ((SOURCE) == TIM_CLEARINPUTSOURCE_ETR)) 
/**
  * @}
  */

/** @defgroup TIM_ClearInput_Polarity
  * @{
  */
#define TIM_CLEARINPUTPOLARITY_INVERTED           TIM_ETRPOLARITY_INVERTED                    /*!< Polarity for ETRx pin */ 
#define TIM_CLEARINPUTPOLARITY_NONINVERTED        TIM_ETRPOLARITY_NONINVERTED                 /*!< Polarity for ETRx pin */ 
#define IS_TIM_CLEARINPUT_POLARITY(POLARITY)   (((POLARITY) == TIM_CLEARINPUTPOLARITY_INVERTED) || \
                                               ((POLARITY) == TIM_CLEARINPUTPOLARITY_NONINVERTED))
/**
  * @}
  */

/** @defgroup TIM_ClearInput_Prescaler
  * @{
  */
#define TIM_CLEARINPUTPRESCALER_DIV1                    TIM_ETRPRESCALER_DIV1      /*!< No prescaler is used */
#define TIM_CLEARINPUTPRESCALER_DIV2                    TIM_ETRPRESCALER_DIV2      /*!< Prescaler for External ETR pin: Capture performed once every 2 events. */
#define TIM_CLEARINPUTPRESCALER_DIV4                    TIM_ETRPRESCALER_DIV4      /*!< Prescaler for External ETR pin: Capture performed once every 4 events. */
#define TIM_CLEARINPUTPRESCALER_DIV8                    TIM_ETRPRESCALER_DIV8        /*!< Prescaler for External ETR pin: Capture performed once every 8 events. */
#define IS_TIM_CLEARINPUT_PRESCALER(PRESCALER)   (((PRESCALER) == TIM_CLEARINPUTPRESCALER_DIV1) || \
                                                 ((PRESCALER) == TIM_CLEARINPUTPRESCALER_DIV2) || \
                                                 ((PRESCALER) == TIM_CLEARINPUTPRESCALER_DIV4) || \
                                                 ((PRESCALER) == TIM_CLEARINPUTPRESCALER_DIV8))
/**
  * @}
  */

/** @defgroup TIM_ClearInput_Filter
  * @{
  */
#define IS_TIM_CLEARINPUT_FILTER(ICFILTER) ((ICFILTER) <= 0xF) 
/**
  * @}
  */

/** @defgroup TIM_OSSR_Off_State_Selection_for_Run_mode_state
  * @{
  */  
#define TIM_OSSR_ENABLE 	      (TIM_BDTR_OSSR)
#define TIM_OSSR_DISABLE              ((uint32_t)0x0000)

#define IS_TIM_OSSR_STATE(STATE) (((STATE) == TIM_OSSR_ENABLE) || \
                                  ((STATE) == TIM_OSSR_DISABLE))
/**
  * @}
  */
  
/** @defgroup TIM_OSSI_Off_State_Selection_for_Idle_mode_state 
  * @{
  */
#define TIM_OSSI_ENABLE	 	    (TIM_BDTR_OSSI)
#define TIM_OSSI_DISABLE            ((uint32_t)0x0000)

#define IS_TIM_OSSI_STATE(STATE) (((STATE) == TIM_OSSI_ENABLE) || \
                                  ((STATE) == TIM_OSSI_DISABLE))
/**
  * @}
  */
/** @defgroup TIM_Lock_level 
  * @{
  */
#define TIM_LOCKLEVEL_OFF	   ((uint32_t)0x0000)
#define TIM_LOCKLEVEL_1            (TIM_BDTR_LOCK_0)
#define TIM_LOCKLEVEL_2            (TIM_BDTR_LOCK_1)
#define TIM_LOCKLEVEL_3            (TIM_BDTR_LOCK)

#define IS_TIM_LOCK_LEVEL(LEVEL) (((LEVEL) == TIM_LOCKLEVEL_OFF) || \
                                  ((LEVEL) == TIM_LOCKLEVEL_1) || \
                                  ((LEVEL) == TIM_LOCKLEVEL_2) || \
                                  ((LEVEL) == TIM_LOCKLEVEL_3)) 
/**
  * @}
  */  
/** @defgroup TIM_Break_Input_enable_disable 
  * @{
  */                         
#define TIM_BREAK_ENABLE          (TIM_BDTR_BKE)
#define TIM_BREAK_DISABLE         ((uint32_t)0x0000)

#define IS_TIM_BREAK_STATE(STATE) (((STATE) == TIM_BREAK_ENABLE) || \
                                   ((STATE) == TIM_BREAK_DISABLE))
/**
  * @}
  */
/** @defgroup TIM_Break_Polarity 
  * @{
  */
#define TIM_BREAKPOLARITY_LOW        ((uint32_t)0x0000)
#define TIM_BREAKPOLARITY_HIGH       (TIM_BDTR_BKP)

#define IS_TIM_BREAK_POLARITY(POLARITY) (((POLARITY) == TIM_BREAKPOLARITY_LOW) || \
                                         ((POLARITY) == TIM_BREAKPOLARITY_HIGH))
/**
  * @}
  */
/** @defgroup TIM_AOE_Bit_Set_Reset 
  * @{
  */
#define TIM_AUTOMATICOUTPUT_ENABLE           (TIM_BDTR_AOE)
#define	TIM_AUTOMATICOUTPUT_DISABLE          ((uint32_t)0x0000)

#define IS_TIM_AUTOMATIC_OUTPUT_STATE(STATE) (((STATE) == TIM_AUTOMATICOUTPUT_ENABLE) || \
                                              ((STATE) == TIM_AUTOMATICOUTPUT_DISABLE))
/**
  * @}
  */  
  
/** @defgroup TIM_Master_Mode_Selection
  * @{
  */  
#define	TIM_TRGO_RESET            ((uint32_t)0x0000)             
#define	TIM_TRGO_ENABLE           (TIM_CR2_MMS_0)           
#define	TIM_TRGO_UPDATE           (TIM_CR2_MMS_1)             
#define	TIM_TRGO_OC1              ((TIM_CR2_MMS_1 | TIM_CR2_MMS_0))    
#define	TIM_TRGO_OC1REF           (TIM_CR2_MMS_2)           
#define	TIM_TRGO_OC2REF           ((TIM_CR2_MMS_2 | TIM_CR2_MMS_0))          
#define	TIM_TRGO_OC3REF           ((TIM_CR2_MMS_2 | TIM_CR2_MMS_1))           
#define	TIM_TRGO_OC4REF           ((TIM_CR2_MMS_2 | TIM_CR2_MMS_1 | TIM_CR2_MMS_0))   

#define IS_TIM_TRGO_SOURCE(SOURCE) (((SOURCE) == TIM_TRGO_RESET) || \
                                    ((SOURCE) == TIM_TRGO_ENABLE) || \
                                    ((SOURCE) == TIM_TRGO_UPDATE) || \
                                    ((SOURCE) == TIM_TRGO_OC1) || \
                                    ((SOURCE) == TIM_TRGO_OC1REF) || \
                                    ((SOURCE) == TIM_TRGO_OC2REF) || \
                                    ((SOURCE) == TIM_TRGO_OC3REF) || \
                                    ((SOURCE) == TIM_TRGO_OC4REF))
      

/**
  * @}
  */ 
/** @defgroup TIM_Slave_Mode 
  * @{
  */
#define TIM_SLAVEMODE_DISABLE              ((uint32_t)0x0000)
#define TIM_SLAVEMODE_RESET                ((uint32_t)0x0004)
#define TIM_SLAVEMODE_GATED                ((uint32_t)0x0005)
#define TIM_SLAVEMODE_TRIGGER              ((uint32_t)0x0006)
#define TIM_SLAVEMODE_EXTERNAL1            ((uint32_t)0x0007)

#define IS_TIM_SLAVE_MODE(MODE) (((MODE) == TIM_SLAVEMODE_DISABLE) || \
                                 ((MODE) == TIM_SLAVEMODE_GATED) || \
                                 ((MODE) == TIM_SLAVEMODE_RESET) || \
                                 ((MODE) == TIM_SLAVEMODE_TRIGGER) || \
                                 ((MODE) == TIM_SLAVEMODE_EXTERNAL1))
/**
  * @}
  */

/** @defgroup TIM_Master_Slave_Mode 
  * @{
  */

#define TIM_MASTERSLAVEMODE_ENABLE          ((uint32_t)0x0080)
#define TIM_MASTERSLAVEMODE_DISABLE         ((uint32_t)0x0000)
#define IS_TIM_MSM_STATE(STATE) (((STATE) == TIM_MASTERSLAVEMODE_ENABLE) || \
                                 ((STATE) == TIM_MASTERSLAVEMODE_DISABLE))
/**
  * @}
  */ 
/** @defgroup TIM_Trigger_Selection 
  * @{
  */
#define TIM_TS_ITR0                        ((uint32_t)0x0000)
#define TIM_TS_ITR1                        ((uint32_t)0x0010)
#define TIM_TS_ITR2                        ((uint32_t)0x0020)
#define TIM_TS_ITR3                        ((uint32_t)0x0030)
#define TIM_TS_TI1F_ED                     ((uint32_t)0x0040)
#define TIM_TS_TI1FP1                      ((uint32_t)0x0050)
#define TIM_TS_TI2FP2                      ((uint32_t)0x0060)
#define TIM_TS_ETRF                        ((uint32_t)0x0070)
#define TIM_TS_NONE                        ((uint32_t)0xFFFF)
#define IS_TIM_TRIGGER_SELECTION(SELECTION) (((SELECTION) == TIM_TS_ITR0) || \
                                             ((SELECTION) == TIM_TS_ITR1) || \
                                             ((SELECTION) == TIM_TS_ITR2) || \
                                             ((SELECTION) == TIM_TS_ITR3) || \
                                             ((SELECTION) == TIM_TS_TI1F_ED) || \
                                             ((SELECTION) == TIM_TS_TI1FP1) || \
                                             ((SELECTION) == TIM_TS_TI2FP2) || \
                                             ((SELECTION) == TIM_TS_ETRF))
#define IS_TIM_INTERNAL_TRIGGER_SELECTION(SELECTION) (((SELECTION) == TIM_TS_ITR0) || \
                                                      ((SELECTION) == TIM_TS_ITR1) || \
                                                      ((SELECTION) == TIM_TS_ITR2) || \
                                                      ((SELECTION) == TIM_TS_ITR3))
#define IS_TIM_INTERNAL_TRIGGEREVENT_SELECTION(SELECTION) (((SELECTION) == TIM_TS_ITR0) || \
                                                           ((SELECTION) == TIM_TS_ITR1) || \
                                                           ((SELECTION) == TIM_TS_ITR2) || \
                                                           ((SELECTION) == TIM_TS_ITR3) || \
                                                           ((SELECTION) == TIM_TS_NONE))
/**
  * @}
  */  

/** @defgroup TIM_Trigger_Polarity 
  * @{
  */
#define TIM_TRIGGERPOLARITY_INVERTED           TIM_ETRPOLARITY_INVERTED      /*!< Polarity for ETRx trigger sources */ 
#define TIM_TRIGGERPOLARITY_NONINVERTED        TIM_ETRPOLARITY_NONINVERTED   /*!< Polarity for ETRx trigger sources */ 
#define TIM_TRIGGERPOLARITY_RISING             TIM_INPUTCHANNELPOLARITY_RISING        /*!< Polarity for TIxFPx or TI1_ED trigger sources */ 
#define TIM_TRIGGERPOLARITY_FALLING            TIM_INPUTCHANNELPOLARITY_FALLING       /*!< Polarity for TIxFPx or TI1_ED trigger sources */ 
#define TIM_TRIGGERPOLARITY_BOTHEDGE           TIM_INPUTCHANNELPOLARITY_BOTHEDGE      /*!< Polarity for TIxFPx or TI1_ED trigger sources */ 

#define IS_TIM_TRIGGERPOLARITY(POLARITY)     (((POLARITY) == TIM_TRIGGERPOLARITY_INVERTED   ) || \
                                              ((POLARITY) == TIM_TRIGGERPOLARITY_NONINVERTED) || \
                                              ((POLARITY) == TIM_TRIGGERPOLARITY_RISING     ) || \
                                              ((POLARITY) == TIM_TRIGGERPOLARITY_FALLING    ) || \
                                              ((POLARITY) == TIM_TRIGGERPOLARITY_BOTHEDGE   ))
/**
  * @}
  */

/** @defgroup TIM_Trigger_Prescaler 
  * @{
  */
#define TIM_TRIGGERPRESCALER_DIV1             TIM_ETRPRESCALER_DIV1     /*!< No prescaler is used */
#define TIM_TRIGGERPRESCALER_DIV2             TIM_ETRPRESCALER_DIV2     /*!< Prescaler for External ETR Trigger: Capture performed once every 2 events. */
#define TIM_TRIGGERPRESCALER_DIV4             TIM_ETRPRESCALER_DIV4     /*!< Prescaler for External ETR Trigger: Capture performed once every 4 events. */
#define TIM_TRIGGERPRESCALER_DIV8             TIM_ETRPRESCALER_DIV8     /*!< Prescaler for External ETR Trigger: Capture performed once every 8 events. */

#define IS_TIM_TRIGGERPRESCALER(PRESCALER)  (((PRESCALER) == TIM_TRIGGERPRESCALER_DIV1) || \
                                             ((PRESCALER) == TIM_TRIGGERPRESCALER_DIV2) || \
                                             ((PRESCALER) == TIM_TRIGGERPRESCALER_DIV4) || \
                                             ((PRESCALER) == TIM_TRIGGERPRESCALER_DIV8)) 
/**
  * @}
  */

/** @defgroup TIM_Trigger_Filter
  * @{
  */
#define IS_TIM_TRIGGERFILTER(ICFILTER)     ((ICFILTER) <= 0xF) 
/**
  * @}
  */

  /** @defgroup TIM_TI1_Selection
  * @{
  */
#define TIM_TI1SELECTION_CH1                ((uint32_t)0x0000)
#define TIM_TI1SELECTION_XORCOMBINATION     (TIM_CR2_TI1S)

#define IS_TIM_TI1SELECTION(TI1SELECTION)   (((TI1SELECTION) == TIM_TI1SELECTION_CH1) || \
                                             ((TI1SELECTION) == TIM_TI1SELECTION_XORCOMBINATION))
/**
  * @}
  */ 

/** @defgroup TIM_DMA_Base_address 
  * @{
  */
#define TIM_DMABase_CR1                    (0x00000000)
#define TIM_DMABase_CR2                    (0x00000001)
#define TIM_DMABase_SMCR                   (0x00000002)
#define TIM_DMABase_DIER                   (0x00000003)
#define TIM_DMABase_SR                     (0x00000004)
#define TIM_DMABase_EGR                    (0x00000005)
#define TIM_DMABase_CCMR1                  (0x00000006)
#define TIM_DMABase_CCMR2                  (0x00000007)
#define TIM_DMABase_CCER                   (0x00000008)
#define TIM_DMABase_CNT                    (0x00000009)
#define TIM_DMABase_PSC                    (0x0000000A)
#define TIM_DMABase_ARR                    (0x0000000B)
#define TIM_DMABase_RCR                    (0x0000000C)
#define TIM_DMABase_CCR1                   (0x0000000D)
#define TIM_DMABase_CCR2                   (0x0000000E)
#define TIM_DMABase_CCR3                   (0x0000000F)
#define TIM_DMABase_CCR4                   (0x00000010)
#define TIM_DMABase_BDTR                   (0x00000011)
#define TIM_DMABase_DCR                    (0x00000012)
#define TIM_DMABase_OR                     (0x00000013)
#define IS_TIM_DMA_BASE(BASE) (((BASE) == TIM_DMABase_CR1) || \
                               ((BASE) == TIM_DMABase_CR2) || \
                               ((BASE) == TIM_DMABase_SMCR) || \
                               ((BASE) == TIM_DMABase_DIER) || \
                               ((BASE) == TIM_DMABase_SR) || \
                               ((BASE) == TIM_DMABase_EGR) || \
                               ((BASE) == TIM_DMABase_CCMR1) || \
                               ((BASE) == TIM_DMABase_CCMR2) || \
                               ((BASE) == TIM_DMABase_CCER) || \
                               ((BASE) == TIM_DMABase_CNT) || \
                               ((BASE) == TIM_DMABase_PSC) || \
                               ((BASE) == TIM_DMABase_ARR) || \
                               ((BASE) == TIM_DMABase_RCR) || \
                               ((BASE) == TIM_DMABase_CCR1) || \
                               ((BASE) == TIM_DMABase_CCR2) || \
                               ((BASE) == TIM_DMABase_CCR3) || \
                               ((BASE) == TIM_DMABase_CCR4) || \
                               ((BASE) == TIM_DMABase_BDTR) || \
                               ((BASE) == TIM_DMABase_DCR) || \
                               ((BASE) == TIM_DMABase_OR))
/**
  * @}
  */ 

/** @defgroup TIM_DMA_Burst_Length 
  * @{
  */
#define TIM_DMABurstLength_1Transfer           (0x00000000)
#define TIM_DMABurstLength_2Transfers          (0x00000100)
#define TIM_DMABurstLength_3Transfers          (0x00000200)
#define TIM_DMABurstLength_4Transfers          (0x00000300)
#define TIM_DMABurstLength_5Transfers          (0x00000400)
#define TIM_DMABurstLength_6Transfers          (0x00000500)
#define TIM_DMABurstLength_7Transfers          (0x00000600)
#define TIM_DMABurstLength_8Transfers          (0x00000700)
#define TIM_DMABurstLength_9Transfers          (0x00000800)
#define TIM_DMABurstLength_10Transfers         (0x00000900)
#define TIM_DMABurstLength_11Transfers         (0x00000A00)
#define TIM_DMABurstLength_12Transfers         (0x00000B00)
#define TIM_DMABurstLength_13Transfers         (0x00000C00)
#define TIM_DMABurstLength_14Transfers         (0x00000D00)
#define TIM_DMABurstLength_15Transfers         (0x00000E00)
#define TIM_DMABurstLength_16Transfers         (0x00000F00)
#define TIM_DMABurstLength_17Transfers         (0x00001000)
#define TIM_DMABurstLength_18Transfers         (0x00001100)
#define IS_TIM_DMA_LENGTH(LENGTH) (((LENGTH) == TIM_DMABurstLength_1Transfer) || \
                                   ((LENGTH) == TIM_DMABurstLength_2Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_3Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_4Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_5Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_6Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_7Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_8Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_9Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_10Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_11Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_12Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_13Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_14Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_15Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_16Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_17Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_18Transfers))
/**
  * @}
  */

/** @defgroup TIM_Input_Capture_Filer_Value 
  * @{
  */
#define IS_TIM_IC_FILTER(ICFILTER) ((ICFILTER) <= 0xF) 
/**
  * @}
  */ 

/** @defgroup DMA_Handle_index 
  * @{
  */
#define TIM_DMA_ID_UPDATE                ((uint16_t) 0x0)       /*!< Index of the DMA handle used for Update DMA requests */
#define TIM_DMA_ID_CC1                   ((uint16_t) 0x1)       /*!< Index of the DMA handle used for Capture/Compare 1 DMA requests */
#define TIM_DMA_ID_CC2                   ((uint16_t) 0x2)       /*!< Index of the DMA handle used for Capture/Compare 2 DMA requests */
#define TIM_DMA_ID_CC3                   ((uint16_t) 0x3)       /*!< Index of the DMA handle used for Capture/Compare 3 DMA requests */
#define TIM_DMA_ID_CC4                   ((uint16_t) 0x4)       /*!< Index of the DMA handle used for Capture/Compare 4 DMA requests */
#define TIM_DMA_ID_COMMUTATION           ((uint16_t) 0x5)       /*!< Index of the DMA handle used for Commutation DMA requests */
#define TIM_DMA_ID_TRIGGER               ((uint16_t) 0x6)       /*!< Index of the DMA handle used for Trigger DMA requests */
/**
  * @}
  */ 

/** @defgroup Channel_CC_State 
  * @{
  */
#define TIM_CCx_ENABLE                   ((uint32_t)0x0001)
#define TIM_CCx_DISABLE                  ((uint32_t)0x0000)
#define TIM_CCxN_ENABLE                  ((uint32_t)0x0004)
#define TIM_CCxN_DISABLE                 ((uint32_t)0x0000)
/**
  * @}
  */ 

/**
  * @}
  */   
  
/* Exported macro ------------------------------------------------------------*/

/** @brief Reset TIM handle state
  * @param  __HANDLE__: TIM handle
  * @retval None
  */
#define __HAL_TIM_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_TIM_STATE_RESET)

/**
  * @brief  Enable the TIM peripheral.
  * @param  __HANDLE__: TIM handle
  * @retval None
 */
#define __HAL_TIM_ENABLE(__HANDLE__)                 ((__HANDLE__)->Instance->CR1|=(TIM_CR1_CEN))

/**
  * @brief  Enable the TIM main Output.
  * @param  __HANDLE__: TIM handle
  * @retval None
  */
#define __HAL_TIM_MOE_ENABLE(__HANDLE__)             ((__HANDLE__)->Instance->BDTR|=(TIM_BDTR_MOE))


/* The counter of a timer instance is disabled only if all the CCx and CCxN
   channels have been disabled */
#define CCER_CCxE_MASK ((uint32_t)(TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E))
#define CCER_CCxNE_MASK ((uint32_t)(TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE))

/**
  * @brief  Disable the TIM peripheral.
  * @param  __HANDLE__: TIM handle
  * @retval None
  */
#define __HAL_TIM_DISABLE(__HANDLE__) \
                        do { \
                          if (((__HANDLE__)->Instance->CCER & CCER_CCxE_MASK) == 0) \
                          { \
                            if(((__HANDLE__)->Instance->CCER & CCER_CCxNE_MASK) == 0) \
                            { \
                              (__HANDLE__)->Instance->CR1 &= ~(TIM_CR1_CEN); \
                            } \
                          } \
                        } while(0)

/* The Main Output of a timer instance is disabled only if all the CCx and CCxN
   channels have been disabled */
/**
  * @brief  Disable the TIM main Output.
  * @param  __HANDLE__: TIM handle
  * @retval None
  */
#define __HAL_TIM_MOE_DISABLE(__HANDLE__) \
                        do { \
                          if (((__HANDLE__)->Instance->CCER & CCER_CCxE_MASK) == 0) \
                          { \
                            if(((__HANDLE__)->Instance->CCER & CCER_CCxNE_MASK) == 0) \
                            { \
                              (__HANDLE__)->Instance->BDTR &= ~(TIM_BDTR_MOE); \
                            } \
                          } \
                        } while(0)

#define __HAL_TIM_ENABLE_IT(__HANDLE__, __INTERRUPT__)    ((__HANDLE__)->Instance->DIER |= (__INTERRUPT__))
#define __HAL_TIM_ENABLE_DMA(__HANDLE__, __DMA__)         ((__HANDLE__)->Instance->DIER |= (__DMA__))
#define __HAL_TIM_DISABLE_IT(__HANDLE__, __INTERRUPT__)   ((__HANDLE__)->Instance->DIER &= ~(__INTERRUPT__))
#define __HAL_TIM_DISABLE_DMA(__HANDLE__, __DMA__)        ((__HANDLE__)->Instance->DIER &= ~(__DMA__))
#define __HAL_TIM_GET_FLAG(__HANDLE__, __FLAG__)          (((__HANDLE__)->Instance->SR &(__FLAG__)) == (__FLAG__))
#define __HAL_TIM_CLEAR_FLAG(__HANDLE__, __FLAG__)        ((__HANDLE__)->Instance->SR = ~(__FLAG__))

#define __HAL_TIM_GET_ITSTATUS(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->DIER & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)
#define __HAL_TIM_CLEAR_IT(__HANDLE__, __INTERRUPT__)     ((__HANDLE__)->Instance->SR = ~(__INTERRUPT__))

#define __HAL_TIM_DIRECTION_STATUS(__HANDLE__)            (((__HANDLE__)->Instance->CR1 &(TIM_CR1_DIR)) == (TIM_CR1_DIR))
#define __HAL_TIM_PRESCALER (__HANDLE__, __PRESC__)       ((__HANDLE__)->Instance->PSC = (__PRESC__))

#define __HAL_TIM_SetICPrescalerValue(__HANDLE__, __CHANNEL__, __ICPSC__) \
(((__CHANNEL__) == TIM_CHANNEL_1) ? ((__HANDLE__)->Instance->CCMR1 |= (__ICPSC__)) :\
 ((__CHANNEL__) == TIM_CHANNEL_2) ? ((__HANDLE__)->Instance->CCMR1 |= ((__ICPSC__) << 8)) :\
 ((__CHANNEL__) == TIM_CHANNEL_3) ? ((__HANDLE__)->Instance->CCMR2 |= (__ICPSC__)) :\
 ((__HANDLE__)->Instance->CCMR2 |= ((__ICPSC__) << 8)))

#define __HAL_TIM_ResetICPrescalerValue(__HANDLE__, __CHANNEL__) \
(((__CHANNEL__) == TIM_CHANNEL_1) ? ((__HANDLE__)->Instance->CCMR1 &= (uint16_t)~TIM_CCMR1_IC1PSC) :\
 ((__CHANNEL__) == TIM_CHANNEL_2) ? ((__HANDLE__)->Instance->CCMR1 &= (uint16_t)~TIM_CCMR1_IC2PSC) :\
 ((__CHANNEL__) == TIM_CHANNEL_3) ? ((__HANDLE__)->Instance->CCMR2 &= (uint16_t)~TIM_CCMR2_IC3PSC) :\
 ((__HANDLE__)->Instance->CCMR2 &= (uint16_t)~TIM_CCMR2_IC4PSC))

/**
  * @brief  Sets the TIM Capture Compare Register value on runtime without
  *         calling another time ConfigChannel function.
  * @param  __HANDLE__: TIM handle.
  * @param  __CHANNEL__ : TIM Channels to be configured.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @param  __COMPARE__: specifies the Capture Compare register new value.
  * @retval None
  */
#define __HAL_TIM_SetCompare(__HANDLE__, __CHANNEL__, __COMPARE__) \
(*(__IO uint32_t *)(&((__HANDLE__)->Instance->CCR1) + ((__CHANNEL__) >> 2)) = (__COMPARE__))

/**
  * @brief  Gets the TIM Capture Compare Register value on runtime
  * @param  __HANDLE__: TIM handle.
  * @param  __CHANNEL__ : TIM Channel associated with the capture compare register
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: get capture/compare 1 register value
  *            @arg TIM_CHANNEL_2: get capture/compare 2 register value
  *            @arg TIM_CHANNEL_3: get capture/compare 3 register value
  *            @arg TIM_CHANNEL_4: get capture/compare 4 register value
  * @retval None
  */
#define __HAL_TIM_GetCompare(__HANDLE__, __CHANNEL__) \
  (*(__IO uint32_t *)(&((__HANDLE__)->Instance->CCR1) + ((__CHANNEL__) >> 2)))

/**
  * @brief  Sets the TIM Counter Register value on runtime.
  * @param  __HANDLE__: TIM handle.
  * @param  __COUNTER__: specifies the Counter register new value.
  * @retval None
  */
#define __HAL_TIM_SetCounter(__HANDLE__, __COUNTER__) ((__HANDLE__)->Instance->CNT = (__COUNTER__))

/**
  * @brief  Gets the TIM Counter Register value on runtime.
  * @param  __HANDLE__: TIM handle.
  * @retval None
  */
#define __HAL_TIM_GetCounter(__HANDLE__) ((__HANDLE__)->Instance->CNT)

/**
  * @brief  Sets the TIM Autoreload Register value on runtime without calling 
  *         another time any Init function.
  * @param  __HANDLE__: TIM handle.
  * @param  __AUTORELOAD__: specifies the Counter register new value.
  * @retval None
  */
#define __HAL_TIM_SetAutoreload(__HANDLE__, __AUTORELOAD__)                  \
                        do{                                                  \
                            (__HANDLE__)->Instance->ARR = (__AUTORELOAD__);  \
                            (__HANDLE__)->Init.Period = (__AUTORELOAD__);    \
                          } while(0)
/**
  * @brief  Gets the TIM Autoreload Register value on runtime
  * @param  __HANDLE__: TIM handle.
  * @retval None
  */
#define __HAL_TIM_GetAutoreload(__HANDLE__) ((__HANDLE__)->Instance->ARR)

/**
  * @brief  Sets the TIM Clock Division value on runtime without calling 
  *         another time any Init function. 
  * @param  __HANDLE__: TIM handle.
  * @param  __CKD__: specifies the clock division value.
  *          This parameter can be one of the following value:
  *            @arg TIM_CLOCKDIVISION_DIV1
  *            @arg TIM_CLOCKDIVISION_DIV2
  *            @arg TIM_CLOCKDIVISION_DIV4
  * @retval None
  */
#define __HAL_TIM_SetClockDivision(__HANDLE__, __CKD__) \
                        do{                                                             \
                              (__HANDLE__)->Instance->CR1 &= (uint16_t)(~TIM_CR1_CKD);  \
                              (__HANDLE__)->Instance->CR1 |= (__CKD__);                 \
                              (__HANDLE__)->Init.ClockDivision = (__CKD__);             \
                          } while(0)
/**
  * @brief  Gets the TIM Clock Division value on runtime
  * @param  __HANDLE__: TIM handle.
  * @retval None
  */
#define __HAL_TIM_GetClockDivision(__HANDLE__) ((__HANDLE__)->Instance->CR1 & TIM_CR1_CKD)

/**
  * @brief  Sets the TIM Input Capture prescaler on runtime without calling 
  *         another time HAL_TIM_IC_ConfigChannel() function.
  * @param  __HANDLE__: TIM handle.
  * @param  __CHANNEL__ : TIM Channels to be configured.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @param  __ICPSC__: specifies the Input Capture4 prescaler new value.
  *          This parameter can be one of the following values:
  *            @arg TIM_ICPSC_DIV1: no prescaler
  *            @arg TIM_ICPSC_DIV2: capture is done once every 2 events
  *            @arg TIM_ICPSC_DIV4: capture is done once every 4 events
  *            @arg TIM_ICPSC_DIV8: capture is done once every 8 events
  * @retval None
  */
#define __HAL_TIM_SetICPrescaler(__HANDLE__, __CHANNEL__, __ICPSC__) \
                        do{                                                    \
                              __HAL_TIM_ResetICPrescalerValue((__HANDLE__), (__CHANNEL__));  \
                              __HAL_TIM_SetICPrescalerValue((__HANDLE__), (__CHANNEL__), (__ICPSC__)); \
                          } while(0)

/**
  * @brief  Gets the TIM Input Capture prescaler on runtime
  * @param  __HANDLE__: TIM handle.
  * @param  __CHANNEL__ : TIM Channels to be configured.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: get input capture 1 prescaler value
  *            @arg TIM_CHANNEL_2: get input capture 2 prescaler value
  *            @arg TIM_CHANNEL_3: get input capture 3 prescaler value
  *            @arg TIM_CHANNEL_4: get input capture 4 prescaler value
  * @retval None
  */
#define __HAL_TIM_GetICPrescaler(__HANDLE__, __CHANNEL__)  \
  (((__CHANNEL__) == TIM_CHANNEL_1) ? ((__HANDLE__)->Instance->CCMR1 & TIM_CCMR1_IC1PSC) :\
   ((__CHANNEL__) == TIM_CHANNEL_2) ? (((__HANDLE__)->Instance->CCMR1 & TIM_CCMR1_IC2PSC) >> 8) :\
   ((__CHANNEL__) == TIM_CHANNEL_3) ? ((__HANDLE__)->Instance->CCMR2 & TIM_CCMR2_IC3PSC) :\
   (((__HANDLE__)->Instance->CCMR2 & TIM_CCMR2_IC4PSC)) >> 8)
/**
  * @}
  */

/* Include TIM HAL Extension module */
#include "stm32f4xx_hal_tim_ex.h"

/* Exported functions --------------------------------------------------------*/

/* Time Base functions ********************************************************/
HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim);
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop(TIM_HandleTypeDef *htim);
/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop_IT(TIM_HandleTypeDef *htim);
/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_TIM_Base_Start_DMA(TIM_HandleTypeDef *htim, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_Base_Stop_DMA(TIM_HandleTypeDef *htim);

/* Timer Output Compare functions **********************************************/
HAL_StatusTypeDef HAL_TIM_OC_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_OC_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_MspDeInit(TIM_HandleTypeDef *htim);
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIM_OC_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIM_OC_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_TIM_OC_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_OC_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);

/* Timer PWM functions *********************************************************/
HAL_StatusTypeDef HAL_TIM_PWM_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_PWM_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *htim);
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIM_PWM_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_PWM_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);

/* Timer Input Capture functions ***********************************************/
HAL_StatusTypeDef HAL_TIM_IC_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_IC_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef *htim);
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIM_IC_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIM_IC_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_TIM_IC_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_IC_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);

/* Timer One Pulse functions ***************************************************/
HAL_StatusTypeDef HAL_TIM_OnePulse_Init(TIM_HandleTypeDef *htim, uint32_t OnePulseMode);
HAL_StatusTypeDef HAL_TIM_OnePulse_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OnePulse_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OnePulse_MspDeInit(TIM_HandleTypeDef *htim);
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIM_OnePulse_Start(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIM_OnePulse_Stop(TIM_HandleTypeDef *htim, uint32_t OutputChannel);

/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIM_OnePulse_Start_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIM_OnePulse_Stop_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);

/* Timer Encoder functions *****************************************************/
HAL_StatusTypeDef HAL_TIM_Encoder_Init(TIM_HandleTypeDef *htim,  TIM_Encoder_InitTypeDef* sConfig);
HAL_StatusTypeDef HAL_TIM_Encoder_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef *htim);
 /* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIM_Encoder_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIM_Encoder_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_TIM_Encoder_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData1, uint32_t *pData2, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);

/* Interrupt Handler functions  **********************************************/
void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim);

/* Control functions  *********************************************************/
HAL_StatusTypeDef HAL_TIM_OC_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef* sConfig, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef* sConfig, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_ConfigChannel(TIM_HandleTypeDef *htim, TIM_IC_InitTypeDef* sConfig, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OnePulse_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OnePulse_InitTypeDef* sConfig, uint32_t OutputChannel,  uint32_t InputChannel);
HAL_StatusTypeDef HAL_TIM_ConfigOCrefClear(TIM_HandleTypeDef *htim, TIM_ClearInputConfigTypeDef * sClearInputConfig, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_ConfigClockSource(TIM_HandleTypeDef *htim, TIM_ClockConfigTypeDef * sClockSourceConfig);    
HAL_StatusTypeDef HAL_TIM_ConfigTI1Input(TIM_HandleTypeDef *htim, uint32_t TI1_Selection);
HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchronization(TIM_HandleTypeDef *htim, TIM_SlaveConfigTypeDef * sSlaveConfig);
HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress, uint32_t BurstRequestSrc, \
                                              uint32_t  *BurstBuffer, uint32_t  BurstLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStop(TIM_HandleTypeDef *htim, uint32_t BurstRequestSrc);
HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress, uint32_t BurstRequestSrc, \
                                              uint32_t  *BurstBuffer, uint32_t  BurstLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStop(TIM_HandleTypeDef *htim, uint32_t BurstRequestSrc);
HAL_StatusTypeDef HAL_TIM_GenerateEvent(TIM_HandleTypeDef *htim, uint32_t EventSource);
uint32_t HAL_TIM_ReadCapturedValue(TIM_HandleTypeDef *htim, uint32_t Channel);

/* Callback in non blocking modes (Interrupt and DMA) *************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim);

/* Peripheral State functions  **************************************************/
HAL_TIM_StateTypeDef HAL_TIM_Base_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_OC_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_PWM_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_IC_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_OnePulse_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_Encoder_GetState(TIM_HandleTypeDef *htim);

void TIM_Base_SetConfig(TIM_TypeDef *TIMx, TIM_Base_InitTypeDef *Structure);
void TIM_TI1_SetConfig(TIM_TypeDef *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICSelection, uint32_t TIM_ICFilter);
void TIM_OC2_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config);
void HAL_TIM_DMADelayPulseCplt(DMA_HandleTypeDef *hdma);
void HAL_TIM_DMAError(DMA_HandleTypeDef *hdma);
void HAL_TIM_DMACaptureCplt(DMA_HandleTypeDef *hdma);
void TIM_CCxChannelCmd(TIM_TypeDef* TIMx, uint32_t Channel, uint32_t ChannelState);

/**
  * @}
  */ 

/**
  * @}
  */ 
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_TIM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
