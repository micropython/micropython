/**
  ******************************************************************************
  * @file    stm32n6xx_hal_lptim.h
  * @author  MCD Application Team
  * @brief   Header file of LPTIM HAL module.
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
#ifndef STM32N6xx_HAL_LPTIM_H
#define STM32N6xx_HAL_LPTIM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/* Include low level driver */
#include "stm32n6xx_ll_lptim.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

#if defined (LPTIM1) || defined (LPTIM2) || defined (LPTIM3) || defined (LPTIM4) || defined (LPTIM5)

/** @addtogroup LPTIM
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup LPTIM_Exported_Types LPTIM Exported Types
  * @{
  */
#define LPTIM_EXTI_LINE_LPTIM1  EXTI_IMR2_IM52  /*!< External interrupt line 52 Connected to the LPTIM1 EXTI Line */
#define LPTIM_EXTI_LINE_LPTIM2  EXTI_IMR2_IM53  /*!< External interrupt line 53 Connected to the LPTIM2 EXTI Line */
#define LPTIM_EXTI_LINE_LPTIM3  EXTI_IMR2_IM55  /*!< External interrupt line 55 Connected to the LPTIM3 EXTI Line */
#define LPTIM_EXTI_LINE_LPTIM4  EXTI_IMR2_IM57  /*!< External interrupt line 57 Connected to the LPTIM4 EXTI Line */
#define LPTIM_EXTI_LINE_LPTIM5  EXTI_IMR2_IM58  /*!< External interrupt line 58 Connected to the LPTIM5 EXTI Line */

/**
  * @brief  LPTIM Clock configuration definition
  */
typedef struct
{
  uint32_t Source;         /*!< Selects the clock source.
                           This parameter can be a value of @ref LPTIM_Clock_Source   */

  uint32_t Prescaler;      /*!< Specifies the counter clock Prescaler.
                           This parameter can be a value of @ref LPTIM_Clock_Prescaler */

} LPTIM_ClockConfigTypeDef;

/**
  * @brief  LPTIM Clock configuration definition
  */
typedef struct
{
  uint32_t Polarity;      /*!< Selects the polarity of the active edge for the counter unit
                           if the ULPTIM input is selected.
                           Note: This parameter is used only when Ultra low power clock source is used.
                           Note: If the polarity is configured on 'both edges', an auxiliary clock
                           (one of the Low power oscillator) must be active.
                           This parameter can be a value of @ref LPTIM_Clock_Polarity */

  uint32_t SampleTime;     /*!< Selects the clock sampling time to configure the clock glitch filter.
                           Note: This parameter is used only when Ultra low power clock source is used.
                           This parameter can be a value of @ref LPTIM_Clock_Sample_Time */

} LPTIM_ULPClockConfigTypeDef;

/**
  * @brief  LPTIM Trigger configuration definition
  */
typedef struct
{
  uint32_t Source;        /*!< Selects the Trigger source.
                          This parameter can be a value of @ref LPTIM_Trigger_Source */

  uint32_t ActiveEdge;    /*!< Selects the Trigger active edge.
                          Note: This parameter is used only when an external trigger is used.
                          This parameter can be a value of @ref LPTIM_External_Trigger_Polarity */

  uint32_t SampleTime;    /*!< Selects the trigger sampling time to configure the clock glitch filter.
                          Note: This parameter is used only when an external trigger is used.
                          This parameter can be a value of @ref LPTIM_Trigger_Sample_Time  */
} LPTIM_TriggerConfigTypeDef;

/**
  * @brief  LPTIM Initialization Structure definition
  */
typedef struct
{
  LPTIM_ClockConfigTypeDef     Clock;             /*!< Specifies the clock parameters */

  LPTIM_ULPClockConfigTypeDef  UltraLowPowerClock;/*!< Specifies the Ultra Low Power clock parameters */

  LPTIM_TriggerConfigTypeDef   Trigger;           /*!< Specifies the Trigger parameters */

  uint32_t                     Period;            /*!< Specifies the period value to be loaded into the active
                                                  Auto-Reload Register at the next update event.
                                                  This parameter can be a number between
                                                  Min_Data = 0x0001 and Max_Data = 0xFFFF.  */

  uint32_t                     UpdateMode;        /*!< Specifies whether the update of the autoreload and the compare
                                                  values is done immediately or after the end of current period.
                                                  This parameter can be a value of @ref LPTIM_Updating_Mode */

  uint32_t                     CounterSource;     /*!< Specifies whether the counter is incremented each internal event
                                                  or each external event.
                                                  This parameter can be a value of @ref LPTIM_Counter_Source */

  uint32_t                     Input1Source;      /*!< Specifies source selected for input1 (GPIO or comparator output).
                                                  This parameter can be a value of @ref LPTIM_Input1_Source */

  uint32_t                     Input2Source;      /*!< Specifies source selected for input2 (GPIO or comparator output).
                                                  Note: This parameter is used only for encoder feature so is used only
                                                  for LPTIM1 instance.
                                                  This parameter can be a value of @ref LPTIM_Input2_Source */

  uint32_t                     RepetitionCounter;/*!< Specifies the repetition counter value.
                                                  Each time the RCR downcounter reaches zero, an update event is
                                                  generated and counting restarts from the RCR value (N).
                                                  Note: When using repetition counter the UpdateMode field must be
                                                        set to LPTIM_UPDATE_ENDOFPERIOD otherwise unpredictable
                                                        behavior may occur.
                                                  This parameter must be a number between Min_Data = 0x00 and
                                                  Max_Data = 0xFF. */
} LPTIM_InitTypeDef;

/**
  * @brief  LPTIM Output Compare Configuration Structure definition
  */
typedef struct
{
  uint32_t Pulse;                 /*!< Specifies the pulse value to be loaded into the Capture Compare Register.
                                       This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF */
  uint32_t OCPolarity;            /*!< Specifies the output polarity.
                                       This parameter can be a value of @ref LPTIM_Output_Compare_Polarity */
} LPTIM_OC_ConfigTypeDef;

/**
  * @brief  LPTIM Input Capture Configuration Structure definition
  */
typedef struct
{
  uint32_t ICInputSource; /*!< Specifies source selected for IC channel.
                               This parameter can be a value of @ref LPTIM_Input_Capture_Source */

  uint32_t ICPrescaler;   /*!< Specifies the Input Capture Prescaler.
                               This parameter can be a value of @ref LPTIM_Input_Capture_Prescaler */

  uint32_t ICPolarity;    /*!< Specifies the active edge of the input signal.
                               This parameter can be a value of @ref LPTIM_Input_Capture_Polarity */

  uint32_t ICFilter;      /*!< Specifies the input capture filter.
                               This parameter can be a value of @ref LPTIM_Input_Capture_Filter */
} LPTIM_IC_ConfigTypeDef;

/**
  * @brief  HAL LPTIM State structure definition
  */
typedef enum
{
  HAL_LPTIM_STATE_RESET            = 0x00U,    /*!< Peripheral not yet initialized or disabled  */
  HAL_LPTIM_STATE_READY            = 0x01U,    /*!< Peripheral Initialized and ready for use    */
  HAL_LPTIM_STATE_BUSY             = 0x02U,    /*!< An internal process is ongoing              */
  HAL_LPTIM_STATE_TIMEOUT          = 0x03U,    /*!< Timeout state                               */
  HAL_LPTIM_STATE_ERROR            = 0x04U     /*!< Internal Process is ongoing                 */
} HAL_LPTIM_StateTypeDef;

/**
  * @brief  HAL Active channel structures definition
  */
typedef enum
{
  HAL_LPTIM_ACTIVE_CHANNEL_1        = 0x01U,    /*!< The active channel is 1     */
  HAL_LPTIM_ACTIVE_CHANNEL_2        = 0x02U,    /*!< The active channel is 2     */
  HAL_LPTIM_ACTIVE_CHANNEL_CLEARED  = 0x00U     /*!< All active channels cleared */
} HAL_LPTIM_ActiveChannel;

/**
  * @brief  LPTIM Channel States definition
  */
typedef enum
{
  HAL_LPTIM_CHANNEL_STATE_RESET             = 0x00U,    /*!< LPTIM Channel initial state                         */
  HAL_LPTIM_CHANNEL_STATE_READY             = 0x01U,    /*!< LPTIM Channel ready for use                         */
  HAL_LPTIM_CHANNEL_STATE_BUSY              = 0x02U,    /*!< An internal process is ongoing on the LPTIM channel */
} HAL_LPTIM_ChannelStateTypeDef;

/**
  * @brief  LPTIM handle Structure definition
  */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
