/**
  ******************************************************************************
  * @file    stm32n6xx_hal_rtc.h
  * @author  GPM Application Team
  * @brief   Header file of RTC HAL module.
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
#ifndef STM32N6xx_HAL_RTC_H
#define STM32N6xx_HAL_RTC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup RTC RTC
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup RTC_Exported_Types RTC Exported Types
  * @{
  */

/**
  * @brief  HAL State structures definition
  */
typedef enum
{
  HAL_RTC_STATE_RESET             = 0x00U,  /*!< RTC not yet initialized or disabled */
  HAL_RTC_STATE_READY             = 0x01U,  /*!< RTC initialized and ready for use   */
  HAL_RTC_STATE_BUSY              = 0x02U,  /*!< RTC process is ongoing              */
  HAL_RTC_STATE_TIMEOUT           = 0x03U,  /*!< RTC timeout state                   */
  HAL_RTC_STATE_ERROR             = 0x04U   /*!< RTC error state                     */

} HAL_RTCStateTypeDef;

/**
  * @brief  RTC Configuration Structure definition
  */
typedef struct
{
  uint32_t HourFormat;        /*!< Specifies the RTC Hour Format.
                                 This parameter can be a value of @ref RTC_Hour_Formats */

  uint32_t AsynchPrediv;      /*!< Specifies the RTC Asynchronous Predivider value.
                                 This parameter must be a number between Min_Data = 0x00 and Max_Data = 0x7F */

  uint32_t SynchPrediv;       /*!< Specifies the RTC Synchronous Predivider value.
                                 This parameter must be a number between Min_Data = 0x00 and Max_Data = 0x7FFF */

  uint32_t OutPut;            /*!< Specifies which signal will be routed to the RTC output.
                                 This parameter can be a value of @ref RTCEx_Output_selection_Definitions */

  uint32_t OutPutRemap;       /*!< Specifies the remap for RTC output.
                                 This parameter can be a value of @ref  RTC_Output_ALARM_OUT_Remap */

  uint32_t OutPutPolarity;    /*!< Specifies the polarity of the output signal.
                                 This parameter can be a value of @ref RTC_Output_Polarity_Definitions */

  uint32_t OutPutType;        /*!< Specifies the RTC Output Pin mode.
                                 This parameter can be a value of @ref RTC_Output_Type_ALARM_OUT */

  uint32_t OutPutPullUp;      /*!< Specifies the RTC Output Pull-Up mode.
                                 This parameter can be a value of @ref RTC_Output_PullUp_ALARM_OUT */

  uint32_t BinMode;           /*!< Specifies the RTC binary mode.
                                 This parameter can be a value of @ref RTCEx_Binary_Mode */

  uint32_t BinMixBcdU;        /*!< Specifies the BCD calendar update if and only if BinMode = RTC_BINARY_MIX.
                                 This parameter can be a value of @ref RTCEx_Binary_mix_BCDU */
} RTC_InitTypeDef;

/**
  * @brief  RTC Time structure definition
  */
typedef struct
{
  uint8_t Hours;            /*!< Specifies the RTC Time Hour.
                                 This parameter must be a number between:
                                 Min_Data = 0 and Max_Data = 12 if the RTC_HOURFORMAT_12 is selected.
                                 This parameter must be a number between:
                                 Min_Data = 0 and Max_Data = 23 if the RTC_HOURFORMAT_24 is selected */

  uint8_t Minutes;          /*!< Specifies the RTC Time Minutes.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 59 */

  uint8_t Seconds;          /*!< Specifies the RTC Time Seconds.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 59 */

  uint8_t TimeFormat;       /*!< Specifies the RTC AM/PM Time.
                                 This parameter can be a value of @ref RTC_AM_PM_Definitions */

  uint32_t SubSeconds;      /*!< Specifies the RTC_SSR RTC Sub Second register content.
                                 This field is not used by HAL_RTC_SetTime.
                                 If the free running 32 bit counter is not activated (mode binary none)
                                    - This parameter corresponds to a time unit range
                                 between [0-1] Second with [1 Sec / SecondFraction +1] granularity
                                 else
                                    - This parameter corresponds to the free running 32 bit counter. */

  uint32_t SecondFraction;  /*!< Specifies the range or granularity of Sub Second register content
                                 corresponding to Synchronous pre-scaler factor value (PREDIV_S)
                                 This parameter corresponds to a time unit range between [0-1] Second
                                 with [1 Sec / SecondFraction +1] granularity.
                                 This field will be used only by HAL_RTC_GetTime function */

  uint32_t DayLightSaving;  /*!< This interface is deprecated.
                                 To manage Daylight Saving Time, please use HAL_RTC_DST_xxx functions */

  uint32_t StoreOperation;  /*!< This interface is deprecated.
                                 To manage Daylight Saving Time, please use HAL_RTC_DST_xxx functions */
} RTC_TimeTypeDef;

/**
  * @brief  RTC Date structure definition
  */
typedef struct
{
  uint8_t WeekDay;  /*!< Specifies the RTC Date WeekDay.
                         This parameter can be a value of @ref RTC_WeekDay_Definitions */

  uint8_t Month;    /*!< Specifies the RTC Date Month (in BCD format).
                         This parameter can be a value of @ref RTC_Month_Date_Definitions */

  uint8_t Date;     /*!< Specifies the RTC Date.
                         This parameter must be a number between Min_Data = 1 and Max_Data = 31 */

  uint8_t Year;     /*!< Specifies the RTC Date Year.
                         This parameter must be a number between Min_Data = 0 and Max_Data = 99 */
} RTC_DateTypeDef;

/**
  * @brief  RTC Alarm structure definition
  */
