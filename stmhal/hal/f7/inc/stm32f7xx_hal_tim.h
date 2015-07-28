/**
  ******************************************************************************
  * @file    stm32f7xx_hal_tim.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-June-2015
  * @brief   Header file of TIM HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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
#ifndef __STM32F7xx_HAL_TIM_H
#define __STM32F7xx_HAL_TIM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal_def.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @addtogroup TIM
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup TIM_Exported_Types TIM Exported Types
  * @{
  */

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
                               This parameter can be a value of @ref TIMEx_Output_Compare_and_PWM_modes */

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
                               This parameter can be a value of @ref TIMEx_Output_Compare_and_PWM_modes */

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
                                      This parameter can be a value of @ref TIMEx_ClearInput_Source */
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
                                  This parameter can be a value of @ref TIMEx_Slave_Mode */
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
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup TIM_Exported_Constants  TIM Exported Constants
  * @{
  */

/** @defgroup TIM_Input_Channel_Polarity TIM Input Channel Polarity
  * @{
  */
#define  TIM_INPUTCHANNELPOLARITY_RISING      ((uint32_t)0x00000000)            /*!< Polarity for TIx source */
#define  TIM_INPUTCHANNELPOLARITY_FALLING     (TIM_CCER_CC1P)                   /*!< Polarity for TIx source */
#define  TIM_INPUTCHANNELPOLARITY_BOTHEDGE    (TIM_CCER_CC1P | TIM_CCER_CC1NP)  /*!< Polarity for TIx source */
/**
  * @}
  */

/** @defgroup TIM_ETR_Polarity  TIM ETR Polarity
  * @{
  */
#define TIM_ETRPOLARITY_INVERTED              (TIM_SMCR_ETP)                    /*!< Polarity for ETR source */
#define TIM_ETRPOLARITY_NONINVERTED           ((uint32_t)0x0000)                /*!< Polarity for ETR source */
/**
  * @}
  */

/** @defgroup TIM_ETR_Prescaler  TIM ETR Prescaler
  * @{
  */
#define TIM_ETRPRESCALER_DIV1                 ((uint32_t)0x0000)                /*!< No prescaler is used */
#define TIM_ETRPRESCALER_DIV2                 (TIM_SMCR_ETPS_0)                 /*!< ETR input source is divided by 2 */
#define TIM_ETRPRESCALER_DIV4                 (TIM_SMCR_ETPS_1)                 /*!< ETR input source is divided by 4 */
#define TIM_ETRPRESCALER_DIV8                 (TIM_SMCR_ETPS)                   /*!< ETR input source is divided by 8 */
/**
  * @}
  */

/** @defgroup TIM_Counter_Mode  TIM Counter Mode
  * @{
  */
#define TIM_COUNTERMODE_UP                 ((uint32_t)0x0000)
#define TIM_COUNTERMODE_DOWN               TIM_CR1_DIR
#define TIM_COUNTERMODE_CENTERALIGNED1     TIM_CR1_CMS_0
#define TIM_COUNTERMODE_CENTERALIGNED2     TIM_CR1_CMS_1
#define TIM_COUNTERMODE_CENTERALIGNED3     TIM_CR1_CMS
/**
  * @}
  */

/** @defgroup TIM_ClockDivision TIM Clock Division
  * @{
  */
#define TIM_CLOCKDIVISION_DIV1                       ((uint32_t)0x0000)
#define TIM_CLOCKDIVISION_DIV2                       (TIM_CR1_CKD_0)
#define TIM_CLOCKDIVISION_DIV4                       (TIM_CR1_CKD_1)
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_State TIM Output Compare State
  * @{
  */
#define TIM_OUTPUTSTATE_DISABLE            ((uint32_t)0x0000)
#define TIM_OUTPUTSTATE_ENABLE             (TIM_CCER_CC1E)

/**
  * @}
  */

/** @defgroup TIM_Output_Fast_State  TIM Output Fast State
  * @{
  */
#define TIM_OCFAST_DISABLE                ((uint32_t)0x0000)
#define TIM_OCFAST_ENABLE                 (TIM_CCMR1_OC1FE)
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_N_State TIM Complementary Output Compare State
  * @{
  */
#define TIM_OUTPUTNSTATE_DISABLE            ((uint32_t)0x0000)
#define TIM_OUTPUTNSTATE_ENABLE             (TIM_CCER_CC1NE)
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_Polarity TIM Output Compare Polarity
  * @{
  */
#define TIM_OCPOLARITY_HIGH                ((uint32_t)0x0000)
#define TIM_OCPOLARITY_LOW                 (TIM_CCER_CC1P)
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_N_Polarity TIM Complementary Output Compare Polarity
  * @{
  */
#define TIM_OCNPOLARITY_HIGH               ((uint32_t)0x0000)
#define TIM_OCNPOLARITY_LOW                (TIM_CCER_CC1NP)
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_Idle_State  TIM Output Compare Idle State
  * @{
  */
#define TIM_OCIDLESTATE_SET                (TIM_CR2_OIS1)
#define TIM_OCIDLESTATE_RESET              ((uint32_t)0x0000)
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_N_Idle_State  TIM Output Compare N Idle State
  * @{
  */
#define TIM_OCNIDLESTATE_SET               (TIM_CR2_OIS1N)
#define TIM_OCNIDLESTATE_RESET             ((uint32_t)0x0000)
/**
  * @}
  */

/** @defgroup TIM_Input_Capture_Polarity  TIM Input Capture Polarity
  * @{
  */
#define  TIM_ICPOLARITY_RISING             TIM_INPUTCHANNELPOLARITY_RISING
#define  TIM_ICPOLARITY_FALLING            TIM_INPUTCHANNELPOLARITY_FALLING
#define  TIM_ICPOLARITY_BOTHEDGE           TIM_INPUTCHANNELPOLARITY_BOTHEDGE
/**
  * @}
  */

/** @defgroup TIM_Input_Capture_Selection  TIM Input Capture Selection
  * @{
  */
#define TIM_ICSELECTION_DIRECTTI           (TIM_CCMR1_CC1S_0)   /*!< TIM Input 1, 2, 3 or 4 is selected to be
                                                                     connected to IC1, IC2, IC3 or IC4, respectively */
#define TIM_ICSELECTION_INDIRECTTI         (TIM_CCMR1_CC1S_1)   /*!< TIM Input 1, 2, 3 or 4 is selected to be
                                                                     connected to IC2, IC1, IC4 or IC3, respectively */
#define TIM_ICSELECTION_TRC                (TIM_CCMR1_CC1S)     /*!< TIM Input 1, 2, 3 or 4 is selected to be connected to TRC */

/**
  * @}
  */

/** @defgroup TIM_Input_Capture_Prescaler  TIM Input Capture Prescaler
  * @{
  */
#define TIM_ICPSC_DIV1                     ((uint32_t)0x0000)       /*!< Capture performed each time an edge is detected on the capture input */
#define TIM_ICPSC_DIV2                     (TIM_CCMR1_IC1PSC_0)     /*!< Capture performed once every 2 events */
#define TIM_ICPSC_DIV4                     (TIM_CCMR1_IC1PSC_1)     /*!< Capture performed once every 4 events */
#define TIM_ICPSC_DIV8                     (TIM_CCMR1_IC1PSC)       /*!< Capture performed once every 8 events */
/**
  * @}
  */

/** @defgroup TIM_One_Pulse_Mode TIM One Pulse Mode
  * @{
  */
#define TIM_OPMODE_SINGLE                  (TIM_CR1_OPM)
#define TIM_OPMODE_REPETITIVE              ((uint32_t)0x0000)
/**
  * @}
  */

/** @defgroup TIM_Encoder_Mode TIM Encoder Mode
  * @{
  */
#define TIM_ENCODERMODE_TI1                (TIM_SMCR_SMS_0)
#define TIM_ENCODERMODE_TI2                (TIM_SMCR_SMS_1)
#define TIM_ENCODERMODE_TI12               (TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0)

/**
  * @}
  */

/** @defgroup TIM_Interrupt_definition  TIM Interrupt definition
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
/**
  * @}
  */

/** @defgroup TIM_Commutation_Source  TIM Commutation Source
  * @{
  */
#define TIM_COMMUTATION_TRGI              (TIM_CR2_CCUS)
#define TIM_COMMUTATION_SOFTWARE          ((uint32_t)0x0000)
/**
  * @}
  */

/** @defgroup TIM_DMA_sources  TIM DMA sources
  * @{
  */
#define TIM_DMA_UPDATE                     (TIM_DIER_UDE)
#define TIM_DMA_CC1                        (TIM_DIER_CC1DE)
#define TIM_DMA_CC2                        (TIM_DIER_CC2DE)
#define TIM_DMA_CC3                        (TIM_DIER_CC3DE)
#define TIM_DMA_CC4                        (TIM_DIER_CC4DE)
#define TIM_DMA_COM                        (TIM_DIER_COMDE)
#define TIM_DMA_TRIGGER                    (TIM_DIER_TDE)
/**
  * @}
  */

/** @defgroup TIM_Event_Source  TIM Event Source
  * @{
  */
#define TIM_EVENTSOURCE_UPDATE              TIM_EGR_UG
#define TIM_EVENTSOURCE_CC1                 TIM_EGR_CC1G
#define TIM_EVENTSOURCE_CC2                 TIM_EGR_CC2G
#define TIM_EVENTSOURCE_CC3                 TIM_EGR_CC3G
#define TIM_EVENTSOURCE_CC4                 TIM_EGR_CC4G
#define TIM_EVENTSOURCE_COM                 TIM_EGR_COMG
#define TIM_EVENTSOURCE_TRIGGER             TIM_EGR_TG
#define TIM_EVENTSOURCE_BREAK               TIM_EGR_BG
#define TIM_EVENTSOURCE_BREAK2              TIM_EGR_B2G
/**
  * @}
  */

/** @defgroup TIM_Flag_definition  TIM Flag definition
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
#define TIM_FLAG_BREAK2                    (TIM_SR_B2IF)
#define TIM_FLAG_CC1OF                     (TIM_SR_CC1OF)
#define TIM_FLAG_CC2OF                     (TIM_SR_CC2OF)
#define TIM_FLAG_CC3OF                     (TIM_SR_CC3OF)
#define TIM_FLAG_CC4OF                     (TIM_SR_CC4OF)
/**
  * @}
  */

/** @defgroup TIM_Clock_Source  TIM Clock Source
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
/**
  * @}
  */

/** @defgroup TIM_Clock_Polarity  TIM Clock Polarity
  * @{
  */
#define TIM_CLOCKPOLARITY_INVERTED           TIM_ETRPOLARITY_INVERTED          /*!< Polarity for ETRx clock sources */
#define TIM_CLOCKPOLARITY_NONINVERTED        TIM_ETRPOLARITY_NONINVERTED       /*!< Polarity for ETRx clock sources */
#define TIM_CLOCKPOLARITY_RISING             TIM_INPUTCHANNELPOLARITY_RISING   /*!< Polarity for TIx clock sources */
#define TIM_CLOCKPOLARITY_FALLING            TIM_INPUTCHANNELPOLARITY_FALLING   /*!< Polarity for TIx clock sources */
#define TIM_CLOCKPOLARITY_BOTHEDGE           TIM_INPUTCHANNELPOLARITY_BOTHEDGE  /*!< Polarity for TIx clock sources */
/**
  * @}
  */

/** @defgroup TIM_Clock_Prescaler  TIM Clock Prescaler
  * @{
  */
#define TIM_CLOCKPRESCALER_DIV1              TIM_ETRPRESCALER_DIV1     /*!< No prescaler is used */
#define TIM_CLOCKPRESCALER_DIV2              TIM_ETRPRESCALER_DIV2     /*!< Prescaler for External ETR Clock: Capture performed once every 2 events. */
#define TIM_CLOCKPRESCALER_DIV4              TIM_ETRPRESCALER_DIV4     /*!< Prescaler for External ETR Clock: Capture performed once every 4 events. */
#define TIM_CLOCKPRESCALER_DIV8              TIM_ETRPRESCALER_DIV8     /*!< Prescaler for External ETR Clock: Capture performed once every 8 events. */
/**
  * @}
  */

/** @defgroup TIM_ClearInput_Polarity  TIM Clear Input Polarity
  * @{
  */
#define TIM_CLEARINPUTPOLARITY_INVERTED           TIM_ETRPOLARITY_INVERTED                    /*!< Polarity for ETRx pin */
#define TIM_CLEARINPUTPOLARITY_NONINVERTED        TIM_ETRPOLARITY_NONINVERTED                 /*!< Polarity for ETRx pin */
/**
  * @}
  */

/** @defgroup TIM_ClearInput_Prescaler TIM Clear Input Prescaler
  * @{
  */
#define TIM_CLEARINPUTPRESCALER_DIV1                    TIM_ETRPRESCALER_DIV1      /*!< No prescaler is used */
#define TIM_CLEARINPUTPRESCALER_DIV2                    TIM_ETRPRESCALER_DIV2      /*!< Prescaler for External ETR pin: Capture performed once every 2 events. */
#define TIM_CLEARINPUTPRESCALER_DIV4                    TIM_ETRPRESCALER_DIV4      /*!< Prescaler for External ETR pin: Capture performed once every 4 events. */
#define TIM_CLEARINPUTPRESCALER_DIV8                    TIM_ETRPRESCALER_DIV8        /*!< Prescaler for External ETR pin: Capture performed once every 8 events. */
/**
  * @}
  */

/** @defgroup TIM_OSSR_Off_State_Selection_for_Run_mode_state TIM OSSR OffState Selection for Run mode state
  * @{
  */
#define TIM_OSSR_ENABLE 	      (TIM_BDTR_OSSR)
#define TIM_OSSR_DISABLE          ((uint32_t)0x0000)
/**
  * @}
  */

/** @defgroup TIM_OSSI_Off_State_Selection_for_Idle_mode_state TIM OSSI OffState Selection for Idle mode state
  * @{
  */
#define TIM_OSSI_ENABLE	 	    (TIM_BDTR_OSSI)
#define TIM_OSSI_DISABLE            ((uint32_t)0x0000)
/**
  * @}
  */

/** @defgroup TIM_Lock_level  TIM Lock level
  * @{
  */
#define TIM_LOCKLEVEL_OFF	   ((uint32_t)0x0000)
#define TIM_LOCKLEVEL_1            (TIM_BDTR_LOCK_0)
#define TIM_LOCKLEVEL_2            (TIM_BDTR_LOCK_1)
#define TIM_LOCKLEVEL_3            (TIM_BDTR_LOCK)
/**
  * @}
  */
/** @defgroup TIM_Break_Input_enable_disable  TIM Break Input State
  * @{
  */
#define TIM_BREAK_ENABLE          (TIM_BDTR_BKE)
#define TIM_BREAK_DISABLE         ((uint32_t)0x0000)
/**
  * @}
  */

/** @defgroup TIM_Break_Polarity  TIM Break Polarity
  * @{
  */
#define TIM_BREAKPOLARITY_LOW        ((uint32_t)0x0000)
#define TIM_BREAKPOLARITY_HIGH       (TIM_BDTR_BKP)
/**
  * @}
  */

/** @defgroup TIM_AOE_Bit_Set_Reset  TIM AOE Bit State
  * @{
  */
#define TIM_AUTOMATICOUTPUT_ENABLE           (TIM_BDTR_AOE)
#define	TIM_AUTOMATICOUTPUT_DISABLE          ((uint32_t)0x0000)
/**
  * @}
  */

/** @defgroup TIM_Master_Mode_Selection TIM Master Mode Selection
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
/**
  * @}
  */

/** @defgroup TIM_Master_Slave_Mode  TIM Master Slave Mode
  * @{
  */
#define TIM_MASTERSLAVEMODE_ENABLE          ((uint32_t)0x0080)
#define TIM_MASTERSLAVEMODE_DISABLE         ((uint32_t)0x0000)
/**
  * @}
  */

/** @defgroup TIM_Trigger_Selection  TIM Trigger Selection
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
/**
  * @}
  */

/** @defgroup TIM_Trigger_Polarity TIM Trigger Polarity
  * @{
  */
#define TIM_TRIGGERPOLARITY_INVERTED           TIM_ETRPOLARITY_INVERTED      /*!< Polarity for ETRx trigger sources */
#define TIM_TRIGGERPOLARITY_NONINVERTED        TIM_ETRPOLARITY_NONINVERTED   /*!< Polarity for ETRx trigger sources */
#define TIM_TRIGGERPOLARITY_RISING             TIM_INPUTCHANNELPOLARITY_RISING        /*!< Polarity for TIxFPx or TI1_ED trigger sources */
#define TIM_TRIGGERPOLARITY_FALLING            TIM_INPUTCHANNELPOLARITY_FALLING       /*!< Polarity for TIxFPx or TI1_ED trigger sources */
#define TIM_TRIGGERPOLARITY_BOTHEDGE           TIM_INPUTCHANNELPOLARITY_BOTHEDGE      /*!< Polarity for TIxFPx or TI1_ED trigger sources */
/**
  * @}
  */

/** @defgroup TIM_Trigger_Prescaler TIM Trigger Prescaler
  * @{
  */
#define TIM_TRIGGERPRESCALER_DIV1             TIM_ETRPRESCALER_DIV1     /*!< No prescaler is used */
#define TIM_TRIGGERPRESCALER_DIV2             TIM_ETRPRESCALER_DIV2     /*!< Prescaler for External ETR Trigger: Capture performed once every 2 events. */
#define TIM_TRIGGERPRESCALER_DIV4             TIM_ETRPRESCALER_DIV4     /*!< Prescaler for External ETR Trigger: Capture performed once every 4 events. */
#define TIM_TRIGGERPRESCALER_DIV8             TIM_ETRPRESCALER_DIV8     /*!< Prescaler for External ETR Trigger: Capture performed once every 8 events. */
/**
  * @}
  */


/** @defgroup TIM_TI1_Selection TIM TI1 Selection
  * @{
  */
#define TIM_TI1SELECTION_CH1                ((uint32_t)0x0000)
#define TIM_TI1SELECTION_XORCOMBINATION     (TIM_CR2_TI1S)
/**
  * @}
  */

/** @defgroup TIM_DMA_Base_address  TIM DMA Base address
  * @{
  */
#define TIM_DMABASE_CR1                    (0x00000000)
#define TIM_DMABASE_CR2                    (0x00000001)
#define TIM_DMABASE_SMCR                   (0x00000002)
#define TIM_DMABASE_DIER                   (0x00000003)
#define TIM_DMABASE_SR                     (0x00000004)
#define TIM_DMABASE_EGR                    (0x00000005)
#define TIM_DMABASE_CCMR1                  (0x00000006)
#define TIM_DMABASE_CCMR2                  (0x00000007)
#define TIM_DMABASE_CCER                   (0x00000008)
#define TIM_DMABASE_CNT                    (0x00000009)
#define TIM_DMABASE_PSC                    (0x0000000A)
#define TIM_DMABASE_ARR                    (0x0000000B)
#define TIM_DMABASE_RCR                    (0x0000000C)
#define TIM_DMABASE_CCR1                   (0x0000000D)
#define TIM_DMABASE_CCR2                   (0x0000000E)
#define TIM_DMABASE_CCR3                   (0x0000000F)
#define TIM_DMABASE_CCR4                   (0x00000010)
#define TIM_DMABASE_BDTR                   (0x00000011)
#define TIM_DMABASE_DCR                    (0x00000012)
#define TIM_DMABASE_OR                     (0x00000013)
/**
  * @}
  */

/** @defgroup TIM_DMA_Burst_Length  TIM DMA Burst Length
  * @{
  */
#define TIM_DMABURSTLENGTH_1TRANSFER           (0x00000000)
#define TIM_DMABURSTLENGTH_2TRANSFERS          (0x00000100)
#define TIM_DMABURSTLENGTH_3TRANSFERS          (0x00000200)
#define TIM_DMABURSTLENGTH_4TRANSFERS          (0x00000300)
#define TIM_DMABURSTLENGTH_5TRANSFERS          (0x00000400)
#define TIM_DMABURSTLENGTH_6TRANSFERS          (0x00000500)
#define TIM_DMABURSTLENGTH_7TRANSFERS          (0x00000600)
#define TIM_DMABURSTLENGTH_8TRANSFERS          (0x00000700)
#define TIM_DMABURSTLENGTH_9TRANSFERS          (0x00000800)
#define TIM_DMABURSTLENGTH_10TRANSFERS         (0x00000900)
#define TIM_DMABURSTLENGTH_11TRANSFERS         (0x00000A00)
#define TIM_DMABURSTLENGTH_12TRANSFERS         (0x00000B00)
#define TIM_DMABURSTLENGTH_13TRANSFERS         (0x00000C00)
#define TIM_DMABURSTLENGTH_14TRANSFERS         (0x00000D00)
#define TIM_DMABURSTLENGTH_15TRANSFERS         (0x00000E00)
#define TIM_DMABURSTLENGTH_16TRANSFERS         (0x00000F00)
#define TIM_DMABURSTLENGTH_17TRANSFERS         (0x00001000)
#define TIM_DMABURSTLENGTH_18TRANSFERS         (0x00001100)
/**
  * @}
  */

/** @defgroup DMA_Handle_index  DMA Handle index
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

/** @defgroup Channel_CC_State  Channel CC State
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
/** @defgroup TIM_Exported_Macros TIM Exported Macros
  * @{
  */
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
  * @brief  Enable the TIM update source request.
  * @param  __HANDLE__: TIM handle
  * @retval None
 */
#define __HAL_TIM_URS_ENABLE(__HANDLE__)             ((__HANDLE__)->Instance->CR1|=(TIM_CR1_URS))

/**
  * @brief  Enable the TIM main Output.
  * @param  __HANDLE__: TIM handle
  * @retval None
  */
#define __HAL_TIM_MOE_ENABLE(__HANDLE__)             ((__HANDLE__)->Instance->BDTR|=(TIM_BDTR_MOE))


/* The counter of a timer instance is disabled only if all the CCx and CCxN
   channels have been disabled */
#define TIM_CCER_CCxE_MASK ((uint32_t)(TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E))
#define TIM_CCER_CCxNE_MASK ((uint32_t)(TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE))

/**
  * @brief  Disable the TIM peripheral.
  * @param  __HANDLE__: TIM handle
  * @retval None
  */
#define __HAL_TIM_DISABLE(__HANDLE__) \
                        do { \
                          if (((__HANDLE__)->Instance->CCER & TIM_CCER_CCxE_MASK) == 0) \
                          { \
                            if(((__HANDLE__)->Instance->CCER & TIM_CCER_CCxNE_MASK) == 0) \
                            { \
                              (__HANDLE__)->Instance->CR1 &= ~(TIM_CR1_CEN); \
                            } \
                          } \
                        } while(0)

/**
  * @brief  Disable the TIM update source request.
  * @param  __HANDLE__: TIM handle
  * @retval None
 */
#define __HAL_TIM_URS_DISABLE(__HANDLE__)            ((__HANDLE__)->Instance->CR1&=~(TIM_CR1_URS))


/* The Main Output of a timer instance is disabled only if all the CCx and CCxN
   channels have been disabled */
/**
  * @brief  Disable the TIM main Output.
  * @param  __HANDLE__: TIM handle
  * @retval None
  */
#define __HAL_TIM_MOE_DISABLE(__HANDLE__) \
                        do { \
                          if (((__HANDLE__)->Instance->CCER & TIM_CCER_CCxE_MASK) == 0) \
                          { \
                            if(((__HANDLE__)->Instance->CCER & TIM_CCER_CCxNE_MASK) == 0) \
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

#define __HAL_TIM_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->DIER & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)
#define __HAL_TIM_CLEAR_IT(__HANDLE__, __INTERRUPT__)     ((__HANDLE__)->Instance->SR = ~(__INTERRUPT__))

#define __HAL_TIM_IS_TIM_COUNTING_DOWN(__HANDLE__)            (((__HANDLE__)->Instance->CR1 &(TIM_CR1_DIR)) == (TIM_CR1_DIR))
#define __HAL_TIM_SET_PRESCALER (__HANDLE__, __PRESC__)       ((__HANDLE__)->Instance->PSC = (__PRESC__))

#define TIM_SET_ICPRESCALERVALUE(__HANDLE__, __CHANNEL__, __ICPSC__) \
(((__CHANNEL__) == TIM_CHANNEL_1) ? ((__HANDLE__)->Instance->CCMR1 |= (__ICPSC__)) :\
 ((__CHANNEL__) == TIM_CHANNEL_2) ? ((__HANDLE__)->Instance->CCMR1 |= ((__ICPSC__) << 8)) :\
 ((__CHANNEL__) == TIM_CHANNEL_3) ? ((__HANDLE__)->Instance->CCMR2 |= (__ICPSC__)) :\
 ((__HANDLE__)->Instance->CCMR2 |= ((__ICPSC__) << 8)))

#define TIM_RESET_ICPRESCALERVALUE(__HANDLE__, __CHANNEL__) \
(((__CHANNEL__) == TIM_CHANNEL_1) ? ((__HANDLE__)->Instance->CCMR1 &= (uint16_t)~TIM_CCMR1_IC1PSC) :\
 ((__CHANNEL__) == TIM_CHANNEL_2) ? ((__HANDLE__)->Instance->CCMR1 &= (uint16_t)~TIM_CCMR1_IC2PSC) :\
 ((__CHANNEL__) == TIM_CHANNEL_3) ? ((__HANDLE__)->Instance->CCMR2 &= (uint16_t)~TIM_CCMR2_IC3PSC) :\
 ((__HANDLE__)->Instance->CCMR2 &= (uint16_t)~TIM_CCMR2_IC4PSC))

#define TIM_SET_CAPTUREPOLARITY(__HANDLE__, __CHANNEL__, __POLARITY__) \
(((__CHANNEL__) == TIM_CHANNEL_1) ? ((__HANDLE__)->Instance->CCER |= (__POLARITY__)) :\
 ((__CHANNEL__) == TIM_CHANNEL_2) ? ((__HANDLE__)->Instance->CCER |= ((__POLARITY__) << 4)) :\
 ((__CHANNEL__) == TIM_CHANNEL_3) ? ((__HANDLE__)->Instance->CCER |= ((__POLARITY__) << 8)) :\
 ((__HANDLE__)->Instance->CCER |= (((__POLARITY__) << 12) & TIM_CCER_CC4P)))

#define TIM_RESET_CAPTUREPOLARITY(__HANDLE__, __CHANNEL__) \
(((__CHANNEL__) == TIM_CHANNEL_1) ? ((__HANDLE__)->Instance->CCER &= (uint16_t)~(TIM_CCER_CC1P | TIM_CCER_CC1NP)) :\
 ((__CHANNEL__) == TIM_CHANNEL_2) ? ((__HANDLE__)->Instance->CCER &= (uint16_t)~(TIM_CCER_CC2P | TIM_CCER_CC2NP)) :\
 ((__CHANNEL__) == TIM_CHANNEL_3) ? ((__HANDLE__)->Instance->CCER &= (uint16_t)~(TIM_CCER_CC3P | TIM_CCER_CC3NP)) :\
 ((__HANDLE__)->Instance->CCER &= (uint16_t)~TIM_CCER_CC4P))

/**
  * @brief  Sets the TIM Counter Register value on runtime.
  * @param  __HANDLE__: TIM handle.
  * @param  __COUNTER__: specifies the Counter register new value.
  * @retval None
  */
#define __HAL_TIM_SET_COUNTER(__HANDLE__, __COUNTER__) ((__HANDLE__)->Instance->CNT = (__COUNTER__))

/**
  * @brief  Gets the TIM Counter Register value on runtime.
  * @param  __HANDLE__: TIM handle.
  * @retval None
  */
#define __HAL_TIM_GET_COUNTER(__HANDLE__) ((__HANDLE__)->Instance->CNT)

/**
  * @brief  Sets the TIM Autoreload Register value on runtime without calling
  *         another time any Init function.
  * @param  __HANDLE__: TIM handle.
  * @param  __AUTORELOAD__: specifies the Counter register new value.
  * @retval None
  */
#define __HAL_TIM_SET_AUTORELOAD(__HANDLE__, __AUTORELOAD__)                  \
                        do{                                                  \
                            (__HANDLE__)->Instance->ARR = (__AUTORELOAD__);  \
                            (__HANDLE__)->Init.Period = (__AUTORELOAD__);    \
                          } while(0)
/**
  * @brief  Gets the TIM Autoreload Register value on runtime
  * @param  __HANDLE__: TIM handle.
  * @retval None
  */
#define __HAL_TIM_GET_AUTORELOAD(__HANDLE__) ((__HANDLE__)->Instance->ARR)

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
#define __HAL_TIM_SET_CLOCKDIVISION(__HANDLE__, __CKD__) \
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
#define __HAL_TIM_GET_CLOCKDIVISION(__HANDLE__) ((__HANDLE__)->Instance->CR1 & TIM_CR1_CKD)

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
#define __HAL_TIM_SET_ICPRESCALER(__HANDLE__, __CHANNEL__, __ICPSC__) \
                        do{                                                    \
                              TIM_RESET_ICPRESCALERVALUE((__HANDLE__), (__CHANNEL__));  \
                              TIM_SET_ICPRESCALERVALUE((__HANDLE__), (__CHANNEL__), (__ICPSC__)); \
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
#define __HAL_TIM_GET_ICPRESCALER(__HANDLE__, __CHANNEL__)  \
  (((__CHANNEL__) == TIM_CHANNEL_1) ? ((__HANDLE__)->Instance->CCMR1 & TIM_CCMR1_IC1PSC) :\
   ((__CHANNEL__) == TIM_CHANNEL_2) ? (((__HANDLE__)->Instance->CCMR1 & TIM_CCMR1_IC2PSC) >> 8) :\
   ((__CHANNEL__) == TIM_CHANNEL_3) ? ((__HANDLE__)->Instance->CCMR2 & TIM_CCMR2_IC3PSC) :\
   (((__HANDLE__)->Instance->CCMR2 & TIM_CCMR2_IC4PSC)) >> 8)

/**
  * @brief  Sets the TIM Capture x input polarity on runtime.
  * @param  __HANDLE__: TIM handle.
  * @param  __CHANNEL__: TIM Channels to be configured.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @param  __POLARITY__: Polarity for TIx source
  *            @arg TIM_INPUTCHANNELPOLARITY_RISING: Rising Edge
  *            @arg TIM_INPUTCHANNELPOLARITY_FALLING: Falling Edge
  *            @arg TIM_INPUTCHANNELPOLARITY_BOTHEDGE: Rising and Falling Edge
  * @note  The polarity TIM_INPUTCHANNELPOLARITY_BOTHEDGE is not authorized  for TIM Channel 4.
  * @retval None
  */
#define __HAL_TIM_SET_CAPTUREPOLARITY(__HANDLE__, __CHANNEL__, __POLARITY__)                          \
                       do{                                                                            \
                           TIM_RESET_CAPTUREPOLARITY((__HANDLE__), (__CHANNEL__));               \
                           TIM_SET_CAPTUREPOLARITY((__HANDLE__), (__CHANNEL__), (__POLARITY__)); \
                         }while(0)

/**
  * @}
  */

/* Include TIM HAL Extension module */
#include "stm32f7xx_hal_tim_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @addtogroup TIM_Exported_Functions
  * @{
  */

/** @addtogroup TIM_Exported_Functions_Group1
  * @{
  */

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
/**
  * @}
  */

/** @addtogroup TIM_Exported_Functions_Group2
  * @{
  */
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

/**
  * @}
  */

/** @addtogroup TIM_Exported_Functions_Group3
  * @{
  */
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

/**
  * @}
  */

/** @addtogroup TIM_Exported_Functions_Group4
  * @{
  */
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

/**
  * @}
  */

/** @addtogroup TIM_Exported_Functions_Group5
  * @{
  */
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

/**
  * @}
  */

/** @addtogroup TIM_Exported_Functions_Group6
  * @{
  */
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

/**
  * @}
  */

/** @addtogroup TIM_Exported_Functions_Group7
  * @{
  */
/* Interrupt Handler functions  **********************************************/
void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim);

/**
  * @}
  */

/** @addtogroup TIM_Exported_Functions_Group8
  * @{
  */
/* Control functions  *********************************************************/
HAL_StatusTypeDef HAL_TIM_OC_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef* sConfig, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef* sConfig, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_ConfigChannel(TIM_HandleTypeDef *htim, TIM_IC_InitTypeDef* sConfig, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OnePulse_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OnePulse_InitTypeDef* sConfig, uint32_t OutputChannel,  uint32_t InputChannel);
HAL_StatusTypeDef HAL_TIM_ConfigOCrefClear(TIM_HandleTypeDef *htim, TIM_ClearInputConfigTypeDef * sClearInputConfig, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_ConfigClockSource(TIM_HandleTypeDef *htim, TIM_ClockConfigTypeDef * sClockSourceConfig);
HAL_StatusTypeDef HAL_TIM_ConfigTI1Input(TIM_HandleTypeDef *htim, uint32_t TI1_Selection);
HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchronization(TIM_HandleTypeDef *htim, TIM_SlaveConfigTypeDef * sSlaveConfig);
HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchronization_IT(TIM_HandleTypeDef *htim, TIM_SlaveConfigTypeDef * sSlaveConfig);
HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress, uint32_t BurstRequestSrc, \
                                              uint32_t  *BurstBuffer, uint32_t  BurstLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStop(TIM_HandleTypeDef *htim, uint32_t BurstRequestSrc);
HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress, uint32_t BurstRequestSrc, \
                                              uint32_t  *BurstBuffer, uint32_t  BurstLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStop(TIM_HandleTypeDef *htim, uint32_t BurstRequestSrc);
HAL_StatusTypeDef HAL_TIM_GenerateEvent(TIM_HandleTypeDef *htim, uint32_t EventSource);
uint32_t HAL_TIM_ReadCapturedValue(TIM_HandleTypeDef *htim, uint32_t Channel);

/**
  * @}
  */

/** @addtogroup TIM_Exported_Functions_Group9
  * @{
  */
/* Callback in non blocking modes (Interrupt and DMA) *************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim);

/**
  * @}
  */

/** @addtogroup TIM_Exported_Functions_Group10
  * @{
  */
/* Peripheral State functions  **************************************************/
HAL_TIM_StateTypeDef HAL_TIM_Base_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_OC_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_PWM_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_IC_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_OnePulse_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_Encoder_GetState(TIM_HandleTypeDef *htim);

/**
  * @}
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup TIM_Private_Macros TIM Private Macros
  * @{
  */

/** @defgroup TIM_IS_TIM_Definitions TIM Private macros to check input parameters
  * @{
  */
#define IS_TIM_COUNTER_MODE(__MODE__) (((__MODE__) == TIM_COUNTERMODE_UP)              || \
                                       ((__MODE__) == TIM_COUNTERMODE_DOWN)            || \
                                       ((__MODE__) == TIM_COUNTERMODE_CENTERALIGNED1)  || \
                                       ((__MODE__) == TIM_COUNTERMODE_CENTERALIGNED2)  || \
                                       ((__MODE__) == TIM_COUNTERMODE_CENTERALIGNED3))

#define IS_TIM_CLOCKDIVISION_DIV(__DIV__) (((__DIV__) == TIM_CLOCKDIVISION_DIV1) || \
                                           ((__DIV__) == TIM_CLOCKDIVISION_DIV2) || \
                                           ((__DIV__) == TIM_CLOCKDIVISION_DIV4))

#define IS_TIM_FAST_STATE(__STATE__) (((__STATE__) == TIM_OCFAST_DISABLE) || \
                                      ((__STATE__) == TIM_OCFAST_ENABLE))

#define IS_TIM_OUTPUT_STATE(STATE) (((STATE) == TIM_OUTPUTSTATE_DISABLE) || \
                                    ((STATE) == TIM_OUTPUTSTATE_ENABLE))

#define IS_TIM_OUTPUTN_STATE(STATE) (((STATE) == TIM_OUTPUTNSTATE_DISABLE) || \
                                     ((STATE) == TIM_OUTPUTNSTATE_ENABLE))

#define IS_TIM_OC_POLARITY(__POLARITY__) (((__POLARITY__) == TIM_OCPOLARITY_HIGH) || \
                                          ((__POLARITY__) == TIM_OCPOLARITY_LOW))

#define IS_TIM_OCN_POLARITY(__POLARITY__) (((__POLARITY__) == TIM_OCNPOLARITY_HIGH) || \
                                           ((__POLARITY__) == TIM_OCNPOLARITY_LOW))

#define IS_TIM_OCIDLE_STATE(__STATE__) (((__STATE__) == TIM_OCIDLESTATE_SET) || \
                                        ((__STATE__) == TIM_OCIDLESTATE_RESET))

#define IS_TIM_OCNIDLE_STATE(__STATE__) (((__STATE__) == TIM_OCNIDLESTATE_SET) || \
                                         ((__STATE__) == TIM_OCNIDLESTATE_RESET))

#define IS_TIM_IC_POLARITY(__POLARITY__) (((__POLARITY__) == TIM_ICPOLARITY_RISING)   || \
                                          ((__POLARITY__) == TIM_ICPOLARITY_FALLING)  || \
                                          ((__POLARITY__) == TIM_ICPOLARITY_BOTHEDGE))

#define IS_TIM_IC_SELECTION(__SELECTION__) (((__SELECTION__) == TIM_ICSELECTION_DIRECTTI) || \
                                            ((__SELECTION__) == TIM_ICSELECTION_INDIRECTTI) || \
                                            ((__SELECTION__) == TIM_ICSELECTION_TRC))