typedef struct __LPTIM_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
{
  LPTIM_TypeDef                 *Instance;         /*!< Register base address     */

  LPTIM_InitTypeDef              Init;             /*!< LPTIM required parameters */

  HAL_LPTIM_ActiveChannel        Channel;          /*!< Active channel            */

  DMA_HandleTypeDef             *hdma[3];          /*!< DMA Handlers array, This array is accessed by a @ref LPTIM_DMA_Handle_index */

  HAL_StatusTypeDef              Status;           /*!< LPTIM peripheral status   */

  HAL_LockTypeDef                Lock;             /*!< LPTIM locking object      */

  __IO  HAL_LPTIM_StateTypeDef   State;            /*!< LPTIM peripheral state    */

  __IO  HAL_LPTIM_ChannelStateTypeDef   ChannelState[2];  /*!< LPTIM channel operation state                       */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
  void (* MspInitCallback)(struct __LPTIM_HandleTypeDef *hlptim);            /*!< LPTIM Base Msp Init Callback                 */
  void (* MspDeInitCallback)(struct __LPTIM_HandleTypeDef *hlptim);          /*!< LPTIM Base Msp DeInit Callback               */
  void (* CompareMatchCallback)(struct __LPTIM_HandleTypeDef *hlptim);       /*!< Compare match Callback                       */
  void (* AutoReloadMatchCallback)(struct __LPTIM_HandleTypeDef *hlptim);    /*!< Auto-reload match Callback                   */
  void (* TriggerCallback)(struct __LPTIM_HandleTypeDef *hlptim);            /*!< External trigger event detection Callback    */
  void (* CompareWriteCallback)(struct __LPTIM_HandleTypeDef *hlptim);       /*!< Compare register write complete Callback     */
  void (* AutoReloadWriteCallback)(struct __LPTIM_HandleTypeDef *hlptim);    /*!< Auto-reload register write complete Callback */
  void (* DirectionUpCallback)(struct __LPTIM_HandleTypeDef *hlptim);        /*!< Up-counting direction change Callback        */
  void (* DirectionDownCallback)(struct __LPTIM_HandleTypeDef *hlptim);      /*!< Down-counting direction change Callback      */
  void (* UpdateEventCallback)(struct __LPTIM_HandleTypeDef *hlptim);        /*!< Update event detection Callback              */
  void (* RepCounterWriteCallback)(struct __LPTIM_HandleTypeDef *hlptim);    /*!< Repetition counter register write complete Callback */
  void (* UpdateEventHalfCpltCallback)(struct __LPTIM_HandleTypeDef *hlptim);/*!< Update event half complete detection Callback */
  void (* ErrorCallback)(struct __LPTIM_HandleTypeDef *hlptim);              /*!< LPTIM Error Callback                         */
  void (* IC_CaptureCallback)(struct __LPTIM_HandleTypeDef *hlptim);         /*!< Input capture Callback                       */
  void (* IC_CaptureHalfCpltCallback)(struct __LPTIM_HandleTypeDef *htim);   /*!< Input Capture half complete Callback         */
  void (* IC_OverCaptureCallback)(struct __LPTIM_HandleTypeDef *hlptim);     /*!< Over capture Callback                        */
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
} LPTIM_HandleTypeDef;

#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
/**
  * @brief  HAL LPTIM Callback ID enumeration definition
  */
typedef enum
{
  HAL_LPTIM_MSPINIT_CB_ID          = 0x00U,    /*!< LPTIM Base Msp Init Callback ID                  */
  HAL_LPTIM_MSPDEINIT_CB_ID        = 0x01U,    /*!< LPTIM Base Msp DeInit Callback ID                */
  HAL_LPTIM_COMPARE_MATCH_CB_ID    = 0x02U,    /*!< Compare match Callback ID                        */
  HAL_LPTIM_AUTORELOAD_MATCH_CB_ID = 0x03U,    /*!< Auto-reload match Callback ID                    */
  HAL_LPTIM_TRIGGER_CB_ID          = 0x04U,    /*!< External trigger event detection Callback ID     */
  HAL_LPTIM_COMPARE_WRITE_CB_ID    = 0x05U,    /*!< Compare register write complete Callback ID      */
  HAL_LPTIM_AUTORELOAD_WRITE_CB_ID = 0x06U,    /*!< Auto-reload register write complete Callback ID  */
  HAL_LPTIM_DIRECTION_UP_CB_ID     = 0x07U,    /*!< Up-counting direction change Callback ID         */
  HAL_LPTIM_DIRECTION_DOWN_CB_ID   = 0x08U,    /*!< Down-counting direction change Callback ID       */
  HAL_LPTIM_UPDATE_EVENT_CB_ID      = 0x09U,   /*!< Update event detection Callback ID               */
  HAL_LPTIM_REP_COUNTER_WRITE_CB_ID = 0x0AU,   /*!< Repetition counter register write complete Callback ID */
  HAL_LPTIM_UPDATE_EVENT_HALF_CB_ID = 0x0BU,   /*!< Update event half complete detection Callback ID */
  HAL_LPTIM_ERROR_CB_ID             = 0x0CU,   /*!< LPTIM Error Callback ID                          */
  HAL_LPTIM_IC_CAPTURE_CB_ID        = 0x0DU,   /*!< Input capture Callback ID                        */
  HAL_LPTIM_IC_CAPTURE_HALF_CB_ID   = 0x0EU,   /*!< Input capture half complete Callback ID          */
  HAL_LPTIM_OVER_CAPTURE_CB_ID      = 0x0FU,   /*!< Over capture Callback ID                         */
} HAL_LPTIM_CallbackIDTypeDef;

/**
  * @brief  HAL TIM Callback pointer definition
  */
typedef  void (*pLPTIM_CallbackTypeDef)(LPTIM_HandleTypeDef *hlptim);  /*!< pointer to the LPTIM callback function */