typedef struct
{
  RTC_TimeTypeDef AlarmTime;     /*!< Specifies the RTC Alarm Time members */

  uint32_t AlarmMask;            /*!< Specifies the RTC Alarm Masks.
                                      This parameter can be a value of @ref RTC_AlarmMask_Definitions */

  uint32_t AlarmSubSecondMask;   /*!< Specifies the RTC Alarm SubSeconds Masks.
                                      if Binary mode is RTC_BINARY_ONLY or is RTC_BINARY_MIX
                                        This parameter can be a value of
                                        @ref RTCEx_Alarm_Sub_Seconds_binary_Masks_Definitions
                                      else if Binary mode is RTC_BINARY_NONE
                                        This parameter can be a value of
                                        @ref RTC_Alarm_Sub_Seconds_BCD_Masks_Definitions */

  uint32_t BinaryAutoClr;        /*!< Clear synchronously counter (RTC_SSR) on binary alarm.
                                      RTC_ALARMSUBSECONDBIN_AUTOCLR_YES must only be used if Binary mode
                                       is RTC_BINARY_ONLY
                                      This parameter can be a value of
                                      @ref RTCEx_Alarm_Sub_Seconds_binary_Clear_Definitions */

  uint32_t AlarmDateWeekDaySel;  /*!< Specifies the RTC Alarm is on Date or WeekDay.
                                     This parameter can be a value of @ref RTC_AlarmDateWeekDay_Definitions */

  uint8_t AlarmDateWeekDay;      /*!< Specifies the RTC Alarm Date/WeekDay.
                                      If the Alarm Date is selected, this parameter must be set to a value
                                       in the 1-31 range.
                                      If the Alarm WeekDay is selected, this parameter can be a value of
                                      @ref RTC_WeekDay_Definitions */

  uint32_t FlagAutoClr;          /*!< Specifies the alarm trigger generation. This feature is meaningful
                                      to avoid any RTC software execution after configuration.
                                      This parameter can be a value of @ref RTC_ALARM_Flag_AutoClear_Definitions */

  uint32_t Alarm;                /*!< Specifies the alarm.
                                      This parameter can be a value of @ref RTC_Alarms_Definitions */
} RTC_AlarmTypeDef;

/**
  * @brief  RTC Handle Structure definition
  */
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
typedef struct __RTC_HandleTypeDef
#else
typedef struct
#endif /* (USE_HAL_RTC_REGISTER_CALLBACKS) */
{
  RTC_TypeDef               *Instance;  /*!< Legacy register base address. Not used anymore, the driver directly uses cmsis base address */

  RTC_InitTypeDef           Init;       /*!< RTC required parameters  */

  HAL_LockTypeDef           Lock;       /*!< RTC locking object       */

  __IO HAL_RTCStateTypeDef  State;      /*!< Time communication state */

#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
  void (* AlarmAEventCallback)(struct __RTC_HandleTypeDef *hrtc);            /*!< RTC Alarm A Event callback            */
  void (* AlarmBEventCallback)(struct __RTC_HandleTypeDef *hrtc);            /*!< RTC Alarm B Event callback            */
  void (* TimeStampEventCallback)(struct __RTC_HandleTypeDef *hrtc);         /*!< RTC TimeStamp Event callback          */
  void (* WakeUpTimerEventCallback)(struct __RTC_HandleTypeDef *hrtc);       /*!< RTC WakeUpTimer Event callback        */
  void (* SSRUEventCallback)(struct __RTC_HandleTypeDef *hrtc);              /*!< RTC SSRU Event callback               */
  void (* Tamper1EventCallback)(struct __RTC_HandleTypeDef *hrtc);           /*!< RTC Tamper 1 Event callback           */
  void (* Tamper2EventCallback)(struct __RTC_HandleTypeDef *hrtc);           /*!< RTC Tamper 2 Event callback           */
  void (* Tamper3EventCallback)(struct __RTC_HandleTypeDef *hrtc);           /*!< RTC Tamper 3 Event callback           */
  void (* Tamper4EventCallback)(struct __RTC_HandleTypeDef *hrtc);           /*!< RTC Tamper 4 Event callback           */
  void (* Tamper5EventCallback)(struct __RTC_HandleTypeDef *hrtc);           /*!< RTC Tamper 5 Event callback           */
  void (* Tamper6EventCallback)(struct __RTC_HandleTypeDef *hrtc);           /*!< RTC Tamper 6 Event callback           */
  void (* Tamper7EventCallback)(struct __RTC_HandleTypeDef *hrtc);           /*!< RTC Tamper 7 Event callback           */
  void (* Tamper8EventCallback)(struct __RTC_HandleTypeDef *hrtc);           /*!< RTC Tamper 8 Event callback           */
  void (* InternalTamper1EventCallback)(struct __RTC_HandleTypeDef *hrtc);   /*!< RTC Internal Tamper 1 Event callback  */
  void (* InternalTamper2EventCallback)(struct __RTC_HandleTypeDef *hrtc);   /*!< RTC Internal Tamper 2 Event callback  */
  void (* InternalTamper3EventCallback)(struct __RTC_HandleTypeDef *hrtc);   /*!< RTC Internal Tamper 3 Event callback  */
  void (* InternalTamper4EventCallback)(struct __RTC_HandleTypeDef *hrtc);   /*!< RTC Internal Tamper 4 Event callback  */
  void (* InternalTamper5EventCallback)(struct __RTC_HandleTypeDef *hrtc);   /*!< RTC Internal Tamper 5 Event callback  */
  void (* InternalTamper6EventCallback)(struct __RTC_HandleTypeDef *hrtc);   /*!< RTC Internal Tamper 6 Event callback  */
  void (* InternalTamper7EventCallback)(struct __RTC_HandleTypeDef *hrtc);   /*!< RTC Internal Tamper 7 Event callback  */
  void (* InternalTamper8EventCallback)(struct __RTC_HandleTypeDef *hrtc);   /*!< RTC Internal Tamper 8 Event callback  */
  void (* InternalTamper9EventCallback)(struct __RTC_HandleTypeDef *hrtc);   /*!< RTC Internal Tamper 9 Event callback  */
  void (* InternalTamper11EventCallback)(struct __RTC_HandleTypeDef *hrtc);  /*!< RTC Internal Tamper 11 Event callback */
  void (* MspInitCallback)(struct __RTC_HandleTypeDef *hrtc);                /*!< RTC Msp Init callback                 */
  void (* MspDeInitCallback)(struct __RTC_HandleTypeDef *hrtc);              /*!< RTC Msp DeInit callback               */

#endif /* (USE_HAL_RTC_REGISTER_CALLBACKS) */

} RTC_HandleTypeDef;

#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
/**
  * @brief  HAL RTC Callback ID enumeration definition
  */