#define IS_TIM_IC_PRESCALER(__PRESCALER__) (((__PRESCALER__) == TIM_ICPSC_DIV1) || \
                                            ((__PRESCALER__) == TIM_ICPSC_DIV2) || \
                                            ((__PRESCALER__) == TIM_ICPSC_DIV4) || \
                                            ((__PRESCALER__) == TIM_ICPSC_DIV8))

#define IS_TIM_OPM_MODE(__MODE__) (((__MODE__) == TIM_OPMODE_SINGLE) || \
                                   ((__MODE__) == TIM_OPMODE_REPETITIVE))

#define IS_TIM_ENCODER_MODE(__MODE__) (((__MODE__) == TIM_ENCODERMODE_TI1) || \
                                       ((__MODE__) == TIM_ENCODERMODE_TI2) || \
                                       ((__MODE__) == TIM_ENCODERMODE_TI12))

#define IS_TIM_IT(__IT__) ((((__IT__) & 0xFFFFFF00) == 0x00000000) && ((__IT__) != 0x00000000))


#define IS_TIM_GET_IT(__IT__) (((__IT__) == TIM_IT_UPDATE)  || \
                               ((__IT__) == TIM_IT_CC1)     || \
                               ((__IT__) == TIM_IT_CC2)     || \
                               ((__IT__) == TIM_IT_CC3)     || \
                               ((__IT__) == TIM_IT_CC4)     || \
                               ((__IT__) == TIM_IT_COM)     || \
                               ((__IT__) == TIM_IT_TRIGGER) || \
                               ((__IT__) == TIM_IT_BREAK))