#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup LPTIM_Exported_Constants LPTIM Exported Constants
  * @{
  */

/** @defgroup LPTIM_Clock_Source LPTIM Clock Source
  * @{
  */
#define LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC        0x00000000U
#define LPTIM_CLOCKSOURCE_ULPTIM                LPTIM_CFGR_CKSEL
/**
  * @}
  */

/** @defgroup LPTIM_Clock_Prescaler LPTIM Clock Prescaler
  * @{
  */
#define LPTIM_PRESCALER_DIV1                    0x00000000U
#define LPTIM_PRESCALER_DIV2                    LPTIM_CFGR_PRESC_0
#define LPTIM_PRESCALER_DIV4                    LPTIM_CFGR_PRESC_1
#define LPTIM_PRESCALER_DIV8                    (LPTIM_CFGR_PRESC_0 | LPTIM_CFGR_PRESC_1)
#define LPTIM_PRESCALER_DIV16                   LPTIM_CFGR_PRESC_2
#define LPTIM_PRESCALER_DIV32                   (LPTIM_CFGR_PRESC_0 | LPTIM_CFGR_PRESC_2)
#define LPTIM_PRESCALER_DIV64                   (LPTIM_CFGR_PRESC_1 | LPTIM_CFGR_PRESC_2)
#define LPTIM_PRESCALER_DIV128                  LPTIM_CFGR_PRESC
/**
  * @}
  */

/** @defgroup LPTIM_Clock_Sample_Time LPTIM Clock Sample Time
  * @{
  */
#define LPTIM_CLOCKSAMPLETIME_DIRECTTRANSITION  0x00000000U
#define LPTIM_CLOCKSAMPLETIME_2TRANSITIONS      LPTIM_CFGR_CKFLT_0
#define LPTIM_CLOCKSAMPLETIME_4TRANSITIONS      LPTIM_CFGR_CKFLT_1
#define LPTIM_CLOCKSAMPLETIME_8TRANSITIONS      LPTIM_CFGR_CKFLT
/**
  * @}
  */

/** @defgroup LPTIM_Clock_Polarity LPTIM Clock Polarity
  * @{
  */
#define LPTIM_CLOCKPOLARITY_RISING              0x00000000U
#define LPTIM_CLOCKPOLARITY_FALLING             LPTIM_CFGR_CKPOL_0
#define LPTIM_CLOCKPOLARITY_RISING_FALLING      LPTIM_CFGR_CKPOL_1
/**
  * @}
  */

/** @defgroup LPTIM_Trigger_Source LPTIM Trigger Source
  * @{
  */
#define LPTIM_TRIGSOURCE_SOFTWARE               0x0000FFFFU
#define LPTIM_TRIGSOURCE_0                      0x00000000U
#define LPTIM_TRIGSOURCE_1                      LPTIM_CFGR_TRIGSEL_0
#define LPTIM_TRIGSOURCE_2                      LPTIM_CFGR_TRIGSEL_1
#define LPTIM_TRIGSOURCE_3                      (LPTIM_CFGR_TRIGSEL_0 | LPTIM_CFGR_TRIGSEL_1)
#define LPTIM_TRIGSOURCE_4                      LPTIM_CFGR_TRIGSEL_2
#define LPTIM_TRIGSOURCE_5                      (LPTIM_CFGR_TRIGSEL_0 | LPTIM_CFGR_TRIGSEL_2)
#define LPTIM_TRIGSOURCE_6                      (LPTIM_CFGR_TRIGSEL_1 | LPTIM_CFGR_TRIGSEL_2)
#define LPTIM_TRIGSOURCE_7                      LPTIM_CFGR_TRIGSEL
/**
  * @}
  */

/** @defgroup LPTIM_External_Trigger_Polarity LPTIM External Trigger Polarity
  * @{
  */
#define LPTIM_ACTIVEEDGE_RISING                LPTIM_CFGR_TRIGEN_0
#define LPTIM_ACTIVEEDGE_FALLING               LPTIM_CFGR_TRIGEN_1
#define LPTIM_ACTIVEEDGE_RISING_FALLING        LPTIM_CFGR_TRIGEN
/**
  * @}
  */

/** @defgroup LPTIM_Trigger_Sample_Time LPTIM Trigger Sample Time
  * @{
  */
#define LPTIM_TRIGSAMPLETIME_DIRECTTRANSITION  0x00000000U
#define LPTIM_TRIGSAMPLETIME_2TRANSITIONS      LPTIM_CFGR_TRGFLT_0
#define LPTIM_TRIGSAMPLETIME_4TRANSITIONS      LPTIM_CFGR_TRGFLT_1
#define LPTIM_TRIGSAMPLETIME_8TRANSITIONS      LPTIM_CFGR_TRGFLT
/**
  * @}
  */

/** @defgroup LPTIM_Updating_Mode LPTIM Updating Mode
  * @{
  */

#define LPTIM_UPDATE_IMMEDIATE                  0x00000000U
#define LPTIM_UPDATE_ENDOFPERIOD                LPTIM_CFGR_PRELOAD
/**
  * @}
  */

/** @defgroup LPTIM_Counter_Source LPTIM Counter Source
  * @{
  */

#define LPTIM_COUNTERSOURCE_INTERNAL            0x00000000U
#define LPTIM_COUNTERSOURCE_EXTERNAL            LPTIM_CFGR_COUNTMODE
/**
  * @}
  */

/** @defgroup LPTIM_Input1_Source LPTIM Input1 Source
  * @{
  */

#define LPTIM_INPUT1SOURCE_GPIO         0x00000000UL          /*!< For LPTIM1 and LPTIM2 */
/**
  * @}
  */

/** @defgroup LPTIM_Input2_Source LPTIM Input2 Source
  * @{
  */

#define LPTIM_INPUT2SOURCE_GPIO         0x00000000UL          /*!< For LPTIM1 and LPTIM2 */
/**
  * @}
  */

/** @defgroup LPTIM_Flag_Definition LPTIM Flags Definition
  * @{
  */

#define LPTIM_FLAG_CC1O                          LPTIM_ISR_CC1OF
#define LPTIM_FLAG_CC2O                          LPTIM_ISR_CC2OF
#define LPTIM_FLAG_CC1                           LPTIM_ISR_CC1IF
#define LPTIM_FLAG_CC2                           LPTIM_ISR_CC2IF
#define LPTIM_FLAG_CMP1OK                        LPTIM_ISR_CMP1OK
#define LPTIM_FLAG_CMP2OK                        LPTIM_ISR_CMP2OK
#define LPTIM_FLAG_DIEROK                        LPTIM_ISR_DIEROK
#define LPTIM_FLAG_REPOK                         LPTIM_ISR_REPOK
#define LPTIM_FLAG_UPDATE                        LPTIM_ISR_UE
#define LPTIM_FLAG_DOWN                          LPTIM_ISR_DOWN
#define LPTIM_FLAG_UP                            LPTIM_ISR_UP
#define LPTIM_FLAG_ARROK                         LPTIM_ISR_ARROK
#define LPTIM_FLAG_EXTTRIG                       LPTIM_ISR_EXTTRIG
#define LPTIM_FLAG_ARRM                          LPTIM_ISR_ARRM
/**
  * @}
  */

/** @defgroup LPTIM_DMA_sources LPTIM DMA Sources
  * @{
  */
#define LPTIM_DMA_UPDATE                     LPTIM_DIER_UEDE         /*!< DMA request is triggered by the update event */
#define LPTIM_DMA_CC1                        LPTIM_DIER_CC1DE        /*!< DMA request is triggered by the capture 1 event */
#define LPTIM_DMA_CC2                        LPTIM_DIER_CC2DE        /*!< DMA request is triggered by the capture 2 event */

/**
  * @}
  */

/** @defgroup LPTIM_DMA_Handle_index LPTIM DMA Handle Index
  * @{
  */
#define LPTIM_DMA_ID_UPDATE                ((uint16_t) 0x0000)       /*!< Index of the DMA handle used for Update DMA requests */
#define LPTIM_DMA_ID_CC1                   ((uint16_t) 0x0001)       /*!< Index of the DMA handle used for Capture/Update event 1 DMA request */
#define LPTIM_DMA_ID_CC2                   ((uint16_t) 0x0002)       /*!< Index of the DMA handle used for Capture/Update event 2 DMA request */
/**
  * @}
  */

/** @defgroup LPTIM_Interrupts_Definition LPTIM Interrupts Definition
  * @{
  */
#define LPTIM_IT_CC1O                            LPTIM_DIER_CC1OIE
#define LPTIM_IT_CC2O                            LPTIM_DIER_CC2OIE
#define LPTIM_IT_CC1                             LPTIM_DIER_CC1IE
#define LPTIM_IT_CC2                             LPTIM_DIER_CC2IE
#define LPTIM_IT_CMP1OK                          LPTIM_DIER_CMP1OKIE
#define LPTIM_IT_CMP2OK                          LPTIM_DIER_CMP2OKIE
#define LPTIM_IT_REPOK                           LPTIM_DIER_REPOKIE
#define LPTIM_IT_UPDATE                          LPTIM_DIER_UEIE
#define LPTIM_IT_DOWN                            LPTIM_DIER_DOWNIE
#define LPTIM_IT_UP                              LPTIM_DIER_UPIE
#define LPTIM_IT_ARROK                           LPTIM_DIER_ARROKIE
#define LPTIM_IT_EXTTRIG                         LPTIM_DIER_EXTTRIGIE
#define LPTIM_IT_ARRM                            LPTIM_DIER_ARRMIE
/**
  * @}
  */

/** @defgroup LPTIM_Channel LPTIM Channel
  * @{
  */
#define LPTIM_CHANNEL_1                      LL_LPTIM_CHANNEL_CH1                          /*!< Capture/compare channel 1 identifier      */
#define LPTIM_CHANNEL_2                      LL_LPTIM_CHANNEL_CH2                          /*!< Capture/compare channel 2 identifier      */
/**
  * @}
  */

/** @defgroup LPTIM_Output_Compare_Polarity LPTIM Output Compare Polarity
  * @{
  */
#define LPTIM_OCPOLARITY_HIGH                    0x00000000U                    /*!< Capture/Compare output polarity */
#define LPTIM_OCPOLARITY_LOW                     0x00000001U                    /*!< Capture/Compare output polarity */
/**
  * @}
  */

/** @defgroup LPTIM_Input_Capture_Prescaler LPTIM Input Capture Prescaler
  * @{
  */
#define LPTIM_ICPSC_DIV1                          0x00000000UL                                  /*!< Capture performed each time an edge is detected on the capture input */
#define LPTIM_ICPSC_DIV2                          LPTIM_CCMR1_IC1PSC_0                          /*!< Capture performed once every 2 events                                */
#define LPTIM_ICPSC_DIV4                          LPTIM_CCMR1_IC1PSC_1                          /*!< Capture performed once every 4 events                                */
#define LPTIM_ICPSC_DIV8                         (LPTIM_CCMR1_IC1PSC_0|LPTIM_CCMR1_IC1PSC_1)    /*!< Capture performed once every 8 events                                */
/**
  * @}
  */

/** @defgroup LPTIM_Input_Capture_Polarity LPTIM Input Capture Polarity
  * @{
  */
#define LPTIM_ICPOLARITY_RISING                    0x00000000UL                              /*!< Capture/Compare input rising polarity */
#define LPTIM_ICPOLARITY_FALLING                   LPTIM_CCMR1_CC1P_0                        /*!< Capture/Compare input falling polarity */
#define LPTIM_ICPOLARITY_RISING_FALLING           (LPTIM_CCMR1_CC1P_0|LPTIM_CCMR1_CC1P_1)    /*!< Capture/Compare input rising and falling polarities */
/**
  * @}
  */

/** @defgroup LPTIM_Input_Capture_Filter LPTIM Input Capture Filter
  * @{
  */
#define LPTIM_ICFLT_CLOCK_DIV1                     0x00000000UL                            /*!< any external input capture signal level change is considered as a valid transition */
#define LPTIM_ICFLT_CLOCK_DIV2                     LPTIM_CCMR1_IC1F_0                      /*!< external input capture signal level change must be stable for at least 2 clock periods before it is considered as valid transition */
#define LPTIM_ICFLT_CLOCK_DIV4                     LPTIM_CCMR1_IC1F_1                      /*!< external input capture signal level change must be stable for at least 4 clock periods before it is considered as valid transition */
#define LPTIM_ICFLT_CLOCK_DIV8                    (LPTIM_CCMR1_IC1F_0|LPTIM_CCMR1_IC1F_1)  /*!< external input capture signal level change must be stable for at least 8 clock periods before it is considered as valid transition */
/**
  * @}
  */

/** @defgroup LPTIM_Input_Capture_Source LPTIM Input Capture Source
  * @{
  */
#define LPTIM_IC1SOURCE_GPIO                       0x00000000UL                                  /*!< For LPTIM1, LPTIM2 and LPTIM3 */
#define LPTIM_IC1SOURCE_I3C1_IBIACK                LPTIM_CFGR2_IC1SEL_0                          /*!< For LPTIM2 and LPTIM3 */
#define LPTIM_IC2SOURCE_GPIO                       0x00000000UL                                  /*!< For LPTIM1, LPTIM2 and LPTIM3 */
#define LPTIM_IC2SOURCE_LSI                        LPTIM_CFGR2_IC2SEL_0                          /*!< For LPTIM1 */
#define LPTIM_IC2SOURCE_LSE                        LPTIM_CFGR2_IC2SEL_1                          /*!< For LPTIM1 */
#define LPTIM_IC2SOURCE_HSI_1024                   LPTIM_CFGR2_IC2SEL_0                          /*!< For LPTIM2 */
#define LPTIM_IC2SOURCE_MSI_128                    LPTIM_CFGR2_IC2SEL_1                          /*!< For LPTIM2 */
#define LPTIM_IC2SOURCE_I3C2_IBIACK                (LPTIM_CFGR2_IC2SEL_1 | LPTIM_CFGR2_IC2SEL_0) /*!< For LPTIM2 and LPTIM3 */
/**
  * @}
  */
/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup LPTIM_Exported_Macros LPTIM Exported Macros
  * @{
  */

/** @brief Reset LPTIM handle state.
  * @param  __HANDLE__ LPTIM handle
  * @retval None
  */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
#define __HAL_LPTIM_RESET_HANDLE_STATE(__HANDLE__) do {                                                        \
                                                        (__HANDLE__)->State             = HAL_LPTIM_STATE_RESET; \
                                                        (__HANDLE__)->ChannelState[0]  = HAL_LPTIM_CHANNEL_STATE_RESET;\
                                                        (__HANDLE__)->ChannelState[1]  = HAL_LPTIM_CHANNEL_STATE_RESET;\
                                                        (__HANDLE__)->MspInitCallback   = NULL;                  \
                                                        (__HANDLE__)->MspDeInitCallback = NULL;                  \
                                                      } while(0)
#else
#define __HAL_LPTIM_RESET_HANDLE_STATE(__HANDLE__) do {                                                              \
                                                        (__HANDLE__)->State            = HAL_LPTIM_STATE_RESET;        \
                                                        (__HANDLE__)->ChannelState[0]  = HAL_LPTIM_CHANNEL_STATE_RESET;\
                                                        (__HANDLE__)->ChannelState[1]  = HAL_LPTIM_CHANNEL_STATE_RESET;\
                                                      } while(0)
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */

/**
  * @brief  Enable the LPTIM peripheral.
  * @param  __HANDLE__ LPTIM handle
  * @retval None
  */
#define __HAL_LPTIM_ENABLE(__HANDLE__)   ((__HANDLE__)->Instance->CR |= (LPTIM_CR_ENABLE))

/**
  * @brief  Disable the LPTIM peripheral.
  * @param  __HANDLE__ LPTIM handle
  * @retval None
  */
#define __HAL_LPTIM_DISABLE(__HANDLE__) \
  do { \
    if (((__HANDLE__)->Instance->CCMR1 & LPTIM_CCMR1_CC1E) == 0UL) \
    { \
      if(((__HANDLE__)->Instance->CCMR1 & LPTIM_CCMR1_CC2E) == 0UL) \
      { \
        (__HANDLE__)->Instance->CR &= ~(LPTIM_CR_ENABLE); \
      } \
    } \
  } while(0)

/**
  * @brief  Start the LPTIM peripheral in Continuous mode.
  * @param  __HANDLE__ LPTIM handle
  * @retval None
  */
#define __HAL_LPTIM_START_CONTINUOUS(__HANDLE__)  ((__HANDLE__)->Instance->CR |=  LPTIM_CR_CNTSTRT)
/**
  * @brief  Start the LPTIM peripheral in single mode.
  * @param  __HANDLE__ LPTIM handle
  * @retval None
  */
#define __HAL_LPTIM_START_SINGLE(__HANDLE__)      ((__HANDLE__)->Instance->CR |=  LPTIM_CR_SNGSTRT)

/**
  * @brief  Reset the LPTIM Counter register in synchronous mode.
  * @param  __HANDLE__ LPTIM handle
  * @retval None
  */
#define __HAL_LPTIM_RESET_COUNTER(__HANDLE__)      ((__HANDLE__)->Instance->CR |=  LPTIM_CR_COUNTRST)

/**
  * @brief  Reset after read of the LPTIM Counter register in asynchronous mode.
  * @param  __HANDLE__ LPTIM handle
  * @retval None
  */
#define __HAL_LPTIM_RESET_COUNTER_AFTERREAD(__HANDLE__)      ((__HANDLE__)->Instance->CR |=  LPTIM_CR_RSTARE)

/**
  * @brief  Write the passed parameter in the Autoreload register.
  * @param  __HANDLE__ LPTIM handle
  * @param  __VALUE__ Autoreload value
  * @retval None
  * @note   The ARR register can only be modified when the LPTIM instance is enabled.
  */
#define __HAL_LPTIM_AUTORELOAD_SET(__HANDLE__ , __VALUE__)  ((__HANDLE__)->Instance->ARR =  (__VALUE__))

/**
  * @brief  Write the passed parameter in the Compare register.
  * @param  __HANDLE__ LPTIM handle
  * @param  __VALUE__ Compare value
  * @param  __CHANNEL__ TIM Channel to be configured
  * @retval None
  * @note   The CCRx registers can only be modified when the LPTIM instance is enabled.
  */
#define __HAL_LPTIM_COMPARE_SET(__HANDLE__ , __CHANNEL__, __VALUE__) \
  (((__CHANNEL__) == LPTIM_CHANNEL_1) ? ((__HANDLE__)->Instance->CCR1 = (__VALUE__)) :\
   ((__CHANNEL__) == LPTIM_CHANNEL_2) ? ((__HANDLE__)->Instance->CCR2 = (__VALUE__)) : 0U)

/**
  * @brief  Write the passed parameter in the Repetition register.
  * @param  __HANDLE__ LPTIM handle
  * @param  __VALUE__ Repetition value
  * @retval None
  */
#define __HAL_LPTIM_REPETITIONCOUNTER_SET(__HANDLE__ , __VALUE__)  ((__HANDLE__)->Instance->RCR =  (__VALUE__))

/**
  * @brief  Return the current Repetition value.
  * @param  __HANDLE__ LPTIM handle
  * @retval Repetition register value
  * @note   The RCR register can only be modified when the LPTIM instance is enabled.
  */
#define __HAL_LPTIM_REPETITIONCOUNTER_GET(__HANDLE__)  ((__HANDLE__)->Instance->RCR)

/**
  * @brief  Enable the LPTIM signal input/output on the corresponding pin.
  * @param  __HANDLE__  LPTIM handle
  * @param  __CHANNEL__ LPTIM Channels to be enabled.
  *          This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @retval None
  */
#define __HAL_LPTIM_CAPTURE_COMPARE_ENABLE(__HANDLE__, __CHANNEL__)                                            \
  do {                                                        \
    switch (__CHANNEL__)                                   \
    {                                                      \
      case LPTIM_CHANNEL_1:                                \
        ((__HANDLE__)->Instance->CCMR1 |= LPTIM_CCMR1_CC1E); \
        break;                                             \
      case LPTIM_CHANNEL_2:                                \
        ((__HANDLE__)->Instance->CCMR1 |= LPTIM_CCMR1_CC2E); \
        break;                                             \
      default:                                             \
        break;                                             \
    }                                                      \
  }                                                          \
  while(0)