typedef enum
{
  HAL_RTC_ALARM_A_EVENT_CB_ID           = 0U,   /*!< RTC Alarm A Event Callback ID      */
  HAL_RTC_ALARM_B_EVENT_CB_ID           = 1U,   /*!< RTC Alarm B Event Callback ID      */
  HAL_RTC_TIMESTAMP_EVENT_CB_ID         = 2U,   /*!< RTC TimeStamp Event Callback ID    */
  HAL_RTC_WAKEUPTIMER_EVENT_CB_ID       = 3U,   /*!< RTC WakeUp Timer Event Callback ID */
  HAL_RTC_SSRU_EVENT_CB_ID              = 4U,   /*!< RTC SSRU Event Callback ID         */
  HAL_RTC_TAMPER1_EVENT_CB_ID           = 5U,   /*!< RTC Tamper 1 Callback ID           */
  HAL_RTC_TAMPER2_EVENT_CB_ID           = 6U,   /*!< RTC Tamper 2 Callback ID           */
  HAL_RTC_TAMPER3_EVENT_CB_ID           = 7U,   /*!< RTC Tamper 3 Callback ID           */
  HAL_RTC_TAMPER4_EVENT_CB_ID           = 8U,   /*!< RTC Tamper 4 Callback ID           */
  HAL_RTC_TAMPER5_EVENT_CB_ID           = 9U,   /*!< RTC Tamper 5 Callback ID           */
  HAL_RTC_TAMPER6_EVENT_CB_ID           = 10U,  /*!< RTC Tamper 6 Callback ID           */
  HAL_RTC_TAMPER7_EVENT_CB_ID           = 11U,  /*!< RTC Tamper 7 Callback ID           */
  HAL_RTC_TAMPER8_EVENT_CB_ID           = 12U,  /*!< RTC Tamper 8 Callback ID           */
  HAL_RTC_INTERNAL_TAMPER1_EVENT_CB_ID  = 13U,  /*!< RTC Internal Tamper 1 Callback ID  */
  HAL_RTC_INTERNAL_TAMPER2_EVENT_CB_ID  = 14U,  /*!< RTC Internal Tamper 2 Callback ID  */
  HAL_RTC_INTERNAL_TAMPER3_EVENT_CB_ID  = 15U,  /*!< RTC Internal Tamper 3 Callback ID  */
  HAL_RTC_INTERNAL_TAMPER4_EVENT_CB_ID  = 16U,  /*!< RTC Internal Tamper 4 Callback ID  */
  HAL_RTC_INTERNAL_TAMPER5_EVENT_CB_ID  = 17U,  /*!< RTC Internal Tamper 5 Callback ID  */
  HAL_RTC_INTERNAL_TAMPER6_EVENT_CB_ID  = 18U,  /*!< RTC Internal Tamper 6 Callback ID  */
  HAL_RTC_INTERNAL_TAMPER7_EVENT_CB_ID  = 19U,  /*!< RTC Internal Tamper 7 Callback ID  */
  HAL_RTC_INTERNAL_TAMPER8_EVENT_CB_ID  = 20U,  /*!< RTC Internal Tamper 8 Callback ID  */
  HAL_RTC_INTERNAL_TAMPER9_EVENT_CB_ID  = 21U,  /*!< RTC Internal Tamper 9 Callback ID  */
  HAL_RTC_INTERNAL_TAMPER11_EVENT_CB_ID = 22U,  /*!< RTC Internal Tamper 11 Callback ID */
  HAL_RTC_MSPINIT_CB_ID                 = 23U,  /*!< RTC Msp Init callback ID           */
  HAL_RTC_MSPDEINIT_CB_ID               = 24U   /*!< RTC Msp DeInit callback ID         */
} HAL_RTC_CallbackIDTypeDef;

/**
  * @brief  HAL RTC Callback pointer definition
  */