#define IS_TIM_DMA_SOURCE(__SOURCE__) ((((__SOURCE__) & 0xFFFF80FF) == 0x00000000) && ((__SOURCE__) != 0x00000000))

#define IS_TIM_EVENT_SOURCE(__SOURCE__) ((((__SOURCE__) & 0xFFFFFE00) == 0x00000000) && ((__SOURCE__) != 0x00000000))

#define IS_TIM_FLAG(__FLAG__) (((__FLAG__) == TIM_FLAG_UPDATE) || \
                               ((__FLAG__) == TIM_FLAG_CC1)     || \
                               ((__FLAG__) == TIM_FLAG_CC2)     || \
                               ((__FLAG__) == TIM_FLAG_CC3)     || \
                               ((__FLAG__) == TIM_FLAG_CC4)     || \
                               ((__FLAG__) == TIM_FLAG_COM)     || \
                               ((__FLAG__) == TIM_FLAG_TRIGGER) || \
                               ((__FLAG__) == TIM_FLAG_BREAK)   || \
                               ((__FLAG__) == TIM_FLAG_BREAK2)  || \
                               ((__FLAG__) == TIM_FLAG_CC1OF)   || \
                               ((__FLAG__) == TIM_FLAG_CC2OF)   || \
                               ((__FLAG__) == TIM_FLAG_CC3OF)   || \
                               ((__FLAG__) == TIM_FLAG_CC4OF))