/**
  * @brief  Disable the LPTIM signal input/output on the corresponding pin.
  * @param  __HANDLE__  LPTIM handle
  * @param  __CHANNEL__ LPTIM Channels to be disabled.
  *          This parameter can be one of the following values:
  *            @arg LPTIM_CHANNEL_1: LPTIM Channel 1 selected
  *            @arg LPTIM_CHANNEL_2: LPTIM Channel 2 selected
  * @retval None
  */
#define __HAL_LPTIM_CAPTURE_COMPARE_DISABLE(__HANDLE__, __CHANNEL__)                                            \
  do {                                                         \
    switch (__CHANNEL__)                                    \
    {                                                       \
      case LPTIM_CHANNEL_1:                                 \
        ((__HANDLE__)->Instance->CCMR1 &= ~LPTIM_CCMR1_CC1E); \
        break;                                              \
      case LPTIM_CHANNEL_2:                                 \
        ((__HANDLE__)->Instance->CCMR1 &= ~LPTIM_CCMR1_CC2E); \
        break;                                              \
      default:                                              \
        break;                                              \
    }                                                       \
  }                                                           \
  while(0)

/**
  * @brief  Check whether the specified LPTIM flag is set or not.
  * @param  __HANDLE__ LPTIM handle
  * @param  __FLAG__ LPTIM flag to check
  *            This parameter can be a value of:
  *            @arg LPTIM_FLAG_REPOK   : Repetition register update OK Flag.
  *            @arg LPTIM_FLAG_UPDATE  : Update event Flag.
  *            @arg LPTIM_FLAG_DOWN    : Counter direction change up Flag.
  *            @arg LPTIM_FLAG_UP      : Counter direction change down to up Flag.
  *            @arg LPTIM_FLAG_ARROK   : Autoreload register update OK Flag.
  *            @arg LPTIM_FLAG_CMP1OK  : Compare register 1 update OK Flag.
  *            @arg LPTIM_FLAG_CMP2OK  : Compare register 2 update OK Flag.
  *            @arg LPTIM_FLAG_EXTTRIG : External trigger edge event Flag.
  *            @arg LPTIM_FLAG_ARRM    : Autoreload match Flag.
  *            @arg LPTIM_FLAG_CC1     : Capture/Compare 1 interrupt flag.
  *            @arg LPTIM_FLAG_CC2     : Capture/Compare 2 interrupt flag.
  *            @arg LPTIM_FLAG_CC1O    : Capture/Compare 1 over-capture flag.
  *            @arg LPTIM_FLAG_CC2O    : Capture/Compare 2 over-capture flag.
  *            @arg LPTIM_FLAG_DIEROK  : DMA & interrupt enable update OK flag.
  * @retval The state of the specified flag (SET or RESET).
  */
#define __HAL_LPTIM_GET_FLAG(__HANDLE__, __FLAG__)          (((__HANDLE__)->Instance->ISR &(__FLAG__)) == (__FLAG__))

/**
  * @brief  Clear the specified LPTIM flag.
  * @param  __HANDLE__ LPTIM handle.
  * @param  __FLAG__ LPTIM flag to clear.
  *            This parameter can be a value of:
  *            @arg LPTIM_FLAG_REPOK   : Repetition register update OK Flag.
  *            @arg LPTIM_FLAG_UPDATE  : Update event Flag.
  *            @arg LPTIM_FLAG_DOWN    : Counter direction change up Flag.
  *            @arg LPTIM_FLAG_UP      : Counter direction change down to up Flag.
  *            @arg LPTIM_FLAG_ARROK   : Autoreload register update OK Flag.
  *            @arg LPTIM_FLAG_CMP1OK  : Compare register 1 update OK Flag.
  *            @arg LPTIM_FLAG_CMP2OK  : Compare register 2 update OK Flag.
  *            @arg LPTIM_FLAG_EXTTRIG : External trigger edge event Flag.
  *            @arg LPTIM_FLAG_ARRM    : Autoreload match Flag.
  *            @arg LPTIM_FLAG_CC1     : Capture/Compare 1 interrupt flag.
  *            @arg LPTIM_FLAG_CC2     : Capture/Compare 2 interrupt flag.
  *            @arg LPTIM_FLAG_CC1O    : Capture/Compare 1 over-capture flag.
  *            @arg LPTIM_FLAG_CC2O    : Capture/Compare 2 over-capture flag.
  *            @arg LPTIM_FLAG_DIEROK  : DMA & interrupt enable update OK flag.
  * @retval None.
  */
#define __HAL_LPTIM_CLEAR_FLAG(__HANDLE__, __FLAG__)        ((__HANDLE__)->Instance->ICR  = (__FLAG__))

/**
  * @brief  Enable the specified LPTIM interrupt.
  * @param  __HANDLE__ LPTIM handle.
  * @param  __INTERRUPT__ LPTIM interrupt to set.
  *            This parameter can be a value of:
  *            @arg LPTIM_IT_REPOK   : Repetition register update OK Interrupt.
  *            @arg LPTIM_IT_UPDATE  : Update event register Interrupt.
  *            @arg LPTIM_IT_DOWN    : Counter direction change up Interrupt.
  *            @arg LPTIM_IT_UP      : Counter direction change down to up Interrupt.
  *            @arg LPTIM_IT_ARROK   : Autoreload register update OK Interrupt.
  *            @arg LPTIM_IT_CMP1OK  : Compare register 1 update OK Interrupt.
  *            @arg LPTIM_IT_CMP2OK  : Compare register 2 update OK Interrupt.
  *            @arg LPTIM_IT_EXTTRIG : External trigger edge event Interrupt.
  *            @arg LPTIM_IT_ARRM    : Autoreload match Interrupt.
  *            @arg LPTIM_IT_CC1     : Capture/Compare 1 interrupt Interrupt.
  *            @arg LPTIM_IT_CC2     : Capture/Compare 2 interrupt Interrupt.
  *            @arg LPTIM_IT_CC1O    : Capture/Compare 1 over-capture Interrupt.
  *            @arg LPTIM_IT_CC2O    : Capture/Compare 2 over-capture Interrupt.
  * @retval None.
  * @note   The LPTIM interrupts can only be enabled when the LPTIM instance is enabled.
  */
#define __HAL_LPTIM_ENABLE_IT(__HANDLE__, __INTERRUPT__)    ((__HANDLE__)->Instance->DIER  |= (__INTERRUPT__))

/**
  * @brief  Disable the specified LPTIM interrupt.
  * @param  __HANDLE__ LPTIM handle.
  * @param  __INTERRUPT__ LPTIM interrupt to set.
  *            This parameter can be a value of:
  *            @arg LPTIM_IT_REPOK   : Repetition register update OK Interrupt.
  *            @arg LPTIM_IT_UPDATE  : Update event register Interrupt.
  *            @arg LPTIM_IT_DOWN    : Counter direction change up Interrupt.
  *            @arg LPTIM_IT_UP      : Counter direction change down to up Interrupt.
  *            @arg LPTIM_IT_ARROK   : Autoreload register update OK Interrupt.
  *            @arg LPTIM_IT_CMP1OK  : Compare register 1 update OK Interrupt.
  *            @arg LPTIM_IT_CMP2OK  : Compare register 2 update OK Interrupt.
  *            @arg LPTIM_IT_EXTTRIG : External trigger edge event Interrupt.
  *            @arg LPTIM_IT_ARRM    : Autoreload match Interrupt.
  *            @arg LPTIM_IT_CC1     : Capture/Compare 1 interrupt Interrupt.
  *            @arg LPTIM_IT_CC2     : Capture/Compare 2 interrupt Interrupt.
  *            @arg LPTIM_IT_CC1O    : Capture/Compare 1 over-capture Interrupt.
  *            @arg LPTIM_IT_CC2O    : Capture/Compare 2 over-capture Interrupt.
  * @retval None.
  * @note   The LPTIM interrupts can only be disabled when the LPTIM instance is enabled.
  */
#define __HAL_LPTIM_DISABLE_IT(__HANDLE__, __INTERRUPT__)   ((__HANDLE__)->Instance->DIER  &= (~(__INTERRUPT__)))

/** @brief  Enable the specified DMA request.
  * @param  __HANDLE__ specifies the TIM Handle.
  * @param  __DMA__ specifies the LPTIM DMA request to enable.
  *          This parameter can be one of the following values:
  *            @arg LPTIM_DMA_UPDATE: Update DMA request
  *            @arg LPTIM_DMA_CC1:   Capture/Compare 1 DMA request
  *            @arg LPTIM_DMA_CC2:  Capture/Compare 2 DMA request
  * @retval None
  */