typedef  void (*pRTC_CallbackTypeDef)(RTC_HandleTypeDef *hrtc);  /*!< pointer to an RTC callback function */
#endif /* USE_HAL_RTC_REGISTER_CALLBACKS */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup RTC_Exported_Constants RTC Exported Constants
  * @{
  */

/** @defgroup RTC_Hour_Formats RTC Hour Formats
  * @{
  */
#define RTC_HOURFORMAT_24                   0U
#define RTC_HOURFORMAT_12                   RTC_CR_FMT
/**
  * @}
  */

/** @defgroup RTCEx_Output_selection_Definitions RTCEx Output Selection Definition
  * @{
  */
#define RTC_OUTPUT_DISABLE                  0U
#define RTC_OUTPUT_ALARMA                   RTC_CR_OSEL_0
#define RTC_OUTPUT_ALARMB                   RTC_CR_OSEL_1
#define RTC_OUTPUT_WAKEUP                   RTC_CR_OSEL
#define RTC_OUTPUT_TAMPER                   RTC_CR_TAMPOE
/**
  * @}
  */

/** @defgroup RTC_Output_Polarity_Definitions RTC Output Polarity Definitions
  * @{
  */
#define RTC_OUTPUT_POLARITY_HIGH            0U
#define RTC_OUTPUT_POLARITY_LOW             RTC_CR_POL
/**
  * @}
  */

/** @defgroup RTC_Output_Type_ALARM_OUT RTC Output Type ALARM OUT
  * @{
  */
#define RTC_OUTPUT_TYPE_PUSHPULL            0U
#define RTC_OUTPUT_TYPE_OPENDRAIN           RTC_CR_TAMPALRM_TYPE
/**
  * @}
  */

/** @defgroup RTC_Output_PullUp_ALARM_OUT RTC Output Pull-Up ALARM OUT
  * @{
  */
#define RTC_OUTPUT_PULLUP_NONE              0U
#define RTC_OUTPUT_PULLUP_ON                RTC_CR_TAMPALRM_PU
/**
  * @}
  */

/** @defgroup RTC_Output_ALARM_OUT_Remap RTC Output ALARM OUT Remap
  * @{
  */
#define RTC_OUTPUT_REMAP_NONE               0U
#define RTC_OUTPUT_REMAP_POS1               RTC_CR_OUT2EN
/**
  * @}
  */

/** @defgroup RTC_AM_PM_Definitions RTC AM PM Definitions
  * @{
  */
#define RTC_HOURFORMAT12_AM                 0U
#define RTC_HOURFORMAT12_PM                 1U
/**
  * @}
  */

/** @defgroup RTC_DayLightSaving_Definitions RTC DayLightSaving Definitions
  * @{
  */
#define RTC_DAYLIGHTSAVING_SUB1H            RTC_CR_SUB1H
#define RTC_DAYLIGHTSAVING_ADD1H            RTC_CR_ADD1H
#define RTC_DAYLIGHTSAVING_NONE             0U
/**
  * @}
  */

/** @defgroup RTC_StoreOperation_Definitions RTC StoreOperation Definitions
  * @{
  */
#define RTC_STOREOPERATION_RESET            0U
#define RTC_STOREOPERATION_SET              RTC_CR_BKP
/**
  * @}
  */

/** @defgroup RTC_Input_parameter_format_definitions RTC Input Parameter Format Definitions
  * @{
  */
#define RTC_FORMAT_BIN                      0U
#define RTC_FORMAT_BCD                      1U
/**
  * @}
  */

/** @defgroup RTC_Month_Date_Definitions RTC Month Date Definitions
  * @{
  */

/* Coded in BCD format */
#define RTC_MONTH_JANUARY                   ((uint8_t)0x01U)
#define RTC_MONTH_FEBRUARY                  ((uint8_t)0x02U)
#define RTC_MONTH_MARCH                     ((uint8_t)0x03U)
#define RTC_MONTH_APRIL                     ((uint8_t)0x04U)
#define RTC_MONTH_MAY                       ((uint8_t)0x05U)
#define RTC_MONTH_JUNE                      ((uint8_t)0x06U)
#define RTC_MONTH_JULY                      ((uint8_t)0x07U)
#define RTC_MONTH_AUGUST                    ((uint8_t)0x08U)
#define RTC_MONTH_SEPTEMBER                 ((uint8_t)0x09U)
#define RTC_MONTH_OCTOBER                   ((uint8_t)0x10U)
#define RTC_MONTH_NOVEMBER                  ((uint8_t)0x11U)
#define RTC_MONTH_DECEMBER                  ((uint8_t)0x12U)

/**
  * @}
  */

/** @defgroup RTC_WeekDay_Definitions RTC WeekDay Definitions
  * @{
  */
#define RTC_WEEKDAY_MONDAY                  ((uint8_t)0x01U)
#define RTC_WEEKDAY_TUESDAY                 ((uint8_t)0x02U)
#define RTC_WEEKDAY_WEDNESDAY               ((uint8_t)0x03U)
#define RTC_WEEKDAY_THURSDAY                ((uint8_t)0x04U)
#define RTC_WEEKDAY_FRIDAY                  ((uint8_t)0x05U)
#define RTC_WEEKDAY_SATURDAY                ((uint8_t)0x06U)
#define RTC_WEEKDAY_SUNDAY                  ((uint8_t)0x07U)

/**
  * @}
  */

/** @defgroup RTC_AlarmDateWeekDay_Definitions RTC AlarmDateWeekDay Definitions
  * @{
  */
#define RTC_ALARMDATEWEEKDAYSEL_DATE        0U
#define RTC_ALARMDATEWEEKDAYSEL_WEEKDAY     RTC_ALRMAR_WDSEL

/**
  * @}
  */

/** @defgroup RTC_AlarmMask_Definitions RTC AlarmMask Definitions
  * @{
  */
#define RTC_ALARMMASK_NONE                  0U
#define RTC_ALARMMASK_DATEWEEKDAY           RTC_ALRMAR_MSK4
#define RTC_ALARMMASK_HOURS                 RTC_ALRMAR_MSK3
#define RTC_ALARMMASK_MINUTES               RTC_ALRMAR_MSK2
#define RTC_ALARMMASK_SECONDS               RTC_ALRMAR_MSK1
#define RTC_ALARMMASK_ALL                   (RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS  | \
                                             RTC_ALARMMASK_MINUTES | RTC_ALARMMASK_SECONDS)

/**
  * @}
  */

/** @defgroup RTC_Alarms_Definitions RTC Alarms Definitions
  * @{
  */
#define RTC_ALARM_A                         RTC_CR_ALRAE
#define RTC_ALARM_B                         RTC_CR_ALRBE

/**
  * @}
  */
/** @defgroup RTC_ALARM_Flag_AutoClear_Definitions RTC Alarms Flag Auto Clear Definitions
  * @{
  */
#define ALARM_FLAG_AUTOCLR_ENABLE           1U
#define ALARM_FLAG_AUTOCLR_DISABLE          0U
/**
  * @}
  */

/** @defgroup RTC_Alarm_Sub_Seconds_BCD_Masks_Definitions RTC Alarm Sub Seconds BCD Masks Definitions
  *           In BCD mode (BIN=00) the overflow bits of the synchronous counter (bits 31:15) are never compared.
  * @{
  */