#define IS_TIM_CLOCKSOURCE(__CLOCK__) (((__CLOCK__) == TIM_CLOCKSOURCE_INTERNAL) || \
                                       ((__CLOCK__) == TIM_CLOCKSOURCE_ETRMODE2) || \
                                       ((__CLOCK__) == TIM_CLOCKSOURCE_ITR0)     || \
                                       ((__CLOCK__) == TIM_CLOCKSOURCE_ITR1)     || \
                                       ((__CLOCK__) == TIM_CLOCKSOURCE_ITR2)     || \
                                       ((__CLOCK__) == TIM_CLOCKSOURCE_ITR3)     || \
                                       ((__CLOCK__) == TIM_CLOCKSOURCE_TI1ED)    || \
                                       ((__CLOCK__) == TIM_CLOCKSOURCE_TI1)      || \
                                       ((__CLOCK__) == TIM_CLOCKSOURCE_TI2)      || \
                                       ((__CLOCK__) == TIM_CLOCKSOURCE_ETRMODE1))

#define IS_TIM_CLOCKPOLARITY(__POLARITY__) (((__POLARITY__) == TIM_CLOCKPOLARITY_INVERTED)    || \
                                        ((__POLARITY__) == TIM_CLOCKPOLARITY_NONINVERTED) || \
                                        ((__POLARITY__) == TIM_CLOCKPOLARITY_RISING)      || \
                                        ((__POLARITY__) == TIM_CLOCKPOLARITY_FALLING)     || \
                                        ((__POLARITY__) == TIM_CLOCKPOLARITY_BOTHEDGE))