#define __HAL_LPTIM_ENABLE_DMA(__HANDLE__, __DMA__)         ((__HANDLE__)->Instance->DIER |= (__DMA__))

/** @brief  Disable the specified DMA request.
  * @param  __HANDLE__ specifies the LPTIM Handle.
  * @param  __DMA__ specifies the LPTIM DMA request to disable.
  *          This parameter can be one of the following values:
  *            @arg LPTIM_DMA_UPDATE: Update DMA request
  *            @arg LPTIM_DMA_CC1:   Capture/Compare 1 DMA request
  *            @arg LPTIM_DMA_CC2:   Capture/Compare 2 DMA request
  * @retval None
  */
#define __HAL_LPTIM_DISABLE_DMA(__HANDLE__, __DMA__)        ((__HANDLE__)->Instance->DIER &= ~(__DMA__))

/**
  * @brief  Check whether the specified LPTIM interrupt source is enabled or not.
  * @param  __HANDLE__ LPTIM handle.
  * @param  __INTERRUPT__ LPTIM interrupt to check.
  *            This parameter can be a value of:
  *            @arg LPTIM_IT_REPOK   : Repetition register update OK Interrupt.
  *            @arg LPTIM_IT_UPDATE  : Update event register Interrupt.
  *            @arg LPTIM_IT_DOWN    : Counter direction change up Interrupt.
  *            @arg LPTIM_IT_UP      : Counter direction change down to up Interrupt.
  *            @arg LPTIM_IT_ARROK   : Autoreload register update OK Interrupt.
  *            @arg LPTIM_IT_CMP1OK  : Compare register 1 update OK Interrupt.
  *            @arg LPTIM_IT_CMP2OK  : Compare register 2 update OK Interrupt.
  *            @arg LPTIM_IT_EXTTRIG : External trigger edge event Interrupt.
  *            @arg LPTIM_IT_ARRM    : Autoreload match Interrupt.
  *            @arg LPTIM_IT_CC1     : Capture/Compare 1 interrupt Interrupt.
  *            @arg LPTIM_IT_CC2     : Capture/Compare 2 interrupt Interrupt.
  *            @arg LPTIM_IT_CC1O    : Capture/Compare 1 over-capture Interrupt.
  *            @arg LPTIM_IT_CC2O    : Capture/Compare 2 over-capture Interrupt.
  * @retval Interrupt status.
  */

#define __HAL_LPTIM_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->DIER\
                                                                & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

/**
  * @brief  Enable the LPTIM1 EXTI line in interrupt mode.
  * @retval None
  */
#define __HAL_LPTIM_LPTIM1_EXTI_ENABLE_IT()            (EXTI->IMR2 |= LPTIM_EXTI_LINE_LPTIM1)

/**
  * @brief  Disable the LPTIM1 EXTI line in interrupt mode.
  * @retval None
  */
#define __HAL_LPTIM_LPTIM1_EXTI_DISABLE_IT()           (EXTI->IMR2\
                                                        &= ~(LPTIM_EXTI_LINE_LPTIM1))

/**
  * @brief  Enable the LPTIM2 EXTI line in interrupt mode.
  * @retval None
  */
#define __HAL_LPTIM_LPTIM2_EXTI_ENABLE_IT()            (EXTI->IMR2 |= LPTIM_EXTI_LINE_LPTIM2)

/**
  * @brief  Disable the LPTIM2 EXTI line in interrupt mode.
  * @retval None
  */
#define __HAL_LPTIM_LPTIM2_EXTI_DISABLE_IT()           (EXTI->IMR2\
                                                        &= ~(LPTIM_EXTI_LINE_LPTIM2))

/**
  * @brief  Enable the LPTIM3 EXTI line in interrupt mode.
  * @retval None
  */
#define __HAL_LPTIM_LPTIM3_EXTI_ENABLE_IT()            (EXTI->IMR2 |= LPTIM_EXTI_LINE_LPTIM3)

/**
  * @brief  Disable the LPTIM3 EXTI line in interrupt mode.
  * @retval None
  */
#define __HAL_LPTIM_LPTIM3_EXTI_DISABLE_IT()           (EXTI->IMR2\
                                                        &= ~(LPTIM_EXTI_LINE_LPTIM3))

/**
  * @brief  Enable the LPTIM4 EXTI line in interrupt mode.
  * @retval None
  */
#define __HAL_LPTIM_LPTIM4_EXTI_ENABLE_IT()            (EXTI->IMR2 |= LPTIM_EXTI_LINE_LPTIM4)

/**
  * @brief  Disable the LPTIM4 EXTI line in interrupt mode.
  * @retval None
  */
#define __HAL_LPTIM_LPTIM4_EXTI_DISABLE_IT()           (EXTI->IMR2\
                                                        &= ~(LPTIM_EXTI_LINE_LPTIM4))

/**
  * @brief  Enable the LPTIM5 EXTI line in interrupt mode.
  * @retval None
  */
#define __HAL_LPTIM_LPTIM5_EXTI_ENABLE_IT()            (EXTI->IMR2 |= LPTIM_EXTI_LINE_LPTIM5)

/**
  * @brief  Disable the LPTIM5 EXTI line in interrupt mode.
  * @retval None
  */
#define __HAL_LPTIM_LPTIM5_EXTI_DISABLE_IT()           (EXTI->IMR2\
                                                        &= ~(LPTIM_EXTI_LINE_LPTIM5))

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup LPTIM_Exported_Functions LPTIM Exported Functions
  * @{
  */

/** @addtogroup LPTIM_Exported_Functions_Group1
  *  @brief    Initialization and Configuration functions.
  * @{
  */
/* Initialization/de-initialization functions  ********************************/
HAL_StatusTypeDef HAL_LPTIM_Init(LPTIM_HandleTypeDef *hlptim);
HAL_StatusTypeDef HAL_LPTIM_DeInit(LPTIM_HandleTypeDef *hlptim);

/* MSP functions  *************************************************************/
void HAL_LPTIM_MspInit(LPTIM_HandleTypeDef *hlptim);
void HAL_LPTIM_MspDeInit(LPTIM_HandleTypeDef *hlptim);
/**
  * @}
  */

/** @addtogroup LPTIM_Exported_Functions_Group2
  *  @brief   Start-Stop operation functions.
  * @{
  */
/* Config functions  **********************************************************/
HAL_StatusTypeDef HAL_LPTIM_OC_ConfigChannel(LPTIM_HandleTypeDef *hlptim, const LPTIM_OC_ConfigTypeDef *sConfig,
                                             uint32_t Channel);

/* Start/Stop operation functions  *********************************************/
/* ################################# PWM Mode ################################*/
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_LPTIM_PWM_Start(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);
HAL_StatusTypeDef HAL_LPTIM_PWM_Stop(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);
/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_LPTIM_PWM_Start_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);
HAL_StatusTypeDef HAL_LPTIM_PWM_Stop_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);
HAL_StatusTypeDef HAL_LPTIM_PWM_Start_DMA(LPTIM_HandleTypeDef *hlptim, uint32_t Channel, const uint32_t *pData,
                                          uint32_t Length);
HAL_StatusTypeDef HAL_LPTIM_PWM_Stop_DMA(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);

/* ############################# One Pulse Mode ##############################*/
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_LPTIM_OnePulse_Start(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);
HAL_StatusTypeDef HAL_LPTIM_OnePulse_Stop(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);
/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_LPTIM_OnePulse_Start_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);
HAL_StatusTypeDef HAL_LPTIM_OnePulse_Stop_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);

/* ############################## Set once Mode ##############################*/
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_LPTIM_SetOnce_Start(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);
HAL_StatusTypeDef HAL_LPTIM_SetOnce_Stop(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);
/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_LPTIM_SetOnce_Start_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);
HAL_StatusTypeDef HAL_LPTIM_SetOnce_Stop_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);

/* ############################### Encoder Mode ##############################*/
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_LPTIM_Encoder_Start(LPTIM_HandleTypeDef *hlptim);
HAL_StatusTypeDef HAL_LPTIM_Encoder_Stop(LPTIM_HandleTypeDef *hlptim);
/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_LPTIM_Encoder_Start_IT(LPTIM_HandleTypeDef *hlptim);
HAL_StatusTypeDef HAL_LPTIM_Encoder_Stop_IT(LPTIM_HandleTypeDef *hlptim);

/* ############################# Time out  Mode ##############################*/
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_LPTIM_TimeOut_Start(LPTIM_HandleTypeDef *hlptim, uint32_t Timeout);
HAL_StatusTypeDef HAL_LPTIM_TimeOut_Stop(LPTIM_HandleTypeDef *hlptim);
/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_LPTIM_TimeOut_Start_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Timeout);
HAL_StatusTypeDef HAL_LPTIM_TimeOut_Stop_IT(LPTIM_HandleTypeDef *hlptim);