#define RTC_ALARMSUBSECONDMASK_ALL          0U                                                                       /*!< All Alarm SS fields are masked. There is no comparison on sub seconds for Alarm */
#define RTC_ALARMSUBSECONDMASK_SS14_1       RTC_ALRMASSR_MASKSS_0                                                    /*!< SS[14:1] not used in Alarmcomparison. Only SS[0] is compared                    */
#define RTC_ALARMSUBSECONDMASK_SS14_2       RTC_ALRMASSR_MASKSS_1                                                    /*!< SS[14:2] not used in Alarm comparison. Only SS[1:0] are compared                */
#define RTC_ALARMSUBSECONDMASK_SS14_3       (RTC_ALRMASSR_MASKSS_0 | RTC_ALRMASSR_MASKSS_1)                          /*!< SS[14:3] not used in Alarm comparison. Only SS[2:0] are compared                */
#define RTC_ALARMSUBSECONDMASK_SS14_4       RTC_ALRMASSR_MASKSS_2                                                    /*!< SS[14:4] not used in Alarm comparison. Only SS[3:0] are compared                */
#define RTC_ALARMSUBSECONDMASK_SS14_5       (RTC_ALRMASSR_MASKSS_0 | RTC_ALRMASSR_MASKSS_2)                          /*!< SS[14:5] not used in Alarm comparison. Only SS[4:0] are compared                */
#define RTC_ALARMSUBSECONDMASK_SS14_6       (RTC_ALRMASSR_MASKSS_1 | RTC_ALRMASSR_MASKSS_2)                          /*!< SS[14:6] not used in Alarm comparison. Only SS[5:0] are compared                */
#define RTC_ALARMSUBSECONDMASK_SS14_7       (RTC_ALRMASSR_MASKSS_0 | RTC_ALRMASSR_MASKSS_1 | RTC_ALRMASSR_MASKSS_2)  /*!< SS[14:7] not used in Alarm comparison. Only SS[6:0] are compared                */
#define RTC_ALARMSUBSECONDMASK_SS14_8       RTC_ALRMASSR_MASKSS_3                                                    /*!< SS[14:8] not used in Alarm comparison. Only SS[7:0] are compared                */
#define RTC_ALARMSUBSECONDMASK_SS14_9       (RTC_ALRMASSR_MASKSS_0 | RTC_ALRMASSR_MASKSS_3)                          /*!< SS[14:9] not used in Alarm comparison. Only SS[8:0] are compared                */
#define RTC_ALARMSUBSECONDMASK_SS14_10      (RTC_ALRMASSR_MASKSS_1 | RTC_ALRMASSR_MASKSS_3)                          /*!< SS[14:10] not used in Alarm comparison. Only SS[9:0] are compared               */
#define RTC_ALARMSUBSECONDMASK_SS14_11      (RTC_ALRMASSR_MASKSS_0 | RTC_ALRMASSR_MASKSS_1 | RTC_ALRMASSR_MASKSS_3)  /*!< SS[14:11] not used in Alarm comparison. Only SS[10:0] are compared              */
#define RTC_ALARMSUBSECONDMASK_SS14_12      (RTC_ALRMASSR_MASKSS_2 | RTC_ALRMASSR_MASKSS_3)                          /*!< SS[14:12] not used in Alarm comparison.Only SS[11:0] are compared               */
#define RTC_ALARMSUBSECONDMASK_SS14_13      (RTC_ALRMASSR_MASKSS_0 | RTC_ALRMASSR_MASKSS_2 | RTC_ALRMASSR_MASKSS_3)  /*!< SS[14:13] not used in Alarm comparison. Only SS[12:0] are compared              */
#define RTC_ALARMSUBSECONDMASK_SS14         (RTC_ALRMASSR_MASKSS_1 | RTC_ALRMASSR_MASKSS_2 | RTC_ALRMASSR_MASKSS_3)  /*!< SS[14] not used in Alarm comparison. Only SS[13:0] are compared                 */
#define RTC_ALARMSUBSECONDMASK_NONE         RTC_ALRMASSR_MASKSS                                                      /*!< SS[14:0] are compared and must match to activate alarm                          */
/**
  * @}
  */

/** @defgroup RTC_Interrupts_Definitions RTC Interrupts Definitions
  * @{
  */
#define RTC_IT_TS                           RTC_CR_TSIE        /*!< Enable Timestamp Interrupt     */
#define RTC_IT_WUT                          RTC_CR_WUTIE       /*!< Enable Wakeup timer Interrupt  */
#define RTC_IT_SSRU                         RTC_CR_SSRUIE      /*!< Enable SSR Underflow Interrupt */
#define RTC_IT_ALRA                         RTC_CR_ALRAIE      /*!< Enable Alarm A Interrupt       */
#define RTC_IT_ALRB                         RTC_CR_ALRBIE      /*!< Enable Alarm B Interrupt       */
/**
  * @}
  */

/** @defgroup RTC_Flags_Definitions RTC Flags Definitions
  * @{
  */
#define RTC_FLAG_RECALPF                    (1U)               /*!< Recalibration pending flag     */
#define RTC_FLAG_INITF                      (2U)               /*!< Initialization flag            */
#define RTC_FLAG_RSF                        (3U)               /*!< Registers synchronization flag */
#define RTC_FLAG_INITS                      (4U)               /*!< Initialization status flag     */
#define RTC_FLAG_SHPF                       (5U)               /*!< Shift operation pending flag   */
#define RTC_FLAG_WUTWF                      (6U)               /*!< Wakeup timer write flag        */
#define RTC_FLAG_SSRUF                      (7U)               /*!< SSR underflow flag             */
#define RTC_FLAG_ITSF                       (8U)               /*!< Internal Time-stamp flag       */
#define RTC_FLAG_TSOVF                      (9U)               /*!< Time-stamp overflow flag       */
#define RTC_FLAG_TSF                        (10U)              /*!< Time-stamp flag                */
#define RTC_FLAG_WUTF                       (11U)              /*!< Wakeup timer flag              */
#define RTC_FLAG_ALRBF                      (12U)              /*!< Alarm B flag                   */
#define RTC_FLAG_ALRAF                      (13U)              /*!< Alarm A flag                   */
/**
  * @}
  */

/** @defgroup RTC_Clear_Flags_Definitions RTC Clear Flags Definitions
  * @{
  */
#define RTC_CLEAR_SSRUF                     RTC_SCR_CSSRUF   /*!< Clear SSR underflow flag       */
#define RTC_CLEAR_ITSF                      RTC_SCR_CITSF    /*!< Clear Internal Time-stamp flag */
#define RTC_CLEAR_TSOVF                     RTC_SCR_CTSOVF   /*!< Clear Time-stamp overflow flag */
#define RTC_CLEAR_TSF                       RTC_SCR_CTSF     /*!< Clear Time-stamp flag          */
#define RTC_CLEAR_WUTF                      RTC_SCR_CWUTF    /*!< Clear Wakeup timer flag        */
#define RTC_CLEAR_ALRBF                     RTC_SCR_CALRBF   /*!< Clear Alarm B flag             */
#define RTC_CLEAR_ALRAF                     RTC_SCR_CALRAF   /*!< Clear Alarm A flag             */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup RTC_Exported_Macros RTC Exported Macros
  * @{
  */

/** @brief Reset RTC handle state
  * @param  __HANDLE__ RTC handle.
  * @retval None
  */
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
#define __HAL_RTC_RESET_HANDLE_STATE(__HANDLE__) do{\
                                                      (__HANDLE__)->State = HAL_RTC_STATE_RESET;\
                                                      (__HANDLE__)->MspInitCallback = NULL;\
                                                      (__HANDLE__)->MspDeInitCallback = NULL;\
                                                     }while(0)
#else
#define __HAL_RTC_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_RTC_STATE_RESET)
#endif /* USE_HAL_RTC_REGISTER_CALLBACKS */