#define IS_TIM_CLOCKPRESCALER(__PRESCALER__) (((__PRESCALER__) == TIM_CLOCKPRESCALER_DIV1) || \
                                              ((__PRESCALER__) == TIM_CLOCKPRESCALER_DIV2) || \
                                              ((__PRESCALER__) == TIM_CLOCKPRESCALER_DIV4) || \
                                              ((__PRESCALER__) == TIM_CLOCKPRESCALER_DIV8))

#define IS_TIM_CLOCKFILTER(__ICFILTER__)      ((__ICFILTER__) <= 0xF)

#define IS_TIM_CLEARINPUT_POLARITY(__POLARITY__)   (((__POLARITY__) == TIM_CLEARINPUTPOLARITY_INVERTED) || \
                                                    ((__POLARITY__) == TIM_CLEARINPUTPOLARITY_NONINVERTED))

#define IS_TIM_CLEARINPUT_PRESCALER(__PRESCALER__)   (((__PRESCALER__) == TIM_CLEARINPUTPRESCALER_DIV1) || \
                                                 ((__PRESCALER__) == TIM_CLEARINPUTPRESCALER_DIV2) || \
                                                 ((__PRESCALER__) == TIM_CLEARINPUTPRESCALER_DIV4) || \
                                                 ((__PRESCALER__) == TIM_CLEARINPUTPRESCALER_DIV8))