/* ############################## Counter Mode ###############################*/
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_LPTIM_Counter_Start(LPTIM_HandleTypeDef *hlptim);
HAL_StatusTypeDef HAL_LPTIM_Counter_Stop(LPTIM_HandleTypeDef *hlptim);
/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_LPTIM_Counter_Start_IT(LPTIM_HandleTypeDef *hlptim);
HAL_StatusTypeDef HAL_LPTIM_Counter_Stop_IT(LPTIM_HandleTypeDef *hlptim);

/* ############################## Input Capture Mode ###############################*/
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_LPTIM_IC_ConfigChannel(LPTIM_HandleTypeDef *hlptim, const LPTIM_IC_ConfigTypeDef *sConfig,
                                             uint32_t Channel);
HAL_StatusTypeDef HAL_LPTIM_IC_Start(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);
HAL_StatusTypeDef HAL_LPTIM_IC_Stop(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);
/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_LPTIM_IC_Start_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);
HAL_StatusTypeDef HAL_LPTIM_IC_Stop_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);
HAL_StatusTypeDef HAL_LPTIM_IC_Start_DMA(LPTIM_HandleTypeDef *hlptim, uint32_t Channel, uint32_t *pData,
                                         uint32_t Length);
HAL_StatusTypeDef HAL_LPTIM_IC_Stop_DMA(LPTIM_HandleTypeDef *hlptim, uint32_t Channel);
/**
  * @}
  */

/** @addtogroup LPTIM_Exported_Functions_Group3
  *  @brief  Read operation functions.
  * @{
  */
/* Reading operation functions ************************************************/
uint32_t HAL_LPTIM_ReadCounter(const LPTIM_HandleTypeDef *hlptim);
uint32_t HAL_LPTIM_ReadAutoReload(const LPTIM_HandleTypeDef *hlptim);
uint32_t HAL_LPTIM_ReadCapturedValue(const LPTIM_HandleTypeDef *hlptim, uint32_t Channel);
uint8_t HAL_LPTIM_IC_GetOffset(const LPTIM_HandleTypeDef *hlptim, uint32_t Channel);
/**
  * @}
  */

/** @addtogroup LPTIM_Exported_Functions_Group4
  *  @brief  LPTIM IRQ handler and callback functions.
  * @{
  */
/* LPTIM IRQ functions  *******************************************************/
void HAL_LPTIM_IRQHandler(LPTIM_HandleTypeDef *hlptim);

/* CallBack functions  ********************************************************/
void HAL_LPTIM_CompareMatchCallback(LPTIM_HandleTypeDef *hlptim);
void HAL_LPTIM_AutoReloadMatchCallback(LPTIM_HandleTypeDef *hlptim);
void HAL_LPTIM_TriggerCallback(LPTIM_HandleTypeDef *hlptim);
void HAL_LPTIM_CompareWriteCallback(LPTIM_HandleTypeDef *hlptim);
void HAL_LPTIM_AutoReloadWriteCallback(LPTIM_HandleTypeDef *hlptim);
void HAL_LPTIM_DirectionUpCallback(LPTIM_HandleTypeDef *hlptim);
void HAL_LPTIM_DirectionDownCallback(LPTIM_HandleTypeDef *hlptim);
void HAL_LPTIM_ErrorCallback(LPTIM_HandleTypeDef *hlptim);
void HAL_LPTIM_UpdateEventCallback(LPTIM_HandleTypeDef *hlptim);
void HAL_LPTIM_UpdateEventHalfCpltCallback(LPTIM_HandleTypeDef *hlptim);
void HAL_LPTIM_RepCounterWriteCallback(LPTIM_HandleTypeDef *hlptim);
void HAL_LPTIM_IC_CaptureCallback(LPTIM_HandleTypeDef *hlptim);
void HAL_LPTIM_IC_CaptureHalfCpltCallback(LPTIM_HandleTypeDef *hlptim);
void HAL_LPTIM_IC_OverCaptureCallback(LPTIM_HandleTypeDef *hlptim);

/* Callbacks Register/UnRegister functions  ***********************************/
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_LPTIM_RegisterCallback(LPTIM_HandleTypeDef *lphtim, HAL_LPTIM_CallbackIDTypeDef CallbackID,
                                             pLPTIM_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_LPTIM_UnRegisterCallback(LPTIM_HandleTypeDef *lphtim, HAL_LPTIM_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @addtogroup LPTIM_Group5
  *  @brief   Peripheral State functions.
  * @{
  */
/* Peripheral State functions  ************************************************/
HAL_LPTIM_StateTypeDef HAL_LPTIM_GetState(const LPTIM_HandleTypeDef *hlptim);
/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/** @defgroup LPTIM_Private_Types LPTIM Private Types
  * @{
  */

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup LPTIM_Private_Variables LPTIM Private Variables
  * @{
  */

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup LPTIM_Private_Constants LPTIM Private Constants
  * @{
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup LPTIM_Private_Macros LPTIM Private Macros
  * @{
  */

#define IS_LPTIM_CLOCK_SOURCE(__SOURCE__)       (((__SOURCE__) == LPTIM_CLOCKSOURCE_ULPTIM) || \
                                                 ((__SOURCE__) == LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC))


#define IS_LPTIM_CLOCK_PRESCALER(__PRESCALER__) (((__PRESCALER__) ==  LPTIM_PRESCALER_DIV1  ) || \
                                                 ((__PRESCALER__) ==  LPTIM_PRESCALER_DIV2  ) || \
                                                 ((__PRESCALER__) ==  LPTIM_PRESCALER_DIV4  ) || \
                                                 ((__PRESCALER__) ==  LPTIM_PRESCALER_DIV8  ) || \
                                                 ((__PRESCALER__) ==  LPTIM_PRESCALER_DIV16 ) || \
                                                 ((__PRESCALER__) ==  LPTIM_PRESCALER_DIV32 ) || \
                                                 ((__PRESCALER__) ==  LPTIM_PRESCALER_DIV64 ) || \
                                                 ((__PRESCALER__) ==  LPTIM_PRESCALER_DIV128))

#define IS_LPTIM_CLOCK_PRESCALERDIV1(__PRESCALER__) ((__PRESCALER__) ==  LPTIM_PRESCALER_DIV1)
#define IS_LPTIM_CLOCK_SAMPLE_TIME(__SAMPLETIME__) (((__SAMPLETIME__) == LPTIM_CLOCKSAMPLETIME_DIRECTTRANSITION) || \
                                                    ((__SAMPLETIME__) == LPTIM_CLOCKSAMPLETIME_2TRANSITIONS)     || \
                                                    ((__SAMPLETIME__) == LPTIM_CLOCKSAMPLETIME_4TRANSITIONS)     || \
                                                    ((__SAMPLETIME__) == LPTIM_CLOCKSAMPLETIME_8TRANSITIONS))

#define IS_LPTIM_CLOCK_POLARITY(__POLARITY__)   (((__POLARITY__) == LPTIM_CLOCKPOLARITY_RISING)  || \
                                                 ((__POLARITY__) == LPTIM_CLOCKPOLARITY_FALLING) || \
                                                 ((__POLARITY__) == LPTIM_CLOCKPOLARITY_RISING_FALLING))

#define IS_LPTIM_TRG_SOURCE(__TRIG__)           (((__TRIG__) == LPTIM_TRIGSOURCE_SOFTWARE) || \
                                                 ((__TRIG__) == LPTIM_TRIGSOURCE_0) || \
                                                 ((__TRIG__) == LPTIM_TRIGSOURCE_1) || \
                                                 ((__TRIG__) == LPTIM_TRIGSOURCE_2) || \
                                                 ((__TRIG__) == LPTIM_TRIGSOURCE_3) || \
                                                 ((__TRIG__) == LPTIM_TRIGSOURCE_4) || \
                                                 ((__TRIG__) == LPTIM_TRIGSOURCE_5) || \
                                                 ((__TRIG__) == LPTIM_TRIGSOURCE_6) || \
                                                 ((__TRIG__) == LPTIM_TRIGSOURCE_7))

#define IS_LPTIM_EXT_TRG_POLARITY(__POLARITY__) (((__POLARITY__) == LPTIM_ACTIVEEDGE_RISING         ) || \
                                                 ((__POLARITY__) == LPTIM_ACTIVEEDGE_FALLING        ) || \
                                                 ((__POLARITY__) == LPTIM_ACTIVEEDGE_RISING_FALLING ))