/**
  * @brief  Disable the write protection for RTC registers.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_WRITEPROTECTION_DISABLE(__HANDLE__)             \
  do{                                       \
    RTC->WPR = 0xCAU;   \
    RTC->WPR = 0x53U;   \
  } while(0U)

/**
  * @brief  Enable the write protection for RTC registers.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_WRITEPROTECTION_ENABLE(__HANDLE__)              \
  do{                                       \
    RTC->WPR = 0xFFU;   \
  } while(0U)

/**
  * @brief  Add 1 hour (summer time change).
  * @note   This interface is deprecated.
  *         To manage Daylight Saving Time, please use HAL_RTC_DST_xxx functions
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __BKP__ Backup
  *         This parameter can be:
  *            @arg @ref RTC_STOREOPERATION_RESET
  *            @arg @ref RTC_STOREOPERATION_SET
  * @retval None
  */
#define __HAL_RTC_DAYLIGHT_SAVING_TIME_ADD1H(__HANDLE__, __BKP__)                         \
  do {                                                              \
    __HAL_RTC_WRITEPROTECTION_DISABLE(__HANDLE__);                \
    SET_BIT(RTC->CR, RTC_CR_ADD1H);            \
    MODIFY_REG(RTC->CR, RTC_CR_BKP , (__BKP__)); \
    __HAL_RTC_WRITEPROTECTION_ENABLE(__HANDLE__);                 \
  } while(0);

/**
  * @brief  Subtract 1 hour (winter time change).
  * @note   This interface is deprecated.
  *         To manage Daylight Saving Time, please use HAL_RTC_DST_xxx functions
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __BKP__ Backup
  *         This parameter can be:
  *            @arg @ref RTC_STOREOPERATION_RESET
  *            @arg @ref RTC_STOREOPERATION_SET
  * @retval None
  */
#define __HAL_RTC_DAYLIGHT_SAVING_TIME_SUB1H(__HANDLE__, __BKP__)                         \
  do {                                                              \
    __HAL_RTC_WRITEPROTECTION_DISABLE(__HANDLE__);                \
    SET_BIT(RTC->CR, RTC_CR_SUB1H);            \
    MODIFY_REG(RTC->CR, RTC_CR_BKP , (__BKP__)); \
    __HAL_RTC_WRITEPROTECTION_ENABLE(__HANDLE__);                 \
  } while(0);

/**
  * @brief  Enable the RTC ALARMA peripheral.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_ALARMA_ENABLE(__HANDLE__)  (RTC->CR |= (RTC_CR_ALRAE))

/**
  * @brief  Disable the RTC ALARMA peripheral.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_ALARMA_DISABLE(__HANDLE__)  (RTC->CR &= ~(RTC_CR_ALRAE))

/**
  * @brief  Enable the RTC ALARMB peripheral.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_ALARMB_ENABLE(__HANDLE__)   (RTC->CR |= (RTC_CR_ALRBE))

/**
  * @brief  Disable the RTC ALARMB peripheral.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_ALARMB_DISABLE(__HANDLE__)  (RTC->CR &= ~(RTC_CR_ALRBE))

/**
  * @brief  Enable the RTC Alarm interrupt.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __INTERRUPT__ specifies the RTC Alarm interrupt sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *             @arg @ref RTC_IT_ALRA Alarm A interrupt
  *             @arg @ref RTC_IT_ALRB Alarm B interrupt
  * @retval None
  */
#define __HAL_RTC_ALARM_ENABLE_IT(__HANDLE__, __INTERRUPT__)( \
                                ((__INTERRUPT__) == RTC_IT_ALRA) ? (SET_BIT(RTC->CR, RTC_CR_ALRAIE)):\
                                ((__INTERRUPT__) == RTC_IT_ALRB) ? (SET_BIT(RTC->CR, RTC_CR_ALRBIE)):\
                                (0U)) /* Dummy action because is an invalid parameter value */

/**
  * @brief  Disable the RTC Alarm interrupt.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __INTERRUPT__ specifies the RTC Alarm interrupt sources to be enabled or disabled.
  *         This parameter can be any combination of the following values:
  *            @arg @ref RTC_IT_ALRA Alarm A interrupt
  *            @arg @ref RTC_IT_ALRB Alarm B interrupt
  * @retval None
  */
#define __HAL_RTC_ALARM_DISABLE_IT(__HANDLE__, __INTERRUPT__)( \
                                ((__INTERRUPT__) == RTC_IT_ALRA) ? (CLEAR_BIT(RTC->CR, RTC_CR_ALRAIE)):\
                                ((__INTERRUPT__) == RTC_IT_ALRB) ? (CLEAR_BIT(RTC->CR, RTC_CR_ALRBIE)):\
                                (0U)) /* Dummy action because is an invalid parameter value */

/**
  * @brief  Check whether the specified RTC Alarm interrupt has occurred or not.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __INTERRUPT__ specifies the RTC Alarm interrupt sources to check.
  *         This parameter can be:
  *            @arg @ref RTC_IT_ALRA Alarm A interrupt
  *            @arg @ref RTC_IT_ALRB Alarm B interrupt
  * @retval The state of __INTERRUPT__ (TRUE or FALSE).
  */
#define __HAL_RTC_ALARM_GET_IT(__HANDLE__, __INTERRUPT__)( \
                          ((__INTERRUPT__) == RTC_IT_ALRA) ? (READ_BIT(RTC->MISR, RTC_MISR_ALRAMF) == RTC_MISR_ALRAMF):\
                          ((__INTERRUPT__) == RTC_IT_ALRB) ? (READ_BIT(RTC->MISR, RTC_MISR_ALRBMF) == RTC_MISR_ALRBMF):\
                          (0U)) /* Return 0 because it is an invalid parameter value */

/**
  * @brief  Check whether the specified RTC Alarm interrupt has been enabled or not.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __INTERRUPT__ specifies the RTC Alarm interrupt sources to check.
  *         This parameter can be:
  *            @arg @ref RTC_IT_ALRA Alarm A interrupt
  *            @arg @ref RTC_IT_ALRB Alarm B interrupt
  * @retval The state of __INTERRUPT__ (TRUE or FALSE).
  */
#define __HAL_RTC_ALARM_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)( \
                                ((__INTERRUPT__) == RTC_IT_ALRA) ? (READ_BIT(RTC->CR, RTC_CR_ALRAIE) == RTC_CR_ALRAIE):\
                                ((__INTERRUPT__) == RTC_IT_ALRB) ? (READ_BIT(RTC->CR, RTC_CR_ALRBIE) == RTC_CR_ALRBIE):\
                                (0U)) /* Return 0 because it is an invalid parameter value */