#define IS_TIM_CLEARINPUT_FILTER(__ICFILTER__) ((__ICFILTER__) <= 0xF)

#define IS_TIM_OSSR_STATE(__STATE__) (((__STATE__) == TIM_OSSR_ENABLE) || \
                                      ((__STATE__) == TIM_OSSR_DISABLE))

#define IS_TIM_OSSI_STATE(__STATE__) (((__STATE__) == TIM_OSSI_ENABLE) || \
                                      ((__STATE__) == TIM_OSSI_DISABLE))

#define IS_TIM_LOCK_LEVEL(__LEVEL__) (((__LEVEL__) == TIM_LOCKLEVEL_OFF) || \
                                      ((__LEVEL__) == TIM_LOCKLEVEL_1) || \
                                      ((__LEVEL__) == TIM_LOCKLEVEL_2) || \
                                      ((__LEVEL__) == TIM_LOCKLEVEL_3))

#define IS_TIM_BREAK_STATE(__STATE__) (((__STATE__) == TIM_BREAK_ENABLE) || \
                                       ((__STATE__) == TIM_BREAK_DISABLE))

#define IS_TIM_BREAK_POLARITY(__POLARITY__) (((__POLARITY__) == TIM_BREAKPOLARITY_LOW) || \
                                             ((__POLARITY__) == TIM_BREAKPOLARITY_HIGH))