#define IS_LPTIM_TRIG_SAMPLE_TIME(__SAMPLETIME__) (((__SAMPLETIME__) == LPTIM_TRIGSAMPLETIME_DIRECTTRANSITION) || \
                                                   ((__SAMPLETIME__) == LPTIM_TRIGSAMPLETIME_2TRANSITIONS    ) || \
                                                   ((__SAMPLETIME__) == LPTIM_TRIGSAMPLETIME_4TRANSITIONS    ) || \
                                                   ((__SAMPLETIME__) == LPTIM_TRIGSAMPLETIME_8TRANSITIONS    ))

#define IS_LPTIM_UPDATE_MODE(__MODE__)          (((__MODE__) == LPTIM_UPDATE_IMMEDIATE) || \
                                                 ((__MODE__) == LPTIM_UPDATE_ENDOFPERIOD))

#define IS_LPTIM_COUNTER_SOURCE(__SOURCE__)     (((__SOURCE__) == LPTIM_COUNTERSOURCE_INTERNAL) || \
                                                 ((__SOURCE__) == LPTIM_COUNTERSOURCE_EXTERNAL))

#define IS_LPTIM_COMPARE(__COMPARE__)           ((__COMPARE__) <= 0x0000FFFFUL)

#define IS_LPTIM_PERIOD(__PERIOD__)             ((0x00000001UL <= (__PERIOD__)) &&\
                                                 ((__PERIOD__) <= 0x0000FFFFUL))

#define IS_LPTIM_PULSE(__PULSE__)               ((__PULSE__) <= 0x0000FFFFUL)

#define IS_LPTIM_OC_POLARITY(__OCPOLARITY__)    (((__OCPOLARITY__) == LPTIM_OCPOLARITY_LOW) || \
                                                 ((__OCPOLARITY__) == LPTIM_OCPOLARITY_HIGH))
#define IS_LPTIM_IC_PRESCALER(__PRESCALER__)    (((__PRESCALER__) == LPTIM_ICPSC_DIV1) ||\
                                                 ((__PRESCALER__) == LPTIM_ICPSC_DIV2) ||\
                                                 ((__PRESCALER__) == LPTIM_ICPSC_DIV4) ||\
                                                 ((__PRESCALER__) == LPTIM_ICPSC_DIV8))

#define IS_LPTIM_IC_POLARITY(__POLARITY__)      (((__POLARITY__) == LPTIM_ICPOLARITY_RISING) || \
                                                 ((__POLARITY__) == LPTIM_ICPOLARITY_FALLING) ||\
                                                 ((__POLARITY__) == LPTIM_ICPOLARITY_RISING_FALLING))

#define IS_LPTIM_IC_FILTER(__FILTER__)          (((__FILTER__) == LPTIM_ICFLT_CLOCK_DIV1) ||\
                                                 ((__FILTER__) == LPTIM_ICFLT_CLOCK_DIV2) ||\
                                                 ((__FILTER__) == LPTIM_ICFLT_CLOCK_DIV4) ||\
                                                 ((__FILTER__) == LPTIM_ICFLT_CLOCK_DIV8))

#define IS_LPTIM_REPETITION(__REPETITION__)     ((__REPETITION__) <= 0x000000FFUL)

#define IS_LPTIM_INPUT1_SOURCE(__INSTANCE__, __SOURCE__) \
  ((((__INSTANCE__) == LPTIM1) &&                        \
    ((__SOURCE__) == LPTIM_INPUT1SOURCE_GPIO) )          \
   ||                                                    \
   (((__INSTANCE__) == LPTIM2) &&                        \
    ((__SOURCE__) == LPTIM_INPUT1SOURCE_GPIO))           \
   ||                                                    \
   (((__INSTANCE__) == LPTIM3) &&                        \
    ((__SOURCE__) == LPTIM_INPUT1SOURCE_GPIO)))

#define IS_LPTIM_INPUT2_SOURCE(__INSTANCE__, __SOURCE__) \
  ((((__INSTANCE__) == LPTIM1) &&                        \
    ((__SOURCE__) == LPTIM_INPUT2SOURCE_GPIO) )          \
   ||                                                    \
   (((__INSTANCE__) == LPTIM2) &&                        \
    ((__SOURCE__) == LPTIM_INPUT2SOURCE_GPIO))           \
   ||                                                    \
   (((__INSTANCE__) == LPTIM3) &&                        \
    ((__SOURCE__) == LPTIM_INPUT2SOURCE_GPIO)))

#define IS_LPTIM_IC1_SOURCE(__INSTANCE__, __SOURCE__) \
  ((((__INSTANCE__) == LPTIM1) &&                     \
    (((__SOURCE__) == LPTIM_IC1SOURCE_GPIO)))         \
   ||                                                 \
   (((__INSTANCE__) == LPTIM2) &&                     \
    (((__SOURCE__) == LPTIM_IC1SOURCE_GPIO)     ||    \
     ((__SOURCE__) == LPTIM_IC1SOURCE_I3C1_IBIACK)))  \
   ||                                                 \
   (((__INSTANCE__) == LPTIM3) &&                     \
    (((__SOURCE__) == LPTIM_IC1SOURCE_GPIO)  ||       \
     ((__SOURCE__) == LPTIM_IC1SOURCE_I3C1_IBIACK))))

#define IS_LPTIM_IC2_SOURCE(__INSTANCE__, __SOURCE__) \
  ((((__INSTANCE__) == LPTIM1) &&                     \
    (((__SOURCE__) == LPTIM_IC2SOURCE_GPIO) ||        \
     ((__SOURCE__) == LPTIM_IC2SOURCE_LSI)  ||        \
     ((__SOURCE__) == LPTIM_IC2SOURCE_LSE)))          \
   ||                                                 \
   (((__INSTANCE__) == LPTIM2) &&                     \
    (((__SOURCE__) == LPTIM_IC2SOURCE_GPIO)     ||    \
     ((__SOURCE__) == LPTIM_IC2SOURCE_HSI_1024) ||    \
     ((__SOURCE__) == LPTIM_IC2SOURCE_MSI_128)  ||    \
     ((__SOURCE__) == LPTIM_IC2SOURCE_I3C2_IBIACK)))  \
   ||                                                 \
   (((__INSTANCE__) == LPTIM3) &&                     \
    (((__SOURCE__) == LPTIM_IC2SOURCE_GPIO)  ||       \
     ((__SOURCE__) == LPTIM_IC2SOURCE_I3C2_IBIACK))))

#define LPTIM_CHANNEL_STATE_GET(__INSTANCE__, __CHANNEL__)\
  (((__CHANNEL__) == LPTIM_CHANNEL_1) ? (__INSTANCE__)->ChannelState[0] :\
   (__INSTANCE__)->ChannelState[1])

#define LPTIM_CHANNEL_STATE_SET(__INSTANCE__, __CHANNEL__, __CHANNEL_STATE__) \
  (((__CHANNEL__) == LPTIM_CHANNEL_1) ? ((__INSTANCE__)->ChannelState[0] = (__CHANNEL_STATE__)) :\
   ((__INSTANCE__)->ChannelState[1] = (__CHANNEL_STATE__)))

#define LPTIM_CHANNEL_STATE_SET_ALL(__INSTANCE__,  __CHANNEL_STATE__) do { \
                                                                           (__INSTANCE__)->ChannelState[0]  =\
                                                                                                (__CHANNEL_STATE__);  \
                                                                           (__INSTANCE__)->ChannelState[1]  =\
                                                                                                (__CHANNEL_STATE__);  \
                                                                         } while(0)

#define IS_LPTIM_CCX_INSTANCE(__INSTANCE__, __CHANNEL__) \
  ((((__INSTANCE__) == LPTIM1)  && \
    (((__CHANNEL__)  == LPTIM_CHANNEL_1) ||  \
     ((__CHANNEL__)  == LPTIM_CHANNEL_2)))   \
   ||                                        \
   (((__INSTANCE__) == LPTIM2)  && \
    (((__CHANNEL__)  == LPTIM_CHANNEL_1) ||  \
     ((__CHANNEL__)  == LPTIM_CHANNEL_2)))   \
   ||                                        \
   (((__INSTANCE__) == LPTIM3)  && \
    (((__CHANNEL__)  == LPTIM_CHANNEL_1) ||  \
     ((__CHANNEL__)  == LPTIM_CHANNEL_2)))   \
   ||                                        \
   (((__INSTANCE__) == LPTIM4)  && \
    (((__CHANNEL__)  == LPTIM_CHANNEL_1))) \
   ||                                        \
   (((__INSTANCE__) == LPTIM5)  && \
    ((__CHANNEL__)  == LPTIM_CHANNEL_1)))
/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup LPTIM_Private_Functions LPTIM Private Functions
  * @{
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

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_LPTIM_H */