/**
  * @brief  Get the selected RTC Alarms flag status.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __FLAG__ specifies the RTC Alarm Flag sources to check.
  *         This parameter can be:
  *            @arg @ref RTC_FLAG_ALRAF
  *            @arg @ref RTC_FLAG_ALRBF
  * @retval The state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_RTC_ALARM_GET_FLAG(__HANDLE__, __FLAG__)( \
                                ((__FLAG__) == RTC_FLAG_ALRAF) ? (READ_BIT(RTC->SR, RTC_SR_ALRAF) == RTC_SR_ALRAF):\
                                ((__FLAG__) == RTC_FLAG_ALRBF) ? (READ_BIT(RTC->SR, RTC_SR_ALRBF) == RTC_SR_ALRBF):\
                                (0U)) /* Return 0 because it is an invalid parameter value */

/**
  * @brief  Clear the RTC Alarms pending flags.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __FLAG__ specifies the RTC Alarm Flag sources to clear.
  *          This parameter can be:
  *             @arg @ref RTC_FLAG_ALRAF
  *             @arg @ref RTC_FLAG_ALRBF
  * @retval None
  */
#define __HAL_RTC_ALARM_CLEAR_FLAG(__HANDLE__, __FLAG__)( \
                                ((__FLAG__) == RTC_FLAG_ALRAF) ? (WRITE_REG(RTC->SCR, RTC_SCR_CALRAF)):\
                                ((__FLAG__) == RTC_FLAG_ALRBF) ? (WRITE_REG(RTC->SCR, RTC_SCR_CALRBF)):\
                                (0U)) /* Dummy action because is an invalid parameter value */

/**
  * @brief  Check whether if the RTC Calendar is initialized.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval The state of RTC Calendar initialization (TRUE or FALSE).
  */
#define __HAL_RTC_IS_CALENDAR_INITIALIZED(__HANDLE__)  ((((RTC->ICSR) & (RTC_ICSR_INITS)) == RTC_ICSR_INITS))

/**
  * @}
  */

/* Include RTC HAL Extended module */
#include "stm32n6xx_hal_rtc_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @defgroup RTC_Exported_Functions RTC Exported Functions
  * @{
  */

/** @defgroup RTC_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
/* Initialization and de-initialization functions  ****************************/
HAL_StatusTypeDef HAL_RTC_Init(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTC_DeInit(RTC_HandleTypeDef *hrtc);

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc);
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc);

/* Callbacks Register/UnRegister functions  ***********************************/
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_RTC_RegisterCallback(RTC_HandleTypeDef *hrtc, HAL_RTC_CallbackIDTypeDef CallbackID,
                                           pRTC_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_RTC_UnRegisterCallback(RTC_HandleTypeDef *hrtc, HAL_RTC_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_RTC_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @defgroup RTC_Exported_Functions_Group2 RTC Time and Date functions
  * @{
  */
/* RTC Time and Date functions ************************************************/
HAL_StatusTypeDef HAL_RTC_SetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_GetTime(const RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_SetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_GetDate(const RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format);
void              HAL_RTC_DST_Add1Hour(const RTC_HandleTypeDef *hrtc);
void              HAL_RTC_DST_Sub1Hour(const RTC_HandleTypeDef *hrtc);
void              HAL_RTC_DST_SetStoreOperation(const RTC_HandleTypeDef *hrtc);
void              HAL_RTC_DST_ClearStoreOperation(const RTC_HandleTypeDef *hrtc);
uint32_t          HAL_RTC_DST_ReadStoreOperation(const RTC_HandleTypeDef *hrtc);
/**
  * @}
  */

/** @defgroup RTC_Exported_Functions_Group3 RTC Alarm functions
  * @{
  */
/* RTC Alarm functions ********************************************************/
HAL_StatusTypeDef HAL_RTC_SetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_SetAlarm_IT(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_DeactivateAlarm(RTC_HandleTypeDef *hrtc, uint32_t Alarm);
HAL_StatusTypeDef HAL_RTC_GetAlarm(const RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Alarm,
                                   uint32_t Format);
void              HAL_RTC_AlarmIRQHandler(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTC_PollForAlarmAEvent(const RTC_HandleTypeDef *hrtc, uint32_t Timeout);
void              HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);

/**
  * @}
  */

/** @defgroup  RTC_Exported_Functions_Group4 Peripheral Control functions
  * @{
  */
/* Peripheral Control functions ***********************************************/
HAL_StatusTypeDef   HAL_RTC_WaitForSynchro(RTC_HandleTypeDef *hrtc);
/**
  * @}
  */

/** @defgroup RTC_Exported_Functions_Group5 Peripheral State functions
  * @{
  */
/* Peripheral State functions *************************************************/
HAL_RTCStateTypeDef HAL_RTC_GetState(const RTC_HandleTypeDef *hrtc);
/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup RTC_Private_Constants RTC Private Constants
  * @{
  */
/* Masks Definition */
#define RTC_TR_RESERVED_MASK                (RTC_TR_PM | RTC_TR_HT | RTC_TR_HU | \
                                             RTC_TR_MNT | RTC_TR_MNU| RTC_TR_ST | \
                                             RTC_TR_SU)
#define RTC_DR_RESERVED_MASK                (RTC_DR_YT | RTC_DR_YU | RTC_DR_WDU | \
                                             RTC_DR_MT | RTC_DR_MU | RTC_DR_DT  | \
                                             RTC_DR_DU)
#define RTC_INIT_MASK                       0xFFFFFFFFU
#define RTC_RSF_MASK                        (~(RTC_ICSR_INIT | RTC_ICSR_RSF))

#define RTC_TIMEOUT_VALUE                   1000U

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup RTC_Private_Macros RTC Private Macros
  * @{
  */

/** @defgroup RTC_IS_RTC_Definitions RTC Private macros to check input parameters
  * @{
  */
#define IS_RTC_OUTPUT(OUTPUT) (((OUTPUT) == RTC_OUTPUT_DISABLE) || \
                               ((OUTPUT) == RTC_OUTPUT_ALARMA)  || \
                               ((OUTPUT) == RTC_OUTPUT_ALARMB)  || \
                               ((OUTPUT) == RTC_OUTPUT_WAKEUP)  || \
                               ((OUTPUT) == RTC_OUTPUT_TAMPER))

#define IS_RTC_HOUR_FORMAT(FORMAT)     (((FORMAT) == RTC_HOURFORMAT_12) || \
                                        ((FORMAT) == RTC_HOURFORMAT_24))

#define IS_RTC_OUTPUT_POL(POL) (((POL) == RTC_OUTPUT_POLARITY_HIGH) || \
                                ((POL) == RTC_OUTPUT_POLARITY_LOW))

#define IS_RTC_OUTPUT_TYPE(TYPE) (((TYPE) == RTC_OUTPUT_TYPE_OPENDRAIN) || \
                                  ((TYPE) == RTC_OUTPUT_TYPE_PUSHPULL))

#define IS_RTC_OUTPUT_PULLUP(TYPE) (((TYPE) == RTC_OUTPUT_PULLUP_NONE) || \
                                    ((TYPE) == RTC_OUTPUT_PULLUP_ON))