#define IS_TIM_AUTOMATIC_OUTPUT_STATE(__STATE__) (((__STATE__) == TIM_AUTOMATICOUTPUT_ENABLE) || \
                                                  ((__STATE__) == TIM_AUTOMATICOUTPUT_DISABLE))

#define IS_TIM_TRGO_SOURCE(__SOURCE__) (((__SOURCE__) == TIM_TRGO_RESET) || \
                                        ((__SOURCE__) == TIM_TRGO_ENABLE) || \
                                        ((__SOURCE__) == TIM_TRGO_UPDATE) || \
                                        ((__SOURCE__) == TIM_TRGO_OC1) || \
                                        ((__SOURCE__) == TIM_TRGO_OC1REF) || \
                                        ((__SOURCE__) == TIM_TRGO_OC2REF) || \
                                        ((__SOURCE__) == TIM_TRGO_OC3REF) || \
                                        ((__SOURCE__) == TIM_TRGO_OC4REF))

#define IS_TIM_MSM_STATE(__STATE__) (((__STATE__) == TIM_MASTERSLAVEMODE_ENABLE) || \
                                     ((__STATE__) == TIM_MASTERSLAVEMODE_DISABLE))

#define IS_TIM_TRIGGER_SELECTION(__SELECTION__) (((__SELECTION__) == TIM_TS_ITR0) || \
                                                 ((__SELECTION__) == TIM_TS_ITR1) || \
                                                 ((__SELECTION__) == TIM_TS_ITR2) || \
                                                 ((__SELECTION__) == TIM_TS_ITR3) || \
                                                 ((__SELECTION__) == TIM_TS_TI1F_ED) || \
                                                 ((__SELECTION__) == TIM_TS_TI1FP1) || \
                                                 ((__SELECTION__) == TIM_TS_TI2FP2) || \
                                                 ((__SELECTION__) == TIM_TS_ETRF))

#define IS_TIM_INTERNAL_TRIGGER_SELECTION(SELECTION) (((SELECTION) == TIM_TS_ITR0) || \
                                                      ((SELECTION) == TIM_TS_ITR1) || \
                                                      ((SELECTION) == TIM_TS_ITR2) || \
                                                      ((SELECTION) == TIM_TS_ITR3))

#define IS_TIM_INTERNAL_TRIGGEREVENT_SELECTION(__SELECTION__) (((__SELECTION__) == TIM_TS_ITR0) || \
                                                               ((__SELECTION__) == TIM_TS_ITR1) || \
                                                               ((__SELECTION__) == TIM_TS_ITR2) || \
                                                               ((__SELECTION__) == TIM_TS_ITR3) || \
                                                               ((__SELECTION__) == TIM_TS_NONE))

#define IS_TIM_TRIGGERPOLARITY(__POLARITY__)     (((__POLARITY__) == TIM_TRIGGERPOLARITY_INVERTED   ) || \
                                                  ((__POLARITY__) == TIM_TRIGGERPOLARITY_NONINVERTED) || \
                                                  ((__POLARITY__) == TIM_TRIGGERPOLARITY_RISING     ) || \
                                                  ((__POLARITY__) == TIM_TRIGGERPOLARITY_FALLING    ) || \
                                                  ((__POLARITY__) == TIM_TRIGGERPOLARITY_BOTHEDGE   ))

#define IS_TIM_TRIGGERPRESCALER(__PRESCALER__)  (((__PRESCALER__) == TIM_TRIGGERPRESCALER_DIV1) || \
                                                 ((__PRESCALER__) == TIM_TRIGGERPRESCALER_DIV2) || \
                                                 ((__PRESCALER__) == TIM_TRIGGERPRESCALER_DIV4) || \
                                                 ((__PRESCALER__) == TIM_TRIGGERPRESCALER_DIV8))

#define IS_TIM_TRIGGERFILTER(__ICFILTER__)     ((__ICFILTER__) <= 0xF)

#define IS_TIM_TI1SELECTION(__TI1SELECTION__)   (((__TI1SELECTION__) == TIM_TI1SELECTION_CH1) || \
                                                 ((__TI1SELECTION__) == TIM_TI1SELECTION_XORCOMBINATION))

#define IS_TIM_DMA_BASE(__BASE__) (((__BASE__) == TIM_DMABASE_CR1) || \
                                   ((__BASE__) == TIM_DMABASE_CR2) || \
                                   ((__BASE__) == TIM_DMABASE_SMCR) || \
                                   ((__BASE__) == TIM_DMABASE_DIER) || \
                                   ((__BASE__) == TIM_DMABASE_SR) || \
                                   ((__BASE__) == TIM_DMABASE_EGR) || \
                                   ((__BASE__) == TIM_DMABASE_CCMR1) || \
                                   ((__BASE__) == TIM_DMABASE_CCMR2) || \
                                   ((__BASE__) == TIM_DMABASE_CCER) || \
                                   ((__BASE__) == TIM_DMABASE_CNT) || \
                                   ((__BASE__) == TIM_DMABASE_PSC) || \
                                   ((__BASE__) == TIM_DMABASE_ARR) || \
                                   ((__BASE__) == TIM_DMABASE_RCR) || \
                                   ((__BASE__) == TIM_DMABASE_CCR1) || \
                                   ((__BASE__) == TIM_DMABASE_CCR2) || \
                                   ((__BASE__) == TIM_DMABASE_CCR3) || \
                                   ((__BASE__) == TIM_DMABASE_CCR4) || \
                                   ((__BASE__) == TIM_DMABASE_BDTR) || \
                                   ((__BASE__) == TIM_DMABASE_DCR) || \
                                   ((__BASE__) == TIM_DMABASE_OR))

#define IS_TIM_DMA_LENGTH(__LENGTH__) (((__LENGTH__) == TIM_DMABURSTLENGTH_1TRANSFER) || \
                                       ((__LENGTH__) == TIM_DMABURSTLENGTH_2TRANSFERS) || \
                                       ((__LENGTH__) == TIM_DMABURSTLENGTH_3TRANSFERS) || \
                                       ((__LENGTH__) == TIM_DMABURSTLENGTH_4TRANSFERS) || \
                                       ((__LENGTH__) == TIM_DMABURSTLENGTH_5TRANSFERS) || \
                                       ((__LENGTH__) == TIM_DMABURSTLENGTH_6TRANSFERS) || \
                                       ((__LENGTH__) == TIM_DMABURSTLENGTH_7TRANSFERS) || \
                                       ((__LENGTH__) == TIM_DMABURSTLENGTH_8TRANSFERS) || \
                                       ((__LENGTH__) == TIM_DMABURSTLENGTH_9TRANSFERS) || \
                                       ((__LENGTH__) == TIM_DMABURSTLENGTH_10TRANSFERS) || \
                                       ((__LENGTH__) == TIM_DMABURSTLENGTH_11TRANSFERS) || \
                                       ((__LENGTH__) == TIM_DMABURSTLENGTH_12TRANSFERS) || \
                                       ((__LENGTH__) == TIM_DMABURSTLENGTH_13TRANSFERS) || \
                                       ((__LENGTH__) == TIM_DMABURSTLENGTH_14TRANSFERS) || \
                                       ((__LENGTH__) == TIM_DMABURSTLENGTH_15TRANSFERS) || \
                                       ((__LENGTH__) == TIM_DMABURSTLENGTH_16TRANSFERS) || \
                                       ((__LENGTH__) == TIM_DMABURSTLENGTH_17TRANSFERS) || \
                                       ((__LENGTH__) == TIM_DMABURSTLENGTH_18TRANSFERS))

#define IS_TIM_IC_FILTER(ICFILTER) ((ICFILTER) <= 0xF)


/**
  * @}
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup TIM_Private_Functions TIM Private Functions
  * @{
  */
void TIM_Base_SetConfig(TIM_TypeDef *TIMx, TIM_Base_InitTypeDef *Structure);
void TIM_TI1_SetConfig(TIM_TypeDef *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICSelection, uint32_t TIM_ICFilter);
void TIM_OC1_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config);
void TIM_OC2_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config);
void TIM_OC3_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config);
void TIM_OC4_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config);
void TIM_ETR_SetConfig(TIM_TypeDef* TIMx, uint32_t TIM_ExtTRGPrescaler, uint32_t TIM_ExtTRGPolarity, uint32_t ExtTRGFilter);

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

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F7xx_HAL_TIM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