#define IS_RTC_OUTPUT_REMAP(REMAP)   (((REMAP) == RTC_OUTPUT_REMAP_NONE) || \
                                      ((REMAP) == RTC_OUTPUT_REMAP_POS1))

#define IS_RTC_HOURFORMAT12(PM)  (((PM) == RTC_HOURFORMAT12_AM) || \
                                  ((PM) == RTC_HOURFORMAT12_PM))

#define IS_RTC_DAYLIGHT_SAVING(SAVE) (((SAVE) == RTC_DAYLIGHTSAVING_SUB1H) || \
                                      ((SAVE) == RTC_DAYLIGHTSAVING_ADD1H) || \
                                      ((SAVE) == RTC_DAYLIGHTSAVING_NONE))

#define IS_RTC_STORE_OPERATION(OPERATION) (((OPERATION) == RTC_STOREOPERATION_RESET) || \
                                           ((OPERATION) == RTC_STOREOPERATION_SET))

#define IS_RTC_FORMAT(FORMAT) (((FORMAT) == RTC_FORMAT_BIN) || \
                               ((FORMAT) == RTC_FORMAT_BCD))

#define IS_RTC_YEAR(YEAR)              ((YEAR) <= 99U)

#define IS_RTC_MONTH(MONTH)            (((MONTH) >= 1U) && ((MONTH) <= 12U))

#define IS_RTC_DATE(DATE)              (((DATE) >= 1U) && ((DATE) <= 31U))

#define IS_RTC_WEEKDAY(WEEKDAY) (((WEEKDAY) == RTC_WEEKDAY_MONDAY)    || \
                                 ((WEEKDAY) == RTC_WEEKDAY_TUESDAY)   || \
                                 ((WEEKDAY) == RTC_WEEKDAY_WEDNESDAY) || \
                                 ((WEEKDAY) == RTC_WEEKDAY_THURSDAY)  || \
                                 ((WEEKDAY) == RTC_WEEKDAY_FRIDAY)    || \
                                 ((WEEKDAY) == RTC_WEEKDAY_SATURDAY)  || \
                                 ((WEEKDAY) == RTC_WEEKDAY_SUNDAY))

#define IS_RTC_ALARM_DATE_WEEKDAY_DATE(DATE) (((DATE) > 0U) && ((DATE) <= 31U))

#define IS_RTC_ALARM_DATE_WEEKDAY_WEEKDAY(WEEKDAY) (((WEEKDAY) == RTC_WEEKDAY_MONDAY)    || \
                                                    ((WEEKDAY) == RTC_WEEKDAY_TUESDAY)   || \
                                                    ((WEEKDAY) == RTC_WEEKDAY_WEDNESDAY) || \
                                                    ((WEEKDAY) == RTC_WEEKDAY_THURSDAY)  || \
                                                    ((WEEKDAY) == RTC_WEEKDAY_FRIDAY)    || \
                                                    ((WEEKDAY) == RTC_WEEKDAY_SATURDAY)  || \
                                                    ((WEEKDAY) == RTC_WEEKDAY_SUNDAY))

#define IS_RTC_ALARM_DATE_WEEKDAY_SEL(SEL) (((SEL) == RTC_ALARMDATEWEEKDAYSEL_DATE) || \
                                            ((SEL) == RTC_ALARMDATEWEEKDAYSEL_WEEKDAY))

#define IS_RTC_ALARM_MASK(MASK)  (((MASK) & ~(RTC_ALARMMASK_ALL)) == 0UL)

#define IS_RTC_ALARM(ALARM)      (((ALARM) == RTC_ALARM_A) || \
                                  ((ALARM) == RTC_ALARM_B))

#define IS_RTC_ALARM_SUB_SECOND_VALUE(VALUE) ((VALUE) <= RTC_ALRMASSR_SS)

#define IS_RTC_ALARM_SUB_SECOND_MASK(MASK)          (((MASK) == 0UL) || \
                                                     (((MASK) >= RTC_ALARMSUBSECONDMASK_SS14_1) && \
                                                      ((MASK) <= RTC_ALARMSUBSECONDMASK_NONE)))

#define IS_RTC_ASYNCH_PREDIV(PREDIV)   ((PREDIV) <= (RTC_PRER_PREDIV_A >> RTC_PRER_PREDIV_A_Pos))

#define IS_RTC_SYNCH_PREDIV(PREDIV)    ((PREDIV) <= (RTC_PRER_PREDIV_S >> RTC_PRER_PREDIV_S_Pos))

#define IS_RTC_HOUR12(HOUR)            (((HOUR) > 0U) && ((HOUR) <= 12U))

#define IS_RTC_HOUR24(HOUR)            ((HOUR) <= 23U)

#define IS_RTC_MINUTES(MINUTES)        ((MINUTES) <= 59U)

#define IS_RTC_SECONDS(SECONDS)        ((SECONDS) <= 59U)

/**
  * @}
  */

/**
  * @}
  */

/* Private functions -------------------------------------------------------------*/
/** @defgroup RTC_Private_Functions RTC Private Functions
  * @{
  */
HAL_StatusTypeDef  RTC_EnterInitMode(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef  RTC_ExitInitMode(RTC_HandleTypeDef *hrtc);
uint8_t            RTC_ByteToBcd2(uint8_t Value);
uint8_t            RTC_Bcd2ToByte(uint8_t Value);
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

#endif /* STM32N6xx_HAL_RTC_H */

